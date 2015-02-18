using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Calculates the length of intersection of rivers and land parcels
        // </summary>
        public void IntersectionLength(AxMap axMap1, ToolStripStatusLabel label, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;
            axMap1.GrabProjectionFromData = true;

            string filename1 = dataPath + "landuse.shp";
            string filename2 = dataPath + "waterways.shp";

            if (!File.Exists(filename1) || !File.Exists(filename2))
            {
                MessageBox.Show("The necessary files (waterways.shp, building.shp) are missing: " + dataPath);
            }
            else
            {
                Shapefile sfParcels = new Shapefile();
                sfParcels.Open(filename1, null);
                sfParcels.StartEditingShapes(true, null);

                Field field = new Field {Name = "Length", Type = FieldType.DOUBLE_FIELD, Precision = 10};
                int fieldIndex = sfParcels.NumShapes;
                sfParcels.EditInsertField(field, ref fieldIndex, null);

                Shapefile sfRivers = new Shapefile();
                sfRivers.Open(filename2, null);
                sfRivers.StartEditingShapes(true, null);
                Utils utils = new Utils();
                sfRivers.DefaultDrawingOptions.LineWidth = 2;
                sfRivers.DefaultDrawingOptions.LineColor = utils.ColorByName(tkMapColor.Blue);

                Shapefile sfNew = sfRivers.Clone();
                ShapeDrawingOptions options = sfNew.DefaultDrawingOptions;
            
                LinePattern pattern = new LinePattern();
                pattern.AddLine(utils.ColorByName(tkMapColor.Blue), 8, tkDashStyle.dsSolid);
                pattern.AddLine(utils.ColorByName(tkMapColor.LightBlue), 4, tkDashStyle.dsSolid);
                options.LinePattern = pattern;
                options.UseLinePattern = true;

                for (int i = 0; i < sfParcels.NumShapes; i++)
                {
                    Shape shp1 = sfParcels.Shape[i];
                    double length = 0.0;    // the length of intersection

                    for (int j = 0; j < sfRivers.NumShapes; j++)
                    {
                        Shape shp2 = sfRivers.Shape[j];
                        if (shp1.Intersects(shp2))
                        {
                            Shape result = shp1.Clip(shp2, tkClipOperation.clIntersection);
                            if (result != null)
                            {
                                int index = sfNew.EditAddShape(result);
                                length += result.Length;
                            }
                        }
                    }
                    sfParcels.EditCellValue(fieldIndex, i, length);
                    label.Text = string.Format("Parcel: {0}/{1}", i + 1, sfParcels.NumShapes);
                    Application.DoEvents();
                }

                // generating charts
                var chartField = new ChartField();
                chartField.Name = "Length";
                chartField.Color = utils.ColorByName(tkMapColor.LightBlue);
                chartField.Index = fieldIndex;
                sfParcels.Charts.AddField(chartField);
                sfParcels.Charts.Generate(tkLabelPositioning.lpInteriorPoint);
                sfParcels.Charts.ChartType = tkChartType.chtBarChart;
                sfParcels.Charts.BarHeight = 100;
                sfParcels.Charts.ValuesVisible = true;
                sfParcels.Charts.Visible = true;

                axMap1.AddLayer(sfParcels, true);
                axMap1.AddLayer(sfRivers, true);
                axMap1.AddLayer(sfNew, true);
                axMap1.ZoomToMaxExtents();
            }
        }
    }
}