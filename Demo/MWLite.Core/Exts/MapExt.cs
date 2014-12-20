using System;
using AxMapWinGIS;
using MapWinGIS;
using System.Windows.Forms;
using MWLite.Core.UI;

namespace MWLite.Core.Exts
{
    public static class MapExt
    {
        public static void SetDefaultExtents(this AxMap  map)
        {
            map.Projection = tkMapProjection.PROJECTION_NONE;
            var ext = new Extents();
            ext.SetBounds(0.0, 0.0, 0.0, 100.0, 100.0, 0.0);
            map.Extents = ext;
         }
        
        public static string GetAttributes(this AxMap map, int layerHandle, int shapeIndex)
        {
            string s = "";
            var sf = map.get_Shapefile(layerHandle);
            if (sf == null)
                return s;

            string name = map.get_LayerName(layerHandle);
            s += "Layer: " + name + "\n";
            s += "Shape ID: " + shapeIndex + "\n\n";


            var shp = sf.Shape[shapeIndex];
            if (shp != null && map.ShapeEditor.IsUsingEllipsoid)
            {
                var type = shp.ShapeType2D;
                if (type == ShpfileType.SHP_POLYLINE)
                {
                    s += "Length, m: " + map.GeodesicLength(shp).ToString("0.0") + "\n\n";
                }
                if (type == ShpfileType.SHP_POLYGON)
                {
                    s += "Perimeter, m: " + map.GeodesicLength(shp).ToString("0.0") + "\n";
                    s += "Area, ha: " + (map.GeodesicArea(shp) / 10000).ToString("0.0") + "\n\n";
                }
            }

            s += "Attributes:\n";

            for (int i = 0; i < Math.Min(sf.NumFields, 10); i++)
            {
                s += sf.Field[i].Name + " = " + sf.CellValue[i, shapeIndex] + Environment.NewLine;
            }
            return s;
        }
        
        public static double PixelToProj(this AxMap map, int pixel)
        {
            double h1, w1, h2, w2;
            h1 = h2 = w1 = w2 = 0.0;
            map.PixelToProj(0, 0, ref w1, ref h1);
            map.PixelToProj(0, -pixel, ref w2, ref h2);
            return Math.Abs(h1 - h2);
        }

        public static int VerticalIconOffset(this AxMap map, ShapeDrawingOptions opt, ShpfileType shpType)
        {
            if (shpType == ShpfileType.SHP_POINT && opt.Picture != null && 
                opt.PointType == tkPointSymbolType.ptSymbolPicture)
            {
                return (int)(((double)opt.Picture.Height * opt.PictureScaleY) / 2.0 + 0.5);
            }
            return 0;
        }

        public static double PointTolerance(this AxMap map, ShapeDrawingOptions opt, ShpfileType shpType)
        {
            switch (shpType)
            {
                case ShpfileType.SHP_POINT:
                    double w = 0.0, h = 0.0, w1 = 0.0, h1 = 0.0;
                    var img = opt.Picture;
                    map.PixelToProj(0, 0, ref w, ref h);
                    map.PixelToProj(img != null ? img.Width : opt.PointSize, img != null ? img.Height : opt.PointSize,
                                    ref w1, ref h1);
                    return (Math.Abs(w1 - w) + Math.Abs(h1 - h)) / 2.0;
                default:
                    return 0.0;
            }
        }

        public static string GetLayerFilter(this AxMap map, LayerType layerType)
        {
            switch (layerType)
            {
                case LayerType.All:
                    return map.FileManager.CdlgFilter;
                case LayerType.Raster:
                return map.FileManager.CdlgRasterFilter;
                case LayerType.Vector:
                return map.FileManager.CdlgVectorFilter;
            }
            return "All files|*.*";
        }

        public static void MakeScreenshot(this AxMap map, Form parentForm)
        {
            var ext = map.Extents as MapWinGIS.Extents;

            var img = map.SnapShot3(ext.xMin, ext.xMax, ext.yMax, ext.yMin, map.Width);
            if (img != null)
            {
                using (var dlg = new SaveFileDialog())
                {
                    dlg.Filter = "*.jpg|*.jpg";
                    if (dlg.ShowDialog(parentForm) == DialogResult.OK)
                    {
                        if (!img.Save(dlg.FileName, false, ImageType.JPEG_FILE))
                        {
                            MessageHelper.Warn("Failed to save image: " + img.get_ErrorMsg(img.LastErrorCode));
                        }
                        else
                        {
                            MessageHelper.Warn("Image is saved: " + dlg.FileName);
                        }
                   }
                }
            }
        }
    }
}
