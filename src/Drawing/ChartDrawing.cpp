/**************************************************************************************
 * File name: ChartDrawing.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CChartDrawer
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
#include "ChartDrawing.h"
#include <set>
#include "Shapefile.h"
#include "Charts.h"
#include "Macros.h"
#include "ChartsHelper.h"
#include "TableHelper.h"

using namespace Gdiplus;

// ******************************************************
//		CheckVisibility()
// ******************************************************
bool CChartDrawer::CheckVisibility(ICharts* charts)
{
	// checking scale for dynamic visibility
	VARIANT_BOOL dynVisibility;
	charts->get_DynamicVisibility(&dynVisibility);
	if (dynVisibility)
	{
		double minScale, maxScale;
		charts->get_MinVisibleScale(&minScale);
		charts->get_MaxVisibleScale(&maxScale);
		double scale = _currentScale;
		if (scale < minScale || scale > maxScale)
			return false;
	}

	return true;
}

// ******************************************************
//		PrepareValues()
// ******************************************************
bool CChartDrawer::PrepareValues(IShapefile* sf, ICharts* charts, ChartOptions* options, 
								 std::vector<long>& arr, std::vector<double*>& values)
{
	CShapefile* sfClass = (CShapefile*)sf;
	
	if (!ChartsHelper::ReadChartFields(sfClass, &values)) {
		return false;
	}

	// analyzing visibility expression
	long numFields;
	sf->get_NumFields(&numFields);

	long numShapes;
	sf->get_NumShapes(&numShapes);

	// -----------------------------------------
	// analyzing query expression
	// -----------------------------------------
	CComBSTR expr;
	charts->get_VisibilityExpression(&expr);

	CComPtr<ITable> tbl = NULL;
	sf->get_Table(&tbl);

	std::vector<long> arrInit;
	CStringW err;
	bool useAll = true;

	if (SysStringLen(expr) > 0)
	{
		USES_CONVERSION;
		if (TableHelper::Cast(tbl)->QueryCore(OLE2CW(expr), arrInit, err))
		{
			useAll = false;
		}
	}

	// in case there is no expression or it has syntax error, all the charts will be drawn
	if (useAll)
	{
		for (long i = 0; i < numShapes; i++)
		{
			arrInit.push_back(i);
		}
	}

	std::vector<ShapeRecord*>* positions;
	positions = ((CShapefile*)sf)->get_ShapeVector();
	if (!positions || positions->size() == 0)
		return false;

	// comparing with the visibility expression of the shapefile
	for (unsigned int i = 0; i < arrInit.size(); i++)
	{
		int index = arrInit[i];
		if ((*positions)[index]->isVisible())
		{
			arr.push_back(index);
		}
	}

	return true;
}

// ******************************************************
//		NormalizeValues()
// ******************************************************
bool CChartDrawer::NormalizeValues(ICharts* charts, IShapefile* sf, ChartOptions* options, vector<double*> values)
{
	CShapefile* sfClass = (CShapefile*)sf;

	long numBars;
	charts->get_NumFields(&numBars);

	std::vector<double> normalizationValues;

	if (ChartsHelper::ReadChartField(sfClass, &normalizationValues, options->normalizationField))
	{
		double val;
		for (unsigned int i = 0; i < values.size(); i++)
		{
			val = normalizationValues[i];
			if (val != 0.0)
			{
				for (int j = 0; j < numBars; j++)
				{
					values[i][j] /= val;
				}
			}
			else
			{
				for (int j = 0; j < numBars; j++)
				{
					values[i][j] = 0.0;
				}
			}
		}

		return true;
	}

	return false;
}

// ******************************************************
//		DrawCharts()
// ******************************************************
void CChartDrawer::DrawCharts(IShapefile* sf)
{
	if (!sf)	return;

	REAL dx = _graphics->GetDpiX();

	// reading chart properties
	CComPtr<ICharts> charts = NULL;
	sf->get_Charts(&charts);
	if (!charts)
		return;

	// the charts were cleared
	ICharts* icharts = charts;
	if (!((CCharts*)icharts)->GetChartsExist())
		return;

	if (!CheckVisibility(charts)) {
		return;
	}

	ChartOptions* options = ((CCharts*)icharts)->get_UnderlyingOptions();

	if (!options->visible)
		return;

	// reading the values
	
	std::vector<double*> values;
	std::vector<long> arr;
	PrepareValues(sf, charts, options, arr, values);

	// normalize values
	bool normalized = NormalizeValues(charts, sf, options, values);

	long numBars;
	charts->get_NumFields(&numBars);

	Pen pen(Utility::OleColor2GdiPlus(options->lineColor, (BYTE)options->transparency));
	std::vector<Brush*> brushes;
	std::vector<Brush*> brushesDimmed;
	PrepareBrushes(numBars, charts, options, brushes, brushesDimmed);

	// ------------------------------------------------------
	// initializing the font
	// ------------------------------------------------------
	CBrush brushFrame(options->valuesFrameColor);
	CPen penFrame(PS_SOLID, 1, RGB(150, 150, 150));

	bool vertical = (options->valuesStyle == vsVertical);
	CString sFormat = normalized ? "%.2f" : "%g";	// format for numbers

	CFont* oldFont = NULL;
	CFont fnt;
	Font* gdiPlusFont = NULL;

	if (options->valuesVisible )
	{
		gdiPlusFont = CreateGdiPlusFont(options);
	}

	// -------------------------------------------------------------- 
	//   Drawing of pie charts
	// --------------------------------------------------------------
	if (options->chartType == chtPieChart)
	{
		DrawPieCharts(sf, charts, options, values, arr, brushes, brushesDimmed, numBars, pen, sFormat, brushFrame, penFrame, gdiPlusFont);
	}
	else
	{
		DrawBarCharts(sf, charts, options, values, arr, brushes, brushesDimmed, numBars, pen, sFormat, vertical, brushFrame, penFrame, gdiPlusFont);
	}

	brushFrame.DeleteObject();

	if (gdiPlusFont) 
	{
		delete gdiPlusFont;
		gdiPlusFont = NULL;
	}

	for (size_t i = 0; i < values.size(); i++) {
		delete values[i];
	}

	for (unsigned int i = 0; i < brushes.size(); i++) {
		delete brushes[i];
	}

	for (unsigned int i = 0; i < brushesDimmed.size(); i++) {
		delete brushesDimmed[i];
	}
}

// ******************************************************
//		DrawPieCharts()
// ******************************************************
void CChartDrawer::DrawPieCharts(IShapefile* sf, ICharts* charts, ChartOptions* options, std::vector<double*>& values,
	std::vector<long>& arr, std::vector<Brush*>& brushes, std::vector<Brush*>& brushesDimmed, long numBars,
	Pen& pen, CString sFormat, CBrush& brushFrame, CPen& penFrame, Gdiplus::Font* gdiPlusFont)
{
	long collisionBuffer = ChartsHelper::GetCollisionBuffer(charts);
	REAL offsetX = static_cast<REAL>(ChartsHelper::GetOffsetX(charts));
	REAL offsetY = static_cast<REAL>(ChartsHelper::GetOffsetY(charts));
	
	//positions
	std::vector<ShapeRecord*>* positions = ((CShapefile*)sf)->get_ShapeVector();

	CShapefile* sfClass = (CShapefile*)sf;

	REAL pieThickness = (REAL)(options->thickness * options->tilt / 90.0f);
	REAL pieHeight;

	if (options->use3Dmode)
	{
		pieHeight = (REAL)(options->radius * (1.0f - options->tilt / 90.0f) * 2.0f);
	}
	else
	{
		pieHeight = (REAL)(options->radius * 2.0f);
	}

	REAL pieWidth = (REAL)options->radius * 2.0f;

	// size field
	bool useSizeField = false;
	std::vector<double> sizeValues;
	double maxValue = DBL_MIN;
	double minValue = DBL_MAX;

	if (options->useVariableRadius)
	{
		if (ChartsHelper::ReadChartField(sfClass, &sizeValues, options->sizeField))
		{
			for (unsigned int i = 0; i < sizeValues.size(); i++)
			{
				if (maxValue < sizeValues[i]) maxValue = sizeValues[i];
				if (minValue > sizeValues[i]) minValue = sizeValues[i];
			}
			useSizeField = true;
		}

		// in case size field wasn't specified we'll determine size by sum of values
		if (options->sizeField == -1 || !useSizeField)
		{
			for (unsigned int i = 0; i < values.size(); i++)
			{
				double sum = 0;
				for (int j = 0; j < numBars; j++)
					sum += values[i][j];

				if (maxValue < sum) maxValue = sum;
				if (minValue > sum) minValue = sum;
			}
		}
	}

	std::vector<ValueRectangle> allLabels;		// all labels
	size_t initLabelIndex = 0;

	for (size_t k = 0; k < arr.size(); k++)
	{
		int i = arr[k];		// extracting index from array of visible shapes

		double x, y;
		ShapeRecord* pnt = (*positions)[i];

		// within extents?
		if (pnt->chart->x < _extents->left || pnt->chart->x >_extents->right || pnt->chart->y > _extents->top || pnt->chart->y < _extents->bottom)
			continue;

		ProjectionToPixel(pnt->chart->x, pnt->chart->y, x, y);

		x += (double)offsetX;
		y += (double)offsetY;

		double sum = 0.0;
		for (int j = 0; j < numBars; j++)
			sum += values[i][j];

		if (sum == 0.0)
			continue;

		// size calculation
		if (options->useVariableRadius)
		{
			REAL ratio;
			if (options->sizeField == -1 || !useSizeField)
				ratio = (REAL)((sum - minValue) / (maxValue - minValue));
			else
				ratio = (REAL)((sizeValues[i] - minValue) / (maxValue - minValue));

			if (options->use3Dmode)
				pieHeight = (REAL)((options->radius + (options->radius2 - options->radius) * ratio)  * (1.0f - options->tilt / 90.0f) * 2.0f);
			else
				pieHeight = (REAL)(2.0*(options->radius + (options->radius2 - options->radius) * ratio));

			pieWidth = (REAL)(2.0 * (options->radius + (options->radius2 - options->radius) * ratio));
			offsetX = -pieWidth / 2.0f;
			offsetY = -pieHeight / 2.0f;
		}

		REAL xStart = REAL(int(x + offsetX));
		REAL yStart = REAL(int(y + offsetY));

		CRect* chartRect = NULL;
		chartRect = new CRect((int)(xStart - pieWidth / 2), (int)(yStart - pieHeight / 2), int(xStart + pieWidth / 2), int(yStart + pieHeight / 2));

		// collision avoidance
		if (options->avoidCollisions && _collisionList != NULL)
		{
			if (_collisionList->HaveCollision(*chartRect))
			{
				delete chartRect;
				continue;
			}
		}

		REAL startAngle = 0.0, sweepAngle = 0.0;
		GraphicsPath path, pathTemp;

		// -----------------------------------------------------
		//   testing if we can draw labels
		// -----------------------------------------------------
		std::vector<ValueRectangle> labels;
		if (options->valuesVisible)
		{
			startAngle = 0.0;
			int labelCount = 0;

			initLabelIndex = allLabels.size();
			for (int j = 0; j < numBars; j++)
			{
				sweepAngle = (REAL)(values[i][j] / sum * 360.0);

				// label drawing
				ValueRectangle value;
				CRect* rect = &value.rect;

				value.string = Utility::FormatNumber(values[i][j], sFormat);

				USES_CONVERSION;
				CStringW s = A2W(value.string);
				RectF rectF;
				_graphics->MeasureString(s, s.GetLength(), gdiPlusFont, PointF(0.0f, 0.0f), &rectF); 
				
				rect->left = static_cast<long>(rectF.X);
				rect->top = static_cast<long>(rectF.Y);
				rect->right = static_cast<long>(rectF.X + rectF.Width + 4);
				rect->bottom = static_cast<long>(rectF.Y + rectF.Height);

				REAL labelAngle = startAngle + sweepAngle / 2.0f + 90.0f;
				if (labelAngle > 360.0f)
				{
					labelAngle = labelAngle - 360.0f;
				}

				int x = (int)(xStart + sin(labelAngle / 180.0 * pi_) * options->radius);
				int y = (int)(yStart - cos(labelAngle / 180.0 * pi_) * options->radius);

				if (labelAngle >= 0.0 && labelAngle <= 180.0)
				{
					x += rect->Width() / 2;
				}
				else
				{
					x -= rect->Width() / 2;
				}

				if (labelAngle >= 90.0 && labelAngle <= 270.0)
				{
					y += rect->Height() / 2;
				}
				else
				{
					y -= rect->Height() / 2;
				}

				startAngle += sweepAngle;

				rect->MoveToX(x - rect->Width() / 2);
				rect->MoveToY(y - rect->Height() / 2);

				if (options->avoidCollisions && _collisionList != NULL)
				{
					if (_collisionList->HaveCollision(*rect))
					{
						continue;
					}
				}

				// saving the rect and text; we shall draw it when make sure that all the values don't have collisions
				labels.push_back(value);
				labelCount++;
			}

			if (labelCount != numBars)
			{
				// if there are collisions for values, all the chart won't be rendered
				if (chartRect)
				{
					delete chartRect;
					chartRect = NULL;
				}
				continue;
			}
			else
			{
				for (size_t n = 0; n < labels.size(); n++) {
					allLabels.push_back(labels[n]);
				}
			}
		}	// valuesVisible

		// ---------------------------------------
		// drawing the pie
		// ---------------------------------------
		startAngle = 0.0, sweepAngle = 0.0;
		xStart -= pieWidth / 2.0f;
		yStart -= pieHeight / 2.0f;
		for (int j = 0; j < numBars; j++)
		{
			sweepAngle = (REAL)(values[i][j] / sum * 360.0);
			if (sweepAngle > 0.0)
			{
				_graphics->FillPie(brushes[j], xStart, yStart, pieWidth, pieHeight, startAngle, sweepAngle);
				path.AddPie(xStart, yStart, pieWidth, (REAL)pieHeight, startAngle, sweepAngle);

				// 3D mode
				if (startAngle < 180.0 && options->use3Dmode)
				{
					GraphicsPath pathBottom;
					if (startAngle + sweepAngle > 180.0)
						pathBottom.AddArc(xStart, yStart + pieThickness, pieWidth, pieHeight, startAngle, 180.0f - startAngle);
					else
						pathBottom.AddArc(xStart, yStart + pieThickness, pieWidth, pieHeight, startAngle, sweepAngle);

					PathData pathData;
					pathBottom.GetPathData(&pathData);
					PointF* pntStart, *pntEnd;
					if (pathData.Count > 0)
					{
						pntStart = &(pathData.Points[0]);
						pntEnd = &(pathData.Points[pathData.Count - 1]);
					}
					pathBottom.AddLine(pntEnd->X, pntEnd->Y - (REAL)pieThickness, pntEnd->X, pntEnd->Y);

					if (startAngle + sweepAngle > 180.0)
					{
						pathBottom.AddArc(xStart, yStart, pieWidth, pieHeight, 180.0f, -(180.0f - startAngle));
					}
					else
						pathBottom.AddArc(xStart, yStart, pieWidth, pieHeight, startAngle + sweepAngle, -sweepAngle);

					pathBottom.AddLine(pntStart->X, pntStart->Y - pieThickness, pntStart->X, pntStart->Y);

					_graphics->FillPath(brushesDimmed[j], &pathBottom);
					_graphics->DrawPath(&pen, &pathBottom);
				}
			}
			startAngle += sweepAngle;
		}
		_graphics->DrawPath(&pen, &path);

		// drawing the values
		if (options->valuesVisible)
		{
			for (unsigned int i = initLabelIndex; i < allLabels.size(); i++)
			{
				CRect* rect = &allLabels[i].rect;
				_collisionList->AddRectangle(rect, collisionBuffer, collisionBuffer);
			}
		}

		ShapeRecord* info = (*positions)[i];
		if (info)
		{
			// storing rectangle for dragging operations				
			info->chart->frame = chartRect;
			info->chart->isDrawn = true;
		}

		if (options->avoidCollisions && _collisionList != NULL)
		{
			_collisionList->AddRectangle(chartRect, collisionBuffer, collisionBuffer);
		}
	}		// chart cycle

	if (options->valuesVisible)
	{
		DrawLabels(gdiPlusFont, options, allLabels, false, false);
	}
}

// ******************************************************
//		DrawBarCharts()
// ******************************************************
void CChartDrawer::DrawBarCharts(IShapefile* sf, ICharts* charts, ChartOptions* options, std::vector<double*>& values, 
			std::vector<long>& arr, std::vector<Brush*>& brushes, std::vector<Brush*>& brushesDimmed, 
			long numBars, Pen& pen, CString sFormat, bool vertical, CBrush& brushFrame, CPen& penFrame,
			Gdiplus::Font* gdiPlusFont)
{
	long collisionBuffer = ChartsHelper::GetCollisionBuffer(charts);
	long offsetX = ChartsHelper::GetOffsetX(charts);
	long offsetY = ChartsHelper::GetOffsetY(charts);

	std::vector<ShapeRecord*>* positions = ((CShapefile*)sf)->get_ShapeVector();

	PointF* points = new PointF[5];

	double maxValue = DBL_MIN;
	for (unsigned int i = 0; i < values.size(); i++)
	{
		for (int j = 0; j < numBars; j++)
		{
			if (maxValue < values[i][j]) maxValue = values[i][j];
		}
	}

	for (unsigned int k = 0; k < arr.size(); k++)
	{
		int i = arr[k];	// extracting index of shape from the array of visible shapes

		double x, y;
		ShapeRecord* pnt = (*positions)[i];

		// within extents?
		if (pnt->chart->x < _extents->left || pnt->chart->x >_extents->right || pnt->chart->y > _extents->top || pnt->chart->y < _extents->bottom)
			continue;

		ProjectionToPixel(pnt->chart->x, pnt->chart->y, x, y);

		x += (double)offsetX;
		y += (double)offsetY;

		// calculating max height
		double max = 0.0;
		for (int j = 0; j < numBars; j++)
		{
			if (values[i][j] > max)
				max = values[i][j];
		}
		double maxHeight = options->barHeight / maxValue * max;

		int xStart = int(x - numBars * options->barWidth / 2.0);
		int yStart = int(y + maxHeight / 2.0);

		double angle = 45.0;

		// checking whether the bars are visible
		bool canDraw = false;
		for (int j = 0; j < numBars; j++)
		{
			int height = int((double)options->barHeight / maxValue * values[i][j]);
			if (height > 0)
				canDraw = true;
		}

		if (!canDraw)
		{
			continue;
		}

		// collision avoidance
		CRect* rectChart = NULL;
		if (options->avoidCollisions && _collisionList != NULL)
		{
			rectChart = new CRect((int)xStart, int(yStart - options->barHeight), int(xStart + options->barWidth * numBars), (int)yStart);
			if (options->use3Dmode)
			{
				rectChart->right += (long)(sin(pi_ / 4.0) * options->thickness);
				rectChart->top -= (long)(cos(pi_ / 4.0) * options->thickness);
			}

			if (_collisionList->HaveCollision(*rectChart))
			{
				delete rectChart;
				continue;
			}
		}

		std::vector<ValueRectangle> labels;
		if (options->valuesVisible)
		{
			int xAdd = (int)(sin(45.0 / 180 * pi_) * options->thickness);

			// drawing values
			xStart = int(x - numBars * options->barWidth / 2.0);
			yStart = int(y + maxHeight / 2.0);

			for (int j = 0; j < numBars; j++)
			{
				int height = int((double)options->barHeight / maxValue * values[i][j]);
				if (height != 0)
				{
					CString s = Utility::FormatNumber(values[i][j], sFormat);
					CRect* rect = new CRect();

					USES_CONVERSION;
					CStringW sWide = A2W(s);
					RectF rectF;
					PointF pnt(0.0f, 0.0f);
					_graphics->MeasureString(sWide, sWide.GetLength(), gdiPlusFont, pnt, &rectF);

					rect->left = static_cast<long>(rectF.X);
					rect->top = static_cast<long>(rectF.Y);
					rect->right = static_cast<long>(rectF.X + rectF.Width + 4);
					rect->bottom = static_cast<long>(rectF.Y + rectF.Height - 2);

					if (!vertical)
					{
						rect->MoveToY(yStart - rect->Height() - height - 3);

						if (j == 0)
						{
							rect->MoveToX(xStart + options->barWidth / 2 - rect->Width());
						}
						else if (j == numBars - 1)
						{
							rect->MoveToX(xStart + options->barWidth / 2);
						}
						else
						{
							rect->MoveToX(xStart + options->barWidth / 2 - rect->Width() / 2);
						}
					}
					else
					{
						int xShift, yShift;
						xShift = xStart + options->barWidth / 2 - rect->Height() / 2;
						yShift = yStart - rect->Width() - height - 6;

						rect->MoveToXY(xShift, yShift);
					}

					if (options->avoidCollisions && _collisionList != NULL)
					{
						if (_collisionList->HaveCollision(*rect))
						{
							delete rect;
							continue;
						}
					}

					// we shall store the label, to keep the collision list clean
					ValueRectangle value;
					value.string = s;

					// drawing frame							
					if (!vertical)
					{
						CRect r(rect->left - 2, rect->top, rect->right + 2, rect->bottom);
						value.rect = r;
					}
					else
					{
						CRect r(rect->left, rect->top - 2, rect->right, rect->bottom + 2);
						value.rect = r;
					}
					labels.push_back(value);

					delete rect;
				}

				xStart += options->barWidth;
			}

			// some of the labels has collision with other charts
			if (labels.size() != numBars)
			{
				if (rectChart)
				{
					delete rectChart;
					rectChart = NULL;
				}
				continue;
			}
		}

		// drawing the bars
		xStart = int(x - numBars * options->barWidth / 2.0);
		for (int j = 0; j < numBars; j++)
		{
			int height = int((double)options->barHeight / maxValue * values[i][j]);
			if (height != 0)
			{
				// drawing bars
				_graphics->FillRectangle(brushes[j], xStart, yStart - height, options->barWidth, height);
				_graphics->DrawRectangle(&pen, xStart, yStart - height, options->barWidth, height);

				// 3D mode
				if (options->use3Dmode)
				{
					points[0].X = (REAL)xStart;
					points[1].X = (REAL)(xStart + sin(angle / 180 * pi_) * options->thickness);
					points[2].X = (REAL)points[1].X + options->barWidth;
					points[3].X = (REAL)xStart + options->barWidth;
					points[4].X = (REAL)xStart;

					points[0].Y = (REAL)yStart - height;
					points[1].Y = (REAL)(yStart - height - cos(angle / 180 * pi_) *  options->thickness);
					points[2].Y = (REAL)points[1].Y;
					points[3].Y = (REAL)yStart - height;
					points[4].Y = (REAL)yStart - height;

					_graphics->DrawPolygon(&pen, points, 5);
					_graphics->FillPolygon(brushesDimmed[j], points, 5);

					points[0].X = (REAL)xStart + options->barWidth;
					points[1].X = (REAL)(points[0].X + sin(angle / 180 * pi_) *  options->thickness);
					points[2].X = (REAL)points[1].X;
					points[3].X = (REAL)points[0].X;
					points[4].X = (REAL)points[0].X;

					points[0].Y = (REAL)yStart - height;
					points[1].Y = (REAL)(points[0].Y - cos(angle / 180 * pi_) *  options->thickness);
					points[2].Y = (REAL)points[1].Y + height;
					points[3].Y = (REAL)points[0].Y + height;
					points[4].Y = (REAL)points[0].Y;

					_graphics->DrawPolygon(&pen, points, 5);
					_graphics->FillPolygon(brushesDimmed[j], points, 5);
				}
			}
			xStart += options->barWidth;
		}

		// drawing labels
		DrawLabels(gdiPlusFont, options, labels, true, vertical);

		// adding chart rect to collision list
		if (options->avoidCollisions && _collisionList != NULL)
		{
			_collisionList->AddRectangle(rectChart, collisionBuffer, collisionBuffer);
			ShapeRecord* info = (*positions)[i];
			if (info)
			{
				info->chart->frame = rectChart;
				info->chart->isDrawn = true;
			}
		}
	}

	// cleaning
	if (points)
	{
		delete[] points;
	}
}

// *********************************************************************
// 					CreateFont()										
// *********************************************************************
Font* CChartDrawer::CreateGdiPlusFont(ChartOptions* options)
{
	CStringW fontName = OLE2W(options->valuesFontName);
	REAL fontSize = static_cast<REAL>(options->valuesFontSize);

	int style = FontStyleRegular;
	if (options->valuesFontItalic) style = style | FontStyleItalic;
	if (options->valuesFontBold) style = style | FontStyleBold;

	return new Gdiplus::Font(fontName, (REAL)fontSize, style);
}

// ******************************************************
//		DrawPieLabelsGdi()
// ******************************************************
void CChartDrawer::DrawLabels(Gdiplus::Font* font, ChartOptions* options, std::vector<ValueRectangle>& labels,
	bool addToCollisionList, bool vertical)
{
	SolidBrush textBrush(Color::Black);

	SolidBrush brush(Utility::OleColor2GdiPlus(options->valuesFrameColor));
	Pen pen(Color(150, 150, 150), 1.0f);

	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);

	for (unsigned int i = 0; i < labels.size(); i++)
	{
		CRect* rect = &labels[i].rect;

		RectF r(
			static_cast<REAL>(rect->left),
			static_cast<REAL>(rect->top),
			static_cast<REAL>(rect->Width()),
			static_cast<REAL>(rect->Height()));

		Gdiplus::Matrix transform;

		if (vertical)
		{
			_graphics->GetTransform(&transform);
			_graphics->TranslateTransform(r.X, r.Y);
			_graphics->RotateTransform(-90.0f);

			r.X = -r.Width;
			r.Y = 0;
		}

		if (options->valuesFrameVisible)
		{
			_graphics->FillRectangle(&brush, r);
			_graphics->DrawRectangle(&pen, r);
		}

		USES_CONVERSION;
		CStringW s = A2W(labels[i].string);

		if (!vertical) {
			_graphics->DrawString(s, s.GetLength(), font, r, &format, &textBrush);
		}
		else {
			_graphics->DrawString(s, s.GetLength(), font, r, &format, &textBrush);
			_graphics->SetTransform(&transform);

		}

		if (addToCollisionList && options->avoidCollisions && _collisionList != NULL)
		{
			_collisionList->AddRectangle(rect, 0, 0);
		}
	}
}

// ******************************************************
//		PrepareBrushes()
// ******************************************************
void CChartDrawer::PrepareBrushes(long numBars, ICharts* charts, ChartOptions* options,
	std::vector<Brush*>& brushes, std::vector<Brush*>& brushesDimmed)
{
	for (long i = 0; i < numBars; i++)
	{
		CComPtr<IChartField> chartField = NULL;
		charts->get_Field(i, &chartField);

		OLE_COLOR color;
		chartField->get_Color(&color);

		Brush* br = new SolidBrush(Utility::OleColor2GdiPlus(color, (BYTE)options->transparency));
		brushes.push_back(br);

		short r = GetRValue(color) - 100;	if (r < 0) r = 0;
		short g = GetGValue(color) - 100;	if (g < 0) g = 0;
		short b = GetBValue(color) - 100;	if (b < 0) b = 0;

		Brush* brDimmed = new SolidBrush(Utility::OleColor2GdiPlus(RGB(r, g, b), (BYTE)options->transparency));
		brushesDimmed.push_back(brDimmed);
	}
}