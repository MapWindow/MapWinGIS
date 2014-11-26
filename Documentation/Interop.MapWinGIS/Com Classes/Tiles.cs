using System;
using MapWinGIS;

#if nsp
namespace MapWinGIS
{
#endif
    /// \anchor a_tiles
    /// <summary>
    /// Downloads and displays data from online TMS tile services, like OpenStreetMap. 
    /// </summary>
    /// \attention MapWinGIS doesn't grant you any legal rights to use data from 
    /// particular online services. Please look for license information on acceptable usages of the data 
    /// on official sites of these services.
    /// 
    /// Online tile services provide 2D maps of Earth surface, which are split into small parts (usually 256 by 256 pixels) called tiles. Tiles are 
    /// provided at a number of zoom levels with greater detail on each subsequent level. Typically there are about 15-20 zoom levels with just 4 tiles 
    /// for the whole Earth surface on the first one, and 4^zoom on subsequent levels. Depending on service the maps can be:
    /// - synthetic: features of Earth surface are displayed with some arbitrary set of symbols (OpenStreetMap, GoogleMaps);
    /// - satellite: satellite imagery (Bing satellite, Google satellite);
    /// - hybrid: labels and objects displayed on top of satellite map.
    /// 
    /// To get more information on tiles services see specification: <a href="http://en.wikipedia.org/wiki/Tile_Map_Service">here</a>.\n
    /// 
    /// <b>A. Coordinate system and projection.</b>\n\n
    /// In order to display tiles from online datasource the coordinate system & projection of server must match or at least be compatible with those 
    /// settings for the map control. Three scenarios are possible:
    /// \dot
    /// digraph tiles_projection {
    /// splines = true;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// render [ label="Tiles projection scenarios"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" Projections match\l"];
    /// s2    [label=" Projections are compatible\l"];
    /// s3    [label=" Not compatible or no projection for the map\l"];
    /// 
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" minlen=2 ]
    /// render -> s1;
    /// render -> s2;
    /// render -> s3;
    /// }
    /// \enddot
    /// \n
    /// <div style="margin-left: 20px">
    /// 1) Projections match: tiles will be displayed in their native projection without distortions.\n\n
    /// 2) Projections don't match but transformation from server projection to map projection is possible: tiles will be displayed with distortions, 
    /// which may be acceptable in some cases (spherical Mercator server projection, WGS84 map coordinate system) or hardly usable in others (spherical Mercator 
    /// server projection, UTM projection of WGS84 as map projection).\n\n
    /// 3) No projections for map is set or map/server projections doesn't match and there is no transformation between them: tiles can't be displayed.\n
    /// </div>
    /// Here are screenshots which illustrate types of distortions introduced by projection mismatch
    /// (server projection is Spherical Mercator).
    /// 
    /// <div align = "left">
    /// <table cellspacing="0" cellpadding="0, 20" style="border: none; width: 900px">
    /// <tr align = "center">
    ///     <td>\image html tiles_wgs84.jpg</td>
    ///     <td>\image html tiles_utm16.jpg</td>
    /// </tr>
    /// <tr>
    ///     <td align = "center">Spherical Mercator -> WGS84</td>
    ///     <td align = "center">Spherical Meractor -> WGS84 UTM16</td>
    /// </tr>
    /// </table>
    /// </div>
    /// To check whether map projection match the server one use Tiles.ProjectionStatus property.\n\n
    /// Most tile servers like OpenStreetMap, GoogleMaps, etc. are using spherical Mercator projection (aka Google Mercator; EPSG 3857). Therefore in 
    /// most cases it may be set without additional checks:
    /// \code
    /// axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
    /// \endcode
    /// 
    /// More generalized code to initialize map control with projection used by tile server is:
    /// \code
    /// var ut = new Utils();
    /// tkTileProjection pr = axMap1.Tiles.ServerProjection;
    /// GeoProjection gp = ut.TileProjectionToGeoProjection(pr);
    /// axMap1.GeoProjection = gp;
    /// Debug.Print("Projection was set: " + axMap1.GeoProjection.Name);
    /// Debug.Print("Tiles projection status: " + axMap1.Tiles.ProjectionStatus.ToString());
    /// //if all went was tpsNative status will be returned, i.e. rendering in native projection
    /// \endcode
    /// 
    /// <b>B. Providers.</b>\n
    /// 
    /// MapWinGIS supports built-in default providers and custom providers.
    /// \dot
    /// digraph caching_types {
    /// splines = true;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// render [ label="Tile providers"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" Default built-in\l"];
    /// s2    [label=" Custom configured by user\l"];
    /// 
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" minlen=2 ]
    /// render -> s1;
    /// render -> s2;
    /// }
    /// \enddot
    /// 
    /// MapWinGIS has a number of built-in providers defined in tkTileProvider enumeration. To activate one of those use:
    /// \code
    /// axMap1.Tiles.Provider = tkTileProvider.OpenCycleMap;
    /// // or a shorter version
    /// axMap1.TileProvider = tkTileProvider.OpenCycleMap;
    /// \endcode
    /// The default provider is OpenStreetMap.
    /// 
    /// To display list of available providers and their settings:
    /// \code
    /// TileProviders providers = axMap1.Tiles.Providers;
    /// for (int i = 0; i < providers.Count; i++)
    /// {
    ///     Debug.WriteLine("Provider name: " + providers.get_Name(i));
    ///     Debug.WriteLine("Provider type: " + (providers.get_IsCustom(i) ? "custom" : "default"));
    ///     Debug.WriteLine("Min zoom: " + providers.get_MinZoom(i));
    ///     Debug.WriteLine("Min zoom: " + providers.get_MinZoom(i));
    ///     Debug.WriteLine("-----------------------------------");
    /// }
    /// \endcode
    /// 
    /// To add a custom provider to the list (in this example OpenStreetMap is configured as custom provider):
    /// \code
    /// TileProviders providers = axMap1.Tiles.Providers;
    /// int providerId = (int)tkTileProvider.ProviderCustom + 1;    // (1024 + 1) should be unique across application runs in case disk caching is used
    /// providers.Add(providerId, "Custom TMS provider",
    /// "http://{switch:a,b,c}.tile.openstreetmap.org/{zoom}/{x}/{y}.png",
    /// tkTileProjection.SphericalMercator, 0, 18);
    /// \endcode
    /// The following placeholders can be used:
    /// - "{zoom}", "{x}", "{y}" will be substituted with value for particular tiles to form HTTP request;
    /// - "{switch:a,b,c}" will randomly choose one of the comma separated values, thus switching between URLs of servers.
    /// 
    /// To set this provider as an active one:
    /// \code
    /// axMap1.Tiles.ProviderId = providerId;
    /// // the same property can be used to set default providers as well
    /// // because enumerated constant values match their id:
    /// axMap1.Tiles.ProviderId = (int)tkTileProvider.OpenStreetMap;
    /// \endcode
    /// 
    /// To retrieve custom provider already added to the list:
    /// \code
    /// int providerId = 1025;       // id of provider passed to TileProviders.Add method
    /// TileProviders providers = axMap1.Tiles.Providers;
    /// int index = providers.get_IndexByProviderId(providerId);
    /// Debug.WriteLine("The recently added custom provider is: " + providers.get_Name(index));
    /// \endcode
    /// It's necessary to distinguish unique providerId passed to TileProviders.Add method and used by caching mechanism, 
    /// and index of provider in TileProviders collection which may differ across application runs. 
    /// To get index in collection by providerId use TileProviders.get_IndexByProviderId.
    /// 
    /// <b>C. Zoom levels.</b>\n\n
    /// Originally MapWinGIS doesn't support concept of discrete zoom levels and can display any arbitrary zoom level between those defined by tile 
    /// service. This will lead to scaling of tiles, i.e. separate images will be drawn
    /// using size other than their original (256 by 256 pixels), which may decrease their readability. To force MapWinGIS to display tiles without 
    /// scaling set:
    /// \code
    /// axMap1.ZoomBehavior = zmUseTileLevels;
    /// \endcode
    /// Map can also be zoomed to particular tile level:
    /// \code
    /// axMap1.ZoomToTileLevel(10);
    /// // or 
    /// axMap1.CurrentZoom = 10;    // available from Form designer
    /// \endcode
    /// Zoom levels supported by active provider can be accessed via Tiles.MinZoom, Tiles.MaxZoom properties:
    /// \code
    /// Debug.WriteLine(string.Format("Provider {0} supports zoom levels from {1} to {2}", axMap1.Tiles.Provider.ToString(), 
    ///                                axMap1.Tiles.MinZoom, axMap1.Tiles.MaxZoom));
    /// \endcode
    /// Available zoom levels for other providers can be retrieved via: TileProviders.get_MaxZoom, TileProviders.get_MinZoom.
    /// Visibility of data layers displayed on the map can set for specific zoom levels only with: AxMap.set_LayerMaxVisibleZoom, 
    /// AxMap.set_LayerMaxVisibleZoom.
    /// 
    /// <b>D. Caching.</b>\n\n
    /// To speed up the display and reduce network traffic it's common to use caching for tiles, i.e. save them locally for further reuse.
    /// MapWinGIS supports 2 types of tile cache:
    /// - RAM cache: tiles are stored in memory (is on by default);
    /// - disk cache: tiles are stored in SQLite database (by default existing database is used but no new tiles are written to it).
    /// 
    /// \dot
    /// digraph caching_types {
    /// splines = true;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node  [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// render [ label="Caching types"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" RAM caching\l"];
    /// s2    [label=" Disk caching (SQLite)\l"];
    /// 
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" minlen=2 ]
    /// render -> s1;
    /// render -> s2;
    /// }
    /// \enddot
    /// 
    /// To enable cache usage:
    /// \code
    /// axMap1.Tiles.set_UseCache(tkCacheType.Disk, true);  // is on by default
    /// axMap1.Tiles.set_UseCache(tkCacheType.RAM, true);   // is on by default
    /// \endcode
    /// 
    /// To make MapWinGIS add newly downloaded tiles to cache:
    /// \code
    /// axMap1.Tiles.set_DoCaching(tkCacheType.RAM, true);  // is on by default
    /// axMap1.Tiles.set_DoCaching(tkCacheType.Disk, true); // if off by default
    /// \endcode
    /// 
    /// At this point new SQLite database will created to store tiles if it wasn't created so far. Default filename is mwtiles.db3. 
    /// Location - the folder with application executable. To change location of disk cache use Tiles.DiskCacheFilename property.
    /// \code
    /// axMap1.Tiles.DiskCacheFilename = "D:\tiles_cache.db3";
    /// \endcode
    /// If no file with such name exists it will be created automatically. Only one database can be used at a time.\n
    /// 
    /// For both types of cache there is built-in mechanism to limit its size, under exceeding of which the oldest tiles will be removed to give space 
    /// for the more recent ones:
    /// \code
    /// axMap1.Tiles.set_MaxCacheSize(tkCacheType.RAM, 200.0);    // size in MB; 100 by default
    /// axMap1.Tiles.set_MaxCacheSize(tkCacheType.Disk, 500.0);    // size in MB; 100 by default
    /// \endcode
    /// 
    /// To get information about the size and content of cache use: Tiles.get_CacheSize, Tiles.get_DiskCacheCount. To clear all the cache or only data for 
    /// particular zoom levels and providers use: Tiles.ClearCache, Tiles.ClearCache2.\n
    /// 
    /// <b>E. Functioning.</b>\n\n
    /// If map projection is compatible with server tiles are loaded automatically when map extents change and Tiles.Visible property is set to true. 
    /// AxMap.TilesLoaded event is fired when all tiles for particular extents are loaded. In case all tiles were found in cache the event will fire
    /// immediately, otherwise - after execution of HTTP requests. If new extents were set for map before all HTTP request for previous set of tiles
    /// were completed, AxMap.TilesLoaded event won't fire, tiles already requested will be added to the cache, but other pending requests will be discarded.
    /// Internally to queue HTTP requests 2 thread pools are used. To identify potential problems HTTP requests can logged into file with 
    /// Tiles.StartLogRequests method.\n\n
    /// The behaviour of tile services depend upon how server and map projection are related. Several cases are possible:\n\n
    /// 1. Server and map projections are the same.\n
    /// - tiles are displayed without scaling using original size (256 by 256 pixels);
    /// - snapping to discrete zoom levels works on any change of extents (if AxMap.ZoomBehavior = zbTileLevels);
    /// - examples: OpenStreetMap tiles and Google Mercator projection of the map.
    /// 
    /// 2. Special case of: server projection is GMercator, map projection - WGS84.\n
    /// - width of tiles will match the original, height will be equal (on equator) or smaller than original;
    /// - snapping to discrete zoom levels works on any change of extents (if AxMap.ZoomBehavior = zbTileLevels);
    /// - no gaps between tiles.
    /// 
    /// 3. Server projection is local one (like Amersfoort/RD New), server projection is different:\n
    /// - size of tiles will be adjusted to be as close to original as possible, but will vary for individual tiles;
    /// - gaps between tiles, which gradually become smaller at close zoom levels;
    /// - no automatic snapping to discrete zoom levels on changing extents. 
    /// 
    /// 4. Server projection is GMercator, map projection is some sort of local projection (like WGS84 UTM zones):\n
    /// - the same behaviour as previous plus the following limitation;
    /// - the smallest zoom levels (1-5) aren't available (can be changed with GlobalSettings.TilesMinZoomOnProjectionMismatch).\n\n
    /// Instance of Tiles class associated with AxMap control is accessible via AxMap.Tiles property which is read-only.
    /// \new490 Added in version 4.9.0
#if nsp
    #if upd
        public class Tiles : MapWinGIS.ITiles
    #else        
        public class ITiles
    #endif
#else
    public class Tiles
#endif
    {
        /// <summary>
        /// Retrieves and applies proxy server settings (address and port) from IE settings
        /// </summary>
        /// <returns>True on success and false otherwise</returns>
        public bool AutodetectProxy()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sends HTTP request to the specified address and reports back whether there is a response.
        /// </summary>
        /// <param name="url">The URL to send request to.</param>
        /// <returns>True if there is a connection (even if there is and error code like 404 in response) and false otherwise.</returns>
        public bool CheckConnection(string url)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears cache of the specified type.
        /// </summary>
        /// <param name="cacheType">Type of cache to be cleared.</param>
        public void ClearCache(tkCacheType cacheType)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears cache of the specified to type for a given provider and scales.
        /// </summary>
        /// <param name="cacheType">Type of cache to be cleared.</param>
        /// <param name="Provider">Tile provider to be cleared. ProviderNone will clear tiles for all providers.</param>
        /// <param name="fromScale">Minimal scale (zoom) to clear tiles for.</param>
        /// <param name="toScale">Maximum scale (zoom) to clear tiles for.</param>
        public void ClearCache2(tkCacheType cacheType, tkTileProvider Provider, int fromScale = 0, int toScale = 100)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Gets zoom (scale) of tiles currently displayed on the map.
        /// </summary>
        /// <remarks>Tile zooms are discrete, but the scale for Map control is continuous. 
        /// Therefore in most cases tiles of particular zoom will be somewhat additionally scaled to fit the map, 
        /// i.e. their display size on map won't be equal to the original 256 pixels.</remarks>
        public int CurrentZoom
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Restores the state of Tiles class from string.
        /// </summary>
        /// <param name="newVal">Serialized string generated by Tiles.Serialize method.</param>
        public void Deserialize(string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the filename of SQLite database to cache tiles into.
        /// </summary>
        public string DiskCacheFilename
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the bounds of specific tile in decimal degrees (for inner use/debug purposes).
        /// </summary>
        /// <param name="ProviderId">Id of the provider.</param>
        /// <param name="zoom">Zoom level for a tile.</param>
        /// <param name="tileX">X coordinate of the tile within zoom level.</param>
        /// <param name="tileY">Y coordinate of the tile within zoom level.</param>
        /// <returns>Bounds in decimal degrees or null on failure.</returns>
        public Extents GetTileBounds(int ProviderId, int zoom, int tileX, int tileY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets indices of tiles within specified bounds in coordinates of the given zoom level.
        /// </summary>
        /// <param name="boundsDegrees">Bounds in decimal degrees.</param>
        /// <param name="zoom">Zoom level.</param>
        /// <param name="Provider">Id of the provider.</param>
        /// <returns>Extents object with tile bounds or null on failure.</returns>
        /// <remarks>Can be used at the first step of prefetching operation.</remarks>
        public Extents GetTilesIndices(Extents boundsDegrees, int zoom, int Provider)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a value indicating whether a grid should be drawn to displayes borders of tiles (for debug purposes).
        /// </summary>
        public bool GridLinesVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the maximum zoom (scale) to be cached if the automatic caching is chosen.
        /// </summary>
        /// <remarks>The setting won't affect tile prefetching.</remarks>
        public int MaxScaleToCache
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the minimum zoom (scale) to be cached if the automatic caching is chosen.
        /// </summary>
        /// <remarks>The setting won't affect tile prefetching.</remarks>
        public int MinScaleToCache
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Caches tiles of the specified provider within specified geographic bounds 
        /// to SQLite database for further offline use.
        /// </summary>
        /// <param name="minLat">Minimal latitude to cache within.</param>
        /// <param name="maxLat">Maximum latitude to cache within.</param>
        /// <param name="minLng">Minimal longitude to cache within.</param>
        /// <param name="maxLng">Maximum longitude to cache within.</param>
        /// <param name="zoom">Zoom level.</param>
        /// <param name="ProviderId">Id of the provider.</param>
        /// <param name="stop">StopExecution interface implementation to stop the operation prematurely.</param>
        /// <returns>The number of tiles scheduled for caching.</returns>
        /// <remarks>The operation is executed asynchronously. To get the progress information use Tiles.GlobalCallback property.
        /// The callback will returned the number of tiles already downloaded. When operation is completed, -1 will be returned.</remarks>
        public int Prefetch(double minLat, double maxLat, double minLng, double maxLng, int zoom, int ProviderId, IStopExecution stop)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        ///  Caches tiles of the specified provider to SQLite database for further offline use.
        /// </summary>
        /// <param name="minX">Minimum X index of tile to be cached (in coordinates of tile zoom level).</param>
        /// <param name="maxX">Maximum X index of tile to be cached (in coordinates of tile zoom level).</param>
        /// <param name="minY">Minimum Y index of tile to be cached (in coordinates of tile zoom level).</param>
        /// <param name="maxY">Maximum Y index of tile to be cached (in coordinates of tile zoom level).</param>
        /// <param name="zoom">Zoom level to be cached.</param>
        /// <param name="ProviderId">Id of the provider.</param>
        /// <param name="stop">StopExecution interface implementation to stop the operation prematurely.</param>
        /// <returns>Number of tiles scheduled for caching.</returns>
        /// <remarks>The operation is executed asynchronously. See details in Tiles.Prefetch.</remarks>
        public int Prefetch2(int minX, int maxX, int minY, int maxY, int zoom, int ProviderId, IStopExecution stop)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Caches tiles to the specified file system folder for further offline use.
        /// </summary>
        /// <param name="ext">Extents to cache within in decimal degrees.</param>
        /// <param name="zoom">Zoom level.</param>
        /// <param name="ProviderId">Id of the provider.</param>
        /// <param name="savePath">Directory to save files into. Nested folders for zoom levels, 
        /// X/Y coordinates will be created automatically.</param>
        /// <param name="fileExt">File extension to store tiles with.</param>
        /// <param name="stop">StopExecution interface implementation to stop the operation prematurely.</param>
        /// <returns>Number of tiles scheduled for caching.</returns>
        /// <remarks>Often tiles are saved with fictitious extension, like png.tile, 
        /// to avoid their listing in the gallery of smartphones for example. Tile consuming application often may be configured
        /// to handle such extensions.</remarks>
        /// <remarks>The operation is executed asynchronously. See details in Tiles.Prefetch.</remarks>
        public int PrefetchToFolder(Extents ext, int zoom, int ProviderId, string savePath, string fileExt, IStopExecution stop)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets active provider to serve the tiles.
        /// </summary>
        /// <remarks>The property will return ProviderCustom for custom provider and 
        /// won't be able to set a custom provider (use Tiles.ProviderId to do it).</remarks>
        public tkTileProvider Provider
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets active provider to server the tiles. Ids of both default and custom providers can be specified.
        /// </summary>
        public int ProviderId
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the name of the active provider.
        /// </summary>
        public string ProviderName
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets list of the available default and custom tile providers.
        /// </summary>
        public TileProviders Providers
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets proxy server settings for tiles class including IP and port, e.g. 192.168.0.1:80.
        /// </summary>
        public string Proxy
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Serializes the state of the Tiles class to be restored later with Tiles.Deserialize method.
        /// </summary>
        /// <returns>Serialized string.</returns>
        public string Serialize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Set proxy server for tiles class.
        /// </summary>
        /// <param name="address">IP address of proxy.</param>
        /// <param name="port">Port to be used.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool SetProxy(string address, int port)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the amount of time in milliseconds a worker thread will sleep before the next HTTP request.
        /// </summary>
        /// <remarks>In some cases rapid caching of tiles can enact certain blocking logic for the IP address by server or one of the proxies.</remarks>
        /// \new491 Added in version 4.9.1
        public int SleepBeforeRequestTimeout
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a value indicating whether server requests will be made to get the tiles.
        /// </summary>
        /// <remarks>When set to false tiles will only be loaded from cache.</remarks>
        public bool UseServer
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a value indicating whether the tiles will be displayed on the map.
        /// </summary>
        /// <remarks>When set to false tiles won't be requested either from server or cache.</remarks>
        public bool Visible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the current size of cache.
        /// </summary>
        /// <param name="cacheType">The type of cache to return size for.</param>
        /// <returns>The size of cache in MB.</returns>
        public double get_CacheSize(tkCacheType cacheType)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Gets the current size of cache used for specific provider and zoom level.
        /// </summary>
        /// <param name="cacheType">The type of cache to return size for.</param>
        /// <param name="Provider">Provider. ProviderNone will return size for all providers.</param>
        /// <param name="scale">Scale (zoom) level. -1 will return size for all zoom levels.</param>
        /// <returns>The size of cache in MB.</returns>
        public double get_CacheSize2(tkCacheType cacheType, tkTileProvider Provider = tkTileProvider.ProviderNone, int scale = -1)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value indicating whether tiles requested from server will be automatically cached.
        /// </summary>
        /// <param name="cacheType">The type of cache to be used.</param>
        /// <returns>True if automatic caching is on.</returns>
        public bool get_DoCaching(tkCacheType cacheType)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the value indicating whether tiles requested from server will be automatically cached.
        /// </summary>
        /// <param name="cacheType">The type of cache to be used.</param>
        /// <param name="pVal">True to turn automatic caching on.</param>
        public void set_DoCaching(tkCacheType cacheType, bool pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the maximum allowable size of cache.
        /// </summary>
        /// <param name="cacheType">The type of cache.</param>
        /// <returns>Maximum size in MB.</returns>
        public double get_MaxCacheSize(tkCacheType cacheType)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Sets the maximum allowable size of cache.
        /// </summary>
        /// <param name="cacheType">The type of cache.</param>
        /// <param name="pVal">Maximum size in MB.</param>
        public void set_MaxCacheSize(tkCacheType cacheType, double pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a value indicating whether specific type of cache should be used to display tiles.
        /// </summary>
        /// <param name="cacheType">The type of cache.</param>
        /// <returns>True if the specific type of cache can be used.</returns>
        public bool get_UseCache(tkCacheType cacheType)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a value indicating whether specific type of cache should be used to display tiles.
        /// </summary>
        /// <param name="cacheType">The type of cache.</param>
        /// <param name="pVal">True if the specific type of cache can be used.</param>
        public void set_UseCache(tkCacheType cacheType, bool pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a value indicating whether log will include all HTTP requests or only unsuccessful ones.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public bool LogErrorsOnly
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets filename to write log into.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public string LogFilename
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a value indicating whether logging of HTTP requests will be performed.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public bool LogIsOpened
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Starts logging HTTP requests for tile server.
        /// </summary>
        /// <param name="Filename">Filename to write log into. New file will be created any existing file - overwritten.</param>
        /// <param name="errorsOnly">Indicate whether only unsuccessful requests should logged.</param>
        /// <returns>True if log was opened, and false on failure.</returns>
        /// \new491 Added in version 4.9.1
        public bool StartLogRequests(string Filename, bool errorsOnly = false)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Stops logging of HTTP requests to a file.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public void StopLogRequests()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of unsuccessful HTTP requests during prefetching operation (Tiles.Prefetch and overloads).
        /// </summary>
        /// \new491 Added in version 4.9.1
        public int PrefetchErrorCount
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the total number of requests carried out during prefetching operation.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public int PrefetchTotalCount
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Clears information about prefetching errors.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public void ClearPrefetchErrors()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets number of tiles stored in disk cache for a given provider, zoom and region.
        /// </summary>
        /// <param name="Provider">Id of provider.</param>
        /// <param name="zoom">Zoom level.</param>
        /// <param name="xMin">Min X index of tile.</param>
        /// <param name="xMax">Max X index of tile.</param>
        /// <param name="yMin">Min Y index of tile.</param>
        /// <param name="yMax">Max Y index of tile.</param>
        /// <returns>Number of tiles.</returns>
        public int get_DiskCacheCount(int Provider, int zoom, int xMin, int xMax, int yMin, int yMax)
        {
            throw new NotImplementedException();
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
        /// Gets the description of the specific error code.
        /// </summary>
        /// <param name="ErrorCode">The error code returned by LastErrorCode property.</param>
        /// <returns>String with the description.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets maximum available zoom level for the current provider.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public int MaxZoom
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets minimum available zoom level for the current provider.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public int MinZoom
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Get information about whether map projection and tile projection match.
        /// </summary>
        /// <remarks>This diagnostic value indicates whether tiles will be rendered without scaling and distortions and if they will be rendered at all.</remarks>
        /// \new491 Added in version 4.9.1
        public tkTilesProjectionStatus ProjectionStatus
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets projection used by specific tile service.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public tkTileProjection ServerProjection
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Clears user name and password set by Tiles.SetProxyAuthorization method.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public void ClearProxyAuthorization()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets credentials for proxy authorization. 
        /// </summary>
        /// <remarks>HTTP client will use this credentials when 401 or 407 response is received for original request:
        /// http://msdn.microsoft.com/en-us/library/7shxzbwb%28v=vs.80%29.aspx</remarks>
        /// <param name="username">Username.</param>
        /// <param name="password">Password.</param>
        /// <param name="domain">Domain name.</param>
        /// <returns>True on success.</returns>
        /// \new493 Added in version 4.9.3
        public bool SetProxyAuthorization(string username, string password, string domain)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif
