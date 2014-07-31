using System;

#if nsp
namespace MapWinGIS
{
#endif

    /// <summary>
    /// The grid object is used to represent a grid which can be added to the map.
    /// </summary>
    /// <remarks>
    /// \dot
    /// digraph grid_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// node [color = tan, peripheries = 1, height = 0.3, width = 1.0];
    /// grid [ label="Grid" URL="\ref Grid"];
    /// header [ label="GridHeader" URL="\ref GridHeader"];
    /// 
    /// edge [ dir = none, style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6  ]
    /// grid -> header [ URL="\ref Grid.Header", tooltip = "Grid.Header", headlabel = "   1"];
    /// }
    /// \enddot
    /// 
    /// %Grid datasource holds values describing distribution of certain characteristic over territory (e.g. elevation, population, precipitation, etc.), 
    /// but usually doesn't define how these values should be rendered. In most cases grid datasource have a single band, but there may 
    /// be mutiband ones as well (NetCDF format for example). \n
    /// 
    /// To render a grid, a color scheme must be chosen which will define how grid values will be mapped to colors. Some datasources may hold such 
    /// color scheme (for examples GTiff with indexed colors). In many other cases the choice of color scheme is arbitrary. Here is an example of the 
    /// same grid rendered with 2 different color schemes (SummerMountains and DesertFires).\n
    /// 
    /// \image html grid_pair.jpg
    /// 
    /// MapWinGIS doesn't render Grid class directly, but uses Image class to do it. The distinction between these two is:
    /// - Grid class provides access to underlying values of a stored characteristic (e.g. population, precipitation, etc.);
    /// - Image class holds colors for each of those values mapped with specific color scheme.
    /// .
    /// By default MapWinGIS renders grid with hillshading effect which can be switched on/off and adjusted in GridColorScheme class.\n
    /// 
    /// Metadata about grid can be accessed through GridHeader object (Grid.Header property). It includes size, coordinate system and projection, etc.
    /// 
    /// \attention Grid.Header property must not be called multiple times in a loop as it can trigger reading the data from disk on each occasion.
    /// 
    /// \n
    /// <b>A. TYPES OF GRID RENDERING.</b> \n\n
    /// MapWinGIS can render grid datasource in 2 different ways: \n
    /// 
    /// \dot
    /// digraph rendering_types {
    /// splines = true;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// render [ label="Grid rendering"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" Direct rendering by Image class\l"];
    /// s2    [label=" Using proxy image\l"];
    /// 
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" minlen=2 ]
    /// render -> s1;
    /// render -> s2;
    /// }
    /// \enddot
    /// 
    /// 1. Direct rendering by Image class.\n
    /// 
    /// - grid datasource is opened by GDAL drivers;
    /// - before each rendering values of grids are mapped into colors + additional hillshading effect may be applied;
    /// - the rendering process is much more computationally intensive than rendering of regular RGB images,
    /// therefore an approach provide poor performance for large datasets.
    /// 
    /// %Grid rendering may also by applied to a single band of regular RGB image, displaying it with synthetic colors. 
    /// See more information on direct grid rendering in description of Image class.\n
    /// 
    /// \note <i>Internally grids can be handled by either GDAL drivers or MapWinGIS own drivers. There is no way to affect
    /// which one will be chosen, however it's possible to check which one is used by Grid.SourceType 
    /// property. The significance of it is that grid formats not supported by GDAL can't rendered directly by Image class.</i>
    /// 
    /// 2. Creation of so called <b>proxy image</b> to render a grid.\n
    /// - all values of the grid are mapped to colors and the output is saved as a new image file with BMP or GTiff format;
    /// - the selection of image format depends upon GlobalSettings.GridProxyFormat property;
    /// - conversion is made by Utils.GridToImage2 method; the operation may take significant amount of time;
    /// - Grid.OpenAsImage, Grid.CreateImageProxy, FileManager.Open can create proxy images by running Utils.GridToImage2 method internally;
    /// - a proxy image is written with the same name as for the source grid + "_proxy.bmp" or "_proxy.tif" postfix, for example a proxy for "grid.asc" will 
    /// be named "grid_proxy.tif";
    /// - a color scheme always is written to the disk along with image (xml file with .mwleg extension, for example "grid_proxy.bmp.mwleg");
    /// .
    /// For multiband datasources proxy image will be created for the band marked as active by Grid.ActiveBandIndex. Use Grid.OpenBand to change the index.\n
    /// 
    /// There is no visual differences between 2 methods of grid rendering, however performance may differ substantially.\n
    /// 
    /// <b>B. COLOR SCHEME FOR GRID.</b> \n
    /// 
    /// Color scheme for grids can be retrieved in the following ways:\n
    /// - from .mwleg XML file previously saved to the disk;
    /// - from datasource itself, for example color table in indexed GTiff datasource (Grid.RasterColorTableColoringScheme); 
    /// - can be automatically generated using PredefinedColorScheme and values of grid (Grid.GenerateColorScheme);
    /// - manual generation (in case some very specific coloring is required).
    /// 
    /// The first 2 approaches are covered by Grid.RetrieveColorScheme. Grid.RetrieveOrGenerateColorScheme method will look for any existing color scheme first, and 
    /// if nothing is available will resort to generation. This is recommended approach in the most scenarios. \n
    /// 
    /// To generate color scheme manually the following code can be used:
    /// \code
    /// var grid = some_code_to_open_grid;
    /// var scheme = new GridColorScheme();
    /// scheme.UsePredefined((double)grid.Minimum, (double)grid.Maximum, PredefinedColorScheme.SummerMountains);
    /// scheme.ApplyColoringType(ColoringType.Hillshade);
    /// \endcode
    /// See PredefinedColorScheme enumerations for available colors. Hillshading effect can be turned on/off with GridColorScheme.ApplyColoringType
    /// or by changing coloring of individual breaks (GridColorBreak.ColoringType).
    ///    
    /// <b>C. HOW TO ADD GRID TO THE MAP.</b> \n
    /// 
    /// Depending on requirements it can be done in fully automatic way with a single line or code (which is the best thing to do in most cases), or 
    /// with manual coding if default behaviour isn't suitable. The approaches can be classified as:
    /// - passing Grid object AxMap.AddLayer method;
    /// - manual creation of Image for a Grid and passing it to AxMap.AddLayer;
    /// - using FileManager class (in fact it uses the first approach internally, therefore all description is applicable for it as well).
    /// 
    /// \dot
    /// digraph opening_types {
    /// splines = true;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// parent [ label="Grid opening"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" With FileManager class" ]
    /// s2    [label=" Passing Grid to AxMap.AddLayer" ]
    /// s3    [label=" Passing Image to AxMap.AddLayer" ]
    /// 
    /// 
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" minlen=2]
    /// parent -> s1;
    /// parent -> s2;
    /// parent -> s3;
    /// }
    /// \enddot
    /// 
    /// \attention The samples below assume that the following variables are defined:
    /// \code
    /// string filename = @"d:\grid.asc";
    /// Utils ut = new Utils();
    /// \endcode
    /// 
    /// <b>1. Using FileManager class.</b> \n\n
    /// The most generic one:
    /// \code
    /// axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosAutoDetect, true);
    /// \endcode
    /// 
    /// With creation of proxy image:
    /// \code
    /// int handle = axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosProxyForGrid, true);
    /// \endcode
    /// 
    /// Opening as direct grid. Along with usual reasons (invalid filename, corrupted file), the approach may fail in case: \n
    /// - size of grid exceeds GlobalSettings.MaxNoProxyGridSizeMb property (this may be addressed by changing global settings like demonstrated);
    /// - grid format isn't supported by GDAL (nothing can be done about that).
    /// 
    /// \code
    /// var gs = new GlobalSettings();
    /// gs.MaxNoProxyGridSizeMb = 100.0;    // size in MB; 20.0 is default value
    /// handle = axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosDirectGrid, true);
    /// \endcode
    /// 
    /// With a check that something was actually opened:
    /// \code
    /// int handle = axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosAutoDetect, true);
    /// if (handle != -1 && axMap1.FileManager.LastOpenIsSuccess) 
    /// {
    ///     MessageBox.Show("Grid was added to the map. Open strategy: " + axMap1.FileManager.LastOpenStrategy.ToString());
    /// }
    /// else
    /// {
    ///     MessageBox.Show("Failed to open grid: " + ut.ErrorMsgFromObject(axMap1.FileManager));
    /// }
    /// \endcode
    /// 
    /// Using custom instance of FileManager:
    /// \code
    /// var fm = new FileManager();
    /// if (fm.get_CanOpenAs(filename, tkFileOpenStrategy.fosDirectGrid))
    /// {
    ///    var img = fm.OpenRaster(filename, tkFileOpenStrategy.fosDirectGrid, null);
    ///    if (img == null)
    ///    {
    ///        MessageBox.Show("Failed to open grid: " + ut.ErrorMsgFromObject(fm) + "; Open strategy: " + fm.LastOpenStrategy);
    ///    }
    ///    else
    ///    {
    ///        int handle = axMap1.AddLayer(img, true);
    ///    }
    /// }
    /// \endcode
    /// 
    /// <b>2. Passing Grid to AxMap.AddLayer method.</b> \n\n
    /// The following logic will be executed:
    /// - color scheme will retrieved with Grid.RetrieveOrGenerateColorScheme;
    /// - Grid.OpenAsImage will be called to choose rendering mode.
    /// .
    /// The selection of rendering mode depends on:
    /// - Grid.PreferedDisplayMode property;
    /// - if previous set to auto, GlobalSettings.GridProxyMode property.
    /// 
    /// See details in description of tkGridProxyMode enumeration.\n\n
    /// The selection of proxy format depends on GlobalSettings.GridProxyFormat property.\n\n
    /// A color scheme used for rendering will be written to disk with:
    /// - datasource name + .mwleg extension (for direct rendering);
    /// - proxy name + .mwleg extension (for proxy rendering);
    /// .
    /// For direct rendering saving of color scheme to disk can be disabled with GlobalSettings.SaveGridColorSchemeToFile property.\n\n
    /// The selection of color scheme depends on: 
    /// - GlobalSettings.RandomColorSchemeForGrids (if true will be selected randomly from PredefinedColorScheme enumeration);
    /// - GlobalSettings.DefaultColorSchemeForGrids (when random one is turned off);
    /// .
    /// \code
    /// var grid = new Grid();
    /// if (grid.Open(filename, GridDataType.UnknownDataType, true, GridFileType.UseExtension, null))
    /// {
    ///    grid.PreferedDisplayMode = tkGridProxyMode.gpmAuto;   // or other mode if needed
    ///    int handle = axMap1.AddLayer(grid, true);
    ///    var img = axMap1.get_Image(handle);
    ///    if (img != null)
    ///    {
    ///        MessageBox.Show("Grid was opened. Rendering: " + (img.IsGridProxy ? "using proxy" : "direct"));
    ///    }
    ///    else
    ///    {
    ///        MessageBox.Show("To add grid to map: " + axMap1.get_ErrorMsg(axMap1.LastErrorCode));
    ///    }
    ///    grid.Close();   // we no longer need it as Image class is used for rendering
    /// }
    /// else
    /// {
    ///    MessageBox.Show("Failed to open grid: " + ut.ErrorMsgFromObject(grid));
    /// }
    /// \endcode
    /// <b>3. Manual opening/creation of Image for Grid.</b> \n\n
    /// With automatic selection of rendering mode:
    /// \code
    /// var grid = new Grid();
    /// if (grid.Open(filename, GridDataType.UnknownDataType, true, GridFileType.UseExtension, null))
    /// {
    ///     var scheme = grid.RetrieveOrGenerateColorScheme(tkGridSchemeRetrieval.gsrAuto, tkGridSchemeGeneration.gsgGradient, PredefinedColorScheme.SummerMountains);
    ///     var img = grid.OpenAsImage(scheme, tkGridProxyMode.gpmAuto, null);
    ///     if (img != null)
    ///     {
    ///         int handle = axMap1.AddLayer(img, true);
    ///     }
    ///     else
    ///     {
    ///         MessageBox.Show("Failed to create image representation for the grid: " + ut.ErrorMsgFromObject(grid));
    ///     }
    ///     grid.Close();   // we no longer need it as Image class is used for rendering
    /// }
    /// else
    /// {
    ///     MessageBox.Show("Failed to open grid: " + ut.ErrorMsgFromObject(grid));
    /// }
    /// \endcode
    /// Opening for direct rendering without using Grid class:
    /// \code
    /// var image = new Image();
    /// if (image.Open(filename, ImageType.USE_FILE_EXTENSION, false, null))
    /// {
    ///     if (image.IsRgb)
    ///     {
    ///         MessageBox.Show("It's not a grid.");
    ///     }
    ///     else
    ///     {
    ///         image.ImageColorScheme = PredefinedColorScheme.SummerMountains;     // or any other
    ///         int handle = axMap1.AddLayer(image, true);
    ///         if (handle == -1)
    ///         {
    ///             MessageBox.Show("Failed to open image: " + axMap1.get_ErrorMsg(axMap1.LastErrorCode));
    ///         }
    ///         else
    ///         {
    ///             MessageBox.Show("Data source is rendered as: " + (image.GridRendering ? "grid" : "image"));
    ///         }
    ///     }
    /// }
    /// else
    /// {
    ///     MessageBox.Show("Failed to open image: " + ut.ErrorMsgFromObject(image));
    /// }
    /// \endcode
    /// Manual creation of image proxy and color scheme:
    /// \code
    /// var grid = new Grid();
    /// if (grid.Open(filename, GridDataType.UnknownDataType, true, GridFileType.UseExtension, null))
    /// {
    ///     var scheme = new GridColorScheme();
    ///     scheme.UsePredefined((double)grid.Minimum, (double)grid.Maximum, PredefinedColorScheme.SummerMountains);
    ///     scheme.ApplyColoringType(ColoringType.Hillshade);
    /// 
    ///     // grid.CreateImageProxy(scheme) can be used as well
    ///     var img = ut.GridToImage2(grid, scheme, tkGridProxyFormat.gpfTiffProxy, false, null);
    ///     if (img == null)
    ///     {
    ///         MessageBox.Show("Failed to create proxy image for grid:" + ut.ErrorMsgFromObject(ut));
    ///     }
    ///     else
    ///     {
    ///         int handle = axMap1.AddLayer(img, true);
    ///     }
    /// 
    ///     grid.Close();   // we no longer need it as Image class is used for rendering
    /// }
    /// else
    /// {
    ///     MessageBox.Show("Failed to open grid: " + ut.ErrorMsgFromObject(grid));
    /// }
    /// \endcode
    /// To work with image proxies directly the following API members can be used: Grid.CreateImageProxy, Grid.HasValidImageProxy, Grid.RemoveImageProxy,
    /// Image.IsGridProxy.\n
    /// 
    /// <b>D. Reading and editing the values.</b> \n\n
    /// Grids can be opened in either in-memory or disk based mode. %Grid values can be retrieved and edited using:
    /// - Grid.get_Value / Grid.set_Value methods;
    /// - Grid.GetRow2 / Grid.PutRow2, Grid.GetFloatWindow2 / Grid.PutFloatWindow2 methods (for .NET clients perhaps only after tweaking marshalling in interop assemblies).
    /// 
    /// For in-memory mode to save the changes to disk Grid.Save methods must be called. In disk-based mode the changes are saved automatically.\n\n
    /// The size of grid can be obtained from its header (GridHeader.NumberRows, GridHeader.NumberCols). \n
    /// 
    /// For multiband datasource to read or edit values in the band other than first one use Grid.OpenBand.
    /// \code
    /// bool inRam = true;
    /// var grid = new Grid();
    /// if (grid.Open(filename, GridDataType.UnknownDataType, inRam, GridFileType.UseExtension, null))
    /// {
    ///     var header = grid.Header;   // better to cache it in variable to avoid multiple reading from disk
    ///     double noData = (double)header.NodataValue;
    ///     for (int i =0; i < header.NumberCols; i++)
    ///     {
    ///         for (int j = 0; j < header.NumberCols; j++)
    ///         {
    ///             double val = grid.get_Value(i, j)
    ///             if (val != noData && val > 1000)
    ///             {
    ///                 grid.set_Value(i, j, noData);
    ///             }
    ///         }
    ///     }
    /// 
    ///     // when inRam is set to false no saving is needed
    ///     if (!grid.Save(grid.Filename, GridFileType.UseExtension, null ))
    ///     {
    ///         MessageBox.Show("Failed to save changes: " + ut.ErrorMsgFromObject(grid));
    ///     }
    ///     grid.Close();
    /// }
    /// else
    /// {
    ///     MessageBox.Show("Failed to open grid: " + ut.ErrorMsgFromObject(grid));
    /// }
    /// \endcode
    /// <b>E. Coordinate system and projection.</b> \n\n
    /// Information about geographic coordinate system and projection of grid can be retrieved using
    /// GridHeader.GeoProjection property. It is used by AxMap control when AxMap.GrabProjectionFromData is true. However
    /// grid can also be rendered fine if no coordinate system is specified, although projection aware functionality of MapWinGIS 
    /// (tiles, scalebar) will be unavailable or won't function properly.\n\n
    /// The location and scaling of grid in world coordinates is defined by GridHeader.XllCenter, GridHeader.YllCenter,
    /// GridHeader.dX, GridHeader.dY. While Grid.CellToProj, Grid.ProjToCell can be used to convert from cell coordinates to world coordinates. \n\n
    /// Information about coordinates system and projection will be copied to output image when a proxy for the grid is created by Utils.GridToImage2
    /// and all other API members which are using this method.
    /// \code
    /// var grid = new Grid();
    /// if (grid.Open(filename, GridDataType.UnknownDataType, inRam, GridFileType.UseExtension, null))
    /// {
    ///     var gp = grid.Header.GeoProjection;
    ///     MessageBox.Show(gp.IsEmpty() ? "No coordinates system for grid is specified." :  "Coordinate system for grid: " + grid.Header.GeoProjection.ExportToWKT());
    ///     if (gp.IsEmpty)
    ///     {
    ///         double x, y;
    ///         grid.CellToProj(0, 0, out x, out y);
    ///         MessageBox.Show(string.Format("World coordinates of the first cell: x={0}; y={1}", x, y));
    ///     }
    ///     grid.Close();
    /// }
    /// \endcode
    /// <b>F. Support of GDAL overviews (version 4.9.2): </b>\n\n
    /// When opening grids MapWinGIS will recoginze existing GDAL overviews 
    /// and will automatically create new overviews if needed (GlobalSettings.MinOverviewWidth). 
    /// By default (GlobalSettings.GridProxyMode = gpmAuto) overviews option will always be chosen 
    /// over proxy image creation, as it takes significantly less time. For original grids external 
    /// overviews will be created (.ovr file), for proxy images - built-in overviews.\n
    /// The use of proxy images for grid can still occur under following conditions:
    /// - proxy image already exists while there is no overviews;
    /// - grid datasource isn't supported by GDAL, so can't be opened directly;
    /// - proxy image option was chosen explicitly with GlobalSettings.GridProxyMode = gpmUseProxy
    /// or AxMap.AddLayerFromFilename (filename, fosProxyForGrid, true).
    /// </remarks>
    #if nsp
        #if upd
            public class Grid : MapWinGIS.IGrid
        #else        
            public class IGrid
        #endif
    #else
        public class Grid
    #endif
    {
        #region IGrid Members
        /// <summary>
        /// Assign new projection to the grid.
        /// </summary>
        /// <param name="Projection">Projection string in proj4 format.</param>
        /// <returns>True on success or false otherwise.</returns>
        public bool AssignNewProjection(string Projection)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the common dialog filter containing all supported file extensions in string format. 
        /// </summary>
        public string CdlgFilter
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Uses a cell's column and row position to find the center of the cell in projected map coordinates.
        /// </summary>
        /// <param name="Column">The column of the cell to find the center in projected map coordinates. </param>
        /// <param name="Row">The row of the cell to find the center in projected map coordinates. </param>
        /// <param name="x">Returns the x projected map coordinate of the center of the specified cell.</param>
        /// <param name="y"> Returns the y projected map coordinate of the center of the specified cell.</param>
        public void CellToProj(int Column, int Row, out double x, out double y)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Clears all data in the grid, setting the value of all cells in the grid to the specified clear value. 
        /// </summary>
        /// <param name="ClearValue">The value to set all of the grid's cells to.</param>
        /// <returns>A boolean value representing the success or failure of clearing the grid.</returns>
        public bool Clear(object ClearValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Closes the grid.
        /// </summary>
        /// <returns>A boolean value representing the success or failure of closing the grid.</returns>
        public bool Close()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new grid.
        /// </summary>
        /// <param name="Filename">The filename for the new grid. </param>
        /// <param name="Header">The header defining the attributes of the new grid.</param>
        /// <param name="DataType">The data type of the new grid.</param>
        /// <param name="InitialValue">The initial value for each cell of the new grid.</param>
        /// <param name="InRam">Optional. A boolean value representing the grid being stored in memory(RAM) when True, and the 
        /// grid being stored on disk when False. </param>
        /// <param name="fileType">Optional. The grid file type.</param>
        /// <param name="cBack">Optional. The ICallback object that will receive the progress and error events during the creation of the new grid.</param>
        /// <returns>A boolean value representing the success or failure of the creation of the new grid.</returns>
        public bool CreateNew(string Filename, GridHeader Header, GridDataType DataType, object InitialValue, bool InRam, GridFileType fileType, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the data type of the values stored in the grid.
        /// </summary>
        public GridDataType DataType
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The filename associated with the object.
        /// </summary>
        public string Filename
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns an array with grid values which lie within specified bounds.
        /// </summary>
        /// <remarks>For inner use or C++ use only. Only a single value will be returned to .NET.</remarks>
        /// <param name="StartRow">The start row of the window.</param>
        /// <param name="EndRow">The end row of the window.</param>
        /// <param name="StartCol">The start colour of the window.</param>
        /// <param name="EndCol">The end colour of the window.</param>
        /// <param name="Vals">A supposed array (only a single value in .NET).</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool GetFloatWindow(int StartRow, int EndRow, int StartCol, int EndCol, ref float Vals)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The faster way to read the array values that are of a specific size.
        /// </summary>
        /// <remarks> The row is the integer row to read from the grid 
        /// object. The vals variable is actually the first element of the array of floats that you want to be populated with the values from 
        /// the grid. Since arrays are stored sequentially in memory, passing the first element allows the prediction of where the other 
        /// values must go. It is very important that you always dimension the array as being of type float, and always make sure that 
        /// you dimension it from 0 to numCols - 1. </remarks>
        /// <param name="Row">The Integer value of the row to retrieve values for.</param>
        /// <param name="Vals">Reference to the first element of the array of floats that will hold the row of values. </param>
        /// <returns>True on success and false otherwise.</returns>
        public bool GetRow(int Row, ref float Vals)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The global callback is the interface used by MapWinGIS to pass progress and error events to interested applications.
        /// </summary>
        public ICallback GlobalCallback
        {
            get  { throw new NotImplementedException(); }
            set  { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the header of the grid.
        /// </summary>
        public GridHeader Header
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns whether the grid is loaded in to RAM memory or not. 
        /// </summary>
        public bool InRam
        {
            get { throw new NotImplementedException(); }
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
        }

      

        /// <summary>
        /// Opens a grid.
        /// </summary>
        /// <param name="Filename">The filename of the grid to be opened.</param>
        /// <param name="DataType">Optional. The data type of the grid to be opened. </param>
        /// <param name="InRam">Optional. A boolean value representing whether the grid will be stored in RAM or on disk.</param>
        /// <param name="fileType">Optional. The file type of the grid. The default file type is "Use Extension". </param>
        /// <param name="cBack">Optional. The ICallback object that will receive the progress and error events during the creation 
        /// of the new grid.</param>
        /// <returns>A boolean value that represents the success or failure of opening the grid. </returns>
        public bool Open(string Filename, GridDataType DataType, bool InRam, GridFileType fileType, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Converts a point in projected map coordinates to a cell (column, row) in the grid.
        /// </summary>
        /// <remarks>If the point lies outside the bounds of the grid, a column and row are returned which are outside the boundaries of the grid. 
        /// For example, if the point lies to the left or lies below the grid boundaries, a negative column or row will be returned. Similarly, 
        /// if the point lies above or to the right of the grid boundaries, a column or row which is greater than the number of columns or rows will be returned. </remarks>
        /// <param name="x">The x projected map coordinate for which the corresponding cell in the grid is required.</param>
        /// <param name="y">The y projected map coordinate for which the corresponding cell in the grid is required. </param>
        /// <param name="Column">The column the specified point lies within. This value may not be within the valid bounds of the grid.</param>
        /// <param name="Row"> The row the specified point lies within. This value may not be within the valid bounds of the grid.</param>
        public void ProjToCell(double x, double y, out int Column, out int Row)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        ///  Sets an array of values for a given region of the grid.
        /// </summary>
        /// <param name="StartRow">The start row of the window.</param>
        /// <param name="EndRow">The end row of the window.</param>
        /// <param name="StartCol">The start colour of the window.</param>
        /// <param name="EndCol">The end colour of the window.</param>
        /// <param name="Vals">A supposed array (only a single value in .NET).</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool PutFloatWindow(int StartRow, int EndRow, int StartCol, int EndCol, ref float Vals)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Provides The faster way to write the array values that are of a specific size.
        /// </summary>
        /// <remarks>
        /// The row is the integer row to read from the grid 
        /// object. The vals variable is actually the first element of the array of floats that you want to be populated with the values from 
        /// the grid. Since arrays are stored sequentially in memory, passing the first element allows the prediction of where the other values must go. It is very important that you always dimension the array as being of type float, and always make sure that 
        /// you dimension it from 0 to numCols - 1. 
        /// </remarks>
        /// <param name="Row">The Integer value of the row to retrieve values for.</param>
        /// <param name="Vals">Reference to the first element of the array of floats that will hold the row of values.</param>
        /// <returns>True on success.</returns>
        public bool PutRow(int Row, ref float Vals)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets grid colour scheme object used for rendering of the image representation of the grid, in case 
        /// the datasource has a color table (GTiff with indexed colors, for example).
        /// </summary>
        public GridColorScheme RasterColorTableColoringScheme
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Changes the grid source without closing it.
        /// </summary>
        /// <param name="newSrcPath">The name of the new file source.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool Resource(string newSrcPath)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the grid.
        /// </summary>
        /// <param name="Filename">Optional. The filename the grid will be saved under. 
        /// If no filename is specified the filename in the grid's Filename property is used.</param>
        /// <param name="GridFileType">Optional. The file type to save the grid as. 
        /// If no type is specified, the type stored in the grid object is used. </param>
        /// <param name="cBack">Optional. The ICallback object that will receive the progress and error events during the creation of the new grid.</param>
        /// <returns>A boolean value representing the success or failure of saving the grid. </returns>
        public bool Save(string Filename, GridFileType GridFileType, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets invalid values to no data values.
        /// </summary>
        /// <param name="MinThresholdValue">The minimum valid value.</param>
        /// <param name="MaxThresholdValue">The maximum valid value.</param>
        /// <returns>True in success and false otherwise.</returns>
        public bool SetInvalidValuesToNodata(double MinThresholdValue, double MaxThresholdValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Retrieves the error message associated with the specified error code.
        /// </summary>
        /// <param name="ErrorCode">The numeric code of error returned by Grid.LastErrorCode.</param>
        /// <returns>The description of the error.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the value stored in the grid at the specified cell. 
        /// </summary>
        /// <param name="Column">The column representing the cell for which the value is required.</param>
        /// <param name="Row">The row representing the cell for which the value is required.</param>
        /// <returns>The value stored in the grid in the specified cell.</returns>
        public object get_Value(int Column, int Row)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the value of the specified cell of the grid.
        /// </summary>
        /// <param name="Column">The index of the cell's column.</param>
        /// <param name="Row">The index of the cell's row.</param>
        /// <param name="pVal">The new value as variant data type.</param>
        public void set_Value(int Column, int Row, object pVal)
        {
            throw new NotImplementedException();
        }

        #endregion

        /// <summary>
        /// The index of the active band within grid.
        /// </summary>
        /// <remarks>Active band will be used on data extraction operation like Grid.get_Value, Grid.GetFloatWindow 
        /// and during creation of image proxy. </remarks>
        /// \new490 Added in version 4.9.0
        public int ActiveBandIndex
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the value indicating whether grid source can be displayed by Image class without creation of proxy file.
        /// </summary>
        /// <remarks>Check return value for the reason whey such rendering isn't possible.</remarks>
        /// \new491 Added in version 4.9.1
        public tkCanDisplayGridWoProxy CanDisplayWithoutProxy
        {
            get { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Creates a proxy image file to display grid data.
        /// </summary>
        /// <param name="ColorScheme">Colour scheme which specified how the data will be visualized.</param>
        /// <returns>Created image proxy or null if the operation failed.</returns>
        /// <remarks>The format of proxy can be changed in GlobalSettings.GridProxyFormat.</remarks>
        /// \new491 Added in version 4.9.1
        public Image CreateImageProxy(GridColorScheme ColorScheme)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets extents of the grid.
        /// </summary>
        /// \new490 Added in version 4.9.0
        public Extents Extents
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Generates colour scheme for the grid.
        /// </summary>
        /// <param name="Method">Method of generation to be used.</param>
        /// <param name="Colors">Predefined colour scheme to be mapped to the values of grid.</param>
        /// <returns>Generated colour scheme or null if the operation failed.</returns>
        /// \new491 Added in version 4.9.1
        public GridColorScheme GenerateColorScheme(tkGridSchemeGeneration Method, PredefinedColorScheme Colors)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value indicating whether grid has a valid image proxy for visualization.
        /// </summary>
        /// <remarks>To be considered valid the proxy must have proper name (with "_proxy" postfix) and colour scheme file (.mwleg).
        /// Both this files must have older creation date than the grid source file itself.</remarks>
        /// \new491 Added in version 4.9.1
        public bool HasValidImageProxy
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the maximum value stored in the grid.
        /// </summary>
        public object Maximum
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the minimum value stored in the grid.
        /// </summary>
        public object Minimum
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the number of within the grid.
        /// </summary>
        /// \new490 Added in version 4.9.0
        public int NumBands
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Opens grid as image. 
        /// </summary>
        /// <param name="Scheme">Colour scheme which specifies how grid will be visualized.</param>
        /// <param name="proxyMode">Proxy mode to be used.</param>
        /// <param name="cBack">Callback interface.</param>
        /// <returns>Resulting image object or null if the operation failed.</returns>
        /// <remarks>Depending on the proxy mode, grid datasource can be opened directly, or a proxy file can be created.</remarks>
        /// \new491 Added in version 4.9.1
        public Image OpenAsImage(GridColorScheme Scheme, tkGridProxyMode proxyMode = tkGridProxyMode.gpmAuto, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        ///  Makes the specified band of the grid active.
        /// </summary>
        /// <param name="bandIndex">Index of the band to be set active.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new490 Added in version 4.9.0
        public bool OpenBand(int bandIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the value indicating how the grid should be displayed.
        /// </summary>
        /// <remarks>This value will be used in AxMap.AddLayer and Grid.OpenAsImage methods.</remarks>
        /// \new491 Added in version 4.9.1
        public tkGridProxyMode PreferedDisplayMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Removes proxy image file created for grid display.
        /// </summary>
        /// <returns>True on success or if there is no image proxy and false if the operation failed.</returns>
        /// <remarks>Only the proxy format set in GlobalSettings.GridProxyFormat will be removed.</remarks>
        /// \new491 Added in version 4.9.1
        public bool RemoveImageProxy()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Retrieves colour scheme from the grid. 
        /// </summary>
        /// <param name="Method">Retrieval method. Use Auto to try all the available method.</param>
        /// <returns>Colour scheme object or null if the operation failed.</returns>
        /// <remarks>Is different from Grid.GenerateColorScheme, 
        /// as it get colour scheme already stored in the grid itself or as an external .mwleg file rather than creating a completely new one.</remarks>
        /// \new491 Added in version 4.9.1
        public GridColorScheme RetrieveColorScheme(tkGridSchemeRetrieval Method)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tries to retrieve any existing color scheme associated with grid or generates a new one if none is found.
        /// </summary>
        /// <param name="retrievalMethod">Colour scheme retrieval method.</param>
        /// <param name="generateMethod">Colour scheme generation method</param>
        /// <param name="Colors">Predefined set of colours to be mapped to grid values during generation process.</param>
        /// <returns>Color scheme or null if both retrieval and generation failed.</returns>
        /// \new491 Added in version 4.9.1
        public GridColorScheme RetrieveOrGenerateColorScheme(tkGridSchemeRetrieval retrievalMethod, tkGridSchemeGeneration generateMethod, 
            PredefinedColorScheme Colors)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the type of grid source, which defines a) whether any grid source is opened and b) if so, what the inner representation of grid by MapWinGIS.
        /// </summary>
        /// <remarks>The functionality and behaviour of various source types may differ.</remarks>
        /// \new490 Added in version 4.9.0
        public tkGridSourceType SourceType
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns an array with grid values which lie within specified bounds. Double overload of Grid.GetFloatWindow method.
        /// </summary>
        /// <remarks>For inner use or C++ use only. Only a single value will be returned to .NET.</remarks>
        /// <param name="StartRow">The start row of the window.</param>
        /// <param name="EndRow">The end row of the window.</param>
        /// <param name="StartCol">The start colour of the window.</param>
        /// <param name="EndCol">The end colour of the window.</param>
        /// <param name="Vals">A supposed array (only a single value in .NET).</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new491 Added in version 4.9.1
        public bool GetFloatWindow2(int StartRow, int EndRow, int StartCol, int EndCol, ref double Vals)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Provides the faster way to read the array values that are of a specific size. Double overload of Grid.GetRow.
        /// </summary>
        /// <remarks> The row is the integer row to read from the grid 
        /// object. The vals variable is actually the first element of the array of floats that you want to be populated with the values from 
        /// the grid. Since arrays are stored sequentially in memory, passing the first element allows the prediction of where the other 
        /// values must go. It is very important that you always dimension the array as being of type float, and always make sure that 
        /// you dimension it from 0 to numCols - 1. </remarks>
        /// <param name="Row">The Integer value of the row to retrieve values for.</param>
        /// <param name="Vals">Reference to the first element of the array of doubles that will hold the row of values. </param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new491 Added in version 4.9.1
        public bool GetRow2(int Row, ref double Vals)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        ///  Sets an array of values for a given region of the grid. Double overload of Grid.PutFloatWindow.
        /// </summary>
        /// <param name="StartRow">The start row of the window.</param>
        /// <param name="EndRow">The end row of the window.</param>
        /// <param name="StartCol">The start colour of the window.</param>
        /// <param name="EndCol">The end colour of the window.</param>
        /// <param name="Vals">A supposed array (only a single value in .NET).</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new491 Added in version 4.9.1
        public bool PutFloatWindow2(int StartRow, int EndRow, int StartCol, int EndCol, ref double Vals)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Provides The faster way to write the array values that are of a specific size. Double overload of Grid.PutRow.
        /// </summary>
        /// <remarks>
        /// The row is the integer row to read from the grid 
        /// object. The vals variable is actually the first element of the array of floats that you want to be populated with the values from 
        /// the grid. Since arrays are stored sequentially in memory, passing the first element allows the prediction of where the other values must go. It is very important that you always dimension the array as being of type float, and always make sure that 
        /// you dimension it from 0 to numCols - 1. 
        /// </remarks>
        /// <param name="Row">The Integer value of the row to retrieve values for.</param>
        /// <param name="Vals">Reference to the first element of the array of doubles that will hold the row of values.</param>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool PutRow2(int Row, ref double Vals)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif

