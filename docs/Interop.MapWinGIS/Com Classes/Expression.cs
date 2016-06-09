using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Represents expression parser and evaluator.
    /// </summary>
    /// \new494 Added in version 4.9.4
#if nsp
    #if upd
        public class Expression : MapWinGIS.IExpression
    #else        
        public class IExpression        
    #endif
#else
    public class Expression
#endif
    {
        /// <summary>
        /// Parses the specifies string and build an expression out of it.
        /// </summary>
        /// <param name="expr">The string with expression.</param>
        /// <returns>True on success.</returns>
        public bool Parse(string expr)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the expression.
        /// </summary>
        /// <param name="result">The result.</param>
        /// <returns>Result which is of string, double, or boolean type.</returns>
        public bool Calculate(out object result)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Parses expression seeking for fields in the specified table.
        /// </summary>
        /// <param name="expr">The expression to parse.</param>
        /// <param name="tbl">The table.</param>
        /// <returns>True on success.</returns>
        public bool ParseForTable(string expr, Table tbl)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the expression for the given table row and writes the result to the specified field.
        /// </summary>
        /// <param name="RowIndex">Index of the row.</param>
        /// <param name="targetFieldIndex">Index of the target field.</param>
        /// <returns>True on success.</returns>
        public bool CalculateForTableRow(int RowIndex, int targetFieldIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the expression for the given table row.
        /// </summary>
        /// <param name="RowIndex">Index of the row.</param>
        /// <param name="result">The result of calculation (string, double, or boolean).</param>
        /// <returns>True on success.</returns>
        public bool CalculateForTableRow2(int RowIndex, out object result)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the last error message.
        /// </summary>
        public string LastErrorMessage { get; private set; }

        /// <summary>
        /// Gets the position of the last error.
        /// </summary>
        public int LastErrorPosition { get; private set; }

        /// <summary>
        /// Gets the number of functions recognized by the expression parser.
        /// </summary>
        public int NumSupportedFunctions { get; private set; }

        /// <summary>
        /// Gets the specified function from the list of function recognized by the expression parser.
        /// </summary>
        /// <param name="functionIndex">Index of the function.</param>
        /// <returns>The function object with metadata.</returns>
        public Function get_SupportedFunction(int functionIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the table associated with the current expression.
        /// </summary>
        public Table Table { get; private set; }
    }
#if nsp
}
#endif



