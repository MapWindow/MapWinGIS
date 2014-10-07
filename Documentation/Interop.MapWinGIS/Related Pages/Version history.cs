/// \page version_history Version history
/// 
/// \section apichanges API changes.
/// -  New API in version \link newpage492b 4.9.2b \endlink
/// -  New API in version \link newpage492 4.9.2 \endlink
/// -  New API in version \link newpage491 4.9.1 \endlink
/// -  New API in version \link newpage490 4.9.0 \endlink
/// -  New API in version \link newpage48 4.8 \endlink
/// -  API members that were \link deprecated Deprecated \endlink
/// .
/// 
/// \section changes492 Changes in version 4.9.2.
/// 
/// 1. New zoom bar (AxMap.ShowZoomBar):
/// - displayed when map has a projection; 
/// - depends upon selected tile provider.
/// 
/// 2. Enhanced handling of online tiles:
/// - animation for zooming and panning operations (AxMap.AnimationOnZooming, AxMap.InertiaOnPanning);
/// - in native projection mode snapping to the closest zoom level on all changes of extents;
/// - no scaling in native projection (tile size will be exactly 256 by 256 pixels) and adjustment 
/// as close as possible to original size in non-native projection;
/// - correct rendering of GMercator tiles in WGS84 projection (extrapolation outside world extents);
/// - no duplicate HTTP requests, reduced number of cache searches;
/// - reduced number of available zoom levels when server and map projections don't match to avoid 
/// distortions (GlobalSettings.TilesMinZoomOnProjectionMismatch);
/// 
/// 3. Built-in GDAL overview creation and recognition for grid datasources:
/// - GDAL overviews creation and usage will be chosen as preferable option
/// over creation of proxy image;
/// - proxy images will be used only if they were already created 
/// or if their creation is requested explicitly;
/// - the number of overviews is chosen automatically (GlobalSettings.MinOverviewWidth);
/// - overviews are stored in external .ovr file for original grids and as built-in
/// data for TIFF proxy images;
/// - compression can be specified for TIFF proxy images (GlobalSettings.TiffCompression).
/// 
/// 4. Built-in projection mismatch behaviours (AxMap.ProjectionMismatchBehavior):
/// - affects AxMap.AddLayer and AxMap.AddLayerFromFilename;
/// - if datasource projection doesn't match map projection layer adding operation can be cancelled;
/// - reprojection on the fly is available as an option for shapefiles;
/// - default behaviour is pmbIgnore which is the same as behaviour in previous versions.
/// 
/// 5. New polygon coloring algorithm, which works on assumptions defined in 
/// <a href="http://en.wikipedia.org/wiki/Four_color_theorem">Four color theorem</a>,
/// but uses fast heuristic which can employ somewhat more colors than optimal solution
/// (ShapefileCategories.GeneratePolygonColors).
/// 
/// 6. New <a href="http://hot.openstreetmap.org/">OpenStreetMap Humanitarian</a> tile provider.
/// 
/// \section changes491 Changes in version 4.9.1.
///
/// 1. Enhanced handling of grids:
/// - grids can be passed to AxMap.AddLayer method directly to be displayed on map;
/// - built-in behaviours to open grid datasources (GlobalSettings.GridProxyMode);
/// - GTiff as alternative format for grid proxy rendering (GlobalSettings.GridProxyFormat);
/// - updated API to support grid rendering by %Image class (Image.CustomColorScheme, Image.GridRendering, Image.IsRgb, etc.);
/// - updated API to handle proxy images for grid visualization (Grid.HasValidImageProxy, Grid.CreateImageProxy, etc.)
/// - writing/reading of grid color scheme to/from XML file (GridColorScheme.ReadFromFile, GridColorScheme.WriteToFile);
/// - double overloads for I/O operations: Grid.GetRow2 / Grid.PutRow2, Grid.GetFloatWindow2 / Grid.PutFloatWindow2.
/// 
/// 2. New FileManager class:
/// - encapsulates possible opening strategies of %Image and %Grid datasources, reducing the routine coding needed;
/// - AxMap.AddLayerFromFilename to add data to map with a single line of code by using built AxMap.FileManager instance.
/// 
/// 3. Measuring tool:
/// - available as cmMeasure cursor for AxMap.MapCursor property;
/// - measuring of distance along polyline or area of polygon received from user mouse input;
/// - precise calculations on ellipsoid are performed by <a href="http://geographiclib.sourceforge.net/html/">GeograpicLib</a> if projection for the map is set.
/// 
/// 3. Enhanced scalebar & coordinates display:
/// - 3 scalebar modes including Google styled mode and nicer rendering (AxMap.ScalebarUnits);
/// - scalebar will perform precise calculation on ellipsoid if projection for map is specified;
/// - display of coordinates under mouse cursor (AxMap.ShowCoordinates).
/// 
/// 4. New properties to initialize map control from .NET form designer:
/// - AxMap.Projection, AxMap.CurrentZoom, AxMap.TileProvider, AxMap.Latitude, AxMap.Longitude;
/// - it's possible to set map extents in Form designer and even display tiles for selected provider.
/// 
/// 5. Additional built-in keyboard shortcuts:
/// - "+" = zoom in;
/// - "-" = zoom out;
/// - "*" - zoom to the closest tile level;
/// - "Home" = zoom to combined extents of all data layers (max extents);
/// - "Backspace" = zoom to previous extents; 
/// - "Shift + Left", "Shift + Right" - zoom to the prev/ next layer;
/// - "Z" = ZoomIn tool; 
/// - "M" = measuring tool; 
/// - "Space" = switches to panning mode; after releasing space the previous map cursor is restored.
/// 
/// 6. New functionality to initialize map projection from data layers:
/// - AxMap.GrabProjectionFromData property;
/// - when it is on, projection will also be removed on removal of the last layer from map.
/// 
/// 7. Support of discrete zoom levels defined by tiles providers:
/// - AxMap.ZoomToTileLevel method;
/// - AxMap.ZoomBehavior = zbUseZoomLevels behavior;
/// - AxMap.set_LayerMaxVisibleZoom, AxMap.set_LayerMinVisibleZoom: dynamic visibility of data layers depending on zoom level.
/// 
/// 8. Methods for conversion of map coordinates to decimal degrees (if map projection supports this):
/// - AxMap.PixelToDegrees;
/// - AxMap.DegreesToPixel;
/// - AxMap.ProjToDegrees;
/// - AxMap.DegreesToProj.
/// 
/// 9. tkKnownExtents enumeration for all countries:
/// - AxMap.KnownExtents property (accessible from Form designer);
/// - AxMap.GetKnownExtents method.
/// 
/// 10. Unicode support for filenames in major part of code, although not everywhere yet:
/// - filenames are handled as UTF-16 strings;
/// - filenames are passed to GDAL methods and are serialized to XML as UTF-8 strings.
/// 
/// 11. New API members in GeoProjection class:
/// - GeoProjection.TryAutoDetectEpsg (OGRSpatialReference implementation);
/// - GeoProjection.IsFrozen (to protect AxMap.GeoProjection from changing).
/// 
/// 12. %Shapefile validation:
/// - shapefiles passed as input and output of geoprocessing methods are checked for invalid shapes;
/// - optionally invalid shapes can be fixed and a number of behaviors can be applied if the fixing fails 
/// (GlobalSettings.ShapeInputValidationMode, GlobalSettings.ShapeOutputValidationMode);
/// - new ShapeValidationInfo class to display validation results (Shapefile.LastInputValidation, Shapefile.LastOutputValidation).
/// 
/// 13. Statistic operations on fields of attribute table for shape dissolving functions:
/// - Shapefile.AggregateShapesWithStats, Shapefile.DissolveWithStats;
/// - new FieldStatOperations class to specify fields and operations to calculate for them.
/// 
/// 14. More convenient properties to set visualization category for shape:
/// - by category name (Shapefile.set_ShapeCategory2);
/// - by passing instance of the category itself (Shapefile.set_ShapeCategory3).
/// 
/// 15. New methods in %Shape class:
/// - Shape.ClosestPoints: finds 2 closest points for any pair of shapes;
/// - Shape.ImportFromWKT: imports data from WKT format.





