#pragma warning disable 1587
/// \page version_history Version history
/// 
/// \section apichanges API changes.
/// -  New API in version \link newpage52 5.2 \endlink
/// -  New API in version \link newpage510 5.1 \endlink
/// -  New API in version \link newpage500 5.0 \endlink
/// -  New API in version \link newpage495 4.9.5 \endlink
/// -  New API in version \link newpage494 4.9.4 \endlink
/// -  New API in version \link newpage493 4.9.3 \endlink
/// -  New API in version \link newpage492 4.9.2 \endlink
/// -  New API in version \link newpage491 4.9.1 \endlink
/// -  New API in version \link newpage490 4.9.0 \endlink
/// -  New API in version \link newpage48 4.8 \endlink
/// -  Removed in version \link removed493 4.9.3 \endlink
/// -  API members that were \link deprecated Deprecated \endlink
/// .
/// 
/// \section changes52 Changes in version 5.2.
/// Release notes (24 issues): https://mapwindow.atlassian.net/secure/ReleaseNote.jspa?projectId=10003&version=10605
/// 
/// 1. Compiled with VS2015 and VS2017, thus depending on VC++ 2015 or VC++ 2017 redistributables (included in the installer)
/// 2. Using GDAL 2.4.4, released 2020/01/08
///
/// \section changes51 Changes in version 5.1.
/// Release notes (35 issues): https://mapwindow.atlassian.net/secure/ReleaseNote.jspa?projectId=10003&version=10602
/// 
/// 1. Compiled with VS2015 and VS2017, thus depending on VC++ 2015 or VC++ 2017 redistributables (included in the installer)
/// 2. Using GDAL v2.4.2, released 2019/06/28
/// 
/// \section changes50 Changes in version 5.0.
/// Release notes (21 issues): https://mapwindow.atlassian.net/secure/ReleaseNote.jspa?projectId=10003&version=10400
/// 
/// 1. Compiled with VS2015, thus depending on VC++ 2015 redistributables (included in the installer)
/// 2. Using GDAL v2.3.3
/// 3. Improved UniCode support
/// 4. Enabled HTTPS connections
/// 5. Added some \link newpage500 new geospatial functions \endlink
/// 6. We're now using [semantic versioning](https://semver.org/)
/// 
/// \section changes496 Changes in version 4.9.6.
/// Release notes (11 issues): https://mapwindow.atlassian.net/secure/ReleaseNote.jspa?projectId=10003&version=10600
/// 
/// \section changes495 Changes in version 4.9.5.
/// Release notes (31 issues): https://mapwindow.atlassian.net/secure/ReleaseNote.jspa?projectId=10003&version=10102
/// 
/// \section changes494 Changes in version 4.9.4.
/// Release notes v4.9.4.0 (10 issues): https://mapwindow.atlassian.net/secure/ReleaseNote.jspa?projectId=10003&version=10008
/// Release notes v4.9.4.1 (1 issue): https://mapwindow.atlassian.net/secure/ReleaseNote.jspa?projectId=10003&version=10300
/// Release notes v4.9.4.2 (1 issue): https://mapwindow.atlassian.net/secure/ReleaseNote.jspa?projectId=10003&version=10009
/// 
/// \section changes493 Changes in version 4.9.3.
/// Release notes (12 issues): https://mapwindow.atlassian.net/secure/ReleaseNote.jspa?projectId=10003&version=10007
/// 
/// 1. Added support for variety of vector datasources through GDAL/OGR drivers. 
///     - spatial databases (PostGIS, MS SQL, SpatialLite, etc.)
///     - commonly used vector formats (MapInfo TAB, KML, etc.);
///     - listing of layers in datasource using OgrDatasource class;
///     - adding of OGR layers on the map using OgrLayer class (AxMap.AddLayer); 
///     - SQL queries against datasource (OgrDatasource.RunQuery, OgrDatasource.ExecuteSQL);
///     - editing and saving of changes back to datasource (if particular driver supports it);
///     - saving of style information (either .mwsymb file or as a table in RDMS datasource);
///     - integration with all previously written shapefile-based client code;
/// 
/// 2. Built-in Shape Editor. Supports:
///     - creation and editing of shapes, parts and vertices (cmAddShape and cmEditShape tools);
///     - group operations on the number selected shapes (cmMoveShapes and cmRotateShapes tools);
///     - overlay operations (cmSplitByPolyline, cmSplitByPolygon, cmClipByPolygon, cmEraseByPolygon);
///     - undo/redo capability for all the operations (UndoList class);
///     - snapping to the vertices of existing shapes (ShapeEditor.SnapBehavior);
///     - dynamic display of area, length of segments and their bearing;
///     - user events (AxMap.BeforeShapeEdit, AxMap.ValidateShape, AxMap.ValidationFailed, AxMap.AfterShapeEdit);
///     - ShapeEditor class to control various settings (available via AxMap.ShapeEditor);
/// 
/// 3. .NET Demo application which demonstrates the usage of the latest API:
///     - opening of shapefiles, rasters and OGR vector layers (PostGIS layer and diskbased formats);
///     - complete Shape Editor;
///     - TMS tiles from the range of servers;
///     - geolocation dialog.
/// 
/// 4. Improved selection tool:
///     - selection can be done automatically now (no need to handle SelectBoxFinal event );
///     - a layer to select from can be specified by handling AxMap.ChooseLayer event;
///     - nicer rendering of selection rectangle;
///     - new cmSelectByPolygon tool;
/// 
/// 5. New tool to display shapefile attributes (cmIdentify ):
///     - can highlight shapes under mouse cursor during mouse move operations (fires ShapeHightlighted event);
///     - fires ShapeIdentified event, which can be handled by client code to display attributes;
///     - works either for a selected layer or all layers at once (Identifier.tkIdentifierMode);
/// 
/// 6. Improved API to handle projection mismatch for layers:
///     - AxMap events: LayerProjectionIsEmpty, ProjectionMismatch, LayerReprojected;
///     - GlobalSettings properties: AllowLayersWithoutProjection, AllowProjectionMismatch, ReprojectLayersOnAdding;
///     - decision about adding/rejecting layers can be taken on per-layer basis by handling events;
/// 
/// 7. Improved error reporting:
///     - GlobalSettings.ApplicationCallback property (serves as callback each and every COM class);
///     - reporting of GDAL errors meesages through the application callback;
/// 
/// 8. Major decrease of memory leaking:
///     - thanks of Deleaker tool.
/// 
/// 10. Smaller API changes 
///     - new methods in %Shape class: Shape.SplitByPolyline, Shape.BufferWithParams, Shape.Rotate;
///     - DynamicVisibility for ShapefileCategories (ShapefileCategory.DynamicVisibility);
///     - AxMap.AddLayer, AxMap.RemoveLayer events;
///     - Tiles.SetProxyAuthorization method to set name/password when they are required;
/// 
/// 11. Important fixes for unmanaged IDEs:
///     - division by zero error in Borland C++ Builder;
///     - correct shutdown of GDI+ (affected MFC and VB6);
///     - a fix for MS Access form designer;
/// 
/// 12. Update of tile providers:
///     - methods to set API keys for Bing Maps and Here Maps (GlobalSettings.BingApiKey, GlobalSettings.SetHereMapsApiKey);
///     - free MapQuestAerial provider was added;
///     - Yahoo maps and Topomapper providers were removed (services were discontinued);
///     - Yandex maps providers were removed (license limitations);
/// 
/// \section changes492 Changes in version 4.9.2.
/// 
/// 1. New zoom bar (AxMap.ShowZoomBar):
///     - displayed when map has a projection; 
///     - depends upon selected tile provider.
/// 
/// 2. Enhanced handling of online tiles:
///     - animation for zooming and panning operations (AxMap.AnimationOnZooming, AxMap.InertiaOnPanning);
///     - in native projection mode snapping to the closest zoom level on all changes of extents;
///     - no scaling in native projection (tile size will be exactly 256 by 256 pixels) and adjustment 
/// as close as possible to original size in non-native projection;
///     - correct rendering of GMercator tiles in WGS84 projection (extrapolation outside world extents);
///     - no duplicate HTTP requests, reduced number of cache searches;
///     - reduced number of available zoom levels when server and map projections don't match to avoid 
/// distortions (GlobalSettings.TilesMinZoomOnProjectionMismatch);
/// 
/// 3. Built-in GDAL overview creation and recognition for grid datasources:
///     - GDAL overviews creation and usage will be chosen as preferable option
/// over creation of proxy image;
///     - proxy images will be used only if they were already created 
/// or if their creation is requested explicitly;
///     - the number of overviews is chosen automatically (GlobalSettings.MinOverviewWidth);
///     - overviews are stored in external .ovr file for original grids and as built-in
/// data for TIFF proxy images;
///     - compression can be specified for TIFF proxy images (GlobalSettings.TiffCompression).
/// 
/// 4. Built-in projection mismatch behaviours (AxMap.ProjectionMismatchBehavior):
///     - affects AxMap.AddLayer and AxMap.AddLayerFromFilename;
///     - if datasource projection doesn't match map projection layer adding operation can be cancelled;
///     - reprojection on the fly is available as an option for shapefiles;
///     - default behaviour is pmbIgnore which is the same as behaviour in previous versions.
/// \note Substituted with another set of API members in version 4.9.3.
/// 
/// 5. New polygon coloring algorithm, which works on assumptions defined in 
/// <a href="http://en.wikipedia.org/wiki/Four_color_theorem">Four color theorem</a>,
/// but uses fast heuristic which can employ somewhat more colors than optimal solution
/// (ShapefileCategories.GeneratePolygonColors).
/// 
/// 6. New <a href="https://hot.openstreetmap.org/">OpenStreetMap Humanitarian</a> tile provider.
/// 
/// \section changes491 Changes in version 4.9.1.
///
/// 1. Enhanced handling of grids:
///     - grids can be passed to AxMap.AddLayer method directly to be displayed on map;
///     - built-in behaviours to open grid datasources (GlobalSettings.GridProxyMode);
///     - GTiff as alternative format for grid proxy rendering (GlobalSettings.GridProxyFormat);
///     - updated API to support grid rendering by %Image class (Image.CustomColorScheme, Image.GridRendering, Image.IsRgb, etc.);
///     - updated API to handle proxy images for grid visualization (Grid.HasValidImageProxy, Grid.CreateImageProxy, etc.)
///     - writing/reading of grid color scheme to/from XML file (GridColorScheme.ReadFromFile, GridColorScheme.WriteToFile);
///     - double overloads for I/O operations: Grid.GetRow2 / Grid.PutRow2, Grid.GetFloatWindow2 / Grid.PutFloatWindow2.
/// 
/// 2. New FileManager class:
///     - encapsulates possible opening strategies of %Image and %Grid datasources, reducing the routine coding needed;
///     - AxMap.AddLayerFromFilename to add data to map with a single line of code by using built AxMap.FileManager instance.
/// 
/// 3. Measuring tool:
///     - available as cmMeasure cursor for AxMap.MapCursor property;
///     - measuring of distance along polyline or area of polygon received from user mouse input;
///     - precise calculations on ellipsoid are performed by <a href="http://geographiclib.sourceforge.net/html/">GeograpicLib</a> if projection for the map is set.
/// 
/// 4. Enhanced scalebar & coordinates display:
///     - 3 scalebar modes including Google styled mode and nicer rendering (AxMap.ScalebarUnits);
///     - scalebar will perform precise calculation on ellipsoid if projection for map is specified;
///     - display of coordinates under mouse cursor (AxMap.ShowCoordinates).
/// 
/// 5. New properties to initialize map control from .NET form designer:
///     - AxMap.Projection, AxMap.CurrentZoom, AxMap.TileProvider, AxMap.Latitude, AxMap.Longitude;
///     - it's possible to set map extents in Form designer and even display tiles for selected provider.
/// 
/// 6. Additional built-in keyboard shortcuts:
///     - "+" = zoom in;
///     - "-" = zoom out;
///     - "*" - zoom to the closest tile level;
///     - "Home" = zoom to combined extents of all data layers (max extents);
///     - "Backspace" = zoom to previous extents; 
///     - "Shift + Left", "Shift + Right" - zoom to the prev/ next layer;
///     - "Z" = ZoomIn tool; 
///     - "M" = measuring tool; 
///     - "Space" = switches to panning mode; after releasing space the previous map cursor is restored.
/// 
/// 7. New functionality to initialize map projection from data layers:
///     - AxMap.GrabProjectionFromData property;
///     - when it is on, projection will also be removed on removal of the last layer from map.
/// 
/// 8. Support of discrete zoom levels defined by tiles providers:
///     - AxMap.ZoomToTileLevel method;
///     - AxMap.ZoomBehavior = zbUseZoomLevels behavior;
///     - AxMap.set_LayerMaxVisibleZoom, AxMap.set_LayerMinVisibleZoom: dynamic visibility of data layers depending on zoom level.
/// 
/// 9. Methods for conversion of map coordinates to decimal degrees (if map projection supports this):
///     - AxMap.PixelToDegrees;
///     - AxMap.DegreesToPixel;
///     - AxMap.ProjToDegrees;
///     - AxMap.DegreesToProj.
/// 
/// 10. tkKnownExtents enumeration for all countries:
///     - AxMap.KnownExtents property (accessible from Form designer);
///     - AxMap.GetKnownExtents method.
/// 
/// 11. Unicode support for filenames in major part of code, although not everywhere yet:
///     - filenames are handled as UTF-16 strings;
///     - filenames are passed to GDAL methods and are serialized to XML as UTF-8 strings.
/// 
/// 12. New API members in GeoProjection class:
///     - GeoProjection.TryAutoDetectEpsg (OGRSpatialReference implementation);
///     - GeoProjection.IsFrozen (to protect AxMap.GeoProjection from changing).
/// 
/// 13. %Shapefile validation:
///     - shapefiles passed as input and output of geoprocessing methods are checked for invalid shapes;
///     - optionally invalid shapes can be fixed and a number of behaviors can be applied if the fixing fails 
/// (GlobalSettings.ShapeInputValidationMode, GlobalSettings.ShapeOutputValidationMode);
///     - new ShapeValidationInfo class to display validation results (Shapefile.LastInputValidation, Shapefile.LastOutputValidation).
/// 
/// 14. Statistic operations on fields of attribute table for shape dissolving functions:
///     - Shapefile.AggregateShapesWithStats, Shapefile.DissolveWithStats;
///     - new FieldStatOperations class to specify fields and operations to calculate for them.
/// 
/// 15. More convenient properties to set visualization category for shape:
///     - by category name (Shapefile.set_ShapeCategory2);
///     - by passing instance of the category itself (Shapefile.set_ShapeCategory3).
/// 
/// 16. New methods in %Shape class:
///     - Shape.ClosestPoints: finds 2 closest points for any pair of shapes;
///     - Shape.ImportFromWKT: imports data from WKT format.
#pragma warning restore 1587