
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Holds information about coordinate system and projection of the data.
    /// </summary>
    /// <b>1. Formats and initialization. </b>Information about coordinate system and projection  can be represented in several formats:
    ///     - ESRI WKT string;
    ///     - proj 4 string;
    ///     - EPSG code;
    ///     .
    /// <a href ="http://www.epsg.org/">EPSG codes</a> represent a standardization effort to map each projection to the unique code. 
    /// It's recommended to use these codes whenever possible. GeoProjection.ImportFromAutoDetect() method implements logic to discern 
    /// the format of the provided projection string. \n
    /// \code
    /// GeoProjection proj = new GeoProjection(); 
    /// // EPSG code
    /// proj.ImportFromEPSG(4326);  // WGS84
    ///     
    /// // proj 4 string
    /// proj.ImportFromProj4("+proj=longlat +datum=WGS84 +no_defs");  // WGS84
    /// 
    /// //autodetect the format
    /// string unknown_format = "4326";
    /// proj.ImportFromAutoDetect(unknown_format);
    /// 
    /// // from file
    /// string filename = "some_name";
    /// proj.ReadFromFile(filename);
    /// 
    /// // show the name of the loaded projection
    /// Debug.Print("Projection loaded: " + proj.Name);
    /// 
    /// // show proj 4 representation
    /// Debug.Print("Proj4 representation: " + proj.ExportToProj4());
    /// \endcode
    /// Projection is usually stored in a separate file with .prj extension which accompany the data file. 
    /// GeoProjection.ReadFromFile() and GeoProjection.WriteToFile() to interact with projection file.\n\n
    /// <b>2. Classification and parameters. </b>Coordinate systems are split in 2 broad categories:
    ///     -# Geographical:
    ///         - the data is specified in decimal degrees as a pair of latitude and longitude value;
    ///         - it implies the usage of equirectangular projection for displaying the data on the screen;
    ///         .
    ///     -# Projected:
    ///         - the Earth surface data is projected to the plane according to specific rule (projection);
    ///         - a number of projections can be used (Mercator, Transverse Mercator, Lambert conformal conic, etc),
    ///         each of the them characterized with certain distortions and certain properties of the data which are preserved 
    ///         (like angles, directions, area, etc.);
    ///         - usually the data is specified in meters or feet;
    ///         .
    /// Normally spatial reference holds the information about:
    ///     - the reference ellipsoid (GRS80, Krasovski, etc.);
    ///     - the name of datum, which defines coordinate system relative to the surface of ellipsoid (WGS84, NAD83, Pulkovo1942, etc.);
    ///     - optionally projection, if the data isn't defined in decimal degrees (UTM, Gauss-Kruger, etc).
    ///     . 
    ///     \n
    /// Use GeoProjection.IsGeographic, GeoProjection.IsProjected properties to distinguish geographical and 
    /// projected coordinate systems. To retrieve parameters of geographic coordinate system - GeoProjection.get_GeogCSParam.
    /// To extract parameters of the projection use GeoProjection.get_ProjectionParam.\n\n
    /// <b>3. Well-known coordinate systems. </b>The most widely spread geographic coordinate system is so called WGS84. It is based on WGS84 datum 
    /// and uses GRS80 ellipsoid. Universal Transverse Mercator is the most widely used projection of this coordinate system, 
    /// which defines sixty 6-degree zones which cover the whole globe. There are several methods to setup the widely used 
    /// coordinate systems and projections: GeoProjection.SetWellKnownGeogCS(), GeoProjection.SetWgs84Projection(), 
    /// GeoProjection.SetNad83Projection().\n\n
    /// The definition of WGS84 coordinate system and projection in different formats can be as following:
    ///     -# proj4:
    ///         - geographic WGS84: +proj=longlat +datum=WGS84 +no_defs 
    ///         - WGS84/UTM 10: +proj=utm +zone=10 +datum=WGS84 +units=m +no_defs \n
    ///         .
    ///     -# EPSG code: 
    ///         - geographic: 4326
    ///         - WGS84/UTM 10: 32610
    ///         .
    ///     -# ESRI WKT WGS84:
    ///         \image html wgs.png
    /// See http://spatialreference.org/ for specification of coordinate systems and projections.\n\n
    /// <b>4. Standardization issues. </b>Because of the lack of standardization it's not guaranteed that subsequent exporting and importing operations for the certain projection 
    /// format (proj4, WKT, etc.) will yield the same state of the geoprojection object. Therefore it's no guaranteed way
    /// to copy the state of the object unless knowing the way it was initialized.\n\n
    /// The different variants for specification of the same projection even in the same format became quite a problem for GIS
    /// software. GeoProjection.get_IsSame and GeoProjection.get_IsSameExt perform the comparison of 2 projections. 
    /// The task is critical when one need to determine whether the data from several sources will be displayed correctly on the screen.
    /// .
    /// \new48 Added in version 4.8
    #if nsp
        #if upd
            public class GeoProjection : MapWinGIS.IGeoProjection
        #else        
            public class IGeoProjection
        #endif
    #else
        public class GeoProjection
    #endif
    {
        #region IGeoProjection Members
        /// <summary>
        /// Copies information from another GeoProjection object.
        /// </summary>
        /// <param name="sourceProj">The geoprojection to copy information from.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool CopyFrom(GeoProjection sourceProj)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Exports information to the proj4 format.
        /// </summary>
        /// <returns>The string with proj4 projection.</returns>
        public string ExportToProj4()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Exports information to the WKT format.
        /// </summary>
        /// <returns>The string with WKT projection.</returns>
        public string ExportToWKT()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the name of the coordinate system.
        /// </summary>
        /// <remarks>An empty string will be returned unless GeoProjection.IsGeographic property returns true.</remarks>
        public string GeogCSName
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a callback object which is used to report errors.
        /// </summary>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Initializes geoprojection from the input string of arbitrary format.
        /// </summary>
        /// <remarks>See GDAL documentation on particular formats supported.</remarks>
        /// <param name="proj">The string to initialize projection from.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool ImportFromAutoDetect(string proj)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Initializes geoprojection object from EPSG numeric code.
        /// </summary>
        /// <param name="projCode">EPSG code.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool ImportFromEPSG(int projCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Initializes geoprojection object from ESRI WKT format.
        /// </summary>
        /// <param name="proj">The string in the ESRI WKT format.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool ImportFromESRI(string proj)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Initializes geoprojection object from proj4 format.
        /// </summary>
        /// <param name="proj">The string in the proj4 format.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool ImportFromProj4(string proj)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Initializes geoprojection object from OGC WKT format.
        /// </summary>
        /// <param name="proj">The string in the OGC WKT format.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool ImportFromWKT(string proj)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the inverse flattening of the reference ellipsoid of the coordinate system.
        /// </summary>
        public double InverseFlattening
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a boolean value which indicates whether any projection definition is stored in the object.
        /// </summary>
        public bool IsEmpty
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a boolean value which indicates whether the coordinate system stored in the object is the geographic one.
        /// </summary>
        public bool IsGeographic
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a boolean value which indicates whether the coordinate system stored in the object is the local one.
        /// </summary>
        public bool IsLocal
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a boolean value which indicates whether the coordinate system stored in the object is the projected one.
        /// </summary>
        public bool IsProjected
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// A text string associated with the instance of the class. Any value can be stored by developer in this property.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Retrieves the numeric code of the last error that took place in the class.
        /// </summary>
        /// <remarks>The usage of this property clears the error code.</remarks>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Returns the name of the coordinate system.
        /// </summary>
        /// <remarks>Either GeoProjection.GeogCSName or GeoProjection.ProjectionName will be returned 
        /// depending on the type of coordinate system.</remarks>
        public string Name
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The name of the projected coordinate system.
        /// </summary>
        /// <remarks>An empty string will be returned unless GeoProjection.IsProjected property returns true.</remarks>
        public string ProjectionName
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Initializes geoprojection from the specified file.
        /// </summary>
        /// <remarks>The method is used by MapWinGIS for reading projection of the data layer.
        /// The file with projection must have the same name as the name of the data file and .prj extension.</remarks>
        /// <param name="Filename">The name of the file to read projection string from.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool ReadFromFile(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the half of the major axis of the reference ellipsoid.
        /// </summary>
        public double SemiMajor
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the half of the minor axis of the reference ellipsoid.
        /// </summary>
        public double SemiMinor
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Initializes the object with NAD83 projection specified by enumerated constant.
        /// </summary>
        /// <param name="Projection">Numeric code of the projection (EPSG code).</param>
        public void SetNad83Projection(tkNad83Projection Projection)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Initializes the object with well known geographic coordinate system.
        /// </summary>
        /// <param name="newVal">Numeric code of the projection (EPSG code).</param>
        public void SetWellKnownGeogCS(tkCoordinateSystem newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Initializes the object with WGS84 projection specified by enumerated constant.
        /// </summary>
        /// <param name="Projection">Numeric code of the projection (EPSG code).</param>
        public void SetWgs84Projection(tkWgs84Projection Projection)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Writes projection string in ESRI WKT format to the specified file.
        /// </summary>
        /// <param name="Filename">The name of the file to write the string in.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool WriteToFile(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the description of the specific error code.
        /// </summary>
        /// <param name="ErrorCode">The error code returned by ShapefileCategories.LastErrorCode.</param>
        /// <returns>The string with the description.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets specified parameter of the geographic coordinate system.
        /// </summary>
        /// <param name="Name">The parameter to return.</param>
        /// <param name="pVal">The returned value of the parameter.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \code
        /// GeoProjection proj = some_proj;
        /// 
        /// // shows well-known text representation
        /// Debug.Print("WKT representation: " + proj.ExportToWKT());
        /// // the output: WKT representation: GEOGCS["WGS 84",DATUM["WGS_1984"...
        /// 
        /// // let's show the properties of the geographic projection
        /// string s = "";
        /// double[] arr = new double[5];
        /// for (int i = 0; i < 5; i++)
        /// {
        ///     // extract parameter in the element of array
        ///     proj.get_GeogCSParam((tkGeogCSParameter)i, ref arr[i]);
        ///    
        ///     // append the name of parameter and the value to the string
        ///     s += ((tkGeogCSParameter)i).ToString() + ": " + arr[i] + Environment.NewLine;
        /// }
        /// Debug.Print("Parameters of geographic coordinate system: " + Environment.NewLine + s);
        /// // The output: 
        /// // SemiMajor: 6356752,31424518
        /// // SemiMinor: 6356752,31424518
        /// // InverseFlattening: 298,257223563
        /// // PrimeMeridian: 0
        /// // AngularUnit: 0,0174532925199433
        /// \endcode
        public bool get_GeogCSParam(tkGeogCSParameter Name, ref double pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a boolean value which indicates whether 2 projections are identical.
        /// </summary>
        /// <remarks>Projections will be reported as non-identical if at least one of parameters
        /// differ between them, even if it doesn't affect the reprojection process. For example,
        /// different name of the datums, while all the parameters of the datums match.</remarks>
        /// <param name="proj">The projection object to compare the current projection with.</param>
        /// <returns>True in case the projection definitions are identical and false otherwise.</returns>
        public bool get_IsSame(GeoProjection proj)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Performs a test of identity for 2 projections.
        /// </summary>
        /// <remarks>The method perform reprojection of the random points in the given extents to WGS84 
        /// coordinate system (EPSG:4326) for both projections being compared. In case the 
        /// coordinates of the projected points match - projections are considered the same. 
        /// This doesn't mean that result of test will be the same for some other extents or that 
        /// projection definitions are completely identical. But it ensures that data layers with 
        /// these 2 projection can be displayed jointly within the tested extents.
        /// </remarks>
        /// <param name="proj">The compared projection definition.</param>
        /// <param name="bounds">The bounding box to take random points for comparison from.</param>
        /// <param name="numSamplingPoints">Number of the random points to take for sampling.</param>
        /// <returns>True in case the projections proved to be identical and false otherwise.</returns>
        public bool get_IsSameExt(GeoProjection proj, Extents bounds, int numSamplingPoints)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a boolean value which indicates whether 2 projections belong to the same geographic coordinate system.
        /// </summary>
        /// <param name="proj">The compared projection definition.</param>
        /// <returns>True in case projection share the same geographic coordinate system and false otherwise.</returns>
        public bool get_IsSameGeogCS(GeoProjection proj)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns specified parameter of the projection
        /// </summary>
        /// <param name="Name">The parameter to return.</param>
        /// <param name="Value">The returned value of the parameter.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool get_ProjectionParam(tkProjectionParameter Name, ref double Value)
        {
            throw new NotImplementedException();
        }

        #endregion

        /// <summary>
        /// Gets a value indicating whether transformation to some target projection was opened with GeoProjection.StartTransform() call.
        /// </summary>
        public bool HasTransformation
        {
            get { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Opens transformation to the specified target projection. The transformation will be used by GeoProjection. 
        /// Transform calls until GeoProjection.StopTransform is called.
        /// </summary>
        /// <param name="target">The target projection.</param>
        /// <returns>True if transformation is created and false on failure.</returns>
        public bool StartTransform(GeoProjection target)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Stops transformation created by GeoProjection.StartTransform call.
        /// </summary>
        public void StopTransform()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Transforms a single point from this projection to target projection specified in GeoProjection.StartTransform call.
        /// </summary>
        /// <param name="x">X coordinate of point to transform.</param>
        /// <param name="y">Y coordinate of point to transform.</param>
        /// <returns>True on success.</returns>
        public bool Transform(ref double x, ref double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns object to initial empty state.
        /// </summary>
        /// \see GeoProjection.IsFrozen
        /// \new491 Added in version 4.9.1
        public bool Clear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a copy GeoProjection object
        /// </summary>
        /// <returns>A copy of GeoProjection object.</returns>
        /// <remarks>It's not guaranteed that exact clone will be created. The method uses 
        /// GeoProjection.ExportFromWKT and GeoProjection.ImportFromWKT pair of methods which may yield slightly different result, 
        /// as ExportToWKT may add defaults for parameters missing in initial Proj4 definition.</remarks>
        /// \new491 Added in version 4.9.1
        public GeoProjection Clone()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a value indicating whether the object can be changed.
        /// </summary>
        /// <remarks>
        /// When frozen all the methods which can change the inner state of object will fail.
        /// The property is set to true for map projection applied with AxMap.SetGeoProjection (see details there).
        /// </remarks>
        /// \new491 Added in version 4.9.1
        public bool IsFrozen
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Sets so called Google Mercator projection (aka Spherical Mercator; EPSG:3857), 
        /// commonly used by tile servers.
        /// </summary>
        /// <returns>True on success.</returns>
        /// \new490 Added in version 4.9.0
        public bool SetGoogleMercator()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets WGS84 geographic coordinates system (EPSG:4326). 
        /// </summary>
        /// <remarks>
        /// Equirectangular projection will be implicitly used during rendering.
        /// </remarks>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool SetWgs84()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tries to determine EPSG code projection currently stored in the object.
        /// </summary>
        /// <param name="epsgCode">Determined epsgCode or -1 on failure.</param>
        /// <remarks>It uses GDAL's OGRSpatialReference.AutoIdentifyEPSG method and recognized 
        /// limited number of commonly used coordinate systems (WGS84, NAD83, etc) and their zonal projections,
        /// plus information stored in Authority node for WKT format will be used as well.</remarks>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool TryAutoDetectEpsg(out int epsgCode)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif

