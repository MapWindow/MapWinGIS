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
  using System.Threading;
  using System.Windows.Forms;

  using MapWinGIS;

  /// <summary>Static class to hold the file formats methods</summary>
  internal static class Fileformats
  {
    /// <summary>
    /// Gets or sets Map.
    /// </summary>
    internal static AxMapWinGIS.AxMap Map { get; set; }

    /// <summary>Open the shapefile and load it as an layer.</summary>
    /// <param name="filename">
    /// The filename.
    /// </param>
    /// <param name="theForm">
    /// The form with the callback implementation.
    /// </param>
    /// <param name="clearLayers">
    /// Clear the layers.
    /// </param>
    /// <returns>
    /// The layer handle
    /// </returns>
    internal static int OpenShapefileAsLayer(string filename, Form1 theForm, bool clearLayers)
    {
      var hndl = -1;
      var sf = OpenShapefile(filename, theForm);

      if (sf != null)
      {
        if (clearLayers)
        {
          Map.RemoveAllLayers();
          theForm.Progress(string.Empty, 0, "Removed all layers from the map");
        }

        hndl = Map.AddLayer(sf, true);

        // Check if a symbology file is present:
        var symbFilename = filename + ".mwsymb";
        if (File.Exists(symbFilename))
        {
          var layerDesc = string.Empty;
          Map.LoadLayerOptions(hndl, string.Empty, ref layerDesc);
          theForm.Progress(string.Empty, 100, "Applying symbology");
        }

        theForm.Progress(string.Empty, 100, "The shapefile is of type " + sf.ShapefileType);
        theForm.Progress(string.Empty, 100, "Done opening " + Path.GetFileName(filename));
      }
      else
      {
        theForm.Error(string.Empty, "Cannot load shapefile");
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
    /// <param name="clearLayers">
    /// Clear the layers.
    /// </param>
    /// <returns>
    /// The layer handle
    /// </returns>
    internal static int OpenImageAsLayer(string filename, Form1 theForm, bool clearLayers)
    {
      if (!File.Exists(filename))
      {
        theForm.Error(string.Empty, "Cannot find the file: " + filename);
        return -1;
      }

      var hndl = -1;

      try
      {
        // Track GDAL Errors:
        var settings = new GlobalSettings();
        var img = new Image
          {
            GlobalCallback = theForm 
          };

        // Without overviews this decreases the performance too much:
        if (img.NumOverviews > 1)
        {
          theForm.Progress(string.Empty, 0, "Opening the image with high quality interpolation mode.");
          img.UpsamplingMode = tkInterpolationMode.imHighQualityBilinear;
          img.DownsamplingMode = tkInterpolationMode.imBilinear;
        }

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
          if (clearLayers)
          {
            Map.RemoveAllLayers();
            Application.DoEvents();
          }

          // Log characteristics:
          theForm.Progress(string.Empty, 0, "projection: " + img.GetProjection());
          theForm.Progress(string.Empty, 0, "Number bands: " + img.NoBands);
          theForm.Progress(string.Empty, 0, "Number overviews: " + img.NumOverviews);
          theForm.Progress(string.Empty, 0, "Use transparency?: " + img.UseTransparencyColor);
          theForm.Progress(string.Empty, 0, "Transparency Color: " + img.TransparencyColor);

          hndl = Map.AddLayer(img, true);

          theForm.Progress(string.Empty, 100, "Done opening " + Path.GetFileName(filename));
        }
      }
      catch (System.Runtime.InteropServices.SEHException sehException)
      {
        theForm.Error(string.Empty, "SEHException in OpenImageAsLayer: " + sehException.Message);
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
    /// <param name="clearLayers">
    /// Clear the layers.
    /// </param>
    /// <returns>
    /// The layer handle
    /// </returns>
    internal static int OpenGridAsLayer(string filename, Form1 theForm, bool clearLayers)
    {
      if (!File.Exists(filename))
      {
        theForm.Error(string.Empty, "Cannot find the file: " + filename);
        return -1;
      }

      var hndl = -1;

      try
      {
        var settings = new GlobalSettings();
        var grd = new Grid { GlobalCallback = theForm };

        settings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start opening " + Path.GetFileName(filename));
        if (!grd.Open(filename, GridDataType.UnknownDataType, false, GridFileType.UseExtension, theForm))
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
          if (clearLayers)
          {
            Map.RemoveAllLayers();
          }

          // Log projection:
          theForm.Progress(string.Empty, 0, "Projection: " + grd.Header.GeoProjection.ExportToProj4());
          theForm.Progress(string.Empty, 0, "Number of bands: " + grd.NoBands);

          hndl = Map.AddLayer(grd, true);
          theForm.Progress(string.Empty, 100, "Done opening " + Path.GetFileName(filename));
        }
      }
      catch (System.Runtime.InteropServices.SEHException sehException)
      {
        theForm.Error(string.Empty, "SEHException in OpenGridAsLayer: " + sehException.Message);
      }

      return hndl;
    }

    /// <summary>Open the filename as a shapefile</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="theForm">
    /// The the form.
    /// </param>
    /// <returns>The shapefile object</returns>
    internal static Shapefile OpenShapefile(string shapefilename, Form1 theForm)
    {
      if (!File.Exists(shapefilename))
      {
        theForm.Error(string.Empty, "Cannot find the file: " + shapefilename);
        return null;
      }

      var sf = new Shapefile { GlobalCallback = theForm };
      theForm.Progress(string.Empty, 0, "Start opening " + Path.GetFileName(shapefilename));
      if (!sf.Open(shapefilename, theForm))
      {
        var msg = string.Format("Error opening shapefile: {0}", sf.get_ErrorMsg(sf.LastErrorCode));
        System.Diagnostics.Debug.WriteLine(msg);
        theForm.Error(string.Empty, msg);
        return null;
      }

      // Log some characteristics:
      theForm.Progress(string.Empty, 0, "Number of shapes: " + sf.NumShapes);
      theForm.Progress(string.Empty, 0, "Number of fields: " + sf.NumFields);
      theForm.Progress(string.Empty, 0, "Type: " + sf.ShapefileType);
      theForm.Progress(string.Empty, 0, "Projection: " + sf.GeoProjection.ExportToProj4());
      theForm.Progress(string.Empty, 0, "Has spatial index: " + sf.HasSpatialIndex);

      return sf;
    }
  }
}
