// ReSharper disable UnassignedGetOnlyAutoProperty
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    /// <summary>
    /// Implementation of the GDAL v2 librified functions.
    /// Not all functions are implemented yet.
    /// </summary>
    /// \new495 Added in version 4.9.5
#if nsp
#if upd
    public class GdalUtils : MapWinGIS.IGdalUtils
#else
    public class IGdalUtils
#endif
#else
        public class GdalUtils
#endif
    {
        /// <summary>
        /// Retrieves the last error generated in the object. 
        /// </summary>
        /// \new495 Added in version 4.9.5
        public int LastErrorCode { get; }

        /// <summary>
        /// The global callback is the interface used by MapWinGIS to pass progress and error events to interested applications.
        /// </summary>
        /// \new495 Added in version 4.9.5
        public ICallback GlobalCallback { get; set; }

        /// <summary>
        /// The key may be used by the programmer to store any string data associated with the object.
        /// </summary>
        /// \new495 Added in version 4.9.5
        public string Key { get; set; }

        /// <summary>
        /// Gets the detailed error message.
        /// </summary>
        /// \new495 Added in version 4.9.5
        public string DetailedErrorMsg { get; }

        /// <summary>
        /// Image reprojection and warping utility.
        /// Implementing the librified function of GDAL's gdalwarp.exe tool
        /// </summary>
        /// <param name="bstrSrcFilename">The source filename.</param>
        /// <param name="bstrDstFilename">The destination filename.</param>
        /// <param name="Options">The options, as a string array</param>
        /// <remarks>See GDAL's documentation here: http://www.gdal.org/gdalwarp.html</remarks>
        /// \new495 Added in version 4.9.5
        /// 
        /// \code
        /// // Example of creating VRT file from TIFF file. More options are possible:
        /// var output = Path.GetTempPath() + "GdalWarp.vrt";
        /// var options = new[]
        /// {
        ///     "-of", "vrt",
        ///     "-overwrite"
        /// };
        /// var gdalUtils = new GdalUtils();
        /// if (!gdalUtils.GDALWarp("test.tif", output, options))
        /// {
        ///     Debug.WriteLine("GdalWarp failed: " + gdalUtils.ErrorMsg[gdalUtils.LastErrorCode] + " Detailed error: " + gdalUtils.DetailedErrorMsg);
        /// }
        /// \endcode 
        /// 
        /// \code
        /// // Example of cutting a TIFF file with a border file: 
        /// var output = Path.GetTempPath() + "GdalWarpCutline.vrt";
        /// const string border = @"test.shp";

        /// var options = new[]
        /// {
        ///     "-of", "vrt",
        ///     "-overwrite",
        ///     "-crop_to_cutline",
        ///     "-cutline", border
        /// };
        /// var gdalUtils = new GdalUtils();
        /// if (!gdalUtils.GDALWarp("test.tif", output, options))
        /// {
        ///     Debug.WriteLine("GdalWarp failed: " + gdalUtils.ErrorMsg[gdalUtils.LastErrorCode] + " Detailed error: " + gdalUtils.DetailedErrorMsg);
        /// }
        /// \endcode
        public bool GDALWarp(string bstrSrcFilename, string bstrDstFilename, Array Options)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Converts simple features data between file formats.
        /// Implementing the librified function of GDAL's ogr2ogr.exe tool
        /// </summary>
        /// <param name="bstrSrcFilename">The source filename.</param>
        /// <param name="bstrDstFilename">The destination filename.</param>
        /// <param name="Options">The options, as a string array</param>
        /// <param name="useSharedConnection">If set to <c>true</c> improves performance but also might make it instable.</param>
        /// <remarks>See GDAL's documentation here: http://www.gdal.org/ogr2ogr.html</remarks>
        /// \new495 Added in version 4.9.5
        /// 
        /// \code
        /// // Converting shapefile to gml:
        /// var outputFilename = Path.Combine(Path.GetTempPath(), "translated.gml");
        /// var options = new[]
        /// {
        ///     "-f", "GML"
        /// };
        /// var gdalUtils = new GdalUtils();
        /// if (!gdalUtils.GdalVectorTranslate(inputFilename, outputFilename, options, true))
        /// {
        ///     Debug.WriteLine("GdalVectorTranslate failed: " + gdalUtils.ErrorMsg[gdalUtils.LastErrorCode] + " Detailed error: " + gdalUtils.DetailedErrorMsg);
        /// }
        /// \endcode
        public bool GdalVectorTranslate(string bstrSrcFilename, string bstrDstFilename, Array Options,
            bool useSharedConnection = false)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clips the vector with another vector.
        /// </summary>
        /// <param name="bstrSubjectFilename">The subject filename.</param>
        /// <param name="bstrOverlayFilename">The overlay filename.</param>
        /// <param name="bstrDstFilename">The destination filename.</param>
        /// <param name="useSharedConnection">If set to <c>true</c> improves performance but also might make it instable.</param>
        /// <remarks>Uses GdalUtils.GdalVectorTranslate under the hood.</remarks>
        /// \new495 Added in version 4.9.5
        /// 
        /// \code
        /// // Clipping large shapefile with border file
        /// const string subjectFilename = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\Fishnet.shp";
        /// const string borderFilename = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\border.shp";
        /// var outputFilename = Path.Combine(tempFolder, "GdalVectorTranslate.shp");
        /// var gdalUtils = new GdalUtils();
        /// if (!gdalUtils.ClipVectorWithVector("LargeFile.shp", "Border.shp", outputFilename))
        /// {
        ///     Debug.WriteLine("GdalVectorTranslate failed: " + gdalUtils.ErrorMsg[gdalUtils.LastErrorCode] + " Detailed error: " + gdalUtils.DetailedErrorMsg);
        /// }
        /// \endcode
        public bool ClipVectorWithVector(string bstrSubjectFilename, string bstrOverlayFilename, string bstrDstFilename,
            bool useSharedConnection = true)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Retrieves the error message associated with the specified error code. 
        /// </summary>
        /// <param name="ErrorCode">The error code for which the error message is required.</param>
        /// <returns>The error message description for the specified error code. </returns>
        /// \new495 Added in version 4.9.5
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }
    }
}
