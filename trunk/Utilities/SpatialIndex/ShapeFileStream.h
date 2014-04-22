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
#pragma warning( disable : 4290 )
#include <spatialindex/SpatialIndex.h>
#include <ios>

using namespace std;
using namespace SpatialIndex;

//Shapefile File Info
#define HEADER_BYTES_16 50
#define HEADER_BYTES_32 100
#define FILE_CODE      9994
#define VERSION        1000
#define UNUSEDSIZE 5
#define UNUSEDVAL		0     	    
#define RECORD_HEADER_LENGTH_32 8
#define RECORD_SHAPE_TYPE_32 8

class ShapeFileStream : public IDataStream
{
public:
	ShapeFileStream(string shapeFile) : m_pShpNext(0), m_pIdxNext(0)
    {
		idxID = -1;
		idxOffset = -1;
		idxLength = -1;
		SHXIdxLen = -1;

		string sShpFile = std::string(shapeFile) + ".shp";
		string sShpIdxFile = std::string(shapeFile) + ".shx";

		m_ShpIdx.open(sShpIdxFile.c_str(),ios_base::in | ios_base::binary);
		m_ShpFile.open(sShpFile.c_str(),ios_base::in | ios_base::binary);

		if (! m_ShpIdx || !m_ShpFile)
			throw "Input file not found.";

		readSHXFile(-1);

		if (!m_ShpIdx.good() || !m_ShpFile.good())
        {
			throw Tools::IllegalStateException("Input file not found.");
        }
     }

	 virtual ~ShapeFileStream()
	 {
		if (m_pShpNext != 0)
        delete m_pShpNext;
	 }

	 virtual IData* getNext();
	 virtual IData* getNextShapeRecord();
     virtual IData* getExact(int index);
	 virtual bool hasNext() throw (Tools::NotSupportedException)
	 {
		return (m_pShpNext != 0);
	 }

	 virtual uint32_t size() throw (Tools::NotSupportedException)
	 {
		throw Tools::NotSupportedException("Operation not supported.");
  	 }

	 virtual void rewind() throw (Tools::NotSupportedException)
  	 {
		throw Tools::NotSupportedException("Operation not supported.");
	 }

	  std::ifstream m_ShpIdx, m_ShpFile;
	  RTree::Data* m_pShpNext;

  private:
    void readSHXFile(int recordID);
    void readSHPFile(void);
    void readSHXFileHeader(void);
    void readSHPFileHeader(void);
    inline void swapEndian(char* a,int size);

	  RTree::Data* m_pIdxNext;

    int idxID;
    int idxOffset;
    int idxLength;
    int SHXIdxLen;
};
