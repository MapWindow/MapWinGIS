
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    /// <summary>
    /// A grid color break object defines how a specified region of a grid will be colored. 
    /// </summary>
    /// <remarks>Represents a part of the GridColorScheme.</remarks>
    #if nsp
        #if upd
            public class GridColorBreak : MapWinGIS.IGridColorBreak
        #else        
            public class IGridColorBreak
        #endif
    #else
        public class GridColorBreak
    #endif
    {
        #region IGridColorBreak Members
        /// <summary>
        /// Gets or sets the caption to display for the color break
        /// </summary>
        public string Caption
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the way a break is colored.
        /// </summary>
        public ColoringType ColoringType
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
        /// Gets or sets the model used to color the break.
        /// </summary>
        public GradientModel GradientModel
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the color representing the highest value in the color break.
        /// </summary>
        public uint HighColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the highest value in the color break.
        /// </summary>
        public double HighValue
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
        /// Gets or sets the lowest value in the color break.
        /// </summary>
        public uint LowColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the lowest value in the color break. 
        /// </summary>
        public double LowValue
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Retrieves the error message associated with the specified error code. 
        /// </summary>
        /// <param name="ErrorCode">The error code for which the error message is required.</param>
        /// <returns>The error message description for the specified error code. </returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a value indicating whether the break is visible.
        /// </summary>
        /// \new494 Added in version 4.9.4
        public bool Visible { get; set; }

        #endregion
    }
#if nsp
}
#endif

