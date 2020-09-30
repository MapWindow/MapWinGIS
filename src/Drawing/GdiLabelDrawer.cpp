#include "stdafx.h"
#include "GdiLabelDrawer.h"

// *********************************************************************
// 					InitFromCategory()										
// *********************************************************************
void GdiLabelDrawer::InitFromCategory(CLabelOptions* options)
{
	_penFontOutline.CreatePen(PS_SOLID, options->fontOutlineWidth, options->fontOutlineColor);
	_penFrameOutline.CreatePen(options->frameOutlineStyle, options->frameOutlineWidth, options->frameOutlineColor);

	double haloWidth = fabs(options->fontSize / 16.0 * options->haloSize);
	_penHalo.CreatePen(PS_SOLID, (int)haloWidth, options->haloColor);

	// we can select brush at once because it's the only one to use
	_brushFrameBack.CreateSolidBrush(options->frameBackColor);
	_oldBrush = _dc->SelectObject(&_brushFrameBack);

	_alignment = ConvertAlignment(options->inboxAlignment);
	_dc->SetTextColor(options->fontColor);
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
// 					DrawLabel()										
// *********************************************************************
void GdiLabelDrawer::DrawLabel(CLabelOptions* options, CLabelInfo* lbl, CRect& rect, double angleRad, double piX, double piY)
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

	CPen* oldPen = NULL;

	// drawing frame
	if (options->frameVisible)
	{
		oldPen = _dc->SelectObject(&_penFrameOutline);
		DrawFrame(&rect, options);
		_dc->SelectObject(oldPen);
	}

	// drawing outlines
	if (options->shadowVisible)
	{
		_dc->SetWindowOrg(-options->shadowOffsetX, -options->shadowOffsetY);
		_dc->SetTextColor(options->shadowColor);
		DrawTextW(_dc->m_hDC, lbl->text, -1, rect, _alignment);
		_dc->SetTextColor(options->fontColor);
		_dc->SetWindowOrg(0, 0);
	}

	if (options->haloVisible)
	{
		_dc->BeginPath();
		DrawTextW(_dc->m_hDC, lbl->text, -1, rect, _alignment);
		_dc->EndPath();
		oldPen = _dc->SelectObject(&_penHalo);
		_dc->StrokePath();
		_dc->SelectObject(oldPen);
	}

	if (options->fontOutlineVisible)
	{
		_dc->BeginPath();
		DrawTextW(_dc->m_hDC, lbl->text, -1, rect, _alignment);
		_dc->EndPath();
		oldPen = _dc->SelectObject(&_penFontOutline);
		_dc->StrokePath();
		_dc->SelectObject(oldPen);
	}

	DrawTextW(_dc->m_hDC, lbl->text, -1, rect, _alignment);
}

// ********************************************************************
//		DrawFrame
// ********************************************************************
void GdiLabelDrawer::DrawFrame(CRect* rect, CLabelOptions* m_options)
{
	switch (m_options->frameType)
	{
		case lfRectangle:
			_dc->Rectangle(rect->left, rect->top, rect->right, rect->bottom);
			break;
		case lfRoundedRectangle:
			// MWGIS-229; start circle outside of left and right boundaries
			_dc->RoundRect(rect->left - (rect->Height() / 2), rect->top, rect->right + (rect->Height() / 2), rect->bottom, rect->Height(), rect->Height());
			break;
		case lfPointedRectangle:
			_dc->BeginPath();
			_dc->MoveTo(rect->left, rect->top);
			_dc->LineTo(rect->right, rect->top);
			_dc->LineTo(rect->right + rect->Height() / 4, (rect->top + rect->bottom) / 2);
			_dc->LineTo(rect->right, rect->bottom);
			_dc->LineTo(rect->left, rect->bottom);
			_dc->LineTo(rect->left - rect->Height() / 4, (rect->top + rect->bottom) / 2);
			_dc->LineTo(rect->left, rect->top);
			_dc->EndPath();
			_dc->StrokeAndFillPath();
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

	// MWGIS-121; rather than return null, enforce a minimum font size of 4
	if (abs(lf.lfHeight) < 4)	// changed 1 to 4; there is no way to read labels smaller than 4, but they slow down the performance
	{
		lf.lfHeight = 4;
	}

	lf.lfItalic = options->fontStyle & fstItalic;
	lf.lfUnderline = options->fontStyle & fstUnderline;
	lf.lfStrikeOut = options->fontStyle & fstStrikeout;
	lf.lfWeight = options->fontStyle & fstBold ? FW_BOLD : 0;

	fnt->DeleteObject();
	fnt->CreateFontIndirectA(&lf);

	return fnt;
}

// *********************************************************************
// 					MeasureString()										
// *********************************************************************
void GdiLabelDrawer::MeasureString(CLabelInfo* lbl, CRect& rect)
{
	DrawTextW(_dc->m_hDC, lbl->text, -1, rect, DT_CALCRECT);

	// frame for GDI is very narrow; so we'll enlarge it a bit					
	rect.left -= rect.Height() / 6;
	rect.right += rect.Height() / 6;
}

// *********************************************************************
// 					SelectFont()										
// *********************************************************************
void GdiLabelDrawer::SelectFont(CLabelOptions* options, CLabelInfo* lbl, double scaleFactor)
{
	CFont* fontNew = NULL;

	if (!_fonts[lbl->fontSize])
	{
		CFont* font = CreateFont(options, lbl->fontSize, scaleFactor); 
		_fonts[lbl->fontSize] = font;
		fontNew = font;
	}
	else {
		fontNew = _fonts[lbl->fontSize];
	}

	CFont* tempFont = _dc->SelectObject(fontNew);
	if (!_oldFont) {
		_oldFont = tempFont;
	}
}

// *********************************************************************
// 					SelectFont()										
// *********************************************************************
void GdiLabelDrawer::SelectFont(CLabelOptions* options, long fontSize, double scaleFactor)
{
	this->_font = CreateFont(options, fontSize, scaleFactor);
	_oldFont = _dc->SelectObject(this->_font);
}

// *********************************************************************
// 					ReleaseFonts()										
// *********************************************************************
void GdiLabelDrawer::ReleaseFonts(bool useVariableFontSize)
{
	if (useVariableFontSize)
	{
		for (int i = 0; i <= MAX_LABEL_SIZE; i++)
		{
			if (_fonts[i])
			{
				CFont * f = _fonts[i];
				f->DeleteObject();
				delete f;
				_fonts[i] = NULL;
			}
		}
	}
	else {
		if (_font)
		{
			_font->DeleteObject();
			delete _font;
			_font = NULL;
		}
	}
}
