using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using MapWinGIS;

namespace TestGrids
{
    public enum ProxyDisplayMode
    {
        Proxy = 0,
        DirectDrawing = 1,
    }
    
    /// <summary>
    /// Testing code for new grid implementation
    /// </summary>
    public static class GridHelper
    {
        private const int LAYER_POSITION = 0;
        private static AxMapWinGIS.AxMap axMap1;
        private static ICallback callback;

        internal static void Initialize(AxMapWinGIS.AxMap map, ICallback cback)
        {
            axMap1 = map;
            callback = cback;
        }

        public static int GetLayerHandle(this AxMapWinGIS.AxMap map)
        {
            return map.get_LayerHandle(LAYER_POSITION);
        }

        public static Image GetActiveLayer(this AxMapWinGIS.AxMap map)
        {
            int handle = map.GetLayerHandle();
            return map.get_Image(handle);
        }

        #region Tests

        /// <summary>
        /// Toggles between direct rendering and proxy rendering for an image which visualizes a grid.
        /// </summary>
        public static void UpdateProxyMode(Image img, bool proxyNeeded)
        {
            var grid = img.OpenAsGrid();
            var extents = axMap1.Extents as IExtents;
            if (extents != null) Debug.Print(extents.ToDebugString());
            grid.GlobalCallback = callback;
            axMap1.RemoveAllLayers();
            grid.PreferedDisplayMode = proxyNeeded ? tkGridProxyMode.gpmUseProxy : tkGridProxyMode.gpmNoProxy;
            axMap1.AddLayer(grid, true);
            axMap1.Extents = extents;
        }

        /// <summary>
        /// Opens specified grid layer using MapWinGIS global settings
        /// </summary>
        public static void OpenGridLayer(string filename)
        {
            axMap1.RemoveAllLayers();

            if (!File.Exists(filename))
            {
                MessageBox.Show("Failed to find grid: " + filename);
            }
            else
            {
                Grid grid = new Grid { GlobalCallback = callback };

                if (grid.Open(filename, GridDataType.UnknownDataType, false, GridFileType.UseExtension, null))
                {
                    int layerHandle = axMap1.AddLayer(grid, true);
                    if (layerHandle != -1)
                    {
                        var img = axMap1.get_Image(layerHandle);
                        if (img != null)
                        {
                            Debug.Print("Number of bands: " + img.NoBands);
                            Debug.Print("Allow external color scheme: " + img.AllowExternalColorScheme);
                            MessageBox.Show("Layer was added to the map");
                        }
                        
                    }
                    else
                    {
                        MessageBox.Show("Failed to load layer: " + axMap1.get_ErrorMsg(axMap1.LastErrorCode));
                    }
                }
                else
                {
                    MessageBox.Show("Failed to open grid");
                }

                //grid.Close();
            }
        }

        /// <summary>
        /// Removes proxy for specified image layer (must render grid directly)
        /// </summary>
        public static void RemoveGridProxyForLayer()
        {
            var img = axMap1.GetActiveLayer();
            if (img == null) return;

            var grid = img.OpenAsGrid();
            if (grid != null)
            {
                if (img.IsGridProxy)
                {
                    int handle = axMap1.get_LayerHandle(LAYER_POSITION);
                    axMap1.RemoveLayer(handle);
                    img.Close();
                }

                if (!grid.RemoveImageProxy())
                {
                    MessageBox.Show("Failed to remove image proxy");
                }
            }
        }

        /// <summary>
        /// Ensures that there is a proxy for grid, then loads it directly without using grid and checks 
        /// whether it was recognized as proxy.
        /// </summary>
        /// <param name="gridName"></param>
        public static void OpenProxyDirectly(string gridName)
        {
            // TODO: create proxy in code first
            var img = new Image();
            if (img.Open(gridName))
            {
                axMap1.AddLayer(img, true);
            }
            else
            {
                MessageBox.Show("Failed to open image: " + gridName);
            }
        }

        /// <summary>
        /// Saves map state with currently open grid proxy, then reloads and checks if grid was recognized correctly
        /// </summary>
        public static void ReloadMapStateWithGridProxy(string stateFilename)
        {
            var img = axMap1.GetActiveLayer();
            if (img == null || !img.IsGridProxy)
            {
                MessageBox.Show("Map must have one layer with grid proxy");
            }
            else
            {
                if (axMap1.SaveMapState(stateFilename, true, true))
                {
                    axMap1.RemoveAllLayers();
                    if (!axMap1.LoadMapState(stateFilename, null))
                    {
                        MessageBox.Show("Failed to load map state");
                    }
                    else
                    {
                        img = axMap1.GetActiveLayer();
                        if (img == null)
                        {
                            MessageBox.Show("Image layer wasn't reloaded");
                        }
                        else if (!img.IsGridProxy)
                        {
                            MessageBox.Show("Image is no longer a proxy");
                        }
                        else
                        {
                            MessageBox.Show("SUCCESS: Image was successfully reloaded as proxy");
                        }
                    }
                }
                else
                {
                    MessageBox.Show("Failed to save map state");
                }
            }
        }
        
        /// <summary>
        /// Rebuilds color scheme for image layer represented by grid. Grid may be opened directly or using proxy image.
        /// </summary>
        public static void RebuildGridWithNewColorScheme(Image img, PredefinedColorScheme colors, ColoringType coloringType, int bandIndex, bool allowExternalColorScheme)
        {
            // generating new scheme
            var grid = img.OpenAsGrid();
            grid.OpenBand(bandIndex);
            var scheme = grid.GenerateColorScheme(tkGridSchemeGeneration.gsgGradient, colors);
            scheme.ApplyColoringType(coloringType);

            if (img.IsGridProxy)
            {
                var extents = axMap1.Extents;

                // we need to rebuild the proxy layer completely
                axMap1.RemoveAllLayers();

                // in fact it will be removed in grid.CreateProxy, but let's do it explicitly
                if (!grid.RemoveImageProxy())
                {
                    MessageBox.Show("Failed to remove image proxy");
                }
                else
                {
                    grid.GlobalCallback = callback;

                    // first approach
                    grid.OpenBand(bandIndex);

                    // in fact map.AddLayer will make this call internally; but for understanding sake I add it here
                    var newProxy = grid.CreateImageProxy(scheme);
                    if (newProxy == null)
                    {
                        MessageBox.Show("Failed to create image proxy");
                        return;
                    }
                    grid.PreferedDisplayMode = tkGridProxyMode.gpmUseProxy;
                    axMap1.AddLayer(grid, true);
                }

                axMap1.Extents = extents;
            }
            else
            {
                // it's enough to set new color scheme
                img.ExternalColorSchemeBandIndex = bandIndex;
                img.ExternalColorScheme = scheme;
                img.AllowExternalColorScheme = allowExternalColorScheme ? tkUseFunctionality.useAlways : tkUseFunctionality.useNever;
                axMap1.Redraw();
            }
        }
        #endregion
    }
}
