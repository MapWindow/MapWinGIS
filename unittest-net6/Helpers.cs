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
    #endregion

    #region shapefile
    internal static MapWinGIS.Shapefile MakeShapefile(ShpfileType sfType)
    {
        var sf = new MapWinGIS.Shapefile();
        sf.ShouldNotBeNull("Could not create Shapefile");
        var retVal = sf.CreateNewWithShapeID("", sfType);
        retVal.ShouldBeTrue("sf.CreateNewWithShapeID() failed");
        sf.ShapefileType.ShouldBe(sfType, "Shapefile type is unexpected");
        return sf;
    }

    internal static void SaveSf(MapWinGIS.Shapefile sf, string fileLocation)
    {
        sf.ShouldNotBeNull("sf is null");
        var retVal = sf.SaveAs(fileLocation);
        retVal.ShouldBeTrue("sf.SaveAs failed");
    }

    internal static MapWinGIS.Shapefile OpenShapefile(string fileLocation)
    {
        File.Exists(fileLocation).ShouldBeTrue("Could not find shapefile to open");

        var sf = new MapWinGIS.Shapefile();
        var retVal = sf.Open(fileLocation);
        retVal.ShouldBeTrue("sf.Open failed");
        return sf;
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


    internal static Image LoadImageUsingFileManager(string filename)
    {
        File.Exists(filename).ShouldBeTrue("Input file does not exist: " + filename);

        var fm = new FileManager();
        var obj = fm.Open(filename);
        fm.LastOpenIsSuccess.ShouldBeTrue(fm.ErrorMsg[fm.LastErrorCode]);
        var img = obj as Image;
        img.ShouldNotBeNull("Loaded object is not an image");
        return img;
    }

    internal static string GetTestDataLocation()
    {
        var pathAssembly = Assembly.GetExecutingAssembly().Location;
        var folderAssembly = Path.GetDirectoryName(pathAssembly);
        if (folderAssembly?.EndsWith(@"\") == false) folderAssembly += @"\";
        var folderProjectLevel = Path.GetFullPath(folderAssembly + @"..\..\..\..\TestData\");
        if (!Directory.Exists(folderProjectLevel))
            throw new DirectoryNotFoundException("Cannot find TestData folder at " + folderProjectLevel);

        return folderProjectLevel;
    }
}
