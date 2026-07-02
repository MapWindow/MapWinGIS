
using System.Diagnostics;
using System.Reflection;
using MapWinGIS;

namespace WinFormsApp1;

public static class Helpers
{
    internal static void AddShape(Shapefile sf, string wktSting)
    {
        // Create shape
        var shp = new Shape();
        shp.ImportFromWKT(wktSting);

        // Add shape to shapefile:
        sf.EditAddShape(shp);
    }

    internal static Shapefile MakeShapefile(ShpfileType sfType)
    {
        var sf = new Shapefile();
        var retVal = sf.CreateNewWithShapeID("", sfType);
        return sf;
    }

    internal static Shapefile CreateTestPolygonShapefile()
    {
        var sfPolygon = MakeShapefile(ShpfileType.SHP_POLYGON);
        // Add shape to shapefile:
        AddShape(sfPolygon, "POLYGON((330695.973322992 5914896.16305817, 330711.986129861 5914867.19586245, 330713.350435287 5914867.56644015, 330716.510827627 5914862.28973662, 330715.632568651 5914860.60107999, 330652.234582712 5914803.80510632, 330553.749382483 5914715.80328169, 330551.979355848 5914714.83347535, 330549.911988583 5914715.86502807, 330545.027807355 5914724.05916443, 330544.592985976 5914725.93531509, 330544.30963704 5914726.72754692, 330543.612620707 5914726.14904553, 330543.271515787 5914727.06633931, 330542.234090059 5914729.85597723, 330542.959654761 5914730.50411962, 330530.319252794 5914765.86064153, 330505.294840402 5914836.7930124, 330471.411812074 5914931.61558331, 330486.074748666 5914941.33795239, 330585.983154737 5915010.32749106, 330618.427962455 5915031.20447119, 330653.234601917 5914970.37328093, 330695.973322992 5914896.16305817))");

        // Set projection:
        sfPolygon.GeoProjection = MakeProjection(28992);

        return sfPolygon;
    }

    internal static GeoProjection MakeProjection(int epsgCode)
    {
        var geoProjection = new GeoProjection();
        geoProjection.ImportFromEPSG(epsgCode);
        return geoProjection;
    }

    internal static void SaveSf(Shapefile sf, string fileLocation)
    {
        sf.SaveAsEx(fileLocation, true, false);
    }

    internal static string SaveSfToTempFile(Shapefile sf, string filenamePart)
    {
        var fileName = GetRandomFilePath(filenamePart, ".shp");
        SaveSf(sf, fileName);

        return fileName;
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

    internal static string GetTestFilePath(string fileName)
    {
        var path = Path.Combine(GetTestDataLocation(), fileName);
        Debug.Assert(File.Exists(path), $"{fileName} doesn't exists in the TestData location.");

        return path;
    }

    internal static string GetRandomFilePath(string filenamePart, string extension)
    {
        var baseFileName = Path.Combine(Path.GetTempPath(), Path.GetFileNameWithoutExtension(Path.GetRandomFileName()));
        var fileName = Path.ChangeExtension($"{baseFileName}{filenamePart}", extension);

        if (File.Exists(fileName)) File.Delete(fileName);

        return fileName;
    }

    internal static Shapefile OpenShapefile(string fileLocation, ICallback callback)
    {
        Console.WriteLine(@"Opening shapefile: " + fileLocation);
        if (!File.Exists(fileLocation))
            throw new FileNotFoundException("Could not find shapefile to open",fileLocation);

        var sf = new Shapefile();
        if (sf is null)
            throw new NullReferenceException("Could not create shapefile object");
        sf.GlobalCallback = callback;
        Console.WriteLine(@"Before sf.Open");
        var retVal = sf.Open(fileLocation, callback);
        Console.WriteLine(@"After sf.Open");
        if (!retVal)
            throw new Exception("Could not open shapefile");    
        
        return sf;
    }    
}
