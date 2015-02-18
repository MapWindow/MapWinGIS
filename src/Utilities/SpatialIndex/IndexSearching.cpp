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
#include "stdafx.h"
#include "IndexSearching.h"
#include "IndexShapeFiles.h"

namespace IndexSearching
{
	/* ************************************************************************* **
	**
	**   Function to create a spatial index on a shape file.
	** 
	**  
	**
	** ************************************************************************* */
	 int createSpatialIndex(double utilization, int capacity, char *fileName)
	  {
	  
	  std::string baseName = fileName;

	  bool bRet = createIndexFile(utilization, capacity, baseName);

	  return (int) bRet;
	  }
	/* ************************************************************************* **
	**
	**  Function to check if current index file is valid
	** 
	**  Returns true/false
	**
	** ************************************************************************* */
	 bool isValidSpatialIndex(const char *fileName, int bufferSize)
	  {
	  string baseName = fileName;
	  return(isValidIndexFile(baseName, bufferSize));
	  }
	/* ************************************************************************* **
	**
	**  Function to select shapes from shapefile
	** 
	**  Returns a <queue> of found shape file index numbers.
	**
	** ************************************************************************* */
	 int selectShapesFromIndex(const char* fileName, double *lowVals, double *hiVals, QueryTypeFlags queryType, int bufferSize, CIndexSearching *resulSet)
	  {	
	  IStorageManager         *diskfile;
	  StorageManager::IBuffer *file;
	  ISpatialIndex           *tree;

	  int rCode;
	  try
	  {
		  
			diskfile = StorageManager::loadDiskStorageManager(string(fileName));
			// this will try to locate and open an already existing storage manager.
			file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, bufferSize, false);

			tree = RTree::loadRTree(*file, 1); 

			rCode = selectShapesFromIndex(tree, lowVals, hiVals, queryType, resulSet);
	  }
	  catch(...)
	  {
		  cerr << "Error running query on spatial index." << endl;
		  rCode = unknownError;
	  }

	  delete tree;
	  delete file;
	  delete diskfile;

	  return rCode;
	  }

	 int selectShapesFromIndex(CSpatialIndexID spatialIndexID, double *lowVals, double *hiVals, QueryTypeFlags queryType, CIndexSearching *resulSet)
	{
		ISpatialIndex* spatialIndex = CSpatialIndexCache::Instance().getSpatialIndexByID(spatialIndexID);
		if (spatialIndex == NULL)
			return spatialIndexNotFound;

		return selectShapesFromIndex(spatialIndex, lowVals, hiVals, queryType, resulSet);
	}

	 bool loadSpatialIndex(string baseName, bool validateIndex, int bufferSize, CSpatialIndexID& spatialIndexID)
	{
	  IStorageManager         *diskfile;
	  StorageManager::IBuffer *file;
	  ISpatialIndex           *tree;

	  try
	  {
		  diskfile = StorageManager::loadDiskStorageManager(baseName);
		  // this will try to locate and open an already existing storage manager.
		  file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, bufferSize, false);

		  tree = RTree::loadRTree(*file, 1); 
		   
		  spatialIndexID = CSpatialIndexCache::Instance().cacheSpatialIndex(tree, diskfile, file);

		  if (validateIndex && !tree->isIndexValid())
		  {
			CSpatialIndexCache::Instance().uncacheSpatialIndex(spatialIndexID, true);
			return false;
		  }
	  }
	  catch(...)
	  {
		  cerr << "Error loading spatial index." << endl;
		  return false;
	  }

	  return true;
	}

	 void unloadSpatialIndex(CSpatialIndexID spatialIndex)
	{
		CSpatialIndexCache::Instance().uncacheSpatialIndex(spatialIndex, true);
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
	long CIndexSearching::getValue(int index)
	  {
	  long &val = resultList->at(index);
	  return(val);
	  }
	void CIndexSearching::addValue(long Val)
	  {
	  resultList->push_back(Val);
	  }
	int  CIndexSearching::getLength(void)
	  {
	  return (resultList->size());
	  }

	void CIndexSearching::setCapacity(int capacity)
	  {
	  resultList->reserve(capacity);
	  }
}