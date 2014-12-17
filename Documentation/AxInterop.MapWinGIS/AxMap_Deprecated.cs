using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if nsp
namespace AxMapWinGIS
{
#endif

    /// <summary>
    /// Holds members that once were a part of %AxMap class. 
    /// New API members to substitute deprecated ones are listed in description.
    /// </summary>
    class AxMap_Deprecated
    {
        /// <summary>
        /// Gets or sets line separation factor.
        /// </summary>
        /// \deprecated v4.8. Use LinePattern class, and ShapeDrawingOptions.LinePattern property instead.
        /// \removed493 Removed in v4.9.3
        public int LineSeparationFactor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets one user defined stipple row.
        /// </summary> 
        /// <remarks>The user defined fill stipple contains 32 rows of 32 bits. The stipple is created by
        /// setting patterns in the bits contained in each row.</remarks>
        /// <param name="LayerHandle">The handle of the layer for which the fill stipple is to be set. </param>
        /// <param name="StippleRow">The row in the custom stipple for which the pattern is to be set. </param>
        /// <returns>The custom fill stipple as an integer value where the stipple is defined 
        /// by arranging the 32 bits in desired pattern.</returns>
        /// \deprecated v.4.8. Use ShapeDrawingOptions.FillHatchStyle instead.
        /// \removed493 Removed in v4.9.3
        public int get_UDFillStipple(int LayerHandle, int StippleRow)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets one user defined stipple row. 
        /// </summary>
        /// <remarks>The user defined fill stipple contains 32 rows of 32 bits. 
        /// The stipple is created by setting patterns in the bits contained in each row.</remarks>
        /// <param name="LayerHandle">The handle of the layer for which the fill stipple is to be set. </param>
        /// <param name="StippleRow">The row in the custom stipple for which the pattern is to be set. </param>
        /// <param name="newValue">The custom fill stipple as an integer value where the stipple is defined 
        /// by arranging the 32 bits in desired pattern</param>
        /// \deprecated v.4.8. Use ShapeDrawingOptions.FillHatchStyle instead.
        /// \removed493 Removed in v4.9.3
        public void set_UDFillStipple(int LayerHandle, int StippleRow, int newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the user defined line stipple for the specified layer.
        /// </summary>
        /// <remarks>The user defined line stipple is represented by an Integer (VB.NET) or a Long (VB 6). The first digit represents the stipple 
        /// multiplier. Each of the following digits alternate between representing pixels drawn and pixels skipped in the pattern. The 
        /// number of pixels to be drawn or skipped are determined by multiplying the digit representing that segment of the stipple by 
        /// the stipple multiplier.\n
        /// This custom line stipple would draw six pixels(2*3), skip eight pixels(2*4), draw ten pixels(2*5), skip twelve pixels(2*6), with 
        /// the pattern repeating from the beginning after that. The largest valid line stipple is 2147483647. Any value greater than this 
        /// will result in overflow. A line stipple value less than 111 will result in a solid line.</remarks>
        /// <param name="LayerHandle">The handle of the layer for which the user defined shape line stipple is to be set.</param>
        /// <returns>The user defined line stipple.</returns>
        /// \deprecated v.4.8. Use LinePattern class instead.
        /// \removed493 Removed in v4.9.3
        public int get_UDLineStipple(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the user defined line stipple for the specified layer.
        /// </summary>
        /// <remarks>The user defined line stipple is represented by an Integer (VB.NET) or a Long (VB 6). The first digit represents the stipple 
        /// multiplier. Each of the following digits alternate between representing pixels drawn and pixels skipped in the pattern. The 
        /// number of pixels to be drawn or skipped are determined by multiplying the digit representing that segment of the stipple by 
        /// the stipple multiplier.\n
        /// This custom line stipple would draw six pixels(2*3), skip eight pixels(2*4), draw ten pixels(2*5), skip twelve pixels(2*6), with 
        /// the pattern repeating from the beginning after that. The largest valid line stipple is 2147483647. Any value greater than this 
        /// will result in overflow. A line stipple value less than 111 will result in a solid line. 
        /// </remarks>
        /// <param name="LayerHandle">The handle of the layer for which the user defined shape line stipple is to be set.</param>
        /// <param name="newValue">The user defined line stipple.</param>
        /// \deprecated v.4.8. Use LinePattern class instead.
        /// \removed493 Removed in v4.9.3
        public void set_UDLineStipple(int LayerHandle, int newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the font for point markers defined by font characters. 
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="FontName">The name of the font.</param>
        /// <param name="FontSize">The size of the font.</param>
        /// <param name="isBold">A boolean value which indicates whether the font is bold.</param>
        /// <param name="isItalic">A boolean value which indicates whether the font is italic.</param>
        /// <param name="isUnderline">A boolean value which indicates whether the font is underlined.</param>
        /// \deprecated v.4.8. Use ShapeDrawingOptions.PointCharacter and ShapefileCategories instead.
        /// \removed493 Removed in v4.9.3
        public void set_UDPointFontCharFont(int LayerHandle, string FontName, float FontSize, bool isBold, bool isItalic, bool isUnderline)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the font size for point markers defined by font characters. 
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="FontSize">The size of the font.</param>
        /// \deprecated v.4.8. Use ShapeDrawingOptions.PointCharacter and ShapefileCategories instead.
        /// \removed493 Removed in v4.9.3
        public void set_UDPointFontCharFontSize(int LayerHandle, float FontSize)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds a new item in the list of point markers defined by font characters
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="newValue">The ANSI code of the new character.</param>
        /// <param name="Color">The color of the marker.</param>
        /// <returns>The index of the new font character in the list of markers.</returns>
        /// \deprecated v.4.8. Use ShapeDrawingOptions.PointCharacter and ShapefileCategories instead.
        /// \removed493 Removed in v4.9.3
        public int set_UDPointFontCharListAdd(int LayerHandle, int newValue, uint Color)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Allows you to build an image list so that one point shapefile can have multiple icons. 
        /// </summary>
        /// <param name="LayerHandle">The layer handle of the layer for which the image list is defined.</param>
        /// <param name="newValue">The Integer index for the image to retrieve </param>
        /// <returns>The image object which is used as the point image for the specified layer.</returns>
        /// \deprecated v.4.8. Use ShapeDrawingOptions.Picture and ShapefileCategories instead.
        /// \removed493 Removed in v4.9.3
        public int set_UDPointImageListAdd(int LayerHandle, object newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of images currently stored in the image list for custom points images. 
        /// </summary>
        /// <param name="LayerHandle">The layer handle of the layer for which the image list is defined. </param>
        /// <returns>The Count of images currently stored in the image index of custom point types for that layer. 
        /// Remember to subtract 1 from this count when indexing the 0 based image list. 
        /// </returns>
        /// \deprecated v.4.8. Use ShapeDrawingOptions.Picture and ShapefileCategories instead.
        /// \removed493 Removed in v4.9.3
        public int get_UDPointImageListCount(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Allows you to build an image list so that one point shapefile can have multiple icons.
        /// </summary>
        /// <param name="LayerHandle">The layer handle of the layer for which the image list is defined.</param>
        /// <param name="ImageIndex">The Integer index for the image to retrieve</param>
        /// <returns>The image object which is used as the point image for the specified layer.</returns>
        /// \deprecated v.4.8. Use ShapeDrawingOptions.Picture and ShapefileCategories instead.
        /// \removed493 Removed in v4.9.3
        public object get_UDPointImageListItem(int LayerHandle, int ImageIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the image used when drawing points on the specified layer.
        /// </summary>
        /// <param name="LayerHandle">The layer handle of the layer for which the point type is required.</param>
        /// <returns>The image object which is used as the point image for the specified layer.</returns>
        /// \deprecated v.4.8. Use ShapeDrawingOptions.PointType instead.
        /// \removed493 Removed in v4.9.3
        public object get_UDPointType(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the image used when drawing points on the specified layer.
        /// </summary>
        /// <param name="LayerHandle">The layer handle of the layer for which the point type is required.</param>
        /// <param name="newValue">The image object which is used as the point image for the specified layer.</param>
        /// \deprecated v.4.8. Use ShapeDrawingOptions.PointType instead.
        /// \removed493 Removed in v4.9.3
        public void set_UDPointType(int LayerHandle, object newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets whether the specified shape is drawn with a fill. Only works on polygon shapefiles.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the fill is to be set.</param>
        /// <param name="Shape">Handle of the shape for which the fill is to be set.</param>
        /// <returns>Sets whether the shape is drawn with a fill or not.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillVisible property. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public bool get_ShapeDrawFill(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets whether the specified shape is drawn with a fill. Only works on polygon shapefiles.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the fill is to be set. </param>
        /// <param name="Shape">Handle of the shape for which the fill is to be set.</param>
        /// <param name="newValue">Sets whether the shape is drawn with a fill or not. </param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillVisible property. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeDrawFill(int LayerHandle, int Shape, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets whether the lines for the specified shape are drawn.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the lines are to be set.</param>
        /// <param name="Shape">Handle of the shape for which the lines are to be set.</param>
        /// <returns>Sets whether the shape is drawn with lines or not.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.LineVisible instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public bool get_ShapeDrawLine(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets whether the lines for the specified shape are drawn.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the lines are to be set.</param>
        /// <param name="Shape">Handle of the shape for which the lines are to be set. </param>
        /// <param name="newValue">Sets whether the shape is drawn with lines or not.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.LineVisible instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeDrawLine(int LayerHandle, int Shape, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets whether the points/vertices in specified shape are drawn.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the points are to be set.</param>
        /// <param name="Shape">Handle of the shape for which the points are to be set.</param>
        /// <returns>Sets whether the shape is drawn with points or not.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.Visible instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public bool get_ShapeDrawPoint(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets whether the points/vertices in specified shape are drawn. 
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the points are to be set.</param>
        /// <param name="Shape">Handle of the shape for which the points are to be set.</param>
        /// <param name="newValue">Sets whether the shape is drawn with points or not.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.Visible instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeDrawPoint(int LayerHandle, int Shape, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the fill color for the specified shape. Only works on polygon shapefiles.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the fill color is to be set.</param>
        /// <param name="Shape">Handle for the shape for which the fill color is to be set.</param>
        /// <returns>Fill color for the shape in the polygon shapefile. This is a System.UInt32 representation of an RGB color.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillColor instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public uint get_ShapeFillColor(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the fill color for the specified shape. Only works on polygon shapefiles
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the fill color is to be set.</param>
        /// <param name="Shape">Handle for the shape for which the fill color is to be set. </param>
        /// <param name="newValue">Fill color for the shape in the polygon shapefile. This is a System.UInt32 representation of an RGB color</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillColor instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeFillColor(int LayerHandle, int Shape, uint newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the fill stipple for the specified shape. Only works on polygon shapefiles
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the fill stipple is to be set.</param>
        /// <param name="Shape">Handle of the shape for which the fill stipple is required.</param>
        /// <returns>Gets the fill stipple for the specified shape.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillHatchStyle instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public MapWinGIS.tkFillStipple get_ShapeFillStipple(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the fill stipple for the specified shape. Only works on polygon shapefiles. 
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the fill stipple is to be set.</param>
        /// <param name="Shape">Handle of the shape for which the fill stipple is to be set.</param>
        /// <param name="newValue">Sets fill stipple for the specified shape.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillHatchStyle instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeFillStipple(int LayerHandle, int Shape, MapWinGIS.tkFillStipple newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the percentage of fill transparency for the specified layer. Only works on polygon shapefiles.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the fill transparency is to be set. </param>
        /// <param name="Shape">Handle of the shape for which the fill transparency is to be set.</param>
        /// <returns>Gets the percentage of fill transparency for the specified shape.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillTransparency instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public float get_ShapeFillTransparency(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the percentage of fill transparency for the specified layer. Only works on polygon shapefiles. 
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the fill transparency is to be set. </param>
        /// <param name="Shape">Handle of the shape for which the fill transparency is to be set. </param>
        /// <param name="newValue">Sets the percentage of fill transparency for the specified shape. </param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillTransparency instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeFillTransparency(int LayerHandle, int Shape, float newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the line color for the specified shape. Only works on shapefiles.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the line color is required.</param>
        /// <param name="Shape">Handle of the shape for which the line color is required.</param>
        /// <returns>Line color for the shape in the polygon shapefile.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.LineColor instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public uint get_ShapeLineColor(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the line color for the specified shape. Only works on shapefiles. 
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the line color is to be set.</param>
        /// <param name="Shape">Handle of the shape for which the line color is to be set.</param>
        /// <param name="newValue">Line color for the shape in the polygon shapefile. This is a System.UInt32 representation of an RGB color</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.LineColor instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeLineColor(int LayerHandle, int Shape, uint newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the line stipple for the specified shape. Only works on shapefiles.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the line stipple is required.</param>
        /// <param name="Shape">Handle of the shape for which the line stipple is required.</param>
        /// <returns>Line stipple for the shape in the shapefile. </returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.LineStipple instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public MapWinGIS.tkLineStipple get_ShapeLineStipple(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the line stipple for the specified shape. Only works on shapefiles.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the line stipple is to be set. </param>
        /// <param name="Shape">Handle of the shape for which the line stipple is to be set.</param>
        /// <param name="newValue">Line stipple for the shape in the shapefile.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.LineStipple instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeLineStipple(int LayerHandle, int Shape, MapWinGIS.tkLineStipple newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the line width for the specified shape. Only works on shapefiles.
        /// Suggested values for line width: 1 - 5
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer for which the line width is required. </param>
        /// <param name="Shape">Handle of the shape for which the line width is required.</param>
        /// <returns>Line width for the shape in the shapefile.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.LineWidth instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public float get_ShapeLineWidth(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the line width for the specified shape. Only works on shapefiles.
        /// Suggested values for line width: 1 - 5
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the line width is to be set.</param>
        /// <param name="Shape">Handle of the shape for which the line width is to be set</param>
        /// <param name="newValue">Line width for the shape in the shapefile.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.LineWidth instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeLineWidth(int LayerHandle, int Shape, float newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the point color for the specified shape. Only works on shapefiles. 
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the point color is required. </param>
        /// <param name="Shape">Handle of the shape for which the point color is required.</param>
        /// <returns>Point color for the shape in the polygon shapefile.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillColor for point shapefiles and ShapeDrawingOptions.VerticesColor  
        /// for polylines and polygons instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public uint get_ShapePointColor(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the point color for the specified shape. Only works on shapefiles.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the point color is to be set.</param>
        /// <param name="Shape">Handle of the shape for which the point color is to be set. </param>
        /// <param name="newValue">Point color for the shape in the polygon shapefile. This is a System.UInt32 representation of an RGB color</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillColor for point shapefiles and ShapeDrawingOptions.VerticesColor  
        /// for polylines and polygons instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapePointColor(int LayerHandle, int Shape, uint newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the index of the point marker represented by font character.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="Shape">The index of the shape.</param>
        /// <returns>The index of the marker associated with shape.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.PointCharacter instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public int get_ShapePointFontCharListID(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the index of the point marker represented by font character.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="Shape">The index of the shape.</param>
        /// <param name="newValue">The index of the marker associated with shape.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.PointCharacter instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapePointFontCharListID(int LayerHandle, int Shape, int newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Allows you specify an image from the image list so that one point shapefile can have multiple icons.
        /// </summary>
        /// <param name="LayerHandle">The layer handle of the layer for which the image list is defined. </param>
        /// <param name="Shape">The specific shape in the layer that you wish to determine the image index for</param>
        /// <returns>The index in the image list of user defined images.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.Picture instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public int get_ShapePointImageListID(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Allows you specify an image from the image list so that one point shapefile can have multiple icons
        /// </summary>
        /// <param name="LayerHandle">The layer handle of the layer containing the list to specify an image from.</param>
        /// <param name="Shape">The specific shape to be assigned a value in the layer. </param>
        /// <param name="newValue">The index of the image in the imagelist that you want to assign to the specified shape</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.Picture instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapePointImageListID(int LayerHandle, int Shape, int newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the line point/vertex size for the specified shape. Only works on shapefiles. 
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the point/vertex size is required. </param>
        /// <param name="Shape">Handle of the shape for which the point/vertex size is required</param>
        /// <returns>Point/vertex size for the shape in the shapefile.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.PointSize instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public float get_ShapePointSize(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the line point/vertex size for the specified shape. Only works on shapefiles.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the point/vertex size is to be set.</param>
        /// <param name="Shape">Handle of the shape for which the point/vertex size is to be set.</param>
        /// <param name="newValue">Point/vertex size for the shape in the shapefile.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.PointSize instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapePointSize(int LayerHandle, int Shape, float newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the line point type for the specified shape. Only works on shapefiles.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the point type is to be set. </param>
        /// <param name="Shape">Handle of the shape for which the point type is to be set.</param>
        /// <param name="newValue">Point type for the shape in the shapefile.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.PointType instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapePointType(int LayerHandle, int Shape, MapWinGIS.tkPointType newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the line point type for the specified shape. Only works on shapefiles. 
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the point type is required. </param>
        /// <param name="Shape">Handle of the shape for which the point type is required. </param>
        /// <returns>Point type for the shape in the shapefile.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.PointType instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public MapWinGIS.tkPointType get_ShapePointType(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the color of the polygon fill stipple for the given shape of the layer.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="Shape">The index of the shape.</param>
        /// <returns>The color of stipple.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillHatchStyle instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public uint get_ShapeStippleColor(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the color of the polygon fill stipple for the given shape of the layer.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="Shape">The index of the shape.</param>
        /// <param name="newValue">The color of stipple.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillHatchStyle instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeStippleColor(int LayerHandle, int Shape, uint newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets a boolean value which indicates whether the background of fill stipple for given shape of will be transparent.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="Shape">The index of the shape.</param>
        /// <returns>True in case the background is transparent and false otherwise.</returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillBgTransparent instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public bool get_ShapeStippleTransparent(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets a boolean value which indicates whether the background of fill stipple for given shape of will be transparent.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="Shape">The index of the shape.</param>
        /// <param name="newValue">True in case the background is transparent and false otherwise.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.FillBgTransparent instead. See ShapefileCategories, Shapefile.set_ShapeCategory to setup 
        /// visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeStippleTransparent(int LayerHandle, int Shape, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Gets the visibility of the specified shape.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the visibility is required.</param>
        /// <param name="Shape">Handle of the shape for which the visibility is required.</param>
        /// <returns>Boolean value representing whether the the shape is visible or not. </returns>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.Visible and Shapefile.VisibilityExpression instead. 
        /// See ShapefileCategories, Shapefile.set_ShapeCategory to setup visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public bool get_ShapeVisible(int LayerHandle, int Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Sets the visibility of the specified shape. 
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer containing the shape for which the visibility is to be set</param>
        /// <param name="Shape">Handle of the shape for which the visibility is to be set.</param>
        /// <param name="newValue">Boolean value representing whether the shape is to be visible or not.</param>
        /// \deprecated v. 4.8. Use ShapeDrawingOptions.Visible and Shapefile.VisibilityExpression instead. 
        /// See ShapefileCategories, Shapefile.set_ShapeCategory to setup visualization of individual shapes.
        /// \removed493 Removed in v4.9.3
        public void set_ShapeVisible(int LayerHandle, int Shape, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Applies the coloring scheme to a layer (the layer handle is specified in the coloring scheme object).
        /// </summary>
        /// <remarks>Deprecated for shapefile layers.</remarks>
        /// <param name="Legend">The coloring scheme to apply.</param>
        /// <returns>Boolean value indicating the successful application when true, unsuccessful application when false.</returns>
        /// \deprecated v4.9.3 No longer used.
        /// \removed493 Removed in v4.9.3.
        public bool ApplyLegendColors(object Legend)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a boolean value which indicates whether the specified file holds tiff grid.
        /// </summary>
        /// <param name="Filename">The name of the file.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \deprecated v4.9.3 Use Utils.IsTiffGrid instead.
        /// \removed493 Removed in v4.9.3.
        public bool IsTIFFGrid(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the drawing method for vector layers.
        /// </summary>
        /// <remarks>The default value is vdmNewSymbology. From the version 4.8 this is the only value supported.</remarks>
        /// \new48 Added in version 4.8
        /// \deprecated v4.8 No longer used.
        /// \removed493 Removed in v4.9.3
        public tkShapeDrawingMethod ShapeDrawingMethod
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Sets a coloring scheme to be associated with an image layer. 
        /// </summary>
        /// <remarks>The coloring scheme is for reference purposes only and has no effect the coloring or display of the image.</remarks>
        /// <param name="LayerHandle">The handle of the image layer to attach the coloring scheme to.</param>
        /// <param name="ColorScheme">The coloring scheme to attach to the specified image layer.</param>
        /// <returns>Boolean value representing success when true, failure when false.</returns>
        /// \deprecated v4.8 Use Image.CustomColorScheme instead.
        /// \removed493 Removed in v4.9.3
        public bool SetImageLayerColorScheme(int LayerHandle, object ColorScheme)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Updates the display of the specified image object.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// \deprecated v4.9.3 No longer used.
        /// \removed493 Removed in v4.9.3
        public void UpdateImage(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the filename of the Grid object associated with an Image object loaded into the map.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer for which the grid filename is to be set.</param>
        /// <returns>The grid filename to be associated with the specified layer.</returns>
        /// \deprecated v 4.9.1 Use AxMap.get_Image, Image.SourceGridName instead.
        /// \removed493 Removed in v4.9.3
        public string get_GridFileName(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the percentage of transparency of an Image layer.
        /// </summary>
        /// <param name="LayerHandle">The layer handle of the Image layer for which the transparency is to be set.</param>
        /// <returns>The percentage of transparency for the specified image layer.</returns>
        /// \deprecated v4.9.3 Use AxMap.get_Image, Image.TransparencyPercent instead.
        /// \removed493 Removed in v4.9.3
        public float get_ImageLayerPercentTransparent(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the filename of the grid associated with the specified layer.
        /// </summary>
        /// <remarks>MapWinGIS will create a bitmap representation for the grid and use instance of Image class for display.</remarks>
        /// <param name="LayerHandle">The layer handle.</param>
        /// <param name="newValue">The name of the file with grid.</param>
        /// \deprecated v 4.9.1 Use Image.SourceGridName instead.
        /// \removed493 Removed in v4.9.3
        public void set_GridFileName(int LayerHandle, string newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds a label to the map.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer where the label will be added to.</param>
        /// <param name="Text">The text to be used for the label.</param>
        /// <param name="Color">The color of the added label. This is a UInt32 representation of an RGB color.</param>
        /// <param name="x">The x coordinate in projected map units which determines where the label will be added on the map.</param>
        /// <param name="y">The y coordinate in projected map units which determines where the label will be added on the map.</param>
        /// <param name="hJustification">Specifies whether to justify the label's text right, left, or center. </param>
        /// \deprecated v.4.8 The method will ignore Color and hJustification parameters. Use Labels interface ( AxMap.get_LayerLabels ) to change these settings.
        /// \removed493 Removed in v4.9.3
        public void AddLabel(int LayerHandle, string Text, uint Color, double x, double y, MapWinGIS.tkHJustification hJustification)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds an extended label to the map, allowing for rotated labels.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer where the label will be added to.</param>
        /// <param name="Text">The text to be used for the label.</param>
        /// <param name="Color">The color of the added label. This is a UInt32 representation of an RGB color.</param>
        /// <param name="x">The x coordinate in projected map units which determines where the label will be added on the map.</param>
        /// <param name="y">The y coordinate in projected map units which determines where the label will be added on the map.</param>
        /// <param name="hJustification">Specifies whether to justify the label's text right, left, or center.</param>
        /// <param name="Rotation">The number of degrees to rotate the label. Positive angles rotate the text counter-clockwise, and
        /// negative angles rotate the text clockwise</param>
        /// \deprecated v.4.8 The method will ignore Color and hJustification parameters. Use Labels interface ( AxMap.get_LayerLabels ) to change these settings.
        /// \removed493 Removed in v4.9.3
        public void AddLabelEx(int LayerHandle, string Text, uint Color, double x, double y, MapWinGIS.tkHJustification hJustification, double Rotation)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all labels that have been added to the specified layer.
        /// </summary>
        /// <param name="LayerHandle">Handle of the layer.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.Clear instead.
        /// \removed493 Removed in v4.9.3
        public void ClearLabels(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the offset for the labels on the layer. 
        /// </summary>
        /// <remarks>The offset is the distance in pixels from the label point to the text.</remarks>
        /// <param name="LayerHandle">The handle of the layer for which the offset is to be set.</param>
        /// <returns>The offset for the labels on the specified layer.</returns>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.OffsetX, Labels.OffsetY instead.
        /// \removed493 Removed in v4.9.3
        public int get_LayerLabelsOffset(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the offset for the labels on the layer. 
        /// </summary>
        /// <remarks>The offset is the distance in pixels from the label point to the text.</remarks>
        /// <param name="LayerHandle">The handle of the layer for which the offset is to be set. </param>
        /// <param name="newValue">The offset for the labels on the specified layer.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.OffsetX, Labels.OffsetY instead.
        /// \removed493 Removed in v4.9.3
        public void set_LayerLabelsOffset(int LayerHandle, int newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets whether to scale the labels on the layer.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer for which the scaling of labels is to be set.</param>
        /// <returns>A boolean value representing whether or not to scale the labels on the specified layer.</returns>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.ScaleLabels instead.
        /// \removed493 Removed in v4.9.3
        public bool get_LayerLabelsScale(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets whether to scale the labels on the layer
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer for which the scaling of labels is to be set.</param>
        /// <param name="newValue">A boolean value representing whether or not to scale the labels on the specified layer.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.ScaleLabels instead.
        /// \removed493 Removed in v4.9.3
        public void set_LayerLabelsScale(int LayerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets whether to use shadows for the labels on the layer.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer for which the use of label shadows is to be set.</param>
        /// <returns>A boolean value representing whether or not label shadows will be used for the specified layer. </returns>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.HaloVisible instead.
        /// \removed493 Removed in v4.9.3
        public bool get_LayerLabelsShadow(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets whether to use shadows for the labels on the layer.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer for which the use of label shadows is to be set. </param>
        /// <param name="newValue">A boolean value representing whether or not label shadows will be used for the specified layer.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.HaloVisible instead.
        /// \removed493 Removed in v4.9.3
        public void set_LayerLabelsShadow(int LayerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the shadow color for the labels on the layer
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer for which the shadow color is to be set.</param>
        /// <returns>The color to use the shadow color of the specified layer. 
        /// This is a System.UInt32 representation of an RGB color.</returns>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.HaloColor instead.
        /// \removed493 Removed in v4.9.3
        public uint get_LayerLabelsShadowColor(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the shadow color for the labels on the layer. 
        /// </summary>
        /// <param name="LayerHandle">The layer handle of the layer for which the shadow color is to be set.</param>
        /// <param name="newValue">The color of use as the shadow color of the specified layer. 
        /// This is a System.UInt32 representation of an RGB color.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.HaloColor instead.
        /// \removed493 Removed in v4.9.3
        public void set_LayerLabelsShadowColor(int LayerHandle, uint newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets label visibility for the specified layer.
        /// </summary>
        /// <param name="LayerHandle">The handle for the layer for which its label's visibility are to be set.</param>
        /// <returns>A boolean value which determines whether the specified layer's labels are visible or not.</returns>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.Visible instead.
        /// \removed493 Removed in v4.9.3
        public bool get_LayerLabelsVisible(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets label visibility for the specified layer.
        /// </summary>
        /// <param name="LayerHandle">The layer handle for the layer for which its label's visibility are to be set.</param>
        /// <param name="newValue">A boolean value which determines whether the specified layer's labels are visible or not. </param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.Visible instead.
        /// \removed493 Removed in v4.9.3
        public void set_LayerLabelsVisible(int LayerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the standard view width used to scale the labels on the layer
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer for which the standard view width is required.</param>
        /// <param name="Width">Reference parameter. The standard view width for the specified layer is returned through this parameter.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.BasicScale instead.
        /// \removed493 Removed in v4.9.3
        public void GetLayerStandardViewWidth(int LayerHandle, ref double Width)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the standard view width used to scale the labels on the layer.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer for which the standard view width is to be set.</param>
        /// <param name="Width">The new standard view width for the specified layer.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.BasicScale instead.
        /// \removed493 Removed in v4.9.3
        public void SetLayerStandardViewWidth(int LayerHandle, double Width)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a boolean value which indicates whether overlapping labels will be allowed for the layer.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <returns>True in case overlapping labels will be prevented and false otherwise.</returns>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.AvoidCollisions instead.
        /// \removed493 Removed in v4.9.3
        public bool get_UseLabelCollision(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a boolean value which indicates whether overlapping labels will be allowed for the layer.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="newValue">True in case overlapping labels will be forbidden and false otherwise.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.AvoidCollisions instead.
        /// \removed493 Removed in v4.9.3
        public void set_UseLabelCollision(int LayerHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the color of the font for labels of the layer.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="LabelFontColor">The color of the labels font.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.FontColor instead.
        /// \removed493 Removed in v4.9.3
        public void LabelColor(int LayerHandle, uint LabelFontColor)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the font to use when drawing labels for a layer.
        /// </summary>
        /// <param name="LayerHandle">Layer handle of the layer for which the label fonts are to be set.</param>
        /// <param name="FontName">Name of the font to use for the labels. (Ex: "Arial") </param>
        /// <param name="FontSize">Size of the font to use for the labels.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.FontName, Labels.FontSize instead.
        /// \removed493 Removed in v4.9.3
        public void LayerFont(int LayerHandle, string FontName, int FontSize)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the font for labels of the layer.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <param name="FontName">The name of the font.</param>
        /// <param name="FontSize">The size of the font.</param>
        /// <param name="isBold">A boolean value which indicates whether the font is bold.</param>
        /// <param name="isItalic">A boolean value which indicates whether the font is italic.</param>
        /// <param name="isUnderline">A boolean value which indicates whether the font is underlined.</param>
        /// \deprecated v4.9.3 Use AxMap.get_LayerLabels, Labels.FontName, Labels.FontSize, Labels.FontBold, Labels.FontItalic, Labels.FontUnderline instead.
        /// \removed493 Removed in v4.9.3
        public void LayerFontEx(int LayerHandle, string FontName, int FontSize, bool isBold, bool isItalic, bool isUnderline)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether multiline labels will be drawn correctly on the map.
        /// </summary>
        /// \deprecated v 4.8. Multiline labels will be drawn after specification of appropriate label expression. 
        /// \see Labels.Expression.
        /// \removed493 Removed in v4.9.3
        public bool MultilineLabels
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Adds label to the specified drawing layer.
        /// </summary>
        /// <remarks>This method corresponds to the Labels.AddLabel method.</remarks>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="Text">The text of the label.</param>
        /// <param name="Color">The color of the label.</param>
        /// <param name="x">The x coordinate of the label.</param>
        /// <param name="y">The y coordinate of the label.</param>
        /// <param name="hJustification">The justification of the label.</param>
        /// \deprecated v4.9.3 Use AxMap.DrawLabel, AxMap.DrawLabelEx instead.
        /// \removed493 Removed in v4.9.3
        public void AddDrawingLabel(int DrawHandle, string Text, uint Color, double x, double y, tkHJustification hJustification)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds label to the specified drawing layer. Angle of rotation is accepted.
        /// </summary>
        /// <remarks>This method corresponds to the Labels.AddLabel method.</remarks>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="Text">The text of the label.</param>
        /// <param name="Color">The color of the label.</param>
        /// <param name="x">The x coordinate of the label.</param>
        /// <param name="y">The y coordinate of the label.</param>
        /// <param name="hJustification">The justification of the label.</param>
        /// <param name="Rotation">The angle of rotation in degrees.</param>
        /// \deprecated v4.9.3 Use AxMap.DrawLabel, AxMap.DrawLabelEx instead.
        /// \removed493 Removed in v4.9.3
        public void AddDrawingLabelEx(int DrawHandle, string Text, uint Color, double x, double y, tkHJustification hJustification, double Rotation)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value which affects the scaling of labels.
        /// </summary>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="Width">The standard width of the view in pixels to use normal size of labels.</param>
        /// \deprecated v 4.8. Use AxMap.get_DrawingLabels, Labels.BasicScale instead.
        /// \removed493 Removed in v4.9.3
        public void GetDrawingStandardViewWidth(int DrawHandle, ref double Width)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the vertical offset of the labels for the specified drawing layer in pixels.
        /// </summary>
        /// <param name="DrawHandle">The index of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <returns>The offset in pixels.</returns>
        /// \deprecated v. 4.8. Use AxMap.get_DrawingLabels and Labels.OffsetX, Labels.OffsetY instead.
        /// \removed493 Removed in v4.9.3
        public int get_DrawingLabelsOffset(int DrawHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the vertical offset of the labels for the specified drawing layer in pixels.
        /// </summary>
        /// <param name="DrawHandle">The index of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="newValue">The offset in pixels.</param>
        /// \deprecated v. 4.8. Use AxMap.get_DrawingLabels and Labels.OffsetX, Labels.OffsetY instead.
        /// \removed493 Removed in v4.9.3
        public void set_DrawingLabelsOffset(int DrawHandle, int newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a boolean value which indicates whether the labels of the drawing layer will be scaled.
        /// </summary>
        /// <remarks>This property corresponds to the Labels.ScaleLabels property.</remarks>
        /// <param name="DrawHandle">he index of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <returns>True if the labels are scaled and false otherwise.</returns>
        /// \deprecated v 4.9.3 Use AxMap.get_DrawingLabels, Labels.ScaleLabels instead.
        /// \removed493 Removed in v4.9.3
        public bool get_DrawingLabelsScale(int DrawHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a boolean value which indicates whether the labels of the drawing layer will be scaled.
        /// </summary>
        /// <remarks>This property corresponds to the Labels.ScaleLabels property.</remarks>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="newValue">True if the labels are scaled and false otherwise.</param>
        /// \deprecated v 4.9.3 Use AxMap.get_DrawingLabels, Labels.ScaleLabels instead.
        /// \removed493 Removed in v4.9.3
        public void set_DrawingLabelsScale(int DrawHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a boolean value which indicates whether a shadow will be displayed for the labels of the drawing layer.
        /// </summary>
        /// <remarks>This property corresponds to the Labels.HaloVisible property.</remarks>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <returns>True if the shadow is displayed and false otherwise.</returns>
        /// \deprecated v 4.9.3 Use AxMap.get_DrawingLabels, Labels.HaloVisible instead.
        /// \removed493 Removed in v4.9.3
        public bool get_DrawingLabelsShadow(int DrawHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a boolean value which indicates whether a shadow will be displayed for the labels of the drawing layer.
        /// </summary>
        /// <remarks>This property corresponds to the Labels.HaloVisible property.</remarks>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="newValue">True if the shadow is displayed and false otherwise.</param>
        /// \deprecated v 4.9.3 Use AxMap.get_DrawingLabels, Labels.HaloVisible instead.
        /// \removed493 Removed in v4.9.3
        public void set_DrawingLabelsShadow(int DrawHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the color of the shadow for the labels of the specified drawing layer.
        /// </summary>
        /// <remarks>This property corresponds to the Labels.HaloColor property.</remarks>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <returns>The color of shadow.</returns>
        /// \deprecated v 4.9.3 Use AxMap.get_DrawingLabels, Labels.HaloColor instead.
        /// \removed493 Removed in v4.9.3
        public uint get_DrawingLabelsShadowColor(int DrawHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the color of the shadow for the labels of the specified drawing layer.
        /// </summary>
        /// <remarks>This property corresponds to the Labels.HaloColor property.</remarks>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="newValue">The color of shadow.</param>
        /// \deprecated v 4.9.3 Use AxMap.get_DrawingLabels, Labels.HaloColor instead.
        /// \removed493 Removed in v4.9.3
        public void set_DrawingLabelsShadowColor(int DrawHandle, uint newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a boolean value which indicates whether overlapping labels will be allowed for drawing layer.
        /// </summary>
        /// <remarks>This property corresponds to the Labels.AvoidCollisions property.</remarks>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <returns>True in case overlapping labels are removed and false otherwise.</returns>
        /// \deprecated v 4.9.3 Use AxMap.get_DrawingLabels, Labels.AvoidCollisions instead.
        /// \removed493 Removed in v4.9.3
        public bool get_UseDrawingLabelCollision(int DrawHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a boolean value which indicates whether overlapping labels will be allowed for drawing layer.
        /// </summary>
        /// <remarks>This property corresponds to the Labels.AllowCollisions property.</remarks>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="newValue">True in case overlapping labels are removed and false otherwise.</param>
        /// \deprecated v 4.9.3 Use AxMap.get_DrawingLabels, Labels.AvoidCollisions instead.
        /// \removed493 Removed in v4.9.3
        public void set_UseDrawingLabelCollision(int DrawHandle, bool newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the font for the labels of the drawing layer.
        /// </summary>
        /// <param name="DrawHandle">The handle of the drawing layer returned by AxMap.NewDrawing method.</param>
        /// <param name="FontName">The name of the font.</param>
        /// <param name="FontSize">The size of the font.</param>
        /// \deprecated v 4.9.3 Use AxMap.get_DrawingLabels, Labels.FontName, Labels.FontSize instead.
        /// \removed493 Removed in v4.9.3
        public void DrawingFont(int DrawHandle, string FontName, int FontSize)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the angle of map rotation in degrees.
        /// </summary>
        /// \deprecated v.4.9.3 The functionality is no longer supported.
        /// \removed493 Removed in v4.9.3
        public double MapRotationAngle
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Sets the numeric value which determines the transparency of the given image layer.
        /// </summary>
        /// <remarks>It is recommended to the properties of the Image class directly.</remarks>
        /// <param name="LayerHandle">The handle of the layer with the image.</param>
        /// <param name="newValue">The value ranging from 0.0(transparent) to 1.0(opaque).</param>
        /// \deprecated v4.9.3 Use AxMap.get_Image(), Image.TransparencyPercent instead.
        /// \removed493 Removed in v4.9.3
        public void set_ImageLayerPercentTransparent(int LayerHandle, float newValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deprecated. Affects the scaling of the labels of the specified drawing layer.
        /// </summary>
        /// <param name="DrawHandle">The handle of the drawing layer.</param>
        /// <param name="Width">The width in pixels.</param>
        /// \deprecated v.4.8. Use AxMap.get_DrawingLabels, Labels.BasicScale and Labels.ScaleLabels instead.
        /// \removed493 Removed in v4.9.3
        public void SetDrawingStandardViewWidth(int DrawHandle, double Width)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears the images from the user defined list of point symbols.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// \deprecated v4.9.3 Use ShapeDrawingOptions.Picture instead.
        /// \removed493 Removed in v4.9.3
        public void ClearUDPointImageList(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or set the point which is used as a center of the map rotation/
        /// </summary>
        /// <param name="rotX">The x coordinate of the rotation point in map units.</param>
        /// <param name="rotY">The y coordinate of the rotation point in map units.</param>
        /// <returns>The base point for rotation.</returns>
        /// \new48 Added in version 4.8
        /// \deprecated v.4.9.3 The functionality is no longer supported.
        /// \removed493 Removed in v4.9.3
        public MapWinGIS.Point GetBaseProjectionPoint(double rotX, double rotY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the bounding box which holds the extents of the map when rotation is applied.
        /// </summary>
        /// <remarks>The bounding box is equal to the map extents when rotation angle is 0 and larger than 
        /// map extents if the angle differs from it.</remarks>
        /// <returns>The bounding box of the rotated extents.</returns>
        /// \new48 Added in version 4.8
        /// \deprecated v.4.9.3 The functionality is no longer supported.
        /// \removed493 Removed in v4.9.3
        public MapWinGIS.Extents GetRotatedExtent()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tests the identity of 2 projections.
        /// </summary>
        /// <param name="proj4_a">The first projection string.</param>
        /// <param name="proj4_b">The second projection string.</param>
        /// <returns></returns>
        /// \deprecated v4.9.3 Use GeoProjection.get_IsSame, GeoProjection.get_IsSameExt instead.
        /// \removed493 Removed in v4.9.3
        public bool IsSameProjection(string proj4_a, string proj4_b)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates how images with identical size and position on the map will be drawn.
        /// </summary>
        /// <remarks>When set to true such images will be merged into a single bitmap. This may improve performance 
        /// in case of low number of data pixels (those which differ from the transparent color). The default value is false.</remarks>
        /// \new48 Added in version 4.8
        /// \deprecated v.4.9.3 The functionality is no longer supported.
        /// \removed493 Removed in v4.9.3
        public bool CanUseImageGrouping
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets whether or not the map uses double buffering. 
        /// </summary>
        /// <remarks>Double buffering uses a little more memory, but allows the view to be smoother with less flickering.
        /// The default value is true. It's not recommended to change it.</remarks>
        /// \deprecated v.4.9.3 Double buffer is used in all cases.
        /// \removed493 Removed in v4.9.3
        public bool DoubleBuffer
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the MapState string which stores all information needed to restore a view. 
        /// </summary>
        /// <remarks>This includes layer information and coloring schemes.</remarks>
        /// \deprecated v.4.8 Use AxMap.SerializeMapState instead.
        /// \removed493 Removed in v4.9.3
        public string MapState
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Serial number functionality is deprecated as MapWindow is now Open Source. This property may be left unset. 
        /// </summary>
        /// \deprecated v4.8 This property is no longer used.
        /// \removed493 Removed in v4.9.3
        public string SerialNumber
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Deprecated. Updates the extents of the layer before the drawing.
        /// </summary>
        /// <param name="LayerHandle">The handle of the layer.</param>
        /// <returns>True on success and false in case of invalid layer handle.</returns>
        /// \deprecated v.4.8. The call is incorporated into drawing routine.
        /// \removed493 Removed in v4.9.3
        public bool AdjustLayerExtents(int LayerHandle)
        {
            throw new NotImplementedException();
        }


    }
#if nsp
}
#endif
