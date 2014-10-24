// --------------------------------------------------------------------------------------------------------------------
// <copyright file="OgrLayerTests.cs" company="MapWindow Open Source GIS">
//   MapWindow developers community - 2014
// </copyright>
// <summary>
//   The ogr layer tests.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace TestApplication
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.IO;
    using System.Threading;
    using System.Windows.Forms;

    using AxMapWinGIS;

    using MapWinGIS;

    /// <summary>
    ///     The ogr layer tests.
    /// </summary>
    public static class OgrLayerTests
    {
        #region Static Fields

        /// <summary>
        ///     The connection string.
        /// </summary>
        private static string CONNECTION_STRING = "PG:host=localhost dbname=aw_ebrp user=aw_ebrp password=test123";

        /// <summary>
        ///     The form.
        /// </summary>
        private static Form1 form;

        #endregion

        #region Properties

        /// <summary>
        ///     Gets or sets Map.
        /// </summary>
        internal static AxMap Map { get; set; }

        #endregion

        #region Methods

        /// <summary>
        /// The run ogr layer test.
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="theForm">
        /// The the form.
        /// </param>
        /// <returns>
        /// The <see cref="bool"/>.
        /// </returns>
        internal static bool RunOgrLayerTest(string textfileLocation, Form1 theForm)
        {
            form = theForm;
            Map = Fileformats.Map;

            Debug.Print("\nStart OGR layer test");

            // TestFileManager();
            TestReadLayer("fields");

            // TestReprojection();

            // TestEditLayer();

            // TestDeserializeMap();
            AddLayer2Map("SELECT * FROM fields limit 10000");

            Debug.Print("\nEnd OGR layer test");
            return true;
        }

        /// <summary>
        /// The run post gis create database.
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="theForm">
        /// The form.
        /// </param>
        /// <returns>
        /// The <see cref="bool"/>.
        /// </returns>
        internal static bool RunPostGisCreateDatabase(string textfileLocation, Form1 theForm)
        {
            // Open text file:
            if (!File.Exists(textfileLocation))
            {
                throw new FileNotFoundException("Cannot find text file.", textfileLocation);
            }

            var numErrors = 0;

            theForm.Progress("-----------------------The creation of a PostGIS database has started." + Environment.NewLine);

            // Read text file:
            var lines = Helper.ReadTextfile(textfileLocation);

            // First line is connection string:
            var connectionString = lines[0];
            if (!connectionString.StartsWith("PG"))
            {
                throw new Exception(
                    "Input file is not correct. The first line should be the connection string, starting with PG");
            }

            var ds = new OgrDatasource();
            if (!ds.Open(connectionString))
            {
                throw new Exception("Failed to open datasource: " + ds.GdalLastErrorMsg);
            }

            // Get every first and second line:
            for (var i = 1; i < lines.Count; i++)
            {
                var query = lines[i];
                string errorMsg;
                if (!ds.ExecuteSQL(query, out errorMsg))
                {
                    if (ds.GdalLastErrorMsg.Contains("cannot run inside a transaction block"))
                    {
                        errorMsg += " You're probably using a too old version of GDAL v2, please update.";
                    }


                    theForm.WriteError(string.Format("Error executing query [{0}]: {1}", query, errorMsg));
                    numErrors++;
                }
            }

            // Close database connection:
            ds.Close();

            theForm.Progress(string.Format("The postGIS create database test has finished, with {0} errors", numErrors));

            return numErrors == 0;
        }

        /// <summary>
        /// The add layer 2 map.
        /// </summary>
        /// <param name="query">
        /// The query.
        /// </param>
        private static void AddLayer2Map(string query)
        {
            var gs = new GlobalSettings();

            var layer = new OgrLayer();

            if (!layer.OpenFromQuery(CONNECTION_STRING, query))
            {
                Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
                Debug.Print("GDAL last error: " + layer.GdalLastErrorMsg);
            }
            else
            {
                Map.RemoveAllLayers();

                // either a) set layer projection to the map
                Map.GrabProjectionFromData = true;

                // or b) set map projection and reproject layer if needed
                Map.Projection = tkMapProjection.PROJECTION_WGS84;
                Map.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

                int layerHandle = Map.AddLayer(layer, true);
                if (layerHandle == -1)
                {
                    Debug.Print("Failed to add layer to the map");
                }
                else
                {
                    Map.Redraw();

                    // get the reference back from map
                    OgrLayer sameLayerFromMap = Map.get_OgrLayer(layerHandle);
                }
            }
        }

        /// <summary>
        ///     The display underlying data.
        /// </summary>
        private static void DisplayUnderlyingData()
        {
            string layerName = "fields";

            var layer = new OgrLayer();
            if (layer.OpenFromDatabase(CONNECTION_STRING, layerName, false))
            {
                // let's frist try to extract some info without loading the data locally
                Extents ext = null;
                if (layer.get_Extents(out ext))
                {
                    Debug.WriteLine("Extents w/o loading the data: " + ext.ToDebugString());
                }

                Debug.WriteLine("Feature count: " + layer.FeatureCount);

                // now load the data locally and display the same info
                Shapefile shapefile = layer.GetData();
                if (shapefile != null)
                {
                    Debug.WriteLine("Extents from the loaded data: " + shapefile.Extents.ToDebugString());

                    Debug.WriteLine("Shape count: " + shapefile.NumShapes);

                    Debug.WriteLine("Layer fields: ");
                    for (int i = 0; i < shapefile.NumFields; i++)
                    {
                        Debug.WriteLine(shapefile.get_Field(i).Name);
                    }
                }
            }
        }

        /// <summary>
        ///     The test deserialize.
        /// </summary>
        /// <returns>
        ///     The <see cref="bool" />.
        /// </returns>
        private static bool TestDeserialize()
        {
            string filename = @"d:\ogrlayer.xml";
            using (var reader = new StreamReader(filename))
            {
                string state = reader.ReadToEnd();
                var layer = new OgrLayer();
                if (!layer.Deserialize(state))
                {
                    Debug.Print("Failed to deserialize layer");
                    return false;
                }

                AxMap map = Fileformats.Map;
                map.RemoveAllLayers();
                map.Projection = tkMapProjection.PROJECTION_WGS84;
                map.AddLayer(layer, true);
            }

            return true;
        }

        /// <summary>
        /// The test deserialize map.
        /// </summary>
        /// <param name="layername">
        /// The layername.
        /// </param>
        /// <returns>
        /// The <see cref="bool"/>.
        /// </returns>
        private static bool TestDeserializeMap(string layername)
        {
            TestReadLayer(layername);

            string state = Map.SerializeMapState(false, string.Empty);
            Map.RemoveAllLayers();
            Map.Redraw();
            Application.DoEvents();
            Thread.Sleep(4000);

            Map.DeserializeMapState(state, true, string.Empty);
            Map.Redraw();
            return true;
        }

        /// <summary>
        ///     The test edit layer.
        /// </summary>
        /// <returns>
        ///     The <see cref="bool" />.
        /// </returns>
        private static bool TestEditLayer()
        {
            var layer = new OgrLayer();
            layer.GlobalCallback = form;

            if (!layer.OpenFromDatabase(CONNECTION_STRING, "buildings", true))
            {
                Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
                return false;
            }

            // check if editing is supported for driver
            Debug.Print("Driver supports editing: " + layer.TestCapability(tkOgrLayerCapability.olcRandomWrite));

            // now check if we can actually do it, as there can be other limitations
            if (!layer.get_SupportsEditing(tkOgrSaveType.ostSaveAll))
            {
                Debug.Print("Can't edit a layer: " + layer.get_ErrorMsg(layer.LastErrorCode));

                layer.Close();
                return false;
            }

            Shapefile sf = layer.GetData();
            if (sf != null)
            {
                // possible types of editing
                bool editValue = true;
                bool addShape = true;
                bool editShape = true;
                bool removeShape = true;

                if (editValue)
                {
                    int shapeIndex = 0;
                    int fieldIndex = 2;
                    object val = sf.get_CellValue(fieldIndex, shapeIndex);
                    sf.EditCellValue(fieldIndex, shapeIndex, "test_writing");

                    // this flag will notify the driver that changes should saved back to source
                    sf.ShapeModified[shapeIndex] = true;
                }

                if (addShape)
                {
                    int shapeIndex = sf.NumShapes;
                    Shape shp = sf.get_Shape(0);
                    shp = shp.Buffer(1, 50);

                    // modified flag is set automatically in this case
                    bool result = sf.EditInsertShape(shp, ref shapeIndex);
                    Debug.Print("Shape was inserted: " + result);
                }

                if (editShape)
                {
                    // since shapefile is in in-memory mode, geometry of shapes can be changed directly;
                    // bear in mind that this won't work for file-based shapefiles, in that case get_Shape will
                    // populate Shape object which will have no futher link with parent shapefile
                    Shape shp = sf.get_Shape(sf.NumShapes - 1);
                    for (int i = 0; i < shp.numPoints; i++)
                    {
                        double x = 0.0, y = 0.0;
                        if (shp.get_XY(i, ref x, ref y))
                        {
                            shp.put_XY(i, x + 0.01, y + 0.01); // let's move it a little
                        }
                    }
                }

                if (removeShape)
                {
                    bool result = sf.EditDeleteShape(sf.NumShapes - 1);
                    Debug.Print("Shape was deleted: " + result);
                }

                // saving it
                int count;
                tkOgrSaveResult saveResults = layer.SaveChanges(out count);

                Debug.Print("Save result: " + saveResults);
                Debug.Print("Number of shapes saved: " + count);

                // displaying info on errors
                for (int i = 0; i < layer.UpdateSourceErrorCount; i++)
                {
                    Debug.Print(
                        "Error for shape id {0}: {1}", 
                        layer.UpdateSourceErrorShapeIndex[i], 
                        layer.UpdateSourceErrorMsg[i]);
                }

                return true;
            }

            layer.Close();
            return false;
        }

        /// <summary>
        ///     The test file manager.
        /// </summary>
        private static void TestFileManager()
        {
            string sql = "SELECT * FROM Buildings WHERE gid < 50";
            int handle = Map.AddLayerFromDatabase(CONNECTION_STRING, sql, true);
            if (handle == -1)
            {
                Debug.Print("Failed to open layer: " + Map.FileManager.get_ErrorMsg(Map.FileManager.LastErrorCode));

                // in case the reason of failure is still unclear, let's ask GDAL for details
                var gs = new GlobalSettings();
                Debug.Print("Last GDAL error: " + gs.GdalLastErrorMsg);
            }
            else
            {
                OgrLayer l = Map.get_OgrLayer(handle);
                if (l != null)
                {
                    Debug.Print("Number of features: " + l.FeatureCount);

                    // now access the data
                    Shapefile sf = l.GetData();
                    Debug.Print("Number of shapes: " + sf.NumShapes);
                }
            }
        }

        /// <summary>
        ///     The test layer capabilities.
        /// </summary>
        private static void TestLayerCapabilities()
        {
            var layer = new OgrLayer();
            if (!layer.OpenFromDatabase(CONNECTION_STRING, "waterways", true))
            {
                Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
            }
            else
            {
                Array values = Enum.GetValues(typeof(tkOgrLayerCapability));
                foreach (tkOgrLayerCapability value in values)
                {
                    Debug.Print(value + ": " + layer.TestCapability(value));
                }
            }
        }

        /// <summary>
        /// The test read layer.
        /// </summary>
        /// <param name="layerName">
        /// The layer name.
        /// </param>
        /// <returns>
        /// The <see cref="bool"/>.
        /// </returns>
        private static bool TestReadLayer(string layerName)
        {
            var ds = new OgrDatasource();

            if (!ds.Open(CONNECTION_STRING))
            {
                Debug.Print("Failed to establish connection: " + ds.GdalLastErrorMsg);
            }
            else
            {
                OgrLayer layer = ds.GetLayerByName(layerName, true);

                if (layer != null)
                {
                    layer.GlobalCallback = form;
                    Debug.Print("Layer opened: " + layer.Name);

                    Extents ext;
                    if (layer.get_Extents(out ext))
                    {
                        Debug.Print(ext.ToDebugString());
                    }

                    Debug.Print("Geometry column name: " + layer.GeometryColumnName);
                    Debug.Print("Feature count: " + layer.FeatureCount);
                    Debug.Print("Supports editing: " + layer.get_SupportsEditing(tkOgrSaveType.ostSaveAll));

                    Map.RemoveAllLayers();
                    Map.Projection = tkMapProjection.PROJECTION_WGS84;

                    int handle = Map.AddLayer(layer, true);
                    Map.Redraw();

                    Debug.Print("Layer connection: " + layer.GetConnectionString());
                    Debug.Print("Layer source query: " + layer.GetSourceQuery());

                    string state = layer.Serialize();
                    Debug.Print("Serialized state: " + state);
                }
                else
                {
                    Debug.Print("Failed to open layer: " + layerName);
                }

                ds.Close();
            }

            return true;
        }

        /// <summary>
        ///     The test reprojection.
        /// </summary>
        /// <returns>
        ///     The <see cref="bool" />.
        /// </returns>
        private static bool TestReprojection()
        {
            Debug.Print("\nStart OGR test");
            AxMap map = Fileformats.Map;
            map.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            map.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

            var gs = new GlobalSettings();
            gs.ShapeInputValidationMode = tkShapeValidationMode.NoValidation;
            gs.ShapeOutputValidationMode = tkShapeValidationMode.NoValidation;

            int handle = map.AddLayerFromDatabase(CONNECTION_STRING, "belarus", true);
            if (handle == -1)
            {
                Debug.Print("Failed to open database layer");
            }
            else
            {
                Debug.Print("Layer was opened");
                OgrLayer l = map.get_OgrLayer(handle);
                if (l != null)
                {
                    Debug.Print("Layer was reprojected: " + l.DataIsReprojected);
                }
            }

            return true;
        }

        /// <summary>
        ///     The ways to open layer.
        /// </summary>
        private static void WaysToOpenLayer()
        {
            var layer = new OgrLayer();

            string layerName = "waterways";

            // 1) open one of exiting layers in datasource
            if (!layer.OpenFromDatabase(CONNECTION_STRING, layerName, true))
            {
                // waterways = layerName
                Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
            }

            // 2) return temporary layer by a query
            if (!layer.OpenFromQuery(CONNECTION_STRING, "SELECT * FROM " + layerName))
            {
                Debug.Print("Failed to run a query: " + layer.get_ErrorMsg(layer.LastErrorCode));
            }

            // 3) the same using datasource
            var ds = new OgrDatasource();
            if (!ds.Open(CONNECTION_STRING))
            {
                Debug.Print("Failed to open datasource: " + ds.GdalLastErrorMsg);
            }
            else
            {
                layer = ds.GetLayerByName(layerName);
                if (layer == null)
                {
                    Debug.Print("Failed to open layer: " + ds.get_ErrorMsg(ds.LastErrorCode));
                }

                layer = ds.RunQuery("SELECT * FROM " + layerName);
                if (layer == null)
                {
                    Debug.Print("Failed to run a query: " + ds.get_ErrorMsg(ds.LastErrorCode));
                }
            }

            // 4) using FileManager
            var fm = new FileManager();
            layer = fm.OpenFromDatabase(CONNECTION_STRING, layerName); // layer name or query can be passed here
            if (layer == null)
            {
                Debug.WriteLine("Failed to open layer: " + fm.get_ErrorMsg(fm.LastErrorCode));

                // let's check GDAL error as well
                var gs = new GlobalSettings();
                Debug.WriteLine("GDAL error message: " + gs.GdalLastErrorMsg);
            }
        }

        #endregion

        /// <summary>
        /// Test setting PostGIS privileges.
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="theForm">
        /// The form.
        /// </param>
        /// <returns>
        /// The <see cref="bool"/>.
        /// </returns>
        internal static bool RunPostGisPostGisPrivileges(string textfileLocation, Form1 theForm)
        {
            // Open text file:
            if (!File.Exists(textfileLocation))
            {
                throw new FileNotFoundException("Cannot find text file.", textfileLocation);
            }

            var numErrors = 0;

            theForm.Progress(
                "-----------------------The setting of the grants and privileges has started." + Environment.NewLine);

            // Read text file:
            var lines = Helper.ReadTextfile(textfileLocation);

            // First line is connection string:
            var connectionString = lines[0];
            if (!connectionString.StartsWith("PG"))
            {
                throw new Exception(
                    "Input file is not correct. The first line should be the connection string, starting with PG");
            }

            var ds = new OgrDatasource();
            if (!ds.Open(connectionString))
            {
                throw new Exception("Failed to open datasource: " + ds.GdalLastErrorMsg);
            }

            // Get every first and second line:
            for (var i = 1; i < lines.Count; i++)
            {
                var query = lines[i];
                string errorMsg;
                if (!ds.ExecuteSQL(query, out errorMsg))
                {
                    theForm.WriteError(string.Format("Error executing query [{0}]: {1}", query, errorMsg));
                    numErrors++;
                }
            }

            // Close database connection:
            ds.Close();

            theForm.Progress(
                string.Format("The setting of the grants and privileges test has finished, with {0} errors", numErrors));

            return numErrors == 0;
        }

        /// <summary>
        /// Import shapefiles into the PostGIS database
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="theForm">
        /// The form.
        /// </param>
        /// <returns>
        /// The <see cref="bool"/>.
        /// </returns>
        internal static bool RunPostGisImportSf(string textfileLocation, Form1 theForm)
        {
            // Open text file:
            if (!File.Exists(textfileLocation))
            {
                throw new FileNotFoundException("Cannot find text file.", textfileLocation);
            }

            var numErrors = 0;
            Map = Fileformats.Map;
            Map.RemoveAllLayers();
            Application.DoEvents();

            theForm.Progress(
                "----------------------- Importing of shapefiles has started." + Environment.NewLine);

            // Read text file:
            var lines = Helper.ReadTextfile(textfileLocation);

            // First line is connection string:
            var connectionString = lines[0];
            if (!connectionString.StartsWith("PG"))
            {
                throw new Exception(
                    "Input file is not correct. The first line should be the connection string, starting with PG");
            }

            var ds = new OgrDatasource();
            if (!ds.Open(connectionString))
            {
                throw new Exception("Failed to open datasource: " + ds.GdalLastErrorMsg);
            }

            // Get every first and second line:
            for (var i = 1; i < lines.Count; i++)
            {
                var shapefileLocation = lines[i];
                var layerName = Path.GetFileNameWithoutExtension(shapefileLocation);
                if (!File.Exists(shapefileLocation))
                {
                    theForm.WriteError(shapefileLocation + " does not exists. Skipping");
                    continue;
                }

                // Open shapefile:
                theForm.Progress(string.Format("Reading {0} shapefile", layerName));
                var fm = new FileManager();
                var sf = fm.OpenShapefile(shapefileLocation, theForm);
                theForm.Progress(string.Format("Importing {0} shapefile", layerName));
                if (!ds.ImportShapefile(sf, layerName, "OVERWRITE=YES", tkShapeValidationMode.NoValidation))
                {
                    var errorMsg = fm.ErrorMsg[fm.LastErrorCode];

                    // let's check GDAL error as well
                    var gs = new GlobalSettings();
                    errorMsg += " GDAL error message: " + gs.GdalLastErrorMsg;

                    theForm.WriteError(string.Format("Error importing shapefile [{0}]: {1}", shapefileLocation, errorMsg));
                    numErrors++;
                }
                else
                {
                    // Read layer and add to map:
                    theForm.Progress(string.Format("Reading {0} layer from db", layerName));
                    var handle = Map.AddLayerFromDatabase(connectionString, layerName, true);
                    if (handle == -1)
                    {
                        theForm.WriteError("Failed to open database layer: " + layerName);
                    }
                }

                // Close shapefile:
                sf.Close();
            }

            theForm.Progress(
                string.Format("Importing of shapefiles test has finished, with {0} errors", numErrors));

            return numErrors == 0;
        }
    }
}