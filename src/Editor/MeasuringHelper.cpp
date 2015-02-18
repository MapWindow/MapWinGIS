#include "stdafx.h"
#include "MeasuringHelper.h"

// ************************************************************
//		OnCursorChanged
// ************************************************************
bool MeasuringHelper::OnCursorChanged(IMeasuring* measuring, tkCursorMode newCursor)
{
	if (!measuring) return false;

	VARIANT_BOOL isEmpty;
	measuring->get_IsEmpty(&isEmpty);
	if (!isEmpty)
	{
		VARIANT_BOOL persistent;
		measuring->get_Persistent(&persistent);
		if (newCursor != cmMeasure && !persistent)
		{
			measuring->Clear();
			return true;
		}
	}
	return false;
}