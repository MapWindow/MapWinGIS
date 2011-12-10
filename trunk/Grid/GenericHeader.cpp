#include "stdafx.h"
#include "GenericHeader.h"
# include <fstream>
# include <stdio.h>
# include <io.h>

using namespace std;

/**********************************************************
			This class is the C++ Exported Grid Header class
			not the implementation of IGridHeader COM Class
**********************************************************/

//CONSTRUCTORS
header::header()
{	ncols = 0;
	nrows = 0;
	dx = 1.0;
	dy = 1.0;	
	nodataValue = -1;
	xllcenter = 0;
	yllcenter = 0;	
}

header::header( const header & h )
{	ncols = h.ncols;
	nrows = h.nrows;
	dx = h.dx;
	dy = h.dy;
	nodataValue = h.nodataValue;
	xllcenter = h.xllcenter;
	yllcenter = h.yllcenter;	
	projection = h.projection;
	notes = h.notes;	
}

//Destructor
header::~header()
{

}

//OPERATORS
header header::operator=( const header & h )
{	ncols = h.ncols;
	nrows = h.nrows;
	dx = h.dx;
	dy = h.dy;
	nodataValue = h.nodataValue;
	xllcenter = h.xllcenter;
	yllcenter = h.yllcenter;	
	projection = h.projection;
	notes = h.notes;	
	return *this;
}

//DATA ACCESS MEMBERS
inline long header::getNumberCols()
{	return ncols;
}

inline long header::getNumberRows()
{	return nrows;
}

inline double header::getNodataValue()
{	return nodataValue;
}

inline double header::getDx()
{	return dx;
}

inline double header::getDy()
{	return dy;
}

inline double header::getXllcenter()
{	return xllcenter;
}

inline double header::getYllcenter()
{	return yllcenter;
}

char * header::getProjection()
{	if( projection.GetLength() > 0 )
	{	char * retval = new char[ projection.GetLength() + 1 ];
		strcpy( retval, projection );
		return retval;
	}
	else
		return "";
}

char * header::getNotes()
{	if( notes.GetLength() > 0 )
	{	char * retval = new char[ notes.GetLength() + 1 ];
		strcpy( retval, notes );
		return retval;
	}
	else
		return "";
}

void header::setNumberCols( long p_ncols )
{	ncols = p_ncols;
}

void header::setNumberRows( long p_nrows )
{	nrows = p_nrows;
}

void header::setNodataValue( double p_nodata_value )
{	nodataValue = p_nodata_value;
}

void header::setDx( double p_dx )
{	if( p_dx > 0 )
		dx = p_dx;
}

void header::setDy( double p_dy )
{	if( p_dy > 0 )
		dy = p_dy;
}

void header::setXllcenter( double p_xllcenter )
{	xllcenter = p_xllcenter;
}

void header::setYllcenter( double p_yllcenter )
{	yllcenter = p_yllcenter;
}

void header::setProjection( const char * c_projection )
{	CString p_projection = c_projection;
	projection = p_projection.Left( MAX_STRING_LENGTH );
}

void header::setNotes( const char * c_notes )
{	CString p_notes = c_notes;
	notes = p_notes.Left( MAX_STRING_LENGTH );	
}

