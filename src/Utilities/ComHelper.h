#pragma once
class ComHelper
{
public:
	static HRESULT CreateInstance(tkInterface interfaceId, IDispatch** retVal);
	static bool SetRef(IDispatch* newVal, IDispatch** oldVal, bool allowNull = true);
	static CString GetInterfaceName(tkInterface id);
	static void CreatePoint(IPoint** point);
	static void CreateShape(IShape** shp);
	static void CreateExtents(IExtents** box);

#if DEBUG_ALLOCATED_OBJECTS
	static bool GetBreak()
	{
		return _break;
	}

	static void SetBreak(bool breakValue)
	{
		_break = breakValue;
	}
#endif
};

