#pragma once
class LabelsHelper
{
public:
	static void UpdateLabelsPositioning(IShapefile* sf);
	static CString GetFloatNumberFormat(ILabels* labels);
	static long GetCount(ILabels* labels);
};

