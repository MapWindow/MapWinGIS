using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // the handle of the drawing layer
        private int _mDrawingHandle = -1;

        // <summary>
        // Opens a shapefile, registers event handler
        // </summary>
        public void ToolTip(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;

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
                axMap1.SendMouseMove = true;
                axMap1.ShowRedrawTime = true;
                axMap1.MapUnits = tkUnitsOfMeasure.umMeters;
                axMap1.CurrentScale = 50000;
                axMap1.CursorMode = tkCursorMode.cmNone;
                MapEvents.MouseMoveEvent += AxMap1MouseMoveEvent;  // change MapEvents to axMap1

                _mDrawingHandle = axMap1.NewDrawing(tkDrawReferenceList.dlScreenReferencedList);
                Labels labels = axMap1.get_DrawingLabels(_mDrawingHandle);
                labels.FrameVisible = true;
                labels.FrameType = tkLabelFrameType.lfRectangle;
            }
        }

        // <summary>
        // Handles mouse move event. Determines which shape is under cursor. Calls drawing routine.
        // </summary>
        void AxMap1MouseMoveEvent(object sender, _DMapEvents_MouseMoveEvent e)
        {
            Labels labels = axMap1.get_DrawingLabels(0);
            labels.Clear();

            // it's assumed here that the layer we want to edit is the first 1 (with 0 index)
            int layerHandle = axMap1.get_LayerHandle(0);  
            var sf = axMap1.get_Shapefile(layerHandle);
            if (sf != null)
            {
                double projX = 0.0;
                double projY = 0.0;
                axMap1.PixelToProj(e.x, e.y, ref projX, ref projY);

                object result = null;
                var ext = new Extents();
                ext.SetBounds(projX, projY, 0.0, projX, projY, 0.0);
                if (sf.SelectShapes(ext, 0.0, SelectMode.INTERSECTION, ref result))
                {
                    int[] shapes = result as int[];
                    if (shapes != null && shapes.Length == 1)
                    {
                        string s = "";
                        for (int i = 0; i < sf.NumFields; i++)
                        {
                            s += sf.Field[i].Name + ": " + sf.CellValue[i, shapes[0]] + "\n";
                        }
                        labels.AddLabel(s, e.x + 80, e.y);
                    }
                }
            }

            axMap1.Redraw2(tkRedrawType.RedrawSkipDataLayers);
        }
    }
}