using System;
using System.Diagnostics;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Creates a shapefile holding polygons with holes
        // </summary>
        public void PolygonsWithHoles(AxMap axMap1)
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
                Random rnd = new Random(DateTime.Now.Millisecond);

                // builds 10 polygons            
                for (int i = 0; i < 40; i++)
                {
                    double xCenter = xMin + (xMax - xMin) * rnd.NextDouble();
                    double yCenter = yMin + (yMax - yMin) * rnd.NextDouble();

                    // random radius from 10 to 100
                    double radius = 10 + rnd.NextDouble() * 90;

                    var shp = new Shape();
                    shp.Create(ShpfileType.SHP_POLYGON);
                
                    // polygon must have clockwise order of points (first argument - true)
                    this.AddRing(true, xCenter, yCenter, radius, ref shp);

                    // holes must have counter-clockwise order of points (fale for the last argument) 
                    this.AddRing(false, xCenter + radius / 2.0, yCenter, radius / 4.0, ref shp) ;
                    this.AddRing(false, xCenter - radius / 2.0, yCenter, radius / 4.0, ref shp);
                    this.AddRing(false, xCenter, yCenter + radius / 2.0, radius / 4.0, ref shp);
                    this.AddRing(false, xCenter, yCenter - radius / 2.0, radius / 4.0, ref shp);

                    for (int j = 0; j < shp.NumParts; j++)
                    {
                        Debug.Print("Part is clocwise: " + shp.PartIsClockWise[j]);
                    }

                    Debug.Print("Shape is valid: " + shp.IsValid);
                    if (!shp.IsValid)
                        Debug.Print("Reason: " + shp.IsValidReason);

                    sf.EditInsertShape(shp, ref i);
                }

                axMap1.AddLayer(sf, true);
                axMap1.ZoomToLayer(0);

                sf.SaveAs(@"c:\polygons.shp", null);
            }
        }

        // <summary>
        // Adds a ring to the polygon
        // </summary>
        private void AddRing(bool clockWise, double x, double y, double radius, ref Shape shp)
        {
            int partIndex = shp.NumParts;
            if (shp.NumPoints > 0)
                shp.InsertPart(shp.NumPoints, ref partIndex);

            int count = 0;
            for (int j = 0; j < 37; j++)
            {
                double dx = radius * Math.Cos(j * Math.PI / 18);
                double dy = radius * Math.Sin(j * Math.PI / 18);
            
                //dx *= clockWise ? -1 : 1;
                dy *= clockWise ? -1 : 1;

                var pnt = new Point();
                pnt.x = x + dx;
                pnt.y = y + dy;

                count = shp.NumPoints;
                shp.InsertPoint(pnt, ref count);
            }
        }
    }
}

