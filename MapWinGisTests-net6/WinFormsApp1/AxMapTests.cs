using System.Diagnostics;
using MapWinGIS;

namespace WinFormsApp1;
public sealed partial class Form1
{
    public Version GetMapWinGisVersion()
    {
        var versionString = axMap1.VersionNumber;
        var ver = Version.Parse(versionString);
        LogProgress("MapWinGIS version: " + ver);    
        return ver;
    }

    public int OpenFile(string fileLocation)
    {
        if (axMap1 == null) throw new Exception("MapWinGIS.Map is not initialized");
        
        if (!File.Exists(fileLocation)) throw new FileNotFoundException(fileLocation);

        var layerHandle = axMap1.AddLayerFromFilename(fileLocation, tkFileOpenStrategy.fosAutoDetect, true);
        if (layerHandle == -1) throw new Exception($"Could not add file [{fileLocation}] to map");

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

    public void GlobalSettingsCompressOverviewsTest()
    {
        var gs = new GlobalSettings();
        //Read:
        var value = gs.CompressOverviews;
        LogProgress(value.ToString());
        // Loop:
        foreach (tkTiffCompression enumValue in Enum.GetValues(typeof(tkTiffCompression)))
        {
            Console.WriteLine(enumValue.ToString());
            // Change:
            gs.CompressOverviews = enumValue;
            // Check:
            LogProgress($@"Updated value is: {gs.CompressOverviews}");
            Debug.Assert(gs.CompressOverviews == enumValue);
        }
        // Reset:
        gs.CompressOverviews = value;
        LogProgress("Done with GlobalSettingsCompressOverviewsTest");
    }

    private void GdalRasterWarpTest()
    {
        GdalUtils gdalUtils = new();
        var input = Helpers.GetTestFilePath("float32_50m.tif");
        LogProgress(@"Input file: " + input);
        var output = Helpers.GetRandomFilePath("GdalWarp", ".vrt");
        var options = new[]
        {
            "-of", "vrt",
            "-overwrite"
        };

        var retVal = gdalUtils.GdalRasterWarp(input, output, options);
        Debug.Assert(retVal, "GdalRasterWarp failed: " + gdalUtils.ErrorMsg[gdalUtils.LastErrorCode] +
                            " Detailed error: " + gdalUtils.DetailedErrorMsg);
        // Check if output file exists:
        Debug.Assert(File.Exists(output), output + " doesn't exists");
        LogProgress("Done with GdalRasterWarpTest");
    }

    private void GdalRasterTranslateTest()
    {
        GdalUtils gdalUtils = new();
        var input = Helpers.GetTestFilePath("float32_50m.tif");
        LogProgress("Input file: " + input);
        var output = Helpers.GetRandomFilePath("ChangedResolution", ".tif");
        var options = new[]
        {
            "-ot", "Float32",
            "-tr", "0.2", "0.2",
            "-r", "average",
            "-projwin", "-180", "90", "180", "-90"
        };

        try
        {
            var retVal = gdalUtils.GdalRasterTranslate(input, output, options);
            Debug.Assert(retVal, "GdalRasterTranslate failed: " + gdalUtils.ErrorMsg[gdalUtils.LastErrorCode] + " Detailed error: " + gdalUtils.DetailedErrorMsg);
        }
        catch (Exception e)
        {
            Debug.Fail(e.Message, e.InnerException?.Message);
        }

        // Check if output file exists:
        Debug.Assert(File.Exists(output), output + " doesn't exists");
        LogProgress("Done with GdalRasterTranslateTest");
    }

    private void CreateSpatialIndexUnicodeTest()
    {
        // Create shapefile:
        var sfPolygon = Helpers.CreateTestPolygonShapefile();

        // Save shapefile:
        var sfFileLocation = Helpers.SaveSfToTempFile(sfPolygon, "Воздух");
        Console.WriteLine(sfFileLocation);
        Debug.Assert(File.Exists(sfFileLocation), "Shapefile wasn't saved");
        
        // Create index again:
        var retVal = sfPolygon.CreateSpatialIndex();
        Debug.Assert(retVal,"CreateSpatialIndex failed");
    }

    private void OpenShapefile()
    {
        var sfLocation = Helpers.GetTestFilePath("Issue-216.shp");
        var sf = Helpers.OpenShapefile(sfLocation, this);
        LogProgress(sf.GeoProjection.ExportToWktEx());
        sf.GeoProjection.TryAutoDetectEpsg(out var epgCode);
        LogProgress("Issue-216.shp has EPSG code: " + epgCode);
        LogProgress("Done with OpenShapefile");
    }
}
