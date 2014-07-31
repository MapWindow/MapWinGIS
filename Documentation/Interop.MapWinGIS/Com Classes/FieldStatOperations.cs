using System;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Defines group statistic operations for columns of shapefile attribute table.
    /// </summary>
    /// <remarks>Multiple operations can be specified for each field. The results will be written to attribute table 
    /// of resulting shapefile. An instance of this class is used as a parameter by Shapefile.AggregateShapesWithStats
    /// and Shapefile.DissolveWithStats methods. See code sample in description of Shapefile.DissolveWithStats method.\n\n
    /// Validation of group operations against counties shapefiles and displaying report about validation:
    /// \code
    /// var operations = new FieldStatOperations();
    /// operations.AddFieldName("region", tkFieldStatOperation.fsoMin);
    /// operations.AddFieldName("population", tkFieldStatOperation.fsoAvg);
    /// 
    /// string filename = @"d:\counties.shp";
    /// var sf = new Shapefile();
    /// if (sf.Open(filename, null))
    /// {
    ///     operations.Validate(sf);
    ///     for (int i = 0; i < operations.Count; i++)
    ///     {
    ///         if (operations.get_OperationIsValid(i))
    ///         {
    ///             Debug.Print("Operation {0} is valid.", operations.get_Operation(i).ToString());
    ///         }
    ///         else
    ///         {
    ///             Debug.Print("Operation {0} is not valid. Reason: {1}", operations.get_Operation(i).ToString(),  
    ///                                                                    operations.get_OperationIsValidReason(i).ToString());
    ///         }
    ///     }
    /// }
    /// \endcode
    /// </remarks>
    /// \new491 Added in version 4.9.1
    #if nsp
        #if upd
            public class FieldStatOperations : MapWinGIS.IFieldStatOperations
        #else        
            public class IFieldStatOperations
        #endif
    #else
        public class FieldStatOperations
    #endif
    {
        /// <summary>
        /// Adds statistic operation for a specified field.
        /// </summary>
        /// <param name="FieldIndex">Field index in input shapefile.</param>
        /// <param name="Operation">Operation to calculate.</param>
        public void AddFieldIndex(int FieldIndex, tkFieldStatOperation Operation)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds statistic operation for a specified field.
        /// </summary>
        /// <param name="Fieldname">Name of field in input shapefile (case-insensitive)</param>
        /// <param name="Operation">Operation to calculate.</param>
        public void AddFieldName(string Fieldname, tkFieldStatOperation Operation)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all previously added operations
        /// </summary>
        public void Clear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of added operations.
        /// </summary>
        public int Count
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// A text string associated with object. Any value can be stored by developer in this property.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the code of last error which took place inside this object.
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Removes operations with specified index.
        /// </summary>
        /// <param name="operationIndex">Index of operation to remove.</param>
        /// <returns>True on success, false if operation index is invalid.</returns>
        public bool Remove(int operationIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Checks whether current list of operations is applicable for a given shapefile.
        /// </summary>
        /// <remarks>For invalid operations get_OperationIsValid flag and get_OperationIsValidReason property are set.
        /// Invalid operations will be skipped when when doing statistic calculations and no output fields will be created for them.</remarks>
        /// <param name="sf">Shapefile to validate operations list against.</param>
        /// <returns>True if all operations are valid.</returns>
        public bool Validate(Shapefile sf)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the description of the specific error code.
        /// </summary>
        /// <param name="ErrorCode">The error code returned by LastErrorCode property.</param>
        /// <returns>String with the description.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets field index for operation at specified position in the list.
        /// </summary>
        /// <param name="operationIndex">Index of operation in list.</param>
        /// <returns>Field index in input shapefile.</returns>
        public int get_FieldIndex(int operationIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets field name for operation at specified position in the list.
        /// </summary>
        /// <param name="operationIndex">Index of operation in list.</param>
        /// <returns>Field name in input shapefile.</returns>
        public string get_Fieldname(int operationIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets operation at specified position in the list.
        /// </summary>
        /// <param name="operationIndex">Index of operation in list.</param>
        /// <returns>Operation at specified position (fsoSum will be returned in case of invalid operation index).</returns>
        public tkFieldStatOperation get_Operation(int operationIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value of IsValid flag for operation. 
        /// </summary>
        /// <remarks>
        /// The flag is set only after FieldStatOperations.Validate methods was called. 
        /// The same list of operations may be valid for one shapefile and invalid for another.
        /// </remarks>
        /// <param name="operationIndex">Index of operation in list.</param>
        /// <returns>True in case operation is valid.</returns>
        public bool get_OperationIsValid(int operationIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets a value indicating the reason why the specific operation is invalid.
        /// </summary>
        /// <param name="operationIndex">Index of operation to check the property for.</param>
        /// <returns>Reason for invalidity or fovValid in operation is applicable.</returns>
        public tkFieldOperationValidity get_OperationIsValidReason(int operationIndex)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif

