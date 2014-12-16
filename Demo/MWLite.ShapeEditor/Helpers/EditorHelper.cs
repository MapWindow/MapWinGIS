using System;
using System.Diagnostics;
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
            var ogrLayer = map.get_OgrLayer(handle);

            if (sf != null)
            {
                if (sf.InteractiveEditing)
                {
                    SaveLayerChanges(handle);
                }
                else
                {
                    if (ogrLayer != null )
                    {
                        if (ogrLayer.DynamicLoading)
                        {
                            MessageHelper.Info("Editing of dynamically loaded OGR layers isn't allowed.");
                            return;
                        }
                        if (!ogrLayer.SupportsEditing[tkOgrSaveType.ostSaveAll])
                        {
                            MessageHelper.Info("OGR layer doesn't support editing: " + ogrLayer.ErrorMsg[ogrLayer.LastErrorCode]);
                            return;
                        }
                    }
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
                        if (!SaveLayerChanges(layerHandle))
                            return false;
                    }
                }
            }
            return true;
        }

        public static string GetSaveResultString(tkOgrSaveResult saveResult)
        {
            switch (saveResult)
            {
                case tkOgrSaveResult.osrNoChanges:
                    return "OGR layer has no changes";
                case tkOgrSaveResult.osrAllSaved:
                    return "All changes saved";
                case tkOgrSaveResult.osrSomeSaved:
                    return "Some changes saved";
                case tkOgrSaveResult.osrNoneSaved:
                    return "Changes aren't saved";
            }
            return "";
        }
         

        public static bool SaveLayerChanges(int layerHandle)
        {
            var map = App.Map;

            string prompt = string.Format("Save changes for the layer: {0}?", map.get_LayerName(layerHandle));
            var result = MessageHelper.AskYesNoCancel(prompt);
            switch (result)
            {
                case DialogResult.Yes:
                case DialogResult.No:
                    var sf = map.get_Shapefile(layerHandle);
                    var ogrLayer = map.get_OgrLayer(layerHandle);
                    
                    bool save = result == DialogResult.Yes;
                    bool success = false;
                    if (ogrLayer != null)
                    {
                        int savedCount;
                        tkOgrSaveResult saveResult = ogrLayer.SaveChanges(out savedCount);
                        success = saveResult == tkOgrSaveResult.osrAllSaved || saveResult == tkOgrSaveResult.osrNoChanges;
                        string msg = string.Format("{0}: {1}; features: {2}", GetSaveResultString(saveResult), ogrLayer.Name, savedCount);
                        MessageHelper.Info(msg);
                    }
                    else
                    {
                        success = sf.StopEditingShapes(save, true, null);
                    }
                        
                    if (success)
                    {
                        sf.InteractiveEditing = false;
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
                return SaveLayerChanges(layerHandle);
            }
            return true;
        }
    }
}
