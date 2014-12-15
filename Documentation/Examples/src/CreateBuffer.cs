using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Creates several buffers around the waterways.
        // </summary>
        public void CreateBuffer(AxMap axMap1, string dataPath, System.Windows.Forms.ToolStripStatusLabel label)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;

            string filename = dataPath + "waterways.shp";
            if (!File.Exists(filename))
            {
                MessageBox.Show("The shapefile with rivers wasn't found: " + filename);
            }
            else
            {
                var callback = new Callback(label);
            
                var sf = new Shapefile();
                if (!sf.Open(filename, callback))
                {
                    MessageBox.Show(sf.ErrorMsg[sf.LastErrorCode]);
                }
                else
                {
                    int layerHandle = axMap1.AddLayer(sf, true);
                    sf = axMap1.get_Shapefile(layerHandle);     // in case a copy of shapefile was created by AxMap.ProjectionMismatchBehavior
                    
                    var utils = new Utils();
                    sf.DefaultDrawingOptions.LineWidth = 3.0f;
                    sf.DefaultDrawingOptions.LineColor = utils.ColorByName(tkMapColor.Blue);
                
                    const double distance = 150; // meters
                    var buffers = new List<Shapefile>();
                    for (int i = 1; i < 5; i++)
                    {
                        Shapefile sfBuffer = sf.BufferByDistance(distance * i, 30, false, true);
                        if (sfBuffer == null)
                        {
                            MessageBox.Show("Failed to calculate the buffer: " + sf.ErrorMsg[sf.LastErrorCode]);
                            return;
                        }
                        else
                        {
                            sfBuffer.GlobalCallback = callback;
                            buffers.Add(sfBuffer);
                        }
                    }

                    // now subtract smaller buffers from larger ones
                    for (int i = buffers.Count - 1; i > 0; i--)
                    {
                        Shapefile sfDiff = buffers[i].Difference(false, buffers[i - 1], false);
                        if (sfDiff == null)
                        {
                            MessageBox.Show("Failed to calculate the difference: " + sf.ErrorMsg[sf.LastErrorCode]);
                            return;
                        }
                        else
                        {
                            buffers[i] = sfDiff;
                        }
                    }

                    // pass all the resulting shapes to a single shapefile and mark their distance
                    Shapefile sfResult = buffers[0].Clone();
                    sfResult.GlobalCallback = callback;
                    int fieldIndex = sfResult.EditAddField("Distance", FieldType.DOUBLE_FIELD, 10, 12);
                
                    for (int i = 0; i < buffers.Count; i++ )
                    {                
                        Shapefile sfBuffer = buffers[i];
                        for (int j = 0; j < sfBuffer.NumShapes; j++)
                        {
                            int index = sfResult.NumShapes; 
                            sfResult.EditInsertShape(sfBuffer.Shape[j].Clone(), ref index);
                            sfResult.EditCellValue(fieldIndex, index, distance * (i + 1));
                        }
                    }
                
                    // create visualization categories
                    sfResult.DefaultDrawingOptions.FillType = tkFillType.ftStandard;
                    sfResult.Categories.Generate(fieldIndex, tkClassificationType.ctUniqueValues, 0);
                    sfResult.Categories.ApplyExpressions();

                    // apply color scheme
                    var scheme = new ColorScheme();
                    scheme.SetColors2(tkMapColor.LightBlue, tkMapColor.LightYellow);
                    sfResult.Categories.ApplyColorScheme(tkColorSchemeType.ctSchemeGraduated, scheme);

                    layerHandle = axMap1.AddLayer(sfResult, true);
                    axMap1.Redraw();

                    //sfResult.SaveAs(@"c:\buffers.shp", null);
                }
            }
        }
    }

    class Callback : ICallback
    {
        private ToolStripStatusLabel m_label = null;

        public Callback(ToolStripStatusLabel label)
        {
            m_label = label;
            if (label == null)
                throw new NullReferenceException("No reference to the label was provided");
        }
    
        public void Error(string KeyOfSender, string ErrorMsg)
        {
            Debug.Print("Error reported: " + ErrorMsg);
        }

        public void Progress(string KeyOfSender, int Percent, string Message)
        {
            m_label.Text = Message + ": " + Percent + "%";
            Application.DoEvents();
        }
    }
}