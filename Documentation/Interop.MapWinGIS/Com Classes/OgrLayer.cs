
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Represents a single layer in GDAL/OGR datasource or result of SQL query against such datasource.
    /// </summary>
    /// <remarks>
    /// \dot
    /// digraph ogr_layer_type {
    /// splines = true;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// ds [ label="OGR layer source"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" Vector file\l"];
    /// s2    [label=" Database table\l"];
    /// s3    [label=" SQL query\l"];
    /// 
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" minlen=2 ]
    /// ds -> s1;
    /// ds -> s2;
    /// ds -> s3;
    /// }
    /// \enddot
    /// 
    /// <b>A. How to open.</b>\n
    /// 
    /// There are 2 main ways to open a layer from OGR datasource: 
    /// - extract all the data by passing its name;
    /// - run SQL query and extract all or only a part of the data.\n
    /// 
    /// SQL query provides more flexibility, while opening layer as a whole may support editing if 
    /// forUpdate argument was passed and such capability is supported by driver.\n
    /// 
    /// Here is a summary of various ways to open a layer:
    /// \code
    /// private static string CONNECTION_STRING = "PG:host=localhost dbname=london user=postgres password=1234";
    /// 
    /// var layer = new OgrLayer();
    /// string layerName = "waterways";
    /// \endcode
    /// 
    /// 1) open one of exiting layers:
    /// \code
    /// if (!layer.OpenFromDatabase(CONNECTION_STRING, layerName))
    ///     Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
    /// \endcode
    /// 
    /// 2) return temporary layer by a query:
    /// \code
    /// if (!layer.OpenFromQuery(CONNECTION_STRING, "SELECT * FROM " + layerName))
    ///     Debug.Print("Failed to run a query: " + layer.get_ErrorMsg(layer.LastErrorCode));
    /// \endcode
    /// 
    /// 3) the same using datasource:
    /// \code
    /// var ds = new OgrDatasource();
    /// if (!ds.Open(CONNECTION_STRING))
    /// {    
    ///     Debug.Print("Failed to open datasource: " + ds.GdalLastErrorMsg);
    /// }
    /// else
    /// {
    ///     layer = ds.GetLayerByName(layerName);
    ///     if (layer == null)
    ///     {
    ///         Debug.Print("Failed to open layer: " + ds.get_ErrorMsg(ds.LastErrorCode));
    ///     }
    ///     
    ///     // or run a query
    ///     layer = ds.RunQuery("SELECT * FROM " + layerName);
    ///     if (layer == null)
    ///     {
    ///         Debug.Print("Failed to run a query: " + ds.get_ErrorMsg(ds.LastErrorCode));
    ///     }
    /// }
    /// \endcode
    /// 
    /// 4) using FileManager class:
    /// \code
    /// var fm = new FileManager();
    /// layer = fm.OpenFromDatabase(CONNECTION_STRING, layerName);   // layer name or query can be passed here
    /// if (layer = null)
    /// {
    ///     Debug.WriteLine("Failed to open layer: " + fm.get_ErrorMsg(fm.LastErrorCode));
    /// 
    ///     // let's check GDAL error as well
    ///     var gs = new GlobalSettings();
    ///     Debug.WriteLine("GDAL error message: " + gs.GdalLastErrorMsg);
    /// }
    /// \endcode
    /// 
    /// For spatial databases layer name corresponds to the name of underlying table with some driver specifics, like
    /// including name of database schema as a prefix or the name of the geometry column. Use OgrDatasource class to
    /// get the names of available layers.\n
    /// 
    /// <b>B. How to add to the map.</b>\n
    /// 
    /// Instances of %OgrLayer class can be added to the map directly using AxMap.AddLayer method or opened 
    /// internally using AxMap.OpenFromDatabase method. In each case OgrLayer.GetData method
    /// will be called automatically which triggers the loading of data from datasource. Afterwards underlying in-memory
    /// %shapefile will be used for all rendering purposes.\n
    /// 
    /// %OgrLayer added to the map can be accessed using AxMap.get_OgrLayer property.\n
    /// \code
    /// var layer = new OgrLayer();
    /// if (!layer.OpenFromDatabase(CONNECTION_STRING, layerName))
    /// {
    ///     Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
    ///     Debug.Print("GDAL last error: " + layer.GdalLastErrorMsg);
    /// }
    /// else
    /// {
    ///     map.RemoveAllLayers();
    ///     // either a) set layer projection to the map
    ///     map.GrabProjectionFromData = true;
    /// 
    ///     // or b) set map projection and reproject layer if needed
    ///     map.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
    ///     map.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;
    /// 
    ///     int layerHandle = map.AddLayer(layer, true);
    ///     if (layerHandle == -1)
    ///     {
    ///         Debug.Print("Failed to add layer to the map");
    ///     }
    ///     else
    ///     {
    ///         map.Redraw();
    /// 
    ///         // get the reference back from map
    ///         var sameLayerFromMap = map.get_OgrLayer(layerHandle);
    ///     }
    /// }
    /// \endcode
    /// AxMap.AddLayerFromDatabase provides a short-cut for both 2 operations: opening a layer and adding it to the map.\n
    /// 
    /// OGR layers support AxMap.ProjectionMismatchBehavior and AxMap.GrabProjectionFromData properties.
    /// Depending on their values underlying shapefile data 
    /// may be reprojected on adding it to the map. Use OgrLayer.DataIsReprojected property to test
    /// whether it happened.\n
    /// 
    /// OGR layers support map state serialization with AxMap.SerializeMapState. Their data will
    /// be reloaded after AxMap.DeserializeMapState is called.\n
    /// 
    /// <b>C. How to access the data.</b>\n
    /// 
    /// %OgrLayer uses in-memory %shapefile to provide its data to clients. \n
    /// 
    /// This %shapefile is populated on the first call to OgrLayer.GetData method. On subsequent calls 
    /// cached values will be used (lazy loading pattern). \n
    /// 
    /// \code
    /// var layer = new OgrLayer();
    /// if (layer.OpenFromDatabase(CONNECTION_STRING, layerName))
    /// {
    ///     // let's first try to extract some info without loading the data locally
    ///     Extents ext = null;
    ///     if (layer.get_Extents(out ext))
    ///     {
    ///         Debug.WriteLine("Extents w/o loading the data: " + ext.ToDebugString());
    ///     }
    ///     Debug.WriteLine("Feature count: " + layer.FeatureCount);
    /// 
    ///     // now load the data locally and display the same info
    ///     var shapefile = layer.GetData();
    ///     if (shapefile != null)
    ///     {
    ///         Debug.WriteLine("Extents from the loaded data: " + shapefile.Extents.ToDebugString());
    ///         Debug.WriteLine("Shape count: " + shapefile.NumShapes);
    /// 
    ///         Debug.WriteLine("Layer fields: ");
    ///         for (int i = 0; i < shapefile.NumFields; i++)
    ///         {
    ///             Debug.WriteLine(shapefile.get_Field(i).Name);
    ///         }
    ///     }
    /// }
    /// \endcode
    /// 
    /// OgrLayer.GetData method maps:
    /// - OGR geometry types -> shape types;
    /// - OGR features -> shape records (i.e. shape + associated attributes);
    /// - OGR geometries -> instances of Shape class;
    /// - OGR fields -> instances of Field class in attribute table of %shapefile.
    /// .
    /// %OgrLayer may support database tables without geometry column. For example, all tables in PostGreSQL database will
    /// be listed as layers if no PostGIS support was added to the database. In such case table records will
    /// be converted to rows of attribute table, while shapefile will contain empty shapes with Shape.ShapeType = SHP_NULLSHAPE.\n
    /// 
    /// AxMap.get_GetObject and AxMap.get_Shapefile will return underlying in-memory %shapefile. This allows to use  
    /// unified code for processing both regular shapefile layers and OGR layers and ensures 
    /// compatibility of OGR layers with previously written client code.\n
    /// 
    /// %OgrLayer encapsulates an instance of GDAL's %OGRLayer C++ class. Check its 
    /// <a href = "http://www.gdal.org/classOGRLayer.html">documentation</a> to better understand what's going on under the hood. \n
    /// 
    /// <b>D. How to edit the data.</b>\n
    /// 
    /// \note See description of editing in <a href = "group__ogrlayer__editing.html#details">this section</a>.\n
    /// </remarks>
    /// \n Here is a diagram for the %OgrLayer class.
    /// \dot digraph ogr_diagram {
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
    /// lyr -> sf [ URL="\ref OgrLayer.GetData()", tooltip = "OgrLayer.GetData()", headlabel = "   1"];
    /// ds -> lyr [URL="\ref OgrDatasource.GetLayer()", tooltip = "OgrDatasource.GetLayer()", headlabel = "   n"]
    /// }
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>
    /// 
    /// \new492b Added in version 4.9.2
#if nsp
    #if upd
        public class OgrLayer : MapWinGIS.IOgrLayer
    #else        
            public class IOgrLayer
    #endif
#else
        public class OgrLayer
#endif
    {
        /// <summary>
        /// Closes current layer and releases resources associated with it.
        /// </summary>
        /// <remarks>It's recommended to call this method as soon as the layer is no longer needed.</remarks>
        public void Close()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns connection string which was used to open this layer.
        /// </summary>
        /// <remarks>
        /// \attention Password in connection string is stored as plain text. 
        /// Consider measures to ensure its security.
        /// </remarks>
        public string GetConnectionString()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restores the state of layer from string generated with OgrLayer.Serialize method.
        /// </summary>
        /// <remarks>Deserialization includes:\n
        /// a) reconnection to the datasource;\n
        /// b) grabbing data from it;\n 
        /// c) restoring of visualization options set for underlying shapefile.\n\n
        /// Particular set of actions depends on the state of the object before serialization.\n
        /// 
        /// Any data currently stored in this instance will be discarded.
        /// </remarks>
        /// <param name="newVal">String generated with OgrLayer.Serialize method.</param>
        /// <returns>True on success.</returns>
        public bool Deserialize(string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets GeoProjection associated with current layer. 
        /// </summary>
        /// <remarks>
        /// Corresponds to SRID set for the layer in underlying datasource.
        /// When SRID isn't specified (i.e. equals 0), empty GeoProjection instance will be returned.</remarks>
        public GeoProjection GeoProjection
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the name of geometry column which was used to fetch geometry for current layer.
        /// </summary>
        /// <remarks>Depending on data format, a layer may support several geometry columns but 
        /// only one will be used to provide shape data via OgrLayer.GetData(). By default
        /// the first column with geometry/geography type will be used. To access other columns 
        /// temporary layers can be opened via OgrDatasource.RunQuery.</remarks>
        public string GeometryColumnName
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets layer data represented as in-memory shapefile.
        /// </summary>
        /// <remarks>
        /// Implements lazy loading pattern, i.e. data will be grabbed from underlying datasource 
        /// on the first call and will be used for all subsequent calls. To force re-read of the 
        /// data from datasource use OgrLayer.ReloadFromSource.\n
        /// 
        /// This method will automatically be called after adding the layer to the map via
        /// AxMap.AddLayer or AxMap.AddLayerFromDatabase.
        /// </remarks>
        /// <returns>Instance of shapefile with layer data or null on failure or for uninitialized layer.</returns>
        public Shapefile GetData()
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
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// A text string associated with object. Any value can be stored by developer in this property.
        /// </summary>
        public string Key { get; set; }

        /// <summary>
        /// Gets name of the layer.
        /// </summary>
        /// <remarks>The name may correspond to table name in underlying database or store some generic string like 
        /// "sql_statement" for temporary layers opened by OgrDatasource.RunQuery.</remarks>
        public string Name
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Opens layer with specified name from the OGR datasource.
        /// </summary>
        /// <remarks>
        /// This method is called internally by OgrDatasource.GetLayerByName.
        /// </remarks>
        /// <param name="connectionString">Connection string or filename.</param>
        /// <param name="layerName">Layer name.</param>
        /// <param name="forUpdate">Indicates whether the returned layer will support saving of changes back to source
        /// (the functionality should be supported by particular driver).</param>
        /// <returns>True on success.</returns>
        public bool OpenFromDatabase(string connectionString, string layerName, bool forUpdate)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Runs SQL query against datasource and returns results as a temporary layer.
        /// </summary>
        /// <remarks>This method is called internally by OgrDatasource.RunQuery.</remarks>
        /// <param name="connectionString">Connection string or filename.</param>
        /// <param name="sql">SQL query.</param>
        /// <returns>True on success.</returns>
        public bool OpenFromQuery(string connectionString, string sql)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Runs a new query against the datasource.
        /// </summary>
        /// <remarks>The layer must be opened with OgrLayer.OpenFromQuery or OgrDatasource.RunQuery 
        /// (OgrLayer.SourceType = ogrQuery) in order for this method to work.</remarks>
        /// <param name="newSql">New SQL command.</param>
        /// <returns>True on success.</returns>
        public bool RedefineQuery(string newSql)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Discards all the local changes and reloads layer from the source.
        /// </summary>
        /// <returns>True on success.</returns>
        public bool ReloadFromSource()
        {
            throw new NotImplementedException();
        }

       

        /// <summary>
        /// Serializes the state of layer to a string, which can be later restored with OgrLayer.Deserialize.
        /// </summary>
        /// <remarks>State information includes connection string, layer name (or query string)
        /// and visualization options for underlying shapefile 
        /// (in case it is already populated when the method was called.)
        /// \attention Serialized string holds connection password as a plain text. 
        /// Consider measures to ensure its security.
        /// </remarks>
        /// <returns>String with state information.</returns>
        public string Serialize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets shape type of the current layer.
        /// </summary>
        /// <remarks>The property automatically maps underlying OGRwkbGeometryType to corresponding shape type.</remarks>
        public ShpfileType ShapeType
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets layer name or SQL query which was used to open this layer.
        /// </summary>
        public string GetSourceQuery()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Test whether current layer supports certain functionality. 
        /// </summary>
        /// <remarks>In most cases no actual attempts to perform requested operation is made.</remarks>
        /// <param name="capability">A capability to test.</param>
        /// <returns>True in case the capability is supported by the layer.</returns>
        /// The following code opens a layer from datasource and display which capabilities are supported for it.
        /// \code
        /// var layer = new OgrLayer();
        /// if (!layer.OpenFromDatabase(CONNECTION_STRING, "waterways", true))
        /// {
        ///     Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
        /// }
        /// else
        /// {
        ///     var values = Enum.GetValues(typeof(MapWinGIS.tkOgrLayerCapability));
        ///     foreach (tkOgrLayerCapability value in values)
        ///     {
        ///         Debug.Print(value.ToString() + ": " + layer.TestCapability(value).ToString());
        ///     }
        /// }
        /// \endcode
        public bool TestCapability(tkOgrLayerCapability capability)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets extents of the layer.
        /// </summary>
        /// <remarks>
        /// Depending on driver implementation this method may retrieve this information
        /// directly from underlying datasource without loading the data, which provides 
        /// performance benefits.
        /// </remarks>
        /// <param name="layerExtents">Retrieved extents.</param>
        /// <param name="forceLoading">True to instruct driver to load the data locally 
        /// if information can't be retrieved from underlying datasource otherwise.</param>
        /// <returns>True on success.</returns>
        public bool get_Extents(out Extents layerExtents, bool forceLoading = false)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns number of features in the layer.
        /// </summary>
        /// <remarks>
        /// Depending on driver implementation this method may retrieve this information
        /// directly from underlying datasource without loading the data, which provides 
        /// performance benefits.
        /// </remarks>
        /// <param name="forceLoading">True to instruct driver to load the data locally 
        /// if information can't be retrieved from underlying datasource otherwise.</param>
        /// <returns>Number of features in the layer.</returns>
        public int get_FeatureCount(bool forceLoading = false)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets source type of the layer. 
        /// </summary>
        /// <remarks>Any new instance of class starts with ogrUninitialized. Successful call of OgrLayer.OpenFromDatabase
        /// method will set it to ogrDbTable, OgrLayer.OpenFromQuery - to ogrQuery.</remarks>
        /// <returns>Source type.</returns>
        public tkOgrSourceType get_SourceType()
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

        /// \addtogroup ogrlayer_editing OGR layer editing
        /// Here is a list of methods and properties to save changes made to OgrLayer back to datasource.
        /// The properties and methods described here belong to OgrLayer class.\n
        /// 
        ///  \dot
        /// digraph ogrediting {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="OgrLayer" URL="\ref OgrLayer"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="OGR layer editing"   URL="\ref ogrlayer_editing"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>\n
        /// 
        /// Editing of underlying in-memory %shapefile can be done for all types of OGR layers. \n
        /// 
        /// However saving of these changes back to datasource isn't always possible because of following reasons:\n
        /// - it isn't supported by driver;
        /// - forUpdate flag parameter wasn't specified on opening the layer;
        /// - layer was opened from SQL query;
        /// - data was reprojected during adding to the map;
        /// - layer doesn't have feature ID column.\n
        /// 
        /// Use OgrLayer.get_SupportsEditing to check if saving is possible for current layer.\n
        /// 
        /// Feature ID column corresponds to primary key in database table.
        /// Such column will be added as first field in attribute table of %shapefile (with index 0). This field 
        /// must not be edited by client code. The presence of feature ID column can be tested with OgrLayer.FidColumnName 
        /// property which will return empty string if no such column exists.\n
        /// 
        /// To save the changes to datasource OgrLayer.SaveChanges method should be called.
        /// It supports saving of the following types of changes (provided that other preconditions are met ):
        /// - editing of values in attribute table;
        /// - any editing of geometry of individual shapes;
        /// - adding of new shapes;
        /// - deleting of exiting shapes.\n
        /// 
        /// \note OgrLayer doesn't support any changes in order or number of fields in attribute table. Therefore
        /// fields must not be added or deleted in order for OgrLayer.SaveChanges to work.\n
        /// 
        /// Edited shapes must be marked with modified flag via Shapefile.set_ShapeModified. See code sample in OgrLayer.SaveChanges
        /// documentation.\n
        /// 
        /// Errors during the saving operation are registered in the log which can be accessed using OgrLayer.get_UpdateSourceErrorMsg.\n
        /// 
        /// OgrLayer.SaveChanges method may change the order of rows in the database table 
        /// (for example PostgreSQL driver places the updated rows in the end of table). 
        /// Therefore on reloading of layer after the SaveChanges operation indices of shapes may change.
        /// 
        /// The following code demonstrates how various types of editing for OGR layer can be made
        /// with saving of changes back to the datasource.
        /// \code
        /// private static string CONNECTION_STRING = "PG:host=localhost dbname=london user=postgres password=1234";
        /// 
        /// var layer = new OgrLayer();
        /// layer.GlobalCallback = form;
        /// 
        /// bool forUpdate = true;
        /// if (!layer.OpenFromDatabase(CONNECTION_STRING, "buildings", forUpdate))
        /// {
        ///     Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
        ///     return false;
        /// }
        /// 
        /// // check if editing is supported for driver
        /// Debug.Print("Driver supports editing: " + layer.TestCapability(tkOgrLayerCapability.olcRandomWrite));
        /// 
        /// now check if we can actually do it, as there can be other limitations
        /// 
        /// if (!layer.get_SupportsEditing(tkOgrSaveType.ostSaveAll))
        /// {
        ///     Debug.Print("Can't edit a layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
        ///     layer.Close();
        ///     return false;
        /// }
        /// 
        /// var sf = layer.GetData();
        /// if (sf != null)
        /// {
        ///     // possible types of editing
        ///     bool editValue = true;
        ///     bool addShape = true;
        ///     bool editShape = true;
        ///     bool removeShape = true;
        /// 
        ///     if (editValue)
        ///     {
        ///         int shapeIndex = 0;
        ///         int fieldIndex = 2;
        ///         object val = sf.get_CellValue(fieldIndex, shapeIndex);
        ///         sf.EditCellValue(fieldIndex, shapeIndex, "test_writing");
        /// 
        ///         // this flag will notify the driver that changes should saved back to source
        ///         sf.ShapeModified[shapeIndex] = true;
        ///     }
        /// 
        ///     if (addShape)
        ///     {
        ///         int shapeIndex = sf.NumShapes;
        ///         var shp = sf.get_Shape(0);
        ///         shp = shp.Buffer(1, 50);
        /// 
        ///         // modified flag is set automatically in this case
        ///         bool result = sf.EditInsertShape(shp, ref shapeIndex);
        ///         Debug.Print("Shape was inserted: " + result);
        ///     }
        /// 
        ///     if (editShape)
        ///     {
        ///         // since shapefile is in in-memory mode, geometry of shapes can be changed directly;
        ///         // bear in mind that this won't work for file-based shapefiles, in that case get_Shape will
        ///         // populate Shape object which will have no further link with parent shapefile
        ///         var shp = sf.get_Shape(sf.NumShapes - 1);
        ///         for (int i = 0; i < shp.NumPoints; i++)
        ///         {
        ///             double x = 0.0, y = 0.0;
        ///             if (shp.get_XY(i, ref x, ref y))
        ///                 shp.put_XY(i, x + 0.01, y + 0.01);  // let's move it a little
        ///         }
        ///     }
        /// 
        ///     if (removeShape)
        ///     {
        ///         bool result = sf.EditDeleteShape(sf.NumShapes - 1);
        ///         Debug.Print("Shape was deleted: " + result);
        ///     }
        /// 
        ///     // saving it
        ///     int count;
        ///     var saveResults = layer.SaveChanges(out count);
        /// 
        ///     Debug.Print("Save result: " + saveResults.ToString());
        ///     Debug.Print("Number of shapes saved: " + count);
        /// 
        ///     // displaying info on errors
        ///     for (int i = 0; i < layer.UpdateSourceErrorCount; i++)
        ///     {
        ///         Debug.Print(string.Format("Error for shape id {0}: {1}",
        ///         layer.UpdateSourceErrorShapeIndex[i], layer.UpdateSourceErrorMsg[i]));
        ///     }
        ///     return true;
        /// }
        /// layer.Close();
        /// return false;
        /// \endcode
        /// @{
        
        /// <summary>
        /// Gets a value indicating whether the layer supports editing.
        /// </summary>
        /// <remarks>
        /// The property works like this:\n
        /// 1) checks whether underlying driver supports random write operation:
        /// OgrLayer.TestCapability(tkOgrLayerCapability::olcRandomWrite);\n
        /// 2) the presence of Feature Id column is verified;\n
        /// 3) checks if the data is still in the same projection (OgrLayer.DataIsReprojected).\n
        /// 
        /// OgrLayer.SaveChanges method doesn't make the first check, thus trying to write
        /// the data even if functionality isn't advertised by driver. 
        /// </remarks>
        /// <param name="editingType">The requested type of editing.</param>
        /// <returns>True in case editing is supported.</returns>
        public bool get_SupportsEditing(tkOgrSaveType editingType)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a specified error message from the log registered during OgrLayer.SaveChanges call.
        /// </summary>
        /// <param name="errorIndex">Index of error.</param>
        /// <returns>Error message or empty string on invalid index.</returns>
        public string get_UpdateSourceErrorMsg(int errorIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets shape index associated with specified error message 
        /// from the log registered during OgrLayer.SaveChanges call.
        /// </summary>
        /// <param name="errorIndex">Error index.</param>
        /// <returns>Index of shape in underlying in-memory shapefile (OgrLayer.GetData()).</returns>
        public int get_UpdateSourceErrorShapeIndex(int errorIndex)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Gets the number of errors registered in the log during OgrLayer.SaveChanges call.
        /// </summary>
        public int UpdateSourceErrorCount
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets name of feature ID column.
        /// </summary>
        /// <remarks>Feature ID column corresponds to primary key in underlying database table.
        /// It is used to uniquely identify features and to save changes back to datasource.
        /// Feature ID column (if present) will always be inserted as a first field of attribute 
        /// table of underlying shapefile. This field must not be edited.\n
        /// 
        /// For inserted features the column values are set to NULL. \n
        /// 
        /// In case underlying datasource doesn't have feature ID column, an empty string will be returned.</remarks>
        public string FIDColumnName
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a value indicating whether underlying data ( OgrLayer.GetData ) was reprojected.
        /// </summary>
        /// <remarks>
        /// This may happen because of projection mismatch on adding it to the map. See AxMap.ProjectionMismatchBehavior for details.
        /// </remarks>
        public bool DataIsReprojected
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Saves local changes to the datasource.
        /// </summary>
        /// <remarks>
        /// To check whether the operation is supported for current layer use OgrLayer.get_SupportsEditing.\n
        /// 
        /// The method works like this:\n
        /// 
        /// 1) Underlying shapefile is analysed for changes, i.e. for shapes with Shapefile.get_ShapeModified property set to true. \n
        /// 2) For each of such shapes UPDATE statement is generated by driver. 
        /// Shapes are identified in source by the value of Feature ID column.\n
        /// 3) If update operation for particular shape fails the error is registered in:
        /// OgrLayer.get_UpdateSourceErrorMsg().\n
        /// 
        /// The operation may fail for a particular shape because of 2 main reasons:
        /// - shape is invalid, while validateShapes parameter set to true;
        /// - new values aren't accepted by datasource, which often can maintain stricter data constraints.
        /// </remarks>
        /// <param name="savedCount">Returns number of saved changed.</param>
        /// <param name="saveType">Sets which part of data should be saved, geometry, attributes or both.
        /// Default value is tkOgrSaveType.ostSaveAll (i.e. both geometry and attributes).</param>
        /// <param name="validateShapes">Sets whether shapes will be validated before saving. Default value is true,
        /// i.e. invalid shapes won't be saved.</param>
        /// <returns>Result of the operation.</returns>
        public tkOgrSaveResult SaveChanges(out int savedCount, tkOgrSaveType saveType = tkOgrSaveType.ostSaveAll, bool validateShapes = true)
        {
            throw new NotImplementedException();
        }

        /// @}
        #endregion
    }
#if nsp
}
#endif

