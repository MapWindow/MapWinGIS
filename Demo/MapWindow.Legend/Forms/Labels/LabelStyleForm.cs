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

using System;
using System.Drawing;
using System.Windows.Forms;
using MapWindow.Legend.Classes;
using MapWindow.Legend.Controls;
using MapWindow.Legend.Forms.Utilities;
using MapWinGIS;

namespace MapWindow.Legend.Forms.Labels
{
    /// <summary>
    /// GUI for setting options for Labels and LabelCategory classes
    /// </summary>
    public partial class LabelStyleForm : Form
    {
        private Controls.Legend.Legend m_legend = null;

        // The reference to the labels
        private MapWinGIS.LabelCategory m_category = null;

        // The tab number to open on the loading
        private static int tabNumber = 0;
        
        // prevents event handles to process events while on loading
        private bool m_NoEvents = false;
        
        // The label string to display
        private string m_labelText = "";

        // parent shapefile
        private MapWinGIS.Shapefile m_shapefile = null;
        
        // a category is being edited, no expression is available
        private bool m_categoryEdited = false;

        // serialized initial state of the class
        private string m_initState = "";

        // a handle of the layer being edited
        private int m_handle = -1;

        /// <summary>
        /// Constructor for setting label expression and options
        /// </summary>
        public LabelStyleForm(Controls.Legend.Legend legend, MapWinGIS.Shapefile sf, int handle)
        {
            if (sf == null)
            {
                throw new Exception("LabelStyleForm: unexpected null parameter");
            }
            m_legend = legend;
            m_shapefile = sf;
            m_handle = handle;

            InitializeComponent();
           
            //LabelStyle style = new LabelStyle( m_shapefile.Labels.Options);
            
            // old-style labels not based on expression
            if (m_shapefile.Labels.Expression == "" && m_shapefile.Labels.Count > 0 &&
                m_shapefile.Labels.get_Label(0, 0).Text != "")
            {
                richTextBox1.Text = "<no expression>";
                listBox1.Enabled = false;
                btnPlus.Enabled = false;
                btnQuotes.Enabled = false;
                btnNewLine.Enabled = false;
                richTextBox1.Enabled = false;
            }
            else
            {
                richTextBox1.Text = LabelUtilities.StripNewLineQuotes(m_shapefile.Labels.Expression);
            }

            this.Initialize(m_shapefile.Labels.Options);

            tabControl1.SelectedIndex = tabNumber;
        }

        /// <summary>
        /// Constructor for editing single category
        /// </summary>
        /// <param name="lb"></param>
        public LabelStyleForm(MapWinGIS.Shapefile sf, MapWinGIS.LabelCategory lb) 
        {
            m_categoryEdited = true;
            m_shapefile = sf;

            InitializeComponent();
            this.Initialize(lb);
           
            tabControl1.SelectedIndex = tabNumber;
            
            // expression isn't available for the categories
            if (m_categoryEdited)
            {
                tabControl1.TabPages.Remove(tabControl1.TabPages[4]);   // visibility
                tabControl1.TabPages.Remove(tabControl1.TabPages[3]);   // position
                tabControl1.TabPages.Remove(tabControl1.TabPages[0]);   // expression
            }
            lblResult.Visible = false;
            btnApply.Visible = false;
        }

        /// <summary>
        /// Initializes controls of the form
        /// </summary>
        private void Initialize(LabelCategory lb)
        {
            m_category = lb;

            m_NoEvents = true;
            cboFontName.Items.Clear();
            foreach (FontFamily family in FontFamily.Families)
            {
                cboFontName.Items.Add(family.Name);
            }

            icbLineType.ComboStyle = ImageComboStyle.LineStyle;
            icbLineWidth.ComboStyle = ImageComboStyle.LineWidth;

            icbFrameType.Color1 = Color.Transparent;
            icbFrameType.ComboStyle = ImageComboStyle.FrameType;
            icbFrameType.SelectedIndex = 0;

            if (!m_categoryEdited)
            {
                for (int i = 0; i < m_shapefile.NumFields; i++)
                {
                    string name = m_shapefile.Field[i].Name;
                    if (name.ToLower() != "mwshapeid")
                        listBox1.Items.Add(name);
                }
            }

            string[] scales = { "1", "10", "100", "1000", "5000", "10000", "25000", "50000", "100000", 
                                "250000", "500000", "1000000", "10000000" };
            cboMinScale.Items.Clear();
            cboMaxScale.Items.Clear();
            cboBasicScale.Items.Clear();
            foreach (string t in scales)
            {
                cboMinScale.Items.Add(t);
                cboMaxScale.Items.Add(t);
                cboBasicScale.Items.Add(t);
            }

            // displaying options in the GUI
            LabelStyle2Gui(m_category);

            btnTest_Click(null, null);

            txtLabelExpression.Text = m_shapefile.Labels.VisibilityExpression;

            // serialization
            if (m_categoryEdited)
            {
                m_initState = m_category.Serialize();
            }
            else
            {
                tkSavingMode mode = m_shapefile.Labels.SavingMode;
                m_shapefile.Labels.SavingMode = tkSavingMode.modeNone;
                m_initState = m_shapefile.Labels.Serialize();
                m_shapefile.Labels.SavingMode = mode;
            }

            cboLabelsVerticalPosition.Items.Clear();
            cboLabelsVerticalPosition.Items.Add("Above layer");
            cboLabelsVerticalPosition.Items.Add("Above all layers");
            
            var labels = m_shapefile.Labels;

            cboLabelsVerticalPosition.SelectedIndex = (int)labels.VerticalPosition;
            chkLabelsRemoveDuplicates.Checked = labels.RemoveDuplicates;
            chkAviodCollisions.Checked = labels.AvoidCollisions;
            chkScaleLabels.Checked = labels.ScaleLabels;
            cboBasicScale.Text = labels.BasicScale.ToString();

            udLabelOffsetX.SetValue(labels.OffsetX);
            udLabelOffsetY.SetValue(labels.OffsetY);
            udLabelsBuffer.SetValue(labels.CollisionBuffer);

            // alignment
            optAlignBottomCenter.Checked = (labels.Alignment == tkLabelAlignment.laBottomCenter);
            optAlignBottomLeft.Checked = (labels.Alignment == tkLabelAlignment.laBottomLeft);
            optAlignBottomRight.Checked = (labels.Alignment == tkLabelAlignment.laBottomRight);
            optAlignCenter.Checked = (labels.Alignment == tkLabelAlignment.laCenter);
            optAlignCenterLeft.Checked = (labels.Alignment == tkLabelAlignment.laCenterLeft);
            optAlignCenterRight.Checked = (labels.Alignment == tkLabelAlignment.laCenterRight);
            optAlignTopCenter.Checked = (labels.Alignment == tkLabelAlignment.laTopCenter);
            optAlignTopLeft.Checked = (labels.Alignment == tkLabelAlignment.laTopLeft);
            optAlignTopRight.Checked = (labels.Alignment == tkLabelAlignment.laTopRight);

            ShpfileType shpType = Globals.ShapefileType2D(m_shapefile.ShapefileType);
            optAlignCenter.Enabled = (shpType != ShpfileType.SHP_POINT && shpType != ShpfileType.SHP_MULTIPOINT);

            btnApply.Enabled = (m_shapefile.Labels.Expression != "" && m_shapefile.Labels.Count == 0);
            string[] list = new string[]{
                                            "Default",
                                            "SingleBitPerPixelGridFit",
                                            "SingleBitPerPixel",
                                            "AntiAliasGridFit",
                                            "HintAntiAlias",
                                            "ClearType"};
            cboTextRenderingHint.DataSource = list;
            SetSelectedIndex(cboTextRenderingHint, (int)m_shapefile.Labels.TextRenderingHint);
            cboTextRenderingHint.SelectedIndexChanged += Gui2LabelStyle;

            m_NoEvents = false;

            // initial drawing
            this.DrawPreview(null, null);
        }

        /// <summary>
        /// Sets selected index in the combo in case it's the valid one
        /// </summary>
        private static void SetSelectedIndex(ComboBox combo, int index)
        {
            if (index >= 0 && index < combo.Items.Count)
                combo.SelectedIndex = index;
        }

        /// <summary>
        /// Loads label options to the GUI controls
        /// </summary>
        private bool LabelStyle2Gui(LabelCategory lb)
        {
            if (lb == null) 
                return false;

            chkVisible.Checked = m_category.Visible;

            string fontName = lb.FontName;
            int j = 0;
            foreach (FontFamily family in FontFamily.Families)
            {
                if (family.Name == fontName)
                    cboFontName.SelectedIndex = j;
                j++;
            }
            if (cboFontName.SelectedIndex == -1)
            {
                cboFontName.SelectedItem = "Arial";
            }

            // font style
            chkFontBold.Checked = lb.FontBold;
            chkFontItalic.Checked = lb.FontItalic;
            chkFontUnderline.Checked = lb.FontUnderline;
            chkFontStrikeout.Checked = lb.FontStrikeOut;

            udFontSize.Value = lb.FontSize;

            clpFont1.Color = Colors.UintToColor(lb.FontColor);

            udFramePaddingX.SetValue(lb.FramePaddingX);
            udFramePaddingY.SetValue(lb.FramePaddingY);

            // font outlines
            chkHaloVisible.Checked = lb.HaloVisible;
            chkShadowVisible.Checked = lb.ShadowVisible;

            clpShadow.Color = Colors.UintToColor(lb.ShadowColor);
            clpHalo.Color = Colors.UintToColor(lb.HaloColor);

            udHaloSize.SetValue(lb.HaloSize);
            udShadowOffsetX.SetValue(lb.ShadowOffsetX);
            udShadowOffsetY.SetValue(lb.ShadowOffsetY);

            // frame options
            chkUseFrame.Checked = lb.FrameVisible;
            icbFrameType.SelectedIndex = (int)lb.FrameType;

            icbLineType.SelectedIndex = (int)lb.FrameOutlineStyle;

            clpFrame1.Color = Colors.UintToColor(lb.FrameBackColor);
            clpFrameBorder.Color = Colors.UintToColor(lb.FrameOutlineColor);

            udFramePaddingX.SetValue(lb.FramePaddingX);
            udFramePaddingY.SetValue(lb.FramePaddingY);

            if (lb.FrameOutlineWidth < 1) lb.FrameOutlineWidth = 1;
            if (lb.FrameOutlineWidth > icbLineWidth.Items.Count) lb.FrameOutlineWidth = icbLineWidth.Items.Count;
            icbLineWidth.SelectedIndex = (int)lb.FrameOutlineWidth - 1;

            transparencyControl1.Value = (byte)lb.FrameTransparency;

            cboMinScale.Text = m_shapefile.Labels.MinVisibleScale.ToString();
            cboMaxScale.Text = m_shapefile.Labels.MaxVisibleScale.ToString();
            chkDynamicVisibility.Checked = m_shapefile.Labels.DynamicVisibility;

            return true;
        }

        /// <summary>
        /// Saves the options from the GUI to labels style class
        /// </summary>
        private void Gui2LabelStyle(object sender, EventArgs e)
        {
            if (m_NoEvents)
            {
                return;
            }
           
            LabelCategory lb = m_category;

            lb.Visible = chkVisible.Checked;

            // alignment
            lb.FramePaddingX = (int)udFramePaddingX.Value;
            lb.FramePaddingY = (int)udFramePaddingY.Value;

            // font 
            lb.FontBold = chkFontBold.Checked;
            lb.FontItalic = chkFontItalic.Checked;
            lb.FontUnderline = chkFontUnderline.Checked;
            lb.FontStrikeOut = chkFontStrikeout.Checked;
            lb.FontName = cboFontName.Text;
            lb.FontColor = Colors.ColorToUInteger(clpFont1.Color);
            lb.FontSize = (int)udFontSize.Value;

            // outline
            lb.HaloVisible = chkHaloVisible.Checked;
            lb.ShadowVisible = chkShadowVisible.Checked;

            lb.HaloColor = Colors.ColorToUInteger(clpHalo.Color);
            lb.ShadowColor = Colors.ColorToUInteger(clpShadow.Color);

            lb.HaloSize = (int)udHaloSize.Value;
            lb.ShadowOffsetX = (int)udShadowOffsetX.Value;
            lb.ShadowOffsetY = (int)udShadowOffsetY.Value;
            
            // frame fill
            lb.FrameBackColor = Colors.ColorToUInteger(clpFrame1.Color);

            if (tabControl1.SelectedTab.Name == "tabFrameFill")
            {
                lb.FrameVisible = chkUseFrame.Checked;
                lb.FrameType = (MapWinGIS.tkLabelFrameType)icbFrameType.SelectedIndex;
            }

            // frame outline
            lb.FrameOutlineColor = Colors.ColorToUInteger(clpFrameBorder.Color);
            if (icbLineType.SelectedIndex >= 0)
            {
                lb.FrameOutlineStyle = (tkDashStyle)icbLineType.SelectedIndex;
            }
            lb.FrameOutlineWidth = (int)icbLineWidth.SelectedIndex + 1;

            lb.FrameTransparency = transparencyControl1.Value;
            lb.FontTransparency = transparencyControl1.Value;

            // passed from the main form
            m_shapefile.Labels.RemoveDuplicates = chkLabelsRemoveDuplicates.Checked;
            m_shapefile.Labels.AvoidCollisions = chkAviodCollisions.Checked;
            m_shapefile.Labels.ScaleLabels = chkScaleLabels.Checked;
            
            double val;
            m_shapefile.Labels.BasicScale = (double.TryParse(cboBasicScale.Text, out val)) ? val : 0.0;
            m_shapefile.Labels.VerticalPosition = (MapWinGIS.tkVerticalPosition)cboLabelsVerticalPosition.SelectedIndex;

            lb.OffsetX = (double)udLabelOffsetX.Value;
            lb.OffsetY = (double)udLabelOffsetY.Value;
            m_shapefile.Labels.CollisionBuffer = (int)udLabelsBuffer.Value;

            // alignment
            if (optAlignBottomCenter.Checked) lb.Alignment = tkLabelAlignment.laBottomCenter;
            if (optAlignBottomLeft.Checked) lb.Alignment = tkLabelAlignment.laBottomLeft;
            if (optAlignBottomRight.Checked) lb.Alignment = tkLabelAlignment.laBottomRight;
            if (optAlignCenter.Checked) lb.Alignment = tkLabelAlignment.laCenter;
            if (optAlignCenterLeft.Checked) lb.Alignment = tkLabelAlignment.laCenterLeft;
            if (optAlignCenterRight.Checked) lb.Alignment = tkLabelAlignment.laCenterRight;
            if (optAlignTopCenter.Checked) lb.Alignment = tkLabelAlignment.laTopCenter;
            if (optAlignTopLeft.Checked) lb.Alignment = tkLabelAlignment.laTopLeft;
            if (optAlignTopRight.Checked) lb.Alignment = tkLabelAlignment.laTopRight;

            // categories will have the same alignment
            if (!m_categoryEdited)
            {
                for (int i = 0; i < m_shapefile.Labels.NumCategories; i++)
                {
                    var cat = m_shapefile.Labels.Category[i];
                    cat.Alignment = lb.Alignment;
                    cat.OffsetX = lb.OffsetX;
                    cat.OffsetY = lb.OffsetY;
                }
            }

            if (double.TryParse(cboMinScale.Text, out val))
            {
                m_shapefile.Labels.MinVisibleScale = val;
            }

            if (double.TryParse(cboMaxScale.Text, out val))
            {
                m_shapefile.Labels.MaxVisibleScale = val;
            }
            m_shapefile.Labels.DynamicVisibility = chkDynamicVisibility.Checked;

            btnApply.Enabled = true;

            m_shapefile.Labels.TextRenderingHint = (tkTextRenderingHint)cboTextRenderingHint.SelectedIndex;
            // TODO: implement
            //this.labelList1.TextRenderingHint = (TextRenderingHint)cboTextRenderingHint.SelectedIndex;

            DrawPreview(null, null);
            return;
        }

        /// <summary>
        /// Draws preview of the label
        /// </summary>
        private void DrawPreview(object sender, EventArgs e)
        {
            // this function is called after each chnage of state, therefore it makes sense to update availability of controls here
            RefreshControls();

            if (!m_NoEvents)
            {
                
                if (m_category.Visible)
                {
                    string text = m_categoryEdited ? m_shapefile.Labels.Expression : richTextBox1.Text;
                    LabelUtilities.DrawPreview(m_category, m_shapefile, pctPreview, text, true);
                }
                else
                {
                    Bitmap img = new Bitmap((int)pctPreview.ClientRectangle.Width, (int)pctPreview.ClientRectangle.Height);
                    if (pctPreview.Image != null)
                    {
                        pctPreview.Image.Dispose();
                    }
                    pctPreview.Image = img;
                }
            }
        }

        /// <summary>
        /// Enables or disables controls which are dependant upon others
        /// </summary>
        private void RefreshControls()
        {
            m_NoEvents = true;

            // drawing of frame
            bool drawFrame = chkUseFrame.Checked;
            clpFrame1.Enabled = drawFrame;
            icbLineType.Enabled = drawFrame;
            icbLineWidth.Enabled = drawFrame;
            clpFrameBorder.Enabled = drawFrame;
            udFramePaddingX.Enabled = drawFrame;
            udFramePaddingY.Enabled = drawFrame;

            //outlines
            udHaloSize.Enabled = chkHaloVisible.Checked;
            clpHalo.Enabled = chkHaloVisible.Checked;
            label15.Enabled = chkHaloVisible.Checked;

            udShadowOffsetX.Enabled = chkShadowVisible.Checked;
            udShadowOffsetY.Enabled = chkShadowVisible.Checked;
            clpShadow.Enabled = chkShadowVisible.Checked;
            label12.Enabled = chkShadowVisible.Checked;
            label9.Enabled = chkShadowVisible.Checked;

            icbFrameType.Enabled = chkUseFrame.Checked;
            btnSetFrameGradient.Enabled = chkUseFrame.Checked;

            panel1.Enabled = chkDynamicVisibility.Checked;

            cboBasicScale.Enabled = chkScaleLabels.Checked;
            btnSetCurrent.Enabled = chkScaleLabels.Checked;
            lblScaleLabels.Enabled = chkScaleLabels.Checked;

            bool noLabels = false;;
            bool hasExpression = richTextBox1.Text.Length > 0;
            if (!m_categoryEdited)
            {
                noLabels = !hasExpression;
                groupBox6.Enabled = hasExpression;
                groupBox11.Enabled = hasExpression;
                groupBox13.Enabled = hasExpression;
                groupBox20.Enabled = hasExpression;
                groupLabelAlignment.Enabled = hasExpression;
                chkUseFrame.Enabled = hasExpression;
                groupBox2.Enabled = hasExpression;
                groupBox3.Enabled = hasExpression;
                groupBox4.Enabled = hasExpression;
                groupBox5.Enabled = hasExpression;
                chkScaleLabels.Enabled = hasExpression;
            }

            

            groupBox4.Enabled = !noLabels && chkUseFrame.Checked;
            groupBox2.Enabled = !noLabels && chkUseFrame.Checked;

            m_NoEvents = false;
        }
        
        /// <summary>
        /// Sets gradient for the frame color
        /// </summary>
        private void btnSetFrameGradient_Click(object sender, EventArgs e)
        {
            FontGradientForm form = new FontGradientForm(m_category, false);
            if (form.ShowDialog(this) == DialogResult.OK)
            {
                DrawPreview(null, null);
                clpFrame1.Color = Colors.UintToColor(m_category.FrameBackColor);
                btnApply.Enabled = true;
            }
        }
        
        /// <summary>
        /// Clears gradient of the frame
        /// </summary>
        private void btnClearFrameGradient_Click(object sender, EventArgs e)
        {
            m_category.FrameGradientMode = tkLinearGradientMode.gmNone;
            DrawPreview(null, null);
        }

        /// <summary>
        ///  Saves the options, closes the form.
        /// </summary>
        private void btnOk_Click(object sender, EventArgs e)
        {
            if (!ApplyOptions())
                return;

            tabNumber = tabControl1.SelectedIndex;

            if (m_shapefile.Labels.Serialize() != m_initState)
            {
                m_legend.FireLayerPropertiesChanged(m_handle);
            }

            Globals.SaveLayerOptions(m_handle);

            this.DialogResult = DialogResult.OK;
        }

        /// <summary>
        /// Applies the options
        /// </summary>
        bool ApplyOptions()
        {
            if (m_categoryEdited)
                return true;
            
            if (richTextBox1.Text == "" && m_shapefile.Labels.Count > 0)
            {
                // clear
                if (MessageBox.Show("Expression is empty. Remove all the labels?",
                                 Legend.Controls.Legend.Legend.AppName, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    m_shapefile.Labels.Clear();
                    m_shapefile.Labels.Expression = "";
                }
                else
                    return false;
            }
            else if ((!m_shapefile.Labels.Synchronized  || m_shapefile.Labels.Count == 0) && richTextBox1.Text != "")
            {
                // generate
                AddLabelsForm form = new AddLabelsForm(m_shapefile, m_category.Alignment);
                if (form.ShowDialog(this) == DialogResult.OK)
                {
                    ShpfileType type = Globals.ShapefileType2D(m_shapefile.ShapefileType);
                    if (type == ShpfileType.SHP_POINT || type == ShpfileType.SHP_MULTIPOINT)
                    {
                        m_category.Alignment = form.m_alignment;
                    }

                    form.Dispose();
                }
                else
                {
                    form.Dispose();
                    return false;
                }
            }
            else if (richTextBox1.Text == "" && m_shapefile.Labels.Count == 0)
            {
                //MessageBox.Show("No expression was entered.", Globals.AppName, MessageBoxButtons.OK, MessageBoxIcon.Information);
                //return false;
            }

            // applying options
            if (!m_categoryEdited)
            {
                // in case of labels we are editing a copy of the LabelsCategory class, so options should be applied
                m_shapefile.Labels.Options = m_category;

                if (m_shapefile.Labels.Expression != richTextBox1.Text )
                {
                    m_shapefile.Labels.Expression = MapWindow.Legend.Classes.LabelUtilities.FixExpression(richTextBox1.Text);
                }
            }
            return true;
        }

        /// <summary>
        ///  Handles the change of transparency by user
        /// </summary>
        private void transparencyControl1_ValueChanged(object sender, byte value)
        {
            Gui2LabelStyle(sender, null);
        }

        #region Expression
        /// <summary>
        /// Adds field to the expression
        /// </summary>
        private void listBox1_DoubleClick(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem == null)
                return;

            richTextBox1.SelectedText = "[" + listBox1.SelectedItem.ToString() + "] ";
        }

        private void btnPlus_Click(object sender, EventArgs e)
        {
            richTextBox1.SelectedText = "+ ";
        }

        private void btnQuotes_Click(object sender, EventArgs e)
        {
            richTextBox1.SelectedText = "\"\"";
        }

        private void btnNewLine_Click(object sender, EventArgs e)
        {
            richTextBox1.SelectedText = Environment.NewLine.ToString();
        }

        /// <summary>
        /// Tests expression entered by user
        /// </summary>
        private void btnTest_Click(object sender, EventArgs e)
        {
            if (richTextBox1.Text.ToLower() != "<no expression>")
            {
                string expr = MapWindow.Legend.Classes.LabelUtilities.FixExpression(richTextBox1.Text);
                if (expr == String.Empty)
                {
                    lblResult.ForeColor = Color.Black;
                    lblResult.Text = "No expression";
                }
                else
                {
                    string err = "";
                    if (!m_shapefile.Table.TestExpression(expr, tkValueType.vtString, ref err))
                    {
                        lblResult.ForeColor = Color.Red;
                        lblResult.Text = err;
                    }
                    else
                    {
                        lblResult.ForeColor = Color.Green;
                        lblResult.Text = "Expression is valid";
                    }
                }
            }
        }

        /// <summary>
        /// Adds field to the expression
        /// </summary>
        private void listBox1_DoubleClick_1(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem == null)
                return;

            richTextBox1.SelectedText = "[" + listBox1.SelectedItem.ToString() + "] ";
        }
        #endregion

        /// <summary>
        /// Checks the expression during editing
        /// </summary>
        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
            btnTest_Click(null, null);
            
            if (!m_NoEvents)
            {
                MapWindow.Legend.Classes.LabelUtilities.DrawPreview(m_category, m_shapefile, pctPreview, richTextBox1.Text, true);
                RefreshControls();
            }

            btnApply.Enabled = true;
        }

        /// <summary>
        /// Clears the expression in the textbox
        /// </summary>
        private void btnClear_Click(object sender, EventArgs e)
        {
            if (richTextBox1.Text.ToLower() == "<no expression>")
            {
                if (MessageBox.Show("Remove labels?", Legend.Controls.Legend.Legend.AppName,
                                    MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    MapWinGIS.Labels lb = m_shapefile.Labels;
                    for (int i = 0; i < lb.Count; i++)
                    {
                        for (int j = 0; j < lb.get_NumParts(i); j++)
                        {
                            lb.get_Label(i, j).Text = "";
                        }
                    }

                    listBox1.Enabled = true;
                    btnPlus.Enabled = true;
                    btnQuotes.Enabled = true;
                    btnNewLine.Enabled = true;
                    richTextBox1.Enabled = true;
                    richTextBox1.Text = "";

                    lb.SavingMode = tkSavingMode.modeXMLOverwrite;
                    lb.Synchronized = true;
                    if (!lb.Synchronized)
                    {
                        lb.Clear();
                    }
                }
            }
            else
            {
                richTextBox1.Text = "";
            }
        }
        
        /// <summary>
        /// Building labels visiblity expression
        /// </summary>
        private void btnLabelExpression_Click(object sender, EventArgs e)
        {
            string s = txtLabelExpression.Text;
            frmQueryBuilder form = new frmQueryBuilder(m_shapefile, m_handle, s, false);
            if (form.ShowDialog() == DialogResult.OK)
            {
                if (txtLabelExpression.Text != form.Tag.ToString())
                {
                    txtLabelExpression.Text = form.Tag.ToString();
                    m_shapefile.Labels.VisibilityExpression = txtLabelExpression.Text;
                    btnApply.Enabled = true;
                }
            }
            form.Dispose();
        }

        /// <summary>
        /// Clears the label expression
        /// </summary>
        private void btnClearLabelsExpression_Click(object sender, EventArgs e)
        {
            txtLabelExpression.Clear();
            m_shapefile.Labels.VisibilityExpression = "";
        }

        /// <summary>
        /// Saves the options and updates the map without closing the form
        /// </summary>
        private void btnApply_Click(object sender, EventArgs e)
        {
            if (ApplyOptions())
            {
                if (m_legend != null)
                {
                    m_legend.FireLayerPropertiesChanged(m_handle);
                    m_legend.Map.Redraw();
                    m_legend.Redraw();
                }

                m_initState = m_shapefile.Labels.Serialize();
                RefreshControls();
                btnApply.Enabled = false;
            }
        }

        /// <summary>
        /// Reverts the changes if cancel was hit
        /// </summary>
        private void frmLabelStyle_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (this.DialogResult == DialogResult.Cancel)
            {
                if (m_categoryEdited)
                {
                    m_category.Deserialize(m_initState);
                }
                else
                {
                    tkSavingMode mode = m_shapefile.Labels.SavingMode;
                    m_shapefile.Labels.SavingMode = tkSavingMode.modeNone;
                    m_shapefile.Labels.Deserialize(m_initState);
                    m_shapefile.Labels.SavingMode = mode;
                }
            }
        }

        /// <summary>
        /// Sets current scale as basic one
        /// </summary>
        private void btnSetCurrent_Click(object sender, EventArgs e)
        {
            Map map = m_legend.Map;
            if (map != null)
            {
                cboBasicScale.Text = map.CurrentScale.ToString("0.00");
            }
        }

        /// <summary>
        /// Sets max visible scale to current scale
        /// </summary>
        private void btnSetMaxScale_Click(object sender, EventArgs e)
        {
            Map map = m_legend.Map;
            if (map != null)
            {
                cboMaxScale.Text = map.CurrentScale.ToString("0.00");
                btnApply.Enabled = true;
            }
        }

        /// <summary>
        /// Sets min visible scale to current scale
        /// </summary>
        private void btnSetMinScale_Click(object sender, EventArgs e)
        {
            Map map = m_legend.Map;
            if (map != null)
            {
                cboMinScale.Text = map.CurrentScale.ToString("0.00");
                btnApply.Enabled = true;
            }
        }
    }
}
