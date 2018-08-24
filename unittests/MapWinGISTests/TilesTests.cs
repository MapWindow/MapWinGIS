using System;
using System.IO;
using System.Threading;
using AxMapWinGIS;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class TilesTests : ICallback, IStopExecution
    {
        private readonly AxMap _axMap1;
        private static readonly GlobalSettings _settings = new GlobalSettings();

        public TilesTests()
        {
            _settings.ApplicationCallback = this;

            // Create MapWinGIS:
            _axMap1 = Helper.GetAxMap();
            _axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            _axMap1.KnownExtents = tkKnownExtents.keNetherlands;
            _axMap1.ZoomBehavior = tkZoomBehavior.zbUseTileLevels;
            _axMap1.Tiles.Provider = tkTileProvider.OpenStreetMap;

            // Tiles settings:
            _axMap1.Tiles.GlobalCallback = this;
            _settings.StartLogTileRequests(@"D:\tmp\axmap.tiles\TileRequests.log");
        }

        ~TilesTests()
        {
            _settings.StopLogTileRequests();
        }

        [TestMethod]
        public void ShowProviderDetails()
        {
            Console.WriteLine($"Provider {_axMap1.Tiles.Provider.ToString()} supports zoom levels from {_axMap1.Tiles.MinZoom} to {_axMap1.Tiles.MaxZoom}");
            Console.WriteLine(
                $"DiskCacheFilename: {_axMap1.Tiles.DiskCacheFilename}  Cache to disk: {_axMap1.Tiles.DoCaching[tkCacheType.Disk]} Cache to memory: {_axMap1.Tiles.DoCaching[tkCacheType.RAM]}");
            Console.WriteLine($"ProjectionStatus: {_axMap1.Tiles.ProjectionStatus.ToString()}");
        }

        [TestMethod]
        public void PrefetchToFolder()
        {
            Console.WriteLine("Tiles projection status: " + _axMap1.Tiles.ProjectionStatus);
            Console.WriteLine("_axMap1.Tiles.ProjectionIsSphericalMercator: " + _axMap1.Tiles.ProjectionIsSphericalMercator);

            var outputFolder = $@"D:\tmp\axmap.tiles\{_axMap1.Tiles.Provider.ToString()}";
            if (!Directory.Exists(outputFolder)) Directory.CreateDirectory(outputFolder);
            
            var latLongExtents = new Extents();
            latLongExtents.SetBounds(3.3700, 50.7500, 0, 7.2100, 53.4700, 0); // The Netherlands in WGS84: http://spatialreference.org/ref/epsg/28992/
            var providerId = Convert.ToInt32(tkTileProvider.OpenStreetMap);
            var numRounds = 0; // To prevent endless loops
            while (true)
            {
                var numTilesToCache = 0;
                for (var i = _axMap1.Tiles.MinZoom; i < 10; i++)
                {
                    numTilesToCache +=
                        _axMap1.Tiles.PrefetchToFolder(latLongExtents, i, providerId, outputFolder, ".png", this);
                    Console.WriteLine($"numTilesToCache: {numTilesToCache} for zoom {i}");
                    // Wait a moment:
                    Thread.Sleep(2000);
                }
                
                numRounds++;
                Console.WriteLine($"Finished round {numRounds}");

                // Wait before doing another round:
                if (numTilesToCache > 0) Thread.Sleep(2000);
                if (numTilesToCache == 0 || numRounds > 9) break;
            }
        }

        public void Progress(string KeyOfSender, int Percent, string Message)
        {
            Console.WriteLine($"{Percent} {Message}");
        }

        public void Error(string KeyOfSender, string ErrorMsg)
        {
            Console.WriteLine("Error: " + ErrorMsg);
        }

        public bool StopFunction()
        {
            return false;
        }
    }
}
