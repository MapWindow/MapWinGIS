using AxMapWinGIS;
using MapWinGIS;
using MWLite.Core.Events;
using MWLite.Core.Exts;
using MWLite.Core.UI;
using MWLite.GUI.Classes;
using MWLite.GUI.Controls;
using MWLite.ShapeEditor.Forms;
using System;
using System.Diagnostics;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace MWLite.GUI.Forms
{

    public partial class MapForm : DockContent
    {
        public event EventHandler<SelectionChangedArgs> SelectionChanged;
        private IdentifierContextMenu _identifierContextMenu = new IdentifierContextMenu();

        protected virtual void OnSelectionChanged(SelectionChangedArgs e)
        {
            EventHandler<SelectionChangedArgs> handler = SelectionChanged;
            if (handler != null) handler(this, e);
        }

        public MapForm()
        {
            InitializeComponent();

            RegisterEventHandlers();

            InitMap();
        }

        internal AxMap Map
        {
            get { return axMap1; }
        }

        public void LoadMapState(string filename)
        {
            axMap1.LockWindow(tkLockMode.lmLock);
            try
            {
                axMap1.LoadMapState(filename, null);
                InitMap();
            }
            finally
            {
                axMap1.LockWindow(tkLockMode.lmUnlock);
            }
        }

        private void InitMap()
        {
            //axMap1.Tiles.SetProxy("127.0.0.1", 8888);
            //axMap1.Tiles.SetProxyAuthentication("temp", "1234", "");
            axMap1.GrabProjectionFromData = true;
            axMap1.CursorMode = tkCursorMode.cmZoomIn;
            axMap1.SendSelectBoxFinal = true;
            axMap1.SendMouseDown = true;
            axMap1.SendMouseUp = true;
            axMap1.InertiaOnPanning = tkCustomState.csAuto;
            axMap1.ShowRedrawTime = false;
            Map.Identifier.IdentifierMode = tkIdentifierMode.imSingleLayer;
            Map.Identifier.HotTracking = true;
            Map.ShapeEditor.HighlightVertices = tkLayerSelection.lsNoLayer;
            Map.ShapeEditor.SnapBehavior = tkLayerSelection.lsNoLayer;
        }

        private void RegisterEventHandlers()
        {
            axMap1.ProjectionChanged += axMap1_ProjectionChanged;
            axMap1.MouseMoveEvent += axMap1_MouseMoveEvent;
            axMap1.PreviewKeyDown += axMap1_PreviewKeyDown;
            axMap1.SelectionChanged += axMap1_SelectionChanged;
            axMap1.ShapeIdentified += axMap1_ShapeIdentified;
            axMap1.MouseUpEvent += axMap1_MouseUpEvent;
            axMap1.ShapeHighlighted += axMap1_ShapeHighlighted;
            axMap1.SelectBoxFinal += axMap1_SelectBoxFinal;
            axMap1.LayerProjectionIsEmpty += axMap1_LayerProjectionIsEmpty;
            axMap1.ProjectionMismatch += axMap1_ProjectionMismatch;
            axMap1.LayerReprojected += axMap1_LayerReprojected;
            axMap1.LayerAdded += axMap1_LayerAdded;
            axMap1.GridOpened += axMap1_GridOpened;
            axMap1.LayerRemoved += axMap1_LayerRemoved;
            axMap1.BackgroundLoadingStarted += axMap1_BackgroundLoadingStarted;
            axMap1.BackgroundLoadingFinished += axMap1_BackgroundLoadingFinished;
        }

        void axMap1_GridOpened(object sender, _DMapEvents_GridOpenedEvent e)
        {
            Debug.Print("Grid is opened: " + e.gridFilename);
            Debug.Print("Using proxy: " + e.isUsingProxy);
            Debug.Print("BandIndex: " + e.bandIndex);
        }

        void axMap1_BackgroundLoadingFinished(object sender, _DMapEvents_BackgroundLoadingFinishedEvent e)
        {
            Debug.Print("Loading finished: {0}; TaskId: {1}; LayerHandle: {2} Features: {3}; Loaded: {4}", 
            DateTime.Now.TimeOfDay.ToString(), e.taskId, e.layerHandle, e.numFeatures, e.numLoaded);
        }

        void axMap1_BackgroundLoadingStarted(object sender, _DMapEvents_BackgroundLoadingStartedEvent e)
        {
            Debug.Print("Loading started: {0}; TaskId: {1}; LayerHandle: {2}", DateTime.Now.TimeOfDay.ToString(), e.taskId, e.layerHandle);
        }

        void axMap1_LayerRemoved(object sender, _DMapEvents_LayerRemovedEvent e)
        {
            Debug.Print("Layer removed.");
        }

        void axMap1_LayerAdded(object sender, _DMapEvents_LayerAddedEvent e)
        {
            Debug.Print("Layer added.");
        }

        void axMap1_LayerReprojected(object sender, _DMapEvents_LayerReprojectedEvent e)
        {
            if (!e.success)
            {
                string filename = axMap1.get_LayerFilename(e.layerHandle);
                MessageHelper.Warn("Failed to reproject the layer: " + filename);
            }
            else
            {
                Debug.WriteLine("Layer reprojected:" + e.success);    
            }
        }

        void axMap1_ProjectionMismatch(object sender, _DMapEvents_ProjectionMismatchEvent e)
        {
            var sf = axMap1.get_Shapefile(e.layerHandle);
            if (sf != null)
            {
                e.reproject = tkMwBoolean.blnTrue;
                e.cancelAdding = tkMwBoolean.blnFalse;
            }
            else
            {
                string filename = axMap1.get_LayerFilename(e.layerHandle);
                MessageHelper.Info("Layer projection doesn't match the projection of the map: " + filename);
            }
        }

        void axMap1_LayerProjectionIsEmpty(object sender, _DMapEvents_LayerProjectionIsEmptyEvent e)
        {
            Debug.Print("Layer without projection");
        }

        void axMap1_SelectBoxFinal(object sender, _DMapEvents_SelectBoxFinalEvent e)
        {
            if (axMap1.CursorMode == tkCursorMode.cmSelection)
            {
                MessageHelper.Info("No shapefile layer is selected.");
            }
        }

        void axMap1_ShapeHighlighted(object sender, _DMapEvents_ShapeHighlightedEvent e)
        {
            if (Map.CursorMode == tkCursorMode.cmIdentify && AppSettings.Instance.ShowTooltip)
            {
                string s = Map.GetAttributes(e.layerHandle, e.shapeIndex);
                toolTip1.SetToolTip(Map, s);
                Application.DoEvents();
            }
        }

        void axMap1_MouseUpEvent(object sender, _DMapEvents_MouseUpEvent e)
        {
            if (e.button == 2)
            {
                if (axMap1.CursorMode == tkCursorMode.cmIdentify)
                {
                    _identifierContextMenu.Menu.Show(axMap1, e.x, e.y);
                }
            }
        }

        void axMap1_ShapeIdentified(object sender, _DMapEvents_ShapeIdentifiedEvent e)
        {
            var sf = axMap1.get_Shapefile(e.layerHandle);
            if (sf != null)
            {
                using (var form = new AttributesForm(sf, e.shapeIndex, e.layerHandle))
                {
                    form.ShowDialog(MainForm.Instance);
                }
            }
        }

        void axMap1_SelectionChanged(object sender, _DMapEvents_SelectionChangedEvent e)
        {
            App.RefreshUI();
        }

        private void axMap1_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
             switch (e.KeyCode)
             {
                 case Keys.Left:
                 case Keys.Right:
                 case Keys.Up:
                 case Keys.Down:
                     e.IsInputKey = true;
                     return;
             }
        }

        private void axMap1_ProjectionChanged(object sender, System.EventArgs e)
        {
            var gp = axMap1.GeoProjection;
            lblProjection.Text = gp.IsEmpty ? "No projection" : "Projection: " + gp.ExportToProj4();
        }

        private void axMap1_MouseMoveEvent(object sender, _DMapEvents_MouseMoveEvent e)
        {
            if (!axMap1.Focused)
                axMap1.Focus();
        }

        public void HideTooltip()
        {
            toolTip1.SetToolTip(Map, "");
        }

        /// <summary>
        /// The file dropped of the map control
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void AxMap1FileDropped(object sender, _DMapEvents_FileDroppedEvent e)
        {
            Helpers.LayerHelper.AddLayer(this.axMap1.FileManager.Open(e.filename));
        }
    }
}
