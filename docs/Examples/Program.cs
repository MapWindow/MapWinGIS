// ReSharper disable ArrangeAccessorOwnerBody
// ReSharper disable DelegateSubtraction
// ReSharper disable PossibleInvalidCastExceptionInForeachLoop
// ReSharper disable CheckNamespace

using MapWinGIS;
using System;
using System.Windows.Forms;

namespace Examples
{
    public static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        private static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            InitGlobalSettings();
            Application.Run(new StartForm());
        }

        private static void InitGlobalSettings()
        {
            var globalSettings = new GlobalSettings {ReprojectLayersOnAdding = true};
        }
    }

    
}
