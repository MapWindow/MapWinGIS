using MapWinGIS;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Examples
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            InitGlobalSettings();
            Application.Run(new StartForm());
        }

        static void InitGlobalSettings()
        {
            var gs = new GlobalSettings();
            gs.ReprojectLayersOnAdding = true;
        }
    }

    
}
