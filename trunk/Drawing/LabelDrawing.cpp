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
// 					DrawLabels()										
// *********************************************************************
void CLabelDrawer::DrawLabels( ILabels* LabelsClass )
{
	long numLabels;
	LabelsClass->get_Count(&numLabels);
	if (numLabels == 0)
		return;
	
	VARIANT_BOOL visible;
	LabelsClass->get_Visible(&visible);
	if (!visible) return;
	
	// checking scale for dynamic visibility
	VARIANT_BOOL dynVisibility;
	LabelsClass->get_DynamicVisibility(&dynVisibility);
	if (dynVisibility)
	{		
		double minScale, maxScale;
		LabelsClass->get_MinVisibleScale(&minScale);
		LabelsClass->get_MaxVisibleScale(&maxScale);
		double scale = _currentScale;
		if (scale < minScale || scale > maxScale) 
			return;
	}
	
	VARIANT_BOOL avoidCollisions;
	LabelsClass->get_AvoidCollisions(&avoidCollisions);
	
	tkVerticalPosition vp;
	LabelsClass->get_VerticalPosition(&vp);

	// ---------------------------------------------------------------
	//	 Scaling of labels
	// ---------------------------------------------------------------
	VARIANT_BOOL bScale;
	double scaleFactor = 1.0;
	LabelsClass->get_ScaleLabels(&bScale);
	if (bScale && _spatiallyReferenced)		// lsu: I see no reason to scale screen referenced labels
	{	
		double scale = _currentScale;
		if (scale == 0)
			return;
		
		double basicScale;
		LabelsClass->get_BasicScale(&basicScale);
		if (basicScale == 0.0)
		{
			basicScale = scale;
			LabelsClass->put_BasicScale(scale);
		}
		scaleFactor = basicScale/scale;
	}

	// ---------------------------------------------------------------
	//	 Extracting data
	// ---------------------------------------------------------------
	CLabels* lbs = static_cast<CLabels*>(LabelsClass);
	vector<vector<CLabelInfo*>*>* labels = lbs->get_LabelData();
	
	long numCategories;
	LabelsClass->get_NumCategories(&numCategories);

	VARIANT_BOOL useGdiPlus;
	LabelsClass->get_UseGdiPlus(&useGdiPlus);
	
	if (!useGdiPlus)
	{
		// let's check whether it's possible to draw everything by GDI
		CLabelOptions* options = ((CLabels*)LabelsClass)->get_LabelOptions();
		if (options->fontGradientMode != gmNone || options->fontTransparency != 255 || 
		  ((options->frameGradientMode != gmNone || options->frameTransparency != 255) && options->frameVisible))
		{
			useGdiPlus = true;
		}
	}
	
	if (_printing)
		useGdiPlus = VARIANT_TRUE;

	if (!useGdiPlus)
	{
		m_hdc = _graphics->GetHDC();
		_dc = CDC::FromHandle(m_hdc);
		_dc->SetBkMode(TRANSPARENT);
		_dc->SetGraphicsMode(GM_ADVANCED);
	}

	VARIANT_BOOL bRemoveDuplicates;
	LabelsClass->get_RemoveDuplicates(&bRemoveDuplicates);

	long m_buffer = 0;
	LabelsClass->get_CollisionBuffer(&m_buffer);

	// ------------------------------------------------------------
	//		GDI objects
	// ------------------------------------------------------------
	CFont fnt;
	CPen penFontOutline;
	CPen penHalo;
	CPen penFrameOutline;
	CBrush brushFrameBack;
	CBrush* oldBrush = NULL;
	CPen* oldPen = NULL;
	
	CFont* oldFont = NULL;
	CLabelOptions* m_options = NULL;
	
	CRect rect(0,0,0,0);	// bounding box for drawing
	UINT gdiAlignment;
	
	// ------------------------------------------------------------
	//		GDI+ objects
	// ------------------------------------------------------------
	// We create GDI+ objects even if no GDI+ mode is used as variables won't be visible through the code otherwise.
	// Create them inside loop for each category is a waste of time

	// pens
	Gdiplus::Pen* gpPenFontOutline = NULL;
	Gdiplus::Pen* gpPenHalo = NULL;
	Gdiplus::Pen* gpPenFrameOutline = NULL;

	// simple brushes in case no gradient is used
	Gdiplus::SolidBrush* gpBrushFont = NULL;
	Gdiplus::SolidBrush* gpBrushFrame = NULL;
	Gdiplus::SolidBrush* gpBrushShadow = NULL;
	
	Color clFont1(0,0,0);
	Color clFont2(0,0,0);
	Color clFrameBack1(0,0,0);
	Color clFrameBack2(0,0,0);

	// gradient brushes will be allocated dynamically as it's impossible 
	// to change properties of the created brush
	Gdiplus::LinearGradientBrush* gpBrushFontGrad = NULL;
	Gdiplus::LinearGradientBrush* gpBrushFrameGrad = NULL;
	
	CStringW wText;
	Font* gpFont = NULL;

	RectF gpRect(0.0f, 0.0f, 0.0f, 0.0f);

	StringFormat gpStringFormat;
	
	// -----------------------------------------
	// analyzing query expression
	// -----------------------------------------
	CComBSTR expr;
	LabelsClass->get_VisibilityExpression(&expr);

	std::vector<long> arrInit;
	CString err;
	bool useAll = true;
	
	if (SysStringLen(expr) > 0)
	{
		IShapefile* sf = lbs->get_ParentShapefile();
		if (sf)
		{
			CComPtr<ITable> tbl = NULL;
			sf->get_Table(&tbl);
			
			USES_CONVERSION;
			if (TableHelper::Cast(tbl)->QueryCore(OLE2CA(expr), arrInit, err))
			{
				useAll = false;
			}
		}			
	}
	
	// in case there is no expression or it has syntax error, all the labels will be drawn
	if (useAll)
	{
		for (long i = 0; i < numLabels; i++)
		{
			arrInit.push_back(i);
		}
	}
	
	// comparing with the visibility expression of the shapefile
	std::vector<long> arr;
	
	IShapefile* sf = lbs->get_ParentShapefile();
	
	VARIANT_BOOL synchronized;
	LabelsClass->get_Synchronized(&synchronized);

	// do we need to enable auto offset? shapefile type must be point and labels must be synchronized
	VARIANT_BOOL autoOffset;
	LabelsClass->get_AutoOffset(&autoOffset);
	if (autoOffset && sf != NULL)
	{
		ShpfileType shpType;
		sf->get_ShapefileType(&shpType);
		if (shpType == SHP_POINT || shpType == SHP_POINTM || shpType == SHP_POINTZ ||
			shpType == SHP_MULTIPOINT || shpType == SHP_MULTIPOINTZ || shpType == SHP_MULTIPOINTM)
		{
			autoOffset = synchronized;
		}
		else
		{
			autoOffset = VARIANT_FALSE;
		}
	}
	else
	{
		autoOffset = VARIANT_FALSE;
	}
	
	std::vector<ShapeData*>* shapeData = NULL;
	if (sf)
	{
		shapeData = ((CShapefile*)sf)->get_ShapeVector();
	}

	if (synchronized && sf)
	{
		long minSize;
		LabelsClass->get_MinDrawingSize(&minSize);
		
		for (unsigned int i = 0; i < arrInit.size(); i++)
		{
			int index = arrInit[i];
			if ((*shapeData)[index]->size >= minSize)
			{
				arr.push_back(index);
			}
		}
	}
	else
	{
		// for images all the labels will do
		for (int i = 0; i < numLabels; i++)
		{
			arr.push_back(i);
		}
	}

	// removing duplicates
	std::set<CString> uniqueValues;

	Gdiplus::CompositingQuality compositingQualityInit = Gdiplus::CompositingQualityDefault;
	Gdiplus::SmoothingMode smoothingModeInit = Gdiplus::SmoothingModeDefault;
	Gdiplus::TextRenderingHint textRenderingHintInit = Gdiplus::TextRenderingHintSystemDefault;

	if (!useGdiPlus)
	{
		_dc->SaveDC();
	}
	else
	{
		// applying rendering options
		compositingQualityInit = _graphics->GetCompositingQuality();
		smoothingModeInit = _graphics->GetSmoothingMode();
		textRenderingHintInit = _graphics->GetTextRenderingHint();
		
		tkTextRenderingHint textRenderingHint;
		LabelsClass->get_TextRenderingHint(&textRenderingHint);

		tkSmoothingMode smoothingMode = m_globalSettings.labelsSmoothingMode;
		tkCompositingQuality compositingQuality = m_globalSettings.labelsCompositingQuality;

		if (lbs->HasRotation()) {
			if (textRenderingHint == AntiAliasGridFit)
				textRenderingHint= ClearTypeGridFit;	// it looks better this way
		}

		_graphics->SetCompositingQuality((CompositingQuality)compositingQuality);
		_graphics->SetSmoothingMode((SmoothingMode)smoothingMode);
		_graphics->SetTextRenderingHint((TextRenderingHint)textRenderingHint);
		_graphics->SetTextContrast(0u);
	}

	// we'll start form the categories with the higher priority, therefore we'll start with the end of mmap
	//for ( multimap<long, int>::reverse_iterator p = mmap.rbegin() ; p != mmap.rend(); p++)
	for (int iCategory = numCategories - 1; iCategory >= -1; iCategory--)
	{
		LOGFONT lf;	
		if (iCategory != -1)
		{
			// Drawing with category settings
			ILabelCategory* cat;
			LabelsClass->get_Category(iCategory, &cat);
			if (cat == NULL) continue;
			
			CLabelCategory* coCat = static_cast<CLabelCategory*>(cat);
			m_options = reinterpret_cast<CLabelOptions*>(coCat->get_LabelOptions());
			cat->Release();
		}
		else
		{
			// Drawing with standard settings
			CLabels* coLabels = static_cast<CLabels*>(LabelsClass);
			m_options = coLabels->get_LabelOptions();
		}
		
		// skip the category if it is not visible		
		if (!m_options->visible) continue;

		// ---------------------------------------------------------------
		//	 Standard or category settings
		// ---------------------------------------------------------------
		CString s(m_options->fontName);
		fnt.CreatePointFont(m_options->fontSize * 10, s);
		fnt.GetLogFont(&lf);
		
		if (bScale)
		{
			lf.lfWidth = long((double)lf.lfWidth * scaleFactor);
			lf.lfHeight = long ((double)lf.lfHeight * scaleFactor);
		}
		if(abs(lf.lfHeight) < 4)	// changed 1 to 4; there is no way to read labels smaller than 4, but they slow down the performance
		{
			fnt.DeleteObject();
			continue;
		}
		
		if (!useGdiPlus)
			_dc->SetTextColor(m_options->fontColor);

		lf.lfItalic = m_options->fontStyle & fstItalic;
		lf.lfUnderline = m_options->fontStyle & fstUnderline;
		lf.lfStrikeOut = m_options->fontStyle & fstStrikeout;

		if (m_options->fontStyle & fstBold) 
		{
			lf.lfWeight = FW_BOLD;
		}
		else
		{
			lf.lfWeight = 0;
		}
		
		fnt.DeleteObject();
		fnt.CreateFontIndirectA(&lf);
		
		HDC hdc = NULL;
		if (useGdiPlus)
		{
			hdc = _graphics->GetHDC();
			_dc = CDC::FromHandle(hdc);
		}
		
		oldFont = _dc->SelectObject(&fnt);

		// we create separate pens/brushes for each drawing method
		if (useGdiPlus)
		{
			gpFont = new Gdiplus::Font(_dc->m_hDC);
			_graphics->ReleaseHDC(hdc);
			_dc = NULL;
			
			long alphaFont = (m_options->fontTransparency<<24);
			long alphaFrame = (m_options->frameTransparency<<24);
			
			clFont1.SetValue(alphaFont | BGR_TO_RGB(m_options->fontColor));
			clFont2.SetValue(alphaFont | BGR_TO_RGB(m_options->fontColor2));
			clFrameBack1.SetValue(alphaFrame | BGR_TO_RGB(m_options->frameBackColor));
			clFrameBack2.SetValue(alphaFrame | BGR_TO_RGB(m_options->frameBackColor2));
			
			gpPenFontOutline = new Pen(Color(alphaFont | BGR_TO_RGB(m_options->fontOutlineColor)), (Gdiplus::REAL)m_options->fontOutlineWidth);
			gpPenFontOutline->SetLineJoin(Gdiplus::LineJoinRound);

			double haloWidth = fabs(lf.lfHeight/16.0 * m_options->haloSize);
			gpPenHalo = new Pen(Color(alphaFont | BGR_TO_RGB(m_options->haloColor)), (Gdiplus::REAL)haloWidth);
			gpPenHalo->SetLineJoin(Gdiplus::LineJoinRound);

			gpPenFrameOutline = new Pen(Color(alphaFrame | BGR_TO_RGB(m_options->frameOutlineColor)), (Gdiplus::REAL)m_options->frameOutlineWidth);
			gpPenFrameOutline->SetDashStyle(DashStyle(m_options->frameOutlineStyle));

			gpBrushFont = new SolidBrush(clFont1);
			gpBrushFrame = new SolidBrush(clFrameBack1);
			gpBrushShadow = new SolidBrush(Color(alphaFont | BGR_TO_RGB(m_options->shadowColor)));
			
			this->AlignmentToGDIPlus(m_options->inboxAlignment, gpStringFormat);

			gpStringFormat.SetFormatFlags(StringFormatFlagsNoClip);	// doesn't work?
		}
		else
		{
			// TODO: limit options of tkLineStipples maybe part standard penStyles enumeration
			penFontOutline.CreatePen(PS_SOLID, m_options->fontOutlineWidth, m_options->fontOutlineColor);
			penFrameOutline.CreatePen(m_options->frameOutlineStyle, m_options->frameOutlineWidth, m_options->frameOutlineColor);
		
			double haloWidth = fabs(lf.lfHeight/16.0 * m_options->haloSize);
			penHalo.CreatePen(PS_SOLID, (int)haloWidth, m_options->haloColor);

			// we can select brush at once because it's the only one to use
			brushFrameBack.CreateSolidBrush(m_options->frameBackColor);
			oldBrush = _dc->SelectObject(&brushFrameBack);
			
			gdiAlignment = AlignmentToGDI(m_options->inboxAlignment);
		}
		
		// ---------------------------------------------------------------
		//	 drawing of single labels
		// ---------------------------------------------------------------
		unsigned int size = arr.size();
		for (unsigned int k = 0; k < size; k++ )
		{
			int i = arr[k];	// index of label

			vector<CLabelInfo*>* parts = (*labels)[i];
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
					USES_CONVERSION;
					wText = A2W(lbl->text);
					_graphics->MeasureString(wText, wText.GetLength(), gpFont, PointF(0.0f, 0.0f), &gpRect);
					
					// in some case we lose the last letter by clipping; 
					gpRect.Width += 1;
					gpRect.Height +=1;

					// converting to CRect to prevent duplication in the code below
					rect.left = static_cast<LONG>(gpRect.X);
					rect.top = static_cast<LONG>(gpRect.Y);
					rect.right = static_cast<LONG>(gpRect.X + gpRect.Width);
					rect.bottom = static_cast<LONG>(gpRect.Y + gpRect.Height);
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

				// START NEW CODE

				double angle,angleRad;
				if (lbl->rotation == 0.0 && _mapRotation != 0.0)
				{
					angle = long( _mapRotation) % 360;
				}
				else if (lbl->rotation != 0.0 && _mapRotation != 0.0)
				{
					if (lbl->rotation > 0)
					{
 						angle = long(lbl->rotation - _mapRotation) % 360;
						if (angle > 90 && angle < 180 || angle < -90 && angle >= -180)
							angle = long(lbl->rotation) % 360 + 180;
						else if (angle >= 180.0 && angle < 270.0 || angle < -180.0 && angle >= -270.0)
							angle = long( lbl->rotation) % 360 - 180;
						else
							angle = long( lbl->rotation) % 360;
					}
					else 
					{
 						angle = long( _mapRotation - lbl->rotation) % 360;
						if (angle > 0.0 && angle < 90.0)
							angle = long(lbl->rotation) % 360;
						else if (angle > 90.0 && angle < 180.0)
							angle = long(lbl->rotation) % 360 + 180.0;
						else if (angle >= 180.0 && angle < 270.0)
							angle = long( lbl->rotation) % 360  - 180.0;
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
				
				angleRad = angle/180.0 * pi_;
				//END OF NEW CODE

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
					ASSERT(!lbl->rotatedFrame);

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
					ASSERT(!lbl->horizontalFrame);

					lbl->horizontalFrame = rectNew;
					lbl->isDrawn = VARIANT_TRUE;
					if (_collisionList != NULL)
					{
						_collisionList->AddRectangle(rectNew, m_buffer, m_buffer);
					}
				}
				
				if (!useGdiPlus)
				{
					// ------------------------------------------------------
					//	GDI drawing w/o transparency, gradients, etc
					// ------------------------------------------------------
					XFORM xForm;
					xForm.eM11 = (FLOAT)cos(angleRad);
					xForm.eM12 = (FLOAT)sin(angleRad);
					xForm.eM21 = (FLOAT)-sin(angleRad);
					xForm.eM22 = (FLOAT)cos(angleRad);
					xForm.eDx =	(FLOAT)piX;
					xForm.eDy =	(FLOAT)piY;
					_dc->SetWorldTransform(&xForm);

					// drawing frame
					if (m_options->frameVisible)
					{
						oldPen = _dc->SelectObject(&penFrameOutline);
						DrawLabelFrameGdi(_dc, &rect, m_options);
						_dc->SelectObject(oldPen);
					}

					// drawing outlines
					if (m_options->shadowVisible)
					{
						_dc->SetWindowOrg(-m_options->shadowOffsetX , -m_options->shadowOffsetY);
						_dc->SetTextColor(m_options->shadowColor);
						_dc->DrawText(lbl->text, rect, gdiAlignment);
						_dc->SetTextColor(m_options->fontColor);
						_dc->SetWindowOrg(0,0);
					}

					if  (m_options->haloVisible)
					{
						_dc->BeginPath();
						_dc->DrawText(lbl->text,rect, gdiAlignment);
						_dc->EndPath();					
						oldPen = _dc->SelectObject(&penHalo);
						_dc->StrokePath();
						_dc->SelectObject(oldPen);
					}

					if(m_options->fontOutlineVisible)
					{
						_dc->BeginPath();
						_dc->DrawText(lbl->text,rect, gdiAlignment);
						_dc->EndPath();					
						oldPen = _dc->SelectObject(&penFontOutline);
						_dc->StrokePath();
						_dc->SelectObject(oldPen);
					}

					_dc->DrawText(lbl->text, rect, gdiAlignment);	// TODO: make a property (left/center/right)
				}
				else
				{
					// ------------------------------------------------------
					//	GDI+ drawing with transparency, gradients, etc
					// ------------------------------------------------------
					gpRect.X = (Gdiplus::REAL)rect.left;
					gpRect.Y = (Gdiplus::REAL)rect.top;
					gpRect.Width = (Gdiplus::REAL)rect.Width();
					gpRect.Height = (Gdiplus::REAL)rect.Height();


					Gdiplus::Matrix mtxInit;
					_graphics->GetTransform(&mtxInit);
					

					//Gdiplus::Matrix mtx;
					//mtx.Translate((Gdiplus::REAL)piX, (Gdiplus::REAL)piY);
					//mtx.Rotate((Gdiplus::REAL)angle);
					//_graphics->SetTransform(&mtx);
					
					_graphics->TranslateTransform((Gdiplus::REAL)piX, (Gdiplus::REAL)piY);
					_graphics->RotateTransform((Gdiplus::REAL)angle);
					
					// drawing frame
					if (m_options->frameTransparency != 0 && m_options->frameVisible)
					{
						if (m_options->frameGradientMode != gmNone)
						{
							gpBrushFrameGrad = new LinearGradientBrush(gpRect, clFrameBack1, clFrameBack2, (LinearGradientMode)m_options->frameGradientMode);
							this->DrawLabelFrameGdiPlus(*_graphics, gpBrushFrameGrad, *gpPenFrameOutline, gpRect, m_options);
							delete gpBrushFrameGrad;
						}
						else
						{
							this->DrawLabelFrameGdiPlus(*_graphics, gpBrushFrame, *gpPenFrameOutline, gpRect, m_options);
						}
					}
					
					if (m_options->fontTransparency != 0)
					{
						bool pathNeeded = m_options && (m_options->shadowVisible || m_options->haloVisible || m_options->fontOutlineVisible);
						GraphicsPath* gp = NULL;
						if (pathNeeded)
						{
							gp = new GraphicsPath();
							gp->StartFigure();
							FontFamily fam; 
							gpFont->GetFamily(&fam);
							gp->AddString(wText, wText.GetLength(), &fam, gpFont->GetStyle(), gpFont->GetSize(), gpRect, &gpStringFormat);
							gp->CloseFigure();
						}

						// drawing outlines
						if (m_options->shadowVisible)
						{
							Gdiplus::Matrix mtx1;
							mtx1.Translate((Gdiplus::REAL)m_options->shadowOffsetX, (Gdiplus::REAL)m_options->shadowOffsetY);
							gp->Transform(&mtx1);
							_graphics->FillPath(gpBrushShadow, gp);
							mtx1.Translate(Gdiplus::REAL(-2* m_options->shadowOffsetX), Gdiplus::REAL(-2 * m_options->shadowOffsetY));
							gp->Transform(&mtx1);
						}

						if  (m_options && m_options->haloVisible)
							_graphics->DrawPath(gpPenHalo, gp);

						if(m_options->fontOutlineVisible)
							_graphics->DrawPath(gpPenFontOutline, gp);

						if (m_options->fontGradientMode != gmNone)
						{
							gpBrushFontGrad = new LinearGradientBrush(gpRect, clFont1, clFont2, (LinearGradientMode)m_options->fontGradientMode);
							_graphics->DrawString(wText, wText.GetLength(), gpFont, gpRect, &gpStringFormat, gpBrushFontGrad);	// TODO: we need speed test here to choose the function
							delete gpBrushFontGrad;
						}
						else
						{
							_graphics->DrawString(wText, wText.GetLength(), gpFont, gpRect, &gpStringFormat, gpBrushFont);
						}
						if (pathNeeded) {
							delete gp;
						}
					}
					_graphics->SetTransform(&mtxInit);
				}
			}
		} // label
		
		if (useGdiPlus)
		{
			HDC hdc = _graphics->GetHDC();
			_dc = CDC::FromHandle(hdc);
		}

		_dc->SelectObject(oldFont);
		fnt.DeleteObject();

		if (useGdiPlus)
		{
			_graphics->ReleaseHDC(hdc);
			_dc = NULL;
		}

		if (!useGdiPlus)
		{
			_dc->SelectObject(oldBrush);
			brushFrameBack.DeleteObject();
			penFontOutline.DeleteObject();
			penFrameOutline.DeleteObject();
			penHalo.DeleteObject();
		}
		else
		{
			if (gpFont != NULL)
			{
				delete gpFont;
				gpFont = NULL;
			}

			delete gpPenFontOutline;
			delete gpPenHalo;
			delete gpPenFrameOutline;
			
			delete gpBrushFont;
			delete gpBrushFrame;
			delete gpBrushShadow;
		}
	} // category
	
	
	// restoring rendering options
	if (useGdiPlus)
	{
		_graphics->SetCompositingQuality(compositingQualityInit);
		_graphics->SetSmoothingMode(smoothingModeInit);
		_graphics->SetTextRenderingHint(textRenderingHintInit);
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
//		DrawLabelFrameGdiPlus
// ********************************************************************
void CLabelDrawer::DrawLabelFrameGdi(CDC* dc,  CRect* rect, CLabelOptions* m_options )
{
	switch (m_options->frameType)
	{
		case lfRectangle:
			dc->Rectangle(rect->left, rect->top, rect->right, rect->bottom);
			break;
		case lfRoundedRectangle:
			dc->RoundRect(rect->left, rect->top, rect->right, rect->bottom, rect->Height(), rect->Height());
			break;
		case lfPointedRectangle:
			dc->BeginPath();
			dc->MoveTo(rect->left, rect->top);
			dc->LineTo(rect->right, rect->top);
			dc->LineTo(rect->right + rect->Height()/4, (rect->top + rect->bottom)/2);
			dc->LineTo(rect->right, rect->bottom);
			dc->LineTo(rect->left, rect->bottom);
			dc->LineTo(rect->left - rect->Height()/4, (rect->top + rect->bottom)/2);
			dc->LineTo(rect->left, rect->top);
			dc->EndPath();
			dc->StrokeAndFillPath();
			break;
	}
	return;
}

// ********************************************************************
//		DrawLabelFrameGdiPlus
// ********************************************************************
void CLabelDrawer::DrawLabelFrameGdiPlus(Gdiplus::Graphics& graphics, Gdiplus::Brush* brush, Gdiplus::Pen& pen, Gdiplus::RectF& rect, CLabelOptions* m_options)
{
	switch (m_options->frameType)
	{
		case lfRectangle:
			_graphics->FillRectangle(brush, rect);
			_graphics->DrawRectangle(&pen, rect);
			break;
		case lfRoundedRectangle:
		{						
			float left = rect.X;
			float right = rect.X + rect.Width;
			float top = rect.Y;
			float bottom = rect.Y + rect.Height;

			Gdiplus::GraphicsPath* path = new Gdiplus::GraphicsPath();
			path->StartFigure();
			
			path->AddLine(left + rect.Height, top, right - rect.Height, top);
			path->AddArc(right - rect.Height, top, rect.Height, rect.Height, -90.0, 180.0);
			path->AddLine(right - rect.Height, bottom, left + rect.Height, bottom);
			path->AddArc(left, top, rect.Height, rect.Height, 90.0, 180.0);
			path->CloseFigure();
			_graphics->FillPath(brush, path);
			_graphics->DrawPath(&pen, path);
			delete path;
			break;
		}
		case lfPointedRectangle:
		{
			float left = rect.X;
			float right = rect.X + rect.Width;
			float top = rect.Y;
			float bottom = rect.Y + rect.Height;
			
			Gdiplus::GraphicsPath* path = new Gdiplus::GraphicsPath();
			path->StartFigure();
			path->AddLine(left + rect.Height/4, top, right - rect.Height/4, top);

			path->AddLine(right - rect.Height/4, top, right, (top + bottom)/2);
			path->AddLine(right, (top + bottom)/2, right - rect.Height/4, bottom);

			path->AddLine(right - rect.Height/4, bottom, left + rect.Height/4, bottom);
			
			path->AddLine(left + rect.Height/4, bottom, left, (top + bottom)/2);
			path->AddLine(left, (top + bottom)/2, left + rect.Height/4, top);
			
			path->CloseFigure();
			_graphics->FillPath(brush, path);
			_graphics->DrawPath(&pen, path);
			delete path;
			break;
		}
	}
	return;
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

// *****************************************************************
//		Converts label alignment to GDI constants
// *****************************************************************
inline UINT CLabelDrawer::AlignmentToGDI(tkLabelAlignment alignment)
{
	switch (alignment)
	{
		case laCenter:			return DT_VCENTER | DT_CENTER | DT_NOCLIP;
		case laCenterLeft:		return DT_VCENTER | DT_LEFT | DT_NOCLIP;
		case laCenterRight:		return DT_VCENTER | DT_RIGHT | DT_NOCLIP;
		case laBottomCenter:	return DT_BOTTOM | DT_CENTER | DT_NOCLIP;
		case laBottomLeft:		return DT_BOTTOM | DT_LEFT | DT_NOCLIP;
		case laBottomRight:		return DT_BOTTOM | DT_RIGHT | DT_NOCLIP;
		case laTopCenter:		return DT_TOP | DT_CENTER | DT_NOCLIP;
		case laTopLeft:			return DT_TOP | DT_LEFT | DT_NOCLIP;
		case laTopRight:		return DT_TOP | DT_RIGHT | DT_NOCLIP;
		default:				return DT_VCENTER | DT_RIGHT | DT_NOCLIP;
	}
}

// *****************************************************************
//		Converts label alignment to GDI+ constants
// *****************************************************************
inline void CLabelDrawer::AlignmentToGDIPlus(tkLabelAlignment alignment, Gdiplus::StringFormat& format)
{
	switch (alignment)
	{
		case laCenter:			format.SetAlignment(StringAlignmentCenter);		format.SetLineAlignment(StringAlignmentCenter); break;
		case laCenterLeft:		format.SetAlignment(StringAlignmentNear);		format.SetLineAlignment(StringAlignmentCenter); break;
		case laCenterRight:		format.SetAlignment(StringAlignmentFar);		format.SetLineAlignment(StringAlignmentCenter); break;
		case laBottomCenter:	format.SetAlignment(StringAlignmentCenter);		format.SetLineAlignment(StringAlignmentFar); break;
		case laBottomLeft:		format.SetAlignment(StringAlignmentNear);		format.SetLineAlignment(StringAlignmentFar); break;
		case laBottomRight:		format.SetAlignment(StringAlignmentFar);		format.SetLineAlignment(StringAlignmentFar); break;
		case laTopCenter:		format.SetAlignment(StringAlignmentCenter);		format.SetLineAlignment(StringAlignmentNear); break;
		case laTopLeft:			format.SetAlignment(StringAlignmentNear);		format.SetLineAlignment(StringAlignmentNear); break;
		case laTopRight:		format.SetAlignment(StringAlignmentFar);		format.SetLineAlignment(StringAlignmentNear); break;
	}
}

