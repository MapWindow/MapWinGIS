# ifndef FHEADER_H
# define FHEADER_H

# include "grdtypes.h"

#pragma warning(disable:4251)


class __declspec( dllexport ) fHeader
{
	public:	
		//CONSTRUCTORS
		fHeader();
		fHeader( const fHeader & h );

		//Destructor
		~fHeader();


		//OPERATORS
		fHeader operator=( const fHeader & h );

		//DATA ACCESS MEMBERS
		inline long getNumberCols();
		inline long getNumberRows();
		inline float getNodataValue();
		inline double getDx();
		inline double getDy();
		inline double getXllcenter();
		inline double getYllcenter();
		char * getProjection();
		char * getNotes();

		void setNumberCols( long p_ncols );
		void setNumberRows( long p_nrows );
		void setNodataValue( float p_nodata_value );
		void setDx( double p_dx );
		void setDy( double p_dy );
		void setXllcenter( double p_xllcenter );
		void setYllcenter( double p_yllcenter );
		void setProjection( const char * p_projection );
		void setNotes( const char * p_notes );

	private:
		long ncols;
		long nrows;
		float nodataValue;
		double dx;
		double dy;
		double xllcenter;
		double yllcenter;
		CString projection;
		CString notes;

};

# endif