# ifndef TIN_TYPES_H
# define TIN_TYPES_H

# define Undefined -1
# define CompareEpsilom .000001
# define PI 3.14159265359
# define rTOd 180.0/PI
# define dTOr PI/180.0
# define MaxTriangles 1073741824

void CheckZero( double & num );
#define CheckZero(num)	{ if( num < CompareEpsilom )	num = 0.0; }

# endif
