using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Represents a single GDAL driver.
    /// Please refer to GDAL documentation for more details information.
    /// </summary>
    /// \new494 Added in version 4.9.4
#if nsp
    #if upd
        public class GdalDriver : MapWinGIS.IGdalDriver
    #else        
        public class IGdalDriver        
    #endif
#else
    public class GdalDriver
#endif
    {
        /// <summary>
        /// Gets particular metadata associated with the driver.
        /// </summary>
        /// <param name="Metadata">The metadata.</param>
        /// <returns>String with metadata.</returns>
        public string get_Metadata(tkGdalDriverMetadata Metadata)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the the number of metadata items associated with the driver.
        /// </summary>
        public int MetadataCount { get; private set; }

        /// <summary>
        /// Gets specified metadata item.
        /// </summary>
        /// <param name="metadataIndex">Index of the metadata item.</param>
        /// <returns>String with metadata.</returns>
        public string get_MetadataItem(int metadataIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the name of the driver.
        /// </summary>
        public string Name { get; private set; }

        /// <summary>
        /// Gets a value indicating whether the driver works vector data.
        /// </summary>
        public bool IsVector { get; private set; }

        /// <summary>
        /// Gets a value indicating whether the driver works raster data.
        /// </summary>
        public bool IsRaster { get; private set; }

        /// <summary>
        /// Gets the type of the specified metadata item.
        /// </summary>
        /// <param name="metadataIndex">Index of the metadata item.</param>
        /// <returns>Type of the metadata item.</returns>
        public tkGdalDriverMetadata get_MetadataItemType(int metadataIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value of the specified metadata item.
        /// </summary>
        /// <param name="metadataIndex">Index of the metadata.</param>
        /// <returns>String with metadata.</returns>
        public string get_MetadataItemValue(int metadataIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the key of the specified metadata item.
        /// </summary>
        /// <param name="metadataIndex">Index of the metadata item.</param>
        /// <returns>Metadata item key.</returns>
        public string get_MetadataItemKey(int metadataIndex)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif



