using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Creates and displayes custom line patterns
        // </summary>
        public void LinePattern(AxMap axMap1, string iconPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;

            var sf = this.CreateLines();
            axMap1.AddLayer(sf, true);

            var utils = new Utils();

            // railroad pattern
            LinePattern pattern = new LinePattern();
            pattern.AddLine(utils.ColorByName(tkMapColor.Black), 6.0f, tkDashStyle.dsSolid);
            pattern.AddLine(utils.ColorByName(tkMapColor.White), 5.0f, tkDashStyle.dsDot);

            ShapefileCategory ct = sf.Categories.Add("Railroad");
            ct.DrawingOptions.LinePattern = pattern;
            ct.DrawingOptions.UseLinePattern = true;
            sf.set_ShapeCategory(0, 0);

            // river pattern
            pattern = new LinePattern();
            pattern.AddLine(utils.ColorByName(tkMapColor.DarkBlue), 6.0f, tkDashStyle.dsSolid);
            pattern.AddLine(utils.ColorByName(tkMapColor.LightBlue), 4.0f, tkDashStyle.dsSolid);

            ct = sf.Categories.Add("River");
            ct.DrawingOptions.LinePattern = pattern;
            ct.DrawingOptions.UseLinePattern = true;
            sf.set_ShapeCategory(1, 1);

            // road with direction
            pattern = new LinePattern();
            pattern.AddLine(utils.ColorByName(tkMapColor.Gray), 8.0f, tkDashStyle.dsSolid);
            pattern.AddLine(utils.ColorByName(tkMapColor.Yellow), 7.0f, tkDashStyle.dsSolid);
            LineSegment segm = pattern.AddMarker(tkDefaultPointSymbol.dpsArrowRight);
            segm.Color = utils.ColorByName(tkMapColor.Orange);
            segm.MarkerSize = 10;
            segm.MarkerInterval = 32;

            ct = sf.Categories.Add("Direction");
            ct.DrawingOptions.LinePattern = pattern;
            ct.DrawingOptions.UseLinePattern = true;
            sf.set_ShapeCategory(2, 2);
        }

        // <summary>
        // This function creates a number of parallel polylines (segments)
        // </summary>
        private Shapefile CreateLines()
        {
            Shapefile sf = new Shapefile();
            sf.CreateNew("", ShpfileType.SHP_POLYLINE);

            int width = 500;
            int step = 50;

            for (int i = 0; i < 3; i++)
            {
                Shape shp = new Shape();
                shp.Create(ShpfileType.SHP_POLYLINE);

                Point pnt = new Point();
                pnt.x = 0;
                pnt.y = i * step;
                int index = shp.numPoints;
                shp.InsertPoint(pnt, ref index);

                pnt = new Point();
                pnt.x = width;
                pnt.y = i * step;
                index = shp.numPoints;
                shp.InsertPoint(pnt, ref index);

                index = sf.NumShapes;
                sf.EditInsertShape(shp, ref index);
            }
            return sf;
        }
    }
}