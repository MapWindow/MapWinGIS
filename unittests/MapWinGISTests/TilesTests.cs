using System;
using AxMapWinGIS;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class TilesTests : ICallback
    {
        private AxMap _axMap1;
        private static readonly GlobalSettings _settings = new GlobalSettings();

        [TestInitialize]
        public void Init()
        {
            _settings.ApplicationCallback = this;

            // Create MapWinGIS:
            _axMap1 = Helper.GetAxMap();
            _axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            _axMap1.KnownExtents = tkKnownExtents.keNetherlands;
            _axMap1.ZoomBehavior = tkZoomBehavior.zbUseTileLevels;
            _axMap1.Tiles.Provider = tkTileProvider.OpenStreetMap;
        }

        [TestMethod]
        public void ShowProviderDetails()
        {
            Console.WriteLine($"Provider {_axMap1.Tiles.Provider.ToString()} supports zoom levels from {_axMap1.Tiles.MinZoom} to {_axMap1.Tiles.MaxZoom}");
            Console.WriteLine(
                $"DiskCacheFilename: {_axMap1.Tiles.DiskCacheFilename}  Cache to disk: {_axMap1.Tiles.DoCaching[tkCacheType.Disk]} Cache to memory: {_axMap1.Tiles.DoCaching[tkCacheType.RAM]}");
        }

        [TestMethod]
        public void PrefetchToFolder()
        {
            Console.WriteLine("_axMap1.Extents: " + _axMap1.Extents.ToDebugString());
            var numTilesToCache = _axMap1.Tiles.PrefetchToFolder(_axMap1.Extents, 5,
                Convert.ToInt32(tkTileProvider.OpenStreetMap), @"D:\tmp\axmap.tiles\OpenStreetMap", ".png", null);
            Console.WriteLine("numTilesToCache: " + numTilesToCache);
        }

        public void Progress(string KeyOfSender, int Percent, string Message)
        {
            Console.WriteLine($"{Percent} {Message}");
        }

        public void Error(string KeyOfSender, string ErrorMsg)
        {
            Console.WriteLine("Error: " + ErrorMsg);
        }
    }
}
