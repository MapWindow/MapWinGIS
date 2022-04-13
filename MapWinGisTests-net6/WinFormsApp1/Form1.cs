using AxMapWinGIS;
using MapWinGIS;

namespace WinFormsApp1;
public sealed partial class Form1 : Form, ICallback
{
    public Form1()
    {
        InitializeComponent();

        _ = new GlobalSettings
        {
            ApplicationCallback = this,
            CallbackVerbosity = tkCallbackVerbosity.cvAll
        };
        
        LoadOsm();

        GetMapWinGisVersion();
    }

    private void AxMap1_FileDropped(object sender, _DMapEvents_FileDroppedEvent e)
    {
        TxtProgress.Text += $@"Opening {e.filename} after dropping.{Environment.NewLine}";
        axMap1.AddLayerFromFilename(e.filename, tkFileOpenStrategy.fosAutoDetect, true);
        if (axMap1.GeoProjection.TryAutoDetectEpsg(out var epsgCode))
        {
            TxtProgress.Text += $@"Map projection: EPSG:{epsgCode}{Environment.NewLine}";
        }
        else
        {
            TxtProgress.Text += $@"Error! Could not detect EPSG code {Environment.NewLine}";
            TxtProgress.Text += $@"Map projection: {axMap1.GeoProjection.ExportToWktEx()}{Environment.NewLine}";
        }
    }

    private void LoadOsm()
    {
        var geoProjection = new GeoProjection();
        geoProjection.SetGoogleMercator();
        axMap1.GeoProjection = geoProjection;
        axMap1.TileProvider = tkTileProvider.OpenStreetMap;
        axMap1.CurrentZoom = 2;
    }

    private void LogProgress(string msg)
    {
        TxtProgress.Text += $@"{msg}{Environment.NewLine}";
    }

    #region Implementation of ICallback

    public void Progress(string keyOfSender, int percent, string message)
    {
        TxtProgress.Text += $@"{percent} {message}";
    }

    public void Error(string keyOfSender, string errorMsg)
    {
        TxtProgress.Text += $@"Error: {errorMsg}{Environment.NewLine}";
    }

    #endregion

    private void ClearMapButtonClick(object? sender, EventArgs e)
    {
        axMap1.Clear();
        TxtProgress.Text = "";
        LogProgress($@"Map is cleared. Projection is empty: {axMap1.GeoProjection.IsEmpty}");
    }

    private void RunTestButtonClick(object? sender, EventArgs e)
    {
        //LogProgress(@"Running GlobalSettingsCompressOverviewsTest");
        //GlobalSettingsCompressOverviewsTest();
        
        //LogProgress(@"Running GdalRasterWarpTest");
        //GdalRasterWarpTest();   
        
        // TODO: Test still fails:
        //LogProgress(@"Running GdalRasterTranslateTest");
        //GdalRasterTranslateTest();

        //LogProgress(@"Running CreateSpatialIndexUnicodeTest");
        //CreateSpatialIndexUnicodeTest();

        LogProgress(@"Running OpenShapefile");
        OpenShapefile();
    }
}
