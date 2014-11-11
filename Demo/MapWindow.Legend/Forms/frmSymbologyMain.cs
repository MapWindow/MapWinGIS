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

using MapWindow.Legend.Classes;
using MapWindow.Legend.Controls.Legend;

namespace MapWindow.Legend.Forms
{
    using System;
    using System.Windows.Forms;
    using MapWinGIS;
    using MapWindow.Legend;
   
    /// <summary>
    /// Available actions while changing label options
    /// </summary>
    internal enum LabelAction
    {
        ChangeAll = 0,
        ChangeText = 1,
    }

    /// <summary>
    /// A class representing GUI for managing Shapefile symbology
    /// </summary>
    public partial class frmSymbologyMain : Form
    {
        #region Member variables

        // A reference to the plug-in
        private Controls.Legend.Legend m_legend;

        // Handle of the shapefile layer
        public int m_layerHandle = -1;

        // MW the layer being edited
        Layer m_layer = null;

        // the plug-in settings will be held in legend; 
        // but it's possible to add them in the layer interface if needed
        SymbologySettings m_settings = null;
        
        // Reference to the shapefile
        private MapWinGIS.Shapefile m_shapefile = null;
        
        // Temporary shapefile used to show preview (there are problems when trying to create it in back thread)
        private MapWinGIS.Shapefile sfPreview = null;
        
        // The tab index to show on the loading
        static int m_tabIndex = 0;
        
        // Prevents controls from triggering events when managed from code
        private bool _noEvents = false;

        // init state for serialization
        private string m_initState = "";

        // changes flag
        private bool m_stateChanged = false;
        
        // redraw map at once is being changed
        private bool m_redrawModeIsChanging = false;

        #endregion

        #region Constructor
        /// <summary>
        /// Creates new instance of the SymbologyMainForm class
        /// </summary>
        public frmSymbologyMain(Controls.Legend.Legend legend, int layerHandle)
        {
            // Required for Windows Form Designer support
            InitializeComponent();

            // setting member variables (some of them for faster access)
            m_legend = legend;
            m_layerHandle = layerHandle;
            m_layer = m_legend.GetLayer(m_layerHandle);
            m_shapefile = m_layer.GetObject() as MapWinGIS.Shapefile;

            m_settings = Globals.get_LayerSettings(m_layerHandle);
            this.Text = "Layer properties: " + m_layer.Name;

            // update map at once button is off by default which is equal to locked state of the map
            //m_mapWin.View.LockMap();
            //m_mapWin.View.LockLegend();

            m_initState = SaveState();
           
            // the first color in color scheme should be the fill/line color of the shapefile
            Globals.LayerColors.SetDefaultColorScheme(m_shapefile);

            _noEvents = true;

            InitGeneralTab();

            InitModeTab();

            InitAppearanceTab();

            InitCategoriesTab();

            InitLabelsTab();

            InitChartsTab();

            InitExpressionTab();

            InitVisibilityTab();

            UpdateColorSchemes();
            
            // the state should be set after the loading as otherwise we can trigger unnecessary redraws
            chkRedrawMap.Checked = false; //m_settings.UpdateMapAtOnce;

            _noEvents = false;
            
            // sets the enabled state of the controls
            RefreshControlsState(null, null);

            DrawAllPreviews();
            
            AddTooltips();

            // displaying the last tab
            tabControl1.SelectedIndex = m_tabIndex;

            tabControl1.TabPages.Remove(tabCharts);
            tabControl1.TabPages.Remove(tabLabels);
            tabControl1.TabPages.Remove(tabDefault);
        }

        /// <summary>
        /// Sets the state of controls on the expression tab on loading
        /// </summary>
        private void InitExpressionTab()
        {
            txtLayerExpression.Text = m_shapefile.VisibilityExpression;
        }
       
        #endregion

        #region Controls updating

        /// <summary>
        /// Updates shapefile settings according to the values of the controls 
        /// </summary>
        private void GUI2Settings(object sender, EventArgs e)
        {
            if (_noEvents)
                return;

            m_stateChanged = true;
            btnSaveChanges.Enabled = true;

            if (tabControl1.SelectedTab.Name.ToLower() == "tabmode")
            {
                m_shapefile.CollisionMode = (tkCollisionMode)cboCollisionMode.SelectedIndex;
            }
            if (tabControl1.SelectedTab.Name.ToLower() == "tabgeneral")
            {
                m_layer.Visible = chkLayerVisible.Checked;
                m_layer.Name = txtLayerName.Text;
            }
            else if (tabControl1.SelectedTab.Name.ToLower() == "tabdefault")
            {
                Controls2Appearance();
            }
            else if (tabControl1.SelectedTab.Name.ToLower() == "tablabels")
            {
                this.UpdateLabels();
                
            }
            else if (tabControl1.SelectedTab.Name.ToLower() == "tabcharts")
            {
                this.UpdateCharts();
            }

            m_shapefile.VisibilityExpression = txtLayerExpression.Text;

            RefreshControlsState(null, null);
            RedrawMap();
        }

        /// <summary>
        /// Changes the enabled properties of controls
        /// </summary>
        private void RefreshControlsState(object sender, EventArgs e)
        {
            if (_noEvents)
                return;
            
            ShpfileType type = Globals.ShapefileType2D(m_shapefile.ShapefileType);
           
            // appearance
            udDefaultSize.Enabled = (type == ShpfileType.SHP_POINT || type == ShpfileType.SHP_MULTIPOINT);
            clpPointFill.Enabled = (type != ShpfileType.SHP_POLYLINE);
            clpSelection.Enabled = (type != ShpfileType.SHP_POLYLINE);

            // provide the options if there are a single line pattern, otherwise extednded options are needed
            ShapeDrawingOptions options = m_shapefile.DefaultDrawingOptions;
            if (options.UseLinePattern)
            {
                if (options.LinePattern.Count <= 1)
                {
                    panelLineOptions.Visible = true;
                    clpDefaultOutline.Enabled = true;
                    icbLineWidth.Enabled = (options.LinePattern.get_Line(0).LineType == tkLineType.lltSimple);
                    lblMultilinePattern.Visible = false;
                }
                else
                {
                    lblMultilinePattern.Visible = true;
                    panelLineOptions.Visible = false;
                }
            }
            else
            {
                clpDefaultOutline.Enabled = true;
                icbLineWidth.Enabled = true;
                panelLineOptions.Visible = true;
                lblMultilinePattern.Visible = false;
            }

            // categories
            udMinSize.Enabled = chkUseVariableSize.Checked;
            udMaxSize.Enabled = chkUseVariableSize.Checked;
            udNumCategories.Enabled = !chkUniqueValues.Checked;
            btnCategoryAppearance.Enabled = (dgvCategories.SelectedCells.Count > 0);
            btnCategoryRemove.Enabled = (dgvCategories.SelectedCells.Count > 0);
            btnCategoryClear.Enabled = (dgvCategories.Rows.Count > 0);

            // labels
            MapWinGIS.Labels labels = m_shapefile.Labels;

            //btnLabelsAppearance.Enabled = (labels.Count > 0);
            btnLabelsClear.Enabled = (labels.Count > 0);
            groupLabelAppearance.Enabled = (labels.Count > 0);
            groupLabelStyle.Enabled = (labels.Count > 0);
            chkShowLabels.Enabled = (labels.Count > 0);
            panelLabels.Enabled = (labels.Count > 0);
            groupChartAppearance.Enabled = m_shapefile.Charts.Count > 0;

            // charts
            bool enabled = (m_shapefile.Charts.Count > 0); //&& (m_shapefile.Charts.NumFields > 0);
            btnClearCharts.Enabled = (m_shapefile.Charts.Count > 0);
            icbChartColorScheme.Enabled = enabled;
            groupCharts.Enabled = enabled;
            optChartBars.Enabled = enabled;
            optChartsPie.Enabled = enabled;
            chkChartsVisible.Enabled = enabled;
            btnChartsEditColorScheme.Enabled = enabled;
        }
        #endregion

        #region Utility
        /// <summary>
        /// Redraws the map if needed
        /// </summary>
        private void RedrawMap()
        {
            //if (chkRedrawMap.Checked && !_noEvents)
            m_legend.Map.Redraw();
            m_legend.Refresh();
            
            // it's assumed that we call redraw when state changed only
            if (!_noEvents && !m_redrawModeIsChanging)
            {
                MarkStateChanged();
            }
        }

        /// <summary>
        /// Redraws legend
        /// </summary>
        private void RedrawLegend()
        {
            if (chkRedrawMap.Checked && !_noEvents)
            {
                m_legend.Refresh();
            }
        }

        /// <summary>
        /// Redraws preview windows on all tabs
        /// </summary>
        private void DrawAllPreviews()
        {
            DrawAppearancePreview();
            DrawLabelsPreview();
            DrawChartsPreview();
        }

        /// <summary>
        /// Updates color schemes list on the charts tab
        /// </summary>
        public void UpdateColorSchemes()
        {
            icbChartColorScheme.ColorSchemes = Globals.ChartColors;
            icbCategories.ColorSchemes = Globals.LayerColors;
        }

        /// <summary>
        /// Saves all the data before the closing
        /// </summary>
        private void frmSymbologyMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (this.DialogResult == DialogResult.Cancel)
            {
                CancelChanges();
            }
            
            //while (m_mapWin.View.IsMapLocked)
            //{
            //    m_mapWin.View.UnlockMap();
            //}

            //while (Globals.Legend.Locked)
            //{
            //    Globals.Legend.Unlock();
            //}
            
            Layer lyr = m_layer;
            if (lyr != null)
            {
                lyr.Name = txtLayerName.Text;
                m_tabIndex = tabControl1.SelectedIndex;

                if (!chkRedrawMap.Checked)      // we presume here that the map is in actual state in case the checkbox is set
                {
                    m_legend.Map.Redraw();
                    m_legend.Refresh();
                }

                Layer layer = m_legend.Layers.ItemByHandle(m_layerHandle);
                m_settings.ShowLayerPreview = chkLayerPreview.Checked;
                m_settings.UpdateMapAtOnce = chkRedrawMap.Checked;
                m_settings.Comments = txtComments.Text;

                axMap1.RemoveAllLayers();
                if (sfPreview != null)
                {
                    sfPreview.Close();
                    sfPreview = null;
                }
            }
        }

        #endregion

        #region Tooltips
        /// <summary>
        /// Adds tooltips t the controls. Resources should be used here to support internatianalization.
        /// </summary>
        private void AddTooltips()
        {
            // general tab
            toolTip1.SetToolTip(txtLayerName, "The name of layer. \nEditable. Can be different from the name of the source file");
            toolTip1.SetToolTip(txtLayerSource, "Source file information. Can be copied and pasted");
            toolTip1.SetToolTip(chkLayerVisible, "Toggles the visibility of the layer");
            toolTip1.SetToolTip(btnOk, "Closes the window. Saves the settings.");

            // appearance tab
            toolTip1.SetToolTip(pictureBox1, "Symbology used for the shapes which don't fall into any category. \nClick to change settings.");
            toolTip1.SetToolTip(btnDefaultChange, "Changes the default symbology");
            // categories
            toolTip1.SetToolTip(lstFields1, "List of fields from the attribute table");
            toolTip1.SetToolTip(udNumCategories, "Specifies the number of classes to be generated");
            toolTip1.SetToolTip(chkUniqueValues, "A separate category will be generated for every unique value of the field");
            toolTip1.SetToolTip(icbCategories, "List of available color schemes. \nNew color schemes can be added by clicking <...> button");
            toolTip1.SetToolTip(chkSetGradient, "Sets color gradient for particular shapes");
            toolTip1.SetToolTip(chkRandomColors, "Chooses the colors from color scheme randomly");
            toolTip1.SetToolTip(btnChangeColorScheme, "Opens color schemes editor");
            toolTip1.SetToolTip(dgvCategories, "List of categories. \nClick on the preview to change settings." +
            "\nClick on the name to edit it.\nCount column displays number of shapes which fell in the category");

            toolTip1.SetToolTip(chkUseVariableSize, "Enables the graduated symbol size.\nApplicable for point layers only");
            toolTip1.SetToolTip(udMinSize, "Minimum size of symbols");
            toolTip1.SetToolTip(udMinSize, "Maximum size of symbols");
            toolTip1.SetToolTip(btnCategoryGenerate, "Generates categories and applies new settings");
            toolTip1.SetToolTip(btnCategoryAppearance, "Changes style of the selected category");
            toolTip1.SetToolTip(btnCategoryRemove, "Removes selected category");
            toolTip1.SetToolTip(btnCategoryClear, "Removes all categories");

            // TODO: should be written for the rest tabs
        }
        #endregion

        /// <summary>
        /// Reverts the changes and closes the form
        /// </summary>
        private void btnCancel_Click(object sender, EventArgs e)
        {
            
        }

        /// <summary>
        /// Cancels the changes made by user
        /// </summary>
        private void CancelChanges()
        {
            MapWinGIS.Map map = m_legend.Map;
            if (map != null)
            {
                string state = map.SerializeLayer(m_layerHandle);

                if (state != m_initState)
                {
                    // label and chart data must not be serialized
                    tkSavingMode mode1 = m_shapefile.Labels.SavingMode;
                    tkSavingMode mode2 = m_shapefile.Charts.SavingMode;

                    m_shapefile.Labels.SavingMode = tkSavingMode.modeNone;
                    m_shapefile.Charts.SavingMode = tkSavingMode.modeNone;

                    bool res = map.DeserializeLayer(m_layerHandle, m_initState);

                    m_shapefile.Labels.SavingMode = mode1;
                    m_shapefile.Charts.SavingMode = mode2;

                    m_legend.Map.Redraw();
                    //Globals.Legend.Refresh();
                }
            }

            //while (m_mapWin.View.IsMapLocked)
            //{
            //    m_mapWin.View.UnlockMap();
            //}

            //while (Globals.Legend.Locked)
            //{
            //    Globals.Legend.Unlock();
            //}
        }

        /// <summary>
        /// Saves the current changes treating them as the new initial state
        /// </summary>
        private void btnSaveChanges_Click(object sender, EventArgs e)
        {
            if (m_stateChanged)
            {
                GUI2Settings(null, null);

                // triggering redraw
                if (!chkRedrawMap.Checked)
                {
                    MapWinGIS.Map map = m_legend.Map;
                    
                    //while (m_mapWin.View.IsMapLocked)
                    //{
                    //    m_mapWin.View.UnlockMap();
                    //}

                    //while (Globals.Legend.Locked)
                    //{
                    //    Globals.Legend.Unlock();
                    //}
                    
                    //m_legend.AxMap.Redraw();
                    //Globals.Legend.Refresh();
                    Application.DoEvents();

                    //m_legend.AxMap.LockWindow(tkLockMode.lmLock);
                    //m_mapWin.View.LockMap();
                    //Globals.Legend.Lock();
                }

                // saving new state
                m_initState = SaveState();

                m_stateChanged = false;
                btnSaveChanges.Enabled = false;

                //m_mapWin.Project.Modified = true;
            }
        }

        /// <summary>
        /// Applies the changes and closes the form
        /// </summary>
        private void btnOk_Click(object sender, EventArgs e)
        {
            if (m_initState != SaveState())
            {
                //m_mapWin.Project.Modified = true;
            }
            
            //while (m_mapWin.View.IsMapLocked)
            //{
            //    m_mapWin.View.UnlockMap();
            //}

            //while (Globals.Legend.Locked)
            //{
            //    Globals.Legend.Unlock();
            //}
            
            // saves options for default loading behavior
            Globals.SaveLayerOptions(m_layerHandle);
        }

        /// <summary>
        /// Saves the state in the m_InitState variable
        /// </summary>
        private string SaveState()
        {
            string state = "";
            MapWinGIS.Map map = m_legend.Map;
            if (map != null)
            {
                // serializing for undo (label and chart data must not be serialized)
                tkSavingMode mode1 = m_shapefile.Labels.SavingMode;
                tkSavingMode mode2 = m_shapefile.Charts.SavingMode;

                m_shapefile.Labels.SavingMode = tkSavingMode.modeNone;
                m_shapefile.Charts.SavingMode = tkSavingMode.modeNone;

                state = map.SerializeLayer(m_layerHandle);

                m_shapefile.Labels.SavingMode = mode1;
                m_shapefile.Charts.SavingMode = mode2;
            }
            return state;
        }

        /// <summary>
        /// Sets visiblity of the layer
        /// </summary>
        private void chkLayerVisible_CheckedChanged(object sender, EventArgs e)
        {
            GUI2Settings(null, null);
        }

        /// <summary>
        /// Sets visiblity expression
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void txtLayerExpression_TextChanged(object sender, EventArgs e)
        {
            GUI2Settings(null, null);
        }
    }
}
