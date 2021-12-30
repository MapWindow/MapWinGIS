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
#include "LabelDrawingHelper.h"
#include "LabelsHelper.h"
#include "AngleHelper.h"

using namespace Gdiplus;

// *********************************************************************
// 					InitSettings()										
// *********************************************************************
void CLabelDrawer::InitSettings(LabelSettings& settings, ILabels* labels, IShapefile* sf)
{
	labels->get_NumCategories(&settings.numCategories);

	labels->get_RemoveDuplicates(&settings.removeDuplicates);

	labels->get_CollisionBuffer(&settings.buffer);

	labels->get_AvoidCollisions(&settings.avoidCollisions);

	settings.scaleFactor = GetScaleFactor(labels);

	settings.autoOffset = GetAutoOffset(labels, sf);

	settings.numLabels = LabelsHelper::GetCount(labels);

	CLabels* lbs = ((CLabels*)labels);

	settings.hasRotation = lbs->HasRotation();

	settings.useVariableFontSize = lbs->RecalculateFontSize();
}

// *********************************************************************
// 					DrawLabels()										
// *********************************************************************
void CLabelDrawer::DrawLabels(ILabels* labels)
{
	if (!CheckVisibility(labels)) {
		return;
	}

	// ------------------------------------------------
	//  accessing data
	// ------------------------------------------------
	CLabels* lbs = static_cast<CLabels*>(labels);
	vector<vector<CLabelInfo*>*>* labelData = lbs->get_LabelData();

	IShapefile* sf = lbs->get_ParentShapefile();
	std::vector<ShapeRecord*>* shapeData = NULL;
	if (sf) {
		shapeData = ((CShapefile*)sf)->get_ShapeVector();
	}

	// ---------------------------------
	//  settings, filter and sorting
	// ---------------------------------
	LabelSettings settings;
	InitSettings(settings, labels, sf);
	
	vector<bool> visibilityMask(settings.numLabels, false);
	GetVisibilityMask(labels, sf, shapeData, visibilityMask);

	// sort them if sort field is specified
	vector<long>* indices = NULL;
	if (sf) {
		((CShapefile*)sf)->GetSorting(&indices);
	}

	if (indices && indices->size() != settings.numLabels) {
		indices = NULL;
	}

	// ---------------------------------
	// preparing to draw
	// ---------------------------------
	GdiLabelDrawer gdi;
	GdiPlusLabelDrawer gdiPlus;
	CRect rect(0, 0, 0, 0);
	std::set<CStringW> uniqueValues;

	bool useGdiPlus = GetUseGdiPlus(labels);
	if (useGdiPlus) {
		gdiPlus.InitGraphics(_graphics, labels);
	}
	else {
		gdi.InitDc(_graphics);
	}

	// ---------------------------------------------------------------
	//	 drawing categories - we'll start from the categories 
	//   with the higher priority, therefore reverse order
	// ---------------------------------------------------------------
	for (int categoryIndex = settings.numCategories - 1; categoryIndex >= -1; categoryIndex--)
	{
		CLabelOptions* options = GetCategoryOptions(labels, categoryIndex);

		if (!options || !options->visible) continue;

		// we create separate pens/brushes for each drawing method
		if (useGdiPlus) {
			gdiPlus.InitFromCategory(options, settings.hasRotation);
		}
		else {
			gdi.InitFromCategory(options);
		}

		if (!settings.useVariableFontSize)
		{
			if (useGdiPlus) {
				gdiPlus.SelectFont(options, options->fontSize, settings.scaleFactor);
			}
			else {
				gdi.SelectFont(options, options->fontSize, settings.scaleFactor);
			}
		}

		// ---------------------------------------------------------------
		//	 drawing labels within category
		// ---------------------------------------------------------------
		for (long k = 0; k < settings.numLabels; k++)
		{
			long i = indices ? (*indices)[k] : k;

			if (!visibilityMask[i])	{
				continue;
			}

			vector<CLabelInfo*>* parts = (*labelData)[i];
			for (int j = 0; j < (int)parts->size(); j++)
			{
				CLabelInfo* lbl = (*parts)[j];

				if (lbl->x < _extents->left)   continue;
				if (lbl->x > _extents->right)  continue;
				if (lbl->y < _extents->bottom) continue;
				if (lbl->y > _extents->top)	 continue;
				if (lbl->text.GetLength() <= 0) continue;

				if ((lbl->category == categoryIndex) || (categoryIndex == -1 && (lbl->category < 0 || lbl->category >= settings.numCategories)))	{}
				else continue;	/* Wrong category */
				
				// blocking the labels with the text already displayed
				if (settings.removeDuplicates)
				{
					if (uniqueValues.find(lbl->text) != uniqueValues.end())
						continue;
					else
						uniqueValues.insert(lbl->text);
				}

				// choosing appropriate font
				if (settings.useVariableFontSize)
				{
					if (useGdiPlus) {
						gdiPlus.SelectFont(options, lbl, settings.scaleFactor);
					}
					else {
						gdi.SelectFont(options, lbl, settings.scaleFactor);
					}
				}

				// measuring label
				if (useGdiPlus) {
					gdiPlus.MeasureString(lbl, rect);
				}
				else {
					gdi.MeasureString(lbl, rect);
				}

				// rotation angle
				double angle, angleRad;
				LabelDrawingHelper::CalcRotation(lbl, _mapRotation, angle);
				angleRad = AngleHelper::ToRad(angle);

				// calculating screen rectangle
				double piX, piY;

				// Fix for MWGIS-79:
				int shapeSize = 0;
				if (sf)
					shapeSize = (*shapeData)[i]->size;
				CalcScreenRectangle(options, lbl, settings.autoOffset, shapeSize, rect, piX, piY);

				// do we have overlaps?
				if (!TryAvoidCollisions(lbl, settings.avoidCollisions, rect, piX, piY, settings.buffer, angleRad)) {
					continue;
				}

				// actual drawing
				if (useGdiPlus) {
					gdiPlus.DrawLabel(options, rect, piX, piY, angle);
				}
				else {
					gdi.DrawLabel(options, lbl, rect, angleRad, piX, piY);
				}
			}
		} // label

		if (useGdiPlus) {
			gdiPlus.ReleaseForCategory(settings.useVariableFontSize);
		}
		else {
			gdi.ReleaseForCategory(settings.useVariableFontSize);
		}
	} // category

	// restoring rendering options
	if (useGdiPlus) {
		gdiPlus.RestoreGraphics();
	}
	else {
		gdi.ReleaseDc();
	}
}

// *********************************************************************
// 			GetCategoryOptions()										
// *********************************************************************
CLabelOptions* CLabelDrawer::GetCategoryOptions(ILabels* labels, int categoryIndex)
{
	if (categoryIndex != -1)
	{
		CComPtr<ILabelCategory> category = NULL;
		labels->get_Category(categoryIndex, &category);

		if (!category) { 
			return NULL;
		}
		
		CLabelCategory* ct = (CLabelCategory*)&(*category);
		return reinterpret_cast<CLabelOptions*>(ct->get_LabelOptions());
	}
	else
	{
		// standard settings
		CLabels* lbs = static_cast<CLabels*>(labels);
		return lbs->get_LabelOptions();
	}
}

// *********************************************************************
// 			CalcScreenRectangle()										
// *********************************************************************
void CLabelDrawer::CalcScreenRectangle(CLabelOptions* options, CLabelInfo* lbl, bool autoOffset, int shapeSize, CRect& rect, double& piX, double& piY)
{
	int offset = autoOffset ? shapeSize / 2 + 2 : 0;

	// laCenter alignment isn't allowed for auto mode
	tkLabelAlignment align = (autoOffset && options->alignment == laCenter) ? laCenterRight : options->alignment;
	LabelDrawingHelper::AlignRectangle(rect, options->alignment);

    double lblX = lbl->x + lbl->offsetX, lblY = lbl->y + lbl->offsetY;

	if (_spatiallyReferenced)
	{
		this->ProjectionToPixel(lblX, lblY, piX, piY);
	}
	else
	{
		// no calculations for screen referenced labels
		piX = lblX;
		piY = lblY;
	}

	// MWGIS-229; erroneous code removed from here, handling rounded rectangles.
	// Rounded and Pointed rectangles are now properly handled in GDI DrawFrame
	// and GDI+ DrawLabelFrame methods (see https://mapwindow.atlassian.net/browse/MWGIS-229)

	// adding padding				
	if (options->frameVisible)
	{
		rect.left -= (LONG)ceil(double(options->framePaddingX / 2.0));
		rect.right += (LONG)ceil(double(options->framePaddingX / 2.0));
		rect.top -= (LONG)ceil(double(options->framePaddingY / 2.0));
		rect.bottom += (LONG)ceil(double(options->framePaddingY / 2.0));

		offset += options->framePaddingX;
	}

	if (autoOffset)
	{
		LabelDrawingHelper::UpdateAutoOffset(rect, align, offset);
	}

    rect.left += (LONG) options->offsetX;
    rect.right += (LONG) options->offsetX;
    rect.top += (LONG) options->offsetY;
    rect.bottom += (LONG) options->offsetY;
}

// *********************************************************************
// 					TryAvoidCollisions()										
// *********************************************************************
bool CLabelDrawer::TryAvoidCollisions(CLabelInfo* lbl, VARIANT_BOOL avoidCollisions, CRect& rect, double piX, double piY, long buffer, double angleRad)
{
	if (angleRad != 0.0)
	{
		CRotatedRectangle* rectNew = new CRotatedRectangle(rect);
		double angle1 = 2 * pi_ - angleRad;
		for (int i = 0; i < 4; i++)
		{
			long x = rectNew->points[i].x;
			long y = rectNew->points[i].y;
			rectNew->points[i].x = LONG(x * cos(angle1) + y * sin(angle1) + piX);
			rectNew->points[i].y = LONG(-x * sin(angle1) + y * cos(angle1) + piY);
		}

		if (avoidCollisions && _collisionList != NULL)
		{
			if (_collisionList->HaveCollision(*rectNew))
			{
				delete rectNew;
				return false;
			}
		}

		// memory must be cleared at this point
		lbl->rotatedFrame = rectNew;
		lbl->isDrawn = VARIANT_TRUE;
		if (_collisionList != NULL)
		{
			_collisionList->AddRotatedRectangle(rectNew, buffer, buffer);
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
			if (_collisionList->HaveCollision(*rectNew))
			{
				delete rectNew;
				return false;
			}
		}
		// memory must be cleared at this point

		lbl->horizontalFrame = rectNew;
		lbl->isDrawn = VARIANT_TRUE;
		if (_collisionList != NULL)
		{
			_collisionList->AddRectangle(rectNew, buffer, buffer);
		}
	}

	return true;
}

// *********************************************************************
// 					GetExpressionFilter()										
// *********************************************************************
bool CLabelDrawer::GetExpressionFilter(ILabels* labels, IShapefile* sf, vector<long>& filter)
{
	CComBSTR expr;
	labels->get_VisibilityExpression(&expr);

	CStringW err;

	if (SysStringLen(expr) > 0)
	{
		if (sf)
		{
			CComPtr<ITable> tbl = NULL;
			sf->get_Table(&tbl);

			USES_CONVERSION;
			if (TableHelper::Cast(tbl)->QueryCore(OLE2CW(expr), filter, err))
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
void CLabelDrawer::GetVisibilityMask(ILabels* labels, IShapefile* sf, std::vector<ShapeRecord*>* shapeData, std::vector<bool>& visibilityMask)
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
			visibilityMask[index] = (*shapeData)[index]->wasRendered() && (*shapeData)[index]->size >= minSize;
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
// 					CheckVisibility()										
// *********************************************************************
bool CLabelDrawer::CheckVisibility(ILabels* labels)
{
	if (m_globalSettings.forceHideLabels) {
		return false;
	}

	if (LabelsHelper::GetCount(labels) == 0) {
		return false;
	}

	VARIANT_BOOL visible;
	labels->get_Visible(&visible);
	if (!visible) return false;

	if (!CheckDynamicVisibility(labels)) {
		return false;
	}

	return true;
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

		long minZoom, maxZoom;
		labels->get_MinVisibleZoom(&minZoom);
		labels->get_MaxVisibleZoom(&maxZoom);
		
		if (_currentScale < minScale || _currentScale > maxScale ||
			_currentZoom < minZoom || _currentZoom > maxZoom) {
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

	CLabelOptions* options = ((CLabels*)labels)->get_LabelOptions();

	if (useGdiPlus && options->frameVisible) {
		// GDI generally looks better, however it can't be used without frame / background
		// since we are using transparent GDI+ buffer (GDI doesn't have alpha blending)
		useGdiPlus = VARIANT_FALSE;		
	}

	if (!useGdiPlus)
	{
		// when there is some form of alpha blending is used, 
		// we have to stick with GDI+
		if ((options->fontGradientMode != gmNone || options->fontTransparency != 255) ||
			((options->frameGradientMode != gmNone || options->frameTransparency != 255) && options->frameVisible))
		{
			useGdiPlus = VARIANT_TRUE;
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


