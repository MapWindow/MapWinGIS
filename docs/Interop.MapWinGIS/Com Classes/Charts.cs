#if nsp
namespace MapWinGIS
{
#endif
    using System;
    
    /// <summary>
    /// Provides functionality for generation and managing the charts on the map. 
    /// </summary>
    /// \dot
    /// digraph charts_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// cht [ label="Chart" URL="\ref Chart"];
    /// cfld [ label="ChartField" URL="\ref ChartField"];
    /// 
    /// node [color = tan, peripheries = 1, height = 0.3, width = 1.0];
    /// charts [ label="Charts" URL="\ref Charts"];
    /// 
    /// node [style = dashed, color = gray];
    /// sf [ label="Shapefile" URL="\ref Shapefile"];
    /// 
    /// edge [ dir = "none", arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// sf -> charts [ URL="\ref Shapefile.Charts", tooltip = "Shapefile.Labels", headlabel = "   1"];
    /// edge [style = solid]
    /// charts -> cht [ URL="\ref Charts.get_Chart()", tooltip = "Labels.get_Chart()", headlabel = "   n"];
    /// charts -> cfld [ URL="\ref Charts.get_Field()", tooltip = "Labels.get_Field()", headlabel = "   n"];
    /// }
    /// 
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>\n\n
    /// The charts obtain their data from the fields of attribute table of the shapefile. 
    /// A single bar or sector of the chart is mapped to a certain field by means of ChartField class. The number of fields 
    /// to be displayed can be changed by Charts.AddField, Charts.RemoveField. The fields should be of numeric types, either 
    /// double or integer.\n\n
    /// Two types of charts are available:
    /// -# Pie charts:
    ///     - are suitable when it's necessary to display the parts of the whole phenomenon, 
    ///     like the percentage of land use by type (urban, forest, agricultural, etc.);
    ///     - it's possible to set varying width for the charts depending of the certain field, 
    ///     which can give an idea about the values for different shapes on comparative scale;\n
    ///     .
    /// -# Bar charts: 
    ///     - they can be used to display virtually any numeric data;
    ///     - it still makes sense to display fields with the same units of measure;\n
    ///     .
    /// .
    /// \image html charts.png
    /// To generate charts one basically needs:\n
    /// -# To specify a set of fields which will be included (see Charts.AddField and Charts.AddField2). \n\n
    /// -# To define the positions of the charts relative to the parent shape. 
    ///     - Charts.Generate method is the only one to do this job;
    ///     - for large shapefile this procedure can be time consuming, therefore there are means for caching this data to 
    ///     avoid additional recalculations;
    ///     - it's possible to change the generated positions of the individual charts (see Chart.PositionX, Chart.PositionY).\n\n
    ///     .
    /// -# To set the display options of the charts. The options include:
    ///     - the size of charts (Charts.PieRadius for pie charts and Charts.BarHeight for bar charts; the values of these 2 types of charts don't affect each other);
    ///     - the colour of the charts fill and outline;
    ///     - labels with the values from the underlying table, which correspond to particular bar or sector.\n
    ///     .
    /// .
    /// 
    /// \code
    /// private void GenerateCharts(Shapefile sf)
    /// {
    ///     sf.Charts.AddField2(0, 255);    // index of field, colour
    ///     sf.Charts.AddField2(1, 255);
    ///     sf.Charts.Generate(tkLabelPositioning.lpCentroid);
    ///     sf.Charts.ChartType = tkChartType.chtBarChart;
    /// }
    /// \endcode
    /// 
    /// Individual charts can be accessed using Charts.get_Chart() property. It's possible to change their properties like visibility or position.
    /// Chart.ScreenExtents property return screen coordinates occupied by particular chart which provides a convenient way for 
    /// highlighting or mouse dragging operation on the chart.\n
    /// 
    /// By default charts are drawn for each shape of the shapefile, but if only a subset of shapes require charts 
    /// it's possible to set Charts.VisibilityExpression, or toggle  the visibility of individual charts (Chart.Visible).
    /// Though the number of individual charts will always be equal to the number of shapes in the shapefile, which 
    /// means that charts will be automatically added and removed after corresponding editing operation in the parent shapefile.
    /// It's not possible to set different visualization options or the number of fields for individual charts.\n
    /// 
    /// It's not possible to use charts independently of the shapefile layer. But it's possible to create 
    /// a dummy shapefile, populate it with data and display any charts at any location. 
    /// \new48 Added in version 4.8
    #if nsp
        #if upd    
            //public class Charts : MapWinGIS.ICharts
    public class ICharts       
        #else        
            public class ICharts       
        #endif
    #else
        public class Charts
    #endif
    {
        #region ICharts Members
        /// <summary>
        /// Adds a field to the chart. 
        /// </summary>
        /// <remarks>The field will be represented as bar or sector depending on chart type.</remarks>
        /// <param name="Field">A field to add.</param>
        /// <returns>True on success or false otherwise.</returns>
        public bool AddField(ChartField Field)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Adds a field to the chart. 
        /// </summary>
        /// <param name="FieldIndex">The index of the field in the attribute table to take values from.</param>
        /// <param name="Color">The colour of the sector or bar for visualization of a field.</param>
        public void AddField2(int FieldIndex, uint Color)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Gets or set a boolean value which indicates whether charts can overlap each other.
        /// </summary>
        /// <remarks>Setting this property to true will prevent the drawing of overlapping
        /// charts and hence some charts can be left undrawn. The default value is true.</remarks>
        public bool AvoidCollisions
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the maximum height of the bar chart in pixels.
        /// </summary>
        /// <remarks>The value set will be used to display the maximum value of the field. 
        /// The bar height for smaller values will be proportionally smaller. 
        /// </remarks>
        public int BarHeight
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Gets or sets the width of a single bar of the chart in pixels.
        /// </summary>
        public int BarWidth
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the text string for description of the chart data. 
        /// </summary>
        /// <remarks>It can be used for clarification of the map's legend.</remarks>
        public string Caption
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the type of the charts.
        /// </summary>
        /// <remarks>Bar and pie charts are available. All charts of the shapefile
        /// are of the same type.</remarks>
        public tkChartType ChartType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Clears the information about individual charts like position and visibility.
        /// </summary>
        /// <remarks>To restore the charts a call of Charts.Generate() is needed. The fields
        /// will remain untouched by this method.</remarks>
        public void Clear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes all fields which define bars or sectors of the chart.
        /// </summary>
        public void ClearFields()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the minimal distance between individual charts when Charts.AvoidCollisions is set to true.
        /// </summary>
        /// <remarks>This allows to arrange charts in more sparse pattern to make them more readable.
        /// Naturally the larger this distance is, the smaller is the number of charts which can be drawn
        /// on screen simultaneously.</remarks>
        public int CollisionBuffer
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the number of charts which is equal to the number of shapes in the parent shapefile.
        /// </summary>
        public int Count
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Restores the state of the charts from the serialized string.
        /// </summary>
        /// <param name="newVal">Serialized string generated by Charts.Serialize().</param>
        public void Deserialize(string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Draws a chart on the specified device context. Can be used for displaying of map legend.
        /// </summary>
        /// <param name="hdc">The handle of the device context.</param>
        /// <param name="x">The position of the left corner of the drawing in pixels.</param>
        /// <param name="y">The position of the top corner of the drawing in pixels.</param>
        /// <param name="hideLabels">When set to true the labels won't be drawn.</param>
        /// <param name="BackColor">The back colour of the device context. 
        /// Should be provided to ensure correct alpha blending when transparency is set.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool DrawChart(IntPtr hdc, float x, float y, bool hideLabels, uint BackColor)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a value which indicates whether charts will be visible for all scales or in the given range of scales only.
        /// </summary>
        /// \see Charts.MaxVisibleScale, Charts.MinVisibleScale
        public bool DynamicVisibility
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Generates a chart for every shape of the parent shapefile.
        /// </summary>
        /// <remarks>This method is time consuming for large shapefiles, therefore consider the serialization of
        /// charts data after first generation.</remarks>
        /// <param name="Type">The position of chart relative to the parent shape.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool Generate(tkLabelPositioning Type)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a callback object to display progress and error information.
        /// </summary>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
        
        /// <summary>
        /// Gets the minimal height of the image to display a single chart.
        /// </summary>
        /// <remarks>The property can be used for interactive selection of charts and for drawing of legend.</remarks>
        public int IconHeight
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the minimal width of the image to display a single chart.
        /// </summary>
        /// <remarks>The property can be used for interactive selection of charts and for drawing of legend.</remarks>
        public int IconWidth
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Inserts a field which represents single bar or sector of the chart.
        /// </summary>
        /// <param name="Index">The index to insert the field at.</param>
        /// <param name="Field">The chart field object to insert.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool InsertField(int Index, ChartField Field)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inserts a field which represents single bar or sector of the chart.
        /// </summary>
        /// <param name="Index">The index to insert the field at.</param>
        /// <param name="FieldIndex">The index of the field from the attribute table (.dbf) to take values from.</param>
        /// <param name="Color">The colour of the fill.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool InsertField2(int Index, int FieldIndex, uint Color)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a string value associated with the object.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the code of the last error which took place inside the current instance of the class.
        /// </summary>
        /// <remarks>The use of this property will clear the error until the next error occurs.</remarks>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the color of the chart outline
        /// </summary>
        public uint LineColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Loads charts data from the XML file generated by Charts.SaveToXML.
        /// </summary>
        /// <param name="Filename">The filename to load data from.</param>
        /// <returns>True on successful loading and false otherwise.</returns>
        public bool LoadFromXML(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Get or sets the maximum scale the charts are displayed when dynamic visibility is turned on.
        /// </summary>
        public double MaxVisibleScale
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the minimal scale the charts are displayed when dynamic visibility is turned on.
        /// </summary>
        public double MinVisibleScale
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Changes the position of a field (bar or sector) in the list which affects the order of drawing.
        /// </summary>
        /// <param name="OldIndex">The old index of the field.</param>
        /// <param name="NewIndex">The new index of the field.</param>
        /// <returns></returns>
        public bool MoveField(int OldIndex, int NewIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the index of field upon which the values of other fields will be divided before displaying charts.
        /// </summary>
        /// <remarks>It can be used to display the data in percentage for bar charts. For example, if a chart
        /// displays groups of population for a cities, then normalization field can be the total population of the cities, so that
        /// percentages between different towns can be compared on the same scale.</remarks>
        public int NormalizationField
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the number of fields which correspond to the bars of sectors depending on chart type.
        /// </summary>
        public int NumFields
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the horizontal offset for drawing charts.
        /// </summary>
        /// <remarks>Positive values will move charts to the right. This
        /// value affects all the charts. Use Chart.PositionX to move individual charts.</remarks>
        public int OffsetX
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or set the vertical offset for drawing charts. 
        /// </summary>
        /// <remarks>Positive values will move charts up. This value affects all the charts.
        /// Use Chart.PositionY to move individual charts.</remarks>
        public int OffsetY
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the radius of the pie chart in pixels.
        /// </summary>
        public int PieRadius
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the radius of the largest chart when Charts.UseVariableRadius is turned on.
        /// </summary>
        public int PieRadius2
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or set the rotation of the pie chart.
        /// </summary>
        /// <remarks>This property is no implemented.</remarks>
        public double PieRotation
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Removes the field which corresponds to the bar or the sector of the chart.
        /// </summary>
        /// <param name="Index">The index of the field to remove.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool RemoveField(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the state of the charts to the XML file.
        /// </summary>
        /// <remarks>Charts.SavingMode defines the particular data which will be serialized.</remarks>
        /// <param name="Filename">The filename to save into.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool SaveToXML(string Filename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the mode for serialization of charts. Affects Charts.SaveToXML method.
        /// </summary>
        public tkSavingMode SavingMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the indices of charts which are displayed within specified rectangle on the screen.
        /// </summary>
        /// <param name="BoundingBox">The rectangle to select charts within (in screen coordinates).</param>
        /// <param name="Tolerance">The tolerance in pixels. The bounding box will be expanded by this values.</param>
        /// <param name="SelectMode">The selection mode.</param>
        /// <param name="Indices">The array of integer type with indices of the selected charts.</param>
        /// <returns>True if at least one chart was selected and false otherwise.</returns>
        public bool Select(Extents BoundingBox, int Tolerance, SelectMode SelectMode, ref object Indices)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Serializes the state of the charts to the string.
        /// </summary>
        /// <returns>The serialized string.</returns>
        public string Serialize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the index of field which affect the radius of charts when Charts.UseVariableRadius is set to true.
        /// </summary>
        /// <remarks>In most cases this field doesn't included in the chart itself.</remarks>
        public int SizeField
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which affects 3D display of the charts.
        /// </summary>
        /// <remarks>This value corresponds to the "height" for pie charts and "depth" for bar charts.</remarks>
        /// \see Charts.Use3DMode.
        public double Thickness
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the tilt of the charts when 3D display  is on.
        /// </summary>
        /// \see Charts.Use3DMode.
        public double Tilt
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or set the transparency of charts (0-255).
        /// </summary>
        /// <remarks>The values range from 0(transparent) to 255(opaque). The data labels aren't affected by this value.</remarks>
        public short Transparency
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether 3D mode will be used for charts drawing.
        /// </summary>
        public bool Use3DMode
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether all pie charts will have the same or varying radius.
        /// </summary>
        /// <remarks>When set to true, the radius of the individual charts will depend upon the value in Charts.SizeField.</remarks>
        public bool UseVariableRadius
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether data labels for charts will be bold.
        /// </summary>
        public bool ValuesFontBold
        {
           get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
    
        /// <summary>
        /// Gets or sets the colour of the chart's data labels.
        /// </summary>
        public uint ValuesFontColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether data labels for charts will be italic.
        /// </summary>
        public bool ValuesFontItalic
        {
           get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the font name for data labels of charts. The names of font families like "Arial" can be used.
        /// </summary>
        public string ValuesFontName
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or set the size of chart's labels.
        /// </summary>
        public int ValuesFontSize
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the colour of the frame for chart labels.
        /// </summary>
        public uint ValuesFrameColor
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets a boolean value which indicates whether the frame of chart's labels is visible.
        /// </summary>
        public bool ValuesFrameVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the style of the chart's labels.
        /// </summary>
        public tkChartValuesStyle ValuesStyle
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the value which indicates whether labels with values of the fields will be drawn near each chart.
        /// </summary>
        public bool ValuesVisible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the position of charts in the map drawing order.
        /// </summary>
        public tkVerticalPosition VerticalPosition
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the expression which defines the subset of charts which will be displayed on the map.
        /// </summary>
        public string VisibilityExpression
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Turns on or off the visibility of charts.
        /// </summary>
        public bool Visible
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets a reference to the chart with the specified index. 
        /// </summary>
        /// <param name="Chart">The index of the chart to extract.</param>
        /// <returns>The reference to the chart of NULL reference on failure.</returns>
        public Chart get_Chart(int Chart)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the description of the specified error code.
        /// </summary>
        /// <param name="ErrorCode">The error code returned by Charts.LastErrorCode.</param>
        /// <returns>The description of error.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a reference to the field of the chart with the specified index.
        /// </summary>
        /// <param name="FieldIndex">The index of the field.</param>
        /// <returns>The reference to the chart field or NULL reference on failure.</returns>
        public ChartField get_Field(int FieldIndex)
        {
            throw new NotImplementedException();
        }
        #endregion
    }
#if nsp
}
#endif

