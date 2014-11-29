#include "stdafx.h"
#include "ShapefileHelper.h"
#include "Shapefile.h"
#include "FieldHelper.h"

// *****************************************************
//		GetMWShapeIdIndex()
// *****************************************************
long ShapefileHelper::GetMWShapeIdIndex(IShapefile* sf)
{
	long index = -1;
	CComBSTR bstr = L"mwshapeid";
	sf->get_FieldIndexByName(bstr, &index);
	return index;
}

// *****************************************************
//		CopyAttributes()
// *****************************************************
void ShapefileHelper::CopyAttributes(IShapefile* sf, long sourceIndex, long targetIndex, long skipFieldIndex)
{
	long numFields;
	sf->get_NumFields(&numFields);
	CComVariant var;
	VARIANT_BOOL vb;
	for (int i = 0; i < numFields; i++)
	{
		if (i == skipFieldIndex) continue;
		sf->get_CellValue(i, sourceIndex, &var);
		sf->EditCellValue(i, targetIndex, var, &vb);
	}
}

// ***********************************************************************
//		CloneNoFields()
// ***********************************************************************
void ShapefileHelper::CloneNoFields(IShapefile* sf, IShapefile** retVal, bool addShapeId)
{
	if (!sf) {
		*retVal = NULL;
		return;
	}
	CloneNoFields(sf, retVal, GetShapeType(sf), addShapeId);
}

// ***********************************************************************
//		CloneNoFields()
// ***********************************************************************
void ShapefileHelper::CloneNoFields(IShapefile* sfSource, IShapefile** retVal, ShpfileType targetShapeType, bool addShapeId)
{
	IShapefile* sf = NULL;
	CoCreateInstance(CLSID_Shapefile, NULL, CLSCTX_INPROC_SERVER, IID_IShapefile, (void**)&sf);

	VARIANT_BOOL vb;
	if (addShapeId)
	{
		sf->CreateNewWithShapeID(A2BSTR(""), targetShapeType, &vb);
	}
	else {
		sf->CreateNew(A2BSTR(""), targetShapeType, &vb);
	}

	// copying the projection string
	BSTR pVal;
	sfSource->get_Projection(&pVal);
	if (pVal != NULL)
		sf->put_Projection(pVal);

	ICallback* callback = NULL;
	sfSource->get_GlobalCallback(&callback);
	sf->put_GlobalCallback(callback);

	*retVal = sf;
}

// ***********************************************************************
//		CloneCore()
// ***********************************************************************
void ShapefileHelper::CloneCore(IShapefile* sfSource, IShapefile** retVal, ShpfileType shpType, bool addShapeId)
{
	ShapefileHelper::CloneNoFields(sfSource, retVal, shpType, addShapeId);
	VARIANT_BOOL vbretval;

	IShapefile* sf = *retVal;

	long numFields;
	sfSource->get_NumFields(&numFields);

	for (long i = 0; i < numFields; i++)
	{
		IField * fld = NULL;
		IField * fldNew = NULL;
		sfSource->get_Field(i, &fld);
		fld->Clone(&fldNew);
		fld->Release();

		sf->EditInsertField(fldNew, &i, NULL, &vbretval);
		fldNew->Release();

		if (!vbretval)
		{
			sf->Close(&vbretval);
			sf->Release();
			sf = NULL;
			break;
		}
	}
}

// *****************************************************************
//		GetSelectedExtents()
// *****************************************************************
bool ShapefileHelper::GetSelectedExtents(IShapefile* sf, double& xMinRef, double& yMinRef, double& xMaxRef, double& yMaxRef)
{
	double xMin, xMax, yMin, yMax;
	bool found = false;
	long numShapes;
	sf->get_NumShapes(&numShapes);
	VARIANT_BOOL vb;

	for (long i = 0; i < numShapes; i++)
	{
		sf->get_ShapeSelected(i, &vb);
		if (vb)
		{
			if (((CShapefile*)sf)->QuickExtentsCore(i, &xMin, &yMin, &xMax, &yMax))
			{
				if (!found)
				{
					xMinRef = xMin, xMaxRef = xMax;
					yMinRef = yMin, yMaxRef = yMax;
					found = true;
				}
				else
				{
					if (xMin < xMinRef)	xMinRef = xMin;
					if (xMax > xMaxRef)	xMaxRef = xMax;
					if (yMin < yMinRef)	yMinRef = yMin;
					if (yMax > yMaxRef)	yMaxRef = yMax;
				}
			}
		}
	}
	return found;
}

// ****************************************************************
//		CopyFields()
// ****************************************************************
void ShapefileHelper::CopyFields(IShapefile* source, IShapefile* target)
{
	if (!target || !target)
		return;

	LONG numFields, position;
	VARIANT_BOOL vbretval;
	source->get_NumFields(&numFields);

	for (long i = 0; i < numFields; i++)
	{
		IField * field = NULL;
		IField * fieldNew = NULL;
		source->get_Field(i, &field);
		field->Clone(&fieldNew);

		target->get_NumFields(&position);
		target->EditInsertField(fieldNew, &position, NULL, &vbretval);

		field->Release();
		fieldNew->Release();
	}
}

// *****************************************************************
//		GetSelectedIndices()
// *****************************************************************
vector<int>* ShapefileHelper::GetSelectedIndices(IShapefile* sf)
{
	if (!sf) return NULL;
	VARIANT_BOOL vb;
	long numShapes;
	sf->get_NumShapes(&numShapes);
	vector<int>* result = new vector<int>();
	for (long i = 0; i < numShapes; i++)
	{
		sf->get_ShapeSelected(i, &vb);
		if (vb) {
			result->push_back(i);
		}
	}
	return result;
}

// *****************************************************************
//		GetNumShapes()
// *****************************************************************
long ShapefileHelper::GetNumShapes(IShapefile* sf)
{
	if (!sf) return 0;
	long numShapes;
	sf->get_NumShapes(&numShapes);
	return numShapes;
}

// *****************************************************************
//		GetNumFields()
// *****************************************************************
long ShapefileHelper::GetNumFields(IShapefile* sf)
{
	if (!sf) return 0;
	long numFields;
	sf->get_NumFields(&numFields);
	return numFields;
}

// *****************************************************************
//		GetNumSelected()
// *****************************************************************
long ShapefileHelper::GetNumSelected(IShapefile* sf)
{
	if (!sf) return 0;
	long numSelected;
	sf->get_NumSelected(&numSelected);
	return numSelected;
}

// *****************************************************************
//		ShapeSelected()
// *****************************************************************
bool ShapefileHelper::ShapeSelected(IShapefile* sf, int shapeIndex)
{
	if (!sf) return false;
	VARIANT_BOOL vb;
	sf->get_ShapeSelected(shapeIndex, &vb);
	return vb ? true: false;
}

// *****************************************************************
//		Rotate()
// *****************************************************************
void ShapefileHelper::Rotate(IShapefile* sf, double originX, double originY, double angleDegree)
{
	if (!sf) return;
	tkShapefileSourceType sourceType;
	sf->get_SourceType(&sourceType);
	if (sourceType != sstInMemory) return;
	
	long numShapes = GetNumShapes(sf);
	for (long i = 0; i < numShapes; i++)
	{
		CComPtr<IShape> shp = NULL;
		sf->get_Shape(i, &shp);
		if (shp) {
			shp->Rotate(originX, originY, angleDegree);
		}
	}
}

// *****************************************************************
//		GetShapeType2D()
// *****************************************************************
ShpfileType ShapefileHelper::GetShapeType2D(IShapefile* sf)
{
	if (!sf) return SHP_NULLSHAPE;
	ShpfileType shpType;
	sf->get_ShapefileType2D(&shpType);
	return shpType;
}

// *****************************************************************
//		GetShapeType2D()
// *****************************************************************
ShpfileType ShapefileHelper::GetShapeType(IShapefile* sf)
{
	if (!sf) return SHP_NULLSHAPE;
	ShpfileType shpType;
	sf->get_ShapefileType(&shpType);
	return shpType;
}


// *****************************************************************
//		CloneSelection()
// *****************************************************************
IShapefile* ShapefileHelper::CloneSelection(IShapefile* sf)
{
	IShapefile* sfNew = NULL;
	sf->Clone(&sfNew);
	long numShapes, shapeIndex;
	sf->get_NumShapes(&numShapes);
	for (long i = 0; i < numShapes; i++)
	{
		if (!ShapeSelected(sf, i)) continue;
		CComPtr<IShape> shp = NULL;
		sf->get_Shape(i, &shp);
		if (shp) {
			sfNew->EditAddShape(shp, &shapeIndex);
		}
	}
	return sfNew;
}

// *****************************************************************
//		GetClosestPoint()
// *****************************************************************
bool ShapefileHelper::GetClosestPoint(IShapefile* sf, double x, double y, double maxDistance, std::vector<long>& ids, long* shapeIndex, long* pointIndex, double& dist)
{
	if (!sf) return false;

	VARIANT_BOOL vb;
	double minDist = DBL_MAX;
	for (size_t i = 0; i < ids.size(); i++)
	{
		VARIANT_BOOL visible;
		sf->get_ShapeVisible(ids[i], &visible);
		if (!visible) continue;

		IShape* shp = NULL;
		sf->get_Shape(ids[i], &shp);
		if (shp)
		{
			long numPoints;
			shp->get_NumPoints(&numPoints);
			double xPnt, yPnt;
			for (long j = 0; j < numPoints; j++)
			{
				shp->get_XY(j, &xPnt, &yPnt, &vb);
				double dist = sqrt(pow(x - xPnt, 2.0) + pow(y - yPnt, 2.0));
				if (dist < minDist && dist < maxDistance)
				{
					minDist = dist;
					*shapeIndex = ids[i];
					*pointIndex = j;
				}
			}
			shp->Release();
		}
	}
	dist = minDist;
	return minDist < maxDistance;
}

// ********************************************************************
//		PointInPolygon()
// ********************************************************************
bool ShapefileHelper::PointInPolygon(IShapefile* sf, long ShapeIndex, double x, double y)
{
	if (!sf) return false;
	VARIANT_BOOL InPolygon;
	if (ShapeIndex < 0) return FALSE;
	sf->PointInShape(ShapeIndex, x, y, &InPolygon);
	return InPolygon ? true : false;
}

/***********************************************************************/
/*		BoundsWithinPolygon()
/***********************************************************************/
bool ShapefileHelper::BoundsWithinPolygon(IShapefile* sf, int shapeIndex, double b_minX, double b_minY, double b_maxX, double b_maxY)
{
	if (!sf) return false;

	if (ShapefileHelper::PointInPolygon(sf, shapeIndex, b_minX, b_minY))
	{
		return true;
	}
	else if (ShapefileHelper::PointInPolygon(sf, shapeIndex, b_maxX, b_maxY))
	{
		return true;
	}
	else if (ShapefileHelper::PointInPolygon(sf, shapeIndex, b_minX, b_maxY))
	{
		return true;
	}
	else if (ShapefileHelper::PointInPolygon(sf, shapeIndex, b_maxX, b_minY))
	{
		return true;
	}
	return false;
}

// ********************************************************************
//		ShapeTypeIsM()
// ********************************************************************
bool ShapefileHelper::ShapeTypeIsM(IShapefile* sf)
{
	if (!sf) return false;
	ShpfileType shpType = GetShapeType(sf);
	return Utility::ShapeTypeIsM(shpType);
}

// ********************************************************************
//		GetSymbologyFilename()
// ********************************************************************
CStringW ShapefileHelper::GetSymbologyFilename(IShapefile* sf)
{
	if (!sf) return L"";
	CComBSTR bstr;
	sf->get_Filename(&bstr);
	CStringW name = OLE2W(bstr);
	if (name.GetLength() > 0)
	{
		name += L".mwsymb";
		if (Utility::FileExistsUnicode(name))
			return name;
	}
	return L"";
}

