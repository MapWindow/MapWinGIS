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
        // Loads the layers and registers event handler
        // </summary>
        public void RemoveShape(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            axMap1.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

            string filename = dataPath + "natural.shp";

            if (!File.Exists(filename))
            {
                MessageBox.Show("Couldn't file the file: " + filename);
                return;
            }

            var sf = new Shapefile();
            sf.Open(filename, null);
            int layerHandle = axMap1.AddLayer(sf, true);
            sf = axMap1.get_Shapefile(layerHandle);     // in case a copy of shapefile was created by AxMap.ProjectionMismatchBehavior

            if (!sf.StartEditingShapes(true, null))
            {
                MessageBox.Show("Failed to start edit mode: " + sf.ErrorMsg[sf.LastErrorCode]);
            }
            else
            {
                int fieldIndex = sf.EditAddField("ShapeIndex", FieldType.INTEGER_FIELD, 0, 0);
                for (int i = 0; i < sf.NumShapes; i++)
                    sf.EditCellValue(fieldIndex, i, i);
                sf.Labels.Generate("[ShapeIndex]", tkLabelPositioning.lpCentroid, false);
                sf.Labels.Synchronized = true;
                sf.Labels.TextRenderingHint = tkTextRenderingHint.SystemDefault;
                
                axMap1.SendMouseDown = true;
                MapEvents.MouseDownEvent += AxMap1MouseDownEvent1;  // change MapEvents to axMap1
                axMap1.MapUnits = tkUnitsOfMeasure.umMeters;
                axMap1.CurrentScale = 50000;
                axMap1.CursorMode = tkCursorMode.cmNone;
            }
        }

        /// <summary>
        /// Handles mouse down event, removes the shape under cursor
        /// </summary>
        private void AxMap1MouseDownEvent1(object sender, _DMapEvents_MouseDownEvent e)
        {
            // it's assumed here that the layer we want to edit is the first 1 (with 0 index)
            int layerHandle = axMap1.get_LayerHandle(0);  
            Shapefile sf = axMap1.get_Shapefile(layerHandle);
            if (sf != null)
            {
                double projX = 0.0;
                double projY = 0.0;
                axMap1.PixelToProj(e.x, e.y, ref projX, ref projY);
            
                object result = null;
                Extents ext = new Extents();
                ext.SetBounds(projX, projY, 0.0, projX, projY, 0.0);
                if (sf.SelectShapes(ext, 0.0, SelectMode.INCLUSION, ref result))
                {
                    int[] shapes = result as int[];
                    if (shapes == null) return;

                    if (shapes.Length > 1)
                    {
                        string s = "More than one shapes were selected. Shape indices:";
                        for (int i = 0; i < shapes.Length; i++)
                            s += shapes[i] + Environment.NewLine;
                        MessageBox.Show(s);
                    }
                    else
                    {
                        if (!sf.EditDeleteShape(shapes[0]))
                        {
                            MessageBox.Show("Failed to delete a shape: " + sf.ErrorMsg[sf.LastErrorCode]);
                        }
                        else
                        {
                            MessageBox.Show("Shape was removed. Index = " + shapes[0]);
                            sf.Labels.Expression = sf.Labels.Expression;
                            for (int i = 0; i < sf.Labels.Count; i++)
                                sf.Labels.Label[i, 0].Text += "; " + i;
                            axMap1.Redraw();
                        }
                    }
                }
                else
                {
                    MessageBox.Show("Nothing was selected");
                }
            }

            // Execute this code if you want to save the results.
            // sf.StopEditingShapes(true, true, null);
        }
    }
}