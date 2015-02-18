using System;
using MapWinGIS;


#if nsp
namespace MapWinGIS
{
#endif

    /// <summary>
    /// Represents an raster image of particular format which may be added to the map.
    /// </summary>
    /// \dot digraph image_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 1, fontname=Helvetica, fontsize=9, color = tan, style = filled, height = 0.3, width = 1.0];
    /// lbs [ label="Labels" URL="\ref Labels"];
    /// 
    /// node [style = dashed, color = gray];
    /// map [ label="AxMap" URL="\ref AxMap"];
    /// 
    /// node [color = gray, peripheries = 3, style = filled, height = 0.2, width = 0.8]
    /// img [ label="Image" URL="\ref Image"];
    /// 
    /// edge [dir = "none", dir = "none", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// img -> lbs [ URL="\ref Image.Labels()", tooltip = "Image.Labels()", headlabel = "   1"];
    /// map -> img [URL="\ref AxMap.get_Image()", tooltip = "AxMap.get_Image()", headlabel = "   n"]
    /// }
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>\n\n
    /// Here is a diagram with groups of API members for Image class.
    /// \dotfile imagegroups.dot
    /// <a href = "diagrams.html">Graph description</a>\n\n
    /// <remarks>
    /// <b>Types of image handling.</b>\n\n
    /// The class is a wrapper for several representation of images used internally:\n\n
    /// 1. In-RAM images created with Image.CreateNew method and not bound to any file;
    /// - data for each pixel is stored in the memory as R, G, B bytes; no semitransparency is stored;
    /// - can be edited and saved in several formats with Image.Save() method.
    /// 
    /// 2. Disk-based images handled by MapWinGIS own driver;
    /// - available for BMP format only;
    /// - support editing with immediate saving to the source file.
    /// 
    /// 3. GDAL-based images:\n
    /// - data is accessed by GDAL drivers;
    /// - suitable for rendering very large images especially when overviews are preliminary created (Image.BuildOverviews);
    /// - before each redraw the requested part of image is read into memory buffer;
    /// - doesn't support editing of data (values in memory buffer can be changed with Image.set_Value but these changes can't be saved to disk).
    /// 
    /// Inner representation of image can be determined via Image.SourceType property. 
    /// Some properties and methods are applicable to the GDAL images only.\n
    /// </remarks>
    #if nsp
        #if upd
            public class Image : MapWinGIS.IImage
        #else        
            public class IImage
        #endif
    #else
        public class Image
    #endif
    {
        /// The modules listed here are parts of the documentation of Image class.
        /// \addtogroup Image_modules Image modules
        /// \dotfile imagegroups.dot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        #region GDAL images

        /// \addtogroup image_gdal GDAL images
        /// Here is a list of methods and properties that are specific for images accessed through GDAL library.
        /// The properties and methods described here belong to Image class.
        /// \dot
        /// digraph image_common {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Image" URL="\ref Image"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="GDAL images"   URL="\ref image_common"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Gets or sets the size of image buffer in percentage.
        /// </summary>
        /// <remarks>The values less than 100 will increase performance of drawing at the cost of decreasing the visual quality.</remarks>
        public int BufferSize
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Converts the position of pixel in image buffer to the map coordinates.
        /// </summary>
        /// <param name="BufferX">The x coordinate of the pixel.</param>
        /// <param name="BufferY">The y coordinate of the pixel.</param>
        /// <param name="projX">The resulting x coordinate of the map.</param>
        /// <param name="projY">The resulting y coordinate of the map.</param>
        /// \new48 Added in version 4.8
        public void BufferToProjection(int BufferX, int BufferY, out double projX, out double projY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Builds overviews for GDAL image.
        /// </summary>
        /// <remarks>Overviews are the images with lower resolution which are used as a replacement for the
        /// original image while drawing it at small scales. The usage of overviews can significantly 
        /// improve performance and is highly recommended for large images.</remarks>
        /// <param name="ResamplingMethod">The resampling method to use for generation of the image with lower resolution.</param>
        /// <param name="NumOverviews">The number of overviews. Should be equal to the size of OverviewList.</param>
        /// <param name="OverviewList">An array of integer type with the specification of overviews (magnification ratios).</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool BuildOverviews(tkGDALResamplingMethod ResamplingMethod, int NumOverviews, Array OverviewList)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears the cache of GDAL library which can be used for faster display.
        /// </summary>
        /// \new48 Added in version 4.8
        public bool ClearGDALCache
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Loads the whole image in the memory buffer of specified size. Applicable for GDAL images only.
        /// </summary>
        /// <remarks>If the buffer size is smaller than image disk size, the image will be resampled, by decreasing
        /// it's linear dimensions by 2.</remarks>
        /// <param name="maxBufferSize">The maximum size of buffer in MB.</param>
        /// <returns>True on successful loading and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool LoadBuffer(double maxBufferSize)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the number of bands of GDAL image.
        /// </summary>
        public int NoBands
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the number of overviews for GDAL image.
        /// </summary>
        /// \see Image.BuildOverviews.
        /// \new48 Added in version 4.8
        public int NumOverviews
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the type of palette interpretation. Applicable for GDAL images only.
        /// </summary>
        /// <remarks>The possible values are 
        /// - "Grayscale";
        /// - "Red, Green, Blue and Alpha";
        /// - "Cyan, Magenta, Yellow and Black";
        /// - "Hue, Lightness and Saturation"; 
        /// - "Unknown".</remarks>
        public string PaletteInterpretation
        {
            get { throw new NotImplementedException(); }
        }

       

        /// <summary>
        /// Gets or sets a boolean value which indicates whether a histogram will be used. Applicable for GDAL images only.
        /// </summary>
        public bool UseHistogram
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Sets no-data value for the image. Applicable for GDAL images only.
        /// </summary>
        /// <remarks>The pixels with this value will be treated as transparent.</remarks>
        /// <param name="Value">The value to be set.</param>
        /// <param name="Result">A boolean output value which indicates the success of the operation.</param>
        /// \new48 Added in version 4.8
        public void SetNoDataValue(double Value, ref bool Result)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a boolean value which indicates whether an image has a color table. Applicable for GDAL images only.
        /// </summary>
        public bool HasColorTable
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Reloads the image buffer to represent the specified map extents. Applicable for GDAL images only.
        /// </summary>
        /// <remarks>In most cases the method must not be called directly, as MapWinGIS reloads the buffers for the images
        /// automatically after zooming operations.</remarks>
        /// <param name="newMinX">The minimal x coordinate of the new extents.</param>
        /// <param name="newMinY">The minimal y coordinate of the new extents.</param>
        /// <param name="newMaxX">The maximum x coordinate of the new extents.</param>
        /// <param name="newMaxY">The maximum y coordinate of the new extents.</param>
        /// <param name="newPixelsInView">The number of image pixels which fall into the specified extents.</param>
        /// <param name="transPercent">This parameter is no longer used.</param>
        public void SetVisibleExtents(double newMinX, double newMinY, double newMaxX, double newMaxY, int newPixelsInView, float transPercent)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns true if the datasource represents RGB image.
        /// </summary>
        /// <remarks>The property can be used to distinguish grid datasource from genuine images. 
        /// Implementation checks if all 3 R,G,B colors are set as color interpretation for GDAL bands. 
        /// Any additional bands will be ignored. </remarks>
        /// \new491 Added in version 4.9.1
        public bool IsRgb
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns true if the image is warped, i.e. there are rotation parameters in world file.
        /// </summary>
        /// \new490 Added in version 4.9.0
        public bool Warped
        {
            get { throw new NotImplementedException(); }
        }

        /// @}
        
        #endregion

        #region Management
        /// \addtogroup image_common Image common
        /// Here is a list of methods and properties that are common for all images despite their source.
        /// The properties and methods described here belong to Image class.
        /// \dot
        /// digraph image_management {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Image" URL="\ref Image"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Open and close"   URL="\ref image_common"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Returns the filename of the datasource for an image.
        /// </summary>
        /// <remarks>For proxy image grid filename will be renamed, in other cases Filename of Image itself.</remarks>
        /// \new491 Added in version 4.9.1
        public string SourceFilename
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the common dialog filter containing all supported file extensions in string format.
        /// </summary>
        public string CdlgFilter
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Clears the image using the specified canvas color as the new color for every pixel in the image. 
        /// </summary>
        /// <param name="CanvasColor">The color to be used to as the new color of every pixel in the image. This is a UInt32 
        /// representation of an RGB value.</param>
        /// <param name="cBack">Optional. The ICallback object that will receive the progress and error events while the image is cleared.</param>
        /// <returns>A boolean value representing the success or failure of clearing the image.</returns>
        public bool Clear(uint CanvasColor, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Closes the image.
        /// </summary>
        /// <returns>True on successful release of the resources and false otherwise.
        /// This value can be omitted.</returns>
        public bool Close()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new image with specified width and height.
        /// </summary>
        /// <param name="NewWidth">The width of the new image in pixels.</param>
        /// <param name="NewHeight">The height of the new image in pixels.</param>
        /// <returns>A boolean value representing the success or failure of creating the new image. </returns>
        public bool CreateNew(int NewWidth, int NewHeight)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restores the properties of the image object from the serialized string.
        /// </summary>
        /// <param name="newVal">The serialized string generated by Image.Serialize() method.</param>
        /// \new48 Added in version 4.8
        public void Deserialize(string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The filename associated with the object.
        /// </summary>
        public string Filename
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Opens an image from file. 
        /// </summary>
        /// <param name="ImageFileName">The filename of the image to be opened.</param>
        /// <param name="fileType">Optional. The type of image being opened. The default is "USE_FILE_EXTENSION".</param>
        /// <param name="InRam">Optional. A boolean value representing whether the image is stored in RAM memory while open.
        /// The default is false. The settings affects only BMP images, as other image types are handled by GDAL. 
        /// Image.IsInRam property will be set to true for them, but only memory will only hold a buffer currently displayed on screen.</param>
        /// <param name="cBack">Optional. The ICallback object which will receive progress and error events while the image is being opened.</param>
        /// <returns>A boolean value representing the success or failure of opening the image.</returns>
        public bool Open(string ImageFileName, ImageType fileType, bool InRam, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the image object's image type.
        /// </summary>
        public ImageType ImageType
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns true when the image is loaded in RAM memory, and returns false when the image is not loaded in RAM memory.
        /// </summary>
        public bool IsInRam
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Changes the data source of the image object without the its closing.
        /// </summary>
        /// <param name="newImgPath">The new file to serve as a datasource for the image object.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool Resource(string newImgPath)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the image to file.
        /// </summary>
        /// <param name="ImageFileName">The filename to use to save the image.</param>
        /// <param name="WriteWorldFile">Optional. A boolean value representing whether a world file is also written. The default is false.</param>
        /// <param name="fileType">Optional. The file type to use to save the image. Default is "USE_FILE_EXTENSION".</param>
        /// <param name="cBack">Optional. The ICallback object which will receive the progress and error messages while the image is being saved.</param>
        /// <returns>A boolean value representing the success or failure of saving the image.</returns>
        public bool Save(string ImageFileName, bool WriteWorldFile, ImageType fileType, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Serializes the image properties to the string. 
        /// </summary>
        /// <param name="SerializePixels">A boolean value which indicates whether image pixels should be serialized.</param>
        /// <returns>The serialized string.</returns>
        /// \new48 Added in version 4.8
        public string Serialize(bool SerializePixels)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets instance of the labels class associated with the image.
        /// </summary>
        /// \new48 Added in version 4.8
        public Labels Labels
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The global callback is the interface used by MapWinGIS to pass progress and error events to interested applications.
        /// </summary>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
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
        /// The source type of the image.
        /// </summary>
        /// <remarks>Images start with istUninitialized state. After loading bmp images istDiskBased or istInMemory value will be set depending
        /// on the value of inMem parameter. For GDAL images istGDALBased will be set.</remarks>
        /// \new48 Added in version 4.8
        public tkImageSourceType SourceType
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Retrieves the error message associated with the specified error code. 
        /// </summary>
        /// <param name="ErrorCode">The error code for which the error message is required.</param>
        /// <returns>The error message description for the specified error code.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Retrieves the last error generated in the object. 
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates how images with identical size and position on the map will be drawn.
        /// </summary>
        /// <remarks>When set to true such images will be merged into a single bitmap. This may improve performance 
        /// in case of low number of data pixels (those which differ from the transparent color). The default value is false.</remarks>
        /// \new48 Added in version 4.8
        public bool CanUseGrouping
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns file handle for BMP image. For inner use only.
        /// </summary>
        public int FileHandle
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Get or sets an instance of GeoProjection class associated with the image.
        /// </summary>
        /// <remarks>The property can't be set to NULL (there is always an instance of GeoProjection class associated with image).</remarks>
        /// \new493 Added in version 4.9.3
        public GeoProjection GeoProjection
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns true if image object is empty, i.e. not bound to some disk-based datasource and doesn't hold any in-memory data. 
        /// This property is the same as checking Image.SourceType = istUninitialized.
        /// </summary>
        public bool IsEmpty
        {
            get { throw new NotImplementedException(); }
        }

        /// @}
        #endregion

        #region Position
        /// \addtogroup image_position Image position
        /// Here is a list of methods and properties which affect position and scale of image, conversion between map units and image coordinates.
        /// The properties and methods described here belong to Image class.
        /// \dot
        /// digraph image_management {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Image" URL="\ref Image"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Position"   URL="\ref image_position"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Retrieves projection string from the associated .prj file.
        /// </summary>
        /// <returns>Projection string or empty string on it's absence.</returns>
        public string GetProjection()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets projection string for the image.
        /// </summary>
        /// <remarks>The string will be saved to .prj file with the same name as image.</remarks>
        /// <param name="Proj4">The projection string.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool SetProjection(string Proj4)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the extents of the image.
        /// </summary>
        public Extents Extents
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the height of the image in pixels.
        /// </summary>
        public int Height
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Converts coordinates of image's pixel to the screen coordinates.
        /// </summary>
        /// <param name="ImageX">The x coordinate of the image.</param>
        /// <param name="ImageY">The y coordinate of the image.</param>
        /// <param name="projX">The resulting x coordinate of the map.</param>
        /// <param name="projY">The resulting y coordinate of the map.</param>
        /// \new48 Added in version 4.8
        public void ImageToProjection(int ImageX, int ImageY, out double projX, out double projY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the number of map units per image pixel for x axis.
        /// </summary>
        /// <remarks>The changes of this value will result in scaling of the image horizontally.</remarks>
        /// \new48 Added in version 4.8
        public double OriginalDX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the number of map units per image pixel for y axis.
        /// </summary>
        /// <remarks>The changes of this value will result in scaling of the image vertically.</remarks>
        /// \new48 Added in version 4.8
        public double OriginalDY
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the height of the image.
        /// </summary>
        /// <remarks>This property should be used to return the total height of the image, as compared to the height of the image buffer.</remarks>
        /// <returns>The height of the image in pixels.</returns>
        /// \new48 Added in version 4.8
        public int OriginalHeight
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the width of the image.
        /// </summary>
        /// <remarks>This property should be used to return the total width of the image, as compared to the width of the image buffer.</remarks>
        /// <returns>The width of the image in pixels.</returns>
        /// \new48 Added in version 4.8
        public int OriginalWidth
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the x coordinate of the center of lower left pixel of the image in map units.
        /// </summary>
        /// \new48 Added in version 4.8
        public double OriginalXllCenter
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the y coordinate of the center of lower left pixel of the image in map units.
        /// </summary>
        /// \new48 Added in version 4.8
        public double OriginalYllCenter
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the coordinates of pixel in image buffer which correspond to the given point on the map.
        /// </summary>
        /// <param name="projX">X coordinate of the point in map units.</param>
        /// <param name="projY">Y coordinate of the point in map units.</param>
        /// <param name="BufferX">The resulting x coordinate of pixel in the image buffer.</param>
        /// <param name="BufferY">The resulting y coordinate of pixel in the image buffer.</param>
        /// \new48 Added in version 4.8
        public void ProjectionToBuffer(double projX, double projY, out int BufferX, out int BufferY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the coordinates of image pixel which correspond to the given point on the map.
        /// </summary>
        /// <param name="projX">X coordinate of the point in map units.</param>
        /// <param name="projY">Y coordinate of the point in map units.</param>
        /// <param name="ImageX">The resulting x coordinate of image pixel.</param>
        /// <param name="ImageY">The resulting y coordinate of image pixel.</param>
        /// \new48 Added in version 4.8
        public void ProjectionToImage(double projX, double projY, out int ImageX, out int ImageY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the width of the image in pixels.
        /// </summary>
        public int Width
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the x coordinate of the center of the lower-left pixel in the image. 
        /// </summary>
        public double XllCenter
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the y coordinate of the center of the lower-left pixel in the image. 
        /// </summary>
        public double YllCenter
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the number of map units per pixel of image buffer for x axis.
        /// </summary>
        /// <remarks>The set part of the property is not applicable for GDAL images.</remarks>
        public double dX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the number of map units per pixel of image buffer for y axis.
        /// </summary>
        /// <remarks>The set part of the property is not applicable for GDAL images.</remarks>
        public double dY
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
        /// @}
        #endregion

        #region Visualization
        /// \addtogroup image_visualization Image visualization
        /// Here is a list of properties that affect the visualization of the image.
        /// The properties and methods described here belong to Image class.
        /// \dot
        /// digraph image_visualization {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Image" URL="\ref Image"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Visualization"   URL="\ref image_visualization"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        ///  Sets transparent color for image.
        /// </summary>
        /// <param name="Color">New transparent color.</param>
        /// <remarks>This is shortcut to set both Image.TransparencyColor and Image.TransparencyColor2. 
        /// It also set Image.UseSetTranparencyColor to true.</remarks>
        /// \new491 Added in version 4.9.1
        public void SetTransparentColor(uint Color)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The interpolation mode for images which are displayed with the resolution higher than original, i.e. after zoom in (down).
        /// </summary>
        public tkInterpolationMode DownsamplingMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the drawing method of image. 
        /// </summary>
        /// <remarks>Was used temporarily while moving from GDI to GDI+ drawing.</remarks>
        /// \deprecated v 4.8. No longer used.
        public int DrawingMethod
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// A boolean value which indicates whether an image will be displayed using a gray palette.
        /// </summary>
        public bool SetToGrey
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a color which is used as the transparency color for the image. 
        /// </summary>
        public uint TransparencyColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a second color which is used as the transparency color for the image. 
        /// </summary>
        /// <remarks>All the colors between TransparencyColor and TransparencyColor2 will be transparent.
        /// This "between" relation is calculated for each color channel and then the results are combined by AND operation.</remarks>
        public uint TransparencyColor2
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which define alpha component of image colors (0.0-1.0).
        /// </summary>
        /// <remarks>The values range from 0.0(fully transparent) to 1.0(fully opaque).</remarks>
        public double TransparencyPercent
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The interpolation mode for images which are displayed with the resolution lower than original, i.e. after zooming out (up).
        /// </summary>
        public tkInterpolationMode UpsamplingMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }


        /// <summary>
        /// Gets or sets whether or not the transparency color is being used when the image is displayed. 
        /// </summary>
        public bool UseTransparencyColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// @}
        #endregion

        #region Data access
        /// \addtogroup image_data Data access
        /// Here is a list of methods and properties which provide access to the values of pixels.
        /// The properties and methods described here belong to Image class.
        /// \dot
        /// digraph image_data {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Image" URL="\ref Image"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Data access"   URL="\ref image_data"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Gets the data from the image and puts it into the selected bitmap in the specified device context. 
        /// </summary>
        /// <remarks>This function requires the width and height of the selected bitmap and the image to match. 
        /// Use of this function requires advanced knowledge in windows graphics concepts and is intended for advanced users only. </remarks>
        /// <param name="hdc">The handle to the device context. </param>
        /// <returns>A boolean value representing the success or failure of getting the device context handle for the image.</returns>
        public bool GetImageBitsDC(int hdc)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This function uses a bitmap selected in the specified device context handle as the data to copy into the image for which the 
        /// function is called. </summary>
        /// <remarks>This function requires the image to be the same width and height as the bitmap selected in the device context.
        /// Use of this function requires advanced knowledge in windows graphics concepts and is intended for advanced users only.</remarks>
        /// <param name="hdc">The device context handle of the device context for which the selected bitmap is to be used to 
        /// copy the bits into the image used to call the function. </param>
        /// <returns>A boolean value representing success or failure of setting the image's bits using the bitmap 
        /// selected in the specified device context. </returns>
        public bool SetImageBitsDC(int hdc)
        {
            throw new NotImplementedException();
        }


        /// <summary>
        /// Gets a row of pixels from the image. 
        /// </summary>
        /// <param name="Row">The row of pixels you want to get from the image.</param>
        /// <param name="Vals">An array which will return the pixels in the specified row of the image. </param>
        /// <returns>A boolean value representing the success or failure of getting the row of pixels from the image.</returns>
        public bool GetRow(int Row, ref int Vals)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the arrays with the unique colors of the image and their frequencies. 
        /// </summary>
        /// <remarks>For large images it's recommended to perform the operation with the smaller
        /// size of buffer to improve performance and possible RAM overuse.</remarks>
        /// <param name="MaxBufferSizeMB">The size of buffer for calculation.</param>
        /// <param name="Colors">The output array of unsigned integer type with unique colors.</param>
        /// <param name="Frequencies">The output array of the same size with Colors array with the frequency of the each color.</param>
        /// <returns>The number of unique colors.</returns>
        public int GetUniqueColors(double MaxBufferSizeMB, out object Colors, out object Frequencies)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the picture object.
        /// </summary>
        public stdole.IPictureDisp Picture
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the value of a pixel in the image. 
        /// </summary>
        /// <param name="Row">The row in the image for which the value is required.</param>
        /// <param name="col">The column in the image for which the value is required.</param>
        /// <returns>The value of the specified pixel in the image.</returns>
        public int get_Value(int Row, int col)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the value of a pixel in the image. 
        /// </summary>
        /// <param name="Row">The row in the image for which the value is required.</param>
        /// <param name="col">The column in the image for which the value is required.</param>
        /// <param name="pVal">The value of the specified pixel in the image.</param>
        public void set_Value(int Row, int col, int pVal)
        {
            throw new NotImplementedException();
        }

        /// @}
        #endregion 
        
        #region Grid rendering
        /// \addtogroup image_grid_render Grid rendering
        /// Here is a list of properties and methods that affects rendering of grids.
        /// The properties and methods described here belong to Image class.\n\n
        /// \dot
        /// digraph image_grid_render {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Image" URL="\ref Image"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Grid rendering"   URL="\ref image_grid_render"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>\n\n
        /// \anchor a_grid_render
        /// <b>Working with grid data.</b> \n\n
        /// %Image class can be used for rendering grid data. %Grid datasources can be rendered in 2 ways:
        /// - direct rendering using GDAL drivers (see grid rendering section below);
        /// - rendering using proxy image.
        /// 
        /// \dot
        /// digraph rendering_types2 {
        /// splines = true;
        /// ranksep = 0.15;
        /// nodesep = 0.5;
        /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
        /// render [ label="Grid rendering"];
        /// 
        /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
        /// s1    [label=" Direct rendering\l"];
        /// s2    [label=" Using proxy image\l"];
        /// 
        /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" minlen=2 ]
        /// render -> s1;
        /// render -> s2;
        /// }
        /// \enddot
        /// In both cases the name of the original datasource is stored in Image.SourceGridName property. Source grid can be opened using 
        /// Image.OpenAsGrid. No interpolation is usually used during rendering of grid datasources since it's often desirable to
        /// see borders of individual pixels (see Image.UpsamplingMode, Image.DownsamplingMode). More details on grids see of Grid class.\n
        /// 
        /// <b>1. %Grid rendering.</b>\n\n
        /// Any GDAL-based image can be displayed using so called <i>grid rendering</i>, i.e. not by using it's own colors 
        /// but by mapping the values of one of its bands to new set of colors. Grid rendering is used 
        /// depending on datasource type:
        /// - for grid datasources it's the only one available since no colors are stored in datasource itself;
        /// - for regular RGB images the usage can be turned on/off with Image.AllowGridRendering property; grid rendering 
        /// in this case actually is nothing more than the usage of synthetic colors to display on of the original color bands.
        /// 
        /// Image.IsRgb property can be used to determine whether the current datasource is grid or RGB image. The rendering 
        /// method can be returned by Image.GridRendering property. Grid rendering always applies to a single band specified 
        /// by Image.SourceGridBandIndex. \n\n
        /// Color scheme for grid rendering can be set using:
        /// - Image.CustomColorScheme property (or an older alternative Image._pushSchemetkRaster method);
        /// - Image.ImageColorScheme property (will be used when no custom color scheme was set);
        /// .
        /// Forcing grid rendering for RGB image:
        /// \code
        /// string filename = "image.tif";
        /// var img = new Image();
        /// if (img.Open(filename, ImageType.USE_FILE_EXTENSION, true, null))
        /// {
        ///     if (img.IsRgb)
        ///     {
        ///         img.AllowGridRendering = tkGridRendering.grForceForAllFormats;
        ///         img.ImageColorScheme = PredefinedColorScheme.SummerMountains;
        ///         img.SourceGridBandIndex = 2;    // let's choose second band 
        ///         Debug.Print("Image is rendered as grid: " + img.GridRendering);
        ///     }
        /// }
        /// \endcode
        /// Setting custom unique values color scheme for directly rendered grid:
        /// \code
        /// string filename = "grid.asc";        
        /// var img = new Image();
        /// if (img.Open(filename, ImageType.USE_FILE_EXTENSION, true, null))
        /// {
        ///     if (!img.IsRgb)
        ///     {
        ///         var grid = img.OpenAsGrid();
        ///         if (grid != null)
        ///         {
        ///             var scheme = grid.GenerateColorScheme(tkGridSchemeGeneration.gsgUniqueValues, PredefinedColorScheme.FallLeaves);
        ///             img.CustomColorScheme = scheme;
        ///             grid.Close();
        ///             Debug.Print("Image is rendered as grid: " + img.GridRendering);
        ///         }
        ///     }
        /// }
        /// \endcode
        /// Setting default color scheme for directly rendered grid:
        /// \code
        /// string filename = "grid.asc";
        /// var img = new Image();
        /// if (img.Open(filename, ImageType.USE_FILE_EXTENSION, true, null))
        /// {
        ///     if (!img.IsRgb)
        ///     {
        ///         img.ImageColorScheme = PredefinedColorScheme.Glaciers;
        ///         img.CustomColorScheme = null;   // make sure that there is no custom color scheme to override out setting
        ///     }
        /// }
        /// \endcode
        /// 
        /// <b>2. Proxy images for grids.</b>\n\n
        /// Proxy image is regular RGB image file created for rendering a grid. It can have BMP or GTiff format and name of 
        /// the grid + "_proxy" postfix (see details on their creation in Grid class). 
        /// The rendering of proxy image has no differences compared to any other RGB image. 
        /// Additional responsibilities of Image class in this case include:
        /// - identification of proxy image on opening;
        /// - providing diagnostic properties for user, so that original grid can be accessed.
        /// 
        /// It's implemented like this:
        /// - Image.Open method will analyze filename for "_proxy" suffix and then will try to find .mwleg color scheme file for it;
        /// - if such file exists, the name of the source grid will be read from it and set to Image.SourceGridName property;
        /// - Image.IsGridProxy property will be set to true.
        /// .
        /// The source grid can be accessed using Image.OpenAsGrid method.\n\n
        /// Here is an examples which demonstrate changing of format for proxy image already added to the map from BMP to GTiff 
        /// (it's not very practical example, but good demonstration of API):
        /// \code
        /// int layerHandle = 0;     // a layer with image
        /// var img = axMap1.get_Image(layerHandle);
        /// if (img.IsGridProxy && img.SourceType != tkImageSourceType.istGDALBased)
        /// {
        ///     var grid = img.OpenAsGrid();
        ///     if (grid != null)
        ///     {
        ///         var scheme = img.GridProxyColorScheme;  // we shall use the same color scheme
        ///         axMap1.RemoveLayer(layerHandle);        // removing original proxy
        ///  
        ///         var gs = new GlobalSettings();
        ///         gs.GridProxyFormat = tkGridProxyFormat.gpfTiffProxy;    // new one must be GTiff
        /// 
        ///         var newImage = grid.CreateImageProxy(scheme);
        ///         if (newImage != null)
        ///         {
        ///             axMap1.AddLayer(newImage, true);
        ///         }
        ///     }
        /// }
        /// \endcode
        /// @{
                /// <summary>
                /// Gets or sets the value which indicates whether the image will be rendered as grid, i.e. 
                /// with a color scheme applied to a band set by Image.SourceGridBandIndex.
                /// </summary>
                /// \new491 Added in version 4.9.1
                public tkGridRendering AllowGridRendering
                {
                    get { throw new NotImplementedException(); }
                    set { throw new NotImplementedException(); }
                }

                /// <summary>
                /// Gets or sets the color scheme to render image with.
                /// </summary>
                /// <remarks>This property should be used for grid datasources in the first place (when there is no build-in colors). 
                /// But may also be used for a single band of regular RGB/ARGB images for synthetic colors and hillshading.
                /// When no custom color scheme is set, grid rendering is chosen the color scheme from Image.ImageColorScheme will be used.</remarks>
                /// \new491 Added in version 4.9.1
                public GridColorScheme CustomColorScheme
                {
                    get { throw new NotImplementedException(); }
                    set { throw new NotImplementedException(); }
                }

                /// <summary>
                /// Gets the value which indicates whether an image object represents a visualization proxy for another datasource.
                /// </summary>
                /// <remarks>Will return true if the image was opened with Grid.OpenAsImage only. When opened directly the link to the source grid will be lost.</remarks>
                /// \new491 Added in version 4.9.1
                public bool IsGridProxy
                {
                    get { throw new NotImplementedException(); }
                }

                /// <summary>
                /// Returns true if the image is rendered as grid.
                /// </summary>
                /// \new491 Added in version 4.9.1
                public bool GridRendering
                {
                    get { throw new NotImplementedException(); }
                }

                /// <summary>
                /// Opens the datasource of the image as grid. 
                /// </summary>
                /// <returns>Grid object or null if the operation failed.</returns>
                /// <remarks>If the image represents a visualization proxy for grid, than the grid datasource will be opened,
                /// otherwise the datasource of image itself.</remarks>
                /// \new491 Added in version 4.9.1
                public Grid OpenAsGrid()
                {
                    throw new NotImplementedException();
                }

                /// <summary>
                /// Gets the name of the source grid in case the image represent a visualization proxy for a grid.
                /// </summary>
                /// \new490 Added in version 4.9.0
                public string SourceGridName
                {
                    get { throw new NotImplementedException(); }
                }

                /// <summary>
                /// Gets or sets index of band within datasource whose values should be used to apply color scheme. 
                /// </summary>
                /// <remarks>Other bands will ignored during display process. This is useful for multiband grids (e.g. NetCDF),
                /// but may also be used with RGB images, to set synthetic colors for a single band.</remarks>
                /// \new491 Added in version 4.9.1
                public int SourceGridBandIndex
                {
                    get { throw new NotImplementedException(); }
                    set { throw new NotImplementedException(); }
                }

                /// <summary>
                /// Indicate whether hillshade algorithm will be used for generation of image representation of the grid.
                /// </summary>
                /// <remarks>The behaviour has changed in v4.9. Now when active it only switches ColoringType from Hillshade to Gradient. 
                /// In v.4.8. it used to have the same behaviour as Image.AllowExternalColorScheme.</remarks>
                public bool AllowHillshade
                {
                    get { throw new NotImplementedException(); }
                    set { throw new NotImplementedException(); }
                }

                /// <summary>
                /// Sets color scheme to use in process of generation of the image representation of the grid.
                /// </summary>
                /// <param name="cScheme">The color scheme.</param>
                /// <returns>True on success and false otherwise.</returns>
                public bool _pushSchemetkRaster(GridColorScheme cScheme)
                {
                    throw new NotImplementedException();
                }

                /// <summary>
                /// Gets or sets the predefined color scheme for the image. Applicable for GDAL images only.
                /// </summary>
                /// <remarks>The color scheme is used for generation of image representation of the grid.</remarks>
                public PredefinedColorScheme ImageColorScheme
                {
                    get { throw new NotImplementedException(); }
                    set { throw new NotImplementedException(); }
                }

                /// <summary>
                /// Gets grid color scheme this proxy image was generated with. The method should be called for proxy images only: Image.IsGridProxy = true.
                /// </summary>
                /// <remarks>Color scheme will be read from .mwleg XML file on the disk.</remarks>
                public GridColorScheme GridProxyColorScheme
                {
                    get { throw new NotImplementedException(); }
                }
        /// @}
        #endregion

        /// @}
    }
#if nsp
}
#endif
