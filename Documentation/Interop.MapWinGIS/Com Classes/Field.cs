
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    /// <summary>
    /// Represents a single field of the attribute table.
    /// </summary>
    /// \dot
    /// digraph field_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// fld [ label="Field" URL="\ref Field"];
    ///
    /// node [color = tan peripheries = 1 height = 0.3, width = 1.0];
    /// tbl [ label="Table" URL="\ref Table"];
    /// 
    /// edge [ dir = "none", arrowhead="open", style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ];
    /// tbl -> fld [ URL="\ref Table.get_Field()", tooltip = "Table.get_Field()", headlabel = "   n"];
    /// }
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>
    /// 
    /// To show the properties of the field the following code can be used:
    /// \code
    /// Shapefile sf = some_shapefile;
    /// Field field = sf.get_Field(fieldIndex);
    /// if (field != null)
    /// {
    ///     string s = string.Format("Name = {0}; type = {1}; width = {2}; precision = {3}", 
    ///     field.Name, field.Type.ToString(), field.Width.ToString(), field.Precision.ToString());
    ///     Debug.Print(s);
    /// }
    /// \endcode
    #if nsp
        #if upd
            public class Field : MapWinGIS.IField
        #else        
            public class IField
        #endif
    #else
        public class Field
    #endif
    {
        #region IField Members
        /// <summary>
        /// Creates an exact copy of the field.
        /// </summary>
        /// <returns>The copy of the field.</returns>
        /// \new48 Added in version 4.8
        public Field Clone()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The global callback is the interface used by MapWinGIS to pass progress and error events to interested applications.
        /// </summary>
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// The key may be used by the programmer to store any string data associated with the object.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Retrieves the last error generated in the object.
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the name of the field.
        /// </summary>
        public string Name
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the precision of the field. Precision only applies to fields of fieldtype double.
        /// </summary>
        public int Precision
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the field type of the field.
        /// </summary>
        public FieldType Type
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the width of the field.
        /// </summary>
        public int Width
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Retrieves the error message associated with the specified error code.
        /// </summary>
        /// <param name="ErrorCode">The error code for which the error message is required.</param>
        /// <returns>The error message description for the specified error code.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
#if nsp
}
#endif

