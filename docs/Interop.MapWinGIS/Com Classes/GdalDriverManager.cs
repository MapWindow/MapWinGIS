using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Holds all available GDAL drivers.
    /// </summary>
    /// \new494 Added in version 4.9.4
#if nsp
    #if upd
        public class GdalDriverManager : MapWinGIS.IGdalDriverManager
    #else        
        public class IGdalDriverManager        
    #endif
#else
    public class GdalDriverManager
#endif
    {
        /// <summary>
        /// Gets the number of drivers.
        /// </summary>
        public int DriverCount { get; private set; }

        /// <summary>
        /// Gets the specified driver.
        /// </summary>
        /// <param name="driverIndex">Index of the driver.</param>
        /// <returns>The driver.</returns>
        public GdalDriver get_Driver(int driverIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets driver with the specified name.
        /// </summary>
        /// <param name="DriverName">Name of the driver.</param>
        /// <returns>The driver.</returns>
        public GdalDriver get_DriverByName(string DriverName)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif



