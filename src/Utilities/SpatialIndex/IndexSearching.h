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
//
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SPATIALINDEXING_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
//  functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

// Windows Header Files:
#pragma once
#include <windows.h>
#include <string>
#include <queue>

using namespace std;

namespace IndexSearching
{
	typedef vector<long> IDXLIST;
	// This class is exported from the SpatialIndexing.dll
	class CIndexSearching 
	{
		public:
			CIndexSearching(void);
		 ~CIndexSearching(void);
		  long getValue(int index);
		  void addValue(long Val);
		  int  getLength(void);
		  void setCapacity(int capacity);

		private:
		  IDXLIST *resultList;
	};

	typedef unsigned int CSpatialIndexID;

	enum QueryTypeFlags
	  { intersection = 1,
		contained = 2
	  };

	enum SpatialIndexQueryErrors
	{
		success = 0,
		unknownError = -1,
		spatialIndexNotFound = -2
	};

	int  createSpatialIndex(double utilization, int capacity, char *fileName);
	bool isValidSpatialIndex(const char *fileName, int bufferSize);
	bool loadSpatialIndex(string baseName, bool validateIndex, int bufferSize, CSpatialIndexID& spatialIndexID);
	void unloadSpatialIndex(CSpatialIndexID spatialIndex);
	int  selectShapesFromIndex(CSpatialIndexID CSpatialIndexID, double *lowVals, double *highVals, QueryTypeFlags queryType, CIndexSearching *resulSet);
	int  selectShapesFromIndex(const char *fileName, double *lowVals, double *hiVals, QueryTypeFlags queryType, int bufferSize, CIndexSearching *resulSet);
}