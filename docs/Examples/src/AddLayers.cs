using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Adds all the shapefiles and images with .tif and .png extentions from the specified folder to the map
        // </summary>
        public bool AddLayers(AxMap axMap1, string dataPath)
        {
            axMap1.RemoveAllLayers();
            axMap1.LockWindow(tkLockMode.lmLock);

            try
            {
                string[] files = Directory.GetFiles(dataPath);
                foreach (string file in files)
                {
                    int layerHandle = -1;
                    if (file.ToLower().EndsWith(".shp"))
                    {
                        Shapefile sf = new Shapefile();
                        if (sf.Open(file, null))
                        {
                            layerHandle = axMap1.AddLayer(sf, true);
                        }
                        else
                            MessageBox.Show(sf.ErrorMsg[sf.LastErrorCode]);
                    }
                    else if (file.ToLower().EndsWith(".tif") ||
                             file.ToLower().EndsWith(".png"))
                    {
                        Image img = new Image();
                        if (img.Open(file, ImageType.TIFF_FILE, false, null))
                        {
                            layerHandle = axMap1.AddLayer(img, true);
                        }
                        else
                            MessageBox.Show(img.ErrorMsg[img.LastErrorCode]);
                    }

                    if (layerHandle != -1)
                        axMap1.set_LayerName(layerHandle, Path.GetFileName(file));
                }
            }
            finally
            {
                axMap1.LockWindow(tkLockMode.lmUnlock);
                Debug.Print("Layers added to the map: " + axMap1.NumLayers);
            }
            return axMap1.NumLayers > 0;
        }
    }
}