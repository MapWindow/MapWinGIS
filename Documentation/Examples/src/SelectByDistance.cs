using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Selects buildings which lie within specified distance from the parks.
        // </summary>
        public void SelectByDistance(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;
            axMap1.GrabProjectionFromData = true;

            string filename1 = dataPath + "buildings.shp";
            string filename2 = dataPath + "natural.shp";
        
            if (!File.Exists(filename1) || !File.Exists(filename2))
            {
                MessageBox.Show("Failed to open shapefile (natural.shp, buildings.shp): " + dataPath);
                return;
            }

            var sfBuildings = new Shapefile();
            sfBuildings.Open(filename1, null);

            var sfParks = new Shapefile();
            sfParks.Open(filename2, null);
            ShapefileCategory ct = sfParks.Categories.Add("Parks");
        
            // choose parks and make them green
            ct.Expression = "[Type] = \"Park\"";
            var utils = new Utils();
            ct.DrawingOptions.FillColor = utils.ColorByName(tkMapColor.Green);
            sfParks.Categories.ApplyExpression(0);
        
            // hide the rest types of objects on the layer
            sfParks.DefaultDrawingOptions.Visible = false;

            double maxDistance = 150.0;        // in meters

            bool editing = sfBuildings.StartEditingShapes(true, null);
            sfBuildings.UseQTree = true;   // this will build a spatial index to speed up selection

            for (int i = 0; i < sfParks.NumShapes; i++)
            {
                int index = sfParks.ShapeCategory[i];
                if (index == 0)
                {
                    object result = null;
                    Shape shp = sfParks.Shape[i];
                    if (sfBuildings.SelectShapes(shp.Extents, maxDistance, SelectMode.INTERSECTION, ref result))
                    {
                        int[] shapes = result as int[];
                        if (shapes == null) return;
                        for (int j = 0; j < shapes.Length; j++)
                        {
                            if (!sfBuildings.ShapeSelected[shapes[j]])
                            {
                                Shape shp2 = sfBuildings.Shape[shapes[j]];
                                double dist = shp.Distance(shp2);
                                if (dist < maxDistance)
                                    sfBuildings.set_ShapeSelected(shapes[j], true);
                            }
                        }
                    }
                }
            }

            axMap1.AddLayer(sfParks, true);
            axMap1.AddLayer(sfBuildings, true);
            axMap1.ZoomToMaxExtents();
        }
    }
}