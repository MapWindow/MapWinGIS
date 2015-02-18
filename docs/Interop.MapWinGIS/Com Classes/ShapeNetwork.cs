
#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Represents a network built from the polyline shapefile.
    /// </summary>
    /// <remarks>A shape network is created from the shapes in a polyline shapefile which allows you to traverse the shapefile simulating 
    /// water flowing in a watershed to a single outlet. To create a shape network from a polyline shapefile, first choose an outlet 
    /// shape from the shapes in the shapefile. Next, choose an outlet point within the specified outlet shape. Finally, you need to 
    /// select a tolerance value. The tolerance is used to determine whether to include a shape in the shape network if it is not 
    /// connected to the network. Using this starting criteria, a shape network can be created from the polyline shapefile.  
    /// </remarks>
    #if nsp
        #if upd
            public class ShapeNetwork : MapWinGIS.IShapeNetwork
        #else        
                    public class IShapeNetwork
        #endif
    #else
        public class ShapeNetwork
    #endif
    {
        #region IShapeNetwork Members
        /// <summary>
        /// Builds a shape network from the specified line shapefile using the shape index to determine which shape in the shapefile is 
        /// to be used as the outlet shape.
        /// </summary>
        /// <param name="Shapefile">The shapefile to be used to create the shape network.</param>
        /// <param name="ShapeIndex">The index of the shape in the specified shapefile to be used as the outlet shape for the shape network</param>
        /// <param name="FinalPointIndex">The point index of the point in the specified outlet shape to be used as the outlet point for the shape network.</param>
        /// <param name="Tolerance">The tolerance used to find disconnected segments to merge into the network.</param>
        /// <param name="ar">The ambiguity resolution method used to resolve any ambiguity while building the shape network.
        /// Distance to outlet is the only ambiguity resolution method implemented.</param>
        /// <param name="cBack">Optional. The ICallback object which will receive progress and error messages during the
        /// creation of the shape network.</param>
        /// <returns>The success or failure of building the shape network. If a non-zero integer is returned, the 
        /// shape network build was successful. If a zero is returned, the shape network build was not 
        /// successful.</returns>
        public int Build(Shapefile Shapefile, int ShapeIndex, int FinalPointIndex, double Tolerance, AmbiguityResolution ar, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Closes the shape network.
        /// </summary>
        /// <returns>A boolean value representing the success or failure of closing the shape network.</returns>
        public bool Close()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the current shape object in the shape network. 
        /// </summary>
        public Shape CurrentShape
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the index of the current shape in the shape network. 
        /// </summary>
        public int CurrentShapeIndex
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Deletes a shape from the shape network. 
        /// </summary>
        /// <param name="ShapeIndex">The index of the shape to be deleted from the shape network. </param>
        /// <returns>A boolean value representing the success or failure of deleting the shape from the shape network.</returns>
        public bool DeleteShape(int ShapeIndex)
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
        /// Moves the current shape pointer down the network by one link.
        /// </summary>
        /// <returns>A boolean value representing the success or failure of moving the current shape pointer down the 
        /// shape network by one link.</returns>
        public bool MoveDown()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves the current shape pointer to the shape specified by the shape index.
        /// </summary>
        /// <param name="ShapeIndex">The shape index of the shape in the network to move the current shape pointer to.</param>
        /// <returns>A boolean value representing the success or failure of moving the current shape pointer to the 
        /// specified shape.</returns>
        public bool MoveTo(int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves the current shape pointer to the outlet shape in the shape network. 
        /// </summary>
        /// <returns>A boolean value representing the success or failure of moving the current shape pointer to the 
        /// outlet shape in the shape network.</returns>
        public bool MoveToOutlet()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves the current shape pointer up the shape network, taking the specified path. 
        /// </summary>
        /// <param name="UpIndex">The index of the shape in the shape network to use as the upstream path. This is necessary 
        /// because a node may have more than one upstream path. </param>
        /// <returns>A boolean value representing the success or failure of moving the current shape pointer up in the 
        /// shape network.</returns>
        public bool MoveUp(int UpIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of shapes in the shape network. 
        /// </summary>
        public int NetworkSize
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the number of upstream shapes in the network before the next fork. 
        /// </summary>
        public int NumDirectUps
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Opens a shape network.
        /// </summary>
        /// <param name="sf">The shapefile object to open as a shape network. </param>
        /// <param name="cBack">Optional. ICallback object which will receive progress and error messages while the shape network is opened.</param>
        /// <returns>A boolean value representing the success or failure of opening the shape network</returns>
        public bool Open(Shapefile sf, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the index of the parent of the current shape. If the current shape is the outlet shape in the shape network, -1 will be returned.
        /// </summary>
        public int ParentIndex
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Rasterizes the network into a D8 grid. 
        /// </summary>
        /// <param name="UseNetworkBounds">A boolean value representing whether the shape network bounds will be used as the only 
        /// bounds for the D8 grid or not. </param>
        /// <param name="Header">Optional. A grid header to use to create the new grid.</param>
        /// <param name="Cellsize">Optional. The cell size to use in creating the D8 grid.</param>
        /// <param name="cBack">Optional. The ICallback object which will receive progress and error messages when the grid is being created.</param>
        /// <returns>Optional. The rasterized Grid representing the shape network.</returns>
        public Grid RasterizeD8(bool UseNetworkBounds, GridHeader Header, double Cellsize, ICallback cBack)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the shapefile used to create the network.
        /// </summary>
        public Shapefile Shapefile
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the shape index of an ambiguous shape in the shape network.
        /// </summary>
        /// <remarks> The first ambiguous shape is at index 0. If there are no ambiguous shapes, 
        /// AmbigShapeIndex(0) will return -1. If AmbigShapeIndex returns -1 and the index you specified isn't 0, it 
        /// means there are no more ambiguous shapes in the shape network. 
        /// </remarks>
        /// <param name="Index">The index into the ambiguous shapes in the shape network. </param>
        /// <returns>The shape index of the ambiguous shape. Returns -1 if there isn't an ambiguous shape at the 
        /// specified ambiguous shape index.</returns>
        public int get_AmbigShapeIndex(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the distance from the specified point in the current shape to the outlet point in the shape network. 
        /// </summary>
        /// <param name="PointIndex">The index of a point in the current shape.</param>
        /// <returns>The distance from the specified point in the current shape to the outlet point in the shape network.</returns>
        public double get_DistanceToOutlet(int PointIndex)
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

        #endregion
    }
#if nsp
}
#endif

