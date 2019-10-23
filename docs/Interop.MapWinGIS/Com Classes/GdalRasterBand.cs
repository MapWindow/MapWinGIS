using System;
using MapWinGIS;
// ReSharper disable CheckNamespace

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Represents a single band of raster datasource. 
    /// Please refer to GDAL documentation for more details information.
    /// </summary>
    /// \new494 Added in version 4.9.4
#if nsp
    #if upd
        public class GdalRasterBand : MapWinGIS.IGdalRasterBand
    #else        
        public class IGdalRasterBand        
    #endif
#else
    public class GdalRasterBand
#endif
    {
        /// <summary>
        /// Gets unique values of the band.
        /// </summary>
        /// <param name="maxCount">The maximum count of values to be returned.</param>
        /// <param name="arr">The array with values.</param>
        /// <returns>True on success.</returns>
        public bool GetUniqueValues(int maxCount, out object arr)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the default histogram.
        /// </summary>
        /// <param name="forceCalculate">If set to <c>true</c> and there is no dialog available, it will be calculated.</param>
        /// <returns>The histogram.</returns>
        public Histogram GetDefaultHistogram(bool forceCalculate)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets custom histogram for the raster band.
        /// </summary>
        /// <param name="MinValue">The minimum value.</param>
        /// <param name="MaxValue">The maximum value.</param>
        /// <param name="NumBuckets">The number buckets.</param>
        /// <param name="includeOutOfRange">If set to <c>true</c> the values outside the specified range will be included in outer buckets.</param>
        /// <param name="allowApproximate">If set to <c>true</c> the approximate calculation will be allowed.</param>
        /// <returns></returns>
        /// <exception cref="System.NotImplementedException"></exception>
        public Histogram GetHistogram(double MinValue, double MaxValue, int NumBuckets, bool includeOutOfRange, bool allowApproximate)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets statistics for the raster band.
        /// </summary>
        /// <param name="allowApproximate">If set to <c>true</c> the approximate calculation will be allowed.</param>
        /// <param name="forceCalculation">if set to <c>true</c> the calculation will be performed even if there are no cached values.</param>
        /// <param name="Minimum">The minimum value.</param>
        /// <param name="Maximum">The maximum value.</param>
        /// <param name="mean">The mean.</param>
        /// <param name="stdDev">The standard deviation.</param>
        /// <returns>True on success.</returns>
        public bool GetStatistics(bool allowApproximate,
            bool forceCalculation,
            out double Minimum,
            out double Maximum,
            out double mean,
            out double stdDev)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Builds classification for the raster band.
        /// </summary>
        /// <param name="MinValue">The minimum value.</param>
        /// <param name="MaxValue">The maximum value.</param>
        /// <param name="classification">The classification type.</param>
        /// <param name="NumCategories">The number of categories.</param>
        /// <returns>Resulting classification.</returns>
        public GridColorScheme Classify(double MinValue, double MaxValue, tkClassificationType classification, int NumCategories)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Computes the minimum and maximum value for the raster band.
        /// </summary>
        /// <param name="allowApproximate">If set to <c>true</c> the approximate calculation will be allowed.</param>
        /// <param name="Minimum">The minimum value.</param>
        /// <param name="Maximum">The maximum value.</param>
        /// <returns>True on success.</returns>
        public bool ComputeMinMax(bool allowApproximate, out double Minimum, out double Maximum)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Computes local statistics for the data in the vicinity of particular pixel.
        /// </summary>
        /// <param name="Column">The column.</param>
        /// <param name="Row">The row.</param>
        /// <param name="range">Number of pixel on each side to be included in calculation.</param>
        /// <param name="Minimum">The minimum value.</param>
        /// <param name="Maximum">The maximum value.</param>
        /// <param name="mean">The mean.</param>
        /// <param name="stdDev">The standard deviation.</param>
        /// <param name="Count">The count.</param>
        /// <returns>True on success.</returns>
        public bool ComputeLocalStatistics(int Column,
            int Row,
            int range,
            out double Minimum,
            out double Maximum,
            out double mean,
            out double stdDev,
            out int Count)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets no data value.
        /// </summary>
        public double NodataValue { get; private set; }

        /// <summary>
        /// Gets the minimum.
        /// </summary>
        public double Minimum { get; private set; }

        /// <summary>
        /// Gets the maximum.
        /// </summary>
        public double Maximum { get; private set; }

        /// <summary>
        /// Gets the number of overviews for the raster band.
        /// </summary>
        public int OverviewCount { get; private set; }

        /// <summary>
        /// Gets or sets the color interpretation for the raster band.
        /// </summary>
        public tkColorInterpretation ColorInterpretation { get; set; }

        /// <summary>
        /// Gets the data type of the raster band.
        /// </summary>
        public tkGdalDataType DataType { get; private set; }

        /// <summary>
        /// Gets the width of the raster band in pixels.
        /// </summary>
        public int XSize { get; private set; }

        /// <summary>
        /// Gets the height of the raster band in pixels.
        /// </summary>
        public int YSize { get; private set; }

        /// <summary>
        /// Gets the size of horizontal block for the raster (in pixels).
        /// </summary>
        public int BlockSizeX { get; private set; }

        /// <summary>
        /// Gets the size of vertical block for the raster (in pixels).
        /// </summary>
        public int BlockSizeY { get; private set; }

        /// <summary>
        /// Gets the type of the unit.
        /// </summary>
        public string UnitType { get; private set; }

        /// <summary>
        /// Gets the raster value scale (is used for coordinate transformation).
        /// </summary>
        public double Scale { get; private set; }

        /// <summary>
        /// Gets the raster value offset (is used for coordinate transformation).
        /// </summary>
        public double Offset { get; private set; }

        /// <summary>
        /// Gets a value indicating whether the raster band has color table.
        /// </summary>
        public bool HasColorTable { get; private set; }

        /// <summary>
        /// Gets the number of metadata items associated with the band.
        /// </summary>
        public int MetadataCount { get; private set; }

        /// <summary>
        /// Gets the specified metadata item.
        /// </summary>
        /// <param name="itemIndex">Index of the item.</param>
        /// <returns>String with metadata.</returns>
        public string get_MetadataItem(int itemIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the color table associated with the band.
        /// </summary>
        public GridColorScheme ColorTable { get; private set; }

        /// <summary>
        /// Gets specified overview of the raster band.
        /// </summary>
        /// <param name="overviewIndex">Index of the overview.</param>
        /// <returns>The overview as another instance of GdalRasterBand.</returns>
        public GdalRasterBand get_Overview(int overviewIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value of the specified pixel.
        /// </summary>
        /// <param name="Column">The column.</param>
        /// <param name="Row">The row.</param>
        /// <param name="pVal">The value.</param>
        /// <returns>True on success.</returns>
        public bool get_Value(int Column, int Row, out double pVal)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif



