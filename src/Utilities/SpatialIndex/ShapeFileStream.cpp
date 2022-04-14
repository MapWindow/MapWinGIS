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
#include "StdAfx.h"
#include "ShapeFileStream.h"

//namespace IndexSearching
//{
	/* ************************************************************************* **
	**
	**
	**
	** ************************************************************************* */
IData* ShapeFileStream::GetNextShapeRecord()
{
	if (m_pShpNext == nullptr)
		return nullptr;

	// TODO: VS2019 says ret is unused. Is this correct?
	RTree::Data* ret = m_pShpNext;
	m_pShpNext = nullptr;

	return nullptr;
}

/* ************************************************************************* **
**
**
**
** ************************************************************************* */
IData* ShapeFileStream::getNext()
{

	if (m_pShpNext == nullptr)
		return nullptr;

	RTree::Data* ret = m_pShpNext;
	m_pShpNext = nullptr;

	ReadShxFile(idxID++);

	return ret;
}
/* ************************************************************************* **
**
**
**
** ************************************************************************* */
IData* ShapeFileStream::GetExact(int index)
{
	int intBuffer = 0;
	intBuffer = 100 + ((index - 1) * 8);
	m_ShpIdx.seekg(intBuffer);
	m_ShpIdx.read((char*)&intBuffer, sizeof(int));
	if (m_ShpIdx.eof() || m_ShpIdx.bad())
	{
		cerr << "got end of file or error" << endl;
		return nullptr;
	}
	SwapEndian((char*)&intBuffer, sizeof(int));
	idxOffset = intBuffer;
	// Record Length
	m_ShpIdx.read((char*)&intBuffer, sizeof(int));
	SwapEndian((char*)&intBuffer, sizeof(int));
	idxLength = intBuffer;

	ReadShpFile();
	RTree::Data* ret = m_pShpNext;

	return ret;
}
/* ************************************************************************* **
**
**
**
** ************************************************************************* */
void ShapeFileStream::ReadShpFile()
{
	int    intBuffer = 0;
	double low[2], high[2];

	streamoff i = m_ShpFile.tellg();
	m_ShpFile.seekg(idxOffset * 2);
	if (!m_ShpFile.good())
	{
		return;
	}

	m_ShpFile.read((char*)&intBuffer, sizeof(int));
	SwapEndian((char*)&intBuffer, sizeof(int));
	int recNum = intBuffer;
	// read shape rec length
	m_ShpFile.read((char*)&intBuffer, sizeof(int));
	SwapEndian((char*)&intBuffer, sizeof(int));
	int recLen = intBuffer;

	// read shape type
	m_ShpFile.read((char*)&intBuffer, sizeof(int));
	int shpType = intBuffer;

	switch (shpType)
	{
	case 0: // NULL Shape
		break;
	case ShpfileType::SHP_POINT:
	case ShpfileType::SHP_POINTM:
	case ShpfileType::SHP_POINTZ: // Point types
		m_ShpFile.read((char*)&low[0], sizeof(double));
		m_ShpFile.read((char*)&low[1], sizeof(double));
		high[0] = low[0];
		high[1] = low[1];
		break;
	default:
		// Bounding Box 
		m_ShpFile.read((char*)&low[0], sizeof(double));
		m_ShpFile.read((char*)&low[1], sizeof(double));
		m_ShpFile.read((char*)&high[0], sizeof(double));
		m_ShpFile.read((char*)&high[1], sizeof(double));
		break;
	}

	auto r = Region(low, high, 2);
	m_pShpNext = new RTree::Data(0, nullptr, r, recNum);

}
/* ************************************************************************* **
**
**
**
** ************************************************************************* */
void ShapeFileStream::ReadShxFile(int recordId)
{
	int intBuffer = 0;

	if (m_ShpIdx.eof() || m_ShpIdx.bad())
	{
		return;
	}
	streamoff i = m_ShpIdx.tellg();

	if (recordId == -1)
		ReadShxFileHeader();
	else
	{
		idxID = recordId;
		// Offset
		m_ShpIdx.read((char*)&intBuffer, sizeof(int));
		if (m_ShpIdx.eof() || m_ShpIdx.bad())
		{
			//      cerr << "got end of file or error" << endl;
			return;
		}
		SwapEndian((char*)&intBuffer, sizeof(int));
		idxOffset = intBuffer;
		// Record Length
		m_ShpIdx.read((char*)&intBuffer, sizeof(int));
		SwapEndian((char*)&intBuffer, sizeof(int));
		idxLength = intBuffer;

		ReadShpFile();
	}
}
/* ************************************************************************* **
**
**  Read shapefile index header.
**
** ************************************************************************* */
void ShapeFileStream::ReadShxFileHeader()
{
	int intBuffer = 0;

	ShapeFileStream::m_ShpIdx.seekg(-1, ios_base::end);
	ShapeFileStream::SHXIdxLen = m_ShpIdx.tellg();
	ShapeFileStream::SHXIdxLen++;
	ShapeFileStream::m_ShpIdx.seekg(0, ios_base::beg);

	// Validate file code
	m_ShpIdx.read((char*)&intBuffer, sizeof(int));
	SwapEndian((char*)&intBuffer, sizeof(int));
	if (intBuffer != FILE_CODE)
		return;

	// Skip unused
	constexpr int unused = UNUSEDVAL;
	for (int i = 0; i < UNUSEDSIZE; i++)
	{
		m_ShpIdx.read((char*)&intBuffer, sizeof(int));
		SwapEndian((char*)&intBuffer, sizeof(int));
		if (intBuffer != unused)
			return;
	}

	// read filelength
	m_ShpIdx.read((char*)&intBuffer, sizeof(int));
	SwapEndian((char*)&intBuffer, sizeof(int));
	int filelength = intBuffer;
	//  cerr << "filelength=" << filelength <<  endl;

		// Validate version
	m_ShpIdx.read((char*)&intBuffer, sizeof(int));
	if (intBuffer != VERSION)
		return;

	// read shapefile type
	m_ShpIdx.read((char*)&intBuffer, sizeof(int));
	int shpfiletype = intBuffer;

	// read bounding region
	double minX, minY, maxX, maxY, minZ, maxZ, minM, maxM;

	m_ShpIdx.read((char*)&minX, sizeof(double));
	m_ShpIdx.read((char*)&minY, sizeof(double));
	m_ShpIdx.read((char*)&maxX, sizeof(double));
	m_ShpIdx.read((char*)&maxY, sizeof(double));
	m_ShpIdx.read((char*)&minZ, sizeof(double));
	m_ShpIdx.read((char*)&maxZ, sizeof(double));
	m_ShpIdx.read((char*)&minM, sizeof(double));
	m_ShpIdx.read((char*)&maxM, sizeof(double));


	ReadShpFileHeader();
	//Read First Index
	ReadShxFile(0);
}
/* ************************************************************************* **
**
**  Read shapefile header.
**
** ************************************************************************* */
void ShapeFileStream::ReadShpFileHeader()
{
	int intBuffer = 0;

	ShapeFileStream::m_ShpFile.seekg(0, ios_base::beg);

	// Validate file code
	m_ShpFile.read((char*)&intBuffer, sizeof(int));
	SwapEndian((char*)&intBuffer, sizeof(int));
	if (intBuffer != FILE_CODE)
		return;

	// Skip unused
	int unused = UNUSEDVAL;
	for (int i = 0; i < UNUSEDSIZE; i++)
	{
		m_ShpFile.read((char*)&intBuffer, sizeof(int));
		SwapEndian((char*)&intBuffer, sizeof(int));
		if (intBuffer != unused)
			return;
	}

	// read filelength
	m_ShpFile.read((char*)&intBuffer, sizeof(int));
	SwapEndian((char*)&intBuffer, sizeof(int));
	int filelength = intBuffer;

	// Validate version
	m_ShpFile.read((char*)&intBuffer, sizeof(int));
	if (intBuffer != VERSION)
		return;

	// read shapefile type
	m_ShpFile.read((char*)&intBuffer, sizeof(int));
	int shpfiletype = intBuffer;

	// read bounding region
	double minX, minY, maxX, maxY, minZ, maxZ, minM, maxM;

	m_ShpFile.read((char*)&minX, sizeof(double));
	m_ShpFile.read((char*)&minY, sizeof(double));
	m_ShpFile.read((char*)&maxX, sizeof(double));
	m_ShpFile.read((char*)&maxY, sizeof(double));
	m_ShpFile.read((char*)&minZ, sizeof(double));
	m_ShpFile.read((char*)&maxZ, sizeof(double));
	m_ShpFile.read((char*)&minM, sizeof(double));
	m_ShpFile.read((char*)&maxM, sizeof(double));

}
/* ************************************************************************* **
**
**
**
** ************************************************************************* */
inline void ShapeFileStream::SwapEndian(char* a, int size)
{
	for (int i = 0; i < size * .5; i++)
	{
		const char hold = a[i];
		a[i] = a[size - i - 1];
		a[size - i - 1] = hold;
	}
}
//}