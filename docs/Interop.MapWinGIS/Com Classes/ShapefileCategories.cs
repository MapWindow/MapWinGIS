
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Represents a list of visualization categories which determine the appearance of the shapefile.
    /// </summary>
    /// <remarks>
    /// Provides methods for:
    /// - managing categories: ShapefileCategories.Add, ShapefileCategories.Insert(), ShapefileCategories.Remove, ShapefileCategories.Clear();
    /// - generation of categories based on the specified attribute: ShapefileCategories.Generate();
    /// - mapping of categories to particular shapes: ShapefileCategories.ApplyExpressions();
    /// - setting of common color scheme for the range of categories: ShapefileCategories.ApplyColorScheme().
    /// - accessing particular categories: ShapefileCategories.get_Item().
    /// </remarks>
    /// Here is a diagram for the ShapefileCategories class.
    /// \dot
    /// digraph shapefilecategories_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// cat [ label="ShapefileCategory" URL="\ref ShapefileCategory"];
    /// 
    /// node [color = tan, peripheries = 1, height = 0.3, width = 1.0];
    /// lst [ label="ShapefileCategories" URL="\ref ShapefileCategories"];
    /// sdo [ label="ShapeDrawingOptions" URL="\ref ShapeDrawingOptions"];
    /// 
    /// node [style = dashed, color = gray];
    /// sf [ label="Shapefile" URL="\ref Shapefile"];
    /// 
    /// edge [ dir = none, style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6  ]
    /// cat -> sdo [ URL="\ref ShapefileCategory.DrawingOptions", tooltip = "ShapefileCategory.DrawingOptions", headlabel = "   1"];
    /// lst -> cat [ URL="\ref ShapefileCategories.get_Item()", tooltip = "ShapefileCategories.get_Item()", headlabel = "   n"];
    /// sf -> lst [ URL="\ref Shapefile.Categories", tooltip = "Shapefile.Categories", headlabel = "   1"];
    /// }
    /// 
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>
    ///  \new48 Added in version 4.8
    #if nsp
        #if upd
            public class ShapefileCategories : MapWinGIS.IShapefileCategories
        #else        
            public class IShapefileCategories
        #endif
    #else
        public class ShapefileCategories
    #endif
    {
        #region IShapefileCategories Members
        /// <summary>
        /// Creates a new visualization category, adds it to the list and returns its reference to the caller.
        /// </summary>
        /// It's the responsibility of the developer to map the new category to the 
        /// particular shape by either:
        /// - specifying ShapefileCategory.Expression and calling ShapefileCategories.ApplyExpression();
        /// - by using Shapefile.set_ShapeCategory() property;
        /// .
        /// <param name="Name">The name of the new category. It must not be unique.</param>
        /// <returns>The reference to the newly created category or NULL reference on failure.</returns>
        public ShapefileCategory Add(string Name)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates the specified number of visualization categories and expressions for them.
        /// </summary>
        /// <remarks>The method can be useful to override generation routine provided in ShapefileCategories.GenerateCategories()
        /// which always generate categories covering the full range of values of the specified field. This method can
        /// define the range of values explicitly, therefore certain values can be excluded from classification or on contrary
        /// categories can be added for the values which aren't yet present in the table.</remarks>
        /// <param name="FieldIndex">The index of the field to build classification by.</param>
        /// <param name="ClassificationType">The type of the classification.</param>
        /// <param name="numClasses">The number of categories to add. If ClassificationType equals ctUniqueValues this value will be skipped.</param>
        /// <param name="MinValue">The minimal value of the field to include in the classification.</param>
        /// <param name="MaxValue">Th maximum value of the field to include in the classification.</param>
        /// <returns>True on successful adding and false otherwise.</returns>
        public bool AddRange(int FieldIndex, tkClassificationType ClassificationType, int numClasses, object MinValue, object MaxValue)
        {
            throw new NotImplementedException();
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
        /// <param name="Type">The method of interpolation.</param>
        /// <param name="ColorScheme">The color scheme to take colors from.</param>
        /// <param name="ShapeElement">The element of the shape symbology to apply colors to.</param>
        public void ApplyColorScheme2(tkColorSchemeType Type, ColorScheme ColorScheme, tkShapeElements ShapeElement)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Applies color scheme to the visualization categories.
        /// </summary>
        /// <param name="Type">The method of interpolation.</param>
        /// <param name="ColorScheme">The color scheme to take colors from.</param>
        /// <param name="ShapeElement">The element of the shape symbology to apply colors to.</param>
        /// <param name="CategoryStartIndex">The index of the first category to apply colors to.</param>
        /// <param name="CategoryEndIndex">The index of the last category to apply colors to.</param>
        public void ApplyColorScheme3(tkColorSchemeType Type, ColorScheme ColorScheme, tkShapeElements ShapeElement, int CategoryStartIndex, int CategoryEndIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Maps particular shapes to the category based on ShapefileCategory.Expression.
        /// </summary>
        /// <remarks>
        /// The mapping between the category and shapes can be changed by Shapefile.set_ShapeCategory property.
        /// The row index parameters are zero-based and bounds-inclusive, meaning that if you want to
        /// apply expressions on a single row, you have to specifiy its row index as startRowIndex and endRowIndex.
        /// </remarks>
        /// <param name="CategoryIndex">The index of the category.</param>
        /// <param name="startRowIndex">Starting row index to apply expressions on. Negative values translate to the first row.</param>
        /// <param name="endRowIndex">Last row index to apply expressions on. Negative values translate to the last row.</param>
        public void ApplyExpression(int CategoryIndex, int startRowIndex = -1, int endRowIndex = -1)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Maps shapes to the visualization categories based in ShapefileCategory.Expression.
        /// </summary>
        /// <remarks>The mapping between the category and shapes can be changed by Shapefile.set_ShapeCategory property.</remarks>
        public void ApplyExpressions()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The name of the classification scheme to display in the legend.
        /// </summary>
        public string Caption
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Removes all the categories from the list.
        /// </summary>
        public void Clear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the number of the categories in the list.
        /// </summary>
        public int Count
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Restores the state of the object from the string.
        /// </summary>
        /// <param name="newVal">A string generated by ShapefileCategories.Serialize() method.</param>
        public void Deserialize(string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Generates visualization categories by certain attribute
        /// </summary>
        /// <param name="FieldIndex">The index of the field to generate categories by.</param>
        /// <param name="ClassificationType">The type of classification.</param>
        /// <param name="numClasses">Number of classes to generate. The parameter is omitted 
        /// in case unique values classification is used.</param>
        /// <returns>True on successful generation and false otherwise.</returns>
        public bool Generate(int FieldIndex, tkClassificationType ClassificationType, int numClasses)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the callback object which is used to return to the client the information about progress and errors.
        /// </summary>
        /// <remarks>An instance of the class which implements ICallback interface should be passed.
        /// The class must be implemented by caller.</remarks>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Creates a new visualization category and inserts it at specified position of the list.
        /// </summary>
        /// <param name="Index">The index to insert category at.</param>
        /// <param name="Name">The name of the new category.</param>
        /// <returns>Reference to the new category or NULL reference on failure.</returns>
        public ShapefileCategory Insert(int Index, string Name)
        {
            throw new NotImplementedException();
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
        /// Inserts a category at the specified index in the collection.
        /// </summary>
        /// <param name="Index">The index.</param>
        /// <param name="Category">The category to be inserted.</param>
        /// <returns>True on success.</returns>
        /// \new494 Added in version 4.9.4
        public bool Insert2(int Index, ShapefileCategory Category)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Retrieves the numeric code of the last error that took place in the class.
        /// </summary>
        /// <remarks>The usage of this property clears the error code.</remarks>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Moves the specified category down the list by swapping it with the succeeding category.
        /// </summary>
        /// <param name="Index">The index of the category to move down.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool MoveDown(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves the specified category up the list by swapping it with the preceding category.
        /// </summary>
        /// <param name="Index">The index of the category to move.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool MoveUp(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes the specified category.
        /// </summary>
        /// <param name="Index">The index of the category to remove.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool Remove(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the state of the object to the string.
        /// </summary>
        /// <returns>A string with serialized state.</returns>
        public string Serialize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// A reference to the parent shapefile.
        /// </summary>
        /// <remarks>NULL reference will be returned in case the categories aren't associated with a shapefile.</remarks>
        public Shapefile Shapefile
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the description of the specific error code.
        /// </summary>
        /// <param name="ErrorCode">The error code returned by ShapefileCategories.LastErrorCode</param>
        /// <returns>The string with the description</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the category with the specified index.
        /// </summary>
        /// <param name="Index">The index of the category to retrieve.</param>
        /// <returns>The reference to the category or NULL reference on failure.</returns>
        public ShapefileCategory get_Item(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Replaces the category with the specified index with the new one.
        /// </summary>
        /// <param name="Index">The index of the category to replace.</param>
        /// <param name="pVal">The new category.</param>
        public void set_Item(int Index, ShapefileCategory pVal)
        {
            throw new NotImplementedException();
        }

        #endregion

        /// <summary>
        /// Gets index of the specified category within a collection.
        /// </summary>
        /// <param name="Category">Category to find index for.</param>
        /// <returns>Index of category.</returns>
        /// \new491 Added in version 4.9.1
        public int get_CategoryIndex(ShapefileCategory Category)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets index of category by its name. The comparison is case-sensitive.
        /// </summary>
        /// <param name="categoryName">Category name to find index for.</param>
        /// <returns>Index of category.</returns>
        /// \new491 Added in version 4.9.1
        public int get_CategoryIndexByName(string categoryName)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Applies specific coloring to polygon layer based on 2 rules:
        /// - each polygon fill color must be different from the colors of all adjacent polygons;
        /// - the minimum overall number of colors must be used.
        /// </summary>
        /// <param name="Scheme">Color scheme to take colors from. If the number of color breaks 
        /// is smaller than the number of polygon colors, interpolation with ColorScheme.GetGraduatedColor
        /// method will be used.</param>
        /// <remarks>The coloring assumptions used by the method correspond to the formulation of 
        /// <a href="http://en.wikipedia.org/wiki/Four_color_theorem">Four color theorem</a>. But simple greedy
        /// algorithm used by the method usually employs somewhat more colors (5-6), 
        /// although provides much better performance than precise implementations.
        /// </remarks>
        /// <returns>True on success.</returns>
        /// \new492 Added in version 4.9.2
        public bool GeneratePolygonColors(ColorScheme Scheme = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Generates categories based on specified field.
        /// </summary>
        /// <param name="FieldName">Name of the field.</param>
        /// <param name="ClassificationType">Type of the classification.</param>
        /// <param name="numClasses">The number classes.</param>
        /// <returns>True on success.</returns>
        /// \new494 Added in version 4.9.4
        public bool Generate2(string FieldName, tkClassificationType ClassificationType, int numClasses)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds the category to the collection.
        /// </summary>
        /// <param name="Category">The category.</param>
        /// \new494 Added in version 4.9.4
        public void Add2(ShapefileCategory Category)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the index of classification that was used to generate categories.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public int ClassificationField
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
    }
#if nsp
}
#endif

