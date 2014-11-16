// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Program.cs" company="MapWindow Open Source GIS Community">
//   MapWindow Open Source GIS Community
// </copyright>
// <summary>
//   The main class
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace TestApplication
{
  using System;
  using System.Windows.Forms;

  /// <summary>The start class</summary>
  public static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    public static void Main()
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run(new Form1());
    }
  }
}
