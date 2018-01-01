using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Represents a light wrapper around GDAL raster dataset which allows to perform some 
    /// operations without using more advanced classes like RasterSource or GridSource.
    /// </summary>
    /// \new494 Added in version 4.9.4
#if nsp
    #if upd
        public class GdalDataset : MapWinGIS.IGdalDataset
    #else        
        public class IGdalDataset        
    #endif
#else
    public class GdalDataset
#endif
    {
        /// <summary>
        /// Opens datasource from the specified filename.
        /// </summary>
        /// <param name="Filename">The filename.</param>
        /// <param name="readOnly">If set to <c>true</c> the datasource will be opened in read only mode.</param>
        /// <returns>True on success.</returns>
        public bool Open(string Filename, bool readOnly)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Closes the datasource.
        /// </summary>
        public void Close()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets transformation parameters for GDAL datasource.
        /// </summary>
        /// <param name="xLeft">The x coordinate of the top left pixel.</param>
        /// <param name="dX">The width of a pixel.</param>
        /// <param name="yProjOnX">The projection of Y on X axis.</param>
        /// <param name="yTop">The y coordinate of the top left pixel.</param>
        /// <param name="xProjOnY">The projection of X on Y axis.</param>
        /// <param name="dY">The height of a pixel.</param>
        /// <returns></returns>
        /// <exception cref="System.NotImplementedException"></exception>
        public bool SetGeoTransform(double xLeft, double dX, double yProjOnX, double yTop, double xProjOnY, double dY)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets projection of the datasource
        /// </summary>
        /// <param name="Projection">The projection.</param>
        /// <returns>True on success.</returns>
        public bool SetProjection(string Projection)
        {
            throw new NotImplementedException();
        }

        public string GetSubDatasetName(int subDatasetIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the driver which manages this datasource.
        /// </summary>
        public GdalDriver Driver { get; private set; }

        public int SubDatasetCount { get; }
    }
#if nsp
}
#endif



