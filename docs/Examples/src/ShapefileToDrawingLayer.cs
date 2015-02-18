using AxMapWinGIS;
using MapWinGIS;
using System;
using System.Drawing;
using System.Threading;
using System.Windows.Forms;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Creates a drawing layer from the specified shapefile.
        // </summary>
        public void ShapefileToDrawingLayer(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;
            axMap1.GrabProjectionFromData = true;

            string[] filenames = new []{"buildings.shp", "roads.shp",  "points.shp"};
            for (int i = 0; i < filenames.Length; i++)
            {
                filenames[i] = dataPath + filenames[i];
            }
            Extents extents = null;

            var center =  (axMap1.Extents as Extents).Center;

            try
            {
                for (int n = 0; n < filenames.Length; n++)
                {
                    Shapefile sf = new Shapefile();
                    if (sf.Open(filenames[n], null))
                    {
                        if (axMap1.Projection == tkMapProjection.PROJECTION_NONE)
                            axMap1.GeoProjection = sf.GeoProjection.Clone();
                        
                        if (extents == null)
                            extents = sf.Extents;   // the extents of the fist shapefile will be used to setup display

                        int drawHandle = axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
                        for (int i = 0; i < sf.NumShapes; i++)
                        {
                            Shape shp = sf.Shape[i];

                            if (shp.ShapeType == ShpfileType.SHP_POINT)
                            {
                                double x = 0.0;
                                double y = 0.0;
                                shp.get_XY(0, ref x, ref y);
                                axMap1.DrawPointEx(drawHandle, x, y, 5, 0);
                            }
                            else
                            {
                                for (int p = 0; p < shp.NumParts; p++)
                                {
                                    int initIndex = shp.Part[p];
                                    int numPoints = shp.EndOfPart[p] - shp.Part[p] + 1;
                                    if (numPoints > 0)
                                    {
                                        double[] x = new double[numPoints];
                                        double[] y = new double[numPoints];

                                        for (int j = 0; j < numPoints; j++)
                                        {
                                            shp.get_XY(j + initIndex, ref x[j], ref y[j]);
                                        }

                                        object xObj = x;
                                        object yObj = y;
                                        bool drawFill = shp.ShapeType == ShpfileType.SHP_POLYGON;
                                        uint color = sf.ShapefileType == ShpfileType.SHP_POLYGON ? sf.DefaultDrawingOptions.FillColor : 
                                            sf.DefaultDrawingOptions.LineColor;
                                        axMap1.DrawPolygonEx(drawHandle, ref xObj, ref yObj, numPoints, color, drawFill);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            finally 
            {
                if (extents != null)
                    axMap1.Extents = extents;
                axMap1.LockWindow(tkLockMode.lmUnlock);
                axMap1.Redraw();
            }
        }
    }
}