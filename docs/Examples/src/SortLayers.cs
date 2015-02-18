using System;
using System.Collections.Generic;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // This code sorts layers added to the map alphabetically.
        // </summary>
        public  void SortLayers(AxMap axMap1, string dataPath)
        {
            if (this.AddLayers(axMap1, dataPath))
            {
                this.LayersInfo();

                this.SortLayersByType();

                this.ShowLayerNames("Sorted by type: " + Environment.NewLine);

                this.SortAlphabetically();

                this.ShowLayerNames("Sorted alphabetically: " + Environment.NewLine);
            }
        }

        // <summary>
        // Sorts layers alphabetically
        // </summary>
        private void SortAlphabetically()
        {
            int[] handles = new int[axMap1.NumLayers];
            string[] names = new string[axMap1.NumLayers];

            for (int i = 0; i < axMap1.NumLayers; i++)
            {
                int layerHandle = axMap1.get_LayerHandle(i);
                handles[i] = layerHandle;
                names[i] = axMap1.get_LayerName(layerHandle);
            }

            Array.Sort(names, handles);

            axMap1.LockWindow(tkLockMode.lmLock);
            try
            {
                foreach (int handle in handles)
                {
                    int position = axMap1.get_LayerPosition(handle);
                    axMap1.MoveLayerBottom(position);
                }
            }
            finally
            {
                axMap1.LockWindow(tkLockMode.lmUnlock);
            }
        }

        // <summary>
        // Sorts shapefiles by type: polygons - polyline - points 
        // </summary>
        public void SortLayersByType()
        {
            for (int j = 0; j < 3; j++)
            {
                List<int> handles = new List<int>();

                for (int i = 0; i < axMap1.NumLayers; i++)
                {
                    int layerHandle = axMap1.get_LayerHandle(i);
                    Shapefile sf = axMap1.get_Shapefile(layerHandle);
                    if (sf != null)
                    {
                        if (j == 0 && sf.ShapefileType == ShpfileType.SHP_POINT)
                            handles.Add(layerHandle);

                        if (j == 1 && sf.ShapefileType == ShpfileType.SHP_POLYLINE)
                            handles.Add(layerHandle);

                        if (j == 2 && sf.ShapefileType == ShpfileType.SHP_POLYGON)
                            handles.Add(layerHandle);
                    }
                }

                axMap1.LockWindow(tkLockMode.lmLock);
                try
                {
                    foreach (int handle in handles)
                    {
                        int position = axMap1.get_LayerPosition(handle);
                        axMap1.MoveLayerBottom(position);
                    }
                }
                finally
                {
                    axMap1.LockWindow(tkLockMode.lmUnlock);
                }
            }
        }

        // <summary>
        // Displays names of the layers
        // </summary>
        private void ShowLayerNames(string description)
        {
            string message = "";
            for (int i = axMap1.NumLayers - 1; i >= 0 ; i--)
            {
                int layerHandle = axMap1.get_LayerHandle(i);
                string name = axMap1.get_LayerName(layerHandle);
                object layer = axMap1.get_GetObject(layerHandle);
                string type = string.Empty;
                if (layer as Shapefile != null)
                {
                    type = (layer as Shapefile).ShapefileType.ToString();
                }
                else{
                    type = "Image";
                }

                message += name + ": " + type + Environment.NewLine;
            }
            MessageBox.Show(description + message);
        }

        // <summary>
        // Reports the number of the layers by type added to the map.
        // </summary>
        public void LayersInfo()
        {
            int pointCount = 0;
            int lineCount = 0;
            int polyCount = 0;
            int imgCount = 0;

            for (int i = 0; i < axMap1.NumLayers; i++)
            {
                object layer = axMap1.get_GetObject(axMap1.get_LayerHandle(i));
                Shapefile sf = layer as Shapefile;
                if (sf != null)
                {
                    switch (sf.ShapefileType)
                    {
                        case ShpfileType.SHP_POINT:
                            pointCount++;
                            break;
                        case ShpfileType.SHP_POLYLINE:
                            lineCount++;
                            break;
                        case ShpfileType.SHP_POLYGON:
                            polyCount++;
                            break;
                    }
                }
                else
                {
                    Image img = layer as Image;
                    if (img != null)
                    {
                        imgCount++;
                    }
                }
            }

            string s = string.Format("Layers added to the map:" + Environment.NewLine +
                                     "Point shapefiles: {0}" + Environment.NewLine +
                                     "Polyline shapefiles: {0}" + Environment.NewLine +
                                     "Polygon shapefiles: {0}" + Environment.NewLine +
                                     "Images: {3}", pointCount, lineCount, polyCount, imgCount);
            MessageBox.Show(s);
        }
    }
}