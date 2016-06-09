# ifndef VERTEX_H
# define VERTEX_H

# include "MapWinGis.h"
# include <math.h>
# include "vector.h"

class vertex
{
	public:
		vertex();
		~vertex();
		vertex( const vertex & v );
		vertex( double v_X, double v_Y, double v_Z );
		vertex operator=( const vertex & v );
		vertex operator-( const vertex & v );
		vertex operator-( double scalar );
		vertex operator*( double scalar );
		vertex operator+( double scalar );
		bool operator==( const vertex & v );
		double getX();
		double getY();
		double getZ();
		void setX( double v_X );
		void setY( double v_Y );
		void setZ( double v_Z );
		void setNormal( cppVector p_normal );
		cppVector getNormal();
		double xyDistance( const vertex & v );
		double xyzDistance( const vertex & v );
	private:
		double x;
		double y;
		double z;

		cppVector normal;
};

# endif