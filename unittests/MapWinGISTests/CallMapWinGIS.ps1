# Set-ExecutionPolicy Unrestricted
cls
# Add-Type -AssemblyName AxInterop.MapWinGIS.dll

  $mwChart = New-Object -ComObject MapWinGIS.Chart;
 if (!$mwChart){
  write-host "Cannot get MapWinGIS.Chart" 
 }else{
  write-host "TODO: Test MapWinGIS.Chart functions";
  $mwChart = $null;
}

  $mwChartField = New-Object -ComObject MapWinGIS.ChartField;
 if (!$mwChartField){
  write-host "Cannot get MapWinGIS.ChartField" 
 }else{
  write-host "TODO: Test MapWinGIS.ChartField functions";
  $mwChartField = $null;
}

  $mwCharts = New-Object -ComObject MapWinGIS.Charts;
 if (!$mwCharts){
  write-host "Cannot get MapWinGIS.Charts" 
 }else{
  write-host "TODO: Test MapWinGIS.Charts functions";
  $mwCharts = $null;
}

  $mwColorScheme = New-Object -ComObject MapWinGIS.ColorScheme;
 if (!$mwColorScheme){
  write-host "Cannot get MapWinGIS.ColorScheme" 
 }else{
  write-host "TODO: Test MapWinGIS.ColorScheme functions";
  $mwColorScheme = $null;
}

  $mwDrawingRectangle = New-Object -ComObject MapWinGIS.DrawingRectangle;
 if (!$mwDrawingRectangle){
  write-host "Cannot get MapWinGIS.DrawingRectangle" 
 }else{
  write-host "TODO: Test MapWinGIS.DrawingRectangle functions";
  $mwDrawingRectangle = $null;
}

  $mwESRIGridManager = New-Object -ComObject MapWinGIS.ESRIGridManager;
 if (!$mwESRIGridManager){
  write-host "Cannot get MapWinGIS.ESRIGridManager" 
 }else{
  write-host "TODO: Test MapWinGIS.ESRIGridManager functions";
  $mwESRIGridManager = $null;
}

  $mwExpression = New-Object -ComObject MapWinGIS.Expression;
 if (!$mwExpression){
  write-host "Cannot get MapWinGIS.Expression" 
 }else{
  write-host "TODO: Test MapWinGIS.Expression functions";
  $mwExpression = $null;
}

  $mwExtents = New-Object -ComObject MapWinGIS.Extents;
 if (!$mwExtents){
  write-host "Cannot get MapWinGIS.Extents" 
 }else{
  write-host "TODO: Test MapWinGIS.Extents functions";
  $mwExtents = $null;
}

  $mwField = New-Object -ComObject MapWinGIS.Field;
 if (!$mwField){
  write-host "Cannot get MapWinGIS.Field" 
 }else{
  write-host "TODO: Test MapWinGIS.Field functions";
  $mwField = $null;
}

  $mwFieldStatOperations = New-Object -ComObject MapWinGIS.FieldStatOperations;
 if (!$mwFieldStatOperations){
  write-host "Cannot get MapWinGIS.FieldStatOperations" 
 }else{
  write-host "TODO: Test MapWinGIS.FieldStatOperations functions";
  $mwFieldStatOperations = $null;
}

  $mwFileManager = New-Object -ComObject MapWinGIS.FileManager;
 if (!$mwFileManager){
  write-host "Cannot get MapWinGIS.FileManager" 
 }else{
  write-host "TODO: Test MapWinGIS.FileManager functions";
  $mwFileManager = $null;
}

  $mwFunction_ = New-Object -ComObject MapWinGIS.Function_;
 if (!$mwFunction_){
  write-host "Cannot get MapWinGIS.Function_" 
 }else{
  write-host "TODO: Test MapWinGIS.Function_ functions";
  $mwFunction_ = $null;
}

  $mwGdalDataset = New-Object -ComObject MapWinGIS.GdalDataset;
 if (!$mwGdalDataset){
  write-host "Cannot get MapWinGIS.GdalDataset" 
 }else{
  write-host "TODO: Test MapWinGIS.GdalDataset functions";
  $mwGdalDataset = $null;
}

  $mwGdalDriver = New-Object -ComObject MapWinGIS.GdalDriver;
 if (!$mwGdalDriver){
  write-host "Cannot get MapWinGIS.GdalDriver" 
 }else{
  write-host "TODO: Test MapWinGIS.GdalDriver functions";
  $mwGdalDriver = $null;
}

  $mwGdalDriverManager = New-Object -ComObject MapWinGIS.GdalDriverManager;
 if (!$mwGdalDriverManager){
  write-host "Cannot get MapWinGIS.GdalDriverManager" 
 }else{
  write-host "TODO: Test MapWinGIS.GdalDriverManager functions";
  $mwGdalDriverManager = $null;
}

  $mwGdalRasterBand = New-Object -ComObject MapWinGIS.GdalRasterBand;
 if (!$mwGdalRasterBand){
  write-host "Cannot get MapWinGIS.GdalRasterBand" 
 }else{
  write-host "TODO: Test MapWinGIS.GdalRasterBand functions";
  $mwGdalRasterBand = $null;
}

  $mwGdalUtils = New-Object -ComObject MapWinGIS.GdalUtils;
 if (!$mwGdalUtils){
  write-host "Cannot get MapWinGIS.GdalUtils" 
 }else{
  write-host "TODO: Test MapWinGIS.GdalUtils functions";
  $mwGdalUtils = $null;
}

  $mwGeoProjection = New-Object -ComObject MapWinGIS.GeoProjection;
 if (!$mwGeoProjection){
  write-host "Cannot get MapWinGIS.GeoProjection" 
 }else{
  write-host "TODO: Test MapWinGIS.GeoProjection functions";
  $mwGeoProjection = $null;
}

  $mwGlobalSettings = New-Object -ComObject MapWinGIS.GlobalSettings;
 if (!$mwGlobalSettings){
  write-host "Cannot get MapWinGIS.GlobalSettings" 
 }else{
  write-host "TODO: Test MapWinGIS.GlobalSettings functions";
  $mwGlobalSettings = $null;
}

  $mwGrid = New-Object -ComObject MapWinGIS.Grid;
 if (!$mwGrid){
  write-host "Cannot get MapWinGIS.Grid" 
 }else{
  write-host "TODO: Test MapWinGIS.Grid functions";
  $mwGrid = $null;
}

  $mwGridColorBreak = New-Object -ComObject MapWinGIS.GridColorBreak;
 if (!$mwGridColorBreak){
  write-host "Cannot get MapWinGIS.GridColorBreak" 
 }else{
  write-host "TODO: Test MapWinGIS.GridColorBreak functions";
  $mwGridColorBreak = $null;
}

  $mwGridColorScheme = New-Object -ComObject MapWinGIS.GridColorScheme;
 if (!$mwGridColorScheme){
  write-host "Cannot get MapWinGIS.GridColorScheme" 
 }else{
  write-host "TODO: Test MapWinGIS.GridColorScheme functions";
  $mwGridColorScheme = $null;
}

  $mwGridHeader = New-Object -ComObject MapWinGIS.GridHeader;
 if (!$mwGridHeader){
  write-host "Cannot get MapWinGIS.GridHeader" 
 }else{
  write-host "TODO: Test MapWinGIS.GridHeader functions";
  $mwGridHeader = $null;
}

  $mwHistogram = New-Object -ComObject MapWinGIS.Histogram;
 if (!$mwHistogram){
  write-host "Cannot get MapWinGIS.Histogram" 
 }else{
  write-host "TODO: Test MapWinGIS.Histogram functions";
  $mwHistogram = $null;
}

  $mwIdentifier = New-Object -ComObject MapWinGIS.Identifier;
 if (!$mwIdentifier){
  write-host "Cannot get MapWinGIS.Identifier" 
 }else{
  write-host "TODO: Test MapWinGIS.Identifier functions";
  $mwIdentifier = $null;
}

  $mwImage = New-Object -ComObject MapWinGIS.Image;
 if (!$mwImage){
  write-host "Cannot get MapWinGIS.Image" 
 }else{
  write-host "TODO: Test MapWinGIS.Image functions";
  $mwImage = $null;
}

  $mwLabel = New-Object -ComObject MapWinGIS.Label;
 if (!$mwLabel){
  write-host "Cannot get MapWinGIS.Label" 
 }else{
  write-host "TODO: Test MapWinGIS.Label functions";
  $mwLabel = $null;
}

  $mwLabel_ = New-Object -ComObject MapWinGIS.Label_;
 if (!$mwLabel_){
  write-host "Cannot get MapWinGIS.Label_" 
 }else{
  write-host "TODO: Test MapWinGIS.Label_ functions";
  $mwLabel_ = $null;
}

  $mwLabelCategory = New-Object -ComObject MapWinGIS.LabelCategory;
 if (!$mwLabelCategory){
  write-host "Cannot get MapWinGIS.LabelCategory" 
 }else{
  write-host "TODO: Test MapWinGIS.LabelCategory functions";
  $mwLabelCategory = $null;
}

  $mwLabels = New-Object -ComObject MapWinGIS.Labels;
 if (!$mwLabels){
  write-host "Cannot get MapWinGIS.Labels" 
 }else{
  write-host "TODO: Test MapWinGIS.Labels functions";
  $mwLabels = $null;
}

  $mwLinePattern = New-Object -ComObject MapWinGIS.LinePattern;
 if (!$mwLinePattern){
  write-host "Cannot get MapWinGIS.LinePattern" 
 }else{
  write-host "TODO: Test MapWinGIS.LinePattern functions";
  $mwLinePattern = $null;
}

  $mwLineSegment = New-Object -ComObject MapWinGIS.LineSegment;
 if (!$mwLineSegment){
  write-host "Cannot get MapWinGIS.LineSegment" 
 }else{
  write-host "TODO: Test MapWinGIS.LineSegment functions";
  $mwLineSegment = $null;
}

  $mwMap = New-Object -ComObject MapWinGIS.Map;
 if (!$mwMap){
  write-host "Cannot get MapWinGIS.Map" 
 }else{
  write-host "TODO: Test MapWinGIS.Map functions";
  $mwMap = $null;
}

  $mwMeasuring = New-Object -ComObject MapWinGIS.Measuring;
 if (!$mwMeasuring){
  write-host "Cannot get MapWinGIS.Measuring" 
 }else{
  write-host "TODO: Test MapWinGIS.Measuring functions";
  $mwMeasuring = $null;
}

  $mwOgrDatasource = New-Object -ComObject MapWinGIS.OgrDatasource;
 if (!$mwOgrDatasource){
  write-host "Cannot get MapWinGIS.OgrDatasource" 
 }else{
  write-host "TODO: Test MapWinGIS.OgrDatasource functions";
  $mwOgrDatasource = $null;
}

  $mwOgrLayer = New-Object -ComObject MapWinGIS.OgrLayer;
 if (!$mwOgrLayer){
  write-host "Cannot get MapWinGIS.OgrLayer" 
 }else{
  write-host "TODO: Test MapWinGIS.OgrLayer functions";
  $mwOgrLayer = $null;
}

  $mwPoint = New-Object -ComObject MapWinGIS.Point;
 if (!$mwPoint){
  write-host "Cannot get MapWinGIS.Point" 
 }else{
  write-host "TODO: Test MapWinGIS.Point functions";
  $mwPoint = $null;
}

  $mwSelectionList = New-Object -ComObject MapWinGIS.SelectionList;
 if (!$mwSelectionList){
  write-host "Cannot get MapWinGIS.SelectionList" 
 }else{
  write-host "TODO: Test MapWinGIS.SelectionList functions";
  $mwSelectionList = $null;
}

  $mwShape = New-Object -ComObject MapWinGIS.Shape;
 if (!$mwShape){
  write-host "Cannot get MapWinGIS.Shape" 
 }else{
  write-host "TODO: Test MapWinGIS.Shape functions";
  $mwShape = $null;
}

  $mwShapeDrawingOptions = New-Object -ComObject MapWinGIS.ShapeDrawingOptions;
 if (!$mwShapeDrawingOptions){
  write-host "Cannot get MapWinGIS.ShapeDrawingOptions" 
 }else{
  write-host "TODO: Test MapWinGIS.ShapeDrawingOptions functions";
  $mwShapeDrawingOptions = $null;
}

  $mwShapeEditor = New-Object -ComObject MapWinGIS.ShapeEditor;
 if (!$mwShapeEditor){
  write-host "Cannot get MapWinGIS.ShapeEditor" 
 }else{
  write-host "TODO: Test MapWinGIS.ShapeEditor functions";
  $mwShapeEditor = $null;
}

  $mwShapefile = New-Object -ComObject MapWinGIS.Shapefile;
 if (!$mwShapefile){
  write-host "Cannot get MapWinGIS.Shapefile" 
 }else{
  write-host "TODO: Test MapWinGIS.Shapefile functions";
  $mwShapefile = $null;
}

  $mwShapefileCategories = New-Object -ComObject MapWinGIS.ShapefileCategories;
 if (!$mwShapefileCategories){
  write-host "Cannot get MapWinGIS.ShapefileCategories" 
 }else{
  write-host "TODO: Test MapWinGIS.ShapefileCategories functions";
  $mwShapefileCategories = $null;
}

  $mwShapefileCategory = New-Object -ComObject MapWinGIS.ShapefileCategory;
 if (!$mwShapefileCategory){
  write-host "Cannot get MapWinGIS.ShapefileCategory" 
 }else{
  write-host "TODO: Test MapWinGIS.ShapefileCategory functions";
  $mwShapefileCategory = $null;
}

  $mwShapefileLegend = New-Object -ComObject MapWinGIS.ShapefileLegend;
 if (!$mwShapefileLegend){
  write-host "Cannot get MapWinGIS.ShapefileLegend" 
 }else{
  write-host "TODO: Test MapWinGIS.ShapefileLegend functions";
  $mwShapefileLegend = $null;
}

  $mwShapefileLegendBreak = New-Object -ComObject MapWinGIS.ShapefileLegendBreak;
 if (!$mwShapefileLegendBreak){
  write-host "Cannot get MapWinGIS.ShapefileLegendBreak" 
 }else{
  write-host "TODO: Test MapWinGIS.ShapefileLegendBreak functions";
  $mwShapefileLegendBreak = $null;
}

  $mwShapeNetwork = New-Object -ComObject MapWinGIS.ShapeNetwork;
 if (!$mwShapeNetwork){
  write-host "Cannot get MapWinGIS.ShapeNetwork" 
 }else{
  write-host "TODO: Test MapWinGIS.ShapeNetwork functions";
  $mwShapeNetwork = $null;
}

  $mwShapeValidationInfo = New-Object -ComObject MapWinGIS.ShapeValidationInfo;
 if (!$mwShapeValidationInfo){
  write-host "Cannot get MapWinGIS.ShapeValidationInfo" 
 }else{
  write-host "TODO: Test MapWinGIS.ShapeValidationInfo functions";
  $mwShapeValidationInfo = $null;
}

  $mwTable = New-Object -ComObject MapWinGIS.Table;
 if (!$mwTable){
  write-host "Cannot get MapWinGIS.Table" 
 }else{
  write-host "TODO: Test MapWinGIS.Table functions";
  $mwTable = $null;
}

  $mwTileProviders = New-Object -ComObject MapWinGIS.TileProviders;
 if (!$mwTileProviders){
  write-host "Cannot get MapWinGIS.TileProviders" 
 }else{
  write-host "TODO: Test MapWinGIS.TileProviders functions";
  $mwTileProviders = $null;
}

  $mwTiles = New-Object -ComObject MapWinGIS.Tiles;
 if (!$mwTiles){
  write-host "Cannot get MapWinGIS.Tiles" 
 }else{
  write-host "TODO: Test MapWinGIS.Tiles functions";
  $mwTiles = $null;
}

  $mwTin = New-Object -ComObject MapWinGIS.Tin;
 if (!$mwTin){
  write-host "Cannot get MapWinGIS.Tin" 
 }else{
  write-host "TODO: Test MapWinGIS.Tin functions";
  $mwTin = $null;
}

  $mwUndoList = New-Object -ComObject MapWinGIS.UndoList;
 if (!$mwUndoList){
  write-host "Cannot get MapWinGIS.UndoList" 
 }else{
  write-host "TODO: Test MapWinGIS.UndoList functions";
  $mwUndoList = $null;
}

  $mwUtils = New-Object -ComObject MapWinGIS.Utils;
 if (!$mwUtils){
  write-host "Cannot get MapWinGIS.Utils" 
 }else{
  write-host "TODO: Test MapWinGIS.Utils functions";
  $mwUtils = $null;
}

  $mwVector = New-Object -ComObject MapWinGIS.Vector;
 if (!$mwVector){
  write-host "Cannot get MapWinGIS.Vector" 
 }else{
  write-host "TODO: Test MapWinGIS.Vector functions";
  $mwVector = $null;
}

  $mwWmsLayer = New-Object -ComObject MapWinGIS.WmsLayer;
 if (!$mwWmsLayer){
  write-host "Cannot get MapWinGIS.WmsLayer" 
 }else{
  write-host "TODO: Test MapWinGIS.WmsLayer functions";
  $mwWmsLayer = $null;
}

[gc]::collect()
[gc]::WaitForPendingFinalizers()
