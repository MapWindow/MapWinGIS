// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Form1.cs" company="MapWindow Open Source GIS Community">
//   MapWindow Open Source GIS Community
// </copyright>
// <summary>
//   Form to test some of the functionality of MapWinGIS
// </summary>
// --------------------------------------------------------------------------------------------------------------------
namespace TestApplication
{
  using System;
  using System.Windows.Forms;

  using MapWinGIS;

  /// <summary>Defines the form</summary>
  public partial class Form1 : Form
  {
    /// <summary>
    /// Initializes a new instance of the <see cref="Form1"/> class.
    /// </summary>
    public Form1()
    {
      InitializeComponent();
    }

    /// <summary>Handle the dropped file</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void AxMap1FileDropped(object sender, AxMapWinGIS._DMapEvents_FileDroppedEvent e)
    {
      // Check extension:
      var ext = System.IO.Path.GetExtension(e.filename);
      if (ext == null)
      {
        return;
      }

      // Load shapefile
      if (ext == ".shp")
      {
        var sf = new Shapefile();
        if (!sf.Open(e.filename, null))
        {
          var msg = string.Format("Error opening shapefile: {0}", sf.get_ErrorMsg(sf.LastErrorCode));
          System.Diagnostics.Debug.WriteLine(msg);
          MessageBox.Show(msg);
        }
        else
        {
          axMap1.AddLayer(sf, true);
        }

        return;
      }
      
      // Track GDAL Errors:
      var settings = new GlobalSettings();

      // Load grid file
      var grd = new Grid();
      if (grd.CdlgFilter.Contains(ext))
      {
        settings.ResetGdalError();
        if (!grd.Open(e.filename, GridDataType.UnknownDataType, true, GridFileType.UseExtension, null))
        {
          var msg = string.Format(
            "Error opening grid: {0}{1}Gdal Error: {2}", 
            grd.get_ErrorMsg(grd.LastErrorCode), 
            Environment.NewLine, 
            settings.GdalLastErrorMsg);
          System.Diagnostics.Debug.WriteLine(msg);
          MessageBox.Show(msg);
        }
        else
        {
          axMap1.AddLayer(grd, true);
        }

        return;
      }

      // Load image file
      var img = new Image();
      if (img.CdlgFilter.Contains(ext))
      {
        settings.ResetGdalError();
        if (!img.Open(e.filename, ImageType.USE_FILE_EXTENSION, false, null))
        {
          var msg = string.Format(
            "Error opening image: {0}{1}Gdal Error: {2}",
            img.get_ErrorMsg(img.LastErrorCode),
            Environment.NewLine,
            settings.GdalLastErrorMsg);
          System.Diagnostics.Debug.WriteLine(msg);
          MessageBox.Show(msg);
        }
        else
        {
          axMap1.AddLayer(img, true);
        }

        return;
      }

      // Don't know how to handle the dropped file:
      return;
    }
  }
}
