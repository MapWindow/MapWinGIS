using System;
using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Calculates a minimal distance from each building to the river.
        // </summary>
        public void MinimalDistance(AxMap axMap1, string dataPath, ToolStripStatusLabel label)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;
            axMap1.GrabProjectionFromData = true;

            string filename1 = dataPath + "waterways.shp";
            string filename2 = dataPath + "buildings.shp";

            if (!File.Exists(filename1) || !File.Exists(filename2))
            {
                MessageBox.Show("The necessary files (waterways.shp, building.shp) are missing: " + dataPath);
            }
            else
            {
                Shapefile sfRivers = new Shapefile();
                sfRivers.Open(filename1, null);
            
                Utils utils = new Utils();
                sfRivers.DefaultDrawingOptions.LineColor = utils.ColorByName(tkMapColor.Blue);
                sfRivers.DefaultDrawingOptions.LineWidth = 5.0f;

                Shapefile sfBuildings = new Shapefile();
                sfBuildings.Open(filename2, null);
            
                // adds a field in the table
                Field field = new Field();
                field.Name = "RiverDist";
                field.Type = FieldType.DOUBLE_FIELD;
                field.Precision = 10;
            
                int fieldIndex = sfBuildings.NumFields;
                sfBuildings.StartEditingShapes(true, null);
                sfBuildings.EditInsertField(field, ref fieldIndex, null);

                ShapefileCategory ct = sfBuildings.Categories.Add("Named buildings");
                ct.Expression = "[Name] <> \"\"";
                sfBuildings.Categories.ApplyExpressions();

                sfRivers.StartEditingShapes(false, null);
                for (int i = 0; i < sfBuildings.NumShapes; i++)
                {
                    if (sfBuildings.ShapeCategory[i] == 0)
                    {
                        label.Text = "Processing building: " + (i + 1) + " / " + sfBuildings.NumShapes;
                        Application.DoEvents();
                    
                        Shape shp = sfBuildings.Shape[i];
                        double minDist = Double.MaxValue;

                        for (int j = 0; j < sfRivers.NumShapes; j++)
                        {
                            Shape shp2 = sfRivers.Shape[j];
                            double distance = shp.Distance(shp2);
                            if (distance < minDist)
                                minDist = distance;
                        }

                        if (minDist != Double.MaxValue)
                            sfBuildings.EditCellValue(fieldIndex, i, minDist);
                    }
                    else
                    {
                        sfBuildings.EditCellValue(fieldIndex, i, 0.0);
                    }
                }
                sfRivers.StopEditingShapes(false, true, null);

                sfBuildings.Categories.Generate(fieldIndex, tkClassificationType.ctNaturalBreaks, 8);
                ColorScheme scheme = new ColorScheme();
                scheme.SetColors2(tkMapColor.Blue, tkMapColor.Yellow);
                sfBuildings.Categories.ApplyColorScheme(tkColorSchemeType.ctSchemeGraduated, scheme);

                sfBuildings.Labels.Generate("[Name] + \"\n\" + [RiverDist] + \" m\"", tkLabelPositioning.lpCentroid, true);
                sfBuildings.Labels.TextRenderingHint = tkTextRenderingHint.SystemDefault;

                sfBuildings.VisibilityExpression = "[Name] <> \"\"";
          
                axMap1.AddLayer(sfRivers, true);
                axMap1.AddLayer(sfBuildings, true);

                label.Text = "";
            }
        }
    }
}