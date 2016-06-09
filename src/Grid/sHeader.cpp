#include "stdafx.h"
#include "sHeader.h"
//# include <fstream>
//# include <stdio.h>
//# include <io.h>

using namespace std;
//CONSTRUCTORS
sHeader::sHeader()
{	ncols = 0;
	nrows = 0;
	dx = 1.0;
	dy = 1.0;	
	nodataValue = -1;
	xllcenter = 0;
	yllcenter = 0;	
}

sHeader::sHeader( const sHeader & h )
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
sHeader::~sHeader()
{
}

//OPERATORS
sHeader sHeader::operator=( const sHeader & h )
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
inline long sHeader::getNumberCols()
{	return ncols;
}

inline long sHeader::getNumberRows()
{	return nrows;
}

inline short sHeader::getNodataValue()
{	return nodataValue;
}

inline double sHeader::getDx()
{	return dx;
}

inline double sHeader::getDy()
{	return dy;
}

inline double sHeader::getXllcenter()
{	return xllcenter;
}

inline double sHeader::getYllcenter()
{	return yllcenter;
}

char * sHeader::getProjection()
{	
	if( projection.GetLength() > 0 )
	{	
		return _strdup(projection);
	}
	else
		return _strdup("");
}

char * sHeader::getNotes()
{	
	if( notes.GetLength() > 0 )
	{	
		return _strdup(notes);
	}
	else
		return _strdup("");
}

void sHeader::setNumberCols( long p_ncols )
{	ncols = p_ncols;
}

void sHeader::setNumberRows( long p_nrows )
{	nrows = p_nrows;
}

void sHeader::setNodataValue( short p_nodata_value )
{	nodataValue = p_nodata_value;
}

void sHeader::setDx( double p_dx )
{	if( p_dx > 0 )
		dx = p_dx;
}

void sHeader::setDy( double p_dy )
{	if( p_dy > 0 )
		dy = p_dy;
}

void sHeader::setXllcenter( double p_xllcenter )
{	xllcenter = p_xllcenter;
}

void sHeader::setYllcenter( double p_yllcenter )
{	yllcenter = p_yllcenter;
}

void sHeader::setProjection( const char * c_projection )
{	CString p_projection = c_projection;
	projection = p_projection.Left( MAX_STRING_LENGTH );
}

void sHeader::setNotes( const char * c_notes )
{	CString p_notes = c_notes;
	notes = p_notes.Left( MAX_STRING_LENGTH );	
}

