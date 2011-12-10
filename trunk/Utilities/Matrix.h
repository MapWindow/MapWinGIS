
# ifndef MATRIX_H
# define MATRIX_H

# define MATRIXSIZE 4
# define PIE 3.14159

class __declspec( dllexport ) Matrix
{
	public:
		Matrix();
		~Matrix();
		Matrix( const Matrix & m );
		Matrix operator=( const Matrix & m );
		Matrix operator*( const Matrix & m );
		Matrix operator+( const Matrix & m );
		Matrix operator-( const Matrix & m );
		void Identity();
		void set( int row, int column, double value );
		double get( int row, int column );
		void rotateX( int degree );
		void rotateY( int degree );
		void rotateMY( int degree );
		void rotateZ( int degree );

	private:
		double matrix[MATRIXSIZE][MATRIXSIZE];
};
# endif