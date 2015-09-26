#pragma once
#include "LabelOptions.h"

class GdiLabelDrawer {

public:	
	GdiLabelDrawer()
	{
	}

public:
	CPen penFontOutline;
	CPen penHalo;
	CPen penFrameOutline;
	CBrush brushFrameBack;
	CBrush* oldBrush = NULL;
	CPen* oldPen = NULL;
	CFont* oldFont = NULL;
	UINT gdiAlignment;
private:
	inline UINT ConvertAlignment(tkLabelAlignment alignment);
	void DrawLabelFrame(CDC* dc, CRect* rect, CLabelOptions* m_options);

public:
	CFont* CreateFont(CLabelOptions* options, long fontSize, double scaleFactor);

	void DrawLabel(CDC* dc, CLabelOptions* options, CLabelInfo* lbl, CRect& rect, double angleRad, double piX, double piY);

	void InitCategory(CDC* dc, CLabelOptions* options);

	void Release() {
		brushFrameBack.DeleteObject();
		penFontOutline.DeleteObject();
		penFrameOutline.DeleteObject();
		penHalo.DeleteObject();
	}
};