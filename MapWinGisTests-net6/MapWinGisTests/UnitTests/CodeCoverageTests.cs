using System.Reflection;

namespace MapWinGisTests.UnitTests;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class CodeCoverageTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public CodeCoverageTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void CheckShapefileClass()
    {
        CheckTests(typeof(ShapefileClass), "Shapefile");
    }

    [Fact]
    public void CheckShapeClass()
    {
        CheckTests(typeof(ShapeClass), "Shape");
    }

    [Fact]
    public void CheckGlobalSettingsClass()
    {
        CheckTests(typeof(GlobalSettingsClass), "GlobalSettings");
    }

    [Fact]
    public void CheckGdalUtilsClass()
    {
        CheckTests(typeof(GdalUtilsClass), "GdalUtils");
    }

    private void CheckTests(Type myType, string className, bool scaffoldUnitTests = false)
    {
        var names = new Dictionary<string, string?>();
        var missingTests = 0;

        // Get the properties:
        var myProperties = myType.GetProperties();
        foreach (var t in myProperties)
        {
            //_testOutputHelper.WriteLine("The name of the method is {0}.", t.Name);
            if (!names.ContainsKey(t.Name)) names[t.Name] = t.PropertyType.FullName;
        }

        // Get the public methods.
        var myArrayMethodInfo = myType.GetMethods(BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly);
        // Display all the methods.
        foreach (var t in myArrayMethodInfo)
        {
            var name = t.Name
                .ReplaceFirstOccurrence("get_", "")
                .ReplaceFirstOccurrence("set_", "");
            //_testOutputHelper.WriteLine("The name of the method is {0}.", name);
            if (!names.ContainsKey(name)) names[name] = "Method";
        }

        // Get all tests available:
        var myAssembly = Assembly.GetExecutingAssembly();
        var allTests = new List<string>();
        foreach (var type in myAssembly.GetTypes())
        {
            if (type.FullName == null || !type.FullName.StartsWith("MapWinGisTests.")) continue;
            // Get the methods of this class:
            var methods = type.GetMethods(BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly);
            allTests.AddRange(methods.Select(method => method.Name));
        }

        foreach (var (name, type) in names)
        {
            if (scaffoldUnitTests)
            {
                ScaffoldUnitTest(className, name, type!);
            }
            else
            {
                // Check if the tests exists, name should be {className}{name}Test
                if (allTests.Contains($"{className}{name}Test", StringComparer.OrdinalIgnoreCase)) continue;

                _testOutputHelper.WriteLine($"{className}.{name} has no unit test");
                missingTests++;
            }
        }

        missingTests.ShouldBe(0, $"No full code coverage for the {className} class");
    }

    private void ScaffoldUnitTest(string className, string name, string type)
    {
        if (className == "GlobalSettings") ScaffoldUnitTestGlobalSettings(className, name, type);
        if (className == "GdalUtils") ScaffoldUnitTestGdalUtils(className, name, type);

    }

    private void ScaffoldUnitTestGdalUtils(string className, string name, string type)
    {
        _testOutputHelper.WriteLine("[Fact]");
        _testOutputHelper.WriteLine($"public void {className}{name}Test()\n{{");
        _testOutputHelper.WriteLine($"\tvar retVal = _gdalUtils.{name};");
        _testOutputHelper.WriteLine($"\tretVal.ShouldBeTrue();");
        _testOutputHelper.WriteLine(@"}\n");
    }

    private void ScaffoldUnitTestGlobalSettings(string className, string name, string type)
    {
        _testOutputHelper.WriteLine("[Fact]");
        _testOutputHelper.WriteLine($"public void {className}{name}Test()\n{{");
        _testOutputHelper.WriteLine($"\t//Read:\n\tvar value = _gs.{name};");

        if (type.Contains("string", StringComparison.CurrentCultureIgnoreCase))
        {
            // String
            _testOutputHelper.WriteLine("\t_testOutputHelper.WriteLine(value);");
            _testOutputHelper.WriteLine($"\tvalue.ShouldNotBeNullOrEmpty(\"{name} is not set\");");
            _testOutputHelper.WriteLine("\tvalue.ShouldBe(\"foo\");");
            _testOutputHelper.WriteLine("\t// Change:\n\tconst string newValue = \"Воздух\";");
            _testOutputHelper.WriteLine($"\t_gs.{name} = newValue;");
            _testOutputHelper.WriteLine($"\t// Check:\n\t_gs.{name}.ShouldBe(newValue);");
            _testOutputHelper.WriteLine($"\t// Reset:\n\t_gs.{name} = value;");
            _testOutputHelper.WriteLine($"\t_gs.{name}.ShouldBe(value);");
        }

        if (type.Contains("double", StringComparison.CurrentCultureIgnoreCase))
        {
            // Double
            _testOutputHelper.WriteLine(
                $"\t_testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));");
            _testOutputHelper.WriteLine($"\tvalue.ShouldBe(100_000);");
            _testOutputHelper.WriteLine($"\t// Change:\n\t_gs.{name} = value * 2.5;");
            _testOutputHelper.WriteLine($"\t// Check:\n\t_gs.{name}.ShouldBe(value * 2.5);");
            _testOutputHelper.WriteLine($"\t// Reset:\n\t_gs.{name} = value;");
            _testOutputHelper.WriteLine($"\t_gs.{name}.ShouldBe(value);");
        }

        if (type.Contains("integer", StringComparison.CurrentCultureIgnoreCase))
        {
            // Double
            _testOutputHelper.WriteLine(
                $"\t_testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));");
            _testOutputHelper.WriteLine($"\tvalue.ShouldBe(500);");
            _testOutputHelper.WriteLine($"\t// Change:\n\t_gs.{name} = value * 2;");
            _testOutputHelper.WriteLine($"\t// Check:\n\t_gs.{name}.ShouldBe(value * 2);");
            _testOutputHelper.WriteLine($"\t// Reset:\n\t_gs.{name} = value;");
            _testOutputHelper.WriteLine($"\t_gs.{name}.ShouldBe(value);");
        }

        if (type.Contains("boolean", StringComparison.CurrentCultureIgnoreCase))
        {
            // Boolean
            _testOutputHelper.WriteLine(
                $"\t_testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));");
            _testOutputHelper.WriteLine($"\tvalue.ShouldBeTrue();");
            _testOutputHelper.WriteLine($"\t// Change:\n\t_gs.{name} = !value;");
            _testOutputHelper.WriteLine($"\t// Check:\n\t_gs.{name}.ShouldBe(!value);");
            _testOutputHelper.WriteLine($"\t// Reset:\n\t_gs.{name} = value;");
            _testOutputHelper.WriteLine($"\t_gs.{name}.ShouldBe(value);");
        }

        if (type.Contains("MapWinGIS.", StringComparison.CurrentCultureIgnoreCase))
        {
            // Type: MapWinGIS.tkGdalError
            // Enum
            var enumClass = type.Replace("MapWinGIS.", "");
            _testOutputHelper.WriteLine($"\t_testOutputHelper.WriteLine(value.ToString());");
            _testOutputHelper.WriteLine($"\tvalue.ShouldBe({enumClass}.??);");
            _testOutputHelper.WriteLine(
                $"\t// Loop:\n\tforeach ({enumClass} enumValue in Enum.GetValues(typeof({enumClass})))\n\t{{");
            _testOutputHelper.WriteLine($"\t\t_testOutputHelper.WriteLine(enumValue.ToString());");
            _testOutputHelper.WriteLine($"\t\t// Change:\n\t\t_gs.{name} = enumValue;");
            _testOutputHelper.WriteLine($"\t\t// Check:\n\t\t_gs.{name}.ShouldBe(enumValue);\n\t}}");
            _testOutputHelper.WriteLine($"\t// Reset:\n\t_gs.{name} = value;");
            _testOutputHelper.WriteLine($"\t_gs.{name}.ShouldBe(value);");
        }

        if (type == "Method")
        {
            _testOutputHelper.WriteLine(
                "\tthrow new NotImplementedException(\"This unit test is scaffolded, needs attention\");");
        }
        _testOutputHelper.WriteLine(@"}");
    }
}
