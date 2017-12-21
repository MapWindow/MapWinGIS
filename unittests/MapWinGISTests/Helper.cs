using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;
using Point = MapWinGIS.Point;

namespace MapWinGISTests
{
    public static class Helper
    {
        private static AxMap _axMap1;

        public static void PrintExtents(Extents extents)
        {
            Console.Write(extents.xMin + "; ");
            Console.Write(extents.xMax + "; ");
            Console.Write(extents.yMin + "; ");
            Console.WriteLine(extents.yMax);
        }

        public static void DeleteShapefile(string filename)
        {
            var folder = Path.GetDirectoryName(filename);
            if (folder == null) return;
            var filenameBody = Path.GetFileNameWithoutExtension(filename);
            foreach (var f in Directory.EnumerateFiles(folder, filenameBody + ".*"))
            {
                DebugMsg("deleting " + f);
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

        public static void SaveAsShapefile(IShapefile sf, string filename)
        {
            DebugMsg("Saving shapefile ...");
            DeleteShapefile(filename);
            var stopWatch = new Stopwatch();
            stopWatch.Start();
            if (!sf.SaveAs(Path.Combine(filename)))
                throw new Exception("Can't save shapefile Error: " + sf.ErrorMsg[sf.LastErrorCode]);
            stopWatch.Stop();
            DebugMsg("Time it took to save shapefile: " + stopWatch.Elapsed);
            if (!File.Exists(filename))
                throw new Exception($"Output file [{filename}] does not exists");

            DebugMsg(filename + " is saved");
        }

        public static Shapefile CreateSfFromWkt(string wkt, int epsgCode)
        {
            var sf = new Shapefile();
            if (!sf.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON))
                throw new Exception("Can't create shapefile. Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            var shp = new Shape();
            if (!shp.ImportFromWKT(wkt))
                throw new Exception("Could not import wkt" + shp.ErrorMsg[shp.LastErrorCode]);

            if (sf.EditAddShape(shp) == -1)
                throw new Exception("Can't EditAddShape. Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            var geoProjection = new GeoProjection();
            if (!geoProjection.ImportFromEPSG(epsgCode))
                throw new Exception("Can't ImportFromEPSG Error: " + geoProjection.ErrorMsg[geoProjection.LastErrorCode]);
            sf.GeoProjection = geoProjection;

            if (sf.HasInvalidShapes())
                throw new Exception("Shapefile has invalid shapes");

            return sf;
        }

        public static Shapefile OpenShapefile(string fileLocation, bool checkInvalidShapes = true, ICallback callback = null)
        {
            if (!File.Exists(fileLocation))
                throw new Exception($"Input file [{fileLocation}] does not exists");

            DebugMsg("Opening " + fileLocation);

            var stopWatch = new Stopwatch();
            stopWatch.Start();
            var sf = new Shapefile();
            if (callback != null) sf.GlobalCallback = callback;

            if (!sf.Open(fileLocation))
                throw new Exception("Can't open " + fileLocation + " Error: " + sf.ErrorMsg[sf.LastErrorCode]);
            stopWatch.Stop();
            DebugMsg("Time it took to open shapefile: " + stopWatch.Elapsed);

            if (!checkInvalidShapes) return sf;

            if (sf.HasInvalidShapes())
                DebugMsg("Input has invalid shapes");
            return sf;
        }


        public static void CheckValidity(IShapefile sf)
        {
            if (sf.NumShapes == 0)
                throw new Exception("Shapefile has no shapes");

            if (!sf.HasInvalidShapes())
            {
                DebugMsg("Shapefile has no invalid shapes");
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
                        DebugMsg($"Invalid shape with id: {i}. Reason: {shp.IsValidReason}");
                    }
                }
            });
            stopwatch.Stop();
            DebugMsg("Logging invalid shapes using partioner took: " + stopwatch.Elapsed);
        }

        public static void DebugMsg(string msg)
        {
            Console.WriteLine(msg);
        }

        public static string WorkingFolder(string subfolder)
        {
            var tempFolder = Path.Combine(Path.GetTempPath(), subfolder);
            if (!Directory.Exists(tempFolder)) Directory.CreateDirectory(tempFolder);
            return tempFolder;
        }

        public static AxMap GetAxMap()
        {
            if (_axMap1 != null) return _axMap1;

            // Create MapWinGIS:
            _axMap1 = new AxMap();
            _axMap1.CreateControl();
            _axMap1.Width = 200;
            _axMap1.Height = 200;

            // Create form and add MapWinGIS:
            var myForm = new Form();
            myForm.Controls.Add(_axMap1);

            _axMap1.ScalebarVisible = true;
            _axMap1.ShowCoordinates = tkCoordinatesDisplay.cdmAuto;
            _axMap1.ShowRedrawTime = true;
            _axMap1.ShowVersionNumber = true;
            _axMap1.ShowZoomBar = true;
            _axMap1.CursorMode = tkCursorMode.cmZoomIn;

            return _axMap1;
        }

        public static string SaveSnapshot2(AxMap axMap1, string baseName, bool shouldFail = false)
        {
            Application.DoEvents();
            var filename = Path.Combine(Path.GetTempPath(), baseName);
            DeleteFile(filename);
            
            var img = axMap1.SnapShot2(0, axMap1.CurrentZoom + 10, 1000);
            if (img == null) throw new NullReferenceException("Snapshot is null");

            var retVal = img.Save(filename);
            img.Close();
            if (!shouldFail)
            {
                if (!retVal) throw new Exception("Snapshot could not be saved: " + img.ErrorMsg[img.LastErrorCode]);
                if (!File.Exists(filename)) throw new FileNotFoundException("The file doesn't exists.", filename);
                DebugMsg(filename);
            }
            else
            {
                DebugMsg("Expected error: " + img.ErrorMsg[img.LastErrorCode]);
                if (retVal) throw new Exception("Image could be saved. This is unexpected.");
            }

            return filename;
        }

        public static string SaveSnapshot(AxMap axMap1, string baseName, IExtents boundBox, double extentEnlarger = 1d)
        {
            Application.DoEvents();
            var filename = Path.Combine(Path.GetTempPath(), baseName);
            DeleteFile(filename);

            if ((boundBox.Width * boundBox.Height).Equals(0))
            {
                double xmin, ymin, xmax, ymax, zmin, zmax;
                boundBox.GetBounds(out xmin, out ymin, out zmin, out xmax, out ymax, out zmax);
                boundBox.SetBounds(xmin - extentEnlarger, ymin - extentEnlarger, zmin, xmax + extentEnlarger, ymax + extentEnlarger, zmax);
            }

            var img = axMap1.SnapShot(boundBox);
            if (img == null)
                throw new NullReferenceException("Snapshot is null: " + axMap1.get_ErrorMsg(axMap1.LastErrorCode));

            var retVal = img.Save(filename);
            img.Close();
            img = null;
            if (!retVal) throw new Exception("Snapshot could not be saved: " + img.ErrorMsg[img.LastErrorCode]);
            if (!File.Exists(filename)) throw new FileNotFoundException("The file doesn't exists.", filename);
            DebugMsg(filename);

            return filename;
        }


        /// <summary>
        /// Deletes the file if it exists
        /// </summary>
        /// <param name="filename">The filename.</param>
        public static void DeleteFile(string filename)
        {
            if (File.Exists(filename))
                File.Delete(filename);
        }

        public static Dictionary<Color, int> GetColorsFromBitmap(string fileLocation)
        {
            if (!File.Exists(fileLocation))
                throw new Exception($"Input file [{fileLocation}] does not exists");

            var retVal = new Dictionary<Color, int>();
            using (var bm = new Bitmap(fileLocation))
            {
                for (var y = 0; y < bm.Height; y++)
                {
                    for (var x = 0; x < bm.Width; x++)
                    {
                        var color = bm.GetPixel(x, y);
                        if (retVal.ContainsKey(color))
                        {
                            retVal[color]++;
                        }
                        else
                        {
                            retVal.Add(color, 1);
                        }
                    }
                }
            }
            return retVal;
        }
    }
}
