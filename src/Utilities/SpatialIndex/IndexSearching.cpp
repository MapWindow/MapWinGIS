// SpatialIndexing.cpp : Defines the entry point for the DLL application.
//
// Index Searching Library
//
// Copyright (C) 2008 Versaterm Inc.
//
// The core of this library is the Spatial Index Library by Marios Hadjieleftheriou. 
//
//		http://www.research.att.com/~marioh/spatialindex/index.html
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
#include "StdAfx.h"
#include "IndexSearching.h"
#include "IndexShapeFiles.h"

// ReSharper disable once CppInconsistentNaming
namespace IndexSearching
{
	/* ************************************************************************* **
	**
	**   Function to create a spatial index on a shape file.
	**
	**
	**
	** ************************************************************************* */
	int CreateSpatialIndex(const double utilization, const int capacity, const char* fileName)
	{
		const std::string baseName = fileName;

		const bool bRet = CreateIndexFile(utilization, capacity, baseName);

		return bRet;
	}
	/* ************************************************************************* **
	**
	**  Function to check if current index file is valid
	**
	**  Returns true/false
	**
	** ************************************************************************* */
	bool IsValidSpatialIndex(const char* fileName, const int bufferSize)
	{
		const string baseName = fileName;
		return IsValidIndexFile(baseName, bufferSize);
	}
	/* ************************************************************************* **
	**
	**  Function to select shapes from shapefile
	**
	**  Returns a <queue> of found shape file index numbers.
	**
	** ************************************************************************* */
	int SelectShapesFromIndex(const char* fileName, double* lowVals, double* hiVals, const QueryTypeFlags queryType, const int bufferSize, CIndexSearching* resulSet)
	{
		/*	IStorageManager* diskfile = nullptr;
			StorageManager::IBuffer* file = nullptr;
			ISpatialIndex* tree = nullptr;*/

		int rCode;
		try
		{
			//diskfile = StorageManager::loadDiskStorageManager(string(fileName));
			//// this will try to locate and open an already existing storage manager.
			//file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, bufferSize, false);
			//tree = RTree::loadRTree(*file, 1);
			//rCode = SelectShapesFromIndex(tree, lowVals, hiVals, queryType, resulSet);

			auto s4 = static_cast<string>(fileName);
			const auto diskfile = unique_ptr<IStorageManager>(StorageManager::loadDiskStorageManager(s4));
			// this will try to locate and open an already existing storage manager.
			const auto file = unique_ptr<StorageManager::IBuffer>(StorageManager::createNewRandomEvictionsBuffer(*diskfile, bufferSize, false));
			const auto tree = unique_ptr<ISpatialIndex>(RTree::loadRTree(*file, 1));

			rCode = SelectShapesFromIndex(tree.get(), lowVals, hiVals, queryType, resulSet);

		}
		catch (...)
		{
			cerr << "Error running query on spatial index." << endl;
			rCode = static_cast<int>(SpatialIndexQueryErrors::unknownError);
		}

		//delete tree;
		//delete file;
		//delete diskfile;

		return rCode;
	}

	int SelectShapesFromIndex(const CSpatialIndexID spatialIndexId, double* lowVals, double* highVals, const QueryTypeFlags queryType, CIndexSearching* resulSet)
	{
		ISpatialIndex* spatialIndex = CSpatialIndexCache::Instance().GetSpatialIndexById(spatialIndexId);
		if (spatialIndex == nullptr)
			return static_cast<int>(SpatialIndexQueryErrors::spatialIndexNotFound);

		return SelectShapesFromIndex(spatialIndex, lowVals, highVals, queryType, resulSet);
	}

	bool LoadSpatialIndex(string baseName, const bool validateIndex, const int bufferSize, CSpatialIndexID& spatialIndexId)
	{
		//IStorageManager* diskfile;
		//StorageManager::IBuffer* file;
		//ISpatialIndex* tree;

		try
		{
			//diskfile = StorageManager::loadDiskStorageManager(baseName);
			// this will try to locate and open an already existing storage manager.
			//file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, bufferSize, false);
			//tree = RTree::loadRTree(*file, 1);
			//spatialIndexId = CSpatialIndexCache::Instance().CacheSpatialIndex(tree, diskfile, file);

			const auto diskfile = unique_ptr<IStorageManager>(StorageManager::loadDiskStorageManager(baseName));
			// this will try to locate and open an already existing storage manager.
			const auto file = unique_ptr<StorageManager::IBuffer>(StorageManager::createNewRandomEvictionsBuffer(*diskfile, bufferSize, false));
			const auto tree = unique_ptr<ISpatialIndex>(RTree::loadRTree(*file, 1));

			spatialIndexId = CSpatialIndexCache::Instance().CacheSpatialIndex(tree.get(), diskfile.get(), file.get());

			if (validateIndex && !tree->isIndexValid())
			{
				CSpatialIndexCache::Instance().UncacheSpatialIndex(spatialIndexId, true);
				return false;
			}
		}
		catch (...)
		{
			cerr << "Error loading spatial index." << endl;
			return false;
		}

		return true;
	}

	void UnloadSpatialIndex(const CSpatialIndexID spatialIndex)
	{
		CSpatialIndexCache::Instance().UncacheSpatialIndex(spatialIndex, true);
	}

	// This is the constructor of a class that has been exported.
	// see SpatialIndexing.h for the class definition
	CIndexSearching::CIndexSearching()
	{
		resultList = new IDXLIST();
	}
	CIndexSearching::~CIndexSearching()
	{
		delete resultList;
	}
	long CIndexSearching::GetValue(const int index)
	{
		const long& val = resultList->at(index);
		return(val);
	}
	void CIndexSearching::AddValue(const long val)
	{
		resultList->push_back(val);
	}
	int  CIndexSearching::GetLength()
	{
		return static_cast<int>(resultList->size());
	}

	void CIndexSearching::SetCapacity(const int capacity)
	{
		resultList->reserve(capacity);
	}
}
