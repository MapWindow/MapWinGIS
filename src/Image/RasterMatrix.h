#pragma once

// Adapted from RasterMatrix.h 
/***************************************************************************
    begin                : 2010-10-23
    copyright            : (C) 20010 by Marco Hugentobler
    email                : marco dot hugentobler at sourcepole dot ch
***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

enum TwoArgOperator
{
	opPLUS,
	opMINUS,
	opMUL,
	opDIV,
	opPOW,
	opEQ,         // =
	opNE,         // != resp. <>
	opGT,         // >
	opLT,         // <
	opGE,         // >=
	opLE,         // <=
	opAND,
	opOR,
	opNONE,
};

enum OneArgOperator
{
	opSQRT,
	opSIN,
	opCOS,
	opTAN,
	opASIN,
	opACOS,
	opATAN,
	opSIGN
};

class RasterMatrix
{
public:

	/**Takes ownership of data array*/
	RasterMatrix();
	//! @note note available in python bindings
	RasterMatrix( int nCols, int nRows, float* data, double nodataValue );
	RasterMatrix( const RasterMatrix& m );

	~RasterMatrix()
	{
		if (mData != NULL)
			delete[] mData;
	}


	/**Returns true if matrix is 1x1 (=scalar number)*/
	bool isNumber() const { return ( mColumns == 1 && mRows == 1 ); }
	double number() const { return mData[0]; }

	/**Returns data array (but not ownership)*/
	//! @note not available in python bindings
	float* data() { return mData; }
	/**Returns data and ownership. Sets data and nrows, ncols of this matrix to 0*/
	//! @note not available in python bindings
	float* takeData();

	void setData( int cols, int rows, float* data, double nodataValue );

	int nColumns() const { return mColumns; }
	int nRows() const { return mRows; }
	size_t GetBufferSize()
	{
		return mColumns * mRows * sizeof(float);
	}

	double nodataValue() const { return mNodataValue; }
	void setNodataValue( double d ) { mNodataValue = d; }
	RasterMatrix* RasterMatrix::Clone(bool memCopyData);

	RasterMatrix& operator=( const RasterMatrix& m );
	/**Adds another matrix to this one*/
	bool add( const RasterMatrix& other );
	/**Subtracts another matrix from this one*/
	bool subtract( const RasterMatrix& other );
	bool multiply( const RasterMatrix& other );
	bool divide( const RasterMatrix& other );
	bool power( const RasterMatrix& other );
	bool equal( const RasterMatrix& other );
	bool notEqual( const RasterMatrix& other );
	bool greaterThan( const RasterMatrix& other );
	bool lesserThan( const RasterMatrix& other );
	bool greaterEqual( const RasterMatrix& other );
	bool lesserEqual( const RasterMatrix& other );
	bool logicalAnd( const RasterMatrix& other );
	bool logicalOr( const RasterMatrix& other );

	bool squareRoot();
	bool sinus();
	bool asinus();
	bool cosinus();
	bool acosinus();
	bool tangens();
	bool atangens();
	bool changeSign();

	/**+,-,*,/,^,<,>,<=,>=,=,!=, and, or*/
	bool twoArgumentOperation( TwoArgOperator op, const RasterMatrix& other );

private:
	int mColumns;
	int mRows;
	double mNodataValue;
	float* mData;

	/*sqrt, sin, cos, tan, asin, acos, atan*/
	bool oneArgumentOperation( OneArgOperator op );
	bool testPowerValidity( double base, double power );
};