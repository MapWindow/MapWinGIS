using MapWinGIS;

namespace WinFormsApp1;
public sealed partial class Form1
{
    public Version GetMapWinGisVersion()
    {
        var versionString = axMap1.VersionNumber;
        var ver = Version.Parse(versionString);
        return ver;
    }

    public int OpenFile(string fileLocation)
    {
        if (!File.Exists(fileLocation)) throw new FileNotFoundException(fileLocation);

        var layerHandle = axMap1.AddLayerFromFilename(fileLocation, tkFileOpenStrategy.fosAutoDetect, true);
        if (layerHandle == -1) throw new Exception("Could not add file name to map");

        return layerHandle;
    }

    public int GetMapProjectionAsEpsgCode()
    {
        if (!axMap1.GeoProjection.TryAutoDetectEpsg(out var epsgCode))
        {
            throw new Exception("Cannot find EPSG code from " + axMap1.GeoProjection.ExportToWktEx());
        }

        return epsgCode;
    }

    public Shapefile GetShapefileFromLayer(int layerHandle)
    {
        return axMap1.get_Shapefile(layerHandle);
    }

    public int AddShapefileToMap(IShapefile sf)
    {
        // Check:
        ArgumentNullException.ThrowIfNull(sf);

        var layerHandle = axMap1.AddLayer(sf, true);
        if (layerHandle == -1) throw new Exception("Could not add shapefile object to map");

        return layerHandle;
    }
}
