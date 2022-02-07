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
    #endregion
}
