using System;
using MapWinGIS;
using MWLite.Core;
using MWLite.Core.UI;
using MWLite.GUI.Forms;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace MWLite.GUI.Helpers
{
    internal static class LayerHelper
    {
        private static string GetLayerFilter(LayerType layerType)
        {
            switch(layerType)
            {
                case LayerType.Vector:
                    var sf = new Shapefile();
                    return sf.CdlgFilter;
                case LayerType.Raster:
                    var gr = new Grid();
                    var im = new Image();
                    return gr.CdlgFilter + "|" + im.CdlgFilter;
                case LayerType.All:
                default:
                    return "All formats|*.*";
            }
        }

        public static void AddLayer(LayerType layerType)
        {
            var dlg = new OpenFileDialog {Filter = GetLayerFilter(layerType), Multiselect = true};

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                var legend = App.Legend;
                var map = App.Map;
                legend.Lock();
                map.LockWindow(tkLockMode.lmLock);

                string layerName = "";
                try
                {
                    var fm = new FileManager();
                    foreach (var name in dlg.FileNames.ToList())
                    {
                        // TODO: show progress
                        layerName = name;
                        var layer = fm.Open(name);
                        if (layer != null)
                        {
                            int handle = legend.Layers.Add(layer, true);
                            legend.Layers[handle].Name = Path.GetFileName(name);
                        }
                    }
                }
                catch
                {
                    MessageHelper.Warn("There was a problem opening layer: " + layerName);
                }
                finally
                {
                    legend.Unlock();
                    map.LockWindow(tkLockMode.lmUnlock);
                }
            }
        }

        public static void RemoveLayer()
        {
            int layerHandle = App.Legend.SelectedLayer;
            if (layerHandle != -1)
            {
                if (MessageBox.Show("Do you want to remove layer?", "",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    App.Legend.Layers.Remove(layerHandle);
                }
            }
        }

        public static void ZoomToLayer()
        {
            int handle = App.Legend.SelectedLayer;
            App.Map.ZoomToLayer(handle);
        }

        public static void ZoomToSelected()
        {
            int handle = App.Legend.SelectedLayer;
            App.Map.ZoomToSelected(handle);
        }

        public static void ClearSelection()
        {
            int handle = App.Legend.SelectedLayer;
            var sf = App.Map.get_Shapefile(handle);
            if (sf != null)
            {
                sf.SelectNone();
                MainForm.Instance.RefreshUI();
                App.Map.Redraw();
            }
        }
    }
}
