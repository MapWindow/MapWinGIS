using MapWinGIS;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace TestApplication
{
    public static class OgrLayerTests
    {
        private static Form1 form;
        private static AxMapWinGIS.AxMap map;
        private static string CONNECTION_STRING = "PG:host=localhost dbname=london user=postgres password=1234";

        internal static bool RunOgrLayerTest(string textfileLocation, Form1 theForm)
        {
            form = theForm;
            map = Fileformats.Map;

            Debug.Print("\nStart OGR layer test");

            //TestFileManager();

            //TestReadLayer();

            //TestReprojection();

            //TestEditLayer();

            //TestDeserializeMap();

            //AddLayer2Map();

            Debug.Print("\nEnd OGR layer test");
            return true;
        }

        private static void AddLayer2Map()
        {
            string layerName = "SELECT * FROM здания1";

            var gs = new GlobalSettings();

            var layer = new OgrLayer();
            if (!layer.OpenFromQuery(CONNECTION_STRING, layerName))
            {
                Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
                Debug.Print("GDAL last error: " + layer.GdalLastErrorMsg);
            }
            else
            {
                map.RemoveAllLayers();
                // either a) set layer projection to the map
                map.GrabProjectionFromData = true;

                // or b) set map projection and reproject layer if needed
                map.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
                map.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

                int layerHandle = map.AddLayer(layer, true);
                if (layerHandle == -1)
                {
                    Debug.Print("Failed to add layer to the map");
                }
                else
                {
                    map.Redraw();

                    // get the reference back from map
                    var sameLayerFromMap = map.get_OgrLayer(layerHandle);
                }
            }
        }

        private static void TestFileManager()
        {
            string sql = "SELECT * FROM Buildings WHERE gid < 50";
            int handle = map.AddLayerFromDatabase(CONNECTION_STRING, sql, true);
            if (handle == -1)
            {
                Debug.Print("Failed to open layer: " + map.FileManager.get_ErrorMsg(map.FileManager.LastErrorCode));
                
                // in case the reason of failure is still unclear, let's ask GDAL for details
                var gs = new GlobalSettings();
                Debug.Print("Last GDAL error: " + gs.GdalLastErrorMsg);
            }
            else
            {
                var l = map.get_OgrLayer(handle);
                if (l != null)
                {
                    Debug.Print("Number of features: " + l.FeatureCount);
                    
                    // no access the data
                    var sf = l.GetData();
                    Debug.Print("Number of shapes: " + sf.NumShapes);
                }
            }
        }

        private static void WaysToOpenLayer()
        {
            var layer = new OgrLayer();
            
            string layerName = "waterways";

            // 1) open one of exiting layers in datasource
            if (!layer.OpenFromDatabase(CONNECTION_STRING, layerName, true))    // waterways = layerName
                Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));

            // 2) return temporary layer by a query
            if (!layer.OpenFromQuery(CONNECTION_STRING, "SELECT * FROM " + layerName))
                Debug.Print("Failed to run a query: " + layer.get_ErrorMsg(layer.LastErrorCode));
            
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
            layer = fm.OpenFromDatabase(CONNECTION_STRING, layerName);   // layer name or query can be passed here
            if (layer == null)
            {
                Debug.WriteLine("Failed to open layer: " + fm.get_ErrorMsg(fm.LastErrorCode));
                
                // let's check GDAL error as well
                var gs = new GlobalSettings();
                Debug.WriteLine("GDAL error message: " + gs.GdalLastErrorMsg);
            }
        }

        

        private static void DisplayUnderlyingData()
        {
            string layerName = "waterways";

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
                var shapefile = layer.GetData();
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

        private static bool TestReadLayer()
        {
            var ds = new OgrDatasource();

            if (!ds.Open(CONNECTION_STRING))
            {
                Debug.Print("Failed to establish connection: " + ds.GdalLastErrorMsg);
            }
            else
            {
                string layerName = "buildings";
                var layer = ds.GetLayerByName(layerName, true);

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

                    map.RemoveAllLayers();
                    map.Projection = tkMapProjection.PROJECTION_WGS84;
                    
                    int handle = map.AddLayer(layer, true);
                    map.Redraw();
                    
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

        private static bool TestReprojection()
        {
            Debug.Print("\nStart OGR test");
            var map = Fileformats.Map;
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
                var l = map.get_OgrLayer(handle);
                if (l != null)
                {
                    Debug.Print("Layer was reprojected: " + l.DataIsReprojected);
                }
            }
            return true;
        }

        private static void TestLayerCapabilities()
        {
            var layer = new OgrLayer();
            if (!layer.OpenFromDatabase(CONNECTION_STRING, "waterways", true))
            {
                Debug.Print("Failed to open layer: " + layer.get_ErrorMsg(layer.LastErrorCode));
            }
            else
            {
                var values = Enum.GetValues(typeof(MapWinGIS.tkOgrLayerCapability));
                foreach (tkOgrLayerCapability value in values)
                {
                    Debug.Print(value.ToString() + ": " + layer.TestCapability(value).ToString());
                }
            }
        }

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
                
            var sf = layer.GetData();
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
                    var shp = sf.get_Shape(0);
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
                    var shp = sf.get_Shape(sf.NumShapes - 1);
                    for (int i = 0; i < shp.NumPoints; i++)
                    {
                        double x = 0.0, y = 0.0;
                        if (shp.get_XY(i, ref x, ref y))
                            shp.put_XY(i, x + 0.01, y + 0.01);  // let's move it a little
                    }
                }

                if (removeShape)
                {
                    bool result = sf.EditDeleteShape(sf.NumShapes - 1);
                    Debug.Print("Shape was deleted: " + result);
                }

                // saving it
                int count;
                var saveResults = layer.SaveChanges(out count);

                Debug.Print("Save result: " + saveResults.ToString());
                Debug.Print("Number of shapes saved: " + count);

                // displaying info on errors
                for (int i = 0; i < layer.UpdateSourceErrorCount; i++)
                {
                    Debug.Print(string.Format("Error for shape id {0}: {1}",
                        layer.UpdateSourceErrorShapeIndex[i], layer.UpdateSourceErrorMsg[i]));
                }
                return true;
            }
            layer.Close();
            return false;
        }

        private static bool TestDeserializeMap()
        {
            TestReadLayer();

            string state = map.SerializeMapState(false, "");
            map.RemoveAllLayers();
            map.Redraw();
            Application.DoEvents();
            Thread.Sleep(4000);

            map.DeserializeMapState(state, true, "");
            map.Redraw();
            return true;
        }

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

                var map = Fileformats.Map;
                map.RemoveAllLayers();
                map.Projection = tkMapProjection.PROJECTION_WGS84;
                map.AddLayer(layer, true);
            }
            return true;
        }
    }
}
