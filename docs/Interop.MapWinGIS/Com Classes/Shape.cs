
#if nsp
namespace MapWinGIS
{
#endif
    using System;
    
    /// <summary>
    /// A shape object represents a geometric shape which can be added to a shapefile which is displayed in the map. 
    /// </summary>
    /// Here is a diagram for the Shape class.
    /// \dot
    /// digraph shape_diagram {
    /// nodesep = 0.3;
    /// ranksep = 0.3;
    /// splines = ortho;
    /// 
    /// node [shape= "polygon", peripheries = 3, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.2, width = 0.8];
    /// pnt [ label="Point" URL="\ref Point"];
    /// shp [ label="Shape" URL="\ref Shape"];
    /// 
    /// node [color = tan peripheries = 1 height = 0.3, width = 1.0];
    /// sf [ label="Shapefile" URL="\ref Shapefile"];
    /// 
    /// edge [ dir = none, style = solid, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060", labeldistance = 0.6 ]
    /// shp -> pnt [ URL="\ref Shape.get_Point()", tooltip = "Table.get_Point()", headlabel = "   n"];
    /// sf -> shp [ URL="\ref Shapefile.get_Shape()", tooltip = "Shapefile.get_Shape", headlabel = "   n"];
    /// }
    /// \enddot
    /// <a href = "diagrams.html">Graph description</a>
    #if nsp
        #if upd
            public class Shape : MapWinGIS.IShape
        #else        
            public class IShape
        #endif
    #else
        public class Shape
    #endif
    {
        #region IShape Members
        /// <summary>
        /// Calculates the area of the shape. For non-polygon shapes this property will return 0.0.
        /// </summary>
        /// <remarks>The area will always be returned in current map units.
        /// No corrections are made to take into account the curved shape of Earth.</remarks>
        /// \new48 Added in version 4.8
        public double Area
        {
            get { throw new NotImplementedException(); }
             //"New API 4.8" "New API members 4.8" Added in the version 4.8.
        }

        /// <summary>
        /// Generates a shape which represents a boundary of the current shape. 
        /// </summary>
        /// <returns>The reference to the resulting shape or NULL on failure.</returns>
        /// \new48 Added in version 4.8
        public Shape Boundary()
        {
            throw new NotImplementedException();
            //"New API 4.9" "New API members 4.9" Added in the version 4.9.
        }

        /// <summary>
        /// Builds a buffer of the specified distance around the shape.
        /// </summary>
        /// <param name="Distance">The buffer distance in map units.</param>
        /// <param name="nQuadSegments">The number of segments use to approximate a circle buffer.</param>
        /// <returns>The reference to the resulting shape which holds the buffer or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shape Buffer(double Distance, int nQuadSegments)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates a center of the shape's bounding box.
        /// </summary>
        /// \new48 Added in version 4.8
        public Point Center
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Calculates a centroid (center of mass) of the shape.
        /// </summary>
        /// \new48 Added in version 4.8
        public Point Centroid
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Performs clipping operation with 2 shapes.
        /// </summary>
        /// <remarks>The available operation are:
        /// - difference;
        /// - intersection;
        /// - symmetrical difference;
        /// - union;
        /// . 
        /// Note that clClip operation will yield the same result as intersection.
        /// </remarks>
        /// <param name="Shape">The second shape for the operation.</param>
        /// <param name="Operation">The operation to perform.</param>
        /// <returns>The reference to the resulting shape or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shape Clip(Shape Shape, tkClipOperation Operation)
        {
            throw new NotImplementedException();
        }

       

        /// <summary>
        /// Creates a deep copy of this object with the same type, parts and points.
        /// </summary>
        /// <returns>The copy of the shape.</returns>
        /// \new48 Added in version 4.8
        public Shape Clone()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tests a "contains" relation between 2 shapes.
        /// </summary>
        /// <param name="Shape">The second shape.</param>
        /// <returns>True if this shape contains the second shape and false if it is not.</returns>
        /// \new48 Added in version 4.8
        public bool Contains(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Builds a convex hull around the current shape.
        /// </summary>
        /// <returns>A new shape with convex hull of the current shape or NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shape ConvexHull()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a new shape of the specified type. 
        /// </summary>
        /// <param name="ShpType">The type of the shape to be created.</param>
        /// <returns>A boolean value representing the success or failure of creating the new shape.</returns>
        public bool Create(ShpfileType ShpType)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Initializes the Shape object and fills it with the geometry defined by the input string. </summary>
        /// <remarks>The input string should be in the 
        /// serialized string format as produced by the function Shape.SerializeToString. </remarks>
        /// <param name="Serialized">The serialized string to load.</param>
        /// <returns>A boolean value representing the success or failure of loading the shape.</returns>
        public bool CreateFromString(string Serialized)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tests a "crosses" relation between 2 shapes.
        /// </summary>
        /// <param name="Shape">The second shape.</param>
        /// <returns>True if this shape crosses the second shape and false if it is not.</returns>
        /// \new48 Added in version 4.8
        public bool Crosses(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deletes a part from a shape. 
        /// </summary>
        /// <param name="PartIndex">The index of the part to be deleted.</param>
        /// <returns>A boolean value representing the success or failure of deleting the part.</returns>
        public bool DeletePart(int PartIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deletes a point in the shape.
        /// </summary>
        /// <param name="PointIndex">The index of the point in the shape to be deleted.</param>
        /// <returns>A boolean value representing the success or failure of deleting the point in the shape. </returns>
        public bool DeletePoint(int PointIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tests a "disjoint" relation between 2 shapes.
        /// </summary>
        /// <param name="Shape">The second shape.</param>
        /// <returns>True if this shape is disjoint from the second shape and false if it is not.</returns>
        /// \new48 Added in version 4.8
        public bool Disjoint(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tests a 'covers' relation between 2 shapes.
        /// </summary>
        /// <param name="shape">The second shape.</param>
        /// <returns>True if this shape covers the second shape and false if it is not.</returns>
        /// \new52 Added in version 5.2
        public bool Covers(Shape shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tests a 'covered by' relation between 2 shapes.
        /// </summary>
        /// <param name="shape">The second shape.</param>
        /// <returns>True if this shape is covered by the second shape and false if it is not.</returns>
        /// \new52 Added in version 5.2
        public bool CoveredBy(Shape shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the distance between 2 shapes.
        /// </summary>
        /// <param name="Shape">The second shape.</param>
        /// <returns>The calculated distance.</returns>
        /// \new48 Added in version 4.8
        public double Distance(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tests an "equals" relation between 2 shapes.
        /// </summary>
        /// <param name="Shape">The second shape.</param>
        /// <returns>True if this shape is equal to the second shape and false if it is not.</returns>
        /// \new48 Added in version 4.8
        public bool Equals(Shape Shape)
        {
            throw new NotImplementedException();
        }
        

        /// <summary>
        /// Splits multipart shape into simple shapes.
        /// </summary>
        /// <remarks>Holes of the polygon will not be considered as separate shapes. However
        /// if a polygon holds several not nested rings they will be passed to output as individual polygons.</remarks>
        /// <param name="Results">The array of Shape type with the resulting shapes.</param>
        /// <returns>The on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool Explode(ref object Results)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns binary representation of the shape data. 
        /// </summary>
        /// <remarks>The binary data will have the same format as disk representation of the shapefile.</remarks>
        /// <param name="bytesArray">The byte array with shape data.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool ExportToBinary(ref object bytesArray)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the extents of the shape.
        /// </summary>
        public Extents Extents
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Fixes the shape in case it is not valid.
        /// </summary>
        /// <remarks>Converts the data to GEOS geometry and builds a small buffer. 
        /// Call Shape.IsValid to find out the invalid shapes.</remarks>
        /// <param name="retval">The fixed shape.</param>
        /// \new48 Added in version 4.8
        public void FixUp(out Shape retval)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Fixes the shape in case it is not valid.
        /// </summary>
        /// <param name="units">The units of measure of the coordinate system.</param>
        /// <returns>The fixed shape.</returns>
        /// \new494 Added in version 4.9.4
        public Shape FixUp2(tkUnitsOfMeasure units)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the intersection of 2 shapes.
        /// </summary>
        /// <remarks>In comparison with Shape.Clip this function returns all the results of intersection
        /// even if they have different shape type. For example the intersection of 2 polygons can hold:
        /// new polygons, polylines and points simultaneously.</remarks>
        /// <param name="Shape">The second shape.</param>
        /// <param name="Results">The array of Shape type with the results of intersection.</param>
        /// <returns>True in case at least one shape is returned and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool GetIntersection(Shape Shape, ref object Results)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets a callback object for reporting about the errors.
        /// </summary>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Restores the state of the shape from binary representation obtained by Shape.ExportToBinary.
        /// </summary>
        /// <param name="bytesArray">The byte array with the state of shape.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool ImportFromBinary(object bytesArray)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inserts a part into the shape. 
        /// </summary>
        /// <remarks>
        /// Parts are used to create polygons with holes. Parts with points ordered in a clockwise direction 
        /// are filled. Parts with points ordered in a counter-clockwise direction are cut out. Only clockwise parts should be used to 
        /// define the outer-most regions of a shape.
        /// </remarks>
        /// <param name="PointIndex">The index of the first point in the part to be inserted.</param>
        /// <param name="PartIndex">The part index desired. This value may be modified if it is not possible to use the desired part index.</param>
        /// <returns>A boolean value representing the success or failure of inserting the part into the shape.</returns>
        public bool InsertPart(int PointIndex, ref int PartIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Inserts the specified point object into the shape using the desired point index if possible.
        /// </summary>
        /// <param name="NewPoint">The point object to be inserted into the shape. </param>
        /// <param name="PointIndex">Reference parameter. The index where the point will be placed if possible. If the desired index 
        /// cannot be used, the actual index will be returned.</param>
        /// <returns>A boolean value representing the success or failure of inserting the point into the shape.</returns>
        public bool InsertPoint(Point NewPoint, ref int PointIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the interior point of the shape. Is applicable for polygon shapes only.
        /// </summary>
        /// The algorithm works as following:
        /// - a line is defined which crosses the bounding box of the polygon horizontally in its center;
        /// - the intersection of this line and polygon is calculated which can be represented by single segment or by a number of segments;
        /// - the longest segment is then chosen and the point on it's center is returned.
        /// \new48 Added in version 4.8
        public Point InteriorPoint
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Tests an "intersects" relation between 2 shapes.
        /// </summary>
        /// <param name="Shape">The second shape.</param>
        /// <returns>True if this shape intersects the second shape and false if it is not.</returns>
        /// \new48 Added in version 4.8
        public bool Intersects(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a boolean value which indicates whether a shape is valid. 
        /// </summary>
        /// <remarks>
        /// Shapes is considered to be valid when it meets specific topological rules for particular shape type.
        /// </remarks>
        /// \new48 Added in version 4.8
        public bool IsValid
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the string with the description of reason why shape was considered invalid.
        /// </summary>
        /// \see Shape.IsValid.
        /// \new48 Added in version 4.8
        public string IsValidReason
        {
            get { throw new NotImplementedException(); }
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
        /// Calculates the length of polyline shape.
        /// </summary>
        /// <remarks>The length will always be returned in current map units.
        /// No corrections are made to take into account the curved shape of the Earth.</remarks>
        public double Length
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the number of parts contained in the shape.
        /// </summary>
        /// <remarks>A polygon shape may have several parts. An example of this would be a 
        /// doughnut shape. The outermost perimeter would be one part, and the hole cut out of the center making up the innermost 
        /// perimeter would be the second part. The outermost perimeter would be specified by a series of points arranged in clockwise 
        /// order, meaning that this part will be filled. The innermost perimeter would be specified by a series of points arranged in 
        /// counter-clockwise order, meaning that this part will not be filled. </remarks>
        public int NumParts
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Tests an "overlaps" relation between 2 shapes.
        /// </summary>
        /// <param name="Shape">The second shape.</param>
        /// <returns>True if this shape overlaps the second shape and false if it is not.</returns>
        /// \new48 Added in version 4.8
        public bool Overlaps(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the perimeter of shape. Is applicable for polygon shapes only.
        /// </summary>
        /// <remarks>The length will always be returned in current map units.
        /// No corrections are made to take into account the curved shape of the Earth.</remarks>
        /// \new48 Added in version 4.8
        public double Perimeter
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Performs a test to find out whether a given point lies within the polygon.
        /// </summary>
        /// <param name="pt">The point to test.</param>
        /// <returns>True if points lies within polygon and false otherwise.</returns>
        public bool PointInThisPoly(Point pt)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tests the specified relation between 2 shapes. 
        /// </summary>
        /// <remarks>This method works the same as individual overloads like Shape.Intersects, Shape.Overlaps, etc.</remarks>
        /// <param name="Shape">The second shape.</param>
        /// <param name="Relation">The relation to test.</param>
        /// <returns>True in case this relation is actually takes place and false if it is not.</returns>
        /// \new48 Added in version 4.8
        public bool Relates(Shape Shape, tkSpatialRelation Relation)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Changes the order of points within shape part to the opposite.
        /// </summary>
        /// <remarks>The property can be useful to to ensure proper topology for polygons.
        /// The points of outer ring of polygon must have clockwise order, while the points of its holes
        /// must be specified in counter-clockwise order.</remarks>
        /// <param name="PartIndex">The index of part to reverse the order of points.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new48 Added in version 4.8
        public bool ReversePointsOrder(int PartIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Provides a string representing the shape's geometry. 
        /// </summary>
        /// <remarks>The string will be in the serialized string format and can be reloaded with Shape.CreateFromString. </remarks>
        /// <returns>A string representing the shape geometry.</returns>
        public string SerializeToString()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the type of the shape.
        /// </summary>
        /// <remarks>The shape's type must match the type of the shapefile the shape is to be added to 
        /// except for shapes of type SHP_NULLSHAPE. </remarks>
        public ShpfileType ShapeType
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Tests a "touches" relation between 2 shapes.
        /// </summary>
        /// <param name="Shape">The second shape.</param>
        /// <returns>True if this shape touches the second shape and false if it is not.</returns>
        /// \new48 Added in version 4.8
        public bool Touches(Shape Shape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Tests a "within" relation between 2 shapes.
        /// </summary>
        /// <param name="Shape">The second shape.</param>
        /// <returns>True if this shape lies within the second shape and false if it is not.</returns>
        /// \new48 Added in version 4.8
        public bool Within(Shape Shape)
        {
            throw new NotImplementedException();
        }
            
        /// <summary>
        /// Returns the index of the last point in the part.
        /// </summary>
        /// <param name="PartIndex">The index of the part.</param>
        /// <returns>The index of the point.</returns>
        /// \new48 Added in version 4.8
        public int get_EndOfPart(int PartIndex)
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
        /// Gets or sets the first point index in the specified part. 
        /// </summary>
        /// <param name="PartIndex">The index of the part for which the first point index is required.</param>
        /// <returns>The index of the first point in the specified part.</returns>
        public int get_Part(int PartIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns part of the shape as a new shape.
        /// </summary>
        /// <remarks>The initial shape and the returned one doesn't shape any common memory.</remarks>
        /// <param name="PartIndex">The index of part to return.</param>
        /// <returns>The reference to the new shape and NULL reference on failure.</returns>
        /// \new48 Added in version 4.8
        public Shape get_PartAsShape(int PartIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a boolean value which indicates whether points of the shape part are placed in the clockwise order.
        /// </summary>
        /// <param name="PartIndex">The index of part.</param>
        /// <returns>True for the clockwise order, and false for counter-clockwise.</returns>
        /// \new48 Added in version 4.8
        public bool get_PartIsClockWise(int PartIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or sets the specified point in the shape. 
        /// </summary>
        /// <param name="PointIndex">The index of the point which is to be accessed in the shape.</param>
        /// <returns>The specified point in the shape.</returns>
        public Point get_Point(int PointIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the coordinates of the specified point.
        /// </summary>
        /// <remarks>It's highly recommended to use this property rather than Shape.get_Point,
        /// for considerable performance boost when Shapefile.FastMode is set to true.</remarks>
        /// <param name="PointIndex">The index of point.</param>
        /// <param name="x">The x coordinate.</param>
        /// <param name="y">The y coordinate.</param>
        /// <returns>True on successful retrieval and false on wrong index of point.</returns>
        /// \new48 Added in version 4.8
        public bool get_XY(int PointIndex, ref double x, ref double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the number of points contained in the shape. 
        /// </summary>
        /// <remarks>There is a confusion with name casing for this property. 
        /// It's actually spelled 'get_NumPoints' in the source code. 
        /// However because of the known bug in PIA generation process the property 
        /// ended up spelled 'get_numPoints' in earlier version of PIAs. After some hesitation we decided to keep it that way. 
        /// See description of the bug here: http://stackoverflow.com/questions/3179676/c-exposed-property-for-com-in-idl-is-showing-as-lower-case-all-of-a-sudden </remarks>
        public int numPoints
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Sets the coordinates of the specified point.
        /// </summary>
        /// <remarks>It's highly recommended to use this property rather than Shape.get_Point,
        /// for considerable performance boost when Shapefile.FastMode is set to true.</remarks>
        /// <param name="PointIndex">The index of point.</param>
        /// <param name="x">The x coordinate.</param>
        /// <param name="y">The y coordinate.</param>
        /// <returns>True on success and false on the invalid index.</returns>
        /// \new48 Added in version 4.8
        public bool put_XY(int PointIndex, double x, double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the index of the first point in part.
        /// </summary>
        /// <param name="PartIndex">The index of part.</param>
        /// <param name="pVal">The index of point.</param>
        public void set_Part(int PartIndex, int pVal)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Replaces the point with the specified index with new one.
        /// </summary>
        /// <param name="PointIndex">The index of point to replace.</param>
        /// <param name="pVal">The new point.</param>
        public void set_Point(int PointIndex, Point pVal)
        {
            throw new NotImplementedException();
        }

        #endregion

        /// <summary>
        /// Adds a point to the shape.
        /// </summary>
        /// <param name="x">The x coordinate of the point.</param>
        /// <param name="y">The y coordinate of the point.</param>
        /// <returns>The index of the new point.</returns>
        /// \new490 Added in version 4.9.0
        public int AddPoint(double x, double y)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates closest points of the 2 shapes
        /// </summary>
        /// <param name="shape2">Second shape</param>
        /// <returns>Polyline shape with 2 points, the first one - closest point of this shape, the second one - closest point of parameter shape.</returns>
        /// <remarks>Uses GEOS implementation, works for all shape types. For any point lying within polygon the closest point between it and polygon will be the point itself.</remarks>
        /// \new491 Added in version 4.9.1
        public Shape ClosestPoints(Shape shape2)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Imports shape data from WKT format.
        /// </summary>
        /// <param name="Serialized">Shape data in WKT format.</param>
        /// <returns>True on success and false otherwise.</returns>
        /// \new491 Added in version 4.9.1
        public bool ImportFromWKT(string Serialized)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Exports shape data to WKT format.
        /// </summary>
        /// <returns>String in WKT format with shape data.</returns>
        /// \new490 Added in version 4.9.0
        public string ExportToWKT()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a deep copy of shape to another instance, including points and parts.
        /// </summary>
        /// <param name="sourceShape">Source shape to copy from.</param>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool CopyFrom(Shape sourceShape)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets M value for specified point.
        /// </summary>
        /// <param name="PointIndex">Index of point.</param>
        /// <param name="M">M value.</param>
        /// <returns>True in case there are M values for current shape type.</returns>
        /// \new491 Added in version 4.9.1
        public bool get_M(int PointIndex, out double M)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets M value for specified point.
        /// </summary>
        /// <param name="PointIndex">Index of point.</param>
        /// <param name="Z">Z value.</param>
        /// <returns>True in case there are Z values for current shape type.</returns>
        /// \new491 Added in version 4.9.1
        public bool get_Z(int PointIndex, out double Z)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Puts M value for specified point.
        /// </summary>
        /// <param name="PointIndex">Index of point.</param>
        /// <param name="M">M value.</param>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool put_M(int PointIndex, double M)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Puts Z value for specified point.
        /// </summary>
        /// <param name="PointIndex">Index of point.</param>
        /// <param name="Z">Z value.</param>
        /// <returns>True on success.</returns>
        /// \new491 Added in version 4.9.1
        public bool put_Z(int PointIndex, double Z)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a new shape which constitutes a buffer around original one.
        /// </summary>
        /// <param name="Distance">Buffer width (same units as shape coordinates are set with).</param>
        /// <param name="numSegments">Number of segments to approximate curves.</param>
        /// <param name="singleSided">True if buffer if single sided buffer is needed (works for polylines only).</param>
        /// <param name="capStyle">Type of caps.</param>
        /// <param name="joinStyle">Type of joins.</param>
        /// <param name="mitreLimit">Mitre limit.</param>
        /// <returns>New buffered shape on success or null on failure.</returns>
        /// \new493 Added in version 4.9.3
        public Shape BufferWithParams(double Distance, int numSegments = 30, bool singleSided = false, tkBufferCap capStyle = tkBufferCap.bcROUND, tkBufferJoin joinStyle = tkBufferJoin.bjROUND, double mitreLimit = 5)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Moves shape at specified offset.
        /// </summary>
        /// <param name="xOffset">X component of offset.</param>
        /// <param name="yOffset">Y component of offset.</param>
        /// \new493 Added in version 4.9.3
        public void Move(double xOffset, double yOffset)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Rotates shape around specified point.
        /// </summary>
        /// <param name="originX">X coordinate to rotate shape around.</param>
        /// <param name="originY">Y coordinate to rotate shape around.</param>
        /// <param name="angle">Angle in degrees.</param>
        /// \new493 Added in version 4.9.3
        public void Rotate(double originX, double originY, double angle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Splits shape with a provided polyline.
        /// </summary>
        /// <param name="polyline">Polyline shape to split with.</param>
        /// <param name="Results">An array of Shape type.</param>
        /// <returns>True on success.</returns>
        /// \new493 Added in version 4.9.3
        public bool SplitByPolyline(Shape polyline, ref object Results)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes all the points and parts.
        /// </summary>
        /// \new494 Added in version 4.9.4
        public void Clear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets "flattened" shape type, i.e. Z and M components will be ignored.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public ShpfileType ShapeType2D { get; private set; }

        /// <summary>
        /// Returns true if the shape doesn't have any points.
        /// </summary>
        /// \new493 Added in version 4.9.3
        public bool IsEmpty
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Return a point at the specified distance (or percentage) along the specified line
        /// </summary>
        /// <param name="startPoint">Starting point along this line (specify Point[0] for beginning of line)</param>
        /// <param name="distance">Distance along this line (or percentage of line length; if a percentage, specify a number between 0.0 and 1.0)</param>
        /// <param name="normalized">Optional value; if FALSE, 'distance' is actual distance; if TRUE, distance is percentage of length; defaults to FALSE</param>
        /// <returns>Returns a Point class representing the point along the sourceLine that is the specified distance (or percentage) along the line.</returns>
        /// <remarks>
        /// Only applies to a Polyline Shape.  If 'distance' is greater than the the line length, the line's endpoint is returned.
        /// 
        /// 'startPoint' does not have to be on the line.  If not on the line, actual starting point will be the nearest point to 'startPoint' that is on the line.
        /// </remarks>
        /// \see Utils.LineInterpolatePoint
        /// 
        /// \new500 Added in version 5.0.0
        public Point InterpolatePoint(Point startPoint, double distance, bool normalized)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Return a distance along the current line to a Point nearest another Shape
        /// </summary>
        /// <param name="referenceShape">Another Shape of any Shapetype</param>
        /// <returns>Returns the distance along this line to the point nearest the specified Shape.</returns>
        /// <remarks>
        /// Only applies to a Polyline Shape.  Returned Distance is measured from Point[0].  If resulting 'distance' would be greater than the length of the line, the line's length is returned.
        /// </remarks>
        /// \see Utils.LineProjectDistanceTo
        /// 
        /// \new500 Added in version 5.0.0
        public Double ProjectDistanceTo(Shape referenceShape)
        {
            throw new NotImplementedException();
        }

    }
#if nsp
}
#endif
