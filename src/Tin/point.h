# ifndef POINT_H
# define POINT_H

# include <math.h>

class Point
{
	public:
		//CONSTRUCTORS
		Point();
		Point( const Point & p );
		Point( double p_X, double p_Y, double p_Z = 0.0 );
		~Point();

		//OPERATORS
		Point operator=( const Point & p );
		bool operator==( const Point & p );

		//DATA MEMBER ACCESS
		double getX();
		double getY();
		double getZ();
		void setX( double p_X );
		void setY( double p_Y );
		void setZ( double p_Z );
	private:
		double x;
		double y;
		double z;
};

# endif