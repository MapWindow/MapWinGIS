using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MapWinGIS;
using System.Windows.Forms;
using System.Threading;
using System.IO;
using System.Diagnostics;

namespace TestApplication
{
    internal static class OgrDatasetTests
    {
        private static Form1 form;
        private static AxMapWinGIS.AxMap map;
        private static string CONNECTION_STRING = "PG:host=localhost dbname=london user=postgres password=1234";

        internal static bool RunOgrDatasourceTest(string textfileLocation, Form1 theForm)
        {
            form = theForm;
            map = Fileformats.Map;

            Debug.Print("\nStart OGR datasource test");

            //ListDriverCapability();

            //ListLayers();

            //TestMetadata();

            //DisplayAllLayers();

            //TestSpatialQuery();

            //ImportShapefilesFromFolder();

            //TestExecuteSQL();
            
            Debug.Print("End OGR datasource test");
            return true;
        }

        private static void ListDriverCapability()
        {
            var ds = new OgrDatasource();
            if (!ds.Open(CONNECTION_STRING))
            {
                Debug.Print("Failed to establish connection: " + ds.GdalLastErrorMsg);
            }
            else
            {
                Debug.Print("OGR driver: " + ds.DriverName);

                Debug.Print("\nDriver capabilities:");
                var values = Enum.GetValues(typeof(tkOgrDSCapability));
                foreach (tkOgrDSCapability value in values)
                {
                    Debug.Print(value.ToString() + ": " + ds.TestCapability(value).ToString());
                }
                ds.Close();
            }
        }

        private static void ListLayers()
        {
            var ds = new OgrDatasource();
            if (!ds.Open(CONNECTION_STRING))
            {
                Debug.Print("Failed to establish connection: " + ds.GdalLastErrorMsg);
            }
            else
            {
                int count = ds.LayerCount;
                Debug.Print("Number of layers: " + count);

                Debug.Print("List of layers by name:");
                for (int i = 0; i < count; i++)
                {
                    var lyr = ds.GetLayer(i);
                    Debug.Print("Layer name: " + lyr.Name);
                    Debug.Print("Projection: " + lyr.GeoProjection.ExportToProj4());
                    Debug.Print("Shape type: " + lyr.ShapeType);
                    lyr.Close();
                }
                ds.Close();
            }
        }

        private static bool TestMetadata()
        {
            var ds = new OgrDatasource();
            if (!ds.Open(CONNECTION_STRING))
            {
                Debug.Print("Failed to establish connection: " + ds.GdalLastErrorMsg);
            }
            else
            {
                Debug.Print("Layer creation options: " +
                    ds.get_DriverMetadata(tkGdalDriverMetadata.dmdLAYER_CREATIONOPTIONLIST));
                Debug.Print("Long name: " + ds.get_DriverMetadata(tkGdalDriverMetadata.dmdLONGNAME));

                Debug.Print("Metadata items: ");
                for (int i = 0; i < ds.DriverMetadataCount; i++)
                {
                    Debug.Print(ds.get_DriverMetadataItem(i));
                }
                ds.Close();
            }
            return true;
        }

        private static bool DisplayAllLayers()
        {
            var ds = new OgrDatasource();
            if (!ds.Open(CONNECTION_STRING))
            {
                Debug.WriteLine("Failed to establish connection: " + ds.GdalLastErrorMsg);
            }
            else
            {
                map.RemoveAllLayers();
                
                // make sure it matches SRID of the layers (4326 in this case)
                map.Projection = tkMapProjection.PROJECTION_WGS84;   

                for (int i = 0; i < ds.LayerCount; i++)
                {
                    var layer = ds.GetLayer(i);
                    if (layer != null)
                    {
                        int handle = map.AddLayer(layer, true);
                        if (handle == -1)
                        {
                            Debug.WriteLine("Failed to add layer to the map: " + map.get_ErrorMsg(map.LastErrorCode));
                        }
                        else
                        {
                            Debug.WriteLine("Layer was added the map: " + layer.Name);
                        }
                    }
                }
                map.ZoomToMaxVisibleExtents();
                ds.Close();
            }
            return true;
        }

        private static bool ImportShapefilesFromFolder()
        {
            var ds = new OgrDatasource();
            if (!ds.Open(CONNECTION_STRING))
            {
                Debug.Print("Failed to establish connection: " + ds.GdalLastErrorMsg);
            }
            else
            {
                string path = @"d:\data\sf\london";
                var files = Directory.GetFiles(path, "*.shp");
                foreach (var file in files)
                {
                    var sf = new Shapefile();
                    if (!sf.Open(file))
                    {
                        Debug.Print("Failed to open shapefile: {0}\n{1}", file, sf.get_ErrorMsg(sf.LastErrorCode));
                    }
                    else
                    {
                        string name = Path.GetFileNameWithoutExtension(file);
                        if (!ds.ImportShapefile(sf, name, "OVERWRITE=YES", tkShapeValidationMode.NoValidation))
                        {
                            Debug.Print("Failed to import shapefile: " + name);
                        }
                        else
                        {
                            Debug.Print("Layer was imported: " + name);
                            var layer = ds.GetLayerByName(name);
                            if (layer != null)
                            {
                                Debug.Print("Imported features count: " + layer.FeatureCount);
                                layer.Close();
                            }
                        }
                    }
                }
                ds.Close();
            }
            return true;
        }

        private static bool TestSpatialQuery()
        {
            var ds = new OgrDatasource();
            if (!ds.Open(CONNECTION_STRING))
            {
                Debug.WriteLine("Failed to establish connection: " + ds.GdalLastErrorMsg);
            }
            else
            {
                map.RemoveAllLayers();
                map.Projection = tkMapProjection.PROJECTION_WGS84;

                string sql = "SELECT st_buffer(wkb_geometry, 0.01) AS buffer, * FROM waterways;";
                var buffer = ds.RunQuery(sql);
                if (buffer == null)
                {
                    Debug.WriteLine("Failed to build buffer: " + ds.GdalLastErrorMsg);
                }
                else
                {
                    Debug.WriteLine("Number of features in a buffer: " + buffer.FeatureCount);
                    map.AddLayer(buffer, true);
                    buffer.GetData().DefaultDrawingOptions.FillColor = 255;     // red
                }

                //layer
                string sql2 = "SELECT * FROM waterways;";
                var layer = ds.RunQuery(sql2);
                if (layer == null)
                {
                    Debug.WriteLine("Failed to open layer: " + ds.GdalLastErrorMsg);
                }
                else
                {
                    Debug.WriteLine("Number of features in layer: " + layer.FeatureCount);
                    map.AddLayer(layer, true);
                }
            }
            return true;
        }

        private static bool TestExecuteSQL()
        {
            var ds = new OgrDatasource();
            if (!ds.Open(CONNECTION_STRING))
            {
                Debug.Print("Failed to establish connection: " + ds.GdalLastErrorMsg);
            }
            else
            {
                string errorMsg;
                bool result = ds.ExecuteSQL("DELETE FROM tableName WHERE gid > 100", out errorMsg);
                if (!result)
                {
                    Debug.Print("Error on running SQL: " + errorMsg);    
                }
                else
                {
                    Debug.Print("SQL was executed successfully.");    
                }
                ds.Close();
            }
            return true;
        }
    }
}
