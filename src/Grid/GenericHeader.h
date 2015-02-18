# ifndef HEADER_H
# define HEADER_H

# include "grdtypes.h"

#pragma warning(disable:4251)

/**********************************************************
			This class is the C++ Exported Grid Header class
			not the implementation of IGridHeader COM Class
**********************************************************/

class __declspec( dllexport ) header
{
	public:		
		//CONSTRUCTORS
		header();
		header( const header & h );

		//Destructor
		~header();

		//OPERATORS
		header operator=( const header & h );

		//DATA ACCESS MEMBERS
		inline long getNumberCols();
		inline long getNumberRows();
		inline double getNodataValue();
		inline double getDx();
		inline double getDy();
		inline double getXllcenter();
		inline double getYllcenter();
		char * getProjection();
		char * getNotes();

		void setNumberCols( long p_ncols );
		void setNumberRows( long p_nrows );
		void setNodataValue( double p_nodata_value );
		void setDx( double p_dx );
		void setDy( double p_dy );
		void setXllcenter( double p_xllcenter );
		void setYllcenter( double p_yllcenter );
		void setProjection( const char * p_projection );
		void setNotes( const char * p_notes );

	private:
		long ncols;
		long nrows;
		double nodataValue;
		double dx;
		double dy;
		double xllcenter;
		double yllcenter;
		CString projection;
		CString notes;

};

# endif