using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWindow.Legend;
using MWLite.Core;
using MWLite.ShapeEditor.Forms;

namespace MWLite.ShapeEditor
{
    public static class Editor
    {
        public static bool Init(IMapApp app)
        {
            return App.Init(app);
        }

        public static void RefreshUI()
        {
            App.RefreshUI();
        }

        public static void RunCommand(EditorCommand command)
        {
            App.Dispatcher.Run(command);
        }
    }
}
