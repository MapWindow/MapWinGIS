#include "stdafx.h"
# include "Matrix.h"
# include <math.h>

#ifdef _AFX // Use MFC mem leak facilities for operator new (but only if compiled with MFC :-))

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#endif

Matrix::Matrix()
{	for( int i = 0; i < MATRIXSIZE; i++ )
		for( int j = 0; j < MATRIXSIZE; j++ )
			matrix[i][j] = 0.0;
}

Matrix::~Matrix()
{
}

Matrix::Matrix( const Matrix & m )
{	for( int i = 0; i < MATRIXSIZE; i++ )
		for( int j = 0; j < MATRIXSIZE; j++ )
			matrix[i][j] = m.matrix[i][j];
}

Matrix Matrix::operator=( const Matrix & m )
{	Matrix result;
	for( int i = 0; i < MATRIXSIZE; i++ )
		for( int j = 0; j < MATRIXSIZE; j++ )
			result.matrix[i][j] = matrix[i][j] - m.matrix[i][j];
	return result;
}

Matrix Matrix::operator*( const Matrix & m )
{	Matrix result;
	for( int i = 0; i < MATRIXSIZE; i++ )
		for( int j = 0; j < MATRIXSIZE; j++ )
			for( int k = 0; k < MATRIXSIZE; k++ )
				result.matrix[i][j] += matrix[i][k]*m.matrix[k][j];
	return result;
}

Matrix Matrix::operator+( const Matrix & m )
{	Matrix result;
	for( int i = 0; i < MATRIXSIZE; i++ )
		for( int j = 0; j < MATRIXSIZE; j++ )
			result.matrix[i][j] = matrix[i][j] + m.matrix[i][j];
	return result;
}

Matrix Matrix::operator-( const Matrix & m )
{	Matrix result;
	for( int i = 0; i < MATRIXSIZE; i++ )
		for( int j = 0; j < MATRIXSIZE; j++ )
			result.matrix[i][j] = matrix[i][j] - m.matrix[i][j];
	return result;
}

void Matrix::Identity()
{	for( int i = 0; i < MATRIXSIZE; i++ )
		for( int j = 0; j < MATRIXSIZE; j++ )
			matrix[i][j] = 0.0;

	for( int d = 0; d < MATRIXSIZE; d++ )
		matrix[d][d] = 1.0;
}

void Matrix::set( int row, int column, double value )
{	if( row >= 0 && column >= 0 && row < MATRIXSIZE && column < MATRIXSIZE )
		matrix[row][column] = value;
}

double Matrix::get( int row, int column )
{	if( row >= 0 && column >= 0 && row < MATRIXSIZE && column < MATRIXSIZE )
		return matrix[row][column];
	else	
		return 0.0;
}

void Matrix::rotateX( int degree )
{	double radian = ((double)PIE*degree)/180;
	double cosX = cos( radian );
	double sinX = sin( radian );

	Identity();
	matrix[1][1] = cosX;
	matrix[1][2] = sinX;
	matrix[2][1] = -sinX;	
	matrix[2][2] = cosX;
}

void Matrix::rotateY( int degree )
{	double radian = ((double)PIE*degree)/180;
	double cosY = cos( radian );
	double sinY = sin( radian );

	Identity();
	matrix[0][0] = cosY;
	matrix[0][2] = -sinY;
	matrix[2][0] = sinY;	
	matrix[2][2] = cosY;
}

void Matrix::rotateMY( int degree )
{	double radian = ((double)PIE*degree)/180;
	double cosY = cos( radian );
	double sinY = sin( radian );

	Identity();
	matrix[0][0] = cosY;
	matrix[0][2] = sinY;
	matrix[2][0] = -sinY;	
	matrix[2][2] = cosY;
}

void Matrix::rotateZ( int degree )
{	double radian = ((double)PIE*degree)/180;
	double cosZ = cos( radian );
	double sinZ = sin( radian );
	
	Identity();
	matrix[0][0] = cosZ;
	matrix[0][1] = sinZ;
	matrix[1][0] = -sinZ;	
	matrix[1][1] = cosZ;
}

