using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;
using System.Diagnostics;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Performs a segmentation of shapes by regular grid
        // </summary>
        public void Segmentation(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;
            axMap1.GrabProjectionFromData = true;

            string[] filenames = new string[2];
            filenames[0] = dataPath + "natural.shp";
            filenames[1] = dataPath + "landuse.shp";

            if (!File.Exists(filenames[0]) || !File.Exists(filenames[1]))
            {
                MessageBox.Show("Couldn't file the files (natural.shp, landuse.shp): " + dataPath);
                return;
            }

            List<Shapefile> layers = new List<Shapefile>();

            for (int i = 0; i < filenames.Length; i++)
            {
                Shapefile sf = new Shapefile();
                sf.Open(filenames[i], null);
                layers.Add(sf);
                axMap1.AddLayer(layers[i], true);
            }

            int count = 4;
            Extents ext = axMap1.MaxExtents as Extents;
            double xStep = (ext.xMax - ext.xMin)/count;
            double yStep = (ext.yMax - ext.yMin)/count;

            Shapefile sfGrid = new Shapefile();
            sfGrid.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON);
            sfGrid.GeoProjection.CopyFrom(layers[0].GeoProjection);

            ColorScheme scheme = new ColorScheme();
            scheme.SetColors2(tkMapColor.Orange, tkMapColor.Yellow);

            for (int i = 0; i < layers.Count; i++)
            {
                string name = Path.GetFileNameWithoutExtension(layers[i].Filename);
                int fieldIndex = sfGrid.EditAddField(name, FieldType.DOUBLE_FIELD, 10, 12);
                uint color = scheme.GraduatedColor[(i + 1) / (double)layers.Count];
                sfGrid.Charts.AddField2(fieldIndex, color);
            }

            for (int i = 0; i < count; i++)
            {
                for (int j = 0; j < count; j++)
                {
                    Shape shp = new Shape();
                    shp.Create(ShpfileType.SHP_POLYGON);
                    shp.AddPoint(ext.xMin + i * xStep, ext.yMin + j * yStep);
                    shp.AddPoint(ext.xMin + i * xStep, ext.yMin + (j + 1) * yStep);
                    shp.AddPoint(ext.xMin + (i + 1) * xStep, ext.yMin + (j + 1) * yStep);
                    shp.AddPoint(ext.xMin + (i + 1) * xStep, ext.yMin + j * yStep);
                    shp.AddPoint(ext.xMin + i * xStep, ext.yMin + j * yStep);
                    int shapeIndex = sfGrid.EditAddShape(shp);

                    for (int layer = 0; layer < layers.Count; layer++)
                    {
                        Shapefile sf = layers[layer];
                        double area = 0.0;
                        for (int n = 0; n < sf.NumShapes; n++)
                        {
                            Shape shp2 = sf.Shape[n];
                            if (shp.Intersects(shp2))
                            {
                                Shape shpResult = shp2.Clip(shp, tkClipOperation.clIntersection);
                                if (shpResult != null)
                                {
                                    area += shpResult.Area;
                                }
                            }
                        }

                        // divide by 10000.0 to convert square meters to hectars
                        bool success = sfGrid.EditCellValue(layer + 1, shapeIndex, area/10000.0);   
                    }
                }
            }

            // generating charts
            Charts charts = sfGrid.Charts;
            charts.Generate(tkLabelPositioning.lpCenter);
            charts.ChartType = tkChartType.chtPieChart;
            charts.PieRadius = 18;
            charts.Thickness = 10;
            charts.ValuesVisible = true;
            charts.Visible = true;

            ShapeDrawingOptions options = sfGrid.DefaultDrawingOptions;
            options.FillVisible = false;
            options.LineWidth = 2;
            options.LineStipple = tkDashStyle.dsDash;
            options.LineColor = 255;

            axMap1.AddLayer(sfGrid, true);
            axMap1.ZoomToMaxExtents();
        }
    }
}