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

        /// <summary>
        /// The global callback is the interface used by MapWinGIS to pass progress and error events to interested applications.
        /// </summary>
        /// \new495 Added in version 4.9.5
        ICallback IGdalUtils.GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
    }
}
