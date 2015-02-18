
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    /// <summary>
    /// A shapefile color scheme defines how a shapefile will be colored. 
    /// </summary>
    /// <remarks>A shapefile color scheme consists of ShapefileColorBreak objects.</remarks>
    /// \deprecated in v. 4.8. Use ShapefileCategories, ShapefileCategory, ColorScheme instead.
    /// \removed493 Removed in 4.9.3
    #if nsp
        #if upd
            public class ShapefileColorScheme : MapWinGIS.IShapefileColorScheme
        #else        
            public class IShapefileColorScheme
        #endif
    #else
        public class ShapefileColorScheme
    #endif
    {
        #region IShapefileColorScheme Members
        /// <summary>
        /// Adds a color break to the color scheme.
        /// </summary>
        /// <param name="Break"></param>
        /// <returns></returns>
        public int Add(IShapefileColorBreak Break)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the index in the attribute table the color scheme is associated with.
        /// </summary>
        public int FieldIndex
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets callback object to return the information about the errors.
        /// </summary>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Insert a color break in the specified position of the scheme.
        /// </summary>
        /// <param name="Position">The position to insert color break at.</param>
        /// <param name="Break">The color break to insert.</param>
        /// <returns>The actual position color break was inserted at.</returns>
        public int InsertAt(int Position, IShapefileColorBreak Break)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets text string associated with the object.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the code of the last error which took place inside this instance of class.
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the layer handle the color scheme is associated with.
        /// </summary>
        public int LayerHandle
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the number of color breaks in the color scheme.
        /// </summary>
        /// <returns>The number of breaks.</returns>
        public int NumBreaks()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes specific color break from the scheme.
        /// </summary>
        /// <param name="Index">The index of the break to remove.</param>
        public void Remove(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a color break from the color scheme.
        /// </summary>
        /// <param name="Index">The index of the break.</param>
        /// <returns>The color break object or NULL reference on failure.</returns>
        public IShapefileColorBreak get_ColorBreak(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the description of the error code returned by ShapefileColorScheme.LastErrorCode.
        /// </summary>
        /// <param name="ErrorCode">The code of error.</param>
        /// <returns>The description of error.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Replaces a color break in the color scheme.
        /// </summary>
        /// <param name="Index">The index of color break to replace.</param>
        /// <param name="pVal">The reference to the new color break.</param>
        public void set_ColorBreak(int Index, IShapefileColorBreak pVal)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
#if nsp
}
#endif

