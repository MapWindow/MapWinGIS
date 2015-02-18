unit QuayMapWinGIS_TLB;

// ************************************************************************ //
// WARNING                                                                    
// -------                                                                    
// The types declared in this file were generated from data read from a       
// Type Library. If this type library is explicitly or indirectly (via        
// another type library referring to this type library) re-imported, or the   
// 'Refresh' command of the Type Library Editor activated while editing the   
// Type Library, the contents of this file will be regenerated and all        
// manual modifications will be lost.                                         
// ************************************************************************ //

// PASTLWTR : 1.2
// File generated on 3/9/2009 2:28:30 PM from Type Library described below.

// ************************************************************************  //
// Type Lib: C:\Program Files\Borland\BDS\3.0\Imports\MapWindow\MapWinGIS.ocx (1)
// LIBID: {C368D713-CC5F-40ED-9F53-F84FE197B96A}
// LCID: 0
// Helpfile: C:\Program Files\Borland\BDS\3.0\Imports\MapWindow\MapWinGIS.chm
// HelpString: MapWinGIS Components
// DepndLst: 
//   (1) v2.0 stdole, (C:\WINDOWS\system32\stdole2.tlb)
// Errors:
//   Hint: Parameter 'Object' of _DMap.AddLayer changed to 'Object_'
//   Hint: Symbol 'Type' renamed to 'type_'
//   Hint: Symbol 'Type' renamed to 'type_'
//   Hint: Parameter 'Type' of IUtils.TinToShapefile changed to 'Type_'
// ************************************************************************ //
{$TYPEDADDRESS OFF} // Unit must be compiled without type-checked pointers. 
{$WARN SYMBOL_PLATFORM OFF}
{$WRITEABLECONST ON}
{$VARPROPSETTER ON}
interface

uses Windows, ActiveX, Classes, Graphics, OleCtrls, OleServer, StdVCL, Variants;
  


// *********************************************************************//
// GUIDS declared in the TypeLibrary. Following prefixes are used:        
//   Type Libraries     : LIBID_xxxx                                      
//   CoClasses          : CLASS_xxxx                                      
//   DISPInterfaces     : DIID_xxxx                                       
//   Non-DISP interfaces: IID_xxxx                                        
// *********************************************************************//
const
  // TypeLibrary Major and minor versions
  MapWinGISMajorVersion = 4;
  MapWinGISMinorVersion = 7;

  LIBID_MapWinGIS: TGUID = '{C368D713-CC5F-40ED-9F53-F84FE197B96A}';

  DIID__DMap: TGUID = '{1D077739-E866-46A0-B256-8AECC04F2312}';
  DIID__DMapEvents: TGUID = '{ABEA1545-08AB-4D5C-A594-D3017211EA95}';
  IID_IShapefileColorScheme: TGUID = '{FAE1B21A-10C5-4C33-8DC2-931EDC9FBF82}';
  IID_IShapefileColorBreak: TGUID = '{E6D4EB7A-3E8F-45B2-A514-90EF7B2F5C0A}';
  IID_ICallback: TGUID = '{90E6BBF7-A956-49BE-A5CD-A4640C263AB6}';
  CLASS_Map: TGUID = '{54F4C2F7-ED40-43B7-9D6F-E45965DF7F95}';
  CLASS_ShapefileColorScheme: TGUID = '{A038D3E9-46CB-4F95-A40A-88826BF71BA6}';
  CLASS_ShapefileColorBreak: TGUID = '{700A2AAA-0D28-4943-92EC-08AA9682617A}';
  IID_IGrid: TGUID = '{18DFB64A-9E72-4CBE-AFD6-A5B7421DD0CB}';
  CLASS_Grid: TGUID = '{B4A353E3-D3DF-455C-8E4D-CFC937800820}';
  IID_IGridHeader: TGUID = '{E42814D1-6269-41B1-93C2-AA848F00E459}';
  IID_IGridColorScheme: TGUID = '{1C43B56D-2065-4953-9138-31AFE8470FF5}';
  IID_IGridColorBreak: TGUID = '{1C6ECF5D-04FA-43C4-97B1-22D5FFB55FBD}';
  IID_IVector: TGUID = '{C60625AB-AD4C-405E-8CA2-62E36E4B3F73}';
  CLASS_GridHeader: TGUID = '{044AFE79-D3DE-4500-A14B-DECEA635B497}';
  IID_IESRIGridManager: TGUID = '{55B3F2DA-EB09-4FA9-B74B-9A1B3E457318}';
  CLASS_ESRIGridManager: TGUID = '{86E02063-602C-47F2-9778-81E6979E3267}';
  IID_IImage: TGUID = '{79C5F83E-FB53-4189-9EC4-4AC25440D825}';
  CLASS_Image: TGUID = '{0DB362E3-6F79-4226-AF19-47B67B27E99B}';
  IID_IShapefile: TGUID = '{5DC72405-C39C-4755-8CFC-9876A89225BC}';
  CLASS_Shapefile: TGUID = '{C0EAC9EB-1D02-4BD9-8DAB-4BF922C8CD13}';
  IID_IExtents: TGUID = '{A5692259-035E-487A-8D89-509DD6DD0F64}';
  IID_IShape: TGUID = '{5FA550E3-2044-4034-BAAA-B4CCE90A0C41}';
  IID_IPoint: TGUID = '{74F07889-1380-43EE-837A-BBB268311005}';
  IID_IField: TGUID = '{3F3751A5-4CF8-4AC3-AFC2-60DE8B90FC7B}';
  CLASS_Shape: TGUID = '{CE7E6869-6F74-4E9D-9F07-3DCBADAB6299}';
  CLASS_Extents: TGUID = '{03F9B3DB-637B-4544-BF7A-2F190F821F0D}';
  CLASS_Point: TGUID = '{CE63AD29-C5EB-4865-B143-E0AC35ED6FBC}';
  IID_ITable: TGUID = '{4365A8A1-2E46-4223-B2DC-65764262D88B}';
  CLASS_Table: TGUID = '{97EFB80F-3638-4BDC-9128-C5A30194C257}';
  CLASS_Field: TGUID = '{C2C71E09-3DEB-4E6C-B54A-D5613986BFFE}';
  IID_IShapeNetwork: TGUID = '{2D4968F2-40D9-4F25-8BE6-B51B959CC1B0}';
  CLASS_ShapeNetwork: TGUID = '{B655545F-1D9C-4D81-A73C-205FC2C3C4AB}';
  IID_IUtils: TGUID = '{360BEC33-7703-4693-B6CA-90FEA22CF1B7}';
  CLASS_Utils: TGUID = '{B898877F-DC9E-4FBF-B997-B65DC97B72E9}';
  IID_ITin: TGUID = '{55DD824E-332E-41CA-B40C-C8DC81EE209C}';
  CLASS_Vector: TGUID = '{D226C4B1-C97C-469D-8CBC-8E3DF2139612}';
  CLASS_GridColorScheme: TGUID = '{ECEB5841-F84E-4DFD-8C96-32216C69C818}';
  CLASS_GridColorBreak: TGUID = '{B82B0EB0-05B6-4FF2-AA16-BCD33FDE6568}';
  CLASS_Tin: TGUID = '{677B1AF6-A28D-4FAB-8A5F-0F8763D88638}';

// *********************************************************************//
// Declaration of Enumerations defined in Type Library                    
// *********************************************************************//
// Constants for enum tkCursorMode
type
  tkCursorMode = TOleEnum;
const
  cmZoomIn = $00000000;
  cmZoomOut = $00000001;
  cmPan = $00000002;
  cmSelection = $00000003;
  cmNone = $00000004;

// Constants for enum tkCursor
type
  tkCursor = TOleEnum;
const
  crsrMapDefault = $00000000;
  crsrAppStarting = $00000001;
  crsrArrow = $00000002;
  crsrCross = $00000003;
  crsrHelp = $00000004;
  crsrIBeam = $00000005;
  crsrNo = $00000006;
  crsrSizeAll = $00000007;
  crsrSizeNESW = $00000008;
  crsrSizeNS = $00000009;
  crsrSizeNWSE = $0000000A;
  crsrSizeWE = $0000000B;
  crsrUpArrow = $0000000C;
  crsrWait = $0000000D;
  crsrUserDefined = $0000000E;

// Constants for enum tkLockMode
type
  tkLockMode = TOleEnum;
const
  lmUnlock = $00000000;
  lmLock = $00000001;

// Constants for enum tkHJustification
type
  tkHJustification = TOleEnum;
const
  hjLeft = $00000000;
  hjCenter = $00000001;
  hjRight = $00000002;
  hjNone = $00000003;

// Constants for enum tkDrawReferenceList
type
  tkDrawReferenceList = TOleEnum;
const
  dlScreenReferencedList = $00000000;
  dlSpatiallyReferencedList = $00000001;

// Constants for enum tkLineStipple
type
  tkLineStipple = TOleEnum;
const
  lsNone = $00000000;
  lsDotted = $00000001;
  lsDashed = $00000002;
  lsDashDotDash = $00000003;
  lsDoubleSolid = $00000004;
  lsDoubleSolidPlusDash = $00000005;
  lsTrainTracks = $00000006;
  lsCustom = $00000007;

// Constants for enum tkFillStipple
type
  tkFillStipple = TOleEnum;
const
  fsNone = $00000000;
  fsVerticalBars = $00000001;
  fsHorizontalBars = $00000002;
  fsDiagonalDownRight = $00000003;
  fsDiagonalDownLeft = $00000004;
  fsPolkaDot = $00000005;
  fsCustom = $00000006;

// Constants for enum tkPointType
type
  tkPointType = TOleEnum;
const
  ptSquare = $00000000;
  ptCircle = $00000001;
  ptDiamond = $00000002;
  ptTriangleUp = $00000003;
  ptTriangleDown = $00000004;
  ptTriangleLeft = $00000005;
  ptTriangleRight = $00000006;
  ptUserDefined = $00000007;
  ptImageList = $00000008;
  ptFontChar = $00000009;

// Constants for enum tkResizeBehavior
type
  tkResizeBehavior = TOleEnum;
const
  rbClassic = $00000000;
  rbModern = $00000001;
  rbIntuitive = $00000002;
  rbWarp = $00000003;

// Constants for enum GridDataType
type
  GridDataType = TOleEnum;
const
  ShortDataType = $00000000;
  LongDataType = $00000001;
  FloatDataType = $00000002;
  DoubleDataType = $00000003;
  InvalidDataType = $FFFFFFFF;
  UnknownDataType = $00000004;

// Constants for enum GridFileType
type
  GridFileType = TOleEnum;
const
  Ascii = $00000000;
  Binary = $00000001;
  Esri = $00000002;
  GeoTiff = $00000003;
  Sdts = $00000004;
  PAux = $00000005;
  PCIDsk = $00000006;
  DTed = $00000007;
  Bil = $00000008;
  Ecw = $00000009;
  MrSid = $0000000A;
  Flt = $0000000B;
  UseExtension = $0000000C;
  InvalidGridFileType = $FFFFFFFF;

// Constants for enum ColoringType
type
  ColoringType = TOleEnum;
const
  Hillshade = $00000000;
  Gradient = $00000001;
  Random = $00000002;

// Constants for enum GradientModel
type
  GradientModel = TOleEnum;
const
  Logorithmic = $00000000;
  Linear = $00000001;
  Exponential = $00000002;

// Constants for enum PredefinedColorScheme
type
  PredefinedColorScheme = TOleEnum;
const
  FallLeaves = $00000000;
  SummerMountains = $00000001;
  Desert = $00000002;
  Glaciers = $00000003;
  Meadow = $00000004;
  ValleyFires = $00000005;
  DeadSea = $00000006;
  Highway1 = $00000007;

// Constants for enum ImageType
type
  ImageType = TOleEnum;
const
  BITMAP_FILE = $00000000;
  GIF_FILE = $00000001;
  USE_FILE_EXTENSION = $00000002;
  TIFF_FILE = $00000003;
  JPEG_FILE = $00000004;
  PNG_FILE = $00000005;
  PPM_FILE = $00000007;
  ECW_FILE = $00000008;
  JPEG2000_FILE = $00000009;
  SID_FILE = $0000000A;
  PNM_FILE = $0000000B;
  PGM_FILE = $0000000C;
  BIL_FILE = $0000000D;
  ADF_FILE = $0000000E;
  GRD_FILE = $0000000F;
  IMG_FILE = $00000010;
  ASC_FILE = $00000011;
  BT_FILE = $00000012;
  MAP_FILE = $00000013;
  LF2_FILE = $00000014;
  KAP_FILE = $00000015;

// Constants for enum ShpfileType
type
  ShpfileType = TOleEnum;
const
  SHP_NULLSHAPE = $00000000;
  SHP_POINT = $00000001;
  SHP_POLYLINE = $00000003;
  SHP_POLYGON = $00000005;
  SHP_MULTIPOINT = $00000008;
  SHP_POINTZ = $0000000B;
  SHP_POLYLINEZ = $0000000D;
  SHP_POLYGONZ = $0000000F;
  SHP_MULTIPOINTZ = $00000012;
  SHP_POINTM = $00000015;
  SHP_POLYLINEM = $00000017;
  SHP_POLYGONM = $00000019;
  SHP_MULTIPOINTM = $0000001C;
  SHP_MULTIPATCH = $0000001F;

// Constants for enum SelectMode
type
  SelectMode = TOleEnum;
const
  INTERSECTION = $00000000;
  INCLUSION = $00000001;

// Constants for enum FieldType
type
  FieldType = TOleEnum;
const
  STRING_FIELD = $00000000;
  INTEGER_FIELD = $00000001;
  DOUBLE_FIELD = $00000002;

// Constants for enum AmbiguityResolution
type
  AmbiguityResolution = TOleEnum;
const
  Z_VALUE = $00000000;
  DISTANCE_TO_OUTLET = $00000001;
  NO_RESOLUTION = $00000002;

// Constants for enum PolygonOperation
type
  PolygonOperation = TOleEnum;
const
  DIFFERENCE_OPERATION = $00000000;
  INTERSECTION_OPERATION = $00000001;
  EXCLUSIVEOR_OPERATION = $00000002;
  UNION_OPERATION = $00000003;

// Constants for enum SplitMethod
type
  SplitMethod = TOleEnum;
const
  InscribedRadius = $00000000;
  AngleDeviation = $00000001;

type

// *********************************************************************//
// Forward declaration of types defined in TypeLibrary                    
// *********************************************************************//
  _DMap = dispinterface;
  _DMapEvents = dispinterface;
  IShapefileColorScheme = interface;
  IShapefileColorSchemeDisp = dispinterface;
  IShapefileColorBreak = interface;
  IShapefileColorBreakDisp = dispinterface;
  ICallback = interface;
  ICallbackDisp = dispinterface;
  IGrid = interface;
  IGridDisp = dispinterface;
  IGridHeader = interface;
  IGridHeaderDisp = dispinterface;
  IGridColorScheme = interface;
  IGridColorSchemeDisp = dispinterface;
  IGridColorBreak = interface;
  IGridColorBreakDisp = dispinterface;
  IVector = interface;
  IVectorDisp = dispinterface;
  IESRIGridManager = interface;
  IESRIGridManagerDisp = dispinterface;
  IImage = interface;
  IImageDisp = dispinterface;
  IShapefile = interface;
  IShapefileDisp = dispinterface;
  IExtents = interface;
  IExtentsDisp = dispinterface;
  IShape = interface;
  IShapeDisp = dispinterface;
  IPoint = interface;
  IPointDisp = dispinterface;
  IField = interface;
  IFieldDisp = dispinterface;
  ITable = interface;
  ITableDisp = dispinterface;
  IShapeNetwork = interface;
  IShapeNetworkDisp = dispinterface;
  IUtils = interface;
  IUtilsDisp = dispinterface;
  ITin = interface;
  ITinDisp = dispinterface;

// *********************************************************************//
// Declaration of CoClasses defined in Type Library                       
// (NOTE: Here we map each CoClass to its Default Interface)              
// *********************************************************************//
  Map = _DMap;
  ShapefileColorScheme = IShapefileColorScheme;
  ShapefileColorBreak = IShapefileColorBreak;
  Grid = IGrid;
  GridHeader = IGridHeader;
  ESRIGridManager = IESRIGridManager;
  Image = IImage;
  Shapefile = IShapefile;
  Shape = IShape;
  Extents = IExtents;
  Point = IPoint;
  Table = ITable;
  Field = IField;
  ShapeNetwork = IShapeNetwork;
  Utils = IUtils;
  Vector = IVector;
  GridColorScheme = IGridColorScheme;
  GridColorBreak = IGridColorBreak;
  Tin = ITin;


// *********************************************************************//
// Declaration of structures, unions and aliases.                         
// *********************************************************************//
  PDouble1 = ^Double; {*}
  POleVariant1 = ^OleVariant; {*}
  PSYSINT1 = ^SYSINT; {*}
  PInteger1 = ^Integer; {*}


// *********************************************************************//
// DispIntf:  _DMap
// Flags:     (4112) Hidden Dispatchable
// GUID:      {1D077739-E866-46A0-B256-8AECC04F2312}
// *********************************************************************//
  _DMap = dispinterface
    ['{1D077739-E866-46A0-B256-8AECC04F2312}']
    property BackColor: OLE_COLOR dispid 1;
    property ZoomPercent: Double dispid 2;
    property CursorMode: tkCursorMode dispid 3;
    property MapCursor: tkCursor dispid 4;
    property UDCursorHandle: Integer dispid 5;
    property SendMouseDown: WordBool dispid 6;
    property SendMouseUp: WordBool dispid 7;
    property SendMouseMove: WordBool dispid 8;
    property SendSelectBoxDrag: WordBool dispid 9;
    property SendSelectBoxFinal: WordBool dispid 10;
    property ExtentPad: Double dispid 11;
    property ExtentHistory: Integer dispid 12;
    property Key: WideString dispid 13;
    property DoubleBuffer: WordBool dispid 14;
    property GlobalCallback: IDispatch dispid 15;
    property NumLayers: Integer dispid 16;
    property Extents: IDispatch dispid 17;
    property LastErrorCode: Integer dispid 18;
    property IsLocked: tkLockMode dispid 19;
    property MapState: WideString dispid 20;
    property SerialNumber: WideString dispid 95;
    property LineSeparationFactor: Integer dispid 96;
    property SendOnDrawBackBuffer: WordBool dispid 118;
    property MultilineLabels: WordBool dispid 133;
    procedure Redraw; dispid 21;
    function AddLayer(const Object_: IDispatch; Visible: WordBool): Integer; dispid 22;
    procedure RemoveLayer(LayerHandle: Integer); dispid 23;
    procedure RemoveLayerWithoutClosing(LayerHandle: Integer); dispid 138;
    procedure RemoveAllLayers; dispid 24;
    function MoveLayerUp(InitialPosition: Integer): WordBool; dispid 25;
    function MoveLayerDown(InitialPosition: Integer): WordBool; dispid 26;
    function MoveLayer(InitialPosition: Integer; TargetPosition: Integer): WordBool; dispid 27;
    function MoveLayerTop(InitialPosition: Integer): WordBool; dispid 28;
    function MoveLayerBottom(InitialPosition: Integer): WordBool; dispid 29;
    procedure ZoomToMaxExtents; dispid 30;
    procedure ZoomToLayer(LayerHandle: Integer); dispid 31;
    procedure ZoomToShape(LayerHandle: Integer; Shape: Integer); dispid 32;
    procedure ZoomIn(Percent: Double); dispid 33;
    procedure ZoomOut(Percent: Double); dispid 34;
    function ZoomToPrev: Integer; dispid 35;
    procedure ProjToPixel(projX: Double; projY: Double; var pixelX: Double; var pixelY: Double); dispid 36;
    procedure PixelToProj(pixelX: Double; pixelY: Double; var projX: Double; var projY: Double); dispid 37;
    procedure ClearDrawing(DrawHandle: Integer); dispid 38;
    procedure ClearDrawings; dispid 39;
    function SnapShot(const BoundBox: IDispatch): IDispatch; dispid 40;
    function ApplyLegendColors(const Legend: IDispatch): WordBool; dispid 41;
    procedure LockWindow(LockMode: tkLockMode); dispid 42;
    procedure Resize(Width: Integer; Height: Integer); dispid 43;
    procedure ShowToolTip(const Text: WideString; Milliseconds: Integer); dispid 44;
    procedure AddLabel(LayerHandle: Integer; const Text: WideString; Color: OLE_COLOR; x: Double; 
                       y: Double; hJustification: tkHJustification); dispid 45;
    procedure ClearLabels(LayerHandle: Integer); dispid 46;
    procedure LayerFont(LayerHandle: Integer; const FontName: WideString; FontSize: Integer); dispid 47;
    function GetColorScheme(LayerHandle: Integer): IDispatch; dispid 48;
    function NewDrawing(Projection: tkDrawReferenceList): Integer; dispid 49;
    procedure DrawPoint(x: Double; y: Double; pixelSize: Integer; Color: OLE_COLOR); dispid 50;
    procedure DrawLine(x1: Double; y1: Double; x2: Double; y2: Double; pixelWidth: Integer; 
                       Color: OLE_COLOR); dispid 51;
    procedure DrawCircle(x: Double; y: Double; pixelRadius: Double; Color: OLE_COLOR; fill: WordBool); dispid 52;
    procedure DrawPolygon(var xPoints: OleVariant; var yPoints: OleVariant; numPoints: Integer; 
                          Color: OLE_COLOR; fill: WordBool); dispid 53;
    property LayerKey[LayerHandle: Integer]: WideString dispid 54;
    property LayerPosition[LayerHandle: Integer]: Integer readonly dispid 55;
    property LayerHandle[LayerPosition: Integer]: Integer readonly dispid 56;
    property LayerVisible[LayerHandle: Integer]: WordBool dispid 57;
    property ShapeLayerFillColor[LayerHandle: Integer]: OLE_COLOR dispid 58;
    property ShapeFillColor[LayerHandle: Integer; Shape: Integer]: OLE_COLOR dispid 59;
    property ShapeLayerLineColor[LayerHandle: Integer]: OLE_COLOR dispid 60;
    property ShapeLineColor[LayerHandle: Integer; Shape: Integer]: OLE_COLOR dispid 61;
    property ShapeLayerPointColor[LayerHandle: Integer]: OLE_COLOR dispid 62;
    property ShapePointColor[LayerHandle: Integer; Shape: Integer]: OLE_COLOR dispid 63;
    property ShapeLayerDrawFill[LayerHandle: Integer]: WordBool dispid 64;
    property ShapeDrawFill[LayerHandle: Integer; Shape: Integer]: WordBool dispid 65;
    property ShapeLayerDrawLine[LayerHandle: Integer]: WordBool dispid 66;
    property ShapeDrawLine[LayerHandle: Integer; Shape: Integer]: WordBool dispid 67;
    property ShapeLayerDrawPoint[LayerHandle: Integer]: WordBool dispid 68;
    property ShapeDrawPoint[LayerHandle: Integer; Shape: Integer]: WordBool dispid 69;
    property ShapeLayerLineWidth[LayerHandle: Integer]: Single dispid 70;
    property ShapeLineWidth[LayerHandle: Integer; Shape: Integer]: Single dispid 71;
    property ShapeLayerPointSize[LayerHandle: Integer]: Single dispid 72;
    property ShapePointSize[LayerHandle: Integer; Shape: Integer]: Single dispid 73;
    property ShapeLayerFillTransparency[LayerHandle: Integer]: Single dispid 74;
    property ShapeFillTransparency[LayerHandle: Integer; Shape: Integer]: Single dispid 75;
    property ShapeLayerLineStipple[LayerHandle: Integer]: tkLineStipple dispid 76;
    property ShapeLineStipple[LayerHandle: Integer; Shape: Integer]: tkLineStipple dispid 77;
    property ShapeLayerFillStipple[LayerHandle: Integer]: tkFillStipple dispid 78;
    property ShapeFillStipple[LayerHandle: Integer; Shape: Integer]: tkFillStipple dispid 79;
    property ShapeVisible[LayerHandle: Integer; Shape: Integer]: WordBool dispid 80;
    property ImageLayerPercentTransparent[LayerHandle: Integer]: Single dispid 81;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 82;
    property DrawingKey[DrawHandle: Integer]: WideString dispid 83;
    property ShapeLayerPointType[LayerHandle: Integer]: tkPointType dispid 84;
    property ShapePointType[LayerHandle: Integer; Shape: Integer]: tkPointType dispid 85;
    property LayerLabelsVisible[LayerHandle: Integer]: WordBool dispid 86;
    property UDLineStipple[LayerHandle: Integer]: Integer dispid 87;
    property UDFillStipple[LayerHandle: Integer; StippleRow: Integer]: Integer dispid 88;
    property UDPointType[LayerHandle: Integer]: IDispatch dispid 89;
    property GetObject[LayerHandle: Integer]: IDispatch readonly dispid 90;
    property LayerName[LayerHandle: Integer]: WideString dispid 91;
    function SetImageLayerColorScheme(LayerHandle: Integer; const ColorScheme: IDispatch): WordBool; dispid 92;
    property GridFileName[LayerHandle: Integer]: WideString dispid 93;
    procedure UpdateImage(LayerHandle: Integer); dispid 94;
    property LayerLabelsShadow[LayerHandle: Integer]: WordBool dispid 97;
    property LayerLabelsScale[LayerHandle: Integer]: WordBool dispid 98;
    procedure AddLabelEx(LayerHandle: Integer; const Text: WideString; Color: OLE_COLOR; x: Double; 
                         y: Double; hJustification: tkHJustification; Rotation: Double); dispid 99;
    procedure GetLayerStandardViewWidth(LayerHandle: Integer; var Width: Double); dispid 100;
    procedure SetLayerStandardViewWidth(LayerHandle: Integer; Width: Double); dispid 101;
    property LayerLabelsOffset[LayerHandle: Integer]: Integer dispid 102;
    property LayerLabelsShadowColor[LayerHandle: Integer]: OLE_COLOR dispid 103;
    property UseLabelCollision[LayerHandle: Integer]: WordBool dispid 104;
    function IsTIFFGrid(const Filename: WideString): WordBool; dispid 105;
    function IsSameProjection(const proj4_a: WideString; const proj4_b: WideString): WordBool; dispid 106;
    procedure ZoomToMaxVisibleExtents; dispid 107;
    property MapResizeBehavior: tkResizeBehavior dispid 108;
    function HWnd: Integer; dispid 109;
    function set_UDPointImageListAdd(LayerHandle: Integer; const newValue: IDispatch): Integer; dispid 110;
    property ShapePointImageListID[LayerHandle: Integer; Shape: Integer]: Integer dispid 111;
    function get_UDPointImageListCount(LayerHandle: Integer): Integer; dispid 112;
    function get_UDPointImageListItem(LayerHandle: Integer; ImageIndex: Integer): IDispatch; dispid 113;
    procedure ClearUDPointImageList(LayerHandle: Integer); dispid 114;
    procedure DrawLineEx(LayerHandle: Integer; x1: Double; y1: Double; x2: Double; y2: Double; 
                         pixelWidth: Integer; Color: OLE_COLOR); dispid 115;
    procedure DrawPointEx(LayerHandle: Integer; x: Double; y: Double; pixelSize: Integer; 
                          Color: OLE_COLOR); dispid 116;
    procedure DrawCircleEx(LayerHandle: Integer; x: Double; y: Double; pixelRadius: Double; 
                           Color: OLE_COLOR; fill: WordBool); dispid 117;
    procedure LabelColor(LayerHandle: Integer; LabelFontColor: OLE_COLOR); dispid 119;
    procedure SetDrawingLayerVisible(LayerHandle: Integer; Visiable: WordBool); dispid 120;
    procedure ClearDrawingLabels(DrawHandle: Integer); dispid 121;
    procedure DrawingFont(DrawHandle: Integer; const FontName: WideString; FontSize: Integer); dispid 122;
    procedure AddDrawingLabelEx(DrawHandle: Integer; const Text: WideString; Color: OLE_COLOR; 
                                x: Double; y: Double; hJustification: tkHJustification; 
                                Rotation: Double); dispid 123;
    procedure AddDrawingLabel(DrawHandle: Integer; const Text: WideString; Color: OLE_COLOR; 
                              x: Double; y: Double; hJustification: tkHJustification); dispid 124;
    property DrawingLabelsOffset[DrawHandle: Integer]: Integer dispid 125;
    property DrawingLabelsScale[DrawHandle: Integer]: WordBool dispid 126;
    property DrawingLabelsShadow[DrawHandle: Integer]: WordBool dispid 127;
    property DrawingLabelsShadowColor[DrawHandle: Integer]: OLE_COLOR dispid 128;
    property UseDrawingLabelCollision[DrawHandle: Integer]: WordBool dispid 129;
    property DrawingLabelsVisible[DrawHandle: Integer]: WordBool dispid 130;
    procedure GetDrawingStandardViewWidth(DrawHandle: Integer; var Width: Double); dispid 131;
    procedure SetDrawingStandardViewWidth(DrawHandle: Integer; Width: Double); dispid 132;
    procedure DrawWidePolygon(var xPoints: OleVariant; var yPoints: OleVariant; numPoints: Integer; 
                              Color: OLE_COLOR; fill: WordBool; Width: Smallint); dispid 134;
    procedure DrawWideCircle(x: Double; y: Double; pixelRadius: Double; Color: OLE_COLOR; 
                             fill: WordBool; Width: Smallint); dispid 135;
    function SnapShot2(ClippingLayerNbr: Integer; Zoom: Double; pWidth: Integer): IDispatch; dispid 136;
    procedure LayerFontEx(LayerHandle: Integer; const FontName: WideString; FontSize: Integer; 
                          isBold: WordBool; isItalic: WordBool; isUnderline: WordBool); dispid 137;
    procedure set_UDPointFontCharFont(LayerHandle: Integer; const FontName: WideString; 
                                      FontSize: Single; isBold: WordBool; isItalic: WordBool; 
                                      isUnderline: WordBool); dispid 139;
    function set_UDPointFontCharListAdd(LayerHandle: Integer; newValue: Integer; Color: OLE_COLOR): Integer; dispid 140;
    procedure set_UDPointFontCharFontSize(LayerHandle: Integer; FontSize: Single); dispid 141;
    property ShapePointFontCharListID[LayerHandle: Integer; Shape: Integer]: Integer dispid 142;
    procedure ReSourceLayer(LayerHandle: Integer; const newSrcPath: WideString); dispid 143;
    property ShapeLayerStippleColor[LayerHandle: Integer]: OLE_COLOR dispid 144;
    property ShapeStippleColor[LayerHandle: Integer; Shape: Integer]: OLE_COLOR dispid 145;
    property ShapeStippleTransparent[LayerHandle: Integer; Shape: Integer]: WordBool dispid 146;
    property ShapeLayerStippleTransparent[LayerHandle: Integer]: WordBool dispid 147;
    property TrapRMouseDown: WordBool dispid 148;
  end;

// *********************************************************************//
// DispIntf:  _DMapEvents
// Flags:     (4096) Dispatchable
// GUID:      {ABEA1545-08AB-4D5C-A594-D3017211EA95}
// *********************************************************************//
  _DMapEvents = dispinterface
    ['{ABEA1545-08AB-4D5C-A594-D3017211EA95}']
    procedure MouseDown(Button: Smallint; Shift: Smallint; x: Integer; y: Integer); dispid 1;
    procedure MouseUp(Button: Smallint; Shift: Smallint; x: Integer; y: Integer); dispid 2;
    procedure MouseMove(Button: Smallint; Shift: Smallint; x: Integer; y: Integer); dispid 3;
    procedure FileDropped(const Filename: WideString); dispid 4;
    procedure SelectBoxFinal(Left: Integer; Right: Integer; Bottom: Integer; Top: Integer); dispid 5;
    procedure SelectBoxDrag(Left: Integer; Right: Integer; Bottom: Integer; Top: Integer); dispid 6;
    procedure ExtentsChanged; dispid 7;
    procedure MapState(LayerHandle: Integer); dispid 8;
    procedure OnDrawBackBuffer(BackBuffer: Integer); dispid 9;
    procedure DblClick; dispid -601;
  end;

// *********************************************************************//
// Interface: IShapefileColorScheme
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {FAE1B21A-10C5-4C33-8DC2-931EDC9FBF82}
// *********************************************************************//
  IShapefileColorScheme = interface(IDispatch)
    ['{FAE1B21A-10C5-4C33-8DC2-931EDC9FBF82}']
    function NumBreaks: Integer; safecall;
    procedure Remove(Index: Integer); safecall;
    function Add(const Break: IShapefileColorBreak): Integer; safecall;
    function Get_ColorBreak(Index: Integer): IShapefileColorBreak; safecall;
    procedure Set_ColorBreak(Index: Integer; const pVal: IShapefileColorBreak); safecall;
    function Get_LayerHandle: Integer; safecall;
    procedure Set_LayerHandle(pVal: Integer); safecall;
    function Get_FieldIndex: Integer; safecall;
    procedure Set_FieldIndex(pVal: Integer); safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    function InsertAt(Position: SYSINT; const Break: IShapefileColorBreak): Integer; safecall;
    property ColorBreak[Index: Integer]: IShapefileColorBreak read Get_ColorBreak write Set_ColorBreak;
    property LayerHandle: Integer read Get_LayerHandle write Set_LayerHandle;
    property FieldIndex: Integer read Get_FieldIndex write Set_FieldIndex;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
  end;

// *********************************************************************//
// DispIntf:  IShapefileColorSchemeDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {FAE1B21A-10C5-4C33-8DC2-931EDC9FBF82}
// *********************************************************************//
  IShapefileColorSchemeDisp = dispinterface
    ['{FAE1B21A-10C5-4C33-8DC2-931EDC9FBF82}']
    function NumBreaks: Integer; dispid 1;
    procedure Remove(Index: Integer); dispid 2;
    function Add(const Break: IShapefileColorBreak): Integer; dispid 3;
    property ColorBreak[Index: Integer]: IShapefileColorBreak dispid 4;
    property LayerHandle: Integer dispid 5;
    property FieldIndex: Integer dispid 6;
    property LastErrorCode: Integer readonly dispid 7;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 8;
    property GlobalCallback: ICallback dispid 9;
    property Key: WideString dispid 10;
    function InsertAt(Position: SYSINT; const Break: IShapefileColorBreak): Integer; dispid 11;
  end;

// *********************************************************************//
// Interface: IShapefileColorBreak
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {E6D4EB7A-3E8F-45B2-A514-90EF7B2F5C0A}
// *********************************************************************//
  IShapefileColorBreak = interface(IDispatch)
    ['{E6D4EB7A-3E8F-45B2-A514-90EF7B2F5C0A}']
    function Get_StartValue: OleVariant; safecall;
    procedure Set_StartValue(pVal: OleVariant); safecall;
    function Get_EndValue: OleVariant; safecall;
    procedure Set_EndValue(pVal: OleVariant); safecall;
    function Get_StartColor: OLE_COLOR; safecall;
    procedure Set_StartColor(pVal: OLE_COLOR); safecall;
    function Get_EndColor: OLE_COLOR; safecall;
    procedure Set_EndColor(pVal: OLE_COLOR); safecall;
    function Get_Caption: WideString; safecall;
    procedure Set_Caption(const pVal: WideString); safecall;
    function Get_Visible: WordBool; safecall;
    procedure Set_Visible(pVal: WordBool); safecall;
    property StartValue: OleVariant read Get_StartValue write Set_StartValue;
    property EndValue: OleVariant read Get_EndValue write Set_EndValue;
    property StartColor: OLE_COLOR read Get_StartColor write Set_StartColor;
    property EndColor: OLE_COLOR read Get_EndColor write Set_EndColor;
    property Caption: WideString read Get_Caption write Set_Caption;
    property Visible: WordBool read Get_Visible write Set_Visible;
  end;

// *********************************************************************//
// DispIntf:  IShapefileColorBreakDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {E6D4EB7A-3E8F-45B2-A514-90EF7B2F5C0A}
// *********************************************************************//
  IShapefileColorBreakDisp = dispinterface
    ['{E6D4EB7A-3E8F-45B2-A514-90EF7B2F5C0A}']
    property StartValue: OleVariant dispid 1;
    property EndValue: OleVariant dispid 2;
    property StartColor: OLE_COLOR dispid 3;
    property EndColor: OLE_COLOR dispid 4;
    property Caption: WideString dispid 5;
    property Visible: WordBool dispid 6;
  end;

// *********************************************************************//
// Interface: ICallback
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {90E6BBF7-A956-49BE-A5CD-A4640C263AB6}
// *********************************************************************//
  ICallback = interface(IDispatch)
    ['{90E6BBF7-A956-49BE-A5CD-A4640C263AB6}']
    procedure Progress(const KeyOfSender: WideString; Percent: Integer; const Message: WideString); safecall;
    procedure Error(const KeyOfSender: WideString; const ErrorMsg: WideString); safecall;
  end;

// *********************************************************************//
// DispIntf:  ICallbackDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {90E6BBF7-A956-49BE-A5CD-A4640C263AB6}
// *********************************************************************//
  ICallbackDisp = dispinterface
    ['{90E6BBF7-A956-49BE-A5CD-A4640C263AB6}']
    procedure Progress(const KeyOfSender: WideString; Percent: Integer; const Message: WideString); dispid 1;
    procedure Error(const KeyOfSender: WideString; const ErrorMsg: WideString); dispid 2;
  end;

// *********************************************************************//
// Interface: IGrid
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {18DFB64A-9E72-4CBE-AFD6-A5B7421DD0CB}
// *********************************************************************//
  IGrid = interface(IDispatch)
    ['{18DFB64A-9E72-4CBE-AFD6-A5B7421DD0CB}']
    function Get_Header: IGridHeader; safecall;
    function Get_Value(Column: Integer; Row: Integer): OleVariant; safecall;
    procedure Set_Value(Column: Integer; Row: Integer; pVal: OleVariant); safecall;
    function Get_InRam: WordBool; safecall;
    function Get_Maximum: OleVariant; safecall;
    function Get_Minimum: OleVariant; safecall;
    function Get_DataType: GridDataType; safecall;
    function Get_Filename: WideString; safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    function Open(const Filename: WideString; DataType: GridDataType; InRam: WordBool; 
                  FileType: GridFileType; const cBack: ICallback): WordBool; safecall;
    function CreateNew(const Filename: WideString; const Header: IGridHeader; 
                       DataType: GridDataType; InitialValue: OleVariant; InRam: WordBool; 
                       FileType: GridFileType; const cBack: ICallback): WordBool; safecall;
    function Close: WordBool; safecall;
    function Save(const Filename: WideString; GridFileType: GridFileType; const cBack: ICallback): WordBool; safecall;
    function Clear(ClearValue: OleVariant): WordBool; safecall;
    procedure ProjToCell(x: Double; y: Double; out Column: Integer; out Row: Integer); safecall;
    procedure CellToProj(Column: Integer; Row: Integer; out x: Double; out y: Double); safecall;
    function Get_CdlgFilter: WideString; safecall;
    function AssignNewProjection(const Projection: WideString): WordBool; safecall;
    function Get_RasterColorTableColoringScheme: IGridColorScheme; safecall;
    function GetRow(Row: Integer; var Vals: Single): WordBool; safecall;
    function PutRow(Row: Integer; var Vals: Single): WordBool; safecall;
    function GetFloatWindow(StartRow: Integer; EndRow: Integer; StartCol: Integer; EndCol: Integer; 
                            var Vals: Single): WordBool; safecall;
    function PutFloatWindow(StartRow: Integer; EndRow: Integer; StartCol: Integer; EndCol: Integer; 
                            var Vals: Single): WordBool; safecall;
    function SetInvalidValuesToNodata(MinThresholdValue: Double; MaxThresholdValue: Double): WordBool; safecall;
    function Resource(const newSrcPath: WideString): WordBool; safecall;
    property Header: IGridHeader read Get_Header;
    property Value[Column: Integer; Row: Integer]: OleVariant read Get_Value write Set_Value;
    property InRam: WordBool read Get_InRam;
    property Maximum: OleVariant read Get_Maximum;
    property Minimum: OleVariant read Get_Minimum;
    property DataType: GridDataType read Get_DataType;
    property Filename: WideString read Get_Filename;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
    property CdlgFilter: WideString read Get_CdlgFilter;
    property RasterColorTableColoringScheme: IGridColorScheme read Get_RasterColorTableColoringScheme;
  end;

// *********************************************************************//
// DispIntf:  IGridDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {18DFB64A-9E72-4CBE-AFD6-A5B7421DD0CB}
// *********************************************************************//
  IGridDisp = dispinterface
    ['{18DFB64A-9E72-4CBE-AFD6-A5B7421DD0CB}']
    property Header: IGridHeader readonly dispid 1;
    property Value[Column: Integer; Row: Integer]: OleVariant dispid 2;
    property InRam: WordBool readonly dispid 3;
    property Maximum: OleVariant readonly dispid 4;
    property Minimum: OleVariant readonly dispid 5;
    property DataType: GridDataType readonly dispid 6;
    property Filename: WideString readonly dispid 7;
    property LastErrorCode: Integer readonly dispid 8;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 9;
    property GlobalCallback: ICallback dispid 10;
    property Key: WideString dispid 11;
    function Open(const Filename: WideString; DataType: GridDataType; InRam: WordBool; 
                  FileType: GridFileType; const cBack: ICallback): WordBool; dispid 12;
    function CreateNew(const Filename: WideString; const Header: IGridHeader; 
                       DataType: GridDataType; InitialValue: OleVariant; InRam: WordBool; 
                       FileType: GridFileType; const cBack: ICallback): WordBool; dispid 13;
    function Close: WordBool; dispid 14;
    function Save(const Filename: WideString; GridFileType: GridFileType; const cBack: ICallback): WordBool; dispid 15;
    function Clear(ClearValue: OleVariant): WordBool; dispid 16;
    procedure ProjToCell(x: Double; y: Double; out Column: Integer; out Row: Integer); dispid 17;
    procedure CellToProj(Column: Integer; Row: Integer; out x: Double; out y: Double); dispid 18;
    property CdlgFilter: WideString readonly dispid 19;
    function AssignNewProjection(const Projection: WideString): WordBool; dispid 20;
    property RasterColorTableColoringScheme: IGridColorScheme readonly dispid 21;
    function GetRow(Row: Integer; var Vals: Single): WordBool; dispid 22;
    function PutRow(Row: Integer; var Vals: Single): WordBool; dispid 23;
    function GetFloatWindow(StartRow: Integer; EndRow: Integer; StartCol: Integer; EndCol: Integer; 
                            var Vals: Single): WordBool; dispid 24;
    function PutFloatWindow(StartRow: Integer; EndRow: Integer; StartCol: Integer; EndCol: Integer; 
                            var Vals: Single): WordBool; dispid 25;
    function SetInvalidValuesToNodata(MinThresholdValue: Double; MaxThresholdValue: Double): WordBool; dispid 26;
    function Resource(const newSrcPath: WideString): WordBool; dispid 27;
  end;

// *********************************************************************//
// Interface: IGridHeader
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {E42814D1-6269-41B1-93C2-AA848F00E459}
// *********************************************************************//
  IGridHeader = interface(IDispatch)
    ['{E42814D1-6269-41B1-93C2-AA848F00E459}']
    function Get_NumberCols: Integer; safecall;
    procedure Set_NumberCols(pVal: Integer); safecall;
    function Get_NumberRows: Integer; safecall;
    procedure Set_NumberRows(pVal: Integer); safecall;
    function Get_NodataValue: OleVariant; safecall;
    procedure Set_NodataValue(pVal: OleVariant); safecall;
    function Get_dX: Double; safecall;
    procedure Set_dX(pVal: Double); safecall;
    function Get_dY: Double; safecall;
    procedure Set_dY(pVal: Double); safecall;
    function Get_XllCenter: Double; safecall;
    procedure Set_XllCenter(pVal: Double); safecall;
    function Get_YllCenter: Double; safecall;
    procedure Set_YllCenter(pVal: Double); safecall;
    function Get_Projection: WideString; safecall;
    procedure Set_Projection(const pVal: WideString); safecall;
    function Get_Notes: WideString; safecall;
    procedure Set_Notes(const pVal: WideString); safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    procedure Set_Owner(var t: SYSINT; var d: SYSINT; var s: SYSINT; var l: SYSINT; Param5: PSYSINT1); safecall;
    procedure CopyFrom(const pVal: IGridHeader); safecall;
    property NumberCols: Integer read Get_NumberCols write Set_NumberCols;
    property NumberRows: Integer read Get_NumberRows write Set_NumberRows;
    property NodataValue: OleVariant read Get_NodataValue write Set_NodataValue;
    property dX: Double read Get_dX write Set_dX;
    property dY: Double read Get_dY write Set_dY;
    property XllCenter: Double read Get_XllCenter write Set_XllCenter;
    property YllCenter: Double read Get_YllCenter write Set_YllCenter;
    property Projection: WideString read Get_Projection write Set_Projection;
    property Notes: WideString read Get_Notes write Set_Notes;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
    property Owner[var t: SYSINT; var d: SYSINT; var s: SYSINT; var l: SYSINT]: PSYSINT1 write Set_Owner;
  end;

// *********************************************************************//
// DispIntf:  IGridHeaderDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {E42814D1-6269-41B1-93C2-AA848F00E459}
// *********************************************************************//
  IGridHeaderDisp = dispinterface
    ['{E42814D1-6269-41B1-93C2-AA848F00E459}']
    property NumberCols: Integer dispid 1;
    property NumberRows: Integer dispid 2;
    property NodataValue: OleVariant dispid 3;
    property dX: Double dispid 4;
    property dY: Double dispid 5;
    property XllCenter: Double dispid 6;
    property YllCenter: Double dispid 7;
    property Projection: WideString dispid 8;
    property Notes: WideString dispid 9;
    property LastErrorCode: Integer readonly dispid 10;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 11;
    property GlobalCallback: ICallback dispid 12;
    property Key: WideString dispid 13;
    property Owner[var t: SYSINT; var d: SYSINT; var s: SYSINT; var l: SYSINT]: {??PSYSINT1}OleVariant writeonly dispid 14;
    procedure CopyFrom(const pVal: IGridHeader); dispid 15;
  end;

// *********************************************************************//
// Interface: IGridColorScheme
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {1C43B56D-2065-4953-9138-31AFE8470FF5}
// *********************************************************************//
  IGridColorScheme = interface(IDispatch)
    ['{1C43B56D-2065-4953-9138-31AFE8470FF5}']
    function Get_NumBreaks: Integer; safecall;
    function Get_AmbientIntensity: Double; safecall;
    procedure Set_AmbientIntensity(pVal: Double); safecall;
    function Get_LightSourceIntensity: Double; safecall;
    procedure Set_LightSourceIntensity(pVal: Double); safecall;
    function Get_LightSourceAzimuth: Double; safecall;
    function Get_LightSourceElevation: Double; safecall;
    procedure SetLightSource(Azimuth: Double; Elevation: Double); safecall;
    procedure InsertBreak(const BrkInfo: IGridColorBreak); safecall;
    function Get_Break(Index: Integer): IGridColorBreak; safecall;
    procedure DeleteBreak(Index: Integer); safecall;
    procedure Clear; safecall;
    function Get_NoDataColor: OLE_COLOR; safecall;
    procedure Set_NoDataColor(pVal: OLE_COLOR); safecall;
    procedure UsePredefined(LowValue: Double; HighValue: Double; Preset: PredefinedColorScheme); safecall;
    function GetLightSource: IVector; safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    procedure InsertAt(Position: SYSINT; const Break: IGridColorBreak); safecall;
    property NumBreaks: Integer read Get_NumBreaks;
    property AmbientIntensity: Double read Get_AmbientIntensity write Set_AmbientIntensity;
    property LightSourceIntensity: Double read Get_LightSourceIntensity write Set_LightSourceIntensity;
    property LightSourceAzimuth: Double read Get_LightSourceAzimuth;
    property LightSourceElevation: Double read Get_LightSourceElevation;
    property Break[Index: Integer]: IGridColorBreak read Get_Break;
    property NoDataColor: OLE_COLOR read Get_NoDataColor write Set_NoDataColor;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
  end;

// *********************************************************************//
// DispIntf:  IGridColorSchemeDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {1C43B56D-2065-4953-9138-31AFE8470FF5}
// *********************************************************************//
  IGridColorSchemeDisp = dispinterface
    ['{1C43B56D-2065-4953-9138-31AFE8470FF5}']
    property NumBreaks: Integer readonly dispid 1;
    property AmbientIntensity: Double dispid 2;
    property LightSourceIntensity: Double dispid 3;
    property LightSourceAzimuth: Double readonly dispid 4;
    property LightSourceElevation: Double readonly dispid 5;
    procedure SetLightSource(Azimuth: Double; Elevation: Double); dispid 6;
    procedure InsertBreak(const BrkInfo: IGridColorBreak); dispid 7;
    property Break[Index: Integer]: IGridColorBreak readonly dispid 8;
    procedure DeleteBreak(Index: Integer); dispid 9;
    procedure Clear; dispid 10;
    property NoDataColor: OLE_COLOR dispid 11;
    procedure UsePredefined(LowValue: Double; HighValue: Double; Preset: PredefinedColorScheme); dispid 12;
    function GetLightSource: IVector; dispid 13;
    property LastErrorCode: Integer readonly dispid 14;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 15;
    property GlobalCallback: ICallback dispid 16;
    property Key: WideString dispid 17;
    procedure InsertAt(Position: SYSINT; const Break: IGridColorBreak); dispid 18;
  end;

// *********************************************************************//
// Interface: IGridColorBreak
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {1C6ECF5D-04FA-43C4-97B1-22D5FFB55FBD}
// *********************************************************************//
  IGridColorBreak = interface(IDispatch)
    ['{1C6ECF5D-04FA-43C4-97B1-22D5FFB55FBD}']
    function Get_HighColor: OLE_COLOR; safecall;
    procedure Set_HighColor(pVal: OLE_COLOR); safecall;
    function Get_LowColor: OLE_COLOR; safecall;
    procedure Set_LowColor(pVal: OLE_COLOR); safecall;
    function Get_HighValue: Double; safecall;
    procedure Set_HighValue(pVal: Double); safecall;
    function Get_LowValue: Double; safecall;
    procedure Set_LowValue(pVal: Double); safecall;
    function Get_ColoringType: ColoringType; safecall;
    procedure Set_ColoringType(pVal: ColoringType); safecall;
    function Get_GradientModel: GradientModel; safecall;
    procedure Set_GradientModel(pVal: GradientModel); safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    function Get_Caption: WideString; safecall;
    procedure Set_Caption(const pVal: WideString); safecall;
    property HighColor: OLE_COLOR read Get_HighColor write Set_HighColor;
    property LowColor: OLE_COLOR read Get_LowColor write Set_LowColor;
    property HighValue: Double read Get_HighValue write Set_HighValue;
    property LowValue: Double read Get_LowValue write Set_LowValue;
    property ColoringType: ColoringType read Get_ColoringType write Set_ColoringType;
    property GradientModel: GradientModel read Get_GradientModel write Set_GradientModel;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
    property Caption: WideString read Get_Caption write Set_Caption;
  end;

// *********************************************************************//
// DispIntf:  IGridColorBreakDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {1C6ECF5D-04FA-43C4-97B1-22D5FFB55FBD}
// *********************************************************************//
  IGridColorBreakDisp = dispinterface
    ['{1C6ECF5D-04FA-43C4-97B1-22D5FFB55FBD}']
    property HighColor: OLE_COLOR dispid 1;
    property LowColor: OLE_COLOR dispid 2;
    property HighValue: Double dispid 3;
    property LowValue: Double dispid 4;
    property ColoringType: ColoringType dispid 5;
    property GradientModel: GradientModel dispid 6;
    property LastErrorCode: Integer readonly dispid 7;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 8;
    property GlobalCallback: ICallback dispid 9;
    property Key: WideString dispid 10;
    property Caption: WideString dispid 11;
  end;

// *********************************************************************//
// Interface: IVector
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {C60625AB-AD4C-405E-8CA2-62E36E4B3F73}
// *********************************************************************//
  IVector = interface(IDispatch)
    ['{C60625AB-AD4C-405E-8CA2-62E36E4B3F73}']
    function Get_i: Double; safecall;
    procedure Set_i(pVal: Double); safecall;
    function Get_j: Double; safecall;
    procedure Set_j(pVal: Double); safecall;
    function Get_k: Double; safecall;
    procedure Set_k(pVal: Double); safecall;
    procedure Normalize; safecall;
    function Dot(const V: IVector): Double; safecall;
    function CrossProduct(const V: IVector): IVector; safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    property i: Double read Get_i write Set_i;
    property j: Double read Get_j write Set_j;
    property k: Double read Get_k write Set_k;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
  end;

// *********************************************************************//
// DispIntf:  IVectorDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {C60625AB-AD4C-405E-8CA2-62E36E4B3F73}
// *********************************************************************//
  IVectorDisp = dispinterface
    ['{C60625AB-AD4C-405E-8CA2-62E36E4B3F73}']
    property i: Double dispid 1;
    property j: Double dispid 2;
    property k: Double dispid 3;
    procedure Normalize; dispid 4;
    function Dot(const V: IVector): Double; dispid 5;
    function CrossProduct(const V: IVector): IVector; dispid 6;
    property LastErrorCode: Integer readonly dispid 7;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 8;
    property GlobalCallback: ICallback dispid 9;
    property Key: WideString dispid 10;
  end;

// *********************************************************************//
// Interface: IESRIGridManager
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {55B3F2DA-EB09-4FA9-B74B-9A1B3E457318}
// *********************************************************************//
  IESRIGridManager = interface(IDispatch)
    ['{55B3F2DA-EB09-4FA9-B74B-9A1B3E457318}']
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function CanUseESRIGrids: WordBool; safecall;
    function DeleteESRIGrids(const Filename: WideString): WordBool; safecall;
    function IsESRIGrid(const Filename: WideString): WordBool; safecall;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
  end;

// *********************************************************************//
// DispIntf:  IESRIGridManagerDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {55B3F2DA-EB09-4FA9-B74B-9A1B3E457318}
// *********************************************************************//
  IESRIGridManagerDisp = dispinterface
    ['{55B3F2DA-EB09-4FA9-B74B-9A1B3E457318}']
    property LastErrorCode: Integer readonly dispid 1;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 2;
    property GlobalCallback: ICallback dispid 3;
    function CanUseESRIGrids: WordBool; dispid 4;
    function DeleteESRIGrids(const Filename: WideString): WordBool; dispid 5;
    function IsESRIGrid(const Filename: WideString): WordBool; dispid 6;
  end;

// *********************************************************************//
// Interface: IImage
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {79C5F83E-FB53-4189-9EC4-4AC25440D825}
// *********************************************************************//
  IImage = interface(IDispatch)
    ['{79C5F83E-FB53-4189-9EC4-4AC25440D825}']
    function Open(const ImageFileName: WideString; FileType: ImageType; InRam: WordBool; 
                  const cBack: ICallback): WordBool; safecall;
    function Save(const ImageFileName: WideString; WriteWorldFile: WordBool; FileType: ImageType; 
                  const cBack: ICallback): WordBool; safecall;
    function CreateNew(NewWidth: Integer; NewHeight: Integer): WordBool; safecall;
    function Close: WordBool; safecall;
    function Clear(CanvasColor: OLE_COLOR; const cBack: ICallback): WordBool; safecall;
    function GetRow(Row: Integer; var Vals: Integer): WordBool; safecall;
    function Get_Width: Integer; safecall;
    function Get_Height: Integer; safecall;
    function Get_YllCenter: Double; safecall;
    procedure Set_YllCenter(pVal: Double); safecall;
    function Get_XllCenter: Double; safecall;
    procedure Set_XllCenter(pVal: Double); safecall;
    function Get_dY: Double; safecall;
    procedure Set_dY(pVal: Double); safecall;
    function Get_dX: Double; safecall;
    procedure Set_dX(pVal: Double); safecall;
    function Get_Value(Row: Integer; col: Integer): Integer; safecall;
    procedure Set_Value(Row: Integer; col: Integer; pVal: Integer); safecall;
    function Get_IsInRam: WordBool; safecall;
    function Get_TransparencyColor: OLE_COLOR; safecall;
    procedure Set_TransparencyColor(pVal: OLE_COLOR); safecall;
    function Get_UseTransparencyColor: WordBool; safecall;
    procedure Set_UseTransparencyColor(pVal: WordBool); safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_CdlgFilter: WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    function Get_FileHandle: Integer; safecall;
    function Get_ImageType: ImageType; safecall;
    function Get_Picture: IPictureDisp; safecall;
    procedure _Set_Picture(const pVal: IPictureDisp); safecall;
    function Get_Filename: WideString; safecall;
    function GetImageBitsDC(hDC: Integer): WordBool; safecall;
    function SetImageBitsDC(hDC: Integer): WordBool; safecall;
    procedure SetVisibleExtents(newMinX: Double; newMinY: Double; newMaxX: Double; newMaxY: Double; 
                                newPixelsInView: Integer; transPercent: Single); safecall;
    function SetProjection(const Proj4: WideString): WordBool; safecall;
    function GetProjection: WideString; safecall;
    procedure get_OriginalWidth(var OriginalWidth: Integer); safecall;
    procedure get_OriginalHeight(var OriginalHeight: Integer); safecall;
    function Resource(const newImgPath: WideString): WordBool; safecall;
    property Width: Integer read Get_Width;
    property Height: Integer read Get_Height;
    property YllCenter: Double read Get_YllCenter write Set_YllCenter;
    property XllCenter: Double read Get_XllCenter write Set_XllCenter;
    property dY: Double read Get_dY write Set_dY;
    property dX: Double read Get_dX write Set_dX;
    property Value[Row: Integer; col: Integer]: Integer read Get_Value write Set_Value;
    property IsInRam: WordBool read Get_IsInRam;
    property TransparencyColor: OLE_COLOR read Get_TransparencyColor write Set_TransparencyColor;
    property UseTransparencyColor: WordBool read Get_UseTransparencyColor write Set_UseTransparencyColor;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property CdlgFilter: WideString read Get_CdlgFilter;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
    property FileHandle: Integer read Get_FileHandle;
    property ImageType: ImageType read Get_ImageType;
    property Picture: IPictureDisp read Get_Picture write _Set_Picture;
    property Filename: WideString read Get_Filename;
  end;

// *********************************************************************//
// DispIntf:  IImageDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {79C5F83E-FB53-4189-9EC4-4AC25440D825}
// *********************************************************************//
  IImageDisp = dispinterface
    ['{79C5F83E-FB53-4189-9EC4-4AC25440D825}']
    function Open(const ImageFileName: WideString; FileType: ImageType; InRam: WordBool; 
                  const cBack: ICallback): WordBool; dispid 1;
    function Save(const ImageFileName: WideString; WriteWorldFile: WordBool; FileType: ImageType; 
                  const cBack: ICallback): WordBool; dispid 2;
    function CreateNew(NewWidth: Integer; NewHeight: Integer): WordBool; dispid 3;
    function Close: WordBool; dispid 4;
    function Clear(CanvasColor: OLE_COLOR; const cBack: ICallback): WordBool; dispid 5;
    function GetRow(Row: Integer; var Vals: Integer): WordBool; dispid 6;
    property Width: Integer readonly dispid 7;
    property Height: Integer readonly dispid 8;
    property YllCenter: Double dispid 9;
    property XllCenter: Double dispid 10;
    property dY: Double dispid 11;
    property dX: Double dispid 12;
    property Value[Row: Integer; col: Integer]: Integer dispid 13;
    property IsInRam: WordBool readonly dispid 14;
    property TransparencyColor: OLE_COLOR dispid 15;
    property UseTransparencyColor: WordBool dispid 16;
    property LastErrorCode: Integer readonly dispid 17;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 18;
    property CdlgFilter: WideString readonly dispid 19;
    property GlobalCallback: ICallback dispid 20;
    property Key: WideString dispid 21;
    property FileHandle: Integer readonly dispid 22;
    property ImageType: ImageType readonly dispid 23;
    property Picture: IPictureDisp dispid 24;
    property Filename: WideString readonly dispid 25;
    function GetImageBitsDC(hDC: Integer): WordBool; dispid 26;
    function SetImageBitsDC(hDC: Integer): WordBool; dispid 27;
    procedure SetVisibleExtents(newMinX: Double; newMinY: Double; newMaxX: Double; newMaxY: Double; 
                                newPixelsInView: Integer; transPercent: Single); dispid 28;
    function SetProjection(const Proj4: WideString): WordBool; dispid 29;
    function GetProjection: WideString; dispid 30;
    procedure get_OriginalWidth(var OriginalWidth: Integer); dispid 31;
    procedure get_OriginalHeight(var OriginalHeight: Integer); dispid 32;
    function Resource(const newImgPath: WideString): WordBool; dispid 33;
  end;

// *********************************************************************//
// Interface: IShapefile
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {5DC72405-C39C-4755-8CFC-9876A89225BC}
// *********************************************************************//
  IShapefile = interface(IDispatch)
    ['{5DC72405-C39C-4755-8CFC-9876A89225BC}']
    function Get_NumShapes: Integer; safecall;
    function Get_NumFields: Integer; safecall;
    function Get_Extents: IExtents; safecall;
    function Get_ShapefileType: ShpfileType; safecall;
    function Get_Shape(ShapeIndex: Integer): IShape; safecall;
    function Get_EditingShapes: WordBool; safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_CdlgFilter: WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    function Open(const ShapefileName: WideString; const cBack: ICallback): WordBool; safecall;
    function CreateNew(const ShapefileName: WideString; ShapefileType: ShpfileType): WordBool; safecall;
    function SaveAs(const ShapefileName: WideString; const cBack: ICallback): WordBool; safecall;
    function Close: WordBool; safecall;
    function EditClear: WordBool; safecall;
    function EditInsertShape(const Shape: IShape; var ShapeIndex: Integer): WordBool; safecall;
    function EditDeleteShape(ShapeIndex: Integer): WordBool; safecall;
    function SelectShapes(const BoundBox: IExtents; Tolerance: Double; SelectMode: SelectMode; 
                          var result: OleVariant): WordBool; safecall;
    function StartEditingShapes(StartEditTable: WordBool; const cBack: ICallback): WordBool; safecall;
    function StopEditingShapes(ApplyChanges: WordBool; StopEditTable: WordBool; 
                               const cBack: ICallback): WordBool; safecall;
    function EditInsertField(const NewField: IField; var FieldIndex: Integer; const cBack: ICallback): WordBool; safecall;
    function EditDeleteField(FieldIndex: Integer; const cBack: ICallback): WordBool; safecall;
    function EditCellValue(FieldIndex: Integer; ShapeIndex: Integer; NewVal: OleVariant): WordBool; safecall;
    function StartEditingTable(const cBack: ICallback): WordBool; safecall;
    function StopEditingTable(ApplyChanges: WordBool; const cBack: ICallback): WordBool; safecall;
    function Get_Field(FieldIndex: Integer): IField; safecall;
    function Get_CellValue(FieldIndex: Integer; ShapeIndex: Integer): OleVariant; safecall;
    function Get_EditingTable: WordBool; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_FileHandle: Integer; safecall;
    function Get_Filename: WideString; safecall;
    function QuickPoint(ShapeIndex: Integer; PointIndex: Integer): IPoint; safecall;
    function QuickExtents(ShapeIndex: Integer): IExtents; safecall;
    function QuickPoints(ShapeIndex: Integer; var numPoints: Integer): PSafeArray; safecall;
    function PointInShape(ShapeIndex: Integer; x: Double; y: Double): WordBool; safecall;
    function PointInShapefile(x: Double; y: Double): Integer; safecall;
    function BeginPointInShapefile: WordBool; safecall;
    procedure EndPointInShapefile; safecall;
    function Get_Projection: WideString; safecall;
    procedure Set_Projection(const pVal: WideString); safecall;
    function Get_FieldByName(const Fieldname: WideString): IField; safecall;
    function Get_numPoints(ShapeIndex: Integer): Integer; safecall;
    function CreateNewWithShapeID(const ShapefileName: WideString; ShapefileType: ShpfileType): WordBool; safecall;
    function Get_UseSpatialIndex: WordBool; safecall;
    procedure Set_UseSpatialIndex(pVal: WordBool); safecall;
    function CreateSpatialIndex(const ShapefileName: WideString): WordBool; safecall;
    function Get_HasSpatialIndex: WordBool; safecall;
    procedure Set_HasSpatialIndex(pVal: WordBool); safecall;
    function Resource(const newShpPath: WideString): WordBool; safecall;
    property NumShapes: Integer read Get_NumShapes;
    property NumFields: Integer read Get_NumFields;
    property Extents: IExtents read Get_Extents;
    property ShapefileType: ShpfileType read Get_ShapefileType;
    property Shape[ShapeIndex: Integer]: IShape read Get_Shape;
    property EditingShapes: WordBool read Get_EditingShapes;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property CdlgFilter: WideString read Get_CdlgFilter;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
    property Field[FieldIndex: Integer]: IField read Get_Field;
    property CellValue[FieldIndex: Integer; ShapeIndex: Integer]: OleVariant read Get_CellValue;
    property EditingTable: WordBool read Get_EditingTable;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property FileHandle: Integer read Get_FileHandle;
    property Filename: WideString read Get_Filename;
    property Projection: WideString read Get_Projection write Set_Projection;
    property FieldByName[const Fieldname: WideString]: IField read Get_FieldByName;
    property numPoints[ShapeIndex: Integer]: Integer read Get_numPoints;
    property UseSpatialIndex: WordBool read Get_UseSpatialIndex write Set_UseSpatialIndex;
    property HasSpatialIndex: WordBool read Get_HasSpatialIndex write Set_HasSpatialIndex;
  end;

// *********************************************************************//
// DispIntf:  IShapefileDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {5DC72405-C39C-4755-8CFC-9876A89225BC}
// *********************************************************************//
  IShapefileDisp = dispinterface
    ['{5DC72405-C39C-4755-8CFC-9876A89225BC}']
    property NumShapes: Integer readonly dispid 1;
    property NumFields: Integer readonly dispid 2;
    property Extents: IExtents readonly dispid 3;
    property ShapefileType: ShpfileType readonly dispid 4;
    property Shape[ShapeIndex: Integer]: IShape readonly dispid 5;
    property EditingShapes: WordBool readonly dispid 6;
    property LastErrorCode: Integer readonly dispid 7;
    property CdlgFilter: WideString readonly dispid 8;
    property GlobalCallback: ICallback dispid 9;
    property Key: WideString dispid 10;
    function Open(const ShapefileName: WideString; const cBack: ICallback): WordBool; dispid 11;
    function CreateNew(const ShapefileName: WideString; ShapefileType: ShpfileType): WordBool; dispid 12;
    function SaveAs(const ShapefileName: WideString; const cBack: ICallback): WordBool; dispid 13;
    function Close: WordBool; dispid 14;
    function EditClear: WordBool; dispid 15;
    function EditInsertShape(const Shape: IShape; var ShapeIndex: Integer): WordBool; dispid 16;
    function EditDeleteShape(ShapeIndex: Integer): WordBool; dispid 17;
    function SelectShapes(const BoundBox: IExtents; Tolerance: Double; SelectMode: SelectMode; 
                          var result: OleVariant): WordBool; dispid 18;
    function StartEditingShapes(StartEditTable: WordBool; const cBack: ICallback): WordBool; dispid 19;
    function StopEditingShapes(ApplyChanges: WordBool; StopEditTable: WordBool; 
                               const cBack: ICallback): WordBool; dispid 20;
    function EditInsertField(const NewField: IField; var FieldIndex: Integer; const cBack: ICallback): WordBool; dispid 22;
    function EditDeleteField(FieldIndex: Integer; const cBack: ICallback): WordBool; dispid 23;
    function EditCellValue(FieldIndex: Integer; ShapeIndex: Integer; NewVal: OleVariant): WordBool; dispid 24;
    function StartEditingTable(const cBack: ICallback): WordBool; dispid 25;
    function StopEditingTable(ApplyChanges: WordBool; const cBack: ICallback): WordBool; dispid 26;
    property Field[FieldIndex: Integer]: IField readonly dispid 27;
    property CellValue[FieldIndex: Integer; ShapeIndex: Integer]: OleVariant readonly dispid 28;
    property EditingTable: WordBool readonly dispid 29;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 21;
    property FileHandle: Integer readonly dispid 30;
    property Filename: WideString readonly dispid 31;
    function QuickPoint(ShapeIndex: Integer; PointIndex: Integer): IPoint; dispid 32;
    function QuickExtents(ShapeIndex: Integer): IExtents; dispid 33;
    function QuickPoints(ShapeIndex: Integer; var numPoints: Integer): {??PSafeArray}OleVariant; dispid 34;
    function PointInShape(ShapeIndex: Integer; x: Double; y: Double): WordBool; dispid 35;
    function PointInShapefile(x: Double; y: Double): Integer; dispid 36;
    function BeginPointInShapefile: WordBool; dispid 37;
    procedure EndPointInShapefile; dispid 38;
    property Projection: WideString dispid 39;
    property FieldByName[const Fieldname: WideString]: IField readonly dispid 40;
    property numPoints[ShapeIndex: Integer]: Integer readonly dispid 41;
    function CreateNewWithShapeID(const ShapefileName: WideString; ShapefileType: ShpfileType): WordBool; dispid 42;
    property UseSpatialIndex: WordBool dispid 43;
    function CreateSpatialIndex(const ShapefileName: WideString): WordBool; dispid 44;
    property HasSpatialIndex: WordBool dispid 45;
    function Resource(const newShpPath: WideString): WordBool; dispid 46;
  end;

// *********************************************************************//
// Interface: IExtents
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {A5692259-035E-487A-8D89-509DD6DD0F64}
// *********************************************************************//
  IExtents = interface(IDispatch)
    ['{A5692259-035E-487A-8D89-509DD6DD0F64}']
    procedure SetBounds(xMin: Double; yMin: Double; zMin: Double; xMax: Double; yMax: Double; 
                        zMax: Double); safecall;
    procedure GetBounds(out xMin: Double; out yMin: Double; out zMin: Double; out xMax: Double; 
                        out yMax: Double; out zMax: Double); safecall;
    function Get_xMin: Double; safecall;
    function Get_xMax: Double; safecall;
    function Get_yMin: Double; safecall;
    function Get_yMax: Double; safecall;
    function Get_zMin: Double; safecall;
    function Get_zMax: Double; safecall;
    function Get_mMin: Double; safecall;
    function Get_mMax: Double; safecall;
    procedure GetMeasureBounds(out mMin: Double; out mMax: Double); safecall;
    procedure SetMeasureBounds(mMin: Double; mMax: Double); safecall;
    property xMin: Double read Get_xMin;
    property xMax: Double read Get_xMax;
    property yMin: Double read Get_yMin;
    property yMax: Double read Get_yMax;
    property zMin: Double read Get_zMin;
    property zMax: Double read Get_zMax;
    property mMin: Double read Get_mMin;
    property mMax: Double read Get_mMax;
  end;

// *********************************************************************//
// DispIntf:  IExtentsDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {A5692259-035E-487A-8D89-509DD6DD0F64}
// *********************************************************************//
  IExtentsDisp = dispinterface
    ['{A5692259-035E-487A-8D89-509DD6DD0F64}']
    procedure SetBounds(xMin: Double; yMin: Double; zMin: Double; xMax: Double; yMax: Double; 
                        zMax: Double); dispid 1;
    procedure GetBounds(out xMin: Double; out yMin: Double; out zMin: Double; out xMax: Double; 
                        out yMax: Double; out zMax: Double); dispid 2;
    property xMin: Double readonly dispid 3;
    property xMax: Double readonly dispid 4;
    property yMin: Double readonly dispid 5;
    property yMax: Double readonly dispid 6;
    property zMin: Double readonly dispid 7;
    property zMax: Double readonly dispid 8;
    property mMin: Double readonly dispid 9;
    property mMax: Double readonly dispid 10;
    procedure GetMeasureBounds(out mMin: Double; out mMax: Double); dispid 11;
    procedure SetMeasureBounds(mMin: Double; mMax: Double); dispid 12;
  end;

// *********************************************************************//
// Interface: IShape
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {5FA550E3-2044-4034-BAAA-B4CCE90A0C41}
// *********************************************************************//
  IShape = interface(IDispatch)
    ['{5FA550E3-2044-4034-BAAA-B4CCE90A0C41}']
    function Get_numPoints: Integer; safecall;
    function Get_NumParts: Integer; safecall;
    function Get_ShapeType: ShpfileType; safecall;
    procedure Set_ShapeType(pVal: ShpfileType); safecall;
    function Get_Point(PointIndex: Integer): IPoint; safecall;
    procedure Set_Point(PointIndex: Integer; const pVal: IPoint); safecall;
    function Get_Part(PartIndex: Integer): Integer; safecall;
    procedure Set_Part(PartIndex: Integer; pVal: Integer); safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    function Create(ShpType: ShpfileType): WordBool; safecall;
    function InsertPoint(const NewPoint: IPoint; var PointIndex: Integer): WordBool; safecall;
    function DeletePoint(PointIndex: Integer): WordBool; safecall;
    function InsertPart(PointIndex: Integer; var PartIndex: Integer): WordBool; safecall;
    function DeletePart(PartIndex: Integer): WordBool; safecall;
    function Get_Extents: IExtents; safecall;
    function SerializeToString: WideString; safecall;
    function CreateFromString(const Serialized: WideString): WordBool; safecall;
    function PointInThisPoly(const pt: IPoint): WordBool; safecall;
    property numPoints: Integer read Get_numPoints;
    property NumParts: Integer read Get_NumParts;
    property ShapeType: ShpfileType read Get_ShapeType write Set_ShapeType;
    property Point[PointIndex: Integer]: IPoint read Get_Point write Set_Point;
    property Part[PartIndex: Integer]: Integer read Get_Part write Set_Part;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
    property Extents: IExtents read Get_Extents;
  end;

// *********************************************************************//
// DispIntf:  IShapeDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {5FA550E3-2044-4034-BAAA-B4CCE90A0C41}
// *********************************************************************//
  IShapeDisp = dispinterface
    ['{5FA550E3-2044-4034-BAAA-B4CCE90A0C41}']
    property numPoints: Integer readonly dispid 1;
    property NumParts: Integer readonly dispid 2;
    property ShapeType: ShpfileType dispid 3;
    property Point[PointIndex: Integer]: IPoint dispid 4;
    property Part[PartIndex: Integer]: Integer dispid 5;
    property LastErrorCode: Integer readonly dispid 6;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 7;
    property GlobalCallback: ICallback dispid 8;
    property Key: WideString dispid 9;
    function Create(ShpType: ShpfileType): WordBool; dispid 10;
    function InsertPoint(const NewPoint: IPoint; var PointIndex: Integer): WordBool; dispid 11;
    function DeletePoint(PointIndex: Integer): WordBool; dispid 12;
    function InsertPart(PointIndex: Integer; var PartIndex: Integer): WordBool; dispid 13;
    function DeletePart(PartIndex: Integer): WordBool; dispid 14;
    property Extents: IExtents readonly dispid 15;
    function SerializeToString: WideString; dispid 16;
    function CreateFromString(const Serialized: WideString): WordBool; dispid 17;
    function PointInThisPoly(const pt: IPoint): WordBool; dispid 18;
  end;

// *********************************************************************//
// Interface: IPoint
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {74F07889-1380-43EE-837A-BBB268311005}
// *********************************************************************//
  IPoint = interface(IDispatch)
    ['{74F07889-1380-43EE-837A-BBB268311005}']
    function Get_x: Double; safecall;
    procedure Set_x(pVal: Double); safecall;
    function Get_y: Double; safecall;
    procedure Set_y(pVal: Double); safecall;
    function Get_Z: Double; safecall;
    procedure Set_Z(pVal: Double); safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    function Get_M: Double; safecall;
    procedure Set_M(pVal: Double); safecall;
    property x: Double read Get_x write Set_x;
    property y: Double read Get_y write Set_y;
    property Z: Double read Get_Z write Set_Z;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
    property M: Double read Get_M write Set_M;
  end;

// *********************************************************************//
// DispIntf:  IPointDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {74F07889-1380-43EE-837A-BBB268311005}
// *********************************************************************//
  IPointDisp = dispinterface
    ['{74F07889-1380-43EE-837A-BBB268311005}']
    property x: Double dispid 1;
    property y: Double dispid 2;
    property Z: Double dispid 3;
    property LastErrorCode: Integer readonly dispid 4;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 5;
    property GlobalCallback: ICallback dispid 6;
    property Key: WideString dispid 7;
    property M: Double dispid 8;
  end;

// *********************************************************************//
// Interface: IField
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {3F3751A5-4CF8-4AC3-AFC2-60DE8B90FC7B}
// *********************************************************************//
  IField = interface(IDispatch)
    ['{3F3751A5-4CF8-4AC3-AFC2-60DE8B90FC7B}']
    function Get_Name: WideString; safecall;
    procedure Set_Name(const pVal: WideString); safecall;
    function Get_Width: Integer; safecall;
    procedure Set_Width(pVal: Integer); safecall;
    function Get_Precision: Integer; safecall;
    procedure Set_Precision(pVal: Integer); safecall;
    function Get_type_: FieldType; safecall;
    procedure Set_type_(pVal: FieldType); safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    property Name: WideString read Get_Name write Set_Name;
    property Width: Integer read Get_Width write Set_Width;
    property Precision: Integer read Get_Precision write Set_Precision;
    property type_: FieldType read Get_type_ write Set_type_;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
  end;

// *********************************************************************//
// DispIntf:  IFieldDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {3F3751A5-4CF8-4AC3-AFC2-60DE8B90FC7B}
// *********************************************************************//
  IFieldDisp = dispinterface
    ['{3F3751A5-4CF8-4AC3-AFC2-60DE8B90FC7B}']
    property Name: WideString dispid 1;
    property Width: Integer dispid 2;
    property Precision: Integer dispid 3;
    property type_: FieldType dispid 4;
    property LastErrorCode: Integer readonly dispid 5;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 6;
    property GlobalCallback: ICallback dispid 7;
    property Key: WideString dispid 8;
  end;

// *********************************************************************//
// Interface: ITable
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {4365A8A1-2E46-4223-B2DC-65764262D88B}
// *********************************************************************//
  ITable = interface(IDispatch)
    ['{4365A8A1-2E46-4223-B2DC-65764262D88B}']
    function Get_NumRows: Integer; safecall;
    function Get_NumFields: Integer; safecall;
    function Get_Field(FieldIndex: Integer): IField; safecall;
    function Get_CellValue(FieldIndex: Integer; RowIndex: Integer): OleVariant; safecall;
    function Get_EditingTable: WordBool; safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_CdlgFilter: WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    function Open(const dbfFilename: WideString; const cBack: ICallback): WordBool; safecall;
    function CreateNew(const dbfFilename: WideString): WordBool; safecall;
    function SaveAs(const dbfFilename: WideString; const cBack: ICallback): WordBool; safecall;
    function Close: WordBool; safecall;
    function EditClear: WordBool; safecall;
    function EditInsertField(const Field: IField; var FieldIndex: Integer; const cBack: ICallback): WordBool; safecall;
    function EditReplaceField(FieldIndex: Integer; const NewField: IField; const cBack: ICallback): WordBool; safecall;
    function EditDeleteField(FieldIndex: Integer; const cBack: ICallback): WordBool; safecall;
    function EditInsertRow(var RowIndex: Integer): WordBool; safecall;
    function EditCellValue(FieldIndex: Integer; RowIndex: Integer; NewVal: OleVariant): WordBool; safecall;
    function StartEditingTable(const cBack: ICallback): WordBool; safecall;
    function StopEditingTable(ApplyChanges: WordBool; const cBack: ICallback): WordBool; safecall;
    function EditDeleteRow(RowIndex: Integer): WordBool; safecall;
    property NumRows: Integer read Get_NumRows;
    property NumFields: Integer read Get_NumFields;
    property Field[FieldIndex: Integer]: IField read Get_Field;
    property CellValue[FieldIndex: Integer; RowIndex: Integer]: OleVariant read Get_CellValue;
    property EditingTable: WordBool read Get_EditingTable;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property CdlgFilter: WideString read Get_CdlgFilter;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
  end;

// *********************************************************************//
// DispIntf:  ITableDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {4365A8A1-2E46-4223-B2DC-65764262D88B}
// *********************************************************************//
  ITableDisp = dispinterface
    ['{4365A8A1-2E46-4223-B2DC-65764262D88B}']
    property NumRows: Integer readonly dispid 1;
    property NumFields: Integer readonly dispid 2;
    property Field[FieldIndex: Integer]: IField readonly dispid 3;
    property CellValue[FieldIndex: Integer; RowIndex: Integer]: OleVariant readonly dispid 4;
    property EditingTable: WordBool readonly dispid 5;
    property LastErrorCode: Integer readonly dispid 6;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 7;
    property CdlgFilter: WideString readonly dispid 8;
    property GlobalCallback: ICallback dispid 9;
    property Key: WideString dispid 10;
    function Open(const dbfFilename: WideString; const cBack: ICallback): WordBool; dispid 11;
    function CreateNew(const dbfFilename: WideString): WordBool; dispid 12;
    function SaveAs(const dbfFilename: WideString; const cBack: ICallback): WordBool; dispid 13;
    function Close: WordBool; dispid 14;
    function EditClear: WordBool; dispid 15;
    function EditInsertField(const Field: IField; var FieldIndex: Integer; const cBack: ICallback): WordBool; dispid 16;
    function EditReplaceField(FieldIndex: Integer; const NewField: IField; const cBack: ICallback): WordBool; dispid 17;
    function EditDeleteField(FieldIndex: Integer; const cBack: ICallback): WordBool; dispid 18;
    function EditInsertRow(var RowIndex: Integer): WordBool; dispid 19;
    function EditCellValue(FieldIndex: Integer; RowIndex: Integer; NewVal: OleVariant): WordBool; dispid 20;
    function StartEditingTable(const cBack: ICallback): WordBool; dispid 21;
    function StopEditingTable(ApplyChanges: WordBool; const cBack: ICallback): WordBool; dispid 22;
    function EditDeleteRow(RowIndex: Integer): WordBool; dispid 23;
  end;

// *********************************************************************//
// Interface: IShapeNetwork
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {2D4968F2-40D9-4F25-8BE6-B51B959CC1B0}
// *********************************************************************//
  IShapeNetwork = interface(IDispatch)
    ['{2D4968F2-40D9-4F25-8BE6-B51B959CC1B0}']
    function Build(const Shapefile: IShapefile; ShapeIndex: Integer; FinalPointIndex: Integer; 
                   Tolerance: Double; ar: AmbiguityResolution; const cBack: ICallback): Integer; safecall;
    function DeleteShape(ShapeIndex: Integer): WordBool; safecall;
    function RasterizeD8(UseNetworkBounds: WordBool; const Header: IGridHeader; Cellsize: Double; 
                         const cBack: ICallback): IGrid; safecall;
    function MoveUp(UpIndex: Integer): WordBool; safecall;
    function MoveDown: WordBool; safecall;
    function MoveTo(ShapeIndex: Integer): WordBool; safecall;
    function MoveToOutlet: WordBool; safecall;
    function Get_Shapefile: IShapefile; safecall;
    function Get_CurrentShape: IShape; safecall;
    function Get_CurrentShapeIndex: Integer; safecall;
    function Get_DistanceToOutlet(PointIndex: Integer): Double; safecall;
    function Get_NumDirectUps: Integer; safecall;
    function Get_NetworkSize: Integer; safecall;
    function Get_AmbigShapeIndex(Index: Integer): Integer; safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    function Get_ParentIndex: Integer; safecall;
    procedure Set_ParentIndex(pVal: Integer); safecall;
    function Open(const sf: IShapefile; const cBack: ICallback): WordBool; safecall;
    function Close: WordBool; safecall;
    property Shapefile: IShapefile read Get_Shapefile;
    property CurrentShape: IShape read Get_CurrentShape;
    property CurrentShapeIndex: Integer read Get_CurrentShapeIndex;
    property DistanceToOutlet[PointIndex: Integer]: Double read Get_DistanceToOutlet;
    property NumDirectUps: Integer read Get_NumDirectUps;
    property NetworkSize: Integer read Get_NetworkSize;
    property AmbigShapeIndex[Index: Integer]: Integer read Get_AmbigShapeIndex;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
    property ParentIndex: Integer read Get_ParentIndex write Set_ParentIndex;
  end;

// *********************************************************************//
// DispIntf:  IShapeNetworkDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {2D4968F2-40D9-4F25-8BE6-B51B959CC1B0}
// *********************************************************************//
  IShapeNetworkDisp = dispinterface
    ['{2D4968F2-40D9-4F25-8BE6-B51B959CC1B0}']
    function Build(const Shapefile: IShapefile; ShapeIndex: Integer; FinalPointIndex: Integer; 
                   Tolerance: Double; ar: AmbiguityResolution; const cBack: ICallback): Integer; dispid 1;
    function DeleteShape(ShapeIndex: Integer): WordBool; dispid 2;
    function RasterizeD8(UseNetworkBounds: WordBool; const Header: IGridHeader; Cellsize: Double; 
                         const cBack: ICallback): IGrid; dispid 3;
    function MoveUp(UpIndex: Integer): WordBool; dispid 4;
    function MoveDown: WordBool; dispid 5;
    function MoveTo(ShapeIndex: Integer): WordBool; dispid 6;
    function MoveToOutlet: WordBool; dispid 7;
    property Shapefile: IShapefile readonly dispid 8;
    property CurrentShape: IShape readonly dispid 9;
    property CurrentShapeIndex: Integer readonly dispid 10;
    property DistanceToOutlet[PointIndex: Integer]: Double readonly dispid 11;
    property NumDirectUps: Integer readonly dispid 12;
    property NetworkSize: Integer readonly dispid 13;
    property AmbigShapeIndex[Index: Integer]: Integer readonly dispid 14;
    property LastErrorCode: Integer readonly dispid 15;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 16;
    property GlobalCallback: ICallback dispid 17;
    property Key: WideString dispid 18;
    property ParentIndex: Integer dispid 19;
    function Open(const sf: IShapefile; const cBack: ICallback): WordBool; dispid 20;
    function Close: WordBool; dispid 21;
  end;

// *********************************************************************//
// Interface: IUtils
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {360BEC33-7703-4693-B6CA-90FEA22CF1B7}
// *********************************************************************//
  IUtils = interface(IDispatch)
    ['{360BEC33-7703-4693-B6CA-90FEA22CF1B7}']
    function PointInPolygon(const Shp: IShape; const TestPoint: IPoint): WordBool; safecall;
    function GridReplace(const Grd: IGrid; OldValue: OleVariant; newValue: OleVariant; 
                         const cBack: ICallback): WordBool; safecall;
    function GridInterpolateNoData(const Grd: IGrid; const cBack: ICallback): WordBool; safecall;
    function RemoveColinearPoints(const Shapes: IShapefile; LinearTolerance: Double; 
                                  const cBack: ICallback): WordBool; safecall;
    function Get_Length(const Shape: IShape): Double; safecall;
    function Get_Perimeter(const Shape: IShape): Double; safecall;
    function Get_Area(const Shape: IShape): Double; safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    function ClipPolygon(op: PolygonOperation; const SubjectPolygon: IShape; 
                         const ClipPolygon: IShape): IShape; safecall;
    function GridMerge(Grids: OleVariant; const MergeFilename: WideString; InRam: WordBool; 
                       GrdFileType: GridFileType; const cBack: ICallback): IGrid; safecall;
    function ShapeMerge(const Shapes: IShapefile; IndexOne: Integer; IndexTwo: Integer; 
                        const cBack: ICallback): IShape; safecall;
    function GridToImage(const Grid: IGrid; const cScheme: IGridColorScheme; const cBack: ICallback): IImage; safecall;
    function GridToShapefile(const Grid: IGrid; const ConnectionGrid: IGrid; const cBack: ICallback): IShapefile; safecall;
    function GridToGrid(const Grid: IGrid; OutDataType: GridDataType; const cBack: ICallback): IGrid; safecall;
    function ShapeToShapeZ(const Shapefile: IShapefile; const Grid: IGrid; const cBack: ICallback): IShapefile; safecall;
    function TinToShapefile(const Tin: ITin; Type_: ShpfileType; const cBack: ICallback): IShapefile; safecall;
    function ShapefileToGrid(const Shpfile: IShapefile; UseShapefileBounds: WordBool; 
                             const GrdHeader: IGridHeader; Cellsize: Double; 
                             UseShapeNumber: WordBool; SingleValue: Smallint): IGrid; safecall;
    function hBitmapToPicture(hBitmap: Integer): IPictureDisp; safecall;
    function GenerateHillShade(const bstrGridFilename: WideString; 
                               const bstrShadeFilename: WideString; Z: Single; scale: Single; 
                               az: Single; alt: Single): WordBool; safecall;
    function GenerateContour(const pszSrcFilename: WideString; const pszDstFilename: WideString; 
                             dfInterval: Double; dfNoData: Double; Is3D: WordBool; 
                             dblFLArray: OleVariant; const cBack: ICallback): WordBool; safecall;
    function TranslateRaster(const bstrSrcFilename: WideString; const bstrDstFilename: WideString; 
                             const bstrOptions: WideString; const cBack: ICallback): WordBool; safecall;
    property Length[const Shape: IShape]: Double read Get_Length;
    property Perimeter[const Shape: IShape]: Double read Get_Perimeter;
    property Area[const Shape: IShape]: Double read Get_Area;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
  end;

// *********************************************************************//
// DispIntf:  IUtilsDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {360BEC33-7703-4693-B6CA-90FEA22CF1B7}
// *********************************************************************//
  IUtilsDisp = dispinterface
    ['{360BEC33-7703-4693-B6CA-90FEA22CF1B7}']
    function PointInPolygon(const Shp: IShape; const TestPoint: IPoint): WordBool; dispid 1;
    function GridReplace(const Grd: IGrid; OldValue: OleVariant; newValue: OleVariant; 
                         const cBack: ICallback): WordBool; dispid 2;
    function GridInterpolateNoData(const Grd: IGrid; const cBack: ICallback): WordBool; dispid 3;
    function RemoveColinearPoints(const Shapes: IShapefile; LinearTolerance: Double; 
                                  const cBack: ICallback): WordBool; dispid 4;
    property Length[const Shape: IShape]: Double readonly dispid 5;
    property Perimeter[const Shape: IShape]: Double readonly dispid 6;
    property Area[const Shape: IShape]: Double readonly dispid 7;
    property LastErrorCode: Integer readonly dispid 8;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 9;
    property GlobalCallback: ICallback dispid 10;
    property Key: WideString dispid 11;
    function ClipPolygon(op: PolygonOperation; const SubjectPolygon: IShape; 
                         const ClipPolygon: IShape): IShape; dispid 12;
    function GridMerge(Grids: OleVariant; const MergeFilename: WideString; InRam: WordBool; 
                       GrdFileType: GridFileType; const cBack: ICallback): IGrid; dispid 13;
    function ShapeMerge(const Shapes: IShapefile; IndexOne: Integer; IndexTwo: Integer; 
                        const cBack: ICallback): IShape; dispid 14;
    function GridToImage(const Grid: IGrid; const cScheme: IGridColorScheme; const cBack: ICallback): IImage; dispid 15;
    function GridToShapefile(const Grid: IGrid; const ConnectionGrid: IGrid; const cBack: ICallback): IShapefile; dispid 16;
    function GridToGrid(const Grid: IGrid; OutDataType: GridDataType; const cBack: ICallback): IGrid; dispid 17;
    function ShapeToShapeZ(const Shapefile: IShapefile; const Grid: IGrid; const cBack: ICallback): IShapefile; dispid 18;
    function TinToShapefile(const Tin: ITin; Type_: ShpfileType; const cBack: ICallback): IShapefile; dispid 19;
    function ShapefileToGrid(const Shpfile: IShapefile; UseShapefileBounds: WordBool; 
                             const GrdHeader: IGridHeader; Cellsize: Double; 
                             UseShapeNumber: WordBool; SingleValue: Smallint): IGrid; dispid 20;
    function hBitmapToPicture(hBitmap: Integer): IPictureDisp; dispid 21;
    function GenerateHillShade(const bstrGridFilename: WideString; 
                               const bstrShadeFilename: WideString; Z: Single; scale: Single; 
                               az: Single; alt: Single): WordBool; dispid 22;
    function GenerateContour(const pszSrcFilename: WideString; const pszDstFilename: WideString; 
                             dfInterval: Double; dfNoData: Double; Is3D: WordBool; 
                             dblFLArray: OleVariant; const cBack: ICallback): WordBool; dispid 23;
    function TranslateRaster(const bstrSrcFilename: WideString; const bstrDstFilename: WideString; 
                             const bstrOptions: WideString; const cBack: ICallback): WordBool; dispid 24;
  end;

// *********************************************************************//
// Interface: ITin
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {55DD824E-332E-41CA-B40C-C8DC81EE209C}
// *********************************************************************//
  ITin = interface(IDispatch)
    ['{55DD824E-332E-41CA-B40C-C8DC81EE209C}']
    function Open(const TinFile: WideString; const cBack: ICallback): WordBool; safecall;
    function CreateNew(const Grid: IGrid; Deviation: Double; SplitTest: SplitMethod; 
                       STParam: Double; MeshDivisions: Integer; MaximumTriangles: Integer; 
                       const cBack: ICallback): WordBool; safecall;
    function Save(const TinFilename: WideString; const cBack: ICallback): WordBool; safecall;
    function Close: WordBool; safecall;
    function Select(var TriangleHint: Integer; x: Double; y: Double; out Z: Double): WordBool; safecall;
    function Get_NumTriangles: Integer; safecall;
    function Get_NumVertices: Integer; safecall;
    function Get_LastErrorCode: Integer; safecall;
    function Get_ErrorMsg(ErrorCode: Integer): WideString; safecall;
    function Get_CdlgFilter: WideString; safecall;
    function Get_GlobalCallback: ICallback; safecall;
    procedure Set_GlobalCallback(const pVal: ICallback); safecall;
    function Get_Key: WideString; safecall;
    procedure Set_Key(const pVal: WideString); safecall;
    procedure Triangle(TriIndex: Integer; out vtx1Index: Integer; out vtx2Index: Integer; 
                       out vtx3Index: Integer); safecall;
    procedure Vertex(VtxIndex: Integer; out x: Double; out y: Double; out Z: Double); safecall;
    procedure Max(out x: Double; out y: Double; out Z: Double); safecall;
    procedure Min(out x: Double; out y: Double; out Z: Double); safecall;
    function Get_Filename: WideString; safecall;
    function Get_IsNDTriangle(TriIndex: Integer): WordBool; safecall;
    procedure TriangleNeighbors(TriIndex: Integer; var triIndex1: Integer; var triIndex2: Integer; 
                                var triIndex3: Integer); safecall;
    function CreateTinFromPoints(Points: PSafeArray): WordBool; safecall;
    property NumTriangles: Integer read Get_NumTriangles;
    property NumVertices: Integer read Get_NumVertices;
    property LastErrorCode: Integer read Get_LastErrorCode;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property CdlgFilter: WideString read Get_CdlgFilter;
    property GlobalCallback: ICallback read Get_GlobalCallback write Set_GlobalCallback;
    property Key: WideString read Get_Key write Set_Key;
    property Filename: WideString read Get_Filename;
    property IsNDTriangle[TriIndex: Integer]: WordBool read Get_IsNDTriangle;
  end;

// *********************************************************************//
// DispIntf:  ITinDisp
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {55DD824E-332E-41CA-B40C-C8DC81EE209C}
// *********************************************************************//
  ITinDisp = dispinterface
    ['{55DD824E-332E-41CA-B40C-C8DC81EE209C}']
    function Open(const TinFile: WideString; const cBack: ICallback): WordBool; dispid 1;
    function CreateNew(const Grid: IGrid; Deviation: Double; SplitTest: SplitMethod; 
                       STParam: Double; MeshDivisions: Integer; MaximumTriangles: Integer; 
                       const cBack: ICallback): WordBool; dispid 2;
    function Save(const TinFilename: WideString; const cBack: ICallback): WordBool; dispid 3;
    function Close: WordBool; dispid 4;
    function Select(var TriangleHint: Integer; x: Double; y: Double; out Z: Double): WordBool; dispid 5;
    property NumTriangles: Integer readonly dispid 6;
    property NumVertices: Integer readonly dispid 7;
    property LastErrorCode: Integer readonly dispid 8;
    property ErrorMsg[ErrorCode: Integer]: WideString readonly dispid 9;
    property CdlgFilter: WideString readonly dispid 10;
    property GlobalCallback: ICallback dispid 11;
    property Key: WideString dispid 12;
    procedure Triangle(TriIndex: Integer; out vtx1Index: Integer; out vtx2Index: Integer; 
                       out vtx3Index: Integer); dispid 13;
    procedure Vertex(VtxIndex: Integer; out x: Double; out y: Double; out Z: Double); dispid 14;
    procedure Max(out x: Double; out y: Double; out Z: Double); dispid 15;
    procedure Min(out x: Double; out y: Double; out Z: Double); dispid 16;
    property Filename: WideString readonly dispid 17;
    property IsNDTriangle[TriIndex: Integer]: WordBool readonly dispid 18;
    procedure TriangleNeighbors(TriIndex: Integer; var triIndex1: Integer; var triIndex2: Integer; 
                                var triIndex3: Integer); dispid 19;
    function CreateTinFromPoints(Points: {??PSafeArray}OleVariant): WordBool; dispid 20;
  end;


// *********************************************************************//
// OLE Control Proxy class declaration
// Control Name     : TMap
// Help String      : Map Control
// Default Interface: _DMap
// Def. Intf. DISP? : Yes
// Event   Interface: _DMapEvents
// TypeFlags        : (34) CanCreate Control
// *********************************************************************//
  TMapMouseDown = procedure(ASender: TObject; Button: Smallint; Shift: Smallint; x: Integer; 
                                              y: Integer) of object;
  TMapMouseUp = procedure(ASender: TObject; Button: Smallint; Shift: Smallint; x: Integer; 
                                            y: Integer) of object;
  TMapMouseMove = procedure(ASender: TObject; Button: Smallint; Shift: Smallint; x: Integer; 
                                              y: Integer) of object;
  TMapFileDropped = procedure(ASender: TObject; const Filename: WideString) of object;
  TMapSelectBoxFinal = procedure(ASender: TObject; Left: Integer; Right: Integer; Bottom: Integer; 
                                                   Top: Integer) of object;
  TMapSelectBoxDrag = procedure(ASender: TObject; Left: Integer; Right: Integer; Bottom: Integer; 
                                                  Top: Integer) of object;
  TMapMapState = procedure(ASender: TObject; LayerHandle: Integer) of object;
  TMapOnDrawBackBuffer = procedure(ASender: TObject; BackBuffer: Integer) of object;

  TMap = class(TOleControl)
  private
    FOnMouseDown: TMapMouseDown;
    FOnMouseUp: TMapMouseUp;
    FOnMouseMove: TMapMouseMove;
    FOnFileDropped: TMapFileDropped;
    FOnSelectBoxFinal: TMapSelectBoxFinal;
    FOnSelectBoxDrag: TMapSelectBoxDrag;
    FOnExtentsChanged: TNotifyEvent;
    FOnMapState: TMapMapState;
    FOnDrawBackBuffer: TMapOnDrawBackBuffer;
    FIntf: _DMap;
    function  GetControlInterface: _DMap;
  protected
    procedure CreateControl;
    procedure InitControlData; override;
    function Get_GlobalCallback: IDispatch;
    procedure Set_GlobalCallback(const Value: IDispatch);
    function Get_Extents: IDispatch;
    procedure Set_Extents(const Value: IDispatch);
    function Get_LayerKey(LayerHandle: Integer): WideString;
    procedure Set_LayerKey(LayerHandle: Integer; const Param2: WideString);
    function Get_LayerPosition(LayerHandle: Integer): Integer;
    function Get_LayerHandle(LayerPosition: Integer): Integer;
    function Get_LayerVisible(LayerHandle: Integer): WordBool;
    procedure Set_LayerVisible(LayerHandle: Integer; Param2: WordBool);
    function Get_ShapeLayerFillColor(LayerHandle: Integer): OLE_COLOR;
    procedure Set_ShapeLayerFillColor(LayerHandle: Integer; Param2: OLE_COLOR);
    function Get_ShapeFillColor(LayerHandle: Integer; Shape: Integer): OLE_COLOR;
    procedure Set_ShapeFillColor(LayerHandle: Integer; Shape: Integer; Param3: OLE_COLOR);
    function Get_ShapeLayerLineColor(LayerHandle: Integer): OLE_COLOR;
    procedure Set_ShapeLayerLineColor(LayerHandle: Integer; Param2: OLE_COLOR);
    function Get_ShapeLineColor(LayerHandle: Integer; Shape: Integer): OLE_COLOR;
    procedure Set_ShapeLineColor(LayerHandle: Integer; Shape: Integer; Param3: OLE_COLOR);
    function Get_ShapeLayerPointColor(LayerHandle: Integer): OLE_COLOR;
    procedure Set_ShapeLayerPointColor(LayerHandle: Integer; Param2: OLE_COLOR);
    function Get_ShapePointColor(LayerHandle: Integer; Shape: Integer): OLE_COLOR;
    procedure Set_ShapePointColor(LayerHandle: Integer; Shape: Integer; Param3: OLE_COLOR);
    function Get_ShapeLayerDrawFill(LayerHandle: Integer): WordBool;
    procedure Set_ShapeLayerDrawFill(LayerHandle: Integer; Param2: WordBool);
    function Get_ShapeDrawFill(LayerHandle: Integer; Shape: Integer): WordBool;
    procedure Set_ShapeDrawFill(LayerHandle: Integer; Shape: Integer; Param3: WordBool);
    function Get_ShapeLayerDrawLine(LayerHandle: Integer): WordBool;
    procedure Set_ShapeLayerDrawLine(LayerHandle: Integer; Param2: WordBool);
    function Get_ShapeDrawLine(LayerHandle: Integer; Shape: Integer): WordBool;
    procedure Set_ShapeDrawLine(LayerHandle: Integer; Shape: Integer; Param3: WordBool);
    function Get_ShapeLayerDrawPoint(LayerHandle: Integer): WordBool;
    procedure Set_ShapeLayerDrawPoint(LayerHandle: Integer; Param2: WordBool);
    function Get_ShapeDrawPoint(LayerHandle: Integer; Shape: Integer): WordBool;
    procedure Set_ShapeDrawPoint(LayerHandle: Integer; Shape: Integer; Param3: WordBool);
    function Get_ShapeLayerLineWidth(LayerHandle: Integer): Single;
    procedure Set_ShapeLayerLineWidth(LayerHandle: Integer; Param2: Single);
    function Get_ShapeLineWidth(LayerHandle: Integer; Shape: Integer): Single;
    procedure Set_ShapeLineWidth(LayerHandle: Integer; Shape: Integer; Param3: Single);
    function Get_ShapeLayerPointSize(LayerHandle: Integer): Single;
    procedure Set_ShapeLayerPointSize(LayerHandle: Integer; Param2: Single);
    function Get_ShapePointSize(LayerHandle: Integer; Shape: Integer): Single;
    procedure Set_ShapePointSize(LayerHandle: Integer; Shape: Integer; Param3: Single);
    function Get_ShapeLayerFillTransparency(LayerHandle: Integer): Single;
    procedure Set_ShapeLayerFillTransparency(LayerHandle: Integer; Param2: Single);
    function Get_ShapeFillTransparency(LayerHandle: Integer; Shape: Integer): Single;
    procedure Set_ShapeFillTransparency(LayerHandle: Integer; Shape: Integer; Param3: Single);
    function Get_ShapeLayerLineStipple(LayerHandle: Integer): tkLineStipple;
    procedure Set_ShapeLayerLineStipple(LayerHandle: Integer; Param2: tkLineStipple);
    function Get_ShapeLineStipple(LayerHandle: Integer; Shape: Integer): tkLineStipple;
    procedure Set_ShapeLineStipple(LayerHandle: Integer; Shape: Integer; Param3: tkLineStipple);
    function Get_ShapeLayerFillStipple(LayerHandle: Integer): tkFillStipple;
    procedure Set_ShapeLayerFillStipple(LayerHandle: Integer; Param2: tkFillStipple);
    function Get_ShapeFillStipple(LayerHandle: Integer; Shape: Integer): tkFillStipple;
    procedure Set_ShapeFillStipple(LayerHandle: Integer; Shape: Integer; Param3: tkFillStipple);
    function Get_ShapeVisible(LayerHandle: Integer; Shape: Integer): WordBool;
    procedure Set_ShapeVisible(LayerHandle: Integer; Shape: Integer; Param3: WordBool);
    function Get_ImageLayerPercentTransparent(LayerHandle: Integer): Single;
    procedure Set_ImageLayerPercentTransparent(LayerHandle: Integer; Param2: Single);
    function Get_ErrorMsg(ErrorCode: Integer): WideString;
    function Get_DrawingKey(DrawHandle: Integer): WideString;
    procedure Set_DrawingKey(DrawHandle: Integer; const Param2: WideString);
    function Get_ShapeLayerPointType(LayerHandle: Integer): tkPointType;
    procedure Set_ShapeLayerPointType(LayerHandle: Integer; Param2: tkPointType);
    function Get_ShapePointType(LayerHandle: Integer; Shape: Integer): tkPointType;
    procedure Set_ShapePointType(LayerHandle: Integer; Shape: Integer; Param3: tkPointType);
    function Get_LayerLabelsVisible(LayerHandle: Integer): WordBool;
    procedure Set_LayerLabelsVisible(LayerHandle: Integer; Param2: WordBool);
    function Get_UDLineStipple(LayerHandle: Integer): Integer;
    procedure Set_UDLineStipple(LayerHandle: Integer; Param2: Integer);
    function Get_UDFillStipple(LayerHandle: Integer; StippleRow: Integer): Integer;
    procedure Set_UDFillStipple(LayerHandle: Integer; StippleRow: Integer; Param3: Integer);
    function Get_UDPointType(LayerHandle: Integer): IDispatch;
    procedure _Set_UDPointType(LayerHandle: Integer; const Param2: IDispatch);
    function Get_GetObject(LayerHandle: Integer): IDispatch;
    function Get_LayerName(LayerHandle: Integer): WideString;
    procedure Set_LayerName(LayerHandle: Integer; const Param2: WideString);
    function Get_GridFileName(LayerHandle: Integer): WideString;
    procedure Set_GridFileName(LayerHandle: Integer; const Param2: WideString);
    function Get_LayerLabelsShadow(LayerHandle: Integer): WordBool;
    procedure Set_LayerLabelsShadow(LayerHandle: Integer; Param2: WordBool);
    function Get_LayerLabelsScale(LayerHandle: Integer): WordBool;
    procedure Set_LayerLabelsScale(LayerHandle: Integer; Param2: WordBool);
    function Get_LayerLabelsOffset(LayerHandle: Integer): Integer;
    procedure Set_LayerLabelsOffset(LayerHandle: Integer; Param2: Integer);
    function Get_LayerLabelsShadowColor(LayerHandle: Integer): OLE_COLOR;
    procedure Set_LayerLabelsShadowColor(LayerHandle: Integer; Param2: OLE_COLOR);
    function Get_UseLabelCollision(LayerHandle: Integer): WordBool;
    procedure Set_UseLabelCollision(LayerHandle: Integer; Param2: WordBool);
    function Get_ShapePointImageListID(LayerHandle: Integer; Shape: Integer): Integer;
    procedure Set_ShapePointImageListID(LayerHandle: Integer; Shape: Integer; Param3: Integer);
    function Get_DrawingLabelsOffset(DrawHandle: Integer): Integer;
    procedure Set_DrawingLabelsOffset(DrawHandle: Integer; Param2: Integer);
    function Get_DrawingLabelsScale(DrawHandle: Integer): WordBool;
    procedure Set_DrawingLabelsScale(DrawHandle: Integer; Param2: WordBool);
    function Get_DrawingLabelsShadow(DrawHandle: Integer): WordBool;
    procedure Set_DrawingLabelsShadow(DrawHandle: Integer; Param2: WordBool);
    function Get_DrawingLabelsShadowColor(DrawHandle: Integer): OLE_COLOR;
    procedure Set_DrawingLabelsShadowColor(DrawHandle: Integer; Param2: OLE_COLOR);
    function Get_UseDrawingLabelCollision(DrawHandle: Integer): WordBool;
    procedure Set_UseDrawingLabelCollision(DrawHandle: Integer; Param2: WordBool);
    function Get_DrawingLabelsVisible(DrawHandle: Integer): WordBool;
    procedure Set_DrawingLabelsVisible(DrawHandle: Integer; Param2: WordBool);
    function Get_ShapePointFontCharListID(LayerHandle: Integer; Shape: Integer): Integer;
    procedure Set_ShapePointFontCharListID(LayerHandle: Integer; Shape: Integer; Param3: Integer);
    function Get_ShapeLayerStippleColor(LayerHandle: Integer): OLE_COLOR;
    procedure Set_ShapeLayerStippleColor(LayerHandle: Integer; Param2: OLE_COLOR);
    function Get_ShapeStippleColor(LayerHandle: Integer; Shape: Integer): OLE_COLOR;
    procedure Set_ShapeStippleColor(LayerHandle: Integer; Shape: Integer; Param3: OLE_COLOR);
    function Get_ShapeStippleTransparent(LayerHandle: Integer; Shape: Integer): WordBool;
    procedure Set_ShapeStippleTransparent(LayerHandle: Integer; Shape: Integer; Param3: WordBool);
    function Get_ShapeLayerStippleTransparent(LayerHandle: Integer): WordBool;
    procedure Set_ShapeLayerStippleTransparent(LayerHandle: Integer; Param2: WordBool);
    function Get_TrapRMouseDown: WordBool;
    procedure Set_TrapRMouseDown(Param1: WordBool);
  public
    procedure Redraw;
    function AddLayer(const Object_: IDispatch; Visible: WordBool): Integer;
    procedure RemoveLayer(LayerHandle: Integer);
    procedure RemoveLayerWithoutClosing(LayerHandle: Integer);
    procedure RemoveAllLayers;
    function MoveLayerUp(InitialPosition: Integer): WordBool;
    function MoveLayerDown(InitialPosition: Integer): WordBool;
    function MoveLayer(InitialPosition: Integer; TargetPosition: Integer): WordBool;
    function MoveLayerTop(InitialPosition: Integer): WordBool;
    function MoveLayerBottom(InitialPosition: Integer): WordBool;
    procedure ZoomToMaxExtents;
    procedure ZoomToLayer(LayerHandle: Integer);
    procedure ZoomToShape(LayerHandle: Integer; Shape: Integer);
    procedure ZoomIn(Percent: Double);
    procedure ZoomOut(Percent: Double);
    function ZoomToPrev: Integer;
    procedure ProjToPixel(projX: Double; projY: Double; var pixelX: Double; var pixelY: Double);
    procedure PixelToProj(pixelX: Double; pixelY: Double; var projX: Double; var projY: Double);
    procedure ClearDrawing(DrawHandle: Integer);
    procedure ClearDrawings;
    function SnapShot(const BoundBox: IDispatch): IDispatch;
    function ApplyLegendColors(const Legend: IDispatch): WordBool;
    procedure LockWindow(LockMode: tkLockMode);
    procedure Resize(Width: Integer; Height: Integer);
    procedure ShowToolTip(const Text: WideString; Milliseconds: Integer);
    procedure AddLabel(LayerHandle: Integer; const Text: WideString; Color: OLE_COLOR; x: Double; 
                       y: Double; hJustification: tkHJustification);
    procedure ClearLabels(LayerHandle: Integer);
    procedure LayerFont(LayerHandle: Integer; const FontName: WideString; FontSize: Integer);
    function GetColorScheme(LayerHandle: Integer): IDispatch;
    function NewDrawing(Projection: tkDrawReferenceList): Integer;
    procedure DrawPoint(x: Double; y: Double; pixelSize: Integer; Color: OLE_COLOR);
    procedure DrawLine(x1: Double; y1: Double; x2: Double; y2: Double; pixelWidth: Integer; 
                       Color: OLE_COLOR);
    procedure DrawCircle(x: Double; y: Double; pixelRadius: Double; Color: OLE_COLOR; fill: WordBool);
    procedure DrawPolygon(var xPoints: OleVariant; var yPoints: OleVariant; numPoints: Integer; 
                          Color: OLE_COLOR; fill: WordBool);
    function SetImageLayerColorScheme(LayerHandle: Integer; const ColorScheme: IDispatch): WordBool;
    procedure UpdateImage(LayerHandle: Integer);
    procedure AddLabelEx(LayerHandle: Integer; const Text: WideString; Color: OLE_COLOR; x: Double; 
                         y: Double; hJustification: tkHJustification; Rotation: Double);
    procedure GetLayerStandardViewWidth(LayerHandle: Integer; var Width: Double);
    procedure SetLayerStandardViewWidth(LayerHandle: Integer; Width: Double);
    function IsTIFFGrid(const Filename: WideString): WordBool;
    function IsSameProjection(const proj4_a: WideString; const proj4_b: WideString): WordBool;
    procedure ZoomToMaxVisibleExtents;
    function HWnd: Integer;
    function set_UDPointImageListAdd(LayerHandle: Integer; const newValue: IDispatch): Integer;
    function get_UDPointImageListCount(LayerHandle: Integer): Integer;
    function get_UDPointImageListItem(LayerHandle: Integer; ImageIndex: Integer): IDispatch;
    procedure ClearUDPointImageList(LayerHandle: Integer);
    procedure DrawLineEx(LayerHandle: Integer; x1: Double; y1: Double; x2: Double; y2: Double;
                         pixelWidth: Integer; Color: OLE_COLOR);
    procedure DrawPointEx(LayerHandle: Integer; x: Double; y: Double; pixelSize: Integer; 
                          Color: OLE_COLOR);
    procedure DrawCircleEx(LayerHandle: Integer; x: Double; y: Double; pixelRadius: Double; 
                           Color: OLE_COLOR; fill: WordBool);
    procedure LabelColor(LayerHandle: Integer; LabelFontColor: OLE_COLOR);
    procedure SetDrawingLayerVisible(LayerHandle: Integer; Visiable: WordBool);
    procedure ClearDrawingLabels(DrawHandle: Integer);
    procedure DrawingFont(DrawHandle: Integer; const FontName: WideString; FontSize: Integer);
    procedure AddDrawingLabelEx(DrawHandle: Integer; const Text: WideString; Color: OLE_COLOR; 
                                x: Double; y: Double; hJustification: tkHJustification; 
                                Rotation: Double);
    procedure AddDrawingLabel(DrawHandle: Integer; const Text: WideString; Color: OLE_COLOR; 
                              x: Double; y: Double; hJustification: tkHJustification);
    procedure GetDrawingStandardViewWidth(DrawHandle: Integer; var Width: Double);
    procedure SetDrawingStandardViewWidth(DrawHandle: Integer; Width: Double);
    procedure DrawWidePolygon(var xPoints: OleVariant; var yPoints: OleVariant; numPoints: Integer; 
                              Color: OLE_COLOR; fill: WordBool; Width: Smallint);
    procedure DrawWideCircle(x: Double; y: Double; pixelRadius: Double; Color: OLE_COLOR; 
                             fill: WordBool; Width: Smallint);
    function SnapShot2(ClippingLayerNbr: Integer; Zoom: Double; pWidth: Integer): IDispatch;
    procedure LayerFontEx(LayerHandle: Integer; const FontName: WideString; FontSize: Integer; 
                          isBold: WordBool; isItalic: WordBool; isUnderline: WordBool);
    procedure set_UDPointFontCharFont(LayerHandle: Integer; const FontName: WideString; 
                                      FontSize: Single; isBold: WordBool; isItalic: WordBool; 
                                      isUnderline: WordBool);
    function set_UDPointFontCharListAdd(LayerHandle: Integer; newValue: Integer; Color: OLE_COLOR): Integer;
    procedure set_UDPointFontCharFontSize(LayerHandle: Integer; FontSize: Single);
    procedure ReSourceLayer(LayerHandle: Integer; const newSrcPath: WideString);
    property  ControlInterface: _DMap read GetControlInterface;
    property  DefaultInterface: _DMap read GetControlInterface;
    property GlobalCallback: IDispatch index 15 read GetIDispatchProp write SetIDispatchProp;
    property Extents: IDispatch index 17 read GetIDispatchProp write SetIDispatchProp;
    property LayerKey[LayerHandle: Integer]: WideString read Get_LayerKey write Set_LayerKey;
    property LayerPosition[LayerHandle: Integer]: Integer read Get_LayerPosition;
    property LayerHandle[LayerPosition: Integer]: Integer read Get_LayerHandle;
    property LayerVisible[LayerHandle: Integer]: WordBool read Get_LayerVisible write Set_LayerVisible;
    property ShapeLayerFillColor[LayerHandle: Integer]: OLE_COLOR read Get_ShapeLayerFillColor write Set_ShapeLayerFillColor;
    property ShapeFillColor[LayerHandle: Integer; Shape: Integer]: OLE_COLOR read Get_ShapeFillColor write Set_ShapeFillColor;
    property ShapeLayerLineColor[LayerHandle: Integer]: OLE_COLOR read Get_ShapeLayerLineColor write Set_ShapeLayerLineColor;
    property ShapeLineColor[LayerHandle: Integer; Shape: Integer]: OLE_COLOR read Get_ShapeLineColor write Set_ShapeLineColor;
    property ShapeLayerPointColor[LayerHandle: Integer]: OLE_COLOR read Get_ShapeLayerPointColor write Set_ShapeLayerPointColor;
    property ShapePointColor[LayerHandle: Integer; Shape: Integer]: OLE_COLOR read Get_ShapePointColor write Set_ShapePointColor;
    property ShapeLayerDrawFill[LayerHandle: Integer]: WordBool read Get_ShapeLayerDrawFill write Set_ShapeLayerDrawFill;
    property ShapeDrawFill[LayerHandle: Integer; Shape: Integer]: WordBool read Get_ShapeDrawFill write Set_ShapeDrawFill;
    property ShapeLayerDrawLine[LayerHandle: Integer]: WordBool read Get_ShapeLayerDrawLine write Set_ShapeLayerDrawLine;
    property ShapeDrawLine[LayerHandle: Integer; Shape: Integer]: WordBool read Get_ShapeDrawLine write Set_ShapeDrawLine;
    property ShapeLayerDrawPoint[LayerHandle: Integer]: WordBool read Get_ShapeLayerDrawPoint write Set_ShapeLayerDrawPoint;
    property ShapeDrawPoint[LayerHandle: Integer; Shape: Integer]: WordBool read Get_ShapeDrawPoint write Set_ShapeDrawPoint;
    property ShapeLayerLineWidth[LayerHandle: Integer]: Single read Get_ShapeLayerLineWidth write Set_ShapeLayerLineWidth;
    property ShapeLineWidth[LayerHandle: Integer; Shape: Integer]: Single read Get_ShapeLineWidth write Set_ShapeLineWidth;
    property ShapeLayerPointSize[LayerHandle: Integer]: Single read Get_ShapeLayerPointSize write Set_ShapeLayerPointSize;
    property ShapePointSize[LayerHandle: Integer; Shape: Integer]: Single read Get_ShapePointSize write Set_ShapePointSize;
    property ShapeLayerFillTransparency[LayerHandle: Integer]: Single read Get_ShapeLayerFillTransparency write Set_ShapeLayerFillTransparency;
    property ShapeFillTransparency[LayerHandle: Integer; Shape: Integer]: Single read Get_ShapeFillTransparency write Set_ShapeFillTransparency;
    property ShapeLayerLineStipple[LayerHandle: Integer]: tkLineStipple read Get_ShapeLayerLineStipple write Set_ShapeLayerLineStipple;
    property ShapeLineStipple[LayerHandle: Integer; Shape: Integer]: tkLineStipple read Get_ShapeLineStipple write Set_ShapeLineStipple;
    property ShapeLayerFillStipple[LayerHandle: Integer]: tkFillStipple read Get_ShapeLayerFillStipple write Set_ShapeLayerFillStipple;
    property ShapeFillStipple[LayerHandle: Integer; Shape: Integer]: tkFillStipple read Get_ShapeFillStipple write Set_ShapeFillStipple;
    property ShapeVisible[LayerHandle: Integer; Shape: Integer]: WordBool read Get_ShapeVisible write Set_ShapeVisible;
    property ImageLayerPercentTransparent[LayerHandle: Integer]: Single read Get_ImageLayerPercentTransparent write Set_ImageLayerPercentTransparent;
    property ErrorMsg[ErrorCode: Integer]: WideString read Get_ErrorMsg;
    property DrawingKey[DrawHandle: Integer]: WideString read Get_DrawingKey write Set_DrawingKey;
    property ShapeLayerPointType[LayerHandle: Integer]: tkPointType read Get_ShapeLayerPointType write Set_ShapeLayerPointType;
    property ShapePointType[LayerHandle: Integer; Shape: Integer]: tkPointType read Get_ShapePointType write Set_ShapePointType;
    property LayerLabelsVisible[LayerHandle: Integer]: WordBool read Get_LayerLabelsVisible write Set_LayerLabelsVisible;
    property UDLineStipple[LayerHandle: Integer]: Integer read Get_UDLineStipple write Set_UDLineStipple;
    property UDFillStipple[LayerHandle: Integer; StippleRow: Integer]: Integer read Get_UDFillStipple write Set_UDFillStipple;
    property UDPointType[LayerHandle: Integer]: IDispatch read Get_UDPointType;
    property GetObject[LayerHandle: Integer]: IDispatch read Get_GetObject;
    property LayerName[LayerHandle: Integer]: WideString read Get_LayerName write Set_LayerName;
    property GridFileName[LayerHandle: Integer]: WideString read Get_GridFileName write Set_GridFileName;
    property LayerLabelsShadow[LayerHandle: Integer]: WordBool read Get_LayerLabelsShadow write Set_LayerLabelsShadow;
    property LayerLabelsScale[LayerHandle: Integer]: WordBool read Get_LayerLabelsScale write Set_LayerLabelsScale;
    property LayerLabelsOffset[LayerHandle: Integer]: Integer read Get_LayerLabelsOffset write Set_LayerLabelsOffset;
    property LayerLabelsShadowColor[LayerHandle: Integer]: OLE_COLOR read Get_LayerLabelsShadowColor write Set_LayerLabelsShadowColor;
    property UseLabelCollision[LayerHandle: Integer]: WordBool read Get_UseLabelCollision write Set_UseLabelCollision;
    property ShapePointImageListID[LayerHandle: Integer; Shape: Integer]: Integer read Get_ShapePointImageListID write Set_ShapePointImageListID;
    property DrawingLabelsOffset[DrawHandle: Integer]: Integer read Get_DrawingLabelsOffset write Set_DrawingLabelsOffset;
    property DrawingLabelsScale[DrawHandle: Integer]: WordBool read Get_DrawingLabelsScale write Set_DrawingLabelsScale;
    property DrawingLabelsShadow[DrawHandle: Integer]: WordBool read Get_DrawingLabelsShadow write Set_DrawingLabelsShadow;
    property DrawingLabelsShadowColor[DrawHandle: Integer]: OLE_COLOR read Get_DrawingLabelsShadowColor write Set_DrawingLabelsShadowColor;
    property UseDrawingLabelCollision[DrawHandle: Integer]: WordBool read Get_UseDrawingLabelCollision write Set_UseDrawingLabelCollision;
    property DrawingLabelsVisible[DrawHandle: Integer]: WordBool read Get_DrawingLabelsVisible write Set_DrawingLabelsVisible;
    property ShapePointFontCharListID[LayerHandle: Integer; Shape: Integer]: Integer read Get_ShapePointFontCharListID write Set_ShapePointFontCharListID;
    property ShapeLayerStippleColor[LayerHandle: Integer]: OLE_COLOR read Get_ShapeLayerStippleColor write Set_ShapeLayerStippleColor;
    property ShapeStippleColor[LayerHandle: Integer; Shape: Integer]: OLE_COLOR read Get_ShapeStippleColor write Set_ShapeStippleColor;
    property ShapeStippleTransparent[LayerHandle: Integer; Shape: Integer]: WordBool read Get_ShapeStippleTransparent write Set_ShapeStippleTransparent;
    property ShapeLayerStippleTransparent[LayerHandle: Integer]: WordBool read Get_ShapeLayerStippleTransparent write Set_ShapeLayerStippleTransparent;
  published
    property Anchors;
    property  TabStop;
    property  Align;
    property  DragCursor;
    property  DragMode;
    property  ParentShowHint;
    property  PopupMenu;
    property  ShowHint;
    property  TabOrder;
    property  Visible;
    property  OnDragDrop;
    property  OnDragOver;
    property  OnEndDrag;
    property  OnEnter;
    property  OnExit;
    property  OnStartDrag;
    property  OnDblClick;
    property BackColor: TColor index 1 read GetTColorProp write SetTColorProp stored False;
    property ZoomPercent: Double index 2 read GetDoubleProp write SetDoubleProp stored False;
    property CursorMode: TOleEnum index 3 read GetTOleEnumProp write SetTOleEnumProp stored False;
    property MapCursor: TOleEnum index 4 read GetTOleEnumProp write SetTOleEnumProp stored False;
    property UDCursorHandle: Integer index 5 read GetIntegerProp write SetIntegerProp stored False;
    property SendMouseDown: WordBool index 6 read GetWordBoolProp write SetWordBoolProp stored False;
    property SendMouseUp: WordBool index 7 read GetWordBoolProp write SetWordBoolProp stored False;
    property SendMouseMove: WordBool index 8 read GetWordBoolProp write SetWordBoolProp stored False;
    property SendSelectBoxDrag: WordBool index 9 read GetWordBoolProp write SetWordBoolProp stored False;
    property SendSelectBoxFinal: WordBool index 10 read GetWordBoolProp write SetWordBoolProp stored False;
    property ExtentPad: Double index 11 read GetDoubleProp write SetDoubleProp stored False;
    property ExtentHistory: Integer index 12 read GetIntegerProp write SetIntegerProp stored False;
    property Key: WideString index 13 read GetWideStringProp write SetWideStringProp stored False;
    property DoubleBuffer: WordBool index 14 read GetWordBoolProp write SetWordBoolProp stored False;
    property NumLayers: Integer index 16 read GetIntegerProp write SetIntegerProp stored False;
    property LastErrorCode: Integer index 18 read GetIntegerProp write SetIntegerProp stored False;
    property IsLocked: TOleEnum index 19 read GetTOleEnumProp write SetTOleEnumProp stored False;
    property MapState: WideString index 20 read GetWideStringProp write SetWideStringProp stored False;
    property SerialNumber: WideString index 95 read GetWideStringProp write SetWideStringProp stored False;
    property LineSeparationFactor: Integer index 96 read GetIntegerProp write SetIntegerProp stored False;
    property SendOnDrawBackBuffer: WordBool index 118 read GetWordBoolProp write SetWordBoolProp stored False;
    property MultilineLabels: WordBool index 133 read GetWordBoolProp write SetWordBoolProp stored False;
    property MapResizeBehavior: TOleEnum index 108 read GetTOleEnumProp write SetTOleEnumProp stored False;
    property OnMouseDown: TMapMouseDown read FOnMouseDown write FOnMouseDown;
    property OnMouseUp: TMapMouseUp read FOnMouseUp write FOnMouseUp;
    property OnMouseMove: TMapMouseMove read FOnMouseMove write FOnMouseMove;
    property OnFileDropped: TMapFileDropped read FOnFileDropped write FOnFileDropped;
    property OnSelectBoxFinal: TMapSelectBoxFinal read FOnSelectBoxFinal write FOnSelectBoxFinal;
    property OnSelectBoxDrag: TMapSelectBoxDrag read FOnSelectBoxDrag write FOnSelectBoxDrag;
    property OnExtentsChanged: TNotifyEvent read FOnExtentsChanged write FOnExtentsChanged;
    property OnMapState: TMapMapState read FOnMapState write FOnMapState;
    property OnDrawBackBuffer: TMapOnDrawBackBuffer read FOnDrawBackBuffer write FOnDrawBackBuffer;
    property TrapRMouseDown: WordBool read Get_TrapRMouseDown write Set_TrapRMouseDown;

  end;

// *********************************************************************//
// The Class CoShapefileColorScheme provides a Create and CreateRemote method to          
// create instances of the default interface IShapefileColorScheme exposed by              
// the CoClass ShapefileColorScheme. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoShapefileColorScheme = class
    class function Create: IShapefileColorScheme;
    class function CreateRemote(const MachineName: string): IShapefileColorScheme;
  end;

// *********************************************************************//
// The Class CoShapefileColorBreak provides a Create and CreateRemote method to          
// create instances of the default interface IShapefileColorBreak exposed by              
// the CoClass ShapefileColorBreak. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoShapefileColorBreak = class
    class function Create: IShapefileColorBreak;
    class function CreateRemote(const MachineName: string): IShapefileColorBreak;
  end;

// *********************************************************************//
// The Class CoGrid provides a Create and CreateRemote method to          
// create instances of the default interface IGrid exposed by              
// the CoClass Grid. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoGrid = class
    class function Create: IGrid;
    class function CreateRemote(const MachineName: string): IGrid;
  end;

// *********************************************************************//
// The Class CoGridHeader provides a Create and CreateRemote method to          
// create instances of the default interface IGridHeader exposed by              
// the CoClass GridHeader. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoGridHeader = class
    class function Create: IGridHeader;
    class function CreateRemote(const MachineName: string): IGridHeader;
  end;

// *********************************************************************//
// The Class CoESRIGridManager provides a Create and CreateRemote method to          
// create instances of the default interface IESRIGridManager exposed by              
// the CoClass ESRIGridManager. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoESRIGridManager = class
    class function Create: IESRIGridManager;
    class function CreateRemote(const MachineName: string): IESRIGridManager;
  end;

// *********************************************************************//
// The Class CoImage provides a Create and CreateRemote method to          
// create instances of the default interface IImage exposed by              
// the CoClass Image. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoImage = class
    class function Create: IImage;
    class function CreateRemote(const MachineName: string): IImage;
  end;

// *********************************************************************//
// The Class CoShapefile provides a Create and CreateRemote method to          
// create instances of the default interface IShapefile exposed by              
// the CoClass Shapefile. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoShapefile = class
    class function Create: IShapefile;
    class function CreateRemote(const MachineName: string): IShapefile;
  end;

// *********************************************************************//
// The Class CoShape provides a Create and CreateRemote method to          
// create instances of the default interface IShape exposed by              
// the CoClass Shape. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoShape = class
    class function Create: IShape;
    class function CreateRemote(const MachineName: string): IShape;
  end;

// *********************************************************************//
// The Class CoExtents provides a Create and CreateRemote method to          
// create instances of the default interface IExtents exposed by              
// the CoClass Extents. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoExtents = class
    class function Create: IExtents;
    class function CreateRemote(const MachineName: string): IExtents;
  end;

// *********************************************************************//
// The Class CoPoint provides a Create and CreateRemote method to          
// create instances of the default interface IPoint exposed by              
// the CoClass Point. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoPoint = class
    class function Create: IPoint;
    class function CreateRemote(const MachineName: string): IPoint;
  end;

// *********************************************************************//
// The Class CoTable provides a Create and CreateRemote method to          
// create instances of the default interface ITable exposed by              
// the CoClass Table. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoTable = class
    class function Create: ITable;
    class function CreateRemote(const MachineName: string): ITable;
  end;

// *********************************************************************//
// The Class CoField provides a Create and CreateRemote method to          
// create instances of the default interface IField exposed by              
// the CoClass Field. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoField = class
    class function Create: IField;
    class function CreateRemote(const MachineName: string): IField;
  end;

// *********************************************************************//
// The Class CoShapeNetwork provides a Create and CreateRemote method to          
// create instances of the default interface IShapeNetwork exposed by              
// the CoClass ShapeNetwork. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoShapeNetwork = class
    class function Create: IShapeNetwork;
    class function CreateRemote(const MachineName: string): IShapeNetwork;
  end;

// *********************************************************************//
// The Class CoUtils provides a Create and CreateRemote method to
// create instances of the default interface IUtils exposed by              
// the CoClass Utils. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoUtils = class
    class function Create: IUtils;
    class function CreateRemote(const MachineName: string): IUtils;
  end;

// *********************************************************************//
// The Class CoVector provides a Create and CreateRemote method to          
// create instances of the default interface IVector exposed by              
// the CoClass Vector. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoVector = class
    class function Create: IVector;
    class function CreateRemote(const MachineName: string): IVector;
  end;

// *********************************************************************//
// The Class CoGridColorScheme provides a Create and CreateRemote method to          
// create instances of the default interface IGridColorScheme exposed by              
// the CoClass GridColorScheme. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoGridColorScheme = class
    class function Create: IGridColorScheme;
    class function CreateRemote(const MachineName: string): IGridColorScheme;
  end;

// *********************************************************************//
// The Class CoGridColorBreak provides a Create and CreateRemote method to          
// create instances of the default interface IGridColorBreak exposed by              
// the CoClass GridColorBreak. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoGridColorBreak = class
    class function Create: IGridColorBreak;
    class function CreateRemote(const MachineName: string): IGridColorBreak;
  end;

// *********************************************************************//
// The Class CoTin provides a Create and CreateRemote method to          
// create instances of the default interface ITin exposed by              
// the CoClass Tin. The functions are intended to be used by
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoTin = class
    class function Create: ITin;
    class function CreateRemote(const MachineName: string): ITin;
  end;

procedure Register;

resourcestring
  dtlServerPage = 'ActiveX';

  dtlOcxPage = 'ActiveX';

implementation

uses ComObj;

procedure TMap.InitControlData;
const
  CEventDispIDs: array [0..8] of DWORD = (
    $00000001, $00000002, $00000003, $00000004, $00000005, $00000006,
    $00000007, $00000008, $00000009);
  CControlData: TControlData2 = (
    ClassID: '{54F4C2F7-ED40-43B7-9D6F-E45965DF7F95}';
    EventIID: '{ABEA1545-08AB-4D5C-A594-D3017211EA95}';
    EventCount: 9;
    EventDispIDs: @CEventDispIDs;
    LicenseKey: nil (*HR:$80004005*);
    Flags: $00000000;
    Version: 401);
begin
  ControlData := @CControlData;
  TControlData2(CControlData).FirstEventOfs := Cardinal(@@FOnMouseDown) - Cardinal(Self);
end;

procedure TMap.CreateControl;

  procedure DoCreate;
  begin
    FIntf := IUnknown(OleObject) as _DMap;
  end;

begin
  if FIntf = nil then DoCreate;
end;

function TMap.GetControlInterface: _DMap;
begin
  CreateControl;
  Result := FIntf;
end;

function TMap.Get_GlobalCallback: IDispatch;
begin
  Result := DefaultInterface.GlobalCallback;
end;

procedure TMap.Set_GlobalCallback(const Value: IDispatch);
begin
  DefaultInterface.GlobalCallback := Value;
end;

function TMap.Get_Extents: IDispatch;
begin
  Result := DefaultInterface.Extents;
end;

procedure TMap.Set_Extents(const Value: IDispatch);
begin
  DefaultInterface.Extents := Value;
end;

function TMap.Get_LayerKey(LayerHandle: Integer): WideString;
begin
    Result := DefaultInterface.LayerKey[LayerHandle];
end;

procedure TMap.Set_LayerKey(LayerHandle: Integer; const Param2: WideString);
  { Warning: The property LayerKey has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.LayerKey := Param2;
end;

function TMap.Get_LayerPosition(LayerHandle: Integer): Integer;
begin
    Result := DefaultInterface.LayerPosition[LayerHandle];
end;

function TMap.Get_LayerHandle(LayerPosition: Integer): Integer;
begin
    Result := DefaultInterface.LayerHandle[LayerPosition];
end;

function TMap.Get_LayerVisible(LayerHandle: Integer): WordBool;
begin
    Result := DefaultInterface.LayerVisible[LayerHandle];
end;

procedure TMap.Set_LayerVisible(LayerHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.LayerVisible[LayerHandle] := Param2;
end;

function TMap.Get_ShapeLayerFillColor(LayerHandle: Integer): OLE_COLOR;
begin
    Result := DefaultInterface.ShapeLayerFillColor[LayerHandle];
end;

procedure TMap.Set_ShapeLayerFillColor(LayerHandle: Integer; Param2: OLE_COLOR);
begin
  DefaultInterface.ShapeLayerFillColor[LayerHandle] := Param2;
end;

function TMap.Get_ShapeFillColor(LayerHandle: Integer; Shape: Integer): OLE_COLOR;
begin
    Result := DefaultInterface.ShapeFillColor[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeFillColor(LayerHandle: Integer; Shape: Integer; Param3: OLE_COLOR);
begin
  DefaultInterface.ShapeFillColor[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerLineColor(LayerHandle: Integer): OLE_COLOR;
begin
    Result := DefaultInterface.ShapeLayerLineColor[LayerHandle];
end;

procedure TMap.Set_ShapeLayerLineColor(LayerHandle: Integer; Param2: OLE_COLOR);
begin
  DefaultInterface.ShapeLayerLineColor[LayerHandle] := Param2;
end;

function TMap.Get_ShapeLineColor(LayerHandle: Integer; Shape: Integer): OLE_COLOR;
begin
    Result := DefaultInterface.ShapeLineColor[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeLineColor(LayerHandle: Integer; Shape: Integer; Param3: OLE_COLOR);
begin
  DefaultInterface.ShapeLineColor[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerPointColor(LayerHandle: Integer): OLE_COLOR;
begin
    Result := DefaultInterface.ShapeLayerPointColor[LayerHandle];
end;

procedure TMap.Set_ShapeLayerPointColor(LayerHandle: Integer; Param2: OLE_COLOR);
begin
  DefaultInterface.ShapeLayerPointColor[LayerHandle] := Param2;
end;

function TMap.Get_ShapePointColor(LayerHandle: Integer; Shape: Integer): OLE_COLOR;
begin
    Result := DefaultInterface.ShapePointColor[LayerHandle, Shape];
end;

procedure TMap.Set_ShapePointColor(LayerHandle: Integer; Shape: Integer; Param3: OLE_COLOR);
begin
  DefaultInterface.ShapePointColor[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerDrawFill(LayerHandle: Integer): WordBool;
begin
    Result := DefaultInterface.ShapeLayerDrawFill[LayerHandle];
end;

procedure TMap.Set_ShapeLayerDrawFill(LayerHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.ShapeLayerDrawFill[LayerHandle] := Param2;
end;

function TMap.Get_ShapeDrawFill(LayerHandle: Integer; Shape: Integer): WordBool;
begin
    Result := DefaultInterface.ShapeDrawFill[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeDrawFill(LayerHandle: Integer; Shape: Integer; Param3: WordBool);
begin
  DefaultInterface.ShapeDrawFill[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerDrawLine(LayerHandle: Integer): WordBool;
begin
    Result := DefaultInterface.ShapeLayerDrawLine[LayerHandle];
end;

procedure TMap.Set_ShapeLayerDrawLine(LayerHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.ShapeLayerDrawLine[LayerHandle] := Param2;
end;

function TMap.Get_ShapeDrawLine(LayerHandle: Integer; Shape: Integer): WordBool;
begin
    Result := DefaultInterface.ShapeDrawLine[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeDrawLine(LayerHandle: Integer; Shape: Integer; Param3: WordBool);
begin
  DefaultInterface.ShapeDrawLine[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerDrawPoint(LayerHandle: Integer): WordBool;
begin
    Result := DefaultInterface.ShapeLayerDrawPoint[LayerHandle];
end;

procedure TMap.Set_ShapeLayerDrawPoint(LayerHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.ShapeLayerDrawPoint[LayerHandle] := Param2;
end;

function TMap.Get_ShapeDrawPoint(LayerHandle: Integer; Shape: Integer): WordBool;
begin
    Result := DefaultInterface.ShapeDrawPoint[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeDrawPoint(LayerHandle: Integer; Shape: Integer; Param3: WordBool);
begin
  DefaultInterface.ShapeDrawPoint[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerLineWidth(LayerHandle: Integer): Single;
begin
    Result := DefaultInterface.ShapeLayerLineWidth[LayerHandle];
end;

procedure TMap.Set_ShapeLayerLineWidth(LayerHandle: Integer; Param2: Single);
begin
  DefaultInterface.ShapeLayerLineWidth[LayerHandle] := Param2;
end;

function TMap.Get_ShapeLineWidth(LayerHandle: Integer; Shape: Integer): Single;
begin
    Result := DefaultInterface.ShapeLineWidth[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeLineWidth(LayerHandle: Integer; Shape: Integer; Param3: Single);
begin
  DefaultInterface.ShapeLineWidth[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerPointSize(LayerHandle: Integer): Single;
begin
    Result := DefaultInterface.ShapeLayerPointSize[LayerHandle];
end;

procedure TMap.Set_ShapeLayerPointSize(LayerHandle: Integer; Param2: Single);
begin
  DefaultInterface.ShapeLayerPointSize[LayerHandle] := Param2;
end;

function TMap.Get_ShapePointSize(LayerHandle: Integer; Shape: Integer): Single;
begin
    Result := DefaultInterface.ShapePointSize[LayerHandle, Shape];
end;

procedure TMap.Set_ShapePointSize(LayerHandle: Integer; Shape: Integer; Param3: Single);
begin
  DefaultInterface.ShapePointSize[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerFillTransparency(LayerHandle: Integer): Single;
begin
    Result := DefaultInterface.ShapeLayerFillTransparency[LayerHandle];
end;

procedure TMap.Set_ShapeLayerFillTransparency(LayerHandle: Integer; Param2: Single);
begin
  DefaultInterface.ShapeLayerFillTransparency[LayerHandle] := Param2;
end;

function TMap.Get_ShapeFillTransparency(LayerHandle: Integer; Shape: Integer): Single;
begin
    Result := DefaultInterface.ShapeFillTransparency[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeFillTransparency(LayerHandle: Integer; Shape: Integer; Param3: Single);
begin
  DefaultInterface.ShapeFillTransparency[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerLineStipple(LayerHandle: Integer): tkLineStipple;
begin
    Result := DefaultInterface.ShapeLayerLineStipple[LayerHandle];
end;

procedure TMap.Set_ShapeLayerLineStipple(LayerHandle: Integer; Param2: tkLineStipple);
begin
  DefaultInterface.ShapeLayerLineStipple[LayerHandle] := Param2;
end;

function TMap.Get_ShapeLineStipple(LayerHandle: Integer; Shape: Integer): tkLineStipple;
begin
    Result := DefaultInterface.ShapeLineStipple[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeLineStipple(LayerHandle: Integer; Shape: Integer; Param3: tkLineStipple);
begin
  DefaultInterface.ShapeLineStipple[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerFillStipple(LayerHandle: Integer): tkFillStipple;
begin
    Result := DefaultInterface.ShapeLayerFillStipple[LayerHandle];
end;

procedure TMap.Set_ShapeLayerFillStipple(LayerHandle: Integer; Param2: tkFillStipple);
begin
  DefaultInterface.ShapeLayerFillStipple[LayerHandle] := Param2;
end;

function TMap.Get_ShapeFillStipple(LayerHandle: Integer; Shape: Integer): tkFillStipple;
begin
    Result := DefaultInterface.ShapeFillStipple[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeFillStipple(LayerHandle: Integer; Shape: Integer; Param3: tkFillStipple);
begin
  DefaultInterface.ShapeFillStipple[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeVisible(LayerHandle: Integer; Shape: Integer): WordBool;
begin
    Result := DefaultInterface.ShapeVisible[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeVisible(LayerHandle: Integer; Shape: Integer; Param3: WordBool);
begin
  DefaultInterface.ShapeVisible[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ImageLayerPercentTransparent(LayerHandle: Integer): Single;
begin
    Result := DefaultInterface.ImageLayerPercentTransparent[LayerHandle];
end;

procedure TMap.Set_ImageLayerPercentTransparent(LayerHandle: Integer; Param2: Single);
begin
  DefaultInterface.ImageLayerPercentTransparent[LayerHandle] := Param2;
end;

function TMap.Get_ErrorMsg(ErrorCode: Integer): WideString;
begin
    Result := DefaultInterface.ErrorMsg[ErrorCode];
end;

function TMap.Get_DrawingKey(DrawHandle: Integer): WideString;
begin
    Result := DefaultInterface.DrawingKey[DrawHandle];
end;

procedure TMap.Set_DrawingKey(DrawHandle: Integer; const Param2: WideString);
  { Warning: The property DrawingKey has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.DrawingKey := Param2;
end;

function TMap.Get_ShapeLayerPointType(LayerHandle: Integer): tkPointType;
begin
    Result := DefaultInterface.ShapeLayerPointType[LayerHandle];
end;

procedure TMap.Set_ShapeLayerPointType(LayerHandle: Integer; Param2: tkPointType);
begin
  DefaultInterface.ShapeLayerPointType[LayerHandle] := Param2;
end;

function TMap.Get_ShapePointType(LayerHandle: Integer; Shape: Integer): tkPointType;
begin
    Result := DefaultInterface.ShapePointType[LayerHandle, Shape];
end;

procedure TMap.Set_ShapePointType(LayerHandle: Integer; Shape: Integer; Param3: tkPointType);
begin
  DefaultInterface.ShapePointType[LayerHandle, Shape] := Param3;
end;

function TMap.Get_LayerLabelsVisible(LayerHandle: Integer): WordBool;
begin
    Result := DefaultInterface.LayerLabelsVisible[LayerHandle];
end;

procedure TMap.Set_LayerLabelsVisible(LayerHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.LayerLabelsVisible[LayerHandle] := Param2;
end;

function TMap.Get_UDLineStipple(LayerHandle: Integer): Integer;
begin
    Result := DefaultInterface.UDLineStipple[LayerHandle];
end;

procedure TMap.Set_UDLineStipple(LayerHandle: Integer; Param2: Integer);
begin
  DefaultInterface.UDLineStipple[LayerHandle] := Param2;
end;

function TMap.Get_UDFillStipple(LayerHandle: Integer; StippleRow: Integer): Integer;
begin
    Result := DefaultInterface.UDFillStipple[LayerHandle, StippleRow];
end;

procedure TMap.Set_UDFillStipple(LayerHandle: Integer; StippleRow: Integer; Param3: Integer);
begin
  DefaultInterface.UDFillStipple[LayerHandle, StippleRow] := Param3;
end;

function TMap.Get_UDPointType(LayerHandle: Integer): IDispatch;
begin
    Result := DefaultInterface.UDPointType[LayerHandle];
end;

procedure TMap._Set_UDPointType(LayerHandle: Integer; const Param2: IDispatch);
  { Warning: The property UDPointType has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.UDPointType := Param2;
end;

function TMap.Get_GetObject(LayerHandle: Integer): IDispatch;
begin
    Result := DefaultInterface.GetObject[LayerHandle];
end;

function TMap.Get_LayerName(LayerHandle: Integer): WideString;
begin
    Result := DefaultInterface.LayerName[LayerHandle];
end;

procedure TMap.Set_LayerName(LayerHandle: Integer; const Param2: WideString);
  { Warning: The property LayerName has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.LayerName := Param2;
end;

function TMap.Get_GridFileName(LayerHandle: Integer): WideString;
begin
    Result := DefaultInterface.GridFileName[LayerHandle];
end;

procedure TMap.Set_GridFileName(LayerHandle: Integer; const Param2: WideString);
  { Warning: The property GridFileName has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.GridFileName := Param2;
end;

function TMap.Get_LayerLabelsShadow(LayerHandle: Integer): WordBool;
begin
    Result := DefaultInterface.LayerLabelsShadow[LayerHandle];
end;

procedure TMap.Set_LayerLabelsShadow(LayerHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.LayerLabelsShadow[LayerHandle] := Param2;
end;

function TMap.Get_LayerLabelsScale(LayerHandle: Integer): WordBool;
begin
    Result := DefaultInterface.LayerLabelsScale[LayerHandle];
end;

procedure TMap.Set_LayerLabelsScale(LayerHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.LayerLabelsScale[LayerHandle] := Param2;
end;

function TMap.Get_LayerLabelsOffset(LayerHandle: Integer): Integer;
begin
    Result := DefaultInterface.LayerLabelsOffset[LayerHandle];
end;

procedure TMap.Set_LayerLabelsOffset(LayerHandle: Integer; Param2: Integer);
begin
  DefaultInterface.LayerLabelsOffset[LayerHandle] := Param2;
end;

function TMap.Get_LayerLabelsShadowColor(LayerHandle: Integer): OLE_COLOR;
begin
    Result := DefaultInterface.LayerLabelsShadowColor[LayerHandle];
end;

procedure TMap.Set_LayerLabelsShadowColor(LayerHandle: Integer; Param2: OLE_COLOR);
begin
  DefaultInterface.LayerLabelsShadowColor[LayerHandle] := Param2;
end;

function TMap.Get_UseLabelCollision(LayerHandle: Integer): WordBool;
begin
    Result := DefaultInterface.UseLabelCollision[LayerHandle];
end;

procedure TMap.Set_UseLabelCollision(LayerHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.UseLabelCollision[LayerHandle] := Param2;
end;

function TMap.Get_ShapePointImageListID(LayerHandle: Integer; Shape: Integer): Integer;
begin
    Result := DefaultInterface.ShapePointImageListID[LayerHandle, Shape];
end;

procedure TMap.Set_ShapePointImageListID(LayerHandle: Integer; Shape: Integer; Param3: Integer);
begin
  DefaultInterface.ShapePointImageListID[LayerHandle, Shape] := Param3;
end;

function TMap.Get_DrawingLabelsOffset(DrawHandle: Integer): Integer;
begin
    Result := DefaultInterface.DrawingLabelsOffset[DrawHandle];
end;

procedure TMap.Set_DrawingLabelsOffset(DrawHandle: Integer; Param2: Integer);
begin
  DefaultInterface.DrawingLabelsOffset[DrawHandle] := Param2;
end;

function TMap.Get_DrawingLabelsScale(DrawHandle: Integer): WordBool;
begin
    Result := DefaultInterface.DrawingLabelsScale[DrawHandle];
end;

procedure TMap.Set_DrawingLabelsScale(DrawHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.DrawingLabelsScale[DrawHandle] := Param2;
end;

function TMap.Get_DrawingLabelsShadow(DrawHandle: Integer): WordBool;
begin
    Result := DefaultInterface.DrawingLabelsShadow[DrawHandle];
end;

procedure TMap.Set_DrawingLabelsShadow(DrawHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.DrawingLabelsShadow[DrawHandle] := Param2;
end;

function TMap.Get_DrawingLabelsShadowColor(DrawHandle: Integer): OLE_COLOR;
begin
    Result := DefaultInterface.DrawingLabelsShadowColor[DrawHandle];
end;

procedure TMap.Set_DrawingLabelsShadowColor(DrawHandle: Integer; Param2: OLE_COLOR);
begin
  DefaultInterface.DrawingLabelsShadowColor[DrawHandle] := Param2;
end;

function TMap.Get_UseDrawingLabelCollision(DrawHandle: Integer): WordBool;
begin
    Result := DefaultInterface.UseDrawingLabelCollision[DrawHandle];
end;

procedure TMap.Set_UseDrawingLabelCollision(DrawHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.UseDrawingLabelCollision[DrawHandle] := Param2;
end;

function TMap.Get_DrawingLabelsVisible(DrawHandle: Integer): WordBool;
begin
    Result := DefaultInterface.DrawingLabelsVisible[DrawHandle];
end;

procedure TMap.Set_DrawingLabelsVisible(DrawHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.DrawingLabelsVisible[DrawHandle] := Param2;
end;

function TMap.Get_ShapePointFontCharListID(LayerHandle: Integer; Shape: Integer): Integer;
begin
    Result := DefaultInterface.ShapePointFontCharListID[LayerHandle, Shape];
end;

procedure TMap.Set_ShapePointFontCharListID(LayerHandle: Integer; Shape: Integer; Param3: Integer);
begin
  DefaultInterface.ShapePointFontCharListID[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerStippleColor(LayerHandle: Integer): OLE_COLOR;
begin
    Result := DefaultInterface.ShapeLayerStippleColor[LayerHandle];
end;

procedure TMap.Set_ShapeLayerStippleColor(LayerHandle: Integer; Param2: OLE_COLOR);
begin
  DefaultInterface.ShapeLayerStippleColor[LayerHandle] := Param2;
end;

function TMap.Get_ShapeStippleColor(LayerHandle: Integer; Shape: Integer): OLE_COLOR;
begin
    Result := DefaultInterface.ShapeStippleColor[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeStippleColor(LayerHandle: Integer; Shape: Integer; Param3: OLE_COLOR);
begin
  DefaultInterface.ShapeStippleColor[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeStippleTransparent(LayerHandle: Integer; Shape: Integer): WordBool;
begin
    Result := DefaultInterface.ShapeStippleTransparent[LayerHandle, Shape];
end;

procedure TMap.Set_ShapeStippleTransparent(LayerHandle: Integer; Shape: Integer; Param3: WordBool);
begin
  DefaultInterface.ShapeStippleTransparent[LayerHandle, Shape] := Param3;
end;

function TMap.Get_ShapeLayerStippleTransparent(LayerHandle: Integer): WordBool;
begin
    Result := DefaultInterface.ShapeLayerStippleTransparent[LayerHandle];
end;

procedure TMap.Set_ShapeLayerStippleTransparent(LayerHandle: Integer; Param2: WordBool);
begin
  DefaultInterface.ShapeLayerStippleTransparent[LayerHandle] := Param2;
end;

function TMap.Get_TrapRMouseDown: WordBool;
begin
    Result := DefaultInterface.TrapRMouseDown;
end;

procedure TMap.Set_TrapRMouseDown(Param1: WordBool);
begin
  DefaultInterface.TrapRMouseDown := Param1;
end;

procedure TMap.Redraw;
begin
  DefaultInterface.Redraw;
end;

function TMap.AddLayer(const Object_: IDispatch; Visible: WordBool): Integer;
begin
  Result := DefaultInterface.AddLayer(Object_, Visible);
end;

procedure TMap.RemoveLayer(LayerHandle: Integer);
begin
  DefaultInterface.RemoveLayer(LayerHandle);
end;

procedure TMap.RemoveLayerWithoutClosing(LayerHandle: Integer);
begin
  DefaultInterface.RemoveLayerWithoutClosing(LayerHandle);
end;

procedure TMap.RemoveAllLayers;
begin
  DefaultInterface.RemoveAllLayers;
end;

function TMap.MoveLayerUp(InitialPosition: Integer): WordBool;
begin
  Result := DefaultInterface.MoveLayerUp(InitialPosition);
end;

function TMap.MoveLayerDown(InitialPosition: Integer): WordBool;
begin
  Result := DefaultInterface.MoveLayerDown(InitialPosition);
end;

function TMap.MoveLayer(InitialPosition: Integer; TargetPosition: Integer): WordBool;
begin
  Result := DefaultInterface.MoveLayer(InitialPosition, TargetPosition);
end;

function TMap.MoveLayerTop(InitialPosition: Integer): WordBool;
begin
  Result := DefaultInterface.MoveLayerTop(InitialPosition);
end;

function TMap.MoveLayerBottom(InitialPosition: Integer): WordBool;
begin
  Result := DefaultInterface.MoveLayerBottom(InitialPosition);
end;

procedure TMap.ZoomToMaxExtents;
begin
  DefaultInterface.ZoomToMaxExtents;
end;

procedure TMap.ZoomToLayer(LayerHandle: Integer);
begin
  DefaultInterface.ZoomToLayer(LayerHandle);
end;

procedure TMap.ZoomToShape(LayerHandle: Integer; Shape: Integer);
begin
  DefaultInterface.ZoomToShape(LayerHandle, Shape);
end;

procedure TMap.ZoomIn(Percent: Double);
begin
  DefaultInterface.ZoomIn(Percent);
end;

procedure TMap.ZoomOut(Percent: Double);
begin
  DefaultInterface.ZoomOut(Percent);
end;

function TMap.ZoomToPrev: Integer;
begin
  Result := DefaultInterface.ZoomToPrev;
end;

procedure TMap.ProjToPixel(projX: Double; projY: Double; var pixelX: Double; var pixelY: Double);
begin
  DefaultInterface.ProjToPixel(projX, projY, pixelX, pixelY);
end;

procedure TMap.PixelToProj(pixelX: Double; pixelY: Double; var projX: Double; var projY: Double);
begin
  DefaultInterface.PixelToProj(pixelX, pixelY, projX, projY);
end;

procedure TMap.ClearDrawing(DrawHandle: Integer);
begin
  DefaultInterface.ClearDrawing(DrawHandle);
end;

procedure TMap.ClearDrawings;
begin
  DefaultInterface.ClearDrawings;
end;

function TMap.SnapShot(const BoundBox: IDispatch): IDispatch;
begin
  Result := DefaultInterface.SnapShot(BoundBox);
end;

function TMap.ApplyLegendColors(const Legend: IDispatch): WordBool;
begin
  Result := DefaultInterface.ApplyLegendColors(Legend);
end;

procedure TMap.LockWindow(LockMode: tkLockMode);
begin
  DefaultInterface.LockWindow(LockMode);
end;

procedure TMap.Resize(Width: Integer; Height: Integer);
begin
  DefaultInterface.Resize(Width, Height);
end;

procedure TMap.ShowToolTip(const Text: WideString; Milliseconds: Integer);
begin
  DefaultInterface.ShowToolTip(Text, Milliseconds);
end;

procedure TMap.AddLabel(LayerHandle: Integer; const Text: WideString; Color: OLE_COLOR; x: Double; 
                        y: Double; hJustification: tkHJustification);
begin
  DefaultInterface.AddLabel(LayerHandle, Text, Color, x, y, hJustification);
end;

procedure TMap.ClearLabels(LayerHandle: Integer);
begin
  DefaultInterface.ClearLabels(LayerHandle);
end;

procedure TMap.LayerFont(LayerHandle: Integer; const FontName: WideString; FontSize: Integer);
begin
  DefaultInterface.LayerFont(LayerHandle, FontName, FontSize);
end;

function TMap.GetColorScheme(LayerHandle: Integer): IDispatch;
begin
  Result := DefaultInterface.GetColorScheme(LayerHandle);
end;

function TMap.NewDrawing(Projection: tkDrawReferenceList): Integer;
begin
  Result := DefaultInterface.NewDrawing(Projection);
end;

procedure TMap.DrawPoint(x: Double; y: Double; pixelSize: Integer; Color: OLE_COLOR);
begin
  DefaultInterface.DrawPoint(x, y, pixelSize, Color);
end;

procedure TMap.DrawLine(x1: Double; y1: Double; x2: Double; y2: Double; pixelWidth: Integer; 
                        Color: OLE_COLOR);
begin
  DefaultInterface.DrawLine(x1, y1, x2, y2, pixelWidth, Color);
end;

procedure TMap.DrawCircle(x: Double; y: Double; pixelRadius: Double; Color: OLE_COLOR; 
                          fill: WordBool);
begin
  DefaultInterface.DrawCircle(x, y, pixelRadius, Color, fill);
end;

procedure TMap.DrawPolygon(var xPoints: OleVariant; var yPoints: OleVariant; numPoints: Integer; 
                           Color: OLE_COLOR; fill: WordBool);
begin
  DefaultInterface.DrawPolygon(xPoints, yPoints, numPoints, Color, fill);
end;

function TMap.SetImageLayerColorScheme(LayerHandle: Integer; const ColorScheme: IDispatch): WordBool;
begin
  Result := DefaultInterface.SetImageLayerColorScheme(LayerHandle, ColorScheme);
end;

procedure TMap.UpdateImage(LayerHandle: Integer);
begin
  DefaultInterface.UpdateImage(LayerHandle);
end;

procedure TMap.AddLabelEx(LayerHandle: Integer; const Text: WideString; Color: OLE_COLOR; 
                          x: Double; y: Double; hJustification: tkHJustification; Rotation: Double);
begin
  DefaultInterface.AddLabelEx(LayerHandle, Text, Color, x, y, hJustification, Rotation);
end;

procedure TMap.GetLayerStandardViewWidth(LayerHandle: Integer; var Width: Double);
begin
  DefaultInterface.GetLayerStandardViewWidth(LayerHandle, Width);
end;

procedure TMap.SetLayerStandardViewWidth(LayerHandle: Integer; Width: Double);
begin
  DefaultInterface.SetLayerStandardViewWidth(LayerHandle, Width);
end;

function TMap.IsTIFFGrid(const Filename: WideString): WordBool;
begin
  Result := DefaultInterface.IsTIFFGrid(Filename);
end;

function TMap.IsSameProjection(const proj4_a: WideString; const proj4_b: WideString): WordBool;
begin
  Result := DefaultInterface.IsSameProjection(proj4_a, proj4_b);
end;

procedure TMap.ZoomToMaxVisibleExtents;
begin
  DefaultInterface.ZoomToMaxVisibleExtents;
end;

function TMap.HWnd: Integer;
begin
  Result := DefaultInterface.HWnd;
end;

function TMap.set_UDPointImageListAdd(LayerHandle: Integer; const newValue: IDispatch): Integer;
begin
  Result := DefaultInterface.set_UDPointImageListAdd(LayerHandle, newValue);
end;

function TMap.get_UDPointImageListCount(LayerHandle: Integer): Integer;
begin
  Result := DefaultInterface.get_UDPointImageListCount(LayerHandle);
end;

function TMap.get_UDPointImageListItem(LayerHandle: Integer; ImageIndex: Integer): IDispatch;
begin
  Result := DefaultInterface.get_UDPointImageListItem(LayerHandle, ImageIndex);
end;

procedure TMap.ClearUDPointImageList(LayerHandle: Integer);
begin
  DefaultInterface.ClearUDPointImageList(LayerHandle);
end;

procedure TMap.DrawLineEx(LayerHandle: Integer; x1: Double; y1: Double; x2: Double; y2: Double; 
                          pixelWidth: Integer; Color: OLE_COLOR);
begin
  DefaultInterface.DrawLineEx(LayerHandle, x1, y1, x2, y2, pixelWidth, Color);
end;

procedure TMap.DrawPointEx(LayerHandle: Integer; x: Double; y: Double; pixelSize: Integer; 
                           Color: OLE_COLOR);
begin
  DefaultInterface.DrawPointEx(LayerHandle, x, y, pixelSize, Color);
end;

procedure TMap.DrawCircleEx(LayerHandle: Integer; x: Double; y: Double; pixelRadius: Double; 
                            Color: OLE_COLOR; fill: WordBool);
begin
  DefaultInterface.DrawCircleEx(LayerHandle, x, y, pixelRadius, Color, fill);
end;

procedure TMap.LabelColor(LayerHandle: Integer; LabelFontColor: OLE_COLOR);
begin
  DefaultInterface.LabelColor(LayerHandle, LabelFontColor);
end;

procedure TMap.SetDrawingLayerVisible(LayerHandle: Integer; Visiable: WordBool);
begin
  DefaultInterface.SetDrawingLayerVisible(LayerHandle, Visiable);
end;

procedure TMap.ClearDrawingLabels(DrawHandle: Integer);
begin
  DefaultInterface.ClearDrawingLabels(DrawHandle);
end;

procedure TMap.DrawingFont(DrawHandle: Integer; const FontName: WideString; FontSize: Integer);
begin
  DefaultInterface.DrawingFont(DrawHandle, FontName, FontSize);
end;

procedure TMap.AddDrawingLabelEx(DrawHandle: Integer; const Text: WideString; Color: OLE_COLOR; 
                                 x: Double; y: Double; hJustification: tkHJustification; 
                                 Rotation: Double);
begin
  DefaultInterface.AddDrawingLabelEx(DrawHandle, Text, Color, x, y, hJustification, Rotation);
end;

procedure TMap.AddDrawingLabel(DrawHandle: Integer; const Text: WideString; Color: OLE_COLOR; 
                               x: Double; y: Double; hJustification: tkHJustification);
begin
  DefaultInterface.AddDrawingLabel(DrawHandle, Text, Color, x, y, hJustification);
end;

procedure TMap.GetDrawingStandardViewWidth(DrawHandle: Integer; var Width: Double);
begin
  DefaultInterface.GetDrawingStandardViewWidth(DrawHandle, Width);
end;

procedure TMap.SetDrawingStandardViewWidth(DrawHandle: Integer; Width: Double);
begin
  DefaultInterface.SetDrawingStandardViewWidth(DrawHandle, Width);
end;

procedure TMap.DrawWidePolygon(var xPoints: OleVariant; var yPoints: OleVariant; 
                               numPoints: Integer; Color: OLE_COLOR; fill: WordBool; Width: Smallint);
begin
  DefaultInterface.DrawWidePolygon(xPoints, yPoints, numPoints, Color, fill, Width);
end;

procedure TMap.DrawWideCircle(x: Double; y: Double; pixelRadius: Double; Color: OLE_COLOR; 
                              fill: WordBool; Width: Smallint);
begin
  DefaultInterface.DrawWideCircle(x, y, pixelRadius, Color, fill, Width);
end;

function TMap.SnapShot2(ClippingLayerNbr: Integer; Zoom: Double; pWidth: Integer): IDispatch;
begin
  Result := DefaultInterface.SnapShot2(ClippingLayerNbr, Zoom, pWidth);
end;

procedure TMap.LayerFontEx(LayerHandle: Integer; const FontName: WideString; FontSize: Integer; 
                           isBold: WordBool; isItalic: WordBool; isUnderline: WordBool);
begin
  DefaultInterface.LayerFontEx(LayerHandle, FontName, FontSize, isBold, isItalic, isUnderline);
end;

procedure TMap.set_UDPointFontCharFont(LayerHandle: Integer; const FontName: WideString; 
                                       FontSize: Single; isBold: WordBool; isItalic: WordBool; 
                                       isUnderline: WordBool);
begin
  DefaultInterface.set_UDPointFontCharFont(LayerHandle, FontName, FontSize, isBold, isItalic, 
                                           isUnderline);
end;

function TMap.set_UDPointFontCharListAdd(LayerHandle: Integer; newValue: Integer; Color: OLE_COLOR): Integer;
begin
  Result := DefaultInterface.set_UDPointFontCharListAdd(LayerHandle, newValue, Color);
end;

procedure TMap.set_UDPointFontCharFontSize(LayerHandle: Integer; FontSize: Single);
begin
  DefaultInterface.set_UDPointFontCharFontSize(LayerHandle, FontSize);
end;

procedure TMap.ReSourceLayer(LayerHandle: Integer; const newSrcPath: WideString);
begin
  DefaultInterface.ReSourceLayer(LayerHandle, newSrcPath);
end;

class function CoShapefileColorScheme.Create: IShapefileColorScheme;
begin
  Result := CreateComObject(CLASS_ShapefileColorScheme) as IShapefileColorScheme;
end;

class function CoShapefileColorScheme.CreateRemote(const MachineName: string): IShapefileColorScheme;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_ShapefileColorScheme) as IShapefileColorScheme;
end;

class function CoShapefileColorBreak.Create: IShapefileColorBreak;
begin
  Result := CreateComObject(CLASS_ShapefileColorBreak) as IShapefileColorBreak;
end;

class function CoShapefileColorBreak.CreateRemote(const MachineName: string): IShapefileColorBreak;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_ShapefileColorBreak) as IShapefileColorBreak;
end;

class function CoGrid.Create: IGrid;
begin
  Result := CreateComObject(CLASS_Grid) as IGrid;
end;

class function CoGrid.CreateRemote(const MachineName: string): IGrid;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Grid) as IGrid;
end;

class function CoGridHeader.Create: IGridHeader;
begin
  Result := CreateComObject(CLASS_GridHeader) as IGridHeader;
end;

class function CoGridHeader.CreateRemote(const MachineName: string): IGridHeader;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_GridHeader) as IGridHeader;
end;

class function CoESRIGridManager.Create: IESRIGridManager;
begin
  Result := CreateComObject(CLASS_ESRIGridManager) as IESRIGridManager;
end;

class function CoESRIGridManager.CreateRemote(const MachineName: string): IESRIGridManager;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_ESRIGridManager) as IESRIGridManager;
end;

class function CoImage.Create: IImage;
begin
  Result := CreateComObject(CLASS_Image) as IImage;
end;

class function CoImage.CreateRemote(const MachineName: string): IImage;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Image) as IImage;
end;

class function CoShapefile.Create: IShapefile;
begin
  Result := CreateComObject(CLASS_Shapefile) as IShapefile;
end;

class function CoShapefile.CreateRemote(const MachineName: string): IShapefile;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Shapefile) as IShapefile;
end;

class function CoShape.Create: IShape;
begin
  Result := CreateComObject(CLASS_Shape) as IShape;
end;

class function CoShape.CreateRemote(const MachineName: string): IShape;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Shape) as IShape;
end;

class function CoExtents.Create: IExtents;
begin
  Result := CreateComObject(CLASS_Extents) as IExtents;
end;

class function CoExtents.CreateRemote(const MachineName: string): IExtents;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Extents) as IExtents;
end;

class function CoPoint.Create: IPoint;
begin
  Result := CreateComObject(CLASS_Point) as IPoint;
end;

class function CoPoint.CreateRemote(const MachineName: string): IPoint;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Point) as IPoint;
end;

class function CoTable.Create: ITable;
begin
  Result := CreateComObject(CLASS_Table) as ITable;
end;

class function CoTable.CreateRemote(const MachineName: string): ITable;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Table) as ITable;
end;

class function CoField.Create: IField;
begin
  Result := CreateComObject(CLASS_Field) as IField;
end;

class function CoField.CreateRemote(const MachineName: string): IField;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Field) as IField;
end;

class function CoShapeNetwork.Create: IShapeNetwork;
begin
  Result := CreateComObject(CLASS_ShapeNetwork) as IShapeNetwork;
end;

class function CoShapeNetwork.CreateRemote(const MachineName: string): IShapeNetwork;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_ShapeNetwork) as IShapeNetwork;
end;

class function CoUtils.Create: IUtils;
begin
  Result := CreateComObject(CLASS_Utils) as IUtils;
end;

class function CoUtils.CreateRemote(const MachineName: string): IUtils;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Utils) as IUtils;
end;

class function CoVector.Create: IVector;
begin
  Result := CreateComObject(CLASS_Vector) as IVector;
end;

class function CoVector.CreateRemote(const MachineName: string): IVector;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Vector) as IVector;
end;

class function CoGridColorScheme.Create: IGridColorScheme;
begin
  Result := CreateComObject(CLASS_GridColorScheme) as IGridColorScheme;
end;

class function CoGridColorScheme.CreateRemote(const MachineName: string): IGridColorScheme;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_GridColorScheme) as IGridColorScheme;
end;

class function CoGridColorBreak.Create: IGridColorBreak;
begin
  Result := CreateComObject(CLASS_GridColorBreak) as IGridColorBreak;
end;

class function CoGridColorBreak.CreateRemote(const MachineName: string): IGridColorBreak;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_GridColorBreak) as IGridColorBreak;
end;

class function CoTin.Create: ITin;
begin
  Result := CreateComObject(CLASS_Tin) as ITin;
end;

class function CoTin.CreateRemote(const MachineName: string): ITin;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Tin) as ITin;
end;

procedure Register;
begin
  RegisterComponents(dtlOcxPage, [TMap]);
end;

end.
