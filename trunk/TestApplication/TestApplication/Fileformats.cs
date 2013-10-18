// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Fileformats.cs" company="MapWindow Open Source GIS Community">
//   MapWindow Open Source GIS Community
// </copyright>
// <summary>
//   Static class to hold the file formats methods
// </summary>
// --------------------------------------------------------------------------------------------------------------------
namespace TestApplication
{
  using System;
  using System.IO;
  using System.Windows.Forms;

  using MapWinGIS;

  /// <summary>Static class to hold the file formats methods</summary>
  internal static class Fileformats
  {
    /// <summary>
    /// Gets or sets Map.
    /// </summary>
    internal static AxMapWinGIS.AxMap Map { get; set; }

    /// <summary>
    /// Open the shapefile and load it as an layer.
    /// </summary>
    /// <param name="filename">
    /// The filename.
    /// </param>
    /// <param name="theForm">
    /// The form with the callback implementation.
    /// </param>
    /// <returns>
    /// The layer handle
    /// </returns>
    internal static int OpenShapefileAsLayer(string filename, Form1 theForm)
    {
      if (!File.Exists(filename))
      {
        theForm.Error(string.Empty, "Cannot find the file: " + filename);
        return -1;
      }

      var sf = new Shapefile { GlobalCallback = theForm };
      var hndl = -1;
      theForm.Progress(string.Empty, 0, "Start opening " + Path.GetFileName(filename));
      if (!sf.Open(filename, theForm))
      {
        var msg = string.Format("Error opening shapefile: {0}", sf.get_ErrorMsg(sf.LastErrorCode));
        System.Diagnostics.Debug.WriteLine(msg);
        theForm.Error(string.Empty, msg);
      }
      else
      {
        Map.RemoveAllLayers();
        hndl = Map.AddLayer(sf, true);
        
        // Check if a symbology file is present:
        var symbFilename = filename + ".mwsymb";
        if (File.Exists(symbFilename))
        {
          var layerDesc = string.Empty;
          Map.LoadLayerOptions(hndl, string.Empty, ref layerDesc);
          theForm.Progress(string.Empty, 100, "Loading symbology");
        }

        theForm.Progress(string.Empty, 100, "Done opening " + Path.GetFileName(filename));
      }

      return hndl;
    }

    /// <summary>
    /// Open the image and load it as an layer.
    /// </summary>
    /// <param name="filename">
    /// The filename.
    /// </param>
    /// <param name="theForm">
    /// The form with the callback implementation.
    /// </param>
    /// <returns>
    /// The layer handle
    /// </returns>
    internal static int OpenImageAsLayer(string filename, Form1 theForm)
    {
      if (!File.Exists(filename))
      {
        theForm.Error(string.Empty, "Cannot find the file: " + filename);
        return -1;
      }

      // Track GDAL Errors:
      var settings = new GlobalSettings();
      var img = new Image { GlobalCallback = theForm };
      var hndl = -1;

      settings.ResetGdalError();
      theForm.Progress(string.Empty, 0, "Start opening " + Path.GetFileName(filename));
      if (!img.Open(filename, ImageType.USE_FILE_EXTENSION, false, null))
      {
        var msg = string.Format(
          "Error opening image: {0}{1}",
          img.get_ErrorMsg(img.LastErrorCode),
          Environment.NewLine);
        if (settings.GdalLastErrorMsg != string.Empty)
        {
          msg += "GDAL Error: " + settings.GdalLastErrorMsg;
        }

        System.Diagnostics.Debug.WriteLine(msg);
        theForm.Error(string.Empty, msg);
      }
      else
      {
        Map.RemoveAllLayers();
        hndl = Map.AddLayer(img, true);
        theForm.Progress(string.Empty, 100, "Done opening " + Path.GetFileName(filename));
      }

      return hndl;
    }

    /// <summary>
    /// Open the grid and load it as an layer.
    /// </summary>
    /// <param name="filename">
    /// The filename.
    /// </param>
    /// <param name="theForm">
    /// The form with the callback implementation.
    /// </param>
    /// <returns>
    /// The layer handle
    /// </returns>
    internal static int OpenGridAsLayer(string filename, Form1 theForm)
    {
      if (!File.Exists(filename))
      {
        theForm.Error(string.Empty, "Cannot find the file: " + filename);
        return -1;
      }

      var settings = new GlobalSettings();
      var grd = new Grid { GlobalCallback = theForm };
      var hndl = -1;

      settings.ResetGdalError();
      theForm.Progress(string.Empty, 0, "Start opening " + Path.GetFileName(filename));
      if (!grd.Open(filename, GridDataType.UnknownDataType, true, GridFileType.UseExtension, null))
      {
        var msg = string.Format(
          "Error opening grid: {0}{1}",
          grd.get_ErrorMsg(grd.LastErrorCode),
          Environment.NewLine);
        if (settings.GdalLastErrorMsg != string.Empty)
        {
          msg += "GDAL Error: " + settings.GdalLastErrorMsg;
        }

        System.Diagnostics.Debug.WriteLine(msg);
        theForm.Error(string.Empty, msg);
      }
      else
      {
        Map.RemoveAllLayers();
        hndl = Map.AddLayer(grd, true);
        theForm.Progress(string.Empty, 100, "Done opening " + Path.GetFileName(filename));
      }

      return hndl;
    }
  }
}
