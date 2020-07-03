
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    using MapWinGIS;

    /// <summary>
    /// Represents a list of labels of the map layer and their visualization options.
    /// </summary>
    /// \dot
    /// digraph labels_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// lcat [ label="LabelCategory" URL="\ref LabelCategory"];
    /// lb [ label="Label" URL="\ref Label"];
    /// 
    /// node [color = tan, peripheries = 1, height = 0.3, width = 1.0];
    /// lbs [ label="Labels" URL="\ref Labels"];
    /// 
    /// node [style = dashed, color = gray];
    /// sf [ label="Shapefile" URL="\ref Shapefile"];
    /// img [ label="Image" URL="\ref Image"];
    /// 
    /// edge [ dir = "none", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// sf -> lbs [ URL="\ref Shapefile.Labels", tooltip = "Shapefile.Labels", headlabel = "   1"];
    /// img -> lbs [ URL="\ref Image.Labels", tooltip = "Image.Labels", headlabel = "   1"];
    /// lbs -> lcat [ URL="\ref Labels.get_Category()", tooltip = "Labels.get_Category()", headlabel = "   n"];
    /// lbs -> lb [ URL="\ref Labels.get_Label()", tooltip = "Labels.get_Label()", headlabel = "   n"];
    /// }
    /// \enddot
    /// 
    /// <a href = "diagrams.html">Graph description</a>\n\n
    /// Here is a diagram with groups of API members of Labels class.
    /// \dotfile labelsgroups.dot
    /// <a href = "diagrams.html">Graph description</a>
    /// \new48 Added in version 4.8
    #if nsp
        #if upd
            public class Labels : MapWinGIS.ILabels
        #else        
            public class ILabels
        #endif
    #else
        public class Labels
    #endif
    {
        #region ILabels Members
        /// <summary>
        /// Gets or sets the global callback object which is used for passing to the client 
        /// an information about progress and errors.
        /// </summary>
        /// <remarks>An instance of the class which implements ICallback interface should be passed.
        /// The class should be implemented by caller.</remarks>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// A text string associated with the instance of the class. Any value can be stored by developer in this property.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Marks that values of size field have changed and the size of labels should be recalculated on the next redraw.
        /// </summary>
        /// \new494 Added in version 4.9.4
        public void UpdateSizeField()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the maximum font size for labels (when size field is used).
        /// </summary>
        /// \new494 Added in version 4.9.4
        public int FontSize2 { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether variable size for labels should be used 
        /// (in the range between Labels.FontSize, Labels.FontSize2).
        /// </summary>
        /// \new494 Added in version 4.9.4
        public bool UseVariableSize { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether logarithmic scale will be used to calculate the size of labels.
        /// </summary>
        /// \see Labels.UseVariableSize
        /// \new494 Added in version 4.9.4
        public bool LogScaleForSize { get; set; }

        /// <summary>
        /// Returns the code of the last error which has occurred within the instance of class.
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the description of the specific error code.
        /// </summary>
        /// <param name="ErrorCode">The error code returned by Labels.LastErrorCode.</param>
        /// <returns>The string with the description.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the list of visualization options of labels. For inner use only.
        /// </summary>
        public LabelCategory Options
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the values indicating whether GDI+ library should be used to draw labels.
        /// </summary>
        /// <remarks>The default value is true. When set to false, GDI will be used for drawing.
        /// In GDI mode no gradients and transparency is supported, while in some cases it can be somewhat faster then GDI+.</remarks>
        public bool UseGdiPlus
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a classification field used by unique values classification.
        /// </summary>
        /// <remarks>The property is used internally in couple with LabelCategory.MinValue and LabelCategory.MaxValue
        /// to avoid the parsing of expressions for the categories in case unique values classification was applied.</remarks>
        public int ClassificationField
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the expression used to generate text of labels from the attribute table of the shapefile.
        /// </summary>
        /// <remarks>Applicable for the instances of Labels class associated with shapefile (obtained by Shapefile.Labels).</remarks>
        public string Expression
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// For labels attached to shapefile layer updates the text of each label based on Expression property.
        /// </summary>
        /// <remarks>The method is necessary when the values in underlying DBF table have changed, 
        /// since Labels class doesn't track these changes automatically.
        /// This method is depraceted since version 5.2 - use the new ApplyLabelExpression instead.</remarks>
        public void ForceRecalculateExpression()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Generates the labels for the layer based on the label expression.
        /// </summary>
        public void ApplyLabelExpression()
        {
            throw new NotImplementedException();
        }

        #endregion

        /// The modules listed here are parts of the documentation of Labels class.
        /// \addtogroup Labels_modules Labels modules
        /// \dotfile labelsgroups.dot
        /// <a href = "diagrams.html">Graph description</a>
        ///  @{

        #region Label categories
        /// \addtogroup labels_categories Labels categories
        /// Here is a list of properties and methods to work with visualization categories for labels
        /// represented by LabelCategory class. This module is a part of the documentation of Labels class.
        /// \dot
        /// digraph labels_categories {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Labels" URL="\ref Labels"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Categories"          URL="\ref labels_categories"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>\n\n
        /// A visualization category is designed to provide a common visualization options for a group of labels with similar attributes.\n\n
        /// Visualization categories can be added in several ways.\n\n
        /// <b>1. Automatic generation. </b> This example demonstrates how to create 6 categories with frame color ranging from orange to blue.
        /// \code
        /// Shapefile sf = some_shapefile;
        /// sf.Labels.FrameVisible = true;  // this option will be used inherited by newly generated categories
        /// 
        /// if (sf.Labels.GenerateCategories(0, tkClassificationType.ctNaturalBreaks, 6))   // 0 - index of field
        /// {
        ///    ColorScheme scheme = new ColorScheme();
        ///    scheme.SetColors2(tkMapColor.Orange, tkMapColor.Blue);
        ///    sf.Labels.ApplyColorScheme2(tkColorSchemeType.ctSchemeGraduated, scheme, tkLabelElements.leFrameBackground);
        /// }
        /// \endcode
        /// 
        /// <b>2. Manual creation.</b> In this scenario particular labels can be attributed to the category by: \n
        /// - specification of expression;
        /// - assigning Label.Category property.
        /// .
        /// In this example a new category with yellow background will be created and objects with area greater than 100 units will be mapped to it.
        /// \code
        /// LabelCategory ct2 = sf.Labels.AddCategory("yellow");
        /// if (ct2 != null)
        /// {
        ///    ct2.FrameBackColor = Convert.ToUInt32(tkMapColor.Yellow);
        ///    ct2.Expression = "[Area] > 100";
        ///    sf.Labels.ApplyCategories();     // this will set Label.Category property
        /// }
        /// \endcode
        /// 
        /// This time a category with gray background will be added and then all the labels with even index will be attributed to it.
        /// \code
        /// LabelCategory ct = sf.Labels.AddCategory("gray");
        /// if (ct != null)
        /// {
        ///    ct.FrameBackColor = Convert.ToUInt32(tkMapColor.Gray);
        ///    int index = sf.Labels.NumCategories - 1;    // it was added as the last one
        ///    for (int i = 0; i < sf.Labels.Count; i = i + 2)
        ///    {
        ///        sf.Labels.get_Label(i, 0).Category = index;  // 0 is the index of part
        ///    }
        /// }
        /// \endcode
        /// Don't forget to redraw the map in the end (AxMap.Redraw).
        /// @{

        /// <summary>
        /// Gets the visualization category (LabelCategory) with the specified index.
        /// </summary>
        /// <param name="Index">The index of the category to get.</param>
        /// <returns>The reference to the category or null in case of the incorrect index.</returns>
        public LabelCategory get_Category(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the visualization category at the given position in the list.
        /// </summary>
        /// <remarks>In fact the method replaces the category at the given index with the new one.
        /// <param name="Index">The index to set the category at. It should be less then Labels.NumCategories.</param>
        /// <param name="pVal">The reference to the category to set.</param>
        public void set_Category(int Index, LabelCategory pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes a visualization category with specified index.
        /// </summary>
        /// <param name="Index">The index of category to be removed.</param>
        /// <returns>True on successful removal and false otherwise.</returns>
        public bool RemoveCategory(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds a visualization category for labels.
        /// </summary>
        /// <remarks>Use LabelCategory.Expression or Label.Category to map specific labels to the newly
        /// created category.</remarks>
        /// <param name="Name">The name of the new category</param>
        /// <returns>The reference to the new category.</returns>
        public LabelCategory AddCategory(string Name)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Maps labels to the visualization categories.
        /// </summary>
        /// <remarks>LabelCategory.Expression property is analysed. 
        /// Afterwards Label.Category property is changed for all labels whose attributes comply with the expression.</remarks>
        public void ApplyCategories()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes all the visualization categories.
        /// </summary>
        public void ClearCategories()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Generates visualization categories for labels based on certain field from the attribute 
        /// table of the parent shapefile.
        /// </summary>
        /// <remarks>Use Labels.ApplyCategories to map particular labels to the generated categories based on the 
        /// LabelCategory.Expression property.</remarks>
        /// <param name="FieldIndex">The index of field from the attribute table.</param>
        /// <param name="ClassificationType">The type of classification.</param>
        /// <param name="numClasses">Number of categories to be generated. It will be ignored for ctUniqueValues.</param>
        /// <returns>True on successful generation and false otherwise.</returns>
        public bool GenerateCategories(int FieldIndex, tkClassificationType ClassificationType, int numClasses)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inserts a new visualization category at the given position of the list.
        /// </summary>
        /// <param name="Index">The index to insert the new category at. Should be greater or equal to 0 and less then 
        /// Labels.numCategories.</param>
        /// <param name="Name">The name of the new category.</param>
        /// <returns>The reference to the newly created category.</returns>
        public LabelCategory InsertCategory(int Index, string Name)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves a specified label category down in the list.
        /// </summary>
        /// <param name="Index">The index of the category to move.</param>
        /// <returns>True on successful operation and false otherwise.</returns>
        public bool MoveCategoryDown(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves a specified label category up in the list.
        /// </summary>
        /// <param name="Index">The index of the category to move.</param>
        /// <returns>True on successful operation and false otherwise.</returns>
        public bool MoveCategoryUp(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Number of visualization categories associated with this instance of class.
        /// </summary>
        public int NumCategories
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Applies color scheme to the visualization categories.
        /// </summary>
        /// <param name="Type">The method of interpolation.</param>
        /// <param name="ColorScheme">Color scheme to take colors from.</param>
        public void ApplyColorScheme(tkColorSchemeType Type, ColorScheme ColorScheme)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Applies color scheme to the visualization categories. 
        /// </summary>
        /// <remarks>Allows to choose the part of label to apply colors to.</remarks>
        /// <param name="Type">The method of interpolation.</param>
        /// <param name="ColorScheme">Color scheme object to borrow colors from.</param>
        /// <param name="Element">The element of the label to apply colors to. Labels.ApplyColorScheme is shortcut for using leDefault option.</param>
        public void ApplyColorScheme2(tkColorSchemeType Type, ColorScheme ColorScheme, tkLabelElements Element)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Applies color scheme to the visualization categories. 
        /// </summary>
        /// <param name="Type">The method of interpolation.</param>
        /// <param name="ColorScheme">Color scheme object to borrow colors from.</param>
        /// <param name="Element">The element of the label to apply colors to. Labels.ApplyColorScheme is shortcut for using leDefault option.</param>
        /// <param name="CategoryStartIndex">The index of the first visualization category to apply colors to.</param>
        /// <param name="CategoryEndIndex">The index of the last visualization category to apply colors to.</param>
        public void ApplyColorScheme3(tkColorSchemeType Type, ColorScheme ColorScheme, tkLabelElements Element, int CategoryStartIndex, int CategoryEndIndex)
        {
            throw new NotImplementedException();
        }

        /// @}

        #endregion

        #region Labels Management
        /// \addtogroup labels_management Labels generation
        /// Here is a list of properties and methods for adding labels (or their parts) to the map or removing them. 
        /// This module is a part of the documentation of Labels class.
        /// \dot
        /// digraph labels_management_graph {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Labels" URL="\ref Labels"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Managemant"          URL="\ref labels_management"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Adds a new label as the last one in the list.
        /// </summary>
        /// <remarks>All the parameters passed to the method can be changed afterwards using Labels.get_Label() property.</remarks>
        /// <param name="Text">The text of the new label.</param>
        /// <param name="x">The x coordinate of the new label.</param>
        /// <param name="y">The y coordinate of the new label.</param>
        /// <param name="offsetX">The x offset in pixels of the new label.</param>
        /// <param name="offsetY">The y offset in pixels of the new label.</param>
        /// <param name="Rotation">The rotation of the label in degrees. Positive values set clockwise rotation, negative - counter-clockwise.</param>
        /// <param name="Category">The index of visualization category to be used for the label drawing. 
        /// Default value is -1, which means the default drawing options will be used.</param>
        /// \new52 offsetX & offsetY were introduced in version 5.2
        public void AddLabel(string Text, double x, double y, double offsetX, double offsetY, double Rotation, int Category)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds a part to the label with specified index.
        /// </summary>
        /// <remarks>According to the shapefile specification shapes such as polylines or polygons can have multiple parts. 
        /// Use this method to add a separate label for each part of the parent shape.</remarks>
        /// <param name="Index">The index of label to add a part to.</param>
        /// <param name="Text">The text to be displayed.</param>
        /// <param name="x">The x coordinate of the part.</param>
        /// <param name="y">The y coordinate of the part.</param>
        /// <param name="offsetX">The x offset in pixels of the new label.</param>
        /// <param name="offsetY">The y offset in pixels of the new label.</param>
        /// <param name="Rotation">The rotation of the label's part.</param>
        /// <param name="Category">The index of visualization category to be used for the label drawing. 
        /// Default value is -1, which means the default drawing options will be used.</param>
        /// /// \new52 offsetX & offsetY were introduced in version 5.2
        public void AddPart(int Index, string Text, double x, double y, double offsetX, double offsetY, double Rotation, int Category)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes all the labels and parts but not the visualization categories.
        /// </summary>
        public void Clear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of labels. 
        /// </summary>
        /// <remarks>Each label can have more then one part.</remarks>
        public int Count
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Generates labels for each shape of the parent shapefile.
        /// </summary>
        /// <remarks>Applicable for the instances of Labels class associated with shapefile, i.e. returned
        /// by Shapefile.Labels property.</remarks>
        /// <param name="Expression">The expression to be used for generation of the text for labels.</param>
        /// <param name="Method">The method to calculate position of each label.</param>
        /// <param name="LargestPartOnly">A value which indicates whether labels should be created for each part of the 
        /// multipart shape or for the largest part only.</param>
        /// <returns>The number of generated labels.</returns>
        public int Generate(string Expression, tkLabelPositioning Method, bool LargestPartOnly)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inserts a new label at the given position.
        /// </summary>
        /// <param name="Index">The index to insert the label at.</param>
        /// <param name="Text">The text of the label.</param>
        /// <param name="x">The x coordinate of the label.</param>
        /// <param name="y">The y coordinate of the label.</param>
        /// <param name="offsetX">The x offset in pixels of the label.</param>
        /// <param name="offsetY">The y offset in pixels of the label.</param>
        /// <param name="Rotation">The angle of label rotation.</param>
        /// <param name="Category">The index of the visualization category for the label. Use -1 if the label doesn't belong to any category.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new52 offsetX & offsetY were introduced in version 5.2
        public bool InsertLabel(int Index, string Text, double x, double y, double offsetX, double offsetY, double Rotation, int Category)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inserts a new part for the specified label.
        /// </summary>
        /// <remarks>%Labels can have several parts which correspond to the parts of shapes they belong to.</remarks>
        /// <param name="Index">The index of the label.</param>
        /// <param name="Part">The index to insert the new part at.</param>
        /// <param name="Text">The text of the label.</param>
        /// <param name="x">The x coordinate of the label.</param>
        /// <param name="y">The y coordinate of the label.</param>
        /// <param name="offsetX">The x offset in pixels of the label.</param>
        /// <param name="offsetY">The y offset in pixels of the label.</param>
        /// <param name="Rotation">The rotation of the label in degrees.</param>
        /// <param name="Category">The index of visualization category. Use -1 if the label doesn't belong to any category.</param>
        /// <returns>True on success or false otherwise.</returns>
        /// /// \new52 offsetX & offsetY were introduced in version 5.2
        public bool InsertPart(int Index, int Part, string Text, double x, double y, double offsetX, double offsetY, double Rotation, int Category)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes a label with the specified index. If the label is multipart then all its parts will be removed.
        /// </summary>
        /// <param name="Index">The index of the label to be removed.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool RemoveLabel(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes a part of label with the specified index.
        /// </summary>
        /// <param name="Index">The index of the label.</param>
        /// <param name="Part">The index of part to be removed.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool RemovePart(int Index, int Part)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether labels should be synchronized with the parent shapefile.
        /// </summary>
        /// <remarks>Synchronization means that labels will be automatically added or removed simultaneously with 
        /// the corresponding operation in the shapefile.
        /// Labels can be synchronized only if their number is equal to the number of shapes.
        /// </remarks>
        /// \see Labels.Count, Shapefile.NumShapes
        public bool Synchronized
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets label with the specified index. 
        /// </summary>
        /// <param name="Index">The index of label to return.</param>
        /// <param name="Part">The part of the label to return. In case of single part labels 0 should be used.</param>
        /// <returns>A reference to the label or null reference in case of incorrect index.</returns>
        public Label get_Label(int Index, int Part)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of parts for the label with specified index.
        /// </summary>
        /// <param name="Index">The index of the label.</param>
        /// <returns>The number of parts or -1 in case of incorrect index</returns>
        public int get_NumParts(int Index)
        {
            throw new NotImplementedException();
        }

        /// @}
        #endregion

        #region Labels positioning
        /// \addtogroup labels_positioning Labels positioning
        /// Here is a list of properties and methods which affects alignment, positioning and scale of labels. This module is a part of the documentation of Labels class.
        /// \dot
        /// digraph labels_positioning {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Labels" URL="\ref Labels"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Positioning"          URL="\ref labels_positioning"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Map scale for which the size of labels will be equal to value set in Labels.FontSize property.
        /// </summary>
        /// <remarks>Applicable only when Label.Scale is set to true.</remarks>
        public double BasicScale
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether the labels will change their size depending upon map scale. The default value is false.
        /// </summary>
        /// \see Labels.BasicScale
        public bool ScaleLabels
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which affects the sequence of labels drawing relative to the map layers.
        /// </summary>
        /// <remarks>See details in the description of tkVerticalPosition enumeration.</remarks>
        public tkVerticalPosition VerticalPosition
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the indices of all labels which are displayed in the given part of the map.
        /// </summary>
        /// <remarks>This property doesn't change appearance of the labels.</remarks>
        /// <param name="BoundingBox">The bounding box in screen coordinates.</param>
        /// <param name="Tolerance">The selection tolerance.</param>
        /// <param name="SelectMode">The selection mode.</param>
        /// <param name="LabelIndices">An output array with indices of the labels.</param>
        /// <param name="PartIndices">An output array with indices of parts for each label.</param>
        /// <returns>True in case at least one label fell into selection.</returns>
        public bool Select(Extents BoundingBox, int Tolerance, SelectMode SelectMode, ref object LabelIndices, ref object PartIndices)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the method of label positioning.
        /// </summary>
        public tkLabelPositioning Positioning
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the type of the label's orientation relative to the direction of the labelled shape.
        /// </summary>
        /// <remarks>Applicable for the instance of the Labels class associated with polyline shapefiles, 
        /// i.e. obtained by Shapefile.Labels property while Shapefile.ShapefileType is polyline.</remarks>
        public tkLineLabelOrientation LineOrientation
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the horizontal offset in pixels which is used to draw labels.
        /// </summary>
        /// <remarks>When labels are subject to the rotation, "horizontal" means "along the text width".</remarks>
        /// \see Labels.AutoOffset property
        public double OffsetX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the vertical offset in pixels which is used to draw labels.
        /// </summary>
        /// <remarks>When labels are subject to rotation "vertical" means "along the text height".</remarks>
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
        /// Gets or sets the value which defines an alignment of text within label's frame.
        /// </summary>
        /// <remarks>Should be used with multiline labels.</remarks>
        public tkLabelAlignment InboxAlignment
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Turns on or off the auto offset behaviour. 
        /// </summary>
        /// <remarks>
        /// When set to true, Labels.OffsetX and Labels.OffsetY property will be changed 
        /// automatically according to the size of symbols for parent shape points to avoid overlaps.
        /// Applicable for point and multipoint shapefiles only.
        /// </remarks>
        public bool AutoOffset
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Sets horizontal and vertical alignment of labels.
        /// </summary>
        public tkLabelAlignment Alignment
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// @}
        
        #endregion

        #region Labels serialization
        /// \addtogroup labels_serialization Labels serialization
        /// Here is a list of methods and properties for serialization of labels and their persistence between the runs of application. 
        /// This module is a part of the documentation of Labels class.
        /// \dot
        /// digraph labels_serialization {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Labels" URL="\ref Labels"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Serialization"          URL="\ref labels_serialization"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>\n\n
        /// It's possible to serialize different types of information for labels (charts):\n
        /// -# The drawing settings, such visualization options, order of drawing, etc. It's desirable to save them in virtually any scenario.\n\n
        /// -# The positions of the labels (charts). It makes sense to save them to avoid the recalculation of the positions on the next loading. 
        /// It's quite relevant even for middle sized datasets if the more or less complex routine for positioning is used 
        /// (Shape.Centroid, Shape.InteriorPoint).\n\n
        /// -# The angles rotation, text and categories for labels. This type of information requires serialization less often. The text of labels
        /// can be easily restored by applying Labels.Expression. Unless the Label.Text property was set manually.
        /// In the same way categories rely on LabelCategory.Expression and will be serialized among the first group of settings, 
        /// unless Label.Category property was set manually. Finally the rotation of labels which is preserved less often.
        /// .
        /// The first group of settings is embedded in the larger XML body, either layer or map settings. It can be saved:
        /// - to the file (see AxMap.SaveLayerOptions, AxMap.SaveMapState);
        /// - returned as a string (see AxMap.SerializeLayer, AxMap.SerializeMapState).
        /// .
        /// For data described in the 2nd and 3rd sections more options are available by Labels.SavingMode property and tkSavingMode enumeration. It can be:
        /// - not serialized at all (modeNone);
        /// - embedded in the layer or map settings in the same way as the first group (modeStandard);
        /// - written to stand-alone XML file with the name [LayerName].lbl (modeXML, modeXMLOverwrite);
        /// - written to the dbf table of the shapefile (modeDBF).
        /// .
        /// @{

        /// <summary>
        /// Saves position and text of the labels to the attribute table (dbf file).
        /// </summary>
        /// <param name="saveText">A value which indicates whether the text of labels will be saved.</param>
        /// <param name="saveCategory">A value which indicates whether the mapping between labels 
        /// and visualization categories will be saved.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool SaveToDbf(bool saveText, bool saveCategory)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves positions and text of the labels to the attribute table (dbf file).
        /// </summary>
        /// <remarks>This method allows to specify the names of fields to save the data in.</remarks>
        /// <param name="xField">The name of field to store x coordinate.</param>
        /// <param name="yField">The name of field to store y coordinate.</param>
        /// <param name="angleField">The name of field to store angle.</param>
        /// <param name="textField">The name of field to store text.</param>
        /// <param name="categoryField">The name of field to store category.</param>
        /// <param name="saveText">A value which indicates whether the text of labels will be saved.</param>
        /// <param name="saveCategory">A value which indicates whether the mapping between labels and visualization categories will be saved.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool SaveToDbf2(string xField, string yField, string angleField, string textField, string categoryField, bool saveText, bool saveCategory)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the text, positions and visualization options of the labels to the XML file.
        /// </summary>
        /// <param name="Filename">The name of the file to save the settings to.</param>
        /// <returns>True on successful saving and false otherwise. 
        /// Use Labels.get_ErrorMsg() to find out the reason of failure.</returns>
        public bool SaveToXML(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the method to save and restore the state labels.
        /// </summary>
        /// <remarks>See details in tkSavingMode enumeration.</remarks>
        public tkSavingMode SavingMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Loads text and position of labels from the attribute table of the parent shapefile.
        /// </summary>
        /// <remarks>Applicable for the instance of the Labels class associated with polyline shapefiles, 
        /// i.e. obtained by Shapefile.Labels property </remarks>
        /// <param name="loadText">A value which indicates whether the text of the labels should be loaded.</param>
        /// <param name="loadCategory">A value which indicates whether the mapping between labels and visualization categories should be restored.</param>
        /// <returns>True on successful loading and false otherwise. </returns>
        public bool LoadFromDbf(bool loadText, bool loadCategory)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Loads text and position of labels from the attribute table of the parent shapefile.
        /// </summary>
        /// <param name="xField">The name of field which holds x values.</param>
        /// <param name="yField">The name of field which holds y values.</param>
        /// <param name="angleField">The name of field which holds rotation angles.</param>
        /// <param name="textField">The name of the field which stores text of the labels.</param>
        /// <param name="categoryField">The name of the field which stores index of visualization category for each label.</param>
        /// <param name="loadText">A value which indicates whether the text of the labels should be loaded.</param>
        /// <param name="loadCategory">A value which indicates whether the mapping between labels and visualization categories should be restored.</param>
        /// <returns>True on successful loading and false otherwise.</returns>
        public bool LoadFromDbf2(string xField, string yField, string angleField, string textField, string categoryField, bool loadText, bool loadCategory)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restores the state of the labels from the specified XML file.
        /// </summary>
        /// <param name="Filename">The name of the file previously generated by Labels.SaveToXML method.</param>
        /// <returns>True on successful loading and false otherwise.</returns>
        public bool LoadFromXML(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the state of the labels.
        /// </summary>
        /// <remarks>Serialized state holds information about visualization options, categories and text expressions. \n\n
        /// Optionally it can hold information about position and text of labels (see Labels.SavingMode property). 
        /// The default values of the properties will not be serialized.</remarks>
        /// <returns>A string with serialized state. On failure an empty string will be returned.</returns>
        public string Serialize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restores the state of the Labels class from the string.
        /// </summary>
        /// <param name="newVal">A string with serialized state generated by Labels.Serialize() method.</param>
        public void Deserialize(string newVal)
        {
            throw new NotImplementedException();
        }

        /// @}
        #endregion

        #region Labels visibility
        /// \addtogroup labels_visibility Labels visibility
        /// Here is list of properties and methods which affect visibility of labels. This module is a part of the documentation of Labels class.
        /// \dot
        /// digraph labels_visibility {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Labels" URL="\ref Labels"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Visibility"          URL="\ref labels_visibility"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Gets or sets the value which indicates whether the labels will be visible on the map.
        /// </summary>
        public bool Visible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets an expression which defines what shapes should be labelled on the map.
        /// </summary>
        /// <remarks>Applicable only to the instances of the Labels class associated with shapefile, i.e.
        /// obtained by Shapefile.Labels property.</remarks>
        /// \code
        /// Labels lb = new Labels();
        /// lb.VisibilityExpression = "[Area] > 100 and [population] > 100000";
        /// \endcode
        public string VisibilityExpression
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Is not implemented.
        /// </summary>
        public bool UseWidthLimits
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether labels with the same text should be drawn.
        /// </summary>
        /// <remarks>When set to true only the labels with unique text will be drawn on map. The choice of label 
        /// to draw depends on their position in the list and the fact of belonging to the visualization category.</remarks>
        public bool RemoveDuplicates
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the maximum scale at which labels are visible on the map.
        /// </summary>
        /// \see Labels.DynamicVisibility, AxMap.CurrentScale
        public double MaxVisibleScale
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the minimum size of the shape in pixels to be labelled at the current map scale.
        /// </summary>
        public int MinDrawingSize
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Get or sets the minimum scale at which labels are visible on the map.
        /// </summary>
        /// \see Labels.DynamicVisibility, AxMap.CurrentScale
        public double MinVisibleScale
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the minimum zoom at which labels are visible on the map.
        /// </summary>
        public int MinVisibleZoom { get; set; }

        /// <summary>
        /// Gets or sets the maximum zoom at which labels are visible on the map.
        /// </summary>
        public int MaxVisibleZoom { get; set; }

        /// <summary>
        /// Gets or sets the value which affects the collision avoidance routine.
        /// Two labels are considered overlapping when the distance between their bounding rectangles
        /// is less then this value.
        /// </summary>
        /// \see Labels.AvoidCollisions
        public int CollisionBuffer
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which turns the dynamic visibility on or off.
        /// </summary>
        /// <remarks>
        /// When set to true, labels will be visible only in the range of scales set by Labels.MinVisibleScale and Labels.MinVisibleScale.
        /// </remarks>
        public bool DynamicVisibility
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether a routine for preventing of label overlaps will be used.
        /// </summary>
        /// <remarks>When set to true overlapping labels won't be drawn. </remarks>
        public bool AvoidCollisions
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        ///@}

        #endregion

        #region Labels visualization
        /// \addtogroup labels_visualization Labels visualization
        /// Here is the list of visualization properties for labels. This module is a part of the documentation of Labels class.\n\n
        /// The more details on the topic in the description of the LabelCategory class. The properties listed below are used for all labels 
        /// without specific visualization category set, i.e. Label.Category == -1;
        /// \dot
        /// digraph labels_visualization {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Labels" URL="\ref Labels"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr  [label="Visualization"          URL="\ref labels_visualization"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Gets or sets the color of the label's shadow.
        /// </summary>
        /// \see ShadowVisible
        public uint ShadowColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the horizontal offset of the label's shadow.
        /// </summary>
        /// \see ShadowVisible
        public int ShadowOffsetX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the vertical offset of the label's shadow.
        /// </summary>
        /// \see ShadowVisible
        public int ShadowOffsetY
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether label's shadow is visible.
        /// </summary>
        public bool ShadowVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the color of the halo around label's text.
        /// </summary>
        /// \see Labels.HaloVisible
        public uint HaloColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the size of the halo drawn around label's text.
        /// </summary>
        /// \see Labels.HaloVisible
        public int HaloSize
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether label's halo is visible.
        /// </summary>
        public bool HaloVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether the font of labels is bold.
        /// </summary>
        public bool FontBold
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the color of the label font.
        /// </summary>
        public uint FontColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the second color of the label font. It is used to set color gradient for the font.
        /// </summary>
        /// \see Labels.FontGradientMode
        public uint FontColor2
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the gradient mode for the labels.
        /// </summary>
        /// \see Labels.FontColor2
        public tkLinearGradientMode FontGradientMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether the font of labels is italic.
        /// </summary>
        public bool FontItalic
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the name of the font to draw labels with, e.g. Arial.
        /// </summary>
        public string FontName
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Sets the color of the font outline.
        /// </summary>
        /// \see Labels.FontOutlineVisible
        public uint FontOutlineColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether font outline is visible. The default value is false.
        /// </summary>
        public bool FontOutlineVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the width of the font outline. The default value is 1.
        /// </summary>
        /// \see FontOutlineVisible
        public int FontOutlineWidth
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the size of the font to draw labels with.
        /// </summary>
        public int FontSize
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether the font of labels is struck out.
        /// </summary>
        public bool FontStrikeOut
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the transparency of the font. A values between 0 (transparent) to 255 (opaque) can be used.
        /// </summary>
        /// <remarks>The default value is 255. Values which fall outside the specified range will be 
        /// corrected to the closest acceptable value without reporting an error.</remarks>
        public int FontTransparency
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether the font of labels is struck out.
        /// </summary>
        public bool FontUnderline
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the color of the frame's background.
        /// </summary>
        /// 
        public uint FrameBackColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the second color of the frame's background.
        /// </summary>
        /// , Labels.FrameGradientMode
        public uint FrameBackColor2
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the type of gradient for the frame of the label. 
        /// </summary>
        /// <remarks>
        /// For all values other than gmNone a gradient
        /// starting from Labels.FrameBackColor and ending by Labels.FrameBackColor2 will be applied.</remarks>
        /// 
        public tkLinearGradientMode FrameGradientMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the outline color of the label's frame
        /// </summary>
        /// 
        public uint FrameOutlineColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the outline style of the label's frame. 
        /// </summary>
        /// 
        public tkDashStyle FrameOutlineStyle
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the width of the outline of the label's frame.
        /// </summary>
        /// 
        public int FrameOutlineWidth
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the horizontal distance between the text of the label and its frame.
        /// </summary>
        public int FramePaddingX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the vertical distance between the text of the label and its frame.
        /// </summary>
        public int FramePaddingY
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the transparency of the label's frame. A values between 0 (transparent) to 255 (opaque) can be used.
        /// </summary>
        /// <remarks>The default value is 255. Values which fall outside the specified range will be 
        /// corrected to the closest acceptable value without reporting an error.</remarks>
        public int FrameTransparency
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the shape of the label's frame.
        /// </summary>
        public tkLabelFrameType FrameType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the values which indicates whether label's frame is visible.
        /// </summary>
        public bool FrameVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets rendering hint to be used during GDI+ rendering.
        /// </summary>
        /// <remarks>This property will be ignored if GlobalSettings.AutoChooseRenderingHintForLabels is set to true.</remarks>
        public tkTextRenderingHint TextRenderingHint
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the formatting to be used for floating point numbers during generation of labels.
        /// </summary>
        /// <remarks>See acceptable formats in description of <a href = "http://www.cplusplus.com/reference/cstdio/printf/">C printf function</a>.
        /// The commonly used values may be: %g (shortest representation), %.2f (two decimal points).</remarks>
        /// \new493 Added in version 4.9.3
        public string FloatNumberFormat { get; set; }

        /// @}
        #endregion

        /// @}
        
    }
#if nsp
}
#endif


