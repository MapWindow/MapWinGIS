using System;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Provides means for interactive creation and editing of vector shapes.
    /// </summary>
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
        /// Gets or sets angle display mode. The default value is AngleNone.
        /// </summary>
        public tkAngleDisplay AngleDisplayMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

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
        /// ShapeEditor.EditorState can be differnt from esNone.</remarks>
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

