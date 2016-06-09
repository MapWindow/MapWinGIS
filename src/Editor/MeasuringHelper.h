#pragma once
#include "ActiveShape.h"

class MeasuringHelper
{
public:
	static bool OnCursorChanged(IMeasuring* measuring, tkCursorMode newCursor);
	static CPLXMLNode* Serialize(ActiveShape* activeShape, CString elementName);
	static bool Deserialize(ActiveShape* activeShape, CPLXMLNode* node);
};

