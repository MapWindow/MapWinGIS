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

#pragma once
#include <set>
#include "IndexSearching.h"
#include "QTree.h"
#include "ClipperConverter.h"
#include "ShapeInfo.h"
#include "ColoringGraph.h"

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
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShapefile, &CLSID_Shapefile>,
	public IDispatchImpl<IShapefile, &IID_IShapefile, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	
	CShapefile();
	~CShapefile();

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SHAPEFILE)

	DECLARE_NOT_AGGREGATABLE(CShapefile)

	BEGIN_COM_MAP(CShapefile)
		COM_INTERFACE_ENTRY(IShapefile)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()


// *********************************************************************
//		IShapefile interface
// *********************************************************************
public:
	STDMETHOD(QuickPoints)(/*[in]*/ long ShapeIndex, /*[in, out]*/ long * NumPoints, /*[out, retval]*/ SAFEARRAY ** retval);
	STDMETHOD(QuickExtents)(/*[in]*/ long ShapeIndex, /*[out, retval]*/ IExtents ** retval);
	STDMETHOD(QuickPoint)(/*[in]*/ long ShapeIndex, /*[in]*/ long PointIndex, /*[out, retval]*/ IPoint ** retval);
	STDMETHOD(get_Filename)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_FileHandle)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_EditingTable)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_CellValue)(/*[in]*/ long FieldIndex, /*[in]*/ long ShapeIndex, /*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(get_Field)(/*[in]*/ long FieldIndex, /*[out, retval]*/ IField * *pVal);
	STDMETHOD(get_FieldByName)(/*[in]*/ BSTR Fieldname, /*[out, retval] */ IField * *pVal);
	STDMETHOD(StopEditingTable)(/*[in, optional, defaultvalue(TRUE)]*/ VARIANT_BOOL ApplyChanges, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(StartEditingTable)(/*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditCellValue)(/*[in]*/ long FieldIndex, /*[in]*/ long ShapeIndex, /*[in]*/ VARIANT NewVal, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditDeleteField)(/*[in]*/ long FieldIndex, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditInsertField)(/*[in]*/ IField * NewField, /*[in,out]*/long * FieldIndex, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(StopEditingShapes)(/*[in, optional, defaultvalue(TRUE)]*/VARIANT_BOOL ApplyChanges,/*[in,optional,defaultvalue(TRUE)]*/VARIANT_BOOL StopEditTable, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(StartEditingShapes)(/*[in,optional,defaultvalue(TRUE)]*/VARIANT_BOOL StartEditTable,/*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(SelectShapes)(/*[in]*/ IExtents * BoundBox, /*[in, optional, defaultvalue(0.0)]*/double Tolerance, /*[in, optional, defaultvalue(INTERSECTION)]*/ SelectMode SelectMode, /*[in, out]*/ VARIANT * Result, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditDeleteShape)(/*[in]*/long ShapeIndex, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditInsertShape)(/*[in]*/IShape * Shape, /*[in, out]*/ long * ShapeIndex, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditClear)(/*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(Close)(/*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(SaveAs)(/*[in]*/ BSTR ShapefileName, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(CreateNew)(/*[in]*/ BSTR ShapefileName, /*[in]*/ ShpfileType ShapefileType, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(CreateNewWithShapeID)(/*[in]*/ BSTR ShapefileName, /*[in]*/ ShpfileType ShapefileType, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(Open)(/*[in]*/ BSTR ShapefileName, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/VARIANT_BOOL * retval);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_CdlgFilter)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_EditingShapes)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_Shape)(/*[in]*/long ShapeIndex, /*[out, retval]*/ IShape * *pVal);
	STDMETHOD(get_ShapefileType)(/*[out, retval]*/ ShpfileType *pVal);
	STDMETHOD(get_Extents)(/*[out, retval]*/ IExtents * *pVal);
	STDMETHOD(get_NumFields)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_NumShapes)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Projection)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Projection)(/*[in]*/BSTR proj4String);
	STDMETHOD(get_NumPoints)(/*[in]*/ long ShapeIndex, /*[out, retval]*/ long *pVal);
	STDMETHOD(get_UseSpatialIndex)(/*[out, retval]*/VARIANT_BOOL *pVal);
	STDMETHOD(put_UseSpatialIndex)(/*[in]*/VARIANT_BOOL pVal);
 	STDMETHOD(get_HasSpatialIndex)(/*[out, retval]*/VARIANT_BOOL *pVal);
	STDMETHOD(put_HasSpatialIndex)(/*[in]*/VARIANT_BOOL pVal);
    STDMETHOD(CreateSpatialIndex)(/*[in]*/BSTR ShapefileName, /*[out, retval]*/ VARIANT_BOOL  *pVal);
	STDMETHOD(Resource)(/*[in]*/ BSTR newSrcPath, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(IsSpatialIndexValid)(/*[out, retval]*/ VARIANT_BOOL  *pVal);
	STDMETHOD(put_SpatialIndexMaxAreaPercent)(/*[in]*/ DOUBLE newVal);
	STDMETHOD(get_SpatialIndexMaxAreaPercent)(/*[out, retval]*/ DOUBLE* pVal);
	STDMETHOD(get_CanUseSpatialIndex)(/*[in]*/ IExtents* pArea, /*[out, retval]*/ VARIANT_BOOL* pVal);
	STDMETHOD(PointInShape)(LONG ShapeIndex, DOUBLE x, DOUBLE y, VARIANT_BOOL* retval);
	STDMETHOD(PointInShapefile)(DOUBLE x, DOUBLE y, LONG* ShapeIndex);
	STDMETHOD(BeginPointInShapefile)(VARIANT_BOOL* retval);
	STDMETHOD(EndPointInShapefile)(void);
	STDMETHOD(get_CacheExtents)(VARIANT_BOOL * pVal);
	STDMETHOD(put_CacheExtents)(VARIANT_BOOL newVal);
	STDMETHOD(RefreshExtents)(VARIANT_BOOL * pVal);
	STDMETHOD(RefreshShapeExtents)(LONG ShapeId, VARIANT_BOOL *pVal);
	STDMETHOD(QuickQueryInEditMode)(/*[in]*/IExtents * BoundBox,int ** Result, int* ResultCount);
	STDMETHOD(get_UseQTree)(VARIANT_BOOL * pVal);
	STDMETHOD(put_UseQTree)(VARIANT_BOOL pVal);
	STDMETHOD(Save)(/*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(GetIntersection)(/*[in]*/VARIANT_BOOL SelectedOnlyOfThis, /*[in]*/IShapefile* sf, /*[in]*/VARIANT_BOOL SelectedOnly, /*[in]*/ ShpfileType fileType, /*[in, optional, defaultvalue(NULL)]*/ ICallback * cBack, /*[out, retval]*/ IShapefile** retval);
	STDMETHOD(SelectByShapefile)(/*[in]*/IShapefile* sf,  /*[in]*/tkSpatialRelation Relation, /*[in]*/VARIANT_BOOL SelectedOnly, /*[in, out]*/ VARIANT *Result, /*[in, optional, defaultvalue(NULL)]*/ ICallback* cBack, /*[out, retval]*/VARIANT_BOOL *retval);
	STDMETHOD(get_SelectionDrawingOptions)(/*[out, retval]*/IShapeDrawingOptions** pVal);
	STDMETHOD(put_SelectionDrawingOptions)(/*[in]*/IShapeDrawingOptions* newVal);
	STDMETHOD(get_ShapeSelected)(/*[in]*/long ShapeIndex,/*[out, retval]*/ VARIANT_BOOL* pVal);
	STDMETHOD(put_ShapeSelected)(/*[in]*/long ShapeIndex, /*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_NumSelected)(/*[out, retval]*/long *pVal);
	STDMETHOD(SelectAll)();
	STDMETHOD(SelectNone)();
	STDMETHOD(InvertSelection)();
	STDMETHOD(Dissolve)(long FieldIndex, VARIANT_BOOL SelectedOnly, IShapefile** sf);
	STDMETHOD(get_Labels)(ILabels** pVal);
	STDMETHOD(put_Labels)(ILabels* newVal);
	STDMETHOD(GenerateLabels)(long FieldIndex, tkLabelPositioning Method, VARIANT_BOOL LargestPartOnly, long* Count);
	STDMETHOD(Clone)(IShapefile** retVal);
	STDMETHOD(get_DefaultDrawingOptions)(IShapeDrawingOptions** pVal);
	STDMETHOD(put_DefaultDrawingOptions)(IShapeDrawingOptions* newVal);
	STDMETHOD(get_Categories)(IShapefileCategories** pVal);
	STDMETHOD(put_Categories)(IShapefileCategories* newVal);
	STDMETHOD(get_Charts)(ICharts** pVal);
	STDMETHOD(put_Charts)(ICharts* newVal);
	STDMETHOD(get_ShapeCategory)(/*[in]*/long ShapeIndex,/*[out, retval]*/ long* pVal);
	STDMETHOD(put_ShapeCategory)(/*[in]*/long ShapeIndex, /*[in]*/ long newVal);
	STDMETHOD(get_Table)(ITable** retVal);
	STDMETHOD(get_VisibilityExpression)(BSTR* retval);
	STDMETHOD(put_VisibilityExpression)(BSTR newVal);
	STDMETHOD(get_FastMode)(VARIANT_BOOL* retval);		// in fast editing mode CShapeWrapper class is used to store shape points
	STDMETHOD(put_FastMode)(VARIANT_BOOL newVal);		// there are some restrictions on editing this mode though
	STDMETHOD(get_MinDrawingSize)(LONG* pVal);
	STDMETHOD(put_MinDrawingSize)(LONG newVal);
	STDMETHOD(get_SourceType)(tkShapefileSourceType* pVal);
	STDMETHOD(BufferByDistance)(double Distance, LONG nSegments, VARIANT_BOOL SelectedOnly, VARIANT_BOOL MergeResults, IShapefile** sf);
	STDMETHOD(get_GeometryEngine)(tkGeometryEngine* pVal);
	STDMETHOD(put_GeometryEngine)(tkGeometryEngine pVal);
	STDMETHOD(Difference)(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval);
	STDMETHOD(Clip)(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval);
	STDMETHOD(SymmDifference)(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval);
	STDMETHOD(Union)(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval);
	STDMETHOD(ExplodeShapes)(VARIANT_BOOL SelectedOnly, IShapefile** retval);
	STDMETHOD(AggregateShapes)(VARIANT_BOOL SelectedOnly, LONG FieldIndex, IShapefile** retval);
	STDMETHOD(ExportSelection)(IShapefile** retval);
	STDMETHOD(Sort)(LONG FieldIndex, VARIANT_BOOL Ascending, IShapefile** retval);
	STDMETHOD(Merge)(VARIANT_BOOL SelectedOnlyThis, IShapefile* sf, VARIANT_BOOL SelectedOnly, IShapefile** retval);
	STDMETHOD(get_SelectionColor)(OLE_COLOR* retval);
	STDMETHOD(put_SelectionColor)(OLE_COLOR newVal);
	STDMETHOD(get_SelectionAppearance)(tkSelectionAppearance* retval);
	STDMETHOD(put_SelectionAppearance)(tkSelectionAppearance newVal);
	STDMETHOD(get_CollisionMode)(tkCollisionMode* retval);
	STDMETHOD(put_CollisionMode)(tkCollisionMode newVal);
	STDMETHOD(get_SelectionTransparency)(BYTE* retval);
	STDMETHOD(put_SelectionTransparency)(BYTE newVal);
	STDMETHOD(put_StopExecution)(IStopExecution* stopper);
	STDMETHOD(Serialize)(VARIANT_BOOL SaveSelection, BSTR* retVal);
	STDMETHOD(Serialize2)(VARIANT_BOOL SaveSelection, VARIANT_BOOL SerializeCategories, BSTR* retVal);
	STDMETHOD(Deserialize)(VARIANT_BOOL LoadSelection, BSTR newVal);
	STDMETHOD(get_GeoProjection)(IGeoProjection** retVal);
	STDMETHOD(put_GeoProjection)(IGeoProjection* pVal);
	STDMETHOD(Reproject)(IGeoProjection* newProjection, LONG* reprojectedCount, IShapefile** retVal);
	STDMETHOD(ReprojectInPlace)(IGeoProjection* newProjection, LONG* reprojectedCount, VARIANT_BOOL* retVal);
	STDMETHOD(SimplifyLines)(DOUBLE Tolerance, VARIANT_BOOL SelectedOnly, IShapefile** retVal);
	STDMETHOD(FixUpShapes)(IShapefile** retVal, VARIANT_BOOL* fixed);
	STDMETHOD(GetRelatedShapes)(long referenceIndex, tkSpatialRelation relation, VARIANT* resultArray, VARIANT_BOOL* retval);
	STDMETHOD(GetRelatedShapes2)(IShape* referenceShape, tkSpatialRelation relation, VARIANT* resultArray, VARIANT_BOOL* retval);
	STDMETHOD(get_HotTracking)(VARIANT_BOOL* retVal);
	STDMETHOD(put_HotTracking)(VARIANT_BOOL newVal);
	STDMETHOD(HasInvalidShapes)(VARIANT_BOOL* result);
	STDMETHOD(EditAddShape)(IShape* shape, long* shapeIndex);
	STDMETHOD(EditAddField)(BSTR name, FieldType type, int precision, int width, long* fieldIndex);
	STDMETHOD(GetClosestVertex)(double x, double y, double maxDistance, long* shapeIndex, long* pointIndex, double* distance, VARIANT_BOOL* retVal);
	STDMETHOD(get_ShapeCategory2)(long ShapeIndex, BSTR* categoryName);
	STDMETHOD(put_ShapeCategory2)(long ShapeIndex, BSTR categoryName);
	STDMETHOD(get_ShapeCategory3)(long ShapeIndex, IShapefileCategory** category);
	STDMETHOD(put_ShapeCategory3)(long ShapeIndex, IShapefileCategory* category);
	STDMETHOD(Dump)(BSTR ShapefileName, ICallback *cBack, VARIANT_BOOL *retval);
	STDMETHOD(LoadDataFrom)(BSTR ShapefileName, ICallback *cBack, VARIANT_BOOL *retval);
	STDMETHOD(Segmentize)(IShapefile** retVal);
	STDMETHOD(get_LastInputValidation)(IShapeValidationInfo** retVal);
	STDMETHOD(get_LastOutputValidation)(IShapeValidationInfo** retVal);
	STDMETHOD(ClearCachedGeometries)();
	STDMETHOD(AggregateShapesWithStats)(VARIANT_BOOL SelectedOnly, LONG FieldIndex, IFieldStatOperations* statOperations, IShapefile** retval);
	STDMETHOD(DissolveWithStats)(long FieldIndex, VARIANT_BOOL SelectedOnly, IFieldStatOperations* statOperations, IShapefile** sf);
	STDMETHOD(get_ShapeRotation)(long ShapeIndex, double* pVal);
	STDMETHOD(put_ShapeRotation)(long ShapeIndex, double newVal);
	STDMETHOD(get_ShapeVisible)(long ShapeIndex, VARIANT_BOOL* pVal);
	STDMETHOD(get_Volatile)(VARIANT_BOOL* retval);
	STDMETHOD(put_Volatile)(VARIANT_BOOL newVal);
	STDMETHOD(EditUpdateShape)(long shapeIndex, IShape* shpNew, VARIANT_BOOL* retVal);
	STDMETHOD(get_ShapeModified)(long ShapeIndex, VARIANT_BOOL* retVal);
	STDMETHOD(put_ShapeModified)(long ShapeIndex, VARIANT_BOOL newVal);
	STDMETHOD(Validate)(tkShapeValidationMode validationMode, VARIANT_BOOL selectedOnly, IShapeValidationInfo** results);
	STDMETHOD(get_UndoList)(IUndoList** pVal);
	STDMETHOD(get_InteractiveEditing)(VARIANT_BOOL* pVal);
	STDMETHOD(put_InteractiveEditing)(VARIANT_BOOL newVal);
	STDMETHOD(get_ShapeIsHidden)(LONG shapeIndex, VARIANT_BOOL* pVal);
	STDMETHOD(put_ShapeIsHidden)(LONG shapeIndex, VARIANT_BOOL newVal);
	STDMETHOD(get_Snappable)(VARIANT_BOOL* pVal);
	STDMETHOD(put_Snappable)(VARIANT_BOOL newVal);

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
	std::vector<PolygonShapefile> m_PolySF;
	
	// -------------------------------------------------------------
	//	private members
	// -------------------------------------------------------------
	tkShapefileSourceType m_sourceType;		// is it disk-based or in-memory?
	ShpfileType _shpfiletype;
	BSTR key;
	long lastErrorCode;
	
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
	FILE * _shpfile;
	FILE * _shxfile;

	CStringW _shpfileName;
	CStringW _shxfileName;
	CStringW _dbffileName;
	CStringW _prjfileName;
	
	std::vector<ShapeData*> _shapeData;
	std::vector<long> shpOffsets;		//(32 bit words)

	// table is initialized in CreateNew or Open methods
	// it is is destroyed in Close() method
	// in case table is null, shapefile will be considered uninitialized
	ITable * dbf;
	IGeoProjection* m_geoProjection;
	IStopExecution* _stopExecution;
	ICharts* m_charts;
	ILabels* m_labels;
	IShapefileCategories* m_categories;
	ICallback * globalCallback;
	IShapeDrawingOptions* m_selectDrawOpt;	
	IShapeDrawingOptions* m_defaultDrawOpt;
	IShapeValidationInfo* _inputValidation;
	IShapeValidationInfo* _outputValidation;
	IUndoList* _undoList;
	
	VARIANT_BOOL _hotTracking;
	bool _geosGeometriesRead;
	tkCollisionMode _collisionMode;		// collision mode for point shapefiles
	tkGeometryEngine _geometryEngine;		// GEOS or Clipper
	bool m_writing;		// is currently writing to the file
	bool m_reading;		// is currently reading data into memory
	
	BSTR _expression;		// visibility expression
	BOOL _isEditingShapes;		//Flag for Disk vs. Memory
	long _nextShapeHandle;		// the next unique handle to assign
	VARIANT_BOOL _interactiveEditing;
	VARIANT_BOOL _snappable;
	
	// When this flag is on CShapeWrapper will be used in the Shape class to store the points
	// otherwise usual COM points
	BOOL _fastMode;
	int _minDrawingSize;	// objects which are less than this value in pixels for current scale, will drawn as point
	BOOL cacheExtents;	// extents won't be recalculated in each get_Extents call
	bool _volatile;

	//Flags for Spatial Indexing
	BOOL useSpatialIndex;
	BOOL hasSpatialIndex;
	IndexSearching::CSpatialIndexID spatialIndexID;
	BOOL spatialIndexLoaded;	
	DOUBLE spatialIndexMaxAreaPercent;
	int spatialIndexNodeCapacity;
	
	// drawing options
	tkSelectionAppearance _selectionAppearance;
	OLE_COLOR _selectionColor;
	unsigned char _selectionTransparency;

	//Neio (07/21/2009) shapes are stored in QTree (EDIT MODE)
	QTree* m_qtree;
	BOOL useQTree;
	vector<int> deleteElement;
	
	// during geoprocessing operations only
	QTree* _tempTree;
	
	bool _useValidationList;

	// -------------------------------------------------------------
	//	private functions
	// -------------------------------------------------------------
	BOOL ReleaseMemoryShapes();
	BOOL verifyMemShapes(ICallback * cBack);
	long FindNewShapeID(long FieldIndex);

	//Read Write Functions
	BOOL readShx();
	BOOL writeShx(FILE * _shxfile, ICallback * cBack);
	BOOL writeShp(FILE * shpfile, ICallback * cBack);	

	// selection Functions
	//BOOL defineShapeBounds(long ShapeIndex, ShpfileType & ShapeType, double &s_minX, double &s_minY, double &s_maxX, double &s_maxY );	
	BOOL defineShapePoints(long ShapeIndex, ShpfileType & ShapeType, std::vector<long> & parts, std::vector<double> & xPts, std::vector<double> & yPts );
	BOOL pointInPolygon( long ShapeIndex, double x, double y );
	BOOL get_MemShapePoints(IShape* shape, ShpfileType & ShapeType, std::vector<long> & parts, std::vector<double> & xPts, std::vector<double> & yPts);
	
	
	//Neio 2009 07 22
	void GenerateQTree();
	void TrimMemShapes();
	
	bool UniqueFieldNames(IShapefile* sf);
	
	QTree* GenerateQTreeCore(bool SelectedOnly);
	bool GenerateTempQTree(bool SelectedOnly);
	void ClearTempQTree();
	QTree* GetTempQTree();
	VARIANT_BOOL SelectShapesAlt(IExtents *BoundBox, double Tolerance, SelectMode SelectMode, VARIANT* arr);
	void put_ReferenceToLabels(bool bNullReference = false);
	void put_ReferenceToCategories(bool bNullReference = false);
	void put_ReferenceToCharts(bool bNullReference = false);
	int get_OuterRingIndex(int ShapeIndex, int PartIndex);

	void GetRelatedShapeCore(IShape* referenceShape, long referenceIndex, tkSpatialRelation relation, VARIANT* resultArray, VARIANT_BOOL* retval);
	void ApplyRandomDrawingOptions();

	// -------------------------------------------------
	// Geoprocessing
	// -------------------------------------------------
	#pragma region Geoprocessing
	void DissolveClipper(long FieldIndex, VARIANT_BOOL SelectedOnly, IFieldStatOperations* operations, IShapefile* sf);
	void DissolveGEOS(long FieldIndex, VARIANT_BOOL SelectedOnly, IFieldStatOperations* operations, IShapefile* sf);

	
	void DoClipOperation(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, 
									 VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval, 
									 tkClipOperation operation, ShpfileType returnType = SHP_NULLSHAPE);

	// intersection
	void IntersectionGEOS(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL SelectedOnlyClip, 
									  IShapefile* sfResult, map<long, long>* fieldMap = NULL, 
									  std::set<int>* subjectShapesToSkip = NULL,  std::set<int>* clippingShapesToSkip = NULL );

	void IntersectionClipper(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL SelectedOnlyClip, 
										 IShapefile* sfResult, map<long, long>* fieldMap = NULL,
										 std::set<int>* subjectShapesToSkip = NULL,  std::set<int>* clippingShapesToSkip = NULL );

	IShapefile* IntersectionClipperNoAttributes(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL SelectedOnlyClip );

	// difference
	void DifferenceGEOS(IShapefile* sfSubject, VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay,  
									IShapefile* sfResult, map<long, long>* fieldMap = NULL, std::set<int>* shapesToSkip = NULL);
	void DifferenceClipper(IShapefile* sfSubject, VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL SelectedOnlyClip, 
									IShapefile* sfResult, map<long, long>* fieldMap = NULL, std::set<int>* shapesToSkip = NULL);

	// clip
	void ClipGEOS(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile* sfResult);
	void ClipClipper(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile* sfResult);
	
	// dissolve
	void AggregateShapesCore(VARIANT_BOOL SelectedOnly, LONG FieldIndex, IFieldStatOperations* statOperations, IShapefile** retval);
	void DissolveCore(long FieldIndex, VARIANT_BOOL SelectedOnly, IFieldStatOperations* statOperations, IShapefile** sf);
	void CalculateFieldStats(map<int, vector<int>*>& indicesMap, IFieldStatOperations* operations, IShapefile* output);

	// utilities
	void InsertShapesVector(IShapefile* sf, vector<IShape* >& vShapes, 
									IShapefile* sfSubject, long subjectId, std::map<long, long>* fieldMapSubject = NULL,
									IShapefile* sfClip = NULL, long clipId = -1, std::map<long, long>* fieldMapClip = NULL);

public:
	// -------------------------------------------------------------
	//	public functions
	// -------------------------------------------------------------
	std::vector<ShapeData*>* get_ShapeVector();
	IShapeWrapper* get_ShapeWrapper(int ShapeIndex);
	IShapeData* get_ShapeData(int ShapeIndex);
	void SetValidationInfo(IShapeValidationInfo* info, tkShapeValidationType validationType);
	
	bool getClosestPoint(double x, double y, double maxDistance, std::vector<long>& ids, long* shapeIndex, long* pointIndex, double& dist);

	bool DeserializeCore(VARIANT_BOOL LoadSelection, CPLXMLNode* node);
	CPLXMLNode* SerializeCore(VARIANT_BOOL SaveSelection, CString ElementName, bool serializeCategories);

	void CopyFields(IShapefile* target);
	void UpdateLabelsPositioning();
	bool OpenCore(CStringW tmp_shpfileName, ICallback* cBack);
	FILE* get_File(){ return _shpfile;}
	void AddPolygonsToClipper(ClipperLib::Clipper& clp, ClipperLib::PolyType clipType, bool selectedOnly);
	
	void CopyFields(IShapefile* source, IShapefile* target);
	
	void CopyFields(IShapefile* sfSubject, IShapefile* sfOverlay, IShapefile* sfResult, map<long, long>& fieldMap, bool mergeFields = false);
	bool FieldsAreEqual(IField* field1, IField* field2);

	bool ReprojectCore(IGeoProjection* newProjection, LONG* reprojectedCount, IShapefile** retVal, bool reprojectInPlace);
	
	void CloneNoFields(IShapefile** retVal, bool addShapeId = false);
	void CloneNoFields(IShapefile** retVal, ShpfileType shpType, bool addShapeId = false);
	
	void ErrorMessage(long ErrorCode);
	void ErrorMessage(long ErrorCode, ICallback* cBack);
	int get_ShapeCategory(int ShapeIndex);
	
	void SetChartsPositions(tkLabelPositioning Method);
	bool ReadChartFields(std::vector<double*>* values);
	bool ReadChartField(std::vector<double>* values, int FieldIndex);
	void ClearChartFrames();
	void ReadGeosGeometries(VARIANT_BOOL selectedOnly);
	bool SelectShapesCore(Extent& extents, double Tolerance, SelectMode SelectMode, std::vector<long>& selectResult);
	bool get_CanUseSpatialIndexCore(Extent& extents);
	HRESULT CreateNewCore(BSTR ShapefileName, ShpfileType ShapefileType, bool applyRandomOptions, VARIANT_BOOL *retval);
	bool QuickExtentsCore(long ShapeIndex, Extent& result);
	bool QuickExtentsCore(long ShapeIndex, double* xMin, double* yMin, double* xMax, double* yMax);
	bool PolygonIntersection(std::vector<double>& xPts, std::vector<double>& yPts, std::vector<long>& parts,
						 double& b_minX, double& b_maxX, double& b_minY, double& b_maxY, double& Tolerance, int& shapeVal);
	bool PolylineIntersection(std::vector<double>& xPts, std::vector<double>& yPts, std::vector<long>& parts,
						  double& b_minX, double& b_maxX, double& b_minY, double& b_maxY, double& Tolerance);

	void RegisterNewShape(IShape* Shape, long ShapeIndex);
	
	// validation
	void CreateValidationList(bool selectedOnly);
	void ClearValidationList();
	HRESULT GetValidatedShape(int shapeIndex, IShape** retVal);
	void SetValidatedShape(int shapeIndex, ShapeValidationStatus status, IShape* shape = NULL);
	bool ValidateInput(IShapefile* isf, CString methodName, CString parameterName, 
		VARIANT_BOOL selectedOnly, CString className = "Shapefile");
	IShapeValidationInfo* ValidateInputCore(IShapefile* isf, CString methodName, CString parameterName,
		VARIANT_BOOL selectedOnly, tkShapeValidationMode validationMode, CString className = "Shapefile", bool reportOnly = false);
	IShapeValidationInfo* ValidateOutput(IShapefile** isf, CString methodName, CString className = "Shapefile", bool abortIfEmpty = true);
	bool ValidateOutput(IShapefile* sf, CString methodName, CString className= "Shapefile", bool abortIfEmpty = true);
	bool ShapeAvailable(int shapeIndex, VARIANT_BOOL selectedOnly);
	GEOSGeometry* GetGeosGeometry(int shapeIndex);
	void CloneCore(IShapefile** retVal, ShpfileType shpType, bool addShapeId = false);
	Coloring::ColorGraph* GeneratePolygonColors();
	bool PointWithinShape(IShape* shape, double projX, double projY, double Tolerance);
	void ReregisterShape(int shapeIndex);
	bool ForceProperShapeType(ShpfileType sfType, IShape* shape);
};
OBJECT_ENTRY_AUTO(__uuidof(Shapefile), CShapefile)
