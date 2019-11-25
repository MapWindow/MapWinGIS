//********************************************************************************************************
//File name: Shapefile_Geoprocessing.cpp
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

#include "StdAfx.h"
#include "Shapefile.h"
#include "OgrConverter.h"
#include "Templates.h"
#include "Extents.h"
#include "clipper.h"
#include <GeosHelper.h>
#include "FieldStatOperations.h"
#include "Shape.h"
#include "ColoringGraph.h"
#include "GeometryHelper.h"

#ifdef SERIALIZE_POLYGONS
	#include <fstream>
	#include <iostream>
	using namespace std;
#endif
#include "GeosConverter.h"
#include "ShapefileHelper.h"
#include "FieldHelper.h"
#include "ShapeHelper.h"
#include "GeoProcessing.h"

// ReSharper disable CppUseAuto

#pragma region Utilities

// ********************************************************************
//		CloneField()
// ********************************************************************
int CloneField(IShapefile* source, IShapefile* target, int fieldIndex, long newFieldIndex)
{
    IField* fld = nullptr;
    source->get_Field(fieldIndex, &fld);
    if (fld)
    {
        IField* newField = nullptr;
        fld->Clone(&newField);
        fld->Release();
        VARIANT_BOOL vb;
        if (newFieldIndex == -1)
        {
            long numShapes;
            target->get_NumShapes(&numShapes);
            newFieldIndex = numShapes;
        }
        target->EditInsertField(newField, &newFieldIndex, nullptr, &vb);
        newField->Release();
    }
    return newFieldIndex;
}

// ******************************************************************
//		InsertShapesVector()
// ******************************************************************
// Inserts shapes, resulting from intersection of one shape of subject shapefile, 
// and one shape of clip shapefile, attributes a copied from both shapefiles
void CShapefile::InsertShapesVector(IShapefile* sf, vector<IShape*>& vShapes,
                                    IShapefile* sfSubject, long subjectId, std::map<long, long>* fieldMapSubject,
                                    IShapefile* sfClip, long clipId, std::map<long, long>* fieldMapClip)
{
    long numFieldSubject, numFieldsClip;
    sfSubject->get_NumFields(&numFieldSubject);
    if (sfClip)
        sfClip->get_NumFields(&numFieldsClip);

    VARIANT_BOOL vbretval;

    ShpfileType fileType;
    sf->get_ShapefileType(&fileType);

    CComVariant var;

    VARIANT_BOOL isGeographic = VARIANT_FALSE;
    sf->get_IsGeographicProjection(&isGeographic);

    for (vector<IShape*>::value_type shp : vShapes)
    {
        ShpfileType shpType;
        shp->get_ShapeType(&shpType);

        if (shpType == fileType)
        {
            // area checking
            shpType = ShapeUtility::Convert2D(shpType);
            if (shpType == SHP_POLYGON)
            {
                double area;
                shp->get_Area(&area);
                if (area < m_globalSettings.GetMinPolygonArea(isGeographic))
                {
                    shp->Release();
                    continue;
                }

                double perimeter;
                shp->get_Perimeter(&perimeter);

                if (isGeographic)
                    area *= METERS_PER_DEGREE;
                // comparison is performed in meters, area will grow as square of linear size
                // we multilpy area only; in reality:((area * c^2)/ (perimeter * c))

                if (area / perimeter < m_globalSettings.minAreaToPerimeterRatio)
                {
                    shp->Release();
                    continue;
                }
            }

            long newId;
            sf->get_NumShapes(&newId);
            sf->EditInsertShape(shp, &newId, &vbretval);

            // passing the values from subject shapefile
            // can be optimized for not extracting the same values many times
            if (fieldMapSubject)
            {
	            std::map<long, long>::iterator p = fieldMapSubject->begin();
                while (p != fieldMapSubject->end())
                {
                    sfSubject->get_CellValue(p->first, subjectId, &var);
                    sf->EditCellValue(p->second, newId, var, &vbretval);
                    ++p;
                }
            }
            else
            {
                for (int iFld = 0; iFld < numFieldSubject; iFld++)
                {
                    sfSubject->get_CellValue(iFld, subjectId, &var);
                    sf->EditCellValue(iFld, newId, var, &vbretval);
                }
            }

            // passing the values from clip shapefile
            if (sfClip && fieldMapClip)
            {
	            std::map<long, long>::iterator p = fieldMapClip->begin();
                while (p != fieldMapClip->end())
                {
                    sfClip->get_CellValue(p->first, clipId, &var);
                    sf->EditCellValue(p->second, newId, var, &vbretval);
                    ++p;
                }
            }
        }
        shp->Release();
    }
}

// **********************************************************************
//		InsertGeosGeometry()
// **********************************************************************
// A utility function to add geometry produced by simplification routine to the sfTarget shapefile,
// with copying of attributes from source shapefile.
// initShapeIndex - the index of shape to copy the attribute from
bool InsertGeosGeometry(IShapefile* sfTarget, GEOSGeometry* gsNew, IShapefile* sfSouce, int initShapeIndex)
{
    if (gsNew)
    {
        ShpfileType shpType;
        sfTarget->get_ShapefileType(&shpType);
        const bool isM = ShapeUtility::IsM(shpType);

        std::vector<IShape*> shapes;
        if (GeosConverter::GeomToShapes(gsNew, &shapes, isM))
        {
            long index, numFields;
            VARIANT_BOOL vbretval;
            sfTarget->get_NumFields(&numFields);

            for (auto& shape : shapes)
            {
                sfTarget->get_NumShapes(&index);
                sfTarget->EditInsertShape(shape, &index, &vbretval);

                if (vbretval)
                {
                    CComVariant val;
                    for (int f = 0; f < numFields; f++)
                    {
                        sfSouce->get_CellValue(f, initShapeIndex, &val);
                        sfTarget->EditCellValue(f, index, val, &vbretval);
                    }
                }
                shape->Release();
            }
            return true;
        }
    }
    return false;
}

// ********************************************************************
//		CopyShape()
// ********************************************************************
// For the sort function
// The row index of attribute table are taken from the key property of the shape
void CopyShape(IShapefile* sfSource, IShape* shp, IShapefile* sfResult)
{
    if (shp)
    {
        USES_CONVERSION;
        CComBSTR key;
        shp->get_Key(&key);
        const CString str = OLE2CA(key);
        const int initIndex = atoi(str);

        VARIANT_BOOL isEditingShapes, vbretval;
        sfSource->get_EditingShapes(&isEditingShapes);

        LONG numShapes;
        sfResult->get_NumShapes(&numShapes);

        if (isEditingShapes)
        {
            // in the editing mode we share a shape with the parent shapefile
            // a copy is needed to avoid conflicts
            IShape* shpCopy = nullptr;
            shp->Clone(&shpCopy);
            sfResult->EditInsertShape(shpCopy, &numShapes, &vbretval);
            shpCopy->Release();
        }
        else
        {
            sfResult->EditInsertShape(shp, &numShapes, &vbretval);
        }

        if (vbretval)
        {
            LONG numFields;
            sfSource->get_NumFields(&numFields);

            // copy attributes
            CComVariant val;
            for (int iFld = 0; iFld < numFields; iFld++)
            {
                sfSource->get_CellValue(iFld, initIndex, &val);
                sfResult->EditCellValue(iFld, numShapes, val, &vbretval);
            }
        }
        shp->Release();
    }
}
#pragma endregion

#pragma region SelectByShapefile

// ******************************************************************
//		SelectByShapefile()
// ******************************************************************
//  Returns numbers(ids) of shapes of this shapefile which are situated 
//	in certain spatial relation to the shapes of the input shapefile. 

STDMETHODIMP CShapefile::SelectByShapefile(IShapefile* sf, tkSpatialRelation Relation,
                                           VARIANT_BOOL SelectedOnly, VARIANT* arr, ICallback* cBack,
                                           VARIANT_BOOL* retval)
{
    
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
    USES_CONVERSION;
    *retval = VARIANT_FALSE;

    if (_globalCallback == nullptr && cBack != nullptr)
    {
        _globalCallback = cBack;
        _globalCallback->AddRef();
    }

    if (sf == nullptr)
    {
        ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
        return S_OK;
    }

    long _numShapes2;
    const long _numShapes1 = _shapeData.size();
    sf->get_NumShapes(&_numShapes2);
    if (_numShapes1 == 0)return NULL;
    if (_numShapes2 == 0) return NULL;

    QTree* qTree = this->GenerateQTreeCore(false);
    if (!qTree)
    {
        ErrorMessage(tkFAILED_TO_BUILD_SPATIAL_INDEX);
        return NULL;
    }

    // ids of selected shapes
    set<long> result;

    // to avoid converting same shapes to ogr geometry multiple times
    vector<OGRGeometry *> vGeometries;
    vGeometries.assign(_numShapes1, nullptr);

    vector<ShapeRecord*>* data = ((CShapefile*)sf)->get_ShapeVector();

    long percent = 0;
    for (long shapeid2 = 0; shapeid2 < _numShapes2; shapeid2++)
    {
        CallbackHelper::Progress(_globalCallback, shapeid2, _numShapes2, "Calculating...", _key, percent);

        if (SelectedOnly && !(*data)[shapeid2]->selected())
            continue;

        double xMin, xMax, yMin, yMax;
        ((CShapefile*)sf)->QuickExtentsCore(shapeid2, &xMin, &yMin, &xMax, &yMax);
        vector<int> shapeIds = qTree->GetNodes(QTreeExtent(xMin, xMax, yMax, yMin));

        if (!shapeIds.empty())
        {
            IShape* shp2 = nullptr;
            sf->get_Shape(shapeid2, &shp2);
            OGRGeometry* oGeom2 = OgrConverter::ShapeToGeometry(shp2);
            shp2->Release();

            if (oGeom2 != nullptr)
            {
                for (long shapeId1 : shapeIds)
                {
                    // shape wasn't selected so far
                    if (result.find(shapeId1) == result.end())
                    {
                        OGRGeometry* oGeom1;

                        if (vGeometries[shapeId1] == nullptr)
                        {
                            IShape* shp1 = nullptr;
                            this->get_Shape(shapeId1, &shp1);
                            oGeom1 = OgrConverter::ShapeToGeometry(shp1);
                            shp1->Release();
                            vGeometries[shapeId1] = oGeom1;
                        }
                        else
                        {
                            oGeom1 = vGeometries[shapeId1];
                        }

                        OGRBoolean res = 0;
                        if (Relation == srContains) res = oGeom1->Contains(oGeom2);
                        else if (Relation == srCrosses) res = oGeom1->Crosses(oGeom2);
                        else if (Relation == srEquals) res = oGeom1->Equals(oGeom2);
                        else if (Relation == srIntersects) res = oGeom1->Intersects(oGeom2);
                        else if (Relation == srDisjoint) res = oGeom1->Intersects(oGeom2);
                        else if (Relation == srOverlaps) res = oGeom1->Overlaps(oGeom2);
                        else if (Relation == srTouches) res = oGeom1->Touches(oGeom2);
                        else if (Relation == srWithin) res = oGeom1->Within(oGeom2);

                        if (res)
                        {
                            result.insert(shapeId1);
                        }
                    }
                }
                OGRGeometryFactory::destroyGeometry(oGeom2);
            }
        }
    }

    // disjoint is opposite to intersects; so to get it we need to find shapes that do intersect with input
    // and then invert the selection
    if (Relation == srDisjoint)
    {
        vector<long> v;
        set<long>::iterator p = result.begin();

        for (int i = 0; i < _numShapes1; i++)
        {
            bool include = true;
            while (p != result.end())
            {
                if (*p == i)
                    include = false; // this shape actually intersects, so it can't be disjointed

                if (*p > i)
                    break;

                ++p;
            }
            if (include)
            {
                v.push_back(i);
            }
        }
        *retval = Templates::Vector2SafeArray(&v, VT_I4, arr) ? VARIANT_TRUE : VARIANT_FALSE;
    }
    else
    {
        *retval = Templates::Set2SafeArray(&result, VT_I4, arr) ? VARIANT_TRUE : VARIANT_FALSE;
    }

    //  cleaning
    CallbackHelper::ProgressCompleted(_globalCallback, _key);

    for (auto& vGeometrie : vGeometries)
    {
        if (vGeometrie != nullptr)
            OGRGeometryFactory::destroyGeometry(vGeometrie);
    }

    delete qTree;

    return S_OK;
}

// ***********************************************************
// 		SelectShapesAlt()
// ***********************************************************
//  Alternative function for selection of shapes which fall in the given 
//	rectangular. Uses quad tree on permanent basis. May become 
//	a substitute for SelectShapes function after some improvements.
//  returns VARIANT_TRUE when shapes were selected, and VARIANT_FALSE if
//	no shapes fell into selection
VARIANT_BOOL CShapefile::SelectShapesAlt(IExtents* BoundBox, double Tolerance, SelectMode SelectMode, VARIANT* arr)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    double xMin, xMax, yMin, yMax, zMin, zMax;
    BoundBox->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);

    if (xMin == xMax && yMin == yMax)
    {
        xMax += 1;
        yMax += 1;
    }

    QTree* qTree = this->GenerateQTreeCore(false);
    if (!qTree)
    {
        ErrorMessage(tkFAILED_TO_BUILD_SPATIAL_INDEX);
        return NULL;
    }

    if (Tolerance > 0.0)
    {
        xMin = xMin - Tolerance / 2;
        yMin = yMin - Tolerance / 2;
        xMax = xMax + Tolerance / 2;
        yMax = yMax + Tolerance / 2;
    }

    BoundBox->SetBounds(xMin, yMin, zMin, xMax, yMax, zMax);

    set<long> results;
    vector<int> shapeIds = qTree->GetNodes(QTreeExtent(xMin, xMax, yMax, yMin));

    IShape* temp = nullptr;
    ((CExtents*)BoundBox)->ToShape(&temp);
    OGRGeometry* oBox = OgrConverter::ShapeToGeometry(temp);
    temp->Release();

    for (int shapeId : shapeIds)
    {
        IShape* shp = nullptr;
        this->get_Shape((long)shapeId, &shp);
        OGRGeometry* oGeom = OgrConverter::ShapeToGeometry(shp);
        shp->Release();

        OGRBoolean res = oBox->Contains(oGeom);

        if (!res)
            res = oGeom->Contains(oBox);

        if (SelectMode == INTERSECTION && !res)
            res = oGeom->Intersects(oBox);

        if (res) results.insert(shapeId);
        OGRGeometryFactory::destroyGeometry(oGeom);
    }

    delete qTree;
    if (oBox != nullptr) OGRGeometryFactory::destroyGeometry(oBox);
    const VARIANT_BOOL retval = Templates::Set2SafeArray(&results, VT_I4, arr);
    return retval;
}
#pragma endregion

#pragma region Dissolve

// *************************************************************
//     Dissolve()
// *************************************************************
//  Merges shapes of the shapefile based on the attribute of the given
// 	field. Shapes with the same attribute are merged into one.
STDMETHODIMP CShapefile::Dissolve(long FieldIndex, VARIANT_BOOL SelectedOnly, IShapefile** sf)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    DissolveCore(FieldIndex, SelectedOnly, nullptr, sf);
    return S_OK;
}

// *************************************************************
//     DissolveWithStats()
// *************************************************************
STDMETHODIMP CShapefile::DissolveWithStats(long FieldIndex, VARIANT_BOOL SelectedOnly,
                                           IFieldStatOperations* statOperations, IShapefile** sf)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    DissolveCore(FieldIndex, SelectedOnly, statOperations, sf);
    return S_OK;
}

// *************************************************************
//     Dissolve()
// *************************************************************
//  Merges shapes of the shapefile based on the attribute of the given
// 	field. Shapes with the same attribute are merged into one.
void CShapefile::DissolveCore(long FieldIndex, VARIANT_BOOL SelectedOnly, IFieldStatOperations* operations,
                              IShapefile** sf)
{
    // ----------------------------------------------
    //   Validation
    // ----------------------------------------------
    long numFields;
    this->get_NumFields(&numFields);

    if (FieldIndex < 0 || FieldIndex >= numFields)
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
        return;
    }

    if (!ValidateInput(this, "Dissolve", "this", SelectedOnly))
        return;

    // ----------------------------------------------
    //   Creating output
    // ----------------------------------------------
    ShpfileType type = _shpfiletype;

    // for points change to multi-point type, as there is no other way to group them
    if (type == SHP_POINT) type = SHP_MULTIPOINT;
    if (type == SHP_POINTZ) type = SHP_MULTIPOINTZ;
    if (type == SHP_POINTM) type = SHP_MULTIPOINTM;

    // ShapefileHelper::CloneNoFields(this, sf, type);
    // -----------------------------------------------
    //	 Creating output
    // -----------------------------------------------
    if (!ShapefileHelper::CloneNoFields(this, sf, type))
    {
        // Get errorcode and pass the source:
        long errorCode;
        (*sf)->get_LastErrorCode(&errorCode);
        *sf = nullptr;
        ErrorMessage(errorCode);
        return;
    }
    CloneField(this, *sf, FieldIndex, -1);

    // -------------------------------------------
    //  processing
    // -------------------------------------------
    if (_geometryEngine == engineGeos)
    {
        this->DissolveGEOS(FieldIndex, SelectedOnly, operations, *sf);
    }
    else
    {
        this->DissolveClipper(FieldIndex, SelectedOnly, operations, *sf);
    }

    // -------------------------------------------
    // output validation
    // -------------------------------------------
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
    ValidateOutput(sf, "Dissolve");
    return;
}

// *************************************************************
//     GetStatOperationName()
// *************************************************************
char* GetStatOperationName(tkFieldStatOperation op)
{
    switch (op)
    {
    case fsoSum:
        return "SUM";
    case fsoAvg:
        return "AVG";
    case fsoMin:
        return "MIN";
    case fsoMax:
        return "MAX";
    case fsoMode:
        return "MOD";
    case fsoWeightedAvg:
        return "WAVG";
    default:
        return "";
    }
}

// *************************************************************
//     CalculateFieldStats()
// *************************************************************
void CShapefile::CalculateFieldStats(map<int, vector<int>*>& fieldMap, IFieldStatOperations* ioperations,
                                     IShapefile* sf)
{
    // --------------------------------------------
    // validating operations
    // --------------------------------------------
    USES_CONVERSION;
    long numFields;
    this->get_NumFields(&numFields);
    std::vector<FieldOperation*>* operations = &((CFieldStatOperations*)ioperations)->_operations;

    VARIANT_BOOL vb;
    ioperations->Validate(this, &vb);

    for (vector<FieldOperation*>::value_type op : *operations)
    {
        // creating output field for operation
        if (op->valid)
        {
            IField* field = nullptr;
            this->get_Field(op->fieldIndex, &field);
            if (field)
            {
                CComBSTR bstr;
                field->get_Name(&bstr);
                CStringW name = OLE2W(bstr);
                name = name + "_" + A2W(GetStatOperationName(op->operation));

                FieldType type;
                field->get_Type(&type);
                if (type == INTEGER_FIELD && op->operation == fsoAvg)
                    type = DOUBLE_FIELD;

                long precision;
                field->get_Precision(&precision);
                long width;
                field->get_Width(&width);

                long fieldIndex;
                const CComBSTR bstrName(name);
                sf->EditAddField(bstrName, type, precision, width, &fieldIndex);

                op->targetIndex = fieldIndex;
                op->targetFieldType = type;
                field->Release();
            }
        }
    }

    // make sure that output field names are unique
    FieldHelper::UniqueFieldNames(sf);

    // --------------------------------------------
    //   calculating
    // --------------------------------------------
    double areaSum = 0;
    double area;
    double val;
    double sum = 0.0;
    CString sSum;
    CComVariant var;
    int index = 0;
    const int size = fieldMap.size();
    long percent = 0;
    map<CString, int> frequencies;

    ShpfileType type;
    sf->get_ShapefileType(&type);
    type = ShapeUtility::Convert2D(type);

    map<int, vector<int>*>::iterator p = fieldMap.begin(); // row in the output, rows in the input
    while (p != fieldMap.end())
    {
        CallbackHelper::Progress(_globalCallback, index, size, "Calculating stats", _key, percent);

        for (vector<FieldOperation*>::value_type op : *operations)
        {
            if (!op->valid) continue;

            if (op->targetFieldType == STRING_FIELD)
            {
                CString sVal = "";
                frequencies.clear();
                for (int j : *p->second)
                {
                    this->get_CellValue(op->fieldIndex, j, &var);
                    stringVal(var, sVal);

                    if (op->operation == fsoMode)
                    {
                        frequencies[sVal] = frequencies.find(sVal) == frequencies.end() ? 1 : frequencies[sVal] + 1;
                    }
                    else
                    {
                        // take the first one, as output despite the operation
                        if (sSum.GetLength() == 0)
                        {
                            sSum = sVal;
                        }
                        else
                        {
                            const int res = sSum.CompareNoCase(sVal);
                            if (res < 0 && op->operation == fsoMax ||
                                res > 0 && op->operation == fsoMin)
                            {
                                sSum = sVal;
                            }
                        }
                    }
                }

                if (op->operation == fsoMode)
                {
                    int max = INT_MIN;
                    map<CString, int>::iterator frequency = frequencies.begin();
                    while (frequency != frequencies.end())
                    {
                        if (max < frequency->second)
                        {
                            max = frequency->second;
                            sSum = frequency->first;
                        }
                        ++frequency;
                    }
                }

                const CComVariant result(sSum);
                sf->EditCellValue(op->targetIndex, p->first, result, &vb);
            }
            else
            {
                switch (op->operation)
                {
                case fsoSum:
                case fsoAvg:
                case fsoWeightedAvg:
                    sum = 0.0;
                    areaSum = 0.0;
                    break;
                case fsoMin:
                    sum = DBL_MAX;
                    break;
                case fsoMax:
                    sum = DBL_MIN;
                    break;
                case fsoMode: break;
                default: ;
                }

                // going through rows of original shapefile, which fell into the same group
                for (int j : *p->second)
                {
                    this->get_CellValue(op->fieldIndex, j, &var);
                    dVal(var, val);

                    switch (op->operation)
                    {
                    case fsoSum:
                    case fsoAvg:
                        sum += val;
                        break;
                    case fsoMin:
                        if (val < sum) sum = val;
                    case fsoMax:
                        if (val > sum) sum = val;
                        break;
                    case fsoWeightedAvg:
                        {
                            if (type == SHP_POLYGON || type == SHP_POLYLINE)
                            {
                                IShape* shp = nullptr;
                                this->GetValidatedShape(j, &shp);
                                if (shp)
                                {
                                    if (type == SHP_POLYGON) shp->get_Area(&area);
                                    else shp->get_Length(&area); // weighting by length of polylines
                                    shp->Release();
                                    areaSum += area;
                                    sum += val * area;
                                }
                            }
                            else
                            {
                                sum += val; // regular sum for points
                            }
                        }
                        break;
                    case fsoMode: break;
                    default: ;
                    }
                }

                if (op->operation == fsoAvg)
                {
                    sum /= p->second->size();
                }
                if (op->operation == fsoWeightedAvg)
                {
                    sum /= areaSum;
                }

                if (op->targetFieldType == INTEGER_FIELD)
                {
                    const CComVariant result(Utility::Rint(sum));
                    sf->EditCellValue(op->targetIndex, p->first, result, &vb);
                }
                else
                {
                    const CComVariant result(sum);
                    sf->EditCellValue(op->targetIndex, p->first, result, &vb);
                }
            }
        }
        index++;
        ++p;
    }

    CallbackHelper::ProgressCompleted(_globalCallback, _key);
}

// *************************************************************
//     DissolveGEOS()
// *************************************************************
void CShapefile::DissolveGEOS(long FieldIndex, VARIANT_BOOL SelectedOnly, IFieldStatOperations* operations,
                              IShapefile* sf)
{
    map<int, vector<int>*> fieldMap; // index in output, indices in input
    map<CComVariant, vector<int>*> indicesMap; // value in input, indices in input
    map<CComVariant, vector<GEOSGeometry*>*> shapeMap;

    CComVariant val; // VARIANT hasn't got comparison operators and therefore
    // can't be used with associative containers

    bool calcStats = false;
    if (operations)
    {
        int count;
        operations->get_Count(&count);
        calcStats = count > 0;
    }

    ReadGeosGeometries(SelectedOnly);

    long percent = 0;
    int size = (int)_shapeData.size();
    for (long i = 0; i < size; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, size, "Grouping shapes...", _key, percent);

        if (!ShapeAvailable(i, SelectedOnly))
            continue;

        GEOSGeometry* gsGeom = this->GetGeosGeometry(i);
        if (gsGeom != nullptr)
        {
            this->get_CellValue(FieldIndex, i, &val);
            if (shapeMap.find(val) != shapeMap.end())
            {
                shapeMap[val]->push_back(gsGeom);
                if (calcStats)
                {
                    indicesMap[val]->push_back(i);
                }
            }
            else
            {
                auto* v = new vector<GEOSGeometry*>;
                v->push_back(gsGeom);
                shapeMap[val] = v;

                if (calcStats)
                {
                    auto* v2 = new vector<int>;
                    v2->push_back(i);
                    indicesMap[val] = v2;
                }
            }
        }
    }

    const bool isM = ShapeUtility::IsM(_shpfiletype);

    // saving results							
    long count = 0; // number of shapes inserted
    int shapeProcessed = 0; // for progress bar
    percent = 0;
    size = shapeMap.size();

    VARIANT_BOOL vbretval;
    map<CComVariant, vector<GEOSGeometry*>*>::iterator p = shapeMap.begin();

    ShpfileType targetType;
    sf->get_ShapefileType(&targetType);

    while (p != shapeMap.end())
    {
        CallbackHelper::Progress(_globalCallback, shapeProcessed, size, "Merging shapes...", _key, percent);

        GEOSGeometry* gsGeom = GeosConverter::MergeGeometries(*p->second, nullptr, false, false);
        delete p->second; // deleting the vector

        if (gsGeom != nullptr)
        {
            std::vector<IShape*> vShapes;
            if (GeosConverter::GeomToShapes(gsGeom, &vShapes, isM))
            {
                for (std::vector<IShape*>::value_type shp : vShapes)
                {
                    if (shp != nullptr)
                    {
                        ShapeHelper::ForceProperShapeType(shp, targetType);

                        sf->EditInsertShape(shp, &count, &vbretval);
                        if (vbretval)
                            sf->EditCellValue(0, count, (VARIANT)p->first, &vbretval);

                        shp->Release();

                        if (calcStats)
                        {
                            std::vector<int>* indices = indicesMap[p->first];
                            fieldMap[count] = indices;
                        }

                        count++;
                    }
                }
            }
            GeosHelper::DestroyGeometry(gsGeom);
        }
        ++p;
        shapeProcessed++;
    }

    if (calcStats)
    {
        CalculateFieldStats(fieldMap, operations, sf);

        // delete indices map
        map<CComVariant, vector<int>*>::iterator p2 = indicesMap.begin();
        while (p2 != indicesMap.end())
        {
            delete p2->second;
            ++p2;
        }
    }

    this->ClearCachedGeometries();
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
}

// *************************************************************
//     DissolveClipper()
// *************************************************************
void CShapefile::DissolveClipper(long FieldIndex, VARIANT_BOOL SelectedOnly, IFieldStatOperations* operations,
                                 IShapefile* sf)
{
    map<int, vector<int>*> fieldMap; // index in output, indices in input
    map<CComVariant, vector<int>*> indicesMap; // value in input, indices in input
    map<CComVariant, ClipperLib::Clipper*> shapeMap;

    CComVariant val; // VARIANT hasn't got comparison operators and therefore
    // can't be used with associative containers
    long percent = 0;
    const int size = (int)_shapeData.size();
    // std::vector<ClipperLib::Polygons*> polygons;
    std::vector<ClipperLib::Paths*> polygons;
    polygons.resize(size, nullptr);

    ClipperConverter ogr(this);

    bool calcStats = false;
    if (operations)
    {
        int count;
        operations->get_Count(&count);
        calcStats = count > 0;
    }

    for (long i = 0; i < size; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, size, "Merging shapes...", _key, percent);

        if (!ShapeAvailable(i, SelectedOnly))
            continue;

        IShape* shp = nullptr;
        this->GetValidatedShape(i, &shp);
        if (!shp) continue;

        // ClipperLib::Polygons* poly = ogr.Shape2ClipperPolygon(shp);
        ClipperLib::Paths* poly = ogr.Shape2ClipperPolygon(shp);
        shp->Release();

        if (poly == nullptr) continue;

        this->get_CellValue(FieldIndex, i, &val);

        if (shapeMap.find(val) != shapeMap.end())
        {
            // shapeMap[val]->AddPolygons(*poly, ClipperLib::ptClip);
            shapeMap[val]->AddPaths(*poly, ClipperLib::ptClip, true);
            polygons[i] = poly;
            if (calcStats)
            {
                indicesMap[val]->push_back(i);
            }
        }
        else
        {
            shapeMap[val] = new ClipperLib::Clipper();
            // shapeMap[val]->AddPolygons(*poly, ClipperLib::ptClip);
            shapeMap[val]->AddPaths(*poly, ClipperLib::ptClip, true);
            polygons[i] = poly;

            if (calcStats)
            {
                auto* v2 = new vector<int>;
                v2->push_back(i);
                indicesMap[val] = v2;
            }
        }
    }

    // perform clipping and saving the results
    VARIANT_BOOL vbretval;
    long count = 0;
    map<CComVariant, ClipperLib::Clipper*>::iterator p = shapeMap.begin();
    while (p != shapeMap.end())
    {
        // ClipperLib::Polygons result;
        ClipperLib::Paths result;
        ClipperLib::Clipper* clip = p->second;
        if (clip)
        {
            clip->Execute(ClipperLib::ctUnion, result);
            IShape* shp = ogr.ClipperPolygon2Shape(&result);

            if (shp)
            {
                long numPoints;
                shp->get_NumPoints(&numPoints);
                if (numPoints > 0)
                {
                    sf->EditInsertShape(shp, &count, &vbretval);
                    if (vbretval)
                        sf->EditCellValue(0, count, (VARIANT)p->first, &vbretval);

                    if (calcStats)
                    {
                        std::vector<int>* indices = indicesMap[p->first];
                        fieldMap[count] = indices;
                    }

                    count++;
                }
                shp->Release();
            }
            delete p->second;
        }
        ++p;
    }

    // deleting the polygons
    for (int i = 0; i < size; i++)
    {
        if (polygons[i])
        {
            delete polygons[i];
        }
    }

    CallbackHelper::ProgressCompleted(_globalCallback, _key);
    shapeMap.clear();

    if (calcStats)
    {
        CalculateFieldStats(fieldMap, operations, sf);

        // delete indices map
        map<CComVariant, vector<int>*>::iterator index = indicesMap.begin();
        while (index != indicesMap.end())
        {
            delete index->second;
            ++index;
        }
    }
}

// ********************************************************************
//		AggregateShapesWithStats()
// ********************************************************************
STDMETHODIMP CShapefile::AggregateShapesWithStats(VARIANT_BOOL SelectedOnly, LONG FieldIndex,
                                                  IFieldStatOperations* statOperations, IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    AggregateShapesCore(SelectedOnly, FieldIndex, statOperations, retval);
    return S_OK;
}

// ********************************************************************
//		AggregateShapes()
// ********************************************************************
STDMETHODIMP CShapefile::AggregateShapes(VARIANT_BOOL SelectedOnly, LONG FieldIndex, IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    AggregateShapesCore(SelectedOnly, FieldIndex, nullptr, retval);
    return S_OK;
}

// ********************************************************************
//		AggregateShapesCore()
// ********************************************************************
// Returns new shapefile instance.
void CShapefile::AggregateShapesCore(VARIANT_BOOL SelectedOnly, LONG FieldIndex, IFieldStatOperations* operations,
                                     IShapefile** retval)
{
    long numFields;
    this->get_NumFields(&numFields);

    if (FieldIndex != -1 && !(FieldIndex >= 0 && FieldIndex < numFields))
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
        return;
    }

    if (!ValidateInput(this, "AggregateShapes", "this", SelectedOnly))
        return;

    // ----------------------------------------------
    //   Creating output
    // ----------------------------------------------
    ShpfileType targetType = _shpfiletype;

    // for points change to multi-point type, as there is no other way to group them
    if (targetType == SHP_POINT) targetType = SHP_MULTIPOINT;
    if (targetType == SHP_POINTZ) targetType = SHP_MULTIPOINTZ;
    if (targetType == SHP_POINTM) targetType = SHP_MULTIPOINTM;

    //ShapefileHelper::CloneNoFields(this, retval, targetType);
    // -----------------------------------------------
    //	 Creating output
    // -----------------------------------------------
    if (!ShapefileHelper::CloneNoFields(this, retval, targetType))
    {
        // Get errorcode and pass the source:
        long errorCode;
        (*retval)->get_LastErrorCode(&errorCode);
        *retval = nullptr;
        ErrorMessage(errorCode);
        return;
    }
    const long newFieldIndex = 0;
    CloneField(this, *retval, FieldIndex, newFieldIndex);

    // ----------------------------------------------
    //   Building groups by field id
    // ----------------------------------------------
    VARIANT_BOOL vbretval;
    map<int, vector<int>*> fieldMap; // index in output, indices in input
    map<CComVariant, vector<int>*> indicesMap; // value in input, indices in input
    map<CComVariant, vector<IShape*>*> shapeMap;

    CComVariant val;

    bool calcStats = false;
    if (operations)
    {
        int count;
        operations->get_Count(&count);
        calcStats = count > 0;
    }

    long percent = 0;
    int size = (int)_shapeData.size();

    for (long i = 0; i < size; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, size, "Grouping shapes...", _key, percent);

        if (!ShapeAvailable(i, SelectedOnly))
            continue;

        IShape* shp = nullptr;
        this->GetValidatedShape(i, &shp);
        if (!shp) continue;

        if (FieldIndex != -1)
            this->get_CellValue(FieldIndex, i, &val);

        if (shapeMap.find(val) != shapeMap.end())
        {
            shapeMap[val]->push_back(shp);
            if (calcStats)
            {
                indicesMap[val]->push_back(i);
            }
        }
        else
        {
            auto* v = new vector<IShape*>;
            v->push_back(shp);
            shapeMap[val] = v;

            if (calcStats)
            {
                auto* v2 = new vector<int>;
                v2->push_back(i);
                indicesMap[val] = v2;
            }
        }
    }

    // ----------------------------------------------
    //   Merging groups
    // ----------------------------------------------
    long count = 0; // number of shapes inserted
    int i = 0; // for progress bar
    percent = 0;
    size = shapeMap.size();
    map<CComVariant, vector<IShape*>*>::iterator p = shapeMap.begin();

    while (p != shapeMap.end())
    {
        CallbackHelper::Progress(_globalCallback, i, size, "Merging shapes...", _key, percent);

        // merging shapes
        vector<IShape*>* shapes = p->second;
        long pntIndex = 0;
        long partIndex = 0;

        IShape* shpBase = nullptr;
        for (unsigned int j = 0; j < shapes->size(); j++)
        {
            if (j == 0)
            {
                if (_isEditingShapes)
                {
                    // in editing mode we share the shape with parent shapefile
                    // so a copy is needed to avoid conflicts
                    (*shapes)[0]->Clone(&shpBase);
                    (*shapes)[0]->Release();
                }
                else
                {
                    // in the regular mode we are the sole owners of shape
                    shpBase = (*shapes)[0];
                }
                shpBase->get_NumPoints(&pntIndex);
                shpBase->get_NumParts(&partIndex);

                ShapeHelper::ForceProperShapeType(shpBase, targetType);
            }
            else
            {
                IShape* shp = (*shapes)[j];

                if (ShapeUtility::Convert2D(targetType) == SHP_MULTIPOINT)
                {
                    // in case of multi-point target type, simply copy all the points to base shape
                    // no need to deal with parts, multi-points don't have those
                    long numPoints = 0;
                    shp->get_NumPoints(&numPoints);
                    for (long n = 0; n < numPoints; n++)
                    {
                        CComPtr<IPoint> pnt = nullptr;
                        shp->get_Point(n, &pnt);
                        long pointCount;
                        shpBase->get_NumPoints(&pointCount);
                        shpBase->InsertPoint(pnt, &pointCount, &vbretval);
                    }
                }
                else
                {
                    long numParts;
                    shp->get_NumParts(&numParts);

                    for (long part = 0; part < numParts; part++)
                    {
                        shpBase->InsertPart(pntIndex, &partIndex, &vbretval);

                        long start, end;
                        shp->get_Part(part, &start);
                        shp->get_EndOfPart(part, &end);

                        for (long point = start; point <= end; point++)
                        {
                            IPoint* pnt = nullptr;
                            shp->get_Point(point, &pnt);
                            if (pnt)
                            {
                                IPoint* pntNew = nullptr;
                                pnt->Clone(&pntNew);
                                shpBase->InsertPoint(pntNew, &pntIndex, &vbretval);
                                pntIndex++;
                                pntNew->Release();
                                pnt->Release();
                            }
                        }

                        partIndex++;
                    }
                }
                shp->Release();
            }
        }

        shpBase->get_NumPoints(&pntIndex);
        shpBase->get_NumParts(&partIndex);

        if (partIndex >= 0 && pntIndex > 0)
        {
            (*retval)->EditInsertShape(shpBase, &count, &vbretval);
            (*retval)->EditCellValue(newFieldIndex, count, p->first, &vbretval);

            if (calcStats)
            {
                std::vector<int>* indices = indicesMap[p->first];
                fieldMap[count] = indices;
            }
        }

        if (_isEditingShapes) // it was cloned in edit mode only
            shpBase->Release();

        delete p->second; // deleting the vector
        count++;
        ++p;
        i++;
    }

    // ----------------------------------------------
    //   Calculation of stats
    // ----------------------------------------------
    if (calcStats)
    {
        CalculateFieldStats(fieldMap, operations, *retval);

        // delete indices map
        map<CComVariant, vector<int>*>::iterator index = indicesMap.begin();
        while (index != indicesMap.end())
        {
            delete index->second;
            ++index;
        }
    }

    // ----------------------------------------------
    //   Validating output
    // ----------------------------------------------
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
    ValidateOutput(retval, "AggregateShapes");
    return;
}
#pragma endregion

#pragma region Buffer

// ********************************************************************
//		BufferByDistance()
// ********************************************************************
STDMETHODIMP CShapefile::BufferByDistance(double Distance, LONG nSegments, VARIANT_BOOL SelectedOnly,
                                          VARIANT_BOOL MergeResults, IShapefile** sf)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (MergeResults)
    {
        // ShapefileHelper::CloneNoFields(this, sf, SHP_POLYGON, true);
        // -----------------------------------------------
        //	 Creating output
        // -----------------------------------------------
        if (!ShapefileHelper::CloneNoFields(this, sf, SHP_POLYGON, true))
        {
            // Get errorcode and pass the source:
            long errorCode;
            (*sf)->get_LastErrorCode(&errorCode);
            *sf = nullptr;
            ErrorMessage(errorCode);
            return S_OK;
        }
    }
    else
    {
        // if not merging shapes, copy fields
        ShapefileHelper::CloneCore(this, sf, SHP_POLYGON, false);
    }

    if (!BufferByDistanceCore(Distance, nSegments, SelectedOnly, MergeResults, *sf))
    {
        (*sf)->Release();
        *sf = nullptr;
    }

    return S_OK;
}

// ********************************************************************
//		BufferByDistance()
// ********************************************************************
VARIANT_BOOL CShapefile::BufferByDistanceCore(double Distance, LONG nSegments, VARIANT_BOOL SelectedOnly,
                                              VARIANT_BOOL MergeResults, IShapefile* sf)
{
    // -------------------------------------------
    //  validating
    // -------------------------------------------
    if (!ValidateInput(this, "BufferByDistance", "this", SelectedOnly))
        return VARIANT_FALSE;

    // -------------------------------------------
    //  processing
    // -------------------------------------------
    VARIANT_BOOL vb;
    const int size = _shapeData.size();
    long count = 0;
    long percent = 0;

    std::vector<GEOSGeometry*> results;
    results.reserve(size);

    ReadGeosGeometries(SelectedOnly);

    const bool isM = ShapeUtility::IsM(_shpfiletype);

    for (long i = 0; i < size; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, size, "Buffering shapes...", _key, percent);

        if (!ShapeAvailable(i, SelectedOnly))
            continue;

        GEOSGeometry* oGeom1 = this->GetGeosGeometry(i);
        if (oGeom1)
        {
            GEOSGeometry* oGeom2 = GeosHelper::Buffer(oGeom1, Distance, (int)nSegments);

            if (oGeom2 == nullptr) continue;

            if (MergeResults)
            {
                results.push_back(oGeom2);
            }
            else
            {
                vector<IShape*> vShapes;

                if (GeosConverter::GeomToShapes(oGeom2, &vShapes, isM))
                {
                    this->InsertShapesVector(sf, vShapes, this, i, nullptr);
                    count += vShapes.size();
                }
                GeosHelper::DestroyGeometry(oGeom2);
            }
        }
    }

    // -------------------------------------------
    //  merging the results
    // -------------------------------------------
    if (MergeResults)
    {
        GEOSGeometry* gsGeom = GeosConverter::MergeGeometries(results, _globalCallback);
        // geometries will be released in the process

        if (gsGeom != nullptr) // the result should always be in g1
        {
            OGRGeometry* oGeom = GeosHelper::CreateFromGEOS(gsGeom);
            GeosHelper::DestroyGeometry(gsGeom);

            if (oGeom)
            {
                // bool isM = ShapeUtility::IsM(this->_shpfiletype);

                const OGRwkbGeometryType type = oGeom->getGeometryType();
                if (type == wkbMultiPolygon || type == wkbMultiPolygon25D)
                {
                    std::vector<OGRGeometry*> polygons;

                    if (OgrConverter::MultiPolygon2Polygons(oGeom, &polygons))
                    {
                        for (auto& polygon : polygons)
                        {
                            IShape* shp = OgrConverter::GeometryToShape(polygon, isM);
                            if (shp)
                            {
                                sf->EditInsertShape(shp, &count, &vb);
                                shp->Release();
                                count++;
                            }
                        }
                    }
                }
                else
                {
                    // Doesn't use any GEOS functions:
                    IShape* shp = OgrConverter::GeometryToShape(oGeom, isM);
                    if (shp)
                    {
                        sf->EditInsertShape(shp, &count, &vb);
                        shp->Release();
                        count++;
                    }
                }
                OGRGeometryFactory::destroyGeometry(oGeom);
            }
        }
    }

    this->ClearCachedGeometries();

    // -------------------------------------------
    // output validation
    // -------------------------------------------
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
    ValidateOutput(sf, "BufferByDistance");

    return VARIANT_TRUE;
}

#pragma endregion

#pragma region Clipping
// ********************************************************************
//		GetDifference()
// ********************************************************************
STDMETHODIMP CShapefile::Difference(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay,
                                    VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    DoClipOperation(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, retval, clDifference);
    return S_OK;
}

// ********************************************************************
//		Clip()
// ********************************************************************
STDMETHODIMP CShapefile::Clip(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay,
                              IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    DoClipOperation(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, retval, clClip);
    // enumeration should be repaired
    return S_OK;
}

// **********************************************************************
// *	GetIntersection()				             
// **********************************************************************
STDMETHODIMP CShapefile::GetIntersection(VARIANT_BOOL SelectedOnlyOfThis, IShapefile* sf,
                                         VARIANT_BOOL SelectedOnly, ShpfileType fileType, ICallback* cBack,
                                         IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    DoClipOperation(SelectedOnlyOfThis, sf, SelectedOnly, retval, clIntersection, fileType);
    return S_OK;
}

// ********************************************************************
//		GetSymmDifference()
// ********************************************************************
STDMETHODIMP CShapefile::SymmDifference(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay,
                                        VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    DoClipOperation(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, retval, clSymDifference);
    // enumeration should be repaired
    return S_OK;
}

// ********************************************************************
//		GetUnion()
// ********************************************************************
STDMETHODIMP CShapefile::Union(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay,
                               VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    DoClipOperation(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, retval, clUnion);
    return S_OK;
}


// ****************************************************************
//		GetClipOperationReturnType()
// ****************************************************************
// auto choosing the resulting type for intersection
ShpfileType GetClipOperationReturnType(ShpfileType type1, ShpfileType type2, tkClipOperation operation)
{
    switch (operation)
    {
    case clClip:
    case clIntersection:
        {
            const bool isZ = ShapeUtility::IsZ(type1) || ShapeUtility::IsZ(type2);
            const bool isM = ShapeUtility::IsM(type1) || ShapeUtility::IsM(type2);

            const ShpfileType type2D = ShapeUtility::Convert2D(type1);
            const ShpfileType type2D2 = ShapeUtility::Convert2D(type2);

            // return type is always has the lower dimension of two
            if (type2D == SHP_POINT || type2D2 == SHP_POINT)
            {
                return ShapeUtility::Get25DShapeType(SHP_POINT, isZ, isM);
            }
            if (type2D == SHP_MULTIPOINT || type2D2 == SHP_MULTIPOINT)
            {
                return ShapeUtility::Get25DShapeType(SHP_MULTIPOINT, isZ, isM);
            }
            if (type2D == SHP_POLYLINE && type2D2 == SHP_POLYLINE)
            {
                return ShapeUtility::Get25DShapeType(SHP_POINT, isZ, isM);
            }
            if (type2D == SHP_POLYLINE || type2D2 == SHP_POLYLINE)
            {
                return ShapeUtility::Get25DShapeType(SHP_POLYLINE, isZ, isM);
            }
            return ShapeUtility::Get25DShapeType(SHP_POLYGON, isZ, isM);
        }
    case clDifference:
        // the subject type remains intact
        return type1;
    case clSymDifference:
    case clUnion:
        // both types should be the same			
        return type1;
    }

    return type1;
}

// ********************************************************************
//		GetClipOperationName()
// ********************************************************************
CString GetClipOperationName(tkClipOperation operation)
{
    switch (operation)
    {
    case clDifference:
        return "Difference";
    case clIntersection:
        return "GetIntersection";
    case clSymDifference:
        return "SymmDifference";
    case clUnion:
        return "Union";
    case clClip:
    default:
        return "Clip";
    }
}

// ********************************************************************
//		DoClipOperarion()
// ********************************************************************
bool CShapefile::ValidateClippingOutputType(ShpfileType type1, ShpfileType type2, ShpfileType returnType,
                                            tkClipOperation operation)
{
    switch (operation)
    {
    case clSymDifference:
    case clUnion:
        if (type1 != type2)
        {
            CallbackHelper::ErrorMsg(
                "Types of both input shapefiles for symmetrical difference and union operations must be the same.");
            return false;
        }
        break;
    case clDifference:
        if (type1 != returnType)
        {
            CallbackHelper::ErrorMsg(
                "The type of output shapefile must be the same as in put for difference operation.");
            return false;
        }
        break;
    case clIntersection:
    case clClip:
        const bool isM1 = ShapeUtility::IsM(type1);
        const bool isM2 = ShapeUtility::IsM(type2);
        const bool isM = ShapeUtility::IsM(returnType);

        const bool isZ1 = ShapeUtility::IsZ(type1);
        const bool isZ2 = ShapeUtility::IsZ(type2);
        const bool isZ = ShapeUtility::IsZ(returnType);

        if (isM1 != isM && isM2 != isM ||
            isZ1 != isZ && isZ2 != isZ)
        {
            // there is no complete certainty how our conversion routines and GEOS will
            // handle Z, M values, so we let it pass with a warning to see what happens
            CallbackHelper::ErrorMsg("Suspicious output type for clipping operation (Z, M values must be preserved).");
        }

        const ShpfileType type2D = ShapeUtility::Convert2D(type1);
        const ShpfileType type2D2 = ShapeUtility::Convert2D(type2);

        switch (returnType)
        {
        case SHP_POLYGON:
            if (type2D != SHP_POLYGON || type2D2 != SHP_POLYGON)
            {
                ErrorMessage(tkINCOMPATIBLE_SHAPEFILE_TYPE);
                return false;
            }
            break;
        case SHP_POLYLINE:
            if (type2D != SHP_POLYLINE && type2D != SHP_POLYGON ||
                type2D2 != SHP_POLYLINE && type2D2 != SHP_POLYGON)
            {
                ErrorMessage(tkINCOMPATIBLE_SHAPEFILE_TYPE);
                return false;
            }
            break;
        case SHP_POINT:
        case SHP_MULTIPOINT:
            // point can be received from any combination of types (even poly vs poly)
            // not sure how multipoints are handled - so put no limitations for the either
            break;
        case SHP_NULLSHAPE: break;
        case SHP_POINTZ: break;
        case SHP_POLYLINEZ: break;
        case SHP_POLYGONZ: break;
        case SHP_MULTIPOINTZ: break;
        case SHP_POINTM: break;
        case SHP_POLYLINEM: break;
        case SHP_POLYGONM: break;
        case SHP_MULTIPOINTM: break;
        case SHP_MULTIPATCH: break;
        default: ;
        }

        break;
    }

    return true;
}

// ********************************************************************
//		DoClipOperarion()
// ********************************************************************
void CShapefile::DoClipOperation(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay,
                                 VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval,
                                 tkClipOperation operation, ShpfileType returnType)
{
    // ----------------------------------------------
    //   Validation
    // ----------------------------------------------
    if (!sfOverlay)
    {
        ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
        return;
    }

    ShpfileType type2;
    const ShpfileType type1 = _shpfiletype;

    sfOverlay->get_ShapefileType(&type2);
    const bool canUseClipper = type1 == SHP_POLYGON && type2 == SHP_POLYGON;

    if (returnType == SHP_NULLSHAPE)
    {
        returnType = GetClipOperationReturnType(type1, type2, operation);
    }

    if (!ValidateClippingOutputType(type1, type2, returnType, operation))
    {
        return;
    }

    if (!ValidateInput(this, GetClipOperationName(operation), "this", SelectedOnlySubject))
        return;

    if (!ValidateInput(sfOverlay, GetClipOperationName(operation), "sfOverlay", SelectedOnlyOverlay))
    {
        return;
    }

    // ----------------------------------------------
    //   Creating output
    // ----------------------------------------------
    // creation of resulting shapefile
    // ShapefileHelper::CloneNoFields(this, retval, returnType);
    if (!ShapefileHelper::CloneNoFields(this, retval, returnType))
    {
        // Get errorcode and pass the source:
        long errorCode;
        (*retval)->get_LastErrorCode(&errorCode);
        *retval = nullptr;
        ErrorMessage(errorCode);
        return;
    }

    // do field mapping
    std::map<long, long> fieldMap;

    // fields from the overlay shapefile will be copied for the limited number of operation only
    IShapefile* sfCopy = operation == clIntersection || operation == clSymDifference || operation == clUnion
                             ? sfOverlay
                             : nullptr;
    GeoProcessing::CopyFields(this, sfCopy, *retval, fieldMap);

    // long numShapes1 = _shapeData.size();
    long numShapes2;
    sfOverlay->get_NumShapes(&numShapes2);

    const bool useClipper = _geometryEngine == engineClipper && canUseClipper;

    if (_globalCallback)
    {
        _globalCallback->QueryInterface(IID_IStopExecution, (void**)&_stopExecution);
    }

    // building spatial index for the operation
    if (! ((CShapefile*)sfOverlay)->GenerateTempQTree(SelectedOnlyOverlay != 0))
    {
        ErrorMessage(tkFAILED_TO_BUILD_SPATIAL_INDEX);
        goto cleaning;
    }

    if (operation == clSymDifference || operation == clUnion)
    {
        if (! this->GenerateTempQTree(SelectedOnlySubject != 0))
        {
            ErrorMessage(tkFAILED_TO_BUILD_SPATIAL_INDEX);
            goto cleaning;
        }
    }

    if (useClipper)
    {
        // do calculation by Clipper
        switch (operation)
        {
        case clDifference:
            this->DifferenceClipper(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
            break;
        case clIntersection:
            this->IntersectionClipper(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, &fieldMap);
            break;
        case clClip:
            this->ClipClipper(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
            break;
        case clSymDifference:
            this->DifferenceClipper(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
            this->DifferenceClipper(sfOverlay, SelectedOnlyOverlay, this, SelectedOnlySubject, *retval, &fieldMap);
            break;
        case clUnion:
            std::set<int> shapesToSkipSubject;
            std::set<int> shapesToSkipClipping;
            this->IntersectionClipper(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, &fieldMap,
                                      &shapesToSkipSubject, &shapesToSkipClipping);
            this->DifferenceClipper(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, nullptr,
                                    &shapesToSkipSubject);
            this->DifferenceClipper(sfOverlay, SelectedOnlyOverlay, this, SelectedOnlySubject, *retval, &fieldMap,
                                    &shapesToSkipClipping);
        }
    }
    else
    {
        this->ReadGeosGeometries(SelectedOnlySubject);
        ((CShapefile*)sfOverlay)->ReadGeosGeometries(SelectedOnlyOverlay);

        // do calculation by GEOS
        switch (operation)
        {
        case clDifference:
            this->DifferenceGEOS(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
            break;
        case clIntersection:
            this->IntersectionGEOS(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, &fieldMap);
            break;
        case clClip:
            this->ClipGEOS(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
            break;
        case clSymDifference:
            // 2 differences
            this->DifferenceGEOS(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
            this->DifferenceGEOS(sfOverlay, SelectedOnlyOverlay, this, SelectedOnlySubject, *retval, &fieldMap);
            break;
        case clUnion:
            // intersection + symmetrical difference
            std::set<int> shapesToSkipSubject;
            std::set<int> shapesToSkipClipping;
            this->IntersectionGEOS(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, &fieldMap,
                                   &shapesToSkipSubject, &shapesToSkipClipping);
            this->DifferenceGEOS(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, nullptr,
                                 &shapesToSkipSubject);
            this->DifferenceGEOS(sfOverlay, SelectedOnlyOverlay, this, SelectedOnlySubject, *retval, &fieldMap,
                                 &shapesToSkipClipping);
            break;
        }

        this->ClearCachedGeometries();
        sfOverlay->ClearCachedGeometries();
    }

    // -------------------------------------------
    // cleaning
    // -------------------------------------------
cleaning:
    CallbackHelper::ProgressCompleted(_globalCallback, _key);

    // clearing spatial index for the operation
    ((CShapefile*)sfOverlay)->ClearTempQTree();
    if (operation == clSymDifference || operation == clUnion)
    {
        this->ClearTempQTree();
    }

    // -------------------------------------------
    // output validation
    // -------------------------------------------
    ValidateOutput(retval, GetClipOperationName(operation));
}
#pragma endregion

#pragma region Clip
// ********************************************************************
//		ClipGEOS()
// ********************************************************************
void CShapefile::ClipGEOS(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay,
                          IShapefile* sfResult)
{
    QTree* qTree = ((CShapefile*)sfOverlay)->GetTempQTree();

    long numShapesSubject, numShapesClip;
    this->get_NumShapes(&numShapesSubject);
    sfOverlay->get_NumShapes(&numShapesClip);
    const bool isM = ShapeUtility::IsM(_shpfiletype);

    long percent = 0;
    for (long subjectId = 0; subjectId < numShapesSubject; subjectId++)
    {
        CallbackHelper::Progress(_globalCallback, subjectId, numShapesSubject, "Clipping shapes...", _key, percent);

        if (!ShapeAvailable(subjectId, SelectedOnlySubject))
            continue;

        double xMin, xMax, yMin, yMax;
        this->QuickExtentsCore(subjectId, &xMin, &yMin, &xMax, &yMax);
        vector<int> shapeIds = qTree->GetNodes(QTreeExtent(xMin, xMax, yMax, yMin));

        if (!shapeIds.empty())
        {
            GEOSGeometry* gsGeom1 = this->GetGeosGeometry(subjectId);
            if (gsGeom1)
            {
                // iterating through clip geometries preparing their union
                vector<GEOSGeometry*> vUnion;

                for (long clipId : shapeIds)
                {
                    // user can abort the operation in any time
                    if (_stopExecution)
                    {
                        VARIANT_BOOL stop;
                        _stopExecution->StopFunction(&stop);
                        if (stop)
                        {
                            sfResult->EditClear(&stop);
                            //GeosHelper::DestroyGeometry(gsGeom1);
                            goto cleaning;
                        }
                    }

                    // extracting clip geometry
                    if (!((CShapefile*)sfOverlay)->ShapeAvailable(clipId, SelectedOnlyOverlay))
                        continue;

                    GEOSGeometry* gsGeom2 = ((CShapefile*)sfOverlay)->GetGeosGeometry(clipId);

                    if (GeosHelper::Intersects(gsGeom1, gsGeom2))
                    {
                        vUnion.push_back(gsGeom2);
                    }
                }

                // merging (input geometries won't be destroyed)
                GEOSGeometry* gsGeom2 = nullptr;
                bool deleteNeeded = false;

                if ((int)vUnion.size() > 1)
                {
                    gsGeom2 = GeosConverter::MergeGeometries(vUnion, nullptr, false, false);
                    deleteNeeded = true;
                }
                else if ((int)vUnion.size() == 1)
                {
                    gsGeom2 = vUnion[0];
                }

                if (gsGeom2)
                {
                    GEOSGeometry* gsResult = GeosHelper::Intersection(gsGeom1, gsGeom2);
                    if (gsResult != nullptr)
                    {
                        vector<IShape*> vShapes;
                        GeosConverter::GeomToShapes(gsResult, &vShapes, isM);
                        GeosHelper::DestroyGeometry(gsResult);
                        this->InsertShapesVector(sfResult, vShapes, this, subjectId, nullptr);
                    }

                    // clipping geometry should be deleted only in case it was build up from several parts
                    if (deleteNeeded)
                        GeosHelper::DestroyGeometry(gsGeom2);
                }

                // subject geometry should always be deleted
                //GeosHelper::DestroyGeometry(gsGeom1);
            }
        }
    }
cleaning:
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
}

// ********************************************************************
//		ClipClipper()
// ********************************************************************
void CShapefile::ClipClipper(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay,
                             IShapefile* sfResult)
{
    QTree* qTree = ((CShapefile*)sfOverlay)->GetTempQTree();

    long numShapesSubject, numShapesClip;
    this->get_NumShapes(&numShapesSubject);
    sfOverlay->get_NumShapes(&numShapesClip);

    // vector<ClipperLib::Polygons*> vPolygons;	// we shall create vectors for both clipper and GEOS 
    vector<ClipperLib::Paths*> vPolygons; // we shall create vectors for both clipper and GEOS 
    vPolygons.assign(numShapesClip, nullptr); // this won't take much RAM or time

    ClipperLib::Clipper clp;
    ClipperConverter ogr(this);

    long percent = 0;
    for (long subjectId = 0; subjectId < numShapesSubject; subjectId++)
    {
        CallbackHelper::Progress(_globalCallback, subjectId, numShapesSubject, "Clipping shapes...", _key, percent);

        if (!ShapeAvailable(subjectId, SelectedOnlySubject))
            continue;

        double xMin, xMax, yMin, yMax;
        this->QuickExtentsCore(subjectId, &xMin, &yMin, &xMax, &yMax);
        vector<int> shapeIds = qTree->GetNodes(QTreeExtent(xMin, xMax, yMax, yMin));

        if (!shapeIds.empty())
        {
            // extracting subject polygon
            IShape* shp1 = nullptr;
            this->GetValidatedShape(subjectId, &shp1);
            if (!shp1) continue;
            // ClipperLib::Polygons* poly1 = ogr.Shape2ClipperPolygon(shp1);
            ClipperLib::Paths* poly1 = ogr.Shape2ClipperPolygon(shp1);
            shp1->Release();

            if (poly1)
            {
                // extracting clipping polygons
                for (long clipId : shapeIds)
                {
                    // user can abort the operation in any time
                    if (_stopExecution)
                    {
                        VARIANT_BOOL stop;
                        _stopExecution->StopFunction(&stop);
                        if (stop)
                        {
                            sfResult->EditClear(&stop);
                            delete poly1;
                            goto cleaning;
                        }
                    }

                    if (!((CShapefile*)sfOverlay)->ShapeAvailable(clipId, SelectedOnlyOverlay))
                        continue;

                    vector<IShape*> vShapes;

                    // ClipperLib::Polygons* poly2 = nullptr;
                    ClipperLib::Paths* poly2;
                    if (vPolygons[clipId] == nullptr)
                    {
                        IShape* shp2 = nullptr;
                        ((CShapefile*)sfOverlay)->GetValidatedShape(clipId, &shp2);
                        if (!shp2) continue;
                        poly2 = ogr.Shape2ClipperPolygon(shp2);
                        vPolygons[clipId] = poly2;
                        shp2->Release();
                    }
                    else
                    {
                        poly2 = vPolygons[clipId];
                    }

                    if (poly2)
                    {
                        // clp.AddPolygons(*poly2, ClipperLib::ptClip);
                        clp.AddPaths(*poly2, ClipperLib::ptClip, true);
                    }
                }

                // in case there are several input polygons, they should be merged before clipping
                if (shapeIds.size() > 1)
                {
                    // ClipperLib::Polygons polyUnion;
                    ClipperLib::Paths polyUnion;
                    clp.Execute(ClipperLib::ctUnion, polyUnion);
                    clp.Clear();
                    // clp.AddPolygons(polyUnion, ClipperLib::ptClip);
                    clp.AddPaths(polyUnion, ClipperLib::ptClip, true);
                }

                // adding subject polygon
                if (poly1)
                {
                    // clp.AddPolygons(*poly1, ClipperLib::ptSubject);
                    clp.AddPaths(*poly1, ClipperLib::ptSubject, true);
                }

                // do clipping
                // ClipperLib::Polygons polyResult;
                ClipperLib::Paths polyResult;
                if (clp.Execute(ClipperLib::ctIntersection, polyResult))
                {
                    IShape* shp = ogr.ClipperPolygon2Shape(&polyResult);
                    if (shp)
                    {
                        vector<IShape*> vShapes;
                        vShapes.push_back(shp);
                        this->InsertShapesVector(sfResult, vShapes, this, subjectId, nullptr);
                    }
                }

                delete poly1;
                clp.Clear();
            }
        }
    }

cleaning:
    CallbackHelper::ProgressCompleted(_globalCallback, _key);

    for (auto& vPolygon : vPolygons)
    {
        delete vPolygon;
    }
}
#pragma endregion

#pragma region Intersection

// ******************************************************************
//		IntersectionGEOS()
// ******************************************************************
// shapesToExclude - vector to return shapes sum of intersected results for those is equals to the initial area
// such shapes can be excluded from further calculation in case of union; 
void CShapefile::IntersectionGEOS(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL SelectedOnlyClip,
                                  IShapefile* sfResult, map<long, long>* fieldMap,
                                  std::set<int>* subjectShapesToSkip,
                                  std::set<int>* clippingShapesToSkip)
{
    QTree* qTree = ((CShapefile*)sfClip)->GetTempQTree();

    long numShapesSubject, numShapesClip;
    this->get_NumShapes(&numShapesSubject);
    sfClip->get_NumShapes(&numShapesClip);

    const bool isM = ShapeUtility::IsM(_shpfiletype);

    long percent = 0;
    for (long subjectId = 0; subjectId < numShapesSubject; subjectId++)
    {
        CallbackHelper::Progress(_globalCallback, subjectId, numShapesSubject, "Intersecting shapes...", _key, percent);

        if (!ShapeAvailable(subjectId, SelectedOnlySubject))
            continue;

        double xMin, xMax, yMin, yMax;
        this->QuickExtentsCore(subjectId, &xMin, &yMin, &xMax, &yMax);
        vector<int> shapeIds = qTree->GetNodes(QTreeExtent(xMin, xMax, yMax, yMin));

        if (!shapeIds.empty())
        {
            GEOSGeometry* geom1 = GetGeosGeometry(subjectId);

            if (geom1)
            {
                // double sumArea = 0.0;

                // iterating through clip geometries
                for (long clipId : shapeIds)
                {
                    // user can abort the operation at any time
                    if (_stopExecution)
                    {
                        VARIANT_BOOL stop;
                        _stopExecution->StopFunction(&stop);
                        if (stop)
                        {
                            sfResult->EditClear(&stop);
                            //GeosHelper::DestroyGeometry(geom1);
                            goto cleaning;
                        }
                    }

                    // extracting clip geometry
                    if (!((CShapefile*)sfClip)->ShapeAvailable(clipId, SelectedOnlyClip))
                        continue;

                    GEOSGeometry* geom2 = ((CShapefile*)sfClip)->GetGeosGeometry(clipId);

                    // calculating intersection
                    GEOSGeometry* geom = GeosHelper::Intersection(geom1, geom2);
                    // don't delete oGeom1 as it will be used on the next loops
                    if (geom == nullptr) continue;

                    // saving the results
                    vector<IShape*> vShapes;
                    GeosConverter::GeomToShapes(geom, &vShapes, isM);
                    GeosHelper::DestroyGeometry(geom);

                    this->InsertShapesVector(sfResult, vShapes, this, subjectId, nullptr, sfClip, clipId, fieldMap);
                    // shapes are released here
                }
                //GeosHelper::DestroyGeometry(geom1);
            }
        }
    }
cleaning:
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
}

// ******************************************************************
//		IntersectionClipperNoAttributes()
// ******************************************************************
IShapefile* CShapefile::IntersectionClipperNoAttributes(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip,
                                                        VARIANT_BOOL SelectedOnlyClip)
{
    if (!sfClip)
        return nullptr;

    IShapefile* sfResult = nullptr;
    this->Clone(&sfResult);

    ClipperLib::Clipper clp;
    ClipperConverter::AddPolygons(this, clp, ClipperLib::PolyType::ptSubject, SelectedOnlySubject == VARIANT_TRUE);
    ClipperConverter::AddPolygons(sfClip, clp, ClipperLib::PolyType::ptClip, SelectedOnlyClip == VARIANT_TRUE);

    // ClipperLib::Polygons polyResult;
    ClipperLib::Paths polyResult;
    if (clp.Execute(ClipperLib::ClipType::ctIntersection, polyResult))
    {
        ClipperConverter converter(sfResult);
        IShape* shp = converter.ClipperPolygon2Shape(&polyResult);

        VARIANT_BOOL vb;
        if (shp)
        {
            long index = 0;
            sfResult->EditInsertShape(shp, &index, &vb);
            shp->Release();
        }
    }
    return sfResult;
}

// ******************************************************************
//		IntersectionClipper()
// ******************************************************************
void CShapefile::IntersectionClipper(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip,
                                     VARIANT_BOOL SelectedOnlyClip,
                                     IShapefile* sfResult, map<long, long>* fieldMap,
                                     std::set<int>* subjectShapesToSkip,
                                     std::set<int>* clippingShapesToSkip)
{
    QTree* qTree = ((CShapefile*)sfClip)->GetTempQTree();

    long numShapesSubject, numShapesClip;
    this->get_NumShapes(&numShapesSubject);
    sfClip->get_NumShapes(&numShapesClip);

    // vector<ClipperLib::Polygons*> vPolygons;	// we shall create vectors for both clipper and GEOS 
    vector<ClipperLib::Paths*> vPolygons; // we shall create vectors for both clipper and GEOS 
    vPolygons.assign(numShapesClip, nullptr); // this won't take much RAM or time

    ClipperLib::Clipper clp;
    ClipperConverter converter(this);

    VARIANT_BOOL isGeographic = VARIANT_FALSE;
    sfResult->get_IsGeographicProjection(&isGeographic);

    const double AREA_TOLERANCE = m_globalSettings.GetMinPolygonArea(isGeographic) * 0.001;

    // initial areas areas of the clipping shapes
    std::vector<double> initClipAreas;
    // areas of the clipping shapes that were passed to the result:  (int)index of shapes -> (double)area
    std::vector<double> resultClipAreas;

    ShpfileType shpType;
    sfClip->get_ShapefileType(&shpType);

    const bool buildSkipLists = subjectShapesToSkip != nullptr && clippingShapesToSkip != nullptr && m_globalSettings.
        shapefileFastUnion &&
        ShapeUtility::Convert2D(_shpfiletype) == SHP_POLYGON && ShapeUtility::Convert2D(shpType) == SHP_POLYGON;

    if (buildSkipLists)
    {
        initClipAreas.resize(numShapesClip, 0.0);
        resultClipAreas.resize(numShapesClip, 0.0);
    }

    long percent = 0;
    for (long subjectId = 0; subjectId < numShapesSubject; subjectId++)
    {
        CallbackHelper::Progress(_globalCallback, subjectId, numShapesSubject, "Intersecting shapes...", _key, percent);

        if (!this->ShapeAvailable(subjectId, SelectedOnlySubject))
            continue;

        double xMin, xMax, yMin, yMax;
        this->QuickExtentsCore(subjectId, &xMin, &yMin, &xMax, &yMax);
        vector<int> shapeIds = qTree->GetNodes(QTreeExtent(xMin, xMax, yMax, yMin));

        if (!shapeIds.empty())
        {
            // extracting subject polygon
            IShape* shp1 = nullptr;
            this->GetValidatedShape(subjectId, &shp1);
            if (!shp1) continue;
            // ClipperLib::Polygons* poly1 = converter.Shape2ClipperPolygon(shp1);
            ClipperLib::Paths* poly1 = converter.Shape2ClipperPolygon(shp1);

            double initArea = 0.0;
            shp1->get_Area(&initArea);
            shp1->Release();

            if (poly1)
            {
                double sumArea = 0.0;

                // extracting clipping polygons
                for (long clipId : shapeIds)
                {
                    // user can abort the operation in any time
                    if (_stopExecution)
                    {
                        VARIANT_BOOL stop;
                        _stopExecution->StopFunction(&stop);
                        if (stop)
                        {
                            sfResult->EditClear(&stop);
                            delete poly1;
                            goto cleaning;
                        }
                    }

                    if (!((CShapefile*)sfClip)->ShapeAvailable(clipId, SelectedOnlyClip))
                        continue;

                    // vector<IShape*> vShapes;

                    // Processing with Clipper
                    // ClipperLib::Polygons* poly2 = nullptr;
                    ClipperLib::Paths* poly2;

                    if (vPolygons[clipId] == nullptr)
                    {
                        IShape* shp2 = nullptr;
                        ((CShapefile*)sfClip)->GetValidatedShape(clipId, &shp2);
                        if (!shp2) continue;
                        poly2 = converter.Shape2ClipperPolygon(shp2);
                        vPolygons[clipId] = poly2;

                        if (buildSkipLists)
                        {
                            double tempArea = 0.0;
                            shp2->get_Area(&tempArea);
                            initClipAreas[clipId] = tempArea;
                        }

                        shp2->Release();
                    }
                    else
                    {
                        poly2 = vPolygons[clipId];
                    }

                    if (poly2)
                    {
                        if (poly1)
                        {
                            // clp.AddPolygons(*poly1, ClipperLib::ptSubject);
                            clp.AddPaths(*poly1, ClipperLib::ptSubject, true);
                        }
                        if (poly2)
                        {
                            // clp.AddPolygons(*poly2, ClipperLib::ptClip);
                            clp.AddPaths(*poly2, ClipperLib::ptClip, true);
                        }

                        // do clipping
                        // ClipperLib::Polygons polyResult;
                        ClipperLib::Paths polyResult;
                        if (clp.Execute(ClipperLib::ctIntersection, polyResult))
                        {
                            IShape* shp = converter.ClipperPolygon2Shape(&polyResult);
                            if (shp)
                            {
                                // sum of area to exclude shapes from difference
                                if (buildSkipLists)
                                {
                                    double areaTemp;
                                    shp->get_Area(&areaTemp);

                                    // subject shape
                                    sumArea += areaTemp;
                                    resultClipAreas[clipId] += areaTemp;
                                }

                                vector<IShape*> vShapes;
                                vShapes.push_back(shp);
                                this->InsertShapesVector(sfResult, vShapes, this, subjectId, nullptr, sfClip, clipId,
                                                         fieldMap);
                            }
                        }
                        clp.Clear();
                    }
                }
                delete poly1;

                if (buildSkipLists)
                {
                    if (fabs(sumArea - initArea) < AREA_TOLERANCE)
                    {
                        subjectShapesToSkip->insert(subjectId);
                    }
                }
            }
        }
    }

    // building the list of clipping shapes to skip
    if (buildSkipLists)
    {
        for (int i = 0; i < numShapesClip; i++)
        {
            if (fabs(initClipAreas[i] - resultClipAreas[i]) < AREA_TOLERANCE && initClipAreas[i] != 0.0)
            {
                clippingShapesToSkip->insert(i);
            }
        }
    }

cleaning:
    CallbackHelper::ProgressCompleted(_globalCallback, _key);

    for (auto& vPolygon : vPolygons)
    {
        delete vPolygon;
    }
}
#pragma endregion

#pragma region Difference
// ********************************************************************
//		DifferenceGEOS()
// ********************************************************************
void CShapefile::DifferenceGEOS(IShapefile* sfSubject, VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay,
                                VARIANT_BOOL SelectedOnlyOverlay,
                                IShapefile* sfResult, map<long, long>* fieldMap, set<int>* shapesToSkip)
{
    QTree* qTree = ((CShapefile*)sfOverlay)->GetTempQTree();

    long numShapesSubject, numShapesClip;
    sfSubject->get_NumShapes(&numShapesSubject);
    sfOverlay->get_NumShapes(&numShapesClip);

    const bool isM = ShapeUtility::IsM(_shpfiletype);

    long percent = 0;
    for (long subjectId = 0; subjectId < numShapesSubject; subjectId++)
    {
        CallbackHelper::Progress(_globalCallback, subjectId, numShapesSubject, "Calculating difference...", _key,
                                 percent);

        if (!((CShapefile*)sfSubject)->ShapeAvailable(subjectId, SelectedOnlySubject))
            continue;

        // those shapes are marked to skip in the course of intersection
        if (shapesToSkip != nullptr)
        {
            if (shapesToSkip->find(subjectId) != shapesToSkip->end())
                continue;
        }

        double xMin, xMax, yMin, yMax;
        ((CShapefile*)sfSubject)->QuickExtentsCore(subjectId, &xMin, &yMin, &xMax, &yMax);
        vector<int> shapeIds = qTree->GetNodes(QTreeExtent(xMin, xMax, yMax, yMin));

        if (!shapeIds.empty())
        {
            GEOSGeometry* gsGeom1 = ((CShapefile*)sfSubject)->GetGeosGeometry(subjectId);
            if (!gsGeom1) continue;

            vector<GEOSGeometry*> vClip;

            // iterating through clip geometries, if the subject will stand this, we add it to the result
            for (long clipId : shapeIds)
            {
                // user can abort the operation in any time
                if (_stopExecution)
                {
                    VARIANT_BOOL stop;
                    _stopExecution->StopFunction(&stop);
                    if (stop)
                    {
                        sfResult->EditClear(&stop);
                        //GeosHelper::DestroyGeometry(gsGeom1);
                        goto cleaning;
                    }
                }

                // extracting clip geometry
                if (!((CShapefile*)sfOverlay)->ShapeAvailable(clipId, SelectedOnlyOverlay))
                    continue;

                GEOSGeometry* gsGeom2 = ((CShapefile*)sfOverlay)->GetGeosGeometry(clipId);

                if (gsGeom2 && GeosHelper::Intersects(gsGeom1, gsGeom2))
                {
                    vClip.push_back(gsGeom2);
                }
            }

            GEOSGeometry* gsClip = nullptr;
            if (vClip.size() == 1)
            {
                gsClip = vClip[0];
            }
            else if (vClip.size() > 1)
            {
                // union of the clipping shapes
                gsClip = GeosConverter::MergeGeometries(vClip, nullptr, false, false);
            }

            bool deleteNeeded = false;
            if (gsClip)
            {
                gsGeom1 = GeosHelper::Difference(gsGeom1, gsClip);
                deleteNeeded = true;

                // if clip geometry was merged, we should delete it
                if (vClip.size() > 1)
                {
                    GeosHelper::DestroyGeometry(gsClip);
                }
            }

            // saving what was left from the subject
            if (gsGeom1 != nullptr)
            {
                vector<IShape*> vShapes;
                GeosConverter::GeomToShapes(gsGeom1, &vShapes, isM);
                if (deleteNeeded)
                    GeosHelper::DestroyGeometry(gsGeom1);
                this->InsertShapesVector(sfResult, vShapes, sfSubject, subjectId, fieldMap); // shapes are released here
            }
        }
        else
        {
            // insert the shape directly no other shapes intersects it
            // TODO: it makes sense to rewrite it in more efficient way
            IShape* shp1 = nullptr;
            vector<IShape*> vShapes;
            ((CShapefile*)sfSubject)->GetValidatedShape(subjectId, &shp1);
            if (shp1)
            {
                vShapes.push_back(shp1);
                this->InsertShapesVector(sfResult, vShapes, sfSubject, subjectId, fieldMap); // shapes are released here
            }
        }
    }

cleaning:
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
}

#ifdef SERIALIZE_POLYGONS
void SerializePolygon(ofstream& out, ClipperLib::Polygons* poly)
{
	if (poly && out.good())
	{
		out.precision(14);
		out << "Polygon" << endl;
		int part = 0;
		for (unsigned int i = 0; i < poly->size(); i++)
		{
			ClipperLib::Polygon* p = &((*poly)[i]);
			if (p)
			{
				part++;
				for (unsigned int j = 0; j < p->size(); j++)
				{
					out << (*p)[j].X << ", " << (*p)[j].Y;
					if (j != p->size() - 1)
					{
						out << ",";
					}
					out << endl;
				}
			}
		}
		out << endl;
	}
}
#endif

// ******************************************************************
//		DifferenceClipper()
// ******************************************************************
void CShapefile::DifferenceClipper(IShapefile* sfSubject, VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip,
                                   VARIANT_BOOL SelectedOnlyClip,
                                   IShapefile* sfResult, map<long, long>* fieldMap, set<int>* shapesToSkip)
{
    QTree* qTree = ((CShapefile*)sfClip)->GetTempQTree();

    long numShapesSubject, numShapesClip;
    sfSubject->get_NumShapes(&numShapesSubject);
    sfClip->get_NumShapes(&numShapesClip);

    // vector<ClipperLib::Polygons*> vPolygons;	// we shall create vectors for both clipper and GEOS 
    vector<ClipperLib::Paths*> vPolygons; // we shall create vectors for both clipper and GEOS 
    vPolygons.assign(numShapesClip, nullptr); // this won't take much RAM or time

    ClipperConverter ogr(sfSubject);

    long percent = 0;
    for (long subjectId = 0; subjectId < numShapesSubject; subjectId++)
    {
        CallbackHelper::Progress(_globalCallback, subjectId, numShapesSubject, "Calculating difference...", _key,
                                 percent);

        if (!((CShapefile*)sfSubject)->ShapeAvailable(subjectId, SelectedOnlySubject))
            continue;

        if (shapesToSkip != nullptr)
        {
            if (shapesToSkip->find(subjectId) != shapesToSkip->end())
                continue;
        }

        double xMin, xMax, yMin, yMax;
        ((CShapefile*)sfSubject)->QuickExtentsCore(subjectId, &xMin, &yMin, &xMax, &yMax);
        vector<int> shapeIds = qTree->GetNodes(QTreeExtent(xMin, xMax, yMax, yMin));

        if (!shapeIds.empty())
        {
            // extracting subject polygon
            IShape* shp1 = nullptr;
            ((CShapefile*)sfSubject)->GetValidatedShape(subjectId, &shp1);
            if (!shp1) continue;
            // ClipperLib::Polygons* poly1 = ogr.Shape2ClipperPolygon(shp1);
            ClipperLib::Paths* poly1 = ogr.Shape2ClipperPolygon(shp1);
            shp1->Release();

            if (poly1)
            {
                auto* clp = new ClipperLib::Clipper;

                // extracting clipping polygons
                // ClipperLib::Polygons* poly2 = nullptr;
                ClipperLib::Paths* poly2;
                for (long clipId : shapeIds)
                {
                    // user can abort the operation in any time
                    if (_stopExecution)
                    {
                        VARIANT_BOOL stop;
                        _stopExecution->StopFunction(&stop);
                        if (stop)
                        {
                            sfResult->EditClear(&stop);
                            delete clp;
                            goto cleaning;
                        }
                    }

                    if (!((CShapefile*)sfClip)->ShapeAvailable(clipId, SelectedOnlyClip))
                        continue;

                    vector<IShape*> vShapes;

                    // processng with Clipper
                    if (vPolygons[clipId] == nullptr)
                    {
                        IShape* shp2 = nullptr;
                        ((CShapefile*)sfClip)->GetValidatedShape(clipId, &shp2);
                        if (!shp2) continue;
                        poly2 = ogr.Shape2ClipperPolygon(shp2);
                        vPolygons[clipId] = poly2;
                        shp2->Release();
                    }
                    else
                    {
                        poly2 = vPolygons[clipId];
                    }

                    if (poly2)
                    {
                        // clp->AddPolygons(*poly2, ClipperLib::ptClip);
                        clp->AddPaths(*poly2, ClipperLib::ptClip, true);
                    }

#ifdef SERIALIZE_POLYGONS
					SerializePolygon(out, poly2);
#endif
                }

                if (poly1)
                {
                    // clp->AddPolygons(*poly1, ClipperLib::ptSubject);
                    clp->AddPaths(*poly1, ClipperLib::ptSubject, true);
                }

#ifdef SERIALIZE_POLYGONS
				SerializePolygon(out, poly1);
#endif

                // do clipping
                // ClipperLib::Polygons polyResult;
                ClipperLib::Paths polyResult;
                if (clp->Execute(ClipperLib::ctDifference, polyResult))
                {
                    IShape* shp = ogr.ClipperPolygon2Shape(&polyResult);
                    if (shp)
                    {
                        vector<IShape*> vShapes;
                        vShapes.push_back(shp);
                        this->InsertShapesVector(sfResult, vShapes, sfSubject, subjectId, fieldMap);
                    }
                }

                delete poly1;
                clp->Clear();
                delete clp;
            }
        }
        else
        {
            // insert the shape directly no other shapes intersects it
            // TODO: it makes sense to rewrite it in more efficient way
            IShape* shp1 = nullptr;
            vector<IShape*> vShapes;
            ((CShapefile*)sfSubject)->GetValidatedShape(subjectId, &shp1);
            if (shp1)
            {
                VARIANT_BOOL editingShapes;
                sfSubject->get_EditingShapes(&editingShapes);
                IShape* shpCopy = nullptr;
                if (editingShapes)
                {
                    shp1->Clone(&shpCopy);
                    shp1->Release();
                }
                else
                {
                    shpCopy = shp1;
                }
                vShapes.push_back(shpCopy);
                this->InsertShapesVector(sfResult, vShapes, sfSubject, subjectId, fieldMap); // shapes are released here
            }
        }
    }

cleaning:
    CallbackHelper::ProgressCompleted(_globalCallback, _key);

    for (auto& vPolygon : vPolygons)
    {
        delete vPolygon;
    }
}
#pragma endregion

#pragma region Properties
// ********************************************************************
//		GeometryEngine()
// ********************************************************************
STDMETHODIMP CShapefile::get_GeometryEngine(tkGeometryEngine* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _geometryEngine;
    return S_OK;
}

STDMETHODIMP CShapefile::put_GeometryEngine(tkGeometryEngine newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _geometryEngine = newVal;
    return S_OK;
}

#pragma endregion

#pragma region PointInPolygon


// ********************************************************************
//		PointInShape()
// ********************************************************************
STDMETHODIMP CShapefile::PointInShape(LONG ShapeIndex, DOUBLE x, DOUBLE y, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
    {
        *retval = VARIANT_FALSE;
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
        return S_OK;
    }

    long numParts, numPoints;
    long* Parts;
    Point2D* Points;

    if (_isEditingShapes)
    {
        if (_shpfiletype != SHP_POLYGON && _shpfiletype != SHP_POLYGONM && _shpfiletype != SHP_POLYGONZ)
        {
            *retval = VARIANT_FALSE;
            return S_OK;
        }

        double xMin, yMin, xMax, yMax;
        this->QuickExtentsCore(ShapeIndex, &xMin, &yMin, &xMax, &yMax);

        if (x < xMin || y < yMin || x > xMax || y > yMax)
        {
            *retval = VARIANT_FALSE;
            return S_OK;
        }

        // retrieving shapes from memory
        IShape* shp = _shapeData[ShapeIndex]->shape;
        shp->get_NumParts(&numParts);
        shp->get_NumPoints(&numPoints);
        Parts = new long[numParts + 1];
        Points = new Point2D[numPoints];

        for (int nPart = numParts - 1; nPart >= 0; nPart--)
            shp->get_Part(nPart, &Parts[nPart]);

        VARIANT_BOOL vbretval;
        for (int nPoint = numPoints - 1; nPoint >= 0; nPoint--)
        {
            shp->get_XY(nPoint, &Points[nPoint].x, &Points[nPoint].y, &vbretval);
        }
        Parts[numParts] = numPoints;
    }
    else
    {
        CSingleLock lock(&_readLock, TRUE);

        int shpType;
        fseek(_shpfile, _shpOffsets[ShapeIndex] + sizeof(int) * 2, SEEK_SET);
        fread(&shpType, sizeof(int), 1, _shpfile);

        shpType = ShapeUtility::Convert2D((ShpfileType)shpType);
        if (shpType != SHP_POLYGON)
        {
            *retval = VARIANT_FALSE;
            return S_OK;
        }

        ShapeHeader shpHeader{};
        fread(&shpHeader, sizeof(ShapeHeader), 1, _shpfile);

        // check the bounds
        if (x < shpHeader.MinX || y < shpHeader.MinY || x > shpHeader.MaxX || y > shpHeader.MaxY)
        {
            *retval = VARIANT_FALSE;
            return S_OK;
        }

        Parts = new long[shpHeader.NumParts + 1];
        Points = new Point2D[shpHeader.NumPoints];

        fread(Parts, sizeof(int), shpHeader.NumParts, _shpfile);
        fread(Points, sizeof(Point2D), shpHeader.NumPoints, _shpfile);
        Parts[shpHeader.NumParts] = shpHeader.NumPoints;
        numParts = shpHeader.NumParts;
    }

    int CrossCount = 0;
    // work backwards through Parts since we only know the first point of each Part
    for (int nPart = numParts - 1; nPart >= 0; nPart--)
    {
        const int nPointMin = Parts[nPart];
        const int nPointMax = Parts[nPart + 1] - 1;
        int i, j;
        // algorithm by W. Randolph Franklin; for a detailed explanation, see
        // https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
        // note that i starts at first point, while j starts at last point, then follows behind i
        for (i = nPointMin, j = nPointMax - 1; i < nPointMax; j = i++)
        {
            if ( ((Points[i].y > y) != (Points[j].y > y)) &&
                 (x < (Points[j].x - Points[i].x) * (y - Points[i].y) / (Points[j].y - Points[i].y) + Points[i].x) )
                // we have a crossing
                CrossCount++;
        }

        //for (int nPoint = Parts[nPart]; nPoint < nPointMax; nPoint++)
        //{
        //    const double x1 = Points[nPoint].x - x;
        //    const double y1 = Points[nPoint].y - y;
        //    const double x2 = Points[nPoint + 1].x - x;
        //    const double y2 = Points[nPoint + 1].y - y;

        //    const double y1y2 = y1 * y2;
        //    if (y1y2 > 0.0) // If the signs are the same
        //    {
        //        // Then it does not cross
        //        continue;
        //    }
        //    if (y1y2 == 0.0) // Then it has intesected a vertex
        //    {
        //        if (y1 == 0.0)
        //        {
        //            if (y2 > 0.0)
        //                continue;
        //        }
        //        else if (y1 > 0.0)
        //            continue;
        //    }

        //    if (x1 > 0.0 && x2 > 0.0)
        //    {
        //        CrossCount++;
        //        continue;
        //    }

        //    // Calculate Intersection
        //    const double dy = y2 - y1;
        //    const double dx = x2 - x1;

        //    // CDM March 2008 - if dy is zero (horiz line), this will be a bad idea...
        //    if (dy != 0)
        //    {
        //        if (x1 - y1 * (dx / dy) > 0.0)
        //            CrossCount++;
        //    }
        //}
    }
    delete [] Points;
    delete [] Parts;

    *retval = CrossCount & 1 ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
}
#pragma endregion

#pragma region PointInShapefile
// ********************************************************************
//		PointInShapefile()
// ********************************************************************
STDMETHODIMP CShapefile::PointInShapefile(DOUBLE x, DOUBLE y, LONG* ShapeIndex)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    const int nShapeCount = _polySf.size();
    for (int nShape = nShapeCount - 1; nShape >= 0; nShape--)
        // for(int nShape = 0; nShape < ShapeCount; nShape++) see http://www.mapwindow.org/phorum/read.php?3,9745,9950#msg-9950
    {
        PolygonShapefile* sf = &_polySf[nShape];

        if (x < sf->shpHeader.MinX || y < sf->shpHeader.MinY || x > sf->shpHeader.MaxX || y > sf->shpHeader.MaxY)
        {
            continue;
        }

        int CrossCount = 0;

        int i, j;
        // work backwards through Parts since we only know the first point of each Part
        for (int nPart = sf->shpHeader.NumParts - 1; nPart >= 0; nPart--)
        {
            const int nPointMin = sf->Parts[nPart];
            const int nPointMax = sf->Parts[nPart + 1] - 1;
            // algorithm by W. Randolph Franklin; for a detailed explanation, see
            // https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
            // note that i starts at first point, while j starts at last point, then follows behind i
            for (i = nPointMin, j = nPointMax - 1; i < nPointMax; j = i++)
            {
                if (((sf->Points[i].y > y) != (sf->Points[j].y > y)) &&
                    (x < (sf->Points[j].x - sf->Points[i].x) * (y - sf->Points[i].y) / (sf->Points[j].y - sf->Points[i].y) + sf->Points[i].x))
                    // we have a crossing
                    CrossCount++;
            }
        }
        //for (int nPart = sf->shpHeader.NumParts - 1; nPart >= 0; nPart--)
        //{
        //    const int nPointMax = sf->Parts[nPart + 1] - 1;
        //    for (int nPoint = sf->Parts[nPart]; nPoint < nPointMax; nPoint++)
        //    {
        //        const double x1 = sf->Points[nPoint].x - x;
        //        const double y1 = sf->Points[nPoint].y - y;
        //        const double x2 = sf->Points[nPoint + 1].x - x;
        //        const double y2 = sf->Points[nPoint + 1].y - y;

        //        const double y1y2 = y1 * y2;

        //        if (y1y2 > 0.0) // If the signs are the same
        //        {
        //            // Then it does not cross
        //            continue;
        //        }
        //        if (y1y2 == 0.0) // Then it has intesected a vertex
        //        {
        //            if (y1 == 0.0)
        //            {
        //                if (y2 > 0.0)
        //                    continue;
        //            }
        //            else if (y1 > 0.0)
        //                continue;
        //        }

        //        if (x1 > 0.0 && x2 > 0.0)
        //        {
        //            CrossCount++;
        //            continue;
        //        }

        //        // Calculate Intersection
        //        const double dy = y2 - y1;
        //        const double dx = x2 - x1;
        //        const double xint = x1 - y1 * (dx / dy);

        //        if (xint > 0.0)
        //            CrossCount++;
        //    }
        //}

        if (CrossCount & 1)
        {
            *ShapeIndex = nShape;
            return S_OK;
        }
    }

    *ShapeIndex = -1;
    return S_OK;
}

// ********************************************************************
//		BeginPointInShapefile()
// ********************************************************************
STDMETHODIMP CShapefile::BeginPointInShapefile(VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (_writing)
    {
        //AfxMessageBox("Can't read");
        *retval = VARIANT_FALSE;
        return S_OK;
    }

    // allow all polygon variations
    if (_shpfiletype != SHP_POLYGON && _shpfiletype != SHP_POLYGONM && _shpfiletype != SHP_POLYGONZ)
    {
        *retval = VARIANT_FALSE;
        ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
        return S_OK;
    }

    CSingleLock lock(&_readLock, TRUE);

    const int size = _shapeData.size();
    _polySf.resize(size);

    for (int nShape = 0; nShape < size; nShape++)
    {
        fseek(_shpfile, _shpOffsets[nShape] + sizeof(int) * 2, SEEK_SET);
        int shpType;
        fread(&shpType, sizeof(int), 1, _shpfile);
        if (shpType != SHP_POLYGON && shpType != SHP_POLYGONM && shpType != SHP_POLYGONZ)
        {
            *retval = VARIANT_FALSE;
            ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
            return S_OK;
        }

        PolygonShapefile& sf = _polySf[nShape];
        fread(&sf.shpHeader, sizeof(ShapeHeader), 1, _shpfile);

        if (sf.shpHeader.NumPoints > 0 && sf.shpHeader.NumParts > 0)
        {
            sf.Points.resize(sf.shpHeader.NumPoints);
            sf.Parts.resize(sf.shpHeader.NumParts + 1);
            fread(&sf.Parts[0], sizeof(int), sf.shpHeader.NumParts, _shpfile);
            fread(&sf.Points[0], sizeof(Point2D), sf.shpHeader.NumPoints, _shpfile);
            sf.Parts[sf.shpHeader.NumParts] = sf.shpHeader.NumPoints;
            *retval = VARIANT_TRUE;
        }
        else
        {
            // this isn't a valid shape, we simply won't process it
            ErrorMessage(tkINVALID_SHAPE);
            *retval = VARIANT_FALSE;
        }
    }
    return S_OK;
}

// ********************************************************************
//		EndPointInShapefile()
// ********************************************************************
STDMETHODIMP CShapefile::EndPointInShapefile()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    _polySf.clear();

    return S_OK;
}
#pragma endregion

#pragma region Non-spatial

// ********************************************************************
//		ExplodeShapesCore()
// ********************************************************************
VARIANT_BOOL CShapefile::ExplodeShapesCore(VARIANT_BOOL SelectedOnly, IShapefile* result)
{
    // ----------------------------------------------
    //   Validation
    // ----------------------------------------------
    if (!ValidateInput(this, "ExplodeShapes", "this", SelectedOnly))
    {
        return VARIANT_FALSE;
    }

    // ----------------------------------------------
    //   Processing
    // ----------------------------------------------
    VARIANT_BOOL vb;
    long count;
    CComVariant var;
    std::vector<IShape*> vShapes;
    long percent = 0;

    LONG numShapes;
    this->get_NumShapes(&numShapes);

    LONG numFields;
    this->get_NumFields(&numFields);

    for (long i = 0; i < numShapes; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, numShapes, "Exploding...", _key, percent);

        if (!ShapeAvailable(i, SelectedOnly))
            continue;

        IShape* shp = nullptr;
        GetValidatedShape(i, &shp);
        if (!shp) continue;

        if (((CShape*)shp)->ExplodeCore(vShapes))
        {
            for (auto& vShape : vShapes)
            {
                // all the shapes are copies of the initial ones, so no further cloning is needed					
                result->get_NumShapes(&count);
                result->EditInsertShape(vShape, &count, &vb);

                if (vb)
                {
                    // copy attributes
                    for (int iFld = 0; iFld < numFields; iFld++)
                    {
                        this->get_CellValue(iFld, i, &var);
                        result->EditCellValue(iFld, count, var, &vb);
                    }
                }
                vShape->Release(); // reference was added in EditInsertShape
            }
        }

        shp->Release();
    }

    // ----------------------------------------------
    //   Output validation
    // ----------------------------------------------
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
    ValidateOutput(result, "ExplodeShapes");

    return VARIANT_TRUE;
}

// ********************************************************************
//		ExplodeShapes()
// ********************************************************************
STDMETHODIMP CShapefile::ExplodeShapes(VARIANT_BOOL SelectedOnly, IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Clone(retval);

    if (!ExplodeShapesCore(SelectedOnly, *retval))
    {
        (*retval)->Release();
        *retval = nullptr;
    }

    return S_OK;
}

// ********************************************************************
//		ExportSelectionCore()
// ********************************************************************
VARIANT_BOOL CShapefile::ExportSelectionCore(IShapefile* result)
{
    // ----------------------------------------------
    //   Validation
    // ----------------------------------------------
    if (!ValidateInput(this, "Sort", "this", false))
    {
        return VARIANT_FALSE;
    }

    // ----------------------------------------------
    //   Processing
    // ----------------------------------------------
    LONG numFields;
    this->get_NumFields(&numFields);

    VARIANT_BOOL vbretval;

    const long numShapes = _shapeData.size();

    long count = 0;
    CComVariant var;

    long percent = 0;

    for (long i = 0; i < numShapes; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, numShapes, "Exporting...", _key, percent);

        if (!_shapeData[i]->selected())
            continue;

        IShape* shp = nullptr;
        this->GetValidatedShape(i, &shp);
        if (!shp) continue;

        if (_isEditingShapes)
        {
            // in the editing mode we share a shape with the parent shapefile
            // a copy is needed to avoid conflicts
            IShape* shpCopy = nullptr;
            shp->Clone(&shpCopy);
            result->EditInsertShape(shpCopy, &count, &vbretval);
            shpCopy->Release();
        }
        else
        {
            result->EditInsertShape(shp, &count, &vbretval);
        }

        if (vbretval)
        {
            // copy attributes
            for (int iFld = 0; iFld < numFields; iFld++)
            {
                this->get_CellValue(iFld, i, &var);
                result->EditCellValue(iFld, count, var, &vbretval);
            }
            count++;
        }
        shp->Release();
    }

    // ----------------------------------------------
    //   Validating output
    // ----------------------------------------------
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
    ValidateOutput(result, "ExportSelection");

    return VARIANT_TRUE;
}

// ********************************************************************
//		ExportSelection()
// ********************************************************************
STDMETHODIMP CShapefile::ExportSelection(IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Clone(retval);

    if (!ExportSelectionCore(*retval))
    {
        (*retval)->Release();
        *retval = nullptr;
    }

    return S_OK;
}

// ********************************************************************
//		Sort()
// ********************************************************************
STDMETHODIMP CShapefile::Sort(LONG FieldIndex, VARIANT_BOOL Ascending, IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    USES_CONVERSION;

    // ----------------------------------------------
    //   Validation
    // ----------------------------------------------
    if (!ValidateInput(this, "Sort", "this", false))
        return S_OK;

    // ----------------------------------------------
    //   Creating output
    // ----------------------------------------------
    this->Clone(retval);

    // ----------------------------------------------
    //   Processing
    // ----------------------------------------------
    LONG numFields;
    this->get_NumFields(&numFields);

    const long numShapes = _shapeData.size();
    multimap<CComVariant, IShape*> shapeMap;
    CComVariant val;

    // processing shapes
    long percent = 0;
    for (long i = 0; i < numShapes; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, numShapes, "Sorting...", _key, percent);

        IShape* shp = nullptr;
        this->GetValidatedShape(i, &shp);
        if (shp)
        {
            // marking the index of shape
            CString str;
            str.Format("%dl", i);
            const CComBSTR bstr(str);
            shp->put_Key(bstr);

            this->get_CellValue(FieldIndex, i, &val);
            pair<CComVariant, IShape*> myPair(val, shp);
            shapeMap.insert(myPair);
        }
    }

    const long count = 0;

    // -------------------------------------------
    // writing the results
    // -------------------------------------------
    if (Ascending)
    {
	    multimap<CComVariant, IShape*>::iterator p = shapeMap.begin();

        while (p != shapeMap.end())
        {
            CallbackHelper::Progress(_globalCallback, count, numShapes, "Writing...", _key, percent);

            IShape* shp = p->second;
            CopyShape(this, shp, *retval);
            ++p;
        }
    }
    else
    {
	    multimap<CComVariant, IShape*>::reverse_iterator p = shapeMap.rbegin();
        while (p != shapeMap.rend())
        {
            CallbackHelper::Progress(_globalCallback, count, numShapes, "Writing...", _key, percent);

            IShape* shp = p->second;
            CopyShape(this, shp, *retval);
            ++p;
        }
    }

    // -------------------------------------------
    // output validation
    // -------------------------------------------
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
    ValidateOutput(retval, "Sort");
    return S_OK;
}

// ********************************************************************
//		Merge()
// ********************************************************************
STDMETHODIMP CShapefile::Merge(VARIANT_BOOL SelectedOnlyThis, IShapefile* sf, VARIANT_BOOL SelectedOnly,
                               IShapefile** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    USES_CONVERSION;

    if (sf == nullptr)
    {
        ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
        return S_OK;
    }

    const long numShapes1 = _shapeData.size();
    long numShapes2;
    sf->get_NumShapes(&numShapes2);

    const ShpfileType type1 = _shpfiletype;
    ShpfileType type2;
    sf->get_ShapefileType(&type2);
    if (type1 != type2)
    {
        ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
        return S_OK;
    }

    if (!ValidateInput(this, "Merge", "this", SelectedOnlyThis))
        return S_OK;

    if (!ValidateInput(sf, "Merge", "sf", SelectedOnly))
    {
        return S_OK;
    }

    // -----------------------------------------------
    //	 Creating output
    // -----------------------------------------------
    if (!ShapefileHelper::CloneNoFields(this, retval))
    {
        // Get errorcode and pass the source:
        long errorCode;
        (*retval)->get_LastErrorCode(&errorCode);
        *retval = nullptr;
        ErrorMessage(errorCode);
        return S_OK;
    }

    VARIANT_BOOL vbretval;

    // copying fields from both shapefiles
    std::map<long, long> fieldMap;
    GeoProcessing::CopyFields(this, sf, *retval, fieldMap, true);

    long numFields;
    this->get_NumFields(&numFields);

    // -----------------------------------------------
    //	 Processing
    // -----------------------------------------------
    long percent = 0;
    long count = 0; // index of shape in new shapefile
    CComVariant val;

    for (int i = 0; i < numShapes1; i++)
    {
        CallbackHelper::Progress(_globalCallback, count, numShapes1, "Writing...", _key, percent);

        if (!ShapeAvailable(i, SelectedOnlyThis))
            continue;

        IShape* shp = nullptr;
        this->GetValidatedShape(i, &shp);
        if (shp)
        {
            vbretval = VARIANT_FALSE;
            if (_isEditingShapes)
            {
                IShape* shpCopy = nullptr;
                shp->Clone(&shpCopy);
                if (shpCopy)
                {
                    (*retval)->EditInsertShape(shpCopy, &count, &vbretval);
                    shpCopy->Release();
                }
            }
            else
            {
                (*retval)->EditInsertShape(shp, &count, &vbretval);
            }

            // copying fields
            if (vbretval)
            {
                for (int iFld = 0; iFld < numFields; iFld++)
                {
                    this->get_CellValue(iFld, i, &val);
                    (*retval)->EditCellValue(iFld, count, val, &vbretval);
                }
                count++;
            }
            shp->Release();
        }
    }

    // working with second shapefile
    percent = 0;

    for (int i = 0; i < numShapes2; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, numShapes2, "Writing...", _key, percent);

        if (!((CShapefile*)sf)->ShapeAvailable(i, SelectedOnly))
            continue;

        IShape* shp = nullptr;
        ((CShapefile*)sf)->GetValidatedShape(i, &shp);
        if (shp)
        {
            vbretval = VARIANT_FALSE;
            if (_isEditingShapes)
            {
                IShape* shpCopy = nullptr;
                shp->Clone(&shpCopy);
                if (shpCopy)
                {
                    (*retval)->EditInsertShape(shpCopy, &count, &vbretval);
                    shpCopy->Release();
                }
            }
            else
            {
                (*retval)->EditInsertShape(shp, &count, &vbretval);
            }

            // copying fields
            if (vbretval)
            {
	            std::map<long, long>::iterator p = fieldMap.begin();
                while (p != fieldMap.end())
                {
                    sf->get_CellValue(p->first, i, &val);
                    (*retval)->EditCellValue(p->second, count, val, &vbretval);
                    ++p;
                }
                count++;
            }
            shp->Release();
        }
    }

    // -------------------------------------------
    // output validation
    // -------------------------------------------
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
    ValidateOutput(retval, "Merge");
    return S_OK;
}

#pragma endregion

#pragma region Lines

// **********************************************************************
//		SimplifyLines()
// **********************************************************************
STDMETHODIMP CShapefile::SimplifyLines(DOUBLE Tolerance, VARIANT_BOOL SelectedOnly, IShapefile** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    USES_CONVERSION;

    // ----------------------------------------------
    //	  Validation
    // ----------------------------------------------
    const ShpfileType shpType = ShapeUtility::Convert2D(_shpfiletype);
    if (shpType != SHP_POLYLINE && shpType != SHP_POLYGON)
    {
        ErrorMessage(tkINCOMPATIBLE_SHAPEFILE_TYPE);
        return S_OK;
    }

    if (!ValidateInput(this, "SimplifyLines", "this", SelectedOnly))
        return S_OK;

    // ----------------------------------------------
    //	  Creating output
    // ----------------------------------------------
    IShapefile* sfNew = nullptr;
    this->Clone(&sfNew);

    LONG numFields;
    this->get_NumFields(&numFields);

    // ----------------------------------------------
    //	  Processing
    // ----------------------------------------------
    ReadGeosGeometries(SelectedOnly);
    // long index = 0;
    long percent = 0;

    const int numShapes = (int)_shapeData.size();
    for (int i = 0; i < numShapes; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, numShapes, "Calculating...", _key, percent);

        if (!ShapeAvailable(i, SelectedOnly))
            continue;

        GEOSGeometry* gsGeom = GetGeosGeometry(i);
        if (gsGeom == nullptr) continue;

        // int numGeom = GeosHelper::GetNumGeometries(gsGeom);

        if (shpType == SHP_POLYLINE)
        {
            // ReSharper disable once CppLocalVariableMayBeConst
            GEOSGeom gsNew = GeosHelper::Simplify(gsGeom, Tolerance);
            if (gsNew)
            {
                InsertGeosGeometry(sfNew, gsNew, this, i);
                GeosHelper::DestroyGeometry(gsNew);
            }
        }
        else
        {
            char* val = GeosHelper::GetGeometryType(gsGeom);
            const CString type = val;
            GeosHelper::Free(val);

            if (type != "MultiPolygon")
            {
                // ReSharper disable once CppLocalVariableMayBeConst
                GEOSGeom gsNew = GeosConverter::SimplifyPolygon(gsGeom, Tolerance);
                if (gsNew)
                {
                    InsertGeosGeometry(sfNew, gsNew, this, i);
                    GeosHelper::DestroyGeometry(gsNew);
                }
            }
            else
            {
                for (int n = 0; n < GeosHelper::GetNumGeometries(gsGeom); n++)
                {
                    const GEOSGeometry* gsPart = GeosHelper::GetGeometryN(gsGeom, n);
                    // ReSharper disable once CppLocalVariableMayBeConst
                    GEOSGeom gsNew = GeosConverter::SimplifyPolygon(gsPart, Tolerance);
                    if (gsPart)
                    {
                        InsertGeosGeometry(sfNew, gsNew, this, i);
                        GeosHelper::DestroyGeometry(gsNew);
                    }
                }
            }
        }
        //GeosHelper::DestroyGeometry(gsGeom);
    }

    this->ClearCachedGeometries();
    *retVal = sfNew;

    // -------------------------------------------
    // output validation
    // -------------------------------------------
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
    ValidateOutput(retVal, "SimplifyLines");
    return S_OK;
}

// **********************************************************************
//		Segmentize()
// **********************************************************************
STDMETHODIMP CShapefile::Segmentize(double metersTolerance, IShapefile** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // ----------------------------------------------
    //    Validating
    // ----------------------------------------------
    const ShpfileType shpType = ShapeUtility::Convert2D(this->_shpfiletype);
    if (shpType != SHP_POLYLINE)
    {
        this->ErrorMessage(tkINCOMPATIBLE_SHAPEFILE_TYPE);
        return S_OK;
    }

    if (!ValidateInput(this, "Segmentize", "this", VARIANT_FALSE))
        return S_OK;

    // ----------------------------------------------- 
    //   Creating output
    // ----------------------------------------------- 
    IShapefile* sfOut = nullptr;
    this->Clone(&sfOut);

    // ----------------------------------------------- 
    //   Processing
    // ----------------------------------------------- 
    // caching geos geometries
    this->ReadGeosGeometries(VARIANT_FALSE);

    // turns on the quad tree
    this->GenerateTempQTree(false);

    const long shapeCount = _shapeData.size();
    long percent = 0;

    // get 'meters' in units of the Shapefile
    tkUnitsOfMeasure layerUnits;
    double oneMeter = 1.0;
    VARIANT_BOOL vb;
    IGeoProjection* gp;
    this->get_GeoProjection(&gp);
    gp->get_LinearUnits(&layerUnits);
    // convert Shapefile units to meters
    GetUtils()->ConvertDistance(layerUnits, tkUnitsOfMeasure::umMeters, &oneMeter, &vb);

    for (long i = 0; i < shapeCount; i++)
    {
        CallbackHelper::Progress(_globalCallback, i, shapeCount, "Segmentizing...", _key, percent);

        GEOSGeometry* geom1 = GetGeosGeometry(i);

        double xMin, xMax, yMin, yMax;
        if (this->QuickExtentsCore(i, &xMin, &yMin, &xMax, &yMax))
        {
            //const QTreeExtent query(xMin, xMax, yMax, yMin);
            const QTreeExtent query(xMin - (metersTolerance * oneMeter), xMax + (metersTolerance * oneMeter),
                                    yMax + (metersTolerance * oneMeter), yMin - (metersTolerance * oneMeter));
            std::vector<int> shapes = this->_tempTree->GetNodes(query);

            // calculation union of all geometries
            if (!shapes.empty())
            {
                GEOSGeometry* gsUnion = nullptr;
                for (int shape : shapes)
                {
                    if (i != shape)
                    {
                        GEOSGeometry* gs = _shapeData[shape]->geosGeom;
                        if (!gsUnion)
                        {
                            gsUnion = GeosHelper::CloneGeometry(gs);
                        }
                        else
                        {
                            GEOSGeometry* gsNew = GeosHelper::Union(gsUnion, gs);
                            if (gsNew)
                            {
                                GeosHelper::DestroyGeometry(gsUnion);
                                gsUnion = gsNew;
                            }
                        }
                    }
                }

                // may have had no shapes
                if (gsUnion)
                {
                    GEOSGeometry* gsOut = nullptr;
                    // in order not to modify original (intolerant) behavior...
                    if (metersTolerance == 0.0)
                    {
                        // is current line distinct from the union?
                        gsOut = GeosHelper::Difference(geom1, gsUnion);
                    }
                    else
                    {
                        // otherwise, we first 'snap' geom1 to the Union, to allow for slight misalignment
                        GEOSGeometry* gsNew = GeosHelper::Snap(geom1, gsUnion, (metersTolerance * oneMeter));
                        // then see if it is distinct from the union
                        gsOut = GeosHelper::Difference(gsNew, gsUnion);
                    }
                    GeosHelper::DestroyGeometry(gsUnion);
                    if (gsOut)
                    {
                        const bool res = InsertGeosGeometry(sfOut, gsOut, this, i);
                        GeosHelper::DestroyGeometry(gsOut);
                        if (res)
                            continue;
                    }
                }

            }
        }
        InsertGeosGeometry(sfOut, geom1, this, i);
    }

    *retVal = sfOut;

    // -------------------------------------------
    // output validation
    // -------------------------------------------
    CallbackHelper::ProgressCompleted(_globalCallback, _key);
    ClearTempQTree();
    ClearCachedGeometries();
    ValidateOutput(retVal, "Segmentize");
    return S_OK;

    // GeneratePolygonColors();
}
#pragma endregion

#pragma region Coloring

double CalcMinAngle(GEOSGeometry* geom, double& xCent, double& yCent)
{
    double minAngle = DBL_MAX;
    auto* cs = const_cast<GEOSCoordSequence*>(GeosHelper::GetCoordinatesSeq(geom));
    if (cs)
    {
        const unsigned int size = GeosHelper::CoordinateSequenceSize(cs);
        double x, y;
        for (unsigned int i = 0; i < size; i++)
        {
            if (GeosHelper::CoordinateSequenceGetXY(cs, i, x, y))
            {
                x -= xCent;
                y -= yCent;
                const double angle = GeometryHelper::GetPointAngle(x, y);
                if (angle < minAngle)
                    minAngle = angle;
            }
        }
    }
    return minAngle;
}

// **********************************************************************
//		GeneratePolygonColors()
// **********************************************************************
Coloring::ColorGraph* CShapefile::GeneratePolygonColors()
{
    GenerateTempQTree(false);
    QTree* tree = GetTempQTree();
    ReadGeosGeometries(VARIANT_FALSE);

    const long numShapes = _shapeData.size();
    long percent = 0;

    auto* graph = new Coloring::ColorGraph();

    // ---------------------------------------
    //  spatial relations
    // ---------------------------------------
    for (size_t i = 0; i < _shapeData.size(); i++)
    {
        CallbackHelper::Progress(_globalCallback, i, numShapes, "Calculating spatial relations...", _key, percent);

        double xMin, xMax, yMin, yMax;
        this->QuickExtentsCore(i, &xMin, &yMin, &xMax, &yMax);
        vector<int> shapeIds = tree->GetNodes(QTreeExtent(xMin, xMax, yMax, yMin));

        graph->InsertNode(i);

        if (!shapeIds.empty())
        {
            GEOSGeometry* geom = GetGeosGeometry(i);
            if (geom)
            {
                for (int shapeId : shapeIds)
                {
                    if (shapeId <= (int)i)
                        // it's a single node, not a pair; or the pair was already analyzed in reverse order
                        continue;

                    GEOSGeometry* geom2 = GetGeosGeometry(shapeId);
                    if (GeosHelper::Touches(geom, geom2))
                    {
                        const double angle = DBL_MAX;
                        bool commonEdge = false;
                        GEOSGeometry* g = GeosHelper::Intersection(geom, geom2);
                        if (g)
                        {
                            // ------------------------------------------------
                            //	Let's calculate clockwise direction for shape
                            // ------------------------------------------------
                            const int geomType = GeosHelper::GetGeometryTypeId(g);
                            switch (geomType)
                            {
                            case GEOS_POINT:
                                break; // we need at least common edge
                            case GEOS_LINESTRING:
                            case GEOS_LINEARRING:
                            case GEOS_MULTILINESTRING:
                                commonEdge = true;
                                break;
                            default:
                                CallbackHelper::AssertionFailed(
                                    Debug::Format("Unexpected geometry type: %d.", geomType));
                                break;
                            }
                            GeosHelper::DestroyGeometry(g);
                        }
                        if (commonEdge)
                        {
                            graph->InsertEdge(i, shapeId, angle);
                        }
                    }
                }
            }
        }
    }

    // ---------------------------------------
    //  actual coloring
    // ---------------------------------------
    graph->DoColoring();

    ClearCachedGeometries();
    ClearTempQTree();

    return graph;
}
#pragma endregion

// ReSharper restore CppUseAuto
