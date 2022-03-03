using System.Reflection;

namespace MapWinGisTests.UnitTests;
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

    private void CheckTests(Type myType, string className, bool scaffoldUnitTests = false)
    {
        var names = new List<string>();
        var missingTests = 0;

        // Get the properties:
        var myProperties = myType.GetProperties();
        foreach (var t in myProperties)
        {
            //_testOutputHelper.WriteLine("The name of the method is {0}.", t.Name);
            if (!names.Contains(t.Name)) names.Add(t.Name);
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
            if (!names.Contains(name)) names.Add(name);
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

        foreach (var name in names)
        {
            if (scaffoldUnitTests)
            {
                //_testOutputHelper.WriteLine("[Fact(Skip=\"Unit test is not yet implemented\")]");
                _testOutputHelper.WriteLine("[Fact]");
                _testOutputHelper.WriteLine($"public void {className}{name}Test() {{\nvar value = _gs.{name};\n_testOutputHelper.WriteLine(value.ToString());\n}}\n");
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
}
