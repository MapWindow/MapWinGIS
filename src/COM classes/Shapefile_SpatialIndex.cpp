//********************************************************************************************************
//File name: Shapefile.cpp
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

#pragma region SpatialIndex
// *****************************************************************
//		get_HasSpatialIndex()
// *****************************************************************
//ajp June 2008 Property does spatial index exist
STDMETHODIMP CShapefile::get_HasSpatialIndex(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_hasSpatialIndex = FALSE;

	if (_shpfileName.GetLength() <= 3)
	{
		ErrorMessage(tkINVALID_FOR_INMEMORY_OBJECT);
		return S_OK;
	}

	try
	{
		// TODO: Stop using custom extension?? Then we can use LibSpatial out of the box:
		const CString mwdfileName = _shpfileName.Left(_shpfileName.GetLength() - 3) + "mwd";
		const CString mwxfileName = _shpfileName.Left(_shpfileName.GetLength() - 3) + "mwx";
		if (Utility::FileExists(mwdfileName) && Utility::FileExists(mwxfileName))
		{
			_hasSpatialIndex = TRUE;
			// TODO: Also check if the spatial index is valid??
		}
	}
	catch (...)
	{
		_hasSpatialIndex = FALSE;
	}

	*pVal = _hasSpatialIndex ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// *****************************************************************
//		get_HasSpatialIndex()
// *****************************************************************
//ajp June 2008 Property does spatial index exist
STDMETHODIMP CShapefile::put_HasSpatialIndex(VARIANT_BOOL pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_hasSpatialIndex = pVal;	// CreateSpatialIndex should be used to create it
	return S_OK;
}

bool TestIndexSearching()
{
	__try
	{
		IndexSearching::IsValidSpatialIndex("", 0);
		return true;
	}
	__except (1)
	{
		return false;
	}
}

// *****************************************************************
//		get/put_UseSpatialIndex()
// *****************************************************************
//ajp June 2008 Property use spatial indexing
STDMETHODIMP CShapefile::get_UseSpatialIndex(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (_useSpatialIndex)
	{
		//useSpatialIndex = TestIndexSearching();
	}
	*pVal = _useSpatialIndex ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CShapefile::put_UseSpatialIndex(VARIANT_BOOL pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_useSpatialIndex = pVal;

	// Unload spatial index in case it needs to be recreated
	if (!_useSpatialIndex && _spatialIndexLoaded)
	{
		IndexSearching::UnloadSpatialIndex(_spatialIndexID);
	}
	return S_OK;
}

// *****************************************************************
//		get/put_SpatialIndexMaxAreaPercent()
// *****************************************************************
//08-24-2009 (sm) spatial index performance
STDMETHODIMP CShapefile::put_SpatialIndexMaxAreaPercent(DOUBLE newVal)
{
	_spatialIndexMaxAreaPercent = newVal;
	return S_OK;
}
STDMETHODIMP CShapefile::get_SpatialIndexMaxAreaPercent(DOUBLE* pVal)
{
	*pVal = _spatialIndexMaxAreaPercent;
	return S_OK;
}

// *****************************************************************
//		get_CanUseSpatialIndex()
// *****************************************************************
//Check that the spatial index exists, is set to be used and should be used.
STDMETHODIMP CShapefile::get_CanUseSpatialIndex(IExtents* pArea, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = VARIANT_FALSE;

	if (_shpfileName.GetLength() <= 3)
	{
		ErrorMessage(tkINVALID_FOR_INMEMORY_OBJECT);
		return S_OK;
	}

	double xMin, xMax, yMin, yMax, zMin, zMax;
	pArea->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
	const Extent extents(xMin, xMax, yMin, yMax);

	if (_isEditingShapes)
	{
		ErrorMessage(tkSHPFILE_IN_EDIT_MODE);
		return S_OK;
	}

	if (!_useSpatialIndex)
	{
		ErrorMessage(tkUSE_SPATIALINDEX_IS_FALSE);
		return S_OK;
	}

	VARIANT_BOOL spatialIndexExists;
	get_HasSpatialIndex(&spatialIndexExists);

	if (spatialIndexExists)
	{
		const double xM = min(_maxX, extents.right);
		const double xm = max(_minX, extents.left);
		const double yM = min(_maxY, extents.top);
		const double ym = max(_minY, extents.bottom);

		const double shapeFileArea = (_maxX - _minX) * (_maxY - _minY);
		const double selectShapeArea = (xM - xm) * (yM - ym);

		if (selectShapeArea / shapeFileArea < _spatialIndexMaxAreaPercent)
		{
			//when large portions of the map are being drawn,
			//the spatial index *probably* won't help, don't use it.
			if (_spatialIndexLoaded)
			{
				*pVal = VARIANT_TRUE;
				return S_OK;
			}

			USES_CONVERSION;
			const string baseName = W2A(_shpfileName.Left(_shpfileName.GetLength() - 4));		// TODO: use Unicode
			if (IndexSearching::LoadSpatialIndex(baseName, false, _spatialIndexNodeCapacity, _spatialIndexID))
			{
				_spatialIndexLoaded = true;
				*pVal = VARIANT_TRUE;
				return S_OK;
			}
		}
	}

	return S_OK;
}

// ***********************************************************
//		CreateSpatialIndex()
// ***********************************************************
//ajp June 2008 Function to create an Index file
__declspec(deprecated("This is a deprecated function, use CShapefile::CreateSpatialIndex0() instead"))
STDMETHODIMP CShapefile::CreateSpatialIndex(BSTR ShapefileName, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;

	*retval = VARIANT_TRUE;

	CString tmp_shpfileName = OLE2CA(ShapefileName);
	if (tmp_shpfileName.GetLength() <= 3)
	{
		*retval = VARIANT_FALSE;
		ErrorMessage(tkINVALID_FILENAME);
	}
	else
	{
		string baseName;
		baseName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 4);

		// 0.9 = utilization rate, 100 = Node Capacity
		// Creates two files baseName.dat and baseName.idx
		try
		{
			if (!IndexSearching::CreateSpatialIndex(0.9, _spatialIndexNodeCapacity, (char*)baseName.c_str()))
			{
				*retval = VARIANT_FALSE;
				ErrorMessage(tkINVALID_FILENAME);
			}
		}
		catch (...)
		{
			*retval = VARIANT_FALSE;
		}
	}

	return S_OK;
}

// ***********************************************************
//		CreateSpatialIndex()
// ***********************************************************
//pm Feb 2022 No need to pass in filename
STDMETHODIMP CShapefile::CreateSpatialIndex0(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;

	*retval = VARIANT_FALSE;

	if (_shpfileName.GetLength() <= 3)
	{
		ErrorMessage(tkINVALID_FOR_INMEMORY_OBJECT);
		return S_OK;
	}

	const string baseName = W2A(_shpfileName.Left(_shpfileName.GetLength() - 4));	// TODO: use Unicode, is also used in different locations. Perhaps create a function for it?

	// 0.9 = utilization rate, 100 = Node Capacity
	// Creates two files baseName.dat and baseName.idx
	try
	{
		if (!IndexSearching::CreateSpatialIndex(0.9, _spatialIndexNodeCapacity, (char*)baseName.c_str()))
		{
			// ErrorMessage(tkINVALID_FILENAME);  // TODO: Is this the expected error code? Isn't tkFAILED_TO_BUILD_SPATIAL_INDEX better?
			ErrorMessage(tkFAILED_TO_BUILD_SPATIAL_INDEX);
		}
		else
		{
			*retval = VARIANT_TRUE;
		}
	}
	catch (...)
	{
		// TODO: Don't log exception?
		ErrorMessage(tkFAILED_TO_BUILD_SPATIAL_INDEX);
		*retval = VARIANT_FALSE;
	}


	return S_OK;
}

// ***********************************************************
//		IsSpatialIndexValid()
// ***********************************************************
STDMETHODIMP CShapefile::IsSpatialIndexValid(VARIANT_BOOL* retval)
{
	VARIANT_BOOL hasSpatialIndex;
	get_HasSpatialIndex(&hasSpatialIndex);
	if (!hasSpatialIndex)
		*retval = VARIANT_FALSE;
	else
	{
		USES_CONVERSION;
		const string baseName = W2A(_shpfileName.Left(_shpfileName.GetLength() - 4));		// TODO: use Unicode
		const bool bIsValid = IndexSearching::IsValidSpatialIndex(baseName.c_str(), _spatialIndexNodeCapacity);
		*retval = bIsValid ? VARIANT_TRUE : VARIANT_FALSE;
	}

	return S_OK;
}
#pragma endregion


#pragma region QuadTree
// ********************************************************************
//		QuickQueryInEditMode()
// ********************************************************************
//Neio 2009/07/21 
STDMETHODIMP CShapefile::QuickQueryInEditMode(IExtents* boundBox, int** result, int* resultCount)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!_isEditingShapes)
	{
		ErrorMessage(tkSHPFILE_NOT_IN_EDIT_MODE);
		return S_OK;
	}

	if (_useQTree)
	{
		double xMin, yMin, zMin, xMax, yMax, zMax;
		boundBox->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);

		const vector<int> r = _qtree->GetNodes(QTreeExtent(xMin, xMax, yMax, yMin));
		const int size = r.size();
		*result = new int[size];

		memcpy(*result, &r[0], sizeof(int) * size);
		*resultCount = size;
	}
	return S_OK;
}

// *****************************************************************
//		get_UseQTree()
// *****************************************************************
STDMETHODIMP CShapefile::get_UseQTree(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _useQTree ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *****************************************************************
//		put_UseQTree()
// *****************************************************************
STDMETHODIMP CShapefile::put_UseQTree(VARIANT_BOOL pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (pVal)
	{
		if (_qtree == nullptr)
			this->GenerateQTree();
		_useQTree = TRUE;
	}
	else
	{
		_useQTree = FALSE;
		delete _qtree;
		_qtree = nullptr;
	}
	return S_OK;
}

// **********************************************************
//		ClearQTree
// **********************************************************
void CShapefile::ClearQTree(double* xMin, double* xMax, double* yMin, double* yMax, double* zMin, double* zMax)
{
	if (_qtree)
	{
		delete _qtree;
		_qtree = nullptr;
	}

	// _qtree = this->GenerateEmptyQTree(xMin, yMin, zMin, xMax, yMax, zMax);
	_qtree = this->GenerateEmptyQTree(xMin, xMax, yMin, yMax, zMin, zMax);
}

// **********************************************************
//		GenerateEmptyQTree
// **********************************************************
QTree* CShapefile::GenerateEmptyQTree(double* xMin, double* xMax, double* yMin, double* yMax, double* zMin, double* zMax)
{
	IExtents* ext = nullptr;
	this->get_Extents(&ext);
	if (!ext) return nullptr;

	ext->GetBounds(xMin, yMin, zMin, xMax, yMax, zMax);
	ext->Release();

	const auto qtree = new QTree(QTreeExtent(*xMin, *xMax, *yMax, *yMin));

	return qtree;
}

// **********************************************************
//		GenerateQTree
// **********************************************************
void CShapefile::GenerateQTree()
{
	if (_qtree)
	{
		delete _qtree;
		_qtree = nullptr;
	}

	_qtree = GenerateQTreeCore(false);
}


// **********************************************************************
// 						GenerateQTreeCore()				           
// **********************************************************************
QTree* CShapefile::GenerateQTreeCore(bool selectedOnly)
{

	double xMin, xMax, yMin, yMax, zMin, zMax;
	QTree* qtree = this->GenerateEmptyQTree(&xMin, &xMax, &yMin, &yMax, &zMin, &zMax);

	if (_shapeData.empty())
		return qtree;

	long percent;
	const int numShapes = static_cast<int>(_shapeData.size());
	for (int i = 0; i < numShapes; i++)
	{
		if (!ShapeAvailable(i, selectedOnly))
			continue;

		this->QuickExtentsCore(i, &xMin, &yMin, &xMax, &yMax);

		QTreeNode node;
		node.Extent.left = xMin;
		node.Extent.right = xMax;
		node.Extent.top = yMax;
		node.Extent.bottom = yMin;
		node.index = i;
		qtree->AddNode(node);

		CallbackHelper::Progress(_globalCallback, i, numShapes, "Building index...", _key, percent);
	}
	CallbackHelper::ProgressCompleted(_globalCallback, _key);

	return qtree;
}
#pragma endregion

// Build the tree anew for geoprocessing operations, as the original one
// probably not 100% accurate/optimal + we may need only selected shapes

// **********************************************************************
// 						GenerateTempQTree()				           
// **********************************************************************
bool CShapefile::GenerateTempQTree(bool selectedOnly)
{
	ClearTempQTree();
	_tempTree = GenerateQTreeCore(selectedOnly);
	return _tempTree != nullptr;
}

// **********************************************************************
// 						ClearTempQTree()				           
// **********************************************************************
void CShapefile::ClearTempQTree()
{
	if (_tempTree)
	{
		delete _tempTree;
		_tempTree = nullptr;
	}
}

// **********************************************************************
// 						GetTempQtree()				           
// **********************************************************************
QTree* CShapefile::GetTempQTree()
{
	return _tempTree;
}

// **********************************************************************
// 						RemoveSpatialIndex()				           
// **********************************************************************
STDMETHODIMP CShapefile::RemoveSpatialIndex(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_TRUE;
	// TODO: Don't use custom extensions anymore:
	const CStringW names[] = { L"mwd", L"mwx" };

	for (const auto& i : names)
	{
		CString name = _shpfileName.Left(_shpfileName.GetLength() - 3) + i;
		if (Utility::FileExists(name))
		{
			if (remove(name) != 0) {
				ErrorMessage(tkCANT_DELETE_FILE);
				*retVal = VARIANT_FALSE;
			}
		}
	}
	return S_OK;
}