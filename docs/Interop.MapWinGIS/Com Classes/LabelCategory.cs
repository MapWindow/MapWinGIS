
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Represents visualization options for labels displayed on the map. 
    /// </summary>
    /// \dot
    /// digraph labelscategory_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// lcat [ label="LabelCategory" URL="\ref LabelCategory"];
    /// 
    /// node [color = tan peripheries = 1 height = 0.3, width = 1.0];
    /// lbs [ label="Labels" URL="\ref Labels"];
    /// 
    /// edge [ dir = "none", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// lbs -> lcat [ URL="\ref Labels.get_Category()", tooltip = "Labels.get_Category()", headlabel = "   n"];
    /// }
    /// 
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>\n
    /// <remarks>
    /// 1. The following elements of label appearance can be set:
    /// - the text itself;
    /// - the outline of text;
    /// - the halo around the text;
    /// - the shadow of the text;
    /// - frame around text.
    /// </remarks>
    /// Here are properties that define the appearance of each of the listed elements. For good visual experience it usually makes sense 
    /// not to show all the decorations at once, but choose either halo, shadow, outline or frame. The main purpose of all these decorations 
    /// is to provide some contrast for the text of labels, as parts of map may have the same color as the text of labels. Halo and frame
    /// are usually the best candidates for it.
    /// \dot
    /// digraph label_elements {
    /// splines = ortho;
    /// ranksep = 0.15;
    /// nodesep = 0.5;
    /// node    [shape = oval, peripheries = 1, fontname=Helvetica, fontsize=9, fillcolor = gray, color = "gray", style = filled, height = 0.3, width = 0.8];
    /// font    [ label="Font"];
    /// outline [ label="Outline, halo,\n shadow"];
    /// frame   [label = "Frame"];
    /// 
    /// node  [shape = "note", width = 0.3, height = 0.3, peripheries = 1 fillcolor = "khaki" ]
    /// s1    [label=" FontName\l FontSize\l FontColor\l FontColor2\l FontGradientMode\l FontItalic\l FontStrikeOut\l FontUnderline\l\l\l"];
    /// s2    [label=" FontOutlineColor\l FontOutlineWidth\l FontOutlineStyle\l HaloVisible\l HaloColor\l HaloSize\l ShadowVisible\l ShadowColor\l ShadowOffsetX\l ShadowOffsetY\l"];
    /// s3    [label=" FrameVisible\l FrameTransparency\l FrameBackColor\l FrameBackColor2\l FrameGradientMode\l FrameOutlineColor\l FrameOutlineStyle\l FrameOutlineWidth\l\l\l"];
    /// 
    /// edge  [dir = none, arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#808080" ]
    /// font -> s1;
    /// outline -> s2;
    /// frame -> s3;
    /// }
    /// \enddot
    /// Here is code that demonstrates how to set some of these options. 
    /// \code
    /// LabelCategory ct = new LabelCategory();
    /// 
    /// // font options
    /// ct.FontName = "Courier New";
    /// ct.FontSize = 10;
    /// 
    /// // now let's set the halo
    /// ct.HaloVisible = true;
    /// ct.HaloSize = 4;
    /// 
    /// // turns off the frame
    /// ct.FrameVisible = false;
    /// \endcode
    /// 2. There are properties which specify the position of each label relative to its base point set by Label.x, Label.y:
    /// - alignment;
    /// - vertical and horizontal offset;
    /// - orientation relative to the line (it is used in the process of label generation only).
    /// .
    /// And properties which define position of text relative to frame border:
    /// - InboxAlignment;
    /// - FramePaddingX;
    /// - FramePaddingY.
    /// .
    /// Here are some examples of their usage.
    /// \code
    /// LabelCategory ct = new LabelCategory();
    /// 
    /// // place the label to the right of the base point and on the same level vertically
    /// ct.Alignment = tkLabelAlignment.laCenterRight;
    /// ct.OffsetX = 10;  // 10 pixels to the right
    /// ct.OffsetY = -5;  // 5 pixels up
    /// 
    /// // displays frame
    /// ct.FrameVisible = true;
    /// ct.InboxAlignment = laTopLeft;  // text will be aligned by top-left side of the frame
    /// ct.FramePaddingX = 20;    // additional 20 pixels will be between vertical border of the frame and text
    /// \endcode
    /// 
    /// See \ref labels_categories "Visualization categories" for information on how to map particular labels to the visualization category. 
    /// 
    /// </remarks>
    /// \new48 Added in version 4.8
    #if nsp
        #if upd
            public class LabelCategory : MapWinGIS.ILabelCategory
        #else        
            public class ILabelCategory
        #endif
    #else
        public class LabelCategory
    #endif
    {
        #region ILabelCategory Members
        /// <summary>
        /// Gets or sets the horizontal and vertical alignment of labels.
        /// </summary>
        public tkLabelAlignment Alignment
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Restores the state of the object from the string.
        /// </summary>
        /// <param name="newVal">A string generated by LabelCategory.Serialize() method.</param>
        public void Deserialize(string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the value which indicates whether the category will be used in the process of shapefile drawing.
        /// </summary>
        public bool Enabled
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets expression which defines shapes that belong to this category.
        /// </summary>
        public string Expression
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether font is bold.
        /// </summary>
        public bool FontBold
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets font color of labels.
        /// </summary>
        public uint FontColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the second font color 
        /// </summary>
        public uint FontColor2
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the gradient for the font of labels.
        /// </summary>
        public tkLinearGradientMode FontGradientMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether font is italic.
        /// </summary>
        public bool FontItalic
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets font name to draw labels with. 
        /// </summary>
        /// <remarks>The names of fonts or font families like "Arial" should be passed.</remarks>
        public string FontName
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the outline color of the font.
        /// </summary>
        /// \see FontOutlineVisible
        public uint FontOutlineColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether outline of the font is visible.
        /// </summary>
        /// \see FontOutlineColor, FontOutlineWidth
        public bool FontOutlineVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the width in pixels of the font outline
        /// </summary>
        /// \see FontOutlineVisible
        public int FontOutlineWidth
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the size of the font
        /// </summary>
        public int FontSize
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether font is striked out.
        /// </summary>
        public bool FontStrikeOut
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the transparency of the font. Values from 0 (opaque) to 255 (transparent) are accepted.
        /// </summary>
        public int FontTransparency
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether the font is underlined.
        /// </summary>
        public bool FontUnderline
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the back color of the frame.
        /// </summary>
        public uint FrameBackColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the end color of the frame gradient.
        /// </summary>
        /// \see FrameGradientMode
        public uint FrameBackColor2
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the gradient mode for the label's frame.
        /// </summary>
        /// <remarks>Use gmNone value to disable gradient.</remarks>
        public tkLinearGradientMode FrameGradientMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or set the outline color of the label's frame.
        /// </summary>
        public uint FrameOutlineColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the outline color of the label's frame.
        /// </summary>
        public tkDashStyle FrameOutlineStyle
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the width of the label's frame outline.
        /// </summary>
        public int FrameOutlineWidth
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the horizontal distance from text to the border of label's frame
        /// </summary>
        public int FramePaddingX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the vertical distance from text to the border of label's frame
        /// </summary>
        public int FramePaddingY
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the transparency of the label's frame. Values from 0 to 255 are accepted.
        /// </summary>
        public int FrameTransparency
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the shape of the label's frame
        /// </summary>
        /// 
        public tkLabelFrameType FrameType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether label's frame will be drawn.
        /// </summary>
        public bool FrameVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the color of the halo around the text of the label.
        /// </summary>
        public uint HaloColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the width of the halo around the text of the label. 
        /// </summary>
        public int HaloSize
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether a halo around text will be drawn.
        /// </summary>
        public bool HaloVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the alignment of text within label's frame. 
        /// </summary>
        /// <remarks>This property is commonly used for the alignment of multi-line labels.</remarks>
        public tkLabelAlignment InboxAlignment
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Gets or sets the orientation of the label relative to the polyline it belongs to.
        /// </summary>
        public tkLineLabelOrientation LineOrientation
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the maximum value for the specified label category in case an LabelCategory.Expression contains a range of values. 
        /// </summary>
        /// <remarks>The property is used internally.</remarks>
        public object MaxValue
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the minimum value for the specified label category in case an LabelCategory.Expression contains a range of values. 
        /// </summary>
        /// <remarks>The property is used internally.</remarks>
        public object MinValue
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Gets or sets the name of the category. The names must not be unique.
        /// </summary>
        public string Name
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the horizontal offset in pixels which is used to draw labels.
        /// </summary>
        /// <remarks>When labels are subject to the rotation, "horizontal" means "along text width".</remarks>
        /// \see Labels.AutoOffset property
        public double OffsetX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the vertical offset in pixels which is used to draw labels.
        /// </summary>
        /// <remarks>When labels are subject to the rotation, "vertical" means "along text height".</remarks>
        /// \see Labels.AutoOffset property
        public double OffsetY
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the field index to use for the horizontal offset in pixels which is used to draw labels.
        /// </summary>
        /// /new52 New in version 5.2
        public int OffsetXField
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the field index to use for the vertical offset in pixels which is used to draw labels.
        /// </summary>
        /// /new52 New in version 5.2
        public int OffsetYField
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The property isn't implemented.
        /// </summary>
        public int Priority
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the state of the object in the XML-formatted string.
        /// </summary>
        /// <remarks>Use LabelCategory.Deserialize() to restore the state.</remarks>
        /// <returns>Serialized string</returns>
        public string Serialize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a color of shadow for the text of the label.
        /// </summary>
        /// \see ShadowVisible, ShadowOffsetX, ShadowOffsetY
        public uint ShadowColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the horizontal distance between text of the label and its shadow.
        /// </summary>
        /// <remarks>When labels are subject to the rotation, "horizontal" means "along text width"</remarks>
        /// \see ShadowOffsetY, ShadowVisible
        public int ShadowOffsetX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the horizontal distance between text of the label and its shadow.
        /// </summary>
        /// <remarks>When labels are subject to the rotation, "vertical" means "along text height"</remarks>
        /// \see ShadowOffsetX, ShadowVisible
        public int ShadowOffsetY
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether shadow of the label will be visible.
        /// </summary>
        /// \see ShadowColor, ShadowOffsetX, ShadowOffsetY
        public bool ShadowVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether shapes that belongs to the category 
        /// will be visible.
        /// </summary>
        public bool Visible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the maximum font size fo the category.
        /// </summary>
        /// \new494 Added in version 4.9.4
        public int FontSize2 { get; set; }

        #endregion
    }
#if nsp
}
#endif

