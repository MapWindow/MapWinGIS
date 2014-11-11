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
                App.Legend.Refresh();
                App.Instance.RefreshUI();
            }
        }

        private static void SaveShapefileChanges(int layerHandle, Shapefile sf)
        {
            var map = App.Map;

            var result = MessageHelper.AskYesNoCancel("Save changes?");
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
                        if (save)
                        {
                            MessageHelper.Info("Changes were saved.");
                        }
                    }
                    map.Redraw();
                    break;
                case DialogResult.Cancel:
                    break;   // do nothing
            }
        }

        public static void CreateLayer()
        {
            using (var form = new NewLayerForm())
            {
                if (form.ShowDialog(App.Instance as Form) == DialogResult.OK)
                {
                    var sf = new Shapefile();
                    sf.GeoProjection.CopyFrom(App.Map.GeoProjection);
                    sf.CreateNewWithShapeID(form.Filename, form.ShapefileType);
                    sf.InteractiveEditing = true;
                    int handle = App.Legend.Layers.Add(sf, true);
                    App.Map.set_LayerName(handle, Path.GetFileName(form.Filename));
                }
            }
        }

        
    }
}
