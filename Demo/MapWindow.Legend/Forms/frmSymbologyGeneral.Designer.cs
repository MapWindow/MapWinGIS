// ********************************************************************************************************
// <copyright file="MapWindow.Legend.cs" company="MapWindow.org">
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

namespace MapWindow.Legend.Forms
{
    using System;
    using System.Windows.Forms;
    using System.Threading;
    using MapWinGIS;

    partial class frmSymbologyMain
    {

        #region General tab

        /// <summary>
        /// Sets the state of controls on the general tab on loading
        /// </summary>
        private void InitGeneralTab()
        {
            chkLayerVisible.Checked = _layer.Visible;
            chkLayerPreview.Checked = _settings.ShowLayerPreview;

            txtLayerName.Text = _layer.Name;

            txtLayerSource.Text = GetLayerDescription();
        }

        private string GetLayerDescription()
        {
            string s = "";

            MapWinGIS.Map map = _legend.Map;
            if (map != null)
            {
                txtComments.Text = map.get_LayerDescription(_layerHandle);
            }

            MapWinGIS.Extents ext = _shapefile.Extents;
            //string units = Globals.get_MapUnits();
            string units = "";
            string type = _shapefile.ShapefileType.ToString().Substring(4).ToLower() + " shapefile";

            var layer = map.get_OgrLayer(_layerHandle);
            if (layer != null)
            {
                s += "Datasource type: OGR layer" + Environment.NewLine;
                s += "Driver name: " + layer.DriverName + Environment.NewLine;
                s += "Connection string: " + layer.GetConnectionString() + Environment.NewLine;
                s += "Layer type: " + layer.SourceType.ToString() + Environment.NewLine;
                s += "Name or query: " + layer.GetSourceQuery() + Environment.NewLine;
                s += "Support editing: " + layer.SupportsEditing[tkOgrSaveType.ostSaveAll] + Environment.NewLine;
                s += "Dynamic loading: " + layer.DynamicLoading + "\n";
            }
            else
            {
                s += "Datasource type: ESRI Shapefile" + Environment.NewLine;
            }

            s += "Type: " + type + Environment.NewLine +
                        "Number of shapes: " + _shapefile.NumShapes + Environment.NewLine +
                        "Selected: " + _shapefile.NumSelected + Environment.NewLine +
                        "Source: " + _shapefile.Filename + Environment.NewLine +
                        "Bounds X: " + String.Format("{0:F2}", ext.xMin) + " to " + String.Format("{0:F2}", ext.xMax) + units + Environment.NewLine +
                        "Bounds Y: " + String.Format("{0:F2}", ext.yMin) + " to " + String.Format("{0:F2}", ext.yMax) + units + Environment.NewLine +
                        "Projection: " + _shapefile.Projection;
            return s;
        }

        /// <summary>
        /// Saves layer name from user input
        /// </summary>
        private void txtLayerName_Validated(object sender, EventArgs e)
        {
            _layer.Name = txtLayerName.Text;
            MarkStateChanged();
            RedrawLegend();
        }

        /// <summary>
        /// Saves layer name from user input
        /// </summary>
        private void txtLayerName_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.Enter)
            {
                _layer.Name = txtLayerName.Text;
                MarkStateChanged();
                RedrawLegend();
            }
        }

        /// <summary>
        /// Saves layer name from user input
        /// </summary>
        private void txtComments_Validated(object sender, EventArgs e)
        {
            MapWinGIS.Map map = _legend.Map;
            if (map != null)
            {
                map.set_LayerDescription(_layerHandle, txtComments.Text);
                MarkStateChanged();
            }
        }

        /// <summary>
        /// Saves layer name from user input
        /// </summary>
        private void txtComments_KeyPress(object sender, KeyPressEventArgs e)
        {
            MapWinGIS.Map map = _legend.Map;
            if (map != null)
            {
                if (e.KeyChar == (char)Keys.Enter)
                {
                    map.set_LayerDescription(_layerHandle, txtComments.Text);
                    MarkStateChanged();
                }
            }
        }
        #endregion

        #region Layer preview
        /// <summary>
        /// Updates the state of the layer preview
        /// </summary>
        private void chkLayerPreview_CheckedChanged(object sender, EventArgs e)
        {
            if (axMap1.NumLayers == 0 && chkLayerPreview.Checked)
            {
                ShowLayerPreview();
            }
            axMap1.Visible = chkLayerPreview.Checked;

        }

        /// <summary>
        /// Refreshes the layer preview
        /// </summary>
        private void ShowLayerPreview()
        {
            axMap1.Visible = true;
            
            var sf = new Shapefile();
            axMap1.AddLayer(_shapefile, true);

            axMap1.CursorMode = tkCursorMode.cmNone;
            axMap1.MouseWheelSpeed = 1.0;
        }
        #endregion
    }
}
