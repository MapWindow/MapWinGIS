
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    /// <summary>
    /// A grid header object is used to hold the information contained in a grid's header
    /// </summary>
    #if nsp
        #if upd
            public class GridHeader : MapWinGIS.IGridHeader
        #else        
            public class IGridHeader
        #endif
    #else
        public class GridHeader
    #endif
    {
        #region IGridHeader Members
        /// <summary>
        /// Gets or sets a text string with information about color table. This value doesn't affect grid processing.
        /// </summary>
        public string ColorTable
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Copies the contents of another instance of the grid header.
        /// </summary>
        /// <param name="pVal">The grid header to copy setting from.</param>
        public void CopyFrom(GridHeader pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Not implemented. Get or sets the geoprojection associated with grid header.
        /// </summary>
        public GeoProjection GeoProjection
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The global callback is the interface used by MapWinGIS to pass progress and error events to interested applications. 
        /// </summary>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
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
        /// The value representing cells in the grid with no-data or missing data values.
        /// </summary>
        public object NodataValue
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets notes or any additional information about the grid stored in the grid header. 
        /// </summary>
        public string Notes
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the number of columns for the grid represented by the grid header.
        /// </summary>
        public int NumberCols
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the number of rows in the grid represented by the grid header. 
        /// </summary>
        public int NumberRows
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets information about the projection of the grid represented by the grid header. Setting this value has no effect on 
        /// the actual grid data. The projection should be expressed as a PROJ4 string. 
        /// </summary>
        public string Projection
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The x coordinate of the center of the lower-left cell in the grid represented by the grid header.
        /// </summary>
        public double XllCenter
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the y coordinate of the center of the lower-left cell in the grid represented by the grid header. 
        /// </summary>
        public double YllCenter
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the width of the grid cell in physical units for the grid represented by the grid header
        /// </summary>
        public double dX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the height of a cell in the grid represented by the grid header.
        /// </summary>
        public double dY
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
        /// For inner use only.
        /// </summary>
        public void set_Owner(ref int t, ref int d, ref int s, ref int l, ref int __p5)
        {
            throw new NotImplementedException();
        }
        #endregion
    }
#if nsp
}
#endif

