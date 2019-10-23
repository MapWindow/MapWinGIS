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
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using MWLite.Symbology.Classes;
using MWLite.Symbology.Controls;
using MapWinGIS;
using MWLite.Symbology.LegendControl;
using MWLite.Symbology.Classes;

namespace MWLite.Symbology.Forms.Symbology
{
    public partial class LinesForm : Form
    {
        #region Member variables

        // reference to the legend
        internal LegendControl.Legend m_legend = null;

        // layer being edited
        internal Layer m_layer = null;
        
        // the options to edit
        ShapeDrawingOptions _options = null;
        
        // prevents the unwanted events
        private bool _noEvents = false;

        // the serialized version of the initial state
        private string _initState = "";

        // tab index to open on the next loading
        internal static int _tabIndex = 0;

        // column indices for the grid
        private const int CMN_PICTURE = 0;
        private const int CMN_TYPE = 1;

        #endregion

        #region Initialization
        /// <summary>
        /// Creates a new instance of PolygonsForm class
        /// </summary>
        public LinesForm(LegendControl.Legend legend, Layer layer, ShapeDrawingOptions options, bool applyDisabled)
        {
            InitializeComponent();

            if (options == null || layer == null)
            {
                throw new Exception("PolygonsForm: unexpected null parameter");
            }
            else
            {
                _options = options;
                m_layer = layer;
            }

            m_legend = legend;
            btnApply.Visible = !applyDisabled;
            _initState = options.Serialize();
            

            icbLineType.ComboStyle = ImageComboStyle.LineStyle;
            icbLineWidth.ComboStyle = ImageComboStyle.LineWidth;

            groupMarker.Parent = tabLine;
            groupMarker.Top = groupLine.Top;
            groupMarker.Left = groupLine.Left;

            cboVerticesType.Items.Clear();
            cboVerticesType.Items.Add("Square");
            cboVerticesType.Items.Add("Circle");

            cboOrientation.Items.Clear();
            cboOrientation.Items.Add("Horizontal");
            cboOrientation.Items.Add("Parallel");
            cboOrientation.Items.Add("Perpendicular");

            cboLineType.Items.Clear();
            cboLineType.Items.Add("Line");
            cboLineType.Items.Add("Marker");

            cboLineType.SelectedIndexChanged += new EventHandler(cboLineType_SelectedIndexChanged);
            cboLineType.SelectedIndex = 0;

            // vertices
            chkVerticesVisible.CheckedChanged += new EventHandler(GUI2Options);
            cboVerticesType.SelectedIndexChanged += new EventHandler(GUI2Options);
            clpVerticesColor.SelectedColorChanged += new EventHandler(GUI2Options);
            chkVerticesFillVisible.CheckedChanged += new EventHandler(GUI2Options);
            udVerticesSize.ValueChanged += new EventHandler(GUI2Options);

            InitLinePattern();

            Options2Grid();

            Options2GUI();

            linePatternControl1.LoadFromXML();

            _noEvents = true;
            tabControl1.SelectedIndex = _tabIndex;
            _noEvents = false;
        }

        /// <summary>
        /// Take cate that a line pattern actually exists
        /// </summary>
        void InitLinePattern()
        {
            if (_options.LinePattern == null)
            {
                _options.LinePattern = new LinePattern();
            }
            if (_options.LinePattern.Count == 0)
            {
                _options.LinePattern.AddLine(_options.LineColor, _options.LineWidth, _options.LineStipple);
            }
        }

        /// <summary>
        /// Updates the enabled state of the controls
        /// </summary>
        void RefreshControlState()
        {
            bool exists = (dgv.SelectedRows.Count > 0) && (dgv.Rows.Count > 1);
            btnRemove.Enabled = exists;
            if (exists)
            {
                int index = dgv.SelectedRows[0].Index;
                btnMoveUp.Enabled = index > 0;
                btnMoveDown.Enabled = index < dgv.Rows.Count - 1;
            }
            else
            {
                btnMoveDown.Enabled = false;
                btnMoveUp.Enabled = false;
            }

        }
        #endregion

        #region PropertyExchange
        /// <summary>
        /// Sets the values entered by user to the class
        /// </summary>
        private void GUI2Options(object sender, EventArgs e)
        {
            if (_noEvents)
                return;

            // vertices
            _options.VerticesVisible = chkVerticesVisible.Checked;
            _options.VerticesFillVisible = chkVerticesFillVisible.Checked;
            _options.VerticesSize = (int)udVerticesSize.Value;
            _options.VerticesColor = Colors.ColorToUInteger(clpVerticesColor.Color);
            _options.VerticesType = (tkVertexType)cboVerticesType.SelectedIndex;

            // transparency
            _options.LineTransparency = transparencyControl1.Value;
            if (_options.LinePattern != null)
            {
                _options.LinePattern.Transparency = (byte)_options.LineTransparency;
            }

            if (dgv.SelectedRows.Count > 0)
            {
                int index = dgv.SelectedRows[0].Index;
                MapWinGIS.LineSegment line = _options.LinePattern.get_Line(index);
                if (line != null)
                {
                    line.LineType = cboLineType.SelectedIndex == 0 ? tkLineType.lltSimple : tkLineType.lltMarker;

                    // showing the options
                    if (cboLineType.SelectedIndex == 0)
                    {
                        line.LineStyle = (tkDashStyle)icbLineType.SelectedIndex;
                        line.LineWidth = icbLineWidth.SelectedIndex + 1;
                        line.Color = Colors.ColorToUInteger(clpOutline.Color);
                    }
                    else
                    {
                        line.Marker = (tkDefaultPointSymbol)pointSymbolControl1.SelectedIndex;
                        line.MarkerInterval = (float)udMarkerInterval.Value;
                        line.MarkerSize = (float)udMarkerSize.Value;
                        line.Color = Colors.ColorToUInteger(clpMarkerFill.Color);
                        line.MarkerOutlineColor = Colors.ColorToUInteger(clpMarkerOutline.Color);
                        line.MarkerOrientation = (tkLineLabelOrientation)cboOrientation.SelectedIndex;
                        line.MarkerOffset = (float)udMarkerOffset.Value;

                        if (pointSymbolControl1.ForeColor != clpMarkerFill.Color)
                        {
                            pointSymbolControl1.ForeColor = clpMarkerFill.Color;
                        }
                    }
                    dgv.Invalidate();
                }
            }

            btnApply.Enabled = true;
            DrawPreview();
        }
        
        /// <summary>
        /// Loads the values of the class instance to the controls
        /// </summary>
        private void Options2GUI()
        {
            _noEvents = true;

            // vertices
            chkVerticesVisible.Checked = _options.VerticesVisible;
            chkVerticesFillVisible.Checked = _options.VerticesFillVisible;
            udVerticesSize.SetValue(_options.VerticesSize);
            clpVerticesColor.Color = Colors.UintToColor(_options.VerticesColor);
            cboVerticesType.SelectedIndex = (int)_options.VerticesType;

            // transparency
            if (_options.LinePattern != null)
            {
                transparencyControl1.Value = _options.LinePattern.Transparency;
            }

            if (dgv.SelectedRows.Count > 0)
            {
                int index = dgv.SelectedRows[0].Index;
                MapWinGIS.LineSegment line = _options.LinePattern.get_Line(index);
                if (line != null)
                {
                    cboLineType.SelectedIndex = (int)line.LineType;

                    groupLine.Visible = false;
                    groupMarker.Visible = false;
                    if (cboLineType.SelectedIndex == 0)
                    {
                        groupLine.Visible = true;
                    }
                    else
                    {
                        groupMarker.Visible = true;
                    }

                    // showing the options
                    if (line.LineType == tkLineType.lltSimple)
                    {
                        _noEvents = true;
                        icbLineType.SelectedIndex = (int)line.LineStyle;
                        icbLineWidth.SelectedIndex = (int)line.LineWidth - 1;
                        clpOutline.Color = Colors.UintToColor(line.Color);
                        _noEvents = false;
                    }
                    else
                    {
                        _noEvents = true;
                        pointSymbolControl1.SelectedIndex = (int)line.Marker;
                        udMarkerInterval.SetValue(line.MarkerInterval);
                        udMarkerSize.SetValue(line.MarkerSize);
                        clpMarkerFill.Color = Colors.UintToColor(line.Color);
                        clpMarkerOutline.Color = Colors.UintToColor(line.MarkerOutlineColor);
                        udMarkerOffset.SetValue(line.MarkerOffset);
                        cboOrientation.SelectedIndex = (int)line.MarkerOrientation;
                        if (pointSymbolControl1.ForeColor != clpMarkerFill.Color)
                        {
                            pointSymbolControl1.ForeColor = clpMarkerFill.Color;
                        }

                        _noEvents = false;
                    }
                    DrawPreview();
                }
            }
            _noEvents = false;
        }
        #endregion

        #region Drawing
        /// <summary>
        /// Draws preview based on the chosen options
        /// </summary>
        private void DrawPreview()
        {
            if (_noEvents)
                return;

            if (pctPreview.Image != null)
            {
                pctPreview.Image.Dispose();
            }

            Rectangle rect = pctPreview.ClientRectangle;
            Bitmap bmp = new Bitmap(rect.Width, rect.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            Graphics g = Graphics.FromImage(bmp);
            IntPtr ptr = g.GetHdc();

            uint backColor = Colors.ColorToUInteger(this.BackColor);
            _options.LinePattern.Draw(ptr.ToInt32(), 20, 0, rect.Width - 40, rect.Height, backColor);

            g.ReleaseHdc();
            
            //bmp.MakeTransparent(Color.White);
            pctPreview.Image = bmp;
        }
        #endregion

        #region User input
        // Toggle between simaple and marker line
        void cboLineType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (_noEvents)
                return;

            if (dgv.SelectedRows.Count > 0)
            {
                int index = dgv.SelectedRows[0].Index;
                MapWinGIS.LineSegment line = _options.LinePattern.get_Line(index);
                line.LineType = (tkLineType)cboLineType.SelectedIndex;
                dgv[CMN_TYPE, index].Value = line.LineType == tkLineType.lltSimple ? "line" : "marker";
                Options2GUI();
                dgv.Invalidate();
                btnApply.Enabled = true;
                
            }
        }
        #endregion

        #region Grid
        
        /// <summary>
        /// Drawing of images in the style column
        /// </summary>
        private void dgv_CellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
        {
            if (_options.LinePattern == null)
            {
                return;
            }

            if (e.RowIndex >= 0 && e.RowIndex < _options.LinePattern.Count)
            {
                if (e.ColumnIndex == CMN_PICTURE)
                {
                    System.Drawing.Image img = e.Value as System.Drawing.Image;
                    if (img != null)
                    {
                        MapWinGIS.LineSegment line = _options.LinePattern.get_Line(e.RowIndex);
                        if (line != null)
                        {
                            Graphics g = Graphics.FromImage(img);
                            g.Clear(Color.White);
                            g.InterpolationMode = InterpolationMode.HighQualityBicubic;
                            g.SmoothingMode = SmoothingMode.HighQuality;
                            
                            IntPtr ptr = g.GetHdc();
                            uint backColor = Colors.ColorToUInteger(this.BackColor);
                            line.Draw(ptr.ToInt32(), 0, 0, img.Width, img.Height, backColor);
                            g.ReleaseHdc();
                            g.Dispose();

                            ((Bitmap)img).MakeTransparent(this.BackColor);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Shows the options of the curently selected line
        /// </summary>
        private void dgv_SelectionChanged(object sender, EventArgs e)
        {
            Options2GUI();
            RefreshControlState();
        }
        #endregion

        #region Managing lines
        /// <summary>
        /// Adds a line to the pattern
        /// </summary>
        private void btnAdd_Click(object sender, EventArgs e)
        {
            if (_options.LinePattern == null)
            {
                _options.LinePattern = new LinePattern();
            }

            if (cboLineType.SelectedIndex == (int)tkLineType.lltSimple)
            {
                _options.LinePattern.AddLine(Colors.ColorToUInteger(Color.Black), 1.0f, tkDashStyle.dsSolid);
            }
            else
            {
                LineSegment segment = _options.LinePattern.AddMarker(tkDefaultPointSymbol.dpsCircle);
            }
            Options2Grid();
            //GUI2Options(null, null);
            DrawPreview();

            // selecting the added line
            _noEvents = true;
            dgv.ClearSelection();
            _noEvents = false;
            int index = dgv.Rows.Count - 1;
            dgv.Rows[index].Selected = true;
            btnApply.Enabled = true;

            RefreshControlState();
        }
        /// <summary>
        /// Removes the current line. It's impossible to remove the last line
        /// </summary>
        private void btnRemove_Click(object sender, EventArgs e)
        {
            if (dgv.SelectedRows.Count > 0 && dgv.Rows.Count > 1)
            {
                int index = dgv.SelectedRows[0].Index;
                _options.LinePattern.RemoveItem(index);
                Options2Grid();
                //GUI2Options(null, null);
                DrawPreview();

                // restoring selection
                _noEvents = true;
                dgv.ClearSelection();
                _noEvents = false;
                if (index >= dgv.Rows.Count )
                    index--;
                dgv.Rows[index].Selected = true;

                btnApply.Enabled = true;
                RefreshControlState();
            }
        }

        /// <summary>
        /// Moves the selected line to the top of the pattern
        /// </summary>
        private void btnMoveUp_Click(object sender, EventArgs e)
        {
            if (dgv.SelectedRows.Count > 0)
            {
                int index = dgv.SelectedRows[0].Index;
                if (index > 0)
                {
                    MapWinGIS.LineSegment segm = _options.LinePattern.get_Line(index);
                    MapWinGIS.LineSegment segmBefore = _options.LinePattern.get_Line(index - 1);
                    _options.LinePattern.set_Line(index - 1, segm);
                    _options.LinePattern.set_Line(index, segmBefore);

                    Options2Grid();
                    DrawPreview();

                    _noEvents = true;
                    dgv.ClearSelection();
                    _noEvents = false;
                    dgv.Rows[index - 1].Selected = true;

                    btnApply.Enabled = true;
                    RefreshControlState();
                }
            }
        }

        /// <summary>
        /// Moves the selected line to the bottom of the pattern
        /// </summary>
        private void btnMoveDown_Click(object sender, EventArgs e)
        {
            if (dgv.SelectedRows.Count > 0)
            {
                int index = dgv.SelectedRows[0].Index;
                if (index < dgv.Rows.Count - 1)
                {
                    MapWinGIS.LineSegment segm = _options.LinePattern.get_Line(index);
                    MapWinGIS.LineSegment segmAfter = _options.LinePattern.get_Line(index + 1);
                    _options.LinePattern.set_Line(index + 1, segm);
                    _options.LinePattern.set_Line(index, segmAfter);

                    Options2Grid();
                    DrawPreview();

                    _noEvents = true;
                    dgv.ClearSelection();
                    _noEvents = false;
                    dgv.Rows[index + 1].Selected = true;

                    btnApply.Enabled = true;
                    RefreshControlState();
                }
            }
        }

        /// <summary>
        /// Chnages the marker for the current line
        /// </summary>
        private void pointSymbolControl1_SelectionChanged()
        {
            GUI2Options(null, null);
        }

        #endregion

        /// <summary>
        /// Preserving the selected index
        /// </summary>
        private void btnOk_Click(object sender, EventArgs e)
        {
            ApplyPattern();
            if (_initState != _options.Serialize())
            {
                m_legend.FireLayerPropertiesChanged(m_layer.Handle);
                m_legend.RedrawLegendAndMap();
            }
            _tabIndex = tabControl1.SelectedIndex;

            // saves options for default loading behavior
            //SymbologyPlugin.SaveLayerOptions(_layerHandle);

            linePatternControl1.SaveToXML();
        }

        private void transparencyControl1_ValueChanged(object sender, byte value)
        {
            GUI2Options(null, null);
        }

        /// <summary>
        /// Updates map and saves the changes without closing the window
        /// </summary>
        private void btnApply_Click(object sender, EventArgs e)
        {
            ApplyPattern();
            if (m_legend != null)
            {
                m_legend.RedrawLegendAndMap();
                m_legend.FireLayerPropertiesChanged(m_layer.Handle);
            }
            _initState = _options.Serialize();
            btnApply.Enabled = false;
        }

        /// <summary>
        /// Reverts the changes if cancel was selected
        /// </summary>
        private void frmLines_FormClosing(object sender, FormClosingEventArgs e)
        {
            _tabIndex = tabControl1.SelectedIndex;
            if (this.DialogResult == DialogResult.Cancel)
            {
                _options.Deserialize(_initState);
            }
        }
        #region Synchronization
        /// <summary>
        /// Returns true if the current line should be represented by the line pattern
        /// </summary>
        private bool CanUseLinePattern()
        {
            if (_options.LinePattern != null)
            {
                if (_options.LinePattern.Count > 0)
                {
                    return _options.LinePattern.Count > 1 || (_options.LinePattern.Count == 1 &&
                           _options.LinePattern.get_Line(0).LineType == tkLineType.lltMarker);
                }
            }
            return false;
        }

        /// <summary>
        /// Fills grid using ShapeDrawingOptions
        /// </summary>
        private void Options2Grid()
        {
            dgv.Rows.Clear();
            if (CanUseLinePattern())
            {
                dgv.Rows.Add(_options.LinePattern.Count);
                for (int i = 0; i < _options.LinePattern.Count; i++)
                {
                    dgv[CMN_TYPE, i].Value = _options.LinePattern.get_Line(i).LineType == tkLineType.lltSimple ? "line" : "marker";
                    Bitmap bmp = new Bitmap(60, 14);
                    dgv[CMN_PICTURE, i].Value = bmp;
                }
                _options.UseLinePattern = true;
            }
            else
            {
                // a single line
                dgv.Rows.Add(1);
                dgv[CMN_TYPE, 0].Value = "line";
                Bitmap bmp = new Bitmap(60, 14);
                dgv[CMN_PICTURE, 0].Value = bmp;
                _options.UseLinePattern = false;
            }
        }
        #endregion

        /// <summary>
        /// Chooses in which form to draw line, as common line or pattern
        /// </summary>
        void ApplyPattern()
        {
            if (_options.LinePattern == null)
            {
                // using line settings
                _options.UseLinePattern = false;
            }
            else if (_options.LinePattern.Count == 1 && _options.LinePattern.get_Line(0).LineType == tkLineType.lltSimple)
            {
                // the pattern can be represented as a single line
                // we need to copy the options only, as all settings were set to pattern
                ILineSegment line = _options.LinePattern.get_Line(0);
                if (line != null)
                {
                    _options.LineStipple = line.LineStyle;
                    _options.LineWidth = line.LineWidth;
                    _options.LineColor = line.Color;
                }
                _options.UseLinePattern = false;
            }
            else
            {
                // line pattern
                _options.UseLinePattern = true;
            }
        }

        #region Line pattern styles
        /// <summary>
        /// Handles the change of the style (style is displayed in preview)
        /// </summary>
        private void linePatternControl1_SelectionChanged()
        {
            if (_noEvents)
                return;

            MapWinGIS.LinePattern pattern = linePatternControl1.SelectedPattern;
            if (pattern != null)
            {
                string s = pattern.Serialize();

                _options.LinePattern = new LinePattern();
                _options.LinePattern.Deserialize(s);
                this.ApplyPattern();

                Options2Grid();
                Options2GUI();
                
                btnApply.Enabled = true;
            }
        }

        /// <summary>
        /// Adds current options as a style to the list
        /// </summary>
        private void btnAddStyle_Click(object sender, EventArgs e)
        {
            MapWinGIS.LinePattern pattern = new LinePattern();
            this.ApplyPattern();

            if (_options.UseLinePattern && _options.LinePattern != null)
            {
                string s = _options.LinePattern.Serialize();
                pattern.Deserialize(s);
            }
            else
            {
                // there is no actual patter, a single line only;
                // pattern object should be created on the fly
                pattern.AddLine(_options.LineColor, _options.LineWidth, _options.LineStipple);
            }
            linePatternControl1.AddPattern(pattern);
        }
        
        /// <summary>
        /// Removes selected style from the list
        /// </summary>
        private void btnRemoveStyle_Click(object sender, EventArgs e)
        {
            linePatternControl1.RemovePattern(linePatternControl1.SelectedIndex);
        }
        
        /// <summary>
        /// Tests ssaving. Temporary
        /// </summary>
        private void btnSaveStyles_Click(object sender, EventArgs e)
        {
            linePatternControl1.SaveToXML();
        }
        
        /// <summary>
        /// Tests loading. Temporary
        /// </summary>
        private void btnLoadStyles_Click(object sender, EventArgs e)
        {
            linePatternControl1.LoadFromXML();
        }
        #endregion
    }
}
