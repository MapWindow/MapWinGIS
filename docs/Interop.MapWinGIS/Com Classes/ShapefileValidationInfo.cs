#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Provides information about validation of a single shapefile before or after certain operation.
    /// </summary>
    /// \new491 Added in version 4.9.1
#if nsp
    #if upd
        public class ShapeValidationInfo : MapWinGIS.IShapeValidationInfo
    #else        
        public class IShapeValidationInfo
    #endif
#else
        public class ShapeValidationInfo
#endif
    {
        /// <summary>
        /// Gets name of the class geoprocessing method was an from.
        /// </summary>
        public string ClassName
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Gets number of errors that were fixed, i.e. invalid shapes were substituted with fixed ones.
        /// </summary>
        public int FixedCount
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Get a value indicating whether the input or output was finally considered valid (probably after fixing invalid shapes).
        /// </summary>
        public bool IsValid
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Gets the name of geoprocessing methods that was validated.
        /// </summary>
        public string MethodName
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Gets the name of parameter of geoprocessing methods that was validated.
        /// </summary>
        /// <remarks>Only parameters of Shapefile type are validated. "this" will be returned 
        /// in case shapefile object itself from which the method was ran was validated.</remarks>
        public string ParameterName
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Gets number of invalid shapes that were skipped during validation (i.e. input shapes not processed at all 
        /// or output shapes excluded from output).
        /// </summary>
        public int SkippedCount
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Gets status of validation operation.
        /// </summary>
        public tkShapeValidationStatus Status
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Gets number of shape that were still invalid after possible attempts to fix them.
        /// </summary>
        public int StillInvalidCount
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Gets validation mode that was used.
        /// </summary>
        /// <remarks>The mode can be changed in GlobalSettings.ShapeInputValidationMode and 
        /// GlobalSettings.ShapeOutputValidationMode</remarks>
        public tkShapeValidationMode ValidationMode
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Gets a value indicating whether it was input or output validation.
        /// </summary>
        public tkShapeValidationType ValidationType
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Gets a value indicating whether validation was performed .
        /// </summary>
        /// <remarks>I.e. whether ValidationMode was set to something other than NoValidation.</remarks>
        public bool WasValidated
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary>
        /// Gets number of shapes that were initially invalid. Depending on validation mode they might be fixed.
        /// </summary>
        public int WereInvalidCount
        {
            get { throw new System.NotImplementedException(); }
        }
    }
#if nsp
}
#endif

