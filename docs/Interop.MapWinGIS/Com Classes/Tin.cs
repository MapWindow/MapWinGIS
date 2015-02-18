
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Represents a triangulated irregular network which may be displayed in the map.
    /// </summary>
    /// <remarks> A tin may be created from a grid, or opened from file.</remarks>
    #if nsp
        #if upd
            public class Tin : MapWinGIS.ITin
        #else        
            public class ITin
        #endif
    #else
        public class Tin
    #endif
    {
        #region ITin Members
        /// <summary>
        /// The string for common open dialog for filtering files which may be used as a source for TIN (*.tin).
        /// </summary>
        public string CdlgFilter
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Closes the TIN.
        /// </summary>
        /// <returns>A boolean value representing the success or failure of closing the TIN.</returns>
        public bool Close()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new TIN object from the specified grid. 
        /// </summary>
        /// <param name="Grid">The grid to be used to create the new TIN.</param>
        /// <param name="Deviation">If the distance between the grid elevation and the triangle surface elevation at any given 
        /// point is greater than this value, the triangle will be split at this location. This value is in projected map coordinates.</param>
        /// <param name="SplitTest">The method to use when splitting triangles.</param>
        /// <param name="STParam">Split Test Parameter. This value depends on the SplitMethod specified. It will either be the 
        /// smallest inscribed radius allowed measured in projected map coordinates, or it will be the         /// smallest angle allowed measured in degrees.</param>
        /// <param name="MeshDivisions">This is the number of dividers used to create an initial mesh for the algorithm. Each cell in 
        /// the initial mesh is subdivided into smaller triangles depending on the deviation within the cell.  </param>
        /// <param name="MaximumTriangles">Optional. The maximum number of triangles allowed in the TIN. The default value is 1073741824. </param>
        /// <param name="cBack">Optional. The ICallback object which will receive progress and error messages while the TIN is being created.</param>
        /// <returns>A boolean value representing the success or failure of creating the new TIN.</returns>
        public bool CreateNew(Grid Grid, double Deviation, SplitMethod SplitTest, double STParam, int MeshDivisions, int MaximumTriangles, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a TIN from the array of points.
        /// </summary>
        /// <param name="Points">An array of Point type.</param>
        /// <returns>True on success and false otherwise.</returns>
        public bool CreateTinFromPoints(Array Points)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The filename associated with the object.
        /// </summary>
        public string Filename
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

        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the maximum x, y, and z values of the TIN's extents. 
        /// </summary>
        /// <param name="x">Returns the maximum x value of the TIN's extents.</param>
        /// <param name="y">Returns the maximum y value of the TIN's extents.</param>
        /// <param name="Z">Returns the maximum Z value of the TIN's extents.</param>
        public void Max(out double x, out double y, out double Z)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the minimum x, y, and z values of the TIN's extents.
        /// </summary>
        /// <param name="x">Returns the minimum x value of the TIN's extents.  </param>
        /// <param name="y">Returns the minimum y value of the TIN's extents</param>
        /// <param name="Z">Returns the minimum Z value of the TIN's extents.</param>
        public void Min(out double x, out double y, out double Z)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the number of the triangles in the TIN.
        /// </summary>
        public int NumTriangles
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the number of the vertices in the TIN.
        /// </summary>
        public int NumVertices
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Opens a TIN from the specified file.
        /// </summary>
        /// <param name="TinFile">The filename of the TIN to be opened.</param>
        /// <param name="cBack">.Optional. The ICallback object which will receive progress and error messages while the TIN is being opened.</param>
        /// <returns>A boolean value representing the success or failure of opening the TIN.</returns>
        public bool Open(string TinFile, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Saves the TIN object under the specified filename.
        /// </summary>
        /// <param name="TinFilename">The filename to be used to save the TIN.</param>
        /// <param name="cBack">Optional. The ICallback object which will receive progress and error messages while the TIN is being saved.</param>
        /// <returns>A boolean value representing the success or failure of saving the TIN. </returns>
        public bool Save(string TinFilename, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Checks if there is a triangle in the TIN at the specified x and y coordinate.
        /// </summary>
        /// <remarks>The index of any triangle at that location is returned through TriangleHint, 
        /// and the z coordinate for the triangle surface at the specified location is returned through Z. 
        /// </remarks>
        /// <param name="TriangleHint">The index of the triangle to start the search with. If the triangle hint is a 
        /// good guess, the search can be much faster.</param>
        /// <param name="x">The x projected map coordinate of the point that is being used to test if it lies within a TIN triangle.</param>
        /// <param name="y">The y projected map coordinate of the point that is being used to test if it lies within a TIN triangle.</param>
        /// <param name="Z">The z projected map coordinate of the selected triangle's surface will be returned by this parameter.</param>
        /// <returns></returns>
        public bool Select(ref int TriangleHint, double x, double y, out double Z)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the vertex indices of the specified triangle in the TIN.
        /// </summary>
        /// <param name="TriIndex">The index of the triangle in the TIN for which the vertex indices are required.</param>
        /// <param name="vtx1Index">The index of the first vertex in the specified triangle in the TIN.</param>
        /// <param name="vtx2Index">The index of the second vertex in the specified triangle in the TIN.</param>
        /// <param name="vtx3Index">The index of the third vertex in the specified triangle in the TIN.</param>
        public void Triangle(int TriIndex, out int vtx1Index, out int vtx2Index, out int vtx3Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the indices of the specified triangle's neighboring triangles in the TIN.
        /// </summary>
        /// <param name="TriIndex">The index of the triangle to get the neighbors of in the TIN.</param>
        /// <param name="triIndex1">The index of the first neighbor the specified triangle.</param>
        /// <param name="triIndex2">The index of the second neighbor the specified triangle.</param>
        /// <param name="triIndex3">The index of the third neighbor the specified triangle.</param>
        public void TriangleNeighbors(int TriIndex, ref int triIndex1, ref int triIndex2, ref int triIndex3)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the x, y, and Z projected map coordinates of the specified vertex in the TIN. 
        /// </summary>
        /// <param name="VtxIndex">The index of the vertex for which the x, y, and Z coordinates are required.</param>
        /// <param name="x">Returns the x projected map coordinate of the specified vertex in the TIN.</param>
        /// <param name="y">Returns the x projected map coordinate of the specified vertex in the TIN.</param>
        /// <param name="Z">Returns the x projected map coordinate of the specified vertex in the TIN. </param>
        public void Vertex(int VtxIndex, out double x, out double y, out double Z)
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
        /// Gets whether or not a triangle is a no-data triangle.
        /// </summary>
        /// <param name="TriIndex">The index of the triangle to be tested whether or not it is a no-data triangle.</param>
        /// <returns>A boolean value representing whether or not the triangle is a no-data triangle.</returns>
        public bool get_IsNDTriangle(int TriIndex)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
#if nsp
}
#endif

