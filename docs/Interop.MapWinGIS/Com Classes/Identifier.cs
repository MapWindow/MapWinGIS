using System;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Provides settings for built-in identifier tool (cmIdentify cursor).
    /// </summary>
    /// \new493 Added in version 4.9.3
    #if nsp
        #if upd
            public class Identifier : MapWinGIS.IIdentifier
        #else        
            public class IIdentifier
        #endif
    #else
            public class Identifier
    #endif
    {
        /// <summary>
        /// Gets or set a value indicating whether shapes of identifiable shapefiles will be highlighted when
        /// mouse pointer is over them.
        /// </summary>
        public bool HotTracking { get; set; }

        /// <summary>
        /// Gets or sets which layers an identifier will be working with.
        /// </summary>
        public tkIdentifierMode IdentifierMode { get; set; }

        /// <summary>
        /// Gets or sets outline color for identified shape.
        /// </summary>
        public uint OutlineColor { get; set; }

        ///// <summary>
        ///// Gets or sets the handle of active layer, which is used when Identifer.IdentifierMode set to imSingleLayer.
        ///// NOTE: this method has been hidden, and functionality replaced through use of the ChooseLayer event.
        ///// </summary>
        //public int ActiveLayer { get; set; }
    }
#if nsp
}
#endif