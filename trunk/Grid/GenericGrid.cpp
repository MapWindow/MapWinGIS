#pragma once

# include "stdafx.h"
# include "GenericGrid.h"

/**********************************************************
			This class is the C++ Exported Grid class
			not the implementation of IGrid COM Class
**********************************************************/

//CONSTRUCTORS
grid::grid()
{	dgrid = NULL;
	fgrid = NULL;
	lgrid = NULL;
	sgrid = NULL;
	initialize_esri();

	lastErrorCode = tkNO_ERROR;
}
grid::~grid()
{	close();
	shutdown_esri();
}
		
//OPERATORS
double grid::operator()( int Column, int Row )
{	if( dgrid != NULL )
		return dgrid->getValue( Column, Row );
	else if( fgrid != NULL )
		return fgrid->getValue( Column, Row );
	else if( lgrid != NULL )
		return lgrid->getValue( Column, Row );
	else if( sgrid != NULL )
		return sgrid->getValue( Column, Row );
	else
		return getHeader().getNodataValue();
}
		
//FUNCTIONS
bool grid::open( const char * Filename, bool InRam, GRID_TYPE GridType, void (*callback)( int number, const char * message ) )
{	close();
	GridManager gm;
	DATA_TYPE DataType = gm.getGridDataType( Filename, GridType );
	if( DataType == DOUBLE_TYPE )
	{	dgrid = new dGrid();
		return dgrid->open( Filename, InRam, GridType, callback );
	}
	else if( DataType == FLOAT_TYPE )
	{	fgrid = new fGrid();
		return fgrid->open( Filename, InRam, GridType, callback );
	}
	else if( DataType == LONG_TYPE )
	{	lgrid = new lGrid();
		return lgrid->open( Filename, InRam, GridType, callback );
	}
	else if( DataType == SHORT_TYPE )
	{	sgrid = new sGrid();
		return sgrid->open( Filename, InRam, GridType, callback );
	}
	else
		return false;
}
bool grid::initialize( const char * Filename, header pheader, double initialValue, DATA_TYPE DataType, bool InRam, GRID_TYPE GridType )	
{	close();
	if( DataType == DOUBLE_TYPE )
	{	dgrid = new dGrid();
		dHeader dheader = dh_copyFromHeader( pheader );		
		return dgrid->initialize( Filename, dheader, initialValue, InRam, GridType );
	}
	else if( DataType == FLOAT_TYPE )
	{	fgrid = new fGrid();
		fHeader fheader = fh_copyFromHeader( pheader );		
		return fgrid->initialize( Filename, fheader, (float)initialValue, InRam, GridType );
	}
	else if( DataType == LONG_TYPE )
	{	lgrid = new lGrid();
		lHeader lheader = lh_copyFromHeader( pheader );		
		return lgrid->initialize( Filename, lheader, (long)initialValue, InRam, GridType );
	}
	else if( DataType == SHORT_TYPE )
	{	sgrid = new sGrid();
		sHeader sheader = sh_copyFromHeader( pheader );				
		return sgrid->initialize( Filename, sheader, (short)initialValue, InRam, GridType );
	}
	else
		return false;	
}
bool grid::save( const char * Filename, GRID_TYPE GridType, void (*callback)(int number, const char * message ) )
{	
	if( dgrid != NULL )
		return dgrid->save( Filename, GridType, callback );
	else if( fgrid != NULL )
		return fgrid->save( Filename, GridType, callback );
	else if( lgrid != NULL )
		return lgrid->save( Filename, GridType, callback );
	else if( sgrid != NULL )
		return sgrid->save( Filename, GridType, callback );
	else
		return false;
}
bool grid::close()
{	bool result = true;
	if( dgrid != NULL )
		result = dgrid->close();
	dgrid = NULL;
	if( fgrid != NULL )
		result = fgrid->close();
	fgrid = NULL;
	if( lgrid != NULL )
		result = lgrid->close();
	lgrid = NULL;
	if( sgrid != NULL )
		result = sgrid->close();
	sgrid = NULL;

	return true;	
}
void grid::clear(double clearValue)
{	
	if( dgrid != NULL )
		dgrid->clear(clearValue);
	else if( fgrid != NULL )
		fgrid->clear((float)clearValue);
	else if( lgrid != NULL )
		lgrid->clear((long)clearValue);
	else if( sgrid != NULL )
		sgrid->clear((short)clearValue);
}

//MAPPING FUNCTIONS
void grid::ProjToCell( double x, double y, long & column, long & row )
{	
	if( dgrid != NULL )
		dgrid->ProjToCell( x, y, column, row );
	else if( fgrid != NULL )
		fgrid->ProjToCell( x, y, column, row );
	else if( lgrid != NULL )
		lgrid->ProjToCell( x, y, column, row );
	else if( sgrid != NULL )
		sgrid->ProjToCell( x, y, column, row );
}
void grid::CellToProj( long column, long row, double & x, double & y )
{
	if( dgrid != NULL )
		dgrid->CellToProj( column, row, x, y );
	else if( fgrid != NULL )
		fgrid->CellToProj( column, row, x, y );
	else if( lgrid != NULL )
		lgrid->CellToProj( column, row, x, y );
	else if( sgrid != NULL )
		sgrid->CellToProj( column, row, x, y );	
}
		
//DATA MEMBER ACCESS
inline header grid::getHeader()
{	header h = copyToHeader();
	return h;
}

void grid::setHeader( header h )
{	
	if( dgrid != NULL )
	{	dHeader dh = dh_copyFromHeader( h );
		dgrid->setHeader( dh );
	}
	else if( fgrid != NULL )
	{	fHeader fh = fh_copyFromHeader( h );
		fgrid->setHeader( fh );
	}
	else if( lgrid != NULL )
	{	lHeader lh = lh_copyFromHeader( h );
		lgrid->setHeader( lh );
	}
	else if( sgrid != NULL )
	{	sHeader sh = sh_copyFromHeader( h );
		sgrid->setHeader( sh );
	}
}

inline double grid::getValue( int Column, int Row )
{	
	if( dgrid != NULL )
		return dgrid->getValue( Column, Row );
	else if( fgrid != NULL )
		return fgrid->getValue( Column, Row );
	else if( lgrid != NULL )
		return lgrid->getValue( Column, Row );
	else if( sgrid != NULL )
		return sgrid->getValue( Column, Row );
	else
		return 0.0;
}
inline void grid::setValue( int Column, int Row, double Value )
{
	if( dgrid != NULL )
		dgrid->setValue( Column, Row, Value );
	else if( fgrid != NULL )
		fgrid->setValue( Column, Row, (float)Value );
	else if( lgrid != NULL )
		lgrid->setValue( Column, Row, (long)Value );
	else if( sgrid != NULL )
		sgrid->setValue( Column, Row, (short)Value );
}
bool grid::inRam()
{
	if( dgrid != NULL )
		return dgrid->inRam();
	else if( fgrid != NULL )
		return fgrid->inRam();
	else if( lgrid != NULL )
		return lgrid->inRam();
	else if( sgrid != NULL )
		return sgrid->inRam();
	else
		return true;
}
double grid::maximum()
{	
	if( dgrid != NULL )
		return dgrid->maximum();
	else if( fgrid != NULL )
		return fgrid->maximum();
	else if( lgrid != NULL )
		return lgrid->maximum();
	else if( sgrid != NULL )
		return sgrid->maximum();
	else
		return getHeader().getNodataValue();
}
double grid::minimum()
{	
	if( dgrid != NULL )
		return dgrid->minimum();
	else if( fgrid != NULL )
		return fgrid->minimum();
	else if( lgrid != NULL )
		return lgrid->minimum();
	else if( sgrid != NULL )
		return sgrid->minimum();
	else
		return getHeader().getNodataValue();
}

header grid::copyToHeader()
{	header h;
	if( dgrid != NULL )
	{	dHeader dh = dgrid->getHeader();
		h.setDx(dh.getDx());
		h.setDy(dh.getDy());
		h.setNodataValue(dh.getNodataValue());
		h.setNotes(dh.getNotes());
		h.setNumberCols(dh.getNumberCols());
		h.setNumberRows(dh.getNumberRows());
		h.setProjection(dh.getProjection());
		h.setXllcenter(dh.getXllcenter());
		h.setYllcenter(dh.getYllcenter());
	}
	else if( fgrid != NULL )
	{	fHeader fh = fgrid->getHeader();
		h.setDx(fh.getDx());
		h.setDy(fh.getDy());
		h.setNodataValue(fh.getNodataValue());
		h.setNotes(fh.getNotes());
		h.setNumberCols(fh.getNumberCols());
		h.setNumberRows(fh.getNumberRows());
		h.setProjection(fh.getProjection());
		h.setXllcenter(fh.getXllcenter());
		h.setYllcenter(fh.getYllcenter());
	}
	else if( lgrid != NULL )
	{	lHeader lh = lgrid->getHeader();
		h.setDx(lh.getDx());
		h.setDy(lh.getDy());
		h.setNodataValue(lh.getNodataValue());
		h.setNotes(lh.getNotes());
		h.setNumberCols(lh.getNumberCols());
		h.setNumberRows(lh.getNumberRows());
		h.setProjection(lh.getProjection());
		h.setXllcenter(lh.getXllcenter());
		h.setYllcenter(lh.getYllcenter());
	}
	else if( sgrid != NULL )
	{	sHeader sh = sgrid->getHeader();
		h.setDx(sh.getDx());
		h.setDy(sh.getDy());
		h.setNodataValue(sh.getNodataValue());
		h.setNotes(sh.getNotes());
		h.setNumberCols(sh.getNumberCols());
		h.setNumberRows(sh.getNumberRows());
		h.setProjection(sh.getProjection());
		h.setXllcenter(sh.getXllcenter());
		h.setYllcenter(sh.getYllcenter());
	}
	return h;
}
dHeader grid::dh_copyFromHeader( header & h )
{	dHeader dh;
	dh.setDx(h.getDx());
	dh.setDy(h.getDy());
	dh.setNodataValue(h.getNodataValue());
	dh.setNotes(h.getNotes());
	dh.setNumberCols(h.getNumberCols());
	dh.setNumberRows(h.getNumberRows());
	dh.setProjection(h.getProjection());
	dh.setXllcenter(h.getXllcenter());
	dh.setYllcenter(h.getYllcenter());
	return dh;
}
fHeader grid::fh_copyFromHeader( header & h )
{	fHeader fh;
	fh.setDx(h.getDx());
	fh.setDy(h.getDy());
	fh.setNodataValue((float)h.getNodataValue());
	fh.setNotes(h.getNotes());
	fh.setNumberCols(h.getNumberCols());
	fh.setNumberRows(h.getNumberRows());
	fh.setProjection(h.getProjection());
	fh.setXllcenter(h.getXllcenter());
	fh.setYllcenter(h.getYllcenter());
	return fh;
}
lHeader grid::lh_copyFromHeader( header & h )
{	lHeader lh;
	lh.setDx(h.getDx());
	lh.setDy(h.getDy());
	lh.setNodataValue((long)h.getNodataValue());
	lh.setNotes(h.getNotes());
	lh.setNumberCols(h.getNumberCols());
	lh.setNumberRows(h.getNumberRows());
	lh.setProjection(h.getProjection());
	lh.setXllcenter(h.getXllcenter());
	lh.setYllcenter(h.getYllcenter());
	return lh;
}
sHeader grid::sh_copyFromHeader( header & h )
{	sHeader sh;
	sh.setDx(h.getDx());
	sh.setDy(h.getDy());
	sh.setNodataValue((short)h.getNodataValue());
	sh.setNotes(h.getNotes());
	sh.setNumberCols(h.getNumberCols());
	sh.setNumberRows(h.getNumberRows());
	sh.setProjection(h.getProjection());
	sh.setXllcenter(h.getXllcenter());
	sh.setYllcenter(h.getYllcenter());
	return sh;
}

DATA_TYPE grid::getDataType()
{	if( dgrid != NULL )
		return DOUBLE_TYPE;
	else if( fgrid != NULL )
		return FLOAT_TYPE;
	else if( lgrid != NULL )
		return LONG_TYPE;
	else if( sgrid != NULL )
		return SHORT_TYPE;
	else
		return INVALID_DATA_TYPE;
}

long grid::LastErrorCode()
{	if( dgrid != NULL )
		return dgrid->LastErrorCode();
	else if( fgrid != NULL )
		return fgrid->LastErrorCode();
	else if( lgrid != NULL )
		return lgrid->LastErrorCode();
	else if( sgrid != NULL )
		return sgrid->LastErrorCode();
	else
		return tkNO_ERROR;
}