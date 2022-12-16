// ReSharper disable CppInconsistentNaming
# ifndef ENUMERATIONS_H
# define ENUMERATIONS_H

enum ShapeLayerFlags
{	sfVisible = 1,
	slfDrawFill = 2,
	slfDrawLine = 4,
	slfDrawPoint = 8,	
};

enum LayerType
{	ImageLayer,
	ShapefileLayer,
	UndefinedLayer,
	OgrLayerSource,
	WmsLayerSource,
};

enum LayerFlags
{	Visible = 1
};
    
enum tkFontStyle
{
    fstRegular = 0,
	fstBold = 1,
    fstItalic = 2,
    fstUnderline = 4,
    fstStrikeout = 8,
};

enum tkSimpleShapeType
{
    shpNone = 0,
	shpPoint = 1,
    shpPolyline = 2,
    shpPolygon = 3,
    shpMultiPoint = 4,
};

enum class tkExtentsRelation
{
	erNone = 0,
	erEqual = 1,
	erInclude = 2,
	erIsIncluded = 3,
	erIntersection = 4
};

enum tkImageDrawingMethod
{
	idmNewWithResampling = 1,	// new drawing procedures will be used
    idmGDIPlusDrawing = 2,		// GDIPlus rather then GDI drawing will be used
    idmGDIPlusResampling = 4,		// Resampling for GDAL based images will be made by GDI+, otherwise it'll be carried out in tkRaster.cpp (ImageRampling.cpp)
};

struct Point2D 
{
	double x, y;
	Point2D() : x(0.0), y(0.0) {}
	Point2D(double x, double y): x(x), y(y)
	{		
	}

	double GetDistance(Point2D& other) 
	{
		return sqrt(pow(x - other.x, 2.0) + pow(y - other.y, 2.0));
	}

	double GetDistance(double x2, double y2) 
	{
		return sqrt(pow(x - x2, 2.0) + pow(y - y2, 2.0));
	}
};

struct PointWithId: Point2D
{
	double x, y;
	int id;
	PointWithId() : x(0.0), y(0.0), id(0) {}
	PointWithId(double x, double y, int id) : x(x), y(y), id(id) {}
};

enum tkTransformationMode
{
	tmNotDefined = 0,
	tmWgs84Complied = 1,
	tmDoTransformation = 2,
};
enum CacheType 
{ 
	// must match tkCacheType enumeration
	tctRamCache = 0,
	tctSqliteCache = 1, 
	tctDiskCache = 2, 
};

enum TileLoaderType
{
	tltMapLoader = 0,
	tltBulkLoader = 1,
};

enum HandleImage
{ 
	asRGB=0,      // byte; 3 bands
	asGrid=1,     // single band with color scheme
	asFloatOrInt=2   // int32 or float
};

enum class ShapeValidityCheck
{
	NoPoints = 0,
	NotEnoughPoints = 1,
	NoParts = 3,
	EmptyParts = 4,
	FirstAndLastPointOfPartMatch = 5,
	DirectionOfPolyRings = 6,
};

enum ShapeValidationStatus
{
	Original = 0,
	Fixed = 1,
	Skip = 2,
};

enum GdalSupport
{
	GdalSupportNone = 0,
	GdalSupportRgb = 1,
	GdalSupportGrid = 2
};

enum tkDrawingDataAvailable
{
	MeasuringData = 0,
	Coordinates = 1,
	TilesData = 2,
	LayersData = 3,
	HotTracking = 4,
	ZoomingAnimation = 5,
	PanningInertia = 6,
	ActShape = 7,
	ShapeDigitizing = 8,
	ZoomBox = 9,
	MovingShapes = 10,
	IdentifiedShapes = 11,
	FocusRect = 12,
};

enum SqliteOpenMode
{
	OpenIfExists = 0,
	OpenOrCreate = 1,
};

enum ZoombarPart
{
	ZoombarNone = 0,
	ZoombarPlus = 1,
	ZoombarMinus = 2,
	ZoombarHandle = 3,
	ZoombarBar = 4,
};

enum DraggingOperation
{
	DragNone = 0,
	DragPanning = 1,
	DragZoombarHandle = 2,
	DragZoombox = 3,
	DragMoveShape = 4,
	DragMoveVertex = 5,
	DragMovePart = 6,
	DragMoveShapes = 7,
	DragRotateShapes = 8,
	DragSelectionBox = 9
};

enum TileProjectionState
{
	ProjectionMatch = 0,
	ProjectionDoTransform = 1,
	ProjectionCompatible = 2,	// WGS84 map; GMercator server; transformation is still necessary but at least results are guaranteed
};

enum class ShapeInputMode
{
	simMeasuring = 0,
	simEditing = 1,
};

enum LayerSelector
{
	slctShapefiles = 0,
	slctInMemorySf = 1,
	slctHotTracking = 2,
	slctInteractiveEditing = 3,
	slctIdentify = 4,
};

enum MouseTolerance
{
	ToleranceSelect = 0,
	ToleranceInsert = 1,
	ToleranceSnap = 2,
};

enum SubjectOperation
{
	SubjectAddPart = 0,
	SubjectClip = 1,
	SubjectSplit = 2,
};

enum PointPart
{
	PartNone = 0,
	PartBegin = 1,
	PartEnd = 2,
};

enum EditorDeleteResult
{
	DeleteNone = 0,
	DeleteShape = 1,
	Delete
};

enum HotTrackingResult
{
	NewShape = 0,
	SameShape = 1,
	NoShape = 2,
};

enum RedrawTarget
{
	rtShapeEditor = 0,
	rtVolatileLayer = 1,
};

enum DebugTarget
{
	DebugAny = 0,
	DebugOgrLoading = 1,
	DebugPanning = 2,
	DebugTiles = 3,
	DebugForceGdal = 4,
};

enum OpenFileDialogFilter
{
	FilterImage = 0, 
	FilterGrid = 1, 
	FilterOgr = 2,
	FilterShapefile = 3,
	FilterAll = 4,
};

enum GdalPath
{
	PathGdalData = 0,
	PathGdalPlugins =1,
	PathProjLib = 2
};

enum BandChannel
{
	BandChannelRed = 1,
	BandChannelGreen = 2,
	BandChannelBlue = 3,
	BandChannelAlpha = 4,
};

enum HistogramState
{
	HistogramNotComputed = 0,
	HistogramFailed = 1,
	HistogramSuccess = 2,
};

enum FunctionId
{
	fnGeometryToWkt,
	fnArea,
	fnLength,
	fnPerimeter,
	fnX,
	fnY,
	fnXat,
	fnYat,
	fnIf,
	fnToInt,
	fnToReal,
	fnToString,
	fnLower,
	fnUpper,
	fnTitleCase,
	fnTrim,
	fnLen,
	fnReplace,
	fnSubstr,
	fnSubstr2,
	fnConcat,
	fnStrpos,
	fnLeft,
	fnRight,
	fnLPad,
	fnRPad,
	fnSqrt,
	fnAbs,
	fnCos,
	fnSin,
	fnTan,
	fnAsin,
	fnAcos,
	fnAtan,
	fnAtan2,
	fnExp,
	fnLn,
	fnLog10,
	fnLog,
	fnRound,
	fnRound2,
	fnRand,
	fnRandf,
	fnMax,
	fnMin,
	fnClamp,
	fnFloor,
	fnCeil,
	fnPi
};

enum tkShapeRecordFlags
{
	shpSelected = 1,
	shpVisible = 2,			// because of the visibility expression 		
	shpHidden = 4,			// set per shape explicitly	
	shpModified = 8,		// for saving of OGR layers
	shpWasRendered = 16,
};

enum class TileHttpContentType
{
	httpUndefined = 0,
	httpImage = 1,
	httpXml = 2,
};

# endif
