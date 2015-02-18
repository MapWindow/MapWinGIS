using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using AxMapWinGIS;
using MWLite.Symbology;
using MWLite.Core;
using MWLite.ShapeEditor.Forms;
using MWLite.ShapeEditor.Helpers;

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

        public static bool StopLayerEditing(int layerHandle)
        {
            return EditorHelper.StopLayerEditing(layerHandle);
        }

        public static bool StopAllEditing()
        {
            return EditorHelper.StopAllEditing();
        }
    }
}
