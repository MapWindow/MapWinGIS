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

#include <StdAfx.h>
#include <afxmt.h>				// for CMutex
#include <gsl/span_ext>

#include "ExtentsHelper.h"
#include "GeosConverter.h"
#include "GeosHelper.h"
#include "Shapefile.h"
#include "ShapeHelper.h"
#include "Templates.h"

#pragma region SelectShapes
// ******************************************************************
//		SelectShapes()
// ******************************************************************
CMutex selectShapesMutex(FALSE);
STDMETHODIMP CShapefile::SelectShapes(IExtents* boundBox, const double tolerance, const SelectMode selectMode, VARIANT* result, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = VARIANT_FALSE;

	selectShapesMutex.Lock();

	double bMinX, bMaxX, bMinY, bMaxY, bMinZ, bMaxZ;
	boundBox->GetBounds(&bMinX, &bMinY, &bMinZ, &bMaxX, &bMaxY, &bMaxZ);

	// the core routine
	std::vector<long> selectResult;
	Extent box(bMinX, bMaxX, bMinY, bMaxY);
	this->SelectShapesCore(box, tolerance, selectMode, selectResult, false);

	*retval = Templates::Vector2SafeArray(&selectResult, VT_I4, result) ? VARIANT_TRUE : VARIANT_FALSE;

	selectShapesMutex.Unlock();
	return S_OK;
}

// ****************************************************************
//		SelectShapesCore()
// ****************************************************************
bool CShapefile::SelectShapesCore(Extent& extents, const double tolerance, const SelectMode selectMode, std::vector<long>& selectResult, bool renderedOnly)
{
	double bMinX = extents.left;
	double bMaxX = extents.right;
	double bMinY = extents.bottom;
	double bMaxY = extents.top;

	double sMinX, sMaxX, sMinY, sMaxY;

	if (tolerance > 0.0)
	{
		const double halfTolerance = tolerance * .5;
		bMinX -= halfTolerance;
		bMinY -= halfTolerance;
		bMaxX += halfTolerance;
		bMaxY += halfTolerance;
	}

	// build GEOSGeom for comparison
	IShape* shpExt = nullptr;
	ComHelper::CreateShape(&shpExt);

	const bool bPtSelection = bMinX == bMaxX && bMinY == bMaxY;
	int localNumShapes = static_cast<int>(_shapeData.size());

	IndexSearching::CIndexSearching* res = nullptr;
	vector<int> qtreeResult;
	bool useSpatialIndexResults = false;
	bool useQTreeResults = false;

	VARIANT_BOOL useSpatialIndex;
	CComPtr<IExtents> box = nullptr;
	box.Attach(ExtentsHelper::Populate(extents));
	this->get_CanUseSpatialIndex(box, &useSpatialIndex);

	if (useSpatialIndex)
	{
		double lowVals[2]{};
		double highVals[2]{};

		lowVals[0] = bMinX;
		lowVals[1] = bMinY;
		highVals[0] = bMaxX;
		highVals[1] = bMaxY;

		const IndexSearching::QueryTypeFlags qType = (selectMode == INTERSECTION)
			? IndexSearching::QueryTypeFlags::intersection
			: IndexSearching::QueryTypeFlags::contained;
		res = new IndexSearching::CIndexSearching();  // TODO: Fix compile warning

		const int ret = SelectShapesFromIndex(_spatialIndexID, lowVals, highVals, qType, res); // TODO: Fix compile warning
		if (ret != 0)
		{
			_hasSpatialIndex = false;
			_spatialIndexLoaded = false;
			IndexSearching::UnloadSpatialIndex(_spatialIndexID);
			delete res;		//TODO throw error
		}
		else
		{
			localNumShapes = res->GetLength();
			useSpatialIndexResults = true;
		}
	}
	else if (_isEditingShapes && _useQTree)
	{
		if (!_qtree)
			GenerateQTree();

		if (bPtSelection)
		{
			qtreeResult = _qtree->GetNodes(QTreeExtent(bMinX, bMinX + 1, bMinY + 1, bMinY));
		}
		else
		{
			qtreeResult = _qtree->GetNodes(QTreeExtent(bMinX, bMaxX, bMaxY, bMinY));
		}

		localNumShapes = qtreeResult.size();
		useQTreeResults = true;
	}

	int shapeVal;
	int i;

	const ShpfileType shpType2D = ShapeUtility::Convert2D(_shpfiletype);

	// --------------------------------------------------
	//	Point Selection
	// --------------------------------------------------
	if (bMinX == bMaxX && bMinY == bMaxY)  // TODO: Fix compile warning
	{
		VARIANT_BOOL ret;
		shpExt->Create(ShpfileType::SHP_POINT, &ret);
		long idx;
		shpExt->AddPoint(bMinX, bMinY, &idx);
		// convert input point to GEOS
		GEOSGeom geosPoint = GeosConverter::ShapeToGeom(shpExt); // TODO: Fix compile warning

		if (shpType2D == SHP_POLYGON)
		{
			for (i = 0; i < localNumShapes; i++)
			{
				if (useSpatialIndexResults)
				{
					shapeVal = (res->GetValue(i)) - 1;
				}
				else if (useQTreeResults)
				{
					//shapeVal = qtreeResult[i];
					shapeVal = gsl::at(qtreeResult, i);
				}
				else
				{
					shapeVal = i;
				}

				// get current shape
				CComPtr<IShape> shape = nullptr;
				get_Shape(shapeVal, &shape);
				// convert querying shape to GEOS
				GEOSGeom geosShape = GeosConverter::ShapeToGeom(shape); // TODO: Fix compile warning
				if (geosShape == nullptr)
					continue;

				// check for containment
				if (GeosHelper::Contains(geosShape, geosPoint))
				{
					selectResult.push_back(shapeVal);
					GeosHelper::DestroyGeometry(geosShape);
					continue;
				}
				GeosHelper::DestroyGeometry(geosShape);
			}
		}

		GeosHelper::DestroyGeometry(geosPoint);
	}
	//	Rectangle selection
	else
	{
		std::vector<long> parts;
		std::vector<double> yPts;
		std::vector<double> xPts;
		ShpfileType shapeType;
		VARIANT_BOOL ret;
		// set up polygon extent
		shpExt->Create(ShpfileType::SHP_POLYGON, &ret);
		long idx;
		shpExt->AddPoint(bMinX, bMinY, &idx);
		shpExt->AddPoint(bMinX, bMaxY, &idx);
		shpExt->AddPoint(bMaxX, bMaxY, &idx);
		shpExt->AddPoint(bMaxX, bMinY, &idx);
		shpExt->AddPoint(bMinX, bMinY, &idx);
		// convert extent to GEOS
		GEOSGeom geosExtent = GeosConverter::ShapeToGeom(shpExt); // TODO: Fix compile warning

		for (i = 0; i < localNumShapes; i++)
		{
			if (useSpatialIndexResults)
			{
				shapeVal = (res->GetValue(i)) - 1;
			}
			else if (useQTreeResults)
			{
				//shapeVal = qtreeResult[i];
				shapeVal = gsl::at(qtreeResult, i);
			}
			else
			{
				shapeVal = i;
			}

			//if (shapeVal < 0 || shapeVal >= static_cast<int>(_shapeData.size())) continue;
			if (shapeVal < 0 || shapeVal >= gsl::narrow_cast<int>(_shapeData.size())) continue;

			// ***********************************************************************************
			// jfaust: This test is not valid here; the wasRendered flag is initially set for each
			// drawn feature, but is then immediately cleared when drawing the volatile layers; so
			// this test appears to always fail, failing to select Point-type features.
			// TODO: Re-evaluate need for wasRendered test; it is not done for Polygon features...
			//if (renderedOnly && !_shapeData[shapeVal]->wasRendered())
			//	continue;
			// NOTE: may be resolved now as a result of MWGIS-137, but should still evaluate
			// ***********************************************************************************

			// bounds
			if (this->QuickExtentsCore(shapeVal, &sMinX, &sMinY, &sMaxX, &sMaxY))
			{
				// check bounds (maybe they don't touch at all)
				if (sMaxX < bMinX ||
					sMinX > bMaxX ||
					sMaxY < bMinY ||
					sMinY > bMaxY)
				{
					continue;
				}

				// inclusion (works for every shape type)
				if (sMinX >= bMinX && sMinX <= bMaxX &&
					sMaxX >= bMinX && sMaxX <= bMaxX &&
					sMinY >= bMinY && sMinY <= bMaxY &&
					sMaxY >= bMinY && sMaxY <= bMaxY)
				{
					selectResult.push_back(shapeVal);
					continue;
				}
			}

			if (shpType2D == SHP_POLYLINE && selectMode == INTERSECTION)
			{
				// get current shape
				CComPtr<IShape> shape = nullptr;
				get_Shape(shapeVal, &shape);
				// convert shape to GEOS
				GEOSGeom geos = GeosConverter::ShapeToGeom(shape); // TODO: Fix compile warning
				// see if shape intersects polygon extent
				if (GeosHelper::Intersects(geosExtent, geos))
				{
					selectResult.push_back(shapeVal);
				}
				GeosHelper::DestroyGeometry(geos);

				//            if( DefineShapePoints( shapeVal, ShapeType, parts, xPts, yPts ) != FALSE )
							//{
							//	if (SelectionHelper::PolylineIntersection(xPts, yPts, parts, b_minX, b_maxX, b_minY, b_maxY, Tolerance))
							//		selectResult.push_back( shapeVal );
							//}		
			}
			else if (shpType2D == SHP_POLYGON && selectMode == INTERSECTION)
			{
				// get current shape
				CComPtr<IShape> shape = nullptr;
				get_Shape(shapeVal, &shape);
				// convert shape to GEOS
				GEOSGeom geos = GeosConverter::ShapeToGeom(shape); // TODO: Fix compile warning
				// see if shape intersects polygon extent
				if (GeosHelper::Intersects(geosExtent, geos))
				{
					selectResult.push_back(shapeVal);
				}
				GeosHelper::DestroyGeometry(geos);
			}
			else if (shpType2D == SHP_MULTIPOINT && selectMode == INTERSECTION)
			{
				if (this->DefineShapePoints(shapeVal, shapeType, parts, xPts, yPts) != FALSE)
				{
					const long numpoints = xPts.size();
					bool addShape = false;
					for (int j = 0; j < numpoints; j++)
					{
						//const double px = xPts[j];
						const double px = gsl::at(xPts, j);
						//const double py = yPts[j];
						const double py = gsl::at(yPts, j);

						if (px >= bMinX && px <= bMaxX)
						{
							if (py >= bMinY && py <= bMaxY)
							{
								addShape = true;
								break;
							}
						}
					}
					if (addShape)
					{
						selectResult.push_back(shapeVal);
						continue;
					}
				}
			}
		}
		GeosHelper::DestroyGeometry(geosExtent);
	}

	if (useSpatialIndexResults)
	{
		delete res;
	}
	else if (_useQTree && _isEditingShapes != FALSE)
	{
		qtreeResult.clear();
	}
	// return true if any selected...
	return !selectResult.empty();
}
#pragma endregion

#pragma region Selection
/***********************************************************************/
/*		ShapeSelected()
/***********************************************************************/
//  Returns and sets the selection state for a shape.
STDMETHODIMP CShapefile::get_ShapeSelected(const long shapeIndex, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (shapeIndex < 0 || shapeIndex >= gsl::narrow_cast<long>(_shapeData.size()))
	{
		*pVal = VARIANT_FALSE;
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else {
		*pVal = (_shapeData[shapeIndex]->selected() == true) ? VARIANT_TRUE : VARIANT_FALSE;
	}

	return S_OK;
}
STDMETHODIMP CShapefile::put_ShapeSelected(const long shapeIndex, const VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (shapeIndex < 0 || shapeIndex >= gsl::narrow_cast<long>(_shapeData.size()))
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else {
		_shapeData[shapeIndex]->selected(newVal == VARIANT_TRUE ? true : false);
	}
	return S_OK;
}


// *************************************************************
//		get_NumSelected
// *************************************************************
STDMETHODIMP CShapefile::get_NumSelected(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long count = 0;
	for (const auto& i : _shapeData)
	{
		if (i->selected())
			count++;
	}
	*pVal = count;
	return S_OK;
}

// *************************************************************
//     SelectAll()
// *************************************************************
STDMETHODIMP CShapefile::SelectAll()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	for (const auto& i : _shapeData)
	{
		i->selected(true);
	}

	return S_OK;
}

// *************************************************************
//     SelectNone()
// *************************************************************
STDMETHODIMP CShapefile::SelectNone()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	for (const auto& i : _shapeData)
	{
		i->selected(false);
	}

	return S_OK;
}

// *************************************************************
//     InvertSelection()
// *************************************************************
STDMETHODIMP CShapefile::InvertSelection()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	for (const auto& i : _shapeData)
	{
		i->selected(!i->selected());
	}

	return S_OK;
}
#pragma endregion

// **************************************************************
//		DefineShapePoints
// **************************************************************
// A fast method to retrieve bounds of the given shape
BOOL CShapefile::DefineShapePoints(const long shapeIndex, ShpfileType& shapeType, std::vector<long>& parts, std::vector<double>& xPts, std::vector<double>& yPts)
{
	parts.clear();
	xPts.clear();
	yPts.clear();

	if (_isEditingShapes != FALSE)
	{
		IShape* shape = _shapeData[shapeIndex]->shape;
		return ShapeHelper::get_MemShapePoints(shape, shapeType, parts, xPts, yPts);
	}

	// not editing
	CSingleLock lock(&_readLock, TRUE);

	//Get the Info from the disk
	fseek(_shpfile, _shpOffsets[shapeIndex], SEEK_SET);

	int intbuf;
	fread(&intbuf, sizeof(int), 1, _shpfile);
	ShapeUtility::SwapEndian((char*)&intbuf, sizeof(int)); // TODO: Fix compile warning

	// shape records are 1 based
	if (intbuf != shapeIndex + 1 && intbuf != shapeIndex)
	{
		ErrorMessage(tkINVALID_SHP_FILE);
		return FALSE;
	}

	long numParts;
	long numPoints;
	fread(&intbuf, sizeof(int), 1, _shpfile);
	ShapeUtility::SwapEndian((char*)&intbuf, sizeof(int)); // TODO: Fix compile warning
	const int contentLength = intbuf * 2;			//(16 to 32 bit words)

	if (contentLength <= 0)
		return FALSE;

	auto cdata = new char[contentLength];
	fread(cdata, sizeof(char), contentLength, _shpfile);
	const auto intdata = (int*)cdata; // TODO: Fix compile warning
	const auto shapetype = static_cast<ShpfileType>(intdata[0]);

	lock.Unlock();

	if (shapetype == SHP_NULLSHAPE)
	{
		shapeType = shapetype;
		return FALSE;
	}
	else if (shapetype == SHP_POINT || shapetype == SHP_POINTZ || shapetype == SHP_POINTM)
	{
		int* begOfPts = &intdata[1];
		const auto pntdata = (double*)begOfPts; // TODO: Fix compile warning
		xPts.push_back(pntdata[0]);
		yPts.push_back(pntdata[1]);
		shapeType = shapetype;
	}
	else if (shapetype == SHP_POLYLINE || shapetype == SHP_POLYLINEZ || shapetype == SHP_POLYLINEM)
	{
		numParts = intdata[9];
		numPoints = intdata[10];

		if (numPoints < 2)
			return FALSE;

		// fill up parts: polyline must have at least 1 part			
		if (numParts > 0)
		{
			for (int p = 0; p < numParts; p++)
				parts.push_back(intdata[11 + p]);
		}
		else
			parts.push_back(0);

		// fill up xPts and yPts
		int* begOfPts = &(intdata[11 + numParts]);
		const auto pntdata = (double*)begOfPts;  // TODO: Fix compile warning
		for (int i = 0; i < numPoints; i++)
		{
			const int idx = 2 * i;
			xPts.push_back(pntdata[idx]);
			yPts.push_back(pntdata[idx + 1]);
		}
		shapeType = shapetype;
	}
	else if (shapetype == SHP_POLYGON || shapetype == SHP_POLYGONZ || shapetype == SHP_POLYGONM)
	{
		numParts = intdata[9];
		numPoints = intdata[10];

		if (numPoints < 2)
			return FALSE;

		// fill up parts: polygon must have at least 1 part			
		if (numParts > 0)
		{
			for (int p = 0; p < numParts; p++)
				parts.push_back(intdata[11 + p]);
		}
		else
			parts.push_back(0);

		// fill up xPts and yPts
		int* begOfPts = &(intdata[11 + numParts]);
		const auto pntdata = (double*)begOfPts;
		for (int i = 0; i < numPoints; i++)
		{
			const int idx = i * 2;
			xPts.push_back(pntdata[idx]);
			yPts.push_back(pntdata[idx + 1]);
		}

		shapeType = shapetype;
	}
	else if (shapetype == SHP_MULTIPOINT || shapetype == SHP_MULTIPOINTZ || shapetype == SHP_MULTIPOINTM)
	{
		numPoints = intdata[9];

		if (numPoints < 1)
			return FALSE;

		// fill up xPts and yPts
		int* begOfPts = &(intdata[10]);
		const auto pntdata = (double*)begOfPts; // TODO: Fix compile warning
		for (int i = 0; i < numPoints; i++)
		{
			const int idx = i * 2;
			xPts.push_back(pntdata[idx]);
			yPts.push_back(pntdata[idx + 1]);
		}
		shapeType = shapetype;
	}
	else
		return FALSE;

	delete[] cdata;
	cdata = nullptr;

	return TRUE;
}

