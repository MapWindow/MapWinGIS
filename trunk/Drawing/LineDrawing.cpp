/**************************************************************************************
 * File name: LinePatternDrawing.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of the CLinePatternDrawing. 
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
 // lsu 13 jan 2011 - Created the file.

#include "stdafx.h"
#include "LineDrawing.h"

struct SEGMENT
{
	double angle;
	int length;
};

// ******************************************************************
//		DrawCustomLine()
// ******************************************************************
bool CLineDrawer::DrawLine(Gdiplus::GraphicsPath& path, ILinePattern* linePattern)
{
	return false;
	

	//CLinePattern* pattern = (CLinePattern*)linePattern;

	//int numLines;
	//pattern->get_NumLines(&numLines);
	//if (numLines == 0)
	//{
	//	return false;
	//}

	//pattern->InitializePens();

	//_dc->EndPath();	   // in GDIPlus mode no drawing occurs otherwise
	//_dc->SaveDC();		
	//_dc->SetGraphicsMode(GM_ADVANCED);

	//CPen* tempPen = new CPen(PS_SOLID, 1, RGB(0,0,0));
	//CPen* oldPen = _dc->SelectObject(tempPen);
	//
	//double minLength = 5;
	//int position = 0;	// position inside the pattern, the next segment must continue the same pattern form this position
	//					// and not from the begining

	//int patternLength;
	//pattern->get_PatternLength(&patternLength);

	//Gdiplus::PathData data; 
	//if (path.GetPathData(&data) == Gdiplus::Ok)
	//{
	//	if (data.Count >= 0)
	//	{
	//		// calculating the directional angles of all segments
	//		SEGMENT* segments = new SEGMENT[data.Count - 1];		// no angle for the last point
	//		for (int i = 0; i < data.Count - 1; i++)
	//		{
	//			if (data.Types[i + 1] != PathPointTypeStart)
	//			{
	//				double dx = data.Points[i].X - data.Points[i + 1].X;
	//				double dy = data.Points[i].Y - data.Points[i + 1].Y;
	//				
	//				segments[i].length = (int)(sqrt(pow(dx,2.0) + pow(dy,2.0)));
	//				segments[i].angle = GetPointAngle(dx, dy);
	//			}
	//			else
	//			{
	//				segments[i].length = 0;
	//			}
	//		}
	//		
	//		#pragma region SegmentsLoop

	//		for (int i = 0; i < data.Count - 1; i++)
	//		{
	//			if (data.Types[i + 1] == PathPointTypeStart)	// in case it's the last point we must process it as well
	//			{
	//				// the next point is the starting one, we must not draw line to it
	//				position = 0;
	//			}
	//			else
	//			{
	//				// ------------------------------------------------------
	//				// Setting world transformation for a current segment
	//				// ------------------------------------------------------
	//				/*double dx = data.Points[i].X - data.Points[i + 1].X;
	//				double dy = data.Points[i].Y - data.Points[i + 1].Y;
	//				
	//				int length = (int)(sqrt(pow(dx,2.0) + pow(dy,2.0)));
	//				if (length < minLength)
	//					continue;
	//				
	//				double angle = GetPointAngle(dx, dy);*/

	//				double angle = segments[i].angle;
	//				int length = segments[i].length;
	//				double angleCorrected = angle + pi/2.0;	// get point will return 0 for vertical line going up, 
	//														// we need to set 0 to horizontal line going left, therefore + 90 deg

	//				XFORM xForm;
	//				xForm.eM11 = (FLOAT)cos(-angleCorrected);
	//				xForm.eM12 = (FLOAT)sin(-angleCorrected);
	//				xForm.eM21 = (FLOAT)-sin(-angleCorrected);
	//				xForm.eM22 = (FLOAT)cos(-angleCorrected);
	//				xForm.eDx =	data.Points[i].X;
	//				xForm.eDy =	data.Points[i].Y;
	//				_dc->SetWorldTransform(&xForm);

	//				// -------------------------------------------------------
	//				//   Finishing pattern from the previous segment
	//				// -------------------------------------------------------
	//				//if (position != 0)
	//				//{
	//				//	if (position + length > patternLength)
	//				//	{
	//				//		// we finished the pattern, but segment isn't finished
	//				//		DrawPart(pattern, position, patternLength);
	//				//		int part = patternLength - position;	// part that was drawn

	//				//		// updating transformation
	//				//		xForm.eDx -= (FLOAT)(sin(angle) * (part));
	//				//		xForm.eDy -= (FLOAT)(cos(angle) * (part));
	//				//		_dc->SetWorldTransform(&xForm);
	//				//		
	//				//		length -= part;	// part that was drawn is excluded
	//				//	}
	//				//	else
	//				//	{
	//				//		int l = position + length;
	//				//		DrawPart(pattern, position, l);
	//				//		position += length;
	//				//		continue;	// the segment was drawn fullwise, nothing to do here futher
	//				//	}
	//				//}
	//				
	//				// ------------------------------------------------------------
	//				//	  Drawing the full patterns
	//				// ------------------------------------------------------------
	//				//if (length > patternLength)
	//				//{
	//					int count = length/patternLength;
	//					
	//					// we can draw this number of full patterns
	//					for (int j = 0; j < count; j++ )
	//					{
	//						for (int i = 0; i < numLines; i++)
	//						{
	//							CLineSegmentExt* segm = pattern->get_LineFast(i);
	//							CPen* oldPen = _dc->SelectObject(segm->pen);
	//							
	//							POINT pnt1;
	//							pnt1.x = segm->point1->x + j * patternLength;
	//							pnt1.y = segm->point1->y;
	//							POINT pnt2;
	//							pnt2.x = segm->point2->x + j * patternLength;
	//							pnt2.y = segm->point2->y;
	//							
	//							_dc->MoveTo(pnt1);
	//							_dc->LineTo(pnt2);

	//							_dc->SelectObject(oldPen);
	//						}
	//					}
	//					
	//					// the residue
	//					length -= (count * patternLength);	
	//					
	//					// updating transformation
	//					if (length > 0)
	//					{
	//						xForm.eDx -= FLOAT(sin(angle) * count * patternLength);
	//						xForm.eDy -= FLOAT(cos(angle) * count * patternLength);

	//						_dc->SetWorldTransform(&xForm);
	//					}
	//				//}
	//				
	//				// ---------------------------------------------------------------
	//				//	 Drawing of the incomplete pattern at the end of segment
	//				// ---------------------------------------------------------------
	//				/*if (length > 0)
	//				{
	//					int start = 0;
	//					DrawPart(pattern, start, length);
	//					position = length;
	//				}*/
	//			}
	//		}
	//		#pragma endregion
	//		
	//		delete[] segments;
	//	}
	//}
	//return true;
	//_dc->SelectObject(oldPen);
	//delete tempPen;
	//_dc->RestoreDC(-1);
}

// ******************************************************************
//		DrawPatternPart()
// ******************************************************************
// It's assumed here that the appropriate transformation have been applied to dc already
// x1, x2 - beginning and the end of pattern respectively
void CLineDrawer::DrawPart(CLinePattern* linePattern, int& beginX, int& endX)
{
	//int numLines;
	//linePattern->get_NumLines(&numLines);
	//
	//for (int i = 0; i < numLines; i++)
	//{
	//	CLineSegmentExt* segm = linePattern->get_LineFast(i);
	//	CPen* oldPen = _dc->SelectObject(segm->pen);	
	//	
	//	if (segm->point1->x >= endX || segm->point2->x <= beginX)
	//		continue;

	//	POINT pnt1;
	//	POINT pnt2;
	//	
	//	bool subtraction = true;

	//	// pattern is split by the beginning of segment
	//	if (segm->point1->x < beginX && segm->point2->x > beginX)
	//	{
	//		double ratio = double(beginX - segm->point1->x)/double(segm->point2->x - segm->point1->x);
	//		pnt1.x = beginX;
	//		pnt1.y = long(segm->point1->y + double(segm->point2->y - segm->point1->y) * ratio);
	//		subtraction = true;
	//	}
	//	else
	//	{
	//		pnt1.x = segm->point1->x;
	//		pnt1.y = segm->point1->y;
	//	}

	//	// pattern is split by the end of segment
	//	if (segm->point1->x < endX && segm->point2->x > endX)
	//	{
	//		double ratio = double(endX - segm->point1->x)/double(segm->point2->x - segm->point1->x);
	//		pnt2.x = endX;
	//		pnt2.y = long(segm->point1->y + double(segm->point2->y - segm->point1->y) * ratio);
	//	}
	//	else
	//	{
	//		pnt2.x = segm->point2->x;
	//		pnt2.y = segm->point2->y;
	//	}
	//	
	//	if (beginX > 0)
	//	{
	//		pnt1.x -= beginX;
	//		pnt2.x -= beginX;
	//	}

	//	_dc->MoveTo(pnt1);
	//	_dc->LineTo(pnt2);
	//}
}