using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Represents a rectangle that can be rendered on the map.
    /// </summary>
    /// \new494 Added in version 4.9.4
#if nsp
    #if upd
        public class DrawingRectangle : MapWinGIS.IDrawingRectangle
    #else        
        public class IDrawingRectangle        
    #endif
#else
    public class DrawingRectangle
#endif
    {
        /// <summary>
        /// The X coordinate of the top-left corner of the rectangle.
        /// </summary>
        public double x { get; set; }

        /// <summary>
        /// The Y coordinate of the top-left corner of the rectangle.
        /// </summary>
        public double y { get; set; }

        /// <summary>
        /// The width of the rectangle.
        /// </summary>
        public double Width { get; set; }

        /// <summary>
        /// The height of the rectangle.
        /// </summary>
        public double Height { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether rectangle is visible.
        /// </summary>
        public bool Visible { get; set; }

        /// <summary>
        /// Type of coordinates which are used to specify the position and size of the rectangle.
        /// </summary>
        public tkDrawReferenceList ReferenceType { get; set; }

        /// <summary>
        /// Gets or sets the opacity of the rectangle.
        /// </summary>
        public byte FillTransparency { get; set; }

        /// <summary>
        /// Gets or sets the color of the rectangle.
        /// </summary>
        public uint Color { get; set; }

        /// <summary>
        /// Gets or sets the width of the rectangle's border.
        /// </summary>
        public float LineWidth { get; set; }
    }
#if nsp
}
#endif



