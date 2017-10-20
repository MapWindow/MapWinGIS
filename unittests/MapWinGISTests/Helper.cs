using System;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    public static class Helper
    {
        public static void PrintExtents(Extents extents)
        {
            Debug.Write(extents.xMin + "; ");
            Debug.Write(extents.xMax + "; ");
            Debug.Write(extents.yMin + "; ");
            Debug.WriteLine(extents.yMax);
        }

        public static void DeleteShapefile(string filename)
        {
            var folder = Path.GetDirectoryName(filename);
            if (folder == null) return;
            var filenameBody = Path.GetFileNameWithoutExtension(filename);
            foreach (var f in Directory.EnumerateFiles(folder, filenameBody + ".*"))
            {
                Debug.WriteLine("deleting " + f);
                File.Delete(f);
            }
        }

        public static IShapefile CreateFishnet(IExtents extents, double blockSizeX, double blockSizeY)
        {
            var sfFishnet = new Shapefile();
            if (!sfFishnet.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON))
                throw new Exception("Could not create shapefile: " + sfFishnet.ErrorMsg[sfFishnet.LastErrorCode]);

            // First point:
            var initialStartPointX = extents.xMin;
            var startX = extents.xMin;
            var startY = extents.yMin;
            var numPoints = 0;

            while (true)
            {
                // Create block
                var shp = new Shape();
                if (!shp.Create(ShpfileType.SHP_POLYGON))
                    throw new Exception("Create a new shape failed: " + shp.ErrorMsg[shp.LastErrorCode]);

                // First point:
                if (!shp.InsertPoint(new Point { x = startX, y = startY }, ref numPoints))
                    throw new Exception("Inserting a point failed: " + shp.ErrorMsg[shp.LastErrorCode]);
                // Second point:
                if (!shp.InsertPoint(new Point { x = startX + blockSizeX, y = startY }, ref numPoints))
                    throw new Exception("Inserting a point failed: " + shp.ErrorMsg[shp.LastErrorCode]);
                // Third point:
                if (!shp.InsertPoint(new Point { x = startX + blockSizeX, y = startY + blockSizeY }, ref numPoints))
                    throw new Exception("Inserting a point failed: " + shp.ErrorMsg[shp.LastErrorCode]);
                // Fourth point:
                if (!shp.InsertPoint(new Point { x = startX, y = startY + blockSizeY }, ref numPoints))
                    throw new Exception("Inserting a point failed: " + shp.ErrorMsg[shp.LastErrorCode]);
                // Closing:
                if (!shp.InsertPoint(new Point { x = startX, y = startY }, ref numPoints))
                    throw new Exception("Inserting a point failed: " + shp.ErrorMsg[shp.LastErrorCode]);

                if (!shp.IsValid)
                    throw new Exception("Shape is invalid: " + shp.IsValidReason);

                // Add shape to shapefile:
                if (sfFishnet.EditAddShape(shp) == -1)
                    throw new Exception($"Could not EditAddShape. Error: {sfFishnet.ErrorMsg[sfFishnet.LastErrorCode]}");

                // Next block on same row:
                startX += blockSizeX;
                if (!(startX >= extents.xMax)) continue;

                // Start next row:
                startX = initialStartPointX;
                startY += blockSizeY;

                if (startY >= extents.yMax)
                {
                    // Completed the whole fishnet, stop:
                    break;
                }
            }
            return sfFishnet;
        }

        public static void SaveShapefile(IShapefile sf, string filename)
        {
            DeleteShapefile(filename);
            if (!sf.SaveAs(Path.Combine(filename)))
                throw new Exception("Can't save shapefile Error: " + sf.ErrorMsg[sf.LastErrorCode]);
            if (!File.Exists(filename))
                throw new Exception($"Output file [{filename}] does not exists");

            Debug.WriteLine(filename + " is saved");
        }

        public static Shapefile CreateSfFromWkt(string wkt, int epsgCode)
        {
            var sf = new ShapefileClass();
            if (!sf.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON))
                throw new Exception("Can't create shapefile. Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            var shp = new ShapeClass();
            if (!shp.ImportFromWKT(wkt))
                throw new Exception("Could not import wkt" + shp.ErrorMsg[shp.LastErrorCode]);

            if (sf.EditAddShape(shp) == -1)
                throw new Exception("Can't EditAddShape. Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            var geoProjection = new GeoProjectionClass();
            if (!geoProjection.ImportFromEPSG(epsgCode))
                throw new Exception("Can't ImportFromEPSG Error: " + geoProjection.ErrorMsg[geoProjection.LastErrorCode]);
            sf.GeoProjection = geoProjection;

            if (sf.HasInvalidShapes())
                throw new Exception("Shapefile has invalid shapes");

            return sf;
        }

        public static void CheckValidity(IShapefile sf)
        {
            if (sf.NumShapes == 0) Assert.Fail("Shapefile has no shapes");

            if (!sf.HasInvalidShapes())
            {
                Debug.WriteLine("Shapefile has no invalid shapes");
                return;
            }

            var stopwatch = new Stopwatch();
            stopwatch.Start();

            // Partition the entire source array.
            var rangePartitioner = Partitioner.Create(0, sf.NumShapes);
            // Loop over the partitions in parallel.
            Parallel.ForEach(rangePartitioner, (range, loopState) =>
            {
                // Loop over each range element without a delegate invocation.
                for (var i = range.Item1; i < range.Item2; i++)
                {
                    var shp = sf.Shape[i];
                    if (!shp.IsValid)
                    {
                        Debug.WriteLine($"Invalid shape with id: {i}. Reason: {shp.IsValidReason}");
                    }
                }
            });
            stopwatch.Stop();
            Debug.WriteLine("Logging invalid shapes using partioner took: " + stopwatch.Elapsed);
        }

        public static void DebugMsg(string msg)
        {
            Debug.WriteLine(msg);
            Console.WriteLine(msg);
        }

        public static string WorkingFolder(string subfolder)
        {
            var tempFolder = Path.Combine(Path.GetTempPath(), subfolder);
            if (!Directory.Exists(tempFolder)) Directory.CreateDirectory(tempFolder);
            return tempFolder;
        }
    }
}
