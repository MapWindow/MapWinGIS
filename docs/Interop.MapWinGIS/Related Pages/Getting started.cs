#pragma warning disable 1587
/// \page getting_started Getting started with MapWinGIS
///
/// Let's start assuming that you:
/// - Installed the latest 32-Bit version of MapWinGIS using the installer available on the 
/// <a href="https://github.com/MapWindow/MapWinGIS/releases">MapWinGIS GitHub site</a>;
/// - Created a new WinForms project in Visual Studio and have a new Form to add Map control to.
/// 
/// \note You can create a 64-Bit version of you application and run it with the 64-Bit version of MapWinGIS, 
/// but you need to install the 32-Bit version of MapWinGIS as well to be able to use the control in Visual Studio.
/// This is because Visual Studio cannot handle 64-Bit ActiveX controls.
/// 
/// \section gs1 A. Adding control control to .NET WinForms project.
/// 
/// Here are steps to start working with MapWinGIS ActiveX control:\n\n
/// 1. Right click on Toolbox window (Ctrl + Alt + X) and select "Choose items" option.\n\n
/// 2. Choose "COM Components" Tab and check MapWinGIS "Map control" item in the list.\n
/// <div class="left">
/// \image html toolbox_add.jpg
/// </div>
/// It will appear in the toolbox:
/// <div class="left">
/// \image html toolbox_add2.jpg
/// </div>
/// 3. Drag the control from Toolbox on design surface of the form.\n
/// <div class="left">
/// \image html form_add.jpg
/// </div>
/// 4. Set the following properties for the control in Properties window (F4):\n
/// \code
/// AxMap.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
/// AxMap.TileProvider = tkTileProvider.OpenStreetMap;
/// AxMap.KnownExtents = tkKnownExtents.keUSA;
/// \endcode
/// \note The screenshots actually show BingMaps provider, but in version 4.9.3 it requires
/// an API key to be specified first, therefore we changed instructions to OpenStreetMap
/// which is available out of the box.
/// 
/// Map control should redraw itself according to these settings.
/// <div class="left">
/// \image html form_add2.jpg
/// </div>
/// 
/// 5. Run the project to make sure that it works at runtime.\n
///
/// To change the area of map being displayed the following properties can be used: AxMap.Latitude, 
/// AxMap.Longitude, AxMap.CurrentZoom. 
/// For example, to display Paris, let set them to: Latitude = 48.85; Longitude = 2.36; CurrentZoom = 12.
/// 
/// <div class="left">
/// \image html form_add3.jpg
/// </div>
/// 
/// 6. If you access Internet via proxy it may also be needed to configure proxy settings for MapWinGIS.\n 
/// 
/// Start with calling:
/// \code
/// // it will try to copy settings from IE
/// axMap1.Tiles.AutoDetectProxy();
/// \endcode
/// If that doesn't help, try to set it explicitly:
/// \code
/// axMap1.Tiles.SetProxy("ip_address_of_proxy", port);
/// \endcode
/// Finally if tiles are still not shown the issue can be debugged with Fiddler. To allow Fiddler
/// intercept HTTP requests set the following proxy settings:
/// \code
/// axMap1.Tiles.SetProxy("127.0.0.1", 8888);
/// \endcode
/// 
/// 7. Additional information displayed on the map can be configured using:
/// - AxMap.ScalebarVisible, AxMap.ScalebarUnits - scalebar in lower left corner;
/// - AxMap.ShowCoordinates - coordinates of mouse cursor position in top left corder;
/// - AxMap.ShowVersionNumber - version number of control in bottom right corner;
/// - AxMap.ShowRedrawTime - redraw time in top right corner.
///
/// \section gs3 B. Adding documentation to Visual Studio IntelliSense.
///
/// Let's add a button, double-click on it and start typing some code. We shall see VS IntelliSense 
/// autocomplete menu, however there won't be any description of members.
/// <div class="left">
/// \image html intellisense.jpg
/// </div>
/// To fix this use the following steps:\n\n
/// 1. Open folder with MapWinGIS installation (by default c:\\dev\\MapWinGIS) and copy Interop.MapWinGIS.xml and
/// AxInterop.MapWinGIS.xml files in obj\\Debug folder in your project. The names of Interop assemblies which should 
/// already be there must match the names of the files: Interop.MapWinGIS.dll and AxInterop.MapWinGIS.dll.\n\n
/// 2. Reload your WinForms project to make sure the new files are found.\n\n
/// Let's try to type the same line once more. Description of API members must be present this time.\n
/// <div class="left">
/// \image html intellisense2.jpg
/// </div>
/// The latest versions of XML documentation for Interop assemblies can also be found in the repository:
/// <a href = "https://github.com/MapWindow/MapWinGIS/blob/master/docs/Interop.MapWinGIS.XML">Interop.MapWinGIS.xml</a>
/// <a href = "https://github.com/MapWindow/MapWinGIS/blob/master/docs/AxInterop.MapWinGIS.XML">AxInterop.MapWinGIS.xml</a>
///
/// \section gs4 C. Default hotkeys.
/// Map control uses the following built-in keyboard hotkeys:
///     - "+" - zoom in;
///     - "-" - zoom out;
///     - "*" - zoom to the closest tile level;
///     - "Home" - zoom to combined extents of all data layers (max extents);
///     - "Backspace" - zoom to previous extents; 
///     - "Shift + Left", "Shift + Right" - zoom to the prev/ next layer;
///     - "Z" - ZoomIn tool; 
///     - "M" - measuring tool; 
///     - "Space" - switches to panning mode; after releasing shift the previous map cursor is restored;
///     - arrow keys - to move the map;
///     - mouse wheel - to zoom in/out regardless of the current tool.
/// 
///     Hot keys will work if map is focused. It's enough to click the map with mouse to set input focus to it.
/// 
///     Arrow keys won't be passed automatically to AxMap control. To enable them the following code should be placed in constructor of Form:
/// \code
/// this.axMap1.PreviewKeyDown += delegate(object sender, PreviewKeyDownEventArgs e)
/// {
///     switch (e.KeyCode)
///     {
///         case Keys.Left:
///         case Keys.Right:
///         case Keys.Up:
///         case Keys.Down:
///             e.IsInputKey = true;
///             return;
///     }
/// };
/// \endcode
/// 
/// \section gs5 D. Measuring tool.
/// MapWinGIS has built-in measuring tool. To start measuring set in Properties window or in code:
/// \code
/// axMap1.CursorMode = cmMeasure;
/// \endcode
/// Or use M hotkey when map is focused. Mouse cursor will have a ruler at its bottom.
/// To toggle to area measuring mode use:
/// \code
/// axMap1.Measuring.MeasuringType= tkMeasuringType.MeasureArea;
/// \endcode
/// Here are screenshots for both modes of measuring:\n
/// <div align = "left">
/// <table cellspacing="0" cellpadding="0, 10" style="border: none; width: 900px;" >
/// <tr align = "center">
///     <td width = "33%">\image html measure_distance.jpg</td>
///     <td width = "33%">\image html measure_area.jpg</td>
/// </tr>
/// <tr>
///     <td align = "center">%Measuring distance</td>
///     <td align = "center">%Measuring area</td>
/// </tr>
/// </table>
/// </div>
/// 
/// The following mouse hotkeys are supported:
/// - Left-click: add new points;
/// - Right-click: undo the last point;
/// - Double-click: finish measuring of the current path; the next left click after it will start a new path;
/// - Ctrl-click on one of the preceding vertices in distance mode, to close polygon and measure its area;
/// - Shift-click to snap to the closest vertex of shapefile.
/// 
/// To clear measured path set different cursor to AxMap.CursorMode property (Z shortcut can be used to switch to cmZoomIn tool).
/// 
/// \section gs6 E. Displaying data layers.
/// Data layers can be opened with or without explicit creation of corresponding COM object.\n\n
/// 1. Without explicit creation of COM object:
/// \code
/// string filename = "z:\\some_filename.shp"
/// int layerHandle = axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosAutoDetect, true);
/// if (layerHandle == -1) {
///     Debug.WriteLine("Failed to open datasource: " + axMap1.FileManager.get_ErrorMsg(axMap1.FileManager.LastErrorCode);
/// }
/// \endcode
/// 2. With explicit creation of COM object:\n\n
/// For shapefiles:
/// \code
/// var sf = new Shapefile();
/// if (sf.Open(filename, null)) {
///     int layerHandle = axMap1.AddLayer(sf, true);
/// }
/// else {
///     Debug.WriteLine("Failed to open shapefile: " + sf.get_ErrorMsg(sf.LastErrorCode));
/// }
/// \endcode
/// For RGB images
/// \code
/// var img = new Image();
/// if (img.Open(filename, null)) {
///     int layerHandle = axMap1.AddLayer(img, true);
/// }
/// else {
///     Debug.WriteLine("Failed to open image: " + img.get_ErrorMsg(img.LastErrorCode));
/// }
/// \endcode
/// 
/// Opening of grid datasources with explicit creation of COM object is somewhat more involved topic.
/// See details in overview of Grid class.\n
/// 
/// It may also be needed to call:
/// \code
/// axMap1.ZoomToLayer(layerHandle);
/// \endcode
/// 
/// \section gs7 F. Projection mismatch testing.
/// Adding of layer to the map may fail because of projection mismatch testing (GlobalSettings.AllowProjectionMismatch).
/// It's turned off by default (i.e. set to true; mismatch is allowed), 
/// so any layer will be added to the map even if its projection doesn't match.  
/// If projection mismatch testing is on and reprojection option is chosen (GlobalSettings.ReprojectLayersOnAdding),
/// a copy of datasource may be created in AxMap.AddLayer. 
/// Any initialization logic in this case should be run after layer was added to the map.
/// \code
/// new GlobalSettings() { AllowProjectionMismatch = false, ReprojectLayersOnAdding = true };
/// var sf = new Shapefile();
/// if (sf.Open(filename, null)) 
/// {
///     //sf.GenerateLabels(0, tkLabelPositioning.lpCentroid); // don't call it here as labels may be lost
///     int layerHandle = axMap1.AddLayer(sf, true);
///     sf = axMap1.get_Shapefile(layerHandle);                // grab the reprojected version of shapefile
///     sf.GenerateLabels(0, tkLabelPositioning.lpCentroid);   // now it's ok to generate labels
/// }
/// \endcode
/// The previous point is relevant only if projection of datasource is different
/// from projection of map. In most cases there is no difference whether to do initialization
/// before or after adding it to the map. To make sure that map uses the same projection
/// as the one set for datasource use:
/// \code
/// axMap1.Projection = tkMapProjection.PROJECTION_NONE;
/// axMap1.GrabProjectionFromData = true;
/// axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosAutoDetect, true);
/// \endcode
/// Built-in reprojection is currently supported for shapefile layers only.
/// 
/// \section gs8 G. Updating the previous versions of MapWinGIS.
/// To update your application to a new version of MapWinGIS the following steps will be needed:\n\n
/// 1) Install the new version of MapWinGIS from installer or register it manually with regsvr32 utility (all dependent 
/// libraries must be present in the latter case);\n
/// 2) Create a new WinForms project in Visual Studio, select necessary version of 
/// .NET Framework, add MapWinGIS control to the form.\n
/// 3) Copy automatically created interop assemblies (Interop.MapWinGIS.dll, AxInterop.MapWinGIS.dll) 
/// from new_winforms_project\\obj\\debug to your_old_winforms_project\\obj\\debug.\n
/// 4) Rebuild project. Over a course of time there were a few small changes in API (like Shape.numPoints -> Shape.NumPoints),
/// so perhaps a couple of batch renamings will be needed.\n
/// 5) It's also recommended to comment the following line in Form Designer: 
/// \code
/// this.axMap1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axMap1.OcxState")));
/// \endcode
/// This will reset control's properties set in the designer, as outdated state information often
/// becomes a source of problems.
#pragma warning restore 1587