#include "stdafx.h"
#include "fHeader.h"
//# include <fstream>
//# include <stdio.h>
//# include <io.h>

using namespace std;

//CONSTRUCTORS
fHeader::fHeader()
{	ncols = 0;
	nrows = 0;
	dx = 1.0;
	dy = 1.0;	
	nodataValue = -1;
	xllcenter = 0;
	yllcenter = 0;	
}

fHeader::fHeader( const fHeader & h )
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
fHeader::~fHeader()
{

}

//OPERATORS
fHeader fHeader::operator=( const fHeader & h )
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
inline long fHeader::getNumberCols()
{	return ncols;
}

inline long fHeader::getNumberRows()
{	return nrows;
}

inline float fHeader::getNodataValue()
{	return nodataValue;
}

inline double fHeader::getDx()
{	return dx;
}

inline double fHeader::getDy()
{	return dy;
}

inline double fHeader::getXllcenter()
{	return xllcenter;
}

inline double fHeader::getYllcenter()
{	return yllcenter;
}

char * fHeader::getProjection()
{	
	if( projection.GetLength() > 0 )
	{	
		return _strdup(projection);
	}
	else
		return _strdup("");
}

char * fHeader::getNotes()
{	
	if( notes.GetLength() > 0 )
	{	
		return _strdup(notes);
	}
	else
		return _strdup("");
}

void fHeader::setNumberCols( long p_ncols )
{	ncols = p_ncols;
}

void fHeader::setNumberRows( long p_nrows )
{	nrows = p_nrows;
}

void fHeader::setNodataValue( float p_nodata_value )
{	nodataValue = p_nodata_value;
}

void fHeader::setDx( double p_dx )
{	if( p_dx > 0 )
		dx = p_dx;
}

void fHeader::setDy( double p_dy )
{	if( p_dy > 0 )
		dy = p_dy;
}

void fHeader::setXllcenter( double p_xllcenter )
{	xllcenter = p_xllcenter;
}

void fHeader::setYllcenter( double p_yllcenter )
{	yllcenter = p_yllcenter;
}

void fHeader::setProjection( const char * c_projection )
{	CString p_projection = c_projection;
	projection = p_projection.Left( MAX_STRING_LENGTH );
}

void fHeader::setNotes( const char * c_notes )
{	CString p_notes = c_notes;
	notes = p_notes.Left( MAX_STRING_LENGTH );	
}

