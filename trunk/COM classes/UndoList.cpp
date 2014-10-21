// UndoList.cpp : Implementation of CUndoList

#include "stdafx.h"
#include "UndoList.h"
#include "TableClass.h"

int CUndoList::g_UniqueId = -1;
const int CUndoList::EMPTY_BATCH_ID = -1; 

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void CUndoList::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	USES_CONVERSION;
	ICallback* cb;
	if (_lastErrorCode != tkNO_ERROR) {
		_shapefile->get_GlobalCallback(&cb);
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
	if (!_shapefile) {
		ErrorMessage(tkUNDO_LIST_NO_SHAPEFILE);
	}
	return _shapefile != NULL;
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
	return S_OK;
}

// **********************************************************
//		CheckShapeIndex
// **********************************************************
bool CUndoList::CheckShapeIndex(LONG ShapeIndex)
{
	long numShapes;
	_shapefile->get_NumShapes(&numShapes);
	if (ShapeIndex < 0 || ShapeIndex >= numShapes) {
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return false;
	}
	return true;
}

// **********************************************************
//		Add
// **********************************************************
STDMETHODIMP CUndoList::Add(tkUndoOperation operation, LONG ShapeIndex, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AddSubOperation(operation, ShapeIndex, NULL, retVal);
	return S_OK;
}

// **********************************************************
//		AddSubOperation
// **********************************************************
STDMETHODIMP CUndoList::AddSubOperation(tkUndoOperation operation, LONG ShapeIndex, IShape* shapeData, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;
	if (!CheckShapeIndex(ShapeIndex)) return S_OK;

	TrimList();

	int id = _batchId != EMPTY_BATCH_ID ? _batchId : NextId();

	UndoListItem* item = new UndoListItem((int)id, (int)ShapeIndex, operation);

	if (shapeData) {
		// shape is in the middle of vertices editing; it wasn't saved to the shapefile yet
		// but we still want to be able to undo those individual vertex editing
		item->Shape = shapeData;
		shapeData->AddRef();
	}
	else {
		// need to save current state before operation
		if (operation == uoRemoveShape || operation == uoEditShape)
			CopyShapeState(ShapeIndex, item);
	}

	_list.push_back(item);
	*retVal = VARIANT_TRUE;

	_position = _list.size() - 1;

	return S_OK;
}

// **********************************************************
//		TrimList
// **********************************************************
// if we are not in the end of list, trim the undone items
void CUndoList::TrimList()
{
	if (_position < _list.size() - 1)
	{
		for (size_t i = _list.size() - 1; i > _position; --i) {
			delete _list[i];
			_list.pop_back();
		}
	}
}

// **********************************************************
//		CopyShapeState
// **********************************************************
void CUndoList::CopyShapeState(int shapeIndex, UndoListItem* undoItem)
{
	IShape* shp = NULL;
	_shapefile->get_Shape(shapeIndex, &shp);
	undoItem->Shape = shp;    // reference has been added in get_Shape

	ITable* tbl = NULL;
	_shapefile->get_Table(&tbl);
	if (tbl) {
		TableRow* row = ((CTableClass*)tbl)->CloneTableRow((int)shapeIndex);
		undoItem->Row = row;
		tbl->Release();
	}
}

// **********************************************************
//		Undo
// **********************************************************
STDMETHODIMP CUndoList::Undo(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;

	if (_position >= 0) 
	{
		ITable* tbl = NULL;
		_shapefile->get_Table(&tbl);
		
		int id = _list[_position]->BatchId;
		
		// undo the whole batch
		while (_position >= 0 && _list[_position]->BatchId == id)
  	    {
			UndoSingleItem(_list[_position], tbl);
			_position--;
		}

		if (tbl) tbl->Release();
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// **********************************************************
//		Redo
// **********************************************************
STDMETHODIMP CUndoList::Redo(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;

	int maxIndex = (int)_list.size() - 1;
	if (_position < maxIndex)
	{
		ITable* tbl = NULL;
		_shapefile->get_Table(&tbl);
		int id = _list[_position + 1]->BatchId;

		while (_position < maxIndex && _list[_position + 1]->BatchId == id)
		{
			UndoSingleItem(_list[_position + 1], tbl);
			_position++;
		}

		if (tbl) tbl->Release();
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// **********************************************************
//		UndoSingleItem
// **********************************************************
bool CUndoList::UndoSingleItem(UndoListItem* item, ITable* tbl)
{
	VARIANT_BOOL vb;
	switch (item->Operation)
	{
		case uoRemoveShape:
		{
			// restore removed shape
			_shapefile->EditInsertShape(item->Shape, &(item->ShapeIndex), &vb);
			if (vb) {
				((CTableClass*)tbl)->InsertTableRow(item->Row, item->ShapeIndex);
				item->Shape->Release(); // a reference was added in EditInsertShape
				item->Shape = NULL;
				item->Row = NULL;		// the instance is used by table now
				item->Undone = true;
				item->Operation = uoAddShape;
				return true;
			}
			break;
		}
		case uoAddShape:
		{
			// remove added shape
			IShape* shp = NULL;
			_shapefile->get_Shape(item->ShapeIndex, &shp);

			if (shp) {
				TableRow* row = ((CTableClass*)tbl)->CloneTableRow(item->ShapeIndex);
				_shapefile->EditDeleteShape(item->ShapeIndex, &vb);
				item->Shape = shp;
				item->Row = row;
				item->Undone = true;
				item->Operation = uoRemoveShape;
				return true;
			}
			break;
		}
		case uoSubEditShape:
		case uoEditShape:
		{
			// revert to the previous state
			IShape* shp = NULL;
			_shapefile->get_Shape(item->ShapeIndex, &shp);

			if (shp) {
				_shapefile->EditUpdateShape(item->ShapeIndex, item->Shape, &vb);
				item->Shape = shp;
				item->Undone = true;
				return true;

				//TableRow* row = ((CTableClass*)tbl)->SwapTableRow(item->Row, item->ShapeIndex);
				//item->Row = row;
			}
		}
		break;
	}
	return false;
}

// **********************************************************
//		get_UndoCount
// **********************************************************
STDMETHODIMP CUndoList::get_UndoCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = CheckState() ? _position + 1  : - 1;
	return S_OK;
}

// **********************************************************
//		get_RedoCount
// **********************************************************
STDMETHODIMP CUndoList::get_RedoCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = CheckState() ? (_list.size() - 1) - _position : -1;
	return S_OK;
}

// **********************************************************
//		get_TotalLength
// **********************************************************
STDMETHODIMP CUndoList::get_TotalLength(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = (long)_list.size();
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
		for (size_t i = _list.size() - 1; i >= 0; --i)
		{
			if (_list[i]->BatchId == _batchId)
				count++;
		}
		*retVal = count;
		_batchId = EMPTY_BATCH_ID;
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
