
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    /// <summary>
    /// The table object is used to store information from the dbf file associated with a shapefile. 
    /// </summary>
    /// \dot
    /// digraph table_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// fld [ label="Field" URL="\ref Field"];
    ///
    /// node [color = tan peripheries = 1 height = 0.3, width = 1.0]
    /// tbl [ label="Table" URL="\ref Table"];
    /// 
    /// node [color = gray, style = dashed];
    /// sf [ label="Shapefile" URL="\ref Shapefile"];
    /// 
    /// edge [ dir = none, style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" labeldistance = 0.6]
    /// sf -> tbl [ URL="\ref Shapefile.Table", tooltip = "Shapefile.Table", headlabel = "   1"];
    /// 
    /// tbl -> fld [ URL="\ref Table.get_Field()", tooltip = "Table.get_Field()", headlabel = "   n"];
    /// }
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>\n\n
    /// The following code will add the field in the table:
    /// \code
    /// Shapefile sf = some_shapefile;
    /// if (sf.EditingTable)
    /// {
    ///     // string field
    ///     Field fld = new Field();
    ///     fld.Name = "New field";
    ///     fld.Type = FieldType.STRING_FIELD;
    ///     fld.Width = 15;      // 15 characters
    ///
    ///     // let's insert it
    ///     int fieldIndex = sf.NumFields;      // it will be inserted as the last one
    ///     sf.EditInsertField(fld, ref fieldIndex, null);
    /// }
    /// \endcode
    /// To find the index of field with the given name in the table:
    /// \code
    /// Shapefile sf = some_shapefile;
    ///    
    /// // fast call
    /// int fieldIndex = sf.Table.get_FieldIndexByName("New field");
    ///
    /// // to do the same "manually"
    /// fieldIndex = -1;
    /// for (int i = 0; i < sf.NumFields; i++)
    /// {
    ///     if (sf.get_Field(i).Name == "New field") {
    ///         fieldIndex = i;
    ///         break;
    ///     }
    /// }
    ///
    /// Debug.Print("Field index: " + fieldIndex.ToString());
    /// \endcode
    #if nsp
        #if upd
            public class Table : MapWinGIS.ITable
        #else        
            public class ITable
        #endif
    #else
        public class Table
    #endif
    {
        #region ITable Members

        /// <summary>
        /// Calculates the the expression taking the values from the specified row of the attribute table.
        /// </summary>
        /// <param name="Expression">The expression to analyse.</param>
        /// <param name="RowIndex">The index of the row.</param>
        /// <param name="Result">The result of calculation as variant data type, either boolean, double or string.</param>
        /// <param name="ErrorString">An output string with the description of error in case method failed.</param>
        /// <returns>True on successful calculation and false otherwise.</returns>
        ///  \new48 Added in version 4.8
        public bool Calculate(string Expression, int RowIndex, out object Result, out string ErrorString)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the common dialog filter containing all supported file extensions in string format. 
        /// </summary>
        public string CdlgFilter
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Closes the attribute table.
        /// </summary>
        /// <returns>The value can be ignored.</returns>
        public bool Close()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new attribute table. 
        /// </summary>
        /// <remarks>A new table is automatically in editing mode after it is created.</remarks>
        /// <param name="dbfFilename">The filename for the new table.</param>
        /// <returns>A boolean value representing the success or failure of creating the new table.</returns>
        public bool CreateNew(string dbfFilename)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the value of the cell. 
        /// </summary>
        /// <remarks>The table must be set to allow editing before a cell's value may be edited.</remarks>
        /// <param name="FieldIndex">The field index of the cell to be edited.</param>
        /// <param name="RowIndex">The row index of the cell to be edited.</param>
        /// <param name="newVal">The new value to be used to set the specified cell's value.</param>
        /// <returns>A boolean value representing the success or failure of setting the value of the specified cell in the table.</returns>
        public bool EditCellValue(int FieldIndex, int RowIndex, object newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deletes all rows and fields from the table. Note: The table must be set to allow editing before the rows and fields can be deleted from the table.
        /// </summary>
        /// <returns>A boolean value representing the success or failure of deleting all rows and fields from the table.</returns>
        public bool EditClear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deletes a field from the table. 
        /// </summary>
        /// <remarks>The table must be set to allow editing before a field can be deleted from the table.</remarks>
        /// <param name="FieldIndex">The index of the field to be deleted from the table.</param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while the field is being deleted from the table.</param>
        /// <returns></returns>
        public bool EditDeleteField(int FieldIndex, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deletes a row from the table.
        /// </summary>
        /// <remarks>The table must be set to allow editing before a row can be deleted from the table.</remarks>
        /// <param name="RowIndex">The index of the row to be deleted from the table.</param>
        /// <returns>A boolean value representing the success or failure of deleting the specified row from the table.</returns>
        public bool EditDeleteRow(int RowIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inserts a new field into the table.
        /// </summary>
        /// <remarks>The table must be set to allow editing before a field can be inserted into the table. </remarks>
        /// <param name="Field">The new field to be inserted into the table.</param>
        /// <param name="FieldIndex">The desired index to be used for the new field being inserted into the table. 
        /// If the desired index is invalid or unavailable, the actual index used for the new field will be returned.</param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while the new field is being inserted into the table.</param>
        /// <returns>A boolean value representing the success or failure of inserting the new field into the table.</returns>
        public bool EditInsertField(Field Field, ref int FieldIndex, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inserts a new row into the table. 
        /// </summary>
        /// <remarks>The table must be set to allow editing before a row can be inserted into the table.</remarks>
        /// <param name="RowIndex">The desired index to use when inserting the new row into the table. If the 
        /// desired index is invalid or unavailable, the actual index used to insert the new row will be returned.</param>
        /// <returns>A boolean value representing the success or failure of inserting the new row into the table.</returns>
        public bool EditInsertRow(ref int RowIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Replaces the specified field in the table with the new field. 
        /// </summary>
        /// <remarks>The table must be set to allow editing before a field can be replaced in the table.</remarks>
        /// <param name="FieldIndex">The index of the field to be replaced.</param>
        /// <param name="NewField">The field to be used to replace the specified field in the table.</param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while the specified field 
        /// is being replced by the new field.</param>
        /// <returns>A boolean value representing the success or failure of replacing the specified field.</returns>
        public bool EditReplaceField(int FieldIndex, Field NewField, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets whether or not the table is in editing mode.
        /// </summary>
        public bool EditingTable
        {
            get { throw new NotImplementedException(); }
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
        /// Gets the number of fields in the table.
        /// </summary>
        public int NumFields
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the number of rows in the table.
        /// </summary>
        public int NumRows
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Opens a .dbf table from file.
        /// </summary>
        /// <param name="dbfFilename">The filename of the table to be opened.</param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while the table is being opened.</param>
        /// <returns>The boolean value representing success or failure of the opening table.</returns>
        public bool Open(string dbfFilename, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Checks if the expression is a valid one.
        /// </summary>
        /// <param name="Expression">The expression to analyse.</param>
        /// <param name="ErrorString">An output string with the description of error in case expression is not valid.</param>
        /// <returns>True if expression is valid and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool ParseExpression(string Expression, ref string ErrorString)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Selects the rows in the table which agree with the specified expression.
        /// </summary>
        /// <param name="Expression">The query expression.</param>
        /// <param name="Result">An array of integer type with the indices of rows which were selected.</param>
        /// <param name="ErrorString">An output string with the description of error on failure.</param>
        /// <returns>True in case at least one row was selected and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool Query(string Expression, ref object Result, ref string ErrorString)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves in-memory version of the table to the source file.
        /// </summary>
        /// <remarks>This method should be called while the table is in editing mode. The editing mode will not be closed.</remarks>
        /// <param name="cBack">A callback object to report information about progress and errors.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool Save(ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the table using the specified filename.
        /// </summary>
        /// <param name="dbfFilename">The filename to be used to save the table. </param>
        /// <param name="cBack">The ICallback object which will receive progress and error messages while the table is being saved.</param>
        /// <returns>A boolean value representing the success or failure of saving the table.</returns>
        public bool SaveAs(string dbfFilename, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the table to allow table editing.
        /// </summary>
        /// <param name="cBack">Optional. The ICallback object which will receive progress and error events while the table is being 
        /// set to allow editing.</param>
        /// <returns>A boolean value representing the success or failure of setting the table to allow editing.</returns>
        public bool StartEditingTable(ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the table to prevent editing.
        /// </summary>
        /// <param name="ApplyChanges">Optional. A boolean value representing whether or not to save changes to the table. The default 
        /// is True, to save the changes. </param>
        /// <param name="cBack">Optional. The ICallback object which will receive progress and error messages while the table is being set to prevent editing.</param>
        /// <returns>A boolean value representing the success or failure of setting the table to prevent editing.</returns>
        public bool StopEditingTable(bool ApplyChanges, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tests the validity of expression and determines its return type.
        /// </summary>
        /// <param name="Expression">The expression to test.</param>
        /// <param name="ReturnType">An output value with the return type, either double, string or boolean.</param>
        /// <param name="ErrorString">An output string with the description of error.</param>
        /// <returns>True if the expression is valid and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool TestExpression(string Expression, tkValueType ReturnType, ref string ErrorString)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value of the specified cell in the table. 
        /// </summary>
        /// <param name="FieldIndex">The field index of the cell for which the value is required.</param>
        /// <param name="RowIndex">The row index of the cell for which the value is required. </param>
        /// <returns>The value of the specified cell in the table.</returns>
        public object get_CellValue(int FieldIndex, int RowIndex)
        {
            throw new NotImplementedException();
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

        /// <summary>
        /// Gets the field object at the specified field index in the table. 
        /// </summary>
        /// <param name="FieldIndex">The index of the field in the table to be returned. </param>
        /// <returns>The field object specified by the field index.</returns>
        public Field get_Field(int FieldIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Seeks field with specified name. Comparison is case insensitive.
        /// </summary>
        /// <param name="Fieldname">The name of field to search.</param>
        /// <returns>The index of field if it exists and -1 otherwise.</returns>
        public int get_FieldIndexByName(string Fieldname)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the maximum value for the specified field of the table.
        /// </summary>
        /// <remarks>NULL will be returned in case of invalid index.</remarks>
        /// <param name="FieldIndex">The index of the field.</param>
        /// <returns>The maximum value, either integer, double or string data type.</returns>
        /// \new48 Added in version 4.8
        public object get_MaxValue(int FieldIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the mean value for the specified field of the table.
        /// </summary>
        /// <remarks>NULL will be returned in case of invalid index.</remarks>
        /// <param name="FieldIndex">The index of the field.</param>
        /// <returns>The mean value, either integer, double or string data type.</returns>
        /// \new48 Added in version 4.8
        public double get_MeanValue(int FieldIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the minimum value for the specified field.
        /// </summary>
        /// <remarks>NULL will be returned in case of invalid index.</remarks>
        /// <param name="FieldIndex">The index of the field.</param>
        /// <returns>The minimum value, either integer, double or string data type.</returns>
        /// \new48 Added in version 4.8
        public object get_MinValue(int FieldIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the standard deviation for the set of values in specified field.
        /// </summary>
        /// <remarks>NULL will be returned in case of invalid index.</remarks>
        /// <param name="FieldIndex">The index of the field.</param>
        /// <returns>The minimum value, either integer, double or string data type.</returns>
        /// \new48 Added in version 4.8
        public double get_StandardDeviation(int FieldIndex)
        {
            throw new NotImplementedException();
        }

        #endregion

        /// <summary>
        /// Adds a field to the table. The table must be in editing mode.
        /// </summary>
        /// <param name="Name">The name of field.</param>
        /// <param name="Type">The type of field.</param>
        /// <param name="Precision">The precision of field.</param>
        /// <param name="Width">The width of field.</param>
        /// <returns>The index of the new field or -1 on failure.</returns>
        /// \new490 Added in version 4.9.0
        public int EditAddField(string Name, FieldType Type, int Precision, int Width)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Restores state of joins from string.
        /// </summary>
        /// <param name="newVal">String generated by Table.Serialize method.</param>
        /// \new490 Added in version 4.9.0
        public void Deserialize(string newVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Writes contents of the DBF file to disk without changing the state of in-memory DBF.
        /// </summary>
        /// <param name="dbfFilename">Filename to write the data to.</param>
        /// <param name="cBack">Callback interface.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// <remarks>The methods may be used for in-memory, when the content should be saved without switching to disk mode.</remarks>
        /// \new490 Added in version 4.9.0
        public bool Dump(string dbfFilename, ICallback cBack = null)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a value indicating whether the table has any joined tables.
        /// </summary>
        /// \new490 Added in version 4.9.0
        public bool IsJoined
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Joins external table to the current one.
        /// </summary>
        /// <param name="table2">External table.</param>
        /// <param name="field1">Join field of this table.</param>
        /// <param name="field2">Join filed of external table.</param>
        /// <returns>True on success.</returns>
        /// <remarks>If formats other than DBF are needed, in-memory dbf table should be populated first.</remarks>
        /// \new490 Added in version 4.9.0
        public bool Join(Table table2, string field1, string field2)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Joins external table to the current one.
        /// </summary>
        /// <param name="table2">>External table.</param>
        /// <param name="field1">Join field of this table.</param>
        /// <param name="field2">Join filed of external table.</param>
        /// <param name="filenameToReopen">Filename to be opened to restore join on deserialization of table state.</param>
        /// <param name="joinOptions">Arbitrary join options to help restore join on deserialization.</param>
        /// <returns>True on success.</returns>
        /// <remarks>If filename specify formats other than dbf (extension is checked), Table.OnUpdateJoin event will be raised. 
        /// Client can handle this event by populating joinSource table parameter using filename, fieldList and options parameters.</remarks>
        /// \new490 Added in version 4.9.0
        public bool Join2(Table table2, string field1, string field2, string filenameToReopen, string joinOptions)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Joins external table to the current one.
        /// </summary>
        /// <param name="table2">>External table.</param>
        /// <param name="field1">Join field of this table.</param>
        /// <param name="field2">Join filed of external table.</param>
        /// <param name="filenameToReopen">Filename to be opened to restore join on deserialization of table state.</param>
        /// <param name="joinOptions">Arbitrary join options to help restore join on deserialization.</param>
        /// <param name="fieldList">Comma separated list of fields to be displayed from external table.</param>
        /// <returns>True on success.</returns>
        /// <remarks>If filename specify formats other than dbf (extension is checked), Table.OnUpdateJoin event will be raised. 
        /// Client can handle this event by populating joinSource table parameter using filename, fieldList and options parameters.</remarks>
        /// \new490 Added in version 4.9.0
        public bool Join3(Table table2, string field1, string field2, string filenameToReopen, string joinOptions, Array fieldList)
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Gets number of joins for the table.
        /// </summary>
        /// \new490 Added in version 4.9.0
        public int JoinCount
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Serializes the state of joins for table to be restored later with Table.Deserialize method.
        /// </summary>
        /// <returns>String with serialized state.</returns>
        /// \new490 Added in version 4.9.0
        public string Serialize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Stops all joins associated with table.
        /// </summary>
        /// \new490 Added in version 4.9.0
        public void StopAllJoins()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Stops (removes) specific join created for the table.
        /// </summary>
        /// <param name="joinIndex">Index of join to be removed.</param>
        /// <returns>True on success.</returns>
        /// \new490 Added in version 4.9.0
        public bool StopJoin(int joinIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tries to create a join and reports number of rows to be joined for the specified join fields.
        /// </summary>
        /// <param name="table2">External table to join.</param>
        /// <param name="fieldTo">Join field of this table.</param>
        /// <param name="fieldFrom">Join field of external table.</param>
        /// <param name="rowCount">Number of rows in this table for which a join is found in external table.</param>
        /// <param name="joinRowCount">Number of unique values from external table, joined to this table.</param>
        /// <returns>True on success.</returns>
        /// <remarks>The method can be used to determine, if the join fields for table are chosen correctly, i.e. their values match.</remarks>
        /// \new490 Added in version 4.9.0
        public bool TryJoin(Table table2, string fieldTo, string fieldFrom, out int rowCount, out int joinRowCount)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value indicating whether the specified field is joined from external table.
        /// </summary>
        /// <param name="FieldIndex">Field index.</param>
        /// <returns>True if the field is brought from external table.</returns>
        /// \new490 Added in version 4.9.0
        public bool get_FieldIsJoined(int FieldIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the index of join operation by which the specified field was brought in.
        /// </summary>
        /// <param name="FieldIndex">Field index.</param>
        /// <returns>The index of join.</returns>
        /// \new490 Added in version 4.9.0
        public int get_FieldJoinIndex(int FieldIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets filename of the external datasource used in specified join.
        /// </summary>
        /// <param name="joinIndex">Index of join operation.</param>
        /// <returns>Filename of external datasource.</returns>
        /// \new490 Added in version 4.9.0
        public string get_JoinFilename(int joinIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets name of FromField in external table for specified join operation.
        /// </summary>
        /// <param name="joinIndex">Index of join operation.</param>
        /// <returns>Name of FromField in external table.</returns>
        /// \new490 Added in version 4.9.0
        public string get_JoinFromField(int joinIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets name of ToField in this table for specified join operation.
        /// </summary>
        /// <param name="joinIndex">Index of join operation.</param>
        /// <returns>Name of ToField in this table.</returns>
        /// \new490 Added in version 4.9.0
        public string get_JoinToField(int joinIndex)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif

