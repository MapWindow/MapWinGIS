using System;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Facilitates interactive creation and editing of vector shapes.
    /// </summary>
    /// <remarks>
    /// 
    /// 
    /// <b>A. General:</b>
    /// 
    /// Editor work with layers which have their Shapefile.InteractiveEditing property set to true.
    /// This may be both regular shapefile layers and OGR layers (shapefile buffer is accessible via OgrLayer.GetBuffer property for them).\n
    /// 
    /// <i>In case of OGR layers some other preconditions must also be met to ensure that the changes can be saved back to datasource (see 
    /// OgrLayer.get_SupportsEditing) even if the interactive editing itself is working.</i>
    /// 
    /// Each AxMap control has a single instance of shape editor associated with it available by AxMap.ShapeEditor property. \n
    /// 
    /// To start editing operation it's enough to set appropriate tool to AxMap.CursorMode property. The following editing tools are 
    /// currently available: 
    /// - cmAddShape, 
    /// - cmEditShape, 
    /// - cmMoveShapes, 
    /// - cmRotateShapes, 
    /// - cmSplitByPolyline, 
    /// - cmSplitByPolygon, 
    /// - cmEraseByPolygon,
    /// - cmClipByPolygon.
    /// 
    /// <b>B. Internal data storage.</b>
    /// 
    /// Editor can store points and parts of only a single shape at a time. It supports all major shape types: points, multipoints, 
    /// polylines, polygons. Points entered by user with mouse are stored in internal buffer which can be accessed with
    /// ShapeEditor.RawData property. \n
    /// 
    /// The data can be validated with ShapeEditor.ValidatedShape. All built-in tools perform validation before saving a new shape to the layer. 
    /// If validation fails AxMap.ShapeValidationFailed event will be fired to notify the user. 
    /// Invalid data can be be discarded by pressing Esc button or by calling ShapeEditor.Clear.\n
    /// 
    /// <i>This validation behavior will also prevent changing the active tool (AxMap.CursorMode) while the editing operation is in progress.</i>
    /// 
    /// The rendering of the point data is carried out independently of the layer. Visualization options can be changed by ShapeEditor.FillColor, 
    /// ShapeEditor.LineColor, ShapeEditor.FillTransparency. When editing of a new shape starts this properties
    /// will automatically be set with the values from the parent layer, so the shape being edited will look similar to its layer.
    /// To force redraw of the editor programatically it's enough to call AxMap.Redraw2 with RedrawDynamicTools parameter. 
    /// 
    /// <b>C. Creation of new shapes.</b>
    /// 
    /// - activated by settings AxMap.CursorMode = cmAddShape;
    /// - only creation of single part shapes supported;
    /// - layer to add the shape to is determined after the first mouse click by handling AxMap.ChooseLayer event;
    /// - new points can be added by left mouse button;
    /// - previous points can be removed by Ctrl+Z shortcut;
    /// - newly entered points by default are snapped to the vertices of exiting shapes (see ShapeEditor.SnapBehavior);
    /// - to finish the creation of shape Ctrl + left mouse click is used;
    /// - if the data passes validation, a new shape will automatically be inserted to the specified layer;
    /// 
    /// <b>D. Editing of vertices and parts.</b>
    /// 
    /// The data from existing shapes during the editing is copied to the editor while the original shape
    /// is hidden by setting Shapefile.set_ShapeIsHidden to true. After editing is finished and data in ShapeEditor is successfully validated
    /// original shape is substituted with this new data. If the changes are discarded then Shapefile.set_ShapeIsHidden 
    /// property of the original shape is simply set to false.
    /// 
    /// Editing mode can be activated by settings AxMap.CursorMode = cmEditShape. Clicking on a shape from any layer with interactive 
    /// editing enabled will start editing session for this shape. After clicking on blank spot of the map without shapes
    /// an attempt to validate the shape and save the changes will be made. Vertices of shapes available for editing are highlighted
    /// under mouse cursor (ShapeEditor.HighlightVertices).
    /// 
    /// cmEditShape cursor supports 2 behaviors which can be set by ShapeEditor.EditorBehavior property:
    /// - vertex editor - adding, moving, deleting of vertices; moving a shape as a whole;
    /// - part editor - moving and deleting of separate shape parts.
    /// 
    /// To add new parts to the shape or create holes in polygon, ShapeEditor.StartOverlay method can be used. 
    /// It allows user to digitize a new polygon which afterwards will be overlayed with original shape in
    /// either union (eoAddPart) or difference mode (eoRemovePart).
    /// 
    /// <b>E. Polyline and polygon overlays:</b>
    /// 
    /// Overlays a single vector layer with custom digitized polygon or polyline. Currently the following tools of this type are available:
    /// - cmSplitByPolyline - splits polylines or polygons into multiple shapes;
    /// - cmSplitByPolygon - can be used to create a whole in polygon and a separate polygon shape to fill this hole;
    /// - cmEraseByPolygon - shapes and parts of shapes that intersect with the polygon will be removed from the layer;
    /// - cmClipByPolygon - only parts of shapes that intersect the polygon will remain, all other shapes will be removed.
    /// 
    /// At first the tools work like cmAddShape tool, i.e. allow to digitize either polyline or polygon. After it's finished 
    /// (Ctrl + left mouse button) and the newly digitized shape is checked for validity, AxMap.ChooseLayer event will be fired.
    /// If the layer handle provided by user in this event represents vector layer in interactive editing mode, then the requested
    /// operation will be performed on this layer.
    /// 
    /// <b>F. Group operations.</b>
    /// 
    /// These operations work on a number of selected shapes within a single layer. The layer can be selected by handing AxMap.ChooseLayer event.
    /// 
    /// Currently there are 2 built-in tools available:
    /// - cmRotateShapes - can rotate selected shapes around the centre of the bounding box;
    /// - cmMoveShapes - can move selected shapes.
    /// 
    /// <i>Technically group operations aren't related to the ShapeEditor class, however they are mentioned here to cover all the available tools.</i>
    /// 
    /// <b>G. Undo list.</b>
    /// 
    /// All the operations performed by user are registered in so called Undo list (AxMap.UndoList). User can revert them (Ctrl+Z 
    /// or UndoList.Undo) or apply once again (Ctrl+Shift+Z or UndoList.Redo).
    /// When interactive editing session is taking place no other editing must be done programatically without registering
    /// in the Undo list. Otherwise Undo list will become invalid.\n
    /// 
    /// \note The fully working implementation of the shape editor can be examined in the the Demo application 
    /// included in MapWinGIS installation (starting from v4.9.3). The source code for this application is available in
    /// the <a href = "https://github.com/MapWindow/MapWinGIS/tree/master/demo" target="_blank">repository</a>.
    /// </remarks>
    /// \new493 Added in version 4.9.3
#if nsp
#if upd
    public class ShapeEditor : MapWinGIS.IShapeEditor
        #else        
            public class IShapeEditor
        #endif
    #else
        public class ShapeEditor
    #endif
    {
        /// <summary>
        /// Calculates the area of polygon being edited. 
        /// </summary>
        /// <remarks>Precise calculations on ellipsoid will be used if map projection is set and compatible
        /// with WGS84. Otherwise simply Euclidean geometry will be used.</remarks>
        public double Area
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets type of the bearing to be display for line segments.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public tkBearingType BearingType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets area display mode for polygon shapes. The default value is admNone.
        /// </summary>
        public tkAreaDisplayMode AreaDisplayMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Clears the editor returning it to an empty state. 
        /// </summary>
        /// <remarks>The method will discard any changes made to the shape being edited.</remarks>
        public void Clear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Applies visualization options defined by ShapeDrawingOptions instance.
        /// </summary>
        /// <remarks>This method can be used to make the editor look consistent
        /// with the way subject shape looks in regular mode.</remarks>
        /// <param name="Options">Instance of visualization options.</param>
        /// \see Shapefile.DefaultDrawingOptions
        public void CopyOptionsFrom(ShapeDrawingOptions Options)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets editor behavior during the editing of existing shape, either vertex editor or part editor.
        /// </summary>
        public tkEditorBehavior EditorBehavior
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the state an editor is currently in. See tkEditorState enumeration for details.
        /// </summary>
        public tkEditorState EditorState
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets fill color for the shape being edited. Applies for polygon shapes only.
        /// </summary>
        public uint FillColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets fill transparency for the shape being edited. Applies for polygon shapes only.
        /// </summary>
        public byte FillTransparency
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a Callback object which handles progress and error messages.
        /// </summary>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns true if editor has changes caused by user input.
        /// </summary>
        public bool HasChanges
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a value indicating whether calculations are performed taking into account the shape of Earth 
        /// (when map projection is defined), or on 2D plane (Euclidean geometry).
        /// </summary>
        public bool IsUsingEllipsoid { get; private set; }

        /// <summary>
        /// Gets the length of measured path (in meters if WGS84 compatible projection is set for map and in current map units otherwise).
        /// </summary>
        public double Length { get; private set; }

        /// <summary>
        /// Gets or sets a value indicating whether area will be displayed during creation or editing of polygons.
        /// </summary>
        public bool ShowArea
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the number of decimal degrees to be used to display area.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public int AreaPrecision
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the number of decimal degrees to be used to display length.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public int LengthPrecision
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets angle format to be used to display bearing.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public tkAngleFormat AngleFormat
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the number of decimal degrees to be used to display bearing.
        /// </summary>
        /// <remarks>This setting is not used when AngleFormat is set to minutes or seconds.</remarks>
        /// \new493 Added in version 4.9.3
        public int AnglePrecision
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a value indicating whether bearing of the line segments will be displayed.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public bool ShowBearing
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a value indicating whether length of the line segments will be displayed.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public bool ShowLength
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets value indicating whether vertices of shapes will be highlighted 
        /// on mouse move when interactive editing tools are used.
        /// </summary>
        public tkLayerSelection HighlightVertices
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a value indicating whether a new shape is currently being created.
        /// </summary>
        /// <remarks>The property will return true is ShapeEditor is either in 
        /// esDigitize, esDigitizeUnbound or esOverlay states.</remarks>
        /// \see ShapeEditor.EditorState
        public bool IsDigitizing
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns true if the editor is empty.
        /// </summary>
        /// <remarks>Empty means that its underlying shape object has no points. However 
        /// ShapeEditor.EditorState can be different from esNone.</remarks>
        public bool IsEmpty
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// A text string associated with object. Any value can be stored by developer in this property.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the code of last error which took place inside this object.
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets layer handle of shape currently being edited. 
        /// </summary>
        /// <remarks>This property will be set automatically by ShapeEditor.StartEdit method, 
        /// and reverted back to -1 value by ShapeEditor.Clear or ShapeEditor.SaveChanges methods.</remarks>
        public int LayerHandle
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets length display mode.
        /// </summary>
        public tkLengthDisplayMode LengthDisplayMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets line color for the shape being edited.
        /// </summary>
        public uint LineColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets line width for the shape being edited.
        /// </summary>
        public float LineWidth
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a value indicating whether the indices of shape's vertices are visible.
        /// </summary>
        public bool IndicesVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets underlying shape data without any attempt to validate it.
        /// </summary>
        public Shape RawData
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Saves any changes made by user.
        /// </summary>
        /// <remarks>The editor will try to validate underlying shape on this call. If validation succeeds
        /// the changes will be passed to the original shapefile or other actions defined by
        /// current interactive tool will be triggered.
        /// Depending on editor state the actions will be:
        /// - esDigitize: new shape will be added to the shapefile set by ShapeEditor.LayerHandle property;
        /// - esEdit: shapefile will be updated with modified version of shape;
        /// - esDigitizeUnbound: a tool which started the unbound mode execute the appropriate action 
        /// (clipping, selection by polygon, etc.);
        /// - esOverlay: current overlay operation (i.e. eoAddPart or eoRemovePart) will be discarded, editor
        /// will return in esEdit state after which an attempt will be made to validate and save shape in regular manner.
        /// </remarks>
        /// <returns>True on success.</returns>
        /// \see AxMap.CursorMode, ShapeEditor.EditorState
        public bool SaveChanges()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the state of the class to the string
        /// </summary>
        /// <returns>A string with the state or an empty string on failure.</returns>
        public string Serialize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restores the state of object from the string.
        /// </summary>
        /// <param name="state">A string generated by ShapeEditor.Serialize() method</param>
        /// <returns>True on success.</returns>
        public bool Deserialize(string state)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the index of currently selected vertex.
        /// </summary>
        public int SelectedVertex
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets index of the shape being edited.
        /// </summary>
        /// <remarks>This property will be set automatically by ShapeEditor.StartEdit method, 
        /// and reverted back to -1 value by ShapeEditor.Clear or ShapeEditor.SaveChanges methods.</remarks>
        public int ShapeIndex
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets shape type of the shape currently being edited.
        /// </summary>
        public ShpfileType ShapeType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets snapping behavior for Shape Editor (snaps vertices to the vertices of 
        /// exiting shapes on other layers).
        /// </summary>
        public tkLayerSelection SnapBehavior
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets snapping tolerance in screen coordinates.
        /// </summary>
        public double SnapTolerance
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Starts editing of a given shape in specified shapefile.
        /// </summary>
        /// <param name="LayerHandle">Layer handle of the shapefile.</param>
        /// <param name="ShapeIndex">Index of shape to edit.</param>
        /// <returns>True on success.</returns>
        /// <remarks>The method will populate the editor with vertices of specified shape,
        /// hide original shape with Shapefile.put_ShapeIsHidden and set ShapeEditor.EditorState to esEdit.</remarks>
        public bool StartEdit(int LayerHandle, int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Starts overlay operation for the current shape.
        /// </summary>
        /// <remarks>Editor must be in esEdit state already, i.e. ShapeEditor.StartEdit must be called first.</remarks>
        /// <param name="overlayType">Type of overlay operation.</param>
        /// <returns>True on success.</returns>
        public bool StartOverlay(tkEditorOverlay overlayType)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Initializes editor for creation of unbound shape.
        /// </summary>
        /// <remarks>Unbound shape is the one which is not linked to particular shapefile. This
        /// method is used internally by a number of built-in tools, 
        /// like cmSelectByPolygon or cmSplitByPolyline.</remarks>
        /// <param name="ShpType">Type of shape to be created.</param>
        /// <returns>True on success.</returns>
        public bool StartUnboundShape(ShpfileType ShpType)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Allows to undo the last vertex entered by user.
        /// </summary>
        /// <remarks>The method works when new shape is being created, i.e. ShapeEditor.IsDigitizing return true.</remarks>
        /// <returns>True on success, false if the editor is empty and there is no points to undo.</returns>
        public bool UndoPoint()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Validates the data stored by editor and returns it as a shape.
        /// </summary>
        public Shape ValidatedShape
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets validation mode for the editor. 
        /// </summary>
        /// <remarks>See tkEditorValidation enumeration for details.</remarks>
        public tkEditorValidation ValidationMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets value indicating whether vertices edited shapes are visible.
        /// </summary>
        public bool VerticesVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the description of the specific error code.
        /// </summary>
        /// <param name="ErrorCode">The error code returned by LastErrorCode property.</param>
        /// <returns>String with the description.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets coordinates of specified point of edited shape.
        /// </summary>
        /// <param name="pointIndex">Index of point.</param>
        /// <param name="x">X in projected map coordinates.</param>
        /// <param name="y">Y in projected map coordinates.</param>
        /// <returns>True if index of point within bounds.</returns>
        public bool get_PointXY(int pointIndex, out double x, out double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets coordinates of specified point of edited shape.
        /// </summary>
        /// <param name="pointIndex">Index of point.</param>
        /// <param name="x">X in projected map coordinates.</param>
        /// <param name="y">Y in projected map coordinates.</param>
        /// <returns>True if index of point within bounds.</returns>
        public bool put_PointXY(int pointIndex, double x, double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns directional angle (bearing) of a given segment of edited shape.
        /// </summary>
        /// <param name="segmentIndex">Segment index to calculate angle for.</param>
        /// <returns>Angle in degrees.</returns>
        public double get_SegmentAngle(int segmentIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets length of a given segment of edited shape.
        /// </summary>
        /// <param name="segmentIndex">Segment index to calculate length for.</param>
        /// <returns>Length of segment in meters.</returns>
        /// <remarks>If map projection is set and compatible with WGS84 geodesic distance will be returned.
        /// Otherwise Euclidean geometry will be used.</remarks>
        public double get_SegmentLength(int segmentIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets number of points the shape being edited has.
        /// </summary>
        public int numPoints
        {
            get { throw new NotImplementedException(); }
        }
    }
#if nsp
}
#endif

