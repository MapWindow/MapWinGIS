
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Represents GDAL/OGR vector datasource which may be represented by spatial database or file-based vector format. 
    /// </summary>
    /// <remarks>The class can be used to access various vector datasources supported by GDAL/OGR. Depending on driver, 
    /// datasources may be represented by particular file formats (
    /// <a href ="http://www.gdal.org/drv_shapefile.html">ESRI %Shapefile</a>, 
    /// <a href ="http://www.gdal.org/drv_mitab.html">MapInfo TAB</a>, <a href="http://www.gdal.org/drv_kml.html">KML</a>, etc)
    /// or spatial databases (<a href="http://www.gdal.org/drv_pg.html">PostGIS</a>, 
    /// <a href ="http://www.gdal.org/drv_mssqlspatial.html">Microsoft SQL Server</a>, 
    /// <a href="http://www.gdal.org/drv_sqlite.html">SpatiaLite</a>, etc). See the full list of formats in GDAL 
    /// <a href = "http://www.gdal.org/ogr_formats.html">documentation</a>.\n\n
    /// \dot
    /// digraph ogr_source_types {
    /// splines = true;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// ds [ label="OGR Datasource"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s2    [label=" File-based vector formats\l"];
    /// s1    [label=" Spatial databases\l"];
    /// 
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" minlen=2 ]
    /// ds -> s2;
    /// ds -> s1;
    /// }
    /// \enddot
    /// 
    /// %OgrDatasource can be used for such tasks as:
    /// - checking connection for particular datasource (OgrDatasource.Open);
    /// - listing and opening layers of datasource (OgrDatasource.GetLayer, OgrDatasource.LayerCount, OgrDatasource.GetLayerByName);
    /// - creation of new layers via shapefile import (OgrDatasource.ImportShapefile);
    /// - running SQL queries against the datasource to retrieve or modify the data (OgrDatasource.RunQuery, OgrDatasource.ExecuteSQL);
    /// - checking for capabilities supported by particular driver (OgrDatasource.TestCapability)
    /// .
    /// An instance of %OgrDatasource can't be added to the map directly, but instances of OgrLayer class opened by its methods can.\n\n
    /// %OgrDatasource encapsulates an instance of GDALDataset C++ class. Check its 
    /// <a href = "http://www.gdal.org/classGDALDataset.html">documentation</a> to better understand what's going on under the hood. \n\n
    /// Here is code sample which lists all layers available in PostGIS database.
    /// \code
    /// private static string CONNECTION_STRING = "PG:host=localhost dbname=london user=postgres password=1234";
    /// 
    /// var ds = new OgrDatasource();
    /// 
    /// if (!ds.Open(CONNECTION_STRING))
    /// {
    ///     Debug.Print("Failed to establish connection: " + ds.GdalLastErrorMsg);
    /// }
    /// else
    /// {
    ///     int count = ds.LayerCount;
    ///     Debug.Print("Number of layers: " + count);
    /// 
    ///     Debug.Print("List of layers by name:");
    ///     for (int i = 0; i < count; i++)
    ///     {
    ///         var lyr = ds.GetLayer(i);
    ///         Debug.Print("Layer name: " + lyr.Name);
    ///         Debug.Print("Projection: " + lyr.GeoProjection.ExportToProj4());
    ///         Debug.Print("Shape type: " + lyr.ShapeType);
    ///         lyr.Close();
    ///     }
    ///     ds.Close();
    /// }
    /// \endcode
    /// See more samples in description of particular methods.\n\n
    /// 
    /// <b>String encoding.</b>\n
    /// 
    /// %OgrDatasource and OgrLayer classes by default use UTF-8 string encoding to interact with underlying drivers.
    /// This applies to connection strings, layer names, SQL queries and error messages. This behavior can be changed
    /// by setting GlobalSettings.OgrStringEncoding property. The new setting will be used immediately by all existing
    /// and new objects. See details about string encoding in the documentation of particular OGR driver.\n\n
    /// 
    /// Here is a diagram for the OgrDatasource class.
    /// \dot 
    /// digraph ogr_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 1, fontname=Helvetica, fontsize=9, color = tan, style = filled, height = 0.3, width = 1.0];
    /// sf [ label="In-memory shapefile with data" URL="\ref Shapefile"];
    /// 
    /// node [style = dashed, color = gray];
    /// ds [ label="OgrDatasource" URL="\ref OgrDatasource"];
    /// 
    /// node [color = gray, peripheries = 3, style = filled, height = 0.2, width = 0.8]
    /// lyr [ label="OgrLayer" URL="\ref OgrLayer"];
    /// 
    /// edge [dir = "none", dir = "none", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// lyr -> sf [ URL="\ref OgrLayer.GetBuffer()", tooltip = "OgrLayer.GetBuffer()", headlabel = "   1"];
    /// ds -> lyr [URL="\ref OgrDatasource.GetLayer()", tooltip = "OgrDatasource.GetLayer()", headlabel = "   n"]
    /// }
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>
    /// </remarks>
    /// \new493 Added in version 4.9.3
#if nsp
    #if upd
        public class OgrDatasource : MapWinGIS.IOgrDatasource
    #else        
        public class IOgrDatasource
    #endif
#else
        public class OgrDatasource
#endif
    {
        /// <summary>
        /// Closes datasource and underlying connection.
        /// </summary>
        /// <remarks>This method should be called as soon as datasource is no longer needed.</remarks>
        public void Close()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Opens particular type of datasource supported by GDAL/OGR.
        /// </summary>
        /// <remarks>See details about connection strings here: http://www.gdal.org/ogr_formats.html</remarks>
        /// <param name="connectionString">Connection string for RDMSs or filename for file-based formats.</param>
        /// <returns>True on success.</returns>
        public bool Open(string connectionString)
        {
            throw new NotImplementedException();
        }

        public OgrLayer GetLayer2(int Index, bool forUpdate, bool newConnection)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Opens particular type of datasource supported by GDAL/OGR.
        /// </summary>
        /// <remarks>See details about connection strings here: http://www.gdal.org/ogr_formats.html</remarks>
        /// <param name="connectionString">Connection string for RDMSs or filename for file-based formats.</param>
        /// <param name="forUpdate">Indicates whether datasource will be opened with update flag.</param>
        /// <returns>True on success.</returns>
        public bool Open2(string connectionString, bool forUpdate)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Gets a layer with specified index from datasource.
        /// </summary>
        /// <remarks>Resulting OgrLayer has no reference to %OgrDatasource object, which can safely closed if it is no longer needed.\n\n</remarks>
        /// <param name="Index">Index of layer to be returned.</param>
        /// <param name="forUpdate">Indicates whether the returned layer will support saving of changes back to source
        /// (the functionality should be supported by particular driver).</param>
        /// <returns>Instance of OgrLayer or null on failure.</returns>
        /// The following code adds all layers found in the datasource to the map:
        /// \code
        /// private static string CONNECTION_STRING = "PG:host=localhost dbname=london user=postgres password=1234";
        /// 
        /// private static bool DisplayAllLayers()
        /// {
        ///     var ds = new OgrDatasource();
        ///     
        ///     if (!ds.Open(CONNECTION_STRING))
        ///     {
        ///         Debug.WriteLine("Failed to establish connection: " + ds.GdalLastErrorMsg);
        ///     }
        ///     else
        ///     {
        ///         map.RemoveAllLayers();
        /// 
        ///         // make sure it matches SRID of the layers (4326 in our case)
        ///         map.Projection = tkMapProjection.PROJECTION_WGS84;   
        /// 
        ///         for (int i = 0; i < ds.LayerCount; i++)
        ///         {
        ///             var layer = ds.GetLayer(i);
        ///             if (layer != null)
        ///             {
        ///                 int handle = map.AddLayer(layer, true);
        ///                 if (handle == -1)
        ///                 {
        ///                     Debug.WriteLine("Failed to add layer to the map: " + map.get_ErrorMsg(map.LastErrorCode));
        ///                 }
        ///                 else
        ///                 {
        ///                     Debug.WriteLine("Layer was added the map: " + layer.Name);
        ///                 }
        ///             }
        ///         }
        ///         map.ZoomToMaxVisibleExtents();
        ///         ds.Close();
        ///     }
        ///     return true;
        /// }
        /// \endcode
        /// See more details on projection & coordinates strategies in 
        /// <a href="group__map__coordinates.html#details">Map projection and coordinates</a> section.
        public OgrLayer GetLayer(int Index, bool forUpdate = false)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a name of the layer with particular index.
        /// </summary>
        /// <param name="layerIndex">Index of layer.</param>
        /// <returns>Name of layer or empty string for invalid layer index.</returns>
        public string GetLayerName(int layerIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a layer with specified name from the datasource.
        /// </summary>
        /// <remarks>In case of spatial databases layerName corresponds to name of the underlying table. 
        /// However there can be some format specifics. For example name may include schema prefix and name of geometry column 
        /// (in case there is more than one geometry column in the table).
        /// For example, <i>"public.buildings(geom2)"</i> may correspond to <i>"geom2"</i> column of table named 
        /// <i>"buildings"</i> in <i>"public"</i> schema.\n
        /// 
        /// Resulting layer has no reference to %OgrDatasource object, which can safely closed if it is no longer needed.</remarks>
        /// <param name="layerName">Layer name (case insensitive).</param>
        /// <param name="forUpdate">Indicates whether the returned layer will support saving of changes back to source
        /// (the functionality should be supported by particular driver).</param>
        /// <returns>Instance of OgrLayer or null on failure.</returns>
        public OgrLayer GetLayerByName(string layerName, bool forUpdate = false)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Runs a query against datasource and returns the result as a temporary layer. 
        /// </summary>
        /// <remarks>Usually some sort of SELECT query returning a set of rows with one 
        /// or several geometry columns is expected here. Other types of SQL instructions will also be processed, 
        /// however OgrDatasource.ExecuteSQL is recommended way to do it.\n\n
        /// Resulting layer has no reference to %OgrDatasource object, which can safely closed if it is no longer needed.\n
        /// </remarks>
        /// <param name="sql">SQL query.</param>
        /// <returns>Temporary layer or null on failure.</returns>
        /// The following code builds 0.01 degree buffer zones around waterways layer extracted from database
        /// and displays both waterways and buffer layers on the map. st_buffer function is supported by PostGIS.
        /// See documentation of particular spatial database for list of supported functions and their arguments.
        /// \code
        /// private static string CONNECTION_STRING = "PG:host=localhost dbname=london user=postgres password=1234";
        /// 
        /// private static bool TestSpatialQuery()
        /// {
        ///     var ds = new OgrDatasource();
        ///     
        ///     if (!ds.Open(CONNECTION_STRING))
        ///     {
        ///         Debug.WriteLine("Failed to establish connection: " + ds.GdalLastErrorMsg);
        ///     }
        ///     else
        ///     {
        ///         map.RemoveAllLayers();
        ///         map.Projection = tkMapProjection.PROJECTION_WGS84;
        /// 
        ///         // build a buffer in a first query
        ///         string sql = "SELECT st_buffer(wkb_geometry, 0.01) AS buffer, * FROM waterways;";
        ///         var buffer = ds.RunQuery(sql);
        ///         if (buffer == null)
        ///         {
        ///             Debug.WriteLine("Failed to build buffer: " + gs.GdalLastErrorMsg);
        ///         }
        ///         else
        ///         {
        ///             Debug.WriteLine("Number of features in a buffer: " + buffer.FeatureCount);
        ///             map.AddLayer(buffer, true);
        ///             buffer.GetBuffer().DefaultDrawingOptions.FillColor = 255;     // red
        ///         }
        /// 
        ///         // simply extract unaltered source features in the second query and display them above the buffer
        ///         string sql2 = "SELECT * FROM waterways;";
        ///         var layer = ds.RunQuery(sql2);
        ///         if (layer == null)
        ///         {
        ///             Debug.WriteLine("Failed to open layer: " + gs.GdalLastErrorMsg);
        ///         }
        ///         else
        ///         {
        ///             Debug.WriteLine("Number of features in layer: " + layer.FeatureCount);
        ///             map.AddLayer(layer, true);
        ///         }
        ///         ds.Close();
        ///     }
        ///     return true;
        /// }
        /// \endcode
        public OgrLayer RunQuery(string sql)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deletes a layer with specified index from datasource.
        /// </summary>
        /// <remarks>Use OgrDatasource.TestCapability(tkOgrDSCapability.odcDeleteLayer) to check 
        /// whether functionality is supported by particular driver.</remarks>
        /// <param name="layerIndex">Layer index within datasource.</param>
        /// <returns>True on success.</returns>
        public bool DeleteLayer(int layerIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Test whether driver of the currently opened datasource supports particular capability.
        /// </summary>
        /// <param name="capability">A capability to test.</param>
        /// <returns>True in case the capability is supported by the driver.</returns>
        public bool TestCapability(tkOgrDSCapability capability)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns index of layer with the specified name.
        /// </summary>
        /// <param name="layerName">Layer name (case insensitive).</param>
        /// <returns>Index of layer or -1 in case it wasn't found.</returns>
        public int LayerIndexByName(string layerName)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Executes SQL statement against datasource.
        /// </summary>
        /// <remarks>Should be used for non-SELECT instructions which don't return resulting set of rows.
        /// To select data use OgrDatasource.RunQuery instead.
        /// </remarks>        
        /// <param name="sql">SQL instruction.</param>
        /// <param name="errorMessage">Error message provided in case of failure.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// The following code deletes records from underlying database table with gid > 100.
        /// \code
        /// var ds = new OgrDatasource();
        /// 
        /// if (!ds.Open(CONNECTION_STRING))
        /// {
        ///     Debug.Pring("Failed to establish connection: " + ds.GdalLastErrorMsg);
        /// }
        /// else
        /// {
        ///     string errorMsg;
        ///     bool result = ds.ExecuteSQL("DELETE FROM tableName WHERE gid > 100", out errorMsg);
        ///     if (!result)
        ///     {
        ///         Debug.Pring("Error on running SQL: " + errorMsg);    
        ///     }
        ///     else
        ///     {
        ///         Debug.Pring("SQL was executed successfully.");    
        ///     }
        ///     ds.Close();
        /// }
        /// \endcode
        public bool ExecuteSQL(string sql, out string errorMessage)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the array with schema names in the datasources.
        /// </summary>
        /// <returns>Boxed string array.</returns>
        /// \new494 Added in version 4.9.4
        public object GetSchemas()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the description of the specific error code.
        /// </summary>
        /// <param name="ErrorCode">The error code returned by LastErrorCode property.</param>
        /// <returns>String with the description.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Gets code of the last error which took place inside this object.
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a Callback object which handles progress and error messages.
        /// </summary>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a text string associated with object. Any value can be stored by developer in this property.
        /// </summary>
        public string Key { get; set; }

        /// <summary>
        /// Gets number of layers in the datasource.
        /// </summary>
        public int LayerCount { get; private set; }
        
        /// <summary>
        /// Gets name driver (name of format) for this datasource.
        /// </summary>
        public string DriverName { get; private set; }

        /// <summary>
        /// Returns certain type of metadata associated with current driver.
        /// </summary>
        /// <param name="metadata">Type of metadata to be returned.</param>
        /// <returns>Metadata or empty string if requested type of metadata isn't set for the driver.</returns>
        /// \code
        /// var ds = new OgrDatasource();
        /// 
        /// if (!ds.Open(CONNECTION_STRING))
        /// {
        ///     Debug.Print("Failed to establish connection: " + ds.GdalLastErrorMsg);
        /// }
        /// else
        /// {
        ///     // first display couple of specific items
        ///     Debug.Print("Layer creation options: " + ds.get_DriverMetadata(tkGdalDriverMetadata.dmdLAYER_CREATIONOPTIONLIST));
        ///     Debug.Print("Long name: " + ds.get_DriverMetadata(tkGdalDriverMetadata.dmdLONGNAME));
        /// 
        ///     // now display all the available items
        ///     Debug.Print("Metadata items: ");
        ///     for (int i = 0; i < ds.DriverMetadataCount; i++)
        ///     {
        ///         Debug.Print(ds.get_DriverMetadataItem(i));
        ///     }
        ///     ds.Close();
        /// }
        /// \endcode
        public string get_DriverMetadata(tkGdalDriverMetadata metadata)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of metadata items associated with current driver.
        /// </summary>
        public int DriverMetadataCount { get; private set; }
        
        /// <summary>
        /// Gets metadata item with specified index associated with current driver.
        /// </summary>
        /// <param name="metadataIndex">Index of metadata item to be returned.</param>
        /// <returns>String with metadata.</returns>
        public string get_DriverMetadataItem(int metadataIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new layer in the datasource and populates it with data from specified shapefile.
        /// </summary>
        /// <remarks>
        /// The list of available creation options can be found in GDAL on-line <a href = http://www.gdal.org/ogr_formats.html> 
        /// documentation</a> or retrieved by calling OgrDatasource.get_DriverMetadata(tkGdalDriverMetadata.dmdLAYER_CREATIONOPTIONLIST). \n\n
        /// In addition MapWinGIS defines its own options:
        /// - MW_MULTI_PART={YES, NO} - sets whether single part or multipart geometry type column will
        /// be created to store data. The default value is "YES", i.e. MultiLineString will be used 
        /// for polylines and MultiPolygon for polygons. "NO" option will result in LineString and Polygon 
        /// types respectively, which won't allow multi-part polylines or polygons with multiple outer rings.
        /// - MW_POSTGIS_VACUUM={YES, NO} - indicates whether a VACUUM operation should be called after importing
        /// a layer into PostGIS database. The default value is "YES".\n
        /// 
        /// Depending on driver invalid geometries may be accepted in the datasource,
        /// but they may cause errors on trying to perform spatial queries against these layers later on.\n
        /// 
        /// Results of validation may be accessed via Shapefile.LastInputValidation of shapefile parameter.
        /// </remarks>
        /// <param name="shapefile">%Shapefile to import.</param>
        /// <param name="layerName">Name for the imported layer.</param>
        /// <param name="creationOptions">Format specific creation options separated with semicolon, e.g.
        /// <i>"OVERWRITE=YES;MW_MULTI_PART=no"</i>.
        /// </param>
        /// <param name="validationMode">Sets validation mode for input shapes,
        /// ranging from no validation to abort the whole operation on the first invalid shape.</param>
        /// <returns>True on success.</returns>
        /// The following code imports all shapefiles from the specified folder as layers into the datasource.
        /// \code
        /// private static string CONNECTION_STRING = "PG:host=localhost dbname=london user=postgres password=1234";
        /// 
        /// private static bool ImportShapefileFromFolder()
        /// {
        ///     var ds = new OgrDatasource();
        ///     
        ///     if (!ds.Open(CONNECTION_STRING))
        ///     {
        ///         Debug.Pring("Failed to establish connection: " + ds.GdalLastErrorMsg);
        ///     }
        ///     else
        ///     {
        ///         string path = @"d:\data\sf\london";
        ///         var files = Directory.GetFiles(path, "*.shp");
        ///         foreach (var file in files)
        ///         {
        ///             var sf = new Shapefile();
        ///             if (!sf.Open(file))
        ///             {
        ///                 Debug.Print("Failed to open shapefile: {0}\n{1}", file, sf.get_ErrorMsg(sf.LastErrorCode));
        ///             }
        ///             else
        ///             {
        ///                 string name = Path.GetFileNameWithoutExtension(file);
        ///                 if (!ds.ImportShapefile(sf, name, "OVERWRITE=YES", tkShapeValidationMode.NoValidation))
        ///                 {
        ///                     Debug.Print("Failed to import shapefile: " + name);
        ///                 }
        ///                 else
        ///                 {
        ///                     Debug.Print("Layer was imported: " + name);
        ///                     var layer = ds.GetLayerByName(name);
        ///                     if (layer != null)
        ///                     {
        ///                         Debug.Print("Imported features count: " + layer.FeatureCount);
        ///                         layer.Close();
        ///                     }
        ///                 }
        ///             }
        ///         }
        ///         ds.Close();
        ///     }
        ///     return true;
        /// }
        /// \endcode
        public bool ImportShapefile(Shapefile shapefile, string layerName, string creationOptions = "",
            tkShapeValidationMode validationMode = tkShapeValidationMode.TryFixSkipOnFailure)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new empty layer in the datasource.
        /// </summary>
        /// <remarks>
        /// SRID will be set to 0 if null or empty GeoProjection object is passed.
        /// See OgrDatasource.ImportShapefile for the details on creation options.\n
        /// 
        /// Known issues: creation of layer in nonexistent schema in PostGIS databases completes successfully, 
        /// yet the neither schema nor objects in it are visible afterwards.
        /// </param>
        /// </remarks>
        /// <param name="layerName">Name of the new layer.</param>
        /// <param name="ShpType">ShpType of the new layer. Will be converted to appropriate OGRwkbGeometryType.</param>
        /// <param name="Projection">%GeoProjection object which sets SRID of the new layer.</param>
        /// <param name="creationOptions">Format specific creation options separated with semicolon, e.g.
        /// "OVERWRITE=YES;MW_MULTI_PART=no". </param>
        /// <returns>True on success.</returns>
        public bool CreateLayer(string layerName, ShpfileType ShpType, GeoProjection Projection = null, string creationOptions = "")
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Extracts the last error message reported by GDAL library.
        /// </summary>
        public string GdalLastErrorMsg
        {
            get { throw new NotImplementedException(); }
        }
    }
#if nsp
}
#endif

