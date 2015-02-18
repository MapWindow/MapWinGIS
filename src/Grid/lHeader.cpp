#include "stdafx.h"
#include "lHeader.h"
//# include <fstream>
//# include <stdio.h>
//# include <io.h>

using namespace std;
//CONSTRUCTORS
lHeader::lHeader()
{	ncols = 0;
	nrows = 0;
	dx = 1.0;
	dy = 1.0;	
	nodataValue = -1;
	xllcenter = 0;
	yllcenter = 0;	
}

lHeader::lHeader( const lHeader & h )
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

//DESTRUCTOR
lHeader::~lHeader()
{

}

//OPERATORS
lHeader lHeader::operator=( const lHeader & h )
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
inline long lHeader::getNumberCols()
{	return ncols;
}

inline long lHeader::getNumberRows()
{	return nrows;
}

inline long lHeader::getNodataValue()
{	return nodataValue;
}

inline double lHeader::getDx()
{	return dx;
}

inline double lHeader::getDy()
{	return dy;
}

inline double lHeader::getXllcenter()
{	return xllcenter;
}

inline double lHeader::getYllcenter()
{	return yllcenter;
}

char * lHeader::getProjection()
{	
	if( projection.GetLength() > 0 )
	{	
		return _strdup(projection);
	}
	else
		return _strdup("");
}

char * lHeader::getNotes()
{	
	if( notes.GetLength() > 0 )
	{	
		return _strdup(notes);
	}
	else
		return _strdup("");
}

void lHeader::setNumberCols( long p_ncols )
{	ncols = p_ncols;
}

void lHeader::setNumberRows( long p_nrows )
{	nrows = p_nrows;
}

void lHeader::setNodataValue( long p_nodata_value )
{	nodataValue = p_nodata_value;
}

void lHeader::setDx( double p_dx )
{	if( p_dx > 0 )
		dx = p_dx;
}

void lHeader::setDy( double p_dy )
{	if( p_dy > 0 )
		dy = p_dy;
}

void lHeader::setXllcenter( double p_xllcenter )
{	xllcenter = p_xllcenter;
}

void lHeader::setYllcenter( double p_yllcenter )
{	yllcenter = p_yllcenter;
}

void lHeader::setProjection( const char * c_projection )
{	CString p_projection = c_projection;
	projection = p_projection.Left( MAX_STRING_LENGTH );
}

void lHeader::setNotes( const char * c_notes )
{	CString p_notes = c_notes;
	notes = p_notes.Left( MAX_STRING_LENGTH );	
}

