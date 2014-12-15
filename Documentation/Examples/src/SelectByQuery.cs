using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Selects shapes with certain attributes.
        // </summary>
        public void SelectByQuery(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            
            string filename = dataPath + "landuse.shp";
            var sf = new Shapefile();
            if (sf.Open(filename, null))
            {
                int layerHandle = axMap1.AddLayer(sf, true);
                sf = axMap1.get_Shapefile(layerHandle);     // in case a copy of shapefile was created by AxMap.ProjectionMismatchBehavior
                
                // showing labels for [name] field
                sf.Labels.Generate("[type]", tkLabelPositioning.lpCentroid, true);

                string error = "";
                object result = null;
            
                // the text values must be placed in quotes; we need to shield them with \ sign in C#
                // fields are must be placed in square brackets
                string query = "[type] = \"residential\" AND [osm_id] > 40000000";

                if (sf.Table.Query(query, ref result, ref error))
                {
                    int[] shapes = result as int[];
                    if (shapes != null)
                    {
                        for (int i = 0; i < shapes.Length; i++)
                        {
                            sf.set_ShapeSelected(shapes[i], true);
                        }
                    }
                    axMap1.ZoomToSelected(layerHandle);
                    MessageBox.Show("Objects selected: " + sf.NumSelected);
                }
                else
                {
                    MessageBox.Show("No shapes agree with the condition.");
                }
            }
        }
    }
}