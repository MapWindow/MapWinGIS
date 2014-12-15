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
		// to output shapefile as; so in case there was an error it was already reported
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
		IShapeValidationInfo* iinfo = ShapeValidator::Validate(*isf, m_globalSettings.outputValidation, svtOutput, 
									className, methodName, "", _globalCallback, _key, false);
		CShapefile* sf = (CShapefile*)this;		// writing validation into this shapefile
		sf->SetValidationInfo(iinfo, svtOutput);
		iinfo->Release();
		CShapeValidationInfo* info = (CShapeValidationInfo*)iinfo;
		if (info->validationStatus == tkShapeValidationStatus::OperationAborted)
			goto clear_result;
		return info;
	}
}

// **************************************************************
//		ValidateOutput()
// **************************************************************
bool CShapefile::ValidateOutput(IShapefile* sf, CString methodName, CString className, bool abortIfEmpty)
{
	if (!_isEditingShapes)
		return true;		// TODO: implement for disk-based shapefile
	return ValidateOutput(&sf, methodName, className, abortIfEmpty) != NULL;
}

// *********************************************************
//		CreateValidationList
// *********************************************************
void CShapefile::CreateValidationList(bool selectedOnly)
{
	if (_useValidationList)
		CallbackHelper::AssertionFailed("Attempting to create validation list which exists.");
	
	if (!_useValidationList)
	{
		size_t size = _shapeData.size();
		for(size_t i = 0; i < size; i++)
		{
			if (selectedOnly && !_shapeData[i]->selected)
			{
				_shapeData[i]->status = ShapeValidationStatus::Skip;
			}
			else
			{
				IShape* shp = NULL;
				this->get_Shape(i, &shp);
				_shapeData[i]->fixedShape = shp;
				_shapeData[i]->status = ShapeValidationStatus::Original; 
			}
		}
		_useValidationList = true;
	}
}

// *********************************************************
//		ClearValidationList
// *********************************************************
void CShapefile::ClearValidationList()
{
	_useValidationList = false;
	for(size_t i = 0; i < _shapeData.size(); i++)
	{
		if (_shapeData[i]->fixedShape)
		{
			_shapeData[i]->fixedShape->Release();
			_shapeData[i]->fixedShape = NULL;
		}
		_shapeData[i]->status = Original;
	}
}

// *********************************************************
//		SetValidatedShape
// *********************************************************
void CShapefile::SetValidatedShape(int shapeIndex, ShapeValidationStatus status, IShape* shape)
{
	if (status == Original)
	{
		CallbackHelper::AssertionFailed("Can't set Original status in SetValidatedShape.");
		return;
	}

	if (shapeIndex > (int)_shapeData.size())
	{
		CallbackHelper::AssertionFailed(Debug::Format("Invalid index for validated shape: %d.", shapeIndex));
		return;
	}

	if (_useValidationList)
	{
		if (_shapeData[shapeIndex]->fixedShape) {
			_shapeData[shapeIndex]->fixedShape->Release();
			_shapeData[shapeIndex]->fixedShape = NULL;
		}
		_shapeData[shapeIndex]->status = status;
		_shapeData[shapeIndex]->fixedShape = shape;
	}
	else
	{
		// edit in place
		if (!_isEditingShapes)
		{
			CallbackHelper::AssertionFailed("Attempt to update shape while not in edit mode.");
			return;
		}
		if (_shapeData[shapeIndex]->shape) _shapeData[shapeIndex]->shape->Release();
		_shapeData[shapeIndex]->shape = shape;
		this->RegisterNewShape(shape, shapeIndex);
	}
}

// *********************************************************
//		GetValidatedShape
// *********************************************************
HRESULT CShapefile::GetValidatedShape(int shapeIndex, IShape** retVal)
{
	if (_useValidationList)
	{
		IShape* shp = _shapeData[shapeIndex]->fixedShape;
		if (shp) shp->AddRef();
		*retVal = shp;
	}
	else
	{
		this->get_Shape(shapeIndex, retVal);
	}
	return S_OK;
}

// *********************************************************
//		ShapeAvailable
// *********************************************************
bool CShapefile::ShapeAvailable(int shapeIndex, VARIANT_BOOL selectedOnly)
{
	if (shapeIndex < 0 || shapeIndex >= (int)_shapeData.size())
		return false;
	if (!_shapeData[shapeIndex]->selected && selectedOnly)
		return false;
	if (_useValidationList && _shapeData[shapeIndex]->status == ShapeValidationStatus::Skip)
		return false;
	return true;
}

// *********************************************************
//		GetGeosGeometry
// *********************************************************
GEOSGeometry* CShapefile::GetGeosGeometry(int shapeIndex)
{
	return _shapeData[shapeIndex]->geosGeom;
}
#pragma endregion 

#pragma region Caching GEOS geometries
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