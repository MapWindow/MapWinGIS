namespace TestApplication
{
  using System.Collections.Generic;
  using System.IO;
  using System.Linq;
  using System.Threading;
  using System.Windows.Forms;

  using MapWinGIS;

  internal static class FileManagerTests
  {
    private static bool error;

    /// <summary>
    /// Gets or sets Map.
    /// </summary>
    internal static AxMapWinGIS.AxMap MyAxMap { get; set; }

    private static void Write(string msg)
    {
      Form1.Instance.WriteMsg(msg);
    }

    private static void Error(string msg)
    {
      Form1.Instance.WriteError(msg);
      error = true;
    }

    /// <summary>Run the Filemanager Analyze Files test</summary>
    /// <param name="textfileLocation">
    /// The location of the text file.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    public static bool RunAnalyzeFilesTest(string textfileLocation, Form1 theForm)
    {
      var numErrors = 0;

      // Open text file:
      if (!File.Exists(textfileLocation))
      {
        throw new FileNotFoundException("Cannot find text file.", textfileLocation);
      }

      theForm.Progress(string.Empty, 0, "-----------------------The Filemanager Analyze Files tests have started.");

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);
      foreach (var line in lines)
      {
        if (!AnalyzeFiles(line, theForm))
        {
          numErrors++;
        }

        // Wait a second to show something:
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, string.Format("The Filemanager Analyze Files tests have finished, with {0} errors", numErrors));

      return numErrors == 0;
    }

    /// <summary>
    /// Analyzes raster file, displaying possible open strategies
    /// </summary>
    /// <param name="filename">
    /// The path.
    /// </param>
    /// <returns>
    /// True on success
    /// </returns>
    private static bool AnalyzeFiles(string filename, Form1 theForm)
    {
      error = false;

      // running check for all files with such extensions
      var manager = new FileManager();

      var count = 0;

      // getting list of extension for images and grids
      var img = new Image();
      var filter = img.CdlgFilter.Replace("*.", string.Empty);
      var dict = filter.Split(new[] { '|' }).ToDictionary(item => item);

      var grid = new Grid();
      filter = grid.CdlgFilter.Replace("*.", string.Empty);
      var dict2 = filter.Split(new[] { '|' }).ToDictionary(item => item);

      dict = dict.Keys.Union(dict2.Keys).ToDictionary(item => item);

      var notSupportedExtensions = new HashSet<string>();

      if (File.Exists(filename))
      {
        var ext = Path.GetExtension(filename).Substring(1).ToLower();

        if (dict.ContainsKey(ext))
        {
          Write(string.Format("{0}. Filename: {1}", count++, Path.GetFileName(filename)));
          Write(string.Format("Is supported: {0}", manager.get_IsSupported(filename)));
          Write(string.Format("Is RGB image: {0}", manager.get_IsRgbImage(filename)));
          Write(string.Format("Is grid: {0}", manager.get_IsGrid(filename)));
          Write(string.Format("DEFAULT OPEN STRATEGY: {0}", manager.get_OpenStrategy(filename)));
          Write(string.Format("Can open as RGB image: {0}", manager.get_CanOpenAs(filename, tkFileOpenStrategy.fosRgbImage)));
          Write(string.Format("Can open as direct grid: {0}", manager.get_CanOpenAs(filename, tkFileOpenStrategy.fosDirectGrid)));
          Write(string.Format("Can open as proxy grid: {0}", manager.get_CanOpenAs(filename, tkFileOpenStrategy.fosProxyForGrid)));
          Write(string.Format("------------------------------------------"));

          Write(string.Format(string.Empty));

          // TODO: try to open with these strategies
          var rst = manager.OpenRaster(filename, tkFileOpenStrategy.fosAutoDetect, theForm);
          if (rst != null)
          {
            MyAxMap.Clear();
            if (MyAxMap.AddLayer(rst, true) == -1)
            {
              Error("Cannot add the raster file to the map");
            }
          }
          else
          {
            Error("Cannot load the raster file");
          }
        }
        else
        {
          if (!notSupportedExtensions.Contains(ext))
          {
            notSupportedExtensions.Add(ext);
          }
        }
      }
      else
      {
        Error(filename + " does not exists.");
      }

      if (notSupportedExtensions.Any())
      {
        Write("The following extensions, are among common dialog filters:");
        foreach (var extension in notSupportedExtensions.ToList())
        {
          Write(extension);
        }
      }

      return !error;
    }

    /// <summary>
    /// Run the Filemanager Grid open test
    /// </summary>
    /// <param name="textfileLocation">
    /// The location of the text file.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>
    /// True on success
    /// </returns>
    public static bool RunGridOpenTest(string textfileLocation, Form1 theForm)
    {
      var numErrors = 0;

      // Open text file:
      if (!File.Exists(textfileLocation))
      {
        throw new FileNotFoundException("Cannot find text file.", textfileLocation);
      }

      theForm.Progress(string.Empty, 0, "-----------------------The Filemanager Grid open tests have started.");

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);
      foreach (var line in lines)
      {
        if (!GridOpenTest(line, theForm))
        {
          numErrors++;
        }

        // Wait a second to show something:
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, string.Format("The Filemanager Grid open tests have finished, with {0} errors", numErrors));

      return numErrors == 0;
    }

    /// <summary>
    /// Opens grid with different options and checks how the open strategy is chosen
    /// </summary>
    /// <param name="filename">
    /// The filename.
    /// </param>
    /// <param name="theForm">
    /// The Form.
    /// </param>
    /// <returns>
    /// True on success
    /// </returns>
    private static bool GridOpenTest(string filename, ICallback theForm)
    {
      error = false;
      if (!File.Exists(filename))
      {
        Error("Filename wasn't found: " + filename);
        return false;
      }

      var gs = new GlobalSettings
      {
        GridProxyFormat = tkGridProxyFormat.gpfTiffProxy, 
        MinOverviewWidth = 512, 
        RasterOverviewCreation = tkRasterOverviewCreation.rocYes
      };

      var fm = new FileManager { GlobalCallback = theForm };

      // first, let's check that overview creation/removal works correctly
      fm.ClearGdalOverviews(filename);
      var overviews = fm.get_HasGdalOverviews(filename);
      if (overviews)
      {
        Error("Failed to remove overviews.");
      }

      fm.BuildGdalOverviews(filename, null);
      overviews = fm.get_HasGdalOverviews(filename);
      if (!overviews)
      {
        Error("Failed to build overviews.");
      }

      fm.ClearGdalOverviews(filename);
      fm.RemoveProxyForGrid(filename);

      var hasValidProxyForGrid = fm.get_HasValidProxyForGrid(filename);
      Write("File has valid proxy for grid: " + hasValidProxyForGrid);

      Image img = null;
      for (var i = 0; i < 6; i++)
      {
        var strategy = tkFileOpenStrategy.fosAutoDetect;
        switch (i)
        {
          case 0:
            Write("1. AUTO DETECT BEHAVIOR. OVERVIEWS MUST BE BUILT.");
            strategy = tkFileOpenStrategy.fosAutoDetect;
            break;
          case 1:
            Write("2. EXPLICIT PROXY MODE. OVERVIEWS MUST BE IGNORED, PROXY CREATED.");
            strategy = tkFileOpenStrategy.fosProxyForGrid;
            break;
          case 2:
            Write("3. AUTODETECT MODE. OVERVIEWS REMOVED. PROXY MUST BE REUSED.");
            strategy = tkFileOpenStrategy.fosAutoDetect;
            fm.ClearGdalOverviews(filename);
            break;
          case 3:
            Write("4. EPLICIT DIRECT MODE; PROXY MUST BE IGNORED; OVERVIEWS CREATED.");
            strategy = tkFileOpenStrategy.fosDirectGrid;
            break;
          case 4:
            Write("5. OVERVIEWS CREATION IS OFF; BUT FILE SIZE IS TOO SMALL FOR PROXY.");
            strategy = tkFileOpenStrategy.fosAutoDetect;
            fm.RemoveProxyForGrid(filename);
            fm.ClearGdalOverviews(filename);
            gs.MaxDirectGridSizeMb = 100;
            gs.RasterOverviewCreation = tkRasterOverviewCreation.rocNo;
            break;
          case 5:
            Write("6. OVERVIEWS CREATION IS OFF; BUT FILE SIZE IS LARGE ENOUGH FOR PROXY.");
            strategy = tkFileOpenStrategy.fosAutoDetect;
            gs.MaxDirectGridSizeMb = 1;
            break;
        }

        Write("Gdal overviews: " + fm.get_HasGdalOverviews(filename));
        Write("Grid proxy: " + fm.get_HasValidProxyForGrid(filename));

        img = fm.OpenRaster(filename, strategy, theForm);
        if (img == null)
        {
          continue;
        }
        
        // Don't add the image to the map, because we're going to delete some helper files:
        img.Close();

        strategy = fm.LastOpenStrategy;
        overviews = fm.get_HasGdalOverviews(filename);
        hasValidProxyForGrid = fm.get_HasValidProxyForGrid(filename);
        Write("Last open strategy: " + strategy);
        Write("Gdal overviews: " + overviews);
        Write("Grid proxy: " + hasValidProxyForGrid);

        switch (i)
        {
          case 0:
            if (!overviews)
            {
              Error("Failed to build overviews.");
            }

            if (strategy != tkFileOpenStrategy.fosDirectGrid)
            {
              Error("Direct grid strategy was expected.");
            }

            break;
          case 1:
            if (!hasValidProxyForGrid)
            {
              Error("Failed to build proxy.");
            }

            if (strategy != tkFileOpenStrategy.fosProxyForGrid)
            {
              Error("Proxy strategy was expected.");
            }

            break;
          case 2:
            if (overviews)
            {
              Error("Failed to remove overviews.");
            }

            if (strategy != tkFileOpenStrategy.fosProxyForGrid)
            {
              Error("Proxy strategy was expected.");
            }

            break;
          case 3:
            if (strategy != tkFileOpenStrategy.fosDirectGrid)
            {
              Error("Direct grid strategy was expected.");
            }

            if (!overviews)
            {
              Error("Failed to build overviews.");
            }

            break;
          case 4:
            if (overviews)
            {
              Error("No overviews is expected.");
            }

            if (strategy != tkFileOpenStrategy.fosDirectGrid)
            {
              Error("Direct grid strategy was expected.");
            }

            break;
          case 5:
            if (!hasValidProxyForGrid)
            {
              Error("Failed to build proxy.");
            }

            if (strategy != tkFileOpenStrategy.fosProxyForGrid)
            {
              Error("Proxy strategy was expected.");
            }

            break;
        }
      }

      fm.ClearGdalOverviews(filename);
      fm.RemoveProxyForGrid(filename);

      overviews = fm.get_HasGdalOverviews(filename);
      hasValidProxyForGrid = fm.get_HasValidProxyForGrid(filename);

      Write("Gdal overviews: " + fm.get_HasGdalOverviews(filename));
      Write("Grid proxy: " + fm.get_HasValidProxyForGrid(filename));
      if (hasValidProxyForGrid)
      {
        Error("Failed to remove proxy.");
      }

      if (overviews)
      {
        Error("Failed to remove overviews.");
      }

      return !error;
    }
  }
}
