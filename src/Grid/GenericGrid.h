#pragma once
# include "GridManager.h"
# include "dGrid.h"
# include "fGrid.h"
# include "lGrid.h"
# include "sGrid.h"
# include "GenericHeader.h"
# include "grdTypes.h"
using namespace std;

#pragma warning(disable:4251)

/**********************************************************
			This class is the C++ Exported Grid class
			not the implementation of IGrid COM Class
**********************************************************/

class __declspec( dllexport ) grid
{
	public:
		
		//CONSTRUCTORS
		grid();
		~grid();
		
		//OPERATORS
		double operator()( int Column, int Row );
				
		//FUNCTIONS
		bool open( const char * Filename, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION, void (*callback)( int number, const char * message ) = NULL );
		bool initialize( const char * Filename, header pheader, double initialValue, DATA_TYPE DataType = DOUBLE_TYPE, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION );		
		bool save( const char * Filename = "", GRID_TYPE GridType = USE_EXTENSION, void (*callback)(int number, const char * message ) = NULL );
		bool close();
		void clear(double clearValue);

		//MAPPING FUNCTIONS
		void ProjToCell( double x, double y, long & column, long & row );
		void CellToProj( long column, long row, double & x, double & y );
				
		//DATA MEMBER ACCESS
		inline header getHeader();
		void setHeader( header h );
		inline double getValue( int Column, int Row );
		inline void setValue( int Column, int Row, double Value );
		bool inRam();		
		double maximum();
		double minimum();
		long LastErrorCode();

		DATA_TYPE getDataType();
	private:

		header copyToHeader();
		dHeader dh_copyFromHeader( header & h );
		fHeader fh_copyFromHeader( header & h );
		lHeader lh_copyFromHeader( header & h );
		sHeader sh_copyFromHeader( header & h );

		dGrid * dgrid;
		fGrid * fgrid;
		lGrid * lgrid;
		sGrid * sgrid;

		long lastErrorCode;
};