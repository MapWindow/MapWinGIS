// ShapeEditor.h : Declaration of the CShapeEditor
#pragma once
#include "EditorBase.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CShapeEditor
class ATL_NO_VTABLE CShapeEditor :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CShapeEditor, &CLSID_ShapeEditor>,
	public IDispatchImpl<IShapeEditor, &IID_IShapeEditor, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CShapeEditor()
	{
		_pUnkMarshaler = NULL;
		_activeShape = new EditorBase();
		_activeShape->AreaDisplayMode = admMetric;
		_lastErrorCode = tkNO_ERROR;
		_globalCallback = NULL;
		_key = SysAllocString(L"");
		_layerHandle = -1;
		_shapeIndex = -1;
		_visible = true;
		_highlightShapes = lsAllLayers;
		_snapTolerance = 10;
		_snapBehavior = lsAllLayers;
		_snapMode = smVerticesAndLines;
		_state = esNone;
		_mapCallback = NULL;
		_isSubjectShape = false;
		_validationMode = evCheckWithGeos;
		_redrawNeeded = false;
		_layerRedrawNeeded = false;
		_overlayType = eoAddPart;
		_behavior = ebVertexEditor;
		_startingUndoCount = -1;
	}
	~CShapeEditor()
	{
		SysFreeString(_key);
		Clear();
		delete _activeShape;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_EDITSHAPE)

	BEGIN_COM_MAP(CShapeEditor)
		COM_INTERFACE_ENTRY(IShapeEditor)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, _pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &_pUnkMarshaler.p);
		return S_OK;
	}

	void FinalRelease()
	{
		_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> _pUnkMarshaler;
public:
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(Clear)();
	STDMETHOD(get_NumPoints)(long* retVal);
	STDMETHOD(get_PointXY)(long pointIndex, double* x, double* y, VARIANT_BOOL* retVal);
	STDMETHOD(put_PointXY)(long pointIndex, double x, double y, VARIANT_BOOL* retVal);
	STDMETHOD(UndoPoint)(VARIANT_BOOL* retVal);
	STDMETHOD(AddPoint)(IPoint *newPoint, VARIANT_BOOL* retVal);
	STDMETHOD(get_SegmentLength)(int segmentIndex, double* retVal);
	STDMETHOD(get_SegmentAngle)(int segmentIndex, double* retVal);
	STDMETHOD(get_IsDigitizing)(VARIANT_BOOL* retVal);
	STDMETHOD(get_ShapeType)(ShpfileType* retVal);
	STDMETHOD(put_ShapeType)(ShpfileType newVal);
	STDMETHOD(SetShape)(IShape* shp);
	STDMETHOD(get_ValidatedShape)(IShape** retVal);
	STDMETHOD(get_LayerHandle)(LONG* retVal);
	STDMETHOD(put_LayerHandle)(LONG newVal);
	STDMETHOD(get_ShapeIndex)(LONG* retVal);
	STDMETHOD(put_ShapeIndex)(LONG newVal);
	STDMETHOD(get_Area)(double* retVal);
	STDMETHOD(get_ShapeVisible)(VARIANT_BOOL* val);
	STDMETHOD(put_ShapeVisible)(VARIANT_BOOL newVal);
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
	STDMETHOD(get_HighlightVertices)(tkLayerSelection* pVal);
	STDMETHOD(put_HighlightVertices)(tkLayerSelection newVal);
	STDMETHOD(get_SnapBehavior)(tkLayerSelection* pVal);
	STDMETHOD(put_SnapBehavior)(tkLayerSelection newVal);
	STDMETHOD(get_SnapMode)(tkSnapMode* pVal);
	STDMETHOD(put_SnapMode)(tkSnapMode newVal);
	STDMETHOD(get_EditorState)(tkEditorState* pVal);
	STDMETHOD(put_EditorState)(tkEditorState newVal);
	STDMETHOD(StartEdit)(LONG LayerHandle, LONG ShapeIndex, VARIANT_BOOL* retVal);
	STDMETHOD(get_IndicesVisible)(VARIANT_BOOL* pVal);
	STDMETHOD(put_IndicesVisible)(VARIANT_BOOL newVal);
	STDMETHOD(get_AreaDisplayMode)(tkAreaDisplayMode* retVal);
	STDMETHOD(put_AreaDisplayMode)(tkAreaDisplayMode newVal);
	STDMETHOD(get_BearingType)(tkBearingType* retVal);
	STDMETHOD(put_BearingType)(tkBearingType newVal);
	STDMETHOD(get_LengthDisplayMode)(tkLengthDisplayMode* pVal);
	STDMETHOD(put_LengthDisplayMode)(tkLengthDisplayMode newVal);
	STDMETHOD(ClearSubjectShapes)();
	STDMETHOD(get_NumSubjectShapes)(LONG* pVal);
	STDMETHOD(StartUnboundShape)(ShpfileType shpTYpe, VARIANT_BOOL* retVal);
	STDMETHOD(get_VerticesVisible)(VARIANT_BOOL* pVal);
	STDMETHOD(put_VerticesVisible)(VARIANT_BOOL newVal);
	STDMETHOD(get_ValidationMode)(tkEditorValidation* pVal);
	STDMETHOD(put_ValidationMode)(tkEditorValidation newVal);
	STDMETHOD(StartOverlay)(tkEditorOverlay overlayType, VARIANT_BOOL* retVal);
	STDMETHOD(get_EditorBehavior)(tkEditorBehavior* pVal);
	STDMETHOD(put_EditorBehavior)(tkEditorBehavior newVal);
	STDMETHOD(SaveChanges)(VARIANT_BOOL* retVal);
	STDMETHOD(get_HasChanges)(VARIANT_BOOL* pVal);
	STDMETHOD(get_IsUsingEllipsoid)(VARIANT_BOOL* pVal);
	STDMETHOD(get_Length)(DOUBLE* pVal);
	STDMETHOD(get_ShowArea)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ShowArea)(VARIANT_BOOL newVal);
	STDMETHOD(get_AreaPrecision)(LONG* pVal);
	STDMETHOD(put_AreaPrecision)(LONG newVal);
	STDMETHOD(get_LengthPrecision)(LONG* pVal);
	STDMETHOD(put_LengthPrecision)(LONG newVal);
	STDMETHOD(get_AngleFormat)(tkAngleFormat* pVal);
	STDMETHOD(put_AngleFormat)(tkAngleFormat newVal);
	STDMETHOD(get_AnglePrecision)(LONG* pVal);
	STDMETHOD(put_AnglePrecision)(LONG newVal);

private:
	BSTR _key;
	ICallback * _globalCallback;
	VARIANT_BOOL _visible;
	tkLayerSelection _highlightShapes;
	tkSnapMode _snapMode;
	double _snapTolerance;
	tkLayerSelection _snapBehavior;
	EditorBase* _activeShape;
	long _layerHandle;
	long _shapeIndex;
	long _lastErrorCode;
	tkEditorState _state;
	vector<CShapeEditor*> _subjects;
	bool _isSubjectShape;
	tkEditorValidation _validationMode;
	bool _redrawNeeded;
	bool _layerRedrawNeeded;
	tkEditorOverlay _overlayType;
	tkEditorBehavior _behavior;
	long _startingUndoCount;
	
public:
	IMapViewCallback* _mapCallback;

public:
	void ErrorMessage(long ErrorCode);
	void CopyData(int firstIndex, int lastIndex, IShape* target);

	void SetMapCallback(IMapViewCallback* callback) {
		_activeShape->SetMapCallback(callback, ShapeInputMode::simEditing);
		_mapCallback = callback;
	}

	bool GetRedrawNeeded(RedrawTarget target) 
	{ 
		if (target == rtShapeEditor)
			return _redrawNeeded; 
		if (target == rtVolatileLayer)
			return _layerRedrawNeeded;
		return false;
	}
	void SetRedrawNeeded(RedrawTarget target) 
	{ 
		if (target == rtShapeEditor) 
			_redrawNeeded = true;
		if (target == rtVolatileLayer) 
			_layerRedrawNeeded = true; 
	}
	void ClearRedrawFlag()
	{
		_redrawNeeded = false;
		_layerRedrawNeeded = false;
	}

	EditorBase* GetActiveShape() { return _activeShape; }
	void SetIsSubject(bool value) { _isSubjectShape = value; }
	void DiscardState();
	void SaveState();
	void MoveShape(double offsetX, double offsetY);
	void MovePart(double offsetX, double offsetY);
	bool InsertVertex(double xProj, double yProj);
	bool RemoveVertex();
	bool RemovePart();
	bool CheckState();
	void Render(Gdiplus::Graphics* g, bool dynamicBuffer, DraggingOperation offsetType, int screenOffsetX, int screenOffsetY);
	void EndOverlay(IShape* overlayShape);
	IShape* GetLayerShape(long layerHandle, long shapeIndex);
	bool GetClosestPoint(double projX, double projY, double& xResult, double& yResult);
	bool HandleDelete();
	bool RemoveShape();
	int GetClosestPart(double projX, double projY, double tolerance);
	bool RestoreState(IShape* shp, long layerHandle, long shapeIndex);
	bool TryStop();
	void HandleProjPointAdd(double projX, double projY);
	bool HasSubjectShape(int LayerHandle, int ShapeIndex);
	bool ValidateWithGeos(IShape** shp);
	bool Validate(IShape** shp);
	ShpfileType GetShapeTypeForTool(tkCursorMode cursor);
	void ApplyColoringForTool(tkCursorMode mode);
	bool StartUnboundShape(tkCursorMode cursor);
	CShapeEditor* Clone();
	void ApplyOverlayColoring(tkEditorOverlay overlay);
	bool TrySaveShape(IShape* shp);
	IShape* CalculateOverlay(IShape* overlay);
	void CopyOptionsFromShapefile();
	void CancelOverlay(bool restoreSubjectShape);
	bool ClearCore(bool all);
	void DiscardChangesFromUndoList();
	
	// exposing active shape API
	bool SetSelectedVertex(int vertexIndex) { return _activeShape->SetSelectedVertex(vertexIndex); }
	bool SetSelectedPart(int partIndex) { return _activeShape->SetSelectedPart(partIndex); }
	bool HasSelectedVertex() { return _activeShape->HasSelectedVertex(); }
	bool HasSelectedPart() { return _activeShape->HasSelectedPart(); }
	int SelectPart(double xProj, double yProj) { return _activeShape->SelectPart(xProj, yProj); }
	int GetClosestVertex(double projX, double projY, double tolerance) { return _activeShape->GetClosestVertex(projX, projY, tolerance); }
	STDMETHOD(get_ShowBearing)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ShowBearing)(VARIANT_BOOL newVal);
	STDMETHOD(get_ShowLength)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ShowLength)(VARIANT_BOOL newVal);
	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR state, VARIANT_BOOL* retVal);
};
OBJECT_ENTRY_AUTO(__uuidof(ShapeEditor), CShapeEditor)
