using System;
using MapWinGIS;

#if nsp
namespace MapWinGIS
{
#endif

    /// <summary>
    /// Provides a functionality for accessing and editing ESRI shapefiles.
    /// </summary>
    /// <remarks>A class which encapsulates all the operation with ESRI shapefiles, for both file-based and in-memory mode.
    /// Facilitates creation, editing, querying and geoprocessing of shapefiles. Shapefile holds geometry of objects 
    /// (.shp and .shx part) and their attribute values (.dbf part)</remarks>
    /// 
    /// Here is a diagram for the Shapefile class.
    /// \dot
    /// digraph shapefile_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.3, width = 0.8];
    /// mcat [ label="ShapefileCategory" URL="\ref ShapefileCategory"];
    /// pnt [ label="Point" URL="\ref Point"];
    /// lb [ label="Label" URL="\ref Label"];
    /// chart [ label="Chart" URL="\ref Chart"];
    /// fld [ label="Field" URL="\ref Field"];
    /// sf [ label="Shapefile" URL="\ref Shapefile"];
    /// node [width = 0.6, height = 0.2]
    /// shp [ label="Shape" URL="\ref Shape"];
    /// 
    /// node [color = tan, peripheries = 1, height = 0.3, width = 1.0];
    /// lst [ label="ShapefileCategories" URL="\ref ShapefileCategories"];
    /// tbl [ label="Table" URL="\ref Table"];
    /// lbs [ label="Labels" URL="\ref Labels"];
    /// cht [ label="Charts" URL="\ref Charts"];
    /// 
    /// node [style = dashed, color = gray];
    /// map [ label="AxMap" URL="\ref AxMap"];
    /// 
    /// edge [ dir = "none", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// sf -> lst [ URL="\ref Shapefile.Categories", tooltip = "Shapefile.Categories", headlabel = "   1"];
    /// sf -> tbl [ URL="\ref Shapefile.Table", tooltip = "Shapefile.Table", headlabel = "   1"];
    /// sf -> lbs [ URL="\ref Shapefile.Labels", tooltip = "Shapefile.Labels", headlabel = "   1"];
    /// sf -> cht [ URL="\ref Shapefile.Charts", tooltip = "Shapefile.Charts", headlabel = "   1"];
    /// sf -> shp [ URL="\ref Shapefile.get_Shape()", tooltip = "Shapefile.get_Shape()", headlabel = "   n"];
    /// shp -> pnt [ URL="\ref Shape.get_Point()", tooltip = "Shapefile.get_Point()", headlabel = "   n"];
    /// lst -> mcat [ URL="\ref ShapefileCategories.get_Item()", tooltip = "ShapefileCategories.get_Item()", headlabel = "   n"];
    /// tbl -> fld [ URL="\ref Table.get_Field()", tooltip = "Table.get_Field()", headlabel = "   n"];
    /// lbs -> lb [ URL="\ref Labels.get_Label()", tooltip = "Labels.get_Label()", headlabel = "   n"];
    /// cht -> chart [ URL="\ref Charts.get_Chart()", tooltip = "Charts.get_Chart()", headlabel = "   n"];
    /// map -> sf [ URL="\ref AxMap.get_Shapefile()", tooltip = "AxMap.get_Shapefile()", headlabel = "   n"];
    /// 
    /// }
    /// \enddot
    /// 
    /// <a href = "diagrams.html">Graph description</a>\n\n
    /// Here are groups of API members for Shapefile class:
    /// \dotfile shapefilegroups.dot
    /// <a href = "diagrams.html">Graph description</a>\n\n
    
    #if nsp
        #if upd    
            public class Shapefile : MapWinGIS.IShapefile
        #else        
            public class IShapefile 
        #endif
    #else
            public class Shapefile
    #endif
    {
        #region IShapefile Members

        /// <summary>
        /// Gets or sets an instance of ShapefileCategories class associated with the shapefile. 
        /// </summary>
        /// <remarks>The property can't be set to NULL (there is always an instance ShapefileCategories class associated with the Shapefile).</remarks>
        /// \new48 Added in version 4.8
        public ShapefileCategories Categories
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets expression to be set for OpenFileDialog.Filter property to select ESRI shapefiles.
        /// </summary>
        public string CdlgFilter
        {
            get { throw new NotImplementedException(); }
        }
       
        /// <summary>
        /// Sets collision mode for labels of the shapefile.
        /// </summary>
        /// <remarks>See tkCollisionMode enumeration.</remarks>
        /// \new48 Added in version 4.8
        public tkCollisionMode CollisionMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets an instance of ShapeDrawingOptions class which holds default drawing options. 
        /// </summary>
        /// <remarks>
        /// Default options are applied to every shape, which doesn't belong to a shapefile category.
        /// The property can't be set to NULL (there is always an instance ShapeDrawingOptions class associated with shapefile).
        /// </remarks>
        /// \new48 Added in version 4.8
        public ShapeDrawingOptions DefaultDrawingOptions
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Restores the state of shapefile from the string.
        /// </summary>
        /// <param name="LoadSelection">The value which indicates whether selection state of individual shapes should be restored.</param>
        /// <param name="newVal">A string with state information generated by Shapefile.Serialize method.</param>
        /// \new48 Added in version 4.8
        public void Deserialize(bool LoadSelection, string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the state of the shapefile in XML string. 
        /// </summary>
        /// <remarks>Serialization covers shapefile properties and all child classes (drawing options, labels, charts, categories). 
        /// Geometry of shapes and values of attribute table will not be serialized.</remarks>
        /// <param name="SaveSelection">The value which indicates whether selection state of individual shapes should be saved.</param>
        /// <returns>A string with serialized state or empty string on failure.
        /// Use Shapefile.ErrorMsg(Shapefile.LastErrorCode) to find out the reason of failure.</returns>
        /// \new48 Added in version 4.8
        public string Serialize(bool SaveSelection)
        {
            throw new NotImplementedException();
        }
       
        /// <summary>
        /// Gets bounding box which encompass all the shapes in the shapefile.
        /// </summary>
        /// <remarks> When fast mode is set on, Shapefile.RefreshExtents call is needed to get the correct 
        /// extents after edits where made.</remarks>
        public Extents Extents
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the handle of the shapefile. For inner use only.
        /// </summary>
        public int FileHandle
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the name of the file which is used as a source for this instance of Shapefile class.
        /// </summary>
        /// <remarks>
        /// The property should be used for disk-based shapefiles only (see Shapefile.SourceType property).
        /// </remarks>
        public string Filename
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Generates labels for the shapefile based on values of the specified attribute.
        /// </summary>
        /// <param name="FieldIndex">The index of field in the attribute table to take the values from.</param>
        /// <param name="Method">The method for calculation of label positions.</param>
        /// <param name="LargestPartOnly">A value which indicates whether all the parts of a multi-part shape should be supplied with individual 
        /// labels or only the largest (longest) one.</param>
        /// <returns>The number of labels generated. Normally it is equal to the number of shapes.
        /// Labels of the multi-part shape are treated like a single label.</returns>
        /// \new48 Added in version 4.8
        public int GenerateLabels(int FieldIndex, tkLabelPositioning Method, bool LargestPartOnly)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the instance of the Labels class associated with the shapefile. 
        /// </summary>
        /// <remarks>The property can't be set to NULL (there is always an instance Labels class associated with shapefile).</remarks> 
        /// \new48 Added in version 4.8
        public Labels Labels
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets an instance of Charts class associated with the shapefile. 
        /// </summary>
        /// <remarks>The property can't be set to NULL (there is always an instance of Charts class associated with shapefile).</remarks>
        /// \new48 Added in version 4.8
        public Charts Charts
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Get or sets an instance of GeoProjection class associated with the shapefile.
        /// </summary>
        /// <remarks>The property can't be set to NULL (there is always an instance of GeoProjection class associated with the shapefile).</remarks>
        /// \new48 Added in version 4.8
        public GeoProjection GeoProjection
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a Callback object which handles progress and error messages of the Shapefile class.
        /// </summary>
        /// <remarks>
        /// It's recommended to set it in case time consuming operation will be used (geoprocessing, generation of labels, etc).
        /// The property is equal to NULL by default.</remarks>
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Sets an object which implements IStopExecution interface and facilitates stopping of the 
        /// geoprocessing operations.
        /// </summary>
        /// \new48 Added in version 4.8
        public IStopExecution StopExecution
        {
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// A text string associated with the shapefile. Any value can be stored by developer in this property.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the code of last error which took place inside this object.
        /// </summary>
        /// <remarks>
        /// To retrieve text description of error, use Shapefile.get_ErrorMsg(Shapefile.LastErrorCode).
        /// Check this value if a certain method has failed.</remarks>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the number of shapes in the shapefile.
        /// </summary>
        public int NumShapes
        {
            get { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Gets or sets the projection string for shapefile. String in proj4 and ESRI WKT formats are supported.
        /// </summary>
        /// \deprecated v 4.8 Use Shapefile.GeoProjection property instead.
        public string Projection
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Gets the type of the shapefile.
        /// </summary>
        /// <remarks>It is not possible to change the type of the shapefile without closing it.</remarks>
        public ShpfileType ShapefileType
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the value which indicate the type the data used by shapefile object.
        /// </summary>
        /// <remarks>See details in the description of tkShapefileSourceType enumeration.</remarks>
        /// \new48 Added in version 4.8
        public tkShapefileSourceType SourceType
        {
            get { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Gets or sets the expression which defines shapes to be visible on the map.
        /// </summary>
        /// <remarks>In case of empty or invalid string all the shapes will be visible.</remarks>
        /// \new48 Added in version 4.8
        public string VisibilityExpression
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the description of the specific error code.
        /// </summary>
        /// <param name="ErrorCode">The error code returned by Shapefile.LastErrorCode.</param>
        /// <returns>The string with the description.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Gets the index of the visualization category which is used for the drawing of the specified shape.
        /// </summary>
        /// <param name="ShapeIndex">The index of the shape.</param>
        /// <returns>The index of the category. -1 will be returned in case no category is set to the shape or 
        /// invalid shape index was passed.</returns>
        /// \see Shapefile.Categories
        /// \new48 Added in version 4.8
        public int get_ShapeCategory(int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the index of the visualization category to use for the drawing of the specified shape.
        /// </summary>
        /// <param name="ShapeIndex">The index of the shape.</param>
        /// <param name="pVal">The index of the visualization category.</param>
        /// \see Shapefile.Categories
        /// \new48 Added in version 4.8
        public void set_ShapeCategory(int ShapeIndex, int pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Get the name of the ShapefileCategory for the shape with specified index.
        /// </summary>
        /// <param name="ShapeIndex">Index of shape.</param>
        /// <returns>The name of the category.</returns>
        /// \new491 Added in version 4.9.1
        public string get_ShapeCategory2(int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the shapefile category for the shape.
        /// </summary>
        /// <param name="ShapeIndex">Index of shape.</param>
        /// <param name="categoryName">Name of the category. Category must be created first. Name comparison is case-sensitive.</param>
        /// \new491 Added in version 4.9.1
        public void set_ShapeCategory2(int ShapeIndex, string categoryName)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the shapefile category for the shape with specified index.
        /// </summary>
        /// <param name="ShapeIndex">Index of shape.</param>
        /// <returns>Shapefile category, or null if no category for the shape was set.</returns>
        /// \new491 Added in version 4.9.1
        public ShapefileCategory get_ShapeCategory3(int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the shapefile category for the shape.
        /// </summary>
        /// <param name="ShapeIndex">Index of shape.</param>
        /// <param name="Category">Shapefile category to set. The category must be added to shapefile beforehand.</param>
        /// \new491 Added in version 4.9.1
        public void set_ShapeCategory3(int ShapeIndex, ShapefileCategory Category)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the reference to the specified shape.
        /// </summary>
        /// <remarks>In the disk-based mode (Shapefile.SourceType = sstDiskBased) a new instance of the Shape class will be created
        /// on each call of this property which is time consuming. Try to avoid calling this property in large cycles or 
        /// open editing mode prior such cycles. In editing mode shape objects stored in memory, so there is no overhead in passing the 
        /// reference.
        /// </remarks>
        /// <param name="ShapeIndex">The index of shape to retrieve.</param>
        /// <returns>The reference to the shape or NULL reference on invalid index.</returns>
        public Shape get_Shape(int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        #endregion

        /// The modules listed here are parts of the documentation of Shapefile class.
        /// \dotfile shapefilegroups.dot
        /// <a href = "diagrams.html">Graph description</a>
        /// \addtogroup Shapefile_modules Shapefile modules
        ///  @{

        #region Attribute table
        /// \addtogroup shapefile_table Shapefile attribute table
        /// Here is a list of properties and methods to for interaction with attribute table associated with a shapefile. 
        /// This module is a part of the documentation of Shapefile class.
        ///  \dot
        /// digraph shapefile_attribute_table {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Shapefile" URL="\ref Shapefile"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Attribute table"          URL="\ref shapefile_table"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Gets the value of the specified field for the shape.
        /// </summary>
        /// <remarks> The value will be returned as VARIANT data type. When passing to then .NET it will be converted 
        /// to either integer, double or string data type depending on the type of field.</returns></remarks>
        /// <param name="FieldIndex">The index of field.</param>
        /// <param name="ShapeIndex">The index of shape.</param>
        /// <returns>The value stored in the field.</returns>
        public object get_CellValue(int FieldIndex, int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a specific field stored in the attribute table.
        /// </summary>
        /// <param name="FieldIndex">The index of the field to be retrieved.</param>
        /// <returns>Reference to the field object or NULL reference on failure.</returns>
        public Field get_Field(int FieldIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets specific field stored in the attribute table.
        /// </summary>
        /// <param name="Fieldname">The name of the field to be retrieved. The comparison of the names is case insensitive.</param>
        /// <returns>Reference to the field object or NULL reference on failure.</returns>
        public Field get_FieldByName(string Fieldname)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the new value for particular cell in attribute table. The table must be in editing mode.
        /// </summary>
        /// <param name="FieldIndex">The index of field in attribute table.</param>
        /// <param name="ShapeIndex">The index of shape (row number in attribute table).</param>
        /// <param name="newVal">The variant value to be passed (integer, double and string values are accepted depending on field type).</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool EditCellValue(int FieldIndex, int ShapeIndex, object newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deletes a field with the specified index from the attribute table. The table must be in editing mode.
        /// </summary>
        /// <param name="FieldIndex">An index of field to be deleted.</param>
        /// <param name="cBack">An instance of class implementing ICallback interface. 
        /// It's recommended to pass NULL and use Shapefile.GlobalCallback property instead.</param>
        /// <returns>Returns true on success and false otherwise</returns>
        public bool EditDeleteField(int FieldIndex, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inserts a new field in the shapefile attribute table. The table must be in editing mode.
        /// </summary>
        /// <param name="NewField">A new instance of field object to insert.</param>
        /// <param name="FieldIndex">A position to insert the new field. An invalid index will be automatically substituted with 0 or Shapefile.NumFields.</param>
        /// <param name="cBack">An instance of class implementing ICallback interface. 
        /// It's recommended to pass NULL and use Shapefile.GlobalCallback property instead.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool EditInsertField(Field NewField, ref int FieldIndex, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value indicating whether editing operations are allowed for shapefile attribute table.
        /// </summary>
        /// <remarks>Use Shapefile.StartEditingTable and Shapefile.StopEditingTable to control editing mode.</remarks>
        public bool EditingTable
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the number of fields in attribute table of the shapefile.
        /// </summary>
        public int NumFields
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the reference to the attribute table associated with the shapefile.
        /// </summary>
        public Table Table
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Opens editing mode for the attribute table.
        /// </summary>
        /// <param name="cBack">A callback object to report errors and progress information.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool StartEditingTable(ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Closes the editing mode for the attribute table.
        /// </summary>
        /// <param name="ApplyChanges">A boolean value which indicates whether changes made to the memory version of table should be saved to the drive.</param>
        /// <param name="cBack">A callback object to report errors and progress information.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool StopEditingTable(bool ApplyChanges, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds a field to the attribute table of the shapefile. The table must be in editing mode.
        /// </summary>
        /// <param name="Name">The name of field.</param>
        /// <param name="Type">The type of field.</param>
        /// <param name="Precision">The precision of field.</param>
        /// <param name="Width">The width of field.</param>
        /// <returns>The index of the new field or -1 on failure.</returns>
        public int EditAddField(string Name, FieldType Type, int Precision, int Width)
        {
            throw new NotImplementedException();
        }

        /// @}
        #endregion

        #region Editing
        /// \addtogroup shapefile_editing Shapefile editing
        /// Here is a list of properties and methods which control the editing of shapefile. 
        /// This module is a part of the documentation of Shapefile class.
        /// \dot
        /// digraph shapefile_editing {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Shapefile" URL="\ref Shapefile"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Editing" URL="\ref shapefile_editing"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Deletes all the shapes from the shapefile.
        /// </summary>
        /// <remarks>Both shapefile and attribute table must be in editing mode for the successful operation.
        /// The records of atribute table will be removed as well to ensure shapefile integrity.</remarks>
        /// <returns>Returns true on success and false otherwise.</returns>
        public bool EditClear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deletes a shape from the shapefile.
        /// </summary>
        /// <remarks>Both shapefile and attribute table must be in editing mode for the successful operation.
        /// Corresponding record will be removed from attribute table to ensure shapefile integrity.
        /// </remarks>
        /// <param name="ShapeIndex">The index of shape to delete.</param>
        /// <returns>Returns true on success and false otherwise.</returns>
        /// \see Shapefile.EditingShapes, Shapefile.EditingTable
        public bool EditDeleteShape(int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inserts a new shape in the shapefile.
        /// </summary>
        /// <remarks>Both shapefile and attribute table must be in editing mode for the successful operation.
        /// A record will be automatically added to the attribute table to ensure shapefile integrity.</remarks>
        /// <param name="Shape">The shape object to insert.</param>
        /// <param name="ShapeIndex">The index to insert the new shape at.
        /// An invalid index will be automatically substituted with closest allowable value without reporting the error.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool EditInsertShape(Shape Shape, ref int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value indicating whether editing operations are allowed for shapefile.
        /// </summary>
        /// <remarks>Shapefile.EditInsertShape, Shapefile.EditDeleteShape, Shapefile.EditClear are affected by this property.
        /// \n Use Shapefile.StartEditingShapes and Shapefile.StopEditingShapes to control the editing mode.</remarks>
        public bool EditingShapes
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Starts editing mode for the shapefile.
        /// </summary>
        /// <remarks>In case editing mode has already been opened the new call will return true which 
        /// is different from the behaviour of the earlier versions (before 4.8).</remarks>
        /// <param name="StartEditTable">A value which indicate whether or not the editing mode will be opened
        /// for attribute table associated with the shapefile.</param>
        /// <param name="cBack">A callback object to report errors and progress information.</param>
        /// <returns>Returns true on successful opening of the editing mode and false on failure.</returns>
        public bool StartEditingShapes(bool StartEditTable, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Stops editing modes for the shapefile and optionally saves the changes.
        /// </summary>
        /// <param name="ApplyChanges">The value which indicates whether changes made to the shapefile should be saved.</param>
        /// <param name="StopEditTable">The value which indicates whether editing mode for associated attribute table
        /// should be closed as well.</param>
        /// <param name="cBack">A callback object to report errors and progress information.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool StopEditingShapes(bool ApplyChanges, bool StopEditTable, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds a new shape to the shapefile.
        /// </summary>
        /// <param name="Shape">The shape to be added.</param>
        /// <returns>The index of the new shape or -1 on failure.</returns>
        public int EditAddShape(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// @}

        #endregion
            
        #region Geoprocessing

        /// \addtogroup shapefile_geoprocessing Shapefile geoprocessing
        /// Here is a list of methods to perform geoprocessing tasks using shapefile data.
        /// This module is a part of the documentation of Shapefile class.
        /// \dot
        /// digraph shapefile_geoprocessing {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Shapefile" URL="\ref Shapefile"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Geoprocessing"          URL="\ref shapefile_geoprocessing"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Creates a new shapefile by creating multi-part shapes from shapes with the same value of specified attribute.
        /// </summary>
        /// <remarks>See sample code in description of Shapefile.DissolveWithStats.</remarks>
        /// <param name="SelectedOnly">A value which indicates whether all or only selected shapes will be processed.</param>
        /// <param name="FieldIndex">The index of field to group shapes by.</param>
        /// <param name="statOperations">Statistic group operations to be calculated for fields of input shapefile
        /// and written to attribute table of output shapefile.</param>
        /// <returns>Reference to the shapefile on success or NULL reference on failure.</returns>
        /// \new491 Added in version 4.9.1
        public Shapefile AggregateShapesWithStats(bool SelectedOnly, int FieldIndex = -1, FieldStatOperations statOperations = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Merges shapes of the input shapefile based on specified attribute.
        /// </summary>
        /// <remarks>All shapes with the same value of the attribute will be merged in a single shape.
        /// If the shapes aren't adjacent, then multi-part shape will be created.
        /// \code
        ///  const string filename = @"d:\counties.shp";
        ///  var sf = new Shapefile();
        ///  if (!sf.Open(filename, null))
        ///  {
        ///     MessageBox.Show("Failed to open shapefile");
        ///  }
        ///  else
        ///  {
        ///     const int stateNameFieldIndex = 1;
        ///     var operations = new FieldStatOperations();
        ///  
        ///     operations.AddFieldName("SUB_REGION", tkFieldStatOperation.fsoMin);
        ///     operations.AddFieldName("sub_region", tkFieldStatOperation.fsoMax);   // casing doesn't matter
        /// 
        ///     operations.AddFieldName("pop1990", tkFieldStatOperation.fsoSum); 
        ///     operations.AddFieldName("pop1990", tkFieldStatOperation.fsoAvg);  
        ///     operations.AddFieldName("pop1990", tkFieldStatOperation.fsoWeightedAvg);
        /// 
        ///     var result = sf.DissolveWithStats(stateNameFieldIndex, false, operations);
        ///     axMap1.RemoveAllLayers();
        ///     axMap1.AddLayer(result, true);
        ///     result.SaveAs(@"d:\dissolve_result.shp");
        /// }
        /// \endcode
        /// </remarks>
        /// <param name="FieldIndex">An index of field from attribute table of the shapefile to dissolve by.</param>
        /// <param name="SelectedOnly">A boolean value which indicates whether all or only selected shapes will be processed.</param>
        /// <param name="statOperations">Statistic group operations to be calculated for fields of input shapefile
        /// and written to attribute table of output shapefile.</param>
        /// <returns>Reference to the output shapefile or NULL reference on failure.</returns>
        /// \new491 Added in version 4.9.1
        public Shapefile DissolveWithStats(int FieldIndex, bool SelectedOnly, FieldStatOperations statOperations = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new shapefile by creating multi-part shapes from shapes with the same value of specified attribute.
        /// </summary>
        /// <param name="SelectedOnly">A boolean value which indicates whether all or only selected shapes will be processed.</param>
        /// <param name="FieldIndex">The index of field to group shapes by.</param>
        /// <returns>Reference to the shapefile on success or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile AggregateShapes(bool SelectedOnly, int FieldIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new shapefile by building a buffer around the shapes of the input shapefile.
        /// </summary>
        /// <param name="Distance">The distance to build buffer at.</param>
        /// <param name="nSegments">Number of segments to approximate a circular buffer.</param>
        /// <param name="SelectedOnly">A value which indicates whether all only selected shapes of the 
        /// shapefile will be processed.</param>
        /// <param name="MergeResults">A value which indicates whether overlapping buffered shapes
        /// will be merged into a single shapes.</param>
        /// <returns>A reference to the output shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile BufferByDistance(double Distance, int nSegments, bool SelectedOnly, bool MergeResults)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clips current shapefile by the definition shapefile.
        /// </summary>
        /// <param name="SelectedOnlySubject">A value which indicates whether all or only selected shape of this shapefile
        /// will be treated as input.</param>
        /// <param name="sfOverlay">The definition shapefile.</param>
        /// <param name="SelectedOnlyOverlay">A value which indicates whether all or only selected shape of the definition shapefile
        /// will be treated as input.</param>
        /// <returns>Reference to the output shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile Clip(bool SelectedOnlySubject, Shapefile sfOverlay, bool SelectedOnlyOverlay)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates difference of 2 shapefiles.
        /// </summary>
        /// <param name="SelectedOnlySubject">A value which indicates whether all or only selected shape of this shapefile
        /// will be treated as input.</param>
        /// <param name="sfOverlay">The overlay shapefile.</param>
        /// <param name="SelectedOnlyOverlay">A value which indicates whether all or only selected shape of the overlay shapefile
        /// will be treated as input.</param>
        /// <returns>Reference to the output shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile Difference(bool SelectedOnlySubject, Shapefile sfOverlay, bool SelectedOnlyOverlay)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Merges shapes of the input shapefile based on specified attribute.
        /// </summary>
        /// <remarks>All shapes with the same value of the attribute will be merged in a single shape.
        /// If the shapes aren't adjacent, then multi-part shape will be created.</remarks>
        /// <param name="FieldIndex">An index of field from attribute table of the shapefile to dissolve by.</param>
        /// <param name="SelectedOnly">A boolean value which indicates whether all or only selected shapes will be processed.</param>
        /// <returns>Reference to the output shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile Dissolve(int FieldIndex, bool SelectedOnly)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new instance of shapefile class with single-part shapes produced from the 
        /// multi-part shapes of the input shapefile. </summary>
        /// <remarks>Single-part input shapes are moved to the output shapefile 
        /// without changes. Attribute values are copied to the output shapefile without changes.</remarks>
        /// <param name="SelectedOnly">A value which indicates whether the operation will be applied to the selected shapes only.</param>
        /// <returns>A new instance of Shapefile class with resultant shapes or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile ExplodeShapes(bool SelectedOnly)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new instance of the shapefile class and copies selected shapes of the input shapefile to it.
        /// </summary>
        /// <returns>A new instance of shapefile class with selected shapes or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile ExportSelection()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new shapefile by fixing invalid shapes of the input shapefile.
        /// </summary>
        /// <param name="retval">The output shapefile.</param>
        /// <returns>True on successful fixing of all shapes and false if at least one shapes wasn't fixed. </returns>
        /// \new48 Added in version 4.8
        public bool FixUpShapes(out Shapefile retval)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the engine (library) to use for geoprocessing operations.
        /// </summary>
        /// <remarks>The following methods are affected by this property.</remarks>
        /// \new48 Added in version 4.8
        public tkGeometryEngine GeometryEngine
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Calculates intersection of 2 input shapefiles.
        /// </summary>
        /// <remarks>Intersection can generate shapes of different types. For example, the intersection of 2 polygons can be
        /// a polygon, a polyline, a point or any combination of those. With SHP_NULLSHAPE fileType parameter the most obvious type will be used:
        /// for example SHP_POLYGON for 2 polygon shapefiles.</remarks>
        /// <param name="SelectedOnlyOfThis">A value which indicates whether operation will be applied to the selected shapes of the current shapefile only.</param>
        /// <param name="sf">The second shapefile to perfrom intersection.</param>
        /// <param name="SelectedOnly">A value which indicates whether operation will be applied to the selected shapes of the second shapefile only.</param>
        /// <param name="fileType">The type of output shapefiles. SHP_NULLSHAPE value should be passed for automatic choosing of type.</param>
        /// <param name="cBack">An instance of class implementing ICallback interface.</param>
        /// <returns>Reference to the output shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile GetIntersection(bool SelectedOnlyOfThis, Shapefile sf, bool SelectedOnly, ShpfileType fileType, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new instance of shapefile class which holds shapes from 2 input shapefiles. 
        /// </summary>
        /// <remarks>
        /// Both shapefiles must have the same ShpfileType. No changes to geometry of individual shapes are made.
        /// Attribute values of both input shapefile will be passed to the resulting one.
        /// </remarks>
        /// <param name="SelectedOnlyThis">The value which indicates whether the operation will be applied 
        /// only to the selected shapes of the first shapefile.</param>
        /// <param name="sf">The second shapefile to take shapes from.</param>
        /// <param name="SelectedOnly">A value which Indicates whether the operation will be applied 
        /// only to the selected shapes of the second shapefile</param>
        /// <returns>Reference to the output shapefile or NULL reference on failure</returns>
        /// \new48 Added in version 4.8
        public Shapefile Merge(bool SelectedOnlyThis, Shapefile sf, bool SelectedOnly)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new shapefile by re-projecting the current one.
        /// </summary>
        /// <param name="newProjection">The projection of the output shapefile.</param>
        /// <param name="reprojectedCount">The output value with the number of shapes successfully reprojected.</param>
        /// <returns>Reference to the output shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile Reproject(GeoProjection newProjection, ref int reprojectedCount)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Reprojects shapes of the current shapefile.
        /// </summary>
        /// <param name="newProjection">The new projection.</param>
        /// <param name="reprojectedCount">The number of shapes successfully reprojected.</param>
        /// <returns>Returns true on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool ReprojectInPlace(GeoProjection newProjection, ref int reprojectedCount)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// aka SpatialQuery. Returns an array with indices of shapes of the current shapefile which 
        /// are in the specified relation to the definition shapefile. 
        /// </summary>
        /// <param name="sf">The definition shapefile.</param>
        /// <param name="Relation">The target relation between shapes of the 2 shapefiles.</param>
        /// <param name="SelectedOnly">The value which indicates whether only selected shapes of the definition shapefile will be used.</param>
        /// <param name="Result">The resulting array of long data type with shape indices.</param>
        /// <param name="cBack">A callback object for passing to the client 
        /// the information about the progress and errors.</param>
        /// <returns>True in case at least one shape was selected and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool SelectByShapefile(Shapefile sf, tkSpatialRelation Relation, bool SelectedOnly, ref object Result, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new shapefile by removing excessive vertices from the current shapefile
        /// </summary>
        /// <remarks>Applicable to the polyline or polygon shapefiles only.
        /// The initial shapefile remains intact. Attribute values are copied to the output shapefile without changes.</remarks>
        /// <param name="Tolerance">The minimal distance between 2 succeeding points. A succeeding point lying closer than
        /// this value, will be removed.</param>
        /// <param name="SelectedOnly">A value which indicates whether all or only selected shapes will be processed.</param>
        /// <returns>A reference to the output shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile SimplifyLines(double Tolerance, bool SelectedOnly)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sorts shapes in the shapefile based on the specified attribute.
        /// </summary>
        /// <param name="FieldIndex">The index of field to set the sorting order.</param>
        /// <param name="Ascending">A value which defines the sorting order.</param>
        /// <returns>Reference to the output shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile Sort(int FieldIndex, bool Ascending)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates symmetrical difference of the 2 shapefiles. 
        /// </summary>
        /// <remarks>The operation consists of 2 mirror-like Difference operations.
        /// Attribute values of both shapefiles are passed to the output one without changes.
        /// Both input shapefiles remain intact.</remarks>
        /// <param name="SelectedOnlySubject">A value which indicates that only selected shapes of the first shapefile will be processed.</param>
        /// <param name="sfOverlay">The second shapefile.</param>
        /// <param name="SelectedOnlyOverlay">A value which indicates that only selected shapes of the second shapefile will be processed.</param>
        /// <returns>A reference to the output shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile SymmDifference(bool SelectedOnlySubject, Shapefile sfOverlay, bool SelectedOnlyOverlay)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates union of 2 shapefiles.
        /// </summary>
        /// <remarks>The operation consists of intersection operation and 2 mirror-like difference operations.</remarks>
        /// <param name="SelectedOnlySubject">The value which indicates that only selected shapes of the first shapefile will be processed.</param>
        /// <param name="sfOverlay">The second shapefile.</param>
        /// <param name="SelectedOnlyOverlay">The value which indicates that only selected shapes of the second shapefile will be processed.</param>
        /// <returns>A reference to the output shapefile or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shapefile Union(bool SelectedOnlySubject, Shapefile sfOverlay, bool SelectedOnlyOverlay)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Splits polylines at intersection with any other polylines. Works for polyline shapefile only.
        /// </summary>
        /// <returns>Resulting shapefile or null if the operation wasn't successful.</returns>
        /// \new490 Added in version 4.9.0
        public Shapefile Segmentize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears GEOS geometries cached during certain geoprocessing tasks.
        /// </summary>
        /// <remarks>Shapefile.GetRelatedShapes currently is the only method that uses such caching
        /// to improve performance if search is performed multiple times for different shapes.</remarks>
        /// \new491 Added in version 4.9.1
        public void ClearCachedGeometries()
        {
            throw new NotImplementedException();
        }

        /// @}
        #endregion
            
        #region Management
        /// \addtogroup shapefile_management Shapefile open and close
        /// Here is a list of methods which affect the state of the shapefile. 
        /// This module is a part of the documentation of Shapefile class.
        /// \dot
        /// digraph shapefile_management {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Shapefile" URL="\ref Shapefile"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Management"          URL="\ref shapefile_management"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Loads data into in-memory shapefile from the specified shapefile on the disk. 
        /// </summary>
        /// <param name="ShapefileName">Filename of the shapefile to load from.</param>
        /// <param name="cBack">Callback interface.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// <remarks>The shapefile object with remain in in-memory state.</remarks>
        /// \new491 Added in version 4.9.1
        public bool LoadDataFrom(string ShapefileName, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        ///  Writes the contents of the shapefile to specified file. 
        /// </summary>
        /// <param name="ShapefileName">Filename to write shapefile data to.</param>
        /// <param name="cBack">Callback interface.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// <remarks>The state of shapefile won't be affected, i.e. in-memory shapefile will remain in-memory one.</remarks>
        /// \new491 Added in version 4.9.1
        public bool Dump(string ShapefileName, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>Creates a copy of the shapefile.</summary>
        /// <remarks>Output shapefile will have the same type, projection and fields of the attribute table,
        /// while shapes and the records of the attribute table aren't copied.
        /// For complete copy of the shapefile use Shapefile.SelectAll() and Shapefile.ExportSelection().</remarks>
        /// \new48 Added in version 4.8
        public Shapefile Clone()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Closes shapefile and releases all the resources. 
        /// </summary>
        /// <remarks>In case shapefile is in editing mode (Shapefile.EditingShapes = true) 
        /// all the edits will be discarded.</remarks>
        /// <returns>This value should not be analysed.</returns>
        public bool Close()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Initializes in-memory shapefile of the specified type. 
        /// </summary>
        /// <remarks>Shapefile.EditingShapes property for the new shapefile will be set to true. 
        /// Any shapefile opened in this instance of class before this call, will be closed without saving the changes.</remarks>
        /// <param name="ShapefileName">The name of the new shapefile. An empty string should be passed here.</param>
        /// <param name="ShapefileType">Type of the shapefile to create.</param>
        /// <returns>Returns true on success and false otherwise.</returns>
        /// Use Shapefile.ErrorMsg(Shapefile.LastErrorCode) to find out the reason of failure.</returns>
        public bool CreateNew(string ShapefileName, ShpfileType ShapefileType)
        {
            throw new NotImplementedException();
        }

        
        /// <summary>
        /// Initializes in-memory shapefile of the specified type with unique index field.
        /// </summary>
        /// <remarks>MWShapeID field will be added to the attribute table. An increasing unique positive integer value 
        /// will be set in this field for each shape added to the shapefile. The field with MWShapeID name
        /// created manually will be treated in the same way.</remarks>
        /// <param name="ShapefileName">The name of the new shapefile. An empty string should be passed here.</param>
        /// <param name="ShapefileType">Type of the shapefile to create.</param>
        /// <returns>Returns true on success and false otherwise.</returns>
        public bool CreateNewWithShapeID(string ShapefileName, ShpfileType ShapefileType)
        {
            throw new NotImplementedException();
        }
       
        /// <summary>
        /// Opens shapefile from the disk. 
        /// </summary>
        /// <remarks>Any other shapefile currently opened will be closed without saving the changes.
        /// Shapefile.SourceType property will be set to sstDiskBased.</remarks>
        /// <param name="ShapefileName">The name of the file to open (.shp extension).</param>
        /// <param name="cBack">A callback object to report errors and progress information.</param>
        /// <returns>True on success and false otherwise. 
        /// Use Shapefile.get_ErrorMsg(Shapefile.LastErrorCode) to find out the reason of failure.</returns>
        public bool Open(string ShapefileName, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Changes file associated with the current instance of the shapefile class.
        /// </summary>
        /// <remarks>It is equivalent to consequent calling of Shapefile.Close and Shapefile.Open.</remarks>
        /// <param name="newShpPath">The filename of the new shapefile to be opened.</param>
        /// <returns>True on successful opening of the new shapefile or false otherwise.</returns>
        public bool Resource(string newShpPath)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves edits of the in-memory shapefile to the disk without closing editing mode.
        /// </summary>
        /// <remarks>Applicable only when Shapefile.EditingShapes = true.</remarks>
        /// <param name="cBack">A callback object to report errors and progress information.</param>
        /// <returns>True on successful saving and false otherwise.</returns>
        public bool Save(ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves in-memory shapefile to the disk.
        /// </summary>
        /// <param name="ShapefileName">The name of the file to save data in.</param>
        /// <param name="cBack">A callback object to report errors and progress information.</param>
        /// <returns>True on successful saving and false otherwise.</returns>
        public bool SaveAs(string ShapefileName, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// @}
        #endregion

        #region Optimizations
        /// \addtogroup shapefile_optimizations Shapefile optimizations
        /// Here is a list of properties and methods which doesn't provide additional functionality but 
        /// can improve the speed of certain operations with shapefile.
        /// This module is a part of the documentation of Shapefile class.
        /// \dot
        /// digraph shapefile_optimizations {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Shapefile" URL="\ref Shapefile"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Optimizations"          URL="\ref shapefile_optimizations"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Gets or sets a boolean value which indicates whether extents of shape will be recalculated before each redraw.
        /// </summary>
        /// <remarks>When the property is set to:
        /// - false: MapWinGIS recalculates the extents of each shape before the redraw assuming that some 
        /// changes *MIGHT* took place. The operation can slow down the drawing for large shapefiles.\n\n
        /// - true: MapWinGIS doesn't recalculate extents of the shapes which leads to faster drawing but also 
        /// makes a developer responsible for updating the extents of shapes in the due time.
        /// To do it one need to call Shapefile.RefreshShapeExtents after each editing operation. Otherwise
        /// there is a chance that old extents of the shape may lie outside the visible area of the map and the shape won't be displayed.\n
        /// .
        /// When Shapefile.FastMode is turned on the extents are cached automatically in spite of this property.
        /// Also the extents will be automatically refreshed after Shape.put_XY call. However
        /// Shape.get_Point().x = [value] call will not be tracked. Therefore Shapefile.RefreshShapeExtents will still be needed.\n
        /// 
        /// To avoid confusion use the following recommendation:
        /// -# If you work with small shapefile and performance isn't a problem, set this property to false.\n\n
        /// -# If the performance is an issue:
        ///     - if you write a new code, set this property to true (as well as Shapefile.FastMode), 
        ///     but assign new coordinates to the points by calling Shape.put_XY only;
        ///     - if you are bound to the exiting code with Shape.get_Point.x = [value] calls, set this property to true but call 
        ///     Shapefile.RefreshShapeExtents after the editing operations.</remarks>
        ///     .
        /// .
        /// \new48 Added in version 4.8
        public bool CacheExtents
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Caches the coordinates of shapefile points for faster point in shape test.
        /// </summary>
        /// <remarks>Shapefile.EndPointInShapefile call is needed to release the memory.</remarks>
        /// <returns>True on successful caching and false otherwise.</returns>
        public bool BeginPointInShapefile()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears the cached data allocated by Shapefile.BeginPointInShapefile.
        /// </summary>
        public void EndPointInShapefile()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns boolean value which indicates whether the given point is located within the specified shape.
        /// </summary>
        /// <remarks>The operation is applicable for polygon shapefiles only.</remarks>
        /// <param name="ShapeIndex">The index shape (polygon) to perform the test.</param>
        /// <param name="x">X coordinate of the point.</param>
        /// <param name="y">Y coordinate of the point.</param>
        /// <returns>True in case the point is located within polygon and false otherwise.</returns>
        public bool PointInShape(int ShapeIndex, double x, double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a number which indicates the index of shapes within which a test point is situated.
        /// </summary>
        /// <remarks>Call Shapefile.BeginPointInShapefile to improve performance.</remarks>
        /// <param name="x">The x coordinate of the point.</param>
        /// <param name="y">The y coordinate of the point.</param>
        /// <returns>The index of shape.</returns>
        public int PointInShapefile(double x, double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the value which indicates whether fast mode will be used for the shapefile. 
        /// </summary>
        /// <remarks>
        /// The mode ensures faster drawing but makes the client responsible for the refreshing of shapefile extents
        /// after editing operation (see Shapefile.RefreshExtents and Shapefile.RefreshShapeExtents). It's highly recommended 
        /// to use the mode for large shapefiles.
        /// </remarks>
        /// \new48 Added in version 4.8
        public bool FastMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which affects the drawing of small shapes on the screen. 
        /// </summary>
        /// <remarks>
        /// The objects with scaled size less than this value will be drawn as a single dot. 
        /// This can noticeably increase performance for large shapefiles at full scale.
        /// But with the increase of speed the quality of drawing will be deteriorating. 
        /// The default value is 1.
        /// </remarks>
        /// \new48 Added in version 4.8
        public int MinDrawingSize
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the number of points in the specified shape of the shapefile.
        /// </summary>
        /// <remarks>This property is more efficient than calling Shapefile.get_Shape().NumPoints.</remarks>
        /// <param name="ShapeIndex">The index of the shape.</param>
        /// <returns>The number of points or -1 in case of invalid index.</returns>
        /// <remarks>The property used to be spelled as "Shapefile.numPoints" in .NET interop assemblies for MapWinGIS versions
        /// prior to 4.9. However it was because of bug in PIA generation process, as the property has always spelled as 
        /// "Shapefile.NumPoints" in MapWinGIS code itself.</remarks>
        public int get_NumPoints(int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the extents of the shape with the given index.
        /// </summary>
        /// <remarks>This operation is faster than Shapefile.get_Shape().Extents as there is no need to read all the shape data.</remarks>
        /// <param name="ShapeIndex">The index of the shape.</param>
        /// <returns>The extents of shape or NULL reference on failure.</returns>
        public Extents QuickExtents(int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the specified point of shape.
        /// </summary>
        /// <remarks>This operation doesn't require the reading of all shape date from disk, therefore
        /// it will be faster when editing mode is false.</remarks>
        /// <param name="ShapeIndex">The index of shape.</param>
        /// <param name="PointIndex">The point index.</param>
        /// <returns>The point or the NULL reference on the invalid index.</returns>
        public Point QuickPoint(int ShapeIndex, int PointIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns an array with the coordinates of points of the specified shape.
        /// </summary>
        /// <remarks>The value specified in numPoints will be adjusted if the specified number is greater than the number of points.</remarks>
        /// <param name="ShapeIndex">The index of shape.</param>
        /// <param name="numPoints">The number of point to return.</param>
        /// <returns>The array of double type with coordinates of the point. 
        /// Every two elements represent x and y coordinate of the point respectively.</returns>
        public System.Array QuickPoints(int ShapeIndex, ref int numPoints)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Refreshes extents of all shapes in the shapefile.
        /// </summary>
        /// <remarks>See Shapefile.CacheExtents for the details.</remarks>
        /// <returns>The value can be omitted.</returns>
        /// \new48 Added in version 4.8
        public bool RefreshExtents()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Refreshes the extents of the particular shape in the shapefile.
        /// </summary>
        /// <remarks>See Shapefile.CacheExtents for the details.</remarks>
        /// <param name="ShapeId">The index of the shape which extents are to be refreshed.</param>
        /// <returns>True on success and false in case of invalid shape index.</returns>
        /// \new48 Added in version 4.8
        public bool RefreshShapeExtents(int ShapeId)
        {
            throw new NotImplementedException();
        }

      


        /// @}
        #endregion

        #region Selection
        /// \addtogroup shapefile_selection Shapefile selection
        /// Here is a list of properties and methods for managing shapefile selection.
        /// This module is a part of the documentation of Shapefile class.
        /// \dot
        /// digraph shapefile_selection {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Shapefile" URL="\ref Shapefile"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Selection"          URL="\ref shapefile_selection"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>
        /// Gets the vertex closest to the specified point.
        /// </summary>
        /// <param name="x">X coordinate of the point to search from.</param>
        /// <param name="y">Y coordinate of the point to search from.</param>
        /// <param name="maxDistance">Maximum distance to search. Use 0 to search at any distance. Passing the max distance can improve performance.</param>
        /// <param name="ShapeIndex">Index of shape to which closest vertex belongs.</param>
        /// <param name="PointIndex">Index of closest vertex within shape.</param>
        /// <param name="Distance">The distance to the closest vertex.</param>
        /// <returns>True if the vertex was found within specified distance and false otherwise.</returns>
        /// \new490 Added in version 4.9.0
        public bool GetClosestVertex(double x, double y, double maxDistance, out int ShapeIndex, out int PointIndex, out double Distance)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets shapes which are located at specified spatial relation to reference shape.
        /// </summary>
        /// <param name="referenceIndex">Index of reference shape.</param>
        /// <param name="Relation">Spatial relation. Isn't implemented for Disjoint.</param>
        /// <param name="resultArray">The array of indices of the found shapes (int[])</param>
        /// <returns>True if at least one related shape was found.</returns>
        /// \new490 Added in version 4.9.0
        public bool GetRelatedShapes(int referenceIndex, tkSpatialRelation Relation, ref object resultArray)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets shapes which are located at specified spatial relation to reference shape.
        /// </summary>
        /// <param name="referenceShape">Reference shape.</param>
        /// <param name="Relation">Spatial relation. Isn't implemented for Disjoint.</param>
        /// <param name="resultArray">The array of indices of the found shapes (int[])</param>
        /// <returns>True if at least one related shape was found.</returns>
        /// \new490 Added in version 4.9.0
        public bool GetRelatedShapes2(Shape referenceShape, tkSpatialRelation Relation, ref object resultArray)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the state of selection for the specified shape
        /// </summary>
        /// <param name="ShapeIndex">The index of shape</param>
        /// <returns>True in case the shape is selected and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool get_ShapeSelected(int ShapeIndex)
        {
            throw new NotImplementedException();
        }
        /// <summary>
        /// Selects or deselects the specified shape
        /// </summary>
        /// <param name="ShapeIndex">The index of shape to be selected.</param>
        /// <param name="pVal">A value which indicates whether a shape should be selected.</param>
        /// \new48 Added in version 4.8
        public void set_ShapeSelected(int ShapeIndex, bool pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inverts selection of the shapefile, i.e. selection state of every shape is changed to the opposite value.
        /// </summary>
        /// \see Shapefile.get_ShapeSelected
        /// \new48 Added in version 4.8
        public void InvertSelection()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of selected shapes in the shapefile.
        /// </summary>
        /// \see Shapefile.get_ShapeSelected
        /// \new48 Added in version 4.8
        public int NumSelected
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Selects all the shapes in the shapefile
        /// </summary>
        /// \new48 Added in version 4.8
        public void SelectAll()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears selection from all shapes.
        /// </summary>
        /// \new48 Added in version 4.8
        public void SelectNone()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the way shapefile selection will be displayed.
        /// </summary>
        /// <remarks>See MapWinGIS.tkSelectionAppearance for the details.</remarks>
        /// \new48 Added in version 4.8
        public tkSelectionAppearance SelectionAppearance
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The colour of the selected shapes.
        /// </summary>
        /// <remarks>It is used when SelectionAppearance is set to saSelectionColor.</remarks>
        /// \new48 Added in version 4.8
        public uint SelectionColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The drawing options used for visualization of the selected shapes.
        /// </summary>
        /// <remarks>It is used when SelectionAppearance is set to saDrawingOptions.</remarks>
        /// \new48 Added in version 4.8
        public ShapeDrawingOptions SelectionDrawingOptions
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which defines the transparency of the selection. 
        /// Values from 0 (transparent) to 255(opaque) are applicable.
        /// </summary>
        /// \new48 Added in version 4.8
        public byte SelectionTransparency
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }


        /// <summary>
        /// Returns an array with indices of shapes which are located inside specified bounds.
        /// </summary>
        /// <remarks>This method doesn't change the state of selection or visualization of shapes.</remarks>
        /// <param name="BoundBox">The bounding rectangular to select shapes within.</param>
        /// <param name="Tolerance">The value of tolerance.</param>
        /// <param name="SelectMode">Selection mode.</param>
        /// <param name="Result">An output array of long type with the indices of the shapes.</param>
        /// <returns>True in case at least one shape is within bounds and false otherwise.</returns>
        public bool SelectShapes(Extents BoundBox, double Tolerance, SelectMode SelectMode, ref object Result)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a value indicating whether the shape under mouse cursor will be highlighted.
        /// </summary>
        /// <remarks>Turning this on may negatively affect the performance.</remarks>
        /// \new490 Added in version 4.9.0
        public bool HotTracking
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// @}

        #endregion

        #region Spatial index
        
        /// \addtogroup shapefile_spatial_index Shapefile spatial index
        /// Here is a list of properties and methods for managing of spatial index for a shapefile.
        /// This module is a part of the documentation of Shapefile class.
        /// \dot
        /// digraph shapefile_sptial_index {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Shapefile" URL="\ref Shapefile"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Spatial index"          URL="\ref shapefile_spatial_index"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>
        /// @{

        /// <summary>Gets the value which indicates whether spatial index will be used
        /// for displaying certain extents of the shapefile.</summary>
        /// \see SpatialIndexMaxAreaPercent
        public bool get_CanUseSpatialIndex(Extents pArea)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new spatial index
        /// </summary>
        /// <param name="ShapefileName">The name of shapefile to create spatial index for. 
        /// Filename of the current shapefile should be passed usually (Shapefile.Filename).</param>
        /// <returns></returns>
        public bool CreateSpatialIndex(string ShapefileName)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value indicating whether spatial index exists for the shapefile.
        /// </summary>
        /// <remarks>The set part of property does nothing. Use Shapefile.CreateSpatialIndex instead to create a new index.</remarks>
        public bool HasSpatialIndex
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Checks the validity of the spatial index.
        /// </summary>
        /// <returns>True if the index is valid and false on its absence or incompatibility.</returns>
        public bool IsSpatialIndexValid()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the value which affects the usage of spatial index. 
        /// </summary>
        /// <remarks>Spatial index won't be used while drawing a part of shapefile larger then specified percent. 
        /// Spatial index ensures faster access to the shapes while a small portion of shapefile
        /// is being displayed. On the contrary when substantial part of shapefile is being displayed 
        /// spatial index can decrease the performance. This property blocks the usage of 
        /// spatial index when it is inefficient.
        public double SpatialIndexMaxAreaPercent
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a value which indicates whether built-in quad-tree spatial index should be used for shapefile.
        /// </summary>
        /// <remarks>Quad is used in editing mode only. It has nothing to do with disk based spatial index which is used by methods like Shapefile.CreateSpatialIndex.</remarks>
        /// \new48 Added in version 4.8
        public bool UseQTree
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>Gets or sets a value which indicates whether spatial index should be used
        /// while working with shapefile.</summary>
        public bool UseSpatialIndex
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// @}
        
        #endregion

        #region Validation
        /// \addtogroup shapefile_validation Validation
        /// Here is a list of properties and methods for shapefile validation.
        /// This module is a part of the documentation of Shapefile class.
        /// \dot
        /// digraph shapefile_validation {
        /// splines = true;
        /// node [shape= "polygon", fontname=Helvetica, fontsize=9, style = filled, color = palegreen, height = 0.3, width = 1.2];
        /// lb [ label="Shapefile" URL="\ref Shapefile"];
        /// node [shape = "ellipse", color = khaki, width = 0.2, height = 0.2, style = filled]
        /// gr   [label="Validation"          URL="\ref shapefile_validation"];
        /// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
        /// lb -> gr;
        /// }
        /// \enddot
        /// <a href = "diagrams.html">Graph description</a>\n\n
        /// <b>%Shapefile validation</b>\n\n
        /// Starting from version 4.9.1 built-in validation for input and output shapefiles passed to geoprocessing methods is used. 
        /// During validation each shape within a shapefile is checked for validity with Shape.IsValid property and optionally an attempt is made 
        /// to fix invalid shapes with Shape.FixUp. If the fixing wasn't successful, various actions may be taken: 
        /// - shape skipped from output;
        /// - shape passed to output all the same;
        /// - the whole operation is aborted.
        /// 
        /// Validation behaviors are specified in GlobalSettings.ShapeInputValidationMode and GlobalSettings.ShapeOutputValidationMode. 
        /// See tkShapeValidationMode enumeration for possible behaviours. By default no validation is performed (tkShapeValidationMode.NoValidation), 
        /// to preserve compatibility with previous versions of library.\n\n
        /// The results of validation can be viewed via Shapefile.LastInputValidation and Shapefile.LastOutputValidation properties. 
        /// Both return instance of ShapeValidationInfo class (see description for list of properties).\n
        /// 
        /// There is slight difference between input and output validation:
        /// - input validation doesn't change shapefile - fixed shapes are placed into temporary storage and 
        /// are used in place of original ones during geoprocessing operation; therefore no editing mode is needed to do the validation; 
        /// - output validation substitutes invalid shapes with new ones; original shapes are discarded.
        /// 
        /// In this example a shapefile with invalid shape is created and then validated as input 
        /// to Shapefile.AggregateShapes methods and as output in Shapefile.Save method.
        /// \code
        /// // creating invalid shapefile
        /// Extents ext = new Extents();
        /// ext.SetBounds(100, 100, 0, 200, 200, 0);
        /// 
        /// Extents ext2 = new Extents();
        /// ext2.SetBounds(200, 100, 0, 300, 200, 0);
        /// 
        /// // polygons will touch along the border after aggregation
        /// var shp = ext.ToShape();
        /// var shp2 = ext2.ToShape();
        /// 
        /// // make invalid by reversing order of points
        /// shp.ReversePointsOrder(0);
        /// 
        /// var sf = new Shapefile();
        /// sf.CreateNew("", ShpfileType.SHP_POLYGON);
        /// int fieldIndex = sf.EditAddField("Id", FieldType.INTEGER_FIELD, 10, 10);
        /// sf.EditAddShape(shp);
        /// sf.EditAddShape(shp2);
        /// sf.EditCellValue(fieldIndex, 0, 1);
        /// sf.EditCellValue(fieldIndex, 1, 1);
        /// 
        /// // doing operation & validating
        /// var gs = new GlobalSettings();
        /// gs.ShapeInputValidationMode = tkShapeValidationMode.TryFixProceedOnFailure;
        /// gs.ShapeOutputValidationMode = tkShapeValidationMode.TryFixProceedOnFailure;
        /// 
        /// var result = sf.AggregateShapes(false, fieldIndex);
        /// 
        /// if (sf.LastInputValidation != null)
        ///     Debug.Print("Input validation: " + sf.LastInputValidation.Status.ToString());
        /// 
        /// if (sf.LastOutputValidation != null)
        ///     Debug.Print("Output validation: " + sf.LastOutputValidation.Status.ToString());
        /// 
        /// Debug.Print("Input still has invalid shapes: " + sf.HasInvalidShapes());
        /// Debug.Print("Output has invalid shapes: " + result.HasInvalidShapes());
        /// 
        /// sf.SaveAs(@"d:\validated.shp", null);
        /// if (sf.LastInputValidation != null)
        ///     Debug.Print("Output validation (for Shapefile.Save): " + sf.LastInputValidation.Status.ToString());
        ///             
        /// Debug.Print("Shapefile still has invalid shapes: " + sf.HasInvalidShapes());
        /// \endcode
        /// Output of the sample: \n
        /// \code
        /// // Input validation: InvalidFixed
        /// // Output validation: InvalidFixed
        /// // Input still has invalid shapes: True
        /// // Output has invalid shapes: False
        /// // Output validation (for Shapefile.Save): InvalidFixed
        /// // Shapefile still has invalid shapes: False
        /// \endcode
        /// 
        /// Here is list of methods with built-in shapefile validation:
        /// Shapefile.Clip, Shapefile.GetIntersection, Shapefile.Difference, Shapefile.SymmDifference, 
        /// Shapefile.Union, Shapefile.ExplodeShapes, Shapefile.AggregateShapes, Shapefile.ExportSelection, 
        /// Shapefile.Sort, Shapefile.Dissolve, Shapefile.BufferByDistance, Shapefile.Merge, Shapefile.Segmentize, 
        /// Shapefile.SimplifyLines, Shapefile.Reproject, Shapefile.ReprojectInPlace (input validation only),
        /// Shapefile.Save (output validation), Shapefile.SaveAs (output validation), Shapefile.Dump (output validation),
        /// Utils.ReprojectShapefile, Utils.GridToShapefile, Utils.TinToShapefile, Utils.OgrLayerToShapefile, 
        /// Utils.RemoveColinearPoints, Utils.ShapefileToGrid.
        /// @{
        
        /// <summary>
        /// Checks whether there are invalid shapes within shapefile.
        /// </summary>
        /// <returns>True if there invalid shapes and false otherwise.</returns>
        /// \new490 Added in version 4.9.0
        public bool HasInvalidShapes()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets results of input data validation for geoprocessing methods.
        /// </summary>
        /// <remarks>Use GlobalSettings.ShapeInputValidationMode to change the settings.</remarks>
        /// \new491 Added in version 4.9.1
        public ShapeValidationInfo LastInputValidation
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets results of output data validation for geoprocessing methods.
        /// </summary>
        /// <remarks>Use GlobalSettings.ShapeOutputValidationMode to change the settings.</remarks>
        /// \new491 Added in version 4.9.1
        public ShapeValidationInfo LastOutputValidation
        {
            get { throw new NotImplementedException(); }
        }

        /// @}
        #endregion

        /// @}

        
        /// <summary>
        /// Gets or sets a value indicating whether shapefile can be redrawn without redrawing other layer on the map.
        /// </summary>
        /// <remarks>
        /// All volatile shapefiles will be placed in main screen buffer (along with drawing layers) rather than data layer buffer,
        /// so that their redraw can be forced with AxMap.Redraw2(tkRedrawType.RedrawSkipDataLayers). This can be recommended 
        /// for in-memory shapefiles with constantly changing values to ensure rendering performance.
        /// </remarks>
        /// \new492 Added in version 4.9.2
        public bool Volatile
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets rotation angle for a shape.
        /// </summary>
        /// <param name="ShapeIndex">Index of shape.</param>
        /// <returns>Angle in degrees (0-360).</returns>
        /// <remarks>This value will be applied for icons of point shapefiles only (ShapeDrawingOptions.PointType = ptSymbolPicture). 
        /// Values other then 0.0 will override ShapeDrawingOptions.PointRotation property for shapefile and categories.</remarks>
        /// \new492 Added in version 4.9.2
        public double get_ShapeRotation(int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets rotation angle for a shape.
        /// </summary>
        /// <param name="ShapeIndex">Index of shape.</param>
        /// <param name="pVal">Angle in degrees (0-360).</param>
        /// <remarks>This value will be applied for icons of point shapefiles only (ShapeDrawingOptions.PointType = ptSymbolPicture). 
        /// Values other then 0.0 will override ShapeDrawingOptions.PointRotation property for shapefile and categories.</remarks>
        /// \new492 Added in version 4.9.2
        public void set_ShapeRotation(int ShapeIndex, double pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets visibility of shape.
        /// </summary>
        /// <param name="ShapeIndex">Index of shape.</param>
        /// <returns>True if shape is visible.</returns>
        /// <remarks>When set to false it will override any other properties, 
        /// like ShapeDrawingOptions.Visible or Shapefile.VisibilityExpression, i.e. the shape is GURANTEED to be hidden.
        /// When set to true all other settings will be taken into account also, so the shape MAY be visible provided
        /// that other properties allow it.</remarks>
        /// \new492 Added in version 4.9.2
        public bool get_ShapeVisible(int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets visibility of shape.
        /// </summary>
        /// <param name="ShapeIndex">Index of shape.</param>
        /// <param name="pVal">True if shape is visible.</param>
        /// <returns>True if shape is visible.</returns>
        /// <remarks>When set to false it will override any other properties, 
        /// like ShapeDrawingOptions.Visible or Shapefile.VisibilityExpression, i.e. the shape is GURANTEED to be hidden.
        /// When set to true all other settings will be taken into account also, so the shape MAY be visible provided
        /// that other properties allow it.</remarks>
        /// \new492 Added in version 4.9.2 
        public void set_ShapeVisible(int ShapeIndex, bool pVal)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif




