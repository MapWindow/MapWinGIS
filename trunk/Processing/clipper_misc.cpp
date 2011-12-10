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
#include "stdafx.h"
#include <cmath>
#include "clipper_misc.h"

namespace clipper {

static double const pi = 3.14159265358979;

TPolygon BuildArc(const TDoublePoint &pt,
  const double a1, const double a2, const double r)
{
  int steps = max(6, int(std::sqrt(std::abs(r)) * std::abs(a2 - a1)));
  TPolygon result(steps);
  int n = steps - 1;
  double da = (a2 - a1) / n;
  double a = a1;
  for (int i = 0; i <= n; ++i)
  {
    result[i].X = pt.X + std::cos(a)*r;
    result[i].Y = pt.Y + std::sin(a)*r;
    a = a + da;
  }
  return result;
}
//------------------------------------------------------------------------------

double Area(const TPolygon &pts)
{
  int highI = (int)pts.size() -1;
  if (highI < 2) return 0;
  double area = pts[highI].X * pts[0].Y - pts[0].X * pts[highI].Y;
  for (int i = 0; i < highI; ++i)
    area += pts[i].X * pts[i+1].Y - pts[i+1].X * pts[i].Y;
  return area/2;
}
//------------------------------------------------------------------------------

TPolyPolygon OffsetPolygons(const TPolyPolygon &pts, const double &delta)
{
  double deltaSq = delta*delta;
  TPolyPolygon result(pts.size());

  for (int j = 0; j < (int)pts.size(); ++j)
  {
    int highI = (int)pts[j].size() -1;
    //to minimize artefacts, strip out those polygons where
    //it's shrinking and where its area < Sqr(delta) ...
    double a1 = Area(pts[j]);
    if (delta < 0) { if (a1 > 0 && a1 < deltaSq) highI = 0;}
    else if (a1 < 0 && -a1 < deltaSq) highI = 0; //nb: a hole if area < 0

    TPolygon pg;
    pg.reserve(highI*2+2);

    if (highI < 2)
    {
      result.push_back(pg);
      continue;
    }

    TPolygon normals(highI+1);
    normals[0] = GetUnitNormal(pts[j][highI], pts[j][0]);
    for (int i = 1; i <= highI; ++i)
      normals[i] = GetUnitNormal(pts[j][i-1], pts[j][i]);

    for (int i = 0; i < highI; ++i)
    {
      pg.push_back(DoublePoint(pts[j][i].X + delta *normals[i].X,
        pts[j][i].Y + delta *normals[i].Y));
      pg.push_back(DoublePoint(pts[j][i].X + delta *normals[i+1].X,
        pts[j][i].Y + delta *normals[i+1].Y));
    }
    pg.push_back(DoublePoint(pts[j][highI].X + delta *normals[highI].X,
      pts[j][highI].Y + delta *normals[highI].Y));
    pg.push_back(DoublePoint(pts[j][highI].X + delta *normals[0].X,
      pts[j][highI].Y + delta *normals[0].Y));

    //round off reflex angles (ie > 180 deg) unless it's almost flat (ie < 10deg angle) ...
    //cross product normals < 0 -> reflex angle; dot product normals == 1 -> no angle
    if ((normals[highI].X *normals[0].Y - normals[0].X *normals[highI].Y) *delta > 0 &&
    (normals[0].X *normals[highI].X + normals[0].Y *normals[highI].Y) < 0.985)
    {
      double a1 = std::atan2(normals[highI].Y, normals[highI].X);
      double a2 = std::atan2(normals[0].Y, normals[0].X);
      if (delta > 0 && a2 < a1) a2 = a2 + pi*2;
      else if (delta < 0 && a2 > a1) a2 = a2 - pi*2;
      TPolygon arc = BuildArc(pts[j][highI], a1, a2, delta);
      TPolygon::iterator it = pg.begin() +highI*2+1;
      pg.insert(it, arc.begin(), arc.end());
    }
    for (int i = highI; i > 0; --i)
      if ((normals[i-1].X*normals[i].Y - normals[i].X*normals[i-1].Y) *delta > 0 &&
      (normals[i].X*normals[i-1].X + normals[i].Y*normals[i-1].Y) < 0.985)
      {
        double a1 = std::atan2(normals[i-1].Y, normals[i-1].X);
        double a2 = std::atan2(normals[i].Y, normals[i].X);
        if (delta > 0 && a2 < a1) a2 = a2 + pi*2;
        else if (delta < 0 && a2 > a1) a2 = a2 - pi*2;
        TPolygon arc = BuildArc(pts[j][i-1], a1, a2, delta);
        TPolygon::iterator it = pg.begin() +(i-1)*2+1;
        pg.insert(it, arc.begin(), arc.end());
      }
    result.push_back(pg);
  }

  //finally, clean up untidy corners ...
  Clipper c;
  c.AddPolyPolygon(result, ptSubject);
  if (delta > 0){
    if(!c.Execute(ctUnion, result, pftNonZero, pftNonZero)) result.clear();
  }
  else
  {
    TDoubleRect r = c.GetBounds();
    TPolygon outer(4);
    outer[0] = DoublePoint(r.left-10, r.bottom+10);
    outer[1] = DoublePoint(r.right+10, r.bottom+10);
    outer[2] = DoublePoint(r.right+10, r.top-10);
    outer[3] = DoublePoint(r.left-10, r.top-10);
    c.AddPolygon(outer, ptSubject);
    if (c.Execute(ctUnion, result, pftNonZero, pftNonZero))
    {
      TPolyPolygon::iterator it = result.begin();
      result.erase(it);
    }
    else
      result.clear();
  }
  return result;
}
//------------------------------------------------------------------------------

}

