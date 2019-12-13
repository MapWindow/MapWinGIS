#include "stdafx.h"
#include "SelectionHelper.h"
#include "Shapefile.h"
#include "ShapeHelper.h"
#include "GeosConverter.h"
#include "GeosHelper.h"
#include "ShapefileHelper.h"

// *****************************************************
//		PolylineIntersection
// *****************************************************
bool SelectionHelper::PolylineIntersection(std::vector<double>& xPts, std::vector<double>& yPts, std::vector<long>& parts,
    double& b_minX, double& b_maxX, double& b_minY, double& b_maxY, double& Tolerance)
{
    double cy = (b_minY + b_maxY) * .5;
    double cx = (b_minX + b_maxX) * .5;

    int beg_part = 0;
    int end_part = 0;

    size_t numpoints = xPts.size();
    long numparts = parts.size();

    for (long i = 0; i < numparts; i++)
    {
        beg_part = parts[i];
        if (beg_part < 0) beg_part = 0;
        end_part = (int)(parts.size() - 1) > i ? parts[i + 1] : numpoints;

        //for(size_t j = 0; j < numpoints - 1; j++)
        for (long j = beg_part; j < end_part - 1; j++)
        {
            double p1x = xPts[j];
            double p1y = yPts[j];
            double p2x = xPts[j + 1];
            double p2y = yPts[j + 1];

            //Test for inclusion p1
            if (p1y <= b_maxY && p1y >= b_minY && p1x <= b_maxX && p1x >= b_minX)
            {
                return true;
            }

            // test the Y line
            if ((p1y > b_maxY && p2y > b_maxY) || (p1y < b_minY && p2y < b_minY))
            {
                continue;
            }

            // test the X line
            if ((p1x > b_maxX && p2x > b_maxX) || (p1x < b_minX && p2x < b_minX))
            {
                continue;
            }

            double dx = p2x - p1x;
            double dy = p2y - p1y;

            // check for vertical lines
            if (fabs(dy) <= Tolerance)
            {
                return true;
            }
            // check for horizontal lines
            if (fabs(dx) <= Tolerance)
            {
                return true;
            }

            // generate the equation of the line							
            double m = dy / dx;
            double b = p1y - m*p1x;

            double pm = -1 * (dx / dy);
            double pb = cy - pm*cx;
            double mx = (pb - b) / (m - pm);
            double my = m*mx + b;

            // test for inclusion mx/my
            if (my <= b_maxY &&
                my >= b_minY &&
                mx <= b_maxX &&
                mx >= b_minX)
            {
                return true;
            }
        }
    }
    return false;
}

// *****************************************************
//		PolygonIntersection
// *****************************************************
bool SelectionHelper::PolygonIntersection(std::vector<double>& xPts, std::vector<double>& yPts, std::vector<long>& parts,
    double& b_minX, double& b_maxX, double& b_minY, double& b_maxY, double& Tolerance)
{
    double cy = (b_minY + b_maxY) * .5;
    double cx = (b_minX + b_maxX) * .5;

    int beg_part = 0;
    int end_part = 0;

    bool selected = false;
    long numparts = parts.size();
    size_t numpoints = xPts.size();

    for (long j = 0; j < numparts && !selected; j++)
    {
        beg_part = parts[j];
        if (beg_part < 0)
            beg_part = 0;

        if ((int)(parts.size() - 1) > j)
            end_part = parts[j + 1];
        else
            end_part = numpoints;

        for (long k = beg_part; k < end_part - 1; k++)
        {
            double p1x = xPts[k];
            double p1y = yPts[k];
            double p2x = xPts[k + 1];
            double p2y = yPts[k + 1];

            // test for inclusion p1
            if (p1y <= b_maxY &&
                p1y >= b_minY &&
                p1x <= b_maxX &&
                p1x >= b_minX)
            {
                return true;
            }

            // test the Y line
            if ((p1y > b_maxY && p2y > b_maxY) ||
                (p1y < b_minY && p2y < b_minY))
            {
                continue;
            }

            // test the X line
            if ((p1x > b_maxX && p2x > b_maxX) ||
                (p1x < b_minX && p2x < b_minX))
            {
                continue;
            }

            double dx = p2x - p1x;
            double dy = p2y - p1y;

            // check for vertical lines
            if (fabs(dy) <= Tolerance)
            {
                return true;
            }

            // check for horizontal lines
            if (fabs(dx) <= Tolerance)
            {
                return true;
            }

            // generate the equation of the line							
            double m = dy / dx;
            double b = p1y - m*p1x;

            double pm = -1 * (dx / dy);
            double pb = cy - pm*cx;
            double mx = (pb - b) / (m - pm);
            double my = m*mx + b;

            // test for inclusion mx/my
            if (my <= b_maxY &&
                my >= b_minY &&
                mx <= b_maxX &&
                mx >= b_minX)
            {
                return true;
            }
        }
    }
    return false;
}

/***********************************************************************/
/*		SelectWithShapeBounds()
/***********************************************************************/
bool SelectionHelper::SelectWithShapeBounds(IShapefile* sf, IShape* shp, vector<long>& indices)
{
    if (!sf || !shp) return false;
    CComPtr<IExtents> box = NULL;
    shp->get_Extents(&box);
    return SelectShapes(sf, Extent(box), SelectMode::INTERSECTION, indices);
}

/***********************************************************************/
/*		SelectShapes()
/***********************************************************************/
bool SelectionHelper::SelectShapes(IShapefile* sf, Extent& extents, SelectMode SelectMode, std::vector<long>& selectResult)
{
    return ((CShapefile*)sf)->SelectShapesCore((extents), 0.0, SelectMode, selectResult, true);
}

/***********************************************************************/
/*		SelectSingleShape()
/***********************************************************************/
bool SelectionHelper::SelectSingleShape(IShapefile* sf, Extent& box, long& shapeIndex)
{
    return SelectSingleShape(sf, box, SelectMode::INTERSECTION, shapeIndex);
}
bool SelectionHelper::SelectSingleShape(IShapefile* sf, Extent& box, SelectMode mode, long& shapeIndex)
{
    vector<long> results;
    if (SelectShapes(sf, box, mode, results))
    {
        for (int i = results.size() - 1; i >= 0; i--)
        {
            VARIANT_BOOL visible;
            sf->get_ShapeRendered(results[i], &visible);
            if (visible)
            {
                shapeIndex = results[i];
                return true;
            }
        }
    }
    return false;
}

/***********************************************************************/
/*		SelectByPoint()
/***********************************************************************/
bool SelectionHelper::SelectByPoint(IShapefile* sf, Extent& box, bool clearPrevious, bool singleShape)
{
    if (!sf) return false;

    bool result = false;

    if (singleShape)
    {
        // default behavior for single-layer selection (via ChooseLayer event)
        long numSelected = ShapefileHelper::GetNumSelected(sf);

        if (clearPrevious) {
            sf->SelectNone();
            if (numSelected > 0)
                result = true;
        }

        long shapeIndex;
        // only a single (topmost) shape is selected
        if (SelectSingleShape(sf, box, shapeIndex))
        {
            // change the state of the top most shape
            bool selected = ShapefileHelper::ShapeSelected(sf, shapeIndex);
            sf->put_ShapeSelected(shapeIndex, (!selected) ? VARIANT_TRUE : VARIANT_FALSE);
            result = true;
        }
    }
    else
    {
        // default behavior for multi-layer selection (via 'Selectable' layers)
        // allow for multiple overlapping shapes in each layer to be selected
        result = SelectByRectangle(sf, box, clearPrevious);
    }
    return result;
}

/***********************************************************************/
/*		SelectByRectangle()
/***********************************************************************/
bool SelectionHelper::SelectByRectangle(IShapefile* sf, Extent& box, bool clearPrevious)
{
    if (!sf) return false;

    if (clearPrevious)
        sf->SelectNone();

    vector<long> results;
    if (SelectShapes(sf, box, SelectMode::INTERSECTION, results))
    {
        for (size_t i = 0; i < results.size(); i++)
        {
            // by default, toggle selection to TRUE
            bool selected = false;
            // if not clearing previous (ctrl-key is pressed), allow
            // toggle of current selection rather than just 'select'
            if (!clearPrevious)
            {
                // change the state of the top most shape
                selected = ShapefileHelper::ShapeSelected(sf, results[i]);
            }
            sf->put_ShapeSelected(results[i], (!selected) ? VARIANT_TRUE : VARIANT_FALSE);
        }
        return true;
    }
    return false;
}

/***********************************************************************/
/*		SelectByPolygon()
/***********************************************************************/
int SelectionHelper::SelectByPolygon(IShapefile* sf, IShape* poly, int& errorCode)
{
    errorCode = tkNO_ERROR;

    if (!sf || !poly) {
        errorCode = tkUNEXPECTED_NULL_PARAMETER;
        return 0;
    }

    if (ShapeHelper::GetShapeType2D(poly) != SHP_POLYGON) {
        errorCode = tkUNEXPECTED_SHAPE_TYPE;
        return 0;
    }

    vector<long> indices;
    if (!SelectWithShapeBounds(sf, poly, indices))
        return 0;

    GEOSGeometry* g = GeosConverter::ShapeToGeom(poly);
    if (!g) {
        errorCode = tkCANT_CONVERT_SHAPE_GEOS;
        return 0;
    }

    sf->SelectNone();   // TODO: make a property to control it

    for (size_t i = 0; i < indices.size(); i++)
    {
        CComPtr<IShape> shp = NULL;
        sf->get_Shape(indices[i], &shp);
        if (shp)
        {
            GEOSGeometry* g2 = GeosConverter::ShapeToGeom(shp);
            if (g2)
            {
                if (GeosHelper::Intersects(g, g2)) {
                    sf->put_ShapeSelected(indices[i], VARIANT_TRUE);
                }
                GeosHelper::DestroyGeometry(g2);
            }
        }
    }

    GeosHelper::DestroyGeometry(g);
    return ShapefileHelper::GetNumSelected(sf);
}

