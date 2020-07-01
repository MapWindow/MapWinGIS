
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Holds the set of options for visualization of shapefiles.
    /// </summary>
    /// \dot
    /// digraph shapedrawingoptions_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 1, fontname=Helvetica, fontsize=9, color = tan, style = filled, height = 0.3, width = 1.0];
    /// sdo [ label="ShapeDrawingOptions" URL="\ref ShapeDrawingOptions"];
    /// ptrn [ label="LinePattern" URL="\ref LinePattern"];
    /// img [ label="Image" URL="\ref Image"];
    /// 
    /// node [style = dashed, color = gray,  height = 0.3, width = 1.0];
    /// sf [ label="Shapefile" URL="\ref Shapefile"];
    /// cat [ label="ShapefileCategory" URL="\ref ShapefileCategory"];
    /// 
    /// edge [ dir = "none", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// sdo -> ptrn [ URL="\ref ShapeDrawingOptions.LinePattern", tooltip = "ShapeDrawingOptions.LinePattern", headlabel = "   1"];
    /// sdo -> img [ URL="\ref ShapeDrawingOptions.Picture", tooltip = "ShapeDrawingOptions.Picture", headlabel = "   1"];
    /// sf -> sdo [ URL="\ref Shapefile.DefaultDrawingOptions()", tooltip = "Shapefile.DefaultDrawingOptions()", headlabel = "   1"];
    /// cat -> sdo [ URL="\ref ShapefileCategory.DrawingOptions", tooltip = "ShapefileCategory.DrawingOptions", headlabel = "   1"];
    /// }
    /// \enddot
    /// 
    /// <a href = "diagrams.html">Graph description</a>\n\n
    /// <b>I.</b> The class is used for rendering different types of shapefiles: point (multipoint), polyline, polygon. 
    /// The set of options available for these types differ accordingly. The whole set options can be split into
    /// following groups:\n
    /// - fill options;
    /// - outline options;
    /// - line pattern options;
    /// - point symbol options;
    /// - vertices options.
    /// .
    /// The table shows relation between shape types and the set of options available for them.
    /// <div align = "center">
    /// <table>
    /// <tr>
    ///     <th>%Shapefile type</th>
    ///     <th align = "center">Fill</th>
    ///     <th align = "center">Outline</th>
    ///     <th align = "center">Line pattern</th>
    ///     <th align = "center">%Point symbols</th>
    ///     <th align = "center">Vertices</th>
    /// </tr>
    /// <tr align = "center">
    ///     <td width = "25%" align = "left">%Point, MultiPoint</td>
    ///     <td width = "10%">+</td>
    ///     <td width = "10%">+</td>
    ///     <td width = "20%">-</td>
    ///     <td width = "20%">+</td>
    ///     <td width = "15%">-</td>
    /// </tr>
    /// <tr align = "center">
    ///     <td align = "left">%Polyline</td>
    ///     <td >-</td>
    ///     <td >+</td>
    ///     <td >+</td>
    ///     <td >-</td>
    ///     <td >+</td>
    /// </tr>
    /// <tr align = "center">
    ///     <td align = "left">%Polygon</td>
    ///     <td >+</td>
    ///     <td >+</td>
    ///     <td >-</td>
    ///     <td >-</td>
    ///     <td >+</td>
    /// </tr>
    /// </table>
    /// </div>
    /// Below the listed sets of options will be described in more detail. Fill options and point symbols provides
    /// different types of symbology, like standard, hatch, gradient, texture for fill; and standard, font character, picture
    /// for point symbols. \n\n
    /// <b>1. Fill options. </b>They are applicable for polygon, point and multipoint shapefiles. Use ShapeDrawingOptions.FillType to change the type of fill.\n\n
    /// \dot
    /// digraph fill_type {
    /// splines = ortho;
    /// ranksep = 0.15;
    /// nodesep = 0.3;
    /// node    [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// solid   [ label="Standard"];
    /// hatch   [ label="Hatch"];
    /// gradient [ label="Gradient"];
    /// texture [ label="Texture"];
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" FillVisible\l FillTransparency\l FillColor\l\l\l\l\l"];
    /// s2    [label=" FillVisible\l FillTransparency\l FillColor\l FillHatchStyle\l FillBgColor\l FillBgTransparent\l\l"];
    /// s3    [label=" FillVisible\l FillTransparency\l FillColor\l FillColor2\l FillGradientType\l FillGradientBounds\l FillRotation\l"];
    /// s4    [label=" FillVisible\l FillTransparency\l Picture\l PictureScaleX\l PictureScaleY\l FillRotation\l\l"];
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" ]
    /// 
    /// solid -> s1;
    /// hatch -> s2;
    /// gradient -> s3;
    /// texture -> s4;
    /// }
    /// \enddot
    /// \code
    /// Shapefile sf = some_shapefile;
    /// ShapeDrawingOptions options = sf.DefaultDrawingOptions;
    /// Utils utils = new Utils();
    /// 
    /// // standard fill
    /// options.FillType = tkFillType.ftStandard;
    /// options.FillColor = utils.ColorByName(tkMapColor.Red);
    /// 
    /// // hatch fill
    /// options.FillType = tkFillType.ftHatch;
    /// options.FillBgTransparent = true;
    /// options.FillHatchStyle = tkGDIPlusHatchStyle.hsHorizontal;
    /// 
    /// // gradient for each shape
    /// options.FillType = tkFillType.ftGradient;
    /// options.FillColor2 = Utils.ColorByName(tkMapColor.Blue);
    /// options.FillGradientBounds = tkGradientBounds.gbPerShape;
    /// 
    /// // texture
    /// options.FillType =tkFillType.ftTexture;
    /// Image img = new Image();
    /// img.Open("some_arguments");
    /// options.Picture = img;
    /// \endcode
    /// <b>2. %Point symbols.</b> They are applicable for point (multipoint) shapefiles only. Use ShapeDrawingOptions.PointType to change the type of symbol.\n\n
    /// \dot
    /// digraph point_type {
    /// splines = ortho;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// st    [ label="Standard"];
    /// font  [ label="Font character"];
    /// pct   [ label="Picture"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" PointRotation    \l PointSize\l PointShape\l PointSidesCount\l PointSidesRatio\l SetDefaultPointSymbol()\l"];
    /// s2    [label=" PointRotation    \l PointSize\l PointCharacter\l FontName\l\l\l"];
    /// s3    [label=" PointRotation    \l Picture\l PictureScaleX\l PictureScaleY\l\l\l"];
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" ]
    /// st -> s1;
    /// font -> s2;
    /// pct -> s3;
    /// }
    /// \enddot
    /// \code
    /// Shapefile sf = some_shapefile;
    /// ShapeDrawingOptions options = sf.DefaultDrawingOptions;
    /// options.FillColor = utils.ColorByName(tkMapColor.Red);
    /// Utils utils = new Utils();
    /// 
    /// // standard symbol
    /// options.PointType = tkPointSymbolType.ptSymbolStandard;
    /// options.PointShape = tkPointShapeType.ptShapeStar;
    /// options.PointSidesCount = 5;
    /// 
    /// // the same using predefined symbols (a short for 3 calls listed above)
    /// options.SetDefaultPointSymbol(tkDefaultPointSymbol.dpsStar);
    /// 
    /// // font character
    /// options.PointType = tkPointSymbolType.ptSymbolFontCharacter;
    /// options.PointCharacter = (short)'a';
    /// options.FontName = "Arial";
    /// 
    /// // picture
    /// options.PointType = tkPointSymbolType.ptSymbolPicture;
    /// Image img = new Image();
    /// img.Open("some_arguments");
    /// options.Picture = img;
    /// options.PointRotation = 45.0;   // rotate picture by 45 degrees
    /// \endcode
    /// 3. <b>Outline, line pattern and vertices. </b>Outline options applicable for all shapefile types, line patterns - for polylines only, 
    /// vertices - for polylines and polygons. \n\n
    /// \dot
    /// digraph point_type {
    /// splines = ortho;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// out    [ label="Outline"];
    /// ptrn  [ label="Line pattern"];
    /// vert   [ label="Vertices"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" LineVisible\l LineStipple\l LineWidth\l LineColor\l LineTransparency\l"];
    /// s2    [label=" LineVisible\l LinePattern\l UseLinePattern\l\l\l"];
    /// s3    [label=" VerticesVisible\l VerticesColor\l VerticesFillVisible\l VerticesSize\l VerticesType\l"];
    /// 
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" ]
    /// out -> s1;
    /// ptrn -> s2;
    /// vert -> s3;
    ///}
    /// \enddot
    /// 
    /// <b>II.</b> The drawing options for shapefile are split into:
    /// -# Default drawing options (Shapefile.DefaultDrawingOptions).
    /// -# Drawing options for shapefile categories (ShapefileCategory.DrawingOptions).
    /// .
    /// The shapes that doesn't belong to any visualization category will be painted with default options.
    /// To return the index of the category a shape belongs to, call Shapefile.get_ShapeCategory(). 
    /// The value -1 means that default drawing options will be used.
    /// \code 
    /// Shapefile sf = some_shapefile;
    /// 
    /// // specifies a visualization category
    /// ShapefileCategory ct = sf.Categories.Add("Thick");
    /// ct.DrawingOptions.LineWidth = 4.0;
    /// 
    /// // let's add first 10 shapes to it
    /// for (int i = 0; i < 10; i++) {
    ///     sf.put_ShapeCategory(i, 0);  // 0 - the index of category
    /// }
    /// 
    /// // now let's set the width for the rest lines
    /// sf.DefaultDrawingOptions.LineWidth = 2.0;
    /// \endcode
    /// The class also provides a set of methods to draw map legend on the device context 
    /// other than AxMap: DrawShape(), DrawLine(), DrawPoint(), DrawRectangle().
    /// \new48 Added in version 4.8
    
    #if nsp
        #if upd
            public class ShapeDrawingOptions : MapWinGIS.IShapeDrawingOptions
        #else        
            public class IShapeDrawingOptions
        #endif
    #else
        public class ShapeDrawingOptions
    #endif
    {
        #region IShapeDrawingOptions Members

        /// <summary>
        /// Creates a new instance of ShapeDrawingOptions class by copying all the properties.
        /// </summary>
        /// <returns>The reference to the new instance of the ShapeDrawingOptions class or NULL reference on failure.</returns>
        public ShapeDrawingOptions Clone()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restores the state of object from the string.
        /// </summary>
        /// <param name="newVal">A string generated by ShapeDrawingOptions.Serialize() method.</param>
        public void Deserialize(string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a line using the stored options on specified device context.
        /// </summary>
        /// <remarks>The method can be used to draw map legend.</remarks>
        /// <param name="hdc">The handle of the device context. Can be obtained with .NET Graphics.GetHDC() method.</param>
        /// <param name="x">The x coordinate of the upper left corner of the drawing.</param>
        /// <param name="y">The y coordinate of the upper left corner of the drawing. </param>
        /// <param name="Width">The width of the drawing.</param>
        /// <param name="Height">The height of the drawing.</param>
        /// <param name="drawVertices">A value which indicates whether or not vertices of the line will be drawn.</param>
        /// <param name="clipWidth">The width of the clipping rectangle.</param>
        /// <param name="clipHeight">The height of the clipping rectangle.</param>
        /// <param name="BackColor">The back color of the device context the drawing is performed at.
        /// The value should be specified to ensure correct blending when semi-transparent colors are used.</param>
        /// <param name="backAlpha">The alpha component of the back color of the device context the drawing is performed at.
        /// Specify a value of 255 for the background color to be fully opaque, and 0 for the background to be transparent.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new495 Optional backAlpha parameter added in version 4.9.5
        public bool DrawLine(IntPtr hdc, float x, float y, int Width, int Height, bool drawVertices, int clipWidth, int clipHeight, uint BackColor, byte backAlpha)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a line using the stored options on the specified device context.
        /// </summary>
        /// <remarks>The method can be used to draw map legend.</remarks>
        /// <param name="hdc">The handle of the device context.</param>
        /// <param name="x">The x coordinate of the upper left corner of the drawing.</param>
        /// <param name="y">The y coordinate of the upper left corner of the drawing. </param>
        /// <param name="Width">The width of the drawing.</param>
        /// <param name="Height">The height of the drawing.</param>
        /// <param name="drawVertices">A value which indicates whether or not vertices of the line will be drawn.</param>
        /// <param name="clipWidth">The width of the clipping rectangle.</param>
        /// <param name="clipHeight">The height of the clipping rectangle.</param>
        /// <param name="BackColor">The back color of the device context the drawing is performed at.
        /// The value should be specified to ensure correct blending when semi-transparent colors are used.</param>
        /// <param name="backAlpha">The alpha component of the back color of the device context the drawing is performed at.
        /// Specify a value of 255 for the background color to be fully opaque, and 0 for the background to be transparent.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new495 Optional backAlpha parameter added in version 4.9.5
        public bool DrawLineVB(int hdc, float x, float y, int Width, int Height, bool drawVertices, int clipWidth, int clipHeight, uint BackColor, byte backAlpha)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a point symbol using the stored options on the specified device context.
        /// </summary>
        /// <remarks>The method can be used to draw map legend.</remarks>
        /// <param name="hdc">The handle of the device context. Can be obtained with .NET Graphics.GetHDC() method.</param>
        /// <param name="x">The x coordinate of the upper left corner of the drawing.</param>
        /// <param name="y">The y coordinate of the upper left corner of the drawing.</param>
        /// <param name="clipWidth">The width of the clipping rectangle.</param>
        /// <param name="clipHeight">The height of the clipping rectangle.</param>
        /// <param name="BackColor">The back color of the device context the drawing is performed at.
        /// The value should be specified to ensure correct blending when semi-transparent colors are used.</param>
        /// <param name="backAlpha">The alpha component of the back color of the device context the drawing is performed at.
        /// Specify a value of 255 for the background color to be fully opaque, and 0 for the background to be transparent.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new495 Optional backAlpha parameter added in version 4.9.5
        public bool DrawPoint(IntPtr hdc, float x, float y, int clipWidth, int clipHeight, uint BackColor, byte backAlpha)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a point symbol using the stored options on the specified device context.
        /// </summary>
        /// <remarks>The method can be used to draw map legend.</remarks>
        /// <param name="hdc">The handle of the device context.</param>
        /// <param name="x">The x coordinate of the upper left corner of the drawing.</param>
        /// <param name="y">The y coordinate of the upper left corner of the drawing.</param>
        /// <param name="clipWidth">The width of the clipping rectangle.</param>
        /// <param name="clipHeight">The height of the clipping rectangle.</param>
        /// <param name="BackColor">The back color of the device context the drawing is performed at.
        /// The value should be specified to ensure correct blending when semi-transparent colors are used.</param>
        /// <param name="backAlpha">The alpha component of the back color of the device context the drawing is performed at.
        /// Specify a value of 255 for the background color to be fully opaque, and 0 for the background to be transparent.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new495 Optional backAlpha parameter added in version 4.9.5
        public bool DrawPointVB(int hdc, float x, float y, int clipWidth, int clipHeight, uint BackColor, byte backAlpha)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a rectangle using the stored options on the specified device context.
        /// </summary>
        /// <remarks>The method can be used to draw map legend.</remarks>
        /// <param name="hdc">The handle of the device context. Can be obtained with .NET Graphics.GetHDC() method.</param>
        /// <param name="x">The x coordinate of the upper left corner of the drawing.</param>
        /// <param name="y">The y coordinate of the upper left corner of the drawing.</param>
        /// <param name="Width">The width of the drawing.</param>
        /// <param name="Height">The height of the drawing.</param>
        /// <param name="drawVertices">A value which indicates whether or not vertices of the rectangle will be drawn.</param>
        /// <param name="clipWidth">The width of the clipping rectangle.</param>
        /// <param name="clipHeight">The height of the clipping rectangle.</param>
        /// <param name="BackColor">The back color of the device context the drawing is performed at.
        /// The value should be specified to ensure correct blending when semi-transparent colors are used.</param>
        /// <param name="backAlpha">The alpha component of the back color of the device context the drawing is performed at.
        /// Specify a value of 255 for the background color to be fully opaque, and 0 for the background to be transparent.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new495 Optional backAlpha parameter added in version 4.9.5
        public bool DrawRectangle(IntPtr hdc, float x, float y, int Width, int Height, bool drawVertices, int clipWidth, int clipHeight, uint BackColor, byte backAlpha)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a rectangle using the stored options on the specified device context.
        /// </summary>
        /// <remarks>The method can be used to draw map legend.</remarks>
        /// <param name="hdc">The handle of the device context. Can be obtained with .NET Graphics.GetHDC() method.</param>
        /// <param name="x">The x coordinate of the upper left corner of the drawing.</param>
        /// <param name="y">The y coordinate of the upper left corner of the drawing.</param>
        /// <param name="Width">The width of the drawing.</param>
        /// <param name="Height">The height of the drawing.</param>
        /// <param name="drawVertices">A value which indicates whether or not vertices of the rectangle will be drawn.</param>
        /// <param name="clipWidth">The width of the clipping rectangle.</param>
        /// <param name="clipHeight">The height of the clipping rectangle.</param>
        /// <param name="BackColor">The back color of the device context the drawing is performed at.
        /// The value should be specified to ensure correct blending when semi-transparent colors are used.</param>
        /// <param name="backAlpha">The alpha component of the back color of the device context the drawing is performed at.
        /// Specify a value of 255 for the background color to be fully opaque, and 0 for the background to be transparent.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new495 Optional backAlpha parameter added in version 4.9.5
        public bool DrawRectangleVB(int hdc, float x, float y, int Width, int Height, bool drawVertices, int clipWidth, int clipHeight, uint BackColor, byte backAlpha)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a shape using the stored options on the specified device context.
        /// </summary>
        /// <remarks>The method can be used to draw map legend.</remarks>
        /// <param name="hdc">The handle of the device context. Can be obtained with .NET Graphics.GetHDC() method.</param>
        /// <param name="x">The x coordinate of the upper left corner of the drawing.</param>
        /// <param name="y">The y coordinate of the upper left corner of the drawing.</param>
        /// <param name="Shape">A shape to draw. Point, polyline and polygon shapes are supported.</param>
        /// <param name="drawVertices">A value which indicates whether or not vertices of the line will be drawn. </param>
        /// <param name="clipWidth">The width of the clipping rectangle.</param>
        /// <param name="clipHeight">The height of the clipping rectangle.</param>
        /// <param name="BackColor">The back color of the device context the drawing is performed at.
        /// The value should be specified to ensure correct blending when semi-transparent colors are used.</param>
        /// <param name="backAlpha">The alpha component of the back color of the device context the drawing is performed at.
        /// Specify a value of 255 for the background color to be fully opaque, and 0 for the background to be transparent.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new495 Optional backAlpha parameter added in version 4.9.5
        public bool DrawShape(IntPtr hdc, float x, float y, Shape Shape, bool drawVertices, int clipWidth, int clipHeight, uint BackColor, byte backAlpha)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a shape using the stored options on the specified device context.
        /// </summary>
        /// <remarks>The method can be used to draw map legend.</remarks>
        /// <param name="hdc">The handle of the device context.</param>
        /// <param name="x">The x coordinate of the upper left corner of the drawing.</param>
        /// <param name="y">The y coordinate of the upper left corner of the drawing.</param>
        /// <param name="Shape">A shape to draw. Point, polyline and polygon shapes are supported.</param>
        /// <param name="drawVertices">A value which indicates whether or not vertices of the line will be drawn. </param>
        /// <param name="clipWidth">The width of the clipping rectangle.</param>
        /// <param name="clipHeight">The height of the clipping rectangle.</param>
        /// <param name="BackColor">The back color of the device context the drawing is performed at.
        /// The value should be specified to ensure correct blending when semi-transparent colors are used.</param>
        /// <param name="backAlpha">The alpha component of the back color of the device context the drawing is performed at.
        /// Specify a value of 255 for the background color to be fully opaque, and 0 for the background to be transparent.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new495 Optional backAlpha parameter added in version 4.9.5
        public bool DrawShapeVB(int hdc, float x, float y, Shape Shape, bool drawVertices, int clipWidth, int clipHeight, uint BackColor, byte backAlpha)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The graphical mode to be used to render symbology.
        /// </summary>
        public tkVectorDrawingMode DrawingMode
        {
            get {  throw new NotImplementedException(); }
            set {  throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the background color of the polygon fill. 
        /// </summary>
        /// <remarks>It is used for polygon shapes when hatch fill is chosen.</remarks>
        /// \see FillType
        public uint FillBgColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether a background will be drawn when ShapeDrawingOptions.FillType is set to ftHatch.
        /// </summary>
        public bool FillBgTransparent
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the fill color of the shape.
        /// </summary>
        /// <remarks>The color is used for point and polygon shapes when ShapeDrawingOptions.FillType is equal to ftSimple or ftHatch,
        /// or the gradient starting color is ShapeDrawingOptions.FillType is equal to ftGradient.</remarks>
        public uint FillColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the end color of the gradient fill.
        /// </summary>
        /// <remarks>It is used when ShapeDrawingOptions.FillType is equal to ftGradient.</remarks>
        public uint FillColor2
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the the value which defines the bounds of the gradient fill.
        /// </summary>
        public tkGradientBounds FillGradientBounds
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The type of the gradient to draw polygon fill of the point symbols and polygon shapes.
        /// </summary>
        /// \see FillType, FillColor, FillColor2.
        public tkGradientType FillGradientType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The hatch style of the fill for point symbols and polygon shapes.
        /// </summary>
        /// \see FillType, FillBgColor, FillBgTransparent.
        public tkGDIPlusHatchStyle FillHatchStyle
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the angle of rotation of the polygon fill in degrees. 
        /// </summary>
        /// <remarks>Affects the rotation of gradient and texture fill.</remarks>
        /// \see FillType
        public double FillRotation
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the transparency of the fill. Ranges from 0 (transparent) to 255 (opaque).
        /// </summary>
        public float FillTransparency
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The type of the polygon fill. See the enumeration for details.
        /// </summary>
        public tkFillType FillType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether the fill is visible.
        /// </summary>
        /// <remarks>Affects the drawing of point symbols and polygon shapes.</remarks>
        public bool FillVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The name of the font to use for drawing point symbols when ShapeDrawingOptions.PointType is equal to pstCharacter.
        /// </summary>
        public string FontName
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Retrieves the numeric code of the last error that took place in the instance class.
        /// </summary>
        /// <remarks>The usage of this property clears the error code.</remarks>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the line color of the shapes.
        /// </summary>
        /// <remarks>Affects the drawing of all shape types.</remarks>
        public uint LineColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets line pattern for rendering polyline shapefile.
        /// </summary>
        public LinePattern LinePattern
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the line style of the shapes.
        /// </summary>
        /// <remarks>Affects the drawing of all shape types.</remarks>
        public tkDashStyle LineStipple
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the transparency of lines. Ranges from 0 (transparent) to 255 (opaque).
        /// </summary>
        public float LineTransparency
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether lines will be visible while drawing shapes.
        /// </summary>
        /// <remarks>This affects drawing of polylines, as well as the drawing of outline of points symbols and polygons.</remarks>
        public bool LineVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the width of the lines to draw shapes.
        /// </summary>
        public float LineWidth
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the picture which will be used as texture brush (ShapeDrawingOptions.FillType = ftPicture) 
        /// or point symbol (ShapeDrawingOptions.PointType = ptSymbolPicture).
        /// </summary>
        /// <remarks>The property affects the drawing of point symbols and polygons.</remarks>
        public Image Picture
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the coefficient for scaling picture horizontally.
        /// </summary>
        public double PictureScaleX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the coefficient for scaling picture vertically.
        /// </summary>
        public double PictureScaleY
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Get or sets the ANSI code of the text character to use as point symbol.
        /// </summary>
        public short PointCharacter
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the rotation of the point symbol in degrees.
        /// </summary>
        public double PointRotation
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets an expression to calculate the rotation of the point symbol in degrees.
        /// When this is set it takes precedence over the PointRotation property.
        /// </summary>
        public double PointRotationExpression
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the type of reflection to apply to the point symbol.
        /// </summary>
        /// <remarks>This value is used when ShapeDrawingOptions.PointType is equal to ptSymbolFontCharacter or ptSymbolPicture.</remarks>
        /// \new495 Added in version 4.9.5
        public tkPointReflectionType PointReflection
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the predefined shape for the point symbol.
        /// </summary>
        /// <remarks>It is used when ShapeDrawingOptions.PointType is equal to ptSymbolStandard.</remarks>
        public tkPointShapeType PointShape
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the number of sides for some of the predefined symbols such as regular, star or cross.
        /// </summary>
        /// <remarks>This value is used when ShapeDrawingOptions.PointType is equal to ptSymbolStandard.</remarks>
        public int PointSidesCount
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the side ratio for some of the predefined point symbols such as regular, star and cross.
        /// </summary>
        /// <remarks>This value is used when ShapeDrawingOptions.PointType is equal to ptSymbolStandard.</remarks>
        public float PointSidesRatio
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the size of the point symbol including predefined symbols, characters and pictures.
        /// </summary>
        public float PointSize
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the type of the point symbols. 
        /// </summary>
        /// <remarks>The possible values include predefined symbols, text characters and pictures.</remarks>
        public tkPointSymbolType PointType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
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
        /// Sets a predefined combination of properties to set the commonly used symbols. 
        /// </summary>
        /// <remarks>This method changes PointType, PointShape, PointSidesCount, PointRotation and PointSidesRatio properties.</remarks>
        /// <param name="symbol">The predefined symbol to be set.</param>
        public void SetDefaultPointSymbol(tkDefaultPointSymbol symbol)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Provides a convenient way to set colors of the gradient fill.
        /// </summary>
        /// <param name="Color">The central color of the gradient.</param>
        /// <param name="range">The range of colors of each channel the gradient covers. 
        /// The values should be within 0-255.</param>
        public void SetGradientFill(uint Color, short range)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a string associated with the instance of class. Can store any information provided by developer.
        /// </summary>
        public string Tag
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or set the value which indicates whether line pattern will be used to render polyline shapefile.
        /// </summary>
        /// <remarks>This property won't be set to true unless line pattern with at least one line exists.</remarks>
        public bool UseLinePattern
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Gets or sets the color of vertices for the shapes.
        /// </summary>
        /// \see VerticesVisible
        public uint VerticesColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether the fill for shape vertices will be drawn.
        /// </summary>
        /// \see VerticesVisible
        public bool VerticesFillVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Gets or sets the size of the shapes's vertices
        /// </summary>
        public int VerticesSize
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the type of shape vertices.
        /// </summary>
        public tkVertexType VerticesType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether vertices of the shape will be visible. The default values is false.
        /// </summary>
        /// <remarks>The visibility of point symbols for point shapefiles is not affected by this property.</remarks>
        public bool VerticesVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the values which indicates whether shapes will be visible.
        /// </summary>
        public bool Visible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the text description of the specified error code.
        /// </summary>
        /// <param name="ErrorCode">The numeric error code retrieved by ShapeDrawingOptions.LastErrorCode property.</param>
        /// <returns>The description of the error.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        #endregion

        /// <summary>
        /// Gets or sets a value indicating whether hot spot of icons should be position at the bottom center. 
        /// When set to false the hotspot will be at the center of icon.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public bool AlignPictureByBottom
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets type of frame to be displayed around point when ShapeDrawingOptions.PointShape is set to ptSymbolFontCharacter.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public tkLabelFrameType FrameType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a value indicating whether a frame will be drawn around point when ShapeDrawingOptions.PointShape is set to ptSymbolFontCharacter.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public bool FrameVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a value indicating whether the category will be displayed only
        /// for scale range specified by MinVisibleScale and MaxVisibleScale properties
        /// </summary>
        /// \new493 Added in version 4.9.3
        public bool DynamicVisibility
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets maximum map scale at which the category is visible 
        /// when ShapeDrawingOptions.DynamicVisibility is set to true.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public double MaxVisibleScale
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets minimum map scale at which the category is visible
        /// when ShapeDrawingOptions.DynamicVisibility is set to true.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public double MinVisibleScale
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets maximum map zoom at which the category is visible 
        /// when ShapeDrawingOptions.DynamicVisibility is set to true.
        /// </summary>
        /// \new52 Added in version 5.2
        public double MaxVisibleZoom
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets minimum map zoom at which the category is visible
        /// when ShapeDrawingOptions.DynamicVisibility is set to true.
        /// </summary>
        /// \new52 Added in version 5.2
        public double MinVisibleZoom
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
    }
#if nsp
}
#endif

