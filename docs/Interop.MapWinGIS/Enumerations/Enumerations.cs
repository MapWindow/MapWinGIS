// An example of comments for enumerated constants

// ReSharper disable InconsistentNaming
#if nsp

namespace MapWinGIS
{
#endif

    /// <summary>
    /// Defines the way to resolve ambiguity resolution for shape networks.
    /// </summary>
    public enum AmbiguityResolution
    {
        Z_VALUE = 0,
        DISTANCE_TO_OUTLET = 1,
        NO_RESOLUTION = 2,
    }

    /// <summary>
    /// Defines the type of coloring for grids.
    /// </summary>
    public enum ColoringType
    {
        Hillshade = 0,
        Gradient = 1,
        Random = 2,
    }

    /// <summary>
    /// The available types of fields of dbf table.
    /// </summary>
    public enum FieldType
    {
        STRING_FIELD = 0,
        INTEGER_FIELD = 1,
        DOUBLE_FIELD = 2,
        BOOLEAN_FIELD = 3,
        DATE_FIELD = 4
    }

    /// <summary>
    /// Gradient model for grid visualization
    /// </summary>
    public enum GradientModel
    {
        Logorithmic = 0,
        Linear = 1,
        Exponential = 2,
    }

    /// <summary>
    /// The data type which represents a single cell of a grid.
    /// </summary>
    public enum GridDataType
    {
        InvalidDataType = -1,
        ShortDataType = 0,
        LongDataType = 1,
        FloatDataType = 2,
        DoubleDataType = 3,
        UnknownDataType = 4,
        ByteDataType = 5,
    }

    /// <summary>
    /// The type of grid supported by MapWinGIS.
    /// </summary>
    public enum GridFileType
    {
        InvalidGridFileType = -1,
        Ascii = 0,
        Binary = 1,
        Esri = 2,
        GeoTiff = 3,
        Sdts = 4,
        PAux = 5,
        PCIDsk = 6,
        DTed = 7,
        Bil = 8,
        Ecw = 9,
        MrSid = 10,
        Flt = 11,
        Dem = 12,
        UseExtension = 13,
    }

    /// <summary>
    /// The type of images supported by MapWinGIS.
    /// </summary>
    public enum ImageType
    {
        BITMAP_FILE = 0,
        GIF_FILE = 1,
        USE_FILE_EXTENSION = 2,
        TIFF_FILE = 3,
        JPEG_FILE = 4,
        PNG_FILE = 5,
        PPM_FILE = 7,
        ECW_FILE = 8,
        JPEG2000_FILE = 9,
        SID_FILE = 10,
        PNM_FILE = 11,
        PGM_FILE = 12,
        BIL_FILE = 13,
        ADF_FILE = 14,
        GRD_FILE = 15,
        IMG_FILE = 16,
        ASC_FILE = 17,
        BT_FILE = 18,
        MAP_FILE = 19,
        LF2_FILE = 20,
        KAP_FILE = 21,
        DEM_FILE = 22,
    }

    /// <summary>
    /// Available polygon operations.
    /// </summary>
    public enum PolygonOperation
    {
        DIFFERENCE_OPERATION = 0,
        INTERSECTION_OPERATION = 1,
        EXCLUSIVEOR_OPERATION = 2,
        UNION_OPERATION = 3,
    }

    /// <summary>
    /// Predefined color schemes which can be used for grid visualization of for initialization of instance of ColorScheme class.
    /// </summary>
    public enum PredefinedColorScheme
    {
        FallLeaves = 0,
        SummerMountains = 1,
        Desert = 2,
        Glaciers = 3,
        Meadow = 4,
        ValleyFires = 5,
        DeadSea = 6,
        Highway1 = 7,
        Rainbox = 8,
        ReversedRainbow = 9,
    }

    /// <summary>
    /// The selection mode, which determines which shapes will be considered as included in the rectangular selection.
    /// </summary>
    public enum SelectMode
    {
        /// <summary>
        /// All the shapes that lie within the rectangle or do intersect it will be included.
        /// </summary>
        INTERSECTION = 0,

        /// <summary>
        /// Only the shapes that lie within the rectangle and doesn't intersect it will be included.
        /// </summary>
        INCLUSION = 1,
    }

    /// <summary>
    /// The type of the shapefile.
    /// </summary>
    /// <remarks>The main types are SHP_NULLSHAPE, SHP_POINT, SHP_POLYLINE, SHP_POLYGON.
    /// SHP_MULTIPOINT is much harder to encounter. SHP_MULTIPATCH isn't supported by MapWinGIS\n
    /// For each of the flat (2D) type there is a 2.5D representation, with either Z (holds z value) or M (holds z and m values) postfix.
    /// See shapefile specification for details: http://www.esri.com/library/whitepapers/pdfs/shapefile.pdf</remarks>
    public enum ShpfileType
    {
        SHP_NULLSHAPE = 0,
        SHP_POINT = 1,
        SHP_POLYLINE = 3,
        SHP_POLYGON = 5,
        SHP_MULTIPOINT = 8,
        SHP_POINTZ = 11,
        SHP_POLYLINEZ = 13,
        SHP_POLYGONZ = 15,
        SHP_MULTIPOINTZ = 18,
        SHP_POINTM = 21,
        SHP_POLYLINEM = 23,
        SHP_POLYGONM = 25,
        SHP_MULTIPOINTM = 28,
        SHP_MULTIPATCH = 31,
    }

    /// <summary>
    /// Split methods used for creation of TIN from grid.
    /// </summary>
    public enum SplitMethod
    {
        InscribedRadius = 0,
        AngleDeviation = 1,
    }

    /// <summary>
    /// The available types of chart.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkChartType
    {
        /// <summary>
        /// Each chart field is represented by bar.
        /// </summary>
        chtBarChart = 0,

        /// <summary>
        /// Each chart field is represented by sector.
        /// </summary>
        chtPieChart = 1,
    }

    /// <summary>
    /// The style of labels with numeric measures of the data for each field.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkChartValuesStyle
    {
        vsHorizontal = 0,
        vsVertical = 1,
    }

    /// <summary>
    /// The type of the classification available for ShapefileCategories.Generate and Labels.GenerateCategories methods.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkClassificationType
    {
        /// <summary>
        /// The position of breaks are chosen to minimize the sum of square deviations within categories.
        /// The algorithm is heuristic, therefore gives only approximation of the real position of natural breaks.
        /// Nevertheless it's recommended as a default classification suitable in most cases.
        /// </summary>
        ctNaturalBreaks = 0,

        /// <summary>
        /// A category will be created for each unique value of the field.
        /// </summary>
        ctUniqueValues = 1,

        /// <summary>
        /// To define the upper bound of a category the expression will be used: (IndexOfCategory + 1) * (MaxValue - MinValue) / NumberOfClasses.
        /// </summary>
        ctEqualIntervals = 2,

        /// <summary>
        /// The bounds of categories will be chosen in such a manner that each of them will have equal number of objects which belongs to it.
        /// In practice this condition is rarely achieved because of the rounding problems, objects with the same values, etc.
        /// </summary>
        ctEqualCount = 3,

        /// <summary>
        /// Six categories will be created, the range of each of them will be equal to the standard deviation of the field data. 
        /// Three categories will hold values greater then mean and three - lower than it.
        /// </summary>
        ctStandardDeviation = 4,

        /// <summary>
        /// The sum values of the classification field will be roughly the same for all categories.
        /// </summary>
        ctEqualSumOfValues = 5,
    }

    /// <summary>
    /// The available clipping operation for Shape.Clip method.
    /// </summary>
    /// <remarks>The clClip value will be yield the same result as clIntersection.</remarks>
    /// \new48 Added in version 4.8
    public enum tkClipOperation
    {
        clDifference = 0,
        clIntersection = 1,
        clSymDifference = 2,
        clUnion = 3,
        clClip = 4,
    }

    /// <summary>
    /// Sets drawing behaviour when overlapping labels and charts are present on map.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkCollisionMode
    {
        /// <summary>
        /// Overlaps of labels (charts) are allowed. The subsequent labels (charts) will be drawn atop of the prior ones, thus hiding them.
        /// </summary>
        AllowCollisions = 0,

        /// <summary>
        /// The overlapping labels (charts) on the same layer are not allowed.
        /// The overlapping labels (charts) on different layers are allowed.
        /// </summary>
        LocalList = 1,

        /// <summary>
        /// No overlapping labels (charts) are allowed for both inside the layer and across the layers.
        /// </summary>
        GlobalList = 2,
    }

    /// <summary>
    /// The type of color scheme. Determines how colors will be extracted from the color scheme (see ShapefileCategories.ApplyColorScheme for example).
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkColorSchemeType
    {
        /// <summary>
        /// The color will be chosen randomly from the available range between two color breaks.
        /// </summary>
        ctSchemeRandom = 0,

        /// <summary>
        /// The color will be chosen by linear interpolation from the available range between two color breaks.
        /// </summary>
        ctSchemeGraduated = 1,
    }

    /// <summary>
    /// The available mouse cursors provided by the system.
    /// </summary>
    public enum tkCursor
    {
        crsrMapDefault = 0,
        crsrAppStarting = 1,
        crsrArrow = 2,
        crsrCross = 3,
        crsrHelp = 4,
        crsrIBeam = 5,
        crsrNo = 6,
        crsrSizeAll = 7,
        crsrSizeNESW = 8,
        crsrSizeNS = 9,
        crsrSizeNWSE = 10,
        crsrSizeWE = 11,
        crsrUpArrow = 12,
        crsrWait = 13,
        crsrUserDefined = 14,
    }

    /// <summary>
    /// Available cursor modes. Determines the default respond of map to the action of user.
    /// </summary>
    /// <remarks>To define the new behaviour set the value to cmNone and handle the events provided by map controls.</remarks>
    public enum tkCursorMode
    {
        /// <summary>
        /// The map will be zoomed in on mouse click. 
        /// </summary>
        cmZoomIn = 0,

        /// <summary>
        /// The map will be zoomed out on mouse click. 
        /// </summary>
        cmZoomOut = 1,

        /// <summary>
        /// The panning of the map will be performed after the dragging operation by the mouse.
        /// </summary>
        cmPan = 2,

        /// <summary>
        /// Rubber band rectangular will be displayed on the dragging operation by the mouse. SelectBoxFinal event will be generated on he release of button.
        /// </summary>
        cmSelection = 3,

        /// <summary>
        /// The map will not respond to the user action.
        /// </summary>
        cmNone = 4,

        /// <summary>
        /// Measuring of area or distance will take place. See Measuring class for details.
        /// </summary>
        cmMeasure = 5,

        /// <summary>
        /// Allows to add shapes to vector layers (Shapefile or OgrLayer) in interactive mode. 
        /// See description of ShapeEditor class.
        /// </summary>
        cmAddShape = 6,

        /// <summary>
        /// Allows to edit shapes of vector layers (Shapefile or OgrLayer) in interactive mode. 
        /// See description of ShapeEditor class.
        /// </summary>
        cmEditShape = 8,

        /// <summary>
        /// Splits polylines or polygons of a single layers with interactive drawn polyline.
        /// AxMap.ChooseLayer event must be handled to select the layer.
        /// </summary>
        cmSplitByPolyline = 9,

        /// <summary>
        /// Fires AxMap.ShapeHighlighted and AxMap.ShapeIdentified events. Optionally visually 
        /// highlights shapes under cursor. See Identifier class for available options. 
        /// The functioning can be tested in the Demo app.
        /// </summary>
        cmIdentify = 10,

        /// <summary>
        /// Allows to moves selected shapes of a single vector layers. AxMap.ChooseLayer event must be handled to 
        /// select the layer.
        /// </summary>
        cmMoveShapes = 11,

        /// <summary>
        /// Allows to rotates selected shapes of a single vector layers. AxMap.ChooseLayer event must be handled to 
        /// select the layer.
        /// </summary>
        cmRotateShapes = 12,

        /// <summary>
        /// Selects shapes of a single vector layer with interactively drawn polygon. 
        /// AxMap.ChooseLayer event must be handled to select the layer.
        /// </summary>
        cmSelectByPolygon = 13,

        /// <summary>
        /// Erases parts of shapes shapes of a single vector that intersects with interactive drawn polygon.
        /// AxMap.ChooseLayer event must be handled to select the layer.
        /// </summary>
        cmEraseByPolygon = 14,

        /// <summary>
        /// Splits polygons or polylines of a single layer with interactively drawn polygon.
        /// AxMap.ChooseLayer event must be handled to select the layer.
        /// </summary>
        cmSplitByPolygon = 15,

        /// <summary>
        /// Removes all shapes of a selected layers which don't intersect with interactively drawn polygon.
        /// AxMap.ChooseLayer event must be handled to select the layer.
        /// </summary>
        cmClipByPolygon = 16,
    }

    /// <summary>
    /// The available style of lines. Can be used for drawing polylines and outlines of the polygons.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkDashStyle
    {
        dsSolid = 0,
        dsDash = 1,
        dsDot = 2,
        dsDashDot = 3,
        dsDashDotDot = 4,

        /// <summary>
        /// The value will be ignored. The default dsSolid will be used instead.
        /// </summary>
        dsCustom = 5,
    }

    /// <summary>
    /// The set of available point symbols. They represent macros for modification of several properties of the ShapeDrawingOptions class at once.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkDefaultPointSymbol
    {
        dpsSquare = 0,
        dpsCircle = 1,
        dpsDiamond = 2,
        dpsTriangleUp = 3,
        dpsTriangleDown = 4,
        dpsTriangleLeft = 5,
        dpsTriangleRight = 6,
        dpsCross = 7,
        dpsXCross = 8,
        dpsStar = 9,
        dpsPentagon = 10,
        dpsArrowUp = 11,
        dpsArrowDown = 12,
        dpsArrowLeft = 13,
        dpsArrowRight = 14,
        dpsAsterisk = 15,
        dpsFlag = 16,
    }

    /// <summary>
    /// The type of spatial reference for the drawing layer.
    /// </summary>
    public enum tkDrawReferenceList
    {
        /// <summary>
        /// The objects of the drawing layer are specified in screen coordinates and are not moved after the changes of the extents.
        /// </summary>
        /// <remarks>To update such layers the full redraw is not needed. Therefore use AxMap.Redraw2 rather than AxMap.Redraw.</remarks>
        dlScreenReferencedList = 0,

        /// <summary>
        /// The objects of the drawing layer are specified in map coordinates. Their position changes after zooming operations.
        /// </summary>
        /// <remarks>To update such layers the full redraw is needed (AxMap.Redraw()).</remarks>
        dlSpatiallyReferencedList = 1,
    }

    /// <summary>
    /// Deprecated. The available types of fill stipple. 
    /// </summary>
    /// \deprecated v.4.8. Use properties which take tkGDIPlusHatchStyle enumeration as input instead.
    public enum tkFillStipple
    {
        fsNone = 0,
        fsVerticalBars = 1,
        fsHorizontalBars = 2,
        fsDiagonalDownRight = 3,
        fsDiagonalDownLeft = 4,
        fsPolkaDot = 5,
        fsCustom = 6,
        fsCross = 7,
        fsRaster = 8,
    }

    /// <summary>
    /// The type of shape fill.
    /// </summary>
    /// \see ShapeDrawingOptions.FillType
    /// \new48 Added in version 4.8
    public enum tkFillType
    {
        /// <summary>
        /// Solid fill.
        /// </summary>
        ftStandard = 0,

        /// <summary>
        /// Hatched fill. See ShapeDrawingOptions.FillHatchStyle.
        /// </summary>
        ftHatch = 1,

        /// <summary>
        /// Gradient fill. See ShapeDrawingOptions.FillGradientType.
        /// </summary>
        ftGradient = 2,

        /// <summary>
        /// A texture fill. See ShapeDrawingOptions.Picture.
        /// </summary>
        ftPicture = 3,
    }

    /// <summary>
    /// The resampling mode which is used for building GDAL overviews, controlling the downsampling method applied.
    /// </summary>
    /// \see GdalUtils.GdalBuildOverviews
    /// \new48 Added in version 4.8
    public enum tkGDALResamplingMethod
    {
        grmNone = 0,
        grmNearest,
        grmGauss,
        grmCubic,
        grmCubicSpline,
        grmAverage,
        grmAverageMagphase,
        grmBilinear,
        grmLanczos,
        grmMode
    }

    /// <summary>
    /// The hatch style available for drawing of fill for polygons and point symbols.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkGDIPlusHatchStyle
    {
        hsNone = -1,
        hsHorizontal = 0,
        hsVertical = 1,
        hsForwardDiagonal = 2,
        hsBackwardDiagonal = 3,
        hsCross = 4,
        hsDiagonalCross = 5,
        hsPercent05 = 6,
        hsPercent10 = 7,
        hsPercent20 = 8,
        hsPercent25 = 9,
        hsPercent30 = 10,
        hsPercent40 = 11,
        hsPercent50 = 12,
        hsPercent60 = 13,
        hsPercent70 = 14,
        hsPercent75 = 15,
        hsPercent80 = 16,
        hsPercent90 = 17,
        hsLightDownwardDiagonal = 18,
        hsLightUpwardDiagonal = 19,
        hsDarkDownwardDiagonal = 20,
        hsDarkUpwardDiagonal = 21,
        hsWideDownwardDiagonal = 22,
        hsWideUpwardDiagonal = 23,
        hsLightVertical = 24,
        hsLightHorizontal = 25,
        hsNarrowVertical = 26,
        hsNarrowHorizontal = 27,
        hsDarkVertical = 28,
        hsDarkHorizontal = 29,
        hsDashedDownwardDiagonal = 30,
        hsDashedUpwardDiagonal = 31,
        hsDashedHorizontal = 32,
        hsDashedVertical = 33,
        hsSmallConfetti = 34,
        hsLargeConfetti = 35,
        hsZigZag = 36,
        hsWave = 37,
        hsDiagonalBrick = 38,
        hsHorizontalBrick = 39,
        hsWeave = 40,
        hsPlaid = 41,
        hsDivot = 42,
        hsDottedGrid = 43,
        hsDottedDiamond = 44,
        hsShingle = 45,
        hsTrellis = 46,
        hsSphere = 47,
        hsSmallGrid = 48,
        hsSmallCheckerBoard = 49,
        hsLargeCheckerBoard = 50,
        hsOutlinedDiamond = 51,
        hsSolidDiamond = 52,
    }

    /// <summary>
    /// The list of parameters of the geographical coordinate system.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkGeogCSParameter
    {
        /// <summary>
        /// The half of the length of the major axis of reference ellipsoid.
        /// </summary>
        SemiMajor = 0,

        /// <summary>
        /// The half of the length of the minor axis of reference ellipsoid.
        /// </summary>
        SemiMinor = 1,

        /// <summary>
        /// The inverse flattening for the reference ellipsoid
        /// </summary>
        InverseFlattening = 2,

        /// <summary>
        /// The prime meridian for geographic coordinate system.
        /// </summary>
        PrimeMeridian = 3,

        /// <summary>
        /// The angular units for geographic coordinate system.
        /// </summary>
        AngularUnit = 4,
    }

    /// <summary>
    /// The possible engines (libraries) to carry out geoprocessing tasks.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkGeometryEngine
    {
        /// <summary>
        /// GEOS library. From version 4.8. linked statically (included in MapWinGIS).
        /// </summary>
        engineGeos = 0,

        /// <summary>
        /// Clipper library. Linked statically (included in MapWinGIS).
        /// </summary>
        engineClipper = 1,
    }

    /// <summary>
    /// Possible size of gradient for the shapefile layer.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkGradientBounds
    {
        /// <summary>
        /// A single gradient will be applied for all shapes of the shapefile.
        /// </summary>
        gbWholeLayer = 0,

        /// <summary>
        /// Gradient will be applied to each shape individually.
        /// </summary>
        gbPerShape = 1,
    }

    /// <summary>
    /// The type of gradient for point and polygon fill.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkGradientType
    {
        /// <summary>
        /// Linear gradient.
        /// </summary>
        gtLinear = 0,

        /// <summary>
        /// Rectangular gradient.
        /// </summary>
        gtRectangular = 1,

        /// <summary>
        /// Circle gradient.
        /// </summary>
        gtCircle = 2,
    }

    /// <summary>
    /// Deprecated. The available justification types for labels.
    /// </summary>
    /// \deprecated v. 4.8. Use properties which take tkLabelAlignment enumeration as an input instead.
    public enum tkHJustification
    {
        hjLeft = 0,
        hjCenter = 1,
        hjRight = 2,
        hjNone = 3,
        hjRaw = 4,
    }

    /// <summary>
    /// The source type for the image. 
    /// </summary>
    /// <remarks>
    /// The image can uninitialized, to be opened by MapWinGIS native code (bmp images) or GDAL library (all other formats).
    /// It's important to check this value as various types of images can have different behaviour and applicable methods.
    /// </remarks>
    /// \new48 Added in version 4.8
    public enum tkImageSourceType
    {
        /// <summary>
        /// The image is not bound to particular source. 
        /// </summary>
        /// <remarks>The image object will have this state before Image.Open or Image.CreateNew
        /// commands or after Image.Close command.</remarks>
        istUninitialized = 0,

        /// <summary>The image is bound to bmp file on disk.</summary>
        /// <remarks>No third-party libraries are used for it's processing. The editing of values is saved to the disk immediately.
        /// The necessary part of image is read from the disk before each redraw, but this data isn't preserved in RAM after the drawing is finished.</remarks>
        istDiskBased = 1,

        /// <summary>
        /// The source of the image is the array of pixels stored in the RAM. 
        /// </summary>
        /// <remarks>It's possible to edit these values and save them to a file afterwards.</remarks>
        istInMemory = 2,

        /// <summary>
        /// The image is opened through GDAL library. 
        /// </summary>
        /// <remarks>Only a part of the image (a "buffer") is loaded into memory before each redraw and remains there till 
        /// the next redraw. The editing of the pixels of he buffer is allowed but the changes will be discarded on the next redraw of the map.
        /// </remarks>
        istGDALBased = 3,

        /// <summary>
        /// This value is set for the point icons stored in the ShapeDrawingOptions.Picture property after the deserialization of the 
        /// object state from string.
        /// </summary>
        istGDIPlus = 4,
    }

    /// <summary>
    /// The interpolation mode which can be use when image is drawn with the resolution higher or lower than original.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkInterpolationMode
    {
        imBilinear = 3,
        imBicubic = 4,
        imNone = 5,
        imHighQualityBilinear = 6,
        imHighQualityBicubic = 7,
    }

    /// <summary>
    /// The available alignments for the labels. Defines both horizontal and vertical alignment.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkLabelAlignment
    {
        laTopLeft = 0,
        laTopCenter = 1,
        laTopRight = 2,
        laCenterLeft = 3,
        laCenter = 4,
        laCenterRight = 5,
        laBottomLeft = 6,
        laBottomCenter = 7,
        laBottomRight = 8,
    }

    /// <summary>
    /// Lists the parts of the label. Can be used to specify particular parts to apply color scheme to.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkLabelElements
    {
        leFont = 0,
        leFontOutline = 1,
        leShadow = 2,
        leHalo = 3,
        leFrameBackground = 4,
        leFrameOutline = 5,

        /// <summary>
        /// If the frame is visible the color scheme will be applied to it. Otherwise it will interpreted as font color.
        /// </summary>
        leDefault = 6,
    }

    /// <summary>
    /// The the available shapes of the frames for labels, i.e. rectangles drawn around the labels to ensure better contrast.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkLabelFrameType
    {
        lfRectangle = 0,
        lfRoundedRectangle = 1,
        lfPointedRectangle = 2,
    }

    /// <summary>
    /// The available positioning of the label relative to the parent shape.
    /// </summary>
    /// <remarks>Each constant is applicable to the specific shape type only, otherwise the operation will generate an error.
    /// This constants doesn't affect the positioning of label for point and multipoint shapefile. So any value can be passed in that case.</remarks>
    /// \new48 Added in version 4.8
    public enum tkLabelPositioning
    {
        /// <summary>
        /// The label will be placed in the center of bounding box. Applicable to polygons only.
        /// </summary>
        lpCenter = 0,

        /// <summary>
        /// The label will be placed in the centroid of the shape. Applicable to polygons only.
        /// </summary>
        lpCentroid = 1,

        /// <summary>
        /// The label will be placed in the interior point (see Shape.InteriorPoint for details). Applicable for polygons only.
        /// </summary>
        lpInteriorPoint = 2,

        /// <summary>
        /// The label will be placed in the beginning of the first segment of the polyline. Applicable to polylines only.
        /// </summary>
        lpFirstSegment = 3,

        /// <summary>
        /// The label will be placed in the end of the last segment of the polyline. Applicable to polylines only.
        /// </summary>
        lpLastSegment = 4,

        /// <summary>
        /// The label will be placed in the center of the middle segment of the polyline. Applicable to polylines only.
        /// </summary>
        /// <remarks>The middle segment is the one which contains a point which splits the polyline into 2 parts of equal length.</remarks>
        lpMiddleSegment = 5,

        /// <summary>
        /// The label will be placed in the center of the longest segment of the polyline. Applicable to polylines only.
        /// </summary>
        lpLongestSegment = 6,

        /// <summary>
        /// Is used internally.
        /// </summary>
        lpNone = 7,
    }

    /// <summary>
    /// The available gradient mode for drawing background of label and for its font.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkLinearGradientMode
    {
        gmHorizontal = 0,
        gmVertical = 1,
        gmForwardDiagonal = 2,
        gmBackwardDiagonal = 3,
        gmNone = 4,
    }

    /// <summary>
    /// Defines the orientation of label relative to the line it marks.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkLineLabelOrientation
    {
        lorHorizontal = 0,
        lorParallel = 1,
        lorPerpindicular = 2,
    }

    /// <summary>
    /// Deprecated. The available line styles. 
    /// </summary>
    /// \deprecated v.4.8 Use properties which take tkDashStyle enumeration as input instead. For drawing complex lines use LinePattern class.
    public enum tkLineStipple
    {
        lsNone = 0,
        lsDotted = 1,
        lsDashed = 2,
        lsDashDotDash = 3,
        lsDoubleSolid = 4,
        lsDoubleSolidPlusDash = 5,
        lsTrainTracks = 6,
        lsCustom = 7,
        lsDashDotDot = 8,
    }

    /// <summary>
    /// The available type of segments in the LinePattern.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkLineType
    {
        /// <summary>
        /// The segment is represented by common line with styles defined by tkDashStyle enumeration.
        /// </summary>
        lltSimple = 0,

        /// <summary>
        /// The segment is represented by the markers placed in organized order through specific interval.
        /// </summary>
        lltMarker = 1,
    }

    /// <summary>
    /// The lock mode of the map, either locked or not.
    /// </summary>
    public enum tkLockMode
    {
        lmUnlock = 0,
        lmLock = 1,
    }

    /// <summary>
    /// A list of named constants for some of the well-known colors.
    /// </summary>
    /// <remarks>Use Utils.ColorByName to pass this values to particular properties.</remarks>
    /// \new48 Added in version 4.8
    public enum tkMapColor
    {
        Black = -16777216,
        Navy = -16777088,
        DarkBlue = -16777077,
        MediumBlue = -16777011,
        Blue = -16776961,
        DarkGreen = -16751616,
        Green = -16744448,
        Teal = -16744320,
        DarkCyan = -16741493,
        DeepSkyBlue = -16728065,
        DarkTurquoise = -16724271,
        MediumSpringGreen = -16713062,
        Lime = -16711936,
        SpringGreen = -16711809,
        Aqua = -16711681,
        Cyan = -16711681,
        MidnightBlue = -15132304,
        DodgerBlue = -14774017,
        LightSeaGreen = -14634326,
        ForestGreen = -14513374,
        SeaGreen = -13726889,
        DarkSlateGray = -13676721,
        LimeGreen = -13447886,
        MediumSeaGreen = -12799119,
        Turquoise = -12525360,
        RoyalBlue = -12490271,
        SteelBlue = -12156236,
        DarkSlateBlue = -12042869,
        MediumTurquoise = -12004916,
        Indigo = -11861886,
        DarkOliveGreen = -11179217,
        CadetBlue = -10510688,
        CornflowerBlue = -10185235,
        MediumAquamarine = -10039894,
        DimGray = -9868951,
        SlateBlue = -9807155,
        OliveDrab = -9728477,
        SlateGray = -9404272,
        LightSlateGray = -8943463,
        MediumSlateBlue = -8689426,
        LawnGreen = -8586240,
        Chartreuse = -8388864,
        Aquamarine = -8388652,
        Maroon = -8388608,
        Purple = -8388480,
        Olive = -8355840,
        Gray = -8355712,
        SkyBlue = -7876885,
        LightSkyBlue = -7876870,
        BlueViolet = -7722014,
        DarkRed = -7667712,
        DarkMagenta = -7667573,
        SaddleBrown = -7650029,
        DarkSeaGreen = -7357301,
        LightGreen = -7278960,
        MediumPurple = -7114533,
        DarkViolet = -7077677,
        PaleGreen = -6751336,
        DarkOrchid = -6737204,
        YellowGreen = -6632142,
        Sienna = -6270419,
        Brown = -5952982,
        DarkGray = -5658199,
        LightBlue = -5383962,
        GreenYellow = -5374161,
        PaleTurquoise = -5247250,
        LightSteelBlue = -5192482,
        PowderBlue = -5185306,
        Firebrick = -5103070,
        DarkGoldenrod = -4684277,
        MediumOrchid = -4565549,
        RosyBrown = -4419697,
        DarkKhaki = -4343957,
        Silver = -4144960,
        MediumVioletRed = -3730043,
        IndianRed = -3318692,
        Peru = -3308225,
        Chocolate = -2987746,
        Tan = -2968436,
        LightGray = -2894893,
        Thistle = -2572328,
        Orchid = -2461482,
        Goldenrod = -2448096,
        PaleVioletRed = -2396013,
        Crimson = -2354116,
        Gainsboro = -2302756,
        Plum = -2252579,
        BurlyWood = -2180985,
        LightCyan = -2031617,
        Lavender = -1644806,
        DarkSalmon = -1468806,
        Violet = -1146130,
        PaleGoldenrod = -1120086,
        LightCoral = -1015680,
        Khaki = -989556,
        AliceBlue = -984833,
        Honeydew = -983056,
        Azure = -983041,
        SandyBrown = -744352,
        Wheat = -663885,
        Beige = -657956,
        WhiteSmoke = -657931,
        MintCream = -655366,
        GhostWhite = -460545,
        Salmon = -360334,
        AntiqueWhite = -332841,
        Linen = -331546,
        LightGoldenrodYellow = -329006,
        OldLace = -133658,
        Red = -65536,
        Magenta = -65281,
        Fuchsia = -65281,
        DeepPink = -60269,
        OrangeRed = -47872,
        Tomato = -40121,
        HotPink = -38476,
        Coral = -32944,
        DarkOrange = -29696,
        LightSalmon = -24454,
        Orange = -23296,
        LightPink = -18751,
        Pink = -16181,
        Gold = -10496,
        PeachPuff = -9543,
        NavajoWhite = -8531,
        Moccasin = -6987,
        Bisque = -6972,
        MistyRose = -6943,
        BlanchedAlmond = -5171,
        PapayaWhip = -4139,
        LavenderBlush = -3851,
        SeaShell = -2578,
        Cornsilk = -1828,
        LemonChiffon = -1331,
        FloralWhite = -1296,
        Snow = -1286,
        Yellow = -256,
        LightYellow = -32,
        Ivory = -16,
        White = -1,
    }

    /// <summary>
    /// The available types of shapes for points when ShapeDrawingOptions.PointType is set to ptSymbolStandard.
    /// </summary>
    /// <remarks>The appearance of regular, cross and star shapes can be modified by 
    /// ShapeDrawingOptions.PointSidesCount, and ShapeDrawingOptions.PointSidesRatio properties.</remarks>
    /// \new48 Added in version 4.8
    public enum tkPointShapeType
    {
        /// <summary>
        /// Regular shapes. 
        /// </summary>
        /// <remarks>Consist of a number of segments of equal length, equally distanced from the center and forming a closed contour. 
        /// For example, equilateral triangle, square, pentagon, etc.</remarks>
        ptShapeRegular = 0,

        /// <summary>
        /// Cross-like shapes.
        /// </summary>
        /// <remarks>Consists of the number of segments with varying direction which start in the center of shape. 
        /// The direction is changed by a constant step chosen in such way as to cover the whole circle.</remarks>
        ptShapeCross = 1,

        /// <summary>
        /// Star like shape.
        /// </summary>
        /// <remarks>Represents stars with different number of rays.</remarks>
        ptShapeStar = 2,

        /// <summary>
        /// Circle shape.
        /// </summary>
        ptShapeCircle = 3,

        /// <summary>
        /// Arrow shape.
        /// </summary>
        ptShapeArrow = 4,

        /// <summary>
        /// Flag shape.
        /// </summary>
        ptShapeFlag = 5,
    }

    /// <summary>
    /// The available types of point symbols.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkPointSymbolType
    {
        /// <summary>
        /// The shape described by tkPointShapeType enumeration can be drawn.
        /// </summary>
        ptSymbolStandard = 0,

        /// <summary>
        /// The ANSI font characters can be drawn. 
        /// </summary>
        /// \see ShapeDrawingOptions.PointCharacter.
        ptSymbolFontCharacter = 1,

        /// <summary>
        /// Icons represented by instance of the image class can be drawn. 
        /// </summary>
        /// \see ShapeDrawingOptions.Picture property.
        ptSymbolPicture = 2,
    }

    /// <summary>
    /// Deprecated. The available point types.
    /// </summary>
    /// \deprecated v4.8. Use properties which take tkDefaultPointSymbol enumeration as input instead.
    public enum tkPointType
    {
        ptSquare = 0,
        ptCircle = 1,
        ptDiamond = 2,
        ptTriangleUp = 3,
        ptTriangleDown = 4,
        ptTriangleLeft = 5,
        ptTriangleRight = 6,
        ptUserDefined = 7,
        ptImageList = 8,
        ptFontChar = 9,
    }

    /// <summary>
    /// Available options applicable to point reflection
    /// </summary>
    /// \new495 Added in version 4.9.5
    public enum tkPointReflectionType
    {
        /// <summary>
        /// No reflection is applied
        /// </summary>
        prtNone = 0,

        /// <summary>
        /// Symbol is flipped left-to-right
        /// </summary>
        prtLeftToRight = 1, 

        /// <summary>
        /// Symbol is flipped top-to-bottom
        /// </summary>
        prtTopToBottom = 2,
    }

    /// <summary>
    /// The available parameters for the projections.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkProjectionParameter
    {
        LatitudeOfOrigin = 0,
        CentralMeridian = 1,
        ScaleFactor = 2,
        FalseEasting = 3,
        FalseNorthing = 4,
        LongitudeOfOrigin = 5,
    }

    /// <summary>
    /// List the behaviours the control will use to adjust its contents after the resizing operation.
    /// </summary>
    public enum tkResizeBehavior
    {
        rbClassic = 0,
        rbModern = 1,
        rbIntuitive = 2,

        /// <summary>
        /// The extents of the map will be kept untouched. 
        /// </summary>
        /// <remarks>This may cause distortions if the scaling factors by x and y axis aren't the same.</remarks>
        rbWarp = 3,

        /// <summary>
        /// The scale of he map will be preserved.
        /// </summary>
        rbKeepScale = 4,
    }

    /// <summary>
    /// The possible behaviours that affect the serialization and deserialization of labels (charts).
    /// </summary>
    /// See \ref labels_serialization "Serialization of labels" for additional information.
    /// \new48 Added in version 4.8
    public enum tkSavingMode
    {
        /// <summary>
        /// Only the visualization settings of the labels (charts) will be serialized. The existing labels and charts won't be cleared on deserialization.  
        /// </summary>
        modeNone = 0,

        /// <summary>
        /// The settings as well as positions of the labels (charts) will be serialized. 
        /// The existing labels and charts will be cleared on deserialization.  
        /// </summary>
        modeStandard = 1,

        /// <summary>
        /// The settings of labels (charts) will be serialized in the large XML body, while positions will be serialized to the stand-alone .lbl file.
        /// </summary>
        modeXML = 2,

        /// <summary>
        /// The settings of labels (charts) will serialized in the large XML body, while positions will be serialized to the standard fields of dbf table.
        /// </summary>
        modeDBF = 3,

        /// <summary>
        /// The same as modeXML but .lbl file will be overwritten (in case it exists).
        /// </summary>
        modeXMLOverwrite = 4,
    }

    /// <summary>
    /// Defines the possible ways to display the selected shapes of the shapefile.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkSelectionAppearance
    {
        /// <summary>
        /// The shapes will be highlighted by Shapefile.SelectionColor with Shapefile.SelectionTransparency setting.
        /// </summary>
        /// <remarks>This value is a default one.</remarks>
        saSelectionColor = 0,

        /// <summary>
        /// The drawing options defined by Shapefile.SelectionDrawingOptions property will be used.
        /// </summary>
        saDrawingOptions = 1,
    }

    /// <summary>
    /// Possible values that were used to setup certain aspects of drawing routines.
    /// </summary>
    /// \new48 Added in version 4.8
    /// \deprecated v4.8. The default value dmNewSymbology must be used in all cases.
    public enum tkShapeDrawingMethod
    {
        /// <summary>
        /// The drawing routine for 4.7 and prior versions, without shapefile categories, polygon gradients, built-in selection, etc.
        /// </summary>
        dmStandard = 0,

        /// <summary>
        /// The intermediate implementation between 4.7 and 4.8.
        /// </summary>
        dmNewWithSelection = 1,

        /// <summary>
        /// The intermediate implementation between 4.7 and 4.8.
        /// </summary>
        dmNewWithLabels = 2,

        /// <summary>
        /// The default drawing routine for version 4.8. Uses all the latest functionality of the control.
        /// </summary>
        dmNewSymbology = 3,
    }

    /// <summary>
    /// The elements of the symbology for a single shape with individual colors.
    /// </summary>
    /// <remarks>The values can be used to setup the color scheme for shapefile categories.</remarks>
    /// \new48 Added in version 4.8
    public enum tkShapeElements
    {
        shElementDefault = 0,

        /// <summary>
        /// The fill of polygon shapes or point symbols.
        /// </summary>
        shElementFill = 1,

        /// <summary>
        /// The second color of fill for polygon shapes or point symbols.
        /// </summary>
        /// <remarks>It is used when ShapeDrawingOptions.FillType = ftGradient.</remarks>
        shElementFill2 = 2,

        /// <summary>
        /// The outlines of polygons and point symbols or the lines of polylines.
        /// </summary>
        shElementLines = 3,

        /// <summary>
        /// The back color of fill for polygons and point symbols.
        /// </summary>
        /// <remarks>It is used when ShapeDrawingOptions.FillType = ftHatch.</remarks>
        shElementFillBackground = 4,
    }

    /// <summary>
    /// Defines possible states of the Shapefile class.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkShapefileSourceType
    {
        /// <summary>
        /// The shapefile isn't initialized.
        /// </summary>
        sstUninitialized = 0,

        /// <summary>
        /// The instance of class is bound to particular file on disk.
        /// </summary>
        /// <remarks>See Shapefile.Filename to find out the file.</remarks>
        sstDiskBased = 1,

        /// <summary>
        /// The instance of class holds in-memory shapefile, which isn't bound to the file on disk.
        /// </summary>
        /// <remarks>Such shapefiles can be created by Shapefile.CreateNew method with empty string as filename argument.</remarks>
        sstInMemory = 2,
    }

    /// <summary>
    /// Defines possible spatial relations between 2 shapes.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkSpatialRelation
    {
        srContains = 0,
        srCrosses = 1,
        srDisjoint = 2,
        srEquals = 3,
        srIntersects = 4,
        srOverlaps = 5,
        srTouches = 6,
        srWithin = 7,
    }

    /// <summary>
    /// The possible units of measure for the data being displaying on map.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkUnitsOfMeasure
    {
        umDecimalDegrees = 0,
        umMiliMeters = 1,
        umCentimeters = 2,
        umInches = 3,
        umFeets = 4,
        umYards = 5,
        umMeters = 6,
        umMiles = 7,
        umKilometers = 8,
    }

    /// <summary>
    /// The list of data types which can be returned as a result of expression evaluation.
    /// </summary>
    /// <remarks>See Table.TestExpression for the details.</remarks> 
    /// \new48 Added in version 4.8
    public enum tkValueType
    {
        vtDouble = 0,
        vtString = 1,
        vtBoolean = 2,
    }

    /// <summary>
    /// The drawing modes for vector data.
    /// </summary>
    /// <remarks>The enumeration is meant to setup a trade-off between performance (GDI) and the set of 
    /// available options (GDI+).</remarks>
    /// \new48 Added in version 4.8
    public enum tkVectorDrawingMode
    {
        /// <summary>
        /// All the elements of vector data will be drawn by GDI library.
        /// </summary>
        /// <remarks>Options like gradients or alpha blending are not supported in this mode. </remarks>
        vdmGDI = 0,

        /// <summary>
        /// The lines will be drawn using GDI library, while all the rest elements - GDI+.
        /// </summary>
        vdmGDIMixed = 1,

        /// <summary>
        /// All the elements of vector data will be drawn using GDI+. 
        /// </summary>
        /// <remarks>The line smoothing will be carried out in this mode only.</remarks>
        vdmGDIPlus = 2,
    }

    /// <summary>
    /// The types of symbols for vertices of shapes when ShapeDrawingOptions.VerticesVisible = true.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkVertexType
    {
        vtSquare = 0,
        vtCircle = 1,
    }

    /// <summary>
    /// The position of labels (charts) relative to the data layers.
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkVerticalPosition
    {
        /// <summary>
        /// Labels (charts) will be drawn immediately after the data layer.
        /// </summary>
        /// <remarks>As a result they can be covered by other data layers.</remarks>
        vpAboveParentLayer = 0,

        /// <summary>
        /// Labels (charts) will be drawn atop of all the data layers.
        /// </summary>
        vpAboveAllLayers = 1,
    }

    /// <summary>
    /// Possible datasource types for grids.
    /// </summary>
    /// \new490 Added in version 4.9.0
    public enum tkGridSourceType
    {
        /// <summary>
        /// No datasource was opened.
        /// </summary>
        gstUninitialized = 0,

        /// <summary>
        /// GDAL-based datasource is opened.
        /// </summary>
        gstGDALBased = 1,

        /// <summary>
        /// Non-GDAL in-memory datasource is opened.
        /// </summary>
        gstNative = 2,
    }

    /// <summary>
    /// Possible formats for images acting as visualization proxies for grid
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkGridProxyFormat
    {
        /// <summary>
        /// BMP format
        /// </summary>
        gpfBmpProxy = 0,

        /// <summary>
        /// GeoTiff format
        /// </summary>
        gpfTiffProxy = 1,
    }

    /// <summary>
    /// Possible behaviours for displaying grid datasource. The behaviours will be used in AxMap.AddLayer and Grid.OpenAsImage methods.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkGridProxyMode
    {
        /// <summary>
        /// The following priorities are used:\n
        /// 1) direct rendering if GDAL overviews are available;
        /// 2) use of proxy image if a valid proxy already exists;\n
        /// 3) direct rendering + overview creation if datasource is supported by GDAL;\n
        /// 4) otherwise - proxy image will be created.
        /// </summary>
        gpmAuto = 0,

        /// <summary>
        /// Disk-based proxy image will be created to display a grid.
        /// </summary>
        gpmUseProxy = 1,

        /// <summary>
        /// Direct rendering will be used. If it's not possible (see Grid.CanDisplayWithoutProxy), than rendering operation will fail.
        /// </summary>
        gpmNoProxy = 2,
    }

    /// <summary>
    /// Possible events during the measuring process.
    /// </summary>
    /// \new491 Added in version 4.9.1
    /// \new510 Modified in version 5.1.0
    public enum tkMeasuringAction
    {
        /// <summary>
        /// Point was add with measuring tool.
        /// </summary>
        PointAdded = 0,

        /// <summary>
        /// Point was removed with measuring tool.
        /// </summary>
        PointRemoved = 1,

        /// \deprecated v5.1.0 Use tkMeasuringAction.MeasuringStopped instead.
        MesuringStopped = 2,

        /// <summary>
        /// Measuring was stopped.
        /// </summary>
        MeasuringStopped = 3
    }

    /// <summary>
    /// Possible reasons for grids no being able to be rendered directly by Image class without creating a proxy image.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkCanDisplayGridWoProxy
    {
        /// <summary>
        /// Grid can be rendered without image proxy.
        /// </summary>
        cdwYes = 0,

        /// <summary>
        /// Grid can not be rendered directly because the format isn't supported by GDAL.
        /// </summary>
        cdwUnsupportedFormat = 1,

        /// <summary>
        /// Grid can not be rendered directly because of the large size. This limitation is artificial to ensure
        /// acceptable performance. The maximum allowable size can be changed through GlobalSettings.MaxNoProxyGridSizeMb property.
        /// </summary>
        cdwSizeLimitation = 2,
    }

    /// <summary>
    /// Possible measuring types.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkMeasuringType
    {
        /// <summary>
        /// Distance is measured. Area can be measured as well in this mode by closing the path 
        /// on one of the previous vertices and holding Ctrl.
        /// </summary>
        MeasureDistance = 0,

        /// <summary>
        /// Area is measured.
        /// </summary>
        MeasureArea = 1,
    }

    /// <summary>
    /// Methods for retrieval of existing color schemes from grid.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkGridSchemeRetrieval
    {
        /// <summary>
        /// All available retrieval methods will be executed until a first successful one is found. 
        /// The methods will be tried in the following order: gsrDiskBased, gsrDiskBasedForProxy, gsrGdalColorTable.
        /// </summary>
        gsrAuto = 0,

        /// <summary>
        /// Color scheme from accompanying .mwleg file will be read if there is any (for example, grid.mwleg).
        /// </summary>
        gsrDiskBased = 1,

        /// <summary>
        /// Color scheme from .mwleg file accompanying proxy image for the grid will be read if there is any (for example, grid_proxy.bmp.mwleg).
        /// </summary>
        gsrDiskBasedForProxy = 2,

        /// <summary>
        /// Built-in color scheme (stored in GeoTiff images, for examples) will retrieved if there is any.
        /// </summary>
        gsrGdalColorTable = 3,
    }

    /// <summary>
    /// Methods of generation of a new color scheme for grid.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkGridSchemeGeneration
    {
        /// <summary>
        /// Gradient color scheme will be built.
        /// </summary>
        gsgGradient = 0,

        /// <summary>
        /// Unique values color scheme will be built.
        /// </summary>
        gsgUniqueValues = 1,

        /// <summary>
        /// Unique values color scheme will be built if there is small number of unique values (GlobalSettings.MaxUniqueValuesCountForGridScheme),
        /// if there are too may unique values, gradient color scheme will be built.
        /// </summary>
        gsgUniqueValuesOrGradient = 2,
    }

    /// <summary>
    /// Possible tile server projection. 
    /// </summary>
    /// <remarks>If additional projections are needed, they should be implemented in MapWinGIS code, there is 
    /// no way to do it in client code.</remarks>
    /// \new490 Added in version 4.9.0
    public enum tkTileProjection
    {
        /// <summary>
        /// Google Mercator, the most popular nowadays (GoogleMaps, BingMaps, OSM, etc.)
        /// </summary>
        SphericalMercator = 0,

        /// <summary>
        /// Amersfoort projection for Netherlands.
        /// </summary>
        Amersfoort = 1,
    }

    /// <summary>
    /// Types of map redraw. 
    /// </summary>
    /// <remarks>Types of redraws other than RedrawAll maybe useful when large datasources are loaded of when frequent updates of only specific map content is necessary.</remarks>
    /// \new491 Added in version 4.9.1
    public enum tkRedrawType
    {
        /// <summary>
        /// All the map will be redrawn anew. Information cached in screen buffer won't be used.
        /// </summary>
        /// <remarks>Corresponds to AxMap.Redraw.</remarks>
        RedrawAll = 1,

        /// <summary>
        /// Data layers will be rendered from screen buffer. Everything else will be redrawn a new.
        /// </summary>
        RedrawSkipDataLayers = 2,

        /// <summary>
        /// Updates shape editor and measuring tool. Data layers, drawing layers and layers in interactive editing mode will be rendered from screen buffer.
        /// </summary>
        RedrawSkipAllLayers = 4,

        /// <summary>
        /// Map will be rendered from the main buffer. Only measurements and coordinate display will be rendered anew.
        /// </summary>
        /// <remarks>Corresponds to AxMap.Redraw2/AxMap.Invalidate.</remarks>
        RedrawMinimal = 3,
    }

    /// <summary>
    /// Possible coordinate formats to display current mouse cursor position on the map.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkCoordinatesDisplay
    {
        /// <summary>
        /// No coordinates will be displayed.
        /// </summary>
        cdmNone = 0,

        /// <summary>
        /// If projection is set for map - degrees, otherwise - map units.
        /// </summary>
        /// <remarks>Together with AxMap.GrabProjectionFromData this mode can be used to quickly check whether certain 
        /// datasource has a valid projection.</remarks>
        cdmAuto = 1,

        /// <summary>
        /// Current position in decimal degrees will be displayed or no coordinates at all if no projection is set for map. 
        /// </summary>
        cdmDegrees = 2,

        /// <summary>
        /// Position in current map units will be displayed, whatever that may be: meters, miles, decimal degrees, etc.
        /// </summary>
        cdmMapUnits = 3,
    }

    /// <summary>
    /// Commonly used map projections to be set in Form Designer (see AxMap.Projection property).
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkMapProjection
    {
        /// <summary>
        /// Some projection not covered by this enumeration was specified manually or taken from the data.
        /// </summary>
        PROJECTION_CUSTOM = -1,

        /// <summary>
        /// Empty projection, the initial state of map.
        /// </summary>
        PROJECTION_NONE = 0,

        /// <summary>
        /// WGS84 coordinate system (EPSG:4326, equirectangular projection will used implicitly during rendering).
        /// </summary>
        PROJECTION_WGS84 = 1,

        /// <summary>
        /// So called Google Mercator projection (EPSG:3857; aka Spherical Mercator) commonly used by tile servers.
        /// </summary>
        PROJECTION_GOOGLE_MERCATOR = 2,
    }

    /// <summary>
    /// Possible zoom behaviours.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkZoomBehavior
    {
        /// <summary>
        /// Zooming is performed without binding to tile levels, which may result in scaling of tiles and certain distortions.
        /// </summary>
        zbDefault = 0,

        /// <summary>
        /// Each zoom operation will displayed the next zoom level for tiles. This will ensure that tiles aren't scaled,
        /// which increases their readability and improve their general look. If no projection is set for map or tiles aren't 
        /// visible, default zooming will be used.
        /// </summary>
        zbUseTileLevels = 1,
    }

    /// <summary>
    /// Type of MapWinGIS subsystems to check support for.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkSupportType
    {
        /// <summary>
        /// GDAL library
        /// </summary>
        stGdal = 0,
    }

    /// <summary>
    /// Possible open strategies for datasources.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkFileOpenStrategy
    {
        /// <summary>
        /// Datasource format isn't supported and can't be opened by MapWinGIS .
        /// </summary>
        fosNotSupported = -1,

        /// <summary>
        /// Open strategy will be detected automatically. See FileManager.get_OpenStrategy for the details.
        /// </summary>
        fosAutoDetect = 0,

        /// <summary>
        /// Datasource is opened by Image class as RGB image. 
        /// </summary>
        fosRgbImage = 1,

        /// <summary>
        /// Grid is directly rendered by Image class using so called grid rendering. Datasource must be supported by GDAL in order for this strategy to work.
        /// </summary>
        fosDirectGrid = 2,

        /// <summary>
        /// Grid is displayed using a proxy image file, specifically created for visualization. This requires significant time on proxy creation but faster rendering afterwards.
        /// </summary>
        fosProxyForGrid = 3,

        /// <summary>
        /// A single layer from vector datasource will be opened 
        /// (for shapefiles Shapefile class will be used; for other vector formats - OgrLayer; for multilayer datasources a first layer 
        /// will be returned).
        /// </summary>
        fosVectorLayer = 4,

        /// <summary>
        /// Opens vector datasource using OgrDatasource class. Should be used with multi-layer vector formats, like KML to add all layers 
        /// to the map with a single call.
        /// </summary>
        fosVectorDatasource = 5,
    }

    /// <summary>
    /// Possible state of validity flag for a field group operation.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkFieldOperationValidity
    {
        /// <summary>
        /// Operation is valid.
        /// </summary>
        fovValid = 0,

        /// <summary>
        /// Operation can't be performed because no such field was found in the input shapefile.
        /// </summary>
        fovFieldNotFound = 1,

        /// <summary>
        /// Operation can't performed because it's not supported for the current field type (e.g. average on string type or mode on numeric types).
        /// </summary>
        fovNotSupported = 2,
    }

    /// <summary>
    /// Possible statistic operations for a field during grouping of shapes.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkFieldStatOperation
    {
        /// <summary>
        /// Sum of values within group.
        /// </summary>
        fsoSum = 0,

        /// <summary>
        /// Minimum value within group.
        /// </summary>
        fsoMin = 1,

        /// <summary>
        /// Maximum value within group.
        /// </summary>
        fsoMax = 2,

        /// <summary>
        /// Average value within group (for numeric fields only).
        /// </summary>
        fsoAvg = 3,

        /// <summary>
        /// Weighted average value within group (for numeric fields only). 
        /// For polygons weighting by area is used for polylines - weighting by length, for other types - none.
        /// </summary>
        fsoWeightedAvg = 4,

        /// <summary>
        /// Mode value (the value with the largest frequency) within group (for string fields only).
        /// </summary>
        fsoMode = 5,
    }

    /// <summary>
    /// Type of shapefile validation.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkShapeValidationType
    {
        /// <summary>
        /// Validation of input of certain operation.
        /// </summary>
        svtInput = 0,

        /// <summary>
        /// Validation of output of certain operation.
        /// </summary>
        svtOutput = 1,
    }

    /// <summary>
    /// Possible validation modes for shapefiles.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkShapeValidationMode
    {
        /// <summary>
        /// No validation is performed, invalid input shapes will be processed and invalid shapes will be passed to output.
        /// </summary>
        NoValidation = 0,

        /// <summary>
        /// An attempt will be made to fix invalid shapes. If fixing fails invalid shapes will be processed the same way as others.
        /// </summary>
        TryFixProceedOnFailure = 1,

        /// <summary>
        /// An attempt will be made to fix invalid shapes. If fixing fails invalid shapes will be skipped, i.e. not processed and not passed to output.
        /// </summary>
        TryFixSkipOnFailure = 2,

        /// <summary>
        /// An attempt will be made to fix invalid shapes. If fixing of at least one shape fails the whole operation will be aborted.
        /// </summary>
        TryFixAbortOnFailure = 3,

        /// <summary>
        /// The whole operation will be aborted on finding at least one invalid shape, without trying to fix it.
        /// </summary>
        AbortOnErrors = 4,
    }

    /// <summary>
    /// Possible shape validation status. See also GlobalSettings.ShapeInputValidationMode, GlobalSettings.ShapeOutputValidationMode.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkShapeValidationStatus
    {
        /// <summary>
        /// Validation wasn't performed.
        /// </summary>
        WasntValidated = 0,

        /// <summary>
        /// No invalid shapes were found.
        /// </summary>
        Valid = 1,

        /// <summary>
        /// Invalid shapes were found but all of the were fixed.
        /// </summary>
        InvalidFixed = 2,

        /// <summary>
        /// Some invalid shapes were skipped, after their fixing failed.
        /// </summary>
        InvalidSkipped = 3,

        /// <summary>
        /// Some invalid shapes were accepted as input or passed to output after their fixing failed.
        /// </summary>
        InvalidReturned = 4,

        /// <summary>
        /// Operation was aborted because of invalid shapes.
        /// </summary>
        OperationAborted = 5,
    }

    /// <summary>
    /// Types of units to be used by scalebar.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkScalebarUnits
    {
        /// <summary>
        /// Metric units (centimetres, meters, kilometres).
        /// </summary>
        Metric = 0,

        /// <summary>
        /// Units of American system of measurements (miles, feet).
        /// </summary>
        American = 1,

        /// <summary>
        /// Mixed mode, when units of both metric and American system are displayed.
        /// </summary>
        GoogleStyle = 2,
    }

    /// <summary>
    /// COM interfaces provided by MapWinGIS (Utils.CreateInstance can be used for creation of actual objects).
    /// </summary>
    /// \new48 Added in version 4.8
    public enum tkInterface
    {
        idChart = 0,
        idChartField = 1,
        idCharts = 2,
        idColorScheme = 3,
        idESRIGridManager = 4,
        idExtents = 5,
        idField = 6,
        idGeoProjection = 7,
        idGlobalSettings = 8,
        idGrid = 9,
        idGridColorBreak = 10,
        idGridColorScheme = 11,
        idGridHeader = 12,
        idImage = 13,
        idLabelCategory = 14,
        idLabel = 15,
        idLabels = 16,
        idLinePattern = 17,
        idLineSegment = 18,
        idPoint = 19,
        idShape = 20,
        idShapeDrawingOptions = 21,
        idShapefile = 22,
        idShapefileCategories = 23,
        idShapefileCategory = 24,
        idShapefileColorBreak = 25,
        idShapefileColorScheme = 26,
        idShapeNetwork = 27,
        idTable = 28,
        idTileProviders = 29,
        idTiles = 30,
        idTin = 31,
        idUtils = 32,
        idVector = 33,
        idShapeValidationInfo = 34,
        idMeasuring = 35,
        idFileManager = 36,
        idFieldStatOperations = 37,
    }

    /// <summary>
    /// GUI strings that may be localized.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkLocalizedStrings
    {
        /// <summary>
        /// Hectares for area measurement. Default = "ha".
        /// </summary>
        lsHectars = 0,

        /// <summary>
        /// Meters for distance measurement. Default = "m".
        /// </summary>
        lsMeters = 1,

        /// <summary>
        /// Kilometres for distance measurement. Default = "km".
        /// </summary>
        lsKilometers = 2,

        /// <summary>
        /// Square kilometres for area measurement. Default = "sq.km".
        /// </summary>
        lsSquareKilometers = 3,

        /// <summary>
        /// Square meters for area measurement. Default = "sq.m".
        /// </summary>
        lsSquareMeters = 4,

        /// <summary>
        /// Undefined map units. Default = "mu".
        /// </summary>
        lsMapUnits = 5,

        /// <summary>
        /// Undefined area map units. Default value is "sq.mu".
        /// </summary>
        lsSquareMapUnits = 6,

        /// <summary>
        /// Miles for distance measurement. Default value is "miles".
        /// </summary>
        lsMiles = 7,

        /// <summary>
        /// Feet for distance measurement. Default value is "feet".
        /// </summary>
        lsFeet = 8,

        /// <summary>
        /// Latitude for coordinates display. Default value is "Lat".
        /// </summary>
        lsLatitude = 9,

        /// <summary>
        /// Longitude for coordinates display. Default value is "Lng".
        /// </summary>
        lsLongitude = 10,
    }

    /// <summary>
    /// Lists types of relation between tiles service projection and map projection.
    /// </summary>
    /// \new491 Added in version 4.9.1
    public enum tkTilesProjectionStatus
    {
        /// <summary>
        /// Projections match. tiles can be rendered without distortions.
        /// </summary>
        tpsNative = 0,
        /// <summary>
        /// Projections don't match, but transformation is possible. Tiles can be rendered with certain distortions.
        /// </summary>
        tpsCompatible = 1,
        /// <summary>
        /// No projection is set or it is not compatible with tile service projection. Tiles can't be displayed.
        /// </summary>
        tpsEmptyOrInvalid = 2,
    }

    /// <summary>
    /// Defines possible state of a boolean property with some default behaviour.
    /// </summary>
    /// \new492 Added in version 4.9.2
    public enum tkCustomState
    {
        /// <summary>
        /// The behaviour will be selected automatically. It may differ from just true/false options.
        /// </summary>
        csAuto = 0,
        csTrue = 1,
        csFalse = 2,
    }

    /// <summary>
    /// Defines amount of information displayed via zoom bar tool tip.
    /// </summary>
    /// \new492 Added in version 4.9.2
    public enum tkZoomBarVerbosity
    {
        /// <summary>
        /// Only zoom level is displayed.
        /// </summary>
        zbvZoomOnly = 0,
        /// <summary>
        /// Zoom level, resolution and scale are displayed.
        /// </summary>
        zbvFull = 1,
        /// <summary>
        /// No tool tip is displayed.
        /// </summary>
        zbvNone = 2,
    }

    /// <summary>
    /// Defined possible styles of zoom box.
    /// </summary>
    /// \new492 Added in version 4.9.2
    public enum tkZoomBoxStyle
    {
        zbsRubberBand = 0,
        zbsGray = 1,
        zbsGrayInverted = 2,
        zbsOrange = 3,
        zbsBlue = 4,
    }

    /// <summary>
    /// Possible raster overview creation modes.
    /// </summary>
    public enum tkRasterOverviewCreation
    {
        /// <summary>
        /// Automatic choice of overview creation. Currently the same as rocYes.
        /// </summary>
        rocAuto = 0,
        /// <summary>
        /// GDAL overviews will be created.
        /// </summary>
        rocYes = 1,
        /// <summary>
        /// GDAL overviews won't be created.
        /// </summary>
        rocNo = 2,
    }

    /// <summary>
    /// Available compression modes for TIFF files provided by GDAL driver.
    /// </summary>
    /// <remarks>See more information<a href="http://www.gdal.org/frmt_gtiff.html">here</a></remarks>
    /// \new492 Added in version 4.9.2
    public enum tkTiffCompression
    {
        tkmAUTO = -1,
        tkmJPEG = 0,
        tkmLZW = 1,
        tkmPACKBITS = 2,
        tkmDEFLATE = 3,
        tkmCCITTRLE = 4,
        tkmCCITTFAX3 = 5,
        tkmCCITTFAX4 = 6,
        tkmNONE = 7,
    }

    /// <summary>
    /// Defines possible subjects of OgrLayer.SaveChanges method.
    /// </summary>
    /// \new493 Added in version 4.9.3
    public enum tkOgrSaveType
    {
        /// <summary>
        /// Only shapes from associated in-memory shapefile will be saved.
        /// </summary>
        ostGeometryOnly = 0,
        /// <summary>
        /// Only values from attribute table of associated in-memory shapefile will be saved.
        /// </summary>
        ostAttributesOnly = 1,
        /// <summary>
        /// Both shapes and attributes of associated in-memory shapefile will be saved.
        /// </summary>
        ostSaveAll = 2,
    }

    /// <summary>
    /// List of capabilities that might or might not be supported by particular OGR driver.
    /// </summary>
    /// <remarks>See description <a href="http://www.gdal.org/classOGRLayer.html#aeedbda1a62f9b89b8e5f24332cf22286">here</a></remarks>.
    /// \new493 Added in version 4.9.3
    public enum tkOgrLayerCapability
    {
        olcRandomRead = 0,
        olcSequentialWrite = 1,
        olcRandomWrite = 2,
        olcFastSpatialFilter = 3,
        olcFastFeatureCount = 4,
        olcFastGetExtent = 5,
        olcCreateField = 6,
        olcDeleteField = 7,
        olcReorderFields = 8,
        olcAlterFieldDefn = 9,
        olcTransactions = 10,
        olcDeleteFeature = 11,
        olcFastSetNextByIndex = 12,
        olcStringsAsUTF8 = 13,
        olcIgnoreFields = 14,
        olcCreateGeomField = 15,
    }

    /// <summary>
    /// Possible results for OgrLayer.SaveChanges operation.
    /// </summary>
    /// \new493 Added in version 4.9.3
    public enum tkOgrSaveResult
    {
        /// <summary>
        /// No changes were found, i.e. there is no shape records with Shapefile.get_ShapeModified property set to true
        /// in the underlying shapefile.
        /// </summary>
        osrNoChanges = 0,
        /// <summary>
        /// All shapes marked as modified were successfully saved.
        /// </summary>
        osrAllSaved = 1,
        /// <summary>
        /// Some of the shapes marked as modified were successfully saved, but for some the operation has failed.
        /// </summary>
        osrSomeSaved = 2,
        /// <summary>
        /// None of the shapes marked as modified were saved.
        /// </summary>
        osrNoneSaved = 3,
    }
    
    /// <summary>
    /// Possible metadata items which can be associated with particular GDAL driver.
    /// </summary>
    /// \new493 Added in version 4.9.3
    public enum tkGdalDriverMetadata
    {
        dmdLONGNAME = 0,
        dmdHELPTOPIC = 1,
        dmdMIMETYPE = 2,
        dmdEXTENSION = 3,
        dmdEXTENSIONS = 4,
        dmdCREATIONOPTIONLIST = 5,
        dmdOPENOPTIONLIST = 6,
        dmdCREATIONDATATYPES = 7,
        dmdSUBDATASETS = 8,
        dmdOPEN = 9,
        dmdCREATE = 10,
        dmdCREATECOPY = 11,
        dmdVIRTUALIO = 12,
        dmdLAYER_CREATIONOPTIONLIST = 13,
        dmdOGR_DRIVER = 14,
    }

    /// <summary>
    /// List of capabilities that might or might not be supported by particular OGR datasource.
    /// </summary>
    /// \new493 Added in version 4.9.3
    public enum tkOgrDSCapability
    {
        odcCreateLayer = 0,
        odcDeleteLayer = 1,
        odcCreateGeomFieldAfterCreateLayer = 2,
        odcCreateDataSource = 3,
        odcDeleteDataSource = 4,
    }

    /// <summary>
    /// Possible source types for instance of OgrLayer class.
    /// </summary>
    /// \new493 Added in version 4.9.3
    public enum tkOgrSourceType
    {
        /// <summary>
        /// The instance isn't bound to any datasource, i.e. it's empty. The default state for any newly created instance.
        /// </summary>
        ogrUninitialized = 0,
        /// <summary>
        /// The instance is bound to particular file-based format.
        /// </summary>
        ogrFile = 1,
        /// <summary>
        /// The instance is bound to a table of a spatial database.
        /// </summary>
        ogrDbTable = 2,
        /// <summary>
        /// The instance holds results of SQL query.
        /// </summary>
        ogrQuery = 3,
    }

    /// <summary>
    /// Possible string encodings for OgrDatasource, OgrLayer classes.
    /// </summary>
    public enum tkOgrEncoding
    {
        /// <summary>
        /// UTF-8 encoding.
        /// </summary>
        oseUtf8 = 0,
        /// <summary>
        /// Current ANSI codepage set in the OS for non-Unicode applications.
        /// </summary>
        oseAnsi = 1,
    }

    /// <summary>
    /// Possible modes for built-in cmIdentify tool.
    /// </summary>
    public enum tkIdentifierMode
    {
        /// <summary>
        /// All shapefile layers with Shapefile.Identifiable property set to true will be analyzed. 
        /// </summary>
        imAllLayers = 0,
        /// <summary>
        /// Only shapefile specified in the context of AxMap.ChooseLayer event will be analyzed.
        /// </summary>
        imSingleLayer = 1,
        /// <summary>
        /// All shapefile layers with Shapefile.Identifiable property set to true will be analyzed,
        /// but search will stop following the first (topmost) layer for which shapes are found.
        /// </summary>
        imAllLayersStopOnFirst = 2,
    }

    /// <summary>
    /// Possible behaviors for Shape Editor.
    /// </summary>
    public enum tkEditorBehavior
    {
        /// <summary>
        /// User is allowed to move, add and delete vertices and move or delete the shape itself.
        /// </summary>
        ebVertexEditor = 0,
        /// <summary>
        /// User is allowed to move or delete parts of multi-part shape.
        /// </summary>
        ebPartEditor = 1,
    }

    /// <summary>
    /// Possible overlay operations for Shape Editor.
    /// </summary>
    public enum tkEditorOverlay
    {
        /// <summary>
        /// Adds part to the shape being edited.
        /// </summary>
        eoAddPart = 0,
        /// <summary>
        /// Removes parts of edited shape by arbitrary polygon.
        /// </summary>
        eoRemovePart = 1,
    }

    /// <summary>
    /// Possible types of caps for GEOS buffering.
    /// </summary>
    public enum tkBufferCap
    {
        bcROUND = 0,
        bcFLAT = 1,
        bcSQUARE = 2,
    }

    /// <summary>
    /// Possible types of joins for GEOS buffering.
    /// </summary>
    public enum tkBufferJoin
    {
        bjROUND = 0,
        bjMITRE = 1,
        bjBEVEL = 2,
    }

    /// <summary>
    /// Possible ways to define values of shapefile category.
    /// </summary>
    public enum tkCategoryValue
    {
        /// <summary>
        /// A shape falls into category if a given field exactly matches ShapefileCategory.MinValue.
        /// </summary>
        cvSingleValue = 0,
        /// <summary>
        /// A shapes falls into category if value of a given field is in interval between
        /// ShapefileCategory.MinValue and ShapefileCategory.MaxValue. 
        /// </summary>
        cvRange = 1,
        /// <summary>
        /// A shape falls into category if its attribute values satisfy ShapefileCategory.Expression.
        /// </summary>
        cvExpression = 2,
    }

    /// <summary>
    /// Possible validation modes for Shape Editor.
    /// </summary>
    public enum tkEditorValidation
    {
        /// <summary>
        /// Some basic checks will be made: shape has enough points and parts for its type, ring direction for
        /// polygon is correct, the first point of polygon matches the last one.
        /// </summary>
        /// <remarks>Invalid ring direction for polygon will be fixed automatically.</remarks>
        evBasic = 0,
        /// <summary>
        /// GEOS validation will be used in addition to basic validation but no attempt to fix errors
        /// will made on this stage.
        /// </summary>
        evCheckWithGeos = 1,
        /// <summary>
        /// Same as evCheckWithGeos, only Shape.FixUp method will be called if GEOS validation failed.
        /// </summary>
        evFixWithGeos = 2,
    }

    /// <summary>
    /// Elements of shapes that can be deleted during interactive editing.
    /// </summary>
    public enum tkDeleteTarget
    {
        dtNone = 0,
        dtShape = 1,
        dtPart = 2,
        dtVertex = 3,
    }

    /// <summary>
    /// Possible ways to display length of segments for measuring tool and shape editor.
    /// </summary>
    public enum tkLengthDisplayMode
    {
        /// <summary>
        /// Depending on length either meters or kilometers will be used.
        /// </summary>
        ldmMetric = 1,

        /// <summary>
        /// Depending on length either feet or miles will be used. These units are defined according to International yard and pound agreement (1959).
        /// </summary>
        ldmAmerican = 2,
    }

    /// <summary>
    /// Possible editor states.
    /// </summary>
    public enum tkEditorState
    {
        /// <summary>
        /// Editor isn't initialized.
        /// </summary>
        esNone = 0,
        /// <summary>
        /// Editor is in the process of creating a new shape which is bound to a certain shapefile.
        /// </summary>
        esDigitize = 1,
        /// <summary>
        /// Editor is in the process of editing an exiting shape of a certain shapefile.
        /// </summary>
        esEdit = 2,
        /// <summary>
        /// Editor is in the process of creating of shape which isn't bound to particular shapefile. 
        /// </summary>
        /// <remarks> This mode is used by built-in interactive tools, cmSelectByPolygon or cmSplitByPolyline.</remarks>
        esDigitizeUnbound = 3,
        /// <summary>
        /// Editor is in the process of adding of an overlay for current shape (eoAddPart, eoRemovePart).
        /// </summary>
        esOverlay = 4,
    }

    /// <summary>
    /// Defines a set of layers to be analyzed by certain tool.
    /// </summary>
    public enum tkLayerSelection
    {
        /// <summary>
        /// All layers will be used.
        /// </summary>
        lsAllLayers = 0,
        /// <summary>
        /// No layers will be used, i.e, the functionality is off.
        /// </summary>
        lsNoLayer = 1,
        /// <summary>
        /// Only active layer will be used.
        /// </summary>
        lsActiveLayer = 2,
    }

    /// <summary>
    /// Possible types of operations in undo list of Shape Editor.
    /// </summary>
    public enum tkUndoOperation
    {
        /// <summary>
        /// Shape was added.
        /// </summary>
        uoAddShape = 0,
        /// <summary>
        /// Shape was removed.
        /// </summary>
        uoRemoveShape = 1,
        /// <summary>
        /// Shape was edited (i.e. vertices moved or deleted, parts added, etc).
        /// </summary>
        uoEditShape = 2,
        /// <summary>
        /// A number of selected shapes were moved.
        /// </summary>
        uoMoveShapes = 3,
        /// <summary>
        /// A number of selected shapes were rotated.
        /// </summary>
        uoRotateShapes = 4,
    }

    /// <summary>
    /// Defines either true and false value.
    /// </summary>
    /// <remarks>Some erratic behavior was observed when passing VARIANT_BOOL values through dispatch interface by reference. 
    /// Hence this enumeration is used.</remarks>
    public enum tkMwBoolean
    {
        blnFalse = 0,
        blnTrue = 1,
    }

    /// <summary>
    /// Possible ways to display bearing of lines.
    /// </summary>
    public enum tkBearingType
    {
        /// <summary>
        /// Absolute bearing (aka full circle bearing or azimuth). Calculated clockwise from the north direction. Range of values [0, 360] degrees.
        /// </summary>
        btAbsolute = 0,

        /// <summary>
        /// Reduced bearing (US army definition of bearing). Absolute bearing split into 4 quadrants (NE, SE, SW, NW). 
        /// Calculated either from north (NE, NW quadrants) or south (SE, SW quadrants) direction. Range of values within quadrant is [0, 90] degrees.
        /// Displayed like "N 45.0° E".
        /// </summary>
        btReducedNDE = 1,

        /// <summary>
        /// Same as btReducedNDE, apart from a bit different format: "NE: 45.0°".
        /// </summary>
        btReducedNED = 2,

        /// <summary>
        /// Relative bearing, represents clockwise angle between direction of the previous line and the next line. Range of values [-180, 180] degrees. 
        /// If the next line turns to the left the value is negative, to the right - positive.
        /// </summary>
        btRelative = 3,

        /// <summary>
        /// Left inner angle formed by the previous line and next line. Range of values [0, 360] degrees. If the next line has the same direction
        /// as the previous one, the value is 180°.
        /// </summary>
        btLeftAngle = 4,

        /// <summary>
        /// Right inner angle formed by the previous line and the next line. Range of values [0, 360] degrees. If the next line has the same direction
        /// as the previous one, the value is 180°.
        /// </summary>
        btRightAngle = 4,
    }

    /// <summary>
    /// Possible ways to display area.
    /// </summary>
    public enum tkAreaDisplayMode
    {
        /// <summary>
        /// Depending on area either square meters, hectares or square kilometers will be used.
        /// </summary>
        admMetric = 0,

        /// <summary>
        /// Hectares only will be used.
        /// </summary>
        admHectars = 1,

        /// <summary>
        /// Depending on area either square feet, acres or square miles will be used. 
        /// These units are defined according to International yard and pound agreement (1959).
        /// </summary>
        admAmerican = 2,
    }

    /// <summary>
    /// Possible formats to display angles.
    /// </summary>
    public enum tkAngleFormat
    {
        afDegrees = 0,
        afMinutes = 1,
        afSeconds = 2,
        afRadians = 3,
    }
    
    /// <summary>
    /// Possible values of the version parameter for WMS requests.
    /// </summary>
    public enum tkWmsVersion
    {
        wvEmpty = 0,
        wvAuto = 1,
        wv100 = 2,
        wv110 = 3,
        wv111 = 4,
        wv13 = 5,
    }
    
    /// <summary>
    /// Possible levels of the callback verbosity.
    /// </summary>
    public enum tkCallbackVerbosity
    {
        cvAll = 0,
        cvLimited = 1,
    }
    
    /// <summary>
    /// The function groups.
    /// </summary>
    public enum tkFunctionGroup
    {
        fgMath = 0,
        fgStrings = 1,
        fgGeometry = 2,
        fgBranching = 3,
        fgConversion = 4,
    }
    
    /// <summary>
    /// The layer types.
    /// </summary>
    public enum tkLayerType
    {
        ltUndefined = 0,
        ltVector = 1,
        ltRaster = 2,
    }
    
    /// <summary>
    /// Types of rendering for GDAL raster datasource.
    /// </summary>
    public enum tkRasterRendering
    {        
        /// <summary>
        /// Unknown (when no datasource is opened).
        /// </summary>
        rrUnknown = 0,
                
        /// <summary>
        /// Gray scale rendering based on a single band.
        /// </summary>
        rrSingleBand = 1,
        
        /// <summary>
        /// RGB rendering based on at least 3 bands with possible addition of alpha band.
        /// </summary>
        rrRGB = 2,
        
        /// <summary>
        /// Rendering based on a single band and custom color scheme.
        /// </summary>
        rrColorScheme = 3,
        
        /// <summary>
        /// Rendering based on a single band and built-in color scheme.
        /// </summary>
        rrBuiltInColorTable = 4,
    }
    
    /// <summary>
    /// A bit mask which defines custom events that will be fired during the rendering process.
    /// </summary>
    public enum tkCustomDrawingFlags
    {
        CustomDrawingNone = 0,
        OnDrawBackBufferHdc = 1,
        OnDrawBackBufferBitmapData = 2,
        BeforeAfterDrawing = 4,
        BeforeAfterLayers = 8,
    }
    
    /// <summary>
    /// GDAL data types.
    /// </summary>
    public enum tkGdalDataType
    {
        gdtUnknown = 0,
        gdtByte = 1,
        gdtUInt16 = 2,
        gdtInt16 = 3,
        gdtUInt32 = 4,
        gdtInt32 = 5,
        gdtFloat32 = 6,
        gdtFloat64 = 7,
        gdtCInt16 = 8,
        gdtCInt32 = 9,
        gdtCFloat32 = 10,
        gdtCFloat64 = 11,
    }
    
    /// <summary>
    /// Possible palette interpretations for a raster datasource.
    /// </summary>
    public enum tkPaletteInterpretation
    {
        piGray = 0,
        piRGB = 1,
        piCMYK = 2,
        piHLS = 3,
    }
    
    /// <summary>
    /// Possible color interpretations for a raster band.
    /// </summary>
    public enum tkColorInterpretation
    {
        ciUndefined = 0,
        ciGrayIndex = 1,
        ciPaletteIndex = 2,
        ciRedBand = 3,
        ciGreenBand = 4,
        ciBlueBand = 5,
        ciAlphaBand = 6,
        ciHueBand = 7,
        ciSaturationBand = 8,
        ciLightnessBand = 9,
        ciCyanBand = 10,
        ciMagentaBand = 11,
        ciYellowBand = 12,
        ciBlackBand = 13,
        ciYCbCr_YBand = 14,
        ciYCbCr_CbBand = 15,
        ciYCbCr_CrBand = 16,
    }
    
    /// <summary>
    /// Types of proxy authentication.
    /// </summary>
    public enum tkProxyAuthentication
    {
        asBasic = 0,
        asNtlm = 1,
    }
    
    /// <summary>
    /// Possible values of pixel offset mode (GDI+ rendering).
    /// </summary>
    public enum tkPixelOffsetMode
    {
        pomDefault = 0,
        pomHighPerformance = 1,
        pomHighQuality = 2,
    }

    /// <summary>
    /// Supports the load of subsets of Projection Strings from Utils.GetProjectionStrings.
    /// Can be combined to get NAD83 and/or WGS84, or the entire set.
    /// </summary>
    public enum tkProjectionSet
    {
        psAll_Projections = 1,
        psNAD83_Subset = 2,
        psWGS84_Subset = 4,
    }

    /// <summary>
    /// Tile providers enumeration
    /// </summary>
    public enum tkTileProvider
    {
        ProviderNone = -1,
        OpenStreetMap = 0,
        /// <summary>
        /// Needs API key now, not implemented yet
        /// </summary>
        OpenCycleMap = 1,
        /// <summary>
        /// Needs API key now, not implemented yet
        /// </summary>
        OpenTransportMap = 2,
        /// <summary>
        /// Needs API key now, see GlobalSettings.BingApiKey
        /// </summary>
        BingMaps = 3,
        /// <summary>
        /// Needs API key now, see GlobalSettings.BingApiKey
        /// </summary>
        BingSatellite = 4,
        /// <summary>
        /// Needs API key now, see GlobalSettings.BingApiKey
        /// </summary>
        BingHybrid = 5,
        /// <summary>
        /// No longer supported due to their policy
        /// </summary>
        GoogleMaps = 6,
        /// <summary>
        /// No longer supported due to their policy
        /// </summary>
        GoogleSatellite = 7,
        /// <summary>
        /// No longer supported due to their policy
        /// </summary>
        GoogleHybrid = 8,
        /// <summary>
        /// No longer supported due to their policy
        /// </summary>
        GoogleTerrain = 9,
        /// <summary>
        /// Needs API key now, see GlobalSettings.SetHereMapsApiKey
        /// </summary>
        HereMaps = 10,
        /// <summary>
        /// Needs API key now, see GlobalSettings.SetHereMapsApiKey
        /// </summary>
        HereSatellite = 11,
        /// <summary>
        /// Needs API key now, see GlobalSettings.SetHereMapsApiKey
        /// </summary>
        HereHybrid = 12,
        /// <summary>
        /// Needs API key now, see GlobalSettings.SetHereMapsApiKey
        /// </summary>
        HereTerrain = 13,
        Rosreestr = 21,
        OpenHumanitarianMap = 22,
        MapQuestAerial = 23,
        ProviderCustom = 1024,
    }

    /// <summary>
    /// The snap modes as using in the ShapeEditor
    /// </summary>
    /// \new510 Added in version 5.1.0
    public enum tkSnapMode {
        /// <summary>
        /// Snap to vertices only
        /// </summary>
        smVertices = 0,
        /// <summary>
        /// Snap to vertices and lines
        /// </summary>
        smVerticesAndLines = 1,
        /// <summary>
        /// Snap to lines only
        /// </summary>
        smLines = 2
    }

#if nsp
}
#endif