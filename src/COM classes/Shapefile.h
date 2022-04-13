//********************************************************************************************************
//File name: Shapefile.h
//Description: Declaration of the CShapefile
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
//********************************************************************************************************

// ReSharper disable CppEnforceOverridingFunctionStyle
// ReSharper disable CppInconsistentNaming
#pragma once
#include <afxmt.h>
#include <set>

#include "ClipperConverter.h"
#include "ColoringGraph.h"
#include "IndexSearching.h"
#include "QTree.h"
#include "ShapeRecord.h"

//Shapefile File Info
#define HEADER_BYTES_16 50
#define HEADER_BYTES_32 100
#define FILE_CODE      9994
#define VERSION        1000
#define UNUSEDSIZE 5
#define UNUSEDVAL		0     	    
#define RECORD_HEADER_LENGTH_32 8
#define RECORD_SHAPE_TYPE_32 8

// *********************************************************************
// CShapefile declaration
// *********************************************************************
class ATL_NO_VTABLE CShapefile :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CShapefile, &CLSID_Shapefile>,
	public IDispatchImpl<IShapefile, &IID_IShapefile, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:

	CShapefile();

	~CShapefile();

	DECLARE_REGISTRY_RESOURCEID(IDR_SHAPEFILE)

	DECLARE_NOT_AGGREGATABLE(CShapefile)

	BEGIN_COM_MAP(CShapefile)
		COM_INTERFACE_ENTRY(IShapefile)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, _pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &_pUnkMarshaler.p);
	}

	void FinalRelease()
	{
		_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> _pUnkMarshaler;


	// *********************************************************************
	//		IShapefile interface
	// *********************************************************************
public:
	STDMETHOD(QuickPoints)(/*[in]*/ long shapeIndex, /*[in, out]*/ long* numPoints, /*[out, retval]*/ SAFEARRAY** retval);
	STDMETHOD(QuickExtents)(/*[in]*/ long shapeIndex, /*[out, retval]*/ IExtents** retval);
	STDMETHOD(QuickPoint)(/*[in]*/ long shapeIndex, /*[in]*/ long pointIndex, /*[out, retval]*/ IPoint** retval);
	STDMETHOD(get_Filename)(/*[out, retval]*/ BSTR* pVal);
	STDMETHOD(get_FileHandle)(/*[out, retval]*/ long* pVal);
	STDMETHOD(get_EditingTable)(/*[out, retval]*/ VARIANT_BOOL* pVal);
	STDMETHOD(get_CellValue)(/*[in]*/ long fieldIndex, /*[in]*/ long shapeIndex, /*[out, retval]*/ VARIANT* pVal);
	STDMETHOD(get_Field)(/*[in]*/ long fieldIndex, /*[out, retval]*/ IField** pVal);
	STDMETHOD(get_FieldByName)(/*[in]*/ BSTR fieldname, /*[out, retval] */ IField** pVal);
	STDMETHOD(StopEditingTable)(/*[in, optional, defaultvalue(TRUE)]*/ VARIANT_BOOL applyChanges, /*[in, optional]*/ ICallback* cBack, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(StartEditingTable)(/*[in, optional]*/ ICallback* cBack, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(EditCellValue)(/*[in]*/ long fieldIndex, /*[in]*/ long shapeIndex, /*[in]*/ VARIANT newVal, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(EditDeleteField)(/*[in]*/ long fieldIndex, /*[in, optional]*/ ICallback* cBack, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(EditInsertField)(/*[in]*/ IField* newField, /*[in,out]*/long* fieldIndex, /*[in, optional]*/ICallback* cBack, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long errorCode, /*[out, retval]*/ BSTR* pVal);
	STDMETHOD(StopEditingShapes)(/*[in, optional, defaultvalue(TRUE)]*/VARIANT_BOOL applyChanges,/*[in,optional,defaultvalue(TRUE)]*/VARIANT_BOOL stopEditTable, /*[in, optional]*/ ICallback* cBack, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(StartEditingShapes)(/*[in,optional,defaultvalue(TRUE)]*/VARIANT_BOOL startEditTable,/*[in, optional]*/ ICallback* cBack, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(SelectShapes)(/*[in]*/ IExtents* boundBox, /*[in, optional, defaultvalue(0.0)]*/double tolerance, /*[in, optional, defaultvalue(INTERSECTION)]*/ SelectMode selectMode, /*[in, out]*/ VARIANT* result, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(EditDeleteShape)(/*[in]*/long shapeIndex, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(EditInsertShape)(/*[in]*/IShape* shape, /*[in, out]*/ long* shapeIndex, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(EditClear)(/*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(Close)(/*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(SaveAs)(/*[in]*/ BSTR shapefileName, /*[in, optional]*/ ICallback* cBack, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(CreateNew)(/*[in]*/ BSTR shapefileName, /*[in]*/ ShpfileType shapefileType, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(CreateNewWithShapeID)(/*[in]*/ BSTR shapefileName, /*[in]*/ ShpfileType shapefileType, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(Open)(/*[in]*/ BSTR shapefileName, /*[in, optional]*/ICallback* cBack, /*[out, retval]*/VARIANT_BOOL* retval);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR* pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback** pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback* newVal);
	STDMETHOD(get_CdlgFilter)(/*[out, retval]*/ BSTR* pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long* pVal);
	STDMETHOD(get_EditingShapes)(/*[out, retval]*/ VARIANT_BOOL* pVal);
	STDMETHOD(get_Shape)(/*[in]*/long shapeIndex, /*[out, retval]*/ IShape** pVal);
	STDMETHOD(get_ShapefileType)(/*[out, retval]*/ ShpfileType* pVal);
	STDMETHOD(get_Extents)(/*[out, retval]*/ IExtents** pVal);
	STDMETHOD(get_NumFields)(/*[out, retval]*/ long* pVal);
	STDMETHOD(get_NumShapes)(/*[out, retval]*/ long* pVal);
	STDMETHOD(get_Projection)(/*[out, retval]*/ BSTR* pVal);
	STDMETHOD(put_Projection)(/*[in]*/BSTR proj4String);
	STDMETHOD(get_NumPoints)(/*[in]*/ long shapeIndex, /*[out, retval]*/ long* pVal);
	STDMETHOD(get_UseSpatialIndex)(/*[out, retval]*/VARIANT_BOOL* pVal);
	STDMETHOD(put_UseSpatialIndex)(/*[in]*/VARIANT_BOOL pVal);
	STDMETHOD(get_HasSpatialIndex)(/*[out, retval]*/VARIANT_BOOL* pVal);
	STDMETHOD(get_HasOldSpatialIndex)(/*[out, retval]*/VARIANT_BOOL* pVal);
	STDMETHOD(put_HasSpatialIndex)(/*[in]*/VARIANT_BOOL pVal);
	STDMETHOD(CreateSpatialIndex)(/*[in, optional]*/BSTR shapefileName, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(Resource)(/*[in]*/ BSTR newShpPath, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(IsSpatialIndexValid)(/*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(put_SpatialIndexMaxAreaPercent)(/*[in]*/ DOUBLE newVal);
	STDMETHOD(get_SpatialIndexMaxAreaPercent)(/*[out, retval]*/ DOUBLE* pVal);
	STDMETHOD(get_CanUseSpatialIndex)(/*[in]*/ IExtents* pArea, /*[out, retval]*/ VARIANT_BOOL* pVal);
	STDMETHOD(PointInShape)(LONG shapeIndex, DOUBLE x, DOUBLE y, VARIANT_BOOL* retval);
	STDMETHOD(PointInShapefile)(DOUBLE x, DOUBLE y, LONG* shapeIndex);
	STDMETHOD(BeginPointInShapefile)(VARIANT_BOOL* retval);
	STDMETHOD(EndPointInShapefile)();
	STDMETHOD(get_CacheExtents)(VARIANT_BOOL* pVal);
	STDMETHOD(put_CacheExtents)(VARIANT_BOOL newVal);
	STDMETHOD(RefreshExtents)(VARIANT_BOOL* pVal);
	STDMETHOD(RefreshShapeExtents)(LONG shapeId, VARIANT_BOOL* pVal);
	STDMETHOD(QuickQueryInEditMode)(/*[in]*/IExtents* boundBox, int** result, int* resultCount);
	STDMETHOD(get_UseQTree)(VARIANT_BOOL* pVal);
	STDMETHOD(put_UseQTree)(VARIANT_BOOL pVal);
	STDMETHOD(Save)(/*[in, optional]*/ ICallback* cBack, /*[out, retval]*/ VARIANT_BOOL* retval);
	STDMETHOD(GetIntersection)(/*[in]*/VARIANT_BOOL selectedOnlyOfThis, /*[in]*/IShapefile* sf, /*[in]*/VARIANT_BOOL selectedOnly, /*[in]*/ ShpfileType fileType, /*[in, optional, defaultvalue(NULL)]*/ ICallback* cBack, /*[out, retval]*/ IShapefile** retval);
	STDMETHOD(SelectByShapefile)(/*[in]*/IShapefile* sf,  /*[in]*/tkSpatialRelation relation, /*[in]*/VARIANT_BOOL selectedOnly, /*[in, out]*/ VARIANT* result, /*[in, optional, defaultvalue(NULL)]*/ ICallback* cBack, /*[out, retval]*/VARIANT_BOOL* retval);
	STDMETHOD(get_SelectionDrawingOptions)(/*[out, retval]*/IShapeDrawingOptions** pVal);
	STDMETHOD(put_SelectionDrawingOptions)(/*[in]*/IShapeDrawingOptions* newVal);
	STDMETHOD(get_ShapeSelected)(/*[in]*/long shapeIndex,/*[out, retval]*/ VARIANT_BOOL* pVal);
	STDMETHOD(put_ShapeSelected)(/*[in]*/long shapeIndex, /*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_NumSelected)(/*[out, retval]*/long* pVal);
	STDMETHOD(SelectAll)();
	STDMETHOD(SelectNone)();
	STDMETHOD(InvertSelection)();
	STDMETHOD(Dissolve)(long fieldIndex, VARIANT_BOOL selectedOnly, IShapefile** sf);
	STDMETHOD(get_Labels)(ILabels** pVal);
	STDMETHOD(put_Labels)(ILabels* newVal);
	STDMETHOD(GenerateLabels)(long fieldIndex, tkLabelPositioning method, VARIANT_BOOL largestPartOnly, long offsetXFieldIndex, long offsetYFieldIndex, long* count);
	STDMETHOD(Clone)(IShapefile** retVal);
	STDMETHOD(get_DefaultDrawingOptions)(IShapeDrawingOptions** pVal);
	STDMETHOD(put_DefaultDrawingOptions)(IShapeDrawingOptions* newVal);
	STDMETHOD(get_Categories)(IShapefileCategories** pVal);
	STDMETHOD(put_Categories)(IShapefileCategories* newVal);
	STDMETHOD(get_Charts)(ICharts** pVal);
	STDMETHOD(put_Charts)(ICharts* newVal);
	STDMETHOD(get_ShapeCategory)(/*[in]*/long shapeIndex,/*[out, retval]*/ long* pVal);
	STDMETHOD(put_ShapeCategory)(/*[in]*/long shapeIndex, /*[in]*/ long newVal);
	STDMETHOD(get_Table)(ITable** retVal);
	STDMETHOD(get_VisibilityExpression)(BSTR* retval);
	STDMETHOD(put_VisibilityExpression)(BSTR newVal);
	STDMETHOD(get_FastMode)(VARIANT_BOOL* retval);		// in fast editing mode CShapeWrapper class is used to store shape points
	STDMETHOD(put_FastMode)(VARIANT_BOOL newVal);		// there are some restrictions on editing this mode though
	STDMETHOD(get_MinDrawingSize)(LONG* pVal);
	STDMETHOD(put_MinDrawingSize)(LONG newVal);
	STDMETHOD(get_SourceType)(tkShapefileSourceType* pVal);
	STDMETHOD(BufferByDistance)(double distance, LONG nSegments, VARIANT_BOOL selectedOnly, VARIANT_BOOL mergeResults, IShapefile** sf);
	STDMETHOD(get_GeometryEngine)(tkGeometryEngine* pVal);
	STDMETHOD(put_GeometryEngine)(tkGeometryEngine pVal);
	STDMETHOD(Difference)(VARIANT_BOOL selectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL selectedOnlyOverlay, IShapefile** retval);
	STDMETHOD(Clip)(VARIANT_BOOL selectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL selectedOnlyOverlay, IShapefile** retval);
	STDMETHOD(SymmDifference)(VARIANT_BOOL selectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL selectedOnlyOverlay, IShapefile** retval);
	STDMETHOD(Union)(VARIANT_BOOL selectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL selectedOnlyOverlay, IShapefile** retval);
	STDMETHOD(ExplodeShapes)(VARIANT_BOOL selectedOnly, IShapefile** retval);
	STDMETHOD(AggregateShapes)(VARIANT_BOOL selectedOnly, LONG fieldIndex, IShapefile** retval);
	STDMETHOD(ExportSelection)(IShapefile** retval);
	STDMETHOD(Sort)(LONG fieldIndex, VARIANT_BOOL ascending, IShapefile** retval);
	STDMETHOD(Merge)(VARIANT_BOOL selectedOnlyThis, IShapefile* sf, VARIANT_BOOL selectedOnly, IShapefile** retval);
	STDMETHOD(get_SelectionColor)(OLE_COLOR* retval);
	STDMETHOD(put_SelectionColor)(OLE_COLOR newVal);
	STDMETHOD(get_SelectionAppearance)(tkSelectionAppearance* retval);
	STDMETHOD(put_SelectionAppearance)(tkSelectionAppearance newVal);
	STDMETHOD(get_CollisionMode)(tkCollisionMode* retval);
	STDMETHOD(put_CollisionMode)(tkCollisionMode newVal);
	STDMETHOD(get_SelectionTransparency)(BYTE* retval);
	STDMETHOD(put_SelectionTransparency)(BYTE newVal);
	STDMETHOD(put_StopExecution)(IStopExecution* stopper);
	STDMETHOD(Serialize)(VARIANT_BOOL saveSelection, BSTR* retVal);
	STDMETHOD(Serialize2)(VARIANT_BOOL saveSelection, VARIANT_BOOL serializeCategories, BSTR* retVal);
	STDMETHOD(Deserialize)(VARIANT_BOOL loadSelection, BSTR newVal);
	STDMETHOD(get_GeoProjection)(IGeoProjection** retVal);
	STDMETHOD(put_GeoProjection)(IGeoProjection* pVal);
	STDMETHOD(Reproject)(IGeoProjection* newProjection, LONG* reprojectedCount, IShapefile** retVal);
	STDMETHOD(ReprojectInPlace)(IGeoProjection* newProjection, LONG* reprojectedCount, VARIANT_BOOL* retVal);
	STDMETHOD(SimplifyLines)(DOUBLE tolerance, VARIANT_BOOL selectedOnly, IShapefile** retVal);
	STDMETHOD(FixUpShapes)(IShapefile** retVal, VARIANT_BOOL* fixed);
	STDMETHOD(GetRelatedShapes)(long referenceIndex, tkSpatialRelation relation, VARIANT* resultArray, VARIANT_BOOL* retval);
	STDMETHOD(GetRelatedShapes2)(IShape* referenceShape, tkSpatialRelation relation, VARIANT* resultArray, VARIANT_BOOL* retval);
	STDMETHOD(get_Identifiable)(VARIANT_BOOL* retVal);
	STDMETHOD(put_Identifiable)(VARIANT_BOOL newVal);
	STDMETHOD(HasInvalidShapes)(VARIANT_BOOL* result);
	STDMETHOD(EditAddShape)(IShape* shape, long* shapeIndex);
	STDMETHOD(EditAddField)(BSTR name, FieldType type, int precision, int width, long* fieldIndex);
	STDMETHOD(GetClosestVertex)(double x, double y, double maxDistance, long* shapeIndex, long* pointIndex, double* distance, VARIANT_BOOL* retVal);
	STDMETHOD(GetClosestSnapPosition)(double x, double y, double maxDistance, long* shapeIndex, double* fx, double* fy, double* distance, VARIANT_BOOL* retVal);
	STDMETHOD(get_ShapeCategory2)(long shapeIndex, BSTR* categoryName);
	STDMETHOD(put_ShapeCategory2)(long shapeIndex, BSTR categoryName);
	STDMETHOD(get_ShapeCategory3)(long shapeIndex, IShapefileCategory** category);
	STDMETHOD(put_ShapeCategory3)(long shapeIndex, IShapefileCategory* category);
	STDMETHOD(Dump)(BSTR shapefileName, ICallback* cBack, VARIANT_BOOL* retval);
	STDMETHOD(LoadDataFrom)(BSTR shapefileName, ICallback* cBack, VARIANT_BOOL* retval);
	STDMETHOD(Segmentize)(double metersTolerance, IShapefile** retVal);
	STDMETHOD(get_LastInputValidation)(IShapeValidationInfo** retVal);
	STDMETHOD(get_LastOutputValidation)(IShapeValidationInfo** retVal);
	STDMETHOD(ClearCachedGeometries)();
	STDMETHOD(AggregateShapesWithStats)(VARIANT_BOOL selectedOnly, LONG fieldIndex, IFieldStatOperations* statOperations, IShapefile** retval);
	STDMETHOD(DissolveWithStats)(long fieldIndex, VARIANT_BOOL selectedOnly, IFieldStatOperations* statOperations, IShapefile** sf);
	STDMETHOD(get_ShapeRotation)(long shapeIndex, double* pVal);
	STDMETHOD(put_ShapeRotation)(long shapeIndex, double newVal);
	STDMETHOD(get_ShapeVisible)(long shapeIndex, VARIANT_BOOL* pVal);
	STDMETHOD(get_Volatile)(VARIANT_BOOL* retval);
	STDMETHOD(put_Volatile)(VARIANT_BOOL newVal);
	STDMETHOD(EditUpdateShape)(long shapeIndex, IShape* shpNew, VARIANT_BOOL* retVal);
	STDMETHOD(get_ShapeModified)(long shapeIndex, VARIANT_BOOL* retVal);
	STDMETHOD(put_ShapeModified)(long shapeIndex, VARIANT_BOOL newVal);
	STDMETHOD(Validate)(tkShapeValidationMode validationMode, VARIANT_BOOL selectedOnly, IShapeValidationInfo** results);
	STDMETHOD(get_UndoList)(IUndoList** pVal);
	STDMETHOD(get_InteractiveEditing)(VARIANT_BOOL* pVal);
	STDMETHOD(put_InteractiveEditing)(VARIANT_BOOL newVal);
	STDMETHOD(get_ShapeIsHidden)(LONG shapeIndex, VARIANT_BOOL* pVal);
	STDMETHOD(put_ShapeIsHidden)(LONG shapeIndex, VARIANT_BOOL newVal);
	STDMETHOD(get_Snappable)(VARIANT_BOOL* pVal);
	STDMETHOD(put_Snappable)(VARIANT_BOOL newVal);
	STDMETHOD(get_ShapefileType2D)(ShpfileType* pVal);
	STDMETHOD(get_FieldIndexByName)(BSTR fieldName, LONG* pVal);
	STDMETHOD(Move)(DOUBLE xProjOffset, DOUBLE yProjOffset, VARIANT_BOOL* retVal);
	STDMETHOD(RemoveSpatialIndex)(VARIANT_BOOL* retVal);
	STDMETHOD(get_ShapeRendered)(LONG shapeIndex, VARIANT_BOOL* pVal);
	STDMETHOD(get_SortField)(BSTR* pVal);
	STDMETHOD(put_SortField)(BSTR newVal);
	STDMETHOD(get_SortAscending)(VARIANT_BOOL* pVal);
	STDMETHOD(put_SortAscending)(VARIANT_BOOL newVal);
	STDMETHOD(UpdateSortField)();
	STDMETHOD(SaveAsEx)(BSTR newFilename, VARIANT_BOOL stopEditing, VARIANT_BOOL unboundFile, VARIANT_BOOL* retVal);
	STDMETHOD(FixUpShapes2)(VARIANT_BOOL selectedOnly, IShapefile** result, VARIANT_BOOL* retVal);
	STDMETHOD(StartAppendMode)(VARIANT_BOOL* retVal);
	STDMETHOD(StopAppendMode)();
	STDMETHOD(get_AppendMode)(VARIANT_BOOL* pVal);
	STDMETHOD(get_IsGeographicProjection)(VARIANT_BOOL* pVal);
	STDMETHOD(get_Selectable)(VARIANT_BOOL* retVal);
	STDMETHOD(put_Selectable)(VARIANT_BOOL newVal);
	STDMETHOD(get_HasOgrFidMapping)(VARIANT_BOOL* pVal);
	STDMETHOD(OgrFid2ShapeIndex)(long ogrFid, LONG* retVal);
private:

	// data for point in shapefile test
	struct ShapeHeader
	{
		double MinX, MinY;
		double MaxX, MaxY;
		int NumParts;
		int NumPoints;
	};
	struct PolygonShapefile
	{
		ShapeHeader shpHeader;
		std::vector<Point2D> Points;
		std::vector<int> Parts;
	};

	CCriticalSection _readLock;

	std::vector<PolygonShapefile> _polySf;

	tkShapefileSourceType _sourceType;		// is it disk-based or in-memory?
	ShpfileType _shpfiletype;
	BSTR _key;
	long _lastErrorCode;

	//Extent Information
	double _minX;
	double _minY;
	double _minZ;
	double _maxX;
	double _maxY;
	double _maxZ;
	double _minM;
	double _maxM;

	//Disk access
	FILE* _shpfile;
	FILE* _shxfile;

	CStringW _shpfileName;
	CStringW _shxfileName;
	CStringW _dbffileName;
	CStringW _prjfileName;

	std::vector<ShapeRecord*> _shapeData;
	std::vector<long> _shpOffsets;		//(32 bit words)

	// OGR layers can lookup by fixed FID rather than ever-changing ShapeIndex
	// (Shape indices change when removing rows, and may change on OGR reload)
	std::map<long, long> _ogrFid2ShapeIndex;
	std::set<long> _deletedFids;
	bool _hasOgrFidMapping = false;

	// table is initialized in CreateNew or Open methods
	// it is is destroyed in Close() method
	// in case table is null, shapefile will be considered uninitialized
	ITable* _table;
	IGeoProjection* _geoProjection;
	IStopExecution* _stopExecution;
	ICharts* _charts;
	ILabels* _labels;
	IShapefileCategories* _categories;
	ICallback* _globalCallback;
	IShapeDrawingOptions* _selectDrawOpt;
	IShapeDrawingOptions* _defaultDrawOpt;
	IShapeValidationInfo* _inputValidation;
	IShapeValidationInfo* _outputValidation;
	IUndoList* _undoList;

	VARIANT_BOOL _hotTracking;
	VARIANT_BOOL _selectable;
	bool _geosGeometriesRead;
	tkCollisionMode _collisionMode;		// collision mode for point shapefiles
	tkGeometryEngine _geometryEngine;		// GEOS or Clipper
	bool _writing;		// is currently writing to the file
	bool _reading;		// is currently reading data into memory

	BSTR _expression;		// visibility expression
	BOOL _isEditingShapes;		//Flag for Disk vs. Memory
	long _nextShapeHandle;		// the next unique handle to assign
	VARIANT_BOOL _interactiveEditing;
	VARIANT_BOOL _snappable;
	VARIANT_BOOL _appendMode;
	int _appendStartShapeCount;	  // number of shapes when append mode is started

	// When this flag is on CShapeWrapper will be used in the Shape class to store the points
	// otherwise usual COM points
	VARIANT_BOOL _fastMode;
	int _minDrawingSize;	// objects which are less than this value in pixels for current scale, will drawn as point
	BOOL _cacheExtents;	// extents won't be recalculated in each get_Extents call
	bool _volatile;

	//Flags for Spatial Indexing
	BOOL _useSpatialIndex;
	BOOL _hasSpatialIndex;
	IndexSearching::CSpatialIndexID _spatialIndexID;
	BOOL _spatialIndexLoaded;
	DOUBLE _spatialIndexMaxAreaPercent;
	int _spatialIndexNodeCapacity;

	// drawing options
	tkSelectionAppearance _selectionAppearance;
	OLE_COLOR _selectionColor;
	unsigned char _selectionTransparency;

	QTree* _qtree;
	BOOL _useQTree;
	vector<int> _deleteElement;

	// during processing operations only
	QTree* _tempTree;

	BSTR _sortField;
	VARIANT_BOOL _sortAscending;
	bool _sortingChanged;
	std::vector<long> _sorting;    // indices of labels

private:
	// memory shapes
	BOOL ReleaseMemoryShapes();
	BOOL VerifyMemShapes(ICallback* cBack);
	//void TrimMemShapes();

	// read/write
	BOOL ReadShx();
	BOOL WriteShx(FILE* shxfile, ICallback* cBack);
	BOOL WriteShp(FILE* shpfile, ICallback* cBack);

	// selection
	BOOL DefineShapePoints(long shapeIndex, ShpfileType& shapeType, std::vector<long>& parts, std::vector<double>& xPts, std::vector<double>& yPts);
	VARIANT_BOOL SelectShapesAlt(IExtents* boundBox, double tolerance, SelectMode selectMode, VARIANT* arr);

	// initialization
	void put_ReferenceToLabels(bool bNullReference = false);
	void put_ReferenceToCategories(bool bNullReference = false);
	void put_ReferenceToCharts(bool bNullReference = false);

	// quad tree
	QTree* GenerateQTreeCore(bool selectedOnly);
	QTree* GenerateEmptyQTree(double* xMin, double* xMax, double* yMin, double* yMax, double* zMin, double* zMax);
	void ClearQTree(double* xMin, double* xMax, double* yMin, double* yMax, double* zMin, double* zMax);
	void GenerateQTree();

	// temp quad tree
	bool GenerateTempQTree(bool selectedOnly);
	void ClearTempQTree();
	QTree* GetTempQTree();

	// geoprocessing
	void DoClipOperation(VARIANT_BOOL selectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL selectedOnlyOverlay, IShapefile** retval, tkClipOperation operation, ShpfileType returnType = SHP_NULLSHAPE);
	void DissolveClipper(long fieldIndex, VARIANT_BOOL selectedOnly, IFieldStatOperations* operations, IShapefile* sf);
	void DissolveGEOS(long fieldIndex, VARIANT_BOOL selectedOnly, IFieldStatOperations* operations, IShapefile* sf);
	void IntersectionGEOS(VARIANT_BOOL selectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL selectedOnlyClip, IShapefile* sfResult, map<long, long>* fieldMap = nullptr, std::set<int>* subjectShapesToSkip = nullptr, std::set<int>* clippingShapesToSkip = nullptr);
	void IntersectionClipper(VARIANT_BOOL selectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL selectedOnlyClip, IShapefile* sfResult, map<long, long>* fieldMap = nullptr, std::set<int>* subjectShapesToSkip = nullptr, std::set<int>* clippingShapesToSkip = nullptr);
	IShapefile* IntersectionClipperNoAttributes(VARIANT_BOOL selectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL selectedOnlyClip);
	void DifferenceGEOS(IShapefile* sfSubject, VARIANT_BOOL selectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL selectedOnlyOverlay, IShapefile* sfResult, map<long, long>* fieldMap = nullptr, std::set<int>* shapesToSkip = nullptr);
	void DifferenceClipper(IShapefile* sfSubject, VARIANT_BOOL selectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL selectedOnlyClip, IShapefile* sfResult, map<long, long>* fieldMap = nullptr, std::set<int>* shapesToSkip = nullptr);
	void ClipGEOS(VARIANT_BOOL selectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL selectedOnlyOverlay, IShapefile* sfResult);
	void ClipClipper(VARIANT_BOOL selectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL selectedOnlyOverlay, IShapefile* sfResult);
	void AggregateShapesCore(VARIANT_BOOL selectedOnly, LONG fieldIndex, IFieldStatOperations* statOperations, IShapefile** retval);
	void DissolveCore(long fieldIndex, VARIANT_BOOL selectedOnly, IFieldStatOperations* statOperations, IShapefile** sf);
	void CalculateFieldStats(map<int, vector<int>*>& indicesMap, IFieldStatOperations* operations, IShapefile* output);
	static void InsertShapesVector(IShapefile* sf, vector<IShape* >& vShapes, IShapefile* sfSubject, long subjectId, std::map<long, long>* fieldMapSubject = nullptr, IShapefile* sfClip = nullptr, long clipId = -1, std::map<long, long>* fieldMapClip = nullptr);
	void GetRelatedShapeCore(IShape* referenceShape, long referenceIndex, tkSpatialRelation relation, VARIANT* resultArray, VARIANT_BOOL* retval);
	void ReleaseRenderingCache();
	bool ReadShapeExtents(long shapeIndex, Extent& result);
	IShape* ReadComShape(long shapeIndex);
	IShape* ReadFastModeShape(long shapeIndex);
	int GetWriteFileLength();
	bool WriteAppendedShape();
	bool AppendToShx(FILE* shx, IShape* shp, int offset);
	bool AppendToShpFile(FILE* shp, IShapeWrapper* wrapper);
	void WriteBounds(FILE* shp);
	bool ReopenFiles(bool writeMode);
	// read only those geometries requested by the specified array
	void ReadGeosGeometries(std::set<int> list);
	bool IsShapeCompatible(IShape* shape);

public:
	// accessing shapes
	bool ShapeAvailable(int shapeIndex, VARIANT_BOOL selectedOnly);
	HRESULT GetValidatedShape(int shapeIndex, IShape** retVal);
	void ReadGeosGeometries(VARIANT_BOOL selectedOnly);
	GEOSGeometry* GetGeosGeometry(int shapeIndex);

	// geoprocessing
	Coloring::ColorGraph* GeneratePolygonColors();
	bool ReprojectCore(IGeoProjection* newProjection, LONG* reprojectedCount, IShapefile** retVal, bool reprojectInPlace);

	// errors
	void ErrorMessage(long errorCode);
	void ErrorMessage(long errorCode, ICallback* cBack);

	// underlying data
	std::vector<ShapeRecord*>* get_ShapeVector();
	IShapeWrapper* get_ShapeWrapper(int shapeIndex);
	IShapeData* get_ShapeRenderingData(int shapeIndex);
	void put_ShapeRenderingData(int shapeIndex, CShapeData* data);
	FILE* get_File() const { return _shpfile; }
	CCriticalSection* get_ReadLock() { return &_readLock; }

	// serialization
	bool DeserializeCore(VARIANT_BOOL loadSelection, CPLXMLNode* node);
	CPLXMLNode* SerializeCore(VARIANT_BOOL saveSelection, CString elementName, bool serializeCategories);

	// charts
	void SetChartsPositions(tkLabelPositioning method);
	void ClearChartFrames();

	// selection
	bool SelectShapesCore(Extent& extents, double tolerance, SelectMode selectMode, std::vector<long>& selectResult, bool renderedOnly);
	bool QuickExtentsCore(long shapeIndex, Extent& result);
	bool QuickExtentsCore(long shapeIndex, double* xMin, double* yMin, double* xMax, double* yMax);

	// editing
	bool OpenCore(CStringW tmpShpfileName, ICallback* cBack);
	HRESULT CreateNewCore(BSTR ShapefileName, ShpfileType shapefileType, bool applyRandomOptions, VARIANT_BOOL* retval);
	void RegisterNewShape(IShape* shape, long shapeIndex);
	void ReregisterShape(int shapeIndex);

	// validation
	void SetValidationInfo(IShapeValidationInfo* info, tkShapeValidationType validationType);
	bool ValidateInput(IShapefile* isf, CString methodName, CString parameterName, VARIANT_BOOL selectedOnly, CString className = "Shapefile");
	IShapeValidationInfo* ValidateInputCore(IShapefile* isf, CString methodName, CString parameterName, VARIANT_BOOL selectedOnly, tkShapeValidationMode validationMode, CString className = "Shapefile", bool reportOnly = false);
	IShapeValidationInfo* ValidateOutput(IShapefile** isf, CString methodName, CString className = "Shapefile", bool abortIfEmpty = true);
	bool ValidateOutput(IShapefile* sf, CString methodName, CString className = "Shapefile", bool abortIfEmpty = true);
	void RestoreShapeRecordsMapping();
	bool ValidateClippingOutputType(ShpfileType type1, ShpfileType type2, ShpfileType returnType, tkClipOperation operation);
	// drawing 
	void MarkUndrawn();
	void GetLabelString(long fieldIndex, long shapeIndex, BSTR* text, CString floatNumberFormat);
	void GetLabelOffset(long offsetFieldIndex, long shapeIndex, double* offset);
	bool GetSorting(vector<long>** indices);

	// OGR data source can map OGR FID to ShapeIndex
	void HasOgrFidMapping(const bool hasMapping) { _hasOgrFidMapping = hasMapping; }
	void MapOgrFid2ShapeIndex(long ogrFid, long shapeIndex)
	{
		_ogrFid2ShapeIndex.insert(std::make_pair(ogrFid, shapeIndex));
	}
	bool MarkShapeDeleted(const long shapeIndex)
	{
		if (!_hasOgrFidMapping)
			return false;

		for (auto const& it : _ogrFid2ShapeIndex)
			if (it.second == shapeIndex)
				_deletedFids.insert(it.first);
		return true;
	}

	[[nodiscard]] std::set<long> GetDeletedShapeFIDs() const
	{
		// return std::set<long>(_deletedFids);
		return _deletedFids;
	}
	void ClearDeleteShapeFIDs()
	{
		_deletedFids.clear();
	}

	// give OGR layers the ability to retain visibility flags on reload
	bool GetVisibilityFlags(map<long, BYTE>& flags);
	bool SetVisibilityFlags(map<long, BYTE>& flags);

public:
	// geoprocessing methods
	VARIANT_BOOL FixupShapesCore(VARIANT_BOOL selectedOnly, IShapefile* result);
	VARIANT_BOOL BufferByDistanceCore(double distance, LONG nSegments, VARIANT_BOOL selectedOnly, VARIANT_BOOL mergeResults, IShapefile* result);
	VARIANT_BOOL ExplodeShapesCore(VARIANT_BOOL selectedOnly, IShapefile* result);
	VARIANT_BOOL ExportSelectionCore(IShapefile* result);

};
OBJECT_ENTRY_AUTO(__uuidof(Shapefile), CShapefile)
