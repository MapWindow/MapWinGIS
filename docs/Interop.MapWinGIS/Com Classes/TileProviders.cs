#if nsp
namespace MapWinGIS
{
#endif
    using System;
    using MapWinGIS;

    /// <summary>
    /// Holds the list of tile providers associated with %Tiles class.
    /// </summary>
    /// \new490 Added in version 4.9.0
#if nsp
    #if upd
        public class TileProviders : MapWinGIS.ITileProviders
    #else        
        public class ITileProviders
    #endif
#else
    public class TileProviders
#endif
    {
        /// <summary>
        /// Adds a custom tile provider.
        /// </summary>
        /// <param name="Id">Unique ID to to access this provider later on (including caching).</param>
        /// <param name="Name">Arbitrary name of provider.</param>
        /// <param name="urlPattern">Url pattern for provider. The pattern may include the following varying components: 
        /// {zoom}, {x}, {y}, {switch:n1,n2,n3}. For example, the following can be used to setup OpenStreetMap 
        /// as custom provider: "http://{switch:a,b,c}.tile.openstreetmap.org/{zoom}/{x}/{y}.png".</param>
        /// <param name="Projection">Projection used by tile server. Projection must not necessarily match the map projection.
        /// MapWinGIS will try to do the transformation even if projection don't match, though these may lead to 
        /// distortions and gaps between tiles.</param>
        /// <param name="minZoom">Minimum zoom level provided by server.</param>
        /// <param name="maxZoom">Maximum zoom level provided by server.</param>
        /// <returns>True on success and false otherwise</returns>
        public bool Add(int Id, string Name, string urlPattern, tkTileProjection Projection, int minZoom = 0, int maxZoom = 17)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes custom providers from the list.
        /// </summary>
        /// <param name="ClearCache">True in case cached tiles, both from RAM and disk, for custom providers should be removed as well.</param>
        public void Clear(bool ClearCache)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of providers in the list.
        /// </summary>
        public int Count
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Removes specified custom provider from the list.
        /// </summary>
        /// <param name="ProviderId">Id of provider to be removed.</param>
        /// <param name="ClearCache">True in case cached tiles should be removed as well.</param>
        /// <returns>True if specified provider was removed, and false otherwise.</returns>
        /// <remarks>It's not allowed to remove default providers.</remarks>
        public bool Remove(int ProviderId, bool ClearCache)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets provider ID by its index in the list.
        /// </summary>
        /// <param name="Index">The index of provider in the list.</param>
        /// <returns>ID of the provider.</returns>
        /// <remarks>For default providers the index in the list and ID are the same. For custom providers any 
        /// unique ID can be specified on creation, which may not match the index in the list.</remarks>
        public int get_Id(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the index of default provider in the list.
        /// </summary>
        /// <param name="Provider">Provider to find index for.</param>
        /// <returns>The index of provider in the list.</returns>
        public int get_IndexByProvider(tkTileProvider Provider)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the index of provider in the list by its ID.
        /// </summary>
        /// <param name="ProviderId">Id of the provider.</param>
        /// <returns>The index of provider in the list.</returns>
        public int get_IndexByProviderId(int ProviderId)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a value indicating whether given provider is a custom one.
        /// </summary>
        /// <param name="Index">Index of provider in the list.</param>
        /// <returns>True if provider is custom, false if it is a default one.</returns>
        public bool get_IsCustom(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets name of the specific provider.
        /// </summary>
        /// <param name="Index">Index of the provider in the list.</param>
        /// <returns>Name of the provider.</returns>
        public string get_Name(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets name of the specific providers.
        /// </summary>
        /// <param name="Index">Index of the provider in the list.</param>
        /// <param name="newVal">New name of the provider.</param>
        /// <remarks>Providers are cached by ID therefore renaming of provider won't affect its cache.</remarks>
        public void set_Name(int Index, string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets server projection for specified provider.
        /// </summary>
        /// <param name="Index">Index of the provider in the list.</param>
        /// <returns>Server projection.</returns>
        public tkTileProjection get_Projection(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Get a version string for specified provider.
        /// </summary>
        /// <param name="Index">Index of the provider in the list.</param>
        /// <returns>Version string.</returns>
        /// <remarks>Many tile servers include version as a parameter of HTTP request. 
        /// Tiles may not load if the obsolete version is passed.</remarks>
        public string get_Version(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a version string for specified provider.
        /// </summary>
        /// <param name="Index">Index of the provider in the list.</param>
        /// <param name="retVal">Version string.</param>
        public void set_Version(int Index, string retVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets maximum zoom level provided by tile server.
        /// </summary>
        /// <param name="Index">Index of the provider in the list.</param>
        /// <returns>Maximum zoom.</returns>
        /// <remarks>Zoom level is hardcoded for default providers and passed as a parameter 
        /// on creation for custom providers. Server isn't queried to check whether the values are correct.</remarks>
        public int get_MaxZoom(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets minimum zoom level provided by tile server.
        /// </summary>
        /// <param name="Index">Index of the provider in the list.</param>
        /// <returns>Minimum zoom.</returns>
        public int get_MinZoom(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets URL pattern for custom provider.
        /// </summary>
        /// <param name="Index">Index of the provider in the list.</param>
        /// <returns>URL pattern for custom providers and empty string for default ones.</returns>
        public string get_UrlPattern(int Index)
        {
            throw new NotImplementedException();
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
        /// Gets language settings for a given provider.
        /// </summary>
        /// <param name="Index">Index of provider in list.</param>
        /// <remarks>The setting will be included in http request and if supported by tile provider 
        /// it may actually change language information is displayed on map with.</remarks>
        /// <returns>Language settings, e.g. "en", "fr", "ru", etc. "en" is used by default.</returns>
        /// \new491 Added in version 4.9.1
        public string get_Language(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets language settings for a given provider.
        /// </summary>
        /// <param name="Index">Index of provider in list.</param>
        /// <param name="newVal">Language settings, e.g. "en", "fr", "ru", etc.</param>
        /// <remarks>The setting will be included in http request and if supported by tile provider 
        /// it may actually change language information is displayed on map with.</remarks>
        /// \new491 Added in version 4.9.1
        public void set_Language(int Index, string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the geographic bounds of the specified provider.
        /// </summary>
        /// <param name="Index">The index of the provider.</param>
        /// <returns>Geographic bounds in decimal degrees.</returns>
        /// \new494 Added in version 4.9.4
        public Extents get_GeographicBounds(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the geographic bounds of the specified provider.
        /// </summary>
        /// <param name="Index">The index.</param>
        /// <param name="pVal">The p value.</param>
        /// \new494 Added in version 4.9.4
        public void set_GeographicBounds(int Index, Extents pVal)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif
