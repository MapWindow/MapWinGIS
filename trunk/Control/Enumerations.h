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

enum tkExtentsRelation
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
	SqliteCache = 0, 
	DiskCache = 1, 
};

enum HandleImage
{ 
	asRGB=0,
	asGrid=1,
	asComplex=2
};

enum ShapeValidityCheck
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
	ShapeEditing = 8,
	ZoomBox = 9,
	MovingShapes = 10,
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

enum ShapeInputMode
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

# endif