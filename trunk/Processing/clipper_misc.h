/*******************************************************************************
*                                                                              *
* Author    :  Angus Johnson                                                   *
* Version   :  3.1.0                                                           *
* Date      :  17 February 2011                                                *
* Website   :  http://www.angusj.com                                           *
* Copyright :  Angus Johnson 2010-2011                                         *
*                                                                              *
* License:                                                                     *
* Use, modification & distribution is subject to Boost Software License Ver 1. *
* http://www.boost.org/LICENSE_1_0.txt                                         *
*                                                                              *
*******************************************************************************/

#ifndef clipper_misc_hpp
#define clipper_misc_hpp

#include "clipper.h"

namespace clipper {

//Area() ...
//The return value will be invalid if the supplied polygon is self-intersecting.
double Area(const TPolygon &pts);

//OffsetPolygons() ...
//A positive delta will offset each polygon edge towards its left, so polygons
//orientated clockwise (ie outer polygons) will expand but inner polyons (holes)
//will shrink. Conversely, negative deltas will offset polygon edges towards
//their right so outer polygons will shrink and inner polygons will expand.
ClipperLib::Polygons OffsetPolygons(const ClipperLib::Polygons &pts, const double &delta);

} //clipper namespace
#endif //clipper_hpp

