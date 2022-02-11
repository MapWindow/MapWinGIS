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

    public bool OpenFile(string fileLocation)
    {
        if (!File.Exists(fileLocation))
            throw new FileNotFoundException(fileLocation);
        var retVal = axMap1.AddLayerFromFilename(fileLocation, tkFileOpenStrategy.fosAutoDetect, true);

        return retVal > -1;
    }

    public int GetMapProjectionAsEpsgCode()
    {
        if (!axMap1.GeoProjection.TryAutoDetectEpsg(out var epsgCode))
        {
            throw new Exception("Cannot find EPSG code from " + axMap1.GeoProjection.ExportToWktEx());
        }

        return epsgCode;
    }
}
