// EditShape.h : Declaration of the CEditShape
#pragma once
#include "EditShapeBase.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CEditShape
class ATL_NO_VTABLE CEditShape :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEditShape, &CLSID_EditShape>,
	public IDispatchImpl<IEditShape, &IID_IEditShape, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CEditShape()
	{
		_editShape = new EditShapeBase();
		_lastErrorCode = tkNO_ERROR;
		_globalCallback = NULL;
		USES_CONVERSION;
		_key = A2BSTR("");
		_layerHandle = -1;
		_shapeIndex = -1;
		_visible = true;
		_hotTracking = VARIANT_TRUE;
		_snapTolerance = 10;
		_snapBehavior = sbSnapByDefault;
	}
	~CEditShape()
	{
		delete _editShape;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_EDITSHAPE)

	BEGIN_COM_MAP(CEditShape)
		COM_INTERFACE_ENTRY(IEditShape)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
public:
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	
	STDMETHOD(Clear)();
	STDMETHOD(get_numPoints)(long* retVal);
	STDMETHOD(get_PointXY)(long pointIndex, double* x, double* y, VARIANT_BOOL* retVal);
	STDMETHOD(put_PointXY)(long pointIndex, double x, double y, VARIANT_BOOL* retVal);
	STDMETHOD(Undo)(VARIANT_BOOL* retVal);
	STDMETHOD(Redo)(VARIANT_BOOL* retVal);
	STDMETHOD(get_HasEnoughPoints)(VARIANT_BOOL* retVal);
	STDMETHOD(get_SegmentLength)(int segmentIndex, double* retVal);
	STDMETHOD(get_SegmentAngle)(int segmentIndex, double* retVal);
	STDMETHOD(get_CreationMode)(VARIANT_BOOL* retVal);
	STDMETHOD(put_CreationMode)(VARIANT_BOOL newVal);
	STDMETHOD(get_ShapeType)(ShpfileType* retVal);
	STDMETHOD(put_ShapeType)(ShpfileType newVal);
	STDMETHOD(AddPoint)(double xProj, double yProj);
	STDMETHOD(SetShape)(IShape* shp);
	STDMETHOD(get_Shape)(VARIANT_BOOL fixup, IShape** retVal);
	STDMETHOD(get_LayerHandle)(int* retVal);
	STDMETHOD(put_LayerHandle)(int newVal);
	STDMETHOD(get_ShapeIndex)(int* retVal);
	STDMETHOD(put_ShapeIndex)(int newVal);
	STDMETHOD(get_Area)(double* retVal);
	STDMETHOD(get_Visible)(VARIANT_BOOL* val);
	STDMETHOD(put_Visible)(VARIANT_BOOL newVal);
	STDMETHOD(get_DrawLabelsOnly)(VARIANT_BOOL* val);
	STDMETHOD(put_DrawLabelsOnly)(VARIANT_BOOL newVal);
	STDMETHOD(get_SelectedVertex)(int* val);
	STDMETHOD(put_SelectedVertex)(int newVal);
	STDMETHOD(get_RawData)(IShape** pVal);
	STDMETHOD(get_FillColor)(OLE_COLOR* pVal);
	STDMETHOD(put_FillColor)(OLE_COLOR newVal);
	STDMETHOD(get_FillTransparency)(BYTE* pVal);
	STDMETHOD(put_FillTransparency)(BYTE newVal);
	STDMETHOD(get_LineColor)(OLE_COLOR* pVal);
	STDMETHOD(put_LineColor)(OLE_COLOR newVal);
	STDMETHOD(get_LineWidth)(FLOAT* pVal);
	STDMETHOD(put_LineWidth)(FLOAT newVal);
	STDMETHOD(CopyOptionsFrom)(IShapeDrawingOptions* options);
	STDMETHOD(get_IsEmpty)(VARIANT_BOOL* pVal);
	STDMETHOD(get_SnapTolerance)(DOUBLE* pVal);
	STDMETHOD(put_SnapTolerance)(DOUBLE newVal);
	STDMETHOD(get_HotTracking)(VARIANT_BOOL* pVal);
	STDMETHOD(put_HotTracking)(VARIANT_BOOL newVal);
	STDMETHOD(get_SnapBehavior)(tkSnapBehavior* pVal);
	STDMETHOD(put_SnapBehavior)(tkSnapBehavior newVal);

	// TODO: perhaps remove
	STDMETHOD(get_AreaDisplayMode)(tkAreaDisplayMode* retVal);
	STDMETHOD(put_AreaDisplayMode)(tkAreaDisplayMode newVal);
	STDMETHOD(get_AngleDisplayMode)(tkAngleDisplay* retVal);
	STDMETHOD(put_AngleDisplayMode)(tkAngleDisplay newVal);
private:
	
	BSTR _key;
	ICallback * _globalCallback;
	VARIANT_BOOL _visible;
	VARIANT_BOOL _hotTracking;
	double _snapTolerance;
	tkSnapBehavior _snapBehavior;
	EditShapeBase* _editShape;
	int _layerHandle;
	int _shapeIndex;
	long _lastErrorCode;
	vector<IShape*> _undoList;

	void ErrorMessage(long ErrorCode);
	void CopyData(int firstIndex, int lastIndex, IShape* target );
	
public:
	void DiscardState();
	void SaveState();
	EditShapeBase* GetBase() { return _editShape; }
	void MoveVertex(double offsetX, double offsetY, bool offset = true);
	void MoveShape(double offsetX, double offsetY);
	bool InsertVertex(double xProj, double yProj);
	bool RemoveVertex();
	vector<IShape*> GetUndoList() { return _undoList; }

	
};

OBJECT_ENTRY_AUTO(__uuidof(EditShape), CEditShape)
