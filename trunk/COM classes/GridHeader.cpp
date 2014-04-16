//********************************************************************************************************
//File name: GridHeader.cpp
//Description: Implementation of CGridHeader.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
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

// CGridHeader

STDMETHODIMP CGridHeader::CopyFrom(IGridHeader * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	double t;
	long t2;
	BSTR t3;
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

	pVal->get_Key(&t3);
	if (t3 != NULL)
	{
		BSTR n = OLE2BSTR(t3);
		this->put_Key(n);
		t3 = NULL;
	}

	pVal->get_ColorTable(&t3);
	if (t3 != NULL)
	{
		BSTR n = OLE2BSTR(t3);
		this->put_ColorTable(n);
		t3 = NULL;
	}
	

	pVal->get_Notes(&t3);
	if (t3 != NULL)
	{
		BSTR n = OLE2BSTR(t3);
		this->put_Notes(n);
		t3 = NULL;
	}

	pVal->get_Projection(&t3);
	if (t3 != NULL)
	{
		BSTR n = OLE2BSTR(t3);
		this->put_Projection(n);
		t3 = NULL;
	}

	this->AttemptSave();

	return S_OK;
}

STDMETHODIMP CGridHeader::put_Owner(int * t, int * d, int * s, int * l, int * f)
{
	myowner_t = (tkGridRaster*) t;
	myowner_d = (dHeader*) d;
	myowner_s = (sHeader*) s;
	myowner_l = (lHeader*) l;
	myowner_f = (fHeader*) f;

	return S_OK;
}

void CGridHeader::AttemptSave()
{
	USES_CONVERSION;

	BSTR s;
	m_geoProjection->ExportToProj4(&s);
	CString projection = OLE2CA(s);
	::SysFreeString(s);

	if (myowner_t != NULL)
	{
		myowner_t->Projection = projection;
		myowner_t->setDX(dx);
		myowner_t->setDY(dy);
		myowner_t->noDataValue = nodatavalue;
		myowner_t->setXllCenter(xllcenter);
		myowner_t->setYllCenter(yllcenter);
		myowner_t->SaveHeaderInfo();
	}
	else if (myowner_d != NULL)
	{
		myowner_d->setDx(dx);
		myowner_d->setDy(dy);
		myowner_d->setNodataValue(nodatavalue);
		myowner_d->setNotes(W2A(notes));
		myowner_d->setProjection(projection);
		myowner_d->setXllcenter(xllcenter);
		myowner_d->setYllcenter(yllcenter);
		// Disallow setting rows or columns. Must recreate for that
	}
	else if (myowner_s != NULL)
	{
		myowner_s->setDx(dx);
		myowner_s->setDy(dy);
		myowner_s->setNodataValue(static_cast<short>(nodatavalue));
		myowner_s->setNotes(W2A(notes));
		myowner_s->setProjection(projection);
		myowner_s->setXllcenter(xllcenter);
		myowner_s->setYllcenter(yllcenter);
		// Disallow setting rows or columns. Must recreate for that
	}
	else if (myowner_f != NULL)
	{
		myowner_f->setDx(dx);
		myowner_f->setDy(dy);
		myowner_f->setNodataValue(static_cast<float>(nodatavalue));
		myowner_f->setNotes(W2A(notes));
		myowner_f->setProjection(projection);
		myowner_f->setXllcenter(xllcenter);
		myowner_f->setYllcenter(yllcenter);
		// Disallow setting rows or columns. Must recreate for that
	}
	else if (myowner_l != NULL)
	{
		myowner_l->setDx(dx);
		myowner_l->setDy(dy);
		myowner_l->setNodataValue(static_cast<long>(nodatavalue));
		myowner_l->setNotes(W2A(notes));
		myowner_l->setProjection(projection);
		myowner_l->setXllcenter(xllcenter);
		myowner_l->setYllcenter(yllcenter);
		// Disallow setting rows or columns. Must recreate for that
	}
}

STDMETHODIMP CGridHeader::get_NumberCols(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = numbercols;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_NumberCols(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( newVal >= 0 )
		numbercols = newVal;
	else
	{	lastErrorCode = tkINVALID_PARAMETER_VALUE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
	}

	return S_OK;
}

STDMETHODIMP CGridHeader::get_NumberRows(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = numberrows;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_NumberRows(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( newVal >= 0 )
		numberrows = newVal;
	else
	{	lastErrorCode = tkINVALID_PARAMETER_VALUE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
	}

	return S_OK;
}

STDMETHODIMP CGridHeader::get_NodataValue(VARIANT *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	pVal->vt = VT_R8;
	pVal->dblVal = nodatavalue;
	
	return S_OK;
}

STDMETHODIMP CGridHeader::put_NodataValue(VARIANT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	double dval;
	if( dVal(newVal,dval) == false )
	{	lastErrorCode = tkINVALID_VARIANT_TYPE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
	}
	else
	{
		nodatavalue = dval;
		AttemptSave();
	}
	return S_OK;
}

STDMETHODIMP CGridHeader::get_dX(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = dx;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_dX(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//if( newVal > 0 )
		dx = newVal;
		AttemptSave();
	//else
	///{	lastErrorCode = tkINVALID_PARAMETER_VALUE;
	//	if( globalCallback != NULL )
	//		globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
	//}

	return S_OK;
}

STDMETHODIMP CGridHeader::get_dY(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = dy;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_dY(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( newVal > 0 )
	{
		dy = newVal;
		AttemptSave();
	}
	else
	{	lastErrorCode = tkINVALID_PARAMETER_VALUE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
	}

	return S_OK;
}

STDMETHODIMP CGridHeader::get_XllCenter(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = xllcenter;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_XllCenter(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	xllcenter = newVal;
	AttemptSave();

	return S_OK;
}

STDMETHODIMP CGridHeader::get_YllCenter(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = yllcenter;

	return S_OK;
}

STDMETHODIMP CGridHeader::put_YllCenter(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	yllcenter = newVal;
	AttemptSave();

	return S_OK;
}

STDMETHODIMP CGridHeader::get_Notes(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(notes);

	return S_OK;
}

STDMETHODIMP CGridHeader::put_Notes(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	//Rob Cairns 9 Nov 2009 Bug 1477
	//::SysFreeString(OLE2BSTR(notes));
	//notes = newVal;
	::SysFreeString(notes);
	notes = OLE2BSTR(newVal);

	AttemptSave();

	return S_OK;
}

STDMETHODIMP CGridHeader::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = lastErrorCode;
	lastErrorCode = tkNO_ERROR;

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

	*pVal = globalCallback;
	if( globalCallback != NULL )
		globalCallback->AddRef();

	return S_OK;
}

STDMETHODIMP CGridHeader::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&globalCallback);
	return S_OK;
}

STDMETHODIMP CGridHeader::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(key);

	return S_OK;
}

STDMETHODIMP CGridHeader::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	::SysFreeString(key);
	key = OLE2BSTR(newVal);

	return S_OK;
}

STDMETHODIMP CGridHeader::get_ColorTable(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(colorTable);

	return S_OK;
}

STDMETHODIMP CGridHeader::put_ColorTable(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	::SysFreeString(colorTable);
	colorTable = OLE2BSTR(newVal);

	return S_OK;
}

// ***********************************************************
//		get_GeoProjection
// ***********************************************************
STDMETHODIMP CGridHeader::get_GeoProjection(IGeoProjection** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (m_geoProjection)
		m_geoProjection->AddRef();
	*pVal = m_geoProjection;
	return S_OK;
}

// ***********************************************************
//		put_GeoProjection
// ***********************************************************
STDMETHODIMP CGridHeader::put_GeoProjection(IGeoProjection* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference((IDispatch*)newVal, (IDispatch**)&m_geoProjection, false);
	return S_OK;
}

STDMETHODIMP CGridHeader::get_Projection(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_geoProjection->ExportToProj4(pVal);
	return S_OK;
}

STDMETHODIMP CGridHeader::put_Projection(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	
	VARIANT_BOOL vbretval;
	m_geoProjection->ImportFromProj4(newVal, &vbretval);
	if (vbretval == VARIANT_FALSE)
	{
		m_geoProjection->ImportFromWKT(newVal, &vbretval);
	}

	//::SysFreeString(projection);
	//projection = OLE2BSTR(newVal);
	this->AttemptSave();
	return S_OK;
}