// ShapeEditor.cpp : Implementation of CShapeEditor

#include "stdafx.h"
#include "ShapeEditor.h"
#include "Shape.h"
#include "map.h"
#include "UndoList.h"
#include <set>

// *******************************************************
//		GetShape
// *******************************************************
IShape* CShapeEditor::GetShape(long layerHandle, long shapeIndex)
{
	if (!CheckState()) return NULL;
	CComPtr<IShapefile> sf = _mapCallback->_GetShapefile(layerHandle);
	if (sf)
	{
		IShape* shp = NULL;
		sf->get_Shape(shapeIndex, &shp);
		return shp;
	}
	return NULL;
}

//***********************************************************************/
//*			CheckState()
//***********************************************************************/
bool CShapeEditor::CheckState()
{
	if (!_mapCallback) {
		ErrorMessage(tkSHAPE_EDITOR_NO_MAP);
	}
	return _mapCallback != NULL;
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void CShapeEditor::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	USES_CONVERSION;
	if( _globalCallback != NULL && _lastErrorCode != tkNO_ERROR)
		_globalCallback->Error( OLE2BSTR(_key),  A2BSTR(ErrorMsg(_lastErrorCode)));
}


// *****************************************************************
//	   get_ErrorMsg()
// *****************************************************************
STDMETHODIMP CShapeEditor::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// ************************************************************
//		get_LastErrorCode()
// ************************************************************
STDMETHODIMP CShapeEditor::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// ************************************************************
//		get_GlobalCallback()
// ************************************************************
STDMETHODIMP CShapeEditor::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*pVal = _globalCallback;
	if( _globalCallback != NULL )
		_globalCallback->AddRef();
	return S_OK;
}

// ************************************************************
//		put_GlobalCallback()
// ************************************************************
STDMETHODIMP CShapeEditor::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		Utility::put_ComReference(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}


// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CShapeEditor::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		::SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	return S_OK;
}

// *******************************************************
//		ShapeType()
// *******************************************************
STDMETHODIMP CShapeEditor::get_ShapeType(ShpfileType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _activeShape->GetShapeType();
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_ShapeType(ShpfileType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->SetShapeType(newVal);
	return S_OK;
}

// *******************************************************
//		RawData()
// *******************************************************
STDMETHODIMP CShapeEditor::get_RawData(IShape** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = NULL;

	ShpfileType shpType = _activeShape->GetShapeType();
	if (shpType == SHP_NULLSHAPE) return S_OK;

	GetUtils()->CreateInstance(idShape, (IDispatch**)retVal);

	VARIANT_BOOL vb;
	(*retVal)->Create(shpType, &vb);

	CopyData(0, _activeShape->GetPointCount(), *retVal);

	return S_OK;
}

// *******************************************************
//		AsShape()
// *******************************************************
STDMETHODIMP CShapeEditor::get_Shape(VARIANT_BOOL geosFixup, IShape** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = NULL;

	VARIANT_BOOL vb;
	get_HasEnoughPoints(&vb);
	if (!vb) {
		return S_OK;
	}

	get_RawData(retVal);
	ShpfileType shpType = _activeShape->GetShapeType();

	// add the last point automatically
	if (shpType == SHP_POLYGON)
		((CShape*)(*retVal))->FixupShapeCore(ShapeValidityCheck::FirstAndLastPointOfPartMatch);

	if (geosFixup)
	{
		(*retVal)->get_IsValid(&vb);
		IShape* shpNew = NULL;
		if (!vb) {
			(*retVal)->FixUp(&shpNew);
			if (shpNew)
			{
				(*retVal)->Release();
				(*retVal) = shpNew;
			}
		}
	}
	return S_OK;
}

// *******************************************************
//		HasValidShape()
// *******************************************************
STDMETHODIMP CShapeEditor::get_HasEnoughPoints(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	ShpfileType type = Utility::ShapeTypeConvert2D(_activeShape->GetShapeType());
	if (type == SHP_NULLSHAPE)
	{
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return S_FALSE;
	}

	int pointCount = _activeShape->GetPointCount();
	if (pointCount == 0 ||
		(pointCount < 2 && type == SHP_POLYLINE) ||
		(pointCount < 3 && type == SHP_POLYGON))
	{
		ErrorMessage(tkNOT_ENOUGH_POINTS_FOR_SHAPE_TYPE);
		return S_FALSE;
	}

	*retVal = VARIANT_TRUE;
	return S_OK;
}

// *******************************************************
//		CopyData()
// *******************************************************
void CShapeEditor::CopyData(int firstIndex, int lastIndex, IShape* target )
{
	long index, partCount = 0;
	VARIANT_BOOL vb;
	for(int i = firstIndex; i < lastIndex; i++)
	{
		MeasurePoint* pnt = _activeShape->GetPoint(i);
		if (pnt) {
			target->AddPoint(pnt->Proj.x, pnt->Proj.y, &index);
			if (pnt->Part == PartBegin) {
				target->InsertPart(i, &partCount, &vb);
				partCount++;
			}
		}
	}
}

// *******************************************************
//		get_PointXY()
// *******************************************************
STDMETHODIMP CShapeEditor::get_PointXY(long pointIndex, double* x, double* y, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (pointIndex < 0 || pointIndex >= (long)_activeShape->GetPointCount())
	{
		ErrorMsg(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		MeasurePoint* pnt = _activeShape->GetPoint(pointIndex);
		if (pnt)
		{
			*x = pnt->Proj.x;
			*y = pnt->Proj.y;
			*retVal = VARIANT_TRUE;
		}
	}
	return S_OK;
}

// *******************************************************
//		put_PointXY()
// *******************************************************
STDMETHODIMP CShapeEditor::put_PointXY(long pointIndex, double x, double y, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (pointIndex < 0 || pointIndex >= (long)_activeShape->GetPointCount())
	{
		ErrorMsg(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		_activeShape->UpdatePoint(pointIndex, x, y);
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// *******************************************************
//		get_PointCount()
// *******************************************************
STDMETHODIMP CShapeEditor::get_numPoints(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _activeShape->GetPointCount();
	return S_OK;
}

// *******************************************************
//		Clear()
// *******************************************************
STDMETHODIMP CShapeEditor::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!CheckState()) return S_OK;
	
	if (_state == EditorEdit) {
		CComPtr<IShapefile> sf = _mapCallback->_GetShapefile(_layerHandle);
		if (sf) {
			if (!_isSubjectShape) {
				sf->put_ShapeIsHidden(_shapeIndex, VARIANT_FALSE);
			}
		}
	}

	_activeShape->Clear();
	
	ClearSubjectShapes();
	
	_shapeIndex = -1;
	_layerHandle = -1;
	_state = EditorEmpty;
	return S_OK;
}

// *******************************************************
//		StartEdit
// *******************************************************
STDMETHODIMP CShapeEditor::StartEdit(LONG LayerHandle, LONG ShapeIndex, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;

	if (LayerHandle == -1 || ShapeIndex == -1) {
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	Clear();
	
	CComPtr<IShapefile> sf = _mapCallback->_GetShapefile(LayerHandle);
	if (sf)
	{
		CComPtr<IShape> shp = NULL;
		sf->get_Shape(ShapeIndex, &shp);
		if (shp)
		{
			put_EditorState(EditorEdit);
			SetShape(shp);
			_layerHandle = LayerHandle;
			_shapeIndex = ShapeIndex;
			if (!_isSubjectShape)
				sf->put_ShapeIsHidden(ShapeIndex, VARIANT_TRUE);

			CComPtr<IShapeDrawingOptions> options = NULL;
			sf->get_DefaultDrawingOptions(&options);
			CopyOptionsFrom(options);
			*retVal = VARIANT_TRUE;
		}
	}

	return S_OK;
}

// *******************************************************
//		AddSubjectShape
// *******************************************************
STDMETHODIMP CShapeEditor::AddSubjectShape(LONG LayerHandle, LONG ShapeIndex, VARIANT_BOOL ClearExisting, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;

	if (LayerHandle == -1 || ShapeIndex == -1) {
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	if (_layerHandle == LayerHandle && _shapeIndex == ShapeIndex) {
		// TODO: report error
		return S_OK;
	}

	// check, maybe it's already there
	for (size_t i = 0; i < _subjects.size(); i++) 
	{
		int handle, index;
		_subjects[i]->get_LayerHandle(&handle);
		_subjects[i]->get_ShapeIndex(&index);
		if (handle == LayerHandle && index == ShapeIndex) {
			*retVal = VARIANT_TRUE;
			return S_OK;
		}
	}

	if (ClearExisting) {
		ClearSubjectShapes();
	}

	CComPtr<IShapefile> sf = _mapCallback->_GetShapefile(LayerHandle);
	if (sf)
	{
		CComPtr<IShape> shp = NULL;
		sf->get_Shape(ShapeIndex, &shp);
		if (shp)
		{
			CShapeEditor* editor = NULL;
			GetUtils()->CreateInstance(idShapeEditor, (IDispatch**)&editor);
			editor->SetIsSubject(true);
			editor->SetMapCallback(_mapCallback);
			editor->StartEdit(LayerHandle, ShapeIndex, retVal);
			_subjects.push_back(editor);
		}
	}
	return S_OK;
}

// ***************************************************************
//		SetShape()
// ***************************************************************
STDMETHODIMP CShapeEditor::SetShape( IShape* shp )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!shp) {
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}

	_activeShape->Clear();

	ShpfileType shpType;
	shp->get_ShapeType(&shpType);
	_activeShape->SetShapeType(shpType);
	_activeShape->SetCreationMode(false);

	long numPoints, numParts;
	shp->get_NumParts(&numParts);
	std::set<long> parts, endParts;
	for (long j = 0; j < numParts; j++) {
		long part;
		shp->get_Part(j, &part);
		parts.insert(part);
		shp->get_EndOfPart(j, &part);
		endParts.insert(part);
	}

	VARIANT_BOOL vb;
	double x, y;
	shp->get_NumPoints(&numPoints);
	
	for(long i = 0; i < numPoints; i++) 
	{
		PointPart part = PartNone;
		if (parts.find(i) != parts.end()) part = PartBegin;
		if (endParts.find(i) != endParts.end()) part = PartEnd;

		shp->get_XY(i, &x, &y, &vb);
		_activeShape->AddPoint(x, y, -1, -1, part);
	}
	return S_OK;
}

// *******************************************************
//		SegmentLength()
// *******************************************************
STDMETHODIMP CShapeEditor::get_SegmentLength(int segmentIndex, double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long errorCode = tkNO_ERROR;
	*retVal = _activeShape->GetSegmentLength(segmentIndex, errorCode);
	return S_OK;
}

// *******************************************************
//		SegmentAngle()
// *******************************************************
STDMETHODIMP CShapeEditor::get_SegmentAngle(int segmentIndex, double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long errorCode = tkNO_ERROR;
	*retVal = _activeShape->GetSegmentAngle(segmentIndex, errorCode);
	return S_OK;
}

// *******************************************************
//		AreaDisplayMode()
// *******************************************************
STDMETHODIMP CShapeEditor::get_AreaDisplayMode(tkAreaDisplayMode* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _activeShape->AreaDisplayMode;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_AreaDisplayMode(tkAreaDisplayMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->AreaDisplayMode = newVal;
	return S_OK;
}

// *******************************************************
//		AngleDisplayMode()
// *******************************************************
STDMETHODIMP CShapeEditor::get_AngleDisplayMode(tkAngleDisplay* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _activeShape->AngleDisplayMode;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_AngleDisplayMode(tkAngleDisplay newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->AngleDisplayMode = newVal;
	return S_OK;
}

// *******************************************************
//		CreationMode()
// *******************************************************
STDMETHODIMP CShapeEditor::get_CreationMode(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _activeShape->GetCreationMode() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_CreationMode(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->SetCreationMode(newVal ? true: false);
	return S_OK;
}

// *******************************************************
//		get_Area()
// *******************************************************
STDMETHODIMP CShapeEditor::get_Area(double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _activeShape->GetArea(false, 0.0, 0.0);
	return S_OK;
}

// *******************************************************
//		LayerHandle()
// *******************************************************
STDMETHODIMP CShapeEditor::get_LayerHandle(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _layerHandle;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_LayerHandle(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_layerHandle = newVal;
	return S_OK;
}

// *******************************************************
//		ShapeIndex()
// *******************************************************
STDMETHODIMP CShapeEditor::get_ShapeIndex(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _shapeIndex;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_ShapeIndex(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_shapeIndex = newVal;
	return S_OK;
}

// *******************************************************
//		Visible()
// *******************************************************
STDMETHODIMP CShapeEditor::get_Visible(VARIANT_BOOL* val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//*val = _activeShape->_geometryVisible;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_Visible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//_activeShape->_geometryVisible = newVal ? true: false;
	return S_OK;
}

// *******************************************************
//		LabelsOnly()
// *******************************************************
STDMETHODIMP CShapeEditor::get_DrawLabelsOnly(VARIANT_BOOL* val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*val = _activeShape->_drawLabelsOnly;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_DrawLabelsOnly(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->_drawLabelsOnly = newVal ? true : false;
	return S_OK;
}

// *******************************************************
//		SelectedVertex()
// *******************************************************
STDMETHODIMP CShapeEditor::get_SelectedVertex(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _activeShape->_selectedVertex;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_SelectedVertex(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->SetSelectedVertex(newVal);
	return S_OK;
}

// *******************************************************
//		CopyOptionsFrom()
// *******************************************************
STDMETHODIMP CShapeEditor::CopyOptionsFrom(IShapeDrawingOptions* options)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	float lineWidth, tranparency;
	OLE_COLOR fillColor, lineColor;
	options->get_FillColor(&fillColor);
	options->get_LineColor(&lineColor);
	options->get_LineWidth(&lineWidth);
	options->get_FillTransparency(&tranparency);

	_activeShape->FillColor = fillColor;
	_activeShape->LineColor = lineColor;
	_activeShape->LineWidth = lineWidth;
	//_editShape->FillTransparency = (BYTE)tranparency;
	return S_OK;
}

// *******************************************************
//		FillColor()
// *******************************************************
STDMETHODIMP CShapeEditor::get_FillColor(OLE_COLOR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->FillColor;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_FillColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->FillColor = newVal;
	return S_OK;
}

// *******************************************************
//		FillTransparency()
// *******************************************************
STDMETHODIMP CShapeEditor::get_FillTransparency(BYTE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->FillTransparency;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_FillTransparency(BYTE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->FillTransparency = newVal;
	return S_OK;
}

// *******************************************************
//		LineColor()
// *******************************************************
STDMETHODIMP CShapeEditor::get_LineColor(OLE_COLOR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->LineColor;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_LineColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->LineColor = newVal;
	return S_OK;
}

// *******************************************************
//		LineWidth()
// *******************************************************
STDMETHODIMP CShapeEditor::get_LineWidth(FLOAT* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->LineWidth;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_LineWidth(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->LineWidth = newVal;
	return S_OK;
}

// *******************************************************
//		MoveShape()
// *******************************************************
void CShapeEditor::MoveShape(double offsetX, double offsetY)
{
	SaveState();
	_activeShape->Move(offsetX, offsetY);
}

// *******************************************************
//		MovePart()
// *******************************************************
void CShapeEditor::MovePart(double offsetX, double offsetY)
{
	SaveState();
	_activeShape->MovePart(offsetX, offsetY);
}

// *******************************************************
//		InsertVertex()
// *******************************************************
bool CShapeEditor::InsertVertex(double xProj, double yProj)
{
	SaveState();
	if (_activeShape->TryInsertVertex(xProj, yProj)) return true;
	DiscardState();
	return false;
}

// *******************************************************
//		RemoveVertex()
// *******************************************************
bool CShapeEditor::RemoveVertex()
{
	SaveState();
	if (_activeShape->RemoveSelectedVertex()) return true;
	DiscardState();
	return false;
}

// *******************************************************
//		RemovePart()
// *******************************************************
bool CShapeEditor::RemovePart()
{
	SaveState();
	if (_activeShape->RemovePart()) return true;
	DiscardState();
	return false;
}

// *******************************************************
//		SaveState()
// *******************************************************
void CShapeEditor::SaveState()
{
	IUndoList* undoList = _mapCallback->_GetUndoList();
	if (undoList) 
	{
		VARIANT_BOOL vb;
		undoList->Add(uoEditShape, _layerHandle, _shapeIndex, &vb);
	}
}

// *******************************************************
//		DiscardState()
// *******************************************************
void CShapeEditor::DiscardState()
{
	IUndoList* undoList = _mapCallback->_GetUndoList();
	if (undoList)
	{
		((CUndoList*)undoList)->DiscardOne();
	}
}

// *******************************************************
//		Undo()
// *******************************************************
STDMETHODIMP CShapeEditor::Undo(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	if (_state == EditorEmpty)
		return VARIANT_FALSE;

	if (_activeShape->GetCreationMode()) {
		*retVal = _activeShape->UndoPoint() ? VARIANT_TRUE : VARIANT_FALSE;
	}
	return S_OK;
}

// *******************************************************
//		Redo()
// *******************************************************
STDMETHODIMP CShapeEditor::Redo(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_activeShape->GetCreationMode()) {
		// TODO: consider whether to implement this
	}
	else {
		// TODO: implement
	}
	*retVal = VARIANT_FALSE;
	return S_OK;
}

// *******************************************************
//		get_IsEmpty()
// *******************************************************
STDMETHODIMP CShapeEditor::get_IsEmpty(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->IsEmpty() && _subjects.size() == 0 ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *******************************************************
//		SnapTolerance()
// *******************************************************
STDMETHODIMP CShapeEditor::get_SnapTolerance(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _snapTolerance;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_SnapTolerance(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal >= 0.0 && newVal <= 100.0) {
		_snapTolerance = newVal;
	}
	else {
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	return S_OK;
}

// *******************************************************
//		get_HotTracking()
// *******************************************************
STDMETHODIMP CShapeEditor::get_HotTracking(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _hotTracking;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_HotTracking(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_hotTracking = newVal;
	return S_OK;
}

// *******************************************************
//		get_SnapBehavior()
// *******************************************************
STDMETHODIMP CShapeEditor::get_SnapBehavior(tkSnapBehavior* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _snapBehavior;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_SnapBehavior(tkSnapBehavior newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_snapBehavior = newVal;
	return S_OK;
}

// *******************************************************
//		EditorState
// *******************************************************
STDMETHODIMP CShapeEditor::get_EditorState(tkShapeEditorState* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _state;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_EditorState(tkShapeEditorState newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->SetCreationMode(newVal == EditorCreation || newVal == EditorCreationUnbound);
	_state = newVal;
	return S_OK;
}

// *******************************************************
//		Render
// *******************************************************
void CShapeEditor::Render(Gdiplus::Graphics* g, bool dynamicBuffer, DraggingOperation offsetType, int screenOffsetX, int screenOffsetY)
{
	for (size_t i = 0; i < _subjects.size(); i++) {
		EditorBase* base = _subjects[i]->GetActiveShape();
		base->DrawData(g, dynamicBuffer, DragNone);
	}
	_activeShape->DrawData(g, dynamicBuffer, offsetType, screenOffsetX, screenOffsetY);
}

// *******************************************************
//		PointLabelsVisible
// *******************************************************
STDMETHODIMP CShapeEditor::get_PointLabelsVisible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->_pointLabelsVisible ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_PointLabelsVisible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->_pointLabelsVisible = newVal ? true: false;
	return S_OK;
}

// *******************************************************
//		LengthDisplayMode
// *******************************************************
STDMETHODIMP CShapeEditor::get_LengthDisplayMode(tkLengthDisplayMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->_lengthDisplayMode;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_LengthDisplayMode(tkLengthDisplayMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->_lengthDisplayMode = newVal;
	return S_OK;
}

// *******************************************************
//		ClearSubjectShapes
// *******************************************************
STDMETHODIMP CShapeEditor::ClearSubjectShapes()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	for (size_t i = 0; i < _subjects.size(); i++) {
		_subjects[i]->Clear();
		_subjects[i]->Release();
	}
	_subjects.clear();
	return S_OK;
}

// *******************************************************
//		NumSubjectShapes
// *******************************************************
STDMETHODIMP CShapeEditor::get_NumSubjectShapes(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = (LONG)_subjects.size();
	return S_OK;
}

// *******************************************************
//		StartUnboundShape
// *******************************************************
STDMETHODIMP CShapeEditor::StartUnboundShape(ShpfileType shpType, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	if (shpType == SHP_NULLSHAPE ) 
	{
		if (_subjects.size() > 0) {
			_subjects[0]->get_ShapeType(&shpType);
			OLE_COLOR line;
			_subjects[0]->get_LineColor(&line);
			put_LineColor(line);
			//put_PointLabelsVisible(VARIANT_FALSE);
			//put_LengthDisplayMode(ldmNone);
			//put_VerticesVisible(VARIANT_FALSE);
		}
		else {
			ErrorMessage(tkINVALID_PARAMETER_VALUE);
			return S_OK;
		}
	}
	
	put_ShapeType(shpType);
	put_EditorState(EditorCreationUnbound);
	return S_OK;
}

// *******************************************************
//		VerticesVisible
// *******************************************************
STDMETHODIMP CShapeEditor::get_VerticesVisible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->_verticesVisible ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_VerticesVisible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->_verticesVisible = newVal ? true: false;
	return S_OK;
}

// *******************************************************
//		ApplyOperation
// *******************************************************
IShape* CShapeEditor::ApplyOperation(SubjectOperation operation, int& layerHandle, int& shapeIndex)
{
	if (_subjects.size() != 1) {
		// TODO: report error
		return NULL;
	}

	VARIANT_BOOL vb;
	get_HasEnoughPoints(&vb);
	if (!vb) {
		// TODO: report error
		return NULL;
	}

	_subjects[0]->get_LayerHandle(&layerHandle);
	_subjects[0]->get_ShapeIndex(&shapeIndex);
	CComPtr<IShape> subject = GetShape(layerHandle, shapeIndex);
	
	CComPtr<IShape> overlay = NULL;
	get_Shape(VARIANT_FALSE, &overlay);
	
	if (!subject || !overlay) {
		return NULL;
	}

	IShape* result = NULL;
	switch (operation)
	{
		case SubjectAddPart:
			subject->Clip(overlay, tkClipOperation::clUnion, &result);
			break;
		case SubjectClip:
			subject->Clip(overlay, tkClipOperation::clDifference, &result);
			break;
		case SubjectSplit:
			subject->Clip(overlay, tkClipOperation::clDifference, &result);
			break;
	}
	return result;
}

// *******************************************************
//		GetClosestPoint
// *******************************************************
bool CShapeEditor::GetClosestPoint(double projX, double projY, double& xResult, double& yResult)
{
	ShpfileType shpType = Utility::ShapeTypeConvert2D(_activeShape->GetShapeType());

	if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT)
		return false;				// TODO: multi points should be supported

	CComPtr<IShape> shp = NULL;
	get_RawData(&shp);
	if (shp)
	{
		// we need ClosestPoints method to return points on contour, and not inner points		
		if (shpType == SHP_POLYGON)
			shp->put_ShapeType(SHP_POLYLINE);

		// creating temp shape
		VARIANT_BOOL vb;
		long pointIndex = 0;
		CComPtr<IShape> shp2 = NULL;
		GetUtils()->CreateInstance(idShape, (IDispatch**)&shp2);
		shp2->Create(SHP_POINT, &vb);
		shp2->AddPoint(projX, projY, &pointIndex);

		CComPtr<IShape> result = NULL;
		shp->ClosestPoints(shp2, &result);
		if (result)
		{
			result->get_XY(0, &xResult, &yResult, &vb);		// 0 = point lying on the line
			return vb ? true : false;
		}
	}
	return false;
}

// *******************************************************
//		GetClosestPart
// *******************************************************
int CShapeEditor::GetClosestPart(double projX, double projY, double tolerance)
{
	double x, y;
	if (GetClosestPoint(projX, projY, x, y)) {
		double dist = sqrt(pow(x - projX, 2.0) + pow(y - projY, 2.0));
		if (dist < tolerance) {
			return _activeShape->SelectPart(x, y);
		}
	}
	return -1;
}

// *******************************************************
//		GetClosestPoint
// *******************************************************
bool CShapeEditor::HandleDelete()
{
	if (!CheckState()) return false;

	tkDeleteTarget target = _activeShape->GetDeleteTarget();

	if (target == dtNone) return false;

	tkMwBoolean cancel = blnFalse;
	_mapCallback->_FireBeforeDeleteShape(target, &cancel);
	if (cancel == blnTrue) return false;

	switch (target){
		case  dtShape:
			if (RemoveShape())	{
				Clear();
				return true;
			}
		case dtPart:
			return RemovePart();
		case dtVertex:
			return RemoveVertex();
	}
	return false;
}

// ************************************************************
//		RemoveShape
// ************************************************************
bool CShapeEditor::RemoveShape()
{
	if (!CheckState()) return false;
	CComPtr<IShapefile> sf = _mapCallback->_GetShapefile(_layerHandle);
	if (sf) 
	{
		tkMwBoolean cancel = blnFalse;
		if (cancel == blnTrue) return false;

		VARIANT_BOOL vb;
		IUndoList* undoList = _mapCallback->_GetUndoList();
		if (undoList != NULL) 
		{
			undoList->Add(uoRemoveShape, _layerHandle, _shapeIndex, &vb);
			if (vb) 
			{
				sf->EditDeleteShape(_shapeIndex, &vb);
				//FireAfterShapeEdit(uoRemoveShape, layerHandle, shapeIndex);			// TODO: restore
				return true;
			}
		}
	}
	return false;
}

// ************************************************************
//		TrySave
// ************************************************************
bool CShapeEditor::TrySave()
{
	if (!CheckState()) return false;

	bool newShape = _shapeIndex == -1;

	// first check if it's enough points
	VARIANT_BOOL enoughPoints;
	get_HasEnoughPoints(&enoughPoints);
	if (!enoughPoints) {
		_mapCallback->_FireValidationResults(VARIANT_FALSE, "The shape doesn't have enough points");		// TODO: localize
		return false;
	}

	tkCursorMode cursorMode = _mapCallback->_GetCursorMode();
	bool subjectOperation = cursorMode == cmAddPart || cursorMode == cmRemovePart;

	int layerHandle = _layerHandle, shapeIndex = _shapeIndex;
	CComPtr<IShape> shp = NULL;
	if (subjectOperation) {
		SubjectOperation op = cursorMode == cmAddPart ? SubjectAddPart : SubjectClip;
		shp = ApplyOperation(op, layerHandle, shapeIndex);
		newShape = false;
	}
	else {
		get_Shape(VARIANT_FALSE, &shp);
	}

	// does user want to validate with GEOS?
	tkMwBoolean geosCheck = blnTrue, tryFix = blnTrue;
	_mapCallback->_FireValidationMode(&geosCheck, &tryFix);

	// if so validate and optionally fix
	VARIANT_BOOL valid = VARIANT_TRUE;
	if (geosCheck == blnTrue)
	{
		shp->get_IsValid(&valid);
		if (!valid && tryFix == blnTrue)
		{
			IShape* shpNew = NULL;
			shp->FixUp(&shpNew);
			if (shpNew) {
				shp = NULL;				// TODO: will it release the shape?
				shp = shpNew;
				valid = true;
			}
		}

		// report results back to user
		CComBSTR reason;
		if (!valid) {
			shp->get_IsValidReason(&reason);
		}
		else {
			reason = ::SysAllocString(L"");
		}
		USES_CONVERSION;
		_mapCallback->_FireValidationResults(valid, OLE2A(reason));
	}

	if (!valid) {
		return false;
	}

	// now let the user check custom validation rules
	if (valid) {
		tkMwBoolean cancel = blnFalse;
		_mapCallback->_FireValidateShape(cursorMode, layerHandle, shp, &cancel);
		if (cancel == blnTrue) {
			return false;
		}
	}

	//finally ready to save	
	CComPtr<IShapefile> sf = NULL;
	sf = _mapCallback->_GetShapefile(layerHandle);

	if (!sf) {
		// TODO: check it earlier on
		return false;
	}

	long numShapes;
	sf->get_NumShapes(&numShapes);

	IUndoList* undoList = _mapCallback->_GetUndoList();

	VARIANT_BOOL vb;
	if (newShape) {
		undoList->Add(uoAddShape, (long)layerHandle, (long)numShapes, &vb);
	}

	if (subjectOperation) {
		undoList->Add(uoEditShape, (long)layerHandle, (long)shapeIndex, &vb);
	}

	// add new shape
	if (newShape) {
		sf->EditInsertShape(shp, &numShapes, &vb);
		shapeIndex = numShapes;
	}
	else {
		sf->EditUpdateShape(shapeIndex, shp, &vb);
	}

	Clear();

	// let the user set new attributes
	_mapCallback->_FireAfterShapeEdit(uoEditShape, layerHandle, shapeIndex);
	return true;
}

// ************************************************************
//		RestoreState
// ************************************************************
bool CShapeEditor::RestoreState(IShape* shp, long layerHandle, long shapeIndex)
{
	bool newShape = layerHandle != _layerHandle || shapeIndex != _shapeIndex;
	bool hasShape = _layerHandle != -1;

	if (hasShape && newShape) {
		if (!TrySave())
			return false;
	}
	
	if (newShape) {
		Clear();
		VARIANT_BOOL vb;
		StartEdit(layerHandle, shapeIndex, &vb);
		if (!vb) return false;
	}
	
	SetShape(shp);
	_mapCallback->_SetMapCursor(cmEditShape);
	return true;
}

// ***************************************************************
//		HandleProjPointAdd()
// ***************************************************************
void CShapeEditor::HandleProjPointAdd(double projX, double projY)
{
	double pixelX, pixelY;
	_mapCallback->_ProjectionToPixel(projX, projY, &pixelX, &pixelY);
	_activeShape->AddPoint(projX, projY, pixelX, pixelY);
	
	// TODO: revisit; isn't it better to set it on changing of map cursor
	put_EditorState(_layerHandle != -1 ? EditorCreation : EditorCreationUnbound);    
}

// ***************************************************************
//		HasSubjectShape()
// ***************************************************************
bool CShapeEditor::HasSubjectShape(int LayerHandle, int ShapeIndex)
{
	for (size_t i = 0; i < _subjects.size(); i++) 
	{
		int handle, index;
		_subjects[i]->get_LayerHandle(&handle);
		_subjects[i]->get_ShapeIndex(&index);
		if (LayerHandle == handle && ShapeIndex == index)
			return true;
	}
	return false;
}

