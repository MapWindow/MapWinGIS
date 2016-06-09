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
        // Some operaions with GeoProjection object
        // </summary>
        public void GeoProjection(AxMap axMap1)
        {
            GeoProjection proj = new GeoProjection();
        
            // EPSG code
            proj.ImportFromEPSG(4326);  // WGS84
        
            // proj 4 string
            proj.ImportFromProj4("+proj=longlat +datum=WGS84 +no_defs");  // WGS84

            // autodetect the format
            string unknown_format = "4326";
            proj.ImportFromAutoDetect(unknown_format);

            // from file
            string filename = "some_name";
            proj.ReadFromFile(filename);

            // show the name of the loaded projection
            Debug.Print("Projection loaded: " + proj.Name);

            // show proj 4 representation
            Debug.Print("Proj4 representation: " + proj.ExportToProj4());

            // let's show the properties of the geographic projection
            string s = "";
            double[] arr = new double[5];
            for (int i = 0; i < 5; i++)
            {
                // extract the parameter in element of val arr
                proj.get_GeogCSParam((tkGeogCSParameter)i, ref arr[i]);
            
                // append the name of parameter and the value to the string
                s += (tkGeogCSParameter)i + ": " + arr[i] + Environment.NewLine;
            }
            MessageBox.Show("Parameters of geographic coordinate system: " + Environment.NewLine + s);
        }
    }
}
