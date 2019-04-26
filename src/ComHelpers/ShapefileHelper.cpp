//********************************************************************************************************
//File name: ShapefileHelper.cpp
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
// Paul Meems August 2018: Modernized the code as suggested by CLang and ReSharper

#include "StdAfx.h"
#include "ShapefileHelper.h"
#include "Shapefile.h"
#include "TableClass.h"
#include "TableHelper.h"

// *****************************************************
//		GetMWShapeIdIndex()
// *****************************************************
long ShapefileHelper::GetMWShapeIdIndex(IShapefile* sf)
{
    long index = -1;
    const CComBSTR bstr("MWShapeID");
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
bool ShapefileHelper::CloneNoFields(IShapefile* sf, IShapefile** retVal, bool addShapeId)
{
    if (!sf)
    {
        *retVal = nullptr;
        return false;
    }
    return CloneNoFields(sf, retVal, GetShapeType(sf), addShapeId);
}

// ***********************************************************************
//		CloneNoFields()
// ***********************************************************************
bool ShapefileHelper::CloneNoFields(IShapefile* sfSource, IShapefile** retVal, ShpfileType targetShapeType,
                                    bool addShapeId)
{
    IShapefile* sf = nullptr;
    CoCreateInstance(CLSID_Shapefile, nullptr, CLSCTX_INPROC_SERVER, IID_IShapefile, (void**)&sf);

    VARIANT_BOOL vb;
    if (addShapeId)
    {
        sf->CreateNewWithShapeID(m_globalSettings.emptyBstr, targetShapeType, &vb);
    }
    else
    {
        sf->CreateNew(m_globalSettings.emptyBstr, targetShapeType, &vb);
    }

    if (!vb)
    {
        // Pass error back to source:
        *retVal = sf;
        return false;
    }

    CComPtr<IGeoProjection> gpSource = nullptr;
    CComPtr<IGeoProjection> gpTarget = nullptr;
    sfSource->get_GeoProjection(&gpSource);
    sf->get_GeoProjection(&gpTarget);

    if (gpSource && gpTarget)
    {
        gpTarget->CopyFrom(gpSource, &vb);
    }

    ICallback* callback = nullptr;
    sfSource->get_GlobalCallback(&callback);
    sf->put_GlobalCallback(callback);

    *retVal = sf;
    return true;
}

// ***********************************************************************
//		CloneCore()
// ***********************************************************************
void ShapefileHelper::CloneCore(IShapefile* sfSource, IShapefile** retVal, ShpfileType shpType, bool addShapeId)
{
    CloneNoFields(sfSource, retVal, shpType, addShapeId);
    VARIANT_BOOL vbretval;

    IShapefile* sf = *retVal;

    long numFields;
    sfSource->get_NumFields(&numFields);

    for (long i = 0; i < numFields; i++)
    {
        IField* fld = nullptr;
        IField* fldNew = nullptr;
        sfSource->get_Field(i, &fld);
        fld->Clone(&fldNew);
        fld->Release();

        sf->EditInsertField(fldNew, &i, nullptr, &vbretval);
        fldNew->Release();

        if (!vbretval)
        {
            sf->Close(&vbretval);
            sf->Release();
            sf = nullptr;
            break;
        }
    }
}

// *****************************************************************
//		GetSelectedExtents()
// *****************************************************************
bool ShapefileHelper::GetSelectedExtents(IShapefile* sf, double& xMinRef, double& yMinRef, double& xMaxRef,
                                         double& yMaxRef)
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
                    if (xMin < xMinRef) xMinRef = xMin;
                    if (xMax > xMaxRef) xMaxRef = xMax;
                    if (yMin < yMinRef) yMinRef = yMin;
                    if (yMax > yMaxRef) yMaxRef = yMax;
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
    if (!source || !target)
        return;

    LONG numFields, position;
    VARIANT_BOOL vbretval;
    source->get_NumFields(&numFields);

    for (long i = 0; i < numFields; i++)
    {
        IField* field = nullptr;
        IField* fieldNew = nullptr;
        source->get_Field(i, &field);
        field->Clone(&fieldNew);

        target->get_NumFields(&position);
        target->EditInsertField(fieldNew, &position, nullptr, &vbretval);

        field->Release();
        fieldNew->Release();

        if (!vbretval)
            return;
    }
}

// *****************************************************************
//		GetSelectedIndices()
// *****************************************************************
vector<int>* ShapefileHelper::GetSelectedIndices(IShapefile* sf)
{
    if (!sf) return nullptr;
    VARIANT_BOOL vb;
    long numShapes;
    sf->get_NumShapes(&numShapes);
    auto* result = new vector<int>();
    for (long i = 0; i < numShapes; i++)
    {
        sf->get_ShapeSelected(i, &vb);
        if (vb)
        {
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
    return vb != 0;
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

    const long numShapes = GetNumShapes(sf);
    for (long i = 0; i < numShapes; i++)
    {
        CComPtr<IShape> shp = nullptr;
        sf->get_Shape(i, &shp);
        if (shp)
        {
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
    IShapefile* sfNew = nullptr;
    sf->Clone(&sfNew);
    long numShapes, shapeIndex;
    sf->get_NumShapes(&numShapes);
    for (long i = 0; i < numShapes; i++)
    {
        if (!ShapeSelected(sf, i)) continue;
        CComPtr<IShape> shp = nullptr;
        sf->get_Shape(i, &shp);
        if (shp)
        {
            sfNew->EditAddShape(shp, &shapeIndex);
        }
    }
    return sfNew;
}

// *****************************************************************
//		GetClosestPoint()
// *****************************************************************
bool ShapefileHelper::GetClosestPoint(IShapefile* sf, double x, double y, double maxDistance, std::vector<long>& ids,
                                      long* shapeIndex, long* pointIndex, double& dist)
{
    if (!sf) return false;

    VARIANT_BOOL vb;
    double minDist = DBL_MAX;
    for (long id : ids)
    {
        VARIANT_BOOL visible;
        sf->get_ShapeVisible(id, &visible);
        if (!visible) continue;

        IShape* shp = nullptr;
        sf->get_Shape(id, &shp);
        if (shp)
        {
            long numPoints;
            shp->get_NumPoints(&numPoints);
            double xPnt, yPnt;
            for (long j = 0; j < numPoints; j++)
            {
                shp->get_XY(j, &xPnt, &yPnt, &vb);
                const double distance = sqrt(pow(x - xPnt, 2.0) + pow(y - yPnt, 2.0));
                if (distance < minDist && distance < maxDistance)
                {
                    minDist = distance;
                    *shapeIndex = id;
                    *pointIndex = j;
                }
            }
            shp->Release();
        }
    }
    dist = minDist;
    return minDist < maxDistance;
}

// *****************************************************************
//		tryGetCloserPointForShape()
// *****************************************************************
bool tryGetCloserPointForShape(IShape* shp, IShape* ptShp, double& minDist, double maxDistance, double& fx, double& fy) {
	IShape* resShp = NULL;
	VARIANT_BOOL vb;

	// Get closest points:
	shp->ClosestPoints(ptShp, &resShp);

	if (resShp != NULL) {
		// Get the point snapped on geometry
		double xPnt, yPnt;
		resShp->get_XY(0, &xPnt, &yPnt, &vb);
		// Get the distance
		double distance;
		resShp->get_Length(&distance);

		// Check if this is allowed and/or smaller than the previous found point:
		if (distance < minDist && distance < maxDistance) {
			fx = xPnt;
			fy = yPnt;
			minDist = distance;
			resShp->Release();
			return true;
		}
	}
	return false;
}

// *****************************************************************
//		GetClosestSnapPosition()
// *****************************************************************
bool ShapefileHelper::GetClosestSnapPosition(IShapefile* sf, double x, double y, double maxDistance, std::vector<long>& ids,
	long* shapeIndex, double& fx, double& fy, double& dist)
{
	if (!sf) return false;

	VARIANT_BOOL vb;
	double minDist = DBL_MAX;

    IPoint* pnt = NULL;
    IShape* ptShp = NULL;

	for (long id : ids)
	{
		VARIANT_BOOL visible;
		sf->get_ShapeVisible(id, &visible);
		if (visible == VARIANT_FALSE) continue;

		IShape* shp = NULL;
		sf->get_Shape(id, &shp);

		if (shp != NULL)
		{
            // Create point shape
            ComHelper::CreateShape(&ptShp);
            ptShp->Create(SHP_POINT, &vb);
            if (vb != VARIANT_TRUE)
            {
                shp->Release();
                pnt->Release();
                continue;
            }

            // Add point
            ComHelper::CreatePoint(&pnt);
            pnt->put_X(x);
            pnt->put_Y(y);
            long position = 0;
            ptShp->InsertPoint(pnt, &position, &vb);
            pnt->Release();

			// If shape is a polygon, extract parts as polyline and snap to them instead
			ShpfileType shptype = SHP_NULLSHAPE;
			shp->get_ShapeType(&shptype);
			if (shptype == SHP_POLYGON || shptype == SHP_POLYGONM || shptype == SHP_POLYGONZ) {

				long numParts;
				shp->get_NumParts(&numParts);
				for (int i = 0; i < numParts; i++) {
					// Get part start and end index
					long beg_part, end_part;
					shp->get_Part(i, &beg_part);
					shp->get_EndOfPart(i, &end_part);

					// If weirdness occurs, ignore
					if (beg_part == -1 || end_part == -1)
						continue;

					// Create a new shape
					IShape* partShp = NULL;
					ComHelper::CreateShape(&partShp);

					// Set shape type to polyline
					if (shptype == SHP_POLYGON)
						partShp->put_ShapeType(SHP_POLYLINE);
					else if (shptype == SHP_POLYGONM)
						partShp->put_ShapeType(SHP_POLYLINEM);
					else if (shptype == SHP_POLYGONZ)
						partShp->put_ShapeType(SHP_POLYLINEZ);

					// Insert part
					long part = 0;
					VARIANT_BOOL vbretval;
					partShp->InsertPart(0, &part, &vbretval);

					// Copy & insert points
					long cnt = 0;
					IPoint* pntOld = NULL;
					IPoint* pntNew = NULL;
					for (int i = beg_part; i <= end_part; i++)
					{
						shp->get_Point(i, &pntOld);
						pntOld->Clone(&pntNew);
						partShp->InsertPoint(pntNew, &cnt, &vbretval);
						pntOld->Release();
						pntNew->Release();
						cnt++;
					}


					// Test for snap on part polyline point:
					tryGetCloserPointForShape(partShp, ptShp, minDist, maxDistance, fx, fy);
					partShp->Release();
				}
			}
			else if (shptype != SHP_NULLSHAPE) {
				// Test for snap point on original shape:
				tryGetCloserPointForShape(shp, ptShp, minDist, maxDistance, fx, fy);
			}           
			ptShp->Release();
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
    return InPolygon != 0;
}

// ***********************************************************************
//		BoundsWithinPolygon()
// **********************************************************************
bool ShapefileHelper::BoundsWithinPolygon(IShapefile* sf, int shapeIndex, double b_minX, double b_minY, double b_maxX,
                                          double b_maxY)
{
    if (!sf) return false;

    if (PointInPolygon(sf, shapeIndex, b_minX, b_minY))
    {
        return true;
    }
    if (PointInPolygon(sf, shapeIndex, b_maxX, b_maxY))
    {
        return true;
    }
    if (PointInPolygon(sf, shapeIndex, b_minX, b_maxY))
    {
        return true;
    }
    if (PointInPolygon(sf, shapeIndex, b_maxX, b_minY))
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
    const ShpfileType shpType = GetShapeType(sf);
    return ShapeUtility::IsM(shpType);
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

// ********************************************************************
//		Cast()
// ********************************************************************
CShapefile* ShapefileHelper::Cast(CComPtr<IShapefile>& sf)
{
    return (CShapefile*)&*sf;
}

// ********************************************************************
//		InteractiveEditing()
// ********************************************************************
bool ShapefileHelper::InteractiveEditing(IShapefile* sf)
{
    if (!sf) return false;
    VARIANT_BOOL editing;
    sf->get_InteractiveEditing(&editing);
    return editing != 0;
}

// ********************************************************************
//		IsVolatile()
// ********************************************************************
bool ShapefileHelper::IsVolatile(IShapefile* sf)
{
    if (!sf) return false;
    VARIANT_BOOL isVolatile;
    sf->get_Volatile(&isVolatile);
    return isVolatile != 0;
}

// ********************************************************************
//		InsertMwShapeIdField()
// ********************************************************************
void ShapefileHelper::InsertMwShapeIdField(IShapefile* sf)
{
    if (!sf) return;

    CComPtr<IField> shapeIDField = nullptr;
    ComHelper::CreateInstance(idField, (IDispatch**)&shapeIDField);

    const CComBSTR bstr("MWShapeID");
    shapeIDField->put_Name(bstr);
    shapeIDField->put_Type(INTEGER_FIELD);
    shapeIDField->put_Width(10);
    shapeIDField->put_Precision(10);

    long fldIndex = 0;
    VARIANT_BOOL retVal;
    sf->EditInsertField(shapeIDField, &fldIndex, nullptr, &retVal);
}

// ********************************************************************
//		GetSourceType()
// ********************************************************************
tkShapefileSourceType ShapefileHelper::GetSourceType(IShapefile* sf)
{
    if (!sf) return sstUninitialized;
    tkShapefileSourceType type;
    sf->get_SourceType(&type);
    return type;
}

// ********************************************************************
//		GetModifiedCount()
// ********************************************************************
int ShapefileHelper::GetModifiedCount(IShapefile* sf)
{
    if (!sf) return 0;

    const long numShapes = GetNumShapes(sf);

    int count = 0;
    for (long i = 0; i < numShapes; i++)
    {
        VARIANT_BOOL modified;
        sf->get_ShapeModified(i, &modified);

        if (modified)
        {
            count++;
        }
    }

    return count;
}

// ****************************************************************** 
//		ClearShapefileModifiedFlag
// ****************************************************************** 
void ShapefileHelper::ClearShapefileModifiedFlag(IShapefile* sf)
{
    if (!sf) return;

    long numShapes = 0;
    sf->get_NumShapes(&numShapes);

    CComPtr<ITable> table = nullptr;
    sf->get_Table(&table);
    CTableClass* tableInternal = TableHelper::Cast(table);

    for (int i = 0; i < numShapes; i++)
    {
        sf->put_ShapeModified(i, VARIANT_FALSE);
        tableInternal->MarkRowIsClean(i);
    }
}

// ****************************************************************** 
//		Delete
// ****************************************************************** 
bool ShapefileHelper::Delete(CStringW filename)
{
    vector<CStringW> exts = {L"shp", L"shx", L"dbf", L"prj", L"lbl", L"chart", L"mwd", L"mwx", L"shp.mwsymb", L"mwsr"};

    for (const auto& ext : exts)
    {
        const CStringW name = Utility::ChangeExtension(filename, ext);
        if (Utility::FileExistsW(name) && _wremove(name) != 0)
        {
            USES_CONVERSION;
            CallbackHelper::ErrorMsg(Debug::Format("Failed to remove file: %s", W2A(name)));
            return false;
        }
    }

    return true;
}

// **************************************************
//		MarkFieldsAreUnmodified()
// **************************************************
void ShapefileHelper::MarkFieldsAreUnmodified(IShapefile* table)
{
    long numFields;
    table->get_NumFields(&numFields);

    for (long i = 0; i < numFields; i++)
    {
        CComPtr<IField> field = nullptr;
        table->get_Field(i, &field);
        field->put_Modified(VARIANT_FALSE);
    }
}

// **************************************************
//		MarkShapeRecordIsUnmodified()
// **************************************************
void ShapefileHelper::MarkShapeRecordIsUnmodified(IShapefile* sf, long shapeIndex)
{
    if (!sf) return;

    CComPtr<ITable> table = nullptr;
    sf->get_Table(&table);
    CTableClass* tableInternal = TableHelper::Cast(table);

    const long numShapes = GetNumShapes(sf);
    if (shapeIndex >= 0 && shapeIndex < numShapes)
    {
        sf->put_ShapeModified(shapeIndex, VARIANT_FALSE);
        tableInternal->MarkRowIsClean(shapeIndex);
    }
    else
    {
        CallbackHelper::ErrorMsg("Failed to mark shape as clean. Invalid shape index.");
    }
}
