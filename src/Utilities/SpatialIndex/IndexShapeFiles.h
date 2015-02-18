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
#pragma once
#include "spatialindex/SpatialIndex.h"
#include "IndexSearching.h"
#include <string>
#include <list>
#include <queue>
#include <ios>

//Shapefile File Info
#define HEADER_BYTES_16 50
#define HEADER_BYTES_32 100
#define FILE_CODE      9994
#define VERSION        1000
#define UNUSEDSIZE 5
#define UNUSEDVAL		0     	    
#define RECORD_HEADER_LENGTH_32 8
#define RECORD_SHAPE_TYPE_32 8

using namespace std;
using namespace SpatialIndex;

namespace IndexSearching
{
	class ShapeIdxVisitor : public IVisitor
	  {
	  private:

	  public:
		  size_t m_indexIO;
		  size_t m_leafIO;
		queue<SpatialIndex::id_type> ids;

	  public:
		~ShapeIdxVisitor()
		  {
		  }
		  ShapeIdxVisitor() : m_indexIO(0), m_leafIO(0) {}

		void ShapeIdxVisitor::visitData(const IData& d);

		  void visitNode(const INode& n);

		void visitData(std::vector<const IData*>& v){};
	  };

	class CSpatialIndexCache 
	  {
	  public:
		static CSpatialIndexCache& Instance();
		ISpatialIndex* getSpatialIndexByID(CSpatialIndexID spatialIndexID);
		CSpatialIndexID cacheSpatialIndex(ISpatialIndex* tree, IStorageManager* diskfile, StorageManager::IBuffer* file);
		void uncacheSpatialIndex(CSpatialIndexID spatialIndexID, bool releaseAll);
	  private:
		struct CacheItem
		  {
		  public:
			IStorageManager         *m_diskfile;
			StorageManager::IBuffer *m_file;
			ISpatialIndex           *m_tree;
	      
			CacheItem() : m_diskfile(NULL), m_file(NULL), m_tree(NULL) {}
			CacheItem(IStorageManager* diskfile, StorageManager::IBuffer* file, ISpatialIndex* tree) : m_diskfile(diskfile), m_file(file), m_tree(tree){}
			void releaseAll();
		  };

		CSpatialIndexCache() : m_nextSpatialIndexID(1) {}
		CSpatialIndexCache(const CSpatialIndexCache&);                 // Prevent copy-construction
		CSpatialIndexCache& operator=(const CSpatialIndexCache&);	   // Prevent assignment

		map<CSpatialIndexID, CacheItem> m_cache;
		CSpatialIndexID m_nextSpatialIndexID;
	  };

	bool createIndexFile(double utilization, int capacity, std::string baseName);
	bool isValidIndexFile(string baseName, int bufferSize);
	void queryIndexFile(ISpatialIndex *spatialIndex, SpatialIndex::Region queryRegion, int bufferSize, QueryTypeFlags queryType, ShapeIdxVisitor *vis);
	int  selectShapesFromIndex(ISpatialIndex *spatialIndex, double *lowVals, double *hiVals, QueryTypeFlags queryType, CIndexSearching *resulSet);
}