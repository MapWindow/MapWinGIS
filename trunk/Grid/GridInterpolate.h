#ifndef _GRIDINTERPOLATE_H_
#define _GRIDINTERPOLATE_H_

#include <queue>
#include <stack>

struct neighbor_cell{
	int row;
	int col;
	int count;
	int valid_cell;
	int valid_data;
};

/////////////////////////////////////////////////////////////////////
//
// This class interpolates cells in a grid with nodata values.
//
class GridInterpolate
{
public:
	GridInterpolate(IGrid * g, double nd, int nr, int nc);
	virtual ~GridInterpolate();
	void Interpolate(int startrow, int startcol);

private:
	IGrid * Grid;
	double nodatavalue;
	int numrows;
	int numcols;
	int * visited;
	std::queue<int> q;
	std::stack<int> s;

	int GetPossiblePathCount(int row, int col);
	int GetNeighboringNDCount(int row, int col);
	int GetNeighboringValidDataCount(int row, int col);
	void GetNeighbors(int row, int col, struct neighbor_cell * neighbors);
};

#endif // _GRIDINTERPOLATE_H_