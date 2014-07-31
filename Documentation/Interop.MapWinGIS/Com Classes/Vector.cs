
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    /// <summary>
    /// A vector object is used to represent the light source for a grid color scheme. 
    /// </summary>
    #if nsp
        #if upd
            public class Vector : MapWinGIS.IVector
        #else        
            public class IVector
        #endif
    #else
        public class Vector
    #endif
    {
        #region IVector Members
        /// <summary>
        /// Calculates the vector cross product between the current vector object and the specified vector. 
        /// </summary>
        /// <param name="V">The vector to calculate the cross product with the current vector object.</param>
        /// <returns>The cross product of the current vector object and the specified vector.</returns>
        public Vector CrossProduct(Vector V)
        {
            throw new NotImplementedException();
        }
            
        /// <summary>
        /// Calculates the dot product of the current vector object with the specified vector. 
        /// </summary>
        /// <param name="V">The vector to use to calculate the dot product with the current vector object.</param>
        /// <returns>The dot product of the current vector object and the specified vector.</returns>
        public double Dot(Vector V)
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
        /// Normalizes the vector.
        /// </summary>
        public void Normalize()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Retrieves the error message associated with the specified error code. 
        /// </summary>
        /// <param name="ErrorCode"></param>
        /// <returns></returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the i component of the vector. 
        /// </summary>
        public double i
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the j component of the vector. 
        /// </summary>
        public double j
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the k component of the vector. 
        /// </summary>
        public double k
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        #endregion
    }
#if nsp
}
#endif

