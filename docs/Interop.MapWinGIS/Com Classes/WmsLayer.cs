using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Represents a layer downloading and displaying the data from the particular WMS Server.
    /// </summary>
    /// \new494 Added in version 4.9.4
#if nsp
    #if upd
        public class WmsLayer : MapWinGIS.IWmsLayer
    #else        
        public class IWmsLayer        
    #endif
#else
    public class WmsLayer
#endif
    {
        /// <summary>
        /// Closes the layer.
        /// </summary>
        public void Close()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Serializes the state of the layer.
        /// </summary>
        /// <returns>The string with serialized state.</returns>
        public string Serialize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restores the state of the layer from the string.
        /// </summary>
        /// <param name="state">String with the previous state of the layer.</param>
        /// <returns>True on success.</returns>
        public bool Deserialize(string state)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the code of the last error which occurred inside this instance.
        /// </summary>
        public int LastErrorCode { get; private set; }

        /// <summary>
        /// Gets error message for the specific error code.
        /// </summary>
        /// <param name="ErrorCode">The error code.</param>
        /// <returns>The error message.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a key associated with this instance (any data can be stored in it by applications).
        /// </summary>
        public string Key { get; set; }

        /// <summary>
        /// Gets or sets the name of the layer.
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// Gets or sets the bounding box of the layer (server coordinate system).
        /// </summary>
        public Extents BoundingBox { get; set; }

        /// <summary>
        /// Gets or sets the EPSG code of the coordinate system to be used to request data from the server.
        /// </summary>
        public int Epsg { get; set; }

        /// <summary>
        /// Gets or sets the string with the layers to be requested from server (several comma separated layers are can be specified).
        /// </summary>
        public string Layers { get; set; }

        /// <summary>
        /// Gets or sets the base URL of the WMS layer.
        /// </summary>
        /// <remarks>For example, http://demo.lizardtech.com/lizardtech/iserv/ows. The other parts of the request will 
        /// be generated dynamically.</remarks>
        public string BaseUrl { get; set; }

        /// <summary>
        /// Gets or sets the unique identifier of the layer. It is used to cache the data locally.
        /// </summary>
        public int Id { get; set; }

        /// <summary>
        /// Gets or sets the format parameter of the request.
        /// </summary>
        public string Format { get; set; }

        /// <summary>
        /// Gets a value indicating whether this instance is initialized (bound to particular server).
        /// </summary>
        public bool IsEmpty { get; private set; }

        /// <summary>
        /// Gets the bounding box for the layer in map coordinates.
        /// </summary>
        public Extents MapExtents { get; private set; }

        /// <summary>
        /// Gets the projection of the layer.
        /// </summary>
        public GeoProjection GeoProjection { get; private set; }

        /// <summary>
        /// Gets or sets the opacity of the layer which is used during rendering.
        /// </summary>
        public byte Opacity { get; set; }

        /// <summary>
        /// Gets or sets the brightness of layer (color adjustment to be used during rendering).
        /// </summary>
        public float Brightness { get; set; }

        /// <summary>
        /// Gets or sets the contrast of layer (color adjustment to be used during rendering).
        /// </summary>
        public float Contrast { get; set; }

        /// <summary>
        /// Gets or sets the contrast of layer (color adjustment to be used during rendering).
        /// </summary>
        public float Saturation { get; set; }

        /// <summary>
        /// Gets or sets the hue of layer (color adjustment to be used during rendering).
        /// </summary>
        public float Hue { get; set; }

        /// <summary>
        /// Gets or sets the gamma of layer (color adjustment to be used during rendering).
        /// </summary>
        public float Gamma { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether data from the local cache will be used.
        /// </summary>
        public bool UseCache { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether the data will be cached locally.
        /// </summary>
        public bool DoCaching { get; set; }

        /// <summary>
        /// Gets or sets the color which be treated as transparent during rendering.
        /// </summary>
        public uint TransparentColor { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether transparent color should be used.
        /// </summary>
        public bool UseTransparentColor { get; set; }

        /// <summary>
        /// Gets or sets the version parameter which will be included in the request.
        /// </summary>
        public tkWmsVersion Version { get; set; }

        /// <summary>
        /// Gets or sets the styles parameter which will be included in the request.
        /// </summary>
        public string Styles { get; set; }
    }
#if nsp
}
#endif



