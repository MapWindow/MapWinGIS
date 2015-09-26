/**************************************************************************************
 * File name: LabelDrawing.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CLabelDrawer
 *
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/ 
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */
 // Sergei Leschinski (lsu) 25 june 2010 - created the file

#include "stdafx.h"
#include "LabelDrawing.h"
#include <map>
#include <set>
#include "Labels.h"
#include "LabelCategory.h"
#include "Shapefile.h"
#include "macros.h"
#include "TableHelper.h"
#include "GdiPlusLabelDrawer.h"
#include "GdiLabelDrawer.h"

using namespace std;
using namespace Gdiplus;

// *********************************************************************
// 		Constructor										
// *********************************************************************
CLabelDrawer::CLabelDrawer(Gdiplus::Graphics* graphics, Extent* extents, double pixelPerProjectionX, double pixelPerProjectionY, double currentScale, 
						 CCollisionList* collisionList, double mapRotation, bool printing)
{		
	_currentScale = 0;
	_printing = false;
	m_hdc = NULL;
	_extents = extents;
	_pixelPerProjectionX = pixelPerProjectionX;
	_pixelPerProjectionY = pixelPerProjectionY;
	_currentScale = currentScale;
	_collisionList = collisionList;
	_mapRotation = mapRotation;
	_spatiallyReferenced = true;
	_dc = NULL;
	_graphics = graphics;
	_printing = printing;
}

// screen referenced list
CLabelDrawer::CLabelDrawer(Gdiplus::Graphics* graphics, Extent* extents, CCollisionList* collisionList, double mapRotation)
{
	_currentScale = 0;
	_printing = false;
	m_hdc = NULL;
	_extents = extents;
	_pixelPerProjectionX = 0.0;
	_pixelPerProjectionY = 0.0;
	_collisionList = collisionList;
	_mapRotation = mapRotation;
	_spatiallyReferenced = false;
	_dc = NULL;
	_graphics = graphics;
}

// *********************************************************************
// 					GetExpressionFilter()										
// *********************************************************************
bool CLabelDrawer::GetExpressionFilter(ILabels* labels, IShapefile* sf, vector<long>& filter)
{
	CComBSTR expr;
	labels->get_VisibilityExpression(&expr);

	CString err;

	if (SysStringLen(expr) > 0)
	{
		if (sf)
		{
			CComPtr<ITable> tbl = NULL;
			sf->get_Table(&tbl);

			USES_CONVERSION;
			if (TableHelper::Cast(tbl)->QueryCore(OLE2CA(expr), filter, err))
			{
				return true;
			}
		}
	}

	return false;
}

// *********************************************************************
// 					GetVisibilityMask()										
// *********************************************************************
void CLabelDrawer::GetVisibilityMask(ILabels* labels, IShapefile* sf, std::vector<bool>& visibilityMask)
{
	vector<long> filter;
	bool hasFilter = GetExpressionFilter(labels, sf, filter);

	long numLabels;
	labels->get_Count(&numLabels);

	VARIANT_BOOL synchronized;
	labels->get_Synchronized(&synchronized);

	if (synchronized && sf)
	{
		long minSize;
		labels->get_MinDrawingSize(&minSize);
		long size = hasFilter ? (long)filter.size() : numLabels;

		for (long i = 0; i < size; i++)
		{
			long index = hasFilter ? filter[i] : i;
			visibilityMask[index] = true; // (*shapeData)[index]->size >= minSize;
			// TODO: restore
		}
	}
	else
	{
		// for images all the labels will do
		for (int i = 0; i < numLabels; i++)
		{
			visibilityMask[i] = true;
		}
	}
}

// *********************************************************************
// 					GetScaleFactor()										
// *********************************************************************
double CLabelDrawer::GetScaleFactor(ILabels* labels)
{
	VARIANT_BOOL bScale;
	double scaleFactor = 1.0;
	labels->get_ScaleLabels(&bScale);

	if (bScale && _spatiallyReferenced)
	{
		double scale = _currentScale;
		if (scale == 0) {
			return scaleFactor;
		}

		double basicScale;
		labels->get_BasicScale(&basicScale);
		if (basicScale == 0.0)
		{
			basicScale = scale;
			labels->put_BasicScale(scale);
		}
		scaleFactor = basicScale / scale;
	}

	return scaleFactor;
}

// *********************************************************************
// 					CheckDynamicVisibility()										
// *********************************************************************
bool CLabelDrawer::CheckDynamicVisibility(ILabels* labels)
{
	VARIANT_BOOL dynVisibility;
	labels->get_DynamicVisibility(&dynVisibility);

	if (dynVisibility)
	{
		double minScale, maxScale;
		labels->get_MinVisibleScale(&minScale);
		labels->get_MaxVisibleScale(&maxScale);
		double scale = _currentScale;
		if (scale < minScale || scale > maxScale) {
			return false;
		}
	}

	return true;
}

// *********************************************************************
// 					GetUseGdiPlus()										
// *********************************************************************
bool CLabelDrawer::GetUseGdiPlus(ILabels* labels) 
{
	VARIANT_BOOL useGdiPlus = VARIANT_FALSE;
	labels->get_UseGdiPlus(&useGdiPlus);

	if (!useGdiPlus)
	{
		// let's check whether it's possible to draw everything by GDI
		CLabelOptions* options = ((CLabels*)labels)->get_LabelOptions();
		if (options->fontGradientMode != gmNone || options->fontTransparency != 255 ||
			((options->frameGradientMode != gmNone || options->frameTransparency != 255) && options->frameVisible))
		{
			useGdiPlus = true;
		}
	}

	if (_printing) {
		useGdiPlus = VARIANT_TRUE;
	}

	return useGdiPlus ? true: false;
}

// *********************************************************************
// 					GetAutoOffset()										
// *********************************************************************
bool CLabelDrawer::GetAutoOffset(ILabels* labels, IShapefile* sf)
{
	// do we need to enable auto offset? shapefile type must be point and labels must be synchronized
	VARIANT_BOOL autoOffset;
	labels->get_AutoOffset(&autoOffset);
	if (autoOffset && sf != NULL)
	{
		VARIANT_BOOL synchronized;
		labels->get_Synchronized(&synchronized);

		ShpfileType shpType;
		sf->get_ShapefileType2D(&shpType);
		autoOffset = shpType == SHP_POINT || shpType == SHP_MULTIPOINT ? synchronized : VARIANT_FALSE;
	}
	else
	{
		autoOffset = VARIANT_FALSE;
	}

	return autoOffset ? true : false;
}

// *********************************************************************
// 					DrawLabels()										
// *********************************************************************
void CLabelDrawer::DrawLabels( ILabels* labels )
{
	if (m_globalSettings.forceHideLabels)
		return;

	long numLabels;
	labels->get_Count(&numLabels);
	if (numLabels == 0)
		return;
	
	VARIANT_BOOL visible;
	labels->get_Visible(&visible);
	if (!visible) return;
	
	if (!CheckDynamicVisibility(labels)) {
		return;
	}
	
	VARIANT_BOOL avoidCollisions;
	labels->get_AvoidCollisions(&avoidCollisions);
	
	tkVerticalPosition vp;
	labels->get_VerticalPosition(&vp);

	double scaleFactor = GetScaleFactor(labels);

	// ---------------------------------------------------------------
	//	 Extracting data
	// ---------------------------------------------------------------
	CLabels* lbs = static_cast<CLabels*>(labels);
	vector<vector<CLabelInfo*>*>* labelData = lbs->get_LabelData();
	
	long numCategories;
	labels->get_NumCategories(&numCategories);

	VARIANT_BOOL bRemoveDuplicates;
	labels->get_RemoveDuplicates(&bRemoveDuplicates);

	long m_buffer = 0;
	labels->get_CollisionBuffer(&m_buffer);

	GdiLabelDrawer gdi;
	GdiPlusLabelDrawer gdiPlus;
	CRect rect(0, 0, 0, 0);
	
	CLabelOptions* m_options = NULL;
	CStringW wText;
	
	IShapefile* sf = lbs->get_ParentShapefile();
	
	bool autoOffset = GetAutoOffset(labels, sf);
	
	std::vector<ShapeData*>* shapeData = NULL;
	if (sf)
	{
		shapeData = ((CShapefile*)sf)->get_ShapeVector();
	}

	vector<bool> visibilityMask(numLabels, false);
	GetVisibilityMask(labels, sf, visibilityMask);

	// removing duplicates
	std::set<CString> uniqueValues;

	bool hasRotation = lbs->HasRotation();

	bool useGdiPlus = GetUseGdiPlus(labels);
	if (!useGdiPlus)
	{
		m_hdc = _graphics->GetHDC();
		_dc = CDC::FromHandle(m_hdc);
		_dc->SaveDC();
		_dc->SetBkMode(TRANSPARENT);
		_dc->SetGraphicsMode(GM_ADVANCED);
	}
	else
	{
		gdiPlus.InitializeGraphics(_graphics, labels);
	}

	// sort them if sort field is specified
	vector<long>* indices = NULL;
	lbs->GetSorting(&indices);
	if (indices && indices->size() != numLabels) {
		indices = NULL;
	}

	bool useVariableFontSize = lbs->UpdateFontSize();

	// we'll start from the categories with the higher priority, therefore reverse order
	for (int iCategory = numCategories - 1; iCategory >= -1; iCategory--)
	{
		if (iCategory != -1)
		{
			// Drawing with category settings
			ILabelCategory* cat;
			labels->get_Category(iCategory, &cat);
			if (cat == NULL) continue;
			
			CLabelCategory* coCat = static_cast<CLabelCategory*>(cat);
			m_options = reinterpret_cast<CLabelOptions*>(coCat->get_LabelOptions());
			cat->Release();
		}
		else
		{
			// Drawing with standard settings
			CLabels* coLabels = static_cast<CLabels*>(labels);
			m_options = coLabels->get_LabelOptions();
		}
		
		// skip the category if it is not visible		
		if (!m_options->visible) continue;

		if (!useGdiPlus)
			_dc->SetTextColor(m_options->fontColor);

		// we create separate pens/brushes for each drawing method
		if (useGdiPlus)
		{
			gdiPlus.InitCategory(_graphics, m_options, hasRotation);
		}
		else
		{
			gdi.InitCategory(_dc, m_options);
		}

		// each different font size must its own font instance
		void* fonts[MAX_LABEL_SIZE + 1] = {};
		CFont* font = NULL;
		Gdiplus::Font* gpFont = NULL;

		if (!useVariableFontSize) 
		{
			if (useGdiPlus) {
				gpFont = gdiPlus.CreateFont(m_options, m_options->fontSize, scaleFactor);
			}
			else {
				font = gdi.CreateFont(m_options, m_options->fontSize, scaleFactor);
				gdi.oldFont = _dc->SelectObject(font);
			}
		}

		// ---------------------------------------------------------------
		//	 drawing labels within category
		// ---------------------------------------------------------------
		for (long k = 0; k < numLabels; k++ )
		{
			long i = indices ? (*indices)[k] : k;

			if (!visibilityMask[i])	{
				continue;
			}

			vector<CLabelInfo*>* parts = (*labelData)[i];
			for (int j =0; j < (int)parts->size(); j++ )
			{
				CLabelInfo* lbl = (*parts)[j];

				if( lbl->x < _extents->left)   continue;
				if( lbl->x > _extents->right)  continue;
				if( lbl->y < _extents->bottom) continue;
				if( lbl->y > _extents->top)	 continue;
				if( lbl->text.GetLength() <= 0 ) continue;

				if ((lbl->category == iCategory) || (iCategory == -1 && (lbl->category < 0 || lbl->category >= numCategories)))	{}
				else continue;	/* Wrong category */
				
				// blocking the labels with the text already displayed
				if (bRemoveDuplicates)
				{
					if (uniqueValues.find(lbl->text) != uniqueValues.end())
						continue;
					else
						uniqueValues.insert(lbl->text);
				}

				// -------------------------------------------------------
				//	Choosing appropriate font
				// -------------------------------------------------------
				if (useVariableFontSize) 
				{
					void* fontPtr = NULL;
					if (!fonts[lbl->fontSize]) 
					{
						if (useGdiPlus) {
							fontPtr = gdiPlus.CreateFont(m_options, lbl->fontSize, scaleFactor);
						}
						else {
							fontPtr = gdi.CreateFont(m_options, lbl->fontSize, scaleFactor);
						}
						
						fonts[lbl->fontSize] = fontPtr;
					}
					else {
						fontPtr = fonts[lbl->fontSize];
					}

					if (useGdiPlus) {
						gpFont = (Gdiplus::Font*)fontPtr;
					}
					else {
						font = (CFont*)fontPtr;
						CFont* tempFont = _dc->SelectObject(font);
						if (!gdi.oldFont) {
							gdi.oldFont = tempFont;
						}
					}
				}

				// -------------------------------------------------------
				//	Calculating the width and height of output rectangle
				// -------------------------------------------------------
				if (!useGdiPlus)
				{
					_dc->DrawText(lbl->text, rect, DT_CALCRECT);
					// frame for GDI is very narrow; so we'll enlarge it a bit					
					rect.left -= rect.Height()/6;
					rect.right += rect.Height()/6;
				}
				else
				{
					Gdiplus::RectF* gpRect = &gdiPlus.rect;

					USES_CONVERSION;
					wText = A2W(lbl->text);
					_graphics->MeasureString(wText, wText.GetLength(), gpFont, PointF(0.0f, 0.0f), gpRect);
					
					// in some case we lose the last letter by clipping; 
					gpRect->Width += 1;
					gpRect->Height += 1;

					// converting to CRect to prevent duplication in the code below
					rect.left = static_cast<LONG>(gpRect->X);
					rect.top = static_cast<LONG>(gpRect->Y);
					rect.right = static_cast<LONG>(gpRect->X + gpRect->Width);
					rect.bottom = static_cast<LONG>(gpRect->Y + gpRect->Height);
				}
				
				// laCenter alignment isn't allowed for auto mode
				tkLabelAlignment align = (autoOffset && m_options->alignment == laCenter) ? laCenterRight : m_options->alignment;
				AlignRectangle(rect, m_options->alignment);
				
				double piX, piY;
				if (_spatiallyReferenced)
				{
					this->ProjectionToPixel(lbl->x,lbl->y,piX,piY);
				}
				else
				{
					// no calculations for screen referenced labels
					piX = lbl->x;
					piY = lbl->y;
				}
				
				// calculating offset
				int offset = 0;
				
				if (autoOffset)
				{
					offset = (*shapeData)[i]->size/2 + 2;
				}

				// we make very narrow rect wider to have circular form in case of rounded frames
				if (rect.Height() > rect.Width())
				{
					long add = (long)ceil(double((rect.Height() - rect.Width())/2));
					rect.left  -= add;
					rect.right += add;
					offset += 2 * add;
				}
				
				//// adding padding				
				if (m_options->frameVisible)
				{
					rect.left  -= (LONG)ceil(double(m_options->framePaddingX/2.0));
					rect.right += (LONG)ceil(double(m_options->framePaddingX/2.0));
					rect.top -= (LONG)ceil(double(m_options->framePaddingY/2.0));
					rect.bottom += (LONG)ceil(double(m_options->framePaddingY/2.0));

					offset += m_options->framePaddingX;
				}
				
				if (autoOffset)
				{
					if (align == laTopRight ||
						align == laBottomRight ||
						align == laCenterRight)
					{
						rect.OffsetRect(offset, 0);
					}
					else if (align == laTopLeft ||
						     align == laBottomLeft ||
						     align == laCenterLeft)
					{
						rect.OffsetRect(-offset, 0);
					}
					else if ( align == laTopCenter)
					{
						rect.OffsetRect(0, -offset);
					}
					else if ( align == laBottomCenter)
					{
						rect.OffsetRect(0, offset);
					}
				}
				
				//else		// let's use standard offsets on the top of auto mode as well
				{
					rect.left += (LONG)m_options->offsetX;
					rect.right += (LONG)m_options->offsetX;
					rect.top += (LONG)m_options->offsetY;
					rect.bottom += (LONG)m_options->offsetY;
				}

				double angle;
				CalcRotation(lbl, angle);
				double angleRad = angle/180.0 * pi_;

				if (angle != 0.0)
				{
					CRotatedRectangle* rectNew = new CRotatedRectangle(rect);
					double angle1 = 2* pi_ - angleRad;
					for(int i = 0; i < 4; i++)
					{
						long x = rectNew->points[i].x;
						long y = rectNew->points[i].y;
						rectNew->points[i].x = LONG( x * cos(angle1) + y * sin(angle1) + piX);
						rectNew->points[i].y = LONG(- x * sin(angle1) + y * cos(angle1) + piY);
					}

					if (avoidCollisions && _collisionList != NULL)
					{
						if (_collisionList->HaveCollision(*rectNew))
						{
							delete rectNew;	continue;
						}
					}
					
					// memory must be cleared at this point
					lbl->rotatedFrame = rectNew;
					lbl->isDrawn = VARIANT_TRUE;
					if (_collisionList != NULL)
					{
						_collisionList->AddRotatedRectangle(rectNew, m_buffer, m_buffer);
					}
				}
				else
				{
					CRect* rectNew = new CRect(rect);

					rectNew->left += (LONG)piX;
					rectNew->right += (LONG)piX;
					rectNew->bottom += (LONG)piY;
					rectNew->top += (LONG)piY;

					if (avoidCollisions && _collisionList != NULL)
					{
						if (_collisionList->HaveCollision(*rectNew) )
						{
							delete rectNew; continue;
						}
					}
					// memory must be cleared at this point

					lbl->horizontalFrame = rectNew;
					lbl->isDrawn = VARIANT_TRUE;
					if (_collisionList != NULL)
					{
						_collisionList->AddRectangle(rectNew, m_buffer, m_buffer);
					}
				}
				
				if (!useGdiPlus)
				{
					gdi.DrawLabel(_dc, m_options, lbl, rect, angleRad, piX, piY);
				}
				else
				{
					gdiPlus.DrawLabel(_graphics, m_options, gpFont, rect, wText, piX, piY, angle);
				}
			}
		} // label

		// cleaning fonts
		if (!useGdiPlus)  {
			_dc->SelectObject(gdi.oldFont);
		}

		if (useVariableFontSize) 
		{
			for (int i = 0; i <= MAX_LABEL_SIZE; i++) 
			{
				if (fonts[i]) {
					if (!useGdiPlus) {
						CFont * f = (CFont*)fonts[i];
						f->DeleteObject();
						delete f;
					}
					else {
						Gdiplus::Font * f = (Gdiplus::Font*)fonts[i];
						delete f;
					}
					
					fonts[i] = NULL;
				}
			}
		}
		else {
			if (font) {
				font->DeleteObject();
				delete font;
				font = NULL;
			}
			
			if (gpFont)
			{
				delete gpFont;
				gpFont = NULL;
			}
		}

		// cleaning objects
		if (!useGdiPlus)
		{
			_dc->SelectObject(gdi.oldBrush);
			gdi.Release();
		}
		else
		{
			gdiPlus.Release();
		}
	} // category
	
	
	// restoring rendering options
	if (useGdiPlus)
	{
		gdiPlus.RestoreGraphics(_graphics);
	}
	else
	{
		_dc->RestoreDC(-1);
		_graphics->ReleaseHDC(m_hdc);
		_dc = NULL;
	}

	if (bRemoveDuplicates)
	{
		uniqueValues.clear();
	}
}

// ********************************************************************
//		CalcRotation
// ********************************************************************
void CLabelDrawer::CalcRotation(CLabelInfo* lbl, double& angle )
{
	if (!lbl) return;

	if (lbl->rotation == 0.0 && _mapRotation != 0.0)
	{
		angle = long(_mapRotation) % 360;
	}
	else if (lbl->rotation != 0.0 && _mapRotation != 0.0)
	{
		if (lbl->rotation > 0)
		{
			angle = long(lbl->rotation - _mapRotation) % 360;
			if (angle > 90 && angle < 180 || angle < -90 && angle >= -180)
				angle = long(lbl->rotation) % 360 + 180;
			else if (angle >= 180.0 && angle < 270.0 || angle < -180.0 && angle >= -270.0)
				angle = long(lbl->rotation) % 360 - 180;
			else
				angle = long(lbl->rotation) % 360;
		}
		else
		{
			angle = long(_mapRotation - lbl->rotation) % 360;
			if (angle > 0.0 && angle < 90.0)
				angle = long(lbl->rotation) % 360;
			else if (angle > 90.0 && angle < 180.0)
				angle = long(lbl->rotation) % 360 + 180.0;
			else if (angle >= 180.0 && angle < 270.0)
				angle = long(lbl->rotation) % 360 - 180.0;
			else
				angle = long(lbl->rotation) % 360;
		}
	}
	else
	{
		// we don't want our labels to be upside-down
		angle = long(lbl->rotation) % 360;
		if (angle > 90.0 && angle < 180)			angle += 180.0;
		else if (angle >= 180.0 && angle < 270.0)	angle -= 180.0;
	}
}

// ****************************************************************
//		AlignRectangle
// ****************************************************************
// rectangle for text printing is aligned around (0,0) with proper rotation
inline void CLabelDrawer::AlignRectangle(CRect& rect, tkLabelAlignment alignment)
{
	// width and height, projected on the X and Y axes
	switch (alignment)
	{
		case laTopLeft:
								rect.MoveToXY(- rect.Width(), -rect.Height());
								break;										
		case laTopCenter:
								rect.MoveToXY(- rect.Width()/2, -rect.Height());
								break;					
		case laTopRight:
								rect.MoveToXY(0, -rect.Height());
								break;										
		case laCenterLeft:
								rect.MoveToXY(- rect.Width(), -rect.Height()/2);	
								break;
		case laCenter:
								rect.MoveToXY(- rect.Width()/2, -rect.Height()/2);
								break;
		case laCenterRight:
								rect.MoveToXY(0, -rect.Height()/2);
								break;
		case laBottomLeft:
								rect.MoveToXY(- rect.Width(), 0);	
								break;										
		case laBottomCenter:
								rect.MoveToXY(- rect.Width()/2, 0);	
								break;
		case laBottomRight:
								rect.MoveToXY(0, 0);	
								break;										
			
	}
	return;
}