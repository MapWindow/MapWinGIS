using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using AxMapWinGIS;
using MapWindow.Legend;
using MWLite.Core;
using MWLite.GUI.Classes;
using MWLite.GUI.Forms;

namespace MWLite.GUI
{
    public static class App
    {
        private static IProject _project = new Project();

        public static AxMap Map
        {
            get { return MainForm.Instance.Map;  }
        }

        public static MapWindow.Legend.Controls.Legend.Legend Legend
        {
            get { return MainForm.Instance.Legend; }
        }

        public static IProject Project
        {
            get { return _project; }
        }

        public static void RefreshUI()
        {
            MainForm.Instance.RefreshUI();
        }

        public static void LoadMapState(string filename)
        {
            MainForm.Instance.MapForm.LoadMapState(filename);
        }

    }
}
