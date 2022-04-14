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
#include <queue>

//Shapefile File Info
#define HEADER_BYTES_16 50
#define HEADER_BYTES_32 100
#define FILE_CODE      9994
#define VERSION        1000
#define UNUSEDSIZE 5
#define UNUSEDVAL		0     	    
#define RECORD_HEADER_LENGTH_32 8
#define RECORD_SHAPE_TYPE_32 8

using namespace SpatialIndex; // TODO: Where to move to?

// ReSharper disable once CppInconsistentNaming
namespace IndexSearching
{
	class ShapeIdxVisitor final : public IVisitor
	{
	private:

	public:
		// ReSharper disable once CppInconsistentNaming
		size_t m_indexIO;
		// ReSharper disable once CppInconsistentNaming
		size_t m_leafIO;
		queue<id_type> ids;

	public:
		~ShapeIdxVisitor() override
		{
		}
		ShapeIdxVisitor() : m_indexIO(0), m_leafIO(0) {}

		void visitData(const IData& d) override;

		void visitNode(const INode& n) override;

		void visitData(std::vector<const IData*>& v) override {}
	};

	class CSpatialIndexCache
	{
	public:
		static CSpatialIndexCache& Instance();
		ISpatialIndex* GetSpatialIndexById(CSpatialIndexID spatialIndexId);
		CSpatialIndexID CacheSpatialIndex(ISpatialIndex* tree, IStorageManager* diskfile, StorageManager::IBuffer* file);
		void UncacheSpatialIndex(CSpatialIndexID spatialIndexId, bool releaseAll);
	private:
		struct CacheItem
		{
		public:
			// ReSharper disable once CppInconsistentNaming
			IStorageManager* m_diskfile;
			// ReSharper disable once CppInconsistentNaming
			StorageManager::IBuffer* m_file;
			// ReSharper disable once CppInconsistentNaming
			ISpatialIndex* m_tree;

			CacheItem() : m_diskfile(nullptr), m_file(nullptr), m_tree(nullptr) {}
			CacheItem(IStorageManager* diskfile, StorageManager::IBuffer* file, ISpatialIndex* tree)
				: m_diskfile(diskfile), m_file(file), m_tree(tree) {}
			void ReleaseAll();
		};

		CSpatialIndexCache() : m_nextSpatialIndexID(1) {}
		CSpatialIndexCache(const CSpatialIndexCache&);                 // Prevent copy-construction
		CSpatialIndexCache& operator=(const CSpatialIndexCache&);	   // Prevent assignment

		// ReSharper disable once CppInconsistentNaming
		map<CSpatialIndexID, CacheItem> m_cache;
		// ReSharper disable once CppInconsistentNaming
		CSpatialIndexID m_nextSpatialIndexID;
	};

	bool CreateIndexFile(double utilization, int capacity, std::string baseName);
	bool IsValidIndexFile(string baseName, int bufferSize);
	void QueryIndexFile(ISpatialIndex* spatialIndex, Region queryRegion, int bufferSize, QueryTypeFlags queryType, ShapeIdxVisitor* vis);
	int  SelectShapesFromIndex(ISpatialIndex* spatialIndex, double* lowVals, double* hiVals, QueryTypeFlags queryType, CIndexSearching* resulSet);
}
