using System.IO;
using System.Linq;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        private const int CATEGORY_SELECTED = 0;
    
        // <summary>
        // Build a list of unique values of the given field and imlement zooming to them from the context menu
        // </summary>
        public void LabelSelection(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            axMap1.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

            string filename = dataPath + "buildings.shp";

            if (!File.Exists(filename))
            {
                System.Windows.Forms.MessageBox.Show("Couldn't file the file: " + filename);
                return;
            }

            Shapefile sf = new Shapefile();
            sf.Open(filename, null);
            m_layerHandle = axMap1.AddLayer(sf, true);
            sf = axMap1.get_Shapefile(m_layerHandle);     // in case a copy of shapefile was created by AxMap.ProjectionMismatchBehavior

            // let's add labels consisting of Name and type of building on a separate lines
            sf.Labels.Generate("[Type]", tkLabelPositioning.lpCenter, false);
            sf.Labels.FrameVisible = true;
            sf.Labels.FrameType = tkLabelFrameType.lfRectangle;

            // now let's add categories
            Utils utils = new Utils();  // to specify colors

            LabelCategory ct = sf.Labels.AddCategory("Selected");
            ct.FrameBackColor = utils.ColorByName(tkMapColor.Yellow);

            ct = sf.Labels.AddCategory("Hidden");
            ct.Visible = false;

            

            axMap1.SendSelectBoxFinal = true;
            axMap1.SendMouseDown = true;
            axMap1.CursorMode = tkCursorMode.cmSelection;
            MapEvents.SelectBoxFinal += AxMap1SelectBoxFinal2;
        }

        // <summary>
        // Handles select box final event. Select the label that are within the rectangular specified by user
        // </summary>
        void AxMap1SelectBoxFinal2(object sender, _DMapEvents_SelectBoxFinalEvent e)
        {
            Shapefile sf = axMap1.get_Shapefile(m_layerHandle);
            if (sf != null)
            {
                object labels = null;
                object parts = null;
            
                var ext = new Extents();
                ext.SetBounds(e.left, e.bottom, 0.0, e.right, e.top, 0.0);

                if (sf.Labels.Select(ext, 0, SelectMode.INTERSECTION, ref labels, ref parts))
                {
                    int[] labelIndices = labels as int[];
                    int[] partIndices = parts as int[];
                    for (int i = 0; i < labelIndices.Count(); i++)
                    {
                        Label label = sf.Labels.Label[labelIndices[i], partIndices[i]];
                        if (label.Category == -1)               // selection will be appliedonly to the labels without category, so that hidden
                            label.Category = CATEGORY_SELECTED;     //labels preserve their state
                    }
                    axMap1.Redraw();
                }
            }
        }
    }
}