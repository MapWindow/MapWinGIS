// ReSharper disable ArrangeAccessorOwnerBody
// ReSharper disable DelegateSubtraction
// ReSharper disable PossibleInvalidCastExceptionInForeachLoop
// ReSharper disable CheckNamespace

using System;
using MapWinGIS;
// ReSharper disable UnassignedGetOnlyAutoProperty
#pragma warning disable 67 // Never used warning

#if nsp
namespace AxMapWinGIS
{
#endif
    /// <summary>
    /// Map component for visualization of vector, raster or grid data.
    /// </summary>
    /// Here is a class diagram for the AxMap class:
    /// \dot 
    /// digraph map_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 1, fontname=Helvetica, fontsize=9, color = lightsalmon, style = filled, height = 0.3, width = 1.0];
    /// map [ label="AxMap" URL="\ref AxMap"];
    /// 
    /// node [shape = "polygon", color = gray, peripheries = 3, style = filled, height = 0.2, width = 0.8]
    /// img [ label="Image" URL="\ref Image"];
    /// sf [ label="Shapefile" URL="\ref Shapefile"];
    /// 
    /// node [color = tan, peripheries = 1, height = 0.3, width = 1.0];
    /// gp [ label="GeoProjection" URL="\ref GeoProjection"];
    /// meas [ label="Measuring" URL="\ref Measuring"];
    /// tiles [ label="Tiles" URL="\ref Tiles"];
    /// fm [ label="FileManager" URL="\ref FileManager"];
    /// se [ label="ShapeEditor" URL="\ref ShapeEditor"];
    /// 
    /// edge [ dir = "none", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// map -> sf [ URL="\ref AxMap.get_Shapefile()", tooltip = "AxMap.get_Shapefile()", headlabel = "   n"];
    /// map -> img [URL="\ref AxMap.get_Image()", tooltip = "AxMap.get_Image()", headlabel = "   n"]
    /// 
    /// gp -> map [ URL="\ref AxMap.GeoProjection", tooltip = "AxMap.GeoProjection", taillabel = "1 "];
    /// meas -> map [URL="\ref AxMap.Measuring", tooltip = "AxMap.Measuring", taillabel = "1 "]
    /// tiles -> map [URL="\ref AxMap.Tiles", tooltip = "AxMap.Tiles", taillabel = "1 "]
    /// fm -> map [URL="\ref AxMap.FileManager", tooltip = "AxMap.FileManager", taillabel = "1 "]
    /// se -> map [URL="\ref AxMap.ShapeEditor", tooltip = "AxMap.ShapeEditor", taillabel = "1 "]
    /// 
    /// }
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>\n\n
    /// Here are groups of API members for AxMap class:
    /// \dotfile mapgroups.dot
    /// <a href = "diagrams.html">Graph description</a>
    public class AxMap : _DMap, _DMapEvents
    {
        #region _DMap Members

        /// <summary>
        /// Gets or sets the user defined cursor handle. The handle is a windows cursor handle. 
        /// </summary>
        public int UDCursorHandle { get; set; }

        /// <summary>
        /// Gets or sets the object of Tiles class associated with map.
        /// </summary>
        /// \new490 Added in version 4.9.0
        public Tiles Tiles
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets FileManager object associated with map. The object is used by AxMap.AddLayerFromFilename methods.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public FileManager FileManager 
        {
            get { throw new NotImplementedException();}
        }
        
        /// <summary>
        /// Gets or sets the background color of the map.
        /// </summary>
        /// <remarks>When using VB.NET the color is represented as a System.Drawing.Color. 
        /// When using VB 6 the color can be an OLE_COLOR or an integer representation of an RGB value.</remarks>
        public uint BackColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
#pragma warning disable 1591
        public uint backColor { get; set; }  // TODO: Why does it start with lower-key 'b'?
#pragma warning restore 1591

        /// <summary>
        /// Draws the content of the back buffer to specified device context.
        /// </summary>
        /// <remarks>If the size of the back buffer differ from the provided dimensions, the scaling of the buffer content will be made.</remarks>
        /// <param name="hdc">The handle of the device context.</param>
        /// <param name="imageWidth">The width of the resulting image.</param>
        /// <param name="imageHeight">The height of the resulting image.</param>
        /// \new510 Updated in version 5.1
        public void DrawBackBuffer(int hdc, int imageWidth, int imageHeight)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of previous extents to go to, forward in the undo list
        /// </summary>
        public int ExtentHistoryRedoCount { get; }

        /// <summary>
        /// Gets the number of previous extents to go to, back in the undo list
        /// </summary>
        public int ExtentHistoryUndoCount { get; }

        /// <summary>
        /// The global callback is the interface used by MapWinGIS to pass progress and error events to interested applications. 
        /// </summary>
        /// <remarks>.NET clients passing ICallback implementation to this property must make respective 
        /// class COM visible by setting ComVisible(true) attribute. In fact ComVisible(true) is set by default 
		/// for public classes, so it's enough to make sure that it isn't explicitly set to false for the class
		/// or assembly where the class is defined. Otherwise InvalidCastException may occur at runtime.</remarks>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public object GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the handle of the window of map control.
        /// </summary>
        /// <returns>The handle of window.</returns>
        public int HWnd()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The key may be used by the programmer to store any string data associated with the object.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Retrieves the last error generated in the object.
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
       
        /// <summary>
        /// Gets or sets the cursor used with the map. When using tkCursor.crsrUserDefined be sure to set a AxMap.UDCursorHandle. 
        /// </summary>
        public tkCursor MapCursor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates how the control adjust its content after the resizing operations.
        /// </summary>
        public tkResizeBehavior MapResizeBehavior
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Redraws all layers in the map if the map is not locked. 
        /// </summary>
        /// <remarks>The same as AxMap.Redraw2(tkRedrawType.RedrawAll).</remarks>
        public void Redraw()
        {
            throw new NotImplementedException();
        }
        /// <summary>
        /// Resizes the map to the given width and height.
        /// </summary>
        /// <param name="width">The new width of the control.</param>
        /// <param name="height">The new hight of the control.</param>
        public void Resize(int width, int height)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a value which indicates whether the time of map redraw will be displayed on the screen.
        /// </summary>
        /// <remarks>Only times of full redraws when data layers are drawn and are displayed.</remarks>
        /// \new48 Added in version 4.8
        public bool ShowRedrawTime
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Shows tooltip near the mouse cursor.
        /// </summary>
        /// <param name="text">Text of the tooltip.</param>
        /// <param name="milliseconds">The duration of tooltip display.</param>
        public void ShowToolTip(string text, int milliseconds)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a boolean value which indicate whether control's version number will be displayed on the screen.
        /// </summary>
        /// \new48 Added in version 4.8
        public bool ShowVersionNumber
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Takes snapshot of the contents of the map within the bounds of the specified rectangle, returning an image of the results. 
        /// </summary>
        /// <param name="boundBox">The bounds (rectangle) in map units to get the snapshot from.</param>
        /// <returns>An image of the contents of the map displayed inside the bounds of the specified rectangle.</returns>
        Image _DMap.SnapShot(Extents boundBox)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Takes snapshot of the contents of the map and returns it as an image object. 
        /// </summary>
        /// <param name="clippingLayerNbr">The position of the layer which extents will be used for taking snapshot.</param>
        /// <param name="zoom">The amount of zoom.</param>
        /// <param name="pWidth">The width of the resulting image.</param>
        /// <returns>An image of the contents of the map.</returns>
        Image _DMap.SnapShot2(int clippingLayerNbr, double zoom, int pWidth)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Takes snapshot of the contents of the map in the specified bounding box and returns it as an image object. 
        /// </summary>
        /// <param name="left">The x coordinate of the lower left corner of the extents.</param>
        /// <param name="right">The x coordinate of the top right corner of the extents.</param>
        /// <param name="top">The y coordinate of the top right corner of the extents.</param>
        /// <param name="bottom">The y coordinate of the lower left corner of the extents.</param>
        /// <param name="width">The width of the resulting image in pixels.</param>
        /// <returns>An image of the contents of the map.</returns>
        Image _DMap.SnapShot3(double left, double right, double top, double bottom, int width)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Performs drawing of the map contents to the specified device context.
        /// </summary>
        /// <param name="hDc">The handle of the device context.</param>
        /// <param name="extents">The map extents to be drawn.</param>
        /// <param name="width">The width of the resulting image in pixels.</param>
        /// <returns>True on success and false otherwise.</returns>
        bool _DMap.SnapShotToDC(IntPtr hDc, Extents extents, int width)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Performs drawing of the map contents to the specified device context.
        /// </summary>
        /// <param name="hDc">The handle of the device context.</param>
        /// <param name="extents">The map extents to be drawn.</param>
        /// <param name="width">The width of the resulting image in pixels.</param>
        /// <param name="offsetX"></param>
        /// <param name="offsetY"></param>
        /// <param name="clipX"></param>
        /// <param name="clipY"></param>
        /// <param name="clipWidth"></param>
        /// <param name="clipHeight"></param>
        /// <returns>True on success and false otherwise.</returns>
        bool _DMap.SnapShotToDC2(IntPtr hDc, Extents extents, int width, float offsetX, float offsetY, float clipX, float clipY,
            float clipWidth, float clipHeight)
        {
            throw new NotImplementedException();
        }

  

        /// <summary>
        /// Gets or sets version number of the control.
        /// </summary>
        /// <remarks>The set part of the property isn't supported.</remarks>
        public string VersionNumber
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Retrieves the error message associated with the specified error code.
        /// </summary>
        /// <param name="errorCode">The error code for which the error message is required.  </param>
        /// <returns>The error message description for the specified error code.</returns>
        public string get_ErrorMsg(int errorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets tile provider for the map.
        /// </summary>
        /// <remarks>This property is a shortcut for Tiles.Provider property. 
        /// Setting it to tkTileProvider.ProviderNone will change Tiles.Visible property to false.</remarks>
        /// \new491 Added in version 4.9.1
        public tkTileProvider TileProvider { get; set; }

        /// <summary>
        /// Returns coordinate of the shapefile point closest to specified point on screen.
        /// </summary>
        /// <param name="tolerance">Tolerance in screen pixels. If no vertex is found within the tolerance, operation fails.</param>
        /// <param name="xScreen">X coordinate on screen.</param>
        /// <param name="yScreen">Y coordinate on screen.</param>
        /// <param name="xFound">X coordinate of the vertex (map coordinates).</param>
        /// <param name="yFound">Y coordinate of the vertex (map coordinates).</param>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool FindSnapPoint(double tolerance, double xScreen, double yScreen, ref double xFound, ref double yFound)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Loads tiles for the specified map extents.
        /// </summary>
        /// <param name="extents">Extents to load tiles for (in map coordinates).</param>
        /// <param name="width">The width of the canvas on which the extents will be rendered.</param>
        /// <param name="key">Arbitrary key, to distinguish this request from others in AxMap.TilesLoaded event.</param>
        /// <remarks>If tiles aren't already loaded in cache, the operation will be performed asynchronously.
        /// Use AxMap.TilesLoaded event to be notified of its completion. The main purpose: loading of tiles
        /// for printing.</remarks>
        /// \new491 Added in version 4.9.1
        public bool LoadTilesForSnapshot(Extents extents, int width, string key)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Performs specific type of map redraw. 
        /// </summary>
        /// <param name="redrawType">Type of redraw.</param>
        /// <remarks>Different types of redraw are necessary for performance reasons. If it's known that
        /// only a specific part of map content has changed there is no need to run the full rendering, 
        /// screen buffer can be used instead. According to the amount of content being redrawn the different 
        /// types can be ordered like this (from minimum to the full one):  
        /// RedrawMinimal -> RedrawSkipAllLayers -> RedrawSkipDataLayers -> RedrawAll.
        /// Each subsequent type includes preceding types, so it's never necessary to call two types in a row. 
        /// If it's not clear which type of redraw is appropriate for particular situation,
        /// start with the minimal one and then go to the subsequent types until the changes 
        /// which were made are rendered. Also if datasource being used are small it's easier to ignore this method
        /// altogether and call AxMap.Redraw instead ( equivalent to AxMap.Redraw2(tkRedrawType.RedrawAll)). </remarks>
        /// \new491 Added in version 4.9.1
        public void Redraw2(tkRedrawType redrawType)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adveanced version of AxMap.Redraw2
        /// </summary>
        /// <param name="redrawType">Type of redraw.</param>
        /// <param name="reloadTiles">Reload the tiles, defaults to false when AxMap.Redraw2 is used.</param>
        public void Redraw3(tkRedrawType redrawType, bool reloadTiles)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets list actions performed by user via interactive ShapeEditor. Provides undo/redo capability.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public virtual UndoList UndoList
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets an Identifier object which holds settings of tkCursorMode.cmIdentify tools.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public virtual Identifier Identifier
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets instance of ShapeEditor class associated with map which stores data entered by interactive editing tools.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public virtual ShapeEditor ShapeEditor
        {
            get { throw new NotImplementedException(); }
        }

        #endregion

#pragma warning disable 1587
        /// The modules listed here are parts of the documentation of AxMap class.
        /// \dotfile mapgroups.dot
        /// <a href = "diagrams.html">Graph description</a>
        /// \addtogroup map_modules Map modules
        /// @{
#pragma warning restore 1587

        #region Map interaction
        /// \addtogroup map_interaction Map interaction
        /// Here is a list of properties and methods which affect the way a user interacts with the map. This module is a part of the documentation of AxMap class.
        /// \dot
        /// digraph map_interaction {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="AxMap" URL="\ref AxMap"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Map Interaction" URL="\ref map_interaction"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Gets or sets a boolean value which indicate whether map will be redrawn in the course of panning operation.
        /// </summary>
        /// <remarks>The default value is false. Setting this value to true can improve the user experience but 
        /// decrease the performance in case large layers are displayed.</remarks>
        /// \new48 Added in version 4.8
        public bool UseSeamlessPan
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the cursor mode for the map. 
        /// </summary>
        /// <remarks>The cursor mode determines how the map handles mouse click events on the map. The only mode not handled by the map is cmNone.</remarks>
        public tkCursorMode CursorMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets zoom behaviour for the map control.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public tkZoomBehavior ZoomBehavior { get; set; }

        /// <summary>
        /// Clears the extent history.
        /// </summary>
        public void ClearExtentHistory()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Zoomes map to display selected shapes of the specified shapefile.
        /// </summary>
        /// <param name="layerHandle">Layer handle of shapefile layer with selected shapes.</param>
        /// <returns>True on success.</returns>
        /// <remarks>If the layer isn't shapefile or there is no selected shapes in it, nothing will happen.</remarks>
        /// \new490 Added in version 4.9.0
        public bool ZoomToSelected(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Zooms map to specified zoom level of the active tile provider (Tiles.Provider).
        /// </summary>
        /// <param name="zoom">Zoom level</param>
        /// <returns>True if the zooming took place.</returns>
        /// \new491 Added in version 4.9.1
        public bool ZoomToTileLevel(int zoom)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Zooms to World extents (-180.0; 180.0; -90.0; 90.0) 
        /// </summary>
        /// <remarks>Projection should be specified for map in order for this methods to work.</remarks>
        /// <returns>True on success.</returns>
        /// \new490 Added in version 4.9.0
        public bool ZoomToWorld()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Checks to see if the map is currently locked or not.
        /// </summary>
        public tkLockMode IsLocked
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Locks the window so that any changes will not be displayed until it is unlocked. 
        /// </summary>
        /// <remarks>This is very useful if you are making a large number of changes at once and don't want the map to be redrawn between each change.
        /// The changes of drawing options in the related classes (Shapefile, Labels, etc.) won't cause the redraw of the map even if it's not locked.</remarks>
        /// <param name="lockMode">The state of the lock mode.</param>
        public void LockWindow(tkLockMode lockMode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether a wait cursor will be displayed on map redraw.
        /// </summary>
        public bool DisableWaitCursor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the speed of zooming with mouse wheel.
        /// </summary>
        /// <remarks>The default values is 0.5. The values in 0.1 - 10.0 range are accepted. The value 1.0 will turn off
        /// the zooming by mouse wheel.</remarks>
        public double MouseWheelSpeed
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets whether the map sends mouse down events. 
        /// </summary>
        public bool SendMouseDown
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets whether the map sends mouse move events. 
        /// </summary>
        public bool SendMouseMove
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets whether the map sends mouse up events.
        /// </summary>
        public bool SendMouseUp
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether the map sends on draw back buffer events.
        /// </summary>
        public bool SendOnDrawBackBuffer
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets whether the map sends the SelectBoxDrag event.
        /// </summary>
        public bool SendSelectBoxDrag
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets whether the map sends the SelectBoxFinal event. 
        /// </summary>
        public bool SendSelectBoxFinal
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which can affect the displaying of the context menus in client code by the right click.
        /// </summary>
        public bool TrapRMouseDown
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Zooms the display in by the given factor.
        /// </summary>
        /// <param name="percent">A number between 0.0 and 1.0, being the factor to zoom in by.</param>
        public void ZoomIn(double percent)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Zooms the display out by the specified factor.
        /// </summary>
        /// <param name="percent">A number between 0.0 and 1.0, being the factor to zoom out by.</param>
        public void ZoomOut(double percent)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the factor by which to zoom the view of the map in or out.
        /// </summary>
        public double ZoomPercent
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Zooms the map display to the specified layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer to zoom to.</param>
        public void ZoomToLayer(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Zooms the map to the maximum extents of all loaded layers.  
        /// </summary>
        /// <remarks>Layers which are not visible are still used to compute maximum extents.</remarks>
        public void ZoomToMaxExtents()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Zooms the map to the maximum extents of all loaded visible layers.
        /// </summary>
        /// <remarks>Layers which are not visible are not used to compute maximum extents.</remarks>
        public void ZoomToMaxVisibleExtents()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Zooms the map view to the previous extents if there are previous extents in the extents history.
        /// </summary>
        /// <returns>Returns the number of extents left in the extents history after zooming to previous extents.</returns>
        public int ZoomToPrev()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Zooms the map view to the next extents if there are next extents in the extents history.
        /// </summary>
        /// <returns>Returns the number of extents left in the extents history after zooming to previous extents.</returns>
        public int ZoomToNext()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Zooms the map display to the specified shape in the shapefile contained by the specified layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer containing the shape to zoom to.</param>
        /// <param name="shape">The index of the shape to zoom to.</param>
        public void ZoomToShape(int layerHandle, int shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a value indicating whether zoom bar will be displayed on the map.
        /// </summary>
        /// <remarks>Zoom bar can be displayed only if projection of the map is set. Zoom levels
        /// depend upon tile of the current tile provider.</remarks>
        /// /see AxMap.TileProvider, AxMap.Projection
        /// \new492 Added in version 4.9.2
        public bool ShowZoomBar { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether animation will be displayed zooming map in or out.
        /// </summary>
        /// <remarks>Default csAuto value will display animation only if there are no data layers
        /// within visisble extents. Animation also won't be displayed if there are more than 4
        /// zoom levels between current and target level.</remarks>
        /// \new492 Added in version 4.9.2
        public tkCustomState AnimationOnZooming { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether animation will be displayed to simulate 
        /// inertia after panning operation.
        /// </summary>
        /// <remarks>Default csAuto value will display animation only if there are no data layers
        /// within visisble extents.
        /// \note Mouse panning is somewhat more tricky than the one with smartphone's touch screen,
        /// so some practice may be needed to learn not to stop mouse motion before releasing the button )))
        /// </remarks>
        /// \new492 Added in version 4.9.2
        public tkCustomState InertiaOnPanning { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether currently rendered tile buffer should be scaled 
        /// and reused during the next redraw.
        /// </summary>
        /// \new492 Added in version 4.9.2
        public bool ReuseTileBuffer { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether the legacy 'hand' cursor should be used
        /// for map panning rather than the standard 'NSEW' four-point cursor.
        /// </summary>
        /// \new495 Added in version 4.9.5
        public bool UseAlternatePanCursor { get; set; }

        /// <summary>
        /// Gets or sets the amount of information to be displayed in zoom bar tool tip.
        /// </summary>
        /// \new492 Added in version 4.9.2
        public tkZoomBarVerbosity ZoomBarVerbosity { get; set; }

        /// <summary>
        /// Gets or sets a style of zoom box, which is used to select a region with Zoom In tool.
        /// </summary>
        /// \new492 Added in version 4.9.2
        public tkZoomBoxStyle ZoomBoxStyle { get; set; }

        /// <summary>
        /// Gets or sets minimum zoom level to be used by zoom bar.
        /// </summary>
        /// <remarks>This property won't increase the number of available levels, 
        /// if certain levels aren't supported by the provider they still won't be displayed.</remarks>
        /// \new492 Added in version 4.9.2
        public int ZoomBarMinZoom { get; set; }

        /// <summary>
        /// Gets or sets maximum zoom level to be used by zoom bar.
        /// </summary>
        /// <remarks>This property won't increase the number of available levels, 
        /// if certain levels aren't supported by the provider they still won't be displayed.</remarks>
        /// \new492 Added in version 4.9.2
        public int ZoomBarMaxZoom { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether to move the mouse click point to the center of the map 
        /// when using the Zoom In or Zoom Out tools.
        /// </summary>
        /// <remarks>The default value is False, indicating that the map is not recentered,  
        /// but instead keeps the click point at the mouse position and zooms around that point.</remarks>
        /// \new495 Added in version 4.9.5
        public bool RecenterMapOnZoom { get; set; }

        /// <summary>
        /// Reverts the last operation performed by user in interactive ShapeEditor.
        /// </summary>
        /// <remarks>This method is preferable over AxMap.UndoList.Undo since it can 
        /// also undo point added during creation of new shape, which are not registered in the undo list.</remarks>
        /// \new493 Added in version 4.9.3
        public virtual void Undo()
        {
            throw new NotImplementedException();
        }

#pragma warning disable 1587
        /// @}
#pragma warning restore 1587
        #endregion

        #region Map serialization
        /// \addtogroup map_serialization Map serialization
        /// Here is a list of properties and methods to preserve the state of the map and particular layers. This module is a part of the documentation of AxMap class.
        /// \dot
        /// digraph map_serialization {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="AxMap" URL="\ref AxMap"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Map Serialization" URL="\ref map_serialization"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Serializes the layer settings to the string.
        /// </summary>
        /// <remarks>The serialized string will not include information about the data source. Therefore
        /// the same options can later be applied to any data layer of the same type (either shapefile or image).</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>The string with serialized layer settings.</returns>
        /// \new48 Added in version 4.8
        public string SerializeLayer(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restores the state of the layer from the string generated by AxMap.SerializeLayer().
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newVal">The string to restore values from.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool DeserializeLayer(int layerHandle, string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Serializes map state to the string.
        /// </summary>
        /// <param name="relativePaths">A value which indicates whether relative or absolute paths to data sources will be used.</param>
        /// <param name="basePath">The path to the base folder starting from which relative paths will be calculated.</param>
        /// <returns>The string with the serialized map state.</returns>
        /// \new48 Added in version 4.8
        public string SerializeMapState(bool relativePaths, string basePath)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restores map state from the string generated by AxMap.SerializeMapState.
        /// </summary>
        /// <remarks>This method will remove all the existing layers and load the layers specified in the string.</remarks>
        /// <param name="state">The string with serialized state.</param>
        /// <param name="loadLayers">A boolean value which indicates whether layers or only options of the map control will restored.</param>
        /// <param name="basePath">The path to the base folder starting from which relative paths were calculated.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool DeserializeMapState(string state, bool loadLayers, string basePath)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Applies to the layer a set of settings from file with the specified name.
        /// </summary>
        /// <remarks>The options will be loaded from the file with the name [LayerName].[OptionsName].mwsymb</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="optionsName">The name of the set of options.</param>
        /// <param name="description">The output string which will hold the description of the options stored in file.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool LoadLayerOptions(int layerHandle, string optionsName, ref string description)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes set of options for the layer with the specified name.
        /// </summary>
        /// <remarks>The options are stored in files with the name [LayerName].[OptionsName].mwsymb</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="optionsName">The name of the set of options.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool RemoveLayerOptions(int layerHandle, string optionsName)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves settings of the layer to the file.
        /// </summary>
        /// <remarks>The options are saved to the file with name [LayerName].[OptionsName].mwsymb</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="optionsName">The name which will be attached to the set of options.</param>
        /// <param name="overwrite">A value which indicates whether the existing file will be overwritten.</param>
        /// <param name="description">The description associated with the set of options.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool SaveLayerOptions(int layerHandle, string optionsName, bool overwrite, string description)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Loads the state of the map from the file created by AxMap.SaveMapState method.
        /// </summary>
        /// <param name="filename">The name of the file.</param>
        /// <param name="callback">A callback object to return information about progress and errors.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool LoadMapState(string filename, object callback)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the state of the map to the specified file.
        /// </summary>
        /// <remarks>Use AxMap.LoadState for reverse operation.</remarks>
        /// The serialized state include:
        /// - properties of the control;
        /// - references to the layers;
        /// - drawing options for the layers.
        /// .
        /// <param name="filename">The name of the file.</param>
        /// <param name="relativePaths">A value which indicates whether absolute or relative paths should be used.</param>
        /// <param name="overwrite">A value which indicates whether the exiting file will be overwritten.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool SaveMapState(string filename, bool relativePaths, bool overwrite)
        {
            throw new NotImplementedException();
        }

#pragma warning disable 1587
        /// @}
#pragma warning restore 1587
        #endregion

        #region Layer properties
        /// \addtogroup map_layer_properties Layer properties
        /// Here is a list of properties common to all data layers. This module is a part of the documentation of AxMap class.
        /// \dot
        /// digraph map_layer_properties {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="AxMap" URL="\ref AxMap"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Layer Properties" URL="\ref map_layer_properties"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Gets maximum zoom at which the layer will be displayed.
        /// </summary>
        /// <param name="layerHandle"></param>
        /// <returns></returns>
        /// <remarks>It's analogous to get_LayerMaxVisibleScale, only scale specified as zoom level 
        /// of the current tile provider (Map.Tiles.Provider).</remarks>
        /// \new491 Added in version 4.9.1
        public int get_LayerMaxVisibleZoom(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets maximum zoom at which the layer will be displayed.
        /// </summary>
        /// <param name="layerHandle">Layer handle.</param>
        /// <param name="param0">The new value of max zoom.</param>
        /// <remarks>It's analogous to set_LayerMaxVisibleScale, only scale specified as zoom level 
        /// of the current tile provider (Map.Tiles.Provider).</remarks>
        /// \new491 Added in version 4.9.1
        public void set_LayerMaxVisibleZoom(int layerHandle, int param0)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets filename of datasource for the specified layer.
        /// </summary>
        /// <param name="layerHandle">Layer handle.</param>
        /// <returns>Filename of the layer including path.</returns>
        /// \new490 Added in version 4.9.0
        public string get_LayerFilename(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets minimum zoom at which the layer will be displayed.
        /// </summary>
        /// <param name="layerHandle"></param>
        /// <returns></returns>
        /// <remarks>It's analogous to get_LayerMinVisibleScale, only scale specified as zoom level 
        /// of the current tile provider (Map.Tiles.Provider).</remarks>
        /// \new491 Added in version 4.9.1
        public int get_LayerMinVisibleZoom(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets minimum zoom at which the layer will be displayed.
        /// </summary>
        /// <param name="layerHandle">Layer handle.</param>
        /// <param name="param0">The new value of max zoom.</param>
        /// <remarks>It's analogous to set_LayerMinVisibleScale, only scale specified as zoom level 
        /// of the current tile provider (Map.Tiles.Provider).</remarks>
        /// \new491 Added in version 4.9.1
        public void set_LayerMinVisibleZoom(int layerHandle, int param0)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the description of the layer.
        /// </summary>
        /// <remarks>The information will be stored in .mwsymb file associated with data layer.</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newValue">The description of the layer.</param>
        /// \new48 Added in version 4.8
        public void set_LayerDescription(int layerHandle, string newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the description of the layer from the associated .mwsymb file.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>The string with description.</returns>
        /// \new48 Added in version 4.8
        public string get_LayerDescription(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the boolean value which indicates whether the layer will be displayed at all scales or only within the chosen range of scales.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>True in case the layer will be displayed between specified scales only.</returns>
        /// \see AxMap.get_LayerMaxVisibleScale, AxMap.get_LayerMinVisibleScale
        /// \new48 Added in version 4.8
        public bool get_LayerDynamicVisibility(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the boolean value which indicates whether the layer will be displayed at all scales or only within the chosen range of scales.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newValue">True in case the layer will be displayed between specified scales only.</param>
        /// \see set_LayerMaxVisibleScale, set_LayerMinVisibleScale
        /// \new48 Added in version 4.8
        public void set_LayerDynamicVisibility(int layerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a string associated with a layer in the map. 
        /// </summary>
        /// <remarks>This string can be used to store any string data desired by the developer.</remarks>
        /// <param name="layerHandle">The handle for the layer for which the layer key is to be set.</param>
        /// <returns>The layer key for the specified layer in string format.</returns>
        public string get_LayerKey(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a string associated with a layer in the map. 
        /// </summary>
        /// <remarks>This string can be used to store any string data desired by the developer.</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newValue">The string to be associated with layer.</param>
        public void set_LayerKey(int layerHandle, string newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns labels associated with layer.
        /// </summary>
        /// <remarks>Labels can also be obtained by using Shapefile.Labels and Image.Labels.</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>The labels associated with the layer.</returns>
        /// \new48 Added in version 4.8
        public Labels get_LayerLabels(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the labels associated with the layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newValue">The new instance of the Labels class to be associated with the layer.</param>
        /// \new48 Added in version 4.8
        public void set_LayerLabels(int layerHandle, Labels newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the maximum scale at which the layer is visible on the map.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>The maximum visible scale for the layer.</returns>
        /// \new48 Added in version 4.8
        public double get_LayerMaxVisibleScale(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the maximum scale at which the layer is visible on the map.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newValue">The maximum visible scale for the layer.</param>
        /// \new48 Added in version 4.8
        public void set_LayerMaxVisibleScale(int layerHandle, double newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the minimum scale at which the layer is visible on the map.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>The minimum visible scale for the layer.</returns>
        /// \new48 Added in version 4.8
        public double get_LayerMinVisibleScale(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the minimum scale at which the layer is visible on the map.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newValue">The minimum visible scale for the layer.</param>
        /// \new48 Added in version 4.8
        public void set_LayerMinVisibleScale(int layerHandle, double newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the name of the specified layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer for which the name is to be set.</param>
        /// <returns>The layer name for the specified layer. </returns>
        public string get_LayerName(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the name of the specified layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer for which the name is to be set.</param>
        /// <param name="newValue">The name to be set for the layer.</param>
        public void set_LayerName(int layerHandle, string newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a boolean value which indicates whether the layer will be serialized by appropriate routines.
        /// </summary>
        /// <remarks>This property affects AxMap.SaveMapState, AxMap.SerializeMapState.</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>True in case the layer will be excluded from serialization and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool get_LayerSkipOnSaving(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a boolean value which indicates whether the layer will be serialized by appropriate routines.
        /// </summary>
        /// <remarks>This property affects AxMap.SaveMapState, AxMap.SerializeMapState.</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newValue">True in case the layer will be excluded from serialization and false otherwise.</param>
        /// \new48 Added in version 4.8
        public void set_LayerSkipOnSaving(int layerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the visibility of the specified layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer for which the visibility is being set. </param>
        /// <returns>A boolean value representing whether the layer is to be visible or not.</returns>
        public bool get_LayerVisible(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the visibility of the specified layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer for which the visibility is returned. </param>
        /// <param name="newValue">A boolean value representing whether the layer is to be visible or not.</param>
        public void set_LayerVisible(int layerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a color scheme for the specified layer.
        /// </summary>
        /// <param name="layerHandle">The layer handle of the layer for which the coloring scheme is required</param>
        /// <returns>The color scheme for the specified layer</returns>
        public object GetColorScheme(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the color scheme of an image layer
        /// </summary>
        /// <param name="layerHandle">The handle if the layer the color scheme needs to set</param>
        /// <param name="colorScheme">The color scheme</param>
        /// <returns>True on success</returns>
        public bool SetImageLayerColorScheme(int layerHandle, object colorScheme)
        {
            throw new NotImplementedException();
        }

#pragma warning disable 1587
        /// @}
#pragma warning restore 1587

        #endregion

        #region Layer management
        /// \addtogroup map_layer_management Layer management
        /// Here is a list of properties and methods which allow to work with layers of the map. This module is a part of the documentation of AxMap class.
        /// \dot
        /// digraph map_layer {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="AxMap" URL="\ref AxMap"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Layer Management" URL="\ref map_layer_management"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Removes all data from the map and sets its properties to the default state.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public void Clear()
        {
            throw new NotImplementedException();
        }

#pragma warning disable 1570
        /// <summary>
        /// Adds a layer from spatial database accessed via GDAL/OGR drivers.
        /// </summary>
        /// <remarks>This method will search if there is a layer with specified name in the datasource,
        /// and if so will open it. Otherwise it will run OgrDatasource.RunQuery method with provided
        /// layerNameOrQuery argument. 
        /// Additional information on failure can be obtained through AxMap.FileManager property.</remarks>
        /// <param name="connectionString">Connection string. See details for particular formats <a href ="http://www.gdal.org/ogr_formats.html">here</a>.</param>
        /// <param name="layerNameOrQuery">Name of the layer (i.e. database table) or SQL query.</param>
        /// <param name="visible">A value indicating whether a new layer will be visible.</param>
        /// <returns>Handle of the newly added layer or -1 on failure.</returns>
        /// The following example opens a temporary layer by querying PostGIS datatabase,
        /// adds the layer to the map and then accesses its data.
        /// \code
        /// private static string CONNECTION_STRING = "PG:host=localhost dbname=london user=postgres password=1234";
        ///  
        /// string sqlOrLayerName = "SELECT * FROM Buildings WHERE gid < 50";    
        /// int handle = map.AddLayerFromDatabase(CONNECTION_STRING, sqlOrLayerName, true);
        /// if (handle == -1)
        /// {
        ///     Debug.Print("Failed to open layer: " + map.FileManager.get_ErrorMsg(map.FileManager.LastErrorCode));
        /// 
        ///     // in case the reason of failure is still unclear, let's ask GDAL for details
        ///     var gs = new GlobalSettings();
        ///     Debug.Print("Last GDAL error: " + gs.GdalLastErrorMsg);
        /// }
        /// else
        /// {
        ///     // now let's access the opened layer
        ///     var l = map.get_OgrLayer(handle);
        ///     if (l != null)
        ///     {
        ///         Debug.Print("Number of features: " + l.FeatureCount);
        /// 
        ///         // no access the data
        ///         var sf = l.GetBuffer();
        ///         Debug.Print("Number of shapes: " + sf.NumShapes);
        ///     }
        /// }
        /// \endcode
        /// \new493 Added in version 4.9.3
        public int AddLayerFromDatabase(string connectionString, string layerNameOrQuery, bool visible)
        {
            throw new NotImplementedException();
        }
#pragma warning restore 1570

        /// <summary>
        /// Adds layer from the specified datasource.
        /// </summary>
        /// <remarks>Additional information on failure can be obtained through AxMap.FileManager property. 
        /// </remarks>
        /// <param name="filename">Filename of datasource</param>
        /// <param name="openStrategy">Open strategy (fosAutoDetect the default recommended value).</param>
        /// <param name="visible">A value indicating whether a new layer will be visible.</param>
        /// <returns>Handle of the newly added layer or -1 on failure.</returns>
        /// \new491 Added in version 4.9.1
        public int AddLayerFromFilename(string filename, tkFileOpenStrategy openStrategy, bool visible)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds a layer to the map.
        /// </summary>
        /// <remarks>In case of OgrDatasource all layers will be added to the map. For grid datasources 
        /// an instance of Image class created with Grid.OpenAsImage will be used for rendering.</remarks>
        /// <param name="Object">The object to add to the map. The following types are supported: 
        /// Shapefile, Image, Grid, OgrLayer, OgrDatasource.</param>
        /// <param name="visible">Sets whether the layer is visible after being added to the map.</param>
        /// <returns>Returns the integer handle for the layer added to the map.</returns>
        public int AddLayer(object Object, bool visible)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the handle of the layer at the given position in the map. Returns -1 if there is no layer at the specified position
        /// </summary>
        /// <param name="layerPosition">The position of the layer for which the layer handle is required.</param>
        /// <returns>The layer handle for the layer at the specified position.</returns>
        public int get_LayerHandle(int layerPosition)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the position of the specified layer in the map.
        /// </summary>
        /// <param name="layerHandle">The layer handle of the layer for which the layer position is required.</param>
        /// <returns>The layer position of the specified layer in the map.</returns>
        public int get_LayerPosition(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves a layer in the map from the initial position to a target position.
        /// </summary>
        /// <param name="initialPosition">The initial position of the layer to be moved.</param>
        /// <param name="targetPosition">The final position of the layer being moved.</param>
        /// <returns></returns>
        public bool MoveLayer(int initialPosition, int targetPosition)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves the specified layer below all other layers.
        /// </summary>
        /// <param name="initialPosition">The position of the layer to move to the bottom</param>
        /// <returns>Boolean value representing success when true, and failure when false.</returns>
        public bool MoveLayerBottom(int initialPosition)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves the specified layer down one layer in the map.
        /// </summary>
        /// <param name="initialPosition">The position of the layer to be moved.</param>
        /// <returns>Boolean value representing success when true, and failure when false.</returns>
        public bool MoveLayerDown(int initialPosition)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves the specified layer to the top of all other layers.
        /// </summary>
        /// <param name="initialPosition">The position of the layer to be moved.</param>
        /// <returns>Boolean value representing success when true, failure when false.</returns>
        public bool MoveLayerTop(int initialPosition)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves the specified layer up one layer in the map.
        /// </summary>
        /// <param name="initialPosition">The initial position of the layer to be moved.</param>
        /// <returns>Boolean value representing success when true, failure when false.</returns>
        public bool MoveLayerUp(int initialPosition)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of layers loaded in the map.
        /// </summary>
        public int NumLayers
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Removes all layers from the map.
        /// </summary>
        public void RemoveAllLayers()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Changes the data source for the specified layer without its closing.
        /// </summary>
        /// <remarks>The new data source must be of the same type, i.e. either Shapefile or Image.</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newSrcPath">The the name of the new data source.</param>
        public void ReSourceLayer(int layerHandle, string newSrcPath)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Performs the same operation as OgrLayer.ReloadFromSource, then compares 
        /// projections with the Map, and if the settings allow, will reproject on-the-fly.
        /// </summary>
        /// <param name="ogrLayerHandle">The handle of the layer.  Must be an OGR layer.</param>
        /// <returns>
        /// True if successful; False if not an OGR Layer, could not be reloaded, or could not be reprojected (if necessary)
        /// </returns>
        /// <remarks>
        /// Starting with version 5.2.0, this function will also attempt to maintain the current 
        /// set of Hidden and Selected shapes within the associated Shapefile.
        /// </remarks>
        /// \new52 Updated in version 5.2
        public bool ReloadOgrLayerFromSource(int ogrLayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restarts the background loading thread of dynamically loaded layers
        /// </summary>
        /// <param name="ogrLayerHandle">The handle of the layer. Must be an OGR layer.</param>
        /// \new510 Adding in v5.1.0
        public void RestartBackgroundLoading(int ogrLayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Allow to initiate digitizing a new shape with a starting point
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <returns>True on success</returns>
        /// \new510 Added in v5.1.0
        public bool StartNewBoundShape(double x, double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Allow to initiate digitizing a new shape with a starting point
        /// </summary>
        /// <param name="layerHandle"></param>
        /// <returns>True on success</returns>
        /// \new510 Added in v5.1.0
        public bool StartNewBoundShapeEx(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes the specified layer from the map
        /// </summary>
        /// <param name="layerHandle">The handle of the layer to be removed from the map.</param>
        public void RemoveLayer(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes layer from the map without its closing.
        /// </summary>
        /// <remarks>Shapefile.Close or Image.Close won't be called while removing the layer.</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        public void RemoveLayerWithoutClosing(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets image object associated with the layer.
        /// </summary>
        /// <remarks>This method along with AxMap.get_Shapefile() can used to substitute AxMap.get_GetObject().</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>The reference to the image or NULL reference in case of invalid handle or wrong layer type.</returns>
        /// \new48 Added in version 4.8
        public Image get_Image(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Replaces the image object associated with the layer.
        /// </summary>
        /// <remarks>The old image will not be closed.</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newValue">The new image object.</param>
        /// \new48 Added in version 4.8
        public void set_Image(int layerHandle, Image newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets shapefile object associated with the layer.
        /// </summary>
        /// <remarks>This method along with AxMap.get_Image() can be used to substitute AxMap.get_GetObject().</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>The reference to the shapefile or NULL reference in case of invalid handle or wrong layer type.</returns>
        /// \new48 Added in version 4.8
        public Shapefile get_Shapefile(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Replaces the shapefile object associated with the layer.
        /// </summary>
        /// <remarks>The old shapefile will not be closed.</remarks>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <param name="newValue">The new shapefile object.</param>
        /// \new48 Added in version 4.8
        public void set_Shapefile(int layerHandle, Shapefile newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the layer object with the given handle. The object could be a Shapefile, Grid, or Image object.
        /// </summary>
        /// <remarks>For OGR layers this method will return underlying shapefile from OgrLayer.GetBuffer(). 
        /// Use AxMap.get_OgrLayer to access instance of OgrLayer itself.</remarks>
        /// <param name="layerHandle">The handle of the layer to be retrieved.</param>
        /// <returns>A Shapefile, Grid, or Image object.</returns>
        public object get_GetObject(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets instance of OGR layer object associated with the specified layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>OGR layer or null in case of invalid layer index or wrong layer type.</returns>
        /// \new493 Added in version 4.9.3
        public OgrLayer get_OgrLayer(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the WmsLayer
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>The WmsLayer object</returns>
        public WmsLayer get_WmsLayer(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the layer extents
        /// </summary>
        /// <param name="layerHandle">The handle of the layer.</param>
        /// <returns>The extents</returns>
        /// \new510 Added in version 5.1.0
        public Extents get_layerExtents(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a value which indicates whether a layer is visible at current map scale.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer.</param>
        /// <returns>True in case the layer is visible.</returns>
        /// <remarks>The property checks whether or not the layer is hidden because of AxMap.LayerDynamicVisibility property.
        /// However it doesn't check that the layer has any objects within the current map extents.</remarks>
        /// \new493 Added in version 4.9.3
        public bool get_LayerVisibleAtCurrentScale(int layerHandle)
        {
            throw new NotImplementedException();
        }

#pragma warning disable 1587
        /// @}
#pragma warning restore 1587
        #endregion

        #region Drawing layer
        /// \addtogroup map_drawing_layers Drawing layers
        /// Here is a list of methods and properties to interact with the drawing layers of the map.
        /// The drawing layers are more transient that the standard layers, intended for fast drawing of temporary elements on top of the map.
        /// This module is a part of the documentation of AxMap class.
        /// \dot
        /// digraph map_drawing_layers {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="AxMap" URL="\ref AxMap"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Drawing Layers" URL="\ref map_drawing_layers"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Replaces the labels associated with the drawing layer.
        /// </summary>
        /// <param name="drawingLayerIndex">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="newValue">The new instance of the Labels class to associate with the layer.</param>
        public void set_DrawingLabels(int drawingLayerIndex, Labels newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets labels associated with the drawing layer.
        /// </summary>
        /// <param name="drawingLayerIndex">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <returns>The reference to the Labels class or NULL reference on the invalid handle.</returns>
        public Labels get_DrawingLabels(int drawingLayerIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all drawings on the specified drawing layer, and removes the drawing layer. The drawing handle will no longer be valid.
        /// Call AxMap.NewDrawing again to create a new drawing layer to continue adding new elements.
        /// </summary>
        /// <param name="drawHandle">Drawing handle of the drawing layer to be cleared (and removed).</param>
        public void ClearDrawing(int drawHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all drawings on all drawing layers, and removes all drawing layers.
        /// This method is slower than using ClearDrawing on a specific layer.
        /// Call AxMap.NewDrawing again to create a new drawing layer to continue adding new elements.
        /// </summary>
        public void ClearDrawings()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a circle on the last drawing layer created by AxMap.NewDrawing
        /// </summary>
        /// <param name="x">Center x coordinate for the circle to be drawn.</param>
        /// <param name="y">Center y coordinate for the circle to be drawn.</param>
        /// <param name="pixelRadius">Radius in pixels of the circle to be drawn.</param>
        /// <param name="color">Color of the circle to be drawn. This is a UInt32 representation of an RGB color.</param>
        /// <param name="fill">Boolean value which determines whether the circle will be drawn with a fill or not.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawCircle(double x, double y, double pixelRadius, uint color, bool fill, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a circle on the specified drawing layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the drawing layer created with AxMap.NewDrawing call.</param>
        /// <param name="x">Center x coordinate for the circle to be drawn.</param>
        /// <param name="y">Center y coordinate for the circle to be drawn.</param>
        /// <param name="pixelRadius">Radius in pixels of the circle to be drawn.</param>
        /// <param name="color">Color of the circle to be drawn. This is a UInt32 representation of an RGB color.</param>
        /// <param name="fill">Boolean value which determines whether the circle will be drawn with a fill or not.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawCircleEx(int layerHandle, double x, double y, double pixelRadius, uint color, bool fill, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a line on the last drawing layer created using AxMap.NewDrawing.
        /// </summary>
        /// <param name="x1">X coordinate of the first point used to draw the line</param>
        /// <param name="y1">Y coordinate of the first point used to draw the line.</param>
        /// <param name="x2">X coordinate of the second point used to draw the line.</param>
        /// <param name="y2">Y coordinate of the second point used to draw the line.</param>
        /// <param name="pixelWidth">Width of the line in pixels.</param>
        /// <param name="color">Color to draw the line with. This is a UInt32 representation of an RGB value.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawLine(double x1, double y1, double x2, double y2, int pixelWidth, uint color, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a line on the specified drawing layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the drawing layer created with AxMap.NewDrawing call.</param>
        /// <param name="x1">X coordinate of the first point used to draw the line</param>
        /// <param name="y1">Y coordinate of the first point used to draw the line.</param>
        /// <param name="x2">X coordinate of the second point used to draw the line.</param>
        /// <param name="y2">Y coordinate of the second point used to draw the line.</param>
        /// <param name="pixelWidth">Width of the line in pixels.</param>
        /// <param name="color">Color to draw the line with. This is a UInt32 representation of an RGB value.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawLineEx(int layerHandle, double x1, double y1, double x2, double y2, int pixelWidth, uint color, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a point on the last drawing layer created by AxMap.NewDrawing.
        /// </summary>
        /// <param name="x">The x coordinate of the point to draw</param>
        /// <param name="y">The y coordinate of the point to draw.</param>
        /// <param name="pixelSize">The size in pixels of the point to be drawn.</param>
        /// <param name="color">The color of the point to be drawn. This is a UInt32 representation of an RGB color.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawPoint(double x, double y, int pixelSize, uint color, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a point on the specified drawing layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the drawing layer created with AxMap.NewDrawing call.</param>
        /// <param name="x">The x coordinate of the point to draw</param>
        /// <param name="y">The y coordinate of the point to draw.</param>
        /// <param name="pixelSize">The size in pixels of the point to be drawn.</param>
        /// <param name="color">The color of the point to be drawn. This is a UInt32 representation of an RGB color.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawPointEx(int layerHandle, double x, double y, int pixelSize, uint color, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a polygon on the last drawing layer created using AxMap.NewDrawing.
        /// </summary>
        /// <param name="xPoints">An array containing x-coordinates for each point in the polygon.</param>
        /// <param name="yPoints">An array containing y-coordinates for each point in the polygon.</param>
        /// <param name="numPoints">The number of points in the polygon.</param>
        /// <param name="color">The color to use when drawing the polygon. This is a UInt32 representation of an RGB color.</param>
        /// <param name="fill">A boolean value representing whether the polygon is drawn with a fill or not.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawPolygon(ref object xPoints, ref object yPoints, int numPoints, uint color, bool fill, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a polygon on the specified drawing layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the drawing layer created with AxMap.NewDrawing call.</param>
        /// <param name="xPoints">An array containing x-coordinates for each point in the polygon.</param>
        /// <param name="yPoints">An array containing y-coordinates for each point in the polygon.</param>
        /// <param name="numPoints">The number of points in the polygon.</param>
        /// <param name="color">The color to use when drawing the polygon. This is a UInt32 representation of an RGB color.</param>
        /// <param name="fill">A boolean value representing whether the polygon is drawn with a fill or not.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawPolygonEx(int layerHandle, ref object xPoints, ref object yPoints, int numPoints, uint color, bool fill, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a circle with custom outline width on the last drawing layer created by AxMap.NewDrawing.
        /// </summary>
        /// <param name="x">Center x coordinate for the circle to be drawn.</param>
        /// <param name="y">Center y coordinate for the circle to be drawn.</param>
        /// <param name="pixelRadius">Radius in pixels of the circle to be drawn.</param>
        /// <param name="color">Color of the circle to be drawn. This is a UInt32 representation of an RGB color.</param>
        /// <param name="fill">Boolean value which determines whether the circle will be drawn with a fill or not.</param>
        /// <param name="width">The width of the outline.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>/// 
        public void DrawWideCircle(double x, double y, double pixelRadius, uint color, bool fill, short width, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a circle with custom outline width on the specified drawing layer.
        /// </summary>
        /// <param name="layerHandle">The handle of the drawing layer created with AxMap.NewDrawing call.</param>
        /// <param name="x">Center x coordinate for the circle to be drawn.</param>
        /// <param name="y">Center y coordinate for the circle to be drawn.</param>
        /// <param name="radius">Radius in pixels of the circle to be drawn.</param>
        /// <param name="color">Color of the circle to be drawn. This is a UInt32 representation of an RGB color.</param>
        /// <param name="fill">Boolean value which determines whether the circle will be drawn with a fill or not.</param>
        /// <param name="outlineWidth">The width of the outline.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawWideCircleEx(int layerHandle, double x, double y, double radius, uint color, bool fill, short outlineWidth, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a polygon with custom width of outline on the last drawing layer created using AxMap.NewDrawing.
        /// </summary>
        /// <param name="xPoints">An array containing x-coordinates for each point in the polygon.</param>
        /// <param name="yPoints">An array containing y-coordinates for each point in the polygon.</param>
        /// <param name="numPoints">The number of points in the polygon.</param>
        /// <param name="color">The color to use when drawing the polygon. This is a UInt32 representation of an RGB color.</param>
        /// <param name="fill">A boolean value representing whether the polygon is drawn with a fill or not.</param>
        /// <param name="width">The width of the outline.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawWidePolygon(ref object xPoints, ref object yPoints, int numPoints, uint color, bool fill, short width, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a polygon with custom width of outline on the last drawing layer created using AxMap.NewDrawing.
        /// </summary>
        /// <param name="layerHandle">The handle of the drawing layer created with AxMap.NewDrawing call.</param>
        /// <param name="xPoints">An array containing x-coordinates for each point in the polygon.</param>
        /// <param name="yPoints">An array containing y-coordinates for each point in the polygon.</param>
        /// <param name="numPoints">The number of points in the polygon.</param>
        /// <param name="color">The color to use when drawing the polygon. This is a UInt32 representation of an RGB color.</param>
        /// <param name="fill">A boolean value representing whether the polygon is drawn with a fill or not.</param>
        /// <param name="outlineWidth">The width of the outline.</param>
        /// <param name="alpha">Byte value which determines the alpha channel of the color.</param>
        public void DrawWidePolygonEx(int layerHandle, ref object xPoints, ref object yPoints, int numPoints, uint color, bool fill, short outlineWidth, byte alpha = 255)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new drawing layer on the map returning its handle.
        /// </summary>
        /// <param name="projection">Sets the coordinate system to use for the new drawing layer to be created. (tkDrawReferenceList.dlScreenReferencedList 
        /// uses pixels in screen coordinates. tkDrawReferenceList.dlSpatiallyReferencedList uses projected map units.)</param>
        /// <returns>The handle for the new drawing layer in the map.</returns>
        public int NewDrawing(tkDrawReferenceList projection)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the drawing key which may be used by the programmer to store any string for a drawing layer with specified handle.
        /// </summary>
        /// <param name="drawHandle">The handle of the drawing layer.</param>
        /// <returns>The string associated with the layer.</returns>
        public string get_DrawingKey(int drawHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the drawing key may be used by the programmer to store any string for a drawing layer with specified handle.
        /// </summary>
        /// <param name="drawHandle">The handle of the drawing layer.</param>
        /// <param name="newValue">The string associated with the layer.</param>
        public void set_DrawingKey(int drawHandle, string newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the boolean value which indicates whether the labels of the drawing layer are visible.
        /// </summary>
        /// <param name="layerHandle">The handle of the drawing layer.</param>
        /// <param name="visible">A boolean value which indicates whether the labels are visible.</param>
        public void SetDrawingLayerVisible(int layerHandle, bool visible)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears labels on the specified drawing layer.
        /// </summary>
        /// <param name="drawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        public void ClearDrawingLabels(int drawHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a boolean value which indicates whether labels of the drawing layer are visible.
        /// </summary>
        /// <remarks>This property corresponds to the Labels.Visible property.</remarks>
        /// <param name="drawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <returns>True in case labels are visible and false otherwise.</returns>
        public bool get_DrawingLabelsVisible(int drawHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a boolean value which indicates whether labels of the drawing layer are visible.
        /// </summary>
        /// <remarks>This property corresponds to the Labels.Visible property.</remarks>
        /// <param name="drawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="newValue">True in case labels are visible and false otherwise.</param>
        public void set_DrawingLabelsVisible(int drawHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a label on the current drawing layer.
        /// </summary>
        /// <param name="text">Text of the label.</param>
        /// <param name="x">X coordinate, in either screen or map units depending on the parameters of AxMap.NewDrawing used to create the layer.</param>
        /// <param name="y">Y coordinate, in either screen or map units depending on the parameters of AxMap.NewDrawing used to create the layer.</param>
        /// <param name="rotation">Rotation angle in degrees.</param>
        /// <returns>Layer handle if the label was added successfully or -1 on failure.</returns>
        /// \new493 Added in version 4.9.3
        public int DrawLabel(string text, double x, double y, double rotation)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a label on the specified drawing layer.
        /// </summary>
        /// <param name="drawHandle">Handle of the layer.</param>
        /// <param name="text">Text of the label.</param>
        /// <param name="x">X coordinate, in either screen or map units depending on the parameters of AxMap.NewDrawing used to create the layer.</param>
        /// <param name="y">Y coordinate, in either screen or map units depending on the parameters of AxMap.NewDrawing used to create the layer.</param>
        /// <param name="rotation">Rotation angle in degrees.</param>
        /// <returns>Layer handle if the label was added successfully or -1 on failure.</returns>
        /// \new493 Added in version 4.9.3
        public int DrawLabelEx(int drawHandle, string text, double x, double y, double rotation)
        {
            throw new NotImplementedException();
        }

#pragma warning disable 1587
        /// @}
#pragma warning restore 1587
        #endregion

        #region Shapefile layer
        /// \addtogroup map_shapefile Shapefile visualization
        /// Here is a list of properties for changing appearance of shapefile layer added to the map. 
        /// Consider the usage of ShapeDrawingOptions class to access wider set of options. 
        /// See Shapefile.DefaultDrawingOptions for details. This module is a part of the documentation of AxMap class.
        /// \dot
        /// digraph map_shapefile {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="AxMap" URL="\ref AxMap"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Shapefile visualization" URL="\ref map_shapefile"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Gets whether the specified layer is drawn with a fill. Only works on polygon shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the fill is to be set.</param>
        /// <returns>Gets whether the layer is being drawn with a fill or not.</returns>
        public bool get_ShapeLayerDrawFill(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets whether the specified layer is drawn with a fill. Only works on polygon shapefiles. 
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the fill is to be set. </param>
        /// <param name="newValue">Sets whether the layer is drawn with a fill or not.</param>
        public void set_ShapeLayerDrawFill(int layerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets whether the lines for the shapefile in specified layer are drawn. 
        /// </summary>
        /// <param name="layerHandle">Handle of the layer to test if it is being drawn with a lines or not.</param>
        /// <returns>Gets whether the layer is being drawn with lines or not.</returns>
        public bool get_ShapeLayerDrawLine(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets whether the lines for the shapefile in specified layer are drawn. 
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the lines are to be set.</param>
        /// <param name="newValue">Sets whether the layer is drawn with lines or not.</param>
        public void set_ShapeLayerDrawLine(int layerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets whether the points/vertices for the shapefile in specified layer are drawn. 
        /// </summary>
        /// <param name="layerHandle">Handle of the layer to test if it is being drawn with a points or not.</param>
        /// <returns>Gets whether the layer is being drawn with points or not.</returns>
        public bool get_ShapeLayerDrawPoint(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets whether the points/vertices for the shapefile in specified layer are drawn.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the points are to be set.</param>
        /// <param name="newValue">Sets whether the layer is drawn with points or not.</param>
        public void set_ShapeLayerDrawPoint(int layerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the fill color for the specified layer. Only works on polygon shapefiles. 
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the fill color is required. </param>
        /// <returns>Fill color for the polygon shapefile.</returns>
        public uint get_ShapeLayerFillColor(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the fill color for the specified layer. Only works on polygon shapefiles
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the fill color is to be set.</param>
        /// <param name="newValue">Fill color for the polygon shapefile. This is a System.UInt32 representation of an RGB color.</param>
        public void set_ShapeLayerFillColor(int layerHandle, uint newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the fill stipple for the specified layer. Only works on polygon shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the fill stipple is required. </param>
        /// <returns>Gets the fill stipple for the specified layer.</returns>
        public tkFillStipple get_ShapeLayerFillStipple(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the fill stipple for the specified layer. Only works on polygon shapefiles. 
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the fill stipple is to be set.</param>
        /// <param name="newValue">Sets fill stipple for the specified layer.</param>
        public void set_ShapeLayerFillStipple(int layerHandle, tkFillStipple newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the percentage of fill transparency for the specified layer. Only works on polygon shapefiles. 
        /// </summary>
        /// <param name="layerHandle">Handle of the layer to get percentage of fill transparency. </param>
        /// <returns>Gets the percentage of fill transparency for the specified layer.</returns>
        public float get_ShapeLayerFillTransparency(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the percentage of fill transparency for the specified layer. Only works on polygon shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the fill transparency is to be set. </param>
        /// <param name="newValue">Sets the percentage of fill transparency for the specified layer.</param>
        public void set_ShapeLayerFillTransparency(int layerHandle, float newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the line color for the specified layer. Only works on shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the line color is required. </param>
        /// <returns>Line color for the polygon shapefile. </returns>
        public uint get_ShapeLayerLineColor(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the line color for the specified layer. Only works on shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the line color is to be set.</param>
        /// <param name="newValue">Line color for the polygon shapefile. This is a System.UInt32 representation of an RGB color.</param>
        public void set_ShapeLayerLineColor(int layerHandle, uint newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the line stipple for the specified layer. Only works on shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the line stipple is required.</param>
        /// <returns>Line stipple for the shapefile.</returns>
        public tkLineStipple get_ShapeLayerLineStipple(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the line stipple for the specified layer. Only works on shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the line stipple is required.</param>
        /// <param name="newValue">Line stipple for the shapefile.</param>
        public void set_ShapeLayerLineStipple(int layerHandle, tkLineStipple newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the line width for the specified layer. Only works on shapefiles.
        /// Suggested values for line width: 1 - 5
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the line width is required.</param>
        /// <returns>Line width for the shapefile.</returns>
        public float get_ShapeLayerLineWidth(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the line width for the specified layer. Only works on shapefiles.
        /// Suggested values for line width: 1 - 5
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the line width is to be set.</param>
        /// <param name="newValue">Line width for the shapefile.</param>
        public void set_ShapeLayerLineWidth(int layerHandle, float newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the point color for the specified layer. Only works on shapefiles
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the point color is required. </param>
        /// <returns>Point color for the polygon shapefile. </returns>
        public uint get_ShapeLayerPointColor(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the point color for the specified layer. Only works on shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the point color is to be set.</param>
        /// <param name="newValue">Point color for the polygon shapefile. This is a System.UInt32 representation of an RGB color.</param>
        public void set_ShapeLayerPointColor(int layerHandle, uint newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the line point/vertex size for the specified layer. Only works on shapefiles. 
        /// </summary>
        /// <param name="layerHandle">Gets or sets the line point/vertex size for the specified layer. Only works on shapefiles. </param>
        /// <returns>Point/vertex size for the shapefile. </returns>
        public float get_ShapeLayerPointSize(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the line point/vertex size for the specified layer. Only works on shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the point/vertex size is to be set.</param>
        /// <param name="newValue">Point/vertex size for the shapefile.</param>
        public void set_ShapeLayerPointSize(int layerHandle, float newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the line point type for the specified layer. Only works on shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the point type is required.</param>
        /// <returns>%Point type for the shapefile.</returns>
        public tkPointType get_ShapeLayerPointType(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the line point type for the specified layer. Only works on shapefiles.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer for which the point type is to be set.</param>
        /// <param name="newValue">%Point type for the shapefile.</param>
        public void set_ShapeLayerPointType(int layerHandle, tkPointType newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the color of the polygon stipple for shapefile layer.
        /// </summary>
        /// <remarks>This property corresponds to the ShapeDrawingOptions.FillHatchStyle property.</remarks>
        /// <param name="layerHandle">The handle of the shapefile layer.</param>
        /// <returns>The color of stipple.</returns>
        public uint get_ShapeLayerStippleColor(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the color of the polygon stipple for shapefile layer.
        /// </summary>
        /// <remarks>This property corresponds to the ShapeDrawingOptions.FillBgTransparent property.</remarks>
        /// <remarks>This property corresponds to the ShapeDrawingOptions.FillHatchStyle property.</remarks>
        /// <param name="layerHandle">The handle of the shapefile layer.</param>
        /// <param name="newValue">The color of stipple.</param>
        /// \see Shapefile.DefaultDrawingOptions
        public void set_ShapeLayerStippleColor(int layerHandle, uint newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a boolean value which indicates whether the background of fill stipple for polygon shapefile layer will be transparent.
        /// </summary>
        /// <remarks>This property corresponds to the ShapeDrawingOptions.FillBgTransparent property.</remarks>
        /// <param name="layerHandle">The handle of the shapefile layer.</param>
        /// <returns>True in case the background is transparent and false otherwise.</returns>
        public bool get_ShapeLayerStippleTransparent(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a boolean value which indicates whether the background of fill stipple for polygon shapefile layer will be transparent.
        /// </summary>
        /// <remarks>This property corresponds to the ShapeDrawingOptions.FillBgTransparent property.</remarks>
        /// <param name="layerHandle">The handle of the shapefile layer.</param>
        /// <param name="newValue">True in case the background is transparent and false otherwise.</param>
        public void set_ShapeLayerStippleTransparent(int layerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

#pragma warning disable 1587
        /// @}
#pragma warning restore 1587

        #endregion

        #region Map extents
        /// \addtogroup map_extents Max extents and zoom
        /// Here is list of properties and methods that affect map extents and zoom level. This module is a part of the documentation of AxMap class.
        /// \dot
        /// digraph map_extents {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="AxMap" URL="\ref AxMap"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Map extents adn zoom" URL="\ref map_extents"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Sets geographic extents for the map.
        /// </summary>
        /// <param name="xLongitude">Longitude of center of the screen (degrees).</param>
        /// <param name="yLatitude">Latitude of center of the screen (degrees).</param>
        /// <param name="widthKilometers">Width of displayed extents in kilometres.</param>
        /// <remarks>Projection for the must be specified in order for this method to work.</remarks>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool SetGeographicExtents2(double xLongitude, double yLatitude, double widthKilometers)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets known extents.
        /// </summary>
        /// <param name="extents">Known extents to retrieve.</param>
        /// <returns>Extents object.</returns>
        /// \new491 Added in version 4.9.1
        public Extents GetKnownExtents(tkKnownExtents extents)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets geographic extents of the map in decimal degrees.
        /// </summary>
        /// <remarks>For operation to succeed, projection must be set for the map (see AxMap.GeoProjection property). 
        /// Otherwise null will be returned.</remarks>
        /// \new490 Added in version 4.9.0
        public Extents GeographicExtents
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Sets geographic extents in decimal degrees for the map.
        /// </summary>
        /// <param name="pVal">Geographic extents in decimal degrees.</param>
        /// <returns>True on success.</returns>
        /// <remarks>For operation to succeed, projection must be set for the map (see AxMap.GeoProjection property)</remarks>
        /// \new490 Added in version 4.9.0
        public virtual bool SetGeographicExtents(Extents pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets latitude of the center of screen (in decimal degrees). 
        /// </summary>
        /// <remarks>Map projection must be set in order for this property to work.</remarks>
        /// \new491 Added in version 4.9.1
        public float Latitude { get; set; }

        /// <summary>
        /// Gets or sets longitude of the center of screen (in decimal degrees). 
        /// </summary>
        /// <remarks>Map projection must be set in order for this property to work.</remarks>
        /// \new491 Added in version 4.9.1
        public float Longitude { get; set; }

        /// <summary>
        /// Sets the Latitude and Longitude of the center of the screen in one operation
        /// </summary>
        /// <param name="latitude">Requested Latitude in decimal degrees</param>
        /// <param name="longitude">Requested Longitude in decimal degrees</param>
        /// <remarks>Map projection must be set in order for this method to work.</remarks>
        /// \new510 Added in version 5.1.0
        public void SetLatitudeLongitude(double latitude, double longitude)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the current zoom level for the map. It corresponds to the zoom level of current tile provider.
        /// </summary>
        /// <remarks>Map projection must be set in order for this property to work.</remarks>
        /// \new491 Added in version 4.9.1
        public int CurrentZoom { get; set; }

        /// <summary>
        /// Gets or sets known extents for the map.
        /// </summary>
        /// <remarks>Map projection must be set in order for this property to work.</remarks>
        /// \new491 Added in version 4.9.1
        public tkKnownExtents KnownExtents { get; set; }

        /// <summary>
        /// Gets or sets the current map scale.
        /// </summary>
        /// <remarks>The scale depends on the AxMap.MapUnits property. Number of pixels per logical inch and the size of window are used to
        /// determine the size of the displaying device. The calculations represent the closest approximation as there is no way
        /// to determine the physical size of displaying device. The operation of setting the new scale preserve the map coordinates 
        /// of the center point on the screen.</remarks>
        /// \new48 Added in version 4.8
        public double CurrentScale
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the number of extents to cache in the extents history
        /// </summary>
        public int ExtentHistory
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the percentage of the view used to pad the extents of a layer when zooming to a layer or maximum extents.
        /// </summary>
        /// <remarks>
        /// Padding makes it so that there is a small border around the layer when you zoom to it.</remarks>
        public double ExtentPad
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the extents of the map using an Extents object. 
        /// </summary>
        /// <remarks>If the given extents do not fit the aspect ratio of the map, the map will fit the given extents as well as possible.</remarks>
        /// \new495 Return value changed to IExtents in version 4.9.5
        public Extents Extents
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the extents of the map displayed by the control.
        /// </summary>
        public Extents MaxExtents
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

#pragma warning disable 1587
        /// @}
#pragma warning restore 1587
        #endregion

        #region Projection and coordinates
        /// \addtogroup map_coordinates Map projection and coordinates
        /// Here is list of properties and methods which are related to coordinate system, projection of map and units conversion.
        /// This module is a part of the documentation of AxMap class.
        /// \dot
        /// digraph map_projection {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="AxMap" URL="\ref AxMap"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Max projection and coordianates" URL="\ref map_coordinates"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>\n\n
        /// \anchor a_projection
        /// \section proj1 A. Setting projection for the map.
        ///
        /// No matter what type of GIS application you are going to write, the decision about map coordinate system and projection is one of the first to be made. 
        /// MapWinGIS provides the following options.
        /// 
        /// \dot
        /// digraph projection_options {
        /// splines = true;
        /// ranksep = 0.15;
        /// nodesep = 0.5;
        /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
        /// render [ label="Choosing map projection"];
        /// 
        /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
        /// s1    [label=" Set projection manullay\l"];
        /// s2    [label=" Grab projection from data\l"];
        /// s3    [label=" Don't use projection at all\l"];
        /// 
        /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" minlen=2 ]
        /// render -> s1;
        /// render -> s2;
        /// render -> s3;
        /// }
        /// \enddot
        /// Let's consider each of the approaches.
        ///
        /// <b>1. Settings projection manually.</b> \n\n
        /// It can be done using one of the following approaches:\n\n
        /// a) AxMap.Projection property - provides only most common projections (Spherical Mercator, WGS84) but available in Properties Window of Form designer.
        /// \code
        /// axMap1.Projection = tkMapProjection.PROJECTION_WGS84;
        /// \endcode
        /// b) For all other projections - GeoProjection class and AxMap.GeoProjection property:
        /// \code
        /// var gp = new GeoProjection();
        ///
        /// // one of the following methods can be used; see more in GeoProjection class
        /// // - use projections provided by MapWinGIS enumerations:
        /// gp.SetWgs84Projection(tkWgs84Projection.Wgs84_UTM_zone_22N);    
        /// gp.SetWellKnownGeogCS(tkCoordinateSystem.csNAD83);
        ///
        /// // - EPSG code of coordinate system; in this example Pulkovo 1942(83) / 3-degree Gauss-Kruger zone 5; see wwww.spatialreference.org for EPSG codes
        /// gp.ImportFromEPSG(2399);                                        
        ///
        /// // - importing from proj4 or WKT string; in this example proj4 string for Amersfoort / RD New projection for Netherlands
        /// gp.ImportFromAutoDetect("+proj=sterea +lat_0=52.15616055555555 +lon_0=5.38763888888889 +k=0.9999079 +x_0=155000 +y_0=463000 +ellps=bessel +units=m +no_defs ");
        ///
        /// // applying projection
        /// axMap1.GeoProjection = gp;
        /// \endcode
        ///
        /// <b>2. Grabbing coordinate system and projection from data.</b>
        /// \code
        /// axMap1.GrabProjectionFromData = true;   // default value
        /// axMap1.AddLayerFromFilename(@"d:\some_shapefile.shp", tkFileOpenStrategy.fosVectorLayer, true);
        /// \endcode
        /// GeoProjection will be taken from the first layer added to the map which has metadata about projection. AxMap.GeoProjection property will be updated from this metadata.
        /// When last layer is removed from map AxMap.GeoProjection property will be cleared (set to empty projection).
        /// 
        /// <b>3. Don't specify coordinate system at all.\n</b>
        /// - AxMap.Projection property is equal to PROJECTION_NONE (the default value);
        /// - map units should be set manually via AxMap.MapUnits;
        /// - calculation of distance and area will use Euclidean geometry with no account to the shape of Earth;
        /// - it won't be possible to display tiles from TMS servers.
        /// 
        /// To setup map to work without geoprojection use the code: 
        /// \code
        /// axMap1.Projection = tkMapProjection.PROJECTION_NONE;
        /// axMap1.GrabProjectionFromData = false;
        /// axMap1.MapUnits = tkUnitsOfMeasure.umMeters;	// or another, depending on the data you display
        /// \endcode
        /// 
        /// \section proj2 B. Interaction with already set projection
        /// 
        /// To check that map actually has a projection:
        /// \code
        /// Debug.WriteLine("Map has projection:" + (axMap1.Projection != tkMapProjection.PROJECTION_NONE));
        /// // or with mode details
        /// Debug.WriteLine("Projection of the map: " + (axMap1.GeoProjection.IsEmpty() ? "None" : axMap1.GeoProjection.ExportToWKT());
        /// \endcode
        ///
        /// Another way to check whether map has geoprojection is to see how coordinates are displayed when AxMap.ShowCoordinates is set to cdmAuto. 
        /// In case decimal degrees are displayed (Lat/Lng) - map has geoprojection, if x/y pair is shown - no geoprojection was set.
        ///
        /// All of these methods will result in updating AxMap.GeoProjection property. GeoProjection object assigned to map is protected from changes, 
        /// i.e. GeoProjection.IsFrozen = true. To change already assigned projection, a new instance of GeoProjection object must be created:
        /// \code
        /// // this one will fail, because projection is frozen
        /// if (!axMap1.GeoProjection.ImportFromEPSG(2399))
        /// {
        ///	   Debug.WriteLine("Projection wasn't set: " + axMap1.GeoProjection.get_ErrorMsg(axMap1.GeoProjection.LastErrorCode));
        /// }
        /// // this one will succeed as a new GeoProjection instance is created by Clone method:
        /// var gp = axMap1.GeoProjection.Clone();
        /// if (gp.ImportFromEPSG(2399))
        /// {
        ///	   axMap1.GeoProjection = gp;
        /// }
        /// \endcode
        ///
        /// Regardless of the method for settings projection MapWinGIS will update AxMap.MapUnits. IF coordinate system is geographic one umDecimalDegrees 
        /// will be set, otherwise umMeters. This will ensure that scalebar and measuring will work correctly.
        /// 
        /// \section proj3 C. Choosing projection
        /// 
        /// When making decision about coordinate system and projection consider:\n\n
        /// 1. Whether it's important to display tiles from online services. Most TMS servers use Shperical Mercator projection (EPSG:3857), 
        /// therefore to avoid distortions of tiles map projection should be set to GoogleMercator as well:
        /// \code
        /// axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
        /// \endcode
        /// \note See more details on the issue in description of Tiles class.\n\n
        /// 
        /// 2. What data you plan to display and what projection it's using. It's possible to do a reprojection with:
        /// - Utils.ReprojectShapefile for shapefiles;
        /// - GdalUtils.GdalRasterWarp for images and grids.
        /// 
        /// But it should be considered whether it is worth the effort.
        /// 
        /// Starting from version 4.9.2. built-in projection mismatch testing is implemented + optional 
        /// transformation for shapefiles. See GlobalSettings.AllowProjectionMismatch, GlobalSettings.ReprojectLayersOnAdding. 
        /// 
        /// @{

        /// <summary>
        /// Gets measuring object associated with map.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public Measuring Measuring
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Get the identified shapes
        /// </summary>
        public SelectionList IdentifiedShapes { get; }

        /// <summary>
        /// For selection boxes or dragging operations, updated with tkRedrawType.Minimal
        /// </summary>
        public DrawingRectangle FocusRectangle { get; }

        /// <summary>
        /// Set custom drawing flags
        /// </summary>
        public tkCustomDrawingFlags CustomDrawingFlags { get; set; }

        /// <summary>
        /// Allows to format coordinates in top right corner in degrees/minutes/seconds format; by default the former minutes format is used
        /// </summary>
        public tkAngleFormat ShowCoordinatesFormat { get; set; }
        
        /// <summary>
        /// Gets or sets the units of measure for the map.
        /// </summary>
        /// <remarks>This units must be the same as the units of the datasources being displayed. 
        /// This method affects the calculation of map scale (see AxMap.CurrentScale).</remarks>
        /// \new48 Added in version 4.8
        public tkUnitsOfMeasure MapUnits
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Converts pixel coordinates to projected map coordinates
        /// </summary>
        /// <param name="pixelX">The x pixel coordinate to be converted into the projected x map coordinate.</param>
        /// <param name="pixelY">The y pixel coordinate to be converted into the projected y map coordinate</param>
        /// <param name="projX">The projected x map coordinate is returned through this reference parameter.</param>
        /// <param name="projY">The projected y map coordinate is returned through this reference parameter.</param>
        public void PixelToProj(double pixelX, double pixelY, ref double projX, ref double projY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of screen pixels per the decimal degree of the data.
        /// </summary>
        /// <remarks>The set part of this property isn't supported. The correctness of results depends on AxMap.MapUnits property.</remarks>
        /// \new48 Added in version 4.8
        public double PixelsPerDegree
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Converts projected map coordinates into screen pixel units
        /// </summary>
        /// <param name="projX">The projected x map coordinate to be converted into the x pixel coordinate.</param>
        /// <param name="projY">The projected y map coordinate to be converted into the y pixel coordinate.</param>
        /// <param name="pixelX">The pixel x coordinate is returned through this reference parameter</param>
        /// <param name="pixelY">The pixel y coordinate is returned through this reference parameter.</param>
        public void ProjToPixel(double projX, double projY, ref double pixelX, ref double pixelY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a value indicating whether coordinates of the current mouse position will be displayed on map.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public tkCoordinatesDisplay ShowCoordinates { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether to display a white background behind the coordinates, for better contrast
        /// </summary>
        /// \new52 Added in version 5.2
        public bool ShowCoordinatesBackground
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }


        /// <summary>
        /// Gets or sets a value which indicate whether scalebar will be displayed on the map.
        /// </summary>
        /// \new490 Added in version 4.9.0
        public bool ScalebarVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets units to be displayed for map scalebar.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public tkScalebarUnits ScalebarUnits { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether projection for will be taken from the first datasource added to it.
        /// </summary>
        /// <remarks>If set to true projection will be taken from the first layer added to the map which has a projection. 
        /// On removing the last layer projection of the map will be cleared (set to an empty one).</remarks>
        /// \new491 Added in version 4.9.1
        public bool GrabProjectionFromData { get; set; }

        /// <summary>
        /// Sets projection of the map. It providers 2 most commonly used coordinate system/projections to be easily set from Form Designer. 
        /// To set other projections initialize GeoProjection object manually and use AxMap.GeoProjection property.
        /// </summary>
        /// <remarks></remarks>
        /// \new491 Added in version 4.9.1
        public tkMapProjection Projection { get; set; }

        /// <summary>
        /// Gets or sets projection for map. 
        /// </summary>
        /// <remarks>Projection layered set to map must not be changed, but rather a new instance 
        /// of GeoProjection should be created (GeoProjection.Clone) and set to the property.
        /// This property must be set in order for certain functionality to work (tiles, for example).</remarks>
        /// \new490 Added in version 4.9.0
        public GeoProjection GeoProjection
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Converts projected map coordinates to decimal degrees (map projection must be specified for this method to work). 
        /// </summary>
        /// <param name="projX">Projected X map coordinate.</param>
        /// <param name="projY">Projected Y map coordinate.</param>
        /// <param name="degreesLngX">Converted longitude in decimal degrees</param>
        /// <param name="degreesLatY">Converted latitude in decimal degrees</param>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool ProjToDegrees(double projX, double projY, ref double degreesLngX, ref double degreesLatY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Converts coordinates in decimal degrees to pixel coordinates (map projection must be specified for this method to work). 
        /// </summary>
        /// <param name="pixelX">X screen coordinate.</param>
        /// <param name="pixelY">Y screen coordinate.</param>
        /// <param name="degreesLngX">Converted longitude in decimal degrees</param>
        /// <param name="degreesLatY">Converted latitude in decimal degrees</param>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool PixelToDegrees(double pixelX, double pixelY, ref double degreesLngX, ref double degreesLatY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Converts coordinates in decimal degrees to projected map coordinates (map projection must be specified for this method to work). 
        /// </summary>
        /// <param name="degreesLngX">Longitude in decimal degrees.</param>
        /// <param name="degreesLatY">Latitude in decimal degrees.</param>
        /// <param name="projX">Resulting projected X map coordinate.</param>
        /// <param name="projY">Resulting Projected Y map coordinate.</param>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool DegreesToProj(double degreesLngX, double degreesLatY, ref double projX, ref double projY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Converts pixel coordinates to decimal degrees (map projection must be specified for this method to work). 
        /// </summary>
        /// <param name="degreesLngX">Longitude in decimal degrees.</param>
        /// <param name="degreesLatY">Latitude in decimal degrees.</param>
        /// <param name="pixelX">Converted X screen coordinate.</param>
        /// <param name="pixelY">Converted Y screen coordinate.</param>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool DegreesToPixel(double degreesLngX, double degreesLatY, ref double pixelX, ref double pixelY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates area of polygon taking into account the shape of Earth.
        /// </summary>
        /// <remarks>For the time being only single-part polygons are accepted as input. Calculation are made using 
        /// <a href = "http://geographiclib.sourceforge.net/html/">GeographicLib</a>.</remarks>
        /// <param name="polygon">Single part polygon shape defined in coordinates of map.</param>
        /// <returns>Area of shape in square meters or 0.0 if current map projection doesn't support transformation to WGS84.</returns>
        /// \new493 Added in version 4.9.3
        public double GeodesicArea(Shape polygon)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Calculates geodesic distance between 2 points defined in map coordinate system.
        /// </summary>
        /// <remarks>To calculate geodesic distance between 2 points in screen coordinates use AxMap.PixelToProj first.
        ///  Calculation are made using <a href = "http://geographiclib.sourceforge.net/html/">GeographicLib</a>.</remarks>
        /// <param name="projX1">X coordinate of the first point.</param>
        /// <param name="projY1">Y coordinate of the first point.</param>
        /// <param name="projX2">X coordinate of the second point.</param>
        /// <param name="projY2">Y coordinate of the second point.</param>
        /// <returns>Distance between 2 points in meters 0.0 if current map projection doesn't support transformation to WGS84</returns>
        /// \new493 Added in version 4.9.3
        public double GeodesicDistance(double projX1, double projY1, double projX2, double projY2)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates length of polyline or perimeter of polygon taking into account the shape of Earth.
        /// </summary>
        /// <remarks>For the time being only single-part shapes are accepted as input. Calculation are made using 
        /// <a href = "http://geographiclib.sourceforge.net/html/">GeographicLib</a>.</remarks>
        /// <param name="polyline">Single part polygon or polyline shape defined in coordinates of map.</param>
        /// <returns>Length of polyline or perimeter of polygon in meters or 0.0 if current map projection doesn't support transformation to WGS84.</returns>
        /// \new493 Added in version 4.9.3
        public double GeodesicLength(Shape polyline)
        {
            throw new NotImplementedException();
        }

#pragma warning disable 1587
        /// @}
#pragma warning restore 1587
        #endregion

        #region Events

        /// \addtogroup map_events Map events
        /// \dot
        /// digraph map_events_graph {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="AxMap" URL="\ref AxMap"];
        /// node [shape = "ellipse", color = gray, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Map events" URL="\ref map_events"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// This event is fired after the rendering of drawing layers. Handle of device context is passed to allow the user to implement custom drawing. 
        /// </summary>
        /// <param name="hdc">Handle of device context of screen buffer.</param>
        /// <param name="xMin">Minimum X coordinate of the rectangle being rendered.</param>
        /// <param name="xMax">Maximum X coordinate of the rectangle being rendered.</param>
        /// <param name="yMin">Minimum Y coordinate of the rectangle being rendered.</param>
        /// <param name="yMax">Maximum Y coordinate of the rectangle being rendered.</param>
        /// <param name="handled">Passed by reference. The value should be set to tkMwBoolean.blnTrue in case some additional drawing is performed in client code.</param>
        public event _DMapEvents_AfterDrawingEventHandler AfterDrawing;
        void _DMapEvents.AfterDrawing(int hdc, int xMin, int xMax, int yMin, int yMax, ref tkMwBoolean handled)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired after the rendering of standard layers. Handle of device context is passed to allow the user to implement custom drawing. 
        /// </summary>
        /// <param name="hdc">Handle of device context of screen buffer.</param>
        /// <param name="xMin">Minimum X coordinate of the rectangle being rendered.</param>
        /// <param name="xMax">Maximum X coordinate of the rectangle being rendered.</param>
        /// <param name="yMin">Minimum Y coordinate of the rectangle being rendered.</param>
        /// <param name="yMax">Maximum Y coordinate of the rectangle being rendered.</param>
        /// <param name="handled">Passed by reference. The value should be set to tkMwBoolean.blnTrue in case some additional drawing is performed in client code.</param>
        /// \new495 Added in version 4.9.5
        public event _DMapEvents_AfterLayersEventHandler AfterLayers;
        void _DMapEvents.AfterLayers(int hdc, int xMin, int xMax, int yMin, int yMax, ref tkMwBoolean handled)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired whenever a snap point is requested by the map before the MapWinGIS snap algorithm was run. 
        /// If the isFinal flag is set to true, the default snapping algorithm of MapWinGIS will be skipped.
        /// </summary>
        /// <param name="pointX">the X coordinate of the point a snap point is requested for</param>
        /// <param name="pointY">the Y coordinate of the point a snap point is requested for</param>
        /// <param name="snappedX">the X coordinate of the snap point found so far (can be changed)</param>
        /// <param name="snappedY">the Y coordinate of the snap point found so far (can be changed)</param>
        /// <param name="isFound">a flag indicating if a snap point has been found (can be changed)</param>
        /// <param name="isFinal">a flag indicating if the found snap point should be considered final</param>
        /// \new52 Added in version 5.2
        public event _DMapEvents_SnapPointRequestedEventHandler SnapPointRequested;
        void _DMapEvents.SnapPointRequested(double pointX, double pointY, ref double snappedX, ref double snappedY, ref tkMwBoolean isFound,
            ref tkMwBoolean isFinal)
        {
        }

        /// <summary>
        /// This event is fired after a snap point was found either by the default snapping algorithm of MapWinGIS or by a SnapPointRequested handler.
        /// Setting the snappedX and snappedY allows you to override this result (use with care!).
        /// </summary>
        /// <param name="pointX">the X coordinate of the point a snap point is requested for</param>
        /// <param name="pointY">the Y coordinate of the point a snap point is requested for</param>
        /// <param name="snappedX">the X coordinate of the snap point found so far (can be changed)</param>
        /// <param name="snappedY">the Y coordinate of the snap point found so far (can be changed)</param>
        /// \new52 Added in version 5.2
        public event _DMapEvents_SnapPointFoundEventHandler SnapPointFound;
        void _DMapEvents.SnapPointFound(double pointX, double pointY, ref double snappedX, ref double snappedY)
        {
        }

        /// <summary>
        /// This event is fired after interactive editing of shape is finished (AxMap.CursorMode is set to tkCursorMode.cmEditShape).
        /// </summary>
        /// <param name="operation">The type of editing operation that was performed.</param>
        /// <param name="layerHandle">Handle of the layer the shape being edited belongs to.</param>
        /// <param name="shapeIndex">Index of the shape withing layer.</param>
        /// <remarks>The common use of the event is set attributes of the shape, update its label or style. 
        /// The operation parameter can be one of the following values: tkUndoOperation.uoAddShape, tkUndoOperation.uoRemoveShape, tkUndoOperation.uoEditShape.</remarks>
        public event _DMapEvents_AfterShapeEditEventHandler AfterShapeEdit;
        void _DMapEvents.AfterShapeEdit(tkUndoOperation operation, int layerHandle, int shapeIndex)
        {
            throw new NotImplementedException();
        }
        

        /// <summary>
        /// This event is fired when background loading of data for OgrLayer finishes.
        /// </summary>
        /// <param name="taskId">Unique Id of the loading task.</param>
        /// <param name="layerHandle">Handle of the layer the loading is peformed for.</param>
        /// <param name="numFeatures">Number of features within current map extents.</param>
        /// <param name="numLoaded">Number of features that were actually loaded.</param>
        /// <remarks>The loading task may be finished without any features being loaded under the following circumstances:
        /// - number of features within current extents is larger than GlobalSettings.OgrLayerMaxFeatureCount parameter;
        /// - the extents of map have been changed since the loading was started so the data being loaded is no longer needed;
        /// - there is a problem with accessing the datasource (e.g. lost network connection).\n
        /// In case of failure, i.e. (numLoaded = 0) the event will be fired from background thread. In case 
        /// of success - from the main thread before the rendering of the loaded data.
        /// </remarks>
        public event _DMapEvents_BackgroundLoadingFinishedEventHandler BackgroundLoadingFinished;
        void _DMapEvents.BackgroundLoadingFinished(int taskId, int layerHandle, int numFeatures, int numLoaded)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when background loading of data for OgrLayer starts.
        /// </summary>
        /// <param name="taskId">Unique Id of the loading task, 
        /// which can be tracked down in AxMap.BackgroundLoadingFinished to determine the results of the operation.</param>
        /// <param name="layerHandle">Handle of the layer loading starts for.</param>
        /// <remarks>The event is fired after map extents change for each OgrLayer with OgrLayer.DynamicLoading proprety set to true,
        /// when the layer doesn't have the necessary data in its buffer (OgrLayer.GetBuffer). The event can be used
        /// to display some kind of loading indicator to notify the user that some data is still being loaded.</remarks>
        public event _DMapEvents_BackgroundLoadingStartedEventHandler BackgroundLoadingStarted;
        void _DMapEvents.BackgroundLoadingStarted(int taskId, int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired before shape is deleted in interactive ShapeEditor.
        /// </summary>
        /// <param name="target">What element of shape (whole shape, part, single vertex) is to be deleted.</param>
        /// <param name="cancel">Passed by reference. To cancel the operation this value should be set to tkMwBoolean.blnTrue.</param>
        /// <remarks>This event is fired when shape (or some of its elements) is selected in ShapeEditor (AxMap.CursorMode = tkCursorMode.cmEditShape)
        /// and user presses Delete button. The common use of his event is to display a message box asking the user whether 
        /// the operation should be performed.</remarks>
        public event _DMapEvents_BeforeDeleteShapeEventHandler BeforeDeleteShape;
        void _DMapEvents.BeforeDeleteShape(tkDeleteTarget target, ref tkMwBoolean cancel)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired before the rendering of drawing layers. Handle of device context is passed to allow the user to implement custom drawing. 
        /// </summary>
        /// <param name="hdc">Handle of device context of screen buffer.</param>
        /// <param name="xMin">Minimum X coordinate of the rectangle being rendered.</param>
        /// <param name="xMax">Maximum X coordinate of the rectangle being rendered.</param>
        /// <param name="yMin">Minimum Y coordinate of the rectangle being rendered.</param>
        /// <param name="yMax">Maximum Y coordinate of the rectangle being rendered.</param>
        /// <param name="handled">Passed by reference. 
        /// The value should be set to tkMwBoolean.blnTrue in case some additional drawing is performed in client code.</param>
        public event _DMapEvents_BeforeDrawingEventHandler BeforeDrawing;
        void _DMapEvents.BeforeDrawing(int hDc, int xMin, int xMax, int yMin, int yMax, ref tkMwBoolean handled)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// This event is fired before the rendering of standard layers. Handle of device context is passed to allow the user to implement custom drawing. 
        /// </summary>
        /// <param name="hdc">Handle of device context of screen buffer.</param>
        /// <param name="xMin">Minimum X coordinate of the rectangle being rendered.</param>
        /// <param name="xMax">Maximum X coordinate of the rectangle being rendered.</param>
        /// <param name="yMin">Minimum Y coordinate of the rectangle being rendered.</param>
        /// <param name="yMax">Maximum Y coordinate of the rectangle being rendered.</param>
        /// <param name="handled">Passed by reference. 
        /// The value should be set to tkMwBoolean.blnTrue in case some additional drawing is performed in client code.</param>
        /// \new495 Added in version 4.9.5
        public event _DMapEvents_BeforeLayersEventHandler BeforeLayers;
        void _DMapEvents.BeforeLayers(int hdc, int xMin, int xMax, int yMin, int yMax, ref tkMwBoolean handled)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// This event is fired before editing starts for particular shape (after user click on the shape when map cursor is set to tkCursorMode.cmEditShape). 
        /// </summary>
        /// <param name="layerHandle">Handle of the layer the shape was selected from.</param>
        /// <param name="shapeIndex">Index of shape to be edited.</param>
        /// <param name="cancel">Passed by reference. Allows user to cancel the editing operations by setting the value to tkMwBoolean.blnTrue.</param>
        public event _DMapEvents_BeforeShapeEditEventHandler BeforeShapeEdit;
        void _DMapEvents.BeforeShapeEdit(int layerHandle, int shapeIndex, ref tkMwBoolean cancel)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The event is fired when current map tool asks for the layer to work with.
        /// </summary>
        /// <param name="xProj">X coordinate of mouse click in map coordinates (if the event was triggered by mouse click).</param>
        /// <param name="yProj">Y coordinate of mouse click in map coordinates (if the event was triggered by mouse click).</param>
        /// <param name="layerHandle">Passed by reference. Handle of the layer to apply the tool to should be set.</param>
        /// <remarks>LayerHandle parameter in most cases is initially set to -1, which means "layer not defined".
        /// If this value is left unchanged the pending operation won't be preformed. Further details are provided 
        /// in description of particular tools.</remarks>
        public event _DMapEvents_ChooseLayerEventHandler ChooseLayer;
        void _DMapEvents.ChooseLayer(double xProj, double yProj, ref int layerHandle)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// The event is fired when user performs double click with left mouse button while cursor is within the map control.
        /// </summary>
        public event EventHandler DblClick;
        void _DMapEvents.DblClick()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when the extents of the map change. 
        /// </summary>
        public event EventHandler ExtentsChanged;
        void _DMapEvents.ExtentsChanged()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when a user drags a file and drops it on the map. 
        /// </summary>
        /// <param name="filename">The filename of the file dropped on the map.</param>
        public event _DMapEvents_FileDroppedEventHandler FileDropped;
        void _DMapEvents.FileDropped(string filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when a new layer has been added to the map.
        /// </summary>
        /// <param name="layerHandle">Handle of the newly added layer.</param>
        public event _DMapEvents_LayerAddedEventHandler LayerAdded;
        void _DMapEvents.LayerAdded(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when a new layer has been added to the map and it has no metadata about its coordinate system and projection.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer.</param>
        /// <param name="cancelAdding">Passed by reference. The value should be set to tkMwBoolean.blnTrue to cancel the adding operation.</param>
        /// <remarks>If this event is not handled then decision about the layer will be taken based on 
        /// the value of GlobalSettings.AllowLayersWithoutProjections property.</remarks>
        public event _DMapEvents_LayerProjectionIsEmptyEventHandler LayerProjectionIsEmpty;
        void _DMapEvents.LayerProjectionIsEmpty(int layerHandle, ref tkMwBoolean cancelAdding)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when a layer has been removed from map.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer.</param>
        /// <param name="fromRemoveAllLayers">The value will be set to true in case layer removal is caused by AxMap.RemoveAllLayers call.</param>
        public event _DMapEvents_LayerRemovedEventHandler LayerRemoved;
        void _DMapEvents.LayerRemoved(int layerHandle, bool fromRemoveAllLayers)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired after a layer was reprojected (its original projection was different from the map projection).
        /// </summary>
        /// <param name="layerHandle">Handle of the layer.</param>
        /// <param name="success">True in case reprojection was performed successfully.</param>
        /// <remarks>Automatic reprojection if performed in case of projection mismatch for vector datasources.
        /// Reprojected datasource will be represented by in-memory shapefile, no disk version will be saved automatically.
        /// Raster datasource will be rejected without an attempt to reproject them. 
        /// The projection mismatch behavior is controlled by the GlobalSettings.AllowProjectionMismatch, 
        /// GlobalSettings.ReprojectLayersOnAdding properties.</remarks>
        public event _DMapEvents_LayerReprojectedEventHandler LayerReprojected;
        void _DMapEvents.LayerReprojected(int layerHandle, bool success)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when a layer was not fully reprojected.
        /// It is controlled by the GlobalSettings.AllowLayersWithIncompleteReprojection property.
        /// </summary>
        /// <param name="layerHandle"></param>
        /// <param name="numReprojected"></param>
        /// <param name="numShapes"></param>
        /// \new510 Added in version 5.1.0
        public event _DMapEvents_LayerReprojectedIncompleteEventHandler LayerReprojectedIncomplete;
        void _DMapEvents.LayerReprojectedIncomplete(int layerHandle, int numReprojected, int numShapes)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// This event is fired for each layer in the map when the map state is changed.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer.</param>
        public event _DMapEvents_MapStateEventHandler MapState;
        void _DMapEvents.MapState(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired after the user adds or removes a point from the path of Measuring tool.
        /// </summary>
        /// <param name="action">Particular action (like tkMeasuringAction.PointAdded) performed by user.</param>
        public event _DMapEvents_MeasuringChangedEventHandler MeasuringChanged;
        void _DMapEvents.MeasuringChanged(tkMeasuringAction action)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when a user has pressed a mouse button while the cursor is inside the map control. 
        /// The map property AxMap.SendMouseDown must be set to True for this event to be fired.
        /// </summary>
        /// <param name="button">Mouse button that was pressed.</param>
        /// <param name="shift">The shift/ctrl modifiers pressed during the creation of this event. </param>
        /// <param name="x">X coordinate of mouse cursor position in pixels relative to controls origin.</param>
        /// <param name="y">Y coordinate of mouse cursor position in pixels relative to controls origin.</param>
        public event _DMapEvents_MouseDownEventHandler MouseDown;
        void _DMapEvents.MouseDown(short button, short shift, int x, int y)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// This event is fired when the mouse is moved while the cursor is inside the map control. 
        /// The map property AxMap.SendMouseMove must be set to True for this event to be fired. 
        /// </summary>
        /// <param name="button">Mouse button that was pressed.</param>
        /// <param name="shift">The shift/ctrl modifiers pressed during the creation of this event.</param>
        /// <param name="x">X coordinate of mouse cursor position in pixels relative to controls origin.</param>
        /// <param name="y">Y coordinate of mouse cursor position in pixels relative to controls origin.</param>
        public event _DMapEvents_MouseMoveEventHandler MouseMove;
        void _DMapEvents.MouseMove(short button, short shift, int x, int y)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// This event is fired when the mouse button is released while the cursor is in the map control. 
        /// The map property AxMap.SendMouseUp must be set to True for this event to be fired. 
        /// </summary>
        /// <param name="button">Mouse button that was pressed.</param>
        /// <param name="shift">The shift/ctrl modifiers pressed during the creation of this event.</param>
        /// <param name="x">X coordinate of mouse cursor position in pixels relative to controls origin.</param>
        /// <param name="y">Y coordinate of mouse cursor position in pixels relative to controls origin.</param>
        public event _DMapEvents_MouseUpEventHandler MouseUp;
        void _DMapEvents.MouseUp(short button, short shift, int x, int y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired during the rendering of map after data layers and drawing layers were rendered.
        /// The map property AxMap.SendOnDrawBackBuffer must be set to True for this event to be fired. 
        /// </summary>
        /// <param name="backBuffer">Handle of the device context of back buffer bitmap.</param>
        public event _DMapEvents_OnDrawBackBufferEventHandler OnDrawBackBuffer;
        void _DMapEvents.OnDrawBackBuffer(int backBuffer)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Special version of _DMapEvents.OnDrawBackBuffer
        /// </summary>
        /// <param name="height"></param>
        /// <param name="width"></param>
        /// <param name="stride"></param>
        /// <param name="pixelFormat"></param>
        /// <param name="scan0"></param>
        public event _DMapEvents_OnDrawBackBuffer2EventHandler OnDrawBackBuffer2;
        void _DMapEvents.OnDrawBackBuffer2(int height, int width, int stride, int pixelFormat, int scan0)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// The event is fired when projection of the map control was changed.
        /// </summary>
        public event EventHandler ProjectionChanged;
        void _DMapEvents.ProjectionChanged()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when a layer was added to the map with projection / coordinate system different from 
        /// those of the map control.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer.</param>
        /// <param name="cancelAdding">Passed by reference. The value should be set to tkMwBoolean.blnTrue in case the adding of the layer should be cancelled.</param>
        /// <param name="reproject">Passed by reference. The value should be set to tkMwBoolean.blnTrue to instruct the control 
        /// to run automatic reprojection of the layer.</param>
        public event _DMapEvents_ProjectionMismatchEventHandler ProjectionMismatch;
        void _DMapEvents.ProjectionMismatch(int layerHandle, ref tkMwBoolean cancelAdding, ref tkMwBoolean reproject)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// This event is fired while the user is dragging a selection box in the map control. 
        /// The map property AxMap.SendSelectBoxDrag must be set to True for this event to be fired. 
        /// </summary>
        /// <param name="left">The left boundary of the selection box in pixel coordinates.</param>
        /// <param name="right">The right boundary of the selection box in pixel coordinates.</param>
        /// <param name="bottom">The bottom boundary of the selection box in pixel coordinates.</param>
        /// <param name="top">The top boundary of the selection box in pixel coordinates.</param>
        public event _DMapEvents_SelectBoxDragEventHandler SelectBoxDrag;
        void _DMapEvents.SelectBoxDrag(int left, int right, int bottom, int top)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when the user finishes dragging a selection box in the map control. 
        /// The map property AxMap.SendSelectBoxFinal must be set to True for this event to be fired.
        /// </summary>
        /// <param name="left">The left boundary of the selection box in pixel coordinates.</param>
        /// <param name="right">The right boundary of the selection box in pixel coordinates.</param>
        /// <param name="bottom">The bottom boundary of the selection box in pixel coordinates.</param>
        /// <param name="top">The top boundary of the selection box in pixel coordinates.</param>
        /// <remarks>
        /// Prior to version 5.0, there was ambiguity as to whether or not this event was fired 
        /// in all circumstances.  In the case of the tkCursorMode.cmZoomIn tool, this event was always fired.  
        /// But in the case of the tkCursorMode.cmSelection tool, it was only fired in the case when no shapes 
        /// were actually 'selected'.  If any shapes were 'selected', the AxMap.SelectionChanged event 
        /// was fired, but the AxMap.SelectBoxFinal was not. As of version 5.0, the AxMap.SelectBoxFinal event 
        /// will always be fired after dragging a rectangle, whether or not any shapes were actually 'selected'.
        /// </remarks>
        /// \new500 Modified in version 5.0
        public event _DMapEvents_SelectBoxFinalEventHandler SelectBoxFinal;
        void _DMapEvents.SelectBoxFinal(int left, int right, int bottom, int top)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired after shapes were selected with tkCursorMode.cmSelection tool.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer the shapes were selected on.</param>
        /// <remarks>The event will be fired only when selection is done by AxMap control internally, i.e.
        /// - AxMap.CursorMode set to tkCursorMode.cmSelection;
        /// - a layer handle is passed to AxMap.ChooseLayer event handler;
        /// - user changes selection by clicking on shapes or drawing selection box.
        /// When Shapefile.set_ShapeSelected() property is changed from client code no event is fired.
        /// 
        /// Starting in version 5.2.0, this event is raised when shapes are selected or deselected,
        /// and is also raised one final time, with layerHandle = -1, indicating that all selection 
        /// events are complete.  This is particularly useful when multiple layers are 'Selectable', 
        /// and you want to know when all layers have completed their selection.
        /// </remarks>
        /// \new52 Updated in version 5.2
        public event _DMapEvents_SelectionChangedEventHandler SelectionChanged;
        void _DMapEvents.SelectionChanged(int layerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when mouse cursor is being moved by user and the cursor enters or leaves neighborhood of particular shape.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer the shape belongs to.</param>
        /// <param name="shapeIndex">Index of the shape.</param>
        /// <remarks>The event is fired when:
        /// - AxMap.CursorMode is set to tkCursorMode.cmIdentify tool or one of the editing tools (tkCursorMode.cmAddShape, tkCursorMode.cmEditShape, etc.);
        /// - number shapes within visible extents is smaller than GlobalSettings.HotTrackingMaxShapeCount.
        /// 
        /// For tkCursorMode.cmIdentify tool the shape will be automatically highlighted (see AxMap.Identifier for details);
        /// For editing cursors vertices of the shape under cursor will be displayed (see ShapeEditor.HighlightVertices property).
        /// </remarks>
        public event _DMapEvents_ShapeHighlightedEventHandler ShapeHighlighted;
        void _DMapEvents.ShapeHighlighted(int layerHandle, int shapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when user click on a shape with tkCursorMode.cmIdentify tool active.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer.</param>
        /// <param name="shapeIndex">Index of the shape.</param>
        /// <param name="projX">X coordinate of mouse click position in map coordinates.</param>
        /// <param name="projY">Y coordinate of mouse click position in map coordinates</param>
        public event _DMapEvents_ShapeIdentifiedEventHandler ShapeIdentified;
        void _DMapEvents.ShapeIdentified(int layerHandle, int shapeIndex, double pointX, double pointY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when shape being created or edited has topological errors and therefore can't be saved to the layer.
        /// </summary>
        /// <param name="errorMessage">Message about the reasons as to why the validation has failed.</param>
        public event _DMapEvents_ShapeValidationFailedEventHandler ShapeValidationFailed;
        void _DMapEvents.ShapeValidationFailed(string errorMessage)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when complete set of tiles has been loaded for the new map extents.
        /// </summary>
        /// <param name="snapshot">True in case the loading of tiles was done as a part of making snapshot of the map.</param>
        /// <param name="key">A key of operation set in AxMap.LoadTilesForSnapshot method.</param>
        public event _DMapEvents_TilesLoadedEventHandler TilesLoaded;
        void _DMapEvents.TilesLoaded(bool snapShot, string key, bool fromCache)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired when an operations is added or removed from undo/redo list of interactive ShapeEditor.
        /// </summary>
        public event EventHandler UndoListChanged;
        void _DMapEvents.UndoListChanged()
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// This event is fired before a shape which is being created or edited is about to be saved back to the layer.
        /// </summary>
        /// <param name="layerHandle">Handle of the layer.</param>
        /// <param name="shape">Shape to be validated.</param>
        /// <param name="cancel">Passed by reference. This value should be set to tkMwBoolean.blnTrue in case shape don't pass custom validation.</param>
        /// <remarks>ShapeEditor performs its own validation determined by ShapeEditor.ValidationMode, so there is 
        /// no need to run the same checks once again here (like Shape.IsValid). However if some form of custom
        /// rules should be enforced, this is the right place to do it.</remarks>
        public event _DMapEvents_ValidateShapeEventHandler ValidateShape;
        void _DMapEvents.ValidateShape(int layerHandle, Shape shape, ref tkMwBoolean cancel)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// This event is fired after grid datasource was added to the map (AxMap.AddLayer).
        /// </summary>
        /// <param name="layerHandle">Handle of the layer.</param>
        /// <param name="gridFilename">The filename of the original datasource.</param>
        /// <param name="bandIndex">Index of band which is used for visualization.</param>
        /// <param name="isUsingProxy">Whether the datasource is rendered by Image class directly or
        /// by using an RBG image that was created to serve as proxy (Grid.CreateImageProxy).</param>
        public event _DMapEvents_GridOpenedEventHandler GridOpened;
        void _DMapEvents.GridOpened(int layerHandle, string gridFilename, int bandIndex, bool isUsingProxy)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This event is fired after the user digitized a new point and held the ALT button.
        /// Can be used to modify the coordinates.
        /// </summary>
        /// <param name="pointX">X coordinate</param>
        /// <param name="pointY">Y coordinate</param>
        /// \new510 Added in version 5.1.0
        public event _DMapEvents_BeforeVertexDigitizedEventHandler BeforeVertexDigitized;
        void _DMapEvents.BeforeVertexDigitized(ref double pointX, ref double pointY)
        {
            throw new NotImplementedException();
        }
        
        /// @}
        #endregion

        /// @}
    }
#if nsp
}
#endif
