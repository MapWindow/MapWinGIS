#include "stdafx.h"
#include "Shapefile.h"
#include "ShapeValidator.h"
#include "OgrConverter.h"
#include "GeosConverter.h"
#include "ShapeValidationInfo.h"

#pragma region Validation

// *****************************************************************
//		Validate()
// *****************************************************************
STDMETHODIMP CShapefile::Validate(tkShapeValidationMode validationMode, VARIANT_BOOL selectedOnly, IShapeValidationInfo** results)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*results = NULL;

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
	if (m_globalSettings.inputValidation != AbortOnErrors) {
		// this the only mode where shall be doing it before hand
		// in all other cases validation & fixing will be done in get_ValidateShape call directly
		return true;
	}

	IShapeValidationInfo* info = ValidateInputCore(isf, methodName, parameterName, selectedOnly, m_globalSettings.inputValidation, className);
	bool result = info != NULL;
	if (info) info->Release();
	return result;
}

// ********************************************************************
//		ValidateInputCore()
// ********************************************************************
IShapeValidationInfo* CShapefile::ValidateInputCore(IShapefile* isf, CString methodName, 
	CString parameterName, VARIANT_BOOL selectedOnly, tkShapeValidationMode validationMode, CString className, bool reportOnly)
{
	tkShapefileSourceType sourceType;
	if (isf->get_SourceType(&sourceType))
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
		return NULL;
	}
	
	long numShapes;
	isf->get_NumShapes(&numShapes);
	if (numShapes == 0)
	{
		ErrorMessage(tkSHAPEFILE_IS_EMPTY);
		return NULL;
	}
	
	long numSelected;
	isf->get_NumSelected(&numSelected);
	if (selectedOnly && numSelected == 0)
	{
		ErrorMessage(tkSELECTION_EMPTY);
		return NULL;
	}
	
	IShapeValidationInfo* iinfo = ShapeValidator::Validate(isf, validationMode, svtInput,
			className, methodName, parameterName, _globalCallback, _key, selectedOnly ? true: false, reportOnly);
	
	CShapefile* sf = (CShapefile*)isf;
	sf->SetValidationInfo(iinfo, svtInput);

	CShapeValidationInfo* info = (CShapeValidationInfo*)iinfo;
	if (info->validationStatus == OperationAborted)
	{
		ErrorMessage(tkABORTED_ON_INPUT_VALIDATION);
		return NULL;
	}
	return info;
}

// ********************************************************************
//		ValidateOutput()
// ********************************************************************
IShapeValidationInfo* CShapefile::ValidateOutput(IShapefile** isf, CString methodName, CString className, bool abortIfEmpty)
{
	if (!(*isf)) return NULL;

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
		(*isf) = NULL;
		return NULL;
	}
	else
	{
		IShapeValidationInfo* iinfo = ShapeValidator::Validate(*isf, m_globalSettings.outputValidation, svtOutput, className, methodName, "", _globalCallback, _key, false);

		CShapefile* sf = (CShapefile*)this;		// writing validation into this shapefile
		sf->SetValidationInfo(iinfo, svtOutput);
		iinfo->Release();

		CShapeValidationInfo* info = (CShapeValidationInfo*)iinfo;

		if (info->validationStatus == tkShapeValidationStatus::OperationAborted) {
			goto clear_result;
		}

		return info;
	}
}

// **************************************************************
//		ValidateOutput()
// **************************************************************
bool CShapefile::ValidateOutput(IShapefile* sf, CString methodName, CString className, bool abortIfEmpty)
{
	if (!_isEditingShapes) {
		return true;
	}

	return ValidateOutput(&sf, methodName, className, abortIfEmpty) != NULL;		// validationInfo instance is referenced by shapefile
}

// *********************************************************
//		GetValidatedShape
// *********************************************************
HRESULT CShapefile::GetValidatedShape(int shapeIndex, IShape** retVal)
{
	IShape* shp = NULL;
	get_Shape(shapeIndex, &shp);

	if (!shp) {
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

			if (vb) {
				// everything is good
				*retVal = shp;
				return S_OK;
			}

			IShape* shpNew = NULL;
			(*retVal)->FixUp(&shpNew);

			if (shpNew) {
				// fixed, no problems
				shp->Release();
				*retVal = shpNew;
				return S_OK;
			}


			if (m_globalSettings.inputValidation == TryFixProceedOnFailure) {
				// can't be fixed? we don't care :)
				*retVal = shp;
			}
			else {
				// TryFixSkipOnFailure
				// well, we should cope somehow without it :(
				*retVal = NULL;
			}
	}
	
	return S_OK;
}

// *********************************************************
//		ShapeAvailable
// *********************************************************
bool CShapefile::ShapeAvailable(int shapeIndex, VARIANT_BOOL selectedOnly)
{
	if (shapeIndex < 0 || shapeIndex >= (int)_shapeData.size()) {
		return false;
	}

	if (!_shapeData[shapeIndex]->selected() && selectedOnly) {
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
		for (size_t i = 0; i < _shapeData.size(); i++)
		{
			if (_shapeData[i]->geosGeom)
			{
				GEOSGeom_destroy(_shapeData[i]->geosGeom);
				_shapeData[i]->geosGeom = NULL;
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
	int size = (int)_shapeData.size();
	for (int i = 0; i < size; i++)
	{
		CallbackHelper::Progress(_globalCallback, i, size, "Converting to geometries", _key, percent);
		
		if (!ShapeAvailable(i, selectedOnly))
			continue;

		if(_shapeData[i]->geosGeom)
			CallbackHelper::AssertionFailed("GEOS Geometry during the reading was expected to be empty.");

		IShape* shp = NULL;
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