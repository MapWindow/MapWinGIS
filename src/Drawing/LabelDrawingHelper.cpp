#include "stdafx.h"
#include "LabelDrawingHelper.h"


// ********************************************************************
//		CalcRotation
// ********************************************************************
void LabelDrawingHelper::CalcRotation(CLabelInfo* lbl, double mapRotation, double& angle)
{
	if (!lbl) return;

	if (lbl->rotation == 0.0 && mapRotation != 0.0)
	{
		angle = long(mapRotation) % 360;
	}
	else if (lbl->rotation != 0.0 && mapRotation != 0.0)
	{
		if (lbl->rotation > 0)
		{
			angle = long(lbl->rotation - mapRotation) % 360;
			if (angle > 90 && angle < 180 || angle < -90 && angle >= -180)
				angle = long(lbl->rotation) % 360 + 180;
			else if (angle >= 180.0 && angle < 270.0 || angle < -180.0 && angle >= -270.0)
				angle = long(lbl->rotation) % 360 - 180;
			else
				angle = long(lbl->rotation) % 360;
		}
		else
		{
			angle = long(mapRotation - lbl->rotation) % 360;
			if (angle > 0.0 && angle < 90.0)
				angle = long(lbl->rotation) % 360;
			else if (angle > 90.0 && angle < 180.0)
				angle = long(lbl->rotation) % 360 + 180.0;
			else if (angle >= 180.0 && angle < 270.0)
				angle = long(lbl->rotation) % 360 - 180.0;
			else
				angle = long(lbl->rotation) % 360;
		}
	}
	else
	{
		// we don't want our labels to be upside-down
		angle = long(lbl->rotation) % 360;
		if (angle > 90.0 && angle < 180)			angle += 180.0;
		else if (angle >= 180.0 && angle < 270.0)	angle -= 180.0;
	}
}

// ********************************************************************
//		AlignRectangle
// ********************************************************************
// rectangle for text printing is aligned around (0,0) with proper rotation
void LabelDrawingHelper::AlignRectangle(CRect& rect, tkLabelAlignment alignment)
{
	// width and height, projected on the X and Y axes
	switch (alignment)
	{
		case laTopLeft:
			rect.MoveToXY(-rect.Width(), -rect.Height());
			break;
		case laTopCenter:
			rect.MoveToXY(-rect.Width() / 2, -rect.Height());
			break;
		case laTopRight:
			rect.MoveToXY(0, -rect.Height());
			break;
		case laCenterLeft:
			rect.MoveToXY(-rect.Width(), -rect.Height() / 2);
			break;
		case laCenter:
			rect.MoveToXY(-rect.Width() / 2, -rect.Height() / 2);
			break;
		case laCenterRight:
			rect.MoveToXY(0, -rect.Height() / 2);
			break;
		case laBottomLeft:
			rect.MoveToXY(-rect.Width(), 0);
			break;
		case laBottomCenter:
			rect.MoveToXY(-rect.Width() / 2, 0);
			break;
		case laBottomRight:
			rect.MoveToXY(0, 0);
			break;

	}
	return;
}

// ********************************************************************
//		UpdateAutoOffset
// ********************************************************************
void LabelDrawingHelper::UpdateAutoOffset(CRect& rect, tkLabelAlignment align, int offset)
{
	if (align == laTopRight ||
		align == laBottomRight ||
		align == laCenterRight)
	{
		rect.OffsetRect(offset, 0);
	}
	else if (align == laTopLeft ||
		align == laBottomLeft ||
		align == laCenterLeft)
	{
		rect.OffsetRect(-offset, 0);
	}
	else if (align == laTopCenter)
	{
		rect.OffsetRect(0, -offset);
	}
	else if (align == laBottomCenter)
	{
		rect.OffsetRect(0, offset);
	}
}
