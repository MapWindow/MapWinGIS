using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWindow.Legend;
using MapWindow.Legend.Controls.Legend;
using MapWinGIS;
using MWLite.Core;
using MWLite.ShapeEditor.Forms;
using MWLite.ShapeEditor.UI;

namespace MWLite.ShapeEditor
{
    internal static class App
    {
        private static IMapApp _app = null;
        private static EditorMenu _menu = new EditorMenu();
        private static EditorToolbar _toolbar = new EditorToolbar();
        private static EditorDispatcher _dispatcher = new EditorDispatcher();
        private static EditorContextMenu _contextMenu = new EditorContextMenu();
        private static VertexContextMenu _vertexContextMenu = new VertexContextMenu();

        static App()
        {
            _dispatcher.InitMenu(_toolbar.Toolbar.Items);
            _dispatcher.InitMenu(_contextMenu.Menu.Items);
            _dispatcher.InitMenu(_vertexContextMenu.Menu.Items);
            _dispatcher.InitMenu(_menu.Menu.DropDownItems);
        }

        internal static bool Init(IMapApp app)
        {
            _app = app;
            app.AddMenu(_menu.Menu);
            app.AddToolbar(_toolbar.Toolbar);
            MapEventHandlers.Init(App.Map);
            Editor.RefreshUI();
            return _app != null;
        }

        internal static AxMap Map
        {
            get { return _app.Map; }
        }

        internal static Legend Legend
        {
            get { return _app.Legend; }
        }

        internal static EditorDispatcher Dispatcher
        {
            get { return _dispatcher; }
        }

        internal static EditorToolbar Toolbar
        {
            get { return _toolbar; }
        }

        internal static ContextMenuStrip SelectionContextMenu
        {
            get { return _contextMenu.Menu; }
        }

        internal static ContextMenuStrip VertexContextMenu
        {
            get { return _vertexContextMenu.Menu;  }
        }

        internal static IMapApp Instance
        {
            get { return _app; }
        }

        internal static Form MainForm
        {
            get { return Instance as Form; }
        }

        internal static void RefreshUI()
        {
            _toolbar.RefreshUI();
        }

        internal static Shapefile SelectedShapefile
        {
            get
            {
                return Map.get_Shapefile(Legend.SelectedLayer);
            }
        }
    }
}
