using System.Diagnostics;
using System.Windows.Forms;
using MapWinGIS;

namespace TestGrids
{
    public static class MapHelper
    {
        private static AxMapWinGIS.AxMap axMap1;
        private static ICallback callback;

        internal static void Initialize(AxMapWinGIS.AxMap map, ICallback cback)
        {
            axMap1 = map;
            callback = cback;
        }
        
        public static void StartMeasureingTool(tkMeasuringType toolType, string filename)
        {
            var sf = new Shapefile();
            if (!sf.Open(filename))
            {
                MessageBox.Show("Failed to open shapefile");
            }
            else
            {
                var ut = new Utils();
                axMap1.RemoveAllLayers();
                int handle = axMap1.AddLayer(sf, true);
                axMap1.ZoomToLayer(handle);
                axMap1.Measuring.MeasuringType = toolType;
                axMap1.CursorMode = tkCursorMode.cmMeasure;
            }
        }

        public static void CheckProjection()
        {
            var proj = new GeoProjection {GlobalCallback = callback};
            if (proj.SetWgs84())
            {
                if (!proj.SetGoogleMercator())
                {
                    MessageBox.Show("ERROR: failed to set projection");
                }
                else
                {
                    axMap1.GeoProjection = proj;
                    if (!proj.SetWgs84())
                    {
                        Debug.WriteLine("Failed to change projection of the map; Frozen: " + proj.IsFrozen);
                        axMap1.GeoProjection = proj.Clone();
                        if (!proj.SetWgs84())
                        {
                            MessageBox.Show("ERROR: projection is still frozen when no longer applied to map");
                        }
                        else
                        {
                            Debug.WriteLine("Projection changed; Frozen: " + proj.IsFrozen);
                            MessageBox.Show("Success");
                        }
                    }
                    else
                    {
                        MessageBox.Show("ERROR: projection of the map was changed; Frozen: " + proj.IsFrozen);
                    }
                }
            }
            else
            {
                MessageBox.Show("Failed to set WGS84 projection");
            }
        }
    }
}
