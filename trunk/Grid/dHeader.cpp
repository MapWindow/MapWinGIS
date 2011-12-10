#include "stdafx.h"
#include "dHeader.h"
# include <fstream>
# include <stdio.h>
# include <io.h>

using namespace std;

//CONSTRUCTORS
dHeader::dHeader()
{	ncols = 0;
	nrows = 0;
	dx = 1.0;
	dy = 1.0;	
	nodataValue = -1;
	xllcenter = 0;
	yllcenter = 0;	
}

dHeader::dHeader( const dHeader & h )
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
dHeader::~dHeader()
{

}

//OPERATORS
dHeader dHeader::operator=( const dHeader & h )
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
inline long dHeader::getNumberCols()
{	return ncols;
}

inline long dHeader::getNumberRows()
{	return nrows;
}

inline double dHeader::getNodataValue()
{	return nodataValue;
}

inline double dHeader::getDx()
{	return dx;
}

inline double dHeader::getDy()
{	return dy;
}

inline double dHeader::getXllcenter()
{	return xllcenter;
}

inline double dHeader::getYllcenter()
{	return yllcenter;
}

char * dHeader::getProjection()
{	
	if( projection.GetLength() > 0 )
	{	
		return _strdup(projection);
	}
	else
		return _strdup("");
}

char * dHeader::getNotes()
{	
	if( notes.GetLength() > 0 )
	{	
		return _strdup(notes);
	}
	else
		return _strdup("");
}

void dHeader::setNumberCols( long p_ncols )
{	ncols = p_ncols;
}

void dHeader::setNumberRows( long p_nrows )
{	nrows = p_nrows;
}

void dHeader::setNodataValue( double p_nodata_value )
{	nodataValue = p_nodata_value;
}

void dHeader::setDx( double p_dx )
{	if( p_dx > 0 )
		dx = p_dx;
}

void dHeader::setDy( double p_dy )
{	if( p_dy > 0 )
		dy = p_dy;
}

void dHeader::setXllcenter( double p_xllcenter )
{	xllcenter = p_xllcenter;
}

void dHeader::setYllcenter( double p_yllcenter )
{	yllcenter = p_yllcenter;
}

void dHeader::setProjection( const char * c_projection )
{	CString p_projection = c_projection;
	projection = p_projection.Left( MAX_STRING_LENGTH );
}

void dHeader::setNotes( const char * c_notes )
{	CString p_notes = c_notes;
	notes = p_notes.Left( MAX_STRING_LENGTH );	
}

