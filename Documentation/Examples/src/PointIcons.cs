using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Assigns markers for points
        // </summary>
        public void PointIcons(AxMap axMap1, string dataPath, string iconPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            axMap1.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

            if (!Directory.Exists(iconPath))
            {
                MessageBox.Show("Icons folder wasn't found: " + iconPath);
                return;
            }
        
            string roads = dataPath + "roads.shp";
            string buildings = dataPath + "buildings.shp";
            if (File.Exists(roads))
            {
                Shapefile sfRoads = new Shapefile();
                sfRoads.Open(roads, null);
                sfRoads.DefaultDrawingOptions.LineWidth = 5;
                //axMap1.AddLayer(sfRoads, true);

                Shapefile sfBuildings = new Shapefile();
                sfBuildings.Open(buildings, null);
                axMap1.AddLayer(sfBuildings, true);
            }

            Shapefile sf = new Shapefile();
            string filename = dataPath + "points.shp";
            if (!sf.Open(filename, null))
            {
                MessageBox.Show("Failed to open shapefile: " + sf.ErrorMsg[sf.LastErrorCode]);
            }
            else
            {
                int layerHandle = axMap1.AddLayer(sf, true);
                sf = axMap1.get_Shapefile(layerHandle);     // in case a copy of shapefile was created by AxMap.ProjectionMismatchBehavior
                
                int index = sf.Table.FieldIndexByName["Type"];
                var names = new HashSet<string>();
            
                for (int i = 0; i < sf.Table.NumRows; i++)
                {
                    names.Add((string)sf.Table.CellValue[index, i]);
                }

                string[] files = Directory.GetFiles(iconPath);
                foreach (string file in files)
                {
                    string name = Path.GetFileNameWithoutExtension(file);
                    if (Path.GetExtension(file).ToLower() == ".png" && names.Contains(name))
                    {
                        Image img = new Image();
                        if (img.Open(file, ImageType.USE_FILE_EXTENSION, true, null))
                        {
                            ShapefileCategory ct = sf.Categories.Add(name);
                            ct.Expression = "[Type] = \"" + name + "\"";
                            ct.DrawingOptions.PointType = tkPointSymbolType.ptSymbolPicture;
                            ct.DrawingOptions.Picture = img;
                        }
                    }
                }

                sf.DefaultDrawingOptions.Visible = false;   // hide all the unclassified points
                sf.Categories.ApplyExpressions();
                axMap1.Redraw();
            }
        }
    }
}