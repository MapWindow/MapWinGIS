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

		FormClosing += Form1_FormClosing;

		if(!IsRunningOnGitHubActions)
        {
			// Don't call LoadOsm() when running on GitHub Actions.
			Shown += (s, e) => LoadOsm(); // This need to be done after the form is shown (x64)
		}
        /*else
        {
            LoadOsm();
        }*/
    }

	private void Form1_FormClosing(object? sender, FormClosingEventArgs e)
	{
		// Properly dispose of the AxMap control to avoid crashes on exit.
		_ = new GlobalSettings { ApplicationCallback = null };

		if(axMap1 == null) return;

		try {
			if(!axMap1.IsDisposed)
			{
				axMap1.RemoveAllLayers();
				if(axMap1.GlobalCallback != null)
					axMap1.GlobalCallback = null;
			}
		} catch {
			/* ignore callback/layer teardown errors during shutdown */
		}

		try {
			if(!axMap1.IsDisposed)
				axMap1.Dispose();
		} catch {
			/* ignore control disposal errors during shutdown */
		}
	}

	public static bool IsRunningOnGitHubActions =>
		string.Equals(
			Environment.GetEnvironmentVariable("GITHUB_ACTIONS"),
			"true",
			StringComparison.OrdinalIgnoreCase);

	// WM_GETOBJECT is sent when a UI Automation / accessibility client connects to the window.
	// Handling it makes WinForms create a UIA provider for the control. When the form is later
	// disposed, Control.ReleaseUiaProvider tears that provider down via
	// UiaReturnRawElementProvider, which forces UI Automation to initialize by doing a
	// cross-apartment CoCreateInstance(CUIAutomation7). On the 32-bit (x86) CI runner that
	// cross-apartment call never returns and blocks in the STA modal loop, so the test hangs
	// during Form1.Dispose(). Ignoring WM_GETOBJECT on the CI runner prevents the provider from
	// ever being created, so teardown no longer performs that blocking call. This keeps the
	// map round-trip (AddShapefileToMap) working on both x86 and x64.
	private const int WM_GETOBJECT = 0x003D;

	protected override void WndProc(ref Message m)
	{
		if(IsRunningOnGitHubActions && m.Msg == WM_GETOBJECT)
		{
			m.Result = IntPtr.Zero;
			return;
		}

		base.WndProc(ref m);
	}

	public void EnsureMapControlCreated()
	{
		CreateControl();

		// Force the Form's own native window handle to exist even though it is never
		// shown. Control.CreateControl() is a no-op while the control is invisible, so
		// in headless mode the form otherwise has no handle. That makes the teardown in
		// WinFormsTestRunner.DisposeFormWhilePumping fall back to a synchronous
		// form.Dispose(), which destroys the child window (and performs the blocking
		// cross-apartment UIA disconnect in Control.ReleaseUiaProvider) outside any
		// active message pump - the deadlock observed as a hang on the 32-bit (x86)
		// test host. Creating the handle here guarantees the pumped BeginInvoke +
		// Application.Run teardown path is used instead, which services that call.
		_ = Handle;

		axMap1.CreateControl();

		if(!axMap1.IsHandleCreated)
		{
			throw new InvalidOperationException("AxMap control handle was not created.");
		}
	}


	private void AxMap1_FileDropped(object sender, _DMapEvents_FileDroppedEvent e)
    {
        TxtProgress.Text += $@"Opening {e.filename} after dropping.{Environment.NewLine}";
        axMap1.AddLayerFromFilename(e.filename, tkFileOpenStrategy.fosAutoDetect, true);
        if (axMap1.GeoProjection.TryAutoDetectEpsg(out var epsgCode))
        {
            TxtProgress.Text += $@"Map projection: EPSG:{epsgCode}{Environment.NewLine}";
            axMap1.TileProvider = tkTileProvider.OpenStreetMap;
            axMap1.CurrentZoom = 2;
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
        axMap1.KnownExtents = tkKnownExtents.keNetherlands;
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
        LogProgress(@"Running GlobalSettingsCompressOverviewsTest");
        GlobalSettingsCompressOverviewsTest();

        LogProgress(@"Running GdalRasterWarpTest");
        GdalRasterWarpTest();

        // TODO: Test still fails:
        //LogProgress(@"Running GdalRasterTranslateTest");
        //GdalRasterTranslateTest();

        LogProgress(@"Running CreateSpatialIndexUnicodeTest");
        CreateSpatialIndexUnicodeTest();

        LogProgress(@"Running OpenShapefile");
        OpenShapefile();

        LogProgress(@"Running SetGeographicExtents");
        SetGeographicExtents();
    }
}
