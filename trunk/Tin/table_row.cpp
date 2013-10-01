# include "stdafx.h"
# include "table_row.h"

tinTableRow::tinTableRow()
{	vertexOne = Undefined;
	vertexTwo = Undefined;
	vertexThree = Undefined;
	borderOne = Undefined;
	borderTwo = Undefined;
	borderThree = Undefined;
	maxDev = Undefined;	
}

tinTableRow::~tinTableRow()
{
}

tinTableRow::tinTableRow( long v1, long v2, long v3, long b1, long b2, long b3 )
{	vertexOne = v1;
	vertexTwo = v2;
	vertexThree = v3;
	borderOne = b1;
	borderTwo = b2;
	borderThree = b3;	
	maxDev = 0;
}

tinTableRow::tinTableRow( long v1, long v2, long v3, long b1, long b2, long b3, double dev, vertex dV )
{	vertexOne = v1;
	vertexTwo = v2;
	vertexThree = v3;
	borderOne = b1;
	borderTwo = b2;
	borderThree = b3;	
	maxDev = dev;
	devVertex = dV;
}

tinTableRow::tinTableRow( const tinTableRow & row )
{	vertexOne = row.vertexOne;
	vertexTwo = row.vertexTwo;
	vertexThree = row.vertexThree;
	borderOne = row.borderOne;
	borderTwo = row.borderTwo;
	borderThree = row.borderThree;
	maxDev = row.maxDev;
	devVertex = row.devVertex;
}

tinTableRow tinTableRow::operator=( const tinTableRow & row )
{	vertexOne = row.vertexOne;
	vertexTwo = row.vertexTwo;
	vertexThree = row.vertexThree;
	borderOne = row.borderOne;
	borderTwo = row.borderTwo;
	borderThree = row.borderThree;
	maxDev = row.maxDev;
	devVertex = row.devVertex;
	return *this;
}