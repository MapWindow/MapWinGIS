using System.Collections.Generic;
using System.IO;
using System.Linq;
using MapWinGIS;

namespace TestApplication
{
    public static class FileManagerTests
    {
        private static bool error = false;

        private static void Write(string msg)
        {
            Form1.Instance.WriteMsg(msg);
        }

        private static void Error(string msg)
        {
            Form1.Instance.WriteMsg(msg);
            error = true;
        }
        
        /// <summary>
        /// Analyzes raster files in particular folder, displaying possible open strategies
        /// </summary>
        public static bool AnalyeFiles(string path)
        {
            Write(" ");
            Write("\nStart ANALYZE FILES TEST.");
            error = false;
            if (!Directory.Exists(path))
            {
                Write("ERROR: directory not found: " + path);
                return false;
            }

            // getting list of extension for images and grids
            var img = new Image();
            string filter = img.CdlgFilter.Replace("*.", "");
            var dict = filter.Split(new[] { '|' }).ToDictionary(item => item);

            var grid = new Grid();
            filter = grid.CdlgFilter.Replace("*.", "");
            var dict2 = filter.Split(new[] { '|' }).ToDictionary(item => item);

            dict = dict.Keys.Union(dict2.Keys).ToDictionary(item => item);

            var notSupportedExtensions = new HashSet<string>();

            // running check for all files with such extensions
            var manager = new FileManager();

            int count = 1;
            var names = Directory.GetFiles(path);
            foreach (var name in names)
            {
                string ext = Path.GetExtension(name).Substring(1).ToLower();
                if (dict.ContainsKey(ext))
                {
                    
                    Write(string.Format("{0}. Filename: {1}", count, Path.GetFileName(name)));
                    Write(string.Format("Is supported: {0}", manager.get_IsSupported(name)));
                    Write(string.Format("Is RGB image: {0}", manager.get_IsRgbImage(name)));
                    Write(string.Format("Is grid: {0}", manager.get_IsGrid(name)));
                    Write(string.Format("DEFAULT OPEN STRATEGY: {0}", manager.get_OpenStrategy(name)));
                    Write(string.Format("Can open as RGB image: {0}", manager.get_CanOpenAs(name, tkFileOpenStrategy.fosRgbImage).ToString()));
                    Write(string.Format("Can open as direct grid: {0}", manager.get_CanOpenAs(name, tkFileOpenStrategy.fosDirectGrid).ToString()));
                    Write(string.Format("Can open as proxy grid: {0}", manager.get_CanOpenAs(name, tkFileOpenStrategy.fosProxyForGrid).ToString()));
                    Write(string.Format("------------------------------------------"));

                    Write(string.Format(""));
                    // TODO: try to open with these strategies
                    count++;
                }
                else
                {
                    if (!notSupportedExtensions.Contains(ext))
                        notSupportedExtensions.Add(ext);
                }
            }

            if (notSupportedExtensions.Any())
            {
                Write("The following extensions, are among common dialog filters:");
                foreach (var ext in notSupportedExtensions.ToList())
                {
                    Write(ext);
                }
            }
            Write("TEST COMPLETED\n");
            return !error;
        }

        /// <summary>
        /// Opens grid with different options and checks how the open strategy is chosen
        /// </summary>
        public static bool GridOpenTest(string filename)
        {
            Write(" ");
            Write("Start GRID OPEN TEST.");
            error = false;
            if (!File.Exists(filename))
            {
                Error( "Filename wasn't found: " + filename);
                return false;
            }

            ICallback callback = null; //Form1.Instance;

            GlobalSettings gs = new GlobalSettings
                                {
                                    GridProxyFormat = tkGridProxyFormat.gpfTiffProxy,
                                    MinOverviewWidth = 512,
                                    RasterOverviewCreation = tkRasterOverviewCreation.rocYes
                                };

            FileManager fm = new FileManager();
            
            // first, let's check that overview creation/removal works correctly
            fm.ClearGdalOverviews(filename);
            bool overviews = fm.HasGdalOverviews[filename];
            if (overviews) Error( "Failed to remove overviews.");
            
            fm.BuildGdalOverviews(filename, null);
            overviews = fm.HasGdalOverviews[filename];
            if (!overviews) Error( "Failed to build overviews.");

            fm.ClearGdalOverviews(filename);
            fm.RemoveProxyForGrid(filename);

            bool proxy = fm.HasValidProxyForGrid[filename];

            Image img = null;
            for (int i = 0; i < 6; i++)
            {
                tkFileOpenStrategy strategy = tkFileOpenStrategy.fosAutoDetect;
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

                Write("Gdal overviews: " + fm.HasGdalOverviews[filename]);
                Write("Grid proxy: " + fm.HasValidProxyForGrid[filename]);

                img = fm.OpenRaster(filename, strategy, callback);
                if (img != null)
                {
                    img.Close();

                    strategy = fm.LastOpenStrategy;
                    overviews = fm.HasGdalOverviews[filename];
                    proxy = fm.HasValidProxyForGrid[filename];
                    Write("Last open strategy: " + strategy);
                    Write("Gdal overviews: " + overviews);
                    Write("Grid proxy: " + proxy);

                    switch (i)
                    {
                        case 0:
                            if (!overviews) Error( "Failed to build overviews.");
                            if (strategy != tkFileOpenStrategy.fosDirectGrid) Error( "Direct grid strategy was expected.");
                            break;
                        case 1:
                            if (!proxy) Error( "Failed to build proxy.");
                            if (strategy != tkFileOpenStrategy.fosProxyForGrid) Error( "Proxy strategy was expected.");
                            break;
                        case 2:
                            if (overviews) Error( "Failed to remove overviews.");
                            if (strategy != tkFileOpenStrategy.fosProxyForGrid) Error( "Proxy strategy was expected.");
                            break;
                        case 3:
                            if (strategy != tkFileOpenStrategy.fosDirectGrid) Error( "Direct grid strategy was expected.");
                            if (!overviews) Error( "Failed to build overviews.");
                            break;
                        case 4:
                            if (overviews) Error( "No overviews is expected.");
                            if (strategy != tkFileOpenStrategy.fosDirectGrid) Error( "Direct grid strategy was expected.");
                            break;
                        case 5:
                            if (!proxy) Error( "Failed to build proxy.");
                            if (strategy != tkFileOpenStrategy.fosProxyForGrid) Error( "Proxy strategy was expected.");
                            break;
                    }
                }
            }

            fm.ClearGdalOverviews(filename);
            fm.RemoveProxyForGrid(filename);

            overviews = fm.HasGdalOverviews[filename];
            proxy = fm.HasValidProxyForGrid[filename];

            Write("Gdal overviews: " + fm.HasGdalOverviews[filename]);
            Write("Grid proxy: " + fm.HasValidProxyForGrid[filename]);
            if (proxy) Error( "Failed to remove proxy.");
            if (overviews) Error( "Failed to remove overviews.");

            Write("TEST COMPLETED\n");

            return !error;
        }
    }
}
