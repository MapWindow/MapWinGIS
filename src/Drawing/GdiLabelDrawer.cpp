#include "stdafx.h"
#include "GdiLabelDrawer.h"

// *********************************************************************
// 					InitializeGdiCategory()										
// *********************************************************************
void GdiLabelDrawer::InitCategory(CDC* dc, CLabelOptions* options)
{
	penFontOutline.CreatePen(PS_SOLID, options->fontOutlineWidth, options->fontOutlineColor);
	penFrameOutline.CreatePen(options->frameOutlineStyle, options->frameOutlineWidth, options->frameOutlineColor);

	double haloWidth = fabs(options->fontSize / 16.0 * options->haloSize);
	penHalo.CreatePen(PS_SOLID, (int)haloWidth, options->haloColor);

	// we can select brush at once because it's the only one to use
	brushFrameBack.CreateSolidBrush(options->frameBackColor);
	oldBrush = dc->SelectObject(&brushFrameBack);

	gdiAlignment = ConvertAlignment(options->inboxAlignment);
}

// *****************************************************************
//		Converts label alignment to GDI constants
// *****************************************************************
inline UINT GdiLabelDrawer::ConvertAlignment(tkLabelAlignment alignment)
{
	switch (alignment)
	{
		case laCenter:			return DT_VCENTER | DT_CENTER | DT_NOCLIP;
		case laCenterLeft:		return DT_VCENTER | DT_LEFT | DT_NOCLIP;
		case laCenterRight:		return DT_VCENTER | DT_RIGHT | DT_NOCLIP;
		case laBottomCenter:	return DT_BOTTOM | DT_CENTER | DT_NOCLIP;
		case laBottomLeft:		return DT_BOTTOM | DT_LEFT | DT_NOCLIP;
		case laBottomRight:		return DT_BOTTOM | DT_RIGHT | DT_NOCLIP;
		case laTopCenter:		return DT_TOP | DT_CENTER | DT_NOCLIP;
		case laTopLeft:			return DT_TOP | DT_LEFT | DT_NOCLIP;
		case laTopRight:		return DT_TOP | DT_RIGHT | DT_NOCLIP;
		default:				return DT_VCENTER | DT_RIGHT | DT_NOCLIP;
	}
}

// *********************************************************************
// 					GdiResources::DrawLabelGdi()										
// *********************************************************************
void GdiLabelDrawer::DrawLabel(CDC* _dc, CLabelOptions* options, CLabelInfo* lbl, CRect& rect, double angleRad, double piX, double piY)
{
	// ------------------------------------------------------
	//	GDI drawing w/o transparency, gradients, etc
	// ------------------------------------------------------
	XFORM xForm;
	xForm.eM11 = (FLOAT)cos(angleRad);
	xForm.eM12 = (FLOAT)sin(angleRad);
	xForm.eM21 = (FLOAT)-sin(angleRad);
	xForm.eM22 = (FLOAT)cos(angleRad);
	xForm.eDx = (FLOAT)piX;
	xForm.eDy = (FLOAT)piY;
	_dc->SetWorldTransform(&xForm);

	// drawing frame
	if (options->frameVisible)
	{
		oldPen = _dc->SelectObject(&penFrameOutline);
		DrawLabelFrame(_dc, &rect, options);
		_dc->SelectObject(oldPen);
	}

	// drawing outlines
	if (options->shadowVisible)
	{
		_dc->SetWindowOrg(-options->shadowOffsetX, -options->shadowOffsetY);
		_dc->SetTextColor(options->shadowColor);
		_dc->DrawText(lbl->text, rect, gdiAlignment);
		_dc->SetTextColor(options->fontColor);
		_dc->SetWindowOrg(0, 0);
	}

	if (options->haloVisible)
	{
		_dc->BeginPath();
		_dc->DrawText(lbl->text, rect, gdiAlignment);
		_dc->EndPath();
		oldPen = _dc->SelectObject(&penHalo);
		_dc->StrokePath();
		_dc->SelectObject(oldPen);
	}

	if (options->fontOutlineVisible)
	{
		_dc->BeginPath();
		_dc->DrawText(lbl->text, rect, gdiAlignment);
		_dc->EndPath();
		oldPen = _dc->SelectObject(&penFontOutline);
		_dc->StrokePath();
		_dc->SelectObject(oldPen);
	}

	_dc->DrawText(lbl->text, rect, gdiAlignment);	// TODO: make a property (left/center/right)
}

// ********************************************************************
//		DrawLabelFrameGdi
// ********************************************************************
void GdiLabelDrawer::DrawLabelFrame(CDC* dc, CRect* rect, CLabelOptions* m_options)
{
	switch (m_options->frameType)
	{
		case lfRectangle:
			dc->Rectangle(rect->left, rect->top, rect->right, rect->bottom);
			break;
		case lfRoundedRectangle:
			dc->RoundRect(rect->left, rect->top, rect->right, rect->bottom, rect->Height(), rect->Height());
			break;
		case lfPointedRectangle:
			dc->BeginPath();
			dc->MoveTo(rect->left, rect->top);
			dc->LineTo(rect->right, rect->top);
			dc->LineTo(rect->right + rect->Height() / 4, (rect->top + rect->bottom) / 2);
			dc->LineTo(rect->right, rect->bottom);
			dc->LineTo(rect->left, rect->bottom);
			dc->LineTo(rect->left - rect->Height() / 4, (rect->top + rect->bottom) / 2);
			dc->LineTo(rect->left, rect->top);
			dc->EndPath();
			dc->StrokeAndFillPath();
			break;
	}
	return;
}

// *********************************************************************
// 					CreateFont()										
// *********************************************************************
CFont* GdiLabelDrawer::CreateFont(CLabelOptions* options, long fontSize, double scaleFactor)
{
	CFont* fnt = new CFont();

	CString s(options->fontName);
	fnt->CreatePointFont(fontSize * 10, s);

	LOGFONT lf;
	fnt->GetLogFont(&lf);

	if (scaleFactor != 1.0)
	{
		lf.lfWidth = long((double)lf.lfWidth * scaleFactor);
		lf.lfHeight = long((double)lf.lfHeight * scaleFactor);
	}
	if (abs(lf.lfHeight) < 4)	// changed 1 to 4; there is no way to read labels smaller than 4, but they slow down the performance
	{
		fnt->DeleteObject();
		delete fnt;
		return NULL;
	}

	lf.lfItalic = options->fontStyle & fstItalic;
	lf.lfUnderline = options->fontStyle & fstUnderline;
	lf.lfStrikeOut = options->fontStyle & fstStrikeout;
	lf.lfWeight = options->fontStyle & fstBold ? FW_BOLD : 0;

	fnt->DeleteObject();
	fnt->CreateFontIndirectA(&lf);

	return fnt;
}