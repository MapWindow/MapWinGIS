// IndexShapeFiles.cpp : Main functions for creating and querying indexes
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
#include "IndexShapeFiles.h"
#include "ShapeFileStream.h"

namespace IndexSearching
{
	/* ************************************************************************* **
	**
	** Function to create inde file on a shape file.
	**
	**   Creates baseName.mwd and baseName.mwx
	**
	** ************************************************************************* */
	bool createIndexFile(double utilization, int capacity, string baseName)
	  {
		bool ret = false;
	  IStorageManager         *diskfile;
	  StorageManager::IBuffer *file;
	  ISpatialIndex           *tree;

	  try
		{
		// Create a new storage manager with the provided base name and a 4K page size.
		  diskfile = StorageManager::createNewDiskStorageManager(baseName, 4096);
		  file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, capacity, false);

		ShapeFileStream inputStream(baseName);

		  // Create and bulk load a new RTree with dimensionality 2, using "file" as
		  // the StorageManager and the RSTAR splitting policy.
		  id_type indexIdentifier;
#ifndef __DEBUG
		  tree = RTree::createAndBulkLoadNewRTree(RTree::BLM_STR, inputStream, *file, utilization,
							 capacity, capacity, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);
#endif			
		  ret = tree->isIndexValid();

		}
	  catch (char *str)
		{
		ret = false;
		cerr << "Got an error " << str << endl;
		}
		delete tree;
		delete file;
		delete diskfile;

	  return(ret);
	  }
	/* ************************************************************************* **
	**
	** Function to query shape file.
	**
	** 
	**
	** ************************************************************************* */
	bool isValidIndexFile(string baseName, int bufferSize)
	  {
		bool ret = false;
	  IStorageManager         *diskfile;
	  StorageManager::IBuffer *file;
	  ISpatialIndex           *tree;

	  try
		{
			diskfile = StorageManager::loadDiskStorageManager(baseName);
			// this will try to locate and open an already existing storage manager.
			file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, bufferSize, false);
	    
			tree = RTree::loadRTree(*file, 1); 
			ret = tree->isIndexValid();
		}
	  catch (char *str)
		{
		cerr << "Got an error " << str << endl;
		}

	  delete tree;
		delete file;
		delete diskfile;
	  return ret;
	  }
	/* ************************************************************************* **
	**
	** Function to query shape file.
	**
	** 
	**
	** ************************************************************************* */
	void queryIndexFile(ISpatialIndex *spatialIndex, SpatialIndex::Region queryRegion, QueryTypeFlags queryType, ShapeIdxVisitor *vis)
	  {
	  try
		{
		  if (queryType == 1)
			spatialIndex->intersectsWithQuery(queryRegion, *vis);
		  else if (queryType == 2)
			spatialIndex->containsWhatQuery(queryRegion, *vis);
		}
	  catch (exception ex)
		{
		  cerr << "Error running query on spatial index."  << endl;
		}
	  }

	int  selectShapesFromIndex(ISpatialIndex *spatialIndex, double *lowVals, double *hiVals, QueryTypeFlags queryType, CIndexSearching *resulSet)
	{
	  int rCode = 0;
	  
	  try
		{
		  long val;
		  double xmin, ymin, xmax, ymax;
		  vector <long> res;
	      
		  xmin = (lowVals[0] < hiVals[0]) ? lowVals[0] : hiVals[0];
			  ymin = (lowVals[1] < hiVals[1]) ? lowVals[1] : hiVals[1];
			  xmax = (lowVals[0] > hiVals[0]) ? lowVals[0] : hiVals[0];
			  ymax = (lowVals[1] > hiVals[1]) ? lowVals[1] : hiVals[1];

		  lowVals[0] = xmin;
		  lowVals[1] = ymin;
		  hiVals[0] = xmax;
		  hiVals[1] = ymax;

		  Region *queryRegion = new Region(lowVals,hiVals,2);
		  ShapeIdxVisitor *vis = new ShapeIdxVisitor();

		  queryIndexFile(spatialIndex, *queryRegion, queryType, vis);

		  resulSet->setCapacity(vis->ids.size());
		  int arrSize = vis->ids.size();
		  for (unsigned i = 0; i < vis->ids.size();vis->ids.pop())
			{
			val = (long)vis->ids.front();
			int len = resulSet->getLength();
			resulSet->addValue(val);
			}
		  delete queryRegion;
		  delete vis;
		}
	  catch (exception ex)
		{
		rCode = -1;
		}
	  catch (Tools::IllegalStateException ex1)
		{
		rCode = -1;
		}
	  catch (Tools::IllegalArgumentException ex2)
		{
		rCode = -1;
		}
	  catch (...)
		{
		rCode = -1;
		}
	  return (rCode);
	}
	/* ************************************************************************* **
	**
	** Function to query shape file.
	**
	** 
	**
	** ************************************************************************* */
	void ShapeIdxVisitor::visitData(const IData& d)
		{

		// data should be an array of characters representing a Region as a string.
		byte* pData = 0;
		uint32_t cLen = 0;
		d.getData(cLen, &pData);
		delete[] pData;

	  __int64 val;
	  val = d.getIdentifier();
		ids.push(val);
		}
		
	void ShapeIdxVisitor::visitNode(const INode& n)
		{
	  if (n.isLeaf()) m_leafIO++;
		else m_indexIO++;
		}


	//08-24-2009 (sm) caching for performance
	CSpatialIndexCache& CSpatialIndexCache::Instance()
	{
		static CSpatialIndexCache spatialIndexCache;
		return spatialIndexCache;
	}

	ISpatialIndex* CSpatialIndexCache::getSpatialIndexByID(CSpatialIndexID spatialIndexID)
	{
		map<CSpatialIndexID, CacheItem>::iterator mapIterator = m_cache.find(spatialIndexID);
		if (mapIterator != m_cache.end())
			return (*mapIterator).second.m_tree;
		
		return NULL;
	}

	CSpatialIndexID CSpatialIndexCache::cacheSpatialIndex(ISpatialIndex *tree, IStorageManager *diskfile, StorageManager::IBuffer *file)
	{
		CSpatialIndexID newSpatialIndexID = m_nextSpatialIndexID++;
		m_cache[newSpatialIndexID] = CacheItem(diskfile, file, tree);
		return newSpatialIndexID;
	}

	void CSpatialIndexCache::uncacheSpatialIndex(CSpatialIndexID spatialIndexID, bool releaseAll)
	{
		map<CSpatialIndexID, CacheItem>::iterator mapIterator = m_cache.find(spatialIndexID);
		if (mapIterator != m_cache.end())
		{
			if (releaseAll)
				(*mapIterator).second.releaseAll();
			m_cache.erase(mapIterator);
		}
	}

	void CSpatialIndexCache::CacheItem::releaseAll()
	{
		delete m_tree;
		delete m_file;
		delete m_diskfile;
	}
}