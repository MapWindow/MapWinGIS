// UndoList.cpp : Implementation of CUndoList

#include "stdafx.h"
#include "UndoList.h"
#include "TableClass.h"
#include "ShapeEditor.h"

int CUndoList::g_UniqueId = -1;
const int CUndoList::EMPTY_BATCH_ID = -1; 

//***********************************************************************/
//*			GetShapefile()
//***********************************************************************/
IShapefile* CUndoList::GetShapefile(long layerHandle)
{
	if (!CheckState()) return NULL;
	return _mapCallback->_GetShapefile(layerHandle);
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void CUndoList::ErrorMessage(long ErrorCode)
{
	if (!CheckState()) return;

	_lastErrorCode = ErrorCode;
	USES_CONVERSION;
	ICallback* cb;
	if (_lastErrorCode != tkNO_ERROR) {
		cb = _mapCallback->_GetGlobalCallback();
		if (cb) {
			cb->Error(OLE2BSTR(_key), A2BSTR(ErrorMsg(_lastErrorCode)));
			cb->Release();
		}
	}
}

//***********************************************************************/
//*			CheckState()
//***********************************************************************/
bool CUndoList::CheckState() {
	if (!_mapCallback) {
		ErrorMessage(tkUNDO_LIST_NO_MAP);
	}
	return _mapCallback != NULL;
}

//***********************************************************************
//*		get/put_Key()
//***********************************************************************
STDMETHODIMP CUndoList::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CUndoList::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_key);
	USES_CONVERSION;
	_key = OLE2BSTR(newVal);
	return S_OK;
}

// **********************************************************
//		get_LastErrorCode
// **********************************************************
STDMETHODIMP CUndoList::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// **********************************************************
//		get_ErrorMsg
// **********************************************************
STDMETHODIMP CUndoList::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// **********************************************************
//		Clear
// **********************************************************
STDMETHODIMP CUndoList::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	for (size_t i = 0; i < _list.size(); i++)
		delete _list[i];
	_list.clear();
	_position = -1;
	if (!_dtor)
		FireUndoListChanged();
	return S_OK;
}

// **********************************************************
//		CheckShapeIndex
// **********************************************************
bool CUndoList::CheckShapeIndex(long layerHandle, LONG shapeIndex)
{
	CComPtr<IShapefile> sf = NULL;
	sf.Attach(GetShapefile(layerHandle));
	if (!sf) return false;
	long numShapes;
	sf->get_NumShapes(&numShapes);
	if (shapeIndex < 0 || shapeIndex >= numShapes) {
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return false;
	}
	return true;
}

// **********************************************************
//		Add
// **********************************************************
STDMETHODIMP CUndoList::Add(tkUndoOperation operation, LONG LayerHandle, LONG ShapeIndex, VARIANT_BOOL *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;

	if (operation != uoAddShape) {
		if (!CheckShapeIndex(LayerHandle, ShapeIndex))
			return S_OK;
	}

	TrimList();

	int id = _batchId != EMPTY_BATCH_ID ? _batchId : NextId();

	UndoListItem* item = new UndoListItem((int)id, LayerHandle, ShapeIndex, operation);
	item->WithinBatch = _batchId != EMPTY_BATCH_ID;

	bool copyAttributes = operation == uoRemoveShape || operation == uoEditShape;
	CopyShapeState(LayerHandle, ShapeIndex, copyAttributes, item);

	_list.push_back(item);
	*retVal = VARIANT_TRUE;

	_position = _list.size() - 1;

	if (!item->WithinBatch) {
		FireUndoListChanged();
	}

	return S_OK;
}

// **********************************************************
//		AddMoveOperation
// **********************************************************
bool CUndoList::AddMoveOperation(int layerHandle, vector<int>* indices, double xProjOffset, double yProjOffset)
{
	return AddGroupOperation(uoMoveShapes, layerHandle, indices, xProjOffset, yProjOffset, 0.0);
}

// **********************************************************
//		AddRotateOperation
// **********************************************************
bool CUndoList::AddRotateOperation(int layerHandle, vector<int>* indices, 
							double xProjOrigin, double yProjOrigin, double angleDegrees)
{
	return AddGroupOperation(uoRotateShapes, layerHandle, indices, xProjOrigin, yProjOrigin, angleDegrees);
}

// **********************************************************
//		AddGroupOperation
// **********************************************************
bool CUndoList::AddGroupOperation(tkUndoOperation operation, int layerHandle, vector<int>* indices,
		double xProjOrigin, double yProjOrigin, double angleDegrees)
{
	if (!CheckState()) return false;

	if (!indices) return false;

	TrimList();

	int id = NextId();
	UndoListItem* item = new UndoListItem(id, layerHandle, indices, operation);
	item->ProjOffset.x = xProjOrigin;
	item->ProjOffset.y = yProjOrigin;
	item->RotationAngle = angleDegrees;

	_list.push_back(item);
	_position = _list.size() - 1;

	FireUndoListChanged();
	return true;
}

// **********************************************************
//		TrimList
// **********************************************************
// if we are not in the end of list, trim the undone items
void CUndoList::TrimList()
{
	for (int i = (int)(_list.size() - 1); i > _position; --i) {
		delete _list[i];
		_list.pop_back();
	}
}

// **********************************************************
//		CopyShapeState
// **********************************************************
bool CUndoList::CopyShapeState(long layerHandle, long shapeIndex, bool copyAttributes, UndoListItem* item)
{
	item->Shape = GetCurrentState(layerHandle, shapeIndex);

	if (copyAttributes) {
		CComPtr<IShapefile> sf = NULL;
		sf.Attach(GetShapefile(layerHandle));
		ITable* tbl = NULL;
		sf->get_Table(&tbl);
		if (tbl) {
			TableRow* row = ((CTableClass*)tbl)->CloneTableRow((int)shapeIndex);
			item->Row = row;
			tbl->Release();
		}
	}
	return true;
}

// **********************************************************
//		Undo
// **********************************************************
STDMETHODIMP CUndoList::Undo(VARIANT_BOOL zoomToShape, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;

	CComPtr<IShapeEditor> editor = _mapCallback->_GetShapeEditor();

	if (_position >= 0) 
	{
		int pos = _position;
		int id = _list[pos]->BatchId;
		UndoListItem* item = _list[pos];

		while (_list[_position]->BatchId == id)
  	    {
			UndoSingleItem(_list[_position]);
			_position--;
			if (_position < 0) break;
		}

		ZoomToShape(zoomToShape, item->Operation == uoAddShape ? _position: pos);

		FireUndoListChanged();
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}


// **********************************************************
//		Redo
// **********************************************************
STDMETHODIMP CUndoList::Redo(VARIANT_BOOL zoomToShape, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;

	int maxIndex = (int)_list.size() - 1;
	if (_position + 1 <= maxIndex)
	{
		int pos = _position + 1;
		int id = _list[pos]->BatchId;
		UndoListItem* item = _list[pos];

		while (_list[_position + 1]->BatchId == id)
		{
			UndoSingleItem(_list[_position + 1]);
			_position++;
			if (_position + 1 > maxIndex) 
				break;
		}

		ZoomToShape(zoomToShape, item->Operation == uoRemoveShape ? _position: pos );

		FireUndoListChanged();
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// **********************************************************
//		ZoomToShape
// **********************************************************
void CUndoList::ZoomToShape(VARIANT_BOOL zoomToShape, int itemIndex)
{
	if (!zoomToShape) return;
	if (itemIndex < 0 || itemIndex >= (int)_list.size())return;
	UndoListItem* item = _list[itemIndex];

	if (item->Operation == uoEditShape) {
		_mapCallback->_ZoomToEditor();
	}
	else {
		_mapCallback->_ZoomToShape(item->LayerHandle, item->ShapeIndex);
	}
}

// **********************************************************
//		DiscardOne
// **********************************************************
bool CUndoList::DiscardOne()
{
	if (_list.size() > 0) 
	{
		delete _list[_list.size() - 1];
		_list.pop_back();
		_position--;
		return true;
	}
	return false;
}

// **********************************************************
//		UndoSingleItem
// **********************************************************
bool CUndoList::UndoSingleItem(UndoListItem* item)
{
	CComPtr<IShapefile> sf = NULL;
	sf.Attach(GetShapefile(item->LayerHandle));
	ITable* tbl = NULL;
	sf->get_Table(&tbl);

	VARIANT_BOOL vb;
	switch (item->Operation)
	{
		case uoRotateShapes:
			if (item->ShapeIndices)
			{
				int size = item->ShapeIndices->size();
				for (int i = 0; i < size; i++)
				{
					int index = (*item->ShapeIndices)[i];
					CComPtr<IShape> shp = NULL;
					sf->get_Shape((long)index, &shp);
					if (shp) {
						shp->Rotate(item->ProjOffset.x, item->ProjOffset.y, -item->RotationAngle);
					}
				}
				item->RotationAngle *= -1;
			}
			break;
		case uoMoveShapes:
			if (item->ShapeIndices)
			{
				int size = item->ShapeIndices->size();
				for (int i = 0; i < size; i++)
				{
					int index = (*item->ShapeIndices)[i];
					CComPtr<IShape> shp = NULL;
					sf->get_Shape((long)index, &shp);
					if (shp) {
						shp->Move(item->ProjOffset.x, item->ProjOffset.y);
					}
				}
				item->ProjOffset.x *= -1;	// simply change the sign for the redo
				item->ProjOffset.y *= -1;
			}
			break;
		case uoRemoveShape:
		{
			// restore removed shape
			sf->EditInsertShape(item->Shape, &(item->ShapeIndex), &vb);
			if (vb) {
				((CTableClass*)tbl)->InsertTableRow(item->Row, item->ShapeIndex);
				item->Shape->Release(); // a reference was added in EditInsertShape
				item->Shape = NULL;
				item->Row = NULL;		// the instance is used by table now
				item->Operation = uoAddShape;
				IShapeEditor* editor = _mapCallback->_GetShapeEditor();
				if (editor && !item->WithinBatch) {
					IShape* shp = GetCurrentState(item->LayerHandle, item->ShapeIndex);
					((CShapeEditor*)editor)->RestoreState(shp, item->LayerHandle, item->ShapeIndex);
				}
				return true;
			}
			break;
		}
		case uoAddShape:
		{
			// remove added shape
			IShape* shp = GetCurrentState(item->LayerHandle, item->ShapeIndex);
			if (!item->WithinBatch)
			{
				if (ShapeInEditor(item->LayerHandle, item->ShapeIndex)) {
					_mapCallback->_GetShapeEditor()->Clear();
				}
			}

			if (shp) {
				TableRow* row = ((CTableClass*)tbl)->CloneTableRow(item->ShapeIndex);
				sf->EditDeleteShape(item->ShapeIndex, &vb);
				item->Shape = shp;
				item->Row = row;
				item->Operation = uoRemoveShape;
				return true;
			}
			break;
		}
		case uoEditShape:
		{
			IShapeEditor* editor = _mapCallback->_GetShapeEditor();
			if (editor) {
				IShape* shp = GetCurrentState(item->LayerHandle, item->ShapeIndex);
				((CShapeEditor*)editor)->RestoreState(item->Shape, item->LayerHandle, item->ShapeIndex);
				item->Shape = shp;
			}
			break;
		}
	}
	if (tbl) {
		tbl->Release();
	}

	return false;
}

// **********************************************************
//		ShapeInEditor
// **********************************************************
bool CUndoList::ShapeInEditor(long layerHandle, long shapeIndex) 
{
	IShapeEditor* editor = _mapCallback->_GetShapeEditor();
	int handle, index;
	editor->get_LayerHandle(&handle);
	editor->get_ShapeIndex(&index);
	return handle == layerHandle && shapeIndex == index;
}

// **********************************************************
//		GetCurrentState
// **********************************************************
IShape* CUndoList::GetCurrentState(long layerHandle, long shapeIndex) 
{
	IShapeEditor* editor = _mapCallback->_GetShapeEditor();
	
	IShape* shp = NULL;
	if (ShapeInEditor(layerHandle, shapeIndex)) {
		editor->get_RawData(&shp);
	}
	else {
		CComPtr<IShapefile> sf = NULL;
		sf.Attach(GetShapefile(layerHandle));
		if (sf) {
			sf->get_Shape(shapeIndex, &shp);
		}
	}
	return shp;
}

// **********************************************************
//		get_UndoCount
// **********************************************************
STDMETHODIMP CUndoList::get_UndoCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = CheckState() ? FindPosition(_position) : -1;
	return S_OK;
}

// **********************************************************
//		get_RedoCount
// **********************************************************
STDMETHODIMP CUndoList::get_RedoCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long totalLength, undoCount;
	get_TotalLength(&totalLength);
	get_UndoCount(&undoCount);
	*pVal = CheckState() ? totalLength - undoCount : -1;
	return S_OK;
}

// **********************************************************
//		FindPosition
// **********************************************************
long CUndoList::FindPosition(int position)
{
	int lastId = EMPTY_BATCH_ID;
	long count = 0;
	for (size_t i = 0; i < _list.size(); i++)
	{
		if (_list[i]->BatchId != lastId) {
			lastId = _list[i]->BatchId;
			count++;
		}
		if (i == position) return count;
	}
	return 0;
}

// **********************************************************
//		FindPosition
// **********************************************************
bool CUndoList::WithinBatch(int position)
{
	if (position < 0 || position >= (int)_list.size()) return false;
	int id = _list[position]->BatchId;
	if (position + 1 < (int)_list.size() && _list[position + 1]->BatchId == id)
		return true;
	if (position - 1 >= 0 && _list[position - 1]->BatchId == id)
		return true;
	return false;
}

// **********************************************************
//		get_TotalLength
// **********************************************************
STDMETHODIMP CUndoList::get_TotalLength(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int lastId = EMPTY_BATCH_ID;
	long count = 0;
	for (size_t i = 0; i < _list.size(); i++)
	{
		if (_list[i]->BatchId != lastId) {
			lastId = _list[i]->BatchId;
			count++;
		}
	}
	*pVal = count;
	return S_OK;
}

// **********************************************************
//		BeginBatch
// **********************************************************
STDMETHODIMP CUndoList::BeginBatch(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_batchId == EMPTY_BATCH_ID) {
		_batchId = NextId();
		*retVal = VARIANT_TRUE;
	}
	else {
		ErrorMessage(tkALREADY_WITHIN_BATCH);
		*retVal = VARIANT_FALSE;
	}
	return S_OK;
}

// **********************************************************
//		EndBatch
// **********************************************************
STDMETHODIMP CUndoList::EndBatch(LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_batchId == EMPTY_BATCH_ID) {
		*retVal = -1;
		return S_OK;
	}
	else {
		long count = 0;
		for (int i = _list.size() - 1; i >= 0; --i)
		{
			if (_list[i]->BatchId == _batchId)
				count++;
		}
		*retVal = count;
		_batchId = EMPTY_BATCH_ID;

		if (count > 0)
			FireUndoListChanged();

		return S_OK;
	}
}

// **********************************************************
//		GetLastId
// **********************************************************
STDMETHODIMP CUndoList::GetLastId(LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _list.size() > 0 ? _list.at(_list.size() - 1)->BatchId : -1;
	return S_OK;
}

// **********************************************************
//		ShortcutKey
// **********************************************************
STDMETHODIMP CUndoList::get_ShortcutKey(tkUndoShortcut* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _shortcutKey;
	return S_OK;
}
STDMETHODIMP CUndoList::put_ShortcutKey(tkUndoShortcut newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_shortcutKey = newVal;
	return S_OK;
}

// **********************************************************
//		FireUndoListChanged
// **********************************************************
void CUndoList::FireUndoListChanged()
{
	if (_mapCallback) {
		_mapCallback->_FireUndoListChanged();
	}
}

// **********************************************************
//		ClearForLayer
// **********************************************************
STDMETHODIMP CUndoList::ClearForLayer(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	bool changed = false;
	for (int i = (int)_list.size() - 1; i >= 0; i--)
	{
		if (_list[i]->LayerHandle == LayerHandle) 
		{
			delete _list[i];
			_list.erase(_list.begin() + i);
			changed = true;
			if (_position >= i)
				_position--;
		}
	}
	if (changed)
		FireUndoListChanged();
	return S_OK;
}

