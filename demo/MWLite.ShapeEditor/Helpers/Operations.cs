using System;
using System.Windows.Forms;
using MWLite.Symbology.LegendControl;
using MapWinGIS;
using MWLite.Core.UI;
using MWLite.ShapeEditor.Operations;

namespace MWLite.ShapeEditor.Helpers
{
    public static class OperationHelper
    {
        private static CopyOperation _copyOperation = new CopyOperation();

        /// <summary>
        /// Merges selected shapes
        /// </summary>
        public static void MergeShapes()
        {
            var result = MergeOperation.Merge();
            switch (result)
            {
                case MergeResult.Ok:
                    App.Map.Redraw2(tkRedrawType.RedrawSkipDataLayers);
                    App.RefreshUI();
                    MessageHelper.Info("Shapes were merged successfully.");
                    break;
                case MergeResult.TooManyShapes:
                    MessageHelper.Info("Too many shapes. The number of shapes for operation is limited to 50.");
                    break;
                case MergeResult.Failed:
                    MessageHelper.Info("Failed to merge.");
                    break;
                case MergeResult.NoInput:
                    MessageHelper.Info("No input for operation was found.");
                    break;
            }
        }

        /// <summary>
        /// Splits selected multipart shapes
        /// </summary>
        public static void ExplodeShapes()
        {
            var result = ExplodeOperation.Explode();
            switch (result)
            {
                case ExplodeResult.Ok:
                    App.Map.Redraw2(tkRedrawType.RedrawSkipDataLayers);
                    App.RefreshUI();
                    MessageHelper.Info("Shapes were split successfully.");
                    break;
                case ExplodeResult.NoMultiPart:
                    MessageHelper.Info("No multipart shapes were found within selection.");
                    break;
                case ExplodeResult.Failed:
                    MessageHelper.Info("Failed to merge.");
                    break;
                case ExplodeResult.NoInput:
                    MessageHelper.Info("No input for operation was found.");
                    break;
            }
        }

        /// <summary>
        /// Removes selected shapes
        /// </summary>
        public static void RemoveShapes()
        {
            var sf = App.SelectedShapefile;
            if (sf == null || !sf.InteractiveEditing || sf.NumSelected == 0) return;
            if (MessageHelper.Ask("Remove selected shapes: " + sf.NumSelected + "?") == DialogResult.Yes)
            {
                int layerHandle = App.Legend.SelectedLayer;
                RemoveOperation.Remove(sf, layerHandle);
                App.Map.Redraw();
            }
        }

        public static bool BufferIsEmpty
        {
            get { return _copyOperation.IsEmpty; }
        }

        public static void CopyShapes()
        {
            int layerHandle = App.Legend.SelectedLayer;
            var sf = App.SelectedShapefile;
            _copyOperation.Copy(layerHandle, sf);
        }

        public static void PasteShapes()
        {
            var result = _copyOperation.Paste(App.Legend.SelectedLayer, App.SelectedShapefile);
            switch (result)
            {
                case PasteResult.Ok:
                    App.Map.Redraw();
                    App.RefreshUI();
                    MessageHelper.Info("Shapes were copied.");
                    break;
                case PasteResult.NoInput:
                    MessageHelper.Info("No input was found.");
                    break;
                case PasteResult.ShapeTypeMismatch:
                    MessageHelper.Info("Shape type of source and target shapefiles doesn't match.");
                    break;
            }
        }

        public static void CutShapes()
        {
            _copyOperation.Cut(App.Legend.SelectedLayer, App.SelectedShapefile);
            App.Map.Redraw();
        }
    }
}
