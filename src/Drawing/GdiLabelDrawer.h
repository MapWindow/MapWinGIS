#pragma once
#include "LabelOptions.h"

class GdiLabelDrawer {

public:	
	GdiLabelDrawer()
		: _oldBrush(NULL), _oldFont(NULL), _font(NULL)
	{
		for (long i = 0; i <= MAX_LABEL_SIZE; i++) {
			_fonts[i] = NULL;
		}
	}

private:
	CFont* _fonts[MAX_LABEL_SIZE + 1];
	CDC* _dc;
	HDC _hdc;
	Gdiplus::Graphics* _graphics;	
	CPen _penFontOutline;
	CPen _penHalo;
	CPen _penFrameOutline;
	CBrush _brushFrameBack;
	CBrush* _oldBrush;
	CFont* _oldFont;
	CFont* _font;
	UINT _alignment;

private:
	inline UINT ConvertAlignment(tkLabelAlignment alignment);

	CFont* CreateFont(CLabelOptions* options, long fontSize, double scaleFactor);

	void DrawFrame(CRect* rect, CLabelOptions* m_options);

	void ReleaseFonts(bool useVariableFontSize);

public:
	void DrawLabel(CLabelOptions* options, CLabelInfo* lbl, CRect& rect, double angleRad, double piX, double piY);

	void InitFromCategory(CLabelOptions* options);

	void MeasureString(CLabelInfo* lbl, CRect& rect);

	void SelectFont(CLabelOptions* options, long fontSize, double scaleFactor);

	void SelectFont(CLabelOptions* options, CLabelInfo* lbl, double scaleFactor, long fontSize);

	void InitDc(Gdiplus::Graphics* g)
	{
		_graphics = g;
		_hdc = g->GetHDC();
		_dc = CDC::FromHandle(_hdc);
		_dc->SaveDC();
		_dc->SetBkMode(TRANSPARENT);
		_dc->SetGraphicsMode(GM_ADVANCED);
	}

	void ReleaseDc()
	{
		_dc->RestoreDC(-1);
		_dc = NULL;
		_graphics->ReleaseHDC(_hdc);
		_hdc = NULL;
		_graphics = NULL;
	}

	void ReleaseForCategory(bool useVariableFontSize) {
		_dc->SelectObject(_oldBrush);
		_dc->SelectObject(_oldFont);

		_brushFrameBack.DeleteObject();
		_penFontOutline.DeleteObject();
		_penFrameOutline.DeleteObject();
		_penHalo.DeleteObject();

		ReleaseFonts(useVariableFontSize);
	}
};