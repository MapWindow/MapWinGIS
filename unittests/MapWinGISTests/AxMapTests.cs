using System;
using AxMapWinGIS;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class AxMapTests: ICallback
    {
        private readonly AxMap _axMap1;
        private static readonly GlobalSettings _settings = new GlobalSettings();

        public AxMapTests()
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
        public void GetExtents()
        {
            Console.WriteLine(_axMap1.Extents.ToDebugString());
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
