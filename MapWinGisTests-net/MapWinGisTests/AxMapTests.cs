using System.Runtime.InteropServices;
using System.Windows.Forms;
using Xunit.Abstractions;
using Xunit.Sdk;

namespace MapWinGisTests;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class AxMapTests
{
	private readonly ITestOutputHelper _testOutputHelper;

	[DllImport("kernel32.dll", EntryPoint = "OutputDebugStringW", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
	private static extern void OutputDebugString(string lpOutputString);


	internal static bool IsRunningOnGitHubActions =>
		string.Equals(
			Environment.GetEnvironmentVariable("GITHUB_ACTIONS"),
			"true",
			StringComparison.OrdinalIgnoreCase);

	public AxMapTests(ITestOutputHelper testOutputHelper)
	{
		_testOutputHelper = testOutputHelper;
	}

	[StaFact]
	public void VersionTest()
	{
		WinFormsTestRunner.Run(form=>
		{
			form.ShouldNotBeNull();
			form.EnsureMapControlCreated();

			var version = form.GetMapWinGisVersion();
			version.ShouldNotBeNull();
			version.Major.ShouldBe(5);
			version.Minor.ShouldBe(4);
			version.Build.ShouldBeGreaterThanOrEqualTo(0);
			_testOutputHelper.WriteLine("Version: {0}", version);
		});
	}

	[StaFact]
	public void MapProjectionTest()
	{
		WinFormsTestRunner.Run(form =>
		{
			//using var form = new WinFormsApp1.Form1();
			//if(Environment.Is64BitProcess)
			form.Show(); // We need to show the form to have a valid map control (x64)
			form.ShouldNotBeNull();
			form.EnsureMapControlCreated();

			var visible = !IsRunningOnGitHubActions; // Don't render when running on GitHub Actions
			var sfLocation = Helpers.GetTestFilePath("UnitedStates-3857.shp");

			_testOutputHelper.WriteLine($"MapProjectionTest() call form.OpenFile() with visible: {visible}");
			OutputDebugString($"MapProjectionTest() call form.OpenFile() with visible: {visible}");
			var layerHandle = form.OpenFile(sfLocation, visible);
			//Application.DoEvents();

			layerHandle.ShouldNotBe(-1, "form.OpenFile failed");

			var epsgCode = form.GetMapProjectionAsEpsgCode();
			epsgCode.ShouldBe(3857);
		});
	}

	[StaFact]
	public void OpenShapefileWithInvalidSpatialIndex()
	{
		WinFormsTestRunner.Run(form =>
		{
			//using var form = new WinFormsApp1.Form1();
			form.ShouldNotBeNull();
			var visible = !IsRunningOnGitHubActions; // Don't render when running on GitHub Actions
			form.EnsureMapControlCreated();

			var sfLocation = Helpers.GetTestFilePath("Issue-216.shp");
			_testOutputHelper.WriteLine($"OpenShapefileWithInvalidSpatialIndex() call form.OpenFile() with visible: {visible}");
			OutputDebugString($"OpenShapefileWithInvalidSpatialIndex() call form.OpenFile() with visible: {visible}");
			var layerHandle = form.OpenFile(sfLocation, visible);
			//Application.DoEvents();
			layerHandle.ShouldNotBe(-1, "form.OpenFile failed");

			var sf = form.GetShapefileFromLayer(layerHandle);
			sf.ShouldNotBeNull("Could not get shapefile from layer");

			// Test
			sf.HasSpatialIndex.ShouldBeTrue();
			sf.UseSpatialIndex.ShouldBeTrue();
			sf.IsSpatialIndexValid().ShouldBeTrue();
			sf.HasInvalidShapes().ShouldBeFalse();
			sf.NumShapes.ShouldBe(13424);
			sf.Extents.xMin.ShouldBe(108.722071, 0.00001);
			sf.Extents.yMin.ShouldBe(34.149021, 0.00001);
			sf.Extents.xMax.ShouldBe(109.139842, 0.00001);
			sf.Extents.yMax.ShouldBe(34.457816, 0.00001);
		});
	}

	[StaFact]
	public void ShapefileKeyTest()
	{
		// AS mentioned at https://mapwindow.discourse.group/t/key-property-of-shape-object-not-work/1250

		WinFormsTestRunner.Run(form =>
		{
			//using var form = new WinFormsApp1.Form1();
			//if(Environment.Is64BitProcess)
			OutputDebugString("calling form.Show()");
			form.Show(); // We need to show the form to have a valid map control (x64)
			OutputDebugString("form.Show() done!");

			form.ShouldNotBeNull();
			OutputDebugString("calling form.EnsureMapControlCreated()");
			form.EnsureMapControlCreated();
			OutputDebugString("form.EnsureMapControlCreated() done!");

			// Create shapefile:
			var sfPolygon = Helpers.CreateTestPolygonShapefile();

			// Set key for shapefile:
			const string sfKeyValue = "This is my sf key";
			sfPolygon.Key = sfKeyValue;
			// Check:
			sfPolygon.Key.ShouldBe(sfKeyValue);

			// Set key for first shape:
			const string shpKeyValue = "This is my shp key";
			sfPolygon.Shape[0].Key = shpKeyValue;
			// Check:
			sfPolygon.Shape[0].Key.ShouldBe(shpKeyValue);
			var shp = sfPolygon.Shape[0];
			shp.Key.ShouldBe(shpKeyValue);

			// Re-check:
			sfPolygon.Key.ShouldBe(sfKeyValue);

			// Add shapefile to map and test again:
			var visible = !IsRunningOnGitHubActions; // Don't render when running on GitHub Actions
			_testOutputHelper.WriteLine($"ShapefileKeyTest() call form.AddShapefileToMap() with visible: {visible}");
			OutputDebugString($"ShapefileKeyTest() call form.AddShapefileToMap() with visible: {visible}");
			var layerHandle = form.AddShapefileToMap(sfPolygon, visible);
			//Application.DoEvents();
			// Get sf back:
			var sf = form.GetShapefileFromLayer(layerHandle);
			sf.Key.ShouldBe(sfKeyValue);
			sf.Shape[0].Key.ShouldBe(shpKeyValue);
		});
	}
}