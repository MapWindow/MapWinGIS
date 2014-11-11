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
            chkLayerVisible.Checked = m_layer.Visible;
            chkLayerPreview.Checked = m_settings.ShowLayerPreview;

            txtLayerName.Text = m_layer.Name;

            MapWinGIS.Map map = m_legend.Map;
            if (map != null)
            {
                txtComments.Text = map.get_LayerDescription(m_layerHandle);
            }

            MapWinGIS.Extents ext = m_shapefile.Extents;
            //string units = Globals.get_MapUnits();
            string units = "";
            string type = m_shapefile.ShapefileType.ToString().Substring(4).ToLower() + " shapefile";
            txtLayerSource.Text = "Type: " + type + Environment.NewLine +
                                  "Number of shapes: " + m_shapefile.NumShapes + Environment.NewLine +
                                  "Selected: " + m_shapefile.NumSelected + Environment.NewLine +
                                  "Source: " + m_shapefile.Filename + Environment.NewLine +
                                  "Bounds X: " + String.Format("{0:F2}", ext.xMin) + " to " + String.Format("{0:F2}", ext.xMax) + units + Environment.NewLine +
                                  "Bounds Y: " + String.Format("{0:F2}", ext.yMin) + " to " + String.Format("{0:F2}", ext.yMax) + units + Environment.NewLine +
                                  "Projection: " + m_shapefile.Projection;
        }

        /// <summary>
        /// Saves layer name from user input
        /// </summary>
        private void txtLayerName_Validated(object sender, EventArgs e)
        {
            m_layer.Name = txtLayerName.Text;
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
                m_layer.Name = txtLayerName.Text;
                MarkStateChanged();
                RedrawLegend();
            }
        }

        /// <summary>
        /// Saves layer name from user input
        /// </summary>
        private void txtComments_Validated(object sender, EventArgs e)
        {
            MapWinGIS.Map map = m_legend.Map;
            if (map != null)
            {
                map.set_LayerDescription(m_layerHandle, txtComments.Text);
                MarkStateChanged();
            }
        }

        /// <summary>
        /// Saves layer name from user input
        /// </summary>
        private void txtComments_KeyPress(object sender, KeyPressEventArgs e)
        {
            MapWinGIS.Map map = m_legend.Map;
            if (map != null)
            {
                if (e.KeyChar == (char)Keys.Enter)
                {
                    map.set_LayerDescription(m_layerHandle, txtComments.Text);
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
            sfPreview = new Shapefile();
            sfPreview.Open(m_shapefile.Filename, null);

            axMap1.CursorMode = tkCursorMode.cmNone;
            axMap1.MouseWheelSpeed = 1.0;

            Thread thread = new Thread(new ThreadStart(LoadPreviewLayer));
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();
        }

        /// <summary>
        /// Shows preview of the layer in the own map control on the general tab
        /// </summary>
        private void LoadPreviewLayer()
        {
            if (sfPreview.SourceType != tkShapefileSourceType.sstUninitialized)
            {
                if (sfPreview.ShapefileType == ShpfileType.SHP_POINT || sfPreview.ShapefileType == ShpfileType.SHP_POINTM || sfPreview.ShapefileType == ShpfileType.SHP_POINTZ)
                {
                    sfPreview.DefaultDrawingOptions.PointSize = 4;
                }
                int handle = axMap1.AddLayer(sfPreview, true);
                axMap1.ZoomToMaxExtents();
                axMap1.MapCursor = tkCursor.crsrArrow;
            }
        }
        #endregion
    }
}
