
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Defines a part of shapefile color scheme and specifies how a certain region of a shapefile will be colored. 
    /// </summary>
    /// \deprecated in v. 4.8. Use ShapefileCategories, ShapefileCategory, ColorScheme classes instead.
    #if nsp
        #if upd
            public class ShapefileColorBreak : MapWinGIS.IShapefileColorBreak
        #else        
            public class IShapefileColorBreak
        #endif
    #else
        public class ShapefileColorBreak
    #endif
    {
        #region IShapefileColorBreak Members
        /// <summary>
        /// Gets or sets the caption of the shapefile color break.
        /// </summary>
        public string Caption
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the color which will be used for drawing objects characterized by ShapefileColorBreak.EndValue.
        /// </summary>
        public uint EndColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which represent the end of shapefile color break.
        /// </summary>
        public object EndValue
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the color which will be used for drawing objects characterized by ShapefileColorBreak.StartValue.
        /// </summary>
        public uint StartColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which represent the start of shapefile color break.
        /// </summary>
        public object StartValue
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether the objects defined by color break will be displayed.
        /// </summary>
        public bool Visible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        #endregion
    }
#if nsp
}
#endif

