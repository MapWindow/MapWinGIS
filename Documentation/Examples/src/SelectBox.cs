using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Adds the layers and register event handler.
        // </summary>
        public void SelectBox(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            axMap1.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

            string filename = dataPath + "landuse.shp";

            if (!File.Exists(filename))
            {
                MessageBox.Show("Couldn't file the file: " + filename);
                return;
            }

            Shapefile sf = new Shapefile();
            sf.Open(filename, null);
            if (!sf.StartEditingShapes(true, null))
            {
                MessageBox.Show("Failed to start edit mode: " + sf.Table.ErrorMsg[sf.LastErrorCode]);
            }
            else
            {
                sf.UseQTree = true;
                sf.Labels.Generate("[Name]", tkLabelPositioning.lpCentroid, false);

                axMap1.AddLayer(sf, true);
                axMap1.SendSelectBoxFinal = true;
                MapEvents.SelectBoxFinal += AxMap1SelectBoxFinal; // change MapEvents to axMap1
                axMap1.MapUnits = tkUnitsOfMeasure.umMeters;
                axMap1.CurrentScale = 50000;
                axMap1.CursorMode = tkCursorMode.cmSelection;
            }
        }

        // <summary>
        // Performs selection, updates charts
        // </summary>
        void AxMap1SelectBoxFinal(object sender, _DMapEvents_SelectBoxFinalEvent e)
        {
            // it's assumed here that the layer we want to edit is the first 1 (with 0 index)
            int layerHandle = axMap1.get_LayerHandle(0);  
            Shapefile sf = axMap1.get_Shapefile(layerHandle);
            if (sf != null)
            {
                double left = 0.0;
                double top = 0.0;
                double bottom = 0.0;
                double right = 0.0;
                axMap1.PixelToProj(e.left, e.top, ref left, ref top);
                axMap1.PixelToProj(e.right, e.bottom, ref right, ref bottom);

                object result = null;
                var ext = new Extents();
                ext.SetBounds(left, bottom, 0.0, right, top, 0.0);
            
                sf.SelectNone();
                if (sf.SelectShapes(ext, 0.0, SelectMode.INTERSECTION, ref result))
                {
                    int[] shapes = result as int[];
                    if (shapes == null) return;
                    for (int i = 0; i < shapes.Length; i++)
                    {
                        sf.set_ShapeSelected(shapes[i], true);
                    }
                }
                axMap1.Redraw();
            }
        }
    }
}