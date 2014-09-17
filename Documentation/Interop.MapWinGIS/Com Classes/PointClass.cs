
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// A point object represents a point with x, y, Z, and M values. Shapes created by adding point objects to the shape. 
    /// </summary>
    /// Here is a diagram for the Point class.
    /// \dot
    /// digraph point_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// pnt [ label="Point" URL="\ref Point"];
    /// 
    /// node [color = tan peripheries = 1 height = 0.3, width = 1.0];
    /// shp [ label="Shape" URL="\ref Shape"];
    /// 
    /// edge [ dir = "none", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// shp -> pnt [ URL="\ref Shape.get_Point()", tooltip = "Labels.get_Point()", headlabel = "   n"];
    /// }
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>
    #if nsp
        #if upd
            public class Point : MapWinGIS.IPoint
        #else        
                            public class IPoint
        #endif
    #else
        public class Point
    #endif
    {
        #region IPoint Members

        /// <summary>
        /// Creates an exact copy of the point.
        /// </summary>
        /// <returns>The new point.</returns>
        /// \new48 Added in version 4.8
        public Point Clone()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The global callback is the interface used by MapWinGIS to pass progress and error events to interested applications.
        /// </summary>
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
        /// Retrieves the last error generated in the object. 
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the measure value of this point. Measures only apply to shapefiles with measure data.
        /// </summary>
        public double M
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the Z value of this point. 
        /// </summary>
        public double Z
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
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
        /// Gets or sets the x value of the point. 
        /// </summary>
        public double x
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the y value of the point. 
        /// </summary>
        public double y
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        #endregion
    }
#if nsp
}
#endif
