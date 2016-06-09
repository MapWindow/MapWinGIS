#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Represents a single bar or sector of the chart.
    /// </summary>
    /// \dot
    /// digraph chartfield_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// lb [ label="ChartField" URL="\ref ChartField"];
    /// 
    /// node [color = tan peripheries = 1 height = 0.3, width = 1.0];
    /// lbs [ label="Charts" URL="\ref Charts"];
    /// 
    /// edge [ dir = none, arrowhead="open", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// lbs -> lb [ URL="\ref Charts.get_Field()", tooltip = "Charts.get_Field()", headlabel = "   n"];
    /// }
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>\n\n
    /// <remarks>The chart fields set mapping between bars or sectors of the chart and a fields in the attribute table 
    /// of the shapefile (see Table). The visual representation of chart field, either bar or sector, 
    /// depends on Charts.ChartType property.\n\n
    /// It's necessary to distinguish the index of bar or sector in the chart 
    /// and the index of field in dbf table it is bound to. For example, in case the chart contains a single bar 
    /// which is mapped to the 5-th field of the dbf table, the index of bar for Charts.get_Field() property will be 0
    /// (as there is only one bar in the chart), while ChartField.Index() will return 5 (as it is mapped to the 5 field).
    /// \code 
    /// Shapefile sf = some_shapefile;
    /// Charts charts = sf.Charts;
    /// charts.ChartType = tkChartType.chtBarChart;
    /// charts.ClearFields();
    /// 
    /// // adds a first bar mapped to 6th field of dbf
    /// Utils utils = new Utils();
    /// ChartField bar = new ChartField();
    /// bar.Index = 5;
    /// bar.Color = utils.ColorByName(tkMapColor.Red);
    /// charts.AddField(bar);
    /// 
    /// // adds one more bar mapped to 9th field
    /// charts.AddField2(8, utils.ColorByName(tkMapColor.Blue));  // index of field, colour
    /// 
    /// for (int i = 0; i < charts.NumFields; i++)
    /// {
    ///    string s = string.Format("Bar index = {0}; mapped to field index = {1}", i, charts.get_Field(i).Index);
    ///    Debug.Print(s + Environment.NewLine);
    /// }
    /// // the output:
    /// // Bar index = 0; mapped to field index = 5
    /// // Bar index = 1; mapped to field index = 8
    /// \endcode
    /// \new48 Added in version 4.8
    #if nsp
        #if upd    
            public class ChartField : MapWinGIS.IChartField
        #else        
            public class IChartField        
        #endif
    #else
        public class ChartField
    #endif
    {
        #region IChartField Members
        /// <summary>
        /// Gets or sets the colour of the bar or sector.
        /// </summary>
        public uint Color
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the index of field from attribute table (.dbf) to take values from.
        /// </summary>
        public int Index
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the name of the chart field.
        /// </summary>
        /// <remarks>In most cases it should be set equal to the name of the .dbf field from which
        /// the data is taken, unless some more comprehensive alias is needed.</remarks>
        public string Name
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        #endregion
    }
#if nsp
}
#endif

