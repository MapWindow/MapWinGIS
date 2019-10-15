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

using MWLite.Symbology.Classes;
using MWLite.Symbology.Controls;
using MWLite.Symbology.Forms.Charts;
using MWLite.Symbology.Forms.Utilities;
using MWLite.Symbology.Classes;

namespace MWLite.Symbology.Forms
{
    using System;
    using System.Collections.Generic;
    using System.Windows.Forms;
    using MapWinGIS;
    using System.Drawing.Drawing2D;
    using System.Drawing;
    using MWLite.Symbology;

    partial class frmSymbologyMain
    {
        /// <summary>
        /// The code for initialization of the charts tab
        /// </summary>
        private void InitChartsTab()
        {

            icbChartColorScheme.ComboStyle = ImageComboStyle.ColorSchemeGraduated;
            //icbChartColorScheme.ColorSchemes = m_plugin.ChartColors;
            if (icbChartColorScheme.Items.Count > 0)
            {
                icbChartColorScheme.SelectedIndex = 0;
            }

            MapWinGIS.Charts charts = _shapefile.Charts;
            chkChartsVisible.Checked = charts.Visible;
            //cboChartVerticalPosition.SelectedIndex = (int)charts.VerticalPosition;

            optChartBars.Checked = (charts.ChartType == tkChartType.chtBarChart);
            optChartsPie.Checked = (charts.ChartType == tkChartType.chtPieChart);
        }

        /// <summary>
        ///  Draws preview on the charts tab
        /// </summary>
        private void DrawChartsPreview()
        {
            Rectangle rect = pctCharts.ClientRectangle;
            Bitmap bmp = new Bitmap(rect.Width, rect.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            if (_shapefile.Charts.Count > 0 && _shapefile.Charts.NumFields > 0)
            {
                Graphics g = Graphics.FromImage(bmp);
                IntPtr ptr = g.GetHdc();

                int width = rect.Width;
                int height = rect.Height;

                MapWinGIS.Charts charts = _shapefile.Charts;

                if (charts.ChartType == MapWinGIS.tkChartType.chtPieChart)
                    charts.DrawChart(ptr.ToInt32(), (width - charts.IconWidth) / 2, (height - charts.IconHeight) / 2, false, Colors.ColorToUInteger(Color.White));
                else
                    charts.DrawChart(ptr.ToInt32(), (width - charts.IconWidth) / 2, (height - charts.IconHeight) / 2, false, Colors.ColorToUInteger(Color.White));

                g.ReleaseHdc(ptr);
            }
            pctCharts.Image = bmp;
        }


        /// <summary>
        /// Opens form to change chart appearance
        /// </summary>
        private void btnChartAppearance_Click(object sender, EventArgs e)
        {
            ChartStyleForm form = new ChartStyleForm(_legend, _shapefile, false, _layerHandle);
            form.ShowDialog();

            // even if cancel was hit, a user could have applied the options
            bool state = _noEvents;
            _noEvents = true;
            optChartBars.Checked = (_shapefile.Charts.ChartType == tkChartType.chtBarChart);
            optChartsPie.Checked = (_shapefile.Charts.ChartType == tkChartType.chtPieChart);
            _noEvents = state;

            DrawChartsPreview();
            RefreshControlsState(null, null);
            RedrawMap();
        }

        /// <summary>
        /// Removes all chart fields and redraws the map
        /// </summary>
        private void btnClearCharts_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Do you want to delete charts?", LegendControl.Legend.AppName, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                _shapefile.Charts.ClearFields();
                _shapefile.Charts.Clear();
                RefreshControlsState(null, null);
                DrawChartsPreview();
                RedrawMap();
            }
        }

        /// <summary>
        /// Updating preview for charts
        /// </summary>
        private void optChartBars_CheckedChanged(object sender, EventArgs e)
        {
            GUI2Settings(null, null);
            DrawAllPreviews();
        }

        /// <summary>
        /// Updating colors of the charts
        /// </summary>
        private void icbChartColorScheme_SelectedIndexChanged(object sender, EventArgs e)
        {
            List<ColorBlend> schemes = icbChartColorScheme.ColorSchemes.List;
            if (schemes != null && icbChartColorScheme.SelectedIndex >= 0)
            {
                GUI2Settings(null, null);
                DrawChartsPreview();
                RedrawMap();
            }
        }

        /// <summary>
        /// Sets the properties of the labels based upon user input
        /// </summary>
        private void UpdateCharts()
        {
            MapWinGIS.Charts charts = _shapefile.Charts;
            charts.Visible = chkChartsVisible.Checked;
            charts.ChartType = optChartBars.Checked ? MapWinGIS.tkChartType.chtBarChart : MapWinGIS.tkChartType.chtPieChart;
            this.UpdateFieldColors();
            DrawChartsPreview();
        }

        private void UpdateFieldColors()
        {
            List<ColorBlend> schemes = icbChartColorScheme.ColorSchemes.List;
            if (schemes != null && icbChartColorScheme.SelectedIndex >= 0)
            {
                ColorBlend blend = (ColorBlend)schemes[icbChartColorScheme.SelectedIndex];
                MapWinGIS.ColorScheme scheme = ColorSchemes.ColorBlend2ColorScheme(blend);
                if (scheme != null)
                {
                    for (int i = 0; i < _shapefile.Charts.NumFields; i++)
                    {
                        MapWinGIS.ChartField field = _shapefile.Charts.get_Field(i);
                        double value = (double)(i) / (double)(_shapefile.Charts.NumFields - 1);
                        field.Color = scheme.get_GraduatedColor(value);
                    }
                }

            }
        }

        /// <summary>
        /// Opens editor of color schemes
        /// </summary>
        private void btnChartsEditColorScheme_Click(object sender, EventArgs e)
        {
            frmColorSchemes form = new frmColorSchemes(ref Globals.ChartColors);
            if (form.ShowDialog(this) == DialogResult.OK)
            {
                icbChartColorScheme.ColorSchemes = Globals.ChartColors;
            }
            form.Dispose();
        }
    }
}
