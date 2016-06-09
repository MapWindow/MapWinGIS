using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MapWinGIS;

namespace MWLite.ShapeEditor.Operations
{
    public static class MergeOperation
    {
        /// <summary>
        /// Merges selected shapes of the active shapefile
        /// </summary>
        /// <returns></returns>
        public static MergeResult Merge()
        {
            var sf = App.SelectedShapefile;
            if (sf == null) return MergeResult.NoInput;
            if (sf.NumSelected <= 1) return MergeResult.NoInput;
            if (!sf.InteractiveEditing) return MergeResult.NoInput;

            if (sf.NumSelected > 50) return MergeResult.TooManyShapes;   // report the error to user

            return Merge(App.Legend.SelectedLayer, sf);
        }

        private static MergeResult Merge(int layerHandle, Shapefile sf)
        {
            // merging
            Shape shp = null;
            for (int i = 0; i < sf.NumShapes; i++)
            {
                if (sf.ShapeSelected[i])
                    shp = shp == null ? sf.Shape[i].Clone() : shp.Clip(sf.Shape[i], tkClipOperation.clUnion);
            }

            if (shp == null)
            {
                return MergeResult.Failed;
            }

            // TODO: request for attributes

            // registering in undo list
            var undoList = App.Map.UndoList;
            undoList.BeginBatch();

            for (int i = sf.NumShapes - 1; i >= 0; i--)
            {
                if (sf.ShapeSelected[i])
                {
                    undoList.Add(tkUndoOperation.uoRemoveShape, layerHandle, i);
                    sf.EditDeleteShape(i);
                }
            }
            int shapeIndex = sf.EditAddShape(shp);
            if (shapeIndex != -1)
                undoList.Add(tkUndoOperation.uoAddShape, layerHandle, shapeIndex);

            undoList.EndBatch();

            return MergeResult.Ok;
        }
    }
}
