// ShapeEditor.cpp : Implementation of CShapeEditor

#include "stdafx.h"
#include "ShapeEditor.h"
#include "Shape.h"
#include "UndoList.h"
#include <set>
#include "GeosHelper.h"
#include "EditorHelper.h"
#include "ShapefileHelper.h"
#include "MeasuringHelper.h"

// *******************************************************
//		GetShape
// *******************************************************
IShape* CShapeEditor::GetLayerShape(long layerHandle, long shapeIndex)
{
	if (!CheckState()) return NULL;
	CComPtr<IShapefile> sf = NULL;
	sf.Attach(_mapCallback->_GetShapefile(layerHandle));
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
	CallbackHelper::ErrorMsg("ShapeEditor", _globalCallback, _key, ErrorMsg(_lastErrorCode));
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
		ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
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
	*retVal = _activeShape->GetShapeType2D();
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
	if (shpType == SHP_NULLSHAPE || _activeShape->GetPointCount() == 0) {
		return S_OK;
	}

	ComHelper::CreateShape(retVal);
	

	VARIANT_BOOL vb;
	(*retVal)->Create(shpType, &vb);

	CopyData(0, _activeShape->GetPointCount(), *retVal);

	return S_OK;
}

// *******************************************************
//		get_ValidatedShape()
// *******************************************************
STDMETHODIMP CShapeEditor::get_ValidatedShape(IShape** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = NULL;

	IShape* shp = NULL;
	get_RawData(&shp);
	
	if (Validate(&shp)) {
		*retVal = shp;
	}
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
STDMETHODIMP CShapeEditor::get_NumPoints(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _activeShape->GetPointCount();
	return S_OK;
}

// *******************************************************
//		ClearCore()
// *******************************************************
bool CShapeEditor::ClearCore(bool all)
{
	if (!CheckState()) return true;

	if (_state == esOverlay)
	{
		CancelOverlay(true);
		if (!all) return false;    // clear overlay only; one more call is needed to finish it
	}

	if (_state == esEdit) {

		CComPtr<IShapefile> sf = NULL;
		sf.Attach(_mapCallback->_GetShapefile(_layerHandle));

		if (sf) {
			sf->put_ShapeIsHidden(_shapeIndex, VARIANT_FALSE);
		}

		if (_startingUndoCount != -1) {
			DiscardChangesFromUndoList();
		}
	}

	_activeShape->Clear();

	_startingUndoCount = -1;
	_shapeIndex = -1;
	_layerHandle = -1;
	_state = esNone;
	return true;
}

// *******************************************************
//		DiscardChanges()
// *******************************************************
void CShapeEditor::DiscardChangesFromUndoList()
{
	if (_startingUndoCount == -1) return;
	
	IUndoList* undoList = _mapCallback->_GetUndoList();
	if (undoList)
	{
		// as we discard the changes, clear them from undo list;
		// it's also possible to actually call Undo rather than DiscardState
		// but there is no point in doing so, as we aren't going to substitute
		// the underlying shape
		long length;
		undoList->get_UndoCount(&length);
		long undoCount = length - _startingUndoCount;
		for (long i = 0; i < undoCount; i++)
			((CUndoList*)undoList)->DiscardOne();
		if (undoCount > 0) {
			_mapCallback->_FireUndoListChanged();
		}
	}
}

// *******************************************************
//		Clear()
// *******************************************************
STDMETHODIMP CShapeEditor::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ClearCore(true);
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

	CComPtr<IShapefile> sf = NULL;
	sf.Attach(_mapCallback->_GetShapefile(LayerHandle));
	if (!sf) {
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return S_OK;
	}

	if (!ShapefileHelper::InteractiveEditing(sf))
	{
		ErrorMessage(tkNO_INTERACTIVE_EDITING);
		return S_OK;
	}

	Clear();

	IUndoList* list = _mapCallback->_GetUndoList();
	if (list) {
		list->get_UndoCount(&_startingUndoCount);
	}
	
	CComPtr<IShape> shp = NULL;
	sf->get_Shape(ShapeIndex, &shp);
	if (shp)
	{
		put_EditorState(esEdit);
		SetShape(shp);
		_layerHandle = LayerHandle;
		_shapeIndex = ShapeIndex;
		sf->put_ShapeIsHidden(ShapeIndex, VARIANT_TRUE);
		EditorHelper::CopyOptionsFrom(this, sf);
		*retVal = VARIANT_TRUE;
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
STDMETHODIMP CShapeEditor::get_BearingType(tkBearingType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _activeShape->BearingType;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_BearingType(tkBearingType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->BearingType = newVal;
	return S_OK;
}

// *******************************************************
//		CreationMode()
// *******************************************************
STDMETHODIMP CShapeEditor::get_IsDigitizing(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _activeShape->GetCreationMode() ? VARIANT_TRUE : VARIANT_FALSE;
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
STDMETHODIMP CShapeEditor::get_LayerHandle(LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _layerHandle;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_LayerHandle(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_layerHandle = newVal;
	return S_OK;
}

// *******************************************************
//		ShapeIndex()
// *******************************************************
STDMETHODIMP CShapeEditor::get_ShapeIndex(LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _shapeIndex;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_ShapeIndex(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_shapeIndex = newVal;
	return S_OK;
}

// *******************************************************
//		LabelsOnly()
// *******************************************************
STDMETHODIMP CShapeEditor::get_ShapeVisible(VARIANT_BOOL* val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*val = _activeShape->_drawLabelsOnly;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_ShapeVisible(VARIANT_BOOL newVal)
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
	int index = _activeShape->GetSelectedVertex();
	
	SaveState();

	if (!_activeShape->RemoveSelectedVertex())
	{
		DiscardState();
		return false;
	}

	if (index == _activeShape->GetPointCount())
	{
		index--;
	}
	
	_activeShape->SetSelectedVertex(index);

	return true;
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
//		AddPoint()
// *******************************************************
STDMETHODIMP CShapeEditor::AddPoint(IPoint *newPoint, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (newPoint == NULL) {
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
	} else {
		VARIANT_BOOL digitizing;
		get_IsDigitizing(&digitizing);
		tkCursorMode cursor = _mapCallback->_GetCursorMode();
		if (digitizing) {
			double x, y;
			newPoint->get_X(&x);
			newPoint->get_Y(&y);
			newPoint->Release();
			_activeShape->AddPoint(x, y, -1, -1, PartBegin);
			*retVal = VARIANT_TRUE;
			return S_OK;
		}
	}
	return S_OK;
}

// *******************************************************
//		UndoPoint()
// *******************************************************
STDMETHODIMP CShapeEditor::UndoPoint(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	VARIANT_BOOL digitizing;
	get_IsDigitizing(&digitizing);
	
	if (digitizing) {
		*retVal = _activeShape->UndoPoint() ? VARIANT_TRUE : VARIANT_FALSE;
	}
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
//		HighlightShapes()
// *******************************************************
STDMETHODIMP CShapeEditor::get_HighlightVertices(tkLayerSelection* pVal)
{
	*pVal = _highlightShapes;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_HighlightVertices(tkLayerSelection newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_highlightShapes = newVal;
	return S_OK;
}

// *******************************************************
//		get_SnapBehavior()
// *******************************************************
STDMETHODIMP CShapeEditor::get_SnapBehavior(tkLayerSelection* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _snapBehavior;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_SnapBehavior(tkLayerSelection newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_snapBehavior = newVal;
	return S_OK;
}

// *******************************************************
//		get_SnapMode()
// *******************************************************
STDMETHODIMP CShapeEditor::get_SnapMode(tkSnapMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _snapMode;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_SnapMode(tkSnapMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_snapMode = newVal;
	return S_OK;
}

// *******************************************************
//		EditorState
// *******************************************************
STDMETHODIMP CShapeEditor::get_EditorState(tkEditorState* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _state;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_EditorState(tkEditorState newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->SetCreationMode(newVal == esDigitize || newVal == esDigitizeUnbound || newVal == esOverlay);
	_activeShape->OverlayTool = newVal == esOverlay || newVal == esDigitizeUnbound;
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
STDMETHODIMP CShapeEditor::get_IndicesVisible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->PointLabelsVisible ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_IndicesVisible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->PointLabelsVisible = newVal ? true: false;
	return S_OK;
}

// *******************************************************
//		LengthDisplayMode
// *******************************************************
STDMETHODIMP CShapeEditor::get_LengthDisplayMode(tkLengthDisplayMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->LengthUnits;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_LengthDisplayMode(tkLengthDisplayMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->LengthUnits = newVal;
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
// for built-in cursors
bool CShapeEditor::StartUnboundShape(tkCursorMode cursor)
{
	ShpfileType	shpType = GetShapeTypeForTool(cursor);
	if (shpType == SHP_NULLSHAPE)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}
	put_ShapeType(shpType);
	put_EditorState(esDigitizeUnbound);
	ApplyColoringForTool(cursor);
	return S_OK;
}

// *******************************************************
//		StartUnboundShape
// *******************************************************
STDMETHODIMP CShapeEditor::StartUnboundShape(ShpfileType shpType, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	tkCursorMode cursor = _mapCallback->_GetCursorMode();
	if (cursor != cmAddShape) {
		ErrorMessage(tkADD_SHAPE_MODE_ONLY);
		return S_OK;
	}

	if (shpType == SHP_NULLSHAPE)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}
	
	put_ShapeType(shpType);
	put_EditorState(esDigitizeUnbound);

	return S_OK;
}

// *******************************************************
//		VerticesVisible
// *******************************************************
STDMETHODIMP CShapeEditor::get_VerticesVisible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->PointsVisible ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_VerticesVisible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->PointsVisible = newVal ? true: false;
	return S_OK;
}

// *******************************************************
//		GetClosestPoint
// *******************************************************
bool CShapeEditor::GetClosestPoint(double projX, double projY, double& xResult, double& yResult)
{
	ShpfileType shpType = ShapeUtility::Convert2D(_activeShape->GetShapeType2D());

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
		ComHelper::CreateShape(&shp2);
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
	CComPtr<IShapefile> sf = NULL;
	sf.Attach(_mapCallback->_GetShapefile(_layerHandle));
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
				_startingUndoCount = -1;
				_mapCallback->_FireAfterShapeEdit(uoRemoveShape, _layerHandle, _shapeIndex);
				return true;
			}
		}
	}
	return false;
}

// ************************************************************
//		Validate
// ************************************************************
bool CShapeEditor::Validate(IShape** shp)
{
	if (!(*shp)) return false;

	ShpfileType shpType;
	(*shp)->get_ShapeType(&shpType);
	
	long numPoints;
	(*shp)->get_NumPoints(&numPoints);

	// close the poly automatically
	if (shpType == SHP_POLYGON)
		((CShape*)(*shp))->FixupShapeCore(ShapeValidityCheck::FirstAndLastPointOfPartMatch);

	VARIANT_BOOL vb;
	ShapeValidityCheck validityCheck;
	CString errMsg;
	if (!((CShape*)(*shp))->ValidateBasics(validityCheck, errMsg))
	{
		if (validityCheck == ShapeValidityCheck::DirectionOfPolyRings) 
		{
			(*shp)->ReversePointsOrder(0, &vb);
		}
	}

	if (!((CShape*)(*shp))->ValidateBasics(validityCheck, errMsg)) {
		_mapCallback->_FireShapeValidationFailed(errMsg);
		return false;
	}

	return ValidateWithGeos(shp);
}

// ************************************************************
//		ValidateWithGeos
// ************************************************************
bool CShapeEditor::ValidateWithGeos(IShape** shp)
{
	VARIANT_BOOL valid = VARIANT_TRUE;

	ShpfileType shpType;
	(*shp)->get_ShapeType(&shpType);
	ShapeUtility::Convert2D(shpType);

	bool skipGeosCheck = false;
	if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT || shpType == SHP_POLYLINE)
		skipGeosCheck = true;     // there is hardly anything else to check for those

	if ((_validationMode == evCheckWithGeos || _validationMode == evFixWithGeos) && !skipGeosCheck)
	{
		(*shp)->get_IsValid(&valid);

		if (!valid && _validationMode == evFixWithGeos)
		{
			IShape* shpNew = NULL;
			(*shp)->FixUp(&shpNew);
			if (shpNew) {
				(*shp) = NULL;				// TODO: will it release the shape?
				(*shp) = shpNew;
				valid = VARIANT_TRUE;
			}
		}

		// report results back to user
		CComBSTR reason;
		if (!valid) {
			(*shp)->get_IsValidReason(&reason);
			USES_CONVERSION;
			_mapCallback->_FireShapeValidationFailed(OLE2A(reason));
		}
	}
	return valid ? true : false;
}

// ************************************************************
//		TryStopDigitizing
// ************************************************************
bool CShapeEditor::TryStop()
{
	if (!CheckState()) return false;
	
	CComPtr<IShape> shp = NULL;
	get_ValidatedShape(&shp);

	switch (_state)
	{
		case esDigitizeUnbound:
			_mapCallback->_UnboundShapeFinished(shp);
			SetRedrawNeeded(rtShapeEditor);
			break;
		case esOverlay:
			EndOverlay(shp);
			SetRedrawNeeded(rtShapeEditor);
			break;
		case esDigitize:
		case esEdit:
			VARIANT_BOOL isEmpty;
			get_IsEmpty(&isEmpty);
			if (isEmpty) return true;
			if (!shp) return false;
			bool result = TrySaveShape(shp);
			SetRedrawNeeded(rtVolatileLayer);
			break;
	}
	return true;
}

// ************************************************************
//		TrySaveShape
// ************************************************************
bool CShapeEditor::TrySaveShape(IShape* shp)
{
	CComPtr<IShapefile> sf = NULL;
	sf.Attach(_mapCallback->_GetShapefile(_layerHandle));
	if (!sf) {
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return false;
	}

	// custom validation
	tkMwBoolean cancel = blnFalse;
	_mapCallback->_FireValidateShape(_layerHandle, shp, &cancel);
	if (cancel == blnTrue) {
		return false;
	}

	VARIANT_BOOL vb;
	int shapeIndex = _shapeIndex;
	int layerHandle = _layerHandle;
	bool newShape = _shapeIndex == -1;

	if (newShape)
	{
		long numShapes = ShapefileHelper::GetNumShapes(sf);
		IUndoList* undoList = _mapCallback->_GetUndoList();
		undoList->Add(uoAddShape, (long)_layerHandle, (long)numShapes, &vb);
		sf->EditInsertShape(shp, &numShapes, &vb);
		shapeIndex = numShapes;
	}
	else 
	{
		this->get_HasChanges(&vb);

		if (vb) {
			// don't update it if there are no changes 
			// or it will be marked as modified & send to database on saving
			sf->EditUpdateShape(shapeIndex, shp, &vb);
		}
	}
	_startingUndoCount = -1;	// don't discard states; operation succeeded

	Clear();

	_mapCallback->_Redraw(tkRedrawType::RedrawAll, false, true);

	// let the user set new attributes
	_mapCallback->_FireAfterShapeEdit(newShape ? uoAddShape : uoEditShape, layerHandle, shapeIndex);

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
		if (!TryStop())
			return false;
	}
	
	if (newShape) {
		Clear();
		VARIANT_BOOL vb;
		StartEdit(layerHandle, shapeIndex, &vb);
		if (!vb) return false;
	}
	
	SetShape(shp);
	_mapCallback->_SetMapCursor(cmEditShape, false);
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
	_activeShape->ClearSnapPoint();
}

// ***************************************************************
//		HasSubjectShape()
// ***************************************************************
bool CShapeEditor::HasSubjectShape(int LayerHandle, int ShapeIndex)
{
	for (size_t i = 0; i < _subjects.size(); i++) 
	{
		LONG handle, index;
		_subjects[i]->get_LayerHandle(&handle);
		_subjects[i]->get_ShapeIndex(&index);
		if (LayerHandle == handle && ShapeIndex == index)
			return true;
	}
	return false;
}

// ***************************************************************
//		ValidationMode()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_ValidationMode(tkEditorValidation* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _validationMode;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_ValidationMode(tkEditorValidation newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_validationMode = newVal;
	return S_OK;
}

// ***************************************************************
//		GetOverlayTypeForSubjectOperation()
// ***************************************************************
ShpfileType CShapeEditor::GetShapeTypeForTool(tkCursorMode cursor)
{
	switch (cursor)
	{
		case cmSelectByPolygon:
		case cmClipByPolygon:
		case cmSplitByPolygon:
		case cmEraseByPolygon:
			return SHP_POLYGON;
		case cmSplitByPolyline:
			return SHP_POLYLINE;
	}
	return SHP_NULLSHAPE;
}

// ***************************************************************
//		ApplyColoringForTool()
// ***************************************************************
void CShapeEditor::ApplyColoringForTool(tkCursorMode mode)
{
	OLE_COLOR color;
	GetUtils()->ColorByName(LightSlateGray, &color);
	put_LineColor(color);
	put_LineWidth(1.0f);
	switch (mode)
	{
		case cmClipByPolygon:
			GetUtils()->ColorByName(Green, &color);
			put_FillColor(color);
			break;
		case cmEraseByPolygon:
			GetUtils()->ColorByName(Red, &color);
			put_FillColor(color);
			break;		
		case cmSplitByPolyline:
			put_LineColor(RGB(255, 0, 0));
			break;
		case cmSelectByPolygon:
			GetUtils()->ColorByName(Yellow, &color);
			put_FillColor(color);
			put_LineColor(color);
			break;
		case cmSplitByPolygon:
			put_FillColor(RGB(127, 127, 127));
			put_LineColor(RGB(255, 0, 0));
			break;
	}
}

// ***************************************************************
//		ApplyOverlayColoring()
// ***************************************************************
void CShapeEditor::ApplyOverlayColoring(tkEditorOverlay overlay)
{
	_overlayType =  overlay;
	OLE_COLOR color;
	switch (overlay)
	{
		case eoAddPart:
			GetUtils()->ColorByName(Green, &color);
			put_FillColor(color);
			break;
		case eoRemovePart:
			GetUtils()->ColorByName(Red, &color);
			put_FillColor(color);
			break;
	}
}

// ***************************************************************
//		Clone()
// ***************************************************************
CShapeEditor* CShapeEditor::Clone()
{
	CShapeEditor* editor = NULL;
	ComHelper::CreateInstance(idShapeEditor, (IDispatch**)&editor);
	editor->SetIsSubject(true);
	editor->SetMapCallback(_mapCallback);

	CComPtr<IShapefile> sf = NULL;
	sf.Attach(_mapCallback->_GetShapefile(_layerHandle));
	if (sf) {
		EditorHelper::CopyOptionsFrom(editor, sf);
	}

	CComPtr<IShape> shp = NULL;
	get_RawData(&shp);
	if (shp) {
		editor->SetShape(shp);
	}
	return editor;
}

// ***************************************************************
//		StartOverlay()
// ***************************************************************
STDMETHODIMP CShapeEditor::StartOverlay(tkEditorOverlay overlayType, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	
	if (_activeShape->IsEmpty())
	{
		ErrorMessage(tkEDITOR_OVERLAY_NO_SUBJECT);
		return S_OK;
	}

	if (_subjects.size() > 0)
	{
		ErrorMessage(tkEDITOR_OVERLAY_ALREADY_STARTED);
		return S_OK;
	}
	
	CShapeEditor* editor = Clone();
	_subjects.push_back(editor);
	ShpfileType shpType = overlayType == eoRemovePart ? SHP_POLYGON: _activeShape->GetShapeType2D();

	_activeShape->Clear();
	put_ShapeType(shpType);
	put_EditorState(esOverlay);
	ApplyOverlayColoring(overlayType);

	_mapCallback->_SetMapCursor(cmAddShape, false);

	return S_OK;
}

// *******************************************************
//		CalculateOverlay
// *******************************************************
IShape* CShapeEditor::CalculateOverlay(IShape* overlay)
{
	if (!overlay) return NULL;

	if (_subjects.size() != 1) {
		return NULL;
	}

	CComPtr<IShape> subject = NULL;
	_subjects[0]->get_RawData(&subject);

	if (!subject || !overlay) {
		return NULL;
	}

	IShape* result = NULL;
	switch (_overlayType)
	{
		case eoAddPart:
			subject->Clip(overlay, tkClipOperation::clUnion, &result);
			break;
		case eoRemovePart:
			subject->Clip(overlay, tkClipOperation::clDifference, &result);
			break;
	}
	return result;
}

// *******************************************************
//		EndOverlay
// *******************************************************
void CShapeEditor::EndOverlay(IShape* overlay)
{
	CComPtr<IShape> result = NULL;
	result.Attach(CalculateOverlay(overlay));

	CancelOverlay(false);

	if (result)
	{
		VARIANT_BOOL vb;
		IUndoList* undoList = _mapCallback->_GetUndoList();
		undoList->Add(uoEditShape, _layerHandle, _shapeIndex, &vb);
		SetShape(result);
	}
}

// ***************************************************************
//		CancelOverlay()
// ***************************************************************
void CShapeEditor::CancelOverlay(bool restoreSubjectShape)
{
	if (_state == esOverlay)
	{
		if (_subjects.size() > 0)
		{
			CComPtr<IShape> shp = NULL;
			_subjects[0]->get_RawData(&shp);
			SetShape(shp);
		}
		ClearSubjectShapes();
		put_EditorState(esEdit);
		CopyOptionsFromShapefile();
		_mapCallback->_SetMapCursor(cmEditShape, false);
	}
}

// ***************************************************************
//		CopyOptionsFromShapefile()
// ***************************************************************
void CShapeEditor::CopyOptionsFromShapefile()
{
	CComPtr<IShapefile> sf = NULL;
	sf.Attach(_mapCallback->_GetShapefile(_layerHandle));
	EditorHelper::CopyOptionsFrom(this, sf);
}

// ***************************************************************
//		CopyOptionsFrom()
// ***************************************************************
STDMETHODIMP CShapeEditor::CopyOptionsFrom(IShapeDrawingOptions* options)
{
	EditorHelper::CopyOptionsFrom(this, options);
	return S_OK;
}

// ***************************************************************
//		EditorBehavior()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_EditorBehavior(tkEditorBehavior* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _behavior;
	return S_OK;
}
STDMETHODIMP CShapeEditor::put_EditorBehavior(tkEditorBehavior newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_behavior = newVal;
	switch (newVal)
	{
		case ebVertexEditor:
			SetSelectedPart(-1);
			_redrawNeeded = true;
			break;
		case ebPartEditor:
			SetSelectedVertex(-1);
			_redrawNeeded = true;
			break;
	}
	return S_OK;
}

// ***************************************************************
//		SaveChanges()
// ***************************************************************
STDMETHODIMP CShapeEditor::SaveChanges(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (_state == esOverlay)
	{
		CancelOverlay(true);    // discard any overlay
	}
	bool result = TryStop();
	*retVal = result ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ***************************************************************
//		HasChanges()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_HasChanges(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;
	
	switch (_state)
	{
		case esNone:
			*pVal = VARIANT_FALSE;
			break;
		case esDigitizeUnbound:
		case esDigitize:
			*pVal = EditorHelper::IsEmpty(this) ? VARIANT_FALSE : VARIANT_TRUE;
			break;
		case esEdit:
		case esOverlay:
			IUndoList* list = _mapCallback->_GetUndoList();
			if (list) {
				long undoCount;
				list->get_UndoCount(&undoCount);
				*pVal = undoCount > _startingUndoCount ? VARIANT_TRUE : VARIANT_FALSE;
			}
			break;
	}
	return S_OK;
}

// ***************************************************************
//		get_IsUsingEllipsoid()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_IsUsingEllipsoid(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->GetTransformationMode() != tmNotDefined;
	return S_OK;
}

// ***************************************************************
//		get_Length()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_Length(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->GetDistance();
	return S_OK;
}

// ***************************************************************
//		AreaVisible()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_ShowArea(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->ShowArea;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_ShowArea(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->ShowArea = newVal ? true : false;
	return S_OK;
}

// ***************************************************************
//		AreaPrecision()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_AreaPrecision(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->AreaPrecision;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_AreaPrecision(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->AreaPrecision = newVal;
	return S_OK;
}

// ***************************************************************
//		LengthPrecision()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_LengthPrecision(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->LengthPrecision;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_LengthPrecision(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->LengthPrecision = newVal;
	return S_OK;
}

// ***************************************************************
//		AngleFormat()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_AngleFormat(tkAngleFormat* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->AngleFormat;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_AngleFormat(tkAngleFormat newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->AngleFormat = newVal;
	return S_OK;
}

// ***************************************************************
//		AnglePrecision()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_AnglePrecision(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->AnglePrecision;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_AnglePrecision(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->AnglePrecision = newVal;
	return S_OK;
}

// ***************************************************************
//		ShowBearing()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_ShowBearing(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->ShowBearing;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_ShowBearing(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->ShowBearing = newVal ? true : false;
	return S_OK;
}

// ***************************************************************
//		ShowLength()
// ***************************************************************
STDMETHODIMP CShapeEditor::get_ShowLength(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeShape->ShowLength;
	return S_OK;
}

STDMETHODIMP CShapeEditor::put_ShowLength(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeShape->ShowLength = newVal ? true : false;
	return S_OK;
}

// *******************************************************
//		Serialize()
// *******************************************************
STDMETHODIMP CShapeEditor::Serialize(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPLXMLNode* node = MeasuringHelper::Serialize(_activeShape, "ShapeEditorClass");
	Utility::SerializeAndDestroyXmlTree(node, retVal);

	return S_OK;
}

// *******************************************************
//		Deserialize()
// *******************************************************
STDMETHODIMP CShapeEditor::Deserialize(BSTR state, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	*retVal = VARIANT_FALSE;

	CString s = OLE2CA(state);
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		CPLXMLNode* nodeEditor = CPLGetXMLNode(node, "=ShapeEditorClass");
		if (nodeEditor)
		{
			MeasuringHelper::Deserialize(_activeShape, nodeEditor);
			*retVal = VARIANT_TRUE;
		}
		CPLDestroyXMLNode(node);
	}

	return S_OK;
}
