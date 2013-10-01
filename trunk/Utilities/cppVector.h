# ifndef cVECTOR_H
# define cVECTOR_H

# include "Matrix.h"

class cppVector
{
	public:
		cppVector();
		~cppVector();
		cppVector( double ii, double jj, double kk );
		cppVector( const cppVector & v );
		cppVector operator=( const cppVector & v );
		cppVector operator*( Matrix & m );
		cppVector operator+( const cppVector & v );

		void seti( double ii );
		void setj( double jj );
		void setk( double kk );
		double geti();
		double getj();
		double getk();
		void Normalize();
		double dot( const cppVector & v );
		cppVector crossProduct( const cppVector & v );

	private:
		double i;
		double j;
		double k;
};

# endif