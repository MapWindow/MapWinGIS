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
#include <StdAfx.h>
#include "IndexShapeFiles.h"
#include "ShapeFileStream.h"

namespace IndexSearching
{
	/* ************************************************************************* **
	**
	** Function to create inde file on a shape file.
	**
	**   Creates baseName.dat and baseName.idx
	**
	** ************************************************************************* */
	bool CreateIndexFile(double utilization, int capacity, string baseName)
	{
		bool ret = false;
		IStorageManager* diskfile = nullptr;
		StorageManager::IBuffer* file = nullptr;
		ISpatialIndex* tree = nullptr;

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
		catch (char* str)
		{
			ret = false;
			cerr << "Got an error " << str << endl;
		}
		catch (exception ex)
		{
			ret = false;
			cerr << "Got an error " << ex.what() << endl;
		}
		catch (Tools::IllegalStateException ex)
		{
			ret = false;
			cerr << "Got an error " << ex.what() << endl;
		}
		catch (Tools::IllegalArgumentException ex)
		{
			ret = false;
			cerr << "Got an error " << ex.what() << endl;
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
	bool IsValidIndexFile(string baseName, int bufferSize)
	{
		bool ret = false;
		IStorageManager* diskfile = nullptr;
		StorageManager::IBuffer* file = nullptr;
		ISpatialIndex* tree = nullptr;

		try
		{
			diskfile = StorageManager::loadDiskStorageManager(baseName);
			// this will try to locate and open an already existing storage manager.
			file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, bufferSize, false);

			tree = RTree::loadRTree(*file, 1);
			ret = tree->isIndexValid();
		}
		catch (char* str)
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
	void QueryIndexFile(ISpatialIndex* spatialIndex, SpatialIndex::Region queryRegion, QueryTypeFlags queryType, ShapeIdxVisitor* vis)
	{
		try
		{
			// TODO: Use enum instead of magic number:
			if (queryType == 1)
				spatialIndex->intersectsWithQuery(queryRegion, *vis);
			else if (queryType == 2)
				spatialIndex->containsWhatQuery(queryRegion, *vis);
		}
		catch (exception ex)
		{
			cerr << "Error running query on spatial index." << endl;
		}
	}

	int  SelectShapesFromIndex(ISpatialIndex* spatialIndex, double* lowVals, double* hiVals, QueryTypeFlags queryType, CIndexSearching* resulSet)
	{
		int rCode = 0;

		try
		{
			vector <long> res;

			const double xmin = lowVals[0] < hiVals[0] ? lowVals[0] : hiVals[0];
			const double ymin = lowVals[1] < hiVals[1] ? lowVals[1] : hiVals[1];
			const double xmax = lowVals[0] > hiVals[0] ? lowVals[0] : hiVals[0];
			const double ymax = lowVals[1] > hiVals[1] ? lowVals[1] : hiVals[1];

			lowVals[0] = xmin;
			lowVals[1] = ymin;
			hiVals[0] = xmax;
			hiVals[1] = ymax;

			const Region* const queryRegion = new Region(lowVals, hiVals, 2);
			const auto vis = new ShapeIdxVisitor();

			QueryIndexFile(spatialIndex, *queryRegion, queryType, vis);

			resulSet->SetCapacity(vis->ids.size());
			// int arrSize = vis->ids.size();
			for (const unsigned i = 0; i < vis->ids.size(); vis->ids.pop())
			{
				const long val = static_cast<long>(vis->ids.front());
				//int len = resulSet->GetLength();
				resulSet->AddValue(val);
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
		uint8_t* pData = nullptr;
		uint32_t cLen = 0;
		d.getData(cLen, &pData);
		delete[] pData;

		const auto val = d.getIdentifier();
		ids.push(val);
	}

	void ShapeIdxVisitor::visitNode(const INode& n)
	{
		if (n.isLeaf())
			m_leafIO++;
		else
			m_indexIO++;
	}


	//08-24-2009 (sm) caching for performance
	CSpatialIndexCache& CSpatialIndexCache::Instance()
	{
		static CSpatialIndexCache spatialIndexCache;
		return spatialIndexCache;
	}

	ISpatialIndex* CSpatialIndexCache::GetSpatialIndexById(CSpatialIndexID spatialIndexId)
	{
		const auto mapIterator = m_cache.find(spatialIndexId);
		if (mapIterator != m_cache.end())
			return (*mapIterator).second.m_tree;

		return nullptr;
	}

	CSpatialIndexID CSpatialIndexCache::CacheSpatialIndex(ISpatialIndex* tree, IStorageManager* diskfile, StorageManager::IBuffer* file)
	{
		const CSpatialIndexID newSpatialIndexId = m_nextSpatialIndexID++;
		m_cache[newSpatialIndexId] = CacheItem(diskfile, file, tree);
		return newSpatialIndexId;
	}

	void CSpatialIndexCache::UncacheSpatialIndex(CSpatialIndexID spatialIndexId, bool releaseAll)
	{
		const auto mapIterator = m_cache.find(spatialIndexId);
		if (mapIterator != m_cache.end())
		{
			if (releaseAll)
				(*mapIterator).second.ReleaseAll();
			m_cache.erase(mapIterator);
		}
	}

	void CSpatialIndexCache::CacheItem::ReleaseAll()
	{
		delete m_tree;
		delete m_file;
		delete m_diskfile;
	}
}