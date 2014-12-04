//********************************************************************************************************
//File name: GridHeader.cpp
//Description: Implementation of CGridHeader.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
//3-28-2005 dpa - Identical to public domain version.
//********************************************************************************************************

#include "stdafx.h"
#include "GridHeader.h"
#include "Projections.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ****************************************************************
//						ErrorMessage()						         
// ****************************************************************
void CGridHeader::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	Utility::DisplayErrorMsg("GridHeader", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// CGridHeader
STDMETHODIMP CGridHeader::CopyFrom(IGridHeader * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	double t;
	long t2;
	VARIANT t4;
	
	pVal->get_dX(&t);
	this->put_dX(t);
	
	pVal->get_dY(&t);
	this->put_dY(t);
	
	pVal->get_XllCenter(&t);
	this->put_XllCenter(t);

	pVal->get_YllCenter(&t);
	this->put_YllCenter(t);

	pVal->get_NumberCols(&t2);
	this->put_NumberCols(t2);

	pVal->get_NumberRows(&t2);
	this->put_NumberRows(t2);

	pVal->get_NodataValue(&t4);
	dVal(t4, t);
	VARIANT ndv;
	VariantInit(&ndv);
	ndv.vt = VT_R8;
	ndv.dblVal = t;
	this->put_NodataValue(ndv);
	VariantClear(&ndv);

	CComBSTR bstrKey;            // hardly makes sense to copy key; but let's keep it as it is
	pVal->get_Key(&bstrKey);
	this->put_Key(bstrKey);

	CComBSTR colorTable;
	pVal->get_ColorTable(&colorTable);
	this->put_ColorTable(colorTable);
	
	CComBSTR bstrNote;
	pVal->get_Notes(&bstrNote);
	this->put_Notes(bstrNote);
	
	CComBSTR bstrProjection;
	pVal->get_Projection(&bstrProjection);
	this->put_Projection(bstrProjection);

	this->AttemptSave();

	return S_OK;
}

STDMETHODIMP CGridHeader::put_Owner(int * t, int * d, int * s, int * l, int * f)
{
	_myowner_t = (tkGridRaster*) t;
	_myowner_d = (dHeader*) d;
	_myowner_s = (sHeader*) s;
	_myowner_l = (lHeader*) l;
	_myowner_f = (fHeader*) f;

	return S_OK;
}

void CGridHeader::AttemptSave()
{
	USES_CONVERSION;

	CComBSTR s;
	_projection->ExportToProj4(&s);
	CString projection = OLE2CA(s);

	if (_myowner_t != NULL)
	{
		_myowner_t->Projection = projection;
		_myowner_t->setDX(_dx);
		_myowner_t->setDY(_dy);
		_myowner_t->noDataValue = _nodatavalue;
		_myowner_t->setXllCenter(_xllcenter);
		_myowner_t->setYllCenter(_yllcenter);
		_myowner_t->SaveHeaderInfo();
	}
	else if (_myowner_d != NULL)
	{
		_myowner_d->setDx(_dx);
		_myowner_d->setDy(_dy);
		_myowner_d->setNodataValue(_nodatavalue);
		_myowner_d->setNotes(W2A(_notes));
		_myowner_d->setProjection(projection);
		_myowner_d->setXllcenter(_xllcenter);
		_myowner_d->setYllcenter(_yllcenter);
		// Disallow setting rows or columns. Must recreate for that
	}
	else if (_myowner_s != NULL)
	{
		_myowner_s->setDx(_dx);
		_myowner_s->setDy(_dy);
		_myowner_s->setNodataValue(static_cast<short>(_nodatavalue));
		_myowner_s->setNotes(W2A(_notes));
		_myowner_s->setProjection(projection);
		_myowner_s->setXllcenter(_xllcenter);
		_myowner_s->setYllcenter(_yllcenter);
		// Disallow setting rows or columns. Must recreate for that
	}
	else if (_myowner_f != NULL)
	{
		_myowner_f->setDx(_dx);
		_myowner_f->setDy(_dy);
		_myowner_f->setNodataValue(static_cast<float>(_nodatavalue));
		_myowner_f->setNotes(W2A(_notes));
		_myowner_f->setProjection(projection);
		_myowner_f->setXllcenter(_xllcenter);
		_myowner_f->setYllcenter(_yllcenter);
		// Disallow setting rows or columns. Must recreate for that
	}
	else if (_myowner_l != NULL)
	{
		_myowner_l->setDx(_dx);
		_myowner_l->setDy(_dy);
		_myowner_l->setNodataValue(static_cast<long>(_nodatavalue));
		_myowner_l->setNotes(W2A(_notes));
		_myowner_l->setProjection(projection);
		_myowner_l->setXllcenter(_xllcenter);
		_myowner_l->setYllcenter(_yllcenter);
		// Disallow setting rows or columns. Must recreate for that
	}
}

STDMETHODIMP CGridHeader::get_NumberCols(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _numbercols;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_NumberCols(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( newVal >= 0 )
		_numbercols = newVal;
	else
	{	
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}

	return S_OK;
}

STDMETHODIMP CGridHeader::get_NumberRows(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _numberrows;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_NumberRows(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( newVal >= 0 )
		_numberrows = newVal;
	else
		ErrorMessage(tkINVALID_PARAMETER_VALUE);

	return S_OK;
}

STDMETHODIMP CGridHeader::get_NodataValue(VARIANT *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	pVal->vt = VT_R8;
	pVal->dblVal = _nodatavalue;
	
	return S_OK;
}

STDMETHODIMP CGridHeader::put_NodataValue(VARIANT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	double dval;
	if( dVal(newVal,dval) == false )
	{	
		ErrorMessage(tkINVALID_VARIANT_TYPE);
	}
	else
	{
		_nodatavalue = dval;
		AttemptSave();
	}
	return S_OK;
}

STDMETHODIMP CGridHeader::get_dX(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _dx;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_dX(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_dx = newVal;
	AttemptSave();
	return S_OK;
}

STDMETHODIMP CGridHeader::get_dY(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _dy;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_dY(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( newVal > 0 )
	{
		_dy = newVal;
		AttemptSave();
	}
	else {	
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}

	return S_OK;
}

STDMETHODIMP CGridHeader::get_XllCenter(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _xllcenter;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_XllCenter(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	_xllcenter = newVal;
	AttemptSave();

	return S_OK;
}

STDMETHODIMP CGridHeader::get_YllCenter(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _yllcenter;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_YllCenter(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	_yllcenter = newVal;
	AttemptSave();

	return S_OK;
}

STDMETHODIMP CGridHeader::get_Notes(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(_notes);

	return S_OK;
}

STDMETHODIMP CGridHeader::put_Notes(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	::SysFreeString(_notes);
	_notes = OLE2BSTR(newVal);

	AttemptSave();

	return S_OK;
}

STDMETHODIMP CGridHeader::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;

	return S_OK;
}

STDMETHODIMP CGridHeader::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

STDMETHODIMP CGridHeader::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _globalCallback;
	if( _globalCallback != NULL )
		_globalCallback->AddRef();

	return S_OK;
}

STDMETHODIMP CGridHeader::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

STDMETHODIMP CGridHeader::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);

	return S_OK;
}

STDMETHODIMP CGridHeader::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);

	return S_OK;
}

STDMETHODIMP CGridHeader::get_ColorTable(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(_colorTable);

	return S_OK;
}

STDMETHODIMP CGridHeader::put_ColorTable(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	::SysFreeString(_colorTable);
	_colorTable = OLE2BSTR(newVal);

	return S_OK;
}

// ***********************************************************
//		get_GeoProjection
// ***********************************************************
STDMETHODIMP CGridHeader::get_GeoProjection(IGeoProjection** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_projection)
		_projection->AddRef();
	*pVal = _projection;
	return S_OK;
}

// ***********************************************************
//		put_GeoProjection
// ***********************************************************
STDMETHODIMP CGridHeader::put_GeoProjection(IGeoProjection* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef((IDispatch*)newVal, (IDispatch**)&_projection, false);
	return S_OK;
}

STDMETHODIMP CGridHeader::get_Projection(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_projection->ExportToProj4(pVal);
	return S_OK;
}

STDMETHODIMP CGridHeader::put_Projection(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	
	VARIANT_BOOL vbretval;
	_projection->ImportFromProj4(newVal, &vbretval);
	if (vbretval == VARIANT_FALSE)
	{
		_projection->ImportFromWKT(newVal, &vbretval);
	}

	this->AttemptSave();
	return S_OK;
}