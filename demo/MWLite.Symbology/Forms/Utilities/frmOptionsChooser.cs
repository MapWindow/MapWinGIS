// ********************************************************************************************************
// <copyright file="MWLite.Symbology.cs" company="MapWindow.org">
// Copyright (c) MapWindow.org. All rights reserved.
// </copyright>
// The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at 
// http:// Www.mozilla.org/MPL/ 
// Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
// ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
// limitations under the License. 
// 
// The Initial Developer of this version of the Original Code is Sergei Leschinski
// 
// Contributor(s): (Open source contributors should list themselves and their modifications here). 
// Change Log: 
// Date            Changed By      Notes
// ********************************************************************************************************

using System;
using System.Windows.Forms;
using MWLite.Symbology.Classes;
using MWLite.Symbology.Classes;

namespace MWLite.Symbology.Forms.Utilities
{
    public partial class frmOptionsChooser : Form
    {
        private LegendControl.Legend m_legend = null;

        // the name of data source to be loaded
        private string m_filename = "";

        // handle of the layer
        private int m_handle = -1;
        
        // no events during the init of the form
        private bool m_NoEvents = false;

        // initial serialized state of the layer
        private string m_initState = "";
        
        /// <summary>
        /// Creates a new instance of the frmOptionsChooser class
        /// </summary>
        public frmOptionsChooser(LegendControl.Legend legend, string filename, int handle)
        {
            InitializeComponent();
            if (!System.IO.File.Exists(filename))
            {
                throw new Exception("File doesn't exists");
                return;
            }

            m_legend = legend;

            m_filename = filename;
            m_handle = handle;
            
            Globals.FillSymbologyList(listView1, filename, false, ref m_NoEvents);

            m_NoEvents = true;
            LoadLayer();
            m_NoEvents = false;
        }

        /// <summary>
        /// Loads layer from datasource specifed by filename
        /// </summary>
        private void LoadLayer()
        {
            axMap1.RemoveAllLayers();
            int handle = -1;
            string ext = System.IO.Path.GetExtension(m_filename).ToLower();
            if (ext == ".shp")
            {
                MapWinGIS.Shapefile sf = new MapWinGIS.Shapefile();
                if (sf.Open(m_filename, null))
                {
                    handle = axMap1.AddLayer(sf, true);
                    sf.Labels.SavingMode = MapWinGIS.tkSavingMode.modeNone;
                    sf.Charts.SavingMode = MapWinGIS.tkSavingMode.modeNone;
                }
            }
            else
            {
                MapWinGIS.Image img = new MapWinGIS.Image();
                if (img.Open(m_filename, MapWinGIS.ImageType.USE_FILE_EXTENSION, false, null))
                {
                    handle = this.axMap1.AddLayer(img, true);
                }
            }

            // serializing initial state to display random options afterwrads
            m_initState = axMap1.SerializeLayer(handle);
        }

        /// <summary>
        /// Applies a given set of options to the layer
        /// </summary>
        private void btnOk_Click(object sender, EventArgs e)
        {
            MapWinGIS.Map map = m_legend.Map;
            if (map != null)
            {
                if (listView1.SelectedItems.Count > 0)
                {
                    if (listView1.SelectedItems[0].Index > 0)    // otherwise that's random options
                    {
                        int row = listView1.SelectedItems[0].Index;
                        string name = listView1.SelectedItems[0].Text;
                        SymbologyType type = (SymbologyType)listView1.SelectedItems[0].Tag;

                        if (type == SymbologyType.Default)
                        {
                            name = "";
                        }
                        string description = "";
                        bool res = map.LoadLayerOptions(m_handle, name, ref description);
                    }
                }
            }
        }

        /// <summary>
        /// Cancels layer adding
        /// </summary>
        private void btnCancel_Click(object sender, EventArgs e)
        {
            // do nothing
        }

        /// <summary>
        /// Closes the form by double clicking on the options
        /// </summary>
        private void listBox1_DoubleClick(object sender, EventArgs e)
        {
            btnOk_Click(null, null);
            this.DialogResult = DialogResult.OK;
        }

        /// <summary>
        /// Sets symbology loading behavior to deafult options mode
        /// </summary>
        private void chkDontShow_CheckedChanged(object sender, EventArgs e)
        {
            //m_mapWin.ApplicationInfo.SymbologyLoadingBehavior = MapWindow.Interfaces.SymbologyBehavior.DefaultOptions;
        }

        /// <summary>
        /// Unloading the layer before closing form
        /// </summary>
        private void frmOptionsChooser_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (axMap1 != null)
            {
                axMap1.RemoveAllLayers();
                axMap1 = null;
            }
            GC.Collect();
        }

        /// <summary>
        /// Display selected options in the preview window
        /// </summary>
        private void dgv_CurrentCellChanged(object sender, EventArgs e)
        {
            if (!(listView1.SelectedItems.Count > 0))
                return;

            int row = listView1.SelectedItems[0].Index;
            int handle = axMap1.get_LayerHandle(0);
            string name = listView1.SelectedItems[0].Text;
            SymbologyType type = (SymbologyType)listView1.SelectedItems[0].Tag;

            if (type == SymbologyType.Random)
            {
                lblDescription.Text = Globals.GetSymbologyDescription(SymbologyType.Random);
                lblDescription.Refresh();
                var sf = axMap1.get_Shapefile(handle);
                if (!axMap1.DeserializeLayer(handle, m_initState))
                {
                    Globals.MessageBoxError("Error while loading options");
                }
                else
                {
                    axMap1.Redraw();
                }
                return;
            }
            else if (type == SymbologyType.Default)
            {
                name = "";
                lblDescription.Text = Globals.GetSymbologyDescription(SymbologyType.Default);
                lblDescription.Refresh();
            }
            

            // previously saved options (.mwsymb or .mwsr file)
            string description = "";
            if (!axMap1.LoadLayerOptions(handle, name, ref description))
            {
                Globals.MessageBoxError("Error while loading options");
            }
            else
            {
                if (name != "")
                {
                    this.lblDescription.Text = description == "Enter description" ? "" :description;
                    lblDescription.Refresh();
                }
                axMap1.Redraw();
            }
        }

        /// <summary>
        /// Loads selected set of options. 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void dgv_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            btnOk_Click(null, null);
            this.DialogResult = DialogResult.OK;
        }
    }
}
