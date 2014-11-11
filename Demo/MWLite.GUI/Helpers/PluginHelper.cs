using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MWLite.Core;

namespace MWLite.GUI.Helpers
{
    internal static class PluginHelper
    {
        public static void Init(IMapApp app)
        {
            ShapeEditor.Editor.Init(app);
        }
    }
}
