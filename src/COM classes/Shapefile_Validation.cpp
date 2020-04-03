//********************************************************************************************************
//File name: Shapefile_Validation.cpp
//Description: Implementation of the CShapefile (see other cpp files as well)
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
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
// -------------------------------------------------------------------------------------------------------
// lsu 3-02-2011: split the initial Shapefile.cpp file to make entities of the reasonable size

#include "StdAfx.h"
#include "Shapefile.h"
#include "ShapeValidator.h"
#include "OgrConverter.h"
#include "GeosConverter.h"
#include "ShapeValidationInfo.h"

// ReSharper disable CppUseAuto

#pragma region Validation

// *****************************************************************
//		Validate()
// *****************************************************************
STDMETHODIMP CShapefile::Validate(tkShapeValidationMode validationMode, VARIANT_BOOL selectedOnly,
                                  IShapeValidationInfo** results)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *results = nullptr;

    if (validationMode == NoValidation)
        return S_OK;

    VARIANT_BOOL editing;
    get_EditingShapes(&editing);
    if (!editing)
    {
        ErrorMessage(tkNO_FIXING_IN_DISK_MODE);
        *results = ValidateInputCore(this, "Validate", "this", selectedOnly, validationMode, "Shapefile", true);
    }
    else
    {
        *results = ValidateInputCore(this, "Validate", "this", selectedOnly, validationMode);
    }
    return S_OK;
}

// ********************************************************************
//		ValidateInput()
// ********************************************************************
bool CShapefile::ValidateInput(IShapefile* isf, CString methodName,
                               CString parameterName, VARIANT_BOOL selectedOnly, CString className /*= "Shapefile"*/)
{
    // MWGIS-132; this code, suggested by CLang for MWGIS-104 on 24 Aug 2018,
    // is being removed because it prevents any in-memory Shapefile from passing 
    // validation (since in-memory shapefiles have no FILE * (_shpfile == NULL). 
    // This also affected all OGR layers, and was observed when attempting to 
    // load an OGR layer that required reprojection on-the-fly.
    //// Always check if the shapefile is open:
    //if (_shpfile == nullptr)
    //{
    //    ErrorMessage(tkFILE_NOT_OPEN);
    //    return false;
    //}

    if (m_globalSettings.inputValidation != AbortOnErrors)
    {
        // this the only mode where shall be doing it before hand
        // in all other cases validation & fixing will be done in get_ValidateShape call directly
        return true;
    }

    IShapeValidationInfo* info = ValidateInputCore(isf, methodName, parameterName, selectedOnly,
                                                   m_globalSettings.inputValidation,
                                                   className);
    const bool result = info != nullptr;
    if (info) info->Release();
    return result;
}

// ********************************************************************
//		ValidateInputCore()
// ********************************************************************
IShapeValidationInfo* CShapefile::ValidateInputCore(IShapefile* isf, CString methodName,
                                                    CString parameterName, VARIANT_BOOL selectedOnly,
                                                    tkShapeValidationMode validationMode, CString className,
                                                    bool reportOnly)
{
    tkShapefileSourceType sourceType;
    if (isf->get_SourceType(&sourceType))
    {
        ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
        return nullptr;
    }

    long numShapes;
    isf->get_NumShapes(&numShapes);
    if (numShapes == 0)
    {
        ErrorMessage(tkSHAPEFILE_IS_EMPTY);
        return nullptr;
    }

    long numSelected;
    isf->get_NumSelected(&numSelected);
    if (selectedOnly && numSelected == 0)
    {
        ErrorMessage(tkSELECTION_EMPTY);
        return nullptr;
    }

    IShapeValidationInfo* iinfo = ShapeValidator::Validate(isf, validationMode, svtInput,
                                                           className, methodName, parameterName, _globalCallback, _key,
                                                           selectedOnly != 0, reportOnly);

    auto* sf = (CShapefile*)isf;
    sf->SetValidationInfo(iinfo, svtInput);

    auto* info = (CShapeValidationInfo*)iinfo;
    if (info->validationStatus == OperationAborted)
    {
        ErrorMessage(tkABORTED_ON_INPUT_VALIDATION);
        return nullptr;
    }
    return info;
}

// ********************************************************************
//		ValidateOutput()
// ********************************************************************
IShapeValidationInfo* CShapefile::ValidateOutput(IShapefile** isf, CString methodName, CString className,
                                                 bool abortIfEmpty)
{
    if (!*isf) return nullptr;

    long numShapes;
    (*isf)->get_NumShapes(&numShapes);
    if (numShapes == 0 && abortIfEmpty)
    {
        ErrorMessage(tkRESULTINGSHPFILE_EMPTY);

    clear_result:
        // TODO: actually I don't see much sense in it; GlobalCallback, if there is any, must have been passed 
        // to output shapefile as well; so in case there was an error it was already reported
        long errorCode = 0;
        (*isf)->get_LastErrorCode(&errorCode);
        if (errorCode != 0)
            ErrorMessage(errorCode);

        VARIANT_BOOL vb;
        (*isf)->Close(&vb);
        (*isf)->Release();
        *isf = nullptr;
        return nullptr;
    }

    IShapeValidationInfo* iinfo = ShapeValidator::Validate(*isf, m_globalSettings.outputValidation, svtOutput,
                                                           className, methodName, "", _globalCallback, _key, false);

    auto* sf = (CShapefile*)this; // writing validation into this shapefile
    sf->SetValidationInfo(iinfo, svtOutput);
    iinfo->Release();

    auto* info = (CShapeValidationInfo*)iinfo;

    if (info->validationStatus == OperationAborted)
    {
        goto clear_result;
    }

    return info;
}

// **************************************************************
//		ValidateOutput()
// **************************************************************
bool CShapefile::ValidateOutput(IShapefile* sf, CString methodName, CString className, bool abortIfEmpty)
{
    if (!_isEditingShapes)
    {
        return true;
    }

    return ValidateOutput(&sf, methodName, className, abortIfEmpty) != nullptr;
    // validationInfo instance is referenced by shapefile
}

// *********************************************************
//		GetValidatedShape
// *********************************************************
HRESULT CShapefile::GetValidatedShape(int shapeIndex, IShape** retVal)
{
    IShape* shp = nullptr;
    get_Shape(shapeIndex, &shp);

    if (!shp)
    {
        return S_OK;
    }

    switch (m_globalSettings.inputValidation)
    {
    case NoValidation:
    case AbortOnErrors:
        // for abort on errors validation must be run before the execution of the method,
        // the fact that we are processing further means that it has been passed
        *retVal = shp;
        return S_OK;
    case TryFixProceedOnFailure:
    case TryFixSkipOnFailure:
        VARIANT_BOOL vb;
        (*retVal)->get_IsValid(&vb);

        if (vb)
        {
            // everything is good
            *retVal = shp;
            return S_OK;
        }

        IShape* shpNew = nullptr;
        (*retVal)->FixUp(&shpNew);

        if (shpNew)
        {
            // fixed, no problems
            shp->Release();
            *retVal = shpNew;
            return S_OK;
        }


        if (m_globalSettings.inputValidation == TryFixProceedOnFailure)
        {
            // can't be fixed? we don't care :)
            *retVal = shp;
        }
        else
        {
            // TryFixSkipOnFailure
            // well, we should cope somehow without it :(
            *retVal = nullptr;
        }
    }

    return S_OK;
}

// *********************************************************
//		ShapeAvailable
// *********************************************************
bool CShapefile::ShapeAvailable(int shapeIndex, VARIANT_BOOL selectedOnly)
{
    if (shapeIndex < 0 || shapeIndex >= (int)_shapeData.size())
    {
        return false;
    }

    if (!_shapeData[shapeIndex]->selected() && selectedOnly)
    {
        return false;
    }

    return true;
}

#pragma endregion

#pragma region Caching GEOS geometries

// *********************************************************
//		GetGeosGeometry
// *********************************************************
GEOSGeometry* CShapefile::GetGeosGeometry(int shapeIndex)
{
    return _shapeData[shapeIndex]->geosGeom;
}

// *********************************************************
//		ClearCachedGeometries()
// *********************************************************
STDMETHODIMP CShapefile::ClearCachedGeometries()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    if (_geosGeometriesRead)
    {
        for (auto& i : _shapeData)
        {
            if (i->geosGeom)
            {
                GEOSGeom_destroy(i->geosGeom);
                i->geosGeom = nullptr;
            }
        }
        _geosGeometriesRead = false;
    }
    return S_OK;
}

// *********************************************************
//		ReadGeosGeometries()
// *********************************************************
void CShapefile::ReadGeosGeometries(VARIANT_BOOL selectedOnly)
{
    if (_geosGeometriesRead)
    {
        CallbackHelper::AssertionFailed("Attempt to reread GEOS geometries while they are in memory.");
        ClearCachedGeometries();
    }

    long percent = 0;
    const int size = (int)_shapeData.size();
    for (int i = 0; i < size; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, size, "Converting to geometries", _key, percent);

        if (!ShapeAvailable(i, selectedOnly))
            continue;

        if (_shapeData[i]->geosGeom)
            CallbackHelper::AssertionFailed("GEOS Geometry during the reading was expected to be empty.");

        IShape* shp = nullptr;
        this->GetValidatedShape(i, &shp);
        if (shp)
        {
            // ReSharper disable once CppLocalVariableMayBeConst
            GEOSGeom geom = GeosConverter::ShapeToGeom(shp);
            if (geom)
            {
                _shapeData[i]->geosGeom = geom;
            }
            shp->Release();
        }
    }
    _geosGeometriesRead = true;
}

// read only those geometries requested by the specified array
void CShapefile::ReadGeosGeometries(std::set<int> list)
{
    if (_geosGeometriesRead)
    {
        // pre-clear cache
        ClearCachedGeometries();
    }

    // list is small subset of shapes,
    // so iterate list instead of shapeData
    for each (int i in list)
    {
        IShape* shp = nullptr;
        this->GetValidatedShape(i, &shp);
        if (shp)
        {
            GEOSGeom geom = GeosConverter::ShapeToGeom(shp);
            if (geom)
            {
                _shapeData[i]->geosGeom = geom;
            }
            shp->Release();
        }
    }

    _geosGeometriesRead = true;
}

#pragma endregion

// ReSharper restore CppUseAuto
