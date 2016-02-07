namespace interopCreator
{
  using System;
  using System.IO;
  using System.Windows.Forms;
  using MapWinGIS;

  public partial class Form1 : Form
  {
    public Form1()
    {
      InitializeComponent();
    }

    private void CheckNewMethods()
    {
      this.axMap1.DeserializeLayer(1, "Test");
      var projection = new GeoProjection();
      projection.ImportFromProj4("g");
      var t = tkSavingMode.modeXMLOverwrite;
      axMap1.SaveMapState("r", true, true);

      axMap1.set_LayerSkipOnSaving(1, true);

      var sf = new Shapefile();
      var reprojectedCount = 0;
      sf.Reproject(projection, ref reprojectedCount);
      var geoProjection = sf.GeoProjection;
      var sfSimple = sf.SimplifyLines(10, false);

      var gridHeader = new GridHeader();
      var gridHeaderGeoProjection = gridHeader.GeoProjection;

      var ext = this.axMap1.MaxExtents;

      var utils = new Utils();
      utils.ClipGridWithPolygon("test", sfSimple.get_Shape(0), "new", false);

      utils.Polygonize(string.Empty, string.Empty, 1, false, string.Empty, "ESRI Shapefile", string.Empty, "DN", null);

      utils.GDALInfo(string.Empty, string.Empty, null);
      utils.TranslateRaster(string.Empty, string.Empty, string.Empty, null);

      utils.OGRInfo(string.Empty, string.Empty, null, null);
      utils.OGR2OGR(string.Empty, string.Empty, string.Empty, null);
      utils.GDALAddOverviews(string.Empty, string.Empty, string.Empty, null);
      utils.GDALBuildVrt(string.Empty, string.Empty, null);
      utils.GDALRasterize(string.Empty, string.Empty, string.Empty, null);
      utils.GDALWarp(string.Empty, string.Empty, string.Empty, null);
    }

    private void ClipGridWithPolygon()
    {
      var workingFolder = @"C:\dev\TestingScripts\ScriptData\ClipGridWithPolygon";
      var tempFolder = @"C:\dev\TestingScripts\Temp\ClipGridWithPolygon";

      var gridFilename = Path.Combine(workingFolder, "AHN2.asc");
      var sf = new Shapefile();
      sf.Open(Path.Combine(workingFolder, "Areas.shp"), null);
      var numShapes = sf.NumShapes;
      var clippedWrong = 0;
      var clippedGood = 0;

      // Needed for the new method:
      var utils = new Utils();
      var grd = new Grid();
      grd.Open(gridFilename, GridDataType.UnknownDataType, true, GridFileType.UseExtension, null);

      for (var i = 0; i < numShapes; i++)
      {
        var polygon = sf.get_Shape(i);
        var resultGrid = Path.Combine(tempFolder, "AHN2_clipped" + i + ".asc");
        
        // Using thr mwGeoProc version takes almost 4 hours with this data:
        // if (MapWinGeoProc.SpatialOperations.ClipGridWithPolygon(ref gridFilename, ref polygon, ref resultGrid))
        if (utils.ClipGridWithPolygon2(grd, polygon, resultGrid, false))
        {
          clippedGood++;
        }
        else
        {
          clippedWrong++;
          /*
          testsMethods.ReportMessage(
            string.Format(
              "Clipping grid with polygon number {0} failed. Error: {1}", i, MapWinGeoProc.Error.GetLastErrorMsg()));
          */
        }
      }

      var msg = string.Empty;
      msg = clippedWrong == 0 ? "All tests successful!" : string.Format("{0} clippings went OK, {1} went wrong", clippedGood, clippedWrong);
      MessageBox.Show(msg);
    }

    private void button1_Click(object sender, EventArgs e)
    {
      this.ClipGridWithPolygon();
    }
  }
}
