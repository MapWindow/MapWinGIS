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

using MWLite.Symbology.Controls;
using MWLite.Symbology.Forms.Utilities;
using MWLite.Symbology.LegendControl;
using MWLite.Symbology.Classes;

namespace MWLite.Symbology.Forms
{
    using System;
    using System.Collections.Generic;
    using System.Windows.Forms;
    using MapWinGIS;
    using MWLite.Symbology.Classes;
    using System.Drawing.Drawing2D;
    using System.Drawing;
    using MWLite.Symbology;

    partial class frmSymbologyMain
    {
        #region Categories Tab

        // column indices of the categories grid
        private const int CMN_CATEGORYID = 0;
        private const int CMN_VISIBLE = 1;
        private const int CMN_STYLE = 2;
        private const int CMN_NAME = 3;
        private const int CMN_EXPRESSION = 4;
        private const int CMN_COUNT = 5;

        /// <summary>
        /// Initializes the categories tab
        /// </summary>
        private void InitCategoriesTab()
        {
            icbCategories.ComboStyle = ImageComboStyle.ColorSchemeGraduated;
            icbCategories.ColorSchemes = Globals.LayerColors;
            if (icbCategories.Items.Count > 0)
            {
                icbCategories.SelectedIndex = 0;
            }

            // layer settings

            chkSetGradient.Checked = _settings.CategoriesUseGradient;
            chkRandomColors.Checked = _settings.CategoriesRandomColors;
            udNumCategories.Value = _settings.CategoriesCount;
            chkUniqueValues.Checked = (_settings.CategoriesClassification == tkClassificationType.ctUniqueValues);
            chkUseVariableSize.Checked = _settings.CategoriesVariableSize;

            // fills in the list of fields
            FillFieldList(_settings.CategoriesFieldName);

            // setting the color scheme that is in use
            for (int i = 0; i < icbCategories.Items.Count; i++)
            {
                //if (m_plugin.LayerColors.List[i] == _settings.CategoriesColorScheme)
                //{
                //    icbCategories.SelectedIndex = i;
                //    break;
                //}
            }

            MapWinGIS.ShpfileType type = Globals.ShapefileType2D(_shapefile.ShapefileType);
            groupVariableSize.Visible = (type == ShpfileType.SHP_POINT || type == ShpfileType.SHP_POLYLINE);

            if (type == ShpfileType.SHP_POINT)
            {
                udMinSize.SetValue(_shapefile.DefaultDrawingOptions.PointSize);
            }
            else if (type == ShpfileType.SHP_POLYLINE)
            {
                udMinSize.SetValue(_shapefile.DefaultDrawingOptions.LineWidth);
            }
            udMaxSize.SetValue((double)udMinSize.Value + _settings.CategoriesSizeRange);

            RefreshCategoriesList();

            if (dgvCategories.Rows.Count > 0 && dgvCategories.Columns.Count > 0)
            {
                dgvCategories[0, 0].Selected = true;
            }
        }

        /// <summary>
        /// Generates shapefile categories
        /// </summary>
        private void btnCategoryGenerate_Click(object sender, EventArgs e)
        {
            int count = Convert.ToInt32(udNumCategories.Value);
            MapWinGIS.ShapefileCategories categories = _shapefile.Categories;

            if (lstFields1.SelectedItem == null) return;
            string name = lstFields1.SelectedItem.ToString().ToLower().Trim();

            int index = -1;
            for (int i = 0; i < _shapefile.NumFields; i++)
            {
                if (_shapefile.get_Field(i).Name.ToLower() == name)
                {
                    index = i;
                    break;
                }
            }

            if (index == -1)
                return;

            MapWinGIS.tkClassificationType classification = chkUniqueValues.Checked ? tkClassificationType.ctUniqueValues : tkClassificationType.ctNaturalBreaks;

            // preventing the large number of categories
            bool showWaiting = false;
            if (classification == tkClassificationType.ctUniqueValues)
            {
                HashSet<object> set = new HashSet<object>();
                for (int i = 0; i < _shapefile.NumShapes; i++)
                {
                    object val = _shapefile.get_CellValue(index, i);
                    set.Add(val);
                }

                if (set.Count > 300)
                {
                    showWaiting = true;
                    string s = string.Format("The chosen field = {1}.\nThe number of unique values = {0}.\n" +
                                             "Large number of categories negatively affects performance.\nDo you want to continue?", set.Count, "[" + name.ToUpper() + "]");
                    if (MessageBox.Show(s, "MapWindow 4", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No)
                    {
                        return;
                    }
                }
                set.Clear();
            }

            if (showWaiting)
            {
                this.Enabled = false;
                this.Cursor = Cursors.WaitCursor;
            }
            else
            {
                btnCategoryGenerate.Enabled = false;
            }

            // generating
            categories.Generate(index, classification, count);
            categories.Caption = "Categories: " + _shapefile.get_Field(index).Name;
            ApplyColorScheme2Categories();

            if (chkUseVariableSize.Checked)
            {
                ApplyVariablePointSize();
            }

            _shapefile.Categories.ApplyExpressions();

            // updating labels
            //LabelUtilities.GenerateCategories(m_mapWin, _layerHandle);

            RefreshCategoriesList();
            RedrawMap();

            // saving the settings

            _settings.CategoriesClassification = classification;
            _settings.CategoriesFieldName = name;
            _settings.CategoriesSizeRange = (int)(udMaxSize.Value - udMinSize.Value);
            _settings.CategoriesCount = (int)udNumCategories.Value;
            _settings.CategoriesRandomColors = chkRandomColors.Checked;
            _settings.CategoriesUseGradient = chkSetGradient.Checked;
            _settings.CategoriesVariableSize = chkUseVariableSize.Checked;

            // cleaning
            if (showWaiting)
            {
                this.Enabled = true;
                this.Cursor = Cursors.Default;
            }
            else
            {
                btnCategoryGenerate.Enabled = true;
            }

            RefreshControlsState(null, null);
            MarkStateChanged();
        }

        /// <summary>
        /// Sets the changed flag to the layer state
        /// </summary>
        private void MarkStateChanged()
        {
            _stateChanged = true;
            btnSaveChanges.Enabled = true;
        }

        /// <summary>
        /// Toggles between unique values and natural breaks. Natural break are available for numeric fields only.
        /// </summary>
        private void chkUniqueValues_CheckedChanged(object sender, EventArgs e)
        {
            FillFieldList(string.Empty);
        }

        /// <summary>
        /// Sets symbols with variable size for point categories 
        /// </summary>
        private void ApplyVariablePointSize()
        {
            if (chkUseVariableSize.Checked && (udMinSize.Value != udMaxSize.Value))
            {
                MapWinGIS.ShapefileCategories categories = _shapefile.Categories;
                if (_shapefile.ShapefileType == ShpfileType.SHP_POINT || _shapefile.ShapefileType == ShpfileType.SHP_MULTIPOINT)
                {
                    double step = (double)(udMaxSize.Value - udMinSize.Value) / ((double)categories.Count - 1);
                    for (int i = 0; i < categories.Count; i++)
                    {
                        categories.get_Item(i).DrawingOptions.PointSize = (int)udMinSize.Value + Convert.ToInt32(i * step);
                    }
                }
                else if (_shapefile.ShapefileType == ShpfileType.SHP_POLYLINE)
                {
                    double step = (double)(udMaxSize.Value + udMinSize.Value) / (double)categories.Count;
                    for (int i = 0; i < categories.Count; i++)
                    {
                        categories.get_Item(i).DrawingOptions.LineWidth = (int)udMinSize.Value + Convert.ToInt32(i * step);
                    }
                }
            }
        }

        /// <summary>
        /// Toggles between random and graduated colors schemes
        /// </summary>
        private void chkRandomColors_CheckedChanged(object sender, EventArgs e)
        {
            int index = icbCategories.SelectedIndex;
            if (chkRandomColors.Checked)
            {
                icbCategories.ComboStyle = ImageComboStyle.ColorSchemeRandom;
            }
            else
            {
                icbCategories.ComboStyle = ImageComboStyle.ColorSchemeGraduated;
            }

            if (index >= 0 && index < icbCategories.Items.Count)
            {
                icbCategories.SelectedIndex = index;
            }
        }

        /// <summary>
        /// Applies color scheme chosen in the image combo to actegories
        /// </summary>
        private void ApplyColorScheme2Categories()
        {
            if (_shapefile.Categories.Count > 0)
            {
                MapWinGIS.ColorScheme scheme = null;
                if (icbCategories.SelectedIndex >= 0)
                {
                    ColorBlend blend = (ColorBlend)icbCategories.ColorSchemes.List[icbCategories.SelectedIndex];
                    scheme = ColorSchemes.ColorBlend2ColorScheme(blend);

                    // saving the settings
                    //MWLite.Symbology.Layer layer = Globals.Legend.Layers.ItemByHandle(_layerHandle);
                    //_settings.CategoriesColorScheme = blend;
                }
                else
                    return;

                if (chkRandomColors.Checked)
                {
                    _shapefile.Categories.ApplyColorScheme(MapWinGIS.tkColorSchemeType.ctSchemeRandom, scheme);
                }
                else
                {
                    _shapefile.Categories.ApplyColorScheme(MapWinGIS.tkColorSchemeType.ctSchemeGraduated, scheme);
                }

                MapWinGIS.ShapefileCategories categories = _shapefile.Categories;
                if (chkSetGradient.Checked)
                {
                    for (int i = 0; i < categories.Count; i++)
                    {
                        ShapeDrawingOptions options = categories.get_Item(i).DrawingOptions;
                        options.SetGradientFill(options.FillColor, 75);
                        options.FillType = tkFillType.ftGradient;
                    }
                }
                else
                {
                    for (int i = 0; i < categories.Count; i++)
                    {
                        ShapeDrawingOptions options = categories.get_Item(i).DrawingOptions;
                        options.FillColor2 = options.FillColor;
                        options.FillType = tkFillType.ftStandard;
                    }
                }
            }
        }

        /// <summary>
        /// Removes selected category
        /// </summary>
        private void btnCategoryRemove_Click(object sender, EventArgs e)
        {
            if (dgvCategories.CurrentRow != null)
            {
                try
                {
                    int cmn = dgvCategories.CurrentCell.ColumnIndex;
                    int index = dgvCategories.CurrentRow.Index;

                    int realIndex = (int)dgvCategories[CMN_CATEGORYID, dgvCategories.CurrentRow.Index].Value;
                    _shapefile.Categories.Remove(realIndex);
                    RefreshCategoriesList();

                    if (index >= 0 && index < dgvCategories.Rows.Count)
                    {
                        dgvCategories.CurrentCell = dgvCategories[cmn, index];
                    }
                    else if (dgvCategories.Rows.Count > 0)
                    {
                        dgvCategories.CurrentCell = dgvCategories[cmn, dgvCategories.Rows.Count];
                    }

                    // updating the map
                    _shapefile.Categories.ApplyExpressions();

                    RedrawMap();
                }
                catch (System.Exception)
                {
                }
            }
        }

        /// <summary>
        /// Shows form to chnage visualization of the given category
        /// </summary>
        private void btnCategoryAppearance_Click(object sender, EventArgs e)
        {
            if (dgvCategories.CurrentRow != null)
            {
                ChangeCategoryStyle(dgvCategories.CurrentRow.Index);
            }
        }

        /// <summary>
        /// Removes all the categories in the list
        /// </summary>
        private void btnCategoryClear_Click(object sender, EventArgs e)
        {
            _shapefile.Categories.Clear();
            RefreshCategoriesList();

            Layer layer = _legend.Layers.ItemByHandle(_layerHandle);
            _settings.CategoriesClassification = chkUniqueValues.Checked ? tkClassificationType.ctUniqueValues : tkClassificationType.ctNaturalBreaks;

            RedrawMap();
        }

        /// <summary>
        /// Fills the data grid view with information about label categories
        /// </summary>
        private void RefreshCategoriesList()
        {
            dgvCategories.SuspendLayout();
            dgvCategories.Rows.Clear();

            int numCategories = _shapefile.Categories.Count;
            if (numCategories == 0)
            {
                dgvCategories.ColumnHeadersVisible = false;
                dgvCategories.ResumeLayout();
                RefreshControlsState(null, null);
                return;
            }
            else
            {
                dgvCategories.ColumnHeadersVisible = true;
            }

            dgvCategories.Rows.Add(numCategories);

            bool noEventsState = _noEvents;
            _noEvents = true;

            // calculating the number of shapes per category
            Dictionary<int, int> values = new Dictionary<int, int>();  // id of category, count
            int category;

            for (int i = 0; i < _shapefile.NumShapes; i++)
            {
                category = _shapefile.get_ShapeCategory(i);
                if (values.ContainsKey(category))
                {
                    values[category] += 1;
                }
                else
                {
                    values.Add(category, 1);
                }
            }

            for (int i = 0; i < numCategories; i++)
            {
                MapWinGIS.ShapefileCategory cat = _shapefile.Categories.get_Item(i);
                dgvCategories[CMN_CATEGORYID, i].Value = i;
                dgvCategories[CMN_VISIBLE, i].Value = cat.DrawingOptions.Visible;
                dgvCategories[CMN_STYLE, i].Value = new Bitmap(dgvCategories.Columns[CMN_STYLE].Width - 20, dgvCategories.Rows[i].Height - 8);
                dgvCategories[CMN_NAME, i].Value = cat.Name;
                dgvCategories[CMN_EXPRESSION, i].Value = cat.Expression;

                if (values.ContainsKey(i))
                    dgvCategories[CMN_COUNT, i].Value = values[i];
                else
                    dgvCategories[CMN_COUNT, i].Value = 0;
            }
            dgvCategories.ResumeLayout();

            // autosizing columns
            for (int i = 1; i < dgvCategories.Columns.Count; i++)
            {
                if (i != CMN_STYLE && i != CMN_COUNT)
                {
                    dgvCategories.AutoResizeColumn(i, DataGridViewAutoSizeColumnMode.AllCells);
                    dgvCategories.Columns[i].Width += 10;
                }
            }
            RefreshControlsState(null, null);
            _noEvents = noEventsState;
        }

        /// <summary>
        /// Changes the style of the selected category
        /// </summary>
        private void ChangeCategoryStyle(int row)
        {
            ShapefileCategory cat = _shapefile.Categories.get_Item(row);
            if (cat == null) return;

            Form form = FormHelper.GetSymbologyForm(_legend, _layerHandle, _shapefile.ShapefileType, cat.DrawingOptions, true);
            form.Text = "Category drawing options";

            if (form.ShowDialog(this) == DialogResult.OK)
            {
                dgvCategories.Invalidate();
                RedrawMap();
            }
            form.Dispose();
        }

        /// <summary>
        /// Opening forms for editing the category
        /// </summary>
        private void dgvCategories_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex == CMN_STYLE)
            {
                btnCategoryAppearance_Click(null, null);
            }
        }

        /// <summary>
        /// Drawing of images in the style column
        /// </summary>
        private void dgvCategories_CellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
        {
            if (e.ColumnIndex != CMN_STYLE) return;
            if (e.RowIndex >= 0 && e.RowIndex < _shapefile.Categories.Count)
            {
                System.Drawing.Image img = e.Value as System.Drawing.Image;
                if (img == null) return;

                ShapefileCategory cat = _shapefile.Categories.get_Item(e.RowIndex);
                if (cat == null) return;
                ShapeDrawingOptions sdo = cat.DrawingOptions;

                Graphics g = Graphics.FromImage(img);
                g.Clear(Color.White);
                g.InterpolationMode = InterpolationMode.HighQualityBicubic;
                g.SmoothingMode = SmoothingMode.HighQuality;

                if (_shapefile.ShapefileType == ShpfileType.SHP_POLYGON)
                {
                    sdo.DrawRectangle(g.GetHdc().ToInt32(), 0, 0, img.Width - 1, img.Height - 1, true, img.Width, img.Height, Colors.ColorToUInteger(dgvCategories.BackgroundColor));
                }
                else if (_shapefile.ShapefileType == ShpfileType.SHP_POLYLINE)
                {
                    sdo.DrawLine(g.GetHdc().ToInt32(), 0, 0, img.Width - 1, img.Height - 1, true, img.Width, img.Height, Colors.ColorToUInteger(dgvCategories.BackgroundColor));
                }
                else if (_shapefile.ShapefileType == ShpfileType.SHP_POINT)
                {
                    sdo.DrawPoint(g.GetHdc().ToInt32(), 0.0f, 0.0f, img.Width, img.Height, Colors.ColorToUInteger(dgvCategories.BackgroundColor));
                }

                g.ReleaseHdc();
                g.Dispose();
            }
        }

        /// <summary>
        /// Drawing the focus rectangle
        /// </summary>
        private void dgvCategories_CellPainting(object sender, DataGridViewCellPaintingEventArgs e)
        {
            if (this.dgvCategories.CurrentCell == null) return;
            if (e.ColumnIndex == this.dgvCategories.CurrentCell.ColumnIndex && e.RowIndex == this.dgvCategories.CurrentCell.RowIndex)
            {
                e.Paint(e.CellBounds, DataGridViewPaintParts.All);

                using (Pen p = new Pen(Color.Black, 4))
                {
                    Rectangle rect = e.CellBounds;
                    rect.Width -= 1;
                    rect.Height -= 1;
                    ControlPaint.DrawFocusRectangle(e.Graphics, rect);
                }
                e.Handled = true;
            }
        }

        /// <summary>
        /// Toggles visibility of the categories
        /// </summary>
        private void dgvCategories_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex == -1 || e.ColumnIndex == -1) return;

            if (_noEvents)
                return;

            if (e.ColumnIndex == CMN_VISIBLE)
            {
                int index = (int)dgvCategories[CMN_CATEGORYID, e.RowIndex].Value;
                _shapefile.Categories.get_Item(index).DrawingOptions.Visible = (bool)dgvCategories[e.ColumnIndex, e.RowIndex].Value;

                // toggle labels in case they are present
                MapWinGIS.LabelCategory cat = _shapefile.Labels.get_Category(index);
                if (cat != null && cat.Enabled)
                {
                    cat.Visible = (bool)dgvCategories[e.ColumnIndex, e.RowIndex].Value;
                }
                RedrawMap();
            }
        }

        /// <summary>
        /// Committing changes of the checkbox state immediately, CellValueChanged event won't be triggered otherwise
        /// </summary>
        private void dgvCategories_CurrentCellDirtyStateChanged(object sender, EventArgs e)
        {
            if (dgvCategories.CurrentCell.ColumnIndex == CMN_VISIBLE)
            {
                if (dgvCategories.IsCurrentCellDirty)
                {
                    dgvCategories.CommitEdit(DataGridViewDataErrorContexts.Commit);
                }
            }
        }

        /// <summary>
        /// Bans editing of the count column
        /// </summary>
        private void dgvCategories_CellBeginEdit(object sender, DataGridViewCellCancelEventArgs e)
        {
            if (e.ColumnIndex == CMN_COUNT)
                e.Cancel = true;
        }

        /// <summary>
        /// Saves editing of the category names
        /// </summary>
        private void dgvCategories_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex < 0)
                return;

            if (e.ColumnIndex == CMN_NAME)
            {
                _shapefile.Categories.get_Item(e.RowIndex).Name = dgvCategories[CMN_NAME, e.RowIndex].Value.ToString();
                RedrawLegend();
            }
        }

        /// <summary>
        /// Fills the list of fields
        /// </summary>
        private void FillFieldList(string name)
        {
            if (name == string.Empty)
            {
                // we need to preserve currently selected field
                if (lstFields1.SelectedItem != null)
                {
                    name = lstFields1.SelectedItem.ToString().Trim();
                }
                else
                {
                    name = string.Empty;
                }
            }
            // else  = we need some particular field as selected

            lstFields1.Items.Clear();

            // adding names
            for (int i = 0; i < _shapefile.NumFields; i++)
            {
                if (!chkUniqueValues.Checked && _shapefile.get_Field(i).Type == FieldType.STRING_FIELD ||
                     chkUniqueValues.Checked && _shapefile.get_Field(i).Type != FieldType.STRING_FIELD)
                {
                    continue;
                }

                lstFields1.Items.Add("  " + _shapefile.get_Field(i).Name);
            }

            // setting the selected field back
            if (name != string.Empty)
            {
                for (int i = 0; i < lstFields1.Items.Count; i++)
                {
                    if (lstFields1.Items[i].ToString().ToLower().Trim() == name.ToLower())
                    {
                        lstFields1.SelectedIndex = i;
                        break;
                    }
                }
            }

            if (lstFields1.SelectedItem == null && lstFields1.Items.Count > 0)
            {
                lstFields1.SelectedIndex = 0;
            }
            RefreshControlsState(null, null);
        }

        /// <summary>
        /// Modifies the list of available color schemes for the layer
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnChangeColorScheme_Click(object sender, EventArgs e)
        {
            frmColorSchemes form = new frmColorSchemes(ref Globals.LayerColors);
            if (form.ShowDialog(this) == DialogResult.OK)
            {
                icbCategories.ColorSchemes = Globals.LayerColors;
            }
            form.Dispose();
        }
        #endregion
    }
}
