#include "stdafx.h"
#include "GdiPlusLabelDrawer.h"
#include "UtilityFunctions.h"

using namespace Gdiplus;

// *********************************************************************
// 			InitGraphics()										
// *********************************************************************
void GdiPlusLabelDrawer::InitGraphics(Gdiplus::Graphics* g, ILabels* labels)
{
	_graphics = g;
	
	_compositingQuality = _graphics->GetCompositingQuality();
	_smoothingMode = _graphics->GetSmoothingMode();
	_textRenderingHint = _graphics->GetTextRenderingHint();

	tkTextRenderingHint textRenderingHint;
	labels->get_TextRenderingHint(&textRenderingHint);

	tkSmoothingMode smoothingMode = m_globalSettings.labelsSmoothingMode;
	tkCompositingQuality compositingQuality = m_globalSettings.labelsCompositingQuality;

	_graphics->SetCompositingQuality((CompositingQuality)compositingQuality);
	_graphics->SetSmoothingMode((SmoothingMode)smoothingMode);
	_graphics->SetTextRenderingHint((TextRenderingHint)textRenderingHint);
	_graphics->SetTextContrast(0u);
}

// *********************************************************************
// 			RestoreGraphics()										
// *********************************************************************
void GdiPlusLabelDrawer::RestoreGraphics()
{
	_graphics->SetCompositingQuality(_compositingQuality);
	_graphics->SetSmoothingMode(_smoothingMode);
	_graphics->SetTextRenderingHint(_textRenderingHint);
}

// *********************************************************************
// 					InitFromCategory()										
// *********************************************************************
void GdiPlusLabelDrawer::InitFromCategory(CLabelOptions* options, bool hasRotation)
{
	InitPensAndBrushes(options, hasRotation);

	TryAutoSetRenderingHint(options, hasRotation);
}

// *********************************************************************
// 					InitPensAndBrushes()										
// *********************************************************************
void GdiPlusLabelDrawer::InitPensAndBrushes(CLabelOptions* options, bool hasRotation)
{
	long alphaFont = (options->fontTransparency << 24);
	long alphaFrame = (options->frameTransparency << 24);

	_clFont1 = Utility::OleColor2GdiPlus(options->fontColor, options->fontTransparency);
	_clFont2 = Utility::OleColor2GdiPlus(options->fontColor2, options->fontTransparency);

	_clFrameBack1.SetValue(alphaFrame | BGR_TO_RGB(options->frameBackColor));
	_clFrameBack2.SetValue(alphaFrame | BGR_TO_RGB(options->frameBackColor2));

	_penFontOutline = new Pen(Color(alphaFont | BGR_TO_RGB(options->fontOutlineColor)), (Gdiplus::REAL)options->fontOutlineWidth);
	_penFontOutline->SetLineJoin(LineJoinRound);

	double haloWidth = fabs(options->fontSize / 16.0 * options->haloSize);
	_penHalo = new Pen(Color(alphaFont | BGR_TO_RGB(options->haloColor)), (Gdiplus::REAL)haloWidth);
	_penHalo->SetLineJoin(LineJoinRound);

	_penFrameOutline = new Pen(Color(alphaFrame | BGR_TO_RGB(options->frameOutlineColor)), (Gdiplus::REAL)options->frameOutlineWidth);
	_penFrameOutline->SetDashStyle(DashStyle(options->frameOutlineStyle));

	_brushFont = new SolidBrush(_clFont1);
	_brushFrame = new SolidBrush(_clFrameBack1);
	_brushShadow = new SolidBrush(Color(alphaFont | BGR_TO_RGB(options->shadowColor)));

	ConvertAlignment(options->inboxAlignment, stringFormat);

	stringFormat.SetFormatFlags(StringFormatFlagsNoClip);	// doesn't work?
}

// *********************************************************************
// 					TryAutoSetRenderingHint()										
// *********************************************************************
void GdiPlusLabelDrawer::TryAutoSetRenderingHint(CLabelOptions* options, bool hasRotation)
{
	if (m_globalSettings.autoChooseRenderingHintForLabels)
	{
		Gdiplus::TextRenderingHint hint = TextRenderingHintSingleBitPerPixelGridFit;
		// JIRA MWGIS-74: not only rotation, but GraphicsPath-related functions also require AntiAliasing
		if (hasRotation || options->shadowVisible || options->haloVisible || options->fontOutlineVisible)
		{
			if (options->frameVisible && options->frameTransparency == 255) {
				hint = TextRenderingHintClearTypeGridFit;
			}
			else {
				hint = TextRenderingHintAntiAlias;
			}

		}
		_graphics->SetTextRenderingHint(hint);
	}
}

// *********************************************************************
// 					DrawLabel()										
// *********************************************************************
void GdiPlusLabelDrawer::DrawLabel(CLabelOptions* options, CRect& r, double piX, double piY, double angle)
{
	rect.X = static_cast<Gdiplus::REAL>(r.left);
	rect.Y = static_cast<Gdiplus::REAL>(r.top);
	rect.Width = static_cast<Gdiplus::REAL>(r.Width());
	rect.Height = static_cast<Gdiplus::REAL>(r.Height());

	Gdiplus::Matrix mtxInit;
	_graphics->GetTransform(&mtxInit);

	//Gdiplus::Matrix mtx;
	//mtx.Translate((Gdiplus::REAL)piX, (Gdiplus::REAL)piY);
	//mtx.Rotate((Gdiplus::REAL)angle);
	//_graphics->SetTransform(&mtx);

	_graphics->TranslateTransform(static_cast<Gdiplus::REAL>(piX), static_cast<Gdiplus::REAL>(piY));
	_graphics->RotateTransform(static_cast<Gdiplus::REAL>(angle));

	// drawing frame
	if (options->frameTransparency != 0 && options->frameVisible)
	{
		if (options->frameGradientMode != gmNone)
		{
			_brushFrameGrad = new LinearGradientBrush(rect, _clFrameBack1, _clFrameBack2, (LinearGradientMode)options->frameGradientMode);
			DrawLabelFrame(_brushFrameGrad, *_penFrameOutline, options);
			delete _brushFrameGrad;
		}
		else
		{
			DrawLabelFrame(_brushFrame, *_penFrameOutline, options);
		}
	}

	if (options->fontTransparency != 0)
	{
		const bool pathNeeded = options && (options->shadowVisible || options->haloVisible || options->fontOutlineVisible);
		GraphicsPath* gp = NULL;
		if (pathNeeded)
		{
			gp = new GraphicsPath();
			gp->StartFigure();
			FontFamily fam;
			font->GetFamily(&fam);
			// JIRA MWGIS-74: GraphicsPath font units are 'em's rather than Points
			const Gdiplus::REAL factor = _graphics->GetDpiX() / 72; // 4.0 / 3.0;
			gp->AddString(text, text.GetLength(), &fam, font->GetStyle(), (font->GetSize() * factor), rect, &stringFormat);
			gp->CloseFigure();
		}

		// drawing outlines
		if (options->shadowVisible)
		{
			Gdiplus::Matrix mtx1;
			mtx1.Translate(static_cast<Gdiplus::REAL>(options->shadowOffsetX), static_cast<Gdiplus::REAL>(options->shadowOffsetY));
			gp->Transform(&mtx1);
			_graphics->FillPath(_brushShadow, gp);
			mtx1.Translate(static_cast<Gdiplus::REAL>(-1 * options->shadowOffsetX), static_cast<Gdiplus::REAL>(-1 * options->shadowOffsetY));
			gp->Transform(&mtx1);
		}

		if (options && options->haloVisible)
		{
			_graphics->DrawPath(_penHalo, gp);
			_graphics->FillPath(_brushFont, gp);
		}

		if (options->fontOutlineVisible)
		{
			_graphics->DrawPath(_penFontOutline, gp);
			_graphics->FillPath(_brushFont, gp);
		}

		Gdiplus::Status status;

		if (options->fontGradientMode != gmNone)
		{
			_brushFontGrad = new LinearGradientBrush(rect, _clFont1, _clFont2, (LinearGradientMode)options->fontGradientMode);
			status = _graphics->DrawString(text, text.GetLength(), font, rect, &stringFormat, _brushFontGrad);
			delete _brushFontGrad;
		}
		else if (!pathNeeded)
		{
			// if path has not already been filled, draw the text here
			status = _graphics->DrawString(text, text.GetLength(), font, rect, &stringFormat, _brushFont);
		}

		if (pathNeeded) {
			delete gp;
		}
	}

	_graphics->SetTransform(&mtxInit);
}

// *****************************************************************
//		Converts label alignment to GDI+ constants
// *****************************************************************
inline void GdiPlusLabelDrawer::ConvertAlignment(tkLabelAlignment alignment, Gdiplus::StringFormat& format)
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

// ********************************************************************
//		DrawLabelFrame
// ********************************************************************
void GdiPlusLabelDrawer::DrawLabelFrame(Gdiplus::Brush* brush, Gdiplus::Pen& pen, CLabelOptions* options)
{
	switch (options->frameType)
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

			// MWGIS-229
			// start arcs outside of left and right boundaries, rather than
			// fitting them within, else we may not fully contain the text.
			path->AddLine(left, top, right, top);
			path->AddArc(right, top, rect.Height, rect.Height, -90.0, 180.0);
			path->AddLine(right, bottom, left, bottom);
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

			// MWGIS-229
			// place endcaps outside of left and right boundaries, rather than
			// fitting them within, else we may not fully contain the text.
			path->AddLine(left, top, right, top);
			path->AddLine(right, top, right, (top + bottom) / 2);
			path->AddLine(right, (top + bottom) / 2, right, bottom);
			path->AddLine(right, bottom, left, bottom);
			path->AddLine(left, bottom, left, (top + bottom) / 2);
			path->AddLine(left, (top + bottom) / 2, left, top);

			path->CloseFigure();
			_graphics->FillPath(brush, path);
			_graphics->DrawPath(&pen, path);
			delete path;
			break;
		}
	}
	return;
}

// *********************************************************************
// 					CreateFont()										
// *********************************************************************
Gdiplus::Font* GdiPlusLabelDrawer::CreateFont(CLabelOptions* options, double fontSize, double scaleFactor)
{
	CStringW fontName = OLE2W(options->fontName);

	if (scaleFactor != 1.0) {
		fontSize = fontSize * scaleFactor;
	}

	// MWGIS-121; rather than return null, enforce a minimum font size of 4
	if (fontSize < 4)  {
		fontSize = 4;
	}

	int style = FontStyleRegular;
	if (options->fontStyle & fstItalic) style = style | FontStyleItalic;
	if (options->fontStyle & fstBold) style = style | FontStyleBold;
	if (options->fontStyle & fstStrikeout) style = style | FontStyleStrikeout;
	if (options->fontStyle & fstUnderline) style = style | FontStyleUnderline;

	return new Gdiplus::Font(fontName, (Gdiplus::REAL)fontSize, style);
}

// *********************************************************************
// 					MeasureString()										
// *********************************************************************
void GdiPlusLabelDrawer::MeasureString(CLabelInfo* lbl, CRect& r)
{
	USES_CONVERSION;
	text = lbl->text;
	_graphics->MeasureString(text, text.GetLength(), font, PointF(0.0f, 0.0f), &rect);

	// in some case we lose the last letter by clipping; 
	rect.Width += 1;
	rect.Height += 1;

	// converting to CRect to prevent duplication in the code below
	r.left = static_cast<LONG>(rect.X);
	r.top = static_cast<LONG>(rect.Y);
	r.right = static_cast<LONG>(rect.X + rect.Width);
	r.bottom = static_cast<LONG>(rect.Y + rect.Height);
}

// *********************************************************************
// 		SelectFont()										
// *********************************************************************
void GdiPlusLabelDrawer::SelectFont(CLabelOptions* options, CLabelInfo* lbl, double scaleFactor)
{
	if (!_fonts[lbl->fontSize])
	{
		this->font = CreateFont(options, lbl->fontSize, scaleFactor);
		_fonts[lbl->fontSize] = this->font;
	}
	else {
		this->font = _fonts[lbl->fontSize];
	}
}

// *********************************************************************
// 		SelectFont()										
// *********************************************************************
void GdiPlusLabelDrawer::SelectFont(CLabelOptions* options, double fontSize, double scaleFactor)
{
	this->font = CreateFont(options, fontSize, scaleFactor);
}

// *********************************************************************
// 		ReleaseFonts()										
// *********************************************************************
void GdiPlusLabelDrawer::ReleaseFonts(bool useVariableFontSize)
{
	if (useVariableFontSize)
	{
		for (int i = 0; i <= MAX_LABEL_SIZE; i++)
		{
			if (_fonts[i])
			{
				Gdiplus::Font * f = _fonts[i];
				delete f;
				_fonts[i] = NULL;
			}
		}
	}
	else {
		if (font)
		{
			delete font;
			font = NULL;
		}
	}
}
