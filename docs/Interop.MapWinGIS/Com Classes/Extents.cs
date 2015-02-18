
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    
    /// <summary>
    /// Represents a rectangle on the map.
    /// </summary>
    /// <remarks>In some cases additional Z and M dimensions can also be specified to denote the altitude of the displayed data for example.</remarks>\n
    /// Let's see how to display certain extents on the map.
    /// \code
    /// double y = 48.7;    // latitude, deg.
    /// double x = 2.3;     // longitude, deg.
    /// double span = 0.1;  // deg.
    ///
    /// // extents from 48.5 to 48.9 degrees of north latitude and 
    /// // from 2.1 to 2.5 degrees of east longitude will be set
    /// Extents ext = new Extents();
    /// ext.SetBounds(x - span, y - span, 0.0, x + span, y + span, 0.0);
    ///
    /// // show them on the map
    /// AxMap axMap;
    /// if (axMap.MapUnits == tkUnitsOfMeasure.umDecimalDegrees){  
    ///    axMap.Extents = ext;
    /// }
    /// \endcode
    /// MapWinGIS doesn't hold instances of %Extents class for layers or map, but generates them on each client call.
    /// Therefore it's useless to try to change those extents in the way like this:
    /// \code
    /// AxMap axMap;
    /// Extents ext = axMap.Extents;
    /// ext.SetBounds(some_bounds);   // map won't be updated
    /// \endcode
    /// The following line is needed:
    /// \code axMap.Extents = ext; \endcode
    /// In case of data layers, like shapefiles or images, the extents are obtained by calculation,
    /// so the only way to alter them is to change the underlying data.
    #if nsp
        #if upd
            public class Extents : MapWinGIS.IExtents
        #else        
            public class IExtents
        #endif
    #else
        public class Extents
    #endif
    {
        #region IExtents Members
        /// <summary>
        /// Gets the bounds of the extents object. 
        /// </summary>
        /// <param name="xMin">Returns the minimum x value for the extents object.</param>
        /// <param name="yMin">Returns the minimum y value for the extents object.</param>
        /// <param name="zMin">Returns the minimum z value for the extents object.</param>
        /// <param name="xMax">Returns the maximum x value for the extents object.</param>
        /// <param name="yMax">Returns the maximum y value for the extents object.</param>
        /// <param name="zMax">Returns the maximum z value for the extents object.</param>
        public void GetBounds(out double xMin, out double yMin, out double zMin, out double xMax, out double yMax, out double zMax)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Gets the minimum and maximum measure bounds for the extents object. 
        /// </summary>
        /// <remarks>Measure bounds only apply to shapefiles containing measure data. </remarks>
        /// <param name="mMin"></param>
        /// <param name="mMax"></param>
        public void GetMeasureBounds(out double mMin, out double mMax)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the bounds for the extents object. 
        /// </summary>
        /// <param name="xMin">The new minimum x value for the bounds of the extents object. </param>
        /// <param name="yMin">The new minimum y value for the bounds of the extents object.</param>
        /// <param name="zMin">The new minimum z value for the bounds of the extents object.</param>
        /// <param name="xMax">The new maximum x value for the bounds of the extents object.</param>
        /// <param name="yMax">The new maximum y value for the bounds of the extents object.</param>
        /// <param name="zMax">The new maximum z value for the bounds of the extents object.</param>
        public void SetBounds(double xMin, double yMin, double zMin, double xMax, double yMax, double zMax)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the measure bounds of the extents object. 
        /// </summary>
        /// <remarks>Measure bounds only apply to shapefiles containing measure data.</remarks>
        /// <param name="mMin">The new minimum measure bound for the extents object.</param>
        /// <param name="mMax">The new maximum measure bound for the extents object.</param>
        public void SetMeasureBounds(double mMin, double mMax)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The maximum measure bound in the exents object. Measure bounds only apply to shapefiles containing measure data.
        /// </summary>
        public double mMax
        {
            get { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Gets the minimum measure bound for the extents object. Measure bounds only apply to shapefiles containing measure data.
        /// </summary>
        public double mMin
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The maximum x bound for the extents object.
        /// </summary>
        public double xMax
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the minimum x bound for the extents object.
        /// </summary>
        public double xMin
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The maximum y bound for the extents object.
        /// </summary>
        public double yMax
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the minimum y bound for the extents object
        /// </summary>
        public double yMin
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The maximum z bound for the extents object.
        /// </summary>
        public double zMax
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the minimum z bound for the extents object.
        /// </summary>
        public double zMin
        {
            get { throw new NotImplementedException(); }
        }

        #endregion

        /// <summary>
        /// Gets serialized contents of the extents for debug purposes.
        /// </summary>
        /// <returns>Serialized string.</returns>
        /// \new491 Added in version 4.9.1
        public string ToDebugString()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a center point for the extents.
        /// </summary>
        /// \new491 Added in version 4.9.1
        public Point Center
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Moves extents so that center point will be at specified coordinates, while width and height will be preserved.
        /// </summary>
        /// <param name="x">X coordinate of new center.</param>
        /// <param name="y">Y coordinate of new center.</param>
        /// \new491 Added in version 4.9.1
        public void MoveTo(double x, double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Checks whether a point is within extents
        /// </summary>
        /// <param name="x">X coordinate of point</param>
        /// <param name="y">Y coordinate of point</param>
        /// <returns>True if point is within extents</returns>
        /// \new491 Added in version 4.9.1
        public bool PointIsWithin(double x, double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a rectangular polygon matching current extents.
        /// </summary>
        /// <returns>New polygon shape.</returns>
        /// \new491 Added in version 4.9.1
        public Shape ToShape()
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif

