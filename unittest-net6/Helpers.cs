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
}
