using System.ComponentModel;
using System.Windows.Forms;

namespace MWLite.GUI.Forms
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            WeifenLuo.WinFormsUI.Docking.DockPanelSkin dockPanelSkin2 = new WeifenLuo.WinFormsUI.Docking.DockPanelSkin();
            WeifenLuo.WinFormsUI.Docking.AutoHideStripSkin autoHideStripSkin2 = new WeifenLuo.WinFormsUI.Docking.AutoHideStripSkin();
            WeifenLuo.WinFormsUI.Docking.DockPanelGradient dockPanelGradient4 = new WeifenLuo.WinFormsUI.Docking.DockPanelGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient8 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPaneStripSkin dockPaneStripSkin2 = new WeifenLuo.WinFormsUI.Docking.DockPaneStripSkin();
            WeifenLuo.WinFormsUI.Docking.DockPaneStripGradient dockPaneStripGradient2 = new WeifenLuo.WinFormsUI.Docking.DockPaneStripGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient9 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPanelGradient dockPanelGradient5 = new WeifenLuo.WinFormsUI.Docking.DockPanelGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient10 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPaneStripToolWindowGradient dockPaneStripToolWindowGradient2 = new WeifenLuo.WinFormsUI.Docking.DockPaneStripToolWindowGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient11 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient12 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPanelGradient dockPanelGradient6 = new WeifenLuo.WinFormsUI.Docking.DockPanelGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient13 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient14 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            this._mainToolStrip = new System.Windows.Forms.ToolStrip();
            this.toolZoomIn = new System.Windows.Forms.ToolStripButton();
            this.toolZoomOut = new System.Windows.Forms.ToolStripButton();
            this.toolZoomMax = new System.Windows.Forms.ToolStripButton();
            this.toolZoomToLayer = new System.Windows.Forms.ToolStripButton();
            this.toolPan = new System.Windows.Forms.ToolStripButton();
            this.toolSearch = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.toolIdentify = new System.Windows.Forms.ToolStripButton();
            this.toolMeasure = new System.Windows.Forms.ToolStripButton();
            this.toolMeasureArea = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.toolSelection = new System.Windows.Forms.ToolStripDropDownButton();
            this.toolSelect = new System.Windows.Forms.ToolStripMenuItem();
            this.toolSelectByPolygon = new System.Windows.Forms.ToolStripMenuItem();
            this.toolZoomToSelected = new System.Windows.Forms.ToolStripButton();
            this.toolClearSelection = new System.Windows.Forms.ToolStripButton();
            this._menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.mnuFile = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuCloseProject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuOpen = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuAddVector = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuAddRaster = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator7 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuLoadProject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuSaveProject = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuSaveProjectAs = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuCloseApp = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuMap = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuSetProjection = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuSearch = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator9 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuSnapshot = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuTiles = new System.Windows.Forms.ToolStripMenuItem();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.statusSelectedCount = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblProgressMessage = new System.Windows.Forms.ToolStripStatusLabel();
            this.progressBar1 = new System.Windows.Forms.ToolStripProgressBar();
            this.toolStripPanel1 = new System.Windows.Forms.ToolStripPanel();
            this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
            this.dockPanel1 = new WeifenLuo.WinFormsUI.Docking.DockPanel();
            this._toolStripLayer = new System.Windows.Forms.ToolStrip();
            this.toolCloseProject = new System.Windows.Forms.ToolStripButton();
            this.toolLoadProject = new System.Windows.Forms.ToolStripButton();
            this.toolSaveProject = new System.Windows.Forms.ToolStripButton();
            this.toolSaveProjectAs = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator8 = new System.Windows.Forms.ToolStripSeparator();
            this.toolOpen = new System.Windows.Forms.ToolStripButton();
            this.toolAddVector = new System.Windows.Forms.ToolStripButton();
            this.toolAddRaster = new System.Windows.Forms.ToolStripButton();
            this.toolAddDatabase = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolCreateLayer = new System.Windows.Forms.ToolStripButton();
            this.toolRemoveLayer = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator10 = new System.Windows.Forms.ToolStripSeparator();
            this.toolSetProjection = new System.Windows.Forms.ToolStripButton();
            this._mainToolStrip.SuspendLayout();
            this._menuStrip1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.toolStripContainer1.ContentPanel.SuspendLayout();
            this.toolStripContainer1.LeftToolStripPanel.SuspendLayout();
            this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
            this.toolStripContainer1.SuspendLayout();
            this._toolStripLayer.SuspendLayout();
            this.SuspendLayout();
            // 
            // _mainToolStrip
            // 
            this._mainToolStrip.Dock = System.Windows.Forms.DockStyle.None;
            this._mainToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolZoomIn,
            this.toolZoomOut,
            this.toolZoomMax,
            this.toolZoomToLayer,
            this.toolPan,
            this.toolStripSeparator10,
            this.toolSetProjection,
            this.toolSearch,
            this.toolStripSeparator4,
            this.toolIdentify,
            this.toolMeasure,
            this.toolMeasureArea,
            this.toolStripSeparator3,
            this.toolSelection,
            this.toolZoomToSelected,
            this.toolClearSelection});
            this._mainToolStrip.Location = new System.Drawing.Point(3, 0);
            this._mainToolStrip.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this._mainToolStrip.Name = "_mainToolStrip";
            this._mainToolStrip.Padding = new System.Windows.Forms.Padding(0);
            this._mainToolStrip.Size = new System.Drawing.Size(532, 41);
            this._mainToolStrip.TabIndex = 0;
            this._mainToolStrip.Text = "toolStrip1";
            // 
            // toolZoomIn
            // 
            this.toolZoomIn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolZoomIn.Image = ((System.Drawing.Image)(resources.GetObject("toolZoomIn.Image")));
            this.toolZoomIn.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolZoomIn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolZoomIn.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolZoomIn.Name = "toolZoomIn";
            this.toolZoomIn.Padding = new System.Windows.Forms.Padding(5);
            this.toolZoomIn.Size = new System.Drawing.Size(38, 39);
            this.toolZoomIn.Text = "Zoom In";
            // 
            // toolZoomOut
            // 
            this.toolZoomOut.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolZoomOut.Image = ((System.Drawing.Image)(resources.GetObject("toolZoomOut.Image")));
            this.toolZoomOut.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolZoomOut.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolZoomOut.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolZoomOut.Name = "toolZoomOut";
            this.toolZoomOut.Padding = new System.Windows.Forms.Padding(5);
            this.toolZoomOut.Size = new System.Drawing.Size(38, 39);
            this.toolZoomOut.Text = "Zoom Out";
            // 
            // toolZoomMax
            // 
            this.toolZoomMax.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolZoomMax.Image = ((System.Drawing.Image)(resources.GetObject("toolZoomMax.Image")));
            this.toolZoomMax.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolZoomMax.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolZoomMax.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolZoomMax.Name = "toolZoomMax";
            this.toolZoomMax.Padding = new System.Windows.Forms.Padding(5);
            this.toolZoomMax.Size = new System.Drawing.Size(38, 39);
            this.toolZoomMax.Text = "Zoom To Max Extents";
            // 
            // toolZoomToLayer
            // 
            this.toolZoomToLayer.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolZoomToLayer.Image = global::MWLite.GUI.Properties.Resources.zoom_layer;
            this.toolZoomToLayer.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolZoomToLayer.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolZoomToLayer.Name = "toolZoomToLayer";
            this.toolZoomToLayer.Padding = new System.Windows.Forms.Padding(5);
            this.toolZoomToLayer.Size = new System.Drawing.Size(38, 38);
            this.toolZoomToLayer.Text = "Zoom To Layer";
            // 
            // toolPan
            // 
            this.toolPan.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolPan.Image = global::MWLite.GUI.Properties.Resources.pan;
            this.toolPan.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolPan.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolPan.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolPan.Name = "toolPan";
            this.toolPan.Padding = new System.Windows.Forms.Padding(5);
            this.toolPan.Size = new System.Drawing.Size(38, 39);
            this.toolPan.Text = "Pan";
            // 
            // toolSearch
            // 
            this.toolSearch.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolSearch.Image = global::MWLite.GUI.Properties.Resources.search;
            this.toolSearch.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolSearch.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolSearch.Name = "toolSearch";
            this.toolSearch.Padding = new System.Windows.Forms.Padding(5);
            this.toolSearch.Size = new System.Drawing.Size(38, 38);
            this.toolSearch.Text = "Find location";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(6, 41);
            // 
            // toolIdentify
            // 
            this.toolIdentify.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolIdentify.Image = global::MWLite.GUI.Properties.Resources.attribute_show;
            this.toolIdentify.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolIdentify.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolIdentify.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolIdentify.Name = "toolIdentify";
            this.toolIdentify.Padding = new System.Windows.Forms.Padding(5);
            this.toolIdentify.Size = new System.Drawing.Size(38, 39);
            this.toolIdentify.Text = "Attributes";
            // 
            // toolMeasure
            // 
            this.toolMeasure.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolMeasure.Image = global::MWLite.GUI.Properties.Resources.measure;
            this.toolMeasure.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolMeasure.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolMeasure.Name = "toolMeasure";
            this.toolMeasure.Padding = new System.Windows.Forms.Padding(5);
            this.toolMeasure.Size = new System.Drawing.Size(38, 38);
            this.toolMeasure.Text = "Measure Distance";
            // 
            // toolMeasureArea
            // 
            this.toolMeasureArea.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolMeasureArea.Image = global::MWLite.GUI.Properties.Resources.area_measure;
            this.toolMeasureArea.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolMeasureArea.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolMeasureArea.Name = "toolMeasureArea";
            this.toolMeasureArea.Padding = new System.Windows.Forms.Padding(5);
            this.toolMeasureArea.Size = new System.Drawing.Size(38, 38);
            this.toolMeasureArea.Text = "Measure Area";
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 41);
            // 
            // toolSelection
            // 
            this.toolSelection.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolSelection.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolSelect,
            this.toolSelectByPolygon});
            this.toolSelection.Image = global::MWLite.GUI.Properties.Resources.select;
            this.toolSelection.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolSelection.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolSelection.Name = "toolSelection";
            this.toolSelection.Padding = new System.Windows.Forms.Padding(5);
            this.toolSelection.Size = new System.Drawing.Size(47, 38);
            this.toolSelection.Text = "Selection Tools";
            // 
            // toolSelect
            // 
            this.toolSelect.Name = "toolSelect";
            this.toolSelect.Size = new System.Drawing.Size(142, 22);
            this.toolSelect.Text = "By Rectangle";
            // 
            // toolSelectByPolygon
            // 
            this.toolSelectByPolygon.Name = "toolSelectByPolygon";
            this.toolSelectByPolygon.Size = new System.Drawing.Size(142, 22);
            this.toolSelectByPolygon.Text = "By Polygon";
            // 
            // toolZoomToSelected
            // 
            this.toolZoomToSelected.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolZoomToSelected.Image = global::MWLite.GUI.Properties.Resources.zoom_selection;
            this.toolZoomToSelected.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolZoomToSelected.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolZoomToSelected.Name = "toolZoomToSelected";
            this.toolZoomToSelected.Padding = new System.Windows.Forms.Padding(5);
            this.toolZoomToSelected.Size = new System.Drawing.Size(38, 38);
            this.toolZoomToSelected.Text = "Zoom To Selected";
            // 
            // toolClearSelection
            // 
            this.toolClearSelection.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolClearSelection.Image = global::MWLite.GUI.Properties.Resources.selected_delete;
            this.toolClearSelection.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolClearSelection.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolClearSelection.Name = "toolClearSelection";
            this.toolClearSelection.Padding = new System.Windows.Forms.Padding(5);
            this.toolClearSelection.Size = new System.Drawing.Size(38, 38);
            this.toolClearSelection.Text = "Clear Selection";
            // 
            // _menuStrip1
            // 
            this._menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuFile,
            this.mnuMap,
            this.mnuTiles});
            this._menuStrip1.Location = new System.Drawing.Point(0, 0);
            this._menuStrip1.Name = "_menuStrip1";
            this._menuStrip1.Size = new System.Drawing.Size(1010, 24);
            this._menuStrip1.TabIndex = 2;
            this._menuStrip1.Text = "_menuStrip1";
            // 
            // mnuFile
            // 
            this.mnuFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuCloseProject,
            this.toolStripSeparator5,
            this.mnuOpen,
            this.mnuAddVector,
            this.mnuAddRaster,
            this.toolStripSeparator7,
            this.mnuLoadProject,
            this.toolStripSeparator2,
            this.mnuSaveProject,
            this.mnuSaveProjectAs,
            this.toolStripSeparator6,
            this.mnuCloseApp});
            this.mnuFile.Name = "mnuFile";
            this.mnuFile.Size = new System.Drawing.Size(37, 20);
            this.mnuFile.Text = "File";
            // 
            // mnuCloseProject
            // 
            this.mnuCloseProject.Image = global::MWLite.GUI.Properties.Resources.map;
            this.mnuCloseProject.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuCloseProject.Name = "mnuCloseProject";
            this.mnuCloseProject.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
            this.mnuCloseProject.Size = new System.Drawing.Size(188, 30);
            this.mnuCloseProject.Text = "New Map";
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(185, 6);
            // 
            // mnuOpen
            // 
            this.mnuOpen.Image = global::MWLite.GUI.Properties.Resources.layer_add;
            this.mnuOpen.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuOpen.Name = "mnuOpen";
            this.mnuOpen.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.mnuOpen.Size = new System.Drawing.Size(188, 30);
            this.mnuOpen.Text = "Open";
            // 
            // mnuAddVector
            // 
            this.mnuAddVector.Image = global::MWLite.GUI.Properties.Resources.layer_vector_add;
            this.mnuAddVector.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuAddVector.Name = "mnuAddVector";
            this.mnuAddVector.Size = new System.Drawing.Size(188, 30);
            this.mnuAddVector.Text = "Open Vector";
            // 
            // mnuAddRaster
            // 
            this.mnuAddRaster.Image = global::MWLite.GUI.Properties.Resources.layer_raster_add;
            this.mnuAddRaster.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuAddRaster.Name = "mnuAddRaster";
            this.mnuAddRaster.Size = new System.Drawing.Size(188, 30);
            this.mnuAddRaster.Text = "Open Raster";
            // 
            // toolStripSeparator7
            // 
            this.toolStripSeparator7.Name = "toolStripSeparator7";
            this.toolStripSeparator7.Size = new System.Drawing.Size(185, 6);
            // 
            // mnuLoadProject
            // 
            this.mnuLoadProject.Image = global::MWLite.GUI.Properties.Resources.folder1;
            this.mnuLoadProject.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuLoadProject.Name = "mnuLoadProject";
            this.mnuLoadProject.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.L)));
            this.mnuLoadProject.Size = new System.Drawing.Size(188, 30);
            this.mnuLoadProject.Text = "Load Project";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(185, 6);
            // 
            // mnuSaveProject
            // 
            this.mnuSaveProject.Image = global::MWLite.GUI.Properties.Resources.save;
            this.mnuSaveProject.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuSaveProject.Name = "mnuSaveProject";
            this.mnuSaveProject.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.mnuSaveProject.Size = new System.Drawing.Size(188, 30);
            this.mnuSaveProject.Text = "Save Project";
            // 
            // mnuSaveProjectAs
            // 
            this.mnuSaveProjectAs.Image = global::MWLite.GUI.Properties.Resources.save_as;
            this.mnuSaveProjectAs.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuSaveProjectAs.Name = "mnuSaveProjectAs";
            this.mnuSaveProjectAs.Size = new System.Drawing.Size(188, 30);
            this.mnuSaveProjectAs.Text = "Save Project As...";
            // 
            // toolStripSeparator6
            // 
            this.toolStripSeparator6.Name = "toolStripSeparator6";
            this.toolStripSeparator6.Size = new System.Drawing.Size(185, 6);
            // 
            // mnuCloseApp
            // 
            this.mnuCloseApp.Image = global::MWLite.GUI.Properties.Resources.quit;
            this.mnuCloseApp.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuCloseApp.Name = "mnuCloseApp";
            this.mnuCloseApp.Size = new System.Drawing.Size(188, 30);
            this.mnuCloseApp.Text = "Close";
            // 
            // mnuMap
            // 
            this.mnuMap.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuSetProjection,
            this.mnuSearch,
            this.toolStripSeparator9,
            this.mnuSnapshot});
            this.mnuMap.Name = "mnuMap";
            this.mnuMap.Size = new System.Drawing.Size(43, 20);
            this.mnuMap.Text = "Map";
            // 
            // mnuSetProjection
            // 
            this.mnuSetProjection.Image = global::MWLite.GUI.Properties.Resources.crs_change;
            this.mnuSetProjection.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuSetProjection.Name = "mnuSetProjection";
            this.mnuSetProjection.Size = new System.Drawing.Size(160, 30);
            this.mnuSetProjection.Text = "Set Projection";
            // 
            // mnuSearch
            // 
            this.mnuSearch.Image = global::MWLite.GUI.Properties.Resources.search;
            this.mnuSearch.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuSearch.Name = "mnuSearch";
            this.mnuSearch.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.F)));
            this.mnuSearch.Size = new System.Drawing.Size(160, 30);
            this.mnuSearch.Text = "Search";
            // 
            // toolStripSeparator9
            // 
            this.toolStripSeparator9.Name = "toolStripSeparator9";
            this.toolStripSeparator9.Size = new System.Drawing.Size(157, 6);
            // 
            // mnuSnapshot
            // 
            this.mnuSnapshot.Image = global::MWLite.GUI.Properties.Resources.Snapshot;
            this.mnuSnapshot.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.mnuSnapshot.Name = "mnuSnapshot";
            this.mnuSnapshot.Size = new System.Drawing.Size(160, 30);
            this.mnuSnapshot.Text = "Snapshot";
            // 
            // mnuTiles
            // 
            this.mnuTiles.Name = "mnuTiles";
            this.mnuTiles.Size = new System.Drawing.Size(43, 20);
            this.mnuTiles.Text = "Tiles";
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusSelectedCount,
            this.lblProgressMessage,
            this.progressBar1});
            this.statusStrip1.Location = new System.Drawing.Point(0, 592);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(1010, 22);
            this.statusStrip1.TabIndex = 3;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // statusSelectedCount
            // 
            this.statusSelectedCount.Name = "statusSelectedCount";
            this.statusSelectedCount.Size = new System.Drawing.Size(995, 17);
            this.statusSelectedCount.Spring = true;
            this.statusSelectedCount.Text = "Selected: 0";
            this.statusSelectedCount.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblProgressMessage
            // 
            this.lblProgressMessage.Name = "lblProgressMessage";
            this.lblProgressMessage.Size = new System.Drawing.Size(0, 17);
            // 
            // progressBar1
            // 
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(100, 16);
            this.progressBar1.Visible = false;
            // 
            // toolStripPanel1
            // 
            this.toolStripPanel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.toolStripPanel1.Location = new System.Drawing.Point(0, 24);
            this.toolStripPanel1.MaximumSize = new System.Drawing.Size(0, 40);
            this.toolStripPanel1.Name = "toolStripPanel1";
            this.toolStripPanel1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.toolStripPanel1.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.toolStripPanel1.Size = new System.Drawing.Size(1010, 0);
            // 
            // toolStripContainer1
            // 
            // 
            // toolStripContainer1.ContentPanel
            // 
            this.toolStripContainer1.ContentPanel.Controls.Add(this.dockPanel1);
            this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(971, 527);
            this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            // 
            // toolStripContainer1.LeftToolStripPanel
            // 
            this.toolStripContainer1.LeftToolStripPanel.Controls.Add(this._toolStripLayer);
            this.toolStripContainer1.Location = new System.Drawing.Point(0, 24);
            this.toolStripContainer1.Name = "toolStripContainer1";
            this.toolStripContainer1.Size = new System.Drawing.Size(1010, 568);
            this.toolStripContainer1.TabIndex = 5;
            this.toolStripContainer1.Text = "toolStripContainer1";
            // 
            // toolStripContainer1.TopToolStripPanel
            // 
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this._mainToolStrip);
            // 
            // dockPanel1
            // 
            this.dockPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dockPanel1.DockBackColor = System.Drawing.SystemColors.Control;
            this.dockPanel1.DocumentStyle = WeifenLuo.WinFormsUI.Docking.DocumentStyle.DockingSdi;
            this.dockPanel1.Location = new System.Drawing.Point(0, 0);
            this.dockPanel1.Name = "dockPanel1";
            this.dockPanel1.Size = new System.Drawing.Size(971, 527);
            dockPanelGradient4.EndColor = System.Drawing.SystemColors.ControlLight;
            dockPanelGradient4.StartColor = System.Drawing.SystemColors.ControlLight;
            autoHideStripSkin2.DockStripGradient = dockPanelGradient4;
            tabGradient8.EndColor = System.Drawing.SystemColors.Control;
            tabGradient8.StartColor = System.Drawing.SystemColors.Control;
            tabGradient8.TextColor = System.Drawing.SystemColors.ControlDarkDark;
            autoHideStripSkin2.TabGradient = tabGradient8;
            autoHideStripSkin2.TextFont = new System.Drawing.Font("Segoe UI", 9F);
            dockPanelSkin2.AutoHideStripSkin = autoHideStripSkin2;
            tabGradient9.EndColor = System.Drawing.SystemColors.ControlLightLight;
            tabGradient9.StartColor = System.Drawing.SystemColors.ControlLightLight;
            tabGradient9.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripGradient2.ActiveTabGradient = tabGradient9;
            dockPanelGradient5.EndColor = System.Drawing.SystemColors.Control;
            dockPanelGradient5.StartColor = System.Drawing.SystemColors.Control;
            dockPaneStripGradient2.DockStripGradient = dockPanelGradient5;
            tabGradient10.EndColor = System.Drawing.SystemColors.ControlLight;
            tabGradient10.StartColor = System.Drawing.SystemColors.ControlLight;
            tabGradient10.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripGradient2.InactiveTabGradient = tabGradient10;
            dockPaneStripSkin2.DocumentGradient = dockPaneStripGradient2;
            dockPaneStripSkin2.TextFont = new System.Drawing.Font("Segoe UI", 9F);
            tabGradient11.EndColor = System.Drawing.SystemColors.ActiveCaption;
            tabGradient11.LinearGradientMode = System.Drawing.Drawing2D.LinearGradientMode.Vertical;
            tabGradient11.StartColor = System.Drawing.SystemColors.GradientActiveCaption;
            tabGradient11.TextColor = System.Drawing.SystemColors.ActiveCaptionText;
            dockPaneStripToolWindowGradient2.ActiveCaptionGradient = tabGradient11;
            tabGradient12.EndColor = System.Drawing.SystemColors.Control;
            tabGradient12.StartColor = System.Drawing.SystemColors.Control;
            tabGradient12.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripToolWindowGradient2.ActiveTabGradient = tabGradient12;
            dockPanelGradient6.EndColor = System.Drawing.SystemColors.ControlLight;
            dockPanelGradient6.StartColor = System.Drawing.SystemColors.ControlLight;
            dockPaneStripToolWindowGradient2.DockStripGradient = dockPanelGradient6;
            tabGradient13.EndColor = System.Drawing.SystemColors.GradientInactiveCaption;
            tabGradient13.LinearGradientMode = System.Drawing.Drawing2D.LinearGradientMode.Vertical;
            tabGradient13.StartColor = System.Drawing.SystemColors.GradientInactiveCaption;
            tabGradient13.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripToolWindowGradient2.InactiveCaptionGradient = tabGradient13;
            tabGradient14.EndColor = System.Drawing.Color.Transparent;
            tabGradient14.StartColor = System.Drawing.Color.Transparent;
            tabGradient14.TextColor = System.Drawing.SystemColors.ControlDarkDark;
            dockPaneStripToolWindowGradient2.InactiveTabGradient = tabGradient14;
            dockPaneStripSkin2.ToolWindowGradient = dockPaneStripToolWindowGradient2;
            dockPanelSkin2.DockPaneStripSkin = dockPaneStripSkin2;
            this.dockPanel1.TabIndex = 0;
            // 
            // _toolStripLayer
            // 
            this._toolStripLayer.Dock = System.Windows.Forms.DockStyle.None;
            this._toolStripLayer.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolCloseProject,
            this.toolLoadProject,
            this.toolSaveProject,
            this.toolSaveProjectAs,
            this.toolStripSeparator8,
            this.toolOpen,
            this.toolAddVector,
            this.toolAddRaster,
            this.toolAddDatabase,
            this.toolStripSeparator1,
            this.toolCreateLayer,
            this.toolRemoveLayer});
            this._toolStripLayer.Location = new System.Drawing.Point(0, 3);
            this._toolStripLayer.Name = "_toolStripLayer";
            this._toolStripLayer.Size = new System.Drawing.Size(39, 431);
            this._toolStripLayer.TabIndex = 1;
            // 
            // toolCloseProject
            // 
            this.toolCloseProject.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolCloseProject.Image = global::MWLite.GUI.Properties.Resources.map;
            this.toolCloseProject.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolCloseProject.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolCloseProject.Name = "toolCloseProject";
            this.toolCloseProject.Padding = new System.Windows.Forms.Padding(5);
            this.toolCloseProject.Size = new System.Drawing.Size(37, 38);
            this.toolCloseProject.Text = "New Project";
            // 
            // toolLoadProject
            // 
            this.toolLoadProject.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolLoadProject.Image = global::MWLite.GUI.Properties.Resources.folder1;
            this.toolLoadProject.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolLoadProject.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolLoadProject.Name = "toolLoadProject";
            this.toolLoadProject.Padding = new System.Windows.Forms.Padding(5);
            this.toolLoadProject.Size = new System.Drawing.Size(37, 38);
            this.toolLoadProject.Text = "Load Project";
            // 
            // toolSaveProject
            // 
            this.toolSaveProject.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolSaveProject.Image = global::MWLite.GUI.Properties.Resources.save;
            this.toolSaveProject.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolSaveProject.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolSaveProject.Name = "toolSaveProject";
            this.toolSaveProject.Padding = new System.Windows.Forms.Padding(5);
            this.toolSaveProject.Size = new System.Drawing.Size(37, 38);
            this.toolSaveProject.Text = "Save Project";
            // 
            // toolSaveProjectAs
            // 
            this.toolSaveProjectAs.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolSaveProjectAs.Image = global::MWLite.GUI.Properties.Resources.save_as;
            this.toolSaveProjectAs.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolSaveProjectAs.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolSaveProjectAs.Name = "toolSaveProjectAs";
            this.toolSaveProjectAs.Padding = new System.Windows.Forms.Padding(5);
            this.toolSaveProjectAs.Size = new System.Drawing.Size(37, 38);
            this.toolSaveProjectAs.Text = "Save Project As...";
            // 
            // toolStripSeparator8
            // 
            this.toolStripSeparator8.Name = "toolStripSeparator8";
            this.toolStripSeparator8.Size = new System.Drawing.Size(37, 6);
            // 
            // toolOpen
            // 
            this.toolOpen.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolOpen.Image = global::MWLite.GUI.Properties.Resources.layer_add;
            this.toolOpen.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolOpen.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolOpen.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolOpen.Name = "toolOpen";
            this.toolOpen.Padding = new System.Windows.Forms.Padding(5);
            this.toolOpen.Size = new System.Drawing.Size(37, 38);
            this.toolOpen.Text = "Add Layer";
            // 
            // toolAddVector
            // 
            this.toolAddVector.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolAddVector.Image = global::MWLite.GUI.Properties.Resources.layer_vector_add;
            this.toolAddVector.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolAddVector.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolAddVector.Name = "toolAddVector";
            this.toolAddVector.Padding = new System.Windows.Forms.Padding(5);
            this.toolAddVector.Size = new System.Drawing.Size(37, 38);
            this.toolAddVector.Text = "Add Vector Layer";
            // 
            // toolAddRaster
            // 
            this.toolAddRaster.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolAddRaster.Image = global::MWLite.GUI.Properties.Resources.layer_raster_add;
            this.toolAddRaster.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolAddRaster.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolAddRaster.Name = "toolAddRaster";
            this.toolAddRaster.Padding = new System.Windows.Forms.Padding(5);
            this.toolAddRaster.Size = new System.Drawing.Size(37, 38);
            this.toolAddRaster.Text = "Add Raster Layer";
            // 
            // toolAddDatabase
            // 
            this.toolAddDatabase.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolAddDatabase.Image = global::MWLite.GUI.Properties.Resources.layer_db_add;
            this.toolAddDatabase.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolAddDatabase.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolAddDatabase.Name = "toolAddDatabase";
            this.toolAddDatabase.Padding = new System.Windows.Forms.Padding(5);
            this.toolAddDatabase.Size = new System.Drawing.Size(37, 38);
            this.toolAddDatabase.Text = "Add PostGIS Layer";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(37, 6);
            // 
            // toolCreateLayer
            // 
            this.toolCreateLayer.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolCreateLayer.Image = global::MWLite.GUI.Properties.Resources.layer_create;
            this.toolCreateLayer.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolCreateLayer.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolCreateLayer.Name = "toolCreateLayer";
            this.toolCreateLayer.Padding = new System.Windows.Forms.Padding(5);
            this.toolCreateLayer.Size = new System.Drawing.Size(37, 38);
            this.toolCreateLayer.Text = "Create Layer";
            // 
            // toolRemoveLayer
            // 
            this.toolRemoveLayer.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolRemoveLayer.Image = global::MWLite.GUI.Properties.Resources.layer_remove;
            this.toolRemoveLayer.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolRemoveLayer.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolRemoveLayer.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolRemoveLayer.Name = "toolRemoveLayer";
            this.toolRemoveLayer.Padding = new System.Windows.Forms.Padding(5);
            this.toolRemoveLayer.Size = new System.Drawing.Size(37, 38);
            this.toolRemoveLayer.Text = "Remove Layer";
            // 
            // toolStripSeparator10
            // 
            this.toolStripSeparator10.Name = "toolStripSeparator10";
            this.toolStripSeparator10.Size = new System.Drawing.Size(6, 41);
            // 
            // toolSetProjection
            // 
            this.toolSetProjection.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolSetProjection.Image = global::MWLite.GUI.Properties.Resources.crs_change;
            this.toolSetProjection.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolSetProjection.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolSetProjection.Name = "toolSetProjection";
            this.toolSetProjection.Padding = new System.Windows.Forms.Padding(5);
            this.toolSetProjection.Size = new System.Drawing.Size(38, 38);
            this.toolSetProjection.Text = "Set coordinate system and projection";
            // 
            // MainForm
            // 
            this.ClientSize = new System.Drawing.Size(1010, 614);
            this.Controls.Add(this.toolStripContainer1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.toolStripPanel1);
            this.Controls.Add(this._menuStrip1);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this._menuStrip1;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "MapWindow Lite";
            this._mainToolStrip.ResumeLayout(false);
            this._mainToolStrip.PerformLayout();
            this._menuStrip1.ResumeLayout(false);
            this._menuStrip1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.toolStripContainer1.ContentPanel.ResumeLayout(false);
            this.toolStripContainer1.LeftToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.LeftToolStripPanel.PerformLayout();
            this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.PerformLayout();
            this.toolStripContainer1.ResumeLayout(false);
            this.toolStripContainer1.PerformLayout();
            this._toolStripLayer.ResumeLayout(false);
            this._toolStripLayer.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private ToolStrip _mainToolStrip;
        private MenuStrip _menuStrip1;
        private ToolStripMenuItem mnuFile;
        private ToolStripMenuItem mnuOpen;
        private StatusStrip statusStrip1;
        private ToolStripButton toolZoomIn;
        private ToolStripButton toolZoomOut;
        private ToolStripButton toolZoomMax;
        private ToolStripButton toolPan;
        private ToolStripSeparator toolStripSeparator2;
        private ToolStripMenuItem mnuCloseProject;
        private ToolStripMenuItem mnuSaveProject;
        private ToolStripMenuItem mnuTiles;
        private ToolStripButton toolIdentify;
        private ToolStripSeparator toolStripSeparator4;
        private ToolStripPanel toolStripPanel1;
        private ToolStripContainer toolStripContainer1;
        private WeifenLuo.WinFormsUI.Docking.DockPanel dockPanel1;
        private ToolStripButton toolZoomToLayer;
        private ToolStripButton toolMeasureArea;
        private ToolStripButton toolMeasure;
        private ToolStripMenuItem mnuMap;
        private ToolStripMenuItem mnuSetProjection;
        private ToolStrip _toolStripLayer;
        private ToolStripButton toolOpen;
        private ToolStripButton toolRemoveLayer;
        private ToolStripButton toolAddVector;
        private ToolStripButton toolAddRaster;
        private ToolStripSeparator toolStripSeparator1;
        private ToolStripProgressBar progressBar1;
        private ToolStripStatusLabel lblProgressMessage;
        private ToolStripStatusLabel statusSelectedCount;
        private ToolStripButton toolCreateLayer;
        private ToolStripButton toolClearSelection;
        private ToolStripButton toolZoomToSelected;
        private ToolStripSeparator toolStripSeparator3;
        private ToolStripMenuItem mnuAddVector;
        private ToolStripMenuItem mnuAddRaster;
        private ToolStripMenuItem mnuSaveProjectAs;
        private ToolStripSeparator toolStripSeparator5;
        private ToolStripSeparator toolStripSeparator6;
        private ToolStripMenuItem mnuCloseApp;
        private ToolStripMenuItem mnuSearch;
        private ToolStripSeparator toolStripSeparator7;
        private ToolStripMenuItem mnuLoadProject;
        private ToolStripButton toolLoadProject;
        private ToolStripSeparator toolStripSeparator8;
        private ToolStripButton toolCloseProject;
        private ToolStripButton toolSaveProject;
        private ToolStripButton toolSaveProjectAs;
        private ToolStripDropDownButton toolSelection;
        private ToolStripMenuItem toolSelect;
        private ToolStripMenuItem toolSelectByPolygon;
        private ToolStripButton toolAddDatabase;
        private ToolStripSeparator toolStripSeparator9;
        private ToolStripMenuItem mnuSnapshot;
        private ToolStripButton toolSearch;
        private ToolStripSeparator toolStripSeparator10;
        private ToolStripButton toolSetProjection;
    }
}

