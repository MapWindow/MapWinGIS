
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Represents a single label on the map.
    /// </summary>
    /// \dot
    /// digraph label_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// lb [ label="Label" URL="\ref Label"];
    /// 
    /// node [color = tan, peripheries = 1, height = 0.3, width = 1.0];
    /// lbs [ label="Labels" URL="\ref Labels"];
    /// 
    /// edge [ dir = "none", arrowhead="open", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// lbs -> lb [ URL="\ref Labels.get_Label()", tooltip = "Labels.get_Label()", headlabel = "   n"];
    /// }
    /// \enddot
    /// 
    /// <a href = "diagrams.html">Graph description</a>\n\n
    /// <remarks>Multipart shapes can have separate labels for every part. Each of such labels is represented by the instance of Label class. 
    /// Labels added to the map can be accessed via Labels.get_Label() property. It's not recommended to store instances of this class 
    /// retrieved from the map after the layer they belong to was removed.</remarks>\n\n
    /// This code demonstrate how to move the label for a shape with a given index.
    /// \code 
    /// Shapefile sf = some_shapefile;
    /// int shapeIndex = 10;         // the index of shape we want to move
    ///   
    /// // retrieving label object
    /// Label label = sf.Labels.get_Label(shapeIndex, 0);    // 0 - the first part
    /// if (label != null)
    /// {
    ///     // is it currently displayed on the screen?
    ///     Debug.Print("The label is drawn: " + label.IsDrawn.ToString());
    ///        
    ///     label.x += 10.0;            // let's move it to the right by 10 map units
    ///     label.y -= 5.0;             // let's move it to the bottom by 5 map units
    ///     label.Visible = true;       // ensure that it's visible
    ///        
    ///     Debug.Print("The text of the label: " + label.Text);
    ///     map.Redraw();               // redraw is needed to see the changes of position
    /// }
    /// else {
    ///     Debug.Print(string.Format("The chart with index {0} doesn't exists", shapeIndex));
    /// }
    /// \endcode
    /// \new48 Added in version 4.8
    #if nsp
        #if upd
            public class Label : MapWinGIS.ILabel
        #else        
            public class ILabel
        #endif
    #else
        public class Label
    #endif
    {
        #region ILabel Members
        /// <summary>
        /// The index of visualization category the label belongs to.
        /// </summary>
        /// <remarks>The value -1 indicates that the label doesn't belong to any visualization category and 
        /// default drawing options specified in the Labels class will be used for it's drawing.</remarks>
        public int Category
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a boolean value which indicates whether the label was drawn at the current extents of the map.
        /// </summary>
        public bool IsDrawn
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the angle of label rotation in degrees. Positive values set 
        /// clockwise rotation, negative - counter-clockwise.
        /// </summary>
        public double Rotation
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the extents the label occupies on the map (in pixels).
        /// </summary>
        /// <remarks>In case the label wasn't drawn on the map, NULL reference will be returned.</remarks>
        public Extents ScreenExtents
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the text of the label.
        /// </summary>
        public string Text
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether the label is visible.
        /// </summary>
        /// <remarks>Call AxMap.Redraw() to see the effect of this property.</remarks>
        public bool Visible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the horizontal position of the label in map coordinates.
        /// </summary>
        public double x
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the vertical position of the label in map coordinates.
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

