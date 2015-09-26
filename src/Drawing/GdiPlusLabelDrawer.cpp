#include "stdafx.h"
#include "GdiPlusLabelDrawer.h"

using namespace Gdiplus;

// *********************************************************************
// 			InitializeGdiPlusGraphics()										
// *********************************************************************
void GdiPlusLabelDrawer::InitializeGraphics(Gdiplus::Graphics* g, ILabels* labels)
{
	// saving initial state
	compositingQuality = g->GetCompositingQuality();
	smoothingMode = g->GetSmoothingMode();
	textRenderingHint = g->GetTextRenderingHint();

	tkTextRenderingHint textRenderingHint;
	labels->get_TextRenderingHint(&textRenderingHint);

	tkSmoothingMode smoothingMode = m_globalSettings.labelsSmoothingMode;
	tkCompositingQuality compositingQuality = m_globalSettings.labelsCompositingQuality;

	g->SetCompositingQuality((CompositingQuality)compositingQuality);
	g->SetSmoothingMode((SmoothingMode)smoothingMode);
	g->SetTextRenderingHint((TextRenderingHint)textRenderingHint);
	g->SetTextContrast(0u);
}

// *********************************************************************
// 			RestoreGraphics()										
// *********************************************************************
void GdiPlusLabelDrawer::RestoreGraphics(Gdiplus::Graphics* g)
{
	g->SetCompositingQuality(compositingQuality);
	g->SetSmoothingMode(smoothingMode);
	g->SetTextRenderingHint(textRenderingHint);
}

// *********************************************************************
// 					InitializeGdiPlusCategory()										
// *********************************************************************
void GdiPlusLabelDrawer::InitCategory(Gdiplus::Graphics* g, CLabelOptions* options, bool hasRotation)
{
	long alphaFont = (options->fontTransparency << 24);
	long alphaFrame = (options->frameTransparency << 24);

	clFont1.SetValue(alphaFont | BGR_TO_RGB(options->fontColor));
	clFont2.SetValue(alphaFont | BGR_TO_RGB(options->fontColor2));
	clFrameBack1.SetValue(alphaFrame | BGR_TO_RGB(options->frameBackColor));
	clFrameBack2.SetValue(alphaFrame | BGR_TO_RGB(options->frameBackColor2));

	penFontOutline = new Pen(Color(alphaFont | BGR_TO_RGB(options->fontOutlineColor)), (Gdiplus::REAL)options->fontOutlineWidth);
	penFontOutline->SetLineJoin(LineJoinRound);

	double haloWidth = fabs(options->fontSize / 16.0 * options->haloSize);
	penHalo = new Pen(Color(alphaFont | BGR_TO_RGB(options->haloColor)), (Gdiplus::REAL)haloWidth);
	penHalo->SetLineJoin(LineJoinRound);

	penFrameOutline = new Pen(Color(alphaFrame | BGR_TO_RGB(options->frameOutlineColor)), (Gdiplus::REAL)options->frameOutlineWidth);
	penFrameOutline->SetDashStyle(DashStyle(options->frameOutlineStyle));

	brushFont = new SolidBrush(clFont1);
	brushFrame = new SolidBrush(clFrameBack1);
	brushShadow = new SolidBrush(Color(alphaFont | BGR_TO_RGB(options->shadowColor)));

	ConvertAlignment(options->inboxAlignment, stringFormat);

	stringFormat.SetFormatFlags(StringFormatFlagsNoClip);	// doesn't work?

	if (m_globalSettings.autoChooseRenderingHintForLabels)
	{
		Gdiplus::TextRenderingHint hint = TextRenderingHintSingleBitPerPixelGridFit;
		if (hasRotation)
		{
			if (options->frameVisible && options->frameTransparency == 255) {
				hint = TextRenderingHintClearTypeGridFit;
			}
			else {
				hint = TextRenderingHintAntiAlias;
			}

		}
		g->SetTextRenderingHint(hint);
	}
}

// *********************************************************************
// 					DrawLabelGdiPlus()										
// *********************************************************************
void GdiPlusLabelDrawer::DrawLabel(Graphics* g, CLabelOptions* options, Font* font, CRect& r, CStringW wText, double piX, double piY, double angle)
{
	rect.X = (Gdiplus::REAL)r.left;
	rect.Y = (Gdiplus::REAL)r.top;
	rect.Width = (Gdiplus::REAL)r.Width();
	rect.Height = (Gdiplus::REAL)r.Height();

	Gdiplus::Matrix mtxInit;
	g->GetTransform(&mtxInit);

	//Gdiplus::Matrix mtx;
	//mtx.Translate((Gdiplus::REAL)piX, (Gdiplus::REAL)piY);
	//mtx.Rotate((Gdiplus::REAL)angle);
	//_graphics->SetTransform(&mtx);

	g->TranslateTransform((Gdiplus::REAL)piX, (Gdiplus::REAL)piY);
	g->RotateTransform((Gdiplus::REAL)angle);

	// drawing frame
	if (options->frameTransparency != 0 && options->frameVisible)
	{
		if (options->frameGradientMode != gmNone)
		{
			brushFrameGrad = new LinearGradientBrush(rect, clFrameBack1, clFrameBack2, (LinearGradientMode)options->frameGradientMode);
			DrawLabelFrame(g, brushFrameGrad, *penFrameOutline, options);
			delete brushFrameGrad;
		}
		else
		{
			DrawLabelFrame(g, brushFrame, *penFrameOutline, options);
		}
	}

	if (options->fontTransparency != 0)
	{
		bool pathNeeded = options && (options->shadowVisible || options->haloVisible || options->fontOutlineVisible);
		GraphicsPath* gp = NULL;
		if (pathNeeded)
		{
			gp = new GraphicsPath();
			gp->StartFigure();
			FontFamily fam;
			font->GetFamily(&fam);
			gp->AddString(wText, wText.GetLength(), &fam, font->GetStyle(), font->GetSize(), rect, &stringFormat);
			gp->CloseFigure();
		}

		// drawing outlines
		if (options->shadowVisible)
		{
			Gdiplus::Matrix mtx1;
			mtx1.Translate((Gdiplus::REAL)options->shadowOffsetX, (Gdiplus::REAL)options->shadowOffsetY);
			gp->Transform(&mtx1);
			g->FillPath(brushShadow, gp);
			mtx1.Translate(Gdiplus::REAL(-2 * options->shadowOffsetX), Gdiplus::REAL(-2 * options->shadowOffsetY));
			gp->Transform(&mtx1);
		}

		if (options && options->haloVisible)
			g->DrawPath(penHalo, gp);

		if (options->fontOutlineVisible)
			g->DrawPath(penFontOutline, gp);

		if (options->fontGradientMode != gmNone)
		{
			brushFontGrad = new LinearGradientBrush(rect, clFont1, clFont2, (LinearGradientMode)options->fontGradientMode);
			g->DrawString(wText, wText.GetLength(), font, rect, &stringFormat, brushFontGrad);	// TODO: we need speed test here to choose the function
			delete brushFontGrad;
		}
		else
		{
			g->DrawString(wText, wText.GetLength(), font, rect, &stringFormat, brushFont);
		}
		if (pathNeeded) {
			delete gp;
		}
	}

	g->SetTransform(&mtxInit);
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
//		DrawLabelFrameGdiPlus
// ********************************************************************
void GdiPlusLabelDrawer::DrawLabelFrame(Gdiplus::Graphics* graphics, Gdiplus::Brush* brush, Gdiplus::Pen& pen, CLabelOptions* options)
{
	switch (options->frameType)
	{
		case lfRectangle:
			graphics->FillRectangle(brush, rect);
			graphics->DrawRectangle(&pen, rect);
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
			graphics->FillPath(brush, path);
			graphics->DrawPath(&pen, path);
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
			path->AddLine(left + rect.Height / 4, top, right - rect.Height / 4, top);

			path->AddLine(right - rect.Height / 4, top, right, (top + bottom) / 2);
			path->AddLine(right, (top + bottom) / 2, right - rect.Height / 4, bottom);

			path->AddLine(right - rect.Height / 4, bottom, left + rect.Height / 4, bottom);

			path->AddLine(left + rect.Height / 4, bottom, left, (top + bottom) / 2);
			path->AddLine(left, (top + bottom) / 2, left + rect.Height / 4, top);

			path->CloseFigure();
			graphics->FillPath(brush, path);
			graphics->DrawPath(&pen, path);
			delete path;
			break;
		}
	}
	return;
}

// *********************************************************************
// 					CreateGdiPlusFont()										
// *********************************************************************
Gdiplus::Font* GdiPlusLabelDrawer::CreateFont(CLabelOptions* options, double fontSize, double scaleFactor)
{
	CStringW fontName = OLE2W(options->fontName);

	if (scaleFactor != 1.0) {
		fontSize = fontSize * scaleFactor;
	}

	if (fontSize < 4)  {
		return NULL;
	}

	int style = FontStyleRegular;
	if (options->fontStyle & fstItalic) style = style | FontStyleItalic;
	if (options->fontStyle & fstBold) style = style | FontStyleBold;
	if (options->fontStyle & fstStrikeout) style = style | FontStyleStrikeout;
	if (options->fontStyle & fstUnderline) style = style | FontStyleUnderline;

	return new Gdiplus::Font(fontName, (Gdiplus::REAL)fontSize, style);
}