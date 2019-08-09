// ReSharper disable UnassignedGetOnlyAutoProperty
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    /// <summary>
    /// A utils object provides access to a set of utility functions to perform a variety of tasks on other objects such as grids, images, points, shapes, shapefiles, tins, etc.  
    /// Starting at v4.9.5 some specific GDAL methods are moved to GdalUtils.
    /// </summary>
#if nsp
#if upd
    public class Utils : MapWinGIS.IUtils
#else
    public class IUtils
#endif
#else
        public class Utils
#endif
    {
        /// <summary>
        /// Modifies a polygon using the specified method. 
        /// </summary>
        /// <param name="op">The operation to use on the subject polygon.</param>
        /// <param name="SubjectPolygon">The first polygon to perform the specified polygon operation on.</param>
        /// <param name="ClipPolygon">The second polygon to perform the specified polygon operation with.</param>
        /// <returns>The polygon shape created using the specified polygon operation.</returns>
        public Shape ClipPolygon(PolygonOperation op, Shape SubjectPolygon, Shape ClipPolygon)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the numeric representation for the specified color.
        /// </summary>
        /// <remarks>The return value can be used for all properties which require color specification as unsigned integer.</remarks>
        /// <param name="Name">The name of the color.</param>
        /// <returns>The numeric code of the color.</returns>
        /// \new48 Added in version 4.8
        public uint ColorByName(tkMapColor Name)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Generates a hillshade image for a raster data source. 
        /// </summary>
        /// <remarks>This code was written by Matt Perry, perrygeo@gmail.com, published in Gdal-dev Digest, Vol 19, Issue 20 
        /// Note: Scale for Feet/Latlong use scale=370400, for Meters/LatLong use scale=111120 (based on the GRASS GIS algorithm 
        /// from r.shaded.relief) </remarks>
        /// <param name="bstrGridFilename">The input raster data. This can be in virtually any raster data format.</param>
        /// <param name="bstrShadeFilename">The output image filename. This can be virtually any image format, the extension will determine the format. </param>
        /// <param name="Z">Z Factor. Defaults to 1.</param>
        /// <param name="scale">Scale Factor. Defaults to 1.</param>
        /// <param name="az">Azimuth. Defaults to 315.</param>
        /// <param name="alt">Altitude. Defaults to 45.</param>
        /// <returns></returns>
        public bool GenerateHillShade(string bstrGridFilename, string bstrShadeFilename, float Z, float scale, float az, float alt)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The global callback is the interface used by MapWinGIS to pass progress and error events to interested applications.
        /// </summary>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Uses valid data in a grid to replace grid cells containing no-data values with an interpolated value. 
        /// </summary>
        /// <param name="Grd">The grid for which no-data cells will be replaced with interpolated values. </param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages. </param>
        /// <returns>A boolean value representing the success or failure of replacing no-data cells in the grid with interpolated values.</returns>
        public bool GridInterpolateNoData(Grid Grd, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Merges multiple grids into a single output grid. 
        /// </summary>
        /// <param name="Grids">An array of grid objects to be merged into one grid.</param>
        /// <param name="MergeFilename">The filename to use for the new merged grid. </param>
        /// <param name="InRam">Optional. A boolean value representing whether to create the merged grid in RAM or on disk</param>
        /// <param name="GrdFileType">Optional. The file type of the new merged grid. </param>
        /// <param name="cBack">Optional. The ICallback object which will receive progress and error messages while the grids are being merged.</param>
        /// <returns>The new merged grid.</returns>
        public Grid GridMerge(object Grids, string MergeFilename, bool InRam, GridFileType GrdFileType, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Replaces all occurrences of a value in the grid with a new value. 
        /// </summary>
        /// <param name="Grd">The grid to replace the specified value in.</param>
        /// <param name="OldValue">The value in the grid to be replaced with the new value.</param>
        /// <param name="newValue">The value to replace the old values with.</param>
        /// <param name="cBack">The ICallback object which will receive progress and error mesages while 
        /// the old value is being replaced with the new values in the grid.</param>
        /// <returns>A boolean value representing the success or failure of replacing the old value with the new value 
        /// in the grid.</returns>
        public bool GridReplace(Grid Grd, object OldValue, object newValue, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new grid of the same size as the original grid, converting the values of the original grid into a different data type in 
        /// the new grid.
        /// </summary>
        /// <param name="Grid">The original grid.</param>
        /// <param name="OutDataType">The data type to convert the original grid values to for the new grid values.</param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while 
        /// the original grid values are converted and copied into the new grid.</param>
        /// <returns>The new grid filled with the converted values of the original grid.</returns>
        public Grid GridToGrid(Grid Grid, GridDataType OutDataType, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new shapefile from the grid. Note: It is important that the flow grid is used when there are any ambiguous parts of 
        /// the grid.
        /// </summary>
        /// <param name="Grid">The grid to be converted to a shapefile.</param>
        /// <param name="ConnectionGrid">Optional. The flow grid that is used to resolve any ambiguity. Without this flow grid the output 
        /// results are somewhat unpredictable.</param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while 
        /// the grid is being converted to the shapefile.</param>
        /// <returns></returns>
        public Shapefile GridToShapefile(Grid Grid, Grid ConnectionGrid, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The key may be used by the programmer to store any string data associated with the object.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Retrieves the last error generated in the object. 
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Merges several images in a single image.
        /// </summary>
        /// <remarks>All the input images must be of the same size. 
        /// The color marked in the input image as transparent won't be passed to the output.</remarks>
        /// <param name="InputNames">An array of string type with the filenames of the images.</param>
        /// <param name="OutputName">The name if te output file.</param>
        /// <returns></returns>
        /// \include MergeImages.cs
        /// \new48 Added in version 4.8
        public bool MergeImages(Array InputNames, string OutputName)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Partially implemented. Creates a shapefile from any vector format supported by OGR library.
        /// </summary>
        /// <remarks>The method needs testing and elaboration to handle the peculiarities of various data formats.</remarks>
        /// <param name="Filename">The name of the vector data source.</param>
        /// <param name="ShpType">The type of the output shapefile.</param>
        /// <param name="cBack">The callback object to return information about progress and errors.</param>
        /// <returns>The reference to the resulting shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile OGRLayerToShapefile(string Filename, ShpfileType ShpType, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets whether or not a point lies within the specified polygon shape.
        /// </summary>
        /// <remarks>There is a faster option available when working with shapefiles: PointInShape.</remarks>
        /// <param name="Shp">The polygon shape to perform the test on.</param>
        /// <param name="TestPoint">The point to test whether or not it lies within the specified polygon shape.</param>
        /// <returns>A boolean value representing whether or not the point lies within the shape.</returns>
        public bool PointInPolygon(Shape Shp, Point TestPoint)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes colinear points from a shapefile. 
        /// </summary>
        /// <remarks>A shapefile containing colinear points leads to the shapefile taking up 
        /// unnecessary space since some points in the shapefile add detail to the display of the shapes. </remarks>
        /// <param name="Shapes">The shapefile to remove colinear points from.</param>
        /// <param name="LinearTolerance">Points will be considered colinear if the distance in between them is with in this tolerance. This 
        /// distance is measured in projected map coordinates. </param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while
        /// colinear points are being removed.</param>
        /// <returns>A boolean value representing the success or failure of removing colinear points from the shapefile.</returns>
        public bool RemoveColinearPoints(Shapefile Shapes, double LinearTolerance, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Reprojects a shapefile.
        /// </summary>
        /// <param name="sf">The input shapefile.</param>
        /// <param name="source">The source projection.</param>
        /// <param name="target">The target projection.</param>
        /// <returns>The copy of shapefile with new projection or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile ReprojectShapefile(Shapefile sf, GeoProjection source, GeoProjection target)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Merges two shapes in a shapefile to create a new shape.
        /// </summary>
        /// <param name="Shapes">The shapefile containing the two shapes to be merged to create the new shape. </param>
        /// <param name="IndexOne">The index of the first shape to be merged. </param>
        /// <param name="IndexTwo">The index of the second shape to be merged.</param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while
        /// the shapes are being merged.</param>
        /// <returns>The resulting shape.</returns>
        public Shape ShapeMerge(Shapefile Shapes, int IndexOne, int IndexTwo, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new shapefile with z values added from an elevation grid.
        /// </summary>
        /// <param name="Shapefile">The shapefile to be converted to a new shapefile with z values. </param>
        /// <param name="Grid">The elevation grid to get the z values from. </param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while
        /// z values are being added to the shapefile.</param>
        /// <returns>The new shapefile containing z values obtained from the elevation grid.</returns>
        public Shapefile ShapeToShapeZ(Shapefile Shapefile, Grid Grid, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This function is not implemented. Converts a shapefile to a grid.
        /// </summary>
        /// <param name="Shpfile">The shapefile to be converted into a grid.  </param>
        /// <param name="UseShapefileBounds">Optional. A boolean value representing whether or not the grid will have the same extents 
        /// as the shapefile. The default is True</param>
        /// <param name="GrdHeader">Optional. The grid header to use to create the new grid.
        /// If UseShapefileBounds is set to true, the extents supplied in the grid header will be ignored
        /// when the grid is created.</param>
        /// <param name="Cellsize">Optional. The cell size of the new grid. The default cell size is 30. </param>
        /// <param name="UseShapeNumber">Optional. Specifies that the value of each cell in the grid should be the shape index. The default value is true.</param>
        /// <param name="SingleValue">Optional. The value to use when creating the grid. This only applies when
        /// UseShapeNumber is set to false. The default value is 1.</param>
        /// <returns>The grid created from the shapefile. </returns>
        public Grid ShapefileToGrid(Shapefile Shpfile, bool UseShapefileBounds, GridHeader GrdHeader, double Cellsize, bool UseShapeNumber, short SingleValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a shapefile from a TIN object. 
        /// </summary>
        /// <param name="Tin">The TIN object to be used to create a new shapefile.</param>
        /// <param name="Type">The type of the shapefile to be created.</param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while
        /// shapefile is being created from the TIN.</param>
        /// <returns>The new shapefile created from the TIN.</returns>
        public Shapefile TinToShapefile(Tin Tin, ShpfileType Type, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the area of the polygon shape. For multi-part polygons which may contain counter-clockwise holes, the area of the 
        /// holes will be subtracted from that of the surrounding clockwise portions. 
        /// </summary>
        /// <param name="Shape">The polygon shape for which the area is required.</param>
        /// <returns>The area of the polygon shape.</returns>
        public double get_Area(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Retrieves the error message associated with the specified error code. 
        /// </summary>
        /// <param name="ErrorCode">The error code for which the error message is required.</param>
        /// <returns>The error message description for the specified error code. </returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the length of the line shape.
        /// </summary>
        /// <param name="Shape"></param>
        /// <returns></returns>
        public double get_Length(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the perimeter of the polygon shape. 
        /// </summary>
        /// <param name="Shape">The polygon shape for which the perimeter is required. </param>
        /// <returns>The perimeter of the polygon shape. </returns>
        public double get_Perimeter(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Converts an hBitmap to an IPictureDisp object.
        /// </summary>
        /// <param name="hBitmap">A device context handle to a bitmap. </param>
        /// <returns>An IPictureDisp object created from the bitmap.</returns>
        public stdole.IPictureDisp hBitmapToPicture(int hBitmap)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Converts raster data between different formats, potentially performing some operations like subsettings, resampling, and rescaling pixels in the process.
        /// </summary>
        /// <remarks>See the description of appropriate routine in GDAL Tools.</remarks>
        /// <param name="bstrSrcFilename">The name of the source file.</param>
        /// <param name="bstrDstFilename">The name of the destination file.</param>
        /// <param name="bstrOptions">Options of the routine.</param>
        /// <param name="cBack">The callback object.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \deprecated v5.1.0 Use GdalUtils.GdalRasterTranslate instead.
        [Obsolete("Use GdalUtils.GdalRasterTranslate")]
        public bool TranslateRaster(string bstrSrcFilename, string bstrDstFilename, string bstrOptions, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This program generates a vector contour file from the input raster elevation model (DEM). 
        /// </summary>
        /// <remarks>See documentation here: http://www.gdal.org/gdal_contour.html</remarks>
        public bool GenerateContour(string bstrSrcFilename, string bstrDstFilename, double dfInterval, double dfNoData, bool Is3D, object dblFLArray, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new grid from the input one in which all values outside the specified polygon are set to no data value.
        /// </summary>
        /// <param name="inputGridfile">Filename of input grid.</param>
        /// <param name="poly">Clipping polygon.</param>
        /// <param name="resultGridfile">Filename of the results grid.</param>
        /// <param name="keepExtents">True in case grid must not be clipped using polygon extents.</param>
        /// <returns>True on success.</returns>
        /// \new490 Added in version 4.9.0
        public bool ClipGridWithPolygon(string inputGridfile, Shape poly, string resultGridfile, bool keepExtents)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new grid from the input one in which all values outside the specified polygon are set to no data value.
        /// </summary>
        /// <param name="inputGrid">INput grid.</param>
        /// <param name="poly">Clipping polygon.</param>
        /// <param name="resultGridfile">Filename of the results grid.</param>
        /// <param name="keepExtents">True in case grid must not be clipped using polygon extents.</param>
        /// <returns>True on success.</returns>
        /// \new490 Added in version 4.9.0
        public bool ClipGridWithPolygon2(Grid inputGrid, Shape poly, string resultGridfile, bool keepExtents)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Converts distance from one units of measuring to another.
        /// </summary>
        /// <param name="sourceUnit">Source units of measuring.</param>
        /// <param name="targetUnit">Target units of measuring.</param>
        /// <param name="Value">The value in source units to be converted.</param>
        /// <returns>The value in target units.</returns>
        /// \new490 Added in version 4.9.0
        public bool ConvertDistance(tkUnitsOfMeasure sourceUnit, tkUnitsOfMeasure targetUnit, ref double Value)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Copies no data values from one grid datasource to another.
        /// </summary>
        /// <param name="sourceFilename">Filename of the source.</param>
        /// <param name="destFilename">Filename of the target.</param>
        /// <returns>True on success.</returns>
        /// <remarks>Datasources must have the same size, number of bands, byte data type and must both be supported by GDAL.</remarks>
        /// \new491 Added in version 4.9.1
        public bool CopyNodataValues(string sourceFilename, string destFilename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates an instance of specified COM CoClass implementing specified interface.
        /// </summary>
        /// <param name="interfaceId">Interface to be created.</param>
        /// <returns>Requested interface.</returns>
        /// <remarks>This method might be slightly faster than direct object 
        /// creation when mass creation of objects is needed./remarks>
        /// \new490 Added in version 4.9.0
        public object CreateInstance(tkInterface interfaceId)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The same as GDAL's gdaladdo.exe utility. 
        /// </summary>
        /// <remarks>See documentation here: http://www.gdal.org/gdaladdo.html</remarks>
        /// \new490 Added in version 4.9.0
        /// \deprecated v5.1.0 Use GdalUtils.GdalBuildOverviews instead.
        [Obsolete("Use GdalUtils.GdalBuildOverviews")]
        public bool GDALAddOverviews(string bstrSrcFilename, string bstrOptions, string bstrLevels, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The same as GDAL's gdalbuildvert.exe utility.
        /// </summary>
        /// <remarks>See documentation here: http://www.gdal.org/gdalbuildvert.html</remarks>
        /// \new490 Added in version 4.9.0
        public bool GDALBuildVrt(string bstrDstFilename, string bstrOptions, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The same as GDAL's gdalinfo.exe utility.
        /// </summary>
        /// <remarks>See documentation here: http://www.gdal.org/gdalinfo.html</remarks>
        /// \new490 Added in version 4.9.0
        public string GDALInfo(string bstrSrcFilename, string bstrOptions, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The same as GDAL's gdal_rasterize.exe utility.
        /// </summary>
        /// <remarks>See documentation here: http://www.gdal.org/gdal_rasterize.html</remarks>
        /// \new490 Added in version 4.9.0
        public bool GDALRasterize(string bstrSrcFilename, string bstrDstFilename, string bstrOptions, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The same as GDAL's gdalwarp.exe utility
        /// </summary>
        /// <remarks>See documentation here: http://www.gdal.org/gdalwarp.html</remarks>
        /// \new490 Added in version 4.9.0
        /// \deprecated v4.9.5 Use GdalUtils.GdalRasterWarp instead.
        [Obsolete("Use GdalUtils.GdalRasterWarp")]
        public bool GDALWarp(string bstrSrcFilename, string bstrDstFilename, string bstrOptions, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates geodesic distance between 2 points on Earth.
        /// </summary>
        /// <param name="lat1">Latitude of the first point in decimal degrees.</param>
        /// <param name="lng1">Longitude of the first point in decimal degrees.</param>
        /// <param name="lat2">Latitude of the second point in decimal degrees.</param>
        /// <param name="lng2">Longitude of the second point in decimal degrees.</param>
        /// <returns>Geodesic distance between 2 points in meters.</returns>
        /// \new490 Added in version 4.9.0
        public double GeodesicDistance(double lat1, double lng1, double lat2, double lng2)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        ///  Calculates statistics for portion of grid that is overlapped by specified polygon.
        /// </summary>
        /// <param name="Grid">Input grid</param>
        /// <param name="Header">Grid header. Can be retrieved with Grid.Header property.</param>
        /// <param name="gridExtents">Grid extents. Can be retrieved with Grid.Extents property.</param>
        /// <param name="Shape">Polygon shape within borders of which the statistics will be calculated.</param>
        /// <param name="NodataValue">No data value for grid. Can be retrieved from header.</param>
        /// <param name="MeanValue">Calculated mean value.</param>
        /// <param name="MinValue">Calculated min value.</param>
        /// <param name="MaxValue">Calculated max value</param>
        /// <returns>True on success.</returns>
        /// <remarks>Header, gridExtents, NodataValue parameters are added, so that these properties can be retrieved
        /// only ones in case the function is called multiple times for each polygon in shapefile.</remarks>
        /// \new490 Added in version 4.9.0
        public bool GridStatisticsForPolygon(Grid Grid, GridHeader Header, Extents gridExtents, Shape Shape, double NodataValue, ref double MeanValue, ref double MinValue, ref double MaxValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates statistics for grid cells within each polygon of the specified shapefile. Writes results to attribute table of shapefile.
        /// </summary>
        /// <param name="Grid">Input grid.</param>
        /// <param name="sf">Polygon shapefile.</param>
        /// <param name="SelectedOnly">A value indicating whether calculation will be made for selected polygons only. 
        /// When set to false, calculation will be made for all polygons in shapefile.</param>
        /// <param name="overwriteFields">If true output fields will be overwritten (if the fields with such names already exist). 
        /// Otherwise new set of output fields will be created with numerical indices added to their names.</param>
        /// <param name="useCenterWithinMethod">If true (default) the center of the pixel needs to be inside the polygon. 
        /// If set to false Intersection method is used meaning each pixel boundary that intersects with the polygon is used. Added in v4.9.4.3.</param>
        /// <returns>True on success.</returns>
        /// <remarks>The default names for output fields: "Mean", "Median", "Majority", "Minority", "Minimum", "Maximum", 
        /// "Range", "StD", "Sum", "MinX", "MinY", "Variety", "Count"</remarks>
        /// \new490 Added in version 4.9.0
        public bool GridStatisticsToShapefile(Grid Grid, Shapefile sf, bool SelectedOnly, bool overwriteFields, bool useCenterWithinMethod = true)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates an image from the grid using the given grid color scheme.
        /// </summary>
        /// <param name="Grid">The grid to use to create the image. </param>
        /// <param name="cScheme">The grid color scheme to use to create an image from the grid.</param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while 
        /// the image is being created from the grid.</param>
        /// <returns>The image created from the grid.</returns>
        public Image GridToImage(Grid Grid, GridColorScheme cScheme, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates an image proxy for grid visualization.
        /// </summary>
        /// <param name="Grid">Grid to create proxy for.</param>
        /// <param name="ci">Color scheme which defines the mapping of values to colors.</param>
        /// <param name="imageFormat">The format of output image.</param>
        /// <param name="InRam">True in case in-memory image should be created, otherwise it will saved to the drive.</param>
        /// <param name="cBack">Callback interface.</param>
        /// <returns>Resulting image.</returns>
        /// <remarks>The filename of the resulting image (inRam = false) is set automatically: for example, grid.asc -> grid_proxy.bmp.
        /// In case of multi-band grids an image will be created for the active band, determined from Grid.ActiveBandIndex property.
        /// For bmp output format a world file will be written along with resulting file. For GTiff format the same values
        /// will be embedded in the file itself.</remarks>
        /// \new491 Added in version 4.9.1
        public Image GridToImage2(Grid Grid, GridColorScheme ci, tkGridProxyFormat imageFormat, bool InRam, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets all the values inside datasource, which aren't equal to no data value, to a new value.
        /// </summary>
        /// <param name="Filename">Filename of the input datasource.</param>
        /// <param name="newPerBandValue">The new value set for all cells of each band which aren't equal to no data value.</param>
        /// <returns>True on success.</returns>
        /// <remarks>Datasource format must be supported by GDAL. Byte data type for values is expected.</remarks>
        /// \new490 Added in version 4.9.0
        public bool MaskRaster(string Filename, byte newPerBandValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The same as GDAL's ogr2ogr utility.
        /// </summary>
        /// <remarks>See documentation here: http://www.gdal.org/ogr2ogr.html</remarks>
        /// \new490 Added in version 4.9.0
        /// \deprecated v4.9.5 Use GdalUtils.GdalVectorTranslate instead.
        [Obsolete("Use GdalUtils.GdalVectorTranslate")]
        public bool OGR2OGR(string bstrSrcFilename, string bstrDstFilename, string bstrOptions, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The same as GDAL's OGRInfo utility.
        /// </summary>
        /// <remarks>See documentation here: http://www.gdal.org/ogrinfo.html</remarks>
        /// \new490 Added in version 4.9.0
        public string OGRInfo(string bstrSrcFilename, string bstrOptions, string bstrLayers = null, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The same as GDAL's gdal_polygonize utility.
        /// </summary>
        /// <remarks>See documentation here: http://www.gdal.org/gdal_polygonize.html</remarks>
        /// \new490 Added in version 4.9.0
        public bool Polygonize(string pszSrcFilename, string pszDstFilename, int iSrcBand = 1, bool NoMask = false, string pszMaskFilename = "0", string pszOGRFormat = "GML", string pszDstLayerName = "out", string pszPixValFieldName = "DN", ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Displays error message for the last error that took place within object passed as parameter.
        /// </summary>
        /// <param name="comClass">Any MapWinGIS COM class which has LastErrorCode and get_ErrorMsg properties.</param>
        /// <remarks>The methods is alternative syntax of calling: 
        /// \code
        /// shapefile.get_ErrorMsg(shapefile.LastErrorCode);
        /// \endcode
        /// Now it also can be done like:
        /// \code
        /// utils.ErrorMsgFromObject(shapefile);
        /// \endcode
        /// </remarks>
        /// <returns>Error description.</returns>
        /// \new491 Added in version 4.9.1
        public string ErrorMsgFromObject(object comClass)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Initializes GeoProjection object with projection used by specific tile service.
        /// </summary>
        /// <param name="Projection">Projection used by tile service.</param>
        /// <returns>GeoProjection object or null if operation failed.</returns>
        /// \new491 Added in version 4.9.1
        public GeoProjection TileProjectionToGeoProjection(tkTileProjection Projection)
        {
            throw new NotImplementedException();
        }


        /// <summary>
        /// Gets report about COM objects that were created and released.
        /// </summary>
        /// <param name="unreleasedOnly">Indicates whether report must include information about already released objects.</param>
        /// <returns>String with report.</returns>
        /// <remarks>
        /// This method can be used to find memory leaking in MapWinGIS native code 
        /// (forgotten object->Release call). The most vulnerable spots are geoprocessing functions,
        /// where thousands of Shape or Point objects may be created in a single API call.
        /// Before calling this make sure that:
        /// - objects are no longer referenced anywhere in your code;
        /// - in case of .NET, that garbage collection was performed.
        /// \note AxMap control uses several instances of COM objects (GeoProjection, Shape) as a part of normal functioning.
        /// This doesn't amount to memory leakage.
        /// </remarks>
        /// \new492 Added in version 4.9.2
        public string get_ComUsageReport(bool unreleasedOnly)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new raster dataset based on the array of input datasets and an arbitrary formula 
        /// to calculate values of output dataset.
        /// </summary>
        /// <param name="filenames">Array of filenames.</param>
        /// <param name="Expression">Formula to calculate.</param>
        /// <param name="outputFilename">Name of output dataset.</param>
        /// <param name="gdalOutputFormat">GDAL format. See list here: http://www.gdal.org/formats_list.html. Not
        /// all of them are necessarily support creation and writing operations.</param>
        /// <param name="NodataValue">No data value for output datasource.</param>
        /// <param name="cBack">Callback to report progress and errors.</param>
        /// <param name="ErrorMsg">Error description to be returned when method fails.</param>
        /// <returns>True on success.</returns>
        /// <remarks>
        /// Input datasets must have the same size in pixels (projection and transformation parameters 
        /// will be copied to output from first datasource, but won't be used during calculation process).
        /// Names of datasources in formula must match their filenames without path. Additionally @@X postfix
        /// must be added to mark index of band to be used (1-based). The following operation are supported in formula:
        /// arithmetic (+, -, *, /, ^); comparisons
        /// (&gt;, &lt;, &lt;=, &gt;=, &lt;&gt;, =) both for raster values and scalar constants.
        /// \code
        /// string[] names = { @"d:\Clip_L7_20000423_B2.tif", @"d:\Clip_L7_20000423_B5.tif" };
        /// string formula = "[Clip_L7_20000423_B5.tif@1] + [Clip_L7_20000423_B2.tif@1] + 5";   // @1 = first band is used in both cases
        /// string errorMsg;
        /// var ut = new Utils();
        /// bool result = ut.CalculateRaster(names, formula, @"d:\output.tif", "GTiff",
        /// 0f /* no data value */, this /*callback */, out errorMsg);
        /// \endcode
        /// </remarks>
        /// \new492 Added in version 4.9.2
        public bool CalculateRaster(Array filenames, string Expression, string outputFilename, string gdalOutputFormat, float NodataValue, ICallback cBack, out string ErrorMsg)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new datasource by mapping data ranges of the specified band of input datasource 
        /// to new values. 
        /// </summary>
        /// <param name="Filename">Filename of input GDAL raster datasource.</param>
        /// <param name="bandIndex">Band index of input datasource to be processed.</param>
        /// <param name="OutputName">Filename of output datasource.</param>
        /// <param name="LowerBounds">Array of double type with lower bounds of mapping ranges. These values
        /// will be compared with values of input datasource. Must be sorted in ascending order.</param>
        /// <param name="UpperBounds">Array of double type with upper bounds of mapping ranges. These values
        /// will be compared with values of input datasource.</param>
        /// <param name="NewValues">Array of double type with values to be set for output datasource
        /// for each mapping range.</param>
        /// <param name="gdalOutputFormat">GDAL format. See list here: http://www.gdal.org/formats_list.html. Not
        /// all of them are necessarily support creation and writing operations.</param>
        /// <param name="cBack">Callback to report progress and errors.</param>
        /// <returns>True on success.</returns>
        /// <remarks>
        /// All 3 input arrays must be of the same size. In case of overlapping ranges the one located first
        /// will be used. Here is code sample with 5 data ranges using anonymous data type:
        /// \code
        /// var ut = new Utils();
        /// var arr = new[]
        /// {
        ///    new {Low = 0.0, High = 20.0, NewValue = 40.0},
        ///    new {Low = 20.0, High = 40.0, NewValue = 150.0},
        ///    new {Low = 40.0, High = 60.0, NewValue = 100.0},
        ///    new {Low = 60.0, High = 80.0, NewValue = 20.0},
        ///    new {Low = 80.0, High = 100.0, NewValue = 80.0},
        /// };
        /// if (!ut.ReclassifyRaster(@"d:\source.tiff", 1 /* band index */, 
        ///    @"d:\output.tif", 
        ///    arr.Select(i => i.Low).ToArray(),
        ///    arr.Select(i => i.High).ToArray(),
        ///    arr.Select(i => i.NewValue).ToArray(),
        ///    "GTiff", this /* callback */))
        /// {
        ///    MessageBox.Show("Failed to reclassify: " + ut.get_ErrorMsg(ut.LastErrorCode));
        /// }
        /// else
        /// {
        ///    MessageBox.Show("Reclassified successfully");
        /// }
        /// \endcode
        /// </remarks>
        /// \new492 Added in version 4.9.2
        public bool ReclassifyRaster(string Filename, int bandIndex, string OutputName, Array LowerBounds, Array UpperBounds, Array NewValues, string gdalOutputFormat, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a boolean value which indicates whether the specified file holds tiff grid.
        /// </summary>
        /// <param name="Filename">The name of the file.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new493 Added in version 4.9.3 (is moved from AxMap.IsTiffGrid)
        public bool IsTiffGrid(string Filename)
        {
            throw new NotImplementedException();
        }

        public double GeodesicArea(Shape shapeWgs84)
        {
            throw new NotImplementedException();
        }

        public bool FixUpShapes(Shapefile subject, bool SelectedOnly, string outputFilename, bool Overwrite)
        {
            throw new NotImplementedException();
        }

        public bool BufferByDistance(Shapefile subject, double Distance, int nSegments, bool SelectedOnly, bool MergeResults,
            string outputFilename, bool Overwrite)
        {
            throw new NotImplementedException();
        }

        public bool ExplodeShapes(Shapefile subject, bool SelectedOnly, string outputFilename, bool Overwrite)
        {
            throw new NotImplementedException();
        }

        public bool ExportSelection(Shapefile subject, string outputFilename, bool Overwrite)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the tkUnitsOfMeasure enumeration associated with the specified EPSG Unit Code
        /// </summary>
        /// <param name="EPSGUnitCode">EPSG-defined Unit of Measure constant</param>
        /// <returns>The internal enumeration associated with the specified EPSG code</returns>
        /// <remarks>
        /// Supports the more common EPSG-defined Unit constants; specifically those that can be mapped to 
        /// the currently defined tkUnitsOfMeasure enumeration values. If the EPSG code cannot be mapped, 
        /// an Error will be submitted to the global callback with error code tkINVALID_PARAMETER_VALUE, 
        /// and return the default value of umDecimalDegrees.
        /// </remarks>
        /// \new495 Added in version 4.9.5
        public tkUnitsOfMeasure EPSGUnitConversion(int EPSGUnitCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the name of the projection associated with the specified NAD83 enumeration
        /// </summary>
        /// <param name="projectionID">
        /// Value from the the NAD83 projection enumeration tkNad83Projection
        /// </param>
        /// <returns>GDAL-provided projection name, or an empty string on error</returns>
        /// <remarks>
        /// If an error occurs, either because an invalid ID is specified, or the projection string
        /// could not be loaded, a descriptive error will be raised to the Callback function (if one is specified).
        /// </remarks>
        /// \new495 Added in version 4.9.5
        public string GetNAD83ProjectionName(tkNad83Projection projectionID)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the name of the projection associated with the specified WGS84 enumeration
        /// </summary>
        /// <param name="projectionID">
        /// Value from the the NAD83 projection enumeration tkNad83Projection
        /// </param>
        /// <returns>GDAL-provided projection name, or an empty string on error</returns>
        /// <remarks>
        /// If an error occurs, either because an invalid ID is specified, or the projection string
        /// could not be loaded, a descriptive error will be raised to the Callback function (if one is specified).
        /// </remarks>
        /// \new495 Added in version 4.9.5
        public string GetWGS84ProjectionName(tkWgs84Projection projectionID)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the name of the projected coordinate system associated with the specified SRID,
        /// which include the NAD83 and WGS84 codes, as well as those not specified by the enumerations, 
        /// such as NAD27, NAD83 Harn, Beijing, Pulkova, etc.
        /// </summary>
        /// <param name="SRID">
        /// Spatial Reference identifier, could refer to any valid Projected or Geographic coordinate system
        /// </param>
        /// <returns>GDAL-provided name of a projected or geographic coordinate system, or an empty string on error</returns>
        /// <remarks>
        /// If an error occurs, either because an invalid ID is specified, or the projection string
        /// could not be loaded, a descriptive error will be raised to the Callback function (if one is specified).
        /// </remarks>
        /// \new495 Added in version 4.9.5
        public string GetProjectionNameByID(int SRID)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a list of ID,Name pairs of Projected and/or Geographic coordinate systems
        /// </summary>
        /// <param name="projectionSets">One or a combination of values from the tkProjectionSet enumeration</param>
        /// <param name="list">Reference to an Object that will be filled with an array of strings</param>
        /// <returns>True if successful (list will contain an array of strings containing the ID/Name pairs associated with the specified Projection Sets), or False on failure (list will be empty)</returns>
        /// <remarks>
        /// Each string will contain one pair of values, an ID and a Name, separated by a comma (e.g. "32611,WGS 84 / UTM zone 11N").
        /// These can be parsed and used, for example, to populate a ComboBox control for user selection of a Projection.
        /// If an error occurs, or if the projection strings could not be loaded, a descriptive error will be raised to the Callback function (if one is specified).
        /// </remarks>
        /// \new495 Added in version 4.9.5
        public bool GetProjectionList(tkProjectionSet projectionSets, ref Object list)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the angle defined by the two specified coordinates
        /// </summary>
        /// <param name="firstPoint"></param>
        /// <param name="secondPoint"></param>
        /// <returns>Geographic Angle (in degrees) of the vector measured from the first point to the second point</returns>
        /// <remarks>
        /// The angle returned is the so-called Geographic angle, measured in a clockwise direction
        /// from the positive Y-axis, as opposed to the Arithmetic (Cartesian) angle measured in a
        /// counter-clockwise direction from the positive X-axis.  The returned value can be used 
        /// as-is to specify Shape or Label Rotation, which expect the Geographic angle for input.
        /// </remarks>
        /// \see Shapefile.set_ShapeRotation
        /// 
        /// \new500 Added in version 5.0.0
        public double GetAngle(Point firstPoint, Point secondPoint)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Return a point at the specified distance (or percentage) along the specified line
        /// </summary>
        /// <param name="sourceLine">Polyline shape to traverse</param>
        /// <param name="startPoint">Starting point along 'sourceLine' (specify Point[0] for beginning of line)</param>
        /// <param name="distance">Distance along line (or percentage of line length; if a percentage, specify a number between 0.0 and 1.0)</param>
        /// <param name="normalized">Optional value; if FALSE, 'distance' is actual distance; if TRUE, distance is percentage of length; defaults to FALSE</param>
        /// <returns>Returns a Point class representing the point along the sourceLine that is the specified distance (or percentage) along the line.</returns>
        /// <remarks>
        /// 'sourceLine' must be a Polyline Shape.  If 'distance' is greater than the source line length, the line's endpoint is returned.
        /// 
        /// 'startPoint' does not have to be on 'sourceLine'.  If not on the line, actual starting point will be the nearest point to 'startPoint' that is on 'sourceline'.
        /// </remarks>
        /// \see Shape.InterpolatePoint
        /// 
        /// \new500 Added in version 5.0.0
        public Point LineInterpolatePoint(Shape sourceLine, Point startPoint, double distance, bool normalized)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Return a distance along the source polyline to a Point nearest the reference Shape
        /// </summary>
        /// <param name="sourceLine">Source Shape, must be a polyline</param>
        /// <param name="referenceShape">Another Shape of any Shapetype</param>
        /// <returns>Returns the distance along sourceline to the point nearest the specified referenceShape.</returns>
        /// <remarks>
        /// sourceLine must be a Polyline Shape.  Returned Distance is measured from Point[0].  If resulting 'distance' would be greater than the length of sourceLine, then sourceLine's length is returned.
        /// </remarks>
        /// \see Shape.ProjectDistanceTo
        /// 
        /// \new500 Added in version 5.0.0
        public Double ProjectDistanceTo(Shape sourceLine, Shape referenceShape)
        {
            throw new NotImplementedException();
        }

    }
#if nsp
}
#endif

