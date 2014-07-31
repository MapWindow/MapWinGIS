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
        // Calculates area of polygons and sets 3 range of categories with different symbology
        // </summary>
        public void AddCategoryRange(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            axMap1.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

            string filename = dataPath + "landuse.shp";
            if (File.Exists(filename) == false)
            {
                MessageBox.Show("Failed to open file: " + filename);
                return;
            }

            Shapefile sf = new Shapefile();
            if (sf.Open(filename, null))
            {
                if (!sf.StartEditingTable(null))
                {
                    MessageBox.Show("Failed to open editing mode.");
                    return;
                }
            
                int fieldIndex = sf.Table.FieldIndexByName["Area"];
            
                if (fieldIndex == -1)
                    fieldIndex = sf.EditAddField("Area", FieldType.DOUBLE_FIELD, 15, 18);

                for (int i = 0; i < sf.NumShapes; i++)
                {
                    double area = sf.Shape[i].Area * 100000.0;
                    sf.EditCellValue(fieldIndex, i, area);
                }

                // adding to map
                int handle = axMap1.AddLayer(sf, true);
                sf = axMap1.get_Shapefile(handle);     // in case a copy of shapefile was created by AxMap.ProjectionMismatchBehavior

                double mean = sf.Table.MeanValue[fieldIndex];
                double stDev = sf.Table.StandardDeviation[fieldIndex];
                double min = (double)sf.Table.MinValue[fieldIndex];
                double max = (double)sf.Table.MaxValue[fieldIndex];

                var scheme = new ColorScheme();

                // 1. the first range [min; mean - stDev]
                Utils utils = new Utils();
                sf.DefaultDrawingOptions.FillType = tkFillType.ftHatch;
                sf.DefaultDrawingOptions.FillHatchStyle = tkGDIPlusHatchStyle.hsDiagonalBrick;
            
                bool res = sf.Categories.AddRange(fieldIndex, tkClassificationType.ctNaturalBreaks, 5, min, mean);
                scheme.SetColors2(tkMapColor.Red, tkMapColor.Yellow);
            
                // apply colors 0 and 4 are indices of categories, since 5 categories were added - from 0 to 4
                sf.Categories.ApplyColorScheme3(tkColorSchemeType.ctSchemeRandom, scheme, tkShapeElements.shElementFill, 0, 4);

                // 2. the second range [mean - stDev; mean + stDev]
                // the default drawing options will be copied to the new categories
                sf.DefaultDrawingOptions.FillType = tkFillType.ftHatch;
                sf.DefaultDrawingOptions.FillHatchStyle = tkGDIPlusHatchStyle.hsCross;
                res = sf.Categories.AddRange(fieldIndex, tkClassificationType.ctEqualIntervals, 5, mean, mean + stDev);
                scheme.SetColors2(tkMapColor.Green, tkMapColor.Blue);
                sf.Categories.ApplyColorScheme3(tkColorSchemeType.ctSchemeGraduated, scheme, tkShapeElements.shElementFill, 5, 9);

                // 3. the third range [mean + stDev; max]
                // the default drawing options will be copied to the new categories
                sf.DefaultDrawingOptions.FillType = tkFillType.ftGradient;
                sf.DefaultDrawingOptions.FillColor2 = utils.ColorByName(tkMapColor.Gray);
                res = sf.Categories.AddRange(fieldIndex, tkClassificationType.ctEqualIntervals, 5, mean + stDev, max);
                scheme.SetColors2(tkMapColor.Pink, tkMapColor.Violet);
                sf.Categories.ApplyColorScheme3(tkColorSchemeType.ctSchemeGraduated, scheme, tkShapeElements.shElementFill, 10, 14);

                // apply expresions should be called exlicitly
                sf.Categories.ApplyExpressions();
                axMap1.Redraw();

                // saving options to see categories desription in XML
                axMap1.SaveLayerOptions(handle, "categories_sample", true, "");
            }
        }

        // <summary>
        // To apply the same options on the next loading
        // </summary>
        private void RestoreCategories(AxMap axMap1, string dataPath)
        {
            string filename = dataPath + "landuse.shp";
            Shapefile sf = new Shapefile();
            if (sf.Open(filename, null))
            {
                int handle = axMap1.AddLayer(sf, true);
                string description = "";
                axMap1.LoadLayerOptions(handle, "categories_sample", ref description);
            }
        }
    }
}