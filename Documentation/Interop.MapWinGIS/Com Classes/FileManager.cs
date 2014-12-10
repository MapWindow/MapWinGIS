using System;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Provides functionality for opening and examining different types of datasources.
    /// </summary>
    /// <remarks>
    /// Traditionally MapWinGIS uses at least 3 classes to open different types of datasources: Shapefile, Image and Grid.
    /// There are different ways to open GDAL-based raster datasources: they can be opened for editing by %Grid class or for rendering by %Image class.
    /// Also rendering of grids can be performed directly or by creating temporary proxy image. %FileManager was designed to hide all these details from 
    /// user providing a mechanism to open and display any type of datasource with least amount of code.\n
    /// 
    /// <b>A. Opening datasources.</b>\n
    /// 
    /// To open any supported datasource the following code can be used:
    /// \code
    /// var fm = new FileManager();
    /// var obj = fm.Open("some_filename", tkFileOpenStrategy.fosAutoDetect, null);
    /// \endcode
    /// Perhaps receiving an object of unknown type isn't very impressive, here is more practical example
    /// which adds open datasource to the map:
    /// \code
    ///  var fm = new FileManager();
    ///  var obj = fm.Open("some_filename", tkFileOpenStrategy.fosAutoDetect, null);
    ///  if (obj != null && fm.LastOpenIsSuccess)
    ///  {
    ///     int handle = axMap1.AddLayer(obj, true);
    ///     if (handle != -1)
    ///     {
    ///         MessageBox.Show("Layer was added to the map. Open strategy: " + fm.LastOpenStrategy.ToString());
    ///     }
    ///     else
    ///     {
    ///         MessageBox.Show("Failed to add layer to the map: " + axMap1.get_ErrorMsg(axMap1.LastErrorCode));
    ///     }
    ///  }
    ///  else
    ///  {
    ///     MessageBox.Show("Failed to open datasource: " + fm.get_ErrorMsg(fm.LastErrorCode));
    ///  }
    /// \endcode
    /// 
    /// FileManager.Open returns instance of Shapefile class for shapefile datasources and instance of Image class
    /// for any raster datasources both RGB images and grids. In case of grids it may temporarily open a Grid object
    /// to create proxy image for it but then closes it. To exclude casting of opened object to known type use: FileManager.OpenRaster and FileManager.OpenShapefile.
    /// The code below shows how to cast opened object to appropriate type. 
    /// \code
    /// string filename = "some_filename.ext";
    /// var fm = new FileManager();
    /// if (!fm.get_IsSupported(filename))
    /// {
    ///     MessageBox.Show("Datasource isn't supported by MapWinGIS");
    /// }
    /// else
    /// {
    ///     var obj = fm.Open(filename, tkFileOpenStrategy.fosAutoDetect, null);
    ///     if (fm.LastOpenIsSuccess)
    ///     {
    ///         if (fm.LastOpenStrategy == tkFileOpenStrategy.fosVectorLayer)
    ///         {
    ///             var shapefile = obj as Shapefile;
    ///             if (shapefile != null)
    ///                 MessageBox.Show("Shapefile was opened.");
    ///         }
    ///         else
    ///         {
    ///             var image = obj as Image;
    ///             if (image != null)
    ///                 MessageBox.Show("Image was opened.");
    ///         }
    ///     }
    ///     else
    ///     {
    ///         MessageBox.Show("Failed to open datasource: " + fm.get_ErrorMsg(fm.LastErrorCode));
    ///     }
    /// }
    /// \endcode
    /// 
    /// 
    /// \note Look for additional examples on opening raster datasources in description of Grid class.
    /// 
    /// <b>B. Open strategies.</b>\n
    /// 
    /// The following strategies are available through tkFileOpenStrategy enumeration:
    /// \dot
    /// digraph open_strategies {
    /// splines = true;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// render [ label="Open strategies"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" Generic. Any type:\l -fosAutoDetect;\l"];
    /// s2    [label=" Shapefiles:\l -fosVectorLayer;\l"];
    /// s3    [label=" RGB images:\l -fosRgbImage;\l"];
    /// s4    [label=" Grids:\l -fosDirectGrid;\l -fosProxyForGrid\l"];
    /// 
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" minlen=2 ]
    /// render -> s1;
    /// render -> s2;
    /// render -> s3;
    /// render -> s4;
    /// }
    /// \enddot
    /// See details on distinction between available open strategies for grids in description of Grid class.\n
    /// 
    /// 
    /// The default <b>tkFileOpenStrategy.fosAutoDetect</b> strategy works like this: \n\n
    /// 1) Shapefiles are checked by (.shp) extension and fosVectorLayer is chosen. \n\n
    /// 2) It is analyzed whether datasource can be opened with GDAL: 
    /// - if so and it is RGB image, fosRgbImage is chosen;
    /// - otherwise it is checked whether a proxy image exists for rendering this datasource,
    /// and whether its size exceeds GlobalSettings.MaxNoProxyGridSizeMb property; 
    /// if either is true fosProxyForGrid will be chosen;
    /// - otherwise fosDirectGrid will be returned.
    /// 
    /// 3) A check is made whether it can be handled by MapWinGIS own grid drivers - if so fosProxyForGrid will be returned.\n\n
    /// 4) If all previous methods have failed fosNotSupported will be returned.
    /// 
    /// In most cases fosAutoDetect strategy is the best choice. However as grids have 2 strategies
    /// it may be needed to choose on of them explicitly:
    /// \code
    /// string filename = "some_filename.ext";
    /// FileManager fm = new FileManager();
    /// var obj = fm.Open(filename, fm.IsGrid(filename) ? tkFileOpenStrategy.fosProxyForGrid: tkFileOpenStrategy.fosAutoDetect, null);
    /// \endcode
    /// 
    /// <b>C. Built-in file manager for map control.</b>\n
    /// 
    /// An instance of FileManager class is also used by AxMap.AddLayerFromFilename method, 
    /// which require only filename and open strategy to add layer to the map. Here is example of its usage:
    /// \code
    /// int handle = axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosAutoDetect, true);
    /// if (handle != -1 && axMap1.FileManager.LastOpenIsSuccess) 
    /// {
    ///     MessageBox.Show("Layer was added to the map. Open strategy: " + axMap1.FileManager.LastOpenStrategy.ToString());
    /// }
    /// else
    /// {
    ///     MessageBox.Show("Failed to open grid: " + axMap1.FileManager.ErrorMsgFromObject(axMap1.FileManager.LastErrorCode));
    /// }
    /// \endcode
    /// </remarks>
    /// \new491 Added in version 4.9.1
    #if nsp
        #if upd
            public class FileManager : MapWinGIS.IFileManager
        #else        
            public class IFileManager
        #endif
    #else
        public class FileManager
    #endif
    {
        /// <summary>
        /// Gets or sets a Callback object which handles progress and error messages.
        /// </summary>
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
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
        /// Gets filename of the last datasource that was attempted to be opened.
        /// </summary>
        public string LastOpenFilename
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a value indicating whether the last attempt to open a datasource was successful.
        /// </summary>
        public bool LastOpenIsSuccess
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a strategy that was used on last attempt to open datasource.
        /// </summary>
        public tkFileOpenStrategy LastOpenStrategy
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Tries to open datasource with specified name.
        /// </summary>
        /// <param name="Filename">Filename of datasource.</param>
        /// <param name="OpenStrategy">Strategy to be used with default recommended value fosAutoDetect.</param>
        /// <param name="cBack">Callback interface.</param>
        /// <remarks>Return value may be either Shapefile or Image object. The latter is used for rendering both RGB images and grids.
        /// Under the hood the function uses OpenShapefile and OpenRaster methods, so their behaviour will be consistent.</remarks>
        /// <returns>Opened datasource or null on failure.</returns>
        public object Open(string Filename, tkFileOpenStrategy OpenStrategy = tkFileOpenStrategy.fosAutoDetect, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tries to open raster datasource (either RGB image or grid) with specified name.
        /// </summary>
        /// <param name="Filename">Filename of datasource.</param>
        /// <param name="OpenStrategy">Strategy to be used with default recommended value fosAutoDetect. Three more strategies 
        /// are applicable here: fosRgbImage, fosDirectGrid, fosProxyForGrid.</param>
        /// <param name="cBack">Callback interface.</param>
        /// <returns>Image object or null on failure.</returns>
        public Image OpenRaster(string Filename, tkFileOpenStrategy OpenStrategy, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        ///  Tries to open shapefile with specified name.
        /// </summary>
        /// <param name="Filename">Filename of shapefile to open.</param>
        /// <param name="cBack">Callback interface.</param>
        /// <returns>Shapefile object or null on failure.</returns>
        public Shapefile OpenShapefile(string Filename, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Checks whether specific open strategy is applicable for a given datasource.
        /// </summary>
        /// <param name="Filename">Filename of datasource to open.</param>
        /// <param name="OpenStrategy">Open strategy to check.</param>
        /// <remarks>Inconsistent behaviour, i.e. return value is false while datasource is still can be opened with a given strategy, 
        /// should be reported as bugs. The opposite case, i.e. return value is true while datasource fails to open with given strategy, 
        /// may occur by design, but it may also be an indication of bugs in some other parts of MapWinGIS.</remarks>
        /// <returns>True in case strategy is applicable.</returns>
        public bool get_CanOpenAs(string Filename, tkFileOpenStrategy OpenStrategy)
        {
            throw new NotImplementedException();
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
        /// Checks whether specified datasource is grid, i.e. non-RGB raster datasource which 
        /// require synthetic colour scheme for rendering.
        /// </summary>
        /// <param name="Filename">Filename of datasource to be checked.</param>
        /// <returns>True in case datasource is grid.</returns>
        public bool get_IsGrid(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        ///  Checks whether datasource is RGB image.
        /// </summary>
        /// <param name="Filename"></param>
        /// <remarks>Attempt is made to open datasource with GDAL. If it succeeds, a check is made whether bands with Red, Green, Blue
        /// colour interpretation are present in it.</remarks>
        /// <returns>True if datasource is RGB image.</returns>
        public bool get_IsRgbImage(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Checks whether the datasource is supported (can be opened) by MapWinGis.
        /// </summary>
        /// <param name="Filename">Filename of datasource</param>
        /// <remarks>Under the hood open strategy will be determined with FileManager.get_OpenStrategy and then
        /// FileManager.get_CanOpenAs(strategy) is called.</remarks>
        /// <returns>True in case datasource can be opened.</returns>
        public bool get_IsSupported(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Checks whether datasource is supported by specified subsystem of MapWinGIS.
        /// </summary>
        /// <param name="Filename">Filename of datasource</param>
        /// <param name="supportType">The subsystem to choose (the list may be extended later on).</param>
        /// <returns>True in case the datasource is supported, i.e. can be opened.</returns>
        public bool get_IsSupportedBy(string Filename, tkSupportType supportType)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Checks whether specified datasource is vector layer that can be opened by MapWinGIS.
        /// </summary>
        /// <param name="Filename">Filename of datasource.</param>
        /// <remarks>Currently only shapefile format can be opened directly.</remarks>
        /// <returns>True in case datasource is vector one and is supported by MapWinGIS.</returns>
        public bool get_IsVectorLayer(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Determines optimal open strategy for datasource.
        /// </summary>
        /// <param name="Filename">Filename of datasource.</param>
        /// <returns>The optimal strategy to open datasource.</returns>
        public tkFileOpenStrategy get_OpenStrategy(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Opens a layer or runs a query against OGR spatial database.
        /// </summary>
        /// <remarks>This method will search if there is a layer with specified name in the datasource,
        /// and if so will open it. Otherwise it will run OgrDatasource.RunQuery method with provided
        /// layerNameOrQuery argument.</remarks>
        /// <param name="connectionString">Connection string. See details for particular formats <a href ="http://www.gdal.org/ogr_formats.html">here</a>.</param>
        /// <param name="layerNameOrQuery">Name of the layer (i.e. database table) or SQL query.</param>
        /// <returns>Opened layer or null on failure.</returns>
        /// \new493 Added in version 4.9.3
        public OgrLayer OpenFromDatabase(string connectionString, string layerNameOrQuery)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears overviews for a raster datasource managed by GDAL.
        /// </summary>
        /// <param name="Filename">Filename of datasource.</param>
        /// <returns>True on success.</returns>
        public bool ClearGdalOverviews(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Builds overviews for a raster datasource managed by GDAL.
        /// </summary>
        /// <param name="Filename">Filename of datasource.</param>
        /// <returns></returns>
        public bool BuildGdalOverviews(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Remove proxy images generated for grid rendering.
        /// </summary>
        /// <param name="Filename">Filename of grid datasource.</param>
        /// <returns></returns>
        public bool RemoveProxyForGrid(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns true if specified raster datasource managed by GDAL has overviews.
        /// </summary>
        /// <param name="Filename">Filename of datasource.</param>
        /// <returns>True if overviews are exist.</returns>
        public bool get_HasGdalOverviews(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns true if specified grid datasource has proxy images for its rendering.
        /// </summary>
        /// <param name="Filename">Filename of grid datasource.</param>
        /// <returns>True if proxy images exist.</returns>
        public bool get_HasValidProxyForGrid(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tries to open specified file as OGR datasource.
        /// </summary>
        /// <param name="Filename">Filename of the datasource to be opened.</param>
        /// <returns>Instance of OgrDatasource or null on failure.</returns>
        /// \new493 Added in version 4.9.3
        public OgrDatasource OpenVectorDatasource(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tries to open specified file as OGR layer.
        /// </summary>
        /// <remarks>When preferedShapeType is set to default ShpfileType.SHP_NULLSHAPE value or there is no layer 
        /// with preferedShapeType, then the first layer in datasource will be returned.</remarks>
        /// <param name="Filename">Filename of the datasource to be opened.</param>
        /// <param name="preferedShapeType">Sets type of layer to be returned from datasource in case there are multiple layers available.</param>
        /// <param name="forUpdate">True in case layer should be open in read/write mode.</param>
        /// <returns>Instance of OgrLayer or null on failure.</returns>
        /// \new493 Added in version 4.9.3
        public OgrLayer OpenVectorLayer(string Filename, ShpfileType preferedShapeType = ShpfileType.SHP_NULLSHAPE, bool forUpdate = false)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif

