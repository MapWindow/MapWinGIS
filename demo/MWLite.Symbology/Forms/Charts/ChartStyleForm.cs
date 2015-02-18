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
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using MWLite.Symbology.Classes;
using MWLite.Symbology.Controls;
using MWLite.Symbology.Forms.Utilities;
using MapWinGIS;
using MWLite.Symbology.Classes;

namespace MWLite.Symbology.Forms.Charts
{
    public partial class ChartStyleForm : Form
    {
        private MapWinGIS.Shapefile _shapefile = null;
        private MapWinGIS.Charts _charts = null;
        private bool _noEvents = false;
        private static int _selectedTab = 0;
        private string _initState = "";
        private int _handle = -1;
        private LegendControl.Legend m_legend = null;

        /// <summary>
        /// Initializes a new instance of the ChartStyleForm class
        /// </summary>
        public ChartStyleForm(LegendControl.Legend legend, MapWinGIS.Shapefile sf, bool legendCall, int handle)
        {
            InitializeComponent();
            _shapefile = sf;
            _charts = sf.Charts;
            m_legend = legend;
            _handle = handle;

            MapWinGIS.tkSavingMode mode = _charts.SavingMode;
            _charts.SavingMode = MapWinGIS.tkSavingMode.modeNone;
            _initState = _charts.Serialize();
            _charts.SavingMode = mode;

            _noEvents = true;

            panelPieChart.Top = panelBarChart.Top;
            panelPieChart.Left = panelBarChart.Left;

            foreach (FontFamily family in FontFamily.Families)
                cboFontName.Items.Add(family.Name);

            cboValuesStyle.Items.Clear();
            cboValuesStyle.Items.Add("Horizontal");
            cboValuesStyle.Items.Add("Vertical");

            cboChartVerticalPosition.Items.Clear();
            cboChartVerticalPosition.Items.Add("Above current layer");
            cboChartVerticalPosition.Items.Add("Above all layers");

            optBarCharts.Checked = (_charts.ChartType == MapWinGIS.tkChartType.chtBarChart);
            optPieCharts.Checked = (_charts.ChartType == MapWinGIS.tkChartType.chtPieChart);

            // initializing for list of color schemes
            icbColors.ColorSchemes = Globals.ChartColors;
            icbColors.ComboStyle = ImageComboStyle.ColorSchemeGraduated;
            if (icbColors.Items.Count > 0)
            {
                icbColors.SelectedIndex = 0;
            }

            string[] scales = { "1", "10", "100", "1000", "5000", "10000", "25000", "50000", "100000", 
                                "250000", "500000", "1000000", "10000000" };
            cboMinScale.Items.Clear();
            cboMaxScale.Items.Clear();
            for (int i = 0; i < scales.Length; i++)
            {
                cboMinScale.Items.Add(scales[i]);
                cboMaxScale.Items.Add(scales[i]);
            }

            txtChartExpression.Text = sf.Charts.VisibilityExpression;

            SetChartsType();

            InitFields();

            _noEvents = false;

            Settings2GUI();

            Draw();

            RefreshControlsState();

            tabControl1.SelectedIndex = _selectedTab;
        }
            
        /// <summary>
        /// Fills the fields tab
        /// </summary>
        private void InitFields()
        {
            // building list of fields
            listLeft.Items.Clear();
            listRight.Items.Clear();
            for (int i = 0; i < _shapefile.NumFields; i++)
            {
                if (_shapefile.get_Field(i).Type != FieldType.STRING_FIELD)
                    listLeft.Items.Add(_shapefile.get_Field(i).Name);
            }

            // in case some fields have been chosen we must show them
            if (_shapefile.Charts.NumFields > 0)
            {
                for (int i = 0; i < _shapefile.Charts.NumFields; i++)
                {
                    string name = _shapefile.Charts.get_Field(i).Name.ToLower();
                    for (int j = 0; j < listLeft.Items.Count; j++)
                    {
                        if (listLeft.Items[j].ToString().ToLower() == name)
                        {
                            listRight.Items.Add(listLeft.Items[j]);
                            listLeft.Items.Remove(listLeft.Items[j]);
                            break;
                        }
                    }
                }
            }

            if (listLeft.Items.Count > 0)
            {
                listLeft.SelectedIndex = 0;
            }

            if (listRight.Items.Count > 0)
            {
                listRight.SelectedIndex = 0;
            }

            // Filling size and normalization fields
            cboChartSizeField.Items.Clear();
            cboChartNormalizationField.Items.Clear();

            cboChartSizeField.Items.Add("<None>");          // default
            cboChartNormalizationField.Items.Add("<None>");

            for (int i = 0; i < _shapefile.NumFields; i++)
            {
                MapWinGIS.Field fld = _shapefile.get_Field(i);
                if (fld.Type != FieldType.STRING_FIELD)
                {
                    cboChartSizeField.Items.Add(fld.Name);
                    cboChartNormalizationField.Items.Add(fld.Name);
                }
            }

            if (cboChartSizeField.Items.Count >= 0)
                cboChartSizeField.SelectedIndex = 0;

            if (cboChartNormalizationField.Items.Count >= 0)
                cboChartNormalizationField.SelectedIndex = 0;

            // size field
            MapWinGIS.Charts charts = _shapefile.Charts;
            if (charts.SizeField >= 0 && charts.SizeField < cboChartSizeField.Items.Count - 1)  // first item is <none>
            {
                Field fld = _shapefile.get_Field(charts.SizeField);
                if (fld != null)
                {
                    for (int i = 2; i < cboChartSizeField.Items.Count; i++)     // 2 = <none> and <sum of fields>
                    {
                        if (fld.Name == cboChartSizeField.Items[i].ToString())
                        {
                            cboChartSizeField.SelectedIndex = i;
                            break;
                        }
                    }
                }
            }
            else
            {
                cboChartSizeField.SelectedIndex = 0;
            }

            // normalization field
            if (charts.NormalizationField >= 0 && charts.NormalizationField < cboChartNormalizationField.Items.Count - 1)  // first item is <none>
            {
                Field fld = _shapefile.get_Field(charts.NormalizationField);
                if (fld != null)
                {
                    for (int i = 2; i < cboChartNormalizationField.Items.Count; i++)     // 2 = <none> and <sum of fields>
                    {
                        if (fld.Name == cboChartNormalizationField.Items[i].ToString())
                        {
                            cboChartNormalizationField.SelectedIndex = i;
                            break;
                        }
                    }
                }
            }
            else
            {
                cboChartNormalizationField.SelectedIndex = 0;
            }
        }

        /// <summary>
        /// Activating pie charts
        /// </summary>
        private void optPieCharts_CheckedChanged(object sender, EventArgs e)
        {
            _charts.ChartType = MapWinGIS.tkChartType.chtPieChart;
            SetChartsType();
        }

        /// <summary>
        /// Activating bar charts
        /// </summary>
        private void optBarCharts_CheckedChanged(object sender, EventArgs e)
        {
            _charts.ChartType = MapWinGIS.tkChartType.chtBarChart;
            SetChartsType();
        }

        /// <summary>
        /// Chosing the type of charts
        /// </summary>
        private void SetChartsType()
        {
            if (_charts.ChartType == MapWinGIS.tkChartType.chtBarChart)
            {
                tabControl1.TabPages[1].Text = "Bar charts";
                panelBarChart.Visible = true;
                panelPieChart.Visible = false;
            }
            else if (_charts.ChartType == MapWinGIS.tkChartType.chtPieChart)
            {
                tabControl1.TabPages[1].Text = "Pie charts";
                panelPieChart.Visible = true;
                panelBarChart.Visible = false;
            }
            if (!_noEvents)
                btnApply.Enabled = true;
            RefreshControlsState();
            Draw();
        }

        /// <summary>
        /// Copies chart settings from the controls to the charts class
        /// </summary>
        private void GUI2Settings(object sender, EventArgs e)
        {
             if (_charts == null) 
                 return;

             if (_noEvents)
                 return;

            _charts.BarHeight = (int)udBarHeight.Value;
            _charts.BarWidth =  (int)udBarWidth.Value;
            _charts.PieRadius = (int)udPieRadius.Value;
            _charts.PieRadius2 = (int)udPieRadius2.Value;
            _charts.Thickness = (double)udThickness.Value;
            _charts.Tilt = (double)udTilt.Value;

            _charts.Use3DMode = chk3DMode.Checked;
            _charts.Visible = chkVisible.Checked;

            _charts.ValuesFontBold = chkFontBold.Checked;
            _charts.ValuesFontColor = Colors.ColorToUInteger(clpFont.Color);

            _charts.ValuesFontItalic = chkFontItalic.Checked;
            _charts.ValuesFontName = cboFontName.Text;
            _charts.ValuesFontSize = (int)udFontSize.Value;
            
            _charts.ValuesFrameColor = Colors.ColorToUInteger(clpFrame.Color);
            _charts.ValuesFrameVisible = chkValuesFrame.Checked;
            _charts.ValuesVisible = chkValuesVisible.Checked;
            _charts.ValuesStyle = (MapWinGIS.tkChartValuesStyle)cboValuesStyle.SelectedIndex;

            _charts.Transparency = transparencyControl1.Value;

            _charts.UseVariableRadius = (cboChartSizeField.SelectedIndex > 0);
            _charts.OffsetX = (int)udChartsOffsetX.Value;
            _charts.OffsetY = (int)udChartsOffsetY.Value;
            _charts.CollisionBuffer = (int)udChartsBuffer.Value;
            _charts.Transparency = transparencyControl1.Value;

            double val;
            if (double.TryParse(cboMinScale.Text, out val))
            {
                _shapefile.Charts.MinVisibleScale = val;
            }

            if (double.TryParse(cboMaxScale.Text, out val))
            {
                _shapefile.Charts.MaxVisibleScale = val;
            }
            _shapefile.Charts.DynamicVisibility = chkDynamicVisibility.Checked;

            // size field
            if (cboChartSizeField.SelectedIndex > 0)
            {
                string name = (string)cboChartSizeField.SelectedItem;
                for (int i = 0; i < _shapefile.NumFields; i++)
                {
                    Field fld = _shapefile.get_Field(i);
                    if (fld != null)
                    {
                        if (fld.Name == name)
                        {
                            _charts.SizeField = i;
                            break;
                        }
                    }
                }
            }
            else
            {
                _charts.SizeField = -1;
            }

            // normalization field
            if (cboChartNormalizationField.SelectedIndex > 0)
            {
                string name = (string)cboChartNormalizationField.SelectedItem;
                for (int i = 0; i < _shapefile.NumFields; i++)
                {
                    Field fld = _shapefile.get_Field(i);
                    if (fld != null)
                    {
                        if (fld.Name == name)
                        {
                            _charts.NormalizationField = i;
                            break;
                        }
                    }
                }
            }
            else
            {
                _charts.NormalizationField = -1;
            }

            btnApply.Enabled = true;

            RefreshControlsState();

            Draw();
        }

        /// <summary>
        /// Performs drawing
        /// </summary>
        private void Draw()
        {
            int width = pictureBox1.ClientRectangle.Width;
            int height = pictureBox1.ClientRectangle.Height;

            Bitmap bmp = new Bitmap(width, height,  System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            Graphics g = Graphics.FromImage(bmp);
            IntPtr hdc = g.GetHdc();
            uint backColor = Colors.ColorToUInteger(this.BackColor);
            if (_charts.Visible)
            {
                if (_charts.ChartType == MapWinGIS.tkChartType.chtPieChart)
                    _charts.DrawChart(hdc, (width - _charts.IconWidth) / 2, (height - _charts.IconHeight) / 2,  false, backColor);
                else
                    _charts.DrawChart(hdc, (width - _charts.IconWidth) / 2, (height - _charts.IconHeight) / 2, false, backColor);
            }
            g.ReleaseHdc();
            pictureBox1.Image = bmp;
        }

        /// <summary>
        /// Copies settings from the charts class to the controls
        /// </summary>
        private void Settings2GUI()
        {
            if (_charts == null)
                return;

            _noEvents = true;
            udBarHeight.SetValue(_charts.BarHeight);
            udBarWidth.SetValue(_charts.BarWidth);
            
            cboChartVerticalPosition.SelectedIndex = (int)_charts.VerticalPosition;
            udChartsOffsetX.SetValue(_charts.OffsetX);
            udChartsOffsetY.SetValue(_charts.OffsetY);
            udChartsBuffer.SetValue(_charts.CollisionBuffer);

            udPieRadius.SetValue(_charts.PieRadius);
            udPieRadius2.SetValue(_charts.PieRadius2);
            udThickness.SetValue(_charts.Thickness);
            udTilt.SetValue(_charts.Tilt);
            chk3DMode.Checked = _charts.Use3DMode;
            chkVisible.Checked = _charts.Visible;

            chkFontBold.Checked = _charts.ValuesFontBold;
            clpFont.Color = Colors.UintToColor(_charts.ValuesFontColor);
            chkFontItalic.Checked = _charts.ValuesFontItalic;
            udFontSize.SetValue(_charts.ValuesFontSize);

            clpFrame.Color = Colors.UintToColor(_charts.ValuesFrameColor);
            chkValuesFrame.Checked = _charts.ValuesFrameVisible;
            chkValuesVisible.Checked = _charts.ValuesVisible;
            cboValuesStyle.SelectedIndex = (int)_charts.ValuesStyle;

            // looking for the font
            string name = _charts.ValuesFontName.ToLower();
            for (int i = 0; i < cboFontName.Items.Count; i++)
            {
                if (cboFontName.Items[i].ToString().ToLower() == name)
                {
                    cboFontName.SelectedIndex = i;
                }
            }
            if (cboFontName.SelectedIndex < 0)
                cboFontName.Text = "Arial";
               
            // transparency
            transparencyControl1.Value = (byte)_charts.Transparency;

            cboMinScale.Text = _shapefile.Charts.MinVisibleScale.ToString();
            cboMaxScale.Text = _shapefile.Charts.MaxVisibleScale.ToString();
            chkDynamicVisibility.Checked = _shapefile.Charts.DynamicVisibility;

            _noEvents = false;
       }
        
        /// <summary>
        /// Applies options, generates charts if needed
        /// </summary>
        bool ApplyOptions()
        {
            if (_charts.NumFields == 0 )
            {
                if (_charts.Count == 0)
                {
                    //MessageBox.Show("No fields were chosen. No charts will be displayed.", _plugin.Globals.AppName, MessageBoxButtons.OK, MessageBoxIcon.Information);
                    //return false;
                }
                else
                {
                    if (MessageBox.Show("No fields were chosen. Do you want to remove all charts?",
                                    LegendControl.Legend.AppName, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                    {
                        _charts.Clear();
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else if (_shapefile.Charts.Count == 0)
            {
                // there is no charts, start generation
                ShpfileType type = Globals.ShapefileType2D(_shapefile.ShapefileType);
                if (type == ShpfileType.SHP_POINT || type == ShpfileType.SHP_MULTIPOINT)
                {
                    // start generation, no need to prompt the user for position
                    ICallback cBackOld = _shapefile.GlobalCallback;
                    Callback cback = new Callback();
                    _shapefile.GlobalCallback = cback;
                    this.Enabled = false;
                    this.Cursor = Cursors.WaitCursor;
                    try
                    {
                        _shapefile.Charts.Generate(tkLabelPositioning.lpCentroid);
                    }
                    finally
                    {
                        this.Enabled = true;
                        this.Cursor = Cursors.Default;
                        cback.Clear();
                        _shapefile.GlobalCallback = cBackOld;
                    }
                }
                else
                {
                    // prompting user for charts position
                    AddChartsForm form = new AddChartsForm(_shapefile);
                    if (form.ShowDialog() != DialogResult.OK)
                    {
                        return false;
                    }
                    form.Dispose();
                }
            }
            return true;
        }

        /// <summary>
        /// Saves the settings
        /// </summary>
        private void btnOk_Click(object sender, EventArgs e)
        {
            if (!ApplyOptions())
                return;
            
           //_plugin.m_mapWin.View.ForceFullRedraw();
           _selectedTab = tabControl1.SelectedIndex;

           if (_charts.Serialize() != _initState)
           {
               //_plugin.m_mapWin.Project.Modified = true;
           }

           // saves options for default loading behavior
           Globals.SaveLayerOptions(_handle);

           this.DialogResult = DialogResult.OK;
       }

       private void btnRefresh_Click(object sender, EventArgs e)
       {
           Draw();
       }

        /// <summary>
        /// Updating colors of the charts
        /// </summary>
        private void icbColors_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (_noEvents)
                return;

            btnApply.Enabled = true;
            UpdateFieldColors();

            // updating preview
            Draw();
        }

        private void UpdateFieldColors()
        {
            List<ColorBlend> schemes = icbColors.ColorSchemes.List;
            if (schemes != null && icbColors.SelectedIndex >= 0)
            {
                ColorBlend blend = (ColorBlend)schemes[icbColors.SelectedIndex];
                MapWinGIS.ColorScheme scheme = ColorSchemes.ColorBlend2ColorScheme(blend);
                if (scheme != null)
                {
                    for (int i = 0; i < _charts.NumFields; i++)
                    {
                        MapWinGIS.ChartField field = _charts.get_Field(i);
                        double value = (double)(i) / (double)(_charts.NumFields - 1);
                        field.Color = scheme.get_GraduatedColor(value);
                    }
                }
                
            }
        }
        
        /// <summary>
        /// Updates the enabled state of the controls
        /// </summary>
        private void RefreshControlsState()
        {
            cboValuesStyle.Enabled = chkValuesVisible.Checked && (_charts.ChartType == MapWinGIS.tkChartType.chtBarChart);
            groupBox4.Enabled = chkValuesVisible.Checked;
            groupBox5.Enabled = chkValuesVisible.Checked;

            udThickness.Enabled = chk3DMode.Checked;
            udTilt.Enabled = chk3DMode.Checked;

            cboChartSizeField.Enabled = _shapefile.Charts.ChartType == tkChartType.chtPieChart;

            cboMinScale.Enabled = chkDynamicVisibility.Checked;
            cboMaxScale.Enabled = chkDynamicVisibility.Checked;

            groupBox4.Enabled = _charts.NumFields > 0;
            groupBox5.Enabled = _charts.NumFields > 0;
            groupBox6.Enabled = _charts.NumFields > 0;
            groupBox7.Enabled = _charts.NumFields > 0;
            groupBox10.Enabled = _charts.NumFields > 0;
            chkValuesVisible.Enabled = _charts.NumFields > 0;
            cboValuesStyle.Enabled = _charts.NumFields > 0;
            label10.Enabled = _charts.NumFields > 0;
            icbColors.Enabled = _charts.NumFields > 0;
            label1.Enabled = _charts.NumFields > 0;;
            groupboxChartsOffset.Enabled = _charts.NumFields > 0;
            
            groupBox9.Enabled = _charts.NumFields > 0;
            groupBox11.Enabled = _charts.NumFields > 0;
            groupBox2.Enabled = _charts.NumFields > 0;
            groupBox3.Enabled = _charts.NumFields > 0;
            groupBox8.Enabled = _charts.NumFields > 0;
            groupBox13.Enabled = _charts.NumFields > 0;

            btnChangeScheme.Enabled = _charts.NumFields > 0; ;
        }

                #region "Fields"
        /// <summary>
        /// Adds selected field to the chart
        /// </summary>
        private void btnAdd_Click(object sender, EventArgs e)
        {
            if (listLeft.SelectedIndex >= 0)
            {
                int index = listLeft.SelectedIndex;
                listRight.Items.Add(listLeft.SelectedItem);
                listLeft.Items.Remove(listLeft.SelectedItem);

                if (index < listLeft.Items.Count)
                    listLeft.SelectedIndex = index;
                else if (index - 1 >= 0)
                    listLeft.SelectedIndex = index - 1;

                listRight.SelectedIndex = listRight.Items.Count - 1;

                if (!_noEvents)
                    btnApply.Enabled = true;
            }
            RefreshFields();
            RefreshControlsState();
            Draw();
        }

        /// <summary>
        /// Updates fields chosen by user
        /// </summary>
        private void RefreshFields()
        {
            _shapefile.Charts.ClearFields();

            // adding selected fields
            for (int i = 0; i < listRight.Items.Count; i++)
            {
                for (int j = 0; j < _shapefile.NumFields; j++)
                {
                    if (listRight.Items[i].ToString().ToLower() == _shapefile.get_Field(j).Name.ToLower())
                    {
                        double val = (double)i / (double)(listRight.Items.Count - 1);

                        ChartField field = new ChartField();
                        //field.Color = _colorScheme.get_GraduatedColor(val);
                        field.Index = j;
                        field.Name = _shapefile.get_Field(j).Name;
                        _shapefile.Charts.AddField(field);
                    }
                }
            }

            UpdateFieldColors();
        }

        /// <summary>
        /// Removes selected field form the chart
        /// </summary>
        private void btnDelete_Click(object sender, EventArgs e)
        {
            if (listRight.SelectedIndex >= 0)
            {
                int index = listRight.SelectedIndex;
                listLeft.Items.Add(listRight.SelectedItem);
                listRight.Items.Remove(listRight.SelectedItem);

                if (index < listRight.Items.Count)
                    listRight.SelectedIndex = index;
                else if (index - 1 >= 0)
                    listRight.SelectedIndex = index - 1;

                listLeft.SelectedIndex = listLeft.Items.Count - 1;

                if (!_noEvents)
                    btnApply.Enabled = true;
            }
            RefreshFields();
            RefreshControlsState();
            Draw();
        }

        /// <summary>
        /// Adds all the fields to the chart
        /// </summary>
        private void btnAddAll_Click(object sender, EventArgs e)
        {
            if (!_noEvents && listLeft.Items.Count > 0)
                btnApply.Enabled = true;

            for (int i = 0; i < listLeft.Items.Count; i++)
            {
                listRight.Items.Add(listLeft.Items[i]);
            }
            listLeft.Items.Clear();

            if (listRight.Items.Count > 0)
            {
                listRight.SelectedIndex = listRight.Items.Count - 1;
            }
            RefreshFields();
            RefreshControlsState();
            Draw();
        }

        /// <summary>
        ///  Removes all the fields from the chart
        /// </summary>
        private void btnDeleteAll_Click(object sender, EventArgs e)
        {
            if (!_noEvents && listRight.Items.Count > 0)
                btnApply.Enabled = true;

            for (int i = 0; i < listRight.Items.Count; i++)
            {
                listLeft.Items.Add(listRight.Items[i]);
            }
            listRight.Items.Clear();
            
            if (listLeft.Items.Count > 0)
            {
                listLeft.SelectedIndex = listLeft.Items.Count - 1;
            }
            
            RefreshFields();
            RefreshControlsState();
            Draw();
        }

        #endregion

        /// <summary>
        /// Building chart expression
        /// </summary>
        private void btnChartExpression_Click(object sender, EventArgs e)
        {
            string s = txtChartExpression.Text;
            frmQueryBuilder form = new frmQueryBuilder(_shapefile, _handle, s, false);
            if (form.ShowDialog() == DialogResult.OK)
            {
                txtChartExpression.Text = form.Tag.ToString();
                _shapefile.Charts.VisibilityExpression = txtChartExpression.Text;
                btnApply.Enabled = true;
            }
            form.Dispose();
        }

        /// <summary>
        /// Clears the charts expression
        /// </summary>
        private void btnClearChartsExpression_Click(object sender, EventArgs e)
        {
            txtChartExpression.Clear();
            _shapefile.Charts.VisibilityExpression = "";
            btnApply.Enabled = true;
        }

        /// <summary>
        /// Applies options and redraws map without closing the form
        /// </summary>
        private void btnApply_Click(object sender, EventArgs e)
        {
            if (ApplyOptions())
            {
                //_plugin.m_mapWin.View.ForceFullRedraw();
                //_plugin.m_mapWin.Project.Modified = true;
                _initState = _charts.Serialize();
                btnApply.Enabled = false;
            }
        }
        
        /// <summary>
        /// Applies trasnparency set by user
        /// </summary>
        private void transparencyControl1_ValueChanged(object sender, byte value)
        {
            GUI2Settings(null, null);
        }
        
        /// <summary>
        /// Reverts changes if cancel was chosen
        /// </summary>
        private void frmChartStyle_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (this.DialogResult == DialogResult.Cancel)
            {
                MapWinGIS.tkSavingMode mode = _charts.SavingMode;
                _charts.SavingMode = MapWinGIS.tkSavingMode.modeNone;
                _charts.Deserialize(_initState);
                _charts.SavingMode = mode;
            }
        }

        /// <summary>
        /// Opens window to edit the list of color schemes for charts
        /// </summary>
        private void btnChangeScheme_Click(object sender, EventArgs e)
        {
            frmColorSchemes form = new frmColorSchemes(ref Globals.ChartColors);
            if (form.ShowDialog(this) == DialogResult.OK)
            {
                _noEvents = true;
                this.icbColors.ColorSchemes = Globals.ChartColors;
                _noEvents = false;
            }
            form.Dispose();
        }

        /// <summary>
        /// Sets max visible scale to current scale
        /// </summary>
        private void btnSetMaxScale_Click(object sender, EventArgs e)
        {
            MapWinGIS.Map map = m_legend.Map;
            if (map != null)
            {
                cboMaxScale.Text = map.CurrentScale.ToString();
                btnApply.Enabled = true;
            }
        }

        /// <summary>
        /// Sets min visible scale to current scale
        /// </summary>
        private void btnSetMinScale_Click(object sender, EventArgs e)
        {
            MapWinGIS.Map map = m_legend.Map;
            if (map != null)
            {
                cboMinScale.Text = map.CurrentScale.ToString();
                btnApply.Enabled = true;
            }
        }
    }
}
