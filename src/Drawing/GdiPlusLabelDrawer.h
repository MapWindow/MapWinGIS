#pragma once
#include "LabelOptions.h"

class GdiPlusLabelDrawer {
public:
	GdiPlusLabelDrawer()
		: clFont1(0, 0, 0),
		clFont2(0, 0, 0),
		clFrameBack1(0, 0, 0),
		clFrameBack2(0, 0, 0),
		rect(0.0f, 0.0f, 0.0f, 0.0f)
	{
		compositingQuality = Gdiplus::CompositingQualityDefault;
		smoothingMode = Gdiplus::SmoothingModeDefault;
		textRenderingHint = Gdiplus::TextRenderingHintSingleBitPerPixelGridFit;
	}

public:
	// pens
	Gdiplus::Pen* penFontOutline = NULL;
	Gdiplus::Pen* penHalo = NULL;
	Gdiplus::Pen* penFrameOutline = NULL;

	// simple brushes in case no gradient is used
	Gdiplus::SolidBrush* brushFont = NULL;
	Gdiplus::SolidBrush* brushFrame = NULL;
	Gdiplus::SolidBrush* brushShadow = NULL;

	Gdiplus::Color clFont1;
	Gdiplus::Color clFont2;
	Gdiplus::Color clFrameBack1;
	Gdiplus::Color clFrameBack2;

	// gradient brushes will be allocated dynamically as it's impossible 
	// to change properties of the created brush
	Gdiplus::LinearGradientBrush* brushFontGrad = NULL;
	Gdiplus::LinearGradientBrush* brushFrameGrad = NULL;

	Gdiplus::RectF rect;

	Gdiplus::StringFormat stringFormat;

	Gdiplus::TextRenderingHint textRenderingHint;
	Gdiplus::SmoothingMode smoothingMode;
	Gdiplus::CompositingQuality compositingQuality;

private:
	void ConvertAlignment(tkLabelAlignment alignment, Gdiplus::StringFormat& format);
	void DrawLabelFrame(Gdiplus::Graphics* graphics, Gdiplus::Brush* brush, Gdiplus::Pen& pen, CLabelOptions* m_options);

public:
	void DrawLabel(Gdiplus::Graphics* g, CLabelOptions* m_options, Gdiplus::Font* font, CRect& r, CStringW wText, double piX, double piY, double angle);

	void InitCategory(Gdiplus::Graphics* g, CLabelOptions* m_options, bool hasRotation);

	void InitializeGraphics(Gdiplus::Graphics* g, ILabels* labels);

	void RestoreGraphics(Gdiplus::Graphics* g);

	Gdiplus::Font* CreateFont(CLabelOptions* options, double fontSize, double scaleFactor);

	void Release() {
		delete penFontOutline;
		delete penHalo;
		delete penFrameOutline;
		delete brushFont;
		delete brushFrame;
		delete brushShadow;
	}
};