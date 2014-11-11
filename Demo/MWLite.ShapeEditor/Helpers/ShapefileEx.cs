using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MapWinGIS;

namespace MWLite.ShapeEditor.Helpers
{
    public static class ShapefileEx
    {
        public static void CopyAttributes(this Shapefile sf, int sourceIndex, int targetIndex)
        {
            sf.CopyAttributes(sourceIndex, sf, targetIndex);
        }

        public static void CopyAttributes(this Shapefile sf, int sourceIndex, Shapefile target, int targetIndex)
        {
            for (int i = 0; i < sf.NumFields; i++)
            {
                if (sf.Field[i].Name.ToLower() == "mwshapeid")
                    continue;
                target.EditCellValue(i, targetIndex, sf.CellValue[i, sourceIndex]);
            }
        }

        public static void CopyAttributes(this Shapefile sf, int sourceIndex, Shapefile target, int targetIndex,
           Dictionary<int, int> fieldMap)
        {
            if (fieldMap == null || target == null) return;
            var list = fieldMap.ToList();
            foreach (var fld in list)
            {
                object val = sf.get_CellValue(fld.Key, sourceIndex);
                target.EditCellValue(fld.Value, targetIndex, val);
            }
        }

        public static Dictionary<int, int> BuildFieldMap(this Shapefile source, Shapefile target)
        {
            var dict = new Dictionary<int, int>();
            for (int i = 0; i < source.NumFields; i++)
            {
                var fld = source.Field[i];
                var fldTarget = target.FieldByName[fld.Name];
                if (fldTarget == null || fld.Type != fldTarget.Type)
                    continue;
                int targetIndex = target.Table.FieldIndexByName[fldTarget.Name];
                if (targetIndex != -1)
                {
                    dict.Add(i, targetIndex);
                }
            }
            return dict;
        }

        public static bool HasMultiPart(this Shapefile sf, bool selectedOnly = true)
        {
            for (int i = 0; i < sf.NumShapes; i++)
            {
                if (!selectedOnly || sf.ShapeSelected[i])
                {
                    if (sf.Shape[i].NumParts > 1)
                        return true;
                }
            }
            return false;
        }

        public static void OffsetShapes(this Shapefile sf, double screenOffsetX, double screenOffsetY)
        {
            double x1 = 0.0, x2 = 0.0, y1 = 0.0, y2 = 0.0;
            App.Map.PixelToProj(0.0, 0.0, ref x1, ref y1);
            App.Map.PixelToProj(screenOffsetX, screenOffsetY, ref x2, ref y2);
            for (int i = 0; i < sf.NumShapes; i++)
            {
                sf.Shape[i].Move(x2 - x1, y2 - y1);
            }
        }

        
    }
}
