// --------------------------------------------------------------------------------------------------------------------
// <copyright file="GeosTests.cs" company="MapWindow Open Source GIS Community">
//   MapWindow Open Source GIS Community
// </copyright>
// <summary>
//   Static class to hold the tests methods for the GEOS functions
// </summary>
// --------------------------------------------------------------------------------------------------------------------
namespace TestApplication
{
  using System;
  using System.IO;
  using System.Linq;
  using System.Threading;
  using System.Windows.Forms;

  using MapWinGIS;

  /// <summary>Static class to hold the tests methods</summary>
  internal static class GeosTests
  {
    /// <summary>
    /// Gets or sets Map.
    /// </summary>
    internal static AxMapWinGIS.AxMap MyAxMap { get; set; }

    /// <summary>Run the Buffer shapefile test</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static void RunBufferShapefileTest(string shapefilename, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Buffer shapefile test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return;
        }

        // Open the sf:
        // First check if the MWShapeID field is present:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return;
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start buffering " + Path.GetFileName(shapefilename));

        // Make the distance depending on the projection.
        var distance = 1000;
        if (sf.GeoProjection.IsGeographic)
        {
          distance = 1;
        }

        var bufferedSf = sf.BufferByDistance(distance, 16, false, false);

        // Do some checks:
        if (!Helper.CheckShapefile(sf, bufferedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        Helper.ColorShapes(ref bufferedSf, tkMapColor.LightBlue, tkMapColor.LightYellow, false);

        // Load the files:
        Fileformats.OpenShapefileAsLayer(shapefilename, theForm, true);
        bufferedSf.DefaultDrawingOptions.FillVisible = false;
        MyAxMap.AddLayer(bufferedSf, true);
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The Buffer shapefile test has finished.");
    }

    /// <summary>Run the Simplify shapefile test</summary>
    /// <param name="shapefilename">The shapefile name</param>
    /// <param name="theForm">The form</param>
    internal static void RunSimplifyShapefileTest(string shapefilename, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Simplify shapefile test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return;
        }
        
        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start simplifying " + Path.GetFileName(shapefilename));

        // Make the tolerance depending on the projection.
        var tolerance = 10d;
        if (sf.GeoProjection.IsGeographic)
        {
          tolerance = 1;
        }

        var simplifiedSf = sf.SimplifyLines(tolerance, false);

        // Do some checks:
        if (!Helper.CheckShapefile(sf, simplifiedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        // give the resulting lines a good width and color:
        var utils = new Utils { GlobalCallback = theForm };
        simplifiedSf.DefaultDrawingOptions.LineWidth = 2;
        simplifiedSf.DefaultDrawingOptions.LineColor = utils.ColorByName(tkMapColor.OrangeRed);
        simplifiedSf.DefaultDrawingOptions.LineStipple = tkDashStyle.dsSolid;

        // Load the files:
        MyAxMap.RemoveAllLayers();
        MyAxMap.AddLayer(simplifiedSf, true);
        Fileformats.OpenShapefileAsLayer(shapefilename, theForm, false);
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The Simplify shapefile test has finished.");
    }

    /// <summary>Run Aggregate shapefile test</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="fieldIndex">
    /// The field index.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static void RunDissolveShapefileTest(string shapefilename, int fieldIndex, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Dissolve shapefile test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return;
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start dissolving " + Path.GetFileName(shapefilename));

        var dissolvedSf = sf.Dissolve(fieldIndex, false);

        // Do some checks:))
        if (!Helper.CheckShapefile(sf, dissolvedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        /*
        // Save result:
        dissolvedSf.SaveAs(shapefilename.Replace(".shp", "-aggregate.shp"), theForm);
        theForm.Progress(string.Empty, 0, "The resulting file has been saved as " + aggregatedSf.Filename);
        */

        Helper.ColorShapes(ref dissolvedSf, tkMapColor.BlueViolet, tkMapColor.Wheat, true);

        // Load the files:
        MyAxMap.RemoveAllLayers();
        MyAxMap.AddLayer(dissolvedSf, true);

        theForm.Progress(
          string.Empty,
          100,
          string.Format(
            "The dissolved shapefile now has {0} shapes instead of {1} and has {2} rows",
            dissolvedSf.NumShapes,
            sf.NumShapes,
            dissolvedSf.Table.NumRows));
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The Dissolve shapefile test has finished");
    }

    /// <summary>Run the clip shapefile test</summary>
    /// <param name="shapefilename">The shapefile name</param>
    /// <param name="overlayFilename">The name of the overlay shapefile</param>
    /// <param name="theForm">The form</param>
    internal static void RunClipShapefileTest(string shapefilename, string overlayFilename, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Clip shapefile test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return;
        }

        if (!Helper.CheckShapefileLocation(overlayFilename, theForm))
        {
          return;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return;
        }

        var overlaySf = Fileformats.OpenShapefile(overlayFilename, theForm);
        if (overlaySf == null)
        {
          theForm.Error(string.Empty, "Opening overlay shapefile was unsuccessful");
          return;
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start clipping " + Path.GetFileName(shapefilename));

        var clippedSf = sf.Clip(false, overlaySf, false);
        
        // Do some checks:)
        if (!Helper.CheckShapefile(sf, clippedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        Helper.ColorShapes(ref clippedSf, tkMapColor.DarkRed, tkMapColor.LightSeaGreen, true);

        // Load the files:
        MyAxMap.RemoveAllLayers();
        MyAxMap.AddLayer(clippedSf, true);
        Fileformats.OpenShapefileAsLayer(overlayFilename, theForm, false);
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The clipping shapefile test has finished.");
    }

    /// <summary>Run the intersect shapefile test</summary>
    /// <param name="shapefilenameFirst">
    /// The shapefilename first.
    /// </param>
    /// <param name="shapefilenameSecond">
    /// The shapefilename second.
    /// </param>
    /// <param name="theForm">
    /// The the form.
    /// </param>
    internal static void RunIntersectionShapefileTest(string shapefilenameFirst, string shapefilenameSecond, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The get Intersection shapefile test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilenameFirst, theForm))
        {
          return;
        }

        if (!Helper.CheckShapefileLocation(shapefilenameSecond, theForm))
        {
          return;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilenameFirst, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return;
        }

        var overlaySf = Fileformats.OpenShapefile(shapefilenameSecond, theForm);
        if (overlaySf == null)
        {
          theForm.Error(string.Empty, "Opening overlay shapefile was unsuccessful");
          return;
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start intersecting " + Path.GetFileName(shapefilenameFirst));

        var intersectedSf = sf.GetIntersection(false, overlaySf, false, ShpfileType.SHP_NULLSHAPE, theForm);

        // Do some checks:))
        if (!Helper.CheckShapefile(sf, intersectedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        Helper.ColorShapes(ref intersectedSf, tkMapColor.BlueViolet, tkMapColor.DarkRed, true);

        // Save result:
        //intersectedSf.SaveAs(shapefilenameSecond.Replace(".shp", "-intersect.shp"), theForm);

        // Load the files:
        MyAxMap.RemoveAllLayers();
        MyAxMap.AddLayer(intersectedSf, true);
        MyAxMap.AddLayer(overlaySf, true);
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The get intersection shapefile test has finished.");
    }
  }

  /*
   * Methods to test:
   * sf.SelectByShapefile()
  */
}
