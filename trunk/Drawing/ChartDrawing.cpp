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
 // Sergei Leschinski (lsu) 25 june 2010 - created the file.

#include "stdafx.h"
#include "ChartDrawing.h"
#include <set>
#include "Shapefile.h"
#include "Charts.h"
#include "TableClass.h"
#include "Macros.h"

// ******************************************************
//		DrawCharts()
// ******************************************************
void CChartDrawer::DrawCharts(IShapefile* sf)
{
	if ( !sf )	return;

	// reading chart properties
	ICharts* charts = NULL;
	sf->get_Charts(&charts);
	if (!charts)
		return;

	// the charts were cleared
	if (!((CCharts*)charts)->_chartsExist)
	{
		charts->Release();
		return;
	}
	
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
			return;
	}

	long _collisionBuffer;
	long _offsetX;
	long _offsetY;
	charts->get_CollisionBuffer(&_collisionBuffer);
	charts->get_OffsetX(&_offsetX);
	charts->get_OffsetY(&_offsetY);
	
	ChartOptions* _options = ((CCharts*)charts)->get_UnderlyingOptions();

	if ( !  _options->visible )
	{
		charts->Release();
		return;
	}

	// reading values
	CShapefile* sfClass = (CShapefile*)sf;
	std::vector<double*> values;
	if (!sfClass->ReadChartFields(&values))
	{
		charts->Release();
		return;
	}
	
	//positions
	std::vector<ShapeData*>* positions;
	positions = ((CShapefile*)sf)->get_ShapeVector();
	if ( !positions) 
	{
		charts->Release();
		return;
	}
	if ((positions->size() == 0 || values.size() == 0) ||
		(positions->size()!= values.size()))
	{
		charts->Release();
		return;
	}
	
	// analysing visibility expression
	long numFields;
	sf->get_NumFields(&numFields);
	
	long numShapes;
	sf->get_NumShapes(&numShapes);
	
	// -----------------------------------------
	// analyzing query expression
	// -----------------------------------------
	BSTR expr;
	charts->get_VisibilityExpression(&expr);

	ITable* tbl = NULL;
	sf->get_Table(&tbl);
	
	std::vector<long> arrInit;
	CString err;
	bool useAll = true;
	
	if (SysStringLen(expr) > 0)
	{
		USES_CONVERSION;
		if (((CTableClass*)tbl)->Query_(OLE2CA(expr), arrInit, err))
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

	// comparing with the visibility expression of the shapefile
	std::vector<long> arr;
	for (unsigned int i = 0; i < arrInit.size(); i++)
	{
		int index = arrInit[i];
		if ((*positions)[index]->isVisible)
		{
			arr.push_back(index);
		}
	}

	// --------------------------------------------
	// normalization field
	// --------------------------------------------
	long numBars;
	charts->get_NumFields(&numBars);
	std::vector<double> normalizationValues;
	bool normalized = false;
	if (sfClass->ReadChartField(&normalizationValues, _options->normalizationField))
	{
		double val;
		for (unsigned int i = 0; i < values.size(); i++)
		{
			val = normalizationValues[i];
			if (val != 0.0)
			{
				for (int j = 0; j< numBars ; j++)
				{
					values[i][j] /= val;
				}
			}
			else
			{
				for (int j = 0; j< numBars ; j++)
				{
					values[i][j] = 0.0;
				}
			}
		}
		normalized = true;
	}

	HDC hdc = _graphics->GetHDC();
	_dc = CDC::FromHandle(hdc);
	_graphics->ReleaseHDC(hdc);

	long transp = _options->transparency ;
	long alpha = long(transp) << 24;
	Gdiplus::Pen pen(Gdiplus::Color(alpha|BGR_TO_RGB(_options->lineColor)));
	std::vector<Gdiplus::Brush*> brushes;
	std::vector<Gdiplus::Brush*> brushesDimmed;

	for (long i = 0; i < numBars; i++)
	{
		IChartField* chartField = NULL;
		charts->get_Field(i, &chartField);
		
		OLE_COLOR color;
		chartField->get_Color(&color);
		chartField->Release();

		Gdiplus::Brush* br = new Gdiplus::SolidBrush(Gdiplus::Color(alpha|BGR_TO_RGB(color)));
		brushes.push_back(br);
		
		short r = GetRValue(color) - 100;	if (r< 0) r = 0;
		short g = GetGValue(color) - 100;	if (g< 0) g = 0;
		short b = GetBValue(color) - 100;	if (b< 0) b = 0;

		Gdiplus::Brush* brDimmed = new Gdiplus::SolidBrush(Gdiplus::Color(alpha|BGR_TO_RGB(RGB(r,g,b))));
		brushesDimmed.push_back(brDimmed);
	}
	
	// ------------------------------------------------------
	// initializing the font
	// ------------------------------------------------------
	CFont* oldFont = NULL;
	CFont fnt;
	CBrush brushFrame(_options->valuesFrameColor);
	CPen penFrame(PS_SOLID, 1, RGB(150, 150, 150));
	
	bool vertical = (_options->valuesStyle == vsVertical);
	CString sFormat = normalized?"%.2f":"%g";	// format for numbers

	if (_options->valuesVisible)
	{
		LOGFONT lf;	
		CString s(_options->valuesFontName);

		fnt.CreatePointFont(_options->valuesFontSize * 10, s);
		fnt.GetLogFont(&lf);

		if (vertical && _options->chartType != chtPieChart)
		{
			lf.lfEscapement = 900;
		}
		
		_dc->SetTextColor(_options->valuesFontColor);

		lf.lfItalic = (BYTE)_options->valuesFontItalic;

		if (_options->valuesFontBold) 
			lf.lfWeight = FW_BOLD;
		else			
			lf.lfWeight = 0;
		
		fnt.DeleteObject();
		fnt.CreateFontIndirectA(&lf);
		oldFont = _dc->SelectObject(&fnt);
	}

	// -------------------------------------------------------------- 
	//   Drawing of pie charts
	// --------------------------------------------------------------
	if (_options->chartType == chtPieChart)
	{
		Gdiplus::REAL pieThickness = (Gdiplus::REAL)(_options->thickness * _options->tilt/90.0f);
		Gdiplus::REAL pieHeight;

		if (_options->use3Dmode)
		{
			pieHeight = (Gdiplus::REAL)(_options->radius * (1.0f - _options->tilt/90.0f) * 2.0f);
		}
		else
		{
			pieHeight = (Gdiplus::REAL)(_options->radius * 2.0f);
		}

		Gdiplus::REAL pieWidth = (Gdiplus::REAL)_options->radius * 2.0f;
		Gdiplus::REAL offsetX = (Gdiplus::REAL)_offsetX;
		Gdiplus::REAL offsetY = (Gdiplus::REAL)_offsetY;
		
		// size field
		bool useSizeField = false;
		std::vector<double> sizeValues;		
		double maxValue = DBL_MIN;
		double minValue = DBL_MAX;

		if ( _options->useVariableRadius )
		{
			if (sfClass->ReadChartField(&sizeValues, _options->sizeField))
			{
				for (unsigned int i = 0; i< sizeValues.size(); i++)
				{
					if (maxValue < sizeValues[i]) maxValue = sizeValues[i];
					if (minValue > sizeValues[i]) minValue = sizeValues[i];
				}
				useSizeField = true;
			}
			
			// in case size field wasn't specified we'll determine size by sum of values
			if (_options->sizeField == -1 || !useSizeField)
			{
				for (unsigned int i = 0; i< values.size(); i++)
				{
					double sum = 0;
					for (int j = 0; j < numBars; j++)
						sum += values[i][j];
					
					if (maxValue < sum) maxValue = sum;
					if (minValue > sum) minValue = sum;
				}
			}
		}
			
		for (size_t k = 0; k < arr.size(); k++)
		{
			int i = arr[k];		// extracting index from array of visible shapes
			
			double x, y;
			ShapeData* pnt = (*positions)[i];
			
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
			if ( _options->useVariableRadius )
			{
				Gdiplus::REAL ratio;
				if (_options->sizeField == -1 || !useSizeField)
					ratio = (Gdiplus::REAL)((sum - minValue)/ (maxValue - minValue ));
				else
					ratio = (Gdiplus::REAL)((sizeValues[i] - minValue)/ (maxValue - minValue ));
				
				if (_options->use3Dmode)
					pieHeight = (Gdiplus::REAL)((_options->radius + (_options->radius2 - _options->radius) * ratio)  * (1.0f - _options->tilt/90.0f) * 2.0f);
				else
					pieHeight = (Gdiplus::REAL)(2.0*(_options->radius + (_options->radius2 - _options->radius) * ratio));

				pieWidth = (Gdiplus::REAL)(2.0 * (_options->radius + (_options->radius2 - _options->radius) * ratio));
				offsetX = - pieWidth/2.0f;
				offsetY = - pieHeight/2.0f;
			}

			Gdiplus::REAL xStart = Gdiplus::REAL(int(x + offsetX));
			Gdiplus::REAL yStart = Gdiplus::REAL(int(y + offsetY));
			
			CRect* chartRect = NULL;
			chartRect = new CRect((int)(xStart - pieWidth/2), (int)(yStart - pieHeight/2), int(xStart + pieWidth/2), int(yStart + pieHeight/2));

			// collision avoidance
			if ( _options->avoidCollisions && _collisionList != NULL)
			{
				if (_collisionList->HaveCollision(*chartRect))
				{
					delete chartRect;
					continue;
				}
			}
			
			Gdiplus::REAL startAngle = 0.0, sweepAngle = 0.0;
			Gdiplus::GraphicsPath path, pathTemp;

			// -----------------------------------------------------
			//   testing if we can draw labels
			// -----------------------------------------------------
			std::vector<ValueRectangle> labels;
			if (_options->valuesVisible)
			{
				startAngle = 0.0;
				
				for (int j = 0; j < numBars; j++)
				{
					sweepAngle = (Gdiplus::REAL)(values[i][j]/sum * 360.0);

					// label drawing
					ValueRectangle value;
					CRect* rect = &value.rect;

					value.string = Utility::FormatNumber(values[i][j], sFormat);
					
					HDC hdc = _graphics->GetHDC();
					_dc->DrawText(value.string, rect, DT_CALCRECT);	// add alignment
					_graphics->ReleaseHDC(hdc);
					
					Gdiplus::REAL labelAngle = startAngle + sweepAngle/2.0f + 90.0f;
					if (labelAngle > 360.0f)
					{
						labelAngle = labelAngle - 360.0f;
					}
					
					int x = (int)(xStart + sin(labelAngle/180.0 * pi_) * _options->radius);
					int y = (int)(yStart - cos(labelAngle/180.0 * pi_) * _options->radius);
									
					if (labelAngle >= 0.0 && labelAngle <= 180.0)
					{
						x += rect->Width()/2;
					}
					else
					{
						x -= rect->Width()/2;
					}

					if (labelAngle >= 90.0 && labelAngle <= 270.0 )
					{
						y += rect->Height()/2;
					}
					else
					{
						y -= rect->Height()/2;
					}
					
					startAngle += sweepAngle;
					
					rect->MoveToX(x - rect->Width()/2);
					rect->MoveToY(y - rect->Height()/2);
					
					if ( _options->avoidCollisions && _collisionList != NULL)
					{
						if (_collisionList->HaveCollision(*rect))
						{
							continue;
						}
						else
						{
							labels.push_back(value);
						}
					}
					else
					{
						// saving the rect and text; we shall draw it when make sure that all the values don't have collisions
						labels.push_back(value);
					}
				}
				
				// some values will have collisions
				if (labels.size() != numBars)
				{
					if (chartRect)
					{
						delete chartRect;
						chartRect = NULL;
					}
					continue;
				}
			}
			
			// ---------------------------------------
			// drawing the pie
			// ---------------------------------------
			startAngle = 0.0, sweepAngle = 0.0;
			xStart -= pieWidth/2.0f;
			yStart -= pieHeight/2.0f;
			for (int j = 0; j < numBars; j++)
			{
				sweepAngle = (Gdiplus::REAL)(values[i][j]/sum * 360.0);
				if (sweepAngle > 0.0)
				{
					_graphics->FillPie(brushes[j] , xStart, yStart, pieWidth, pieHeight, startAngle, sweepAngle);
					path.AddPie(xStart, yStart, pieWidth, (Gdiplus::REAL)pieHeight, startAngle, sweepAngle);
					
					// 3D mode
					if ( startAngle < 180.0 && _options->use3Dmode)
					{
						Gdiplus::GraphicsPath pathBottom;
						if (startAngle + sweepAngle > 180.0)
							pathBottom.AddArc(xStart, yStart + pieThickness, pieWidth, pieHeight, startAngle, 180.0f - startAngle);
						else
							pathBottom.AddArc(xStart, yStart + pieThickness, pieWidth, pieHeight, startAngle, sweepAngle);
						
						Gdiplus::PathData pathData;
						pathBottom.GetPathData(&pathData);
						Gdiplus::PointF* pntStart, *pntEnd;
						if (pathData.Count > 0)
						{
							pntStart = &(pathData.Points[0]);
							pntEnd = &(pathData.Points[pathData.Count - 1]);
						}
						pathBottom.AddLine(pntEnd->X, pntEnd->Y - (Gdiplus::REAL)pieThickness, pntEnd->X, pntEnd->Y);
	
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
			if (_options->valuesVisible)
			{
				HDC hdc = _graphics->GetHDC();

				for (unsigned int i = 0; i < labels.size(); i++)
				{
					CRect* rect = &labels[i].rect;
					_collisionList->AddRectangle(rect, _collisionBuffer, _collisionBuffer);
						
					// drawing frame							
					if (_options->valuesFrameVisible)
					{
						CBrush* oldBrush = _dc->SelectObject(&brushFrame);
						CPen* oldPen = _dc->SelectObject(&penFrame);
						
						CRect r(rect->left -3, rect->top, rect->right +2, rect->bottom);
						_dc->Rectangle(r);
						
						_dc->SelectObject(oldBrush);
						_dc->SelectObject(oldPen);
					}

					_dc->DrawText(labels[i].string, rect, DT_CENTER | DT_VCENTER);
				}

				_graphics->ReleaseHDC(hdc);
			}
			
			// storing rectangle for drawing operations
			ShapeData* info = (*positions)[i];
			if (info)
			{
				info->chart->frame = chartRect;
				info->chart->isDrawn = true;
			}
			
			// storing rectangle for collision avoidance
			if ( _options->avoidCollisions && _collisionList != NULL)
			{
				_collisionList->AddRectangle(chartRect, _collisionBuffer, _collisionBuffer);
			}
		}
	}

	// -------------------------------------------------------------- 
	//   Drawing of bar charts
	// --------------------------------------------------------------
	else // if ( _chartType == chtBarChart )
	{
		Gdiplus::PointF* points = new Gdiplus::PointF[5];

		double maxValue = DBL_MIN;
		for (unsigned int i = 0; i< values.size(); i++)
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
			ShapeData* pnt = (*positions)[i];

			// within extents?
			if (pnt->chart->x < _extents->left || pnt->chart->x >_extents->right || pnt->chart->y > _extents->top || pnt->chart->y < _extents->bottom)
				continue;

			ProjectionToPixel(pnt->chart->x, pnt->chart->y, x, y);
			
			x += (double)_offsetX;
			y += (double)_offsetY;

			// calculating max height
			double max=0.0;
			for (int j = 0; j < numBars; j++)
			{
				if (values[i][j] > max)
					max = values[i][j];
			}
			double maxHeight = _options->barHeight / maxValue * max;

			int xStart = int(x - numBars * _options->barWidth/2.0);
			int yStart = int(y + maxHeight/2.0);
			
			double angle = 45.0;
			
			// checking whether the bars are visible
			bool canDraw = false;
			for (int j = 0; j < numBars; j++)
			{
				int height = int((double)_options->barHeight/maxValue * values[i][j]);
				if (height > 0)
					canDraw = true;
			}

			if (!canDraw)
			{
				continue;
			}

			// collision avoidance
			CRect* rectChart = NULL;
			if ( _options->avoidCollisions && _collisionList != NULL)
			{
				rectChart = new CRect((int)xStart, int(yStart - _options->barHeight), int(xStart + _options->barWidth * numBars), (int)yStart);
				if (_options->use3Dmode)
				{
					rectChart->right += (long)(sin(pi_/4.0) * _options->thickness);
					rectChart->top -= (long)(cos(pi_/4.0) * _options->thickness);
				}

				if (_collisionList->HaveCollision(*rectChart))
				{
					delete rectChart;
					continue;
				}
			}
			
			std::vector<ValueRectangle> labels;
			if (_options->valuesVisible)
			{
				HDC hdc = _graphics->GetHDC();
				
				int xAdd = (int)(sin(45.0/180*pi_) * _options->thickness);
				
				// drawing values
				xStart = int(x - numBars * _options->barWidth/2.0);
				yStart = int(y + maxHeight/2.0);

				for (int j = 0; j < numBars; j++)
				{
					int height = int((double)_options->barHeight/maxValue * values[i][j]);
					if ( height != 0 )
					{
						CString s = Utility::FormatNumber(values[i][j], sFormat);
						
						CRect* rect = new CRect();
						_dc->DrawText(s, rect, DT_CALCRECT);

						if (!vertical)
						{
							rect->MoveToY(yStart - rect->Height() - height - 3);

							if (j == 0 )
							{
								rect->MoveToX(xStart + _options->barWidth/2 - rect->Width());
							}
							else if (j == numBars - 1)
							{
								rect->MoveToX(xStart + _options->barWidth/2);
							}
							else
							{
								rect->MoveToX(xStart + _options->barWidth/2 - rect->Width()/2);
							}
						}
						else
						{
							int wd = rect->Width();
							int ht = rect->Height();

							rect->bottom = 0;
							rect->left = 0;
							rect->right = ht;
							rect->top = - wd;
							
							int xShift, yShift;
							xShift = xStart + _options->barWidth/2 - rect->Width()/2 + xAdd/2;
							yShift = yStart - rect->Height() - height - 6;
							
							rect->MoveToXY(xShift, yShift);
						}
						
						canDraw = true;
						if ( _options->avoidCollisions && _collisionList != NULL)
						{
							if (_collisionList->HaveCollision(*rect))
							{
								canDraw = false;
								delete rect;
							}
						}
						
						if (canDraw)
						{
							// we shall store the label, to keep the collision list clean
							ValueRectangle value;
							value.string = s;
							
							// drawing frame							
							if (!vertical)
							{
								CRect r(rect->left-2, rect->top, rect->right + 2, rect->bottom);
								value.rect = r;
							}
							else
							{
								CRect r(rect->left, rect->top - 2, rect->right, rect->bottom + 2);
								value.rect = r;
							}
							labels.push_back(value);
						}
					}
					xStart += _options->barWidth;
				}
				_graphics->ReleaseHDC(hdc);
				
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
			xStart = int(x - numBars * _options->barWidth/2.0);
			for (int j = 0; j < numBars; j++)
			{
				int height = int((double)_options->barHeight/maxValue * values[i][j]);
				if ( height != 0 )
				{
					// drawing bars
					_graphics->FillRectangle(brushes[j], xStart, yStart - height, _options->barWidth, height);
					_graphics->DrawRectangle(&pen, xStart, yStart - height, _options->barWidth, height);

					// 3D mode
					if ( _options->use3Dmode )
					{
						points[0].X = (Gdiplus::REAL)xStart;
						points[1].X = (Gdiplus::REAL)(xStart + sin(angle/180*pi_) * _options->thickness);
						points[2].X = (Gdiplus::REAL)points[1].X + _options->barWidth;
						points[3].X = (Gdiplus::REAL)xStart + _options->barWidth;
						points[4].X = (Gdiplus::REAL)xStart;

						points[0].Y = (Gdiplus::REAL)yStart - height;
						points[1].Y = (Gdiplus::REAL)(yStart - height - cos(angle/180*pi_) *  _options->thickness);
						points[2].Y = (Gdiplus::REAL)points[1].Y;
						points[3].Y = (Gdiplus::REAL)yStart - height;
						points[4].Y = (Gdiplus::REAL)yStart - height;

						_graphics->DrawPolygon(&pen, points, 5);
						_graphics->FillPolygon(brushesDimmed[j], points, 5);

						points[0].X = (Gdiplus::REAL)xStart + _options->barWidth;
						points[1].X = (Gdiplus::REAL)(points[0].X  + sin(angle/180*pi_) *  _options->thickness);
						points[2].X = (Gdiplus::REAL)points[1].X;
						points[3].X = (Gdiplus::REAL)points[0].X;
						points[4].X = (Gdiplus::REAL)points[0].X;

						points[0].Y = (Gdiplus::REAL)yStart - height;
						points[1].Y = (Gdiplus::REAL)(points[0].Y - cos(angle/180*pi_) *  _options->thickness);
						points[2].Y = (Gdiplus::REAL)points[1].Y + height;
						points[3].Y = (Gdiplus::REAL)points[0].Y + height;
						points[4].Y = (Gdiplus::REAL)points[0].Y;

						_graphics->DrawPolygon(&pen, points, 5);
						_graphics->FillPolygon(brushesDimmed[j], points, 5);
					}
				}
				xStart += _options->barWidth;
			}

			HDC hdc = _graphics->GetHDC();

			// drawing the labels, all of them are visible
			for (unsigned int j = 0; j < labels.size(); j++)
			{
				CRect* rect = &labels[j].rect;
				if (_options->valuesFrameVisible)
				{
					CBrush* oldBrush = _dc->SelectObject(&brushFrame);
					CPen* oldPen = _dc->SelectObject(&penFrame);

					_dc->Rectangle(rect);
					
					_dc->SelectObject(oldBrush);
					_dc->SelectObject(oldPen);
				}

				if (!vertical)
				{
					_dc->DrawText(labels[j].string, rect, DT_CENTER|DT_VCENTER);
				}
				else
				{
					_dc->TextOutA(rect->left, rect->bottom - 3, labels[j].string);
				}
				if ( _options->avoidCollisions && _collisionList != NULL)
				{
					_collisionList->AddRectangle(rect, 0, 0);
				}
			}

			_graphics->ReleaseHDC(hdc);
			
			// adding chart rect to collision list
			if ( _options->avoidCollisions && _collisionList != NULL)
			{
				_collisionList->AddRectangle(rectChart, _collisionBuffer, _collisionBuffer);
				ShapeData* info = (*positions)[i];
				if (info)
				{
					info->chart->frame = rectChart;
					info->chart->isDrawn = true;
				}
			}
		}
		// cleaning
		if( points )
		{
			delete[] points;
		}
	}
	
	if (_options->valuesVisible)
	{
		_dc->SelectObject(oldFont);
		fnt.DeleteObject();
	}

	brushFrame.DeleteObject();

	//cleaning
	for (unsigned int i = 0; i < brushes.size(); i++ )
		delete brushes[i];
	for (unsigned int i = 0; i < brushesDimmed.size(); i++ )
		delete brushesDimmed[i];
	charts->Release();
}