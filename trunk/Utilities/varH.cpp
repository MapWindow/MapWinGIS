# include "stdafx.h"
# include "varH.h"

#ifdef _AFX // Use MFC mem leak facilities for operator new (but only if compiled with MFC :-))

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#endif

/*
typedef struct tagVARIANT  {
    VARTYPE vt;
    unsigned short wReserved1;
    unsigned short wReserved2;
    unsigned short wReserved3;
    union {
        unsigned char        bVal;                        // VT_UI1.
        short                    iVal;                        // VT_I2    .
        long                    lVal;                        // VT_I4    .
        float                    fltVal;                    // VT_R4    .
        double                dblVal;                    // VT_R8    .
        VARIANT_BOOL        boolVal;                        // VT_BOOL.
        SCODE                    scode;                    // VT_ERROR.
        CY                        cyVal;                    // VT_CY    .
        DATE                    date;                        // VT_DATE.
        BSTR                    bstrVal;                    // VT_BSTR.
        IUnknown                FAR* punkVal;            // VT_UNKNOWN.
        IDispatch             FAR* pdispVal;            // VT_DISPATCH.
        SAFEARRAY            FAR* parray;            // VT_ARRAY|*.
        unsigned char        FAR* pbVal;                // VT_BYREF|VT_UI1.
        short                    FAR* piVal;                // VT_BYREF|VT_I2.
        long                    FAR* plVal;                // VT_BYREF|VT_I4.
        float                    FAR* pfltVal;            // VT_BYREF|VT_R4.
        double                FAR* pdblVal;            // VT_BYREF|VT_R8.
        VARIANT_BOOL        FAR* pboolVal;                // VT_BYREF|VT_BOOL.
        SCODE                    FAR* pscode;            // VT_BYREF|VT_ERROR.
        CY                        FAR* pcyVal;            // VT_BYREF|VT_CY.
        DATE                    FAR* pdate;                // VT_BYREF|VT_DATE.
        BSTR                    FAR* pbstrVal;            // VT_BYREF|VT_BSTR.
        IUnknown FAR*        FAR* ppunkVal;            // VT_BYREF|VT_UNKNOWN.
        IDispatch FAR*        FAR* ppdispVal;        // VT_BYREF|VT_DISPATCH.
        SAFEARRAY FAR*        FAR* pparray;            // VT_ARRAY|*.
        VARIANT                FAR* pvarVal;            // VT_BYREF|VT_VARIANT.
        void                    FAR* byref;                // Generic ByRef.
    };
};
*/        

bool dVal( const VARIANT& v, double & d )
{	USES_CONVERSION;
	switch( v.vt )
	{	
		case(VT_I2):
			d = v.iVal;
			break;
		case(VT_I4):
			d = v.lVal;
			break;
		case(VT_R4):
			d = v.fltVal;
			break;
		case(VT_R8):
			d = v.dblVal;
			break;
		case(VT_BSTR):
			d = atof(OLE2CA(v.bstrVal));
			break;
		case(VT_BYREF|VT_I2):
			d = *(v.piVal);
			break;
		case(VT_BYREF|VT_I4):
			d = *(v.plVal);
			break;
		case(VT_BYREF|VT_R4):
			d = *(v.pfltVal);
			break;
		case(VT_BYREF|VT_R8):
			d = *(v.pdblVal);
			break;
		case(VT_BYREF|VT_BSTR):
			d = atof(OLE2CA(*(v.pbstrVal)));
			break;
		case(VT_UI1):
			d = v.bVal;
			break;
		case(VT_UI2):
			d = v.uiVal;
			break;
		case(VT_UI4):
			d = v.ulVal;
			break;
		case(VT_I1):
			d = v.cVal;
			break;
		default:
			return false;
	}

	return true;
}

bool fVal( const VARIANT& v, float & f )
{	USES_CONVERSION;
	switch( v.vt )
	{	
		case(VT_I2):
			f = v.iVal;
			break;
		case(VT_I4):
			f = (float)v.lVal;
			break;
		case(VT_R4):
			f = v.fltVal;
			break;
		case(VT_R8):
			f =(float) v.dblVal;
			break;
		case(VT_BSTR):
			f = (float)atof(OLE2CA(v.bstrVal));
			break;
		case(VT_BYREF|VT_I2):
			f = (*(v.piVal));
			break;
		case(VT_BYREF|VT_I4):
			f = (float)(*(v.plVal));
			break;
		case(VT_BYREF|VT_R4):
			f = (*(v.pfltVal));
			break;
		case(VT_BYREF|VT_R8):
			f = (float)(*(v.pdblVal));
			break;
		case(VT_BYREF|VT_BSTR):
			f = (float)(atof(OLE2CA(*(v.pbstrVal))));
			break;
		case(VT_UI1):
			f = (float)v.bVal;
			break;
		case(VT_UI2):
			f = (float)v.uiVal;
			break;
		case(VT_UI4):
			f = (float)v.ulVal;
			break;
		case(VT_I1):
			f = (float)v.cVal;
			break;
		default:
			return false;
	}

	return true;
}

bool lVal( const VARIANT& v, long & l )
{	USES_CONVERSION;
	switch( v.vt )
	{	
		case(VT_I2):
			l = v.iVal;
			break;
		case(VT_I4):
			l = v.lVal;
			break;
		case(VT_R4):
			l = (long)v.fltVal;
			break;
		case(VT_R8):
			l = (long)v.dblVal;
			break;
		case(VT_BSTR):
			l = atoi(OLE2CA(v.bstrVal));
			break;
		case(VT_BYREF|VT_I2):
			l = *(v.piVal);
			break;
		case(VT_BYREF|VT_I4):
			l = *(v.plVal);
			break;
		case(VT_BYREF|VT_R4):
			l = (long)(*(v.pfltVal));
			break;
		case(VT_BYREF|VT_R8):
			l = (long)(*(v.pdblVal));
			break;
		case(VT_BYREF|VT_BSTR):
			l = atoi(OLE2CA(*(v.pbstrVal)));
			break;
		case(VT_UI1):
			l = (long)v.bVal;
			break;
		case(VT_UI2):
			l = (long)v.uiVal;
			break;
		case(VT_UI4):
			l = (long)v.ulVal;
			break;
		case(VT_I1):
			l = (long)v.cVal;
			break;
		default:
			return false;
	}
	return true;
}

bool sVal( const VARIANT& v, short & s )
{	USES_CONVERSION;
	switch( v.vt )
	{	
		case(VT_I2):
			s = v.iVal;
			break;
		case(VT_I4):
			s = (short)v.lVal;
			break;
		case(VT_R4):
			s = (short)v.fltVal;
			break;
		case(VT_R8):
			s = (short)v.dblVal;
			break;
		case(VT_BSTR):
			s = atoi(OLE2CA(v.bstrVal));
			break;
		case(VT_BYREF|VT_I2):
			s = *(v.piVal);
			break;
		case(VT_BYREF|VT_I4):
			s = (short)*(v.plVal);
			break;
		case(VT_BYREF|VT_R4):
			s = (short)*(v.pfltVal);
			break;
		case(VT_BYREF|VT_R8):
			s = (short)*(v.pdblVal);
			break;
		case(VT_BYREF|VT_BSTR):
			s = atoi(OLE2CA(*(v.pbstrVal)));
			break;
		case(VT_UI1):
			s = (short)v.bVal;
			break;
		case(VT_UI2):
			s = (short)v.uiVal;
			break;
		case(VT_UI4):
			s = (short)v.ulVal;
			break;
		case(VT_I1):
			s = (short)v.cVal;
			break;
		default:
			return false;
	}
	return true;
}

bool stringVal( const VARIANT& v, CString & string )
{	USES_CONVERSION;
	switch( v.vt )
	{	
		case(VT_I2):
			string.Format("%i",v.iVal);
			break;
		case(VT_I4):
			string.Format("%i",v.lVal);
			break;
		case(VT_R4):
			string.Format("%f",v.fltVal);
			break;
		case(VT_R8):
			string.Format("%f",v.dblVal);
			break;
		case(VT_BSTR):
			string = OLE2CA(v.bstrVal);
			break;
		case(VT_BYREF|VT_I2):
			string.Format("%i",*(v.piVal));
			break;
		/*case(VT_DATE):
			BSTR val;
			VarBstrFromDate(v.date,localeID,LOCALE_NOUSEROVERRIDE,&val);
			string = OLE2A(val);
			SysFreeString(val);
			break;*/
		case(VT_BYREF|VT_I4):
			string.Format("%i",*(v.plVal));
			break;
		case(VT_BYREF|VT_R4):
			string.Format("%f",*(v.pfltVal));
			break;
		case(VT_BYREF|VT_R8):
			string.Format("%f",*(v.pdblVal));
			break;
		case(VT_BYREF|VT_BSTR):
			string = OLE2CA(*(v.pbstrVal));
			break;
		case(VT_UI1):
			string.Format("%f", v.bVal);
			break;
		case(VT_UI2):
			string.Format("%f", v.uiVal);
			break;
		case(VT_UI4):
			string.Format("%f", v.ulVal);
			break;
		case(VT_I1):
			string.Format("%f", v.cVal);
			break;
		//Added by Rob Cairns 21-Mar-06
		case(VT_NULL): case(VT_EMPTY):
			string= "";
			break;
		default:
			string.Format("%i",v.vt);
			AfxMessageBox((LPCTSTR)string);
			return false;
	}
	return true;
}

VARIANT toVariant(double d)
{
	VARIANT v;
	VariantInit(&v); //added by Rob Cairns 4-Jan-06
	v.dblVal = d;
	v.vt = VT_R8;
	return v;
}

VARIANT toVariant(float f)
{
	VARIANT v;
	VariantInit(&v); //added by Rob Cairns 4-Jan-06
	v.fltVal = f;
	v.vt = VT_R4;
	return v;
}

VARIANT toVariant(long l)
{
	VARIANT v;
	VariantInit(&v); //added by Rob Cairns 4-Jan-06
	v.lVal = l;
	v.vt = VT_I4;
	return v;
}

VARIANT toVariant(short s)
{
	VARIANT v;
	VariantInit(&v); //added by Rob Cairns 4-Jan-06
	v.iVal = s;
	v.vt = VT_I2;
	return v;
}