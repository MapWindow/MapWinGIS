#include "stdafx.h"
#include "GridInterpolate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//contructor and destructor
GridInterpolate::GridInterpolate(IGrid * g, double nd, int nr, int nc){
	Grid = g;
	nodatavalue = nd;
	numrows = nr;
	numcols = nc;
	visited = new int[numrows*numcols];

	for(int i = 0; i < numrows*numcols; i++){
		visited[i] = 0;
	}//for
}//constructor

GridInterpolate::~GridInterpolate(){
	delete [] visited;
}//destructor

void GridInterpolate::Interpolate(int startrow, int startcol){
	int row = startrow, col = startcol, nextrow = 0, nextcol = 0, current_index = 0, temp = 0,
		corner_count = 0, side_count = 0, search_direction = 0, i = 0;
	VARIANT nodata_variant, value_variant;
	VariantInit(&nodata_variant); 
	VariantInit(&value_variant); 
	double value, weighted_sum = 0, distance_weighted_average = 0, sqrt_2 = sqrt(2.0);
	int neighbor_index[8];
	struct neighbor_cell neighbors[8];
	bool end_cell = true;

	if(startrow < numrows/2){
		if(startcol < numcols/2){
			search_direction = 7;
		}else{
			search_direction = 5;
		}//else
	}else{
		if(startcol < numcols/2){
			search_direction = 1;
		}else{
			search_direction = 3;
		}//else
	}//else

	Grid->get_Value(col+1,row+1,&nodata_variant);
	dVal(nodata_variant,value);
	while(value == nodatavalue){
		if(search_direction == 0){
			col++;
			Grid->get_Value(col+1,row,&nodata_variant);
		}else if(search_direction == 1){
			col++;
			row--;
			Grid->get_Value(col+1,row-1,&nodata_variant);
		}else if(search_direction == 2){
			row--;
			Grid->get_Value(col,row-1,&nodata_variant);
		}else if(search_direction == 3){
			col--;
			row--;
			Grid->get_Value(col-1,row-1,&nodata_variant);
		}else if(search_direction == 4){
			col--;
			Grid->get_Value(col-1,row,&nodata_variant);
		}else if(search_direction == 5){
			col--;
			row++;
			Grid->get_Value(col-1,row+1,&nodata_variant);
		}else if(search_direction == 6){
			row++;
			Grid->get_Value(col,row+1,&nodata_variant);
		}else{
			col++;
			row++;
			Grid->get_Value(col+1,row+1,&nodata_variant);
		}//else

		if(row < 0 || row >= numrows){
			return;
		}//if
		if(col < 0 || col >= numcols){
			return;
		}//if

		dVal(nodata_variant,value);
	}//while

	//push row col on queue
	q.push(col + row*numcols);
	s.push(col + row*numcols);
	visited[col + row*numcols] = 1;
	while(!q.empty()){
		end_cell = true;
		//pop row col off queue
		current_index = q.front();
		q.pop();

		for(i = 0; i < 8; i++){
			neighbors[i].row = 0;
			neighbors[i].col = 0;
			neighbors[i].count = 0;
			neighbors[i].valid_cell = 0;
			neighbors[i].valid_data = 0;
		}//for

		row = (int)(current_index / numcols);
		col = current_index % numcols;
		GetNeighbors(row,col,neighbors);

		for(i = 0; i < 8; i++){
			if(neighbors[i].valid_data == 1 || neighbors[i].valid_cell == 0){
				neighbors[i].count = -1;
			}else{
				neighbors[i].count = GetNeighboringValidDataCount(neighbors[i].row,neighbors[i].col);
			}//else
			neighbor_index[i] = i;
			for(int j = 0; j < i; j++){
				//Sort the neighbor indexes by each neighbor's valid data neighbor count in descending order
				if(neighbors[neighbor_index[j]].count < neighbors[neighbor_index[i]].count){
					temp = neighbor_index[j];
					neighbor_index[j] = neighbor_index[i];
					neighbor_index[i] = temp;
				}//if
			}//for
		}//for

		for(i = 0; i < 8; i++){
			nextrow = neighbors[neighbor_index[i]].row;
			nextcol = neighbors[neighbor_index[i]].col;
			if(neighbors[neighbor_index[i]].valid_cell == 1 && neighbors[neighbor_index[i]].valid_data != 1){
				if(visited[nextcol + nextrow*numcols] == 0){
					//add to queue...
					q.push(nextcol + nextrow*numcols);
					s.push(nextcol + nextrow*numcols);
					visited[nextcol + nextrow*numcols] = 1;
					end_cell = false;
				}//if
			}//if
		}//for

		if(end_cell){
			corner_count = 0;
			side_count = 0;
			weighted_sum = 0;
			distance_weighted_average = 0;

			for(i = 0; i < 8; i++){
				Grid->get_Value(neighbors[i].col,neighbors[i].row,&value_variant);
				dVal(value_variant, value);
				if(nodatavalue != value){
					if(i % 2 == 0){
						weighted_sum += value;
						side_count++;
					}else{
						weighted_sum += value / sqrt_2;
						corner_count++;
					}//else
				}//if
			}//for

			if( corner_count > 0 && side_count > 0){
				distance_weighted_average = weighted_sum / ((corner_count/sqrt_2)+side_count);

				value_variant.dblVal = distance_weighted_average;
			}else{
				value_variant.dblVal = nodatavalue;
			}//else

			Grid->put_Value(col,row,value_variant);
		}//if
	}//while

	while(!s.empty()){
		corner_count = 0;
		side_count = 0;
		weighted_sum = 0;
		distance_weighted_average = 0;

		current_index = s.top();
		s.pop();
		row = (int)(current_index / numcols);
		col = current_index % numcols;
		Grid->get_Value(col,row,&value_variant);
		dVal(value_variant, value);
		if(value == nodatavalue){
			GetNeighbors(row,col,neighbors);
			for(i = 0; i < 8; i++){
				Grid->get_Value(neighbors[i].col,neighbors[i].row,&value_variant);
				dVal(value_variant, value);
				if(nodatavalue != value){
					if(i % 2 == 0){
						weighted_sum += value;
						side_count++;
					}else{
						weighted_sum += value / sqrt_2;
						corner_count++;
					}//else
				}//if
			}//for

			if( corner_count > 0 && side_count > 0){
				distance_weighted_average = weighted_sum / ((corner_count/sqrt_2)+side_count);

				value_variant.dblVal = distance_weighted_average;
			}else{
				value_variant.dblVal = nodatavalue;
			}//else

			Grid->put_Value(col,row,value_variant);
		}//if
	}//while
	VariantClear(&value_variant); 
	VariantClear(&nodata_variant); 
}//Interpolate

int GridInterpolate::GetPossiblePathCount(int row, int col){
	int count = 0;
	return count;
}//GetPossiblePathCount

int GridInterpolate::GetNeighboringNDCount(int row, int col){
	int count = 0;
	VARIANT nodata_variant;
	VariantInit(&nodata_variant); 
	double current_nodatavalue;

	//Check the neighboring cells to the right of the current cell
	if(col+1 < numcols){
		Grid->get_Value(col+1,row,&nodata_variant);
		dVal(nodata_variant, current_nodatavalue);
		if(nodatavalue == current_nodatavalue){
			count++;
		}//if
		if(row-1 >= 0){
			Grid->get_Value(col+1,row-1,&nodata_variant);
			dVal(nodata_variant, current_nodatavalue);
			if(nodatavalue == current_nodatavalue){
				count++;
			}//if
		}//if
		if(row+1 < numrows){
			Grid->get_Value(col+1,row+1,&nodata_variant);
			dVal(nodata_variant, current_nodatavalue);
			if(nodatavalue == current_nodatavalue){
				count++;
			}//if
		}//if
	}//if

	//Check the neighboring cell directly above the current cell
	if(row-1 >= 0){
		Grid->get_Value(col,row-1,&nodata_variant);
		dVal(nodata_variant, current_nodatavalue);
		if(nodatavalue == current_nodatavalue){
			count++;
		}//if
	}//if

	//Check the neighboring cells to the left of the current cell
	if(col-1 >= 0){
		Grid->get_Value(col-1,row,&nodata_variant);
		dVal(nodata_variant, current_nodatavalue);
		if(nodatavalue == current_nodatavalue){
			count++;
		}//if
		if(row-1 >= 0){
			Grid->get_Value(col-1,row-1,&nodata_variant);
			dVal(nodata_variant, current_nodatavalue);
			if(nodatavalue == current_nodatavalue){
				count++;
			}//if
		}//if
		if(row+1 < numrows){
			Grid->get_Value(col-1,row+1,&nodata_variant);
			dVal(nodata_variant, current_nodatavalue);
			if(nodatavalue == current_nodatavalue){
				count++;
			}//if
		}//if
	}//if

	//Check the neighboring cell directly below the current cell
	if(row+1 < numrows){
		Grid->get_Value(col,row+1,&nodata_variant);
		dVal(nodata_variant, current_nodatavalue);
		if(nodatavalue == current_nodatavalue){
			count++;
		}//if
	}//if
	VariantClear(&nodata_variant); 
	return count;
}//GetNeighboringNDCount

int GridInterpolate::GetNeighboringValidDataCount(int row, int col){
	int count = 0;
	VARIANT nodata_variant;
	VariantInit(&nodata_variant); 
	double current_nodatavalue;

	//Check the neighboring cells to the right of the current cell
	if(col+1 < numcols){
		Grid->get_Value(col+1,row,&nodata_variant);
		dVal(nodata_variant, current_nodatavalue);
		if(nodatavalue != current_nodatavalue){
			count++;
		}//if
		if(row-1 >= 0){
			Grid->get_Value(col+1,row-1,&nodata_variant);
			dVal(nodata_variant, current_nodatavalue);
			if(nodatavalue != current_nodatavalue){
				count++;
			}//if
		}//if
		if(row+1 < numrows){
			Grid->get_Value(col+1,row+1,&nodata_variant);
			dVal(nodata_variant, current_nodatavalue);
			if(nodatavalue != current_nodatavalue){
				count++;
			}//if
		}//if
	}//if

	//Check the neighboring cell directly above the current cell
	if(row-1 >= 0){
		Grid->get_Value(col,row-1,&nodata_variant);
		dVal(nodata_variant, current_nodatavalue);
		if(nodatavalue != current_nodatavalue){
			count++;
		}//if
	}//if

	//Check the neighboring cells to the left of the current cell
	if(col-1 >= 0){
		Grid->get_Value(col-1,row,&nodata_variant);
		dVal(nodata_variant, current_nodatavalue);
		if(nodatavalue != current_nodatavalue){
			count++;
		}//if
		if(row-1 >= 0){
			Grid->get_Value(col-1,row-1,&nodata_variant);
			dVal(nodata_variant, current_nodatavalue);
			if(nodatavalue != current_nodatavalue){
				count++;
			}//if
		}//if
		if(row+1 < numrows){
			Grid->get_Value(col-1,row+1,&nodata_variant);
			dVal(nodata_variant, current_nodatavalue);
			if(nodatavalue != current_nodatavalue){
				count++;
			}//if
		}//if
	}//if

	//Check the neighboring cell directly below the current cell
	if(row+1 < numrows){
		Grid->get_Value(col,row+1,&nodata_variant);
		dVal(nodata_variant, current_nodatavalue);
		if(nodatavalue != current_nodatavalue){
			count++;
		}//if
	}//if
	VariantClear(&nodata_variant); 
	return count;
}//GetNeighboringValidDataCount

void GridInterpolate::GetNeighbors(int row, int col, struct neighbor_cell * neighbors){
	VARIANT value_variant;
	VariantInit(&value_variant); 
	double current_value;
	//Add neighboring cells to the right of the starting cell
	if(col+1 < numcols){
		neighbors[0].row = row;
		neighbors[0].col = col+1;
		neighbors[0].valid_cell = 1;
		if(row-1 >= 0){
			neighbors[1].row = row-1;
			neighbors[1].col = col+1;
			neighbors[1].valid_cell = 1;
		}//if
		if(row+1 < numrows){
			neighbors[7].row = row+1;
			neighbors[7].col = col+1;
			neighbors[7].valid_cell = 1;
		}//if
	}//if

	//Add neighboring cell directly above the starting cell
	if(row-1 >= 0){
		neighbors[2].row = row-1;
		neighbors[2].col = col;
		neighbors[2].valid_cell = 1;
	}//if

	//Add neighboring cells to the left of the starting cell
	if(col-1 >= 0){
		neighbors[4].row = row;
		neighbors[4].col = col-1;
		neighbors[4].valid_cell = 1;
		if(row-1 >= 0){
			neighbors[3].row = row-1;
			neighbors[3].col = col-1;
			neighbors[3].valid_cell = 1;
		}//if
		if(row+1 < numrows){
			neighbors[5].row = row+1;
			neighbors[5].col = col-1;
			neighbors[5].valid_cell = 1;
		}//if
	}//if

	//Add neighboring cell directly below the starting cell
	if(row+1 < numrows){
		neighbors[6].row = row+1;
		neighbors[6].col = col;
		neighbors[6].valid_cell = 1;
	}//if

	for(int i = 0; i < 8; i++){
		if(neighbors[i].valid_cell == 1){
			Grid->get_Value(neighbors[i].col,neighbors[i].row,&value_variant);
			dVal(value_variant, current_value);
			if(nodatavalue != current_value){
				neighbors[i].valid_data = 1;
			}//if
		}//if
	}//for

	VariantClear(&value_variant); 
}//GetNeighbors