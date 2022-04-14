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
#include <locale>
#include <codecvt>
#include <gsl/util>

#pragma region SpatialIndex
// *****************************************************************
//		get_HasSpatialIndex()
// Unit test in unittest_net6.Shapefile.SpatialIndexTests.HasSpatialIndexTest
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
		// Stop using custom extension! We're uinge LibSpatial as vcpkg package:
		//const CString mwdfileName = _shpfileName.Left(_shpfileName.GetLength() - 3) + "mwd";
		//const CString mwxfileName = _shpfileName.Left(_shpfileName.GetLength() - 3) + "mwx";
		const CString datFilename = _shpfileName.Left(_shpfileName.GetLength() - 3) + "dat";
		const CString idxFilename = _shpfileName.Left(_shpfileName.GetLength() - 3) + "idx";
		if (Utility::FileExists(datFilename) && Utility::FileExists(idxFilename))
		{
			_hasSpatialIndex = TRUE;
			// TODO: Also check if the spatial index is valid??
		}
	}
	catch (...)
	{
		_hasSpatialIndex = FALSE;
	}

	// Since LibSpatial v1.9.3 (added Feb. 2022) we use the default file extensions:
	if (!_hasSpatialIndex)
	{
		// Check if it has the old index files:
		VARIANT_BOOL hasOldIndexFiles;
		get_HasOldSpatialIndex(&hasOldIndexFiles);
		if (hasOldIndexFiles)
		{
			// Migrate files by deleting the old ones and creating new ones:
			VARIANT_BOOL vb;
			RemoveSpatialIndex(&vb);
			CreateSpatialIndex(nullptr, &vb);
			if (vb == VARIANT_TRUE) _hasSpatialIndex = TRUE;
		}
	}

	*pVal = _hasSpatialIndex ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CShapefile::get_HasOldSpatialIndex(VARIANT_BOOL* pVal)
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
		const CString datFilename = _shpfileName.Left(_shpfileName.GetLength() - 3) + "mwd";
		const CString idxFilename = _shpfileName.Left(_shpfileName.GetLength() - 3) + "mwx";
		if (Utility::FileExists(datFilename) && Utility::FileExists(idxFilename))
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
//		put_HasSpatialIndex()
// *****************************************************************
//ajp June 2008 Property does spatial index exist
STDMETHODIMP CShapefile::put_HasSpatialIndex(const VARIANT_BOOL pVal)
{
	//pm, feb. 2022 TODO: Do we need this method. Is a bit confusing if it exists but doesn't do anything.
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_hasSpatialIndex = pVal;	// TODO: CreateSpatialIndex should be used to create it
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
STDMETHODIMP CShapefile::put_UseSpatialIndex(const VARIANT_BOOL pVal)
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
STDMETHODIMP CShapefile::put_SpatialIndexMaxAreaPercent(const DOUBLE newVal)
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
// Unit test in unittest_net6.Shapefile.SpatialIndexTests.CanUseSpatialIndex
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
			// const string baseName = GetFileNameFromPath(_shpfileName);
			if (IndexSearching::LoadSpatialIndex(baseName, false, _spatialIndexNodeCapacity, _spatialIndexID))
			{
				_spatialIndexLoaded = true;
				*pVal = VARIANT_TRUE;
				return S_OK;
			}
		}
		else
		{
			ErrorMessage(tkEXCEEDS_SPATIALINDEXMAXAREAPERCENT);
			return S_OK;
		}
	}

	return S_OK;
}

// ***********************************************************
//		CreateSpatialIndex()
// Unit test in unittest_net6.Shapefile.SpatialIndexTests.CreateSpatialIndexTest
// ***********************************************************
//ajp June 2008 Function to create an Index file
STDMETHODIMP CShapefile::CreateSpatialIndex(BSTR shapefileName, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;

	*retval = VARIANT_FALSE;

	string baseName;
	wstring wbaseName;
	string convertedName;
	//setup converter from widestring to UTF8 unicode string
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	if (CComBSTR(shapefileName).Length() < 2)
	{
		if (_shpfileName.GetLength() <= 3)
		{
			ErrorMessage(tkINVALID_FOR_INMEMORY_OBJECT);
			return S_OK;
		}

		// set up for unicode test
		wbaseName = _shpfileName.Left(_shpfileName.GetLength() - 4);
		convertedName = converter.to_bytes(wbaseName);

		baseName = W2A(_shpfileName.Left(_shpfileName.GetLength() - 4));	// TODO: use Unicode, is also used in different locations. Perhaps create a function for it?
	}
	else
	{
		// set up for unicode test
		const CStringW fileName(shapefileName);
		wbaseName = fileName.Left(fileName.GetLength() - 4);
		convertedName = converter.to_bytes(wbaseName);

		const auto tmp_shpfileName = CString(shapefileName);
		if (tmp_shpfileName.GetLength() <= 3)
		{
			*retval = VARIANT_FALSE;
			ErrorMessage(tkINVALID_FILENAME);
		}
		else
		{
			baseName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 4);
		}
	}

	// if baseName is not the same as converted_str, then input name was likely Unicode
	if (baseName != convertedName)
	{
		ErrorMessage(tkCANNOT_APPLY_UNICODE_TO_SPATIALINDEX);
		return S_OK;
	}

	try
	{
		if (!IndexSearching::CreateSpatialIndex(0.9, _spatialIndexNodeCapacity, baseName.c_str()))
		{
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
// Unit test in unittest_net6.Shapefile.SpatialIndexTests.IsSpatialIndexValidTest
// ***********************************************************
STDMETHODIMP CShapefile::IsSpatialIndexValid(VARIANT_BOOL* retval)
{
	*retval = VARIANT_FALSE;

	if (_shpfileName.GetLength() <= 3)
	{
		ErrorMessage(tkINVALID_FOR_INMEMORY_OBJECT);
		return S_OK;
	}

	VARIANT_BOOL hasSpatialIndex;
	get_HasSpatialIndex(&hasSpatialIndex);
	if (!hasSpatialIndex) {
		ErrorMessage(tkHAS_NO_SPATIALINDEX);
		return S_OK;
	}

	USES_CONVERSION;
	const string baseName = W2A(_shpfileName.Left(_shpfileName.GetLength() - 4));		// TODO: use Unicode
	const bool bIsValid = IndexSearching::IsValidSpatialIndex(baseName.c_str(), _spatialIndexNodeCapacity);
	*retval = bIsValid ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// **********************************************************************
// 						RemoveSpatialIndex()
// Unit test in unittest_net6.Shapefile.SpatialIndexTests.RemoveSpatialIndex
// **********************************************************************
STDMETHODIMP CShapefile::RemoveSpatialIndex(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (_shpfileName.GetLength() <= 3)
	{
		ErrorMessage(tkINVALID_FOR_INMEMORY_OBJECT);
		return S_OK;
	}

	// Don't use custom extensions anymore, when they exist do remove them
	const CStringW oldNames[] = { L"mwd", L"mwx" };
	for (const auto& i : oldNames)
	{
		CString name = _shpfileName.Left(_shpfileName.GetLength() - 3) + i;
		if (Utility::FileExists(name))
		{
			*retVal = VARIANT_TRUE;
			if (remove(name) != 0) {
				// Don't report error, migration
				//ErrorMessage(tkCANT_DELETE_FILE);
				//*retVal = VARIANT_FALSE;
			}
		}
	}

	const CStringW newNames[] = { L"dat", L"idx" };
	for (const auto& i : newNames)
	{
		CString name = _shpfileName.Left(_shpfileName.GetLength() - 3) + i;
		if (Utility::FileExists(name))
		{
			*retVal = VARIANT_TRUE;
			if (remove(name) != 0) {
				ErrorMessage(tkCANT_DELETE_FILE);
				*retVal = VARIANT_FALSE;
			}
		}
		else
		{
			ErrorMessage(tkHAS_NO_SPATIALINDEX);
		}
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
		const int size = gsl::narrow_cast<int>(r.size());
		*result = new int[size];

		//memcpy(*result, &r[0], sizeof(int) * size);
		memcpy(*result, &gsl::at(r, 0), sizeof(int) * size);
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
QTree* CShapefile::GenerateQTreeCore(const bool selectedOnly)
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

		CallbackHelper::Progress(_globalCallback, i, numShapes, "Building QTree index...", _key, percent);
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
