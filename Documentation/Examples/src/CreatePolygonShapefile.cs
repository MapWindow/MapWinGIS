using System;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        /// <summary>
        /// Creates a polygon shapefile by placing 100 circles randomly.
        /// </summary>
        public void CreatePolygonShapefile(AxMap axMap1)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;

            var sf = new Shapefile();
            bool result = sf.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON);

            if (!result)
            {
                MessageBox.Show(sf.ErrorMsg[sf.LastErrorCode]);
            }
            else
            {
                double xMin = 0.0;
                double yMin = 0.0;
                double xMax = 1000.0;
                double yMax = 1000.0;
                var rnd = new Random(DateTime.Now.Millisecond);

                int fldX = sf.EditAddField("x", FieldType.DOUBLE_FIELD, 9, 12);
                int fldY = sf.EditAddField("y", FieldType.DOUBLE_FIELD, 9, 12);
                int fldArea = sf.EditAddField("area", FieldType.DOUBLE_FIELD, 9, 12);

                // In a loop we are creating 100 different points using the box established above.
                for (int i = 0; i < 100; i++)
                {
                    if (i % 10 == 0)
                    {
                        Shape shp1 = new Shape();
                        shp1.Create(ShpfileType.SHP_POLYGON);
                        sf.EditInsertShape(shp1, ref i);
                    }
                    else
                    {
                        double xCenter = xMin + (xMax - xMin) * rnd.NextDouble();
                        double yCenter = yMin + (yMax - yMin) * rnd.NextDouble();

                        // random radius from 10 to 100
                        double radius = 10 + rnd.NextDouble() * 90;

                        // polygons must be clockwise
                        Shape shp = new Shape();
                        shp.Create(ShpfileType.SHP_POLYGON);

                        for (int j = 0; j < 37; j++)
                        {
                            Point pnt = new Point();
                            pnt.x = xCenter + radius * Math.Cos(j * Math.PI / 18);
                            pnt.y = yCenter - radius * Math.Sin(j * Math.PI / 18);
                            shp.InsertPoint(pnt, ref j);
                        }
                        sf.EditInsertShape(shp, ref i);

                        sf.EditCellValue(fldX, i, xCenter.ToString());
                        sf.EditCellValue(fldY, i, yCenter.ToString());
                        sf.EditCellValue(fldArea, i, Math.PI * radius * radius);
                    }
                }

                int handle = axMap1.AddLayer(sf, true);
                axMap1.ZoomToLayer(handle);

                sf.Categories.Generate(fldArea, tkClassificationType.ctNaturalBreaks, 7);
                ColorScheme scheme = new ColorScheme();
                scheme.SetColors2(tkMapColor.Wheat, tkMapColor.Salmon);
                sf.Categories.ApplyColorScheme(tkColorSchemeType.ctSchemeGraduated, scheme);

                axMap1.Redraw();

                // save if needed
                //sf.SaveAs(@"c:\polygons.shp", null);
            }
        }
    
    }
}

