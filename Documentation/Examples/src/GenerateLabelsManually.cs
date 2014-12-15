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
        // Adds labels to the layer without using automated procedures like Labels.Generate.
        // </summary>
        public void GenerateLabelsManually(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;

            string filename = dataPath + "buildings.shp";
            if (!File.Exists(filename))
            {
                MessageBox.Show("Failed to open file: " + filename);
                return;
            }

            var sf = new Shapefile();
            if (!sf.Open(filename, null))
            {
                MessageBox.Show("Failed to open shapefile: " + sf.ErrorMsg[sf.LastErrorCode]);
            }
            else
            {
                int layerHandle = axMap1.AddLayer(sf, true);
                sf = axMap1.get_Shapefile(layerHandle);     // in case a copy of shapefile was created by AxMap.ProjectionMismatchBehavior

                int fieldIndex = 0;
                for (int i = 0; i < sf.NumShapes; i++)
                {
                    Shape shp = sf.Shape[i];
                    string text = sf.CellValue[fieldIndex, i].ToString();
                    Point pnt = shp.Centroid;
                    sf.Labels.AddLabel(text, pnt.x, pnt.y, 0.0, -1);

                    // the old method should be used like this
                    //axMap1.AddLabel(layerHandle, text, 0, pnt.x, pnt.y, tkHJustification.hjCenter);
                }
                sf.Labels.Synchronized = true;
            }
        }
    }
}