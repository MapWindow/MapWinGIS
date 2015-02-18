
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    
    /// <summary>
    /// An ESRI grid manager object provides functions which facilitate using ESRI grids with MapWinGIS.
    /// </summary>
    /// <remarks>
    /// The functions and properties are listed below. Clicking on each will yield a description of 
    /// the function and its arguments as well as sample code where applicable.</remarks>
    #if nsp
        #if upd
            public class ESRIGridManager : MapWinGIS.IESRIGridManager
        #else        
            public class IESRIGridManager
        #endif
    #else
        public class ESRIGridManager
    #endif
    {
        #region IESRIGridManager Members
        /// <summary>
        /// Gets whether or not MapWinGIS can open ESRI grids.
        /// </summary>
        /// <remarks>Since ESRI grids are proprietary, MapWinGIS can only open them if another ESRI product has already been installed 
        /// which provides the files needed to manipulate ESRI grids.</remarks>
        /// <returns>A boolean value representing whether or not MapWinGIS can open ESRI grids.</returns>
        public bool CanUseESRIGrids()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deletes the specified ESRI grid.
        /// </summary>
        /// <param name="Filename">The filename of the ESRI grid to be deleted.</param>
        /// <returns>A boolean value representing the success or failure of deleting the specified ESRI grid.</returns>
        public bool DeleteESRIGrids(string Filename)
        {
            throw new NotImplementedException();
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
        /// Gets whether or not the specified file is an ESRI grid.
        /// </summary>
        /// <param name="Filename">The filename of the grid to be checked. </param>
        /// <returns>A boolean value representing whether or not the specified file is an ESRI grid.</returns>
        public bool IsESRIGrid(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Retrieves the last error generated in the object.
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
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

        #endregion
    }
#if nsp
}
#endif

