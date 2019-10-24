# Set-ExecutionPolicy Unrestricted
cls

$mwChart = New-Object -ComObject MapWinGIS.Chart;
if (!$mwChart){
 Write-Output "Cannot get MapWinGIS.Chart" 
}else{
 Write-Output "TODO: Test MapWinGIS.Chart functions";
 Write-Output $mwChart;
 $mwChart = $null;
}

$mwChartField = New-Object -ComObject MapWinGIS.ChartField;
if (!$mwChartField){
 Write-Output "Cannot get MapWinGIS.ChartField" 
}else{
 Write-Output "TODO: Test MapWinGIS.ChartField functions";
 Write-Output $mwChartField;
 $mwChartField = $null;
}

$mwCharts = New-Object -ComObject MapWinGIS.Charts;
if (!$mwCharts){
 Write-Output "Cannot get MapWinGIS.Charts" 
}else{
 Write-Output "TODO: Test MapWinGIS.Charts functions";
 Write-Output $mwCharts;
 $mwCharts = $null;
}

$mwColorScheme = New-Object -ComObject MapWinGIS.ColorScheme;
if (!$mwColorScheme){
 Write-Output "Cannot get MapWinGIS.ColorScheme" 
}else{
 Write-Output "TODO: Test MapWinGIS.ColorScheme functions";
 Write-Output $mwColorScheme;
 $mwColorScheme = $null;
}

$mwDrawingRectangle = New-Object -ComObject MapWinGIS.DrawingRectangle;
if (!$mwDrawingRectangle){
 Write-Output "Cannot get MapWinGIS.DrawingRectangle" 
}else{
 Write-Output "TODO: Test MapWinGIS.DrawingRectangle functions";
 Write-Output $mwDrawingRectangle;
 $mwDrawingRectangle = $null;
}

$mwESRIGridManager = New-Object -ComObject MapWinGIS.ESRIGridManager;
if (!$mwESRIGridManager){
 Write-Output "Cannot get MapWinGIS.ESRIGridManager" 
}else{
 Write-Output "TODO: Test MapWinGIS.ESRIGridManager functions";
 # Write-Output $mwESRIGridManager;
 $mwESRIGridManager = $null;
}

$mwExpression = New-Object -ComObject MapWinGIS.Expression;
if (!$mwExpression){
 Write-Output "Cannot get MapWinGIS.Expression" 
}else{
 Write-Output "TODO: Test MapWinGIS.Expression functions";
 Write-Output $mwExpression;
 $mwExpression = $null;
}

$mwExtents = New-Object -ComObject MapWinGIS.Extents;
if (!$mwExtents){
 Write-Output "Cannot get MapWinGIS.Extents" 
}else{
 Write-Output "TODO: Test MapWinGIS.Extents functions";
 Write-Output $mwExtents;
 $mwExtents = $null;
}

$mwField = New-Object -ComObject MapWinGIS.Field;
if (!$mwField){
 Write-Output "Cannot get MapWinGIS.Field" 
}else{
 Write-Output "TODO: Test MapWinGIS.Field functions";
 Write-Output $mwField;
 $mwField = $null;
}

$mwFieldStatOperations = New-Object -ComObject MapWinGIS.FieldStatOperations;
if (!$mwFieldStatOperations){
 Write-Output "Cannot get MapWinGIS.FieldStatOperations" 
}else{
 Write-Output "TODO: Test MapWinGIS.FieldStatOperations functions";
 Write-Output $mwFieldStatOperations;
 $mwFieldStatOperations = $null;
}

$mwFileManager = New-Object -ComObject MapWinGIS.FileManager;
if (!$mwFileManager){
 Write-Output "Cannot get MapWinGIS.FileManager" 
}else{
 Write-Output "TODO: Test MapWinGIS.FileManager functions";
 Write-Output $mwFileManager;
 $mwFileManager = $null;
}

$mwFunction = New-Object -ComObject MapWinGIS.Function;
if (!$mwFunction){
 Write-Output "Cannot get MapWinGIS.Function" 
}else{
 Write-Output "TODO: Test MapWinGIS.Function functions";
 #Write-Output $mwFunction;
 $mwFunction = $null;
}

$mwGdalDataset = New-Object -ComObject MapWinGIS.GdalDataset;
if (!$mwGdalDataset){
 Write-Output "Cannot get MapWinGIS.GdalDataset" 
}else{
 Write-Output "TODO: Test MapWinGIS.GdalDataset functions";
 Write-Output $mwGdalDataset;
 $mwGdalDataset = $null;
}

$mwGdalDriver = New-Object -ComObject MapWinGIS.GdalDriver;
if (!$mwGdalDriver){
 Write-Output "Cannot get MapWinGIS.GdalDriver" 
}else{
 Write-Output "TODO: Test MapWinGIS.GdalDriver functions";
 Write-Output $mwGdalDriver;
 $mwGdalDriver = $null;
}

$mwGdalDriverManager = New-Object -ComObject MapWinGIS.GdalDriverManager;
if (!$mwGdalDriverManager){
 Write-Output "Cannot get MapWinGIS.GdalDriverManager" 
}else{
 Write-Output "TODO: Test MapWinGIS.GdalDriverManager functions";
 Write-Output $mwGdalDriverManager;
 $mwGdalDriverManager = $null;
}

$mwGdalRasterBand = New-Object -ComObject MapWinGIS.GdalRasterBand;
if (!$mwGdalRasterBand){
 Write-Output "Cannot get MapWinGIS.GdalRasterBand" 
}else{
 Write-Output "TODO: Test MapWinGIS.GdalRasterBand functions";
 Write-Output $mwGdalRasterBand;
 $mwGdalRasterBand = $null;
}

$mwGdalUtils = New-Object -ComObject MapWinGIS.GdalUtils;
if (!$mwGdalUtils){
 Write-Output "Cannot get MapWinGIS.GdalUtils" 
}else{
 Write-Output "TODO: Test MapWinGIS.GdalUtils functions";
 Write-Output $mwGdalUtils;
 $mwGdalUtils = $null;
}

$mwGeoProjection = New-Object -ComObject MapWinGIS.GeoProjection;
if (!$mwGeoProjection){
 Write-Output "Cannot get MapWinGIS.GeoProjection" 
}else{
 Write-Output "TODO: Test MapWinGIS.GeoProjection functions";
 Write-Output $mwGeoProjection;
 $mwGeoProjection = $null;
}

$mwGlobalSettings = New-Object -ComObject MapWinGIS.GlobalSettings;
if (!$mwGlobalSettings){
 Write-Output "Cannot get MapWinGIS.GlobalSettings" 
}else{
 Write-Output "TODO: Test MapWinGIS.GlobalSettings functions";
 Write-Output $mwGlobalSettings;
 $mwGlobalSettings = $null;
}

$mwGrid = New-Object -ComObject MapWinGIS.Grid;
if (!$mwGrid){
 Write-Output "Cannot get MapWinGIS.Grid" 
}else{
 Write-Output "TODO: Test MapWinGIS.Grid functions";
 Write-Output $mwGrid;
 $mwGrid = $null;
}

$mwGridColorBreak = New-Object -ComObject MapWinGIS.GridColorBreak;
if (!$mwGridColorBreak){
 Write-Output "Cannot get MapWinGIS.GridColorBreak" 
}else{
 Write-Output "TODO: Test MapWinGIS.GridColorBreak functions";
 Write-Output $mwGridColorBreak;
 $mwGridColorBreak = $null;
}

$mwGridColorScheme = New-Object -ComObject MapWinGIS.GridColorScheme;
if (!$mwGridColorScheme){
 Write-Output "Cannot get MapWinGIS.GridColorScheme" 
}else{
 Write-Output "TODO: Test MapWinGIS.GridColorScheme functions";
 Write-Output $mwGridColorScheme;
 $mwGridColorScheme = $null;
}

$mwGridHeader = New-Object -ComObject MapWinGIS.GridHeader;
if (!$mwGridHeader){
 Write-Output "Cannot get MapWinGIS.GridHeader" 
}else{
 Write-Output "TODO: Test MapWinGIS.GridHeader functions";
 Write-Output $mwGridHeader;
 $mwGridHeader = $null;
}

$mwHistogram = New-Object -ComObject MapWinGIS.Histogram;
if (!$mwHistogram){
 Write-Output "Cannot get MapWinGIS.Histogram" 
}else{
 Write-Output "TODO: Test MapWinGIS.Histogram functions";
 Write-Output $mwHistogram;
 $mwHistogram = $null;
}

$mwIdentifier = New-Object -ComObject MapWinGIS.Identifier;
if (!$mwIdentifier){
 Write-Output "Cannot get MapWinGIS.Identifier" 
}else{
 Write-Output "TODO: Test MapWinGIS.Identifier functions";
 Write-Output $mwIdentifier;
 $mwIdentifier = $null;
}

$mwImage = New-Object -ComObject MapWinGIS.Image;
if (!$mwImage){
 Write-Output "Cannot get MapWinGIS.Image" 
}else{
 Write-Output "TODO: Test MapWinGIS.Image functions";
 Write-Output $mwImage;
 $mwImage = $null;
}

$mwLabel = New-Object -ComObject MapWinGIS.Label;
if (!$mwLabel){
 Write-Output "Cannot get MapWinGIS.Label" 
}else{
 Write-Output "TODO: Test MapWinGIS.Label functions";
 Write-Output $mwLabel;
 $mwLabel = $null;
}

$mwLabelCategory = New-Object -ComObject MapWinGIS.LabelCategory;
if (!$mwLabelCategory){
 Write-Output "Cannot get MapWinGIS.LabelCategory" 
}else{
 Write-Output "TODO: Test MapWinGIS.LabelCategory functions";
 Write-Output $mwLabelCategory;
 $mwLabelCategory = $null;
}

$mwLabels = New-Object -ComObject MapWinGIS.Labels;
if (!$mwLabels){
 Write-Output "Cannot get MapWinGIS.Labels" 
}else{
 Write-Output "TODO: Test MapWinGIS.Labels functions";
 Write-Output $mwLabels;
 $mwLabels = $null;
}

$mwLinePattern = New-Object -ComObject MapWinGIS.LinePattern;
if (!$mwLinePattern){
 Write-Output "Cannot get MapWinGIS.LinePattern" 
}else{
 Write-Output "TODO: Test MapWinGIS.LinePattern functions";
 Write-Output $mwLinePattern;
 $mwLinePattern = $null;
}

$mwLineSegment = New-Object -ComObject MapWinGIS.LineSegment;
if (!$mwLineSegment){
 Write-Output "Cannot get MapWinGIS.LineSegment" 
}else{
 Write-Output "TODO: Test MapWinGIS.LineSegment functions";
 Write-Output $mwLineSegment;
 $mwLineSegment = $null;
}

$mwMap = New-Object -ComObject MapWinGIS.Map.1;
if (!$mwMap){
 Write-Output "Cannot get MapWinGIS.Map.1" 
}else{
 Write-Output "TODO: Test MapWinGIS.Map.1 functions";
 Write-Output $mwMap;
 $mwMap = $null;
}

$mwMeasuring = New-Object -ComObject MapWinGIS.Measuring;
if (!$mwMeasuring){
 Write-Output "Cannot get MapWinGIS.Measuring" 
}else{
 Write-Output "TODO: Test MapWinGIS.Measuring functions";
 Write-Output $mwMeasuring;
 $mwMeasuring = $null;
}

$mwOgrDatasource = New-Object -ComObject MapWinGIS.OgrDatasource;
if (!$mwOgrDatasource){
 Write-Output "Cannot get MapWinGIS.OgrDatasource" 
}else{
 Write-Output "TODO: Test MapWinGIS.OgrDatasource functions";
 Write-Output $mwOgrDatasource;
 $mwOgrDatasource = $null;
}

$mwOgrLayer = New-Object -ComObject MapWinGIS.OgrLayer;
if (!$mwOgrLayer){
 Write-Output "Cannot get MapWinGIS.OgrLayer" 
}else{
 Write-Output "TODO: Test MapWinGIS.OgrLayer functions";
 Write-Output $mwOgrLayer;
 $mwOgrLayer = $null;
}

$mwPoint = New-Object -ComObject MapWinGIS.Point;
if (!$mwPoint){
 Write-Output "Cannot get MapWinGIS.Point" 
}else{
 Write-Output "TODO: Test MapWinGIS.Point functions";
 Write-Output $mwPoint;
 $mwPoint = $null;
}

$mwSelectionList = New-Object -ComObject MapWinGIS.SelectionList;
if (!$mwSelectionList){
 Write-Output "Cannot get MapWinGIS.SelectionList" 
}else{
 Write-Output "TODO: Test MapWinGIS.SelectionList functions";
 Write-Output $mwSelectionList;
 $mwSelectionList = $null;
}

$mwShape = New-Object -ComObject MapWinGIS.Shape;
if (!$mwShape){
 Write-Output "Cannot get MapWinGIS.Shape" 
}else{
 Write-Output "TODO: Test MapWinGIS.Shape functions";
 Write-Output $mwShape;
 $mwShape = $null;
}

$mwShapeDrawingOptions = New-Object -ComObject MapWinGIS.ShapeDrawingOptions;
if (!$mwShapeDrawingOptions){
 Write-Output "Cannot get MapWinGIS.ShapeDrawingOptions" 
}else{
 Write-Output "TODO: Test MapWinGIS.ShapeDrawingOptions functions";
 Write-Output $mwShapeDrawingOptions;
 $mwShapeDrawingOptions = $null;
}

$mwShapeEditor = New-Object -ComObject MapWinGIS.ShapeEditor;
if (!$mwShapeEditor){
 Write-Output "Cannot get MapWinGIS.ShapeEditor" 
}else{
 Write-Output "TODO: Test MapWinGIS.ShapeEditor functions";
 Write-Output $mwShapeEditor;
 $mwShapeEditor = $null;
}

$mwShapefile = New-Object -ComObject MapWinGIS.Shapefile;
if (!$mwShapefile){
 Write-Output "Cannot get MapWinGIS.Shapefile" 
}else{
 Write-Output "TODO: Test MapWinGIS.Shapefile functions";
 Write-Output $mwShapefile;
 $mwShapefile = $null;
}

$mwShapefileCategories = New-Object -ComObject MapWinGIS.ShapefileCategories;
if (!$mwShapefileCategories){
 Write-Output "Cannot get MapWinGIS.ShapefileCategories" 
}else{
 Write-Output "TODO: Test MapWinGIS.ShapefileCategories functions";
 Write-Output $mwShapefileCategories;
 $mwShapefileCategories = $null;
}

$mwShapefileCategory = New-Object -ComObject MapWinGIS.ShapefileCategory;
if (!$mwShapefileCategory){
 Write-Output "Cannot get MapWinGIS.ShapefileCategory" 
}else{
 Write-Output "TODO: Test MapWinGIS.ShapefileCategory functions";
 Write-Output $mwShapefileCategory;
 $mwShapefileCategory = $null;
}

$mwShapeNetwork = New-Object -ComObject MapWinGIS.ShapeNetwork;
if (!$mwShapeNetwork){
 Write-Output "Cannot get MapWinGIS.ShapeNetwork" 
}else{
 Write-Output "TODO: Test MapWinGIS.ShapeNetwork functions";
 Write-Output $mwShapeNetwork;
 $mwShapeNetwork = $null;
}

$mwShapeValidationInfo = New-Object -ComObject MapWinGIS.ShapeValidationInfo;
if (!$mwShapeValidationInfo){
 Write-Output "Cannot get MapWinGIS.ShapeValidationInfo" 
}else{
 Write-Output "TODO: Test MapWinGIS.ShapeValidationInfo functions";
 Write-Output $mwShapeValidationInfo;
 $mwShapeValidationInfo = $null;
}

$mwTable = New-Object -ComObject MapWinGIS.Table;
if (!$mwTable){
 Write-Output "Cannot get MapWinGIS.Table" 
}else{
 Write-Output "TODO: Test MapWinGIS.Table functions";
 #Write-Output $mwTable;
 $mwTable = $null;
}

$mwTileProviders = New-Object -ComObject MapWinGIS.TileProviders;
if (!$mwTileProviders){
 Write-Output "Cannot get MapWinGIS.TileProviders" 
}else{
 Write-Output "TODO: Test MapWinGIS.TileProviders functions";
 Write-Output $mwTileProviders;
 $mwTileProviders = $null;
}

$mwTiles = New-Object -ComObject MapWinGIS.Tiles;
if (!$mwTiles){
 Write-Output "Cannot get MapWinGIS.Tiles" 
}else{
 Write-Output "TODO: Test MapWinGIS.Tiles functions";
 Write-Output $mwTiles;
 $mwTiles = $null;
}

$mwTin = New-Object -ComObject MapWinGIS.Tin;
if (!$mwTin){
 Write-Output "Cannot get MapWinGIS.Tin" 
}else{
 Write-Output "TODO: Test MapWinGIS.Tin functions";
 Write-Output $mwTin;
 $mwTin = $null;
}

$mwUndoList = New-Object -ComObject MapWinGIS.UndoList;
if (!$mwUndoList){
 Write-Output "Cannot get MapWinGIS.UndoList" 
}else{
 Write-Output "TODO: Test MapWinGIS.UndoList functions";
 #Write-Output $mwUndoList;
 $mwUndoList = $null;
}

$mwUtils = New-Object -ComObject MapWinGIS.Utils;
if (!$mwUtils){
 Write-Output "Cannot get MapWinGIS.Utils" 
}else{
 Write-Output "TODO: Test MapWinGIS.Utils functions";
 Write-Output $mwUtils;
 $mwUtils = $null;
}

$mwVector = New-Object -ComObject MapWinGIS.Vector;
if (!$mwVector){
 Write-Output "Cannot get MapWinGIS.Vector" 
}else{
 Write-Output "TODO: Test MapWinGIS.Vector functions";
 Write-Output $mwVector;
 $mwVector = $null;
}

$mwWmsLayer = New-Object -ComObject MapWinGIS.WmsLayer;
if (!$mwWmsLayer){
 Write-Output "Cannot get MapWinGIS.WmsLayer" 
}else{
 Write-Output "TODO: Test MapWinGIS.WmsLayer functions";
 Write-Output $mwWmsLayer;
 $mwWmsLayer = $null;
}

[gc]::collect()
[gc]::WaitForPendingFinalizers()
