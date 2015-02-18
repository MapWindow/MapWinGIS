# ifndef VARH_H
# define VARH_H

//functions to convert from variant
bool dVal( const VARIANT& v, double & d );
bool fVal( const VARIANT& v, float & f );
bool lVal( const VARIANT& v, long & l );
bool sVal( const VARIANT& v, short & s );
bool stringVal( const VARIANT& v, CString & string );

//functions to convert to variants
VARIANT toVariant(double d);
VARIANT toVariant(float f);
VARIANT toVariant(long l);
VARIANT toVariant(short s);


# endif