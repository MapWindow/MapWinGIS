using System;
using System.Diagnostics;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;
using MWLite.Core.Exts;
using MWLite.Core.UI;
using MWLite.GUI.Events;
using MWLite.GUI.Helpers;
using WeifenLuo.WinFormsUI.Docking;

namespace MWLite.GUI.Forms.Dock
{
    public partial class MapDockForm : DockContent
    {
        public event EventHandler<SelectionChanged> SelectionChanged;

        protected virtual void OnSelectionChanged(SelectionChanged e)
        {
            EventHandler<SelectionChanged> handler = SelectionChanged;
            if (handler != null) handler(this, e);
        }

        public MapDockForm()
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
            axMap1.LoadMapState(filename, null);
            InitMap();
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
            axMap1.SelectBoxFinal += axMap1_SelectBoxFinal;
            axMap1.MouseDownEvent += axMap1_MouseDownEvent;
            axMap1.PreviewKeyDown += axMap1_PreviewKeyDown;
            axMap1.ShapeHighlighted += axMap1_ShapeHighlighted;
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

        private void axMap1_MouseDownEvent(object sender, _DMapEvents_MouseDownEvent e)
        {
            if (axMap1.CursorMode != tkCursorMode.cmSelection || e.button != 1)
                return;
            
            int layerHandle = App.Legend.SelectedLayer;
            if (layerHandle == -1)
            {
                MessageHelper.Info("No layer is selected.");
                return;
            }

            double x = 0.0;
            double y = 0.0;
            axMap1.PixelToProj(e.x, e.y, ref x, ref y);

            var sf = axMap1.get_Shapefile(layerHandle);
            if (sf == null)
            {
                MessageHelper.Info("Selection works for vector layers only.");
                return;
            }

            if (Control.ModifierKeys != Keys.Control)
                sf.SelectNone();
            
            object result = null;
            ShpfileType shpType = sf.ShapefileType;
            if (sf.ShapefileType == ShpfileType.SHP_POINT)   // consider all
            {
                int offset = axMap1.VerticalIconOffset(sf.DefaultDrawingOptions, shpType);

                var ext = new Extents();
                var val = axMap1.PixelToProj(offset);
                ext.SetBounds(x, y - val, 0.0, x, y - val, 0.0);

                sf.SelectShapes(ext, axMap1.PointTolerance(sf.DefaultDrawingOptions, shpType),
                                SelectMode.INTERSECTION, ref result);
            }
            else
            {
                var ext = new Extents();
                ext.SetBounds(x, y, 0.0, x, y, 0.0);
                var tol = shpType == ShpfileType.SHP_POLYLINE ? axMap1.PixelToProj(25) : 10.0;
                sf.SelectShapes(ext, tol, SelectMode.INTERSECTION, ref result);
            }
            var indices = result as int[];

            if (indices != null && indices.Length > 0)
            {
                foreach (var ind in indices)
                {
                    sf.ShapeSelected[ind] = !sf.ShapeSelected[ind];
                    break;
                }
            }
            else
            {
                sf.SelectNone();
            }

            OnSelectionChanged(new SelectionChanged() {LayerHandle = layerHandle, SelectionCount = sf.NumSelected});

            axMap1.Redraw();
        }

        private void axMap1_SelectBoxFinal(object sender, _DMapEvents_SelectBoxFinalEvent e)
        {
            if (axMap1.CursorMode != tkCursorMode.cmSelection)
                return;
            
            int layerHandle = App.Legend.SelectedLayer;
            if (layerHandle == -1)
            {
                MessageHelper.Info("No layer is selected.");
                return;
            }
            
            double left = 0.0;
            double top = 0.0;
            double bottom = 0.0;
            double right = 0.0;
            axMap1.PixelToProj(e.left, e.top, ref left, ref top);
            axMap1.PixelToProj(e.right, e.bottom, ref right, ref bottom);
            var ext = new Extents();
            ext.SetBounds(left, bottom, 0.0, right, top, 0.0);
            
            double x = 0.0, y = 0.0;
            axMap1.PixelToProj(e.left, e.top, ref x, ref y);
            var sf = axMap1.get_Shapefile(layerHandle);
            if (sf != null)
            {
                sf.SelectNone();

                object result = null;
                sf.SelectShapes(ext, 0.0, SelectMode.INTERSECTION, ref result);
                var indices = result as int[];
                if (indices != null)
                {
                    foreach (var ind in indices)
                    {
                        sf.ShapeSelected[ind] = true;
                    }
                }
                axMap1.Redraw();

                OnSelectionChanged(new SelectionChanged() { LayerHandle = layerHandle, SelectionCount = sf.NumSelected });
            }
            else
            {
                MessageHelper.Info("Selection works for vector layers only.");
            }
        }

        private void axMap1_MouseMoveEvent(object sender, _DMapEvents_MouseMoveEvent e)
        {
            if (!axMap1.Focused)
                axMap1.Focus();
        }

        public void axMap1_ShapeHighlighted(object sender, _DMapEvents_ShapeHighlightedEvent e)
        {
            if (Map.CursorMode != tkCursorMode.cmIdentify)
            {
                Debug.Print("Error: shape highlighted must be triggered with identify cursor only");
                return;
            }
            string s = Map.GetAttributes(e.layerHandle, e.shapeIndex);
            toolTip1.SetToolTip(Map, s);
        }
    }
}
