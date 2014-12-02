using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // a label to show result in
        private ToolStripStatusLabel m_label = null; 
    
        // <summary>
        // Shows attributes of shape in mouse move event.
        // </summary>
        public void ShowAttributes(AxMap axMap1, string dataPath, ToolStripStatusLabel label)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            axMap1.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

            string filename = dataPath + "landuse.shp";
            Shapefile sf = new Shapefile(); 
            if (sf.Open(filename))
            {
                m_layerHandle = axMap1.AddLayer(sf, true);
                sf = axMap1.get_Shapefile(m_layerHandle);     // in case a copy of shapefile was created by AxMap.ProjectionMismatchBehavior
                
                axMap1.SendMouseMove = true;
                axMap1.CursorMode = tkCursorMode.cmIdentify;
                axMap1.ShapeHighlighted += AxMap1ShapeHighlighted;
                m_label = label;
            }
            else
            {
                MessageBox.Show("Failed to open shapefile");
            }
        }

        // <summary>
        // Handles ShapeHighlighted event and shows attributes of the selected shape in the label
        // </summary>
        void AxMap1ShapeHighlighted(object sender, _DMapEvents_ShapeHighlightedEvent e)
        {
            Shapefile sf = axMap1.get_Shapefile(e.layerHandle);
            if (sf != null)
            {
                string s = "";
                for (int i = 0; i < sf.NumFields; i++)
                {
                    string val = sf.get_CellValue(i, e.shapeIndex).ToString();
                    if (val == "") val = "null";
                    s += sf.Table.Field[i].Name + ":" + val + "; ";
                }
                m_label.Text = s;
            }
        }
    }
}