using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Represents a function supported by built-in expression parser.
    /// </summary>
    /// \new494 Added in version 4.9.4
#if nsp
    #if upd
        public class Function : MapWinGIS.IFunction
    #else        
        public class IFunction        
    #endif
#else
    public class Function
#endif
    {
        /// <summary>
        /// Gets the name of the function.
        /// </summary>
        public string Name { get; private set; }

        /// <summary>
        /// Gets the alias of the function.
        /// </summary>
        /// <param name="aliasIndex">Index of the alias.</param>
        public string get_Alias(int aliasIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of aliases that the function has.
        /// </summary>
        public int NumAliases { get; private set; }

        /// <summary>
        /// Gets the number of parameters of the function.
        /// </summary>
        public int NumParameters { get; private set; }

        /// <summary>
        /// Gets the group to which this function belongs.
        /// </summary>
        public tkFunctionGroup Group { get; private set; }

        /// <summary>
        /// Gets the description of the function.
        /// </summary>
        public string Description { get; private set; }

        /// <summary>
        /// Gets the name of the specified parameter.
        /// </summary>
        /// <param name="parameterIndex">Index of the parameter.</param>
        /// <returns>The name of the parameter.</returns>
        public string get_ParameterName(int parameterIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the description of the specified parameter of the function.
        /// </summary>
        /// <param name="parameterIndex">Index of the parameter.</param>
        /// <returns>The description of the parameter.</returns>
        public string get_ParameterDescription(int parameterIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the signature of the function.
        /// </summary>
        public string Signature { get; private set; }
    }
#if nsp
}
#endif



