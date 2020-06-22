/**************************************************************************************
 * File name: LineSegment.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CLineSegment
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
 // Sergei Leschinski (lsu) 18 august 2010 - created the file.
 // Paul Meems sept. 2019 - MWGIS-183: Merge .NET and VB drawing functions

#include "stdafx.h"
#include "LineSegment.h"
#include "GdiPlus.h"
#include "DrawingOptions.h"
#include "ShapeDrawingOptions.h"
#include "macros.h"
#include "PointSymbols.h"

// *************************************************************
//		get_Color()
// *************************************************************
STDMETHODIMP CLineSegment::get_Color(OLE_COLOR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _lineColor;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_Color(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_lineColor = newVal;
	return S_OK;
}

// *************************************************************
//		get_MarkerOutlineColor()
// *************************************************************
STDMETHODIMP CLineSegment::get_MarkerOutlineColor(OLE_COLOR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _markerOutlineColor;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_MarkerOutlineColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_markerOutlineColor = newVal;
	return S_OK;
}

// *************************************************************
//		get_LineWidth()
// *************************************************************
STDMETHODIMP CLineSegment::get_LineWidth(float* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _lineWidth;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_LineWidth(float newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_lineWidth = newVal;
	return S_OK;
}

// *************************************************************
//		get_LineStyle()
// *************************************************************
STDMETHODIMP CLineSegment::get_LineStyle(tkDashStyle* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _lineStyle;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_LineStyle(tkDashStyle newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_lineStyle = newVal;
	return S_OK;
}

// *************************************************************
//		get_LineType()
// *************************************************************
STDMETHODIMP CLineSegment::get_LineType(tkLineType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _lineType;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_LineType(tkLineType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_lineType = newVal;
	return S_OK;
}

// *************************************************************
//		get_Marker()
// *************************************************************
STDMETHODIMP CLineSegment::get_Marker(tkDefaultPointSymbol* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _marker;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_Marker(tkDefaultPointSymbol newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_marker = newVal;
	return S_OK;
}

// *************************************************************
//		get_MarkerSize()
// *************************************************************
STDMETHODIMP CLineSegment::get_MarkerSize(float* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _markerSize;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_MarkerSize(float newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_markerSize = newVal;
	return S_OK;
}

// *************************************************************
//		get_MarkerInterval()
// *************************************************************
STDMETHODIMP CLineSegment::get_MarkerInterval(float* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _markerInterval;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_MarkerInterval(float newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_markerInterval = newVal;
	return S_OK;
}

// *************************************************************
//		get_MarkerIntervalIsRelative()
// *************************************************************
STDMETHODIMP CLineSegment::get_MarkerIntervalIsRelative(VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
        * retVal = _markerIntervalIsRelative;
    return S_OK;
}
STDMETHODIMP CLineSegment::put_MarkerIntervalIsRelative(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
        _markerIntervalIsRelative = newVal;
    return S_OK;
}

// *************************************************************
//		get_MarkerAllowOverflow()
// *************************************************************
STDMETHODIMP CLineSegment::get_MarkerAllowOverflow(VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
        * retVal = _markerAllowOverflow;
    return S_OK;
}
STDMETHODIMP CLineSegment::put_MarkerAllowOverflow(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
        _markerAllowOverflow = newVal;
    return S_OK;
}

// *************************************************************
//		get_MarkerOrientation()
// *************************************************************
STDMETHODIMP CLineSegment::get_MarkerOrientation(tkLineLabelOrientation* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _markerOrientation;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_MarkerOrientation(tkLineLabelOrientation newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_markerOrientation = newVal;
	return S_OK;
}

// *************************************************************
//		get_MarkerFlipFirst()
// *************************************************************
STDMETHODIMP CLineSegment::get_MarkerFlipFirst(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _markerFlipFirst;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_MarkerFlipFirst(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_markerFlipFirst = newVal;
	return S_OK;
}
	
// *************************************************************
//		get_MarkerOffset()
// *************************************************************
STDMETHODIMP CLineSegment::get_MarkerOffset(float* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _markerOffset;
	return S_OK;
}
STDMETHODIMP CLineSegment::put_MarkerOffset(float newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_markerOffset = newVal;
	return S_OK;
}

// *************************************************************
//		get_MarkerOffsetIsRelative()
// *************************************************************
STDMETHODIMP CLineSegment::get_MarkerOffsetIsRelative(VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
        * retVal = _markerOffsetIsRelative;
    return S_OK;
}
STDMETHODIMP CLineSegment::put_MarkerOffsetIsRelative(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
        _markerOffsetIsRelative = newVal;
    return S_OK;
}

// *************************************************************
//		Draw()
// *************************************************************
STDMETHODIMP CLineSegment::Draw (int hdc, float x, float y, int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha, VARIANT_BOOL* retVal)
{
	if (!hdc) {
		CallbackHelper::ErrorMsg("Invalid handle of device context.");
		return S_OK;
	}

	CDC* dc = CDC::FromHandle((HDC)hdc);
	*retVal = this->DrawCore(dc, x, y, clipWidth, clipHeight, backColor, backAlpha);
	return S_OK;
}

// *************************************************************
//		Draw()
// *************************************************************
//STDMETHODIMP CLineSegment::DrawVB (int hdc, float x, float y, int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha, VARIANT_BOOL* retVal)
//{
//	if (!hdc) {
//		CallbackHelper::ErrorMsg("Invalid handle of device context.");
//		return S_OK;
//	}
//
//	CDC* dc = CDC::FromHandle((HDC)hdc);
//	*retVal = this->DrawCore(dc, x, y, clipWidth, clipHeight, backColor, backAlpha);
//	return S_OK;
//}

// *************************************************************
//		DrawCore()
// *************************************************************
VARIANT_BOOL CLineSegment::DrawCore(CDC* dc, float x, float y, int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha)
{
	if (!dc) {
		CallbackHelper::ErrorMsg("Invalid handle of device context.");
		return S_OK;
	}

	Gdiplus::Bitmap bmp(clipWidth, clipHeight, PixelFormat32bppARGB);
	Gdiplus::Graphics g(&bmp);
	g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	Gdiplus::Color clr = Utility::OleColor2GdiPlus(backColor, backAlpha);  
	Gdiplus::SolidBrush brushBackground(clr);
	g.Clear(clr);

    VARIANT_BOOL vb;
    Draw(g, (BYTE) 255, clipWidth, clipHeight, (int) x, (int) y, &vb);

	Gdiplus::Graphics gResult(dc->GetSafeHdc());
	gResult.DrawImage(&bmp, x, y);

	return  VARIANT_TRUE;
}

// ***********************************************************
//	   Draw()
// ***********************************************************
STDMETHODIMP CLineSegment::Draw(Gdiplus::Graphics& g, BYTE transparency, int ImageWidth, int ImageHeight, int xOrigin, int yOrigin, VARIANT_BOOL* retVal)
{
    if (_lineType == lltSimple)
        DrawSimpleSegment(g, ImageWidth, ImageHeight, transparency);
    else
        DrawMarkerSegment(g, ImageWidth, ImageHeight, transparency);

	*retVal = VARIANT_TRUE;
	return S_OK;
}

void CLineSegment::DrawSimpleSegment(Gdiplus::Graphics& g, int ImageWidth, int ImageHeight, const BYTE& transparency)
{
    if (_lineWidth <= 0)
        return;       

    Gdiplus::PointF points[2];
    points[0].X = 0.0f;
    points[0].Y = (float)ImageHeight / 2.0f;
    points[1].X = (float)ImageWidth;
    points[1].Y = (float)ImageHeight / 2.0f;

    Gdiplus::GraphicsPath path;
    path.AddLines(&(points[0]), 2);

    Gdiplus::Pen* pen = new Gdiplus::Pen(Utility::OleColor2GdiPlus(_lineColor, transparency), _lineWidth);
    switch (_lineStyle)
    {
    case dsSolid:		pen->SetDashStyle(Gdiplus::DashStyleSolid);		break;
    case dsDash:		pen->SetDashStyle(Gdiplus::DashStyleDash);		break;
    case dsDot:			pen->SetDashStyle(Gdiplus::DashStyleDot);		break;
    case dsDashDotDot:	pen->SetDashStyle(Gdiplus::DashStyleDashDotDot); break;
    case dsDashDot:		pen->SetDashStyle(Gdiplus::DashStyleDashDot);	break;
    default:			pen->SetDashStyle(Gdiplus::DashStyleSolid);
    }

    g.DrawLines(pen, points, 2);
    delete pen;
}

void CLineSegment::DrawMarkerSegment(Gdiplus::Graphics& g, int ImageWidth, int ImageHeight, const BYTE& transparency)
{
    int numPoints = 0;
    float* points = get_SimplePointShape(_marker, _markerSize, &numPoints);

    if (numPoints <= 0)
        return;

    double availableWidth = ImageWidth - (_markerAllowOverflow ? 0 : _markerSize);
    float offset = _markerOffset * (_markerOffsetIsRelative ? (float)availableWidth : 1.0f);

    float interval = _markerInterval * (_markerIntervalIsRelative ? (float)availableWidth : 1.0f);;
    const int count = (interval == 0) ? 1 : (int)((availableWidth - offset) / interval + 1.0 + FLT_EPSILON);

    if (count == 0)
        return;

    if (!_markerAllowOverflow)
        offset += _markerSize * 0.5f;

    Gdiplus::SolidBrush* brush = new Gdiplus::SolidBrush(Utility::OleColor2GdiPlus(_lineColor, transparency));
    Gdiplus::Pen* pen = new Gdiplus::Pen(Utility::OleColor2GdiPlus(_markerOutlineColor, transparency));
    pen->SetAlignment(Gdiplus::PenAlignmentInset);

    g.TranslateTransform(offset, ImageHeight * 0.5f);
    for (int i = 0; i < count; i++)
    {
        if (_markerFlipFirst && i == 0)
            g.RotateTransform(180);
        g.FillPolygon(brush, (Gdiplus::PointF*)points, numPoints);
        g.DrawPolygon(pen, (Gdiplus::PointF*)points, numPoints);
        // If interval is 0, draw once and stop.
        if (_markerInterval == 0)
            break;
        if (_markerFlipFirst && i == 0)
            g.RotateTransform(180);
        g.TranslateTransform(interval, 0);
    }
    g.ResetTransform();
    delete[] points;
    delete brush;
    delete pen;
}
