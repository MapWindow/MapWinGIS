using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using AxMapWinGIS;
using MapWinGIS;
using MWLite.GUI.Forms;

namespace MWLite.GUI.Helpers
{
    internal static class MapHelper
    {
        public static void SetCursor(this AxMap map, tkCursorMode cursor)
        {
            map.CursorMode = cursor;
        }
    }
}
