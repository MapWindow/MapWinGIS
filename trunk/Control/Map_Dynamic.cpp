#include "stdafx.h"
#include "map.h"
#include "Utils.h"
#include "measuring.h"
#include "GeometryHelper.h"
#include "ShapeEditor.h"
#include "ShapefileDrawing.h"

// ***************************************************************
//		DrawMouseMoves()
// ***************************************************************
void CMapView::DrawDynamic(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, bool drawBackBuffer /*= false*/, float offsetX /*= 0.0f*/, float offsetY /*= 0.0f*/)
{
	HDC hdc = pdc->GetSafeHdc();
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromHDC(hdc);
	g->TranslateTransform(offsetX, offsetY);

	Gdiplus::Graphics* gTemp = NULL;
	if (drawBackBuffer)
	{
		// creating a temp buffer, and passing content of main buffer to it;
		// so that all the resulting stuff can passed to screen in one take, without flickering
		gTemp = Gdiplus::Graphics::FromImage(_drawingBitmap);
		gTemp->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
		gTemp->DrawImage(_bufferBitmap, 0.0f, 0.0f);
	}
	else
	{
		// main buffer was already drawn, so we don't care about flickering (snapshot operation),
		// so can draw on the output canvas directly
		gTemp = g;
	}
	
	if (HasDrawingData(MeasuringData))
	{
		GetMeasuringBase()->DrawData(gTemp, true, DragNone);
	}

	if (HasDrawingData(ShapeEditing))
	{
		DrawShapeEditor(gTemp, true);
	}

	if (HasDrawingData(MovingShapes))
	{
		DrawMovingShapes(gTemp, rcBounds, false);
	}

	DrawCoordinates(gTemp);
	
	DrawZoombox(gTemp);

	if (drawBackBuffer)
	{
		// it's not snapshot so we are using temp buffer to avoid flickering;
		// let's pass it to the screen here;
		g->DrawImage(_drawingBitmap, 0.0f, 0.0f);
		delete gTemp;
	}

	g->ReleaseHDC(pdc->GetSafeHdc());
	delete g;
}

// ***************************************************************
//	DrawMovingShapes
// ***************************************************************
void CMapView::DrawMovingShapes(Gdiplus::Graphics* g, const CRect& rect, bool dynamicBuffer)
{
	if (!_moveBitmap)
	{
		_moveBitmap = new Gdiplus::Bitmap(rect.Width(), rect.Height());
		Gdiplus::Graphics* gBuffer = Gdiplus::Graphics::FromImage(_moveBitmap);
		gBuffer->Clear(Gdiplus::Color::Transparent);
		CCollisionList list;	
		CShapefileDrawer drawer(gBuffer, &_extents, _pixelPerProjectionX, _pixelPerProjectionY, &list, GetCurrentScale(), false);
		drawer.Draw(rect, _dragging.Shapefile);
	}
	
	g->DrawImage(_moveBitmap, _dragging.GetOffsetX(), _dragging.GetOffsetY());
}

// ***************************************************************
//	DrawShapeEditor
// ***************************************************************
void CMapView::DrawShapeEditor( Gdiplus::Graphics* g, bool dynamicBuffer )
{
	int offsetX = _dragging.Move.x - _dragging.Start.x;
	int offsetY = _dragging.Move.y - _dragging.Start.y;

	if (_dragging.Operation == DragMoveVertex) 
	{
		offsetX = 0;
		offsetY = 0;
	}
	_shapeEditor->Render(g, dynamicBuffer, _dragging.Operation, offsetX, offsetY);
}

// ****************************************************************
//		DrawZoomboxToScreenBuffer()
// ****************************************************************
void CMapView::DrawZoombox(Gdiplus::Graphics* g)
{
	bool drawZoombox = m_cursorMode == cmZoomIn && _leftButtonDown && _dragging.Operation == DragZoombox && _dragging.Start != _dragging.Move;
	if (drawZoombox)
	{
		CRect r = _dragging.GetRectangle();
		Gdiplus::Rect rect(r.left, r.top, r.right - r.left, r.bottom - r.top);
		
		g->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);

		switch(_zoomBoxStyle)
		{
			case zbsRubberBand:
				{
					Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 1.0f);
					pen.SetDashStyle(Gdiplus::DashStyle::DashStyleDot);
					g->DrawRectangle(&pen, rect);
				}
				break;
			case zbsGray:
				{
					Gdiplus::SolidBrush brush(Gdiplus::Color(100, 192, 192, 192));
					Gdiplus::Pen pen(Gdiplus::Color(255, 192, 192, 192));
					g->FillRectangle(&brush, rect);
					g->DrawRectangle(&pen, rect);
				}
				break;
			case zbsOrange:
				{
					Gdiplus::SolidBrush brush(Gdiplus::Color(65, 255, 165, 0));
					Gdiplus::Pen pen(Gdiplus::Color(255, 255, 165, 0));
					g->FillRectangle(&brush, rect);
					g->DrawRectangle(&pen, rect);
				}
				break;
			case zbsBlue:
				{
					Gdiplus::SolidBrush brush(Gdiplus::Color(100, 173, 216, 230));
					Gdiplus::Pen pen(Gdiplus::Color(255, 30, 144, 255));
					g->FillRectangle(&brush, rect);
					g->DrawRectangle(&pen, rect);
				}
				break;
			case zbsGrayInverted:
				{
					Gdiplus::Rect r1(r.left, 0, r.Width(), r.top);
					Gdiplus::Rect r2(0,0, r.left, _viewHeight);
					Gdiplus::Rect r3(r.left, r.bottom, r.Width(), _viewHeight - r.bottom);
					Gdiplus::Rect r4(r.right, 0, _viewWidth - r.right, _viewHeight);
					Gdiplus::SolidBrush brush(Gdiplus::Color(100, 192, 192, 192));
					
					g->FillRectangle(&brush, r1);
					g->FillRectangle(&brush, r2);
					g->FillRectangle(&brush, r3);
					g->FillRectangle(&brush, r4);

					Gdiplus::Pen pen(Gdiplus::Color(255, 192, 192, 192));
					g->DrawRectangle(&pen, rect);
				}
				break;
		}

		Gdiplus::PixelOffsetMode pixelOffsetMode = g->GetPixelOffsetMode();
		g->SetPixelOffsetMode(pixelOffsetMode);
	}
}

// ****************************************************************
//		DrawCoordinatesToScreenBuffer()
// ****************************************************************
void CMapView::DrawCoordinates(Gdiplus::Graphics* g) 
{
	if(HasDrawingData(tkDrawingDataAvailable::Coordinates)) 
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(&p);
			double x = p.x, y = p.y;
			double prX, prY;
			PixelToProjection(x, y, prX, prY);

			bool canUseDegrees = _transformationMode == tmWgs84Complied;
			if (_transformationMode == tmDoTransformation && (_showCoordinates == cdmDegrees || _showCoordinates == cdmAuto))
			{
				IGeoProjection* p = GetMapToWgs84Transform();
				if (p) {
					VARIANT_BOOL vb;
					p->Transform(&prX, &prY, &vb);
					if (vb) canUseDegrees = true;
				}
			}

			if (_showCoordinates == cdmDegrees && !canUseDegrees) {
				// can't display degrees
			}
			else {
				CStringW s;
				if (canUseDegrees) {
					s.Format(L"%s: %.3f; %s: %.3f", m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsLatitude), prY, 
													m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsLongitude), prX);
				}
				else
				{
					s.Format(L"x=%.2f; y=%.2f", prX, prY);
				}
				Gdiplus::PointF point(0.0f, 0.0f);
				Gdiplus::RectF rect;
				
				g->MeasureString(s, s.GetLength(), _fontCourier, point, Gdiplus::StringFormat::GenericDefault(), &rect);
				if (rect.Width + 15 < _viewWidth)		// control must be big enough to host the string
				{
					point.X = _viewWidth - rect.Width - 7.0f;
					point.Y = 7.0f;
					DrawStringWithShade(g, s, _fontCourier, point, &GetMeasuringBase()->_textBrush, &GetMeasuringBase()->_whiteBrush);
				}
			}
		}
	}
}