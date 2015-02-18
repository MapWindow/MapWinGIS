using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MapWinGIS;
using MWLite.Core.UI;

namespace MWLite.ShapeEditor.Helpers
{
    public static class ShapefileEx
    {
        public static void CalculateArea(this Shapefile sf)
        {
            if (sf == null) return;
            if (sf.ShapefileType2D != ShpfileType.SHP_POLYGON)
            {
                MessageHelper.Info("Area can be calculated for polygon shapefiles only.");
                return;
            }

            bool editing = sf.EditingTable;
            if (!editing)
            {
                if (!sf.StartEditingTable())
                {
                    MessageHelper.Warn("Failed to start editing mode for table.");
                    return;
                }
            }

            bool ellipsoid = App.Map.Measuring.IsUsingEllipsoid;
            string fieldName = ellipsoid ? "GeoArea" : "Area";
            int fieldIndex = sf.EditAddField(fieldName, FieldType.DOUBLE_FIELD, 6, 18);
            for (int i = 0; i < sf.NumShapes; i++)
			{
                double area = ellipsoid ? App.Map.GeodesicArea(sf.Shape[i]) : sf.Shape[i].Area;
                    sf.EditCellValue(fieldIndex, i, area);
			}
            
            if (!editing) 
            {
                if (!sf.StopEditingTable())
                {
                    MessageHelper.Warn("Failed to save calculated area to the datasource.");
                    return;
                }
            }

            MessageHelper.Info(string.Format("Area was calculated in {0} field.", fieldName));
        }
        
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

        public static string SerializeForClipboard(Shapefile sf)
        {
            if (sf == null) return "";
            int numFields = sf.NumFields;
            var sb = new StringBuilder("wkt_geom\t");
            for (int i = 0; i < numFields; i++)
            {
                sb.Append(sf.Field[i].Name);
                if (i < numFields - 1)
                    sb.Append("\t");
            }
            sb.Append(Environment.NewLine);

            for (int i = 0; i < sf.NumShapes; i++)
            {
                sb.Append(sf.Shape[i].ExportToWKT() + "\t");
                for (int j = 0; j < numFields; j++)
                {
                    var val = sf.CellValue[j, i];
                    sb.Append(val ?? "NULL");
                    if (j < numFields)
                        sb.Append("\t");
                }
                if (i < sf.NumShapes - 1)
                    sb.Append(Environment.NewLine);
            }
            return sb.ToString();
        }
    }
}
