using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using MapWinGISTests;

namespace ConsoleApplication1
{
    /// <summary>
    /// This program is used as Debug target for MapWinGIS project. 
    /// </summary>
    public class Program
    {
        [STAThread]
        public static void Main(string[] args)
        {
            var path = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + @"\TestData\";
            Directory.SetCurrentDirectory(path);
            Debug.WriteLine("Current path: " + Directory.GetCurrentDirectory());

            /* *******************************************
             * This binary is used in MapWinGIS to debug
             * and step through the C++ code
             * First make a unit test in the MapWinGISTests 
             * project (also in this solution)
             * Next call this tests below,
             * compile this solution and now you can
             * debug MapWinGIS and set break points
             * *******************************************/

            // var t = new GdalTests();
            // t.CreateLayerSQLiteTest();
            // t.GdalInfoHdf5();

            //var t = new GdalUtilsTests();
            // t.Shapefile2Gml();
            // t.VerySmallClip();

            //var t = new OgrDatasourceTests();
            //t.ImportShapefileTest();
            //t.GetBuffer();

            //var t = new PostGisTest();
            //t.ReadAttributesFromPostGISLayer();

            //var t = new ShapefileTests();
            //t.TableQueryTest();
            //t.ImportExportWKT();
            //t.ShapefileDataTest();
            // t.UnionShapefiles();
            //var t = new PointInPolygonTests();
            //t.TestPointInSimplePolygon();
            //t.TestPointInPolygonM();
            //t.TestPointInPolygonZ();
            //t.TestPointInPolygonWithHole();
            //t.TestPointInMultiPolygon();
            //t.TestPointInLotsOfPolygons();

            //var t = new NewSfMethodsTests();
            //t.DifferenceTest2();

            //t.Reproject2280Test();
            //t.ReadRussionDataFromTable();
            //t.CreateRussionCategories();
            //t.MergeM();
            //t.Reproject2280Test();
            // t.PointInShapefile();
            // t.SpatialIndexMWGIS98();

            //var t = new GridTests();
            //t.ChangeValueGridNotInRam();

            // var t = new ImageTests();
            //t.SaveImageShouldFail();
            //t.SerializeDeserializeImage();

            //var t = new DrawingTests();
            //t.Init();
            //t.DrawLabel();
            //t.CaptureSnapshot();

            //var t = new ClipperTests();
            // t.DissolveSmallfileClipper();
            //t.DissolveLargefileClipper();

            //var t = new UtilTests();
            //t.ProjectionStrings();
            //t.ZonalStatistics();
            //t.ReclassifyRaster();
            //t.ClipPolygon();
            //t.PointInPolygon();

            //var t = new TilesTests();
            //t.PrefetchToFolderDutchOSM();

            var t = new OgrDatasourceTests();
            t.GenerateEmptyWKT();

            Console.WriteLine("Finished");
            Console.ReadLine();
        }
    }
}
