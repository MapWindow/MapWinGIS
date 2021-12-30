//********************************************************************************************************
//File name: Shapefile.cpp
//Description: Implementation of the CShapefile
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//********************************************************************************************************
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
// -------------------------------------------------------------------------------------------------------
// Paul Meems August 2018: Modernized the code as suggested by CLang and ReSharper

#include "StdAfx.h"
#include <io.h>
#include "Shapefile.h"
#include "Labels.h"
#include "Charts.h"
#include "GeoProjection.h"
#include "Templates.h"
#include <GeosHelper.h>
#include "ShapefileCategories.h"
#include "Shape.h"
#include "GeosConverter.h"
#include "ShapefileHelper.h"
#include "LabelsHelper.h"
#include "ShapeStyleHelper.h"
#include "TableClass.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

CShapefile::CShapefile()
{
    _pUnkMarshaler = nullptr;

    _sortingChanged = true;
    _sortAscending = VARIANT_FALSE;
    _sortField = SysAllocString(L"");

    _appendStartShapeCount = -1;
    _appendMode = VARIANT_FALSE;
    _snappable = VARIANT_TRUE;
    _interactiveEditing = VARIANT_FALSE;
    _hotTracking = VARIANT_TRUE;
    _selectable = VARIANT_FALSE;
    _geosGeometriesRead = false;
    _stopExecution = nullptr;

    _selectionTransparency = 180;
    _selectionAppearance = saSelectionColor;
    _selectionColor = RGB(255, 255, 0);
    _collisionMode = tkCollisionMode::LocalList;

    _geometryEngine = m_globalSettings.geometryEngine;

    _sourceType = sstUninitialized;

    _writing = false;
    _reading = false;

    _isEditingShapes = FALSE;
    _fastMode = m_globalSettings.shapefileFastMode ? TRUE : FALSE;
    _minDrawingSize = 1;
    _volatile = false;

    _useSpatialIndex = TRUE;
    _hasSpatialIndex = FALSE;
    _spatialIndexLoaded = FALSE;
    _spatialIndexMaxAreaPercent = 0.5;
    _spatialIndexNodeCapacity = 100;

    //Neio 20090721
    _useQTree = FALSE;
    _cacheExtents = FALSE;
    _qtree = nullptr;
    _tempTree = nullptr;

    _shpfile = nullptr;
    _shxfile = nullptr;

    _shpfiletype = SHP_NULLSHAPE;
    _nextShapeHandle = 0;

    _minX = 0.0;
    _minY = 0.0;
    _minZ = 0.0;
    _maxX = 0.0;
    _maxY = 0.0;
    _maxZ = 0.0;
    _minM = 0.0;
    _maxM = 0.0;

    _key = SysAllocString(L"");
    _expression = SysAllocString(L"");
    _globalCallback = nullptr;
    _lastErrorCode = tkNO_ERROR;
    _table = nullptr;

    // creation of children classes
    _selectDrawOpt = nullptr;
    _defaultDrawOpt = nullptr;
    _labels = nullptr;
    _categories = nullptr;
    _charts = nullptr;
    _geoProjection = nullptr;

    ComHelper::CreateInstance(idShapeValidationInfo, (IDispatch**)&_inputValidation);
    ComHelper::CreateInstance(idShapeValidationInfo, (IDispatch**)&_outputValidation);

    CoCreateInstance(CLSID_ShapeDrawingOptions, nullptr, CLSCTX_INPROC_SERVER, IID_IShapeDrawingOptions,
                     (void**)&_selectDrawOpt);
    CoCreateInstance(CLSID_ShapeDrawingOptions, nullptr, CLSCTX_INPROC_SERVER, IID_IShapeDrawingOptions,
                     (void**)&_defaultDrawOpt);
    CoCreateInstance(CLSID_ShapefileCategories, nullptr, CLSCTX_INPROC_SERVER, IID_IShapefileCategories,
                     (void**)&_categories);
    CoCreateInstance(CLSID_Labels, nullptr, CLSCTX_INPROC_SERVER, IID_ILabels, (void**)&_labels);
    CoCreateInstance(CLSID_Charts, nullptr, CLSCTX_INPROC_SERVER, IID_ICharts, (void**)&_charts);
    CoCreateInstance(CLSID_GeoProjection, nullptr, CLSCTX_INPROC_SERVER, IID_IGeoProjection, (void**)&_geoProjection);

    this->put_ReferenceToLabels();
    this->put_ReferenceToCategories();
    this->put_ReferenceToCharts();

    ComHelper::CreateInstance(idUndoList, (IDispatch**)&_undoList);

    gReferenceCounter.AddRef(tkInterface::idShapefile);
}

CShapefile::~CShapefile()
{
    VARIANT_BOOL vbretval;
    this->CShapefile::Close(&vbretval);

    SysFreeString(_key);
    SysFreeString(_expression);
    SysFreeString(_sortField);

    if (_selectDrawOpt != nullptr)
        _selectDrawOpt->Release();

    if (_defaultDrawOpt != nullptr)
        _defaultDrawOpt->Release();

    if (_labels != nullptr)
    {
        put_ReferenceToLabels(true); // labels class maybe referenced by client and won't be deleted as a result
        _labels->Release(); // therefore we must clear the reference to the parent as it will be invalid
    }

    if (_categories != nullptr)
    {
        put_ReferenceToCategories(true);
        _categories->Release();
    }

    if (_charts != nullptr)
    {
        put_ReferenceToCharts(true);
        _charts->Release();
    }

    if (_stopExecution)
        _stopExecution->Release();

    if (_geoProjection)
        _geoProjection->Release();

    if (_undoList)
    {
        _undoList->Release();
    }
    gReferenceCounter.Release(tkInterface::idShapefile);
}

std::vector<ShapeRecord*>* CShapefile::get_ShapeVector()
{
    return &_shapeData;
}

IShapeWrapper* CShapefile::get_ShapeWrapper(int ShapeIndex)
{
    return ((CShape*)_shapeData[ShapeIndex]->shape)->get_ShapeWrapper();
}

IShapeData* CShapefile::get_ShapeRenderingData(int ShapeIndex)
{
    return _shapeData[ShapeIndex]->get_RenderingData();
}

void CShapefile::put_ShapeRenderingData(int ShapeIndex, CShapeData* data)
{
    return _shapeData[ShapeIndex]->put_RenderingData(data);
}

void CShapefile::SetValidationInfo(IShapeValidationInfo* info, tkShapeValidationType validationType)
{
    ComHelper::SetRef(info,
                      (IDispatch**)&(validationType == svtInput ? _inputValidation : _outputValidation), true);
}


// give OGR layers the ability to retain visibility flags on reload
bool CShapefile::GetVisibilityFlags(map<long, BYTE> &flags)
{
    // can only perform if we have an FID mapping to ShapeID
    if (!_hasOgrFidMapping)
        return false;

    // copy visibility flags into provided map, keyed by OGR_FID

    // iterate all OGR Mappings
    auto iter = _ogrFid2ShapeIndex.begin();
    while (iter != _ogrFid2ShapeIndex.end())
    {
        BYTE visibilityFlags = 0;
        // get currently-assocaited ShapeID
        long shapeID = _ogrFid2ShapeIndex[iter->first];
        // only consider Hidden and Selected values, as others are not intended to be persisted
        if (_shapeData[shapeID]->hidden()) visibilityFlags |= tkShapeRecordFlags::shpHidden;
        if (_shapeData[shapeID]->selected()) visibilityFlags |= tkShapeRecordFlags::shpSelected;
        // set into mapping, mapping OGR_FID to visibility flags
        flags.insert(std::make_pair(iter->first, visibilityFlags));
        iter++;
    }
    return true;
}
bool CShapefile::SetVisibilityFlags(map<long, BYTE> &flags)
{
    // can only perform if we have an FID mapping to ShapeID
    if (!_hasOgrFidMapping)
        return false;

    // copy the specified visibility flags into the current shape data

    // iterate all OGR Mappings
    auto iter = _ogrFid2ShapeIndex.begin();
    while (iter != _ogrFid2ShapeIndex.end())
    {
        // does FID still exist (from the previous mapping)?
        if (flags.find(iter->first) != flags.end())
        {
            BYTE visibilityFlags = flags[iter->first];
            // get currently-assocaited ShapeID
            long shapeID = _ogrFid2ShapeIndex[iter->first];
            // only consider Hidden and Selected values, as others are not intended to be persisted
            _shapeData[shapeID]->hidden((visibilityFlags & tkShapeRecordFlags::shpHidden) ? true : false);
            _shapeData[shapeID]->selected((visibilityFlags & tkShapeRecordFlags::shpSelected) ? true : false);
        }
        iter++;
    }
    return true;
}

#pragma region Properties

// ************************************************************
//		get_EditingShapes()
// ************************************************************
STDMETHODIMP CShapefile::get_EditingShapes(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *pVal = _isEditingShapes ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
}

// ************************************************************
//		get_LastErrorCode()
// ************************************************************
STDMETHODIMP CShapefile::get_LastErrorCode(long* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *pVal = _lastErrorCode;
    _lastErrorCode = tkNO_ERROR;
    return S_OK;
}

// ************************************************************
//		get_CdlgFilter()
// ************************************************************
STDMETHODIMP CShapefile::get_CdlgFilter(BSTR* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    USES_CONVERSION;
    *pVal = A2BSTR("ESRI Shapefiles (*.shp)|*.shp");
    return S_OK;
}

// ************************************************************
//		LastInputValidation
// ************************************************************
STDMETHODIMP CShapefile::get_LastInputValidation(IShapeValidationInfo** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    if (_inputValidation)
        _inputValidation->AddRef();
    *retVal = _inputValidation;
    return S_OK;
}

// ************************************************************
//		LastOutputValidation
// ************************************************************
STDMETHODIMP CShapefile::get_LastOutputValidation(IShapeValidationInfo** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    if (_outputValidation)
        _outputValidation->AddRef();
    *retVal = _outputValidation;
    return S_OK;
}

// ************************************************************
//		get/put_GlobalCallback()
// ************************************************************
STDMETHODIMP CShapefile::get_GlobalCallback(ICallback** pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    *pVal = _globalCallback;
    if (_globalCallback != nullptr)
        _globalCallback->AddRef();
    return S_OK;
}

STDMETHODIMP CShapefile::put_GlobalCallback(ICallback* newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
    if (_table != nullptr)
        _table->put_GlobalCallback(newVal);

    return S_OK;
}

// ************************************************************
//		StopExecution
// ************************************************************
STDMETHODIMP CShapefile::put_StopExecution(IStopExecution* stopper)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    ComHelper::SetRef((IDispatch*)stopper, (IDispatch**)&_stopExecution, true);
    return S_OK;
}

// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CShapefile::get_Key(BSTR* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    USES_CONVERSION;
    *pVal = OLE2BSTR(_key);
    return S_OK;
}

STDMETHODIMP CShapefile::put_Key(BSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    SysFreeString(_key);
    _key = OLE2BSTR(newVal);
    return S_OK;
}

// ************************************************************
//		get/put_VisibilityExpression
// ************************************************************
STDMETHODIMP CShapefile::get_VisibilityExpression(BSTR* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    USES_CONVERSION;
    *pVal = OLE2BSTR(_expression);
    return S_OK;
}

STDMETHODIMP CShapefile::put_VisibilityExpression(BSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    SysFreeString(_expression);
    _expression = OLE2BSTR(newVal);
    return S_OK;
}

// ************************************************************
//		get/put_Volatile
// ************************************************************
STDMETHODIMP CShapefile::get_Volatile(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    if (_interactiveEditing)
    {
        *pVal = VARIANT_TRUE;
    }
    else
    {
        *pVal = _volatile ? VARIANT_TRUE : VARIANT_FALSE;
    }
    return S_OK;
}

STDMETHODIMP CShapefile::put_Volatile(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    _volatile = newVal == VARIANT_TRUE;
    return S_OK;
}

// *****************************************************************
//		get_NumShapes()
// *****************************************************************
STDMETHODIMP CShapefile::get_NumShapes(long* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *pVal = _shapeData.size(); //_numShapes;
    return S_OK;
}

// **************************************************************
//		get_NumFields()
// **************************************************************
STDMETHODIMP CShapefile::get_NumFields(long* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    if (_table != nullptr)
        _table->get_NumFields(pVal);
    else
    {
        ErrorMessage(tkFILE_NOT_OPEN);
        *pVal = 0;
    }
    return S_OK;
}

// ************************************************************
//		get_ShapefileType()
// ************************************************************
STDMETHODIMP CShapefile::get_ShapefileType(ShpfileType* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *pVal = _shpfiletype;
    return S_OK;
}

// *****************************************************************
//	   get_ErrorMsg()
// *****************************************************************
STDMETHODIMP CShapefile::get_ErrorMsg(long ErrorCode, BSTR* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    USES_CONVERSION;
    *pVal = A2BSTR(ErrorMsg(ErrorCode));
    return S_OK;
}

// *****************************************************************
//	   get_FileHandle()
// *****************************************************************
STDMETHODIMP CShapefile::get_FileHandle(long* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    if (_shpfile != nullptr)
    {
        const int handle = _fileno(_shpfile);
        *pVal = _dup(handle);
    }
    else
        *pVal = -1;

    return S_OK;
}

// **************************************************************
//	   get_Filename()
// **************************************************************
STDMETHODIMP CShapefile::get_Filename(BSTR* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    *pVal = W2BSTR(_shpfileName);

    return S_OK;
}

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CShapefile::ErrorMessage(long ErrorCode)
{
    _lastErrorCode = ErrorCode;
    CallbackHelper::ErrorMsg("Shapefile", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

void CShapefile::ErrorMessage(long ErrorCode, ICallback* cBack)
{
    _lastErrorCode = ErrorCode;
    CallbackHelper::ErrorMsg("Shapefile", _globalCallback, _key, ErrorMsg(_lastErrorCode));
    if (cBack != _globalCallback)
        CallbackHelper::ErrorMsg("Shapefile", cBack, _key, ErrorMsg(_lastErrorCode));
}

// ************************************************************
//		get_MinDrawingSize()
// ************************************************************
STDMETHODIMP CShapefile::get_MinDrawingSize(LONG* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _minDrawingSize;
    return S_OK;
}

STDMETHODIMP CShapefile::put_MinDrawingSize(LONG newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _minDrawingSize = newVal;
    return S_OK;
}

// ************************************************************
//		get_SourceType()
// ************************************************************
STDMETHODIMP CShapefile::get_SourceType(tkShapefileSourceType* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _sourceType;
    return S_OK;
}

#pragma endregion

#pragma region CreateAndOpen


// ************************************************************
//		LoadDataFrom()
// ************************************************************
// Loads shape and DBF data from disk file into in-memory mode
STDMETHODIMP CShapefile::LoadDataFrom(BSTR ShapefileName, ICallback* cBack, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    USES_CONVERSION;
    *retval = VARIANT_FALSE;
    if (_sourceType != sstInMemory)
    {
        ErrorMessage(tkINMEMORY_SHAPEFILE_EXPECTED);
        return S_OK;
    }

    if (OpenCore(OLE2CA(ShapefileName), cBack))
    {
        // loading data in-memory
        VARIANT_BOOL vb;
        _isEditingShapes = false;
        StartEditingShapes(VARIANT_TRUE, cBack, &vb);

        // this will trigger loading of all DBF values into the memory
        long numFields;
        this->get_NumFields(&numFields);
        if (numFields > 0)
        {
            CComVariant var;
            for (size_t i = 0; i < _shapeData.size(); i++)
            {
                _table->get_CellValue(0, i, &var);
            }
        }

        // closing disk file despite the result success or failure
        _shpfileName = "";
        _shxfileName = "";
        _dbffileName = "";

        if (_shpfile != nullptr)
            fclose(_shpfile);
        _shpfile = nullptr;

        if (_shxfile != nullptr)
            fclose(_shxfile);
        _shxfile = nullptr;

        if (_table != nullptr)
            ((CTableClass*)_table)->CloseUnderlyingFile();

        *retval = vb;
    }
    return S_OK;
}

// ************************************************************
//		OpenCore()
// ************************************************************
bool CShapefile::OpenCore(CStringW tmpShpfileName, ICallback* cBack)
{
    USES_CONVERSION;
    VARIANT_BOOL vbretval;

    // saving the provided names; 
    // from now on we must clean the class variables in case the operation won't succeed
    _shpfileName = tmpShpfileName;
    _shxfileName = tmpShpfileName.Left(tmpShpfileName.GetLength() - 3) + L"shx";
    _dbffileName = tmpShpfileName.Left(tmpShpfileName.GetLength() - 3) + L"dbf";
    _prjfileName = tmpShpfileName.Left(tmpShpfileName.GetLength() - 3) + L"prj";

    // read mode
    _shpfile = _wfopen(_shpfileName, L"rb");
    _shxfile = _wfopen(_shxfileName, L"rb");

    // opening DBF
    if (!_table)
    {
        CoCreateInstance(CLSID_Table, nullptr, CLSCTX_INPROC_SERVER, IID_ITable, (void**)&_table);
    }
    else
    {
        VARIANT_BOOL vb;
        _table->Close(&vb);
    }

    _table->put_GlobalCallback(_globalCallback);
    ((CTableClass*)_table)->InjectShapefile(this);

    const CComBSTR bstrDbf(_dbffileName);
    _table->Open(bstrDbf, cBack, &vbretval);

    if (_shpfile == nullptr)
    {
        ErrorMessage(tkCANT_OPEN_SHP);
        this->Close(&vbretval);
    }
    else if (_shxfile == nullptr)
    {
        ErrorMessage(tkCANT_OPEN_SHX);
        this->Close(&vbretval);
    }
    else if (vbretval == VARIANT_FALSE)
    {
        _table->get_LastErrorCode(&_lastErrorCode);
        ErrorMessage(_lastErrorCode);
        this->Close(&vbretval);
    }
    else
    {
        if (!ReadShx()) // shapefile header is read here as well
        {
            ErrorMessage(tkINVALID_SHX_FILE);
            this->Close(&vbretval);
        }
        else
        {
            //Check for supported types
            if (_shpfiletype != SHP_NULLSHAPE &&
                _shpfiletype != SHP_POINT &&
                _shpfiletype != SHP_POLYLINE &&
                _shpfiletype != SHP_POLYGON &&
                _shpfiletype != SHP_POINTZ &&
                _shpfiletype != SHP_POLYLINEZ &&
                _shpfiletype != SHP_POLYGONZ &&
                _shpfiletype != SHP_MULTIPOINT &&
                _shpfiletype != SHP_MULTIPOINTZ &&
                _shpfiletype != SHP_POLYLINEM &&
                _shpfiletype != SHP_POLYGONM &&
                _shpfiletype != SHP_POINTM &&
                _shpfiletype != SHP_MULTIPOINTM)
            {
                ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
                this->Close(&vbretval);
            }
            else
            {
                _shapeData.reserve(_shpOffsets.size());
                for (size_t i = 0; i < _shpOffsets.size(); i++)
                {
                    _shapeData.push_back(new ShapeRecord());
                }
                return true;
            }
        }
    }
    return false;
}

// ************************************************************
//		Open()
// ************************************************************
STDMETHODIMP CShapefile::Open(BSTR ShapefileName, ICallback* cBack, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *retval = VARIANT_FALSE;
    VARIANT_BOOL vbretval;

    if (_globalCallback == nullptr && cBack != nullptr)
    {
        _globalCallback = cBack;
        _globalCallback->AddRef();
    }

    USES_CONVERSION;
    CStringW tmp_shpfileName = OLE2CW(ShapefileName);

    if (tmp_shpfileName.GetLength() == 0)
    {
        // better to use CreateNew directly, but this call will be preserved for backward compatibility
        this->CreateNew(m_globalSettings.emptyBstr, _shpfiletype, &vbretval);
    }
    else if (tmp_shpfileName.GetLength() <= 3)
    {
        ErrorMessage(tkINVALID_FILENAME);
    }
    else
    {
        // close the opened shapefile
        this->Close(&vbretval);

        if (vbretval == VARIANT_FALSE)
        {
            // error code in the function
            return S_OK;
        }

        if (OpenCore(tmp_shpfileName, cBack))
        {
            _sourceType = sstDiskBased;

            // reading projection
            const CComBSTR bstrPrj(_prjfileName);
            _geoProjection->ReadFromFileEx(bstrPrj, VARIANT_TRUE, &vbretval);

            ShapeStyleHelper::ApplyRandomDrawingOptions(this);
            LabelsHelper::UpdateLabelsPositioning(this);
            *retval = VARIANT_TRUE;
        }
    }
    return S_OK;
}

// *********************************************************
//		CreateNew()
// *********************************************************
STDMETHODIMP CShapefile::CreateNew(BSTR ShapefileName, ShpfileType ShapefileType, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    return this->CreateNewCore(ShapefileName, ShapefileType, true, retval);
}

// *********************************************************
//		CreateNewCore()
// *********************************************************
HRESULT CShapefile::CreateNewCore(BSTR ShapefileName, ShpfileType ShapefileType, bool applyRandomOptions,
                                  VARIANT_BOOL* retval)
{
    *retval = VARIANT_FALSE;
    VARIANT_BOOL vb;

    // check for supported types
    if (ShapefileType != SHP_NULLSHAPE &&
        ShapefileType != SHP_POINT &&
        ShapefileType != SHP_POLYLINE &&
        ShapefileType != SHP_POLYGON &&
        ShapefileType != SHP_POINTZ &&
        ShapefileType != SHP_POLYLINEZ &&
        ShapefileType != SHP_POLYGONZ &&
        ShapefileType != SHP_MULTIPOINT &&
        ShapefileType != SHP_MULTIPOINTZ &&
        ShapefileType != SHP_POINTM && // MWGIS-69
        ShapefileType != SHP_POLYLINEM &&
        ShapefileType != SHP_POLYGONM &&
        ShapefileType != SHP_MULTIPOINTM)
    {
        ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
        return S_OK;
    }

    USES_CONVERSION;
    CString tmp_shpfileName = OLE2CA(ShapefileName);

    // ----------------------------------------------
    // in memory shapefile (without name)
    // ----------------------------------------------
    if (tmp_shpfileName.GetLength() == 0)
    {
        // closing the old shapefile (error code inside the function)
        Close(&vb);

        if (vb == VARIANT_TRUE)
        {
            CoCreateInstance(CLSID_Table, nullptr, CLSCTX_INPROC_SERVER, IID_ITable, (void**)&_table);

            _table->CreateNew(m_globalSettings.emptyBstr, &vb);

            if (!vb)
            {
                long error;
                _table->get_LastErrorCode(&error);
                ErrorMessage(error);
                _table->Release();
                _table = nullptr;
            }
            else
            {

                ((CTableClass*)_table)->InjectShapefile(this);
                _shpfiletype = ShapefileType;
                _isEditingShapes = true;
                _sourceType = sstInMemory;

                if (applyRandomOptions)
                {
                    ShapeStyleHelper::ApplyRandomDrawingOptions(this);
                }

                *retval = VARIANT_TRUE;
            }
        }

        return S_OK;
    }

    if (tmp_shpfileName.GetLength() <= 3)
    {
        ErrorMessage(tkINVALID_FILENAME);
        return S_OK;
    }

    const CString& shpName = tmp_shpfileName;
    const CString shxName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "shx";
    const CString dbfName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "dbf";
    const CString prjName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "prj";

    // new file is created, so there must not be any files with this names
    if (Utility::FileExists(shpName) != FALSE)
    {
        ErrorMessage(tkSHP_FILE_EXISTS);
        return S_OK;
    }
    if (Utility::FileExists(shxName) != FALSE)
    {
        ErrorMessage(tkSHX_FILE_EXISTS);
        return S_OK;
    }
    if (Utility::FileExists(dbfName) != FALSE)
    {
        ErrorMessage(tkDBF_FILE_EXISTS);
        return S_OK;
    }
    if (Utility::FileExists(prjName) != FALSE)
    {
        ErrorMessage(tkPRJ_FILE_EXISTS);
        return S_OK;
    }

    // closing the old shapefile (error code inside the function)
    this->Close(&vb);

    if (vb == VARIANT_TRUE)
    {
        CoCreateInstance(CLSID_Table, nullptr, CLSCTX_INPROC_SERVER, IID_ITable, (void**)&_table);

        _table->put_GlobalCallback(_globalCallback);

        const CString newDbfName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "dbf";
        const CComBSTR bstrName(newDbfName);
        _table->CreateNew(bstrName, &vb);

        if (!vb)
        {
            _table->get_LastErrorCode(&_lastErrorCode);
            ErrorMessage(_lastErrorCode);
            _table->Release();
            _table = nullptr;
        }
        else
        {
            ((CTableClass*)_table)->InjectShapefile(this);
            _shpfileName = tmp_shpfileName;
            _shxfileName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "shx";
            _dbffileName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "dbf";
            _prjfileName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "prj";

            _shpfiletype = ShapefileType;
            _isEditingShapes = true;
            _sourceType = sstInMemory;

            if (applyRandomOptions)
            {
                ShapeStyleHelper::ApplyRandomDrawingOptions(this);
            }

            *retval = VARIANT_TRUE;
        }
    }

    LabelsHelper::UpdateLabelsPositioning(this);

    return S_OK;
}

// *********************************************************
//		CreateNewWithShapeID()
// *********************************************************
STDMETHODIMP CShapefile::CreateNewWithShapeID(BSTR ShapefileName, ShpfileType ShapefileType, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    USES_CONVERSION;

    CreateNew(ShapefileName, ShapefileType, retval);

    if (*retval)
        ShapefileHelper::InsertMwShapeIdField(this);

    return S_OK;
}

#pragma endregion

#pragma region SaveAndClose
// *****************************************************************
//		Close()
// *****************************************************************
STDMETHODIMP CShapefile::Close(VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (_appendMode)
    {
        StopAppendMode();
    }

    ClearCachedGeometries();

    if (_isEditingShapes)
    {
        // just stop editing shapes, if the shape is in open status
        this->StopEditingShapes(VARIANT_FALSE, VARIANT_TRUE, nullptr, retval);
    }

    // stop editing table in case only it have been edited
    VARIANT_BOOL isEditingTable = VARIANT_FALSE;
    if (_table)
    {
        _table->get_EditingTable(&isEditingTable);
        if (isEditingTable)
        {
            this->StopEditingTable(VARIANT_FALSE, _globalCallback, retval);
            _table->get_EditingTable(&isEditingTable);
        }
    }

    // removing shape data
    this->ReleaseMemoryShapes();
    for (auto& i : _shapeData)
    {
        delete i;
    }
    _shapeData.clear();

    if (_spatialIndexLoaded)
        IndexSearching::unloadSpatialIndex(_spatialIndexID);

    _sourceType = sstUninitialized;
    _shpfiletype = SHP_NULLSHAPE;
    LabelsHelper::UpdateLabelsPositioning(this);

    _shpfileName = "";
    _shxfileName = "";
    _dbffileName = "";

    _minX = 0.0;
    _minY = 0.0;
    _minZ = 0.0;
    _maxX = 0.0;
    _maxY = 0.0;
    _maxZ = 0.0;
    _minM = 0.0;
    _maxM = 0.0;

    if (_inputValidation != nullptr)
    {
        _inputValidation->Release();
        _inputValidation = nullptr;
    }

    if (_outputValidation != nullptr)
    {
        _outputValidation->Release();
        _outputValidation = nullptr;
    }

    if (_shpfile != nullptr) fclose(_shpfile);
    _shpfile = nullptr;

    if (_shxfile != nullptr) fclose(_shxfile);
    _shxfile = nullptr;

    if (_table != nullptr)
    {
        VARIANT_BOOL vbretval;
        _table->Close(&vbretval);
        _table->Release();
        _table = nullptr;
    }
    if (_labels)
    {
        _labels->Clear();
        _labels->ClearCategories();
    }
    if (_charts)
    {
        _charts->Clear();
    }
    if (_categories)
    {
        _categories->Clear();
    }
    *retval = VARIANT_TRUE;

    return S_OK;
}

// **********************************************************
//		Dump()
// **********************************************************
//Saves shapefile without reopening it in a new location
STDMETHODIMP CShapefile::Dump(BSTR ShapefileName, ICallback* cBack, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *retval = VARIANT_FALSE;

    const bool callbackIsNull = _globalCallback == nullptr;
    if (_globalCallback == nullptr && cBack != nullptr)
    {
        _globalCallback = cBack;
        _globalCallback->AddRef();
    }

    if (_table == nullptr || _sourceType == sstUninitialized)
    {
        ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
        return S_OK;
    }

    // in case someone else is writing, we leave
    if (_writing)
    {
        ErrorMessage(tkSHP_WRITE_VIOLATION);
        return S_OK;
    }

    if (!this->ValidateOutput(this, "Dump", "Shapefile", false))
        return S_OK;

    USES_CONVERSION;
    CString sa_shpfileName = OLE2CA(ShapefileName);

    if (sa_shpfileName.GetLength() <= 3)
    {
        ErrorMessage(tkINVALID_FILENAME);
    }
    else
    {
        const CString sa_shxfileName = sa_shpfileName.Left(sa_shpfileName.GetLength() - 3) + "shx";
        CString sa_dbffileName = sa_shpfileName.Left(sa_shpfileName.GetLength() - 3) + "dbf";

        // -----------------------------------------------
        // it's not allowed to rewrite the existing files
        // -----------------------------------------------
        if (Utility::FileExists(sa_shpfileName))
        {
            ErrorMessage(tkSHP_FILE_EXISTS);
            return S_OK;
        }
        if (Utility::FileExists(sa_shxfileName))
        {
            ErrorMessage(tkSHX_FILE_EXISTS);
            return S_OK;
        }
        if (Utility::FileExists(sa_dbffileName))
        {
            ErrorMessage(tkDBF_FILE_EXISTS);
            return S_OK;
        }

        // -----------------------------------------------
        //   checking in-memory shapes
        // -----------------------------------------------
        if (_isEditingShapes)
        {
            if (VerifyMemShapes(cBack) == FALSE)
            {
                // error Code is set in function
                return S_OK;
            }

            // refresh extents
            VARIANT_BOOL retVal;
            this->RefreshExtents(&retVal);
        }

        // -----------------------------------------------
        // opening files
        // -----------------------------------------------
        FILE* shpfile = fopen(sa_shpfileName, "wb+");
        if (shpfile == nullptr)
        {
            ErrorMessage(tkCANT_CREATE_SHP);
            return S_OK;
        }

        //shx
        FILE* shxfile = fopen(sa_shxfileName, "wb+");
        if (shxfile == nullptr)
        {
            fclose(shpfile);
            ErrorMessage(tkCANT_CREATE_SHX);
            return S_OK;
        }

        // ------------------------------------------------
        //	writing the files
        // ------------------------------------------------
        this->WriteShp(shpfile, cBack);
        this->WriteShx(shxfile, cBack);

        fclose(shpfile);
        fclose(shxfile);

        // ------------------------------------------------
        //	saving dbf table
        // ------------------------------------------------
        _table->Dump(sa_dbffileName.AllocSysString(), cBack, retval);
        if (*retval == FALSE)
        {
            _table->get_LastErrorCode(&_lastErrorCode);
            return S_OK;
        }

        // saving projection in new format
        VARIANT_BOOL vbretval;
        const CStringW prjfileName = sa_shpfileName.Left(sa_shpfileName.GetLength() - 3) + L"prj";
        const CComBSTR bstr(prjfileName);
        _geoProjection->WriteToFileEx(bstr, VARIANT_TRUE, &vbretval);

        *retval = VARIANT_TRUE;
    }

    // restoring callback
    if (callbackIsNull)
    {
        _globalCallback = nullptr;
    }

    return S_OK;
}

// **********************************************************
//		SaveAs()
// **********************************************************
// Saves shapefile to the new or the same location. Doesn't stop editing mode.
STDMETHODIMP CShapefile::SaveAs(BSTR ShapefileName, ICallback* cBack, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *retval = VARIANT_FALSE;

    const bool callbackIsNull = _globalCallback == nullptr;
    if (_globalCallback == nullptr && cBack != nullptr)
    {
        _globalCallback = cBack;
        _globalCallback->AddRef();
    }

    if (_table == nullptr || _sourceType == sstUninitialized)
    {
        ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
        return S_OK;
    }

    // in case someone else is writing, we leave
    if (_writing)
    {
        ErrorMessage(tkSHP_WRITE_VIOLATION);
        return S_OK;
    }

    if (!this->ValidateOutput(this, "SaveAs", "Shapefile", false))
        return S_OK;

    USES_CONVERSION;
    CStringW newShpName = OLE2W(ShapefileName);

    if (newShpName.GetLength() <= 3)
    {
        ErrorMessage(tkINVALID_FILENAME);
    }
    else if (!Utility::EndsWith(newShpName, L".shp"))
    {
        ErrorMessage(tkINVALID_FILE_EXTENSION);
    }
    else
    {
        const CStringW newShxName = newShpName.Left(newShpName.GetLength() - 3) + L"shx";
        const CStringW newDbfName = newShpName.Left(newShpName.GetLength() - 3) + L"dbf";

        // -----------------------------------------------
        // it's not allowed to rewrite the existing files
        // -----------------------------------------------
        if (Utility::FileExistsW(newShpName))
        {
            ErrorMessage(tkSHP_FILE_EXISTS);
            return S_OK;
        }
        if (Utility::FileExistsW(newShxName))
        {
            ErrorMessage(tkSHX_FILE_EXISTS);
            return S_OK;
        }
        if (Utility::FileExistsW(newDbfName))
        {
            ErrorMessage(tkDBF_FILE_EXISTS);
            return S_OK;
        }

        // -----------------------------------------------
        //   checking in-memory shapes
        // -----------------------------------------------
        if (_isEditingShapes)
        {
            if (VerifyMemShapes(cBack) == FALSE)
            {
                // error Code is set in function
                return S_OK;
            }

            // refresh extents
            VARIANT_BOOL retVal;
            RefreshExtents(&retVal);
        }

        // -----------------------------------------------
        // opening files
        // -----------------------------------------------
        FILE* shpfile = _wfopen(newShpName, L"wb+");
        if (shpfile == nullptr)
        {
            ErrorMessage(tkCANT_CREATE_SHP);
            return S_OK;
        }

        //shx
        FILE* shxfile = _wfopen(newShxName, L"wb+");
        if (shxfile == nullptr)
        {
            fclose(shpfile);
            ErrorMessage(tkCANT_CREATE_SHX);
            return S_OK;
        }

        // ------------------------------------------------
        //	writing the files
        // ------------------------------------------------
        WriteShp(shpfile, cBack);
        WriteShx(shxfile, cBack);

        fclose(shpfile);
        fclose(shxfile);

        // ------------------------------------------------
        //	saving dbf table
        // ------------------------------------------------
        const CComBSTR bstrDbf(newDbfName);
        _table->SaveAs(bstrDbf, cBack, retval);

        if (*retval != VARIANT_TRUE)
        {
            _table->get_LastErrorCode(&_lastErrorCode);
            _wunlink(newShpName);
            _wunlink(newShxName);
            return S_OK;
        }

        // -------------------------------------------------
        //	switching to the new files
        // -------------------------------------------------
        shpfile = _wfopen(newShpName, L"rb");
        shxfile = _wfopen(newShxName, L"rb");

        if (_shpfile != nullptr) fclose(_shpfile);
        _shpfile = shpfile;

        if (_shxfile != nullptr) fclose(_shxfile);
        _shxfile = shxfile;

        // update all filenames:
        _shpfileName = newShpName; // saving of shp filename should be done before writing the projection;
        _shxfileName = newShxName; // otherwise projection string will be written to the memory
        _dbffileName = newDbfName;
        _prjfileName = newShpName.Left(newShpName.GetLength() - 3) + L"prj";

        _sourceType = sstDiskBased;

        // saving projection in new format
        VARIANT_BOOL vbretval, isEmpty;
        _geoProjection->get_IsEmpty(&isEmpty);
        if (!isEmpty)
        {
            const CComBSTR bstr(_prjfileName);
            _geoProjection->WriteToFileEx(bstr, VARIANT_TRUE, &vbretval);
        }

        if (_useQTree)
            GenerateQTree();

        *retval = VARIANT_TRUE;
    }

    // restoring callback
    if (callbackIsNull)
    {
        _globalCallback = nullptr;
    }

    return S_OK;
}

// **************************************************************
//		Save()
// **************************************************************
// saving without exiting edit mode
STDMETHODIMP CShapefile::Save(ICallback* cBack, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *retval = VARIANT_FALSE;

    if (_globalCallback == nullptr && cBack != nullptr)
    {
        _globalCallback = cBack;
        _globalCallback->AddRef();
    }

    // no edits were made; it doesn't make sense to save it
    if (_isEditingShapes == FALSE)
    {
        ErrorMessage(tkSHPFILE_NOT_IN_EDIT_MODE);
        return S_OK;
    }

    if (VerifyMemShapes(_globalCallback) == FALSE)
    {
        // error Code is set in function
        return S_OK;
    }

    if (!this->ValidateOutput(this, "Save", "Shapefile", false))
        return S_OK;

    // compute the extents
    VARIANT_BOOL res;
    RefreshExtents(&res);

    // -------------------------------------------------
    //	Reopen the files in the write mode
    // -------------------------------------------------
    if (_shpfile && _shxfile)
    {
        _shpfile = _wfreopen(_shpfileName, L"wb+", _shpfile);
        _shxfile = _wfreopen(_shxfileName, L"wb+", _shxfile);
    }
    else
    {
        _shpfile = _wfopen(_shpfileName, L"wb+");
        _shxfile = _wfopen(_shxfileName, L"wb+");
    }

    if (_shpfile == nullptr || _shxfile == nullptr)
    {
        if (_shxfile != nullptr)
        {
            fclose(_shxfile);
            _shxfile = nullptr;
            _lastErrorCode = tkCANT_OPEN_SHX;
        }
        if (_shpfile != nullptr)
        {
            fclose(_shpfile);
            _shpfile = nullptr;
            _lastErrorCode = tkCANT_OPEN_SHP;
        }
        *retval = FALSE;

        ErrorMessage(_lastErrorCode);
    }
    else
    {
        _writing = true;

        // -------------------------------------------------
        //	Writing the files
        // -------------------------------------------------
        WriteShp(_shpfile, cBack);
        WriteShx(_shxfile, cBack);

        if (_useQTree)
            GenerateQTree();

        // -------------------------------------------------
        //	Reopen the updated files
        // -------------------------------------------------
        _shpfile = _wfreopen(_shpfileName, L"rb+", _shpfile);
        _shxfile = _wfreopen(_shxfileName, L"rb+", _shxfile);

        if (_shpfile == nullptr || _shxfile == nullptr)
        {
            if (_shxfile != nullptr)
            {
                fclose(_shxfile);
                _shxfile = nullptr;
                _lastErrorCode = tkCANT_OPEN_SHX;
            }
            if (_shpfile != nullptr)
            {
                fclose(_shpfile);
                _shpfile = nullptr;
                _lastErrorCode = tkCANT_OPEN_SHP;
            }
            *retval = FALSE;

            ErrorMessage(_lastErrorCode);
        }
        else
        {
            //Save the table file
            _table->Save(cBack, retval);

            _sourceType = sstDiskBased;

            // saving projection in new format
            VARIANT_BOOL vbretval, isEmpty;
            _geoProjection->get_IsEmpty(&isEmpty);
            if (!isEmpty)
            {
                const CComBSTR bstr(_prjfileName);
                _geoProjection->WriteToFileEx(bstr, VARIANT_TRUE, &vbretval);
            }

            *retval = VARIANT_TRUE;
        }
    }

    _writing = false;
    return S_OK;
}

// ************************************************************
//		Resource()
// ************************************************************
STDMETHODIMP CShapefile::Resource(BSTR newShpPath, VARIANT_BOOL* retval)
{
    USES_CONVERSION;
    Close(retval);
    Open(newShpPath, nullptr, retval);
    return S_OK;
}
#pragma endregion

// ***********************************************************************
//		Clone()
// ***********************************************************************
//  Creates new shapefile with the same type and fields as existing one
STDMETHODIMP CShapefile::Clone(IShapefile** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    ShapefileHelper::CloneCore(this, retVal, _shpfiletype);
    return S_OK;
}

// ************************************************************
//		get_Extents()
// ************************************************************
STDMETHODIMP CShapefile::get_Extents(IExtents** pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    IExtents* bBox = nullptr;
    ComHelper::CreateExtents(&bBox);

    // Extents could change because of the moving of points of a single shape
    // It's difficult to track such changes, so we still need to recalculate them 
    // here to enforce proper drawing; _fastMode mode - for those who want 
    // to call refresh extents theirselfs
    if (!_fastMode)
    {
        VARIANT_BOOL vbretval;
        this->RefreshExtents(&vbretval);
    }

    bBox->SetBounds(_minX, _minY, _minZ, _maxX, _maxY, _maxZ);
    bBox->SetMeasureBounds(_minM, _maxM);
    *pVal = bBox;

    return S_OK;
}

#pragma region AttributeTable
// ****************************************************************
//	  EditInsertField()
// ****************************************************************
STDMETHODIMP CShapefile::EditInsertField(IField* NewField, long* FieldIndex, ICallback* cBack, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (cBack == nullptr && _globalCallback != nullptr)
        cBack = _globalCallback;

    if (_table != nullptr)
    {
        _table->EditInsertField(NewField, FieldIndex, cBack, retval);
    }
    else
    {
        *retval = VARIANT_FALSE;
        _lastErrorCode = tkFILE_NOT_OPEN;
        ErrorMessage(_lastErrorCode, cBack);
        return S_OK;
    }

    if (*retval == VARIANT_FALSE)
    {
        _table->get_LastErrorCode(&_lastErrorCode);
        *retval = VARIANT_FALSE;
    }

    return S_OK;
}

// ****************************************************************
//	  EditDeleteField()
// ****************************************************************
STDMETHODIMP CShapefile::EditDeleteField(long FieldIndex, ICallback* cBack, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (_globalCallback == nullptr && cBack != nullptr)
    {
        _globalCallback = cBack;
        _globalCallback->AddRef();
    }

    if (_table != nullptr)
    {
        _table->EditDeleteField(FieldIndex, cBack, retval);
    }
    else
    {
        *retval = VARIANT_FALSE;
        ErrorMessage(tkFILE_NOT_OPEN);
        return S_OK;
    }

    if (*retval == VARIANT_FALSE)
    {
        _table->get_LastErrorCode(&_lastErrorCode);
        *retval = VARIANT_FALSE;
    }

    return S_OK;
}

// ****************************************************************
//	  EditCellValue()
// ****************************************************************
STDMETHODIMP CShapefile::EditCellValue(long FieldIndex, long ShapeIndex, VARIANT NewVal, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (_table != nullptr)
    {
        _table->EditCellValue(FieldIndex, ShapeIndex, NewVal, retval);
    }
    else
    {
        *retval = VARIANT_FALSE;
        ErrorMessage(tkFILE_NOT_OPEN);
        return S_OK;
    }

    if (*retval == VARIANT_FALSE)
    {
        _table->get_LastErrorCode(&_lastErrorCode);
    }

    return S_OK;
}

// ****************************************************************
//	  StartEditingTable()
// ****************************************************************
STDMETHODIMP CShapefile::StartEditingTable(ICallback* cBack, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (_appendMode)
    {
        ErrorMessage(tkDBF_NO_EDIT_MODE_WHEN_APPENDING);
        *retval = VARIANT_FALSE;
        return S_OK;
    }

    if (_globalCallback == nullptr && cBack != nullptr)
    {
        _globalCallback = cBack;
        _globalCallback->AddRef();
    }

    if (_table != nullptr)
    {
        _table->StartEditingTable(cBack, retval);
    }
    else
    {
        *retval = VARIANT_FALSE;
        ErrorMessage(tkFILE_NOT_OPEN);
        return S_OK;
    }

    if (*retval == VARIANT_FALSE)
    {
        _table->get_LastErrorCode(&_lastErrorCode);
        *retval = VARIANT_FALSE;
    }

    return S_OK;
}

// ****************************************************************
//	  StopEditingTable()
// ****************************************************************
STDMETHODIMP CShapefile::StopEditingTable(VARIANT_BOOL ApplyChanges, ICallback* cBack, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (_globalCallback == nullptr && cBack != nullptr)
    {
        _globalCallback = cBack;
        _globalCallback->AddRef();
    }

    if (_table != nullptr)
    {
        _table->StopEditingTable(ApplyChanges, cBack, retval);
    }
    else
    {
        *retval = VARIANT_FALSE;
        ErrorMessage(tkFILE_NOT_OPEN);
        return S_OK;
    }

    if (*retval == FALSE)
    {
        _table->get_LastErrorCode(&_lastErrorCode);
        *retval = VARIANT_FALSE;
    }

    return S_OK;
}

// *****************************************************************
//	   get_Field()
// *****************************************************************
STDMETHODIMP CShapefile::get_Field(long FieldIndex, IField** pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (_table != nullptr)
    {
        _table->get_Field(FieldIndex, pVal);
        if (*pVal != nullptr)
        {
            // we need to report error from field class, and will use callback from this class for it
            ICallback* cBack = nullptr;
            if ((*pVal)->get_GlobalCallback(&cBack) == NULL && this->_globalCallback != nullptr)
                (*pVal)->put_GlobalCallback(_globalCallback);

            if (cBack != nullptr)
                cBack->Release(); // we put a reference in field class so must release it here
        }
    }
    else
    {
        ErrorMessage(tkFILE_NOT_OPEN);
        return S_OK;
    }

    return S_OK;
}

// *****************************************************************
//	   get_FieldByName()
// *****************************************************************
STDMETHODIMP CShapefile::get_FieldByName(BSTR Fieldname, IField** pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    USES_CONVERSION;

    long max;

    CString strFieldname;
    IField* testVal;

    _table->get_NumFields(&max);
    if (_table != nullptr)
    {
        if (_tcslen(OLE2CA(Fieldname)) > 0)
        {
            strFieldname = OLE2A(Fieldname);
        }
        else
        {
            ErrorMessage(tkZERO_LENGTH_STRING);
        }

        for (int fld = 0; fld < max; fld++)
        {
            _table->get_Field(fld, &testVal);
            CComBSTR Testname;
            testVal->get_Name(&Testname);
            CString strTestname = OLE2A(Testname);
            if (strTestname.CompareNoCase(strFieldname) == 0)
            {
                *pVal = testVal;
                return S_OK;
            }
            testVal->Release();
        }
    }
    else
    {
        ErrorMessage(tkFILE_NOT_OPEN);
        return S_OK;
    }

    // we did not have a file error, but we also didn't match the name
    pVal = nullptr;
    return S_OK;
}

// *****************************************************************
//	   get_CellValue()
// *****************************************************************
STDMETHODIMP CShapefile::get_CellValue(long FieldIndex, long ShapeIndex, VARIANT* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (_table != nullptr)
    {
        _table->get_CellValue(FieldIndex, ShapeIndex, pVal);
    }
    else
    {
        ErrorMessage(tkFILE_NOT_OPEN);
        return S_OK;
    }

    return S_OK;
}

// *****************************************************************
//	   get_EditingTable()
// *****************************************************************
STDMETHODIMP CShapefile::get_EditingTable(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    if (_table != nullptr)
    {
        _table->get_EditingTable(pVal);
    }
    else
    {
        *pVal = VARIANT_FALSE;
        ErrorMessage(tkFILE_NOT_OPEN);
        return S_OK;
    }

    return S_OK;
}

// *************************************************************
//		get_Table()
// *************************************************************
STDMETHODIMP CShapefile::get_Table(ITable** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *retVal = _table;
    if (_table)
    {
        _table->AddRef();
    }
    return S_OK;
}
#pragma endregion

#pragma region DrawingOptions

// *************************************************************
//		get_ShapeRotation()
// *************************************************************
STDMETHODIMP CShapefile::get_ShapeRotation(long ShapeIndex, double* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        *pVal = -1;
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
        *pVal = _shapeData[ShapeIndex]->rotation;
    return S_OK;
}

STDMETHODIMP CShapefile::put_ShapeRotation(long ShapeIndex, double newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
        _shapeData[ShapeIndex]->rotation = static_cast<float>(newVal);

    return S_OK;
}

// *************************************************************
//		get_ShapeVisible()
// *************************************************************
STDMETHODIMP CShapefile::get_ShapeVisible(long ShapeIndex, VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = VARIANT_FALSE;
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
    {
        // this particular shape was not hidden explicitly or via visibility expression
        if (!_shapeData[ShapeIndex]->hidden() && _shapeData[ShapeIndex]->isVisible())
        {
            long ctIndex = -1;
            get_ShapeCategory(ShapeIndex, &ctIndex);
            if (ctIndex == -1)
            {
                // no category, check default options
                _defaultDrawOpt->get_Visible(pVal);
            }
            else
            {
                // there is category, check whether it is visible
                CComPtr<IShapefileCategory> ct = nullptr;
                get_ShapeCategory3(ShapeIndex, &ct);
                if (ct)
                {
                    CComPtr<IShapeDrawingOptions> options = nullptr;
                    ct->get_DrawingOptions(&options);
                    if (options)
                    {
                        options->get_Visible(pVal);
                    }
                }
            }
        }
    }
    return S_OK;
}

// *************************************************************
//		ShapeIsHidden()
// *************************************************************
STDMETHODIMP CShapefile::get_ShapeIsHidden(LONG shapeIndex, VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (shapeIndex < 0 || shapeIndex >= (long)_shapeData.size())
    {
        *pVal = VARIANT_FALSE;
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
    {
        *pVal = _shapeData[shapeIndex]->hidden() ? VARIANT_TRUE : VARIANT_FALSE;
    }
    return S_OK;
}

STDMETHODIMP CShapefile::put_ShapeIsHidden(LONG shapeIndex, VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (shapeIndex < 0 || shapeIndex >= (long)_shapeData.size())
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
        _shapeData[shapeIndex]->hidden(newVal != 0);

    return S_OK;
}

// *************************************************************
//		get_ShapeModified()
// *************************************************************
STDMETHODIMP CShapefile::get_ShapeModified(long ShapeIndex, VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        *retVal = -1;
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
    {
        *retVal = _shapeData[ShapeIndex]->modified() ? VARIANT_TRUE : VARIANT_FALSE;
    }

    return S_OK;
}

STDMETHODIMP CShapefile::put_ShapeModified(long ShapeIndex, VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
    {
        _shapeData[ShapeIndex]->modified(newVal != 0);
    }

    return S_OK;
}


// *************************************************************
//		get_ShapeCategory()
// *************************************************************
STDMETHODIMP CShapefile::get_ShapeCategory(long ShapeIndex, long* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size()) //_numShapes)
    {
        *pVal = -1;
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
        *pVal = _shapeData[ShapeIndex]->category;
    return S_OK;
}

STDMETHODIMP CShapefile::put_ShapeCategory(long ShapeIndex, long newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size()) //_numShapes )
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
        _shapeData[ShapeIndex]->category = (int)newVal;

    return S_OK;
}

// *************************************************************
//		get_ShapeCategory2()
// *************************************************************
STDMETHODIMP CShapefile::put_ShapeCategory2(long ShapeIndex, BSTR categoryName)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
    {
        int index;
        _categories->get_CategoryIndexByName(categoryName, &index);
        if (index == -1)
        {
            ErrorMessage(tkCATEGORY_WASNT_FOUND);
        }
        else
        {
            _shapeData[ShapeIndex]->category = (int)index;
        }
    }
    return S_OK;
}

STDMETHODIMP CShapefile::get_ShapeCategory2(long ShapeIndex, BSTR* categoryName)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
    {
        const int index = _shapeData[ShapeIndex]->category;
        long count;
        _categories->get_Count(&count);
        if (index >= 0 && index < count)
        {
            IShapefileCategory* ct;
            _categories->get_Item(index, &ct);
            ct->get_Name(categoryName);
            ct->Release();
            return S_OK;
        }
        ErrorMessage(tkCATEGORY_WASNT_FOUND);
    }
    *categoryName = SysAllocString(L"");
    return S_OK;
}

// *************************************************************
//		put_ShapeCategory3()
// *************************************************************
STDMETHODIMP CShapefile::put_ShapeCategory3(long ShapeIndex, IShapefileCategory* category)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
    {
        int index;
        _categories->get_CategoryIndex(category, &index);
        if (index == -1)
        {
            ErrorMessage(tkCATEGORY_WASNT_FOUND);
        }
        else
        {
            _shapeData[ShapeIndex]->category = (int)index;
        }
    }
    return S_OK;
}

STDMETHODIMP CShapefile::get_ShapeCategory3(long ShapeIndex, IShapefileCategory** category)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *category = nullptr;
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
    {
        const int index = _shapeData[ShapeIndex]->category;
        long count;
        _categories->get_Count(&count);
        if (index >= 0 && index < count)
        {
            IShapefileCategory* ct;
            _categories->get_Item(index, &ct);
            *category = ct; // ref was added in the get_Item
        }
        else
        {
            ErrorMessage(tkCATEGORY_WASNT_FOUND);
        }
    }
    return S_OK;
}

// *******************************************************************
//  	SelectionDrawingOptions()
// *******************************************************************
//  Returns and sets parameters used to draw selection for the shapefile.
STDMETHODIMP CShapefile::get_SelectionDrawingOptions(IShapeDrawingOptions** pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _selectDrawOpt;
    if (_selectDrawOpt)
        _selectDrawOpt->AddRef();
    return S_OK;
}

STDMETHODIMP CShapefile::put_SelectionDrawingOptions(IShapeDrawingOptions* newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (!newVal)
    {
        ErrorMessage(tkINVALID_PARAMETER_VALUE);
    }
    else
    {
        ComHelper::SetRef(newVal, (IDispatch**)&_selectDrawOpt, false);
    }
    return S_OK;
}

// *******************************************************************
//		DeafultDrawingOptions()				          
// *******************************************************************
// Returns and sets parameters used to draw shapefile by default.
STDMETHODIMP CShapefile::get_DefaultDrawingOptions(IShapeDrawingOptions** pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _defaultDrawOpt;
    if (_defaultDrawOpt)
        _defaultDrawOpt->AddRef();
    return S_OK;
}

STDMETHODIMP CShapefile::put_DefaultDrawingOptions(IShapeDrawingOptions* newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (!newVal)
    {
        ErrorMessage(tkINVALID_PARAMETER_VALUE);
    }
    else
    {
        ComHelper::SetRef(newVal, (IDispatch**)&_defaultDrawOpt);
    }
    return S_OK;
}

// ***********************************************************************
//		put_ReferenceToCategories
// ***********************************************************************
void CShapefile::put_ReferenceToCategories(bool bNullReference)
{
    if (_categories == nullptr) return;
    auto* coCategories = dynamic_cast<CShapefileCategories*>(_categories);
    if (!bNullReference)
        coCategories->put_ParentShapefile(this);
    else
        coCategories->put_ParentShapefile(nullptr);
};

// ***********************************************************************
//		get/put_Categories
// ***********************************************************************
STDMETHODIMP CShapefile::get_Categories(IShapefileCategories** pVal)
{
    *pVal = _categories;
    if (_categories != nullptr)
        _categories->AddRef();
    return S_OK;
}

STDMETHODIMP CShapefile::put_Categories(IShapefileCategories* newVal)
{
    if (ComHelper::SetRef((IDispatch*)newVal, (IDispatch**)&_categories, false))
    {
        ((CShapefileCategories*)_categories)->put_ParentShapefile(this);
    }
    return S_OK;
}
#pragma endregion

// ********************************************************************
//		get_SelectionColor
// ********************************************************************
STDMETHODIMP CShapefile::get_SelectionColor(OLE_COLOR* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retval = _selectionColor;
    return S_OK;
}

STDMETHODIMP CShapefile::put_SelectionColor(OLE_COLOR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _selectionColor = newVal;
    return S_OK;
}

// ********************************************************************
//		get_SelectionTransparency
// ********************************************************************
STDMETHODIMP CShapefile::get_SelectionTransparency(BYTE* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retval = _selectionTransparency;
    return S_OK;
}

STDMETHODIMP CShapefile::put_SelectionTransparency(BYTE newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (newVal > 255) newVal = 255;
    _selectionTransparency = newVal;
    return S_OK;
}

// ********************************************************************
//		get_SelectionAppearance
// ********************************************************************
STDMETHODIMP CShapefile::get_SelectionAppearance(tkSelectionAppearance* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retval = _selectionAppearance;
    return S_OK;
}

STDMETHODIMP CShapefile::put_SelectionAppearance(tkSelectionAppearance newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _selectionAppearance = newVal;
    return S_OK;
}

// ********************************************************************
//		get_PointCollisionMode
// ********************************************************************
STDMETHODIMP CShapefile::get_CollisionMode(tkCollisionMode* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retval = _collisionMode;
    return S_OK;
}

STDMETHODIMP CShapefile::put_CollisionMode(tkCollisionMode newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _collisionMode = newVal;
    return S_OK;
}

#pragma region "Seialization"
// ********************************************************
//     Serialize()
// ********************************************************
STDMETHODIMP CShapefile::Serialize(VARIANT_BOOL SaveSelection, BSTR* retVal)
{
    return Serialize2(SaveSelection, VARIANT_FALSE, retVal);
}

STDMETHODIMP CShapefile::Serialize2(VARIANT_BOOL SaveSelection, VARIANT_BOOL SerializeCategories, BSTR* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    CPLXMLNode* psTree = this->SerializeCore(VARIANT_TRUE, "ShapefileClass", SerializeCategories != 0);
    Utility::SerializeAndDestroyXmlTree(psTree, retVal);
    return S_OK;
}

// ********************************************************
//     SerializeCore()
// ********************************************************
CPLXMLNode* CShapefile::SerializeCore(VARIANT_BOOL SaveSelection, CString ElementName, bool serializeCategories)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    USES_CONVERSION;

    CPLXMLNode* psTree = CPLCreateXMLNode(nullptr, CXT_Element, ElementName);

    if (psTree)
    {
        CString s = OLE2CA(_expression);
        if (s != "")
            Utility::CPLCreateXMLAttributeAndValue(psTree, "VisibilityExpression", s);


        if (_useQTree != FALSE)
            Utility::CPLCreateXMLAttributeAndValue(psTree, "UseQTree", CPLString().Printf("%d", (int)_useQTree));

        if (_collisionMode != LocalList)
            Utility::CPLCreateXMLAttributeAndValue(psTree, "CollisionMode",
                                                   CPLString().Printf("%d", (int)_collisionMode));

        if (_selectionAppearance != saSelectionColor)
            Utility::CPLCreateXMLAttributeAndValue(psTree, "SelectionAppearance",
                                                   CPLString().Printf("%d", (int)_selectionAppearance));

        if (_selectionColor != RGB(255, 255, 0))
            Utility::CPLCreateXMLAttributeAndValue(psTree, "SelectionColor",
                                                   CPLString().Printf("%d", (int)_selectionColor));

        if (_selectionTransparency != 180)
            Utility::CPLCreateXMLAttributeAndValue(psTree, "SelectionTransparency",
                                                   CPLString().Printf("%d", (int)_selectionTransparency));

        if (_minDrawingSize != 1)
            Utility::CPLCreateXMLAttributeAndValue(psTree, "MinDrawingSize", CPLString().Printf("%d", _minDrawingSize));

        // for in-memory shapefiles only
        if (_sourceType == sstInMemory)
            Utility::CPLCreateXMLAttributeAndValue(psTree, "ShpType",
                                                   CPLString().Printf("%d", (int)this->_shpfiletype));

        s = OLE2CA(_sortField);
        if (s != "")
            Utility::CPLCreateXMLAttributeAndValue(psTree, "SortField", s);

        if (_sortAscending != VARIANT_FALSE)
            Utility::CPLCreateXMLAttributeAndValue(psTree, "SortAscending",
                                                   CPLString().Printf("%d", (int)_sortAscending));

        // drawing options
        CPLXMLNode* node = ((CShapeDrawingOptions*)_defaultDrawOpt)->SerializeCore("DefaultDrawingOptions");
        if (node)
        {
            CPLAddXMLChild(psTree, node);
        }

        if (_selectionAppearance == saDrawingOptions)
        {
            node = ((CShapeDrawingOptions*)_selectDrawOpt)->SerializeCore("SelectionDrawingOptions");
            if (node)
            {
                CPLAddXMLChild(psTree, node);
            }
        }

        // categories
        node = ((CShapefileCategories*)_categories)->SerializeCore("ShapefileCategoriesClass");
        if (node)
        {
            CPLAddXMLChild(psTree, node);
        }

        // labels
        CPLXMLNode* psLabels = ((CLabels*)_labels)->SerializeCore("LabelsClass");
        if (psLabels)
        {
            CPLAddXMLChild(psTree, psLabels);
        }

        // charts
        CPLXMLNode* psCharts = ((CCharts*)_charts)->SerializeCore("ChartsClass");
        if (psCharts)
        {
            CPLAddXMLChild(psTree, psCharts);
        }

        // ----------------------------------------------------
        // selection
        // ----------------------------------------------------
        long numSelected;
        this->get_NumSelected(&numSelected);

        if (numSelected > 0 && SaveSelection)
        {
            auto* selection = new char[_shapeData.size() + 1];
            selection[_shapeData.size()] = '\0';
            for (unsigned int i = 0; i < _shapeData.size(); i++)
            {
                selection[i] = _shapeData[i]->selected() ? '1' : '0';
            }

            CPLXMLNode* nodeSelection = CPLCreateXMLElementAndValue(psTree, "Selection", selection);
            if (nodeSelection)
            {
                Utility::CPLCreateXMLAttributeAndValue(nodeSelection, "TotalCount",
                                                       CPLString().Printf("%d", _shapeData.size()));
                Utility::CPLCreateXMLAttributeAndValue(nodeSelection, "SelectedCount",
                                                       CPLString().Printf("%d", numSelected));
            }
            delete[] selection;
        }

        // ----------------------------------------------------
        // serialization of category indices
        // ----------------------------------------------------
        // Paul Meems TODO: This variable comes in as a parameter as well.
        //            Is this correct?
        bool serializeCategories = false;

        for (auto& i : _shapeData)
        {
            if (i->category != -1)
            {
                serializeCategories = true;
            }
        }

        if (serializeCategories)
        {
            s = "";
            // doing it with CString is ugly of course, better to allocate a buffer
            CString temp;
            for (auto& i : _shapeData)
            {
                temp.Format("%d,", i->category);
                s += temp;
            }

            // when there are no indices assigned, write an empty node with Count = 0;
            // to signal, that categories must not be applied automatically (behavior for older versions)
            CPLXMLNode* nodeCats = CPLCreateXMLElementAndValue(psTree, "CategoryIndices", s.GetBuffer());
            if (nodeCats)
            {
                Utility::CPLCreateXMLAttributeAndValue(nodeCats, "Count",
                                                       CPLString().Printf(
                                                           "%d", serializeCategories ? _shapeData.size() : 0));
            }
        }

        // ----------------------------------------------------
        // table
        // ----------------------------------------------------
        if (_table)
        {
            CPLXMLNode* psTable = ((CTableClass*)_table)->SerializeCore("TableClass");
            if (psTable)
            {
                CPLAddXMLChild(psTree, psTable);
            }
        }
    }
    return psTree;
}

// ********************************************************
//     Deserialize()
// ********************************************************
STDMETHODIMP CShapefile::Deserialize(VARIANT_BOOL LoadSelection, BSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    USES_CONVERSION;

    CString s = OLE2CA(newVal);
    CPLXMLNode* node = CPLParseXMLString(s.GetString());
    if (node)
    {
        CPLXMLNode* nodeSf = CPLGetXMLNode(node, "=ShapefileClass");
        if (nodeSf)
        {
            this->DeserializeCore(VARIANT_TRUE, nodeSf);
        }
        CPLDestroyXMLNode(node);
    }
    return S_OK;
}

// ********************************************************
//     DeserializeCore()
// ********************************************************
bool CShapefile::DeserializeCore(VARIANT_BOOL LoadSelection, CPLXMLNode* node)
{
    USES_CONVERSION;

    if (!node)
        return false;

    CString s = CPLGetXMLValue(node, "VisibilityExpression", nullptr);
    SysFreeString(_expression);
    _expression = A2BSTR(s);

    s = CPLGetXMLValue(node, "UseQTree", nullptr);
    put_UseQTree(s != "" ? (BOOL)atoi(s.GetString()) : FALSE);

    s = CPLGetXMLValue(node, "CollisionMode", nullptr);
    _collisionMode = s != "" ? (tkCollisionMode)atoi(s.GetString()) : LocalList;

    s = CPLGetXMLValue(node, "SelectionAppearance", nullptr);
    _selectionAppearance = s != "" ? (tkSelectionAppearance)atoi(s.GetString()) : saSelectionColor;

    s = CPLGetXMLValue(node, "SelectionColor", nullptr);
    _selectionColor = s != "" ? (OLE_COLOR)atoi(s.GetString()) : RGB(255, 255, 0);

    s = CPLGetXMLValue(node, "SelectionTransparency", nullptr);
    _selectionTransparency = s != "" ? (unsigned char)atoi(s.GetString()) : 180;

    s = CPLGetXMLValue(node, "MinDrawingSize", nullptr);
    _minDrawingSize = s != "" ? atoi(s.GetString()) : 1;

    s = CPLGetXMLValue(node, "SortField", nullptr);
    const CComBSTR bstrSortField = A2W(s);
    this->put_SortField(bstrSortField);

    s = CPLGetXMLValue(node, "SortAscending", nullptr);
    const VARIANT_BOOL sortAsc = s != "" ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_FALSE;
    this->put_SortAscending(sortAsc);

    if (_sourceType == sstInMemory)
    {
        s = CPLGetXMLValue(node, "ShpType", nullptr);
        if (s != "")
        {
            _shpfiletype = (ShpfileType)atoi(s.GetString());
        }
    }

    // drawing options
    CPLXMLNode* psChild = CPLGetXMLNode(node, "DefaultDrawingOptions");
    if (psChild)
    {
        ((CShapeDrawingOptions*)_defaultDrawOpt)->DeserializeCore(psChild);
    }

    if (_selectionAppearance == saDrawingOptions)
    {
        psChild = CPLGetXMLNode(node, "SelectionDrawingOptions");
        if (psChild)
        {
            ((CShapeDrawingOptions*)_selectDrawOpt)->DeserializeCore(psChild);
        }
    }

    // Categories
    psChild = CPLGetXMLNode(node, "ShapefileCategoriesClass");
    if (psChild)
    {
        ((CShapefileCategories*)_categories)->DeserializeCore(psChild, false);
    }

    CPLXMLNode* nodeCats = CPLGetXMLNode(node, "CategoryIndices");
    bool needsApplyExpression = true;
    if (nodeCats)
    {
        CString indices = CPLGetXMLValue(nodeCats, "=CategoryIndices", "");
        if (indices.GetLength() > 0)
        {
            s = CPLGetXMLValue(nodeCats, "Count", "0");
            const long savedCount = atoi(s);
            int foundCount = 0;
            char* buffer = indices.GetBuffer();
            for (int i = 0; i < indices.GetLength(); i++)
            {
                if (buffer[i] == ',')
                {
                    foundCount++;
                }
            }

            if (foundCount == savedCount && foundCount == _shapeData.size())
            {
                const int size = _shapeData.size();
                int pos = 0, count = 0;
                CString ct = indices.Tokenize(",", pos);
                while (ct.GetLength() != 0 && count < size)
                {
                    _shapeData[count]->category = atoi(ct);
                    ct = indices.Tokenize(",", pos);
                    count++;
                };
                bool needsApplyExpression = false;
            }
        }
    }

    // If no indices or invalid indices, re-apply:
    if (needsApplyExpression)
    {
        ((CShapefileCategories*)_categories)->ApplyExpressions();
    }

    // Labels
    psChild = CPLGetXMLNode(node, "LabelsClass");
    if (psChild)
    {
        ((CLabels*)_labels)->DeserializeCore(psChild);
    }

    // Charts
    psChild = CPLGetXMLNode(node, "ChartsClass");
    if (psChild)
    {
        ((CCharts*)_charts)->DeserializeCore(psChild);
    }

    // selection
    CPLXMLNode* nodeSelection = CPLGetXMLNode(node, "Selection");
    if (nodeSelection && LoadSelection)
    {
        this->SelectNone();

        s = CPLGetXMLValue(nodeSelection, "TotalCount", "0");
        const long count = atoi(s);
        s = CPLGetXMLValue(nodeSelection, "=Selection", "");
        if (s.GetLength() == count && s.GetLength() == _shapeData.size())
        {
            char* selection = s.GetBuffer();
            for (unsigned int i = 0; i < _shapeData.size(); i++)
            {
                if (selection[i] == '1')
                {
                    _shapeData[i]->selected(true);
                }
            }
        }
    }


    // table
    if (_table)
    {
        psChild = CPLGetXMLNode(node, "TableClass");
        if (psChild)
        {
            ((CTableClass*)_table)->DeserializeCore(psChild);
        }
    }
    return true;
}
#pragma endregion


#pragma region Projection
// *****************************************************************
//		get_Projection()
// *****************************************************************
STDMETHODIMP CShapefile::get_Projection(BSTR* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    _geoProjection->ExportToProj4(pVal);
    return S_OK;
}

// *****************************************************************
//		put_Projection()
// *****************************************************************
STDMETHODIMP CShapefile::put_Projection(BSTR proj4Projection)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    USES_CONVERSION;

    VARIANT_BOOL vbretval;
    _geoProjection->ImportFromProj4(proj4Projection, &vbretval);
    if (vbretval)
    {
        const CComBSTR bstrFilename(_prjfileName);
        _geoProjection->WriteToFileEx(bstrFilename, VARIANT_TRUE, &vbretval);
    }
    return S_OK;
}

// *****************************************************************
//		get_GeoProjection()
// *****************************************************************
STDMETHODIMP CShapefile::get_GeoProjection(IGeoProjection** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (_geoProjection)
        _geoProjection->AddRef();

    *retVal = _geoProjection;
    return S_OK;
}

// *****************************************************************
//		put_GeoProjection()
// *****************************************************************
STDMETHODIMP CShapefile::put_GeoProjection(IGeoProjection* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ComHelper::SetRef((IDispatch*)pVal, (IDispatch**)&_geoProjection, false);
    if (_prjfileName.GetLength() != 0)
    {
        VARIANT_BOOL vbretval;
        const CComBSTR bstr(_prjfileName);
        _geoProjection->WriteToFileEx(bstr, VARIANT_TRUE, &vbretval);
    }
    return S_OK;
}

// ****************************************************************
//		get_IsGeographicProjection
// ****************************************************************
STDMETHODIMP CShapefile::get_IsGeographicProjection(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *pVal = VARIANT_FALSE;

    if (_geoProjection)
        _geoProjection->get_IsGeographic(pVal);

    return S_OK;
}

// *****************************************************************
//		Reproject()
// *****************************************************************
STDMETHODIMP CShapefile::Reproject(IGeoProjection* newProjection, LONG* reprojectedCount, IShapefile** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (!this->ReprojectCore(newProjection, reprojectedCount, retVal, false))
        *retVal = nullptr;
    return S_OK;
}

// *****************************************************************
//		ReprojectInPlace()
// *****************************************************************
STDMETHODIMP CShapefile::ReprojectInPlace(IGeoProjection* newProjection, LONG* reprojectedCount, VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (!_isEditingShapes)
    {
        ErrorMessage(tkSHPFILE_NOT_IN_EDIT_MODE);
        *retVal = VARIANT_FALSE;
    }
    else
    {
        if (this->ReprojectCore(newProjection, reprojectedCount, nullptr, true))
        {
            // spatial index must be deleted, as it became useful all the same
            VARIANT_BOOL vb;
            RemoveSpatialIndex(&vb);

            // update qtree
            if (_useQTree)
                GenerateQTree();

            VARIANT_BOOL vbretval;
            this->RefreshExtents(&vbretval);
            *retVal = VARIANT_TRUE;
            return S_OK;
        }
        *retVal = NULL;
    }
    return S_OK;
}

// *****************************************************************
//		ReprojectCore()
// *****************************************************************
bool CShapefile::ReprojectCore(IGeoProjection* newProjection, LONG* reprojectedCount, IShapefile** retVal,
                               bool reprojectInPlace)
{
    // ------------------------------------------------------
    //   Validation
    // ------------------------------------------------------
    if (!newProjection)
    {
        ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
        return false;
    }

    VARIANT_BOOL isEmpty1, isEmpty2;
    newProjection->get_IsEmpty(&isEmpty1);
    _geoProjection->get_IsEmpty(&isEmpty2);
    if (isEmpty1 || isEmpty2)
    {
        ErrorMessage(tkPROJECTION_NOT_INITIALIZED);
        return false;
    }

    if (!ValidateInput(this, "Reproject/ReprojectInPlace", "this", VARIANT_FALSE))
        return false;

    m_globalSettings.gdalErrorMessage = "";
    OGRSpatialReference* projSource = ((CGeoProjection*)_geoProjection)->get_SpatialReference();
    OGRSpatialReference* projTarget = ((CGeoProjection*)newProjection)->get_SpatialReference();

    OGRCoordinateTransformation* transf = OGRCreateCoordinateTransformation(projSource, projTarget);
    if (!transf)
    {
        m_globalSettings.gdalErrorMessage = CPLGetLastErrorMsg();
        ErrorMessage(tkFAILED_TO_REPROJECT);
        return false;
    }

    // ------------------------------------------------------
    //   Creating output
    // ------------------------------------------------------
    if (!reprojectInPlace)
        this->Clone(retVal);

    // ------------------------------------------------------
    //   Processing
    // ------------------------------------------------------
    CComVariant var;
    const long numShapes = _shapeData.size();
    long newIndex = 0;

    long numFields, percent = 0;
    this->get_NumFields(&numFields);

    std::map<long, long> reverseOgrFidMapping;
    // we have to update the OgrFid mapping if not reprojecting in-place
    if (_hasOgrFidMapping && !reprojectInPlace)
    {
        // set flag in new Shapefile
        ((CShapefile*)(*retVal))->HasOgrFidMapping(true);
        // one-time set up of reverse mapping for optimized reverse lookup
        auto iter = _ogrFid2ShapeIndex.begin();
        while (iter != _ogrFid2ShapeIndex.end())
        {
            reverseOgrFidMapping.insert(std::make_pair(iter->second, iter->first));
            iter++;
        }
    }

    VARIANT_BOOL vb = VARIANT_FALSE;
    *reprojectedCount = 0;

    for (long i = 0; i < numShapes; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, numShapes, "Reprojecting...", _key, percent);

        IShape* shp = nullptr;
        this->GetValidatedShape(i, &shp);
        if (!shp) continue;

        if (!reprojectInPlace)
        {
            IShape* shpNew = nullptr;
            shp->Clone(&shpNew);
            shp->Release();
            shp = shpNew;
        }

        if (shp)
        {
            long numPoints;
            shp->get_NumPoints(&numPoints);

            if (numPoints > 0)
            {
                auto* x = new double[numPoints];
                auto* y = new double[numPoints];

                // extracting coordinates
                for (long j = 0; j < numPoints; j++)
                {
                    shp->get_XY(j, x + j, y + j, &vb);
                }

                // will work faster after embedding to the CShape class
                const BOOL res = transf->Transform(numPoints, x, y);
                if (!res)
                {
                    // save error message and continue
                    if (m_globalSettings.gdalErrorMessage == "")
                        m_globalSettings.gdalErrorMessage = CPLGetLastErrorMsg();
                }
                else
                {
                    // saving updated coordinates
                    for (long j = 0; j < numPoints; j++)
                    {
                        shp->put_XY(j, x[j], y[j], &vb);
                    }

                    if (!reprojectInPlace)
                    {
                        // get next available index
                        (*retVal)->get_NumShapes(&newIndex);
                        // insert Shape into target at new index
                        (*retVal)->EditInsertShape(shp, &newIndex, &vb);

                        // copy attributes
                        for (long j = 0; j < numFields; j++)
                        {
                            // get cell value at source index i
                            this->get_CellValue(j, i, &var);
                            // set cell value into target at new index
                            (*retVal)->EditCellValue(j, newIndex, var, &vb);
                        }

                        // update OgrFid mapping in new Shapefile
                        if (_hasOgrFidMapping)
                        {
                            // find OgrFid for the current index i, map it to newIndex
                            ((CShapefile*)(*retVal))->MapOgrFid2ShapeIndex(reverseOgrFidMapping[i], newIndex);
                        }
                    }
                    // 
                    (*reprojectedCount)++;
                }
                delete[] x;
                delete[] y;
            }
            shp->Release();
        }
    }

    if (transf)
    {
        OGRCoordinateTransformation::DestroyCT(transf);
        transf = nullptr;
    }

    // copy attributes and clean-up
    if (_hasOgrFidMapping && !reprojectInPlace)
    {
        map<long, BYTE> visibilityFlags;
        // copy visibility flags
        if (this->GetVisibilityFlags(visibilityFlags))
        {
            VARIANT_BOOL vb;
            // copy 'selectable' attribute
            this->get_Selectable(&vb);
            ((CShapefile*)(*retVal))->put_Selectable(vb);
            // restore visibility flags
            ((CShapefile*)(*retVal))->SetVisibilityFlags(visibilityFlags);
        }
        // clean up reverse-mapping
        reverseOgrFidMapping.clear();
    }

    // function result will be based on successful projection setting;
    // BUT if there were no shapes to reproject, consider it a success
    vb = (numShapes == 0) ? VARIANT_TRUE : VARIANT_FALSE;
    // if at least some rows were reprojected...
    if (*reprojectedCount > 0)
    {
        // setting new projection
        if (reprojectInPlace)
        {
            // vb result will be used to determine overall success
            _geoProjection->CopyFrom(newProjection, &vb);
        }
        else
        {
            IGeoProjection* proj = nullptr;
            (*retVal)->get_GeoProjection(&proj);
            if (proj)
            {
                // vb result will be used to determine overall success
                proj->CopyFrom(newProjection, &vb);
                proj->Release();
            }
        }
    }

    // inserted shapes were marked as modified, correct this
    if (reprojectInPlace)
        ShapefileHelper::ClearShapefileModifiedFlag(this);
    else
        ShapefileHelper::ClearShapefileModifiedFlag(*retVal);

    // -------------------------------------- 
    //	  Output validation
    // -------------------------------------- 
    CallbackHelper::ProgressCompleted(_globalCallback, _key);

    if (!reprojectInPlace)
    {
        this->ValidateOutput(retVal, "Reproject/ReprojectInPlace", "Shapefile", false);
    }
    else
    {
        this->ValidateOutput(this, "Reproject/ReprojectInPlace", "Shapefile", false);
    }

    // it's critical to set correct projection, so false will be returned if it wasn't done
    return vb != 0;
}
#pragma endregion

// *****************************************************************
//		FixUpShapes()
// *****************************************************************
STDMETHODIMP CShapefile::FixUpShapes(IShapefile** retVal, VARIANT_BOOL* fixed)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // MWGIS-90: default to all shapes:
    FixUpShapes2(VARIANT_FALSE, retVal, fixed);

    return S_OK;
}

// *********************************************************
//		FixUpShapes2()
// *********************************************************
STDMETHODIMP CShapefile::FixUpShapes2(VARIANT_BOOL SelectedOnly, IShapefile** result, VARIANT_BOOL* fixed)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *fixed = VARIANT_FALSE;

    if (*result == nullptr)
    {
        Clone(result);
    }

    *fixed = FixupShapesCore(SelectedOnly, *result);

    return S_OK;
}

// *********************************************************
//		FixupShapesCore()
// *********************************************************
VARIANT_BOOL CShapefile::FixupShapesCore(VARIANT_BOOL selectedOnly, IShapefile* result)
{
    if (!result) return VARIANT_FALSE;

    tkUnitsOfMeasure units;
    _geoProjection->get_LinearUnits(&units);

    long numFields;
    this->get_NumFields(&numFields);

    long percent = 0;
    const int numShapes = _shapeData.size();
    // VARIANT_BOOL fixed = VARIANT_TRUE;

    for (int i = 0; i < numShapes; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, numShapes, "Fixing...", _key, percent);

        if (!ShapeAvailable(i, selectedOnly))
            continue;

        IShape* shp = nullptr;
        get_Shape(i, &shp);
        if (!shp)
        {
            continue;
        }

        IShape* shpNew = nullptr;
        shp->FixUp2(units, &shpNew);
        shp->Release();

        // failed to fix the shape? skip it.
        if (!shpNew)
        {
            CString s;
            s.Format("Failed to fix shape: %d", i);
            CallbackHelper::ErrorMsg("Shapefile", nullptr, "", s);
            continue;
        }

        long shapeIndex = 0;
        result->get_NumShapes(&shapeIndex);

        VARIANT_BOOL vbretval = VARIANT_FALSE;
        result->EditInsertShape(shpNew, &shapeIndex, &vbretval);
        shpNew->Release();

        if (vbretval)
        {
            // TODO: extract, it's definitely used in other methods as well
            CComVariant var;
            for (int iFld = 0; iFld < numFields; iFld++)
            {
                get_CellValue(iFld, i, &var);
                result->EditCellValue(iFld, shapeIndex, var, &vbretval);
            }
        }
    }

    CallbackHelper::ProgressCompleted(_globalCallback, _key);

    return VARIANT_TRUE;
}

// *********************************************************
//		GetRelatedShapes()
// *********************************************************
STDMETHODIMP CShapefile::GetRelatedShapes(long referenceIndex, tkSpatialRelation relation, VARIANT* resultArray,
                                          VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retval = VARIANT_FALSE;
    if (referenceIndex < 0 || referenceIndex > (long)_shapeData.size())
    {
        this->ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
        return S_OK;
    }

    IShape* shp = nullptr;
    this->get_Shape(referenceIndex, &shp);
    if (shp)
    {
        this->GetRelatedShapeCore(shp, referenceIndex, relation, resultArray, retval);
        shp->Release();
    }
    return S_OK;
}

// *********************************************************
//		GetRelatedShapes2()
// *********************************************************
STDMETHODIMP CShapefile::GetRelatedShapes2(IShape* referenceShape, tkSpatialRelation relation, VARIANT* resultArray,
                                           VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retval = VARIANT_FALSE;
    if (!referenceShape)
    {
        this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
        return S_OK;
    }

    this->GetRelatedShapeCore(referenceShape, -1, relation, resultArray, retval);
    return S_OK;
}

// *********************************************************
//		GetRelatedShapeCore()
// *********************************************************
void CShapefile::GetRelatedShapeCore(IShape* referenceShape, long referenceIndex, tkSpatialRelation relation,
                                     VARIANT* resultArray, VARIANT_BOOL* retval)
{
    // rather than generate geometries for all shapes,
    // only generate for those within qtree extent (see below)
    //this->ReadGeosGeometries(VARIANT_FALSE);

    // turns on the quad tree
    VARIANT_BOOL useQTree = VARIANT_FALSE;
    this->get_UseQTree(&useQTree);
    if (!useQTree) this->put_UseQTree(VARIANT_TRUE);

    double xMin, xMax, yMin, yMax;
    if (((CShape*)referenceShape)->get_ExtentsXY(xMin, yMin, xMax, yMax))
    {
        const QTreeExtent query(xMin, xMax, yMax, yMin);
        std::vector<int> shapes = this->_qtree->GetNodes(query);
        std::vector<int> arr;

        // were any shapes returned ?
        if (shapes.size() > 0)
        {
            // generate GEOS geometries only for shapes within qtree extent
            std::set<int> list;
            for (size_t i = 0; i < shapes.size(); i++)
                    // add subset of indices to local list
                list.insert(shapes[i]);
            // now generate only for list of shapes
            this->ReadGeosGeometries(list);

            GEOSGeom geomBase;
            if (referenceIndex >= 0)
            {
                geomBase = _shapeData[referenceIndex]->geosGeom;
            }
            else
            {
                geomBase = GeosConverter::ShapeToGeom(referenceShape);
            }

            if (geomBase)
            {
                for (size_t i = 0; i < shapes.size(); i++)
                {
                    if (shapes[i] == referenceIndex)
                        continue; // it doesn't make sense to compare the shape with itself

                    // ReSharper disable once CppLocalVariableMayBeConst
                    GEOSGeom geom = _shapeData[shapes[i]]->geosGeom;
                    if (geom != nullptr)
                    {
                        char res = 0;
                        switch (relation)
                        {
                        case srContains: res = GeosHelper::Contains(geomBase, geom);
                            break;
                        case srCrosses: res = GeosHelper::Crosses(geomBase, geom);
                            break;
                        case srEquals: res = GeosHelper::Equals(geomBase, geom);
                            break;
                        case srIntersects: res = GeosHelper::Intersects(geomBase, geom);
                            break;
                        case srOverlaps: res = GeosHelper::Overlaps(geomBase, geom);
                            break;
                        case srTouches: res = GeosHelper::Touches(geomBase, geom);
                            break;
                        case srWithin: res = GeosHelper::Within(geomBase, geom);
                            break;
                        case srCovers: res = GeosHelper::Covers(geomBase, geom);
                            break;
                        case srCoveredBy: res = GeosHelper::CoveredBy(geomBase, geom);
                            break;
                        default:
                        case srDisjoint: res = GeosHelper::Disjoint(geomBase, geom);
                            break;
                        }
                        if (res)
                        {
                            arr.push_back(shapes[i]);
                        }
                    }
                }

                if (referenceIndex == -1)
                    GeosHelper::DestroyGeometry(geomBase);
                // the geometry was created in this function so it must be destroyed
            }
        }
        // return result as SafeArray
        *retval = Templates::Vector2SafeArray(&arr, VT_I4, resultArray);
    }

    // Don't clear the list here as function may be called in a loop
    //this->ClearCachedGeometries();
}

// ***************************************************
//		get_Identifiable
// ***************************************************
STDMETHODIMP CShapefile::get_Identifiable(VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retVal = _hotTracking;
    return S_OK;
}

STDMETHODIMP CShapefile::put_Identifiable(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _hotTracking = newVal;
    return S_OK;
}

// *****************************************************************
//		EditAddField()
// *****************************************************************
STDMETHODIMP CShapefile::EditAddField(BSTR name, FieldType type, int precision, int width, long* fieldIndex)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (!this->_table)
    {
        this->ErrorMessage(tkDBF_FILE_DOES_NOT_EXIST);
    }
    else
    {
        _table->EditAddField(name, type, precision, width, fieldIndex);
    }
    return S_OK;
}

// *****************************************************************
//		EditAddShape()
// *****************************************************************
STDMETHODIMP CShapefile::EditAddShape(IShape* shape, long* shapeIndex)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    VARIANT_BOOL retval;
    *shapeIndex = _shapeData.size();

    EditInsertShape(shape, shapeIndex, &retval);

    if (retval == VARIANT_FALSE)
        *shapeIndex = -1;

    return S_OK;
}

// *****************************************************************
//		GetClosestVertex()
// *****************************************************************
STDMETHODIMP CShapefile::GetClosestVertex(double x, double y, double maxDistance,
                                          long* shapeIndex, long* pointIndex, double* distance, VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = VARIANT_FALSE;
    *shapeIndex = -1;
    *pointIndex = -1;

    bool result = false;
    if (maxDistance <= 0.0)
    {
        // search through all shapefile
        std::vector<long> ids;
        for (size_t i = 0; i < _shapeData.size(); i++)
        {
            ids.push_back(i);
        }
        result = ShapefileHelper::GetClosestPoint(this, x, y, maxDistance, ids, shapeIndex, pointIndex, *distance);
    }
    else
    {
        std::vector<long> ids;
        Extent box(x - maxDistance, x + maxDistance, y - maxDistance, y + maxDistance);
        if (this->SelectShapesCore(box, 0.0, SelectMode::INTERSECTION, ids, false))
        {
            result = ShapefileHelper::GetClosestPoint(this, x, y, maxDistance, ids, shapeIndex, pointIndex, *distance);
        }
    }
    *retVal = result ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
}

// *****************************************************************
//		GetClosestSnapPosition()
// *****************************************************************
STDMETHODIMP CShapefile::GetClosestSnapPosition(double x, double y, double maxDistance,
	long* shapeIndex, double* fx, double* fy, double* distance, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;
	*shapeIndex = -1;

	bool result = false;
	if (maxDistance <= 0.0)
	{
		// search through all shapefile
		std::vector<long> ids;
		for (size_t i = 0; i < _shapeData.size(); i++)
		{
			ids.push_back(i);
		}
		result = ShapefileHelper::GetClosestSnapPosition(this, x, y, maxDistance, ids, shapeIndex, *fx, *fy, *distance);
	}
	else
	{
		std::vector<long> ids;
		Extent box(x - maxDistance, x + maxDistance, y - maxDistance, y + maxDistance);
		if (this->SelectShapesCore(box, 0.0, SelectMode::INTERSECTION, ids, false))
		{
			result = ShapefileHelper::GetClosestSnapPosition(this, x, y, maxDistance, ids, shapeIndex, *fx, *fy, *distance);
		}
	}
	*retVal = result ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *****************************************************************
//		HasInvalidShapes()
// *****************************************************************
STDMETHODIMP CShapefile::HasInvalidShapes(VARIANT_BOOL* result)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *result = VARIANT_FALSE;
    const int numShapes = _shapeData.size();

    for (int i = 0; i < numShapes; i++)
    {
        IShape* shp = nullptr;
        this->get_Shape(i, &shp);

        if (!shp)
        {
            *result = VARIANT_TRUE;
            break;
        }

        VARIANT_BOOL retval = VARIANT_TRUE;
        shp->get_IsValid(&retval);
        shp->Release();
        if (retval == VARIANT_FALSE)
        {
            *result = VARIANT_TRUE;
            break;
        }
    }
    return S_OK;
}

// *****************************************************************
//		get_UndoList()
// *****************************************************************
STDMETHODIMP CShapefile::get_UndoList(IUndoList** pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (_undoList)
        _undoList->AddRef();
    *pVal = _undoList;
    return S_OK;
}

// *****************************************************************
//		Snappable()
// *****************************************************************
STDMETHODIMP CShapefile::get_Snappable(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _snappable;
    return S_OK;
}

STDMETHODIMP CShapefile::put_Snappable(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _snappable = newVal;
    return S_OK;
}

// *****************************************************************
//		ShapefileType2D()
// *****************************************************************
STDMETHODIMP CShapefile::get_ShapefileType2D(ShpfileType* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = ShapeUtility::Convert2D(_shpfiletype);
    return S_OK;
}

// *****************************************************************
//		FieldIndexByName()
// *****************************************************************
STDMETHODIMP CShapefile::get_FieldIndexByName(BSTR FieldName, LONG* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _table->get_FieldIndexByName(FieldName, pVal);
    return S_OK;
}

// ***************************************************
//		get_Selectable
// ***************************************************
STDMETHODIMP CShapefile::get_Selectable(VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retVal = _selectable;
    return S_OK;
}

STDMETHODIMP CShapefile::put_Selectable(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _selectable = newVal;
    return S_OK;
}

// *****************************************************************
//		Move()
// *****************************************************************
STDMETHODIMP CShapefile::Move(DOUBLE xProjOffset, DOUBLE yProjOffset, VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retVal = VARIANT_FALSE;
    if (_sourceType != sstInMemory)
    {
        ErrorMessage(tkSHPFILE_NOT_IN_EDIT_MODE);
        return S_OK;
    }
    long numShapes;
    get_NumShapes(&numShapes);
    for (long i = 0; i < numShapes; i++)
    {
        CComPtr<IShape> shp = nullptr;
        get_Shape(i, &shp);
        if (shp)
        {
            shp->Move(xProjOffset, yProjOffset);
        }
    }
    *retVal = VARIANT_TRUE;
    return S_OK;
}

// *****************************************************************
//		get_ShapeRendered()
// *****************************************************************
STDMETHODIMP CShapefile::get_ShapeRendered(LONG ShapeIndex, VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *pVal = VARIANT_FALSE;
    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
    }
    else
    {
        *pVal = _shapeData[ShapeIndex]->wasRendered() ? VARIANT_TRUE : VARIANT_FALSE;
    }
    return S_OK;
}

// *****************************************************************
//		MarkUndrawn()
// *****************************************************************
void CShapefile::MarkUndrawn()
{
    for (auto& i : _shapeData)
    {
        i->wasRendered(false);
    }
}

// *************************************************************
//		SortField()
// *************************************************************
STDMETHODIMP CShapefile::get_SortField(BSTR* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    USES_CONVERSION;
    *pVal = OLE2BSTR(_sortField);

    return S_OK;
}

STDMETHODIMP CShapefile::put_SortField(BSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    SysFreeString(_sortField);
    USES_CONVERSION;
    _sortField = OLE2BSTR(newVal);

    _sortingChanged = true;

    if (_labels)
    {
        _labels->UpdateSizeField();
    }

    return S_OK;
}

// *************************************************************
//		SortAscending()
// *************************************************************
STDMETHODIMP CShapefile::get_SortAscending(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *pVal = _sortAscending;

    return S_OK;
}

STDMETHODIMP CShapefile::put_SortAscending(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    _sortAscending = newVal;
    _sortingChanged = true;

    return S_OK;
}

// *************************************************************
//		UpdateSorting()
// *************************************************************
STDMETHODIMP CShapefile::UpdateSortField()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // this will trigger rereading of the table on next redraw
    _sortingChanged = true;

    return S_OK;
}

// *************************************************************
//		GetSorting()
// *************************************************************
bool CShapefile::GetSorting(vector<long>** indices)
{
    *indices = nullptr;

    if (!_sortingChanged)
    {
        *indices = &_sorting;
        return true;
    }

    long fieldIndex;
    get_FieldIndexByName(_sortField, &fieldIndex);

    if (fieldIndex == -1)
    {
        return false;
    }

    if (!_table)
    {
        return false;
    }

    _sortingChanged = false;

    if (((CTableClass*)_table)->GetSorting(fieldIndex, _sorting))
    {
        if (!_sortAscending)
        {
            std::reverse(_sorting.begin(), _sorting.end());
        }

        *indices = &_sorting;
        return true;
    }
    CallbackHelper::ErrorMsg("Failed to sort labels");

    return false;
}

// *************************************************************
//		HasOgrFidMapping()
// *************************************************************
STDMETHODIMP CShapefile::get_HasOgrFidMapping(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *pVal = (_hasOgrFidMapping ? VARIANT_TRUE : VARIANT_FALSE);

    return S_OK;
}

// *****************************************************************
//		OgrFid2ShapeIndex()
// *****************************************************************
STDMETHODIMP CShapefile::OgrFid2ShapeIndex(long OgrFid, LONG* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = -1;

    // this shapefile has to have an OGR FID mapping, and a mapping for the specified OgrFid
    if (!_hasOgrFidMapping || _ogrFid2ShapeIndex.find(OgrFid) == _ogrFid2ShapeIndex.end())
    {
        ErrorMessage(tkNO_OGR_DATA_WAS_LOADED);
    }
    else
    {
        // else return the mapping
        *retVal = _ogrFid2ShapeIndex[OgrFid];
    }
    return S_OK;
}
