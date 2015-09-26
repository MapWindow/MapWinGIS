#pragma once
#include "LabelOptions.h"
using namespace Gdiplus;

class GdiPlusLabelDrawer {
public:
	GdiPlusLabelDrawer()
		: _clFont1(0, 0, 0),
		_clFont2(0, 0, 0),
		_clFrameBack1(0, 0, 0),
		_clFrameBack2(0, 0, 0),
		rect(0.0f, 0.0f, 0.0f, 0.0f),
		_graphics(NULL)
	{
		_compositingQuality = CompositingQualityDefault;
		_smoothingMode = SmoothingModeDefault;
		_textRenderingHint = TextRenderingHintSingleBitPerPixelGridFit;
		
		for (long i = 0; i <= MAX_LABEL_SIZE; i++ ) {
			_fonts[i] = NULL;
		}
	}

private:
	Font* _fonts[MAX_LABEL_SIZE + 1];
	Graphics* _graphics;
	StringFormat stringFormat;

	// initial values
	TextRenderingHint _textRenderingHint;
	SmoothingMode _smoothingMode;
	CompositingQuality _compositingQuality;

	// colors
	Color _clFont1;
	Color _clFont2;
	Color _clFrameBack1;
	Color _clFrameBack2;

	// pens
	Pen* _penFontOutline = NULL;
	Pen* _penHalo = NULL;
	Pen* _penFrameOutline = NULL;

	// simple brushes in case no gradient is used
	SolidBrush* _brushFont = NULL;
	SolidBrush* _brushFrame = NULL;
	SolidBrush* _brushShadow = NULL;

	// gradient brushes will be allocated dynamically as it's impossible 
	// to change properties of the created brush
	LinearGradientBrush* _brushFontGrad = NULL;
	LinearGradientBrush* _brushFrameGrad = NULL;
public:
	RectF rect;

	Font* font;

	CStringW text;

private:
	void ConvertAlignment(tkLabelAlignment alignment, StringFormat& format);

	Font* CreateFont(CLabelOptions* options, double fontSize, double scaleFactor);

	void DrawLabelFrame(Brush* brush, Pen& pen, CLabelOptions* m_options);

	void InitPensAndBrushes(CLabelOptions* options, bool hasRotation);

	void TryAutoSetRenderingHint(CLabelOptions* options, bool hasRotation);

	void ReleaseFonts(bool useVariableFontSize);

	void ReleaseObjects() {
		delete _penFontOutline;
		delete _penHalo;
		delete _penFrameOutline;
		delete _brushFont;
		delete _brushFrame;
		delete _brushShadow;
	}

public:
	void DrawLabel(CLabelOptions* options, CRect& r, double piX, double piY, double angle);

	void InitFromCategory(CLabelOptions* m_options, bool hasRotation);

	void InitGraphics(Graphics* g, ILabels* labels);
	
	void MeasureString(CLabelInfo* lbl, CRect& r);

	void RestoreGraphics();

	void SelectFont(CLabelOptions* options, double fontSize, double scaleFactor);

	void SelectFont(CLabelOptions* options, CLabelInfo* lbl, double scaleFactor, long fontSize);

	void ReleaseForCategory(bool useVariableFontSize) {
		ReleaseObjects();

		ReleaseFonts(useVariableFontSize);
	}
};