using System.Reflection;

namespace unittest_net6;

internal static class Helpers
{
    #region GeoProjection
    internal static GeoProjection MakeProjection(int epsgCode)
    {
        var geoProjection = new GeoProjection();
        geoProjection.ShouldNotBeNull();
        geoProjection.ImportFromEPSG(epsgCode);
        geoProjection.Name.ShouldNotBeNull();
        return geoProjection;
    }

    internal static void CheckEpsgCode(IGeoProjection geoProjection, int epsgCodeToCheck, bool isGeographic = true)
    {
        geoProjection.ShouldNotBeNull();
        geoProjection.IsEmpty.ShouldBeFalse();
        if (isGeographic)
        {
            geoProjection.IsGeographic.ShouldBeTrue();
        }
        else
        {
            geoProjection.IsProjected.ShouldBeTrue();
        }
        var retVal = geoProjection.TryAutoDetectEpsg(out var epsgCode);
        retVal.ShouldBeTrue();
        epsgCode.ShouldBe(epsgCodeToCheck);
    }
    #endregion

    #region shapefile
    internal static MapWinGIS.Shapefile MakeShapefile(ShpfileType sfType)
    {
        var sf = new MapWinGIS.Shapefile();
        sf.ShouldNotBeNull("Could not initialize Shapefile object");
        var retVal = sf.CreateNewWithShapeID("", sfType);
        retVal.ShouldBeTrue("sf.CreateNewWithShapeID() failed");
        sf.ShapefileType.ShouldBe(sfType, "Shapefile type is unexpected");
        return sf;
    }

    internal static string SaveSfToTempFile(MapWinGIS.Shapefile sf, string filenamePart)
    {
        var baseFileName = Path.Combine(Path.GetTempPath(), Path.GetFileNameWithoutExtension(Path.GetRandomFileName()));
        var fileName = Path.ChangeExtension($"{baseFileName}{filenamePart}", ".shp");
        SaveSf(sf, fileName);
        return fileName;
    }

    internal static void SaveSf(MapWinGIS.Shapefile sf, string fileLocation)
    {
        sf.ShouldNotBeNull("sf is null");
        var retVal = sf.SaveAsEx(fileLocation, true, false);
        retVal.ShouldBeTrue("sf.SaveAs failed");
        sf.EditingShapes.ShouldBeFalse("sf is still in edit mode.");
    }

    internal static MapWinGIS.Shapefile OpenShapefile(string fileLocation)
    {
        File.Exists(fileLocation).ShouldBeTrue("Could not find shapefile to open");

        var sf = new MapWinGIS.Shapefile();
        var retVal = sf.Open(fileLocation);
        retVal.ShouldBeTrue("sf.Open failed");
        return sf;
    }

    internal static MapWinGIS.Shapefile CreateTestPolygonShapefile()
    {
        var sfPolygon = MakeShapefile(ShpfileType.SHP_POLYGON);
        // Add shape to shapefile:
        AddShape(sfPolygon, "POLYGON((330695.973322992 5914896.16305817, 330711.986129861 5914867.19586245, 330713.350435287 5914867.56644015, 330716.510827627 5914862.28973662, 330715.632568651 5914860.60107999, 330652.234582712 5914803.80510632, 330553.749382483 5914715.80328169, 330551.979355848 5914714.83347535, 330549.911988583 5914715.86502807, 330545.027807355 5914724.05916443, 330544.592985976 5914725.93531509, 330544.30963704 5914726.72754692, 330543.612620707 5914726.14904553, 330543.271515787 5914727.06633931, 330542.234090059 5914729.85597723, 330542.959654761 5914730.50411962, 330530.319252794 5914765.86064153, 330505.294840402 5914836.7930124, 330471.411812074 5914931.61558331, 330486.074748666 5914941.33795239, 330585.983154737 5915010.32749106, 330618.427962455 5915031.20447119, 330653.234601917 5914970.37328093, 330695.973322992 5914896.16305817))");
        // Check:
        sfPolygon.NumShapes.ShouldBe(1);

        // Set projection:
        sfPolygon.GeoProjection = MakeProjection(28992);

        return sfPolygon;
    }
    #endregion

    #region shape

    internal static Shape MakeShape(ShpfileType shpType)
    {
        var shp = new Shape();
        shp.ShouldNotBeNull("Could not create Shape");
        var retVal = shp.Create(shpType);
        retVal.ShouldBeTrue("shp.Create failed");
        return shp;
    }

    internal static void AddShape(MapWinGIS.Shapefile sf, string wktSting)
    {
        // Create shape
        var shp = new Shape();
        shp.ShouldNotBeNull("Could not create Shape");
        var retVal = shp.ImportFromWKT(wktSting);
        retVal.ShouldBeTrue("shp.ImportFromWKT() failed");

        // Add shape to shapefile:
        var newIndex = sf.EditAddShape(shp);
        newIndex.ShouldNotBe(-1);

        // Checks:
        shp.ShapeType.ShouldBe(sf.ShapefileType);
    }

    internal static Shape AddPointShape(MapWinGIS.Shapefile sf, double x, double y)
    {
        // Add shape:
        var shp = MakeShape(ShpfileType.SHP_POINT);
        // Add point:
        var pointIndex = shp.AddPoint(x, y);
        pointIndex.ShouldBe(0, "Invaldid point index");
        var shapeIndex = sf.EditAddShape(shp);
        shapeIndex.ShouldNotBe(-1, "EditAddShape failed");
        return shp;
    }
    #endregion


    internal static MapWinGIS.Image LoadImageUsingFileManager(string filename)
    {
        File.Exists(filename).ShouldBeTrue("Input file does not exist: " + filename);

        var fm = new FileManager();
        var obj = fm.Open(filename);
        fm.LastOpenIsSuccess.ShouldBeTrue(fm.ErrorMsg[fm.LastErrorCode]);
        var img = obj as MapWinGIS.Image;
        img.ShouldNotBeNull("Loaded object is not an image");
        return img;
    }

    internal static string GetTestDataLocation()
    {
        var pathAssembly = Assembly.GetExecutingAssembly().Location;
        var folderAssembly = Path.GetDirectoryName(pathAssembly);
        if (folderAssembly?.EndsWith(@"\") == false) folderAssembly += @"\";
        var folderProjectLevel = Path.GetFullPath(folderAssembly + @"..\..\..\..\..\TestData\");
        if (!Directory.Exists(folderProjectLevel))
            throw new DirectoryNotFoundException("Cannot find TestData folder at " + folderProjectLevel);

        return folderProjectLevel;
    }
}
