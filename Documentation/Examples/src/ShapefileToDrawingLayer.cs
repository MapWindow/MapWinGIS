using AxMapWinGIS;
using MapWinGIS;

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

            string[] filenames = new string[2];
            filenames[0] = dataPath + "buildings.shp";
            filenames[1] = dataPath + "roads.shp";
            Extents extents = null;

            axMap1.LockWindow(tkLockMode.lmLock);
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
                            extents = sf.Extents;   // the extents of the fist shapefile wil be used to setup display

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
                axMap1.Extents = extents;
                axMap1.LockWindow(tkLockMode.lmUnlock);
                axMap1.Redraw();
            }
        }
    }
}