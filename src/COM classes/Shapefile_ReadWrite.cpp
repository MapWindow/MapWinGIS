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

#pragma once
#include "stdafx.h"
#include "Shapefile.h"
#include "Shape.h"
#include "ShapeWrapper.h"
#include "ShapeHelper.h"

#pragma region GetShape

// ************************************************************
//		get_Shape()
// ************************************************************
STDMETHODIMP CShapefile::get_Shape(long shapeIndex, IShape **pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
	VARIANT_BOOL vbretval = VARIANT_FALSE;

	// out of bounds?
	if( shapeIndex < 0 || shapeIndex >= (long)_shapeData.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*pVal = NULL;
		return S_OK;
	}

	// last shape in the append mode is always in memory
	bool appendedShape = _appendMode && shapeIndex == _shapeData.size() - 1;

	// editing shapes?
	if (_isEditingShapes || appendedShape)
	{
		if (_shapeData[shapeIndex]->shape) {
			_shapeData[shapeIndex]->shape->AddRef();
		}

		*pVal = _shapeData[shapeIndex]->shape;
		return S_OK;
	}

	CSingleLock lock(&_readLock, TRUE);

	*pVal = _fastMode ? ReadFastModeShape(shapeIndex) : ReadComShape(shapeIndex);

	return S_OK;
}

// ************************************************************
//		ReadFastModeShape()
// ************************************************************
IShape* CShapefile::ReadFastModeShape(long shapeIndex)
{	fseek(_shpfile, _shpOffsets[shapeIndex], SEEK_SET);

	// read the shp from disk
	int index = ShapeUtility::ReadIntBigEndian(_shpfile);

	if (index != shapeIndex + 1)
	{
		ErrorMessage(tkINVALID_SHP_FILE);
		return NULL;
	}

	int contentLength = ShapeUtility::ReadIntBigEndian(_shpfile) * 2;

	// *2: for conversion from 16-bit words to 8-bit words
	// -2: skip first 2 int - it's record number and content length;
	int length = contentLength; //- 2 * sizeof(int);

	char* data = new char[length];
	int count = (int)fread(data, sizeof(char), length, _shpfile);

	if (count != length)
	{
		delete[] data;
		return NULL;
	}

	IShape* shape = NULL;

	if (data != NULL)
	{
		ComHelper::CreateShape(&shape);
		shape->put_GlobalCallback(_globalCallback);
		((CShape*)shape)->put_RawData(data, contentLength);
		delete[] data;
	}

	return shape;
}

// ************************************************************
//		ReadComShape()
// ************************************************************
IShape* CShapefile::ReadComShape(long shapeIndex)
{	// read the shp from disk
	fseek(_shpfile, _shpOffsets[shapeIndex], SEEK_SET);

	int intbuf;
	fread(&intbuf, sizeof(int), 1, _shpfile);
	ShapeUtility::SwapEndian((char*)&intbuf, sizeof(int));

	// shape records are 1 based - Allow for a mistake
	if (intbuf != shapeIndex + 1 && intbuf != shapeIndex)
	{
		ErrorMessage(tkINVALID_SHP_FILE);
		return NULL;
	}
	
	fread(&intbuf, sizeof(int), 1, _shpfile);
	ShapeUtility::SwapEndian((char*)&intbuf, sizeof(int));
	int contentLength = intbuf * 2;//(32 bit words)

	fread(&intbuf, sizeof(int), 1, _shpfile);

	ShpfileType shpType = (ShpfileType)intbuf;

	IShape * shape = NULL;
	IPoint * pnt = NULL;
	VARIANT_BOOL vbretval;

#pragma region Nullshape Or Mismatch
	// ------------------------------------------------------
	//	  Shape specific record contents
	// ------------------------------------------------------
	
	// MWGIS-91
	bool areEqualTypes = shpType == _shpfiletype;
	if (!areEqualTypes){
		areEqualTypes = ShapeUtility::Convert2D(shpType) == ShapeUtility::Convert2D(_shpfiletype);
	}
	
	if (_shpfiletype == SHP_NULLSHAPE)
	{
		if (shpType != SHP_NULLSHAPE)
		{
			ErrorMessage(tkINVALID_SHP_FILE);
			return NULL;
		}
		else
		{
			shape->Create(shpType, &vbretval);
			shape->put_GlobalCallback(_globalCallback);
			return shape;
		}
	}
	else if (shpType != SHP_NULLSHAPE && !areEqualTypes)
	{
		ErrorMessage(tkINVALID_SHP_FILE);
		return NULL;
	}
#pragma endregion

#pragma region Point
	// ------------------------------------------------------
	//	  SHP_POINT
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_POINT)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);

		// if (shpType == SHP_POINT)
		if (areEqualTypes)
		{
			ComHelper::CreatePoint(&pnt);
			pnt->put_GlobalCallback(_globalCallback);

			double x, y;
			fread(&x, sizeof(double), 1, _shpfile);
			fread(&y, sizeof(double), 1, _shpfile);
			pnt->put_X(x);
			pnt->put_Y(y);

			long pointIndex = 0;
			shape->InsertPoint(pnt, &pointIndex, &vbretval);
			if (vbretval == VARIANT_FALSE)
			{
				shape->Release();
				return NULL;
			}
			pnt->Release();
		}
	}

	// ------------------------------------------------------
	//	  SHP_POINTZ
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_POINTZ)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);

		// if (shpType == SHP_POINTZ)
		if (areEqualTypes)
		{
			ComHelper::CreatePoint(&pnt);
			pnt->put_GlobalCallback(_globalCallback);

			double x, y, z, m;
			fread(&x, sizeof(double), 1, _shpfile);
			fread(&y, sizeof(double), 1, _shpfile);
			fread(&z, sizeof(double), 1, _shpfile);
			fread(&m, sizeof(double), 1, _shpfile);
			pnt->put_X(x);
			pnt->put_Y(y);
			pnt->put_Z(z);
			pnt->put_M(m);

			long pointIndex = 0;
			shape->InsertPoint(pnt, &pointIndex, &vbretval);
			if (vbretval == VARIANT_FALSE)
			{
				shape->Release();
				return NULL;
			}
			pnt->Release();
		}
	}

	// ------------------------------------------------------
	//	  SHP_POINTM
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_POINTM)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);

		// if (shpType == SHP_POINTM)
		if (areEqualTypes)
		{
			ComHelper::CreatePoint(&pnt);
			pnt->put_GlobalCallback(_globalCallback);

			double x, y, m;
			fread(&x, sizeof(double), 1, _shpfile);
			fread(&y, sizeof(double), 1, _shpfile);
			fread(&m, sizeof(double), 1, _shpfile);
			pnt->put_X(x);
			pnt->put_Y(y);
			pnt->put_M(m);

			long pointIndex = 0;
			shape->InsertPoint(pnt, &pointIndex, &vbretval);
			if (vbretval == VARIANT_FALSE)
			{
				shape->Release();
				return NULL;
			}
			pnt->Release();
		}
	}
#pragma endregion
#pragma region Polyline
	// ------------------------------------------------------
	//	  SHP_POLYLINE
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_POLYLINE)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);

		// if (shpType == SHP_POLYLINE)
		if (areEqualTypes)
		{
			VARIANT_BOOL retval;
			double bx, by;
			int numParts;
			int numPoints;
			int part;
			double x, y;

			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);
			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);

			fread(&numParts, sizeof(int), 1, _shpfile);
			fread(&numPoints, sizeof(int), 1, _shpfile);

			long partIndex = 0;
			for (int i = 0; i < numParts; i++)
			{
				fread(&part, sizeof(int), 1, _shpfile);
				partIndex = i;
				shape->InsertPart(part, &partIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
			}

			long pointIndex = 0;
			for (int j = 0; j < numPoints; j++)
			{
				ComHelper::CreatePoint(&pnt);
				pnt->put_GlobalCallback(_globalCallback);
				fread(&x, sizeof(double), 1, _shpfile);
				fread(&y, sizeof(double), 1, _shpfile);
				pnt->put_X(x);
				pnt->put_Y(y);
				pointIndex = j;
				shape->InsertPoint(pnt, &pointIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
				pnt->Release();
			}
		}
	}

	// ------------------------------------------------------
	//	  SHP_POLYLINEZ
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_POLYLINEZ)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);
		// if (shpType == SHP_POLYLINEZ)
		if (areEqualTypes)
		{
			VARIANT_BOOL retval;
			double bx, by, bz, bm;
			int numParts;
			int numPoints;
			int part;
			double x, y, z, m;

			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);
			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);

			fread(&numParts, sizeof(int), 1, _shpfile);
			fread(&numPoints, sizeof(int), 1, _shpfile);

			long partIndex = 0;
			for (int i = 0; i < numParts; i++)
			{
				fread(&part, sizeof(int), 1, _shpfile);
				partIndex = i;
				shape->InsertPart(part, &partIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
			}

			long pointIndex = 0;
			//Read the x, y part of the point
			for (int j = 0; j < numPoints; j++)
			{
				ComHelper::CreatePoint(&pnt);
				pnt->put_GlobalCallback(_globalCallback);
				fread(&x, sizeof(double), 1, _shpfile);
				fread(&y, sizeof(double), 1, _shpfile);
				pnt->put_X(x);
				pnt->put_Y(y);
				pointIndex = j;
				shape->InsertPoint(pnt, &pointIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
				pnt->Release();
			}

			fread(&bz, sizeof(double), 1, _shpfile);
			fread(&bz, sizeof(double), 1, _shpfile);

			for (int k = 0; k < numPoints; k++)
			{
				fread(&z, sizeof(double), 1, _shpfile);
				pointIndex = k;
				IPoint * pnt = NULL;
				shape->get_Point(pointIndex, &pnt);
				if (pnt == NULL)
				{
					shape->Release();
					return NULL;
				}
				pnt->put_Z(z);
				pnt->Release();
			}

			fread(&bm, sizeof(double), 1, _shpfile);
			fread(&bm, sizeof(double), 1, _shpfile);

			for (int mc = 0; mc < numPoints; mc++)
			{
				fread(&m, sizeof(double), 1, _shpfile);
				pointIndex = mc;
				IPoint * pnt = NULL;
				shape->get_Point(pointIndex, &pnt);
				if (pnt == NULL)
				{
					shape->Release();
					return NULL;
				}
				pnt->put_M(m);
				//Rob Cairns 20-Dec-05
				pnt->Release();
			}
		}
	}

	// ------------------------------------------------------
	//	  SHP_POLYLINEM
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_POLYLINEM)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);

		// if (shpType == SHP_POLYLINEM)
		if (areEqualTypes)
		{
			VARIANT_BOOL retval;
			double bx, by, bm;
			int numParts;
			int numPoints;
			int part;
			double x, y, m;

			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);
			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);

			fread(&numParts, sizeof(int), 1, _shpfile);
			fread(&numPoints, sizeof(int), 1, _shpfile);

			long partIndex = 0;
			for (int i = 0; i < numParts; i++)
			{
				fread(&part, sizeof(int), 1, _shpfile);
				partIndex = i;
				shape->InsertPart(part, &partIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
			}

			long pointIndex = 0;
			//Read the x, y part of the point
			for (int j = 0; j < numPoints; j++)
			{
				ComHelper::CreatePoint(&pnt);
				pnt->put_GlobalCallback(_globalCallback);
				fread(&x, sizeof(double), 1, _shpfile);
				fread(&y, sizeof(double), 1, _shpfile);
				pnt->put_X(x);
				pnt->put_Y(y);
				pointIndex = j;
				shape->InsertPoint(pnt, &pointIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
				pnt->Release();
			}

			fread(&bm, sizeof(double), 1, _shpfile);
			fread(&bm, sizeof(double), 1, _shpfile);

			for (int mc = 0; mc < numPoints; mc++)
			{
				fread(&m, sizeof(double), 1, _shpfile);
				pointIndex = mc;
				IPoint * pnt = NULL;
				shape->get_Point(pointIndex, &pnt);
				if (pnt == NULL)
				{
					shape->Release();
					return NULL;
				}
				pnt->put_M(m);
				pnt->Release();

			}
		}
	}
#pragma endregion
#pragma region Polygon
	// ------------------------------------------------------
	//	  SHP_POLYGON
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_POLYGON)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);

		// if (shpType == SHP_POLYGON)
		if (areEqualTypes)
		{
			VARIANT_BOOL retval;
			double bx, by;
			int numParts;
			int numPoints;
			int part;
			double x, y;

			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);
			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);

			fread(&numParts, sizeof(int), 1, _shpfile);
			fread(&numPoints, sizeof(int), 1, _shpfile);

			long partIndex = 0;
			for (int i = 0; i < numParts; i++)
			{
				fread(&part, sizeof(int), 1, _shpfile);
				partIndex = i;
				shape->InsertPart(part, &partIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
			}

			long pointIndex = 0;
			for (int j = 0; j < numPoints; j++)
			{
				ComHelper::CreatePoint(&pnt);
				pnt->put_GlobalCallback(_globalCallback);
				fread(&x, sizeof(double), 1, _shpfile);
				fread(&y, sizeof(double), 1, _shpfile);
				pnt->put_X(x);
				pnt->put_Y(y);
				pointIndex = j;
				shape->InsertPoint(pnt, &pointIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
				pnt->Release();
			}
		}
	}

	// ------------------------------------------------------
	//	  SHP_POLYGONZ
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_POLYGONZ)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);
		// if (shpType == SHP_POLYGONZ)
		if (areEqualTypes)
		{
			VARIANT_BOOL retval;
			double bx, by, bz, bm;
			int numParts;
			int numPoints;
			int part;
			double x, y, z, m;

			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);
			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);

			fread(&numParts, sizeof(int), 1, _shpfile);
			fread(&numPoints, sizeof(int), 1, _shpfile);

			long partIndex = 0;
			for (int i = 0; i < numParts; i++)
			{
				fread(&part, sizeof(int), 1, _shpfile);
				partIndex = i;
				shape->InsertPart(part, &partIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
			}

			//Read the x, y part of the point
			long pointIndex = 0;
			for (int j = 0; j < numPoints; j++)
			{
				ComHelper::CreatePoint(&pnt);
				pnt->put_GlobalCallback(_globalCallback);
				fread(&x, sizeof(double), 1, _shpfile);
				fread(&y, sizeof(double), 1, _shpfile);
				pnt->put_X(x);
				pnt->put_Y(y);
				pointIndex = j;
				shape->InsertPoint(pnt, &pointIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
				pnt->Release();
			}

			fread(&bz, sizeof(double), 1, _shpfile);
			fread(&bz, sizeof(double), 1, _shpfile);

			for (int k = 0; k < numPoints; k++)
			{
				fread(&z, sizeof(double), 1, _shpfile);
				pointIndex = k;
				IPoint * pnt = NULL;
				shape->get_Point(pointIndex, &pnt);
				if (pnt == NULL)
				{
					shape->Release();
					return NULL;
				}
				pnt->put_Z(z);
				pnt->Release();
			}

			fread(&bm, sizeof(double), 1, _shpfile);
			fread(&bm, sizeof(double), 1, _shpfile);

			for (int mc = 0; mc < numPoints; mc++)
			{
				fread(&m, sizeof(double), 1, _shpfile);
				pointIndex = mc;
				IPoint * pnt = NULL;
				shape->get_Point(pointIndex, &pnt);
				if (pnt == NULL)
				{
					shape->Release();
					return NULL;
				}
				pnt->put_M(m);
				pnt->Release();
			}
		}
	}

	// ------------------------------------------------------
	//	  SHP_POLYGONM
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_POLYGONM)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);
		// if (shpType == SHP_POLYGONM)
		if (areEqualTypes)
		{
			VARIANT_BOOL retval;
			double bx, by, bm;
			int numParts;
			int numPoints;
			int part;
			double x, y, m;

			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);
			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);

			fread(&numParts, sizeof(int), 1, _shpfile);
			fread(&numPoints, sizeof(int), 1, _shpfile);

			long partIndex = 0;
			for (int i = 0; i < numParts; i++)
			{
				fread(&part, sizeof(int), 1, _shpfile);
				partIndex = i;
				shape->InsertPart(part, &partIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
			}

			//Read the x, y part of the point
			long pointIndex = 0;
			for (int j = 0; j < numPoints; j++)
			{
				ComHelper::CreatePoint(&pnt);
				pnt->put_GlobalCallback(_globalCallback);
				fread(&x, sizeof(double), 1, _shpfile);
				fread(&y, sizeof(double), 1, _shpfile);
				pnt->put_X(x);
				pnt->put_Y(y);
				pointIndex = j;
				shape->InsertPoint(pnt, &pointIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
				pnt->Release();
			}

			// M range
			fread(&bm, sizeof(double), 1, _shpfile);
			fread(&bm, sizeof(double), 1, _shpfile);

			for (int mc = 0; mc < numPoints; mc++)
			{
				fread(&m, sizeof(double), 1, _shpfile);
				pointIndex = mc;
				IPoint * pnt = NULL;
				shape->get_Point(pointIndex, &pnt);
				if (pnt == NULL)
				{
					shape->Release();
					return NULL;
				}
				pnt->put_M(m);
				pnt->Release();
			}
		}
	}
#pragma endregion
#pragma region Multipoint
	// ------------------------------------------------------
	//	  SHP_MULTIPOINT
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_MULTIPOINT)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);

		// if (shpType == SHP_MULTIPOINT)
		if (areEqualTypes)
		{
			VARIANT_BOOL retval;
			double bx, by;
			int numPoints;
			double x, y;

			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);
			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);

			fread(&numPoints, sizeof(int), 1, _shpfile);

			long pointIndex = 0;
			for (int j = 0; j < numPoints; j++)
			{
				ComHelper::CreatePoint(&pnt);
				pnt->put_GlobalCallback(_globalCallback);
				fread(&x, sizeof(double), 1, _shpfile);
				fread(&y, sizeof(double), 1, _shpfile);
				pnt->put_X(x);
				pnt->put_Y(y);
				pointIndex = j;
				shape->InsertPoint(pnt, &pointIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
				pnt->Release();
			}
		}
	}

	// ------------------------------------------------------
	//	  SHP_MULTIPOINTZ
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_MULTIPOINTZ)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);

		// if (shpType == SHP_MULTIPOINTZ)
		if (areEqualTypes)
		{
			VARIANT_BOOL retval;
			double bx, by, bz, bm;
			int numPoints;
			double x, y, z, m;

			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);
			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);

			fread(&numPoints, sizeof(int), 1, _shpfile);

			long pointIndex = 0;
			for (int j = 0; j < numPoints; j++)
			{
				ComHelper::CreatePoint(&pnt);
				pnt->put_GlobalCallback(_globalCallback);
				fread(&x, sizeof(double), 1, _shpfile);
				fread(&y, sizeof(double), 1, _shpfile);
				pnt->put_X(x);
				pnt->put_Y(y);
				pointIndex = j;
				shape->InsertPoint(pnt, &pointIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
				pnt->Release();
			}

			fread(&bz, sizeof(double), 1, _shpfile);
			fread(&bz, sizeof(double), 1, _shpfile);

			for (int k = 0; k < numPoints; k++)
			{
				fread(&z, sizeof(double), 1, _shpfile);
				pointIndex = k;
				IPoint * pnt = NULL;
				shape->get_Point(pointIndex, &pnt);
				if (pnt == NULL)
				{
					shape->Release();
					return NULL;
				}
				pnt->put_Z(z);
				pnt->Release();
			}

			fread(&bm, sizeof(double), 1, _shpfile);
			fread(&bm, sizeof(double), 1, _shpfile);

			for (int mc = 0; mc < numPoints; mc++)
			{
				fread(&m, sizeof(double), 1, _shpfile);
				pointIndex = mc;
				IPoint * pnt = NULL;
				shape->get_Point(pointIndex, &pnt);
				if (pnt == NULL)
				{
					shape->Release();
					return NULL;
				}
				pnt->put_M(m);
				pnt->Release();
			}
		}
	}

	// ------------------------------------------------------
	//	  SHP_MULTIPOINTM
	// ------------------------------------------------------
	else if (_shpfiletype == SHP_MULTIPOINTM)
	{
		ComHelper::CreateShape(&shape);
		shape->Create(shpType, &vbretval);
		shape->put_GlobalCallback(_globalCallback);

		// if (shpType == SHP_MULTIPOINTM)
		if (areEqualTypes)
		{
			VARIANT_BOOL retval;
			double bx, by, bm;
			int numPoints;
			double x, y, m;

			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);
			fread(&bx, sizeof(double), 1, _shpfile);
			fread(&by, sizeof(double), 1, _shpfile);

			fread(&numPoints, sizeof(int), 1, _shpfile);

			long pointIndex = 0;
			for (int j = 0; j < numPoints; j++)
			{
				ComHelper::CreatePoint(&pnt);
				pnt->put_GlobalCallback(_globalCallback);
				fread(&x, sizeof(double), 1, _shpfile);
				fread(&y, sizeof(double), 1, _shpfile);
				pnt->put_X(x);
				pnt->put_Y(y);
				pointIndex = j;
				shape->InsertPoint(pnt, &pointIndex, &retval);
				if (retval == VARIANT_FALSE)
				{
					shape->Release();
					return NULL;
				}
				pnt->Release();
			}

			fread(&bm, sizeof(double), 1, _shpfile);
			fread(&bm, sizeof(double), 1, _shpfile);

			for (int mc = 0; mc < numPoints; mc++)
			{
				fread(&m, sizeof(double), 1, _shpfile);
				pointIndex = mc;
				IPoint * pnt = NULL;
				shape->get_Point(pointIndex, &pnt);
				if (pnt == NULL)
				{
					shape->Release();
					return NULL;
				}
				pnt->put_M(m);
				pnt->Release();
			}
		}
	}
#pragma endregion

	return shape;
}
#pragma endregion

#pragma region ShxReadingWriting

// **************************************************************
//		ReadShx()
// **************************************************************
BOOL CShapefile::ReadShx()
{	// guaranteed that .shx file is open
	rewind(_shxfile);
	_shpOffsets.clear();

	// file code
	int intbuf;
	fread(&intbuf,sizeof(int),1,_shxfile);
	ShapeUtility::SwapEndian((char*)&intbuf,sizeof(int));
	if( intbuf != FILE_CODE )
		return FALSE;

	// unused
	int unused = UNUSEDVAL;
	for(int i=0; i < UNUSEDSIZE; i++)
	{
		fread(&intbuf,sizeof(int),1,_shxfile);
		ShapeUtility::SwapEndian((char*)&intbuf,sizeof(int));
		if( intbuf != unused )
			return FALSE;
	}

	// file length (16 bit words)
	fread(&intbuf,sizeof(int),1,_shxfile);
	ShapeUtility::SwapEndian((char*)&intbuf,sizeof(int));
	int filelength = intbuf;

	// version
	fread(&intbuf,sizeof(int),1,_shxfile);
	if( intbuf != VERSION )
		return FALSE;

	// shapefile type
	fread(&intbuf,sizeof(int),1,_shxfile);
	_shpfiletype = (ShpfileType)intbuf;

	// bounds
	fread(&_minX,sizeof(double),1,_shxfile);
	fread(&_minY,sizeof(double),1,_shxfile);
	fread(&_maxX,sizeof(double),1,_shxfile);
	fread(&_maxY,sizeof(double),1,_shxfile);
	fread(&_minZ,sizeof(double),1,_shxfile);
	fread(&_maxZ,sizeof(double),1,_shxfile);
	fread(&_minM,sizeof(double),1,_shxfile);
	fread(&_maxM,sizeof(double),1,_shxfile);

	int readLength = HEADER_BYTES_16;
	while( readLength < filelength )
	{
		// offset
		fread(&intbuf,sizeof(int),1,_shxfile);
		ShapeUtility::SwapEndian((char*)&intbuf,sizeof(int));
		_shpOffsets.push_back(intbuf*2);			// convert to (32 bit words)

		// content length
		fread(&intbuf,sizeof(int),1,_shxfile);
		ShapeUtility::SwapEndian((char*)&intbuf,sizeof(int));
		readLength += 4;
	}
	
	rewind(_shxfile);
	return TRUE;
}

// **************************************************************
//		AppendToShx()
// **************************************************************
bool CShapefile::AppendToShx(FILE* shx, IShape* shp, int offset)
{	if (!shx || !shp) return false;

	_shpOffsets.push_back(offset);

	int success = fseek(shx, 0, SEEK_END);

	ShapeUtility::WriteBigEndian(shx, offset / 2);

	int contentLength = ShapeHelper::GetContentLength(shp);
	ShapeUtility::WriteBigEndian(shx, contentLength / 2);

	fflush(shx);

	return true;
}

// **************************************************************
//		WriteShx()
// **************************************************************
BOOL CShapefile::WriteShx(FILE * shx, ICallback * cBack)
{	ICallback* callback = cBack ? cBack : _globalCallback;

	// guaranteed that .shx file is open
	rewind(shx);

	// FILE CODE
	ShapeUtility::WriteBigEndian(shx, FILE_CODE);

	// unused
	for(int j = 0; j < UNUSEDSIZE; j++)
	{
		ShapeUtility::WriteBigEndian(shx, UNUSEDVAL);
	}

	// FILELENGTH (16 bit words)
	int fileLength = HEADER_BYTES_16 + (int)_shapeData.size() * 4;
	ShapeUtility::WriteBigEndian(shx, fileLength);

	//VERSION
	int version = VERSION;
	fwrite(&version, sizeof(int),1,shx);

	//SHAPEFILE TYPE
	int tmpshapefiletype = (short)_shpfiletype;
	fwrite(&tmpshapefiletype, sizeof(int),1,shx);

	//BOUNDS
	WriteBounds(shx);

	int offset = HEADER_BYTES_32;
	long numPoints = 0;
	long numParts = 0;
	ShpfileType shptype;
	IShape * shape = NULL;

	long percent = 0, newpercent = 0;

	_shpOffsets.clear();
	int size  = (int)_shapeData.size();
	for( int i = 0; i < size; i++)
	{
		// convert to (32 bit words)
		_shpOffsets.push_back(offset);

		ShapeUtility::WriteBigEndian(shx, offset / 2);

		get_Shape(i,&shape);
		shape->get_NumPoints(&numPoints);
		shape->get_NumParts(&numParts);
		shape->get_ShapeType(&shptype);

		int contentLength = ShapeUtility::get_ContentLength(shptype, numPoints, numParts);

		offset = offset + RECORD_HEADER_LENGTH_32 + contentLength;

		ShapeUtility::WriteBigEndian(shx, contentLength / 2);

		shape->Release();

		CallbackHelper::Progress(callback, i, size, "Writing .shx file", _key, percent);
	}

	CallbackHelper::ProgressCompleted(callback, _key);

	fflush(shx);

	return TRUE;
}
#pragma endregion

#pragma region ShpReadingWriting

// **************************************************************
//		GetWriteFileLength()
// **************************************************************
int CShapefile::GetWriteFileLength()
{	IShape * sh = NULL;
	long numPoints = 0;
	long numParts = 0;
	long part = 0;
	ShpfileType shptype;

	int filelength = HEADER_BYTES_32;

	int size = (int)_shapeData.size();
	for (int i = 0; i < size; i++)
	{
		this->get_Shape(i, &sh);
		sh->get_NumPoints(&numPoints);
		sh->get_NumParts(&numParts);
		sh->get_ShapeType(&shptype);

		int contentLength = ShapeUtility::get_ContentLength(shptype, numPoints, numParts);
		filelength = filelength + RECORD_HEADER_LENGTH_32 + contentLength;

		sh->Release();
	}
	return filelength;
}

// **************************************************************
//		AppendToShpFile()
// **************************************************************
bool CShapefile::AppendToShpFile(FILE* shp, IShapeWrapper* wrapper)
{	if (!shp || !wrapper) return false;

	int length = wrapper->get_ContentLength();

	// write record header
	ShapeUtility::WriteBigEndian(shp, _shapeData.size());
	ShapeUtility::WriteBigEndian(shp, length / 2);

	// write content		
	wrapper->RefreshBounds();
	int* data = wrapper->get_RawData();
	if (data) 
	{
		fseek(shp, 0, SEEK_END);
		size_t size = fwrite(data, sizeof(char), length, shp);
		fflush(shp);
		
		delete[] data;

		return true;
	}

	return false;
}

// **************************************************************
//		WriteShp()
// **************************************************************
void CShapefile::WriteBounds(FILE * shp)
{	double ShapefileBounds[8];
	ShapefileBounds[0] = _minX;
	ShapefileBounds[1] = _minY;
	ShapefileBounds[2] = _maxX;
	ShapefileBounds[3] = _maxY;
	ShapefileBounds[4] = _minZ;
	ShapefileBounds[5] = _maxZ;
	ShapefileBounds[6] = _minM;
	ShapefileBounds[7] = _maxM;
	fwrite(ShapefileBounds, sizeof(double), 8, shp);
}

// **************************************************************
//		WriteShp()
// **************************************************************
BOOL CShapefile::WriteShp(FILE * shp, ICallback * cBack)
{	// guaranteed that .shp file is open
	rewind(shp);

	//FILE_CODE
	ShapeUtility::WriteBigEndian(shp, FILE_CODE);

	//UNUSED
	for(int j = 0; j < UNUSEDSIZE; j++)
	{
		ShapeUtility::WriteBigEndian(shp, UNUSEDVAL);
	}

	//FILELENGTH (32 bit words)
	int fileLength = GetWriteFileLength();
	ShapeUtility::WriteBigEndian(shp, fileLength / 2);

	//VERSION
	int version = VERSION;
	fwrite(&version, sizeof(int),1,shp);

	//SHAPEFILE TYPE
	int tmpshapefiletype = (short)_shpfiletype;
	fwrite(&tmpshapefiletype, sizeof(int),1,shp);

	//BOUNDS
	WriteBounds(shp);

	long percent = 0, newpercent = 0;

	ICallback* callback = _globalCallback ? _globalCallback : cBack;

	long numPoints = 0;
	long numParts = 0;
	long part = 0;
	IShape * sh = NULL;

	int size = _shapeData.size();

	for( int k = 0; k < size; k++)
	{
		get_Shape(k,&sh);
		CShape* shape = (CShape*)sh;
		
		IShapeWrapper* wrapper = shape->get_ShapeWrapper();
		int length = wrapper->get_ContentLength();
		
		//Write the Record Header
		ShapeUtility::WriteBigEndian(shp, k + 1);
		ShapeUtility::WriteBigEndian(shp, length / 2);

		if ( _fastMode && _isEditingShapes)
		{
			wrapper->RefreshBounds();
			int* data = wrapper->get_RawData();
			fwrite(data, sizeof(char), length, shp);
			delete[] data;
		}
		else
		{
			// disk based mode
			// disk-based + fast data (probably a bit faster procedure can be devised)
			// in-memory mode (COM points)
			ShpfileType shptype = wrapper->get_ShapeType();
			ShpfileType shptype2D = wrapper->get_ShapeType2D();
			numPoints = wrapper->get_PointCount();
			numParts = wrapper->get_PartCount();

			//Write the Record
			if( shptype2D == SHP_NULLSHAPE )
			{
				int ishptype = shptype;
				fwrite(&shptype, sizeof(int), 1, shp);
			}
			else if( shptype2D == SHP_POINT )
			{
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				switch (shptype)
				{
					case SHP_POINT:
						ShapeUtility::WritePointXY(wrapper, 0, shp);
						break;
					case SHP_POINTZ:
						ShapeUtility::WritePointXYZ(wrapper, 0, shp);
						break;
					case SHP_POINTM:
						ShapeUtility::WritePointXYM(wrapper, 0, shp);
						break;
				}
			}
			else if( shptype2D == SHP_POLYLINE || shptype2D == SHP_POLYGON || shptype2D == SHP_MULTIPOINT )
			{
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);

				ShapeUtility::WriteExtentsXY(wrapper, shp);

				if (shptype2D != SHP_MULTIPOINT)		// parts should be written for both polylines and polygons
					fwrite(&numParts,sizeof(int),1,shp);

				fwrite(&numPoints,sizeof(int),1,shp);

				if (shptype2D != SHP_MULTIPOINT)		// parts should be written for both polylines and polygons
				{
					for( int p = 0; p < numParts; p++ )
					{
						shape->get_Part(p,&part);
						fwrite(&part,sizeof(int),1,shp);
					}
				}

				// xy
				for (int i = 0; i < numPoints; i++) {
					ShapeUtility::WritePointXY(wrapper, i, shp);
				}

				// z
				if (shptype == SHP_POLYLINEZ || shptype == SHP_POLYGONZ || shptype == SHP_MULTIPOINTZ)
				{
					ShapeUtility::WriteExtentsZ(wrapper, shp);

					for (int i = 0; i < numPoints; i++) {
						ShapeUtility::WritePointZ(wrapper, i, shp);
					}
				}

				// m
				if (shptype != SHP_POLYLINE && shptype != SHP_POLYGON && shptype != SHP_MULTIPOINT)	// writing for both M and Z
				{
					ShapeUtility::WriteExtentsM(wrapper, shp);

					for (int i = 0; i < numPoints; i++) {
						ShapeUtility::WritePointM(wrapper, i, shp);
					}
				}
			}
		}

		shape->Release();

		CallbackHelper::Progress(callback, k, size, "Writing .shp file", _key, percent);
	}

	CallbackHelper::ProgressCompleted(callback, _key);

	fflush(shp);
	return TRUE;
}
#pragma endregion
