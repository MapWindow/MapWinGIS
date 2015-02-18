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
        // A simple GUI for editing attributes of the individual shapes.
        // </summary>
        public void EditAttributes(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;

            string filename = dataPath + "buildings.shp";

            if (!File.Exists(filename))
            {
                MessageBox.Show("Couldn't file the file: " + filename);
                return;
            }

            var sf = new Shapefile();
            sf.Open(filename, null);
            int layerHandle = axMap1.AddLayer(sf, true);
            sf = axMap1.get_Shapefile(layerHandle);     // in case a copy of shapefile was created by GlobalSettings.ReprojectLayersOnAdding

            if (!sf.Table.StartEditingTable(null))
            {
                MessageBox.Show("Failed to start edit mode: " + sf.Table.ErrorMsg[sf.LastErrorCode]);
            }
            else
            {
                string expression = "";
                for (int i = 1; i < sf.NumFields; i++)      // all the fields will be displayed apart the first one
                {
                    expression += "[" + sf.Field[i].Name + "]";
                    if (i != sf.NumFields - 1)
                    {
                        const string endLine = "\"\n\"";
                        expression += string.Format("+ {0} +", endLine);
                    }
                }
                sf.Labels.Generate(expression, tkLabelPositioning.lpCentroid, false);
                sf.Labels.TextRenderingHint = tkTextRenderingHint.SystemDefault;
                
                axMap1.SendMouseDown = true;
            
                axMap1.CursorMode = tkCursorMode.cmNone;
                MapEvents.MouseDownEvent += AxMap1MouseDownEvent2;  // change MapEvents to axMap1

                this.ZoomToValue(sf, "Name", "Lakeview");
            }
        }

        // <summary>
        // Zooms map to the specified object
        // </summary>
        public void ZoomToValue(Shapefile sf, string fieldName, string value)
        {
            int fieldIndex = sf.Table.FieldIndexByName[fieldName];
            if (fieldIndex != -1)
            {
                for (int i = 0; i < sf.NumShapes; i++)
                {
                    if ((string)sf.CellValue[fieldIndex, i] == value)
                    {
                        axMap1.Extents = sf.Shape[i].Extents;
                        axMap1.MapUnits = tkUnitsOfMeasure.umMeters;
                        axMap1.CurrentScale = 5000;
                    }
                }
            }
        }

        // <summary>
        // Handles mouse down event. Creates the editing form on the fly.
        // </summary>
        private void AxMap1MouseDownEvent2(object sender, _DMapEvents_MouseDownEvent e)
        {
            int layerHandle = axMap1.get_LayerHandle(0);  // it's assumed here that the layer we want to edit is the first 1 (with 0 index)
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
                        sf.set_ShapeSelected(shapes[0], true);  // selecting the shape we are about to edit
                        axMap1.Redraw(); Application.DoEvents();
                    
                        Form form = new Form();
                        for (int i = 0; i < sf.NumFields; i++)
                        {
                            System.Windows.Forms.Label label = new System.Windows.Forms.Label();
                            label.Left = 15;
                            label.Top = i * 30 + 5;
                            label.Text = sf.Field[i].Name;
                            label.Width = 60;
                            form.Controls.Add(label);

                            TextBox box = new TextBox();
                            box.Left = 80;
                            box.Top = label.Top;
                            box.Width = 80;
                            box.Text = sf.CellValue[i, shapes[0]].ToString();
                            box.Name = sf.Field[i].Name;
                            form.Controls.Add(box);
                        }

                        form.Width = 180;
                        form.Height = sf.NumFields * 30 + 70;

                        Button btn = new Button
                                     {
                                         Text = "Ok",
                                         Top = sf.NumFields*30 + 10,
                                         Left = 20,
                                         Width = 70,
                                         Height = 25
                                     };
                        btn.Click += BtnClick;
                        form.Controls.Add(btn);

                        btn = new Button
                              {
                                  Text = "Cancel",
                                  Top = sf.NumFields*30 + 10,
                                  Left = 100,
                                  Width = 70,
                                  Height = 25
                              };
                        btn.Click += BtnClick;
                        form.Controls.Add(btn);

                        form.FormClosed += FormFormClosed;
                        form.Text = "Shape: " + shapes[0];
                        form.ShowInTaskbar = false;
                        form.StartPosition = FormStartPosition.CenterParent;
                        form.FormBorderStyle = FormBorderStyle.FixedDialog;
                        form.MaximizeBox = false;
                        form.MinimizeBox = false;
                        form.ShowDialog(axMap1.Parent);
                    }
                }
            }

            // Execute this code if you want to save the results.
            // sf.StopEditingShapes(true, true, null);
        }

        // <summary>
        // Clears selected shapes on the closing of the form
        // </summary>
        void FormFormClosed(object sender, FormClosedEventArgs e)
        {
            int layerHandle = axMap1.get_LayerHandle(0);
            Shapefile sf = axMap1.get_Shapefile(layerHandle);
            if (sf != null)
            {
                sf.SelectNone();
                axMap1.Redraw();
            }
        }

        // <summary>
        // Submits new attibutes and updates the labels
        // </summary>
        void BtnClick(object sender, EventArgs e)
        {
            Form form = (sender as Control).Parent as Form;
            if (form == null) return;

            Button btn = sender as Button;
            if (btn != null)
            {
                int layerHandle = axMap1.get_LayerHandle(0);
                Shapefile sf = axMap1.get_Shapefile(layerHandle);
                if (sf != null)
                {
                    if (btn.Text == "Ok")
                    {

                        // now we shall find the selected shape, the one being edited
                        // in real-world application would be better of course to store the index of this shape in private variable
                        int shapeIndex = -1;
                        for (int i = 0; i < sf.NumShapes; i++)
                        {
                            if (sf.ShapeSelected[i])
                            {
                                shapeIndex = i;
                                break;
                            }
                        }

                        if (shapeIndex != -1)
                        {
                            foreach (Control control in form.Controls)
                            {
                                if (control is TextBox)
                                {
                                    int fieldIndex = sf.Table.FieldIndexByName[control.Name];
                                    if (fieldIndex != -1)
                                        sf.EditCellValue(fieldIndex, shapeIndex, control.Text);
                                }
                            }

                            sf.Labels.Expression = sf.Labels.Expression;    // update the labels
                            axMap1.Redraw();
                        }
                    }
                }
            }
            form.Close();
        }
    }
}