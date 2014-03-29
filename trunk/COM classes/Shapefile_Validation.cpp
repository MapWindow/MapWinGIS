#include "stdafx.h"
#include "Shapefile.h"
#include "ShapeValidator.h"

#pragma region Validation
// ********************************************************************
//		ValidateInput()
// ********************************************************************
CShapeValidationInfo* CShapefile::ValidateInput(IShapefile* isf, CString methodName, 
												CString parameterName, VARIANT_BOOL selectedOnly, CString className)
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
	
	IShapeValidationInfo* iinfo = ShapeValidator::Validate(isf, m_globalSettings.inputValidation, svtInput, 
			className, methodName, parameterName, selectedOnly ? true: false);
	
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
CShapeValidationInfo* CShapefile::ValidateOutput(IShapefile** isf, CString methodName, CString className, bool abortIfEmpty)
{
	if (!(*isf)) return NULL;

	long numShapes;
	(*isf)->get_NumShapes(&numShapes);
	if (numShapes == 0 && abortIfEmpty)
	{
		ErrorMessage(tkRESULTINGSHPFILE_EMPTY);

clear_result:		
		// TODO: actually I don't see much sense in it; GlobalCallback if there is any must've been passed 
		// to output shapefile as well so in case there was an error it was already reported
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
									className, methodName, "", false);
		CShapefile* sf = (CShapefile*)this;		// writing validation into this shapefile
		sf->SetValidationInfo(iinfo, svtOutput);
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
		Debug::WriteLine("ERROR: Attempting to create validation list which exists");
	
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
		}
	}
}

// *********************************************************
//		SetValidatedShape
// *********************************************************
void CShapefile::SetValidatedShape(int shapeIndex, ShapeValidationStatus status, IShape* shape)
{
	if (status == Original)
	{
		Debug::WriteLine("ERROR: Can't set Original status in SetValidatedShape");
		return;
	}

	if (shapeIndex > (int)_shapeData.size())
	{
		Debug::WriteLine("ERROR: Invalid index for validated shape: %d", shapeIndex);
		return;
	}

	if (_useValidationList)
	{
		_shapeData[shapeIndex]->status = status;
		_shapeData[shapeIndex]->fixedShape = shape;
	}
	else
	{
		// edit in place
		if (!_isEditingShapes)
		{
			Debug::WriteLine("ERROR: attempt to substitute shape while not in edit mode");
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
		IShape* shp = _shapeData[shapeIndex]->shape;
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
// Used by:
// -Shapefile.GetRelatedShapes
// -Shapefile.Segmetize
void CShapefile::ReadGeosGeometries(VARIANT_BOOL selectedOnly)
{
	if (_geosGeometriesRead)
	{
		Debug::WriteLine("ERROR: attempt to reread GEOS geometries while they are in memory");
		ClearCachedGeometries();
	}
	
	// TODO!!!: can shapefile be edited so the geometries are no longer valid?
	// Free them after exiting each function!!!
	long percent = 0;
	int size = (int)_shapeData.size();
	for (int i = 0; i < size; i++)
	{
		Utility::DisplayProgress(globalCallback, i, size, "Converting to geometries", key, percent);
		
		if (!ShapeAvailable(i, selectedOnly))
			continue;

		if(_shapeData[i]->geosGeom)
			Debug::WriteLine("ERROR: GEOS Geometry was expected to be empty");

		IShape* shp = NULL;
		this->GetValidatedShape(i, &shp);
		if (shp)
		{
			GEOSGeom geom = GeometryConverter::Shape2GEOSGeom(shp);
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