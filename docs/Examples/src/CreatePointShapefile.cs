using System;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Creates a point shapefile by placing 1000 points randomly
        // </summary>
        public void CreatePointShapefile(AxMap axMap1)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;

            var sf = new Shapefile();

            // MWShapeId field will be added to attribute table
            bool result = sf.CreateNewWithShapeID("", ShpfileType.SHP_POINT);

            // bounding box for the new shapefile
            double xMin = 0.0;
            double yMin = 0.0;
            double xMax = 1000.0;
            double yMax = 1000.0;

            // the location of points will be random
            Random rnd = new Random(DateTime.Now.Millisecond);

            // creating points and inserting them in the shape
            for (int i = 0; i < 1000; i++)
            {
                var pnt = new Point();
                pnt.x = xMin + (xMax - xMin) * rnd.NextDouble();
                pnt.y = yMin + (yMax - yMin) * rnd.NextDouble();

                Shape shp = new Shape();
                shp.Create(ShpfileType.SHP_POINT);

                int index = 0;
                shp.InsertPoint(pnt, ref index);
                sf.EditInsertShape(shp, ref i);
            }

            sf.DefaultDrawingOptions.SetDefaultPointSymbol(tkDefaultPointSymbol.dpsStar);

            // adds shapefile to the map
            axMap1.AddLayer(sf, true);

            // save if needed
            //sf.SaveAs(@"c:\points.shp", null);
        }
    }
}