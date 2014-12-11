// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Program.cs" company="MapWindow Open Source GIS">
//   MapWindow developers community - 2014
// </copyright>
// <summary>
//   The start class
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace TestApplication
{
    #region

    using System;
    using System.Windows.Forms;

    #endregion

    /// <summary>The start class</summary>
    public static class Program
    {
        #region Public Methods and Operators

        /// <summary>
        ///     The main entry point for the application.
        /// </summary>
        [STAThread]
        public static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }

        #endregion
    }
}