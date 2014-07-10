#include "stdafx.h"
#include "RasterMatrix.h"

// Adapted from RasterMatrix.cpp 
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

RasterMatrix::RasterMatrix(): mColumns( 0 ), mRows( 0 ), mData( 0 )
{
	Debug::WriteLine("Constr");
}

RasterMatrix::RasterMatrix( int nCols, int nRows, float* data, double nodataValue ):
    mColumns( nCols ), mRows( nRows ), mData( data ), mNodataValue( nodataValue )
{
	Debug::WriteLine("No data value: %f", nodataValue);
}

RasterMatrix::RasterMatrix( const RasterMatrix& m ): mColumns( 0 ), mRows( 0 ), mData( 0 )
{
  operator=( m );
}


RasterMatrix& RasterMatrix::operator=( const RasterMatrix & m )
{
  delete[] mData;
  mColumns = m.nColumns();
  mRows = m.nRows();
  int nEntries = mColumns * mRows;
  mData = new float[nEntries];
  memcpy( mData, m.mData, sizeof( float ) * nEntries );
  mNodataValue = m.nodataValue();
  return *this;
}

RasterMatrix* RasterMatrix::Clone(bool memCopyData)
{
	int nEntries = nColumns() * nRows();
	RasterMatrix* m = new RasterMatrix(nColumns(), nRows(), new float[nEntries], mNodataValue);
	if (memCopyData)
	{
		memcpy( m->mData, mData, sizeof( float ) * nEntries );
	}
	return m;
}

void RasterMatrix::setData( int cols, int rows, float* data, double nodataValue )
{
  delete[] mData;
  mColumns = cols;
  mRows = rows;
  mData = data;
  mNodataValue = nodataValue;
}

float* RasterMatrix::takeData()
{
  float* data = mData;
  mData = 0; mColumns = 0; mRows = 0;
  return data;
}

bool RasterMatrix::add( const RasterMatrix& other )
{
  return twoArgumentOperation( opPLUS, other );
}

bool RasterMatrix::subtract( const RasterMatrix& other )
{
  return twoArgumentOperation( opMINUS, other );
}

bool RasterMatrix::multiply( const RasterMatrix& other )
{
  return twoArgumentOperation( opMUL, other );
}

bool RasterMatrix::divide( const RasterMatrix& other )
{
  return twoArgumentOperation( opDIV, other );
}

bool RasterMatrix::power( const RasterMatrix& other )
{
  return twoArgumentOperation( opPOW, other );
}

bool RasterMatrix::equal( const RasterMatrix& other )
{
  return twoArgumentOperation( opEQ, other );
}

bool RasterMatrix::notEqual( const RasterMatrix& other )
{
  return twoArgumentOperation( opNE, other );
}

bool RasterMatrix::greaterThan( const RasterMatrix& other )
{
  return twoArgumentOperation( opGT, other );
}

bool RasterMatrix::lesserThan( const RasterMatrix& other )
{
  return twoArgumentOperation( opLT, other );
}

bool RasterMatrix::greaterEqual( const RasterMatrix& other )
{
  return twoArgumentOperation( opGE, other );
}

bool RasterMatrix::lesserEqual( const RasterMatrix& other )
{
  return twoArgumentOperation( opLE, other );
}

bool RasterMatrix::logicalAnd( const RasterMatrix& other )
{
  return twoArgumentOperation( opAND, other );
}

bool RasterMatrix::logicalOr( const RasterMatrix& other )
{
  return twoArgumentOperation( opOR, other );
}

bool RasterMatrix::squareRoot()
{
  return oneArgumentOperation( opSQRT );
}

bool RasterMatrix::sinus()
{
  return oneArgumentOperation( opSIN );
}

bool RasterMatrix::asinus()
{
  return oneArgumentOperation( opASIN );
}

bool RasterMatrix::cosinus()
{
  return oneArgumentOperation( opCOS );
}

bool RasterMatrix::acosinus()
{
  return oneArgumentOperation( opACOS );
}

bool RasterMatrix::tangens()
{
  return oneArgumentOperation( opTAN );
}

bool RasterMatrix::atangens()
{
  return oneArgumentOperation( opATAN );
}

bool RasterMatrix::changeSign()
{
  return oneArgumentOperation( opSIGN );
}

bool RasterMatrix::oneArgumentOperation( OneArgOperator op )
{
  if ( !mData )
  {
    return false;
  }

  int nEntries = mColumns * mRows;
  double value;
  for ( int i = 0; i < nEntries; ++i )
  {
    value = mData[i];
    if ( value != mNodataValue )
    {
      switch ( op )
      {
        case opSQRT:
          if ( value < 0 ) //no complex numbers
          {
            mData[i] = static_cast<float>( mNodataValue );
          }
          else
          {
            mData[i] = static_cast<float>( sqrt( value ) );
          }
          break;
        case opSIN:
          mData[i] = static_cast<float>( sin( value ) );
          break;
        case opCOS:
          mData[i] = static_cast<float>( cos( value ) );
          break;
        case opTAN:
          mData[i] = static_cast<float>( tan( value ) );
          break;
        case opASIN:
          mData[i] = static_cast<float>( asin( value ) );
          break;
        case opACOS:
          mData[i] = static_cast<float>( acos( value ) );
          break;
        case opATAN:
          mData[i] = static_cast<float>( atan( value ) );
          break;
        case opSIGN:
          mData[i] = static_cast<float>( -value );
      }
    }
  }
  return true;
}

bool RasterMatrix::twoArgumentOperation( TwoArgOperator op, const RasterMatrix& other )
{
  if (op == opNONE) return false;
	
  if ( isNumber() && other.isNumber() ) //operation on two 1x1 matrices
  {
    //operations with nodata values always generate nodata
    if ( mData[0] == mNodataValue || other.number() == other.nodataValue() )
    {
      mData[0] = static_cast<float>( mNodataValue );
      return true;
    }
    switch ( op )
    {
      case opPLUS:
        mData[0] = static_cast<float>( number() + other.number() );
        break;
      case opMINUS:
        mData[0] = static_cast<float>( number() - other.number() );
        break;
      case opMUL:
        mData[0] = static_cast<float>( number() * other.number() );
        break;
      case opDIV:
        if ( other.number() == 0 )
        {
          mData[0] = static_cast<float>( mNodataValue );
        }
        else
        {
          mData[0] = static_cast<float>( number() / other.number() );
        }
        break;
      case opPOW:
        if ( !testPowerValidity( mData[0], ( float ) other.number() ) )
        {
          mData[0] = static_cast<float>( mNodataValue );
        }
        else
        {
          mData[0] = pow( mData[0], ( float ) other.number() );
        }
        break;
      case opEQ:
        mData[0] = mData[0] == other.number() ? 1.0f : 0.0f;
        break;
      case opNE:
        mData[0] = mData[0] == other.number() ? 0.0f : 1.0f;
        break;
      case opGT:
        mData[0] = mData[0] > other.number() ? 1.0f : 0.0f;
        break;
      case opLT:
        mData[0] = mData[0] < other.number() ? 1.0f : 0.0f;
        break;
      case opGE:
        mData[0] = mData[0] >= other.number() ? 1.0f : 0.0f;
        break;
      case opLE:
        mData[0] = mData[0] <= other.number() ? 1.0f : 0.0f;
        break;
      case opAND:
        mData[0] = mData[0] && other.number() ? 1.0f : 0.0f;
        break;
      case opOR:
        mData[0] = mData[0] || other.number() ? 1.0f : 0.0f;
        break;
    }
    return true;
  }

  //two matrices
  if ( !isNumber() && !other.isNumber() )
  {
    float* matrix = other.mData;
    int nEntries = mColumns * mRows;
    double value1, value2;

    for ( int i = 0; i < nEntries; ++i )
    {
      value1 = mData[i]; value2 = matrix[i];
      if ( value1 == mNodataValue || value2 == other.mNodataValue )
      {
        mData[i] = static_cast<float>( mNodataValue );
      }
      else
      {
        switch ( op )
        {
          case opPLUS:
            mData[i] = static_cast<float>( value1 + value2 );
            break;
          case opMINUS:
            mData[i] = static_cast<float>( value1 - value2 );
            break;
          case opMUL:
            mData[i] = static_cast<float>( value1 * value2 );
            break;
          case opDIV:
            if ( value2 == 0 )
            {
              mData[i] = static_cast<float>( mNodataValue );
            }
            else
            {
              mData[i] = static_cast<float>( value1 / value2 );
            }
            break;
          case opPOW:
            if ( !testPowerValidity( value1, value2 ) )
            {
              mData[i] = static_cast<float>( mNodataValue );
            }
            else
            {
              mData[i] = static_cast<float>( pow( value1, value2 ) );
            }
            break;
          case opEQ:
            mData[i] = value1 == value2 ? 1.0f : 0.0f;
            break;
          case opNE:
            mData[i] = value1 == value2 ? 0.0f : 1.0f;
            break;
          case opGT:
            mData[i] = value1 > value2 ? 1.0f : 0.0f;
            break;
          case opLT:
            mData[i] = value1 < value2 ? 1.0f : 0.0f;
            break;
          case opGE:
            mData[i] = value1 >= value2 ? 1.0f : 0.0f;
            break;
          case opLE:
            mData[i] = value1 <= value2 ? 1.0f : 0.0f;
            break;
          case opAND:
            mData[i] = value1 && value2 ? 1.0f : 0.0f;
            break;
          case opOR:
            mData[i] = value1 || value2 ? 1.0f : 0.0f;
            break;
        }
      }
    }
    return true;
  }

  //this matrix is a single number and the other one a real matrix
  if ( isNumber() )
  {
    float* matrix = other.mData;
    int nEntries = other.nColumns() * other.nRows();
    double value = mData[0];
    delete[] mData;
    mData = new float[nEntries]; mColumns = other.nColumns(); mRows = other.nRows();
    mNodataValue = other.nodataValue();

    if ( value == mNodataValue )
    {
      for ( int i = 0; i < nEntries; ++i )
      {
        mData[i] = static_cast<float>( mNodataValue );
      }
      return true;
    }

    for ( int i = 0; i < nEntries; ++i )
    {
      if ( matrix[i] == other.mNodataValue )
      {
        mData[i] = static_cast<float>( mNodataValue );
        continue;
      }

      switch ( op )
      {
        case opPLUS:
          mData[i] = static_cast<float>( value + matrix[i] );
          break;
        case opMINUS:
          mData[i] = static_cast<float>( value - matrix[i] );
          break;
        case opMUL:
          mData[i] = static_cast<float>( value * matrix[i] );
          break;
        case opDIV:
          if ( matrix[i] == 0 )
          {
            mData[i] = static_cast<float>( mNodataValue );
          }
          else
          {
            mData[i] = static_cast<float>( value / matrix[i] );
          }
          break;
        case opPOW:
          if ( !testPowerValidity( value, matrix[i] ) )
          {
            mData[i] = static_cast<float>( mNodataValue );
          }
          else
          {
            mData[i] = pow( static_cast<float>( value ), matrix[i] );
          }
          break;
        case opEQ:
          mData[i] = value == matrix[i] ? 1.0f : 0.0f;
          break;
        case opNE:
          mData[i] = value == matrix[i] ? 0.0f : 1.0f;
          break;
        case opGT:
          mData[i] = value > matrix[i] ? 1.0f : 0.0f;
          break;
        case opLT:
          mData[i] = value < matrix[i] ? 1.0f : 0.0f;
          break;
        case opGE:
          mData[i] = value >= matrix[i] ? 1.0f : 0.0f;
          break;
        case opLE:
          mData[i] = value <= matrix[i] ? 1.0f : 0.0f;
          break;
        case opAND:
          mData[i] = value && matrix[i] ? 1.0f : 0.0f;
          break;
        case opOR:
          mData[i] = value || matrix[i] ? 1.0f : 0.0f;
          break;
      }
    }
    return true;
  }
  else //this matrix is a real matrix and the other a number
  {
    double value = other.number();
    int nEntries = mColumns * mRows;

    if ( other.number() == other.mNodataValue )
    {
      for ( int i = 0; i < nEntries; ++i )
      {
        mData[i] = static_cast<float>( mNodataValue );
      }
      return true;
    }

    for ( int i = 0; i < nEntries; ++i )
    {
      if ( mData[i] == mNodataValue )
      {
        continue;
      }

      switch ( op )
      {
        case opPLUS:
          mData[i] = static_cast<float>( mData[i] + value );
          break;
        case opMINUS:
          mData[i] = static_cast<float>( mData[i] - value );
          break;
        case opMUL:
          mData[i] = static_cast<float>( mData[i] * value );
          break;
        case opDIV:
          if ( value == 0 )
          {
            mData[i] = static_cast<float>( mNodataValue );
          }
          else
          {
            mData[i] = static_cast<float>( mData[i] / value );
          }
          break;
        case opPOW:
          if ( !testPowerValidity( mData[i], value ) )
          {
            mData[i] = static_cast<float>( mNodataValue );
          }
          else
          {
            mData[i] = pow( mData[i], ( float ) value );
          }
          break;
        case opEQ:
          mData[i] = mData[i] == value ? 1.0f : 0.0f;
          break;
        case opNE:
          mData[i] = mData[i] == value ? 0.0f : 1.0f;
          break;
        case opGT:
          mData[i] = mData[i] > value ? 1.0f : 0.0f;
          break;
        case opLT:
          mData[i] = mData[i] < value ? 1.0f : 0.0f;
          break;
        case opGE:
          mData[i] = mData[i] >= value ? 1.0f : 0.0f;
          break;
        case opLE:
          mData[i] = mData[i] <= value ? 1.0f : 0.0f;
          break;
        case opAND:
          mData[i] = mData[i] && value ? 1.0f : 0.0f;
          break;
        case opOR:
          mData[i] = mData[i] || value ? 1.0f : 0.0f;
          break;
      }
    }
    return true;
  }
}

bool RasterMatrix::testPowerValidity( double base, double power )
{
  if (( base == 0 && power < 0 ) || ( power < 0 && ( power - floor( power ) ) > 0 ) )
  {
    return false;
  }
  return true;
}