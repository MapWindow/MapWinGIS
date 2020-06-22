/**************************************************************************************
 * File name: DrawingOptions.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CDrawingOptionsEx
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
#include "DrawingOptions.h"
#include "Image.h"
#include "GeometryHelper.h"
#include "PointSymbols.h"
#include "macros.h"
#include "ImageHelper.h"

#pragma region Operators
// ********************************************************
//	  Operator=
// ********************************************************
CDrawingOptionsEx& CDrawingOptionsEx::operator=(const CDrawingOptionsEx& opt)
{
	if (this == &opt)
		return *this;
	
	this->drawingMode = opt.drawingMode;
	
	this->fillBgColor = opt.fillBgColor;
	this->fillBgTransparent = opt.fillBgTransparent;
	this->fillColor = opt.fillColor;
	this->fillColor2 = opt.fillColor2;
	this->fillTransparency = opt.fillTransparency;
	this->fillType = opt.fillType;
	this->fillVisible = opt.fillVisible;
	this->fillGradientBounds = opt.fillGradientBounds;
	this->fillGradientRotation = opt.fillGradientRotation;
	this->fillGradientType = opt.fillGradientType;
	this->fillTransparency = opt.fillTransparency;
	this->fillHatchStyle = opt.fillHatchStyle;

	::SysFreeString(this->rotationExpression);
	this->rotationExpression = OLE2BSTR(opt.rotationExpression);
	
	this->fontName = opt.fontName;
    this->rotationField = opt.rotationField;

	this->lineColor = opt.lineColor;
	this->lineStipple = opt.lineStipple;
	this->linesVisible = opt.linesVisible;
	this->lineWidth = opt.lineWidth;
	this->lineTransparency = opt.lineTransparency;

	this->pointCharcter = opt.pointCharcter;
	this->pointNumSides = opt.pointNumSides;
	this->pointShapeRatio = opt.pointShapeRatio;
	this->pointShapeType = opt.pointShapeType;
	this->pointSize = opt.pointSize;
	this->pointSymbolType = opt.pointSymbolType;
	
	this->rotation = opt.rotation;
	this->scaleX = opt.scaleX;
	this->scaleY = opt.scaleY;

	this->visible = opt.visible;
	
	this->scale = opt.scale;
	this->minScale = opt.minScale;
	this->maxScale = opt.maxScale;
	this->minLineWidth = opt.minLineWidth;
	this->maxLineWidth = opt.maxLineWidth;

	this->picture = NULL;
	this->bitmapPlus = NULL;
	this->imgAttributes = NULL;

	this->verticesColor = opt.verticesColor;
	this->verticesFillVisible = opt.verticesFillVisible;
	this->verticesSize = opt.verticesSize;
	this->verticesType = opt.verticesType;
	this->verticesVisible = opt.verticesVisible;

	this->useLinePattern = opt.useLinePattern;
	this->alignIconByBottom = opt.alignIconByBottom;
	this->drawFrame = opt.drawFrame;

	this->dynamicVisibility = opt.dynamicVisibility;	
	this->minVisibleScale = opt.minVisibleScale;
	this->maxVisibleScale = opt.maxVisibleScale;
    this->minVisibleZoom = opt.minVisibleZoom;
    this->maxVisibleZoom = opt.maxVisibleZoom;

	brushPlus = NULL;
	if(pen) delete pen;
	pen = new CPen();
	if (brush) delete brush;
	brush = new CBrush();
	penOld = NULL;
	brushOld = NULL;
	
	return *this;
}
#pragma endregion

#pragma region Visibility check
// *********************************************************
//		IsVisible()
// *********************************************************
bool CDrawingOptionsEx::IsVisible(double scale, int zoom)
{
    if (dynamicVisibility)
    {
        return (
            scale >= minVisibleScale && scale <= maxVisibleScale &&
            zoom >= minVisibleZoom && zoom <= maxVisibleZoom
            );
    }
    else
        return true;
}
#pragma endregion

#pragma region GDI
// *********************************************************
//		InitializeGDIBrushAndPen()
// *********************************************************
void CDrawingOptionsEx::InitGdiBrushAndPen(CDC* dc)
{
	this->InitGdiBrushAndPen(dc, false, RGB(255, 255, 255));
}
void CDrawingOptionsEx::InitGdiBrushAndPen(CDC* dc, bool selection, OLE_COLOR selectionColor)
{
	ReleaseGdiBrushAndPen(dc);
	
	// creating pen
	InitGdiPen(selection, selectionColor);
	penOld = dc->SelectObject(pen);

	// creating brush
	InitGdiBrush(selection, selectionColor);
	brushOld = dc->SelectObject(brush);
}

// ***************************************************************
// 		CreateCustomBrush()
// ***************************************************************
//  Makes brush for drawing polygon fill, stipples and bit patterns
void CDrawingOptionsEx::InitGdiBrush()
{
	this->InitGdiBrush(false, RGB(255, 255, 0));
}

void CDrawingOptionsEx::InitGdiBrush(bool selection, OLE_COLOR selectionColor)
{
	if ( this->drawingMode != vdmGDI || this->fillVisible == false)
	{
		brush->CreateStockObject(HOLLOW_BRUSH);	
	}
	else
	{	
		OLE_COLOR color = selection ? selectionColor : this->fillColor;
		
		if (this->fillType == ftStandard || this->fillType == ftGradient)
		{
			brush->CreateSolidBrush(color);
		}
		else if (this->fillType == ftHatch)
		{
			if (int(this->fillHatchStyle) >= 0 && int(this->fillHatchStyle) <=5 )
				brush->CreateHatchBrush((int)this->fillHatchStyle, color);
			else
				brush->CreateSolidBrush(color);
		}
		else if (this->fillType == ftPicture)
		{
			// TODO: implement creation of pattern brushes in separate function
		}
	}
}

// *********************************************************
//		CreateGdiPen()
// *********************************************************
void CDrawingOptionsEx::InitGdiPen()
{
	this->InitGdiPen(false, RGB(255, 255, 0));
}

void CDrawingOptionsEx::InitGdiPen(bool selection, OLE_COLOR selectionColor)
{
	OLE_COLOR color = selection ? selectionColor : this->lineColor;
	
	int width = Utility::Rint((double)this->lineWidth);
	if (this->_shpType == shpPolyline && maxScale > 0.0 && minScale > 0.0)
	{
		if (this->scale <= this->minScale)
		{
			width = Utility::Rint(this->maxLineWidth);
		}
		else if (this->scale >= this->maxScale)
		{
			width = Utility::Rint(this->minLineWidth);
		}
		else
		{
			double ratio = (log10(this->maxScale) - log10(this->scale))/(log10(this->maxScale) - log10(this->minScale));
			width = Utility::Rint(this->minLineWidth + (this->maxLineWidth - this->minLineWidth) * ratio);
		}
	}

	if ((this->lineWidth == 0) || !(this->linesVisible))
	{
		pen->CreatePen(PS_NULL, width, color);
	}
	else
	{
		if( this->lineStipple == lsNone )
		{
			pen->CreatePen(PS_SOLID, width, color);
		}
		else
		{
			int penStyle = LineStippleToGDIPenStyle(this->lineStipple);
			if (this->lineWidth >1)
			{
				LOGBRUSH logBrush;
				logBrush.lbColor = color;
				logBrush.lbStyle = BS_SOLID;
				pen->CreatePen(PS_GEOMETRIC|penStyle, width, &logBrush, 0, 0);
			}
			else
			{
				pen->CreatePen(penStyle,width,color);
			}
		}
	}
}

// *********************************************************
//		LineStippleToGDIPenStyle()
// *********************************************************
int CDrawingOptionsEx::LineStippleToGDIPenStyle(tkDashStyle lineStipple)
{
	switch( lineStipple )
	{
		case( dsSolid ):		return PS_SOLID; 
		case( dsDot ):			return PS_DOT;
		case( dsDash ):			return PS_DASH;
		case( dsDashDot ):		return PS_DASHDOT;
		case( dsDashDotDot ):	return PS_DASHDOTDOT;
		default:				return PS_SOLID; 
	}
}

// *********************************************************
//		ReleaseGDIBrushAndPen()
// *********************************************************
void CDrawingOptionsEx::ReleaseGdiBrushAndPen(CDC* dc)
{
	if (this->penOld)
	{
		dc->SelectObject(this->penOld);
		this->pen->DeleteObject();
		this->penOld = NULL;
	}

	if (this->brushOld)
	{
		dc->SelectObject(this->brushOld);
		this->brush->DeleteObject();
		this->brushOld = NULL;
	}
}
#pragma endregion

#pragma region GdiPlus
// ***************************************************************
//	  InitGDIPlusPen()
// ***************************************************************
void CDrawingOptionsEx::InitGdiPlusPen()
{
	this->ReleaseGdiPlusPen();
	
	if(this->linesVisible && this->lineWidth != 0) 
	{
		penPlus = new Gdiplus::Pen(Utility::OleColor2GdiPlus(this->lineColor, (BYTE)this->lineTransparency), this->lineWidth);
		
		switch (this->lineStipple)
		{
			case dsSolid:		penPlus->SetDashStyle(Gdiplus::DashStyleSolid);		break;
			case dsDash:		penPlus->SetDashStyle(Gdiplus::DashStyleDash);			break;
			case dsDot:			penPlus->SetDashStyle(Gdiplus::DashStyleDot);			break;
			case dsDashDotDot:	penPlus->SetDashStyle(Gdiplus::DashStyleDashDotDot);	break;
			case dsDashDot:		penPlus->SetDashStyle(Gdiplus::DashStyleDashDot);		break;
			default:			penPlus->SetDashStyle(Gdiplus::DashStyleSolid);
		}
	}
}

// *********************************************************
//		InitGDIPlusBrush()
// *********************************************************
// Creates a brush (solid, hatch, gradient, texture).
void CDrawingOptionsEx::InitGdiPlusBrush( Gdiplus::RectF* bounds )
{
	using namespace Gdiplus;

	this->ReleaseGdiPlusBrush();

	long alpha = ((long)this->fillTransparency)<<24;
	
	if (this->fillType == ftStandard )
	{
		// standard fill
		brushPlus = new SolidBrush(Color(alpha | BGR_TO_RGB(this->fillColor)));
	}
	else if (this->fillType == ftHatch)
	{
		//  hatched fill
		if (this->fillHatchStyle == hsNone)
		{
			brushPlus = new SolidBrush(Color(alpha | BGR_TO_RGB(this->fillColor)));
		}
		else 
		{
			Gdiplus::Color clr;
			if ( this->fillBgTransparent )
			{
				clr =  Gdiplus::Color::Transparent;
			}
			else
			{
				clr = Color(alpha | BGR_TO_RGB(this->fillBgColor));
			}

			brushPlus = new HatchBrush((HatchStyle)this->fillHatchStyle, Color(alpha | BGR_TO_RGB(this->fillColor)), clr);
		}
	}
	else if (this->fillType == ftPicture )
	{
		// texture fill
		bool canDraw = true;
		
		if (this->picture == NULL) canDraw = false;
		if (this->scaleX == 0 || this->scaleY == 0) canDraw = false;
		
		long width, height;
		if ( this->picture )
		{
			this->picture->get_Height(&height);
			this->picture->get_Width(&width);
			if (this->scaleX * width * this->scaleY * height > 512 * 512) canDraw = false;	// lsu: don't see reasons to load larger images here
		}

		if ( !canDraw )
		{
			brushPlus = new SolidBrush(Color(alpha | BGR_TO_RGB(this->fillColor)));
		}
		else
		{
			Gdiplus::Bitmap* bmp = this->ImageToGdiPlusBitmap(this->picture);
			
			// swap with scaled bitmap
			if (this->scaleX != 1.0 || this->scaleY != 1.0)
			{
				Bitmap* newBmp = new Bitmap((int)(bmp->GetWidth() * this->scaleX), 
											(int)(bmp->GetHeight() * this->scaleY));
				Graphics* g = new Gdiplus::Graphics(newBmp);
				g->DrawImage(bmp, Rect(0, 0, newBmp->GetWidth(), newBmp->GetHeight()), 0, 0, bmp->GetWidth(), bmp->GetHeight(), Gdiplus::UnitPixel);
				delete bmp;
				bmp = newBmp;
			}
			
			VARIANT_BOOL useTransparency;
			OLE_COLOR transpColor, transpColor2;
			this->picture->get_UseTransparencyColor(&useTransparency);
			this->picture->get_TransparencyColor(&transpColor);
			this->picture->get_TransparencyColor2(&transpColor2);
			
			Gdiplus::ImageAttributes* imgAttr = ImageHelper::GetImageAttributes(this->fillTransparency / 255.0f, useTransparency ? true : false, transpColor, transpColor2);
			Gdiplus::Rect rect(0,0, bmp->GetWidth(), bmp->GetHeight());
			brushPlus = new TextureBrush(bmp, rect, imgAttr);
			((TextureBrush*)brushPlus)->RotateTransform(-(float)this->fillGradientRotation);
			((TextureBrush*)brushPlus)->SetWrapMode(Gdiplus::WrapModeTile);
			delete imgAttr;
			delete bmp;
		}
	}
	else if (this->fillType == ftGradient)
	{
		// gradient fill
		RectF* rotatedBounds = NULL;
		bool needsDeleting = false;
		
		double rot = this->fillGradientRotation;
		if (rot != 0.0)
		{
			double angleRad = (rot/180 * pi_);
			Gdiplus::REAL width = (Gdiplus::REAL)(abs(bounds->Width * cos(angleRad)) + abs(bounds->Height * sin(angleRad)));
			Gdiplus::REAL height = (Gdiplus::REAL)(abs(bounds->Width * sin(angleRad)) + abs(bounds->Height * cos(angleRad)));
			
			if (width < bounds->Width || height < abs(bounds->Height))
			{
				Gdiplus::REAL ratio = max(bounds->Width/width, abs(bounds->Height)/height);
				width *= ratio;
				height *= ratio;
			}

			Gdiplus::REAL x = bounds->X + bounds->Width /2 - width/2;
			Gdiplus::REAL y = bounds->Y + bounds->Height /2 - height/2;
			rotatedBounds = new RectF(x, y, width, height);
			needsDeleting = true;
		}
		else
		{
			rotatedBounds = bounds;
		}
		
		if (this->fillGradientType == gtLinear)
		{
			brushPlus = new LinearGradientBrush(*rotatedBounds, 
							Color(alpha | BGR_TO_RGB(this->fillColor)), 
							Color(alpha | BGR_TO_RGB(this->fillColor2)),
							(Gdiplus::REAL)rot);

			Color colors[2];
			colors[0] = Color(alpha | BGR_TO_RGB(this->fillColor));
			colors[1] = Color(alpha | BGR_TO_RGB(this->fillColor2));
			
			Gdiplus::REAL positions[2];
			positions[0] = 0.0;
			positions[1] = 1.0;

			((LinearGradientBrush*)brushPlus)->SetInterpolationColors(&colors[0], &positions[0], 2);
		}
		else if (this->fillGradientType == gtCircle)
		{
			GraphicsPath round; 
			
			PointF center = PointF(bounds->X + bounds->Width / 2, bounds->Y + bounds->Height / 2);
			float w = bounds->Width * sqrt(2.0f);
			float h = bounds->Height * sqrt(2.0f);
			float x = center.X - w/2.0f; 
			float y = center.Y - h/2.0f; 
			
			RectF ellipse = RectF(x, y, w, h);
			round.AddEllipse(ellipse);
			
			brushPlus = new PathGradientBrush(&round);

			Color colors[2];
			colors[0] = Color(alpha | BGR_TO_RGB(this->fillColor));
			colors[1] = Color(alpha | BGR_TO_RGB(this->fillColor2));
			
			Gdiplus::REAL positions[2];
			positions[0] = 0;
			positions[1] = 1;

			((PathGradientBrush*)brushPlus)->SetInterpolationColors(&colors[0], &positions[0], 2);
		}
		else if (this->fillGradientType == gtRectangular)
		{
			GraphicsPath rect;
			PointF points[5];
			PointF center = PointF(rotatedBounds->X + rotatedBounds->Width / 2, rotatedBounds->Y + rotatedBounds->Height / 2);
			
			double a = rotatedBounds->Width / 2;
			double b = rotatedBounds->Height / 2;
			double angle = rot;
			if (angle < 0) angle = 360.0 + angle;
			angle = (long)angle % 90;
			angle = 2 * (pi_ * angle / 180.0);
			
			double x = a * cos(angle);
			double y = -b - a * sin(angle);
			points[0] = PointF((float)x + center.X, (float)y + center.Y);
			
			x = a + b * sin(angle);
			y = b * cos(angle);
			points[1] = PointF((float)x + center.X, (float)y + center.Y);
			
			x = -a * cos(angle);
			y = b + a * sin(angle);
			points[2] = PointF((float)x + center.X, (float)y + center.Y);
			
			x = -a - b * sin(angle);
			y = -b * cos(angle);
			points[3] = PointF((float)x + center.X, (float)y + center.Y);
			points[4] = points[0];
			
			rect.AddPolygon(&points[0], 5);
			
			// drawing
			brushPlus = new PathGradientBrush(&rect);
			Color colors[2];
			colors[0] = Color(alpha | BGR_TO_RGB(this->fillColor));
			colors[1] = Color(alpha | BGR_TO_RGB(this->fillColor2));
			
			Gdiplus::REAL positions[2];
			positions[0] = 0;
			positions[1] = 1;

			((PathGradientBrush*)brushPlus)->SetInterpolationColors(&colors[0], &positions[0], 2);
		}
		else
		{
			brushPlus = new SolidBrush(Color(alpha | BGR_TO_RGB(this->fillColor)));
		}

		if (needsDeleting)
		{
			delete rotatedBounds;
		}
	}
}

// **************************************************************
//		ImageToGdiPlusBitmap()
// **************************************************************
Gdiplus::Bitmap* CDrawingOptionsEx::ImageToGdiPlusBitmap(IImage* img)
{
	unsigned char* data = ((CImageClass*)img)->get_ImageData();
	long width, height;
	img->get_Width(&width);
	img->get_Height(&height);
	
	int bitsPerPixel = 32;
	int bytesPerPixel = bitsPerPixel / 8;
	int pad = ImageHelper::GetRowBytePad(width, bitsPerPixel);

	BITMAPINFOHEADER bih;
	bih.biCompression=0;
	bih.biXPelsPerMeter=0;
	bih.biYPelsPerMeter=0;
	bih.biClrUsed=0;
	bih.biClrImportant=0;
	bih.biPlanes=1;
	bih.biSize=sizeof(BITMAPINFOHEADER);

	bih.biBitCount = bitsPerPixel;
	bih.biWidth= width;
	bih.biHeight= height;
	bih.biSizeImage = (width * bytesPerPixel + pad) * height;
	
	BITMAPINFO bif;
	bif.bmiHeader = bih;
	
	unsigned char* bitsNew;
	int nBytesInRow = bytesPerPixel + pad;

	if (pad == 0)
	{
		bitsNew = data;
	}
	else
	{
		// we can make number of image buffer pixels in row dividable by 4 them we don't need this condition
		bitsNew = new unsigned char[nBytesInRow * height];

		for (int i = 0; i < height; i++)		
		{
			memcpy(&bitsNew[i * nBytesInRow], &data[i * width * bytesPerPixel], width * bytesPerPixel);
		}
	}

	Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(&bif, (void*)bitsNew); 

	if (pad != 0) {
		delete[] bitsNew;
	}

	return bmp;
}

// ***************************************************************
//		InitGdiPlusPicture()
// ***************************************************************
void CDrawingOptionsEx::InitGdiPlusPicture()
{
	this->ReleaseGdiPlusBitmap();

	if (this->picture == NULL) 
	{
		return;
	}
	
	if (!bitmapPlus) {
		LoadIcon();
	}

	VARIANT_BOOL useTransparency;
	OLE_COLOR transpColor1, transpColor2;
	this->picture->get_UseTransparencyColor(&useTransparency);
	this->picture->get_TransparencyColor(&transpColor1);
	this->picture->get_TransparencyColor2(&transpColor2);

	if (imgAttributes)
	{
		delete imgAttributes;
		imgAttributes = NULL;
	}

	double alpha = fillTransparency/255.0;
	imgAttributes = ImageHelper::GetImageAttributes(static_cast<float>(alpha), (useTransparency == VARIANT_TRUE), transpColor1, transpColor2);
}

// ***************************************************************
//		LoadIcon()
// ***************************************************************
void CDrawingOptionsEx::LoadIcon()
{
	// let's try to load it
	if (bitmapPlus) return;

	tkImageSourceType type;
	this->picture->get_SourceType(&type);

	// first let's check whether it is in-memory GDI+ icon after deserialization
	if (type == istGDIPlus)
	{

		bitmapPlus = ImageHelper::GetGdiPlusIcon(this->picture);
		_needDeleteBitmapPlus = false;
		return;
	}
	
	bool useGDIPlusReading = false;

	ImageType imageType;
	this->picture->get_ImageType(&imageType);
	if (imageType == JPEG_FILE || imageType == PNG_FILE || imageType == GIF_FILE)
	{
		CComBSTR filename;
		this->picture->get_Filename(&filename);

		USES_CONVERSION;
		long size = Utility::GetFileSize(OLE2W(filename));

		if (size < (long)(0x1 << 20))
		{
			useGDIPlusReading = true;
		}
	}

	if (useGDIPlusReading)		// TODO: do we need both methods?
	{
		CComBSTR filename;
		this->picture->get_Filename(&filename);
		bitmapPlus = new Gdiplus::Bitmap(OLE2W(filename));
	}
	else
	{
		bitmapPlus = ImageToGdiPlusBitmap(this->picture);
	}

	_needDeleteBitmapPlus = true;
}

// ***************************************************************
//	  get_CharacterShape()
// ***************************************************************
// Returning pointCharacter as a path
Gdiplus::GraphicsPath* CDrawingOptionsEx::get_FontCharacterPath(CDC* dc, bool previewDrawing)
{
	CFont fnt;
	int val = previewDrawing ? 8 : 10;
	fnt.CreatePointFont(int(this->pointSize * val), this->fontName);

	CFont* oldFont = dc->SelectObject(&fnt);
	Gdiplus::Font fontPlus(dc->m_hDC);
	dc->SelectObject(&oldFont);
	
	CString str((char)this->pointCharcter);
	int fontSize = MultiByteToWideChar(CP_ACP, 0, str.GetString(), -1, NULL, 0);
	WCHAR* wText = new WCHAR[fontSize];
	MultiByteToWideChar(CP_ACP, 0, str.GetString(), -1, wText, fontSize);
	
	Gdiplus::GraphicsPath* path = new Gdiplus::GraphicsPath();
	Gdiplus::StringFormat fmt;
	fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
	fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	Gdiplus::FontFamily family; fontPlus.GetFamily(&family);
	
	path->StartFigure();
	path->AddString(wText, wcslen(wText), &family , fontPlus.GetStyle(), fontPlus.GetSize(), Gdiplus::PointF(0.0f, 0.0f), &fmt);
	delete[] wText;

	return path;
}

// **************************************************************
//		ReleaseGdiPlusBrush()
// **************************************************************
void CDrawingOptionsEx::ReleaseGdiPlusBrush()
{
	if (brushPlus)
	{
		delete brushPlus;
		brushPlus = NULL;
	}
}

// **************************************************************
//		ReleaseGdiPlusBitmap()
// **************************************************************
void CDrawingOptionsEx::ReleaseGdiPlusBitmap()
{
	if (bitmapPlus && _needDeleteBitmapPlus)
	{
		// deleting only in case it's not in-memory bitmap
		delete bitmapPlus; 
	}
	bitmapPlus = NULL;
}

// ***************************************************************
//	  ReleaseGdiPlusPen()
// ***************************************************************
void CDrawingOptionsEx::ReleaseGdiPlusPen()
{
	if (penPlus)
	{
		delete penPlus;
		penPlus = NULL;
	}
}

#pragma region Vertices
// ***************************************************************
//	  InitializeVerticesBrushAndPen()
// ***************************************************************
void CDrawingOptionsEx::InitGdiVerticesPen(CDC* dc)
{
	if (this->verticesVisible)
	{
		if (this->penOld || this->brushOld)
		{
			this->ReleaseGdiBrushAndPen(dc);	
		}

		if (this->verticesFillVisible)
		{
			this->brush->CreateSolidBrush(this->verticesColor);
			this->brushOld = dc->SelectObject(this->brush);

			this->pen->CreatePen(PS_NULL, 1, this->verticesColor);
			this->penOld = dc->SelectObject(this->pen);
		}
		else
		{
			this->pen->CreatePen(PS_SOLID, 1, this->verticesColor);
			this->penOld = dc->SelectObject(this->pen);

			this->brush->CreateStockObject(HOLLOW_BRUSH);
			this->brushOld = dc->SelectObject(this->brush);
		}
	}
}

// ******************************************************************
//		GetVertex()
// ******************************************************************
CPoint* CDrawingOptionsEx::GetVertex()
{
	CPoint* points = new CPoint[4];
	points[0].x = - this->verticesSize/2;
	points[1].x = this->verticesSize/2;
	points[2].x = this->verticesSize/2;
	points[3].x = - this->verticesSize/2;

	points[0].y = this->verticesSize/2;
	points[1].y = this->verticesSize/2;
	points[2].y = - this->verticesSize/2;
	points[3].y = - this->verticesSize/2;

	return points;
}
#pragma endregion

#pragma region PointSymbol
// ****************************************************
//	 DrawPointSymbol()
// ****************************************************
// Used for drawing individual symbols to external canvas (ShapeDrawingOptions.Draw)
void CDrawingOptionsEx::DrawPointSymbol(Gdiplus::Graphics& g, CDC* dc, Gdiplus::Point* points, float* angles, int count)
{
	if (this->pointSymbolType == ptSymbolStandard || (this->pointSymbolType == ptSymbolPicture && this->picture == NULL))
	{
		if (this->pointShapeType == ptShapeCircle)
		{
			float size = this->pointSize;
			for (int j = 0; j < count; j++)
			{
				Gdiplus::RectF rect(points[j].X - size/2.0f, points[j].Y - size/2.0f, size, size);
				
				this->InitGdiPlusBrushAndPen(&rect);
				
				if (this->penPlus)
					this->penPlus->SetAlignment(Gdiplus::PenAlignmentCenter);

				if (this->fillVisible)
				{
					g.FillEllipse(this->brushPlus, rect);
				}
				if (this->penPlus && this->linesVisible)
				{
					g.DrawEllipse(this->penPlus, rect);
				}
			}
		}
		else
		{
			// we need to change rotation options for each figure and to set it back after it
			int numPoints = 0;
			float* pointsTemp = get_SimplePointShape(this->pointShapeType, this->pointSize, this->rotation, this->pointNumSides, this->pointShapeRatio, &numPoints);
			
			Gdiplus::RectF rect(0, 0, this->pointSize, this->pointSize);
			if (this->fillType == ftGradient && this->fillVisible)
			{
				float xMin = FLT_MAX, xMax = -FLT_MAX, yMin = FLT_MAX, yMax = -FLT_MAX;
				for (int i = 0; i < numPoints * 2; i = i + 2)
				{
					if (pointsTemp[i] < xMin)
						xMin = pointsTemp[i];

					if (pointsTemp[i] > xMax)
						xMax = pointsTemp[i];

					if (pointsTemp[i + 1] < yMin)
						yMin = pointsTemp[i + 1];

					if (pointsTemp[i + 1] > yMax)
						yMax = pointsTemp[i + 1];
				}
				rect.X = xMin;
				rect.Y = yMin; 
				rect.Width = xMax - xMin;
				rect.Height = yMax - yMin;
			}
			this->InitGdiPlusBrushAndPen(&rect);
			if (this->penPlus)
					this->penPlus->SetAlignment(Gdiplus::PenAlignmentCenter);

			Gdiplus::Matrix mtx;
			if (pointsTemp)
			{
				for (int j = 0; j < count; j++)
				{
					mtx.Translate((float)points[j].X, (float)points[j].Y);
					if (angles) 
					{
						mtx.Rotate(-angles[j]);
					}

					g.SetTransform(&mtx);
					
					if (this->fillVisible)
					{
						g.FillPolygon(this->brushPlus, reinterpret_cast<Gdiplus::PointF*>(pointsTemp), numPoints);
					}
					
					if (this->penPlus && this->linesVisible)
					{
						g.DrawPolygon(this->penPlus, reinterpret_cast<Gdiplus::PointF*>(pointsTemp), numPoints);
					}
					
					mtx.Reset();
				}
				g.SetTransform(&mtx);
				
				delete[] pointsTemp;
				pointsTemp = NULL;
			}
		}
	}
	else if (this->pointSymbolType == ptSymbolPicture)
	{
		if ( this->picture )
		{
			this->InitGdiPlusPicture();
			
			int newWidth = (int)(this->bitmapPlus->GetWidth() * this->scaleX);
			int newHeight = (int)(this->bitmapPlus->GetHeight() * this->scaleY);
			
			Gdiplus::Matrix mtx;

			for (int j = 0; j < count; j++)
			{
				mtx.Translate((float)points[j].X, (float)points[j].Y);
				mtx.Rotate((float)this->rotation);

				if (angles) 
				{
					mtx.Rotate(-angles[j]);
				}

				// if reflecting
				if (this->pointReflectionType != prtNone)
				{
					Gdiplus::Matrix flipMatrix;
					// set up appropriate matrix
					if (this->pointReflectionType == prtLeftToRight)
						flipMatrix.SetElements(-1, 0, 0, 1, 0, 0);
					else if (this->pointReflectionType == prtTopToBottom)
						flipMatrix.SetElements(1, 0, 0, -1, 0, 0);
					// flip the matrix
					mtx.Multiply(&flipMatrix);
				}

				g.SetTransform(&mtx);

				if (this->bitmapPlus && this->fillVisible)
				{
					Gdiplus::Rect rect(-newWidth/2, -newHeight/2, newWidth, newHeight);
					g.DrawImage(this->bitmapPlus, rect, 0, 0, this->bitmapPlus->GetWidth(), 
								this->bitmapPlus->GetHeight(), Gdiplus::UnitPixel, this->imgAttributes);
				}
				mtx.Reset();
			}
			
			this->ReleaseGdiPlusBitmap();
			g.SetTransform(&mtx);
		}
	}
	else if ( this->pointSymbolType == ptSymbolFontCharacter)
	{
		Gdiplus::GraphicsPath* path = this->get_FontCharacterPath(dc, true);
		
		if (path)
		{
			Gdiplus::RectF rect(0, 0, this->pointSize, this->pointSize);
			if (this->fillType == ftGradient && this->fillVisible)
			{
				Gdiplus::PathData data;
				Gdiplus::Status status = path->GetPathData(&data);

				if (status == Gdiplus::Ok && data.Count > 0)
				{
					Gdiplus::PointF* pnts = new Gdiplus::PointF[data.Count];
					path->GetPathPoints(pnts, data.Count);
				
					float xMin = FLT_MAX, xMax = FLT_MIN, yMin = FLT_MAX, yMax = FLT_MIN;
					for (int i = 0; i < data.Count; i++)
					{
						if (pnts[i].X < xMin)
							xMin = pnts[i].X;

						if (pnts[i].X > xMax)
							xMax = pnts[i].X;

						if (pnts[i].Y < yMin)
							yMin = pnts[i].Y;

						if (pnts[i].Y > yMax)
							yMax = pnts[i].Y;
					}
					rect.X = xMin;
					rect.Y = yMin; 
					rect.Width = xMax - xMin;
					rect.Height = yMax - yMin;
					delete[] pnts;
				}
			}
			this->InitGdiPlusBrushAndPen(&rect);
			if (this->penPlus)
				this->penPlus->SetAlignment(Gdiplus::PenAlignmentCenter);

			Gdiplus::GraphicsPath* path2 = NULL;
			if (path && this->drawFrame)
			{
				path2 = this->GetFrameForPath(*path);
			}

			Gdiplus::Matrix mtx;
			for (int j = 0; j < count; j++)
			{
				mtx.Translate((float)points[j].X, (float)points[j].Y);
				mtx.Rotate((float)this->rotation);
				if (angles) 
					mtx.Rotate(-angles[j]);

				// if reflecting
				if (this->pointReflectionType != prtNone)
				{
					Gdiplus::Matrix flipMatrix;
					// set up appropriate matrix
					if (this->pointReflectionType == prtLeftToRight)
						flipMatrix.SetElements(-1, 0, 0, 1, 0, 0);
					else if (this->pointReflectionType == prtTopToBottom)
						flipMatrix.SetElements(1, 0, 0, -1, 0, 0);
					// flip the matrix
					mtx.Multiply(&flipMatrix);
				}

				g.SetTransform(&mtx);
				
				if (path2)
				{
					this->DrawGraphicPathWithFillColor(&g, path2, 2.0f);
				}

				if (this->fillVisible)
					g.FillPath(this->brushPlus, path);
				
				if (this->penPlus && this->linesVisible)
					g.DrawPath(this->penPlus, path);
				
				mtx.Reset();
			}
		
			g.SetTransform(&mtx);
			delete path;
		}
	}
}
#pragma endregion

#pragma region Utilities
// ***************************************************************
//	  InitGDIPlusBrushAndPen()
// ***************************************************************
void CDrawingOptionsEx::InitGdiPlusBrushAndPen(Gdiplus::RectF* bounds)
{
	this->InitGdiPlusPen();
	this->InitGdiPlusBrush( bounds);
}

// **************************************************************
//		FillGraphicsPath
// **************************************************************
void CDrawingOptionsEx::FillGraphicsPath(Gdiplus::Graphics* graphics, Gdiplus::GraphicsPath* path, Gdiplus::RectF& bounds)
{
	this->InitGdiPlusBrush( &bounds );
	Gdiplus::PixelOffsetMode mode = graphics->GetPixelOffsetMode();
	if (m_globalSettings.pixelOffsetMode != pomDefault)
		graphics->SetPixelOffsetMode((Gdiplus::PixelOffsetMode)m_globalSettings.pixelOffsetMode);
	graphics->FillPath(this->brushPlus, path);
	graphics->SetPixelOffsetMode(mode);
	ReleaseGdiPlusBrush();
}

// **************************************************************
//		DrawGraphicPath
// **************************************************************
void CDrawingOptionsEx::DrawGraphicPath(Gdiplus::Graphics* graphics, Gdiplus::GraphicsPath* path)
{
	this->InitGdiPlusPen();
	if (this->penPlus)
	{
		this->penPlus->SetLineJoin(Gdiplus::LineJoinRound);
		graphics->DrawPath(this->penPlus, path);
		this->ReleaseGdiPlusPen();
	}
}

void CDrawingOptionsEx::DrawGraphicPathWithFillColor(Gdiplus::Graphics* graphics, Gdiplus::GraphicsPath* path, Gdiplus::REAL width)
{
	this->ReleaseGdiPlusPen();
	
	if(this->fillVisible) 
	{
		penPlus = new Gdiplus::Pen(Utility::OleColor2GdiPlus(this->fillColor, (BYTE)this->fillTransparency), width);
		if (this->penPlus)
		{
			this->penPlus->SetLineJoin(Gdiplus::LineJoinRound);
			graphics->DrawPath(this->penPlus, path);
			this->ReleaseGdiPlusPen();
		}
	}
}

#pragma endregion

// *****************************************************
//   CanUseLinePattern
// *****************************************************
bool CDrawingOptionsEx::CanUseLinePattern()
{
	if (this->linePattern)
	{
		int count;
		this->linePattern->get_Count(&count);
		if (count > 0)
		{
			return true;
		}
	}
	return false;
}

Gdiplus::GraphicsPath* CDrawingOptionsEx::GetFrameForPath(Gdiplus::GraphicsPath& path)
{
	Gdiplus::GraphicsPath* path2 = NULL;
	Gdiplus::RectF bounds;
	Gdiplus::Status status = path.GetBounds(&bounds);
	if (status == Gdiplus::Status::Ok)
	{
		path2 = new Gdiplus::GraphicsPath();

		float offset = (bounds.Width + bounds.Height) / 8.0f;	// 1/4
		float x = bounds.X - offset, y = bounds.Y - offset, w = bounds.Width + 2 * offset, h = bounds.Height + 2 * offset;
		if (this->frameType == tkLabelFrameType::lfRoundedRectangle)
		{
			float max = max(w, h);
			float delta = max - w;
			if (delta > 0.0f) x -= delta/2.0f;
			delta = max - h;
			if (delta > 0.0f) y -= delta/2.0f;
			w = h = max;
		}

		switch(this->frameType)
		{
			case tkLabelFrameType::lfRectangle:
			case tkLabelFrameType::lfPointedRectangle:
			default:
				path2->AddLine(x, y, x, y + h);
				path2->AddLine(x, y + h, x + w, y + h);
				path2->AddLine(x + w, y + h, x + w, y);
				path2->AddLine(x + w, y, x, y);
				break;
			case tkLabelFrameType::lfRoundedRectangle:
				path2->AddEllipse(x, y, w, h);
				break;
		}
	}
	return path2;
}
#pragma endregion