#include "stdafx.h"
#include "SelectionHelper.h"

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
