using System.Reflection;

namespace MapWinGisTests;

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
    internal static Shapefile MakeShapefile(ShpfileType sfType)
    {
        var sf = new Shapefile();
        sf.ShouldNotBeNull("Could not initialize Shapefile object");
        var retVal = sf.CreateNewWithShapeID("", sfType);
        retVal.ShouldBeTrue("sf.CreateNewWithShapeID() failed");
        sf.ShapefileType.ShouldBe(sfType, "Shapefile type is unexpected");
        return sf;
    }

    internal static string SaveSfToTempFile(Shapefile sf, string filenamePart)
    {
        try
        {
            var fileName = GetRandomFilePath(filenamePart, ".shp");
            SaveSf(sf, fileName);

            // Check if shapefile files exists:
            CheckSfFiles(fileName, true);

            return fileName;
        }
        catch (Exception e)
        {
            Console.WriteLine(@"Could not write to temp file: " + e.Message);
            return string.Empty;
        }
    }

    internal static void SaveSf(Shapefile sf, string fileLocation)
    {
        sf.ShouldNotBeNull("sf is null");
        var retVal = sf.SaveAsEx(fileLocation, true, false);
        retVal.ShouldBeTrue("sf.SaveAs failed");
        sf.EditingShapes.ShouldBeFalse("sf is still in edit mode. Error: " + sf.ErrorMsg[sf.LastErrorCode]);
    }

    internal static Shapefile OpenShapefile(string fileLocation, ICallback callback)
    {
        Console.WriteLine(@"Opening shapefile: " + fileLocation);
        File.Exists(fileLocation).ShouldBeTrue("Could not find shapefile to open");

        var sf = new Shapefile();
        sf.ShouldNotBeNull("Could not initialize Shapefile object");
        sf.GlobalCallback = callback;
        Console.WriteLine(@"Before sf.Open");
        var retVal = sf.Open(fileLocation, callback);
        Console.WriteLine(@"After sf.Open");
        retVal.ShouldBeTrue("sf.Open failed");
        
        return sf;
    }

    internal static Shapefile LoadSfUsingFileManager(string filename)
    {
        File.Exists(filename).ShouldBeTrue("Input file does not exist: " + filename);

        var fm = new FileManager();
        var obj = fm.Open(filename);
        fm.LastOpenIsSuccess.ShouldBeTrue(fm.ErrorMsg[fm.LastErrorCode]);
        var sf = obj as Shapefile;
        sf.ShouldNotBeNull("Loaded object is not an shapefile");
        return sf;
    }

    internal static Shapefile CreateTestPolygonShapefile()
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

    internal static Shapefile CreateRandomPointShapefile(int numPoints)
    {
        // Create pre-defined shapefile for extent:
        var sfPolygon = CreateTestPolygonShapefile();
        var extents = sfPolygon.Extents;
        extents.ShouldNotBeNull();

        // Create shapefile:
        var sfPoint = MakeShapefile(ShpfileType.SHP_POINT);

        // Create some random points:
        var random = new Random();
        var width = extents.xMax - extents.xMin;
        var height = extents.yMax - extents.yMin;

        for (var i = 0; i < numPoints; i++)
        {
            var x = extents.xMin + width * random.NextDouble();
            var y = extents.yMin + height * random.NextDouble();
            Helpers.AddPointShape(sfPoint, x, y);
        }

        // Checks:
        sfPoint.ShapefileType.ShouldBe(ShpfileType.SHP_POINT);
        sfPoint.NumShapes.ShouldBe(numPoints);

        return sfPoint;
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

    internal static void AddShape(Shapefile sf, string wktSting)
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

    internal static int AddPointShape(Shapefile sf, double x, double y)
    {
        // Add shape:
        var shp = MakeShape(ShpfileType.SHP_POINT);
        shp.ShouldNotBeNull("shape is null");
        // Add point:
        var pointIndex = shp.AddPoint(x, y);
        pointIndex.ShouldBe(0, "Invaldid point index");
        var shapeIndex = sf.EditAddShape(shp);
        shapeIndex.ShouldNotBe(-1, "EditAddShape failed");
        return shapeIndex;
    }
    #endregion


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
        var folderProjectLevel = Path.GetFullPath(folderAssembly + @"..\..\..\..\..\TestData\");
        if (!Directory.Exists(folderProjectLevel))
            throw new DirectoryNotFoundException("Cannot find TestData folder at " + folderProjectLevel);

        return folderProjectLevel;
    }

    internal static string GetTestFilePath(string fileName)
    {
        var path = Path.Combine(GetTestDataLocation(), fileName);
        File.Exists(path).ShouldBeTrue($"{fileName} doesn't exists in the TestData location.");

        return path;
    }

    internal static string GetRandomFilePath(string filenamePart, string extension)
    {
        var baseFileName = Path.Combine(Path.GetTempPath(), Path.GetFileNameWithoutExtension(Path.GetRandomFileName()));
        var fileName = Path.ChangeExtension($"{baseFileName}{filenamePart}", extension);
        Console.WriteLine(@"GetRandomFilePath: " + fileName); 
        
        if (File.Exists(fileName)) File.Delete(fileName);

        return fileName;
    }

    internal static void CheckSfFiles(string sfFileLocation, bool shouldExists)
    {
        var shpFileLocation = Path.ChangeExtension(sfFileLocation, ".shp");
        if (shouldExists)
        {
            File.Exists(shpFileLocation).ShouldBeTrue("Can't find .shp file");
        }
        else
        {
            File.Exists(shpFileLocation).ShouldBeFalse("The .shp file still exists");
        }

        var dbfFileLocation = Path.ChangeExtension(sfFileLocation, ".dbf");
        if (shouldExists)
        {
            File.Exists(dbfFileLocation).ShouldBeTrue("Can't find .dbf file");
        }
        else
        {
            File.Exists(dbfFileLocation).ShouldBeFalse("The .dbf file still exists");
        }

        var shxFileLocation = Path.ChangeExtension(sfFileLocation, ".shx");
        if (shouldExists)
        {
            File.Exists(shxFileLocation).ShouldBeTrue("Can't find .shx file");
        }
        else
        {
            File.Exists(shxFileLocation).ShouldBeFalse("The .shx file still exists");
        }
    }

    internal static string ReplaceFirstOccurrence(this string source, string find, string replace)
    {
        var place = source.IndexOf(find, StringComparison.Ordinal);
        if (place < 0) return source;

        var result = source.Remove(place, find.Length).Insert(place, replace);
        return result;
    }

    internal static void CopyShapefile(string sourceFilename, string destinationFilename)
    {
        // Checks:
        if (!File.Exists(sourceFilename))
            throw new FileNotFoundException("Source file does not exists", sourceFilename);
        if (File.Exists(destinationFilename)) DeleteShapefileFiles(destinationFilename);

        var fileDirSource = Path.GetDirectoryName(sourceFilename);
        if (fileDirSource is null) throw new FileNotFoundException("Source file does not exists", sourceFilename);
        var fileDirDestination = Path.GetDirectoryName(destinationFilename)!;
        if (!Directory.Exists(fileDirDestination)) Directory.CreateDirectory(fileDirDestination);

        var dir = new DirectoryInfo(fileDirSource);
        var baseNameSource = Path.GetFileNameWithoutExtension(sourceFilename);
        var baseNameDestination = Path.GetFileNameWithoutExtension(destinationFilename);
        foreach (var file in dir.EnumerateFiles($"{baseNameSource}.*", SearchOption.TopDirectoryOnly))
        {
            var ext = Path.GetExtension(file.FullName);
            File.Copy(file.FullName, Path.Combine(fileDirDestination, $"{baseNameDestination}{ext}"));
        }
    }

    internal static void DeleteShapefileFiles(string sfFilename)
    {
        var fileDir = Path.GetDirectoryName(sfFilename);
        if (fileDir is null) return;

        var dir = new DirectoryInfo(fileDir);

        var baseName = Path.GetFileNameWithoutExtension(sfFilename);
        foreach (var file in dir.EnumerateFiles($"{baseName}.*", SearchOption.TopDirectoryOnly))
        {
            file.Delete();
        }
    }
}
