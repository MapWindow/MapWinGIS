using AxMapWinGIS;
using MWLite.Symbology.LegendControl;
using MapWinGIS;
using MWLite.Core;
using MWLite.GUI.Classes;
using MWLite.GUI.Helpers;
using MWLite.GUI.MapLegend;
using MWLite.GUI.Properties;
using MWLite.ShapeEditor;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace MWLite.GUI.Forms
{
    public partial class MainForm : DockContent, IMapApp
    {
        private const string WINDOW_TITLE = "MapWinGIS Demo";
        private readonly AppDispatcher _dispatcher = new AppDispatcher();
        private static MainForm _form = null;
        private MapForm _mapForm = null;
        private LegendDockForm _legendForm = null;
        private MapCallback _callback = null;

        private Nagscreen nagscreen;

        public MainForm()
        {
            InitializeComponent();

            _callback = new MapCallback(statusStrip1, progressBar1, lblProgressMessage);

            _form = this;

            Init();
        }
        
        #region Initialization

        private void Init()
        {
            //this.LoadNagscreen();

            //this.nagscreen.ProgressLabel.Text = @"Initialize docking ...";
            InitDockLayout();

            PluginHelper.Init(this);

            //ToolStripManager.LoadSettings(this);

            //this.nagscreen.ProgressLabel.Text = @"Initialize tiling ...";
            TilesHelper.Init(mnuTiles);

            //this.nagscreen.ProgressLabel.Text = @"Initialize legend ...";
            InitLegend();

            //this.nagscreen.ProgressLabel.Text = @"Initialize menus ...";
            this.InitMenus();

            //this.nagscreen.ProgressLabel.Text = @"Initialize screen ...";
            RefreshUI();

            var gs = new GlobalSettings();
            gs.ApplicationCallback = _callback;

            Shown += (s, e) => Map.Focus();

            FormClosing += MainForm_FormClosing;

            App.Project.ProjectChanged += (s, e) => RefreshUI();

            //this.nagscreen.ProgressLabel.Text = @"Loading last project ...";
            App.Project.Load(AppSettings.Instance.LastProject);
            //this.nagscreen.ProgressLabel.Text = @"Ready ...";
        }

        private void LoadNagscreen()
        {
            // TODO: Make this somehow async, because the webpage is shown after the MainForm is shown
            this.nagscreen = new Nagscreen { Owner = this };
            if (false) // Settings.Default.ShowNagScreen)
            {
                this.nagscreen.Show(this);
                Application.DoEvents();
            }
            else
            {
                // Still show for the ads, but hidden:
                this.nagscreen.Visible = false;
                var dummy = this.nagscreen.Handle; // forces the form Control to be created
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            string lastProject = App.Project.GetPath();

            if (!App.Map.ShapeEditor.SaveChanges())
            {
                e.Cancel = true;
                return;
            }

            if (!App.Project.TryClose())
            {
                e.Cancel = true;
                return;
            }
            
            AppSettings.Instance.LastProject = lastProject;
            AppSettings.Save();

            ToolStripManager.SaveSettings(this);
        }

        private void InitLegend()
        {
            Legend.Map = Map.GetOcx() as Map;
            Legend.AssignOrphanLayersToNewGroup(ProjectBase.DEFAULT_GROUP_NAME);
            Legend.LayerSelected += (handle) =>
            {
                RefreshUI();
                Map.Identifier.ActiveLayer = handle;
            };
        }

        private void InitMenus()
        {
            Dispatcher.InitMenu(mnuFile.DropDownItems);
            Dispatcher.InitMenu(mnuMap.DropDownItems);
            Dispatcher.InitMenu(_mainToolStrip.Items);
            Dispatcher.InitMenu(_toolStripLayer.Items);
        }

        private void InitDockLayout()
        {
            _legendForm = new LegendDockForm();
            _legendForm.Show(dockPanel1, DockState.DockLeft);

            _mapForm = new MapForm();
            _mapForm.Show(dockPanel1, DockState.Document);
            _mapForm.SelectionChanged += (s, e) => RefreshUI();
        }

        #endregion
       
        #region Properties

        public static MainForm Instance
        {
            get { return _form; }
        }

        internal AppDispatcher Dispatcher
        {
            get { return _dispatcher; }
        }

        public MapForm MapForm
        {
            get { return _mapForm; }
        }

        #endregion

        #region IMapApp interface

        public IProject Project
        {
            get { return App.Project; }
        }

        public void RunCommand(AppCommand command)
        {
            _dispatcher.Run(command);
        }

        public Legend Legend
        {
            get { return _legendForm.Legend; }
        }

        public AxMap Map
        {
            get { return _mapForm.Map; }
        }

        public void LoadMapState(string filename)
        {
            _mapForm.LoadMapState(filename);
        }

        public void AddMenu(ToolStripMenuItem menu)
        {
            MainMenuStrip.Items.Add(menu);
        }

        public void AddToolbar(ToolStrip toolbar)
        {
            toolStripContainer1.TopToolStripPanel.Join(toolbar, _mainToolStrip.Left + _mainToolStrip.Width, 0);
        }

        public void RefreshUI()
        {
            Text = WINDOW_TITLE;
            if (!App.Project.IsEmpty)
                 Text += " - " + App.Project.GetPath();
            
            toolZoomIn.Checked = Map.CursorMode == tkCursorMode.cmZoomIn;
            toolZoomOut.Checked = Map.CursorMode == tkCursorMode.cmZoomOut;
            toolPan.Checked = Map.CursorMode == tkCursorMode.cmPan;
            toolSelect.Checked = Map.CursorMode == tkCursorMode.cmSelection;
            toolSelectByPolygon.Checked = Map.CursorMode == tkCursorMode.cmSelectByPolygon;
            toolIdentify.Checked = Map.CursorMode == tkCursorMode.cmIdentify;

            bool distance = Map.Measuring.MeasuringType == tkMeasuringType.MeasureDistance;
            toolMeasure.Checked = Map.CursorMode == tkCursorMode.cmMeasure && distance;
            toolMeasureArea.Checked = Map.CursorMode == tkCursorMode.cmMeasure && !distance;

            if (Map.CursorMode != tkCursorMode.cmIdentify)
            {
                MapForm.HideTooltip();
            }

            bool hasShapefile = false;
            int layerHandle = App.Legend.SelectedLayer;
            bool hasLayer = layerHandle != -1;
            if (hasLayer)
            {
                var sf = App.Map.get_Shapefile(layerHandle);
                if (sf != null)
                {
                    statusSelectedCount.Text = string.Format("Shapes: {0}; selected: {1}", sf.NumShapes, sf.NumSelected);
                    toolClearSelection.Enabled = sf.NumSelected > 0;
                    toolZoomToSelected.Enabled = sf.NumSelected > 0;
                    hasShapefile = true;
                }
            }

            if (!hasShapefile)
            {
                statusSelectedCount.Text = "";
                toolClearSelection.Enabled = false;
                toolZoomToSelected.Enabled = false;
            }

            toolRemoveLayer.Enabled = hasLayer;
            Editor.RefreshUI();

            Map.Focus();
        }

        #endregion
    }
}
