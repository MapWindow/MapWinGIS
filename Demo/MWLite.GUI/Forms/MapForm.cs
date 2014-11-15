using System;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;
using MWLite.Core.Events;
using MWLite.Core.Exts;
using MWLite.ShapeEditor.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace MWLite.GUI.Forms
{
    public partial class MapForm : DockContent
    {
        public event EventHandler<SelectionChangedArgs> SelectionChanged;

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

            InitContextMenu();
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
            axMap1.GrabProjectionFromData = true;
            axMap1.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckStrict;
            axMap1.CursorMode = tkCursorMode.cmZoomIn;
            axMap1.SendSelectBoxFinal = true;
            axMap1.SendMouseDown = true;
            axMap1.SendMouseUp = true;
            axMap1.InertiaOnPanning = tkCustomState.csFalse;
            axMap1.ShowRedrawTime = true;
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
        }

        void axMap1_ShapeHighlighted(object sender, _DMapEvents_ShapeHighlightedEvent e)
        {
            if (Map.CursorMode == tkCursorMode.cmIdentify)
            {
                string s = Map.GetAttributes(e.layerHandle, e.shapeIndex);
                toolTip1.SetToolTip(Map, s);
                Application.DoEvents();
            }
        }

        void InitContextMenu()
        {
            MainForm.Instance.Dispatcher.InitMenu(contextMenuStrip1.Items);
            contextMenuStrip1.Opening += contextMenuStrip1_Opening;
        }

        void contextMenuStrip1_Opening(object sender, System.ComponentModel.CancelEventArgs e)
        {
            ctxHighlightShapes.Checked = axMap1.HotTracking;
        }

        void axMap1_MouseUpEvent(object sender, _DMapEvents_MouseUpEvent e)
        {
            if (e.button == 2)
            {
                if (axMap1.CursorMode == tkCursorMode.cmIdentify)
                {
                    contextMenuStrip1.Show(axMap1, e.x, e.y);
                }
            }
        }

        void axMap1_ShapeIdentified(object sender, _DMapEvents_ShapeIdentifiedEvent e)
        {
            var sf = axMap1.get_Shapefile(e.layerHandle);
            if (sf != null)
            {
                using (var form = new AttributesForm(sf, e.shapeIndex))
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
    }
}
