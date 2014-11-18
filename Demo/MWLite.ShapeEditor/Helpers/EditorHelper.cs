using System.IO;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.Core.UI;
using MWLite.ShapeEditor.Forms;
using MWLite.ShapeEditor.Operations;

namespace MWLite.ShapeEditor.Helpers
{
    public static class EditorHelper
    {
        public static void ToggleEditLayer()
        {
            var legend = App.Legend;
            var map = App.Map;

            int handle = legend.SelectedLayer;
            var sf = map.get_Shapefile(handle);
            if (sf != null)
            {
                if (sf.InteractiveEditing)
                {
                    SaveShapefileChanges(handle, sf);
                }
                else
                {
                    sf.InteractiveEditing = true;
                }
                App.Map.Redraw();
                App.Legend.Refresh();
                App.Instance.RefreshUI();
            }
        }

        public static bool StopAllEditing()
        {
            var map = App.Map;

            for (int i = 0; i < map.NumLayers; i++)
            {
                int layerHandle = map.get_LayerHandle(i);
                var sf = map.get_Shapefile(layerHandle);
                if (sf != null)
                {
                    if (sf.InteractiveEditing)
                    {
                        if (!SaveShapefileChanges(layerHandle, sf))
                            return false;
                    }
                }
            }
            return true;
        }

        public static bool SaveShapefileChanges(int layerHandle, Shapefile sf)
        {
            var map = App.Map;

            string prompt = string.Format("Save changes for the layer: {0}?", map.get_LayerName(layerHandle));
            var result = MessageHelper.AskYesNoCancel(prompt);
            switch (result)
            {
                case DialogResult.Yes:
                case DialogResult.No:
                    bool save = result == DialogResult.Yes;
                    sf.InteractiveEditing = false;
                    if (sf.StopEditingShapes(save, save, null))
                    {
                        map.ShapeEditor.Clear();
                        map.UndoList.ClearForLayer(layerHandle);
                    }
                    map.Redraw();
                    return true;
                case DialogResult.Cancel:
                default:
                    return false;
            }
        }

        public static void CreateLayer()
        {
            using (var form = new NewLayerForm())
            {
                if (form.ShowDialog(App.Instance as Form) == DialogResult.OK)
                {
                    var sf = form.Shapefile;
                    sf.GeoProjection.CopyFrom(App.Map.GeoProjection);
                    sf.Save();
                    int handle = App.Legend.Layers.Add(sf, true);
                    App.Map.set_LayerName(handle, Path.GetFileName(sf.Filename));
                }
            }
        }

        public static bool StopLayerEditing(int layerHandle)
        {
            var map = App.Map;
            var sf = map.get_Shapefile(layerHandle);
            if (sf != null && sf.InteractiveEditing)
            {
                return SaveShapefileChanges(layerHandle, sf);
            }
            return true;
        }
    }
}
