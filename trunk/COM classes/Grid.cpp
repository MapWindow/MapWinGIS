//********************************************************************************************************
//File name: Grid.cpp
//Description: Implementation of CGrid.
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
//1-10-2006 - 1-18-2006 - cdm -- Removed tkTiff; added tkGridRaster.
//********************************************************************************************************

#include "stdafx.h"
#include "Grid.h"
#include "Projections.h"
#include "cpl_string.h"
#include "comutil.h"
#include "Image.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CGrid
CGrid * activeGridObject = NULL;

// ***************************************************
//		gridCOMCALLBACK()
// ***************************************************
void gridCOMCALLBACK( int number, const char * message )
{	
	if( activeGridObject != NULL )
		activeGridObject->CallBack(number, message);
}

// ***************************************************
//		CallBack()
// ***************************************************
void CGrid::CallBack(long percent,const char * message)
{	
	Utility::DisplayProgress(_globalCallback, percent, message, _key);
}

// ***************************************************
//		Ctor
// ***************************************************
CGrid::CGrid()
{	
	_dgrid = NULL;
	_fgrid = NULL;
	_lgrid = NULL;
	_sgrid = NULL;
	_trgrid = NULL;

	_globalCallback = NULL;
	_lastErrorCode = tkNO_ERROR;
	_key = SysAllocString(L"");
	_filename = L"";
	_preferedDisplayMode = gpmAuto;
}

// ***************************************************
//		Dtor
// ***************************************************
CGrid::~CGrid()
{	
	if( activeGridObject == this )
		activeGridObject = NULL;

	VARIANT_BOOL retval;
	Close(&retval);

	::SysFreeString(_key);

	if (_globalCallback)
		_globalCallback->Release();
}

// ***************************************************
//		get_RasterColorTableColoringScheme()
// ***************************************************
// See comments at the top of tkGridRaster::GetIntValueGridColorTable
// Builds unique values color scheme for GDAL integer grids
STDMETHODIMP CGrid::get_RasterColorTableColoringScheme(IGridColorScheme **pVal)
{
	if (_trgrid == NULL) 	return S_FALSE;
	return _trgrid->GetIntValueGridColorTable(pVal) ? S_OK : S_FALSE;
}


// ***************************************************
//		get_Header()
// ***************************************************
STDMETHODIMP CGrid::get_Header(IGridHeader **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if (_trgrid != NULL)
	{
		// Make grid header
		CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)pVal);

		double dX = _trgrid->getDX();
		double dY = _trgrid->getDY();
		(*pVal)->put_dX(dX);
		(*pVal)->put_dY(dY);
		
		VARIANT ndv;
		VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
		ndv.vt = VT_R8;
		ndv.dblVal = _trgrid->noDataValue;
		(*pVal)->put_NodataValue(ndv);
		(*pVal)->put_Notes(m_globalSettings.emptyBstr);
		(*pVal)->put_NumberCols(_trgrid->getWidth());
		(*pVal)->put_NumberRows(_trgrid->getHeight());
		(*pVal)->put_XllCenter(_trgrid->getXllCenter());
		(*pVal)->put_YllCenter(_trgrid->getYllCenter());
		(*pVal)->put_Projection(_trgrid->Projection.AllocSysString());
		
		CComBSTR cTbl;
		if (_trgrid->ColorTable2BSTR(&cTbl))
			(*pVal)->put_ColorTable(cTbl);

		VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		(*pVal)->put_Owner((int*)(void*)_trgrid, (int*)NULL, (int*)NULL, (int*)NULL, (int*)NULL);
	}
	else if( _dgrid != NULL )
	{	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)pVal);
		(*pVal)->put_dX(_dgrid->getHeader().getDx());
		(*pVal)->put_dY(_dgrid->getHeader().getDy());
		VARIANT ndv;
		VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
		ndv.vt = VT_R8;
		ndv.dblVal = _dgrid->getHeader().getNodataValue();
		(*pVal)->put_NodataValue(ndv);
		CComBSTR bstrNotes(_dgrid->getHeader().getNotes());
		(*pVal)->put_Notes(bstrNotes);
		CComBSTR bstrProj(_dgrid->getHeader().getProjection());
		(*pVal)->put_Projection(bstrProj);
		(*pVal)->put_NumberCols(_dgrid->getHeader().getNumberCols());
		(*pVal)->put_NumberRows(_dgrid->getHeader().getNumberRows());
		(*pVal)->put_XllCenter(_dgrid->getHeader().getXllcenter());
		(*pVal)->put_YllCenter(_dgrid->getHeader().getYllcenter());
		VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		(*pVal)->put_Owner((int*)NULL, (int*)_dgrid, (int*)NULL, (int*)NULL, (int*)NULL);
	}
	else if( _fgrid != NULL )
	{	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)pVal);
		(*pVal)->put_dX(_fgrid->getHeader().getDx());
		(*pVal)->put_dY(_fgrid->getHeader().getDy());
		VARIANT ndv;
		VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
		ndv.vt = VT_R4;
		ndv.fltVal = _fgrid->getHeader().getNodataValue();
		(*pVal)->put_NodataValue(ndv);
		CComBSTR bstrNotes(_fgrid->getHeader().getNotes());
		(*pVal)->put_Notes(bstrNotes);
		CComBSTR bstrProj(_fgrid->getHeader().getProjection());
		(*pVal)->put_Projection(bstrProj);
		(*pVal)->put_NumberCols(_fgrid->getHeader().getNumberCols());
		(*pVal)->put_NumberRows(_fgrid->getHeader().getNumberRows());
		(*pVal)->put_XllCenter(_fgrid->getHeader().getXllcenter());
		(*pVal)->put_YllCenter(_fgrid->getHeader().getYllcenter());
		VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		(*pVal)->put_Owner((int*)NULL, (int*)NULL, (int*)NULL, (int*)NULL, (int*)_fgrid);
	}
	else if( _lgrid != NULL )
	{	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)pVal);
		(*pVal)->put_dX(_lgrid->getHeader().getDx());
		(*pVal)->put_dY(_lgrid->getHeader().getDy());
		VARIANT ndv;
		VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
		ndv.vt = VT_I4;
		ndv.lVal = _lgrid->getHeader().getNodataValue();
		(*pVal)->put_NodataValue(ndv);
		CComBSTR bstrNotes(_lgrid->getHeader().getNotes());
		(*pVal)->put_Notes(bstrNotes);
		CComBSTR bstrProj(_lgrid->getHeader().getProjection());
		(*pVal)->put_Projection(bstrProj);
		(*pVal)->put_NumberCols(_lgrid->getHeader().getNumberCols());
		(*pVal)->put_NumberRows(_lgrid->getHeader().getNumberRows());
		(*pVal)->put_XllCenter(_lgrid->getHeader().getXllcenter());
		(*pVal)->put_YllCenter(_lgrid->getHeader().getYllcenter());
		VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		(*pVal)->put_Owner((int*)NULL, (int*)NULL, (int*)NULL, (int*)_lgrid, (int*)NULL);
	}
	else if( _sgrid != NULL )
	{	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)pVal);
		(*pVal)->put_dX(_sgrid->getHeader().getDx());
		(*pVal)->put_dY(_sgrid->getHeader().getDy());
		VARIANT ndv;
		VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
		ndv.vt = VT_I2;
		ndv.iVal = _sgrid->getHeader().getNodataValue();
		(*pVal)->put_NodataValue(ndv);
		CComBSTR bstrNotes(_sgrid->getHeader().getNotes());
		(*pVal)->put_Notes(bstrNotes);
		CComBSTR bstrProj(_sgrid->getHeader().getProjection());
		(*pVal)->put_Projection(bstrProj);
		(*pVal)->put_NumberCols(_sgrid->getHeader().getNumberCols());
		(*pVal)->put_NumberRows(_sgrid->getHeader().getNumberRows());
		(*pVal)->put_XllCenter(_sgrid->getHeader().getXllcenter());
		(*pVal)->put_YllCenter(_sgrid->getHeader().getYllcenter());
		VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		(*pVal)->put_Owner((int*)NULL, (int*)NULL, (int*)_sgrid, (int*)NULL, (int*)NULL);
	}
	else
	{	
		*pVal = NULL;
		ErrorMessage(tkGRID_NOT_INITIALIZED);
	}

	return S_OK;
}

// ***************************************************
//		AssignNewProjection()
// ***************************************************
STDMETHODIMP CGrid::AssignNewProjection(BSTR projection, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	set_ProjectionIntoHeader(W2A(projection));

	*retval = TRUE;
	return S_OK;
}

// ***************************************************
//		SaveProjection()
// ***************************************************
// TODO: use geoprojection
void CGrid::SaveProjection(char* projection)						
{
	try
	{
		CStringW  projectionFilename = Utility::GetProjectionFilename(GetFilename());

		if (projectionFilename != "")
		{
			FILE * prjFile = NULL;
			prjFile = _wfopen(projectionFilename, L"wb");
			if (prjFile)
			{
				char * wkt;
				ProjectionTools * p = new ProjectionTools();
				p->ToESRIWKTFromProj4(&wkt, projection);

				if (wkt != NULL)
				{
					fprintf(prjFile, "%s", wkt);
					delete wkt;
				}

				fclose(prjFile);
				prjFile = NULL;
				delete p; //added by Lailin Chen 12/30/2005
			}
		}
	}
	catch(...)
	{
	}
}

// ***************************************************
//		set_ProjectionIntoHeader()
// ***************************************************
void CGrid::set_ProjectionIntoHeader(char * projection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if ( _trgrid != NULL)
	{
		_trgrid->Projection = projection;
	}
	else if( _dgrid != NULL )
	{	
		dHeader hdr = _dgrid->getHeader();
		hdr.setProjection(projection);
		_dgrid->setHeader(hdr);
	}
	else if( _fgrid != NULL )
	{	
		fHeader hdr = _fgrid->getHeader();	
		hdr.setProjection(projection);
		_fgrid->setHeader(hdr);
	}
	else if( _lgrid != NULL )
	{	
		lHeader hdr = _lgrid->getHeader();
		hdr.setProjection(projection);
		_lgrid->setHeader(hdr);
	}
	else if( _sgrid != NULL )
	{	
		sHeader hdr = _sgrid->getHeader();
		hdr.setProjection(projection);
		_sgrid->setHeader(hdr);
	}
	else
	{
		ErrorMessage(tkGRID_NOT_INITIALIZED);
	}
	SaveProjection(projection);
}

// ***************************************************
//		Resource()
// ***************************************************
STDMETHODIMP CGrid::Resource(BSTR newSrcPath, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	Close(retval);
	Open(newSrcPath, UnknownDataType, true, UseExtension, NULL, retval);

	return S_OK;
}

// ***************************************************
//		GetRowCore()
// ***************************************************
void CGrid::GetRowCore(long Row, void *Vals, bool useDouble, VARIANT_BOOL * retval)
{
	double* ValsDouble = reinterpret_cast<double*>(Vals);
	float* ValsFloat = reinterpret_cast<float*>(Vals);

	if (_trgrid != NULL)
	{
		if (Row < 0 || Row >= _trgrid->getHeight() )
		{
			_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
			Vals = NULL;
			*retval = FALSE;
		}

		int ncols = _trgrid->getWidth();

		if (!_trgrid->isInRam())
		{
			_trgrid->GetFloatWindow(Vals, Row, Row, 0, ncols-1, useDouble);
		}
		else
		{
			for (int i = 0; i < ncols; i++)
			{
				if (useDouble) {
					ValsDouble[i] = _trgrid->getValue(Row, i);
				}
				else {
					ValsFloat[i] = static_cast<float>(_trgrid->getValue(Row, i));
				}
			}
		}
	}
	else if( _dgrid != NULL )
	{	
		int ncols = _dgrid->getHeader().getNumberCols();

		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				ValsDouble[i] = _dgrid->getValue(i, Row);
			}
			else {
				ValsFloat[i] = static_cast<float>(_dgrid->getValue(i, Row));
			}
		}
	}
	else if( _fgrid != NULL )
	{	
		int ncols = _fgrid->getHeader().getNumberCols();

		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				ValsDouble[i] = static_cast<double>(_fgrid->getValue(i, Row));
			}
			else {
				ValsFloat[i] = _fgrid->getValue(i, Row);
			}
		}
	}
	else if( _lgrid != NULL )
	{	int ncols = _lgrid->getHeader().getNumberCols();

		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				ValsDouble[i] = static_cast<double>(_lgrid->getValue(i, Row));
			}
			else {
				ValsFloat[i] = static_cast<float>(_lgrid->getValue(i, Row));
			}
		}
	}
	else if( _sgrid != NULL )
	{	int ncols = _sgrid->getHeader().getNumberCols();

		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				ValsDouble[i] = static_cast<double>(_sgrid->getValue(i, Row));
			}
			else {
				ValsFloat[i] = static_cast<float>(_sgrid->getValue(i, Row));
			}
		}
	}
	else
	{	Vals = NULL;
		ErrorMessage(tkGRID_NOT_INITIALIZED);
		*retval = S_FALSE;
	}
	*retval = TRUE;
}

// ***************************************************
//		GetRow()
// ***************************************************
STDMETHODIMP CGrid::GetRow(long Row, float *Vals, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	GetRowCore(Row, Vals, false, retval);
	return *retval ? S_OK : S_FALSE;
}

// ***************************************************
//		GetRow2()
// ***************************************************
STDMETHODIMP CGrid::GetRow2(long Row, double *Vals, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	GetRowCore(Row, Vals, true, retval);
	return *retval ? S_OK : S_FALSE;
}

// ***************************************************
//		PutRowCore()
// ***************************************************
void CGrid::PutRowCore(long Row, void *Vals, bool useDouble, VARIANT_BOOL * retval)
{
	double* ValsDouble = reinterpret_cast<double*>(Vals);
	float* ValsFloat = reinterpret_cast<float*>(Vals);

	if (_trgrid != NULL)
	{
		if (Row < 0 || Row >= _trgrid->getHeight() )
		{
			_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
			Vals = NULL;
			*retval = FALSE;
		}

		long ncols = _trgrid->getWidth();
		
		if (!_trgrid->isInRam())
		{
			_trgrid->PutFloatWindow(Vals, Row, Row, 0, ncols-1, useDouble);
		}
		else
		{
			for (long i = 0; i < ncols; i++)
			{
				if (useDouble) {
					_trgrid->putValue(Row, i, ValsDouble[i]);
				}
				else
				{
					_trgrid->putValue(Row, i, static_cast<double>(ValsFloat[i]));
				}
			}
		}
	}
	else if( _dgrid != NULL )
	{	
		int ncols = _dgrid->getHeader().getNumberCols();
		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				_dgrid->setValue(i, Row, ValsDouble[i]);
			}
			else {
				_dgrid->setValue(i, Row, static_cast<double>(ValsFloat[i]));
			}
		}
	}
	else if( _fgrid != NULL )
	{	
		int ncols = _fgrid->getHeader().getNumberCols();
		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				_fgrid->setValue(i, Row, static_cast<float>(ValsDouble[i]));
			}
			else {
				_fgrid->setValue(i, Row, ValsFloat[i]);
			}
		}
	}
	else if( _lgrid != NULL )
	{	
		int ncols = _lgrid->getHeader().getNumberCols();
		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				double val = ValsDouble[i];
				long lval = static_cast<long>(ValsDouble[i]);
				_lgrid->setValue(i, Row, static_cast<long>(ValsDouble[i]));
			}
			else {
				_lgrid->setValue(i, Row, static_cast<long>(ValsFloat[i]));
			}
		}
	}
	else if( _sgrid != NULL )
	{	
		int ncols = _sgrid->getHeader().getNumberCols();
		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				_sgrid->setValue(i, Row, static_cast<short>(ValsDouble[i]));
			}
			else {
				_sgrid->setValue(i, Row, static_cast<short>(ValsFloat[i]));
			}
		}
	}
	else
	{
		ErrorMessage(tkGRID_NOT_INITIALIZED);
		*retval = FALSE;
	}
	*retval = TRUE;
}

// ***************************************************
//		PutRow()
// ***************************************************
// only gdal grids not in-memory grids are supported, extend if needed
STDMETHODIMP CGrid::PutRow2(long Row, double *Vals, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	PutRowCore(Row, Vals, true, retval);
	return *retval ? S_OK : S_FALSE;
}

// ***************************************************
//		PutRow()
// ***************************************************
STDMETHODIMP CGrid::PutRow(long Row, float *Vals, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	PutRowCore(Row, Vals, false, retval);
	return *retval ? S_OK : S_FALSE;
}

// ***************************************************
//		SetInvalidValuesToNodata()
// ***************************************************
STDMETHODIMP CGrid::SetInvalidValuesToNodata(double MinThresholdValue, double MaxThresholdValue, VARIANT_BOOL * retval)
{
	if (MaxThresholdValue == MinThresholdValue || MaxThresholdValue < MinThresholdValue)
	{
		*retval = VARIANT_FALSE;
		return S_OK;
	}
	
	IGridHeader * hdr = NULL;
	get_Header(&hdr);

	long maxi = 0;
	hdr->get_NumberRows(&maxi);
	register long maxj = 0;
	hdr->get_NumberCols(&maxj);
	VARIANT vndv;
	hdr->get_NodataValue(&vndv);
	double ndv = 0;
	dVal(vndv, ndv);
	float fndv = static_cast<float>(ndv);

	float * Vals = new float[maxj];

	VARIANT_BOOL rt;

	for (long i = 0; i < maxi; i++)
	{
		bool Changed = false;
		this->GetFloatWindow(i, i, 0, maxj, Vals, &rt);
		for (register int j = 0; j < maxj; j++)
		{
			if ((Vals[j] > MaxThresholdValue || Vals[j] < MinThresholdValue) && Vals[j] != fndv)
			{
				Vals[j] = fndv;
				Changed = true;
			}
		}
		if (Changed)
			this->PutFloatWindow(i, i, 0, maxj, Vals, &rt);
	}

	*retval = VARIANT_TRUE;
	return S_OK;
}

// ***************************************************
//		get_Value()
// ***************************************************
STDMETHODIMP CGrid::get_Value(long Column, long Row, VARIANT *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_trgrid != NULL)
	{
		if (Column < 0 || Column >= _trgrid->getWidth() || Row< 0 || Row >= _trgrid->getHeight() )
		{
			_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
			pVal->vt = VT_R8;
			pVal->dblVal = _trgrid->noDataValue;
			return S_OK;
		}

		pVal->vt = VT_R8;
		pVal->dblVal = _trgrid->getValue(Row, Column);
	}
	else if( _dgrid != NULL )
	{	pVal->vt = VT_R8;
		pVal->dblVal = _dgrid->getValue( Column, Row );
	}
	else if( _fgrid != NULL )
	{	pVal->vt = VT_R4;
		pVal->fltVal = _fgrid->getValue( Column, Row );
	}
	else if( _lgrid != NULL )
	{	pVal->vt = VT_I4;
		pVal->lVal = _lgrid->getValue( Column, Row );
	}
	else if( _sgrid != NULL )
	{	pVal->vt = VT_I2;
		pVal->iVal = _sgrid->getValue( Column, Row );
	}
	else
	{	pVal->vt = VT_I2;
		pVal->iVal = 0;
		ErrorMessage(tkGRID_NOT_INITIALIZED);
	}
	return S_OK;
}

// ***************************************************
//		put_Value()
// ***************************************************
STDMETHODIMP CGrid::put_Value(long Column, long Row, VARIANT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	double Value;
	if( dVal(newVal, Value) == false )
	{	
		ErrorMessage(tkINVALID_VARIANT_TYPE);
	}
	else
	{	
		if ( _trgrid != NULL)
		{
			if (Column < 0 || Column >= _trgrid->getWidth() || Row< 0 || Row >= _trgrid->getHeight() )
			{
				_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
				return S_FALSE;
			}
			if (_trgrid->CanCreate()) // Can Create == Can Write
			{
				_trgrid->putValue(Row, Column, Value);
				return S_OK;
			}
			else
			{
				AfxMessageBox("Writing to this format is not allowed.");
				return S_FALSE;
			}
		}
		else if( _dgrid != NULL )
			_dgrid->setValue( Column, Row, Value );
		else if( _fgrid != NULL )
			_fgrid->setValue( Column, Row, (float)Value );
		else if( _lgrid != NULL )
			_lgrid->setValue( Column, Row, (long)Value );
		else if( _sgrid != NULL )
			_sgrid->setValue( Column, Row, (short)Value );
	}
	return S_OK;
}

// ***************************************************
//		get_InRam()
// ***************************************************
STDMETHODIMP CGrid::get_InRam(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if ( _trgrid != NULL)
		*pVal = _trgrid->isInRam()?VARIANT_TRUE:VARIANT_FALSE;
	else if( _dgrid != NULL )
		*pVal = _dgrid->inRam()?VARIANT_TRUE:VARIANT_FALSE;
	else if( _fgrid != NULL )
		*pVal = _fgrid->inRam()?VARIANT_TRUE:VARIANT_FALSE;
	else if( _lgrid != NULL )
		*pVal = _lgrid->inRam()?VARIANT_TRUE:VARIANT_FALSE;
	else if( _sgrid != NULL )
		*pVal = _sgrid->inRam()?VARIANT_TRUE:VARIANT_FALSE;
	else
	{	
		*pVal = VARIANT_TRUE;
		ErrorMessage(tkGRID_NOT_INITIALIZED);
	}
	
	return S_OK;
}

#pragma warning (disable:4244)
// ***************************************************
//		get_Maximum()
// ***************************************************
STDMETHODIMP CGrid::get_Maximum(VARIANT *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_trgrid != NULL)
	{
		pVal->vt = VT_R8;
		pVal->dblVal = _trgrid->GetMaximum();
	}
	else if( _dgrid != NULL )
	{	pVal->vt = VT_R8;
		pVal->dblVal = _dgrid->maximum();
	}
	else if( _fgrid != NULL )
	{	pVal->vt = VT_R4;
		pVal->fltVal = _fgrid->maximum();
	}
	else if( _lgrid != NULL )
	{	pVal->vt = VT_I4;
		pVal->lVal = _lgrid->maximum();
	}
	else if( _sgrid != NULL )
	{	pVal->vt = VT_I2;
		pVal->iVal = _sgrid->maximum();
	}
	else
	{	pVal->vt = VT_I2;
		pVal->iVal = 0;
		ErrorMessage(tkGRID_NOT_INITIALIZED);
	}
	return S_OK;
}

// ***************************************************
//		get_Minimum()
// ***************************************************
STDMETHODIMP CGrid::get_Minimum(VARIANT *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		
	if (_trgrid != NULL)
	{
		// GDAL doesn't always exclude nodata values.
		// If nodata == minimum generate by brute force.
		// This code isn't present on getmax -- only appears
		// to be a problem with ESRI tiffs (i.e., nodata is
		// the default large negative)

		double bruteForceResult = _trgrid->GetMinimum();
		float f1 = Utility::FloatRound(_trgrid->GetMinimum(), 4);
		float f2 = Utility::FloatRound(_trgrid->noDataValue, 4);
		float v1;
		float ndv = Utility::FloatRound(_trgrid->noDataValue, 4);
		if (Utility::FloatsEqual(f1, f2))
		{
			double currentMin = 9999999;
			long w = _trgrid->getWidth();
			long h = _trgrid->getHeight();
			for (register long i = 0; i < w; i++)
			{
				for (register long j = 0; j < h; j++)
				{
					if (_trgrid->getValue(j, i) < currentMin)
					{
						v1 = Utility::FloatRound(_trgrid->getValue(j, i), 4);
						if (!Utility::FloatsEqual(v1, ndv))
							currentMin = _trgrid->getValue(j, i);
					}
				}
			}
						
			if (currentMin != 9999999)
				bruteForceResult = currentMin;

			pVal->vt = VT_R8;
			pVal->dblVal = bruteForceResult;
		}
		else
		{
			pVal->vt = VT_R8;
			pVal->dblVal = _trgrid->GetMinimum();
		}
	}
	else if( _dgrid != NULL )
	{	pVal->vt = VT_R8;
		pVal->dblVal = _dgrid->minimum();
	}
	else if( _fgrid != NULL )
	{	pVal->vt = VT_R4;
		pVal->fltVal = _fgrid->minimum();
	}
	else if( _lgrid != NULL )
	{	pVal->vt = VT_I4;
		pVal->lVal = _lgrid->minimum();
	}
	else if( _sgrid != NULL )
	{	pVal->vt = VT_I2;
		pVal->iVal = _sgrid->minimum();
	}
	else
	{	pVal->vt = VT_I2;
		pVal->iVal = 0;
		ErrorMessage(tkGRID_NOT_INITIALIZED);
	}

	return S_OK;
}
#pragma warning (default:4244)

// ***************************************************
//		get_DataType()
// ***************************************************
STDMETHODIMP CGrid::get_DataType(GridDataType *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if ( _trgrid != NULL)
		*pVal = _trgrid->GetDataType();
	else if( _dgrid != NULL )
		*pVal = DoubleDataType;
	else if( _fgrid != NULL )
		*pVal = FloatDataType;
	else if( _lgrid != NULL )
		*pVal = LongDataType;
	else if( _sgrid != NULL )
		*pVal = ShortDataType;
	else
	{	
		*pVal = UnknownDataType;
		ErrorMessage(tkGRID_NOT_INITIALIZED);
	}

	// Prevent returning unknown data type -- if there's data, it should have a type... default to double ala ASCII grids (as ascii grids do...)
	if (*pVal == UnknownDataType)
		*pVal = DoubleDataType;

	return S_OK;
}

// ***************************************************
//		get_LastErrorCode()
// ***************************************************
STDMETHODIMP CGrid::get_Filename(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = W2BSTR(_filename);
	return S_OK;
}

// ***************************************************
//		get_LastErrorCode()
// ***************************************************
STDMETHODIMP CGrid::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _lastErrorCode;

	if( *pVal != tkNO_ERROR )
	{	if ( _trgrid != NULL)
			*pVal = 0; // todo -- trgrid ought to keep track of an error code
		else if( _dgrid != NULL )
			*pVal = _dgrid->LastErrorCode();
		else if( _fgrid != NULL )
			*pVal = _fgrid->LastErrorCode();
		else if( _lgrid != NULL )
			*pVal = _lgrid->LastErrorCode();
		else if( _sgrid != NULL )
			*pVal = _sgrid->LastErrorCode();
	}

	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// ***************************************************
//		get_ErrorMsg()
// ***************************************************
STDMETHODIMP CGrid::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// ***************************************************
//		GlobalCallback()
// ***************************************************
STDMETHODIMP CGrid::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _globalCallback;
	if( _globalCallback != NULL )
		_globalCallback->AddRef();
	return S_OK;
}
STDMETHODIMP CGrid::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// ***************************************************
//		Key()
// ***************************************************
STDMETHODIMP CGrid::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CGrid::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	return S_OK;
}

// ***************************************************
//		SaveProjectionAsWkt()
// ***************************************************
void CGrid::SaveProjectionAsWkt()
{
	CStringW prjFilename = Utility::GetProjectionFilename(GetFilename());
	if (prjFilename.GetLength() > 0)
	{
		IGeoProjection* proj = NULL;
		ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&proj);
		if (proj)
		{
			VARIANT_BOOL vb;
			CComBSTR bstrProj(prjFilename);
			proj->ReadFromFile(bstrProj, &vb);
			if (vb)
			{
				CComBSTR bstr;
				proj->ExportToProj4(&bstr);
				USES_CONVERSION;
				set_ProjectionIntoHeader(OLE2A(bstr));
			}
			proj->Release();
		}
	}
	else
	{
		set_ProjectionIntoHeader("");
	}
}

// ***************************************************
//		OpenCustomGrid()
// ***************************************************
bool CGrid::OpenCustomGrid(GridDataType DataType, bool inRam, GridFileType FileType)
{
	USES_CONVERSION;
	bool result = false;
	if( DataType == DoubleDataType )
	{	
		_dgrid = new dGrid();
		result = _dgrid->open( W2A(_filename), inRam, (GRID_TYPE)FileType, gridCOMCALLBACK );		// TODO: use Unicode
		if (!result)
		{	
			ErrorMessage(_dgrid->LastErrorCode());
			delete _dgrid;
			_dgrid = NULL;
		}
	}
	else if( DataType == FloatDataType )
	{	
		_fgrid = new fGrid();
		result = _fgrid->open( W2A(_filename), inRam, (GRID_TYPE)FileType, gridCOMCALLBACK );		// TODO: use Unicode
		if (!result)
		{
			ErrorMessage(_fgrid->LastErrorCode());
			delete _fgrid;
			_fgrid = NULL;
		}
	}
	else if( DataType == LongDataType )
	{	
		_lgrid = new lGrid();
		result = _lgrid->open( W2A(_filename), inRam, (GRID_TYPE)FileType, gridCOMCALLBACK );		// TODO: use Unicode
		if (!result)
		{	
			ErrorMessage(_lgrid->LastErrorCode());
			delete _lgrid;
			_lgrid = NULL;
		}
	}
	else if( DataType == ShortDataType )
	{	
		_sgrid = new sGrid();
		result = _sgrid->open( W2A(_filename), inRam, (GRID_TYPE)FileType, gridCOMCALLBACK) ;		// TODO: use Unicode
		if (!result)
		{	
			ErrorMessage(_sgrid->LastErrorCode());
			delete _sgrid;
			_sgrid = NULL;
		}
	}
	return result;
}

// ***************************************************
//		TryOpenAsAsciiGrid()
// ***************************************************
void CGrid::TryOpenAsAsciiGrid(GridDataType DataType, bool& inRam, bool& forcingGDALUse)
{
	if (DataType == DoubleDataType)
	{
		_dgrid = new dGrid();
		ifstream fin(_filename);
		_dgrid->asciiReadHeader(fin);
		dHeader dhd = _dgrid->getHeader();
		long ncol, nrow;
		ncol = dhd.getNumberCols();
		nrow = dhd.getNumberRows();
		if (MemoryAvailable(sizeof(double) * ncol * nrow)) {}
		else
		{
			inRam = false;
			forcingGDALUse = true;
		}
		fin.close();
		delete _dgrid;
		_dgrid = NULL;
	}
	else if (DataType == FloatDataType)
	{
		_fgrid = new fGrid();
		ifstream fin(_filename);
		_fgrid->asciiReadHeader(fin);
		fHeader fhd = _fgrid->getHeader();
		long ncol, nrow;
		ncol = fhd.getNumberCols();
		nrow = fhd.getNumberRows();
		if (MemoryAvailable(sizeof(double) * ncol * nrow)) {}
		else
		{
			inRam = false;
			forcingGDALUse = true;
		}
		fin.close();
		delete _fgrid;
		_fgrid = NULL;
	}
	else if (DataType == ShortDataType)
	{
		_sgrid = new sGrid();
		ifstream fin(_filename);
		_sgrid->asciiReadHeader(fin);
		sHeader shd = _sgrid->getHeader();
		long ncol, nrow;
		ncol = shd.getNumberCols();
		nrow = shd.getNumberRows();
		if (MemoryAvailable(sizeof(double) * ncol * nrow)) {}
		else
		{
			inRam = false;
			forcingGDALUse = true;
		}
		fin.close();
		delete _sgrid;
		_sgrid = NULL;
	}
	else if (DataType == LongDataType)
	{
		_lgrid = new lGrid();
		ifstream fin(_filename);
		_lgrid->asciiReadHeader(fin);
		lHeader lhd = _lgrid->getHeader();
		long ncol, nrow;
		ncol = lhd.getNumberCols();
		nrow = lhd.getNumberRows();
		if (MemoryAvailable(sizeof(double) * ncol * nrow)) {}
		else
		{
			inRam = false;
			forcingGDALUse = true;
		}
		fin.close();
		delete _lgrid;
		_lgrid = NULL;
	}
}

// ***************************************************
//		OpenAuxHeader()
// ***************************************************
bool CGrid::OpenAuxHeader(CStringW& filename)
{
	bool isAuxHeader = false;
	try
	{
		FILE * testAux = _wfopen(filename, L"r");
		if ( testAux )
		{
			char buf[12];
			fread(buf, 1,11,testAux);
			fclose(testAux);
			buf[11]='\0';
			if (strcmp(buf, "EHFA_HEADER") == 0) 
			{
				// Chris M 1/27/2007
				// Only make this replacement if sta.adf actually exists;
				// there are a few rara cases where EHFA_HEADER exists but
				// opening the current file is correct.
				CStringW str = filename;
				str.Delete(str.GetLength() - 4, 4);
				str.Append(L"\\sta.adf");
				struct _stat64i32 buf;
				int i = _wstat (str, &buf);
				if ( i == 0 )
				{
					filename = str;
					isAuxHeader = true;
				}
 			}
		}
	}
	catch(...)
	{
	}
	return isAuxHeader; 
}

// ***************************************************
//		GetDataTypeFromBinaryGrid()
// ***************************************************
void GetDataTypeFromBinaryGrid(GridDataType& DataType, GridFileType& FileType, CStringW& filename)
{
	if( (FileType == Ascii || FileType == Binary || FileType == Sdts) && DataType == UnknownDataType || DataType == InvalidDataType)
	{
		USES_CONVERSION;
		GridManager gm;
		DATA_TYPE dType = gm.getGridDataType( W2CA(filename), (GRID_TYPE)FileType );

		if( dType == DOUBLE_TYPE)
		{
			DataType = DoubleDataType;
		}
		else if(dType == FLOAT_TYPE)
		{
			DataType = FloatDataType;
		}
		else if (dType == LONG_TYPE)
		{
			DataType = LongDataType;
		}
		else if (dType == SHORT_TYPE )
		{
			DataType = ShortDataType;
		}
		else if (dType == BYTE_TYPE )
		{
			DataType = ByteDataType;
		}
	}
}

// ***************************************************
//		Open()
// ***************************************************
STDMETHODIMP CGrid::Open(BSTR Filename, GridDataType DataType, VARIANT_BOOL InRam, GridFileType FileType, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()) 
	USES_CONVERSION;

	Close(retval);

	if (SysStringLen(Filename) == 0)
	{
		ErrorMessage(tkINVALID_FILE);
		return S_FALSE;
	}

	_filename = OLE2BSTR(Filename);
	ICallback * tmpCallback = _globalCallback;
	if( cBack != NULL )	_globalCallback = cBack;
	bool inRam = InRam == VARIANT_TRUE ? true: false;
	
	// Are we handling ECWP imagery? 
	CStringW ecwpTest = _filename;
	if (ecwpTest.MakeLower().Left(4) == L"ecwp") 
	{
		// e.g. ecwp://imagery.oregonexplorer.info/ecwimages/2005orthoimagery.ecw
		_trgrid = new tkGridRaster();
		*retval = _trgrid->LoadRaster(_filename, true, FileType)?VARIANT_TRUE:VARIANT_FALSE;
		return S_OK;
	}
	
	// check or sta.adf file
	bool isAuxHeader = OpenAuxHeader(_filename);

	activeGridObject = this;
	bool opened = false;

	// get extension
	CString extension = W2A(_filename.Right( _filename.GetLength() - 1 - _filename.ReverseFind('.') ));
	ResolveFileType(FileType, extension);
	if (isAuxHeader) extension = "adf";

	// If it's an ASCII, we want to open it with our object ONLY if we have enough memory because disk-based via our object isn't supported.
	// Note that GDAL cannot read BGD as it's our own native format.
	bool forcingGDALUse = false;
	if (FileType == Ascii)
	{
		if (!inRam)
		{
			forcingGDALUse = true;
		}
		else
		{
			TryOpenAsAsciiGrid(DataType, inRam, forcingGDALUse);
		}
	}
	else if (FileType == Sdts)
	{
		inRam = true;
	}

	// check whether it's binary and can be opened with our own grid classes
	GetDataTypeFromBinaryGrid(DataType, FileType, _filename);

	// If opening an existing ASCII grid, this can be done with GDAL. GDAL doesn't
	// support creation of ASCII grids however, so this will be done with our object
	// for now. (This can be worked around using a virtual raster, but this will do for now)
	if ( FileType == Esri || FileType == Flt || FileType == Ecw || FileType == Bil || FileType == MrSid || 
		FileType == PAux || FileType == PCIDsk || FileType == DTed ||
		(FileType == UseExtension && (((A2W(extension) == _filename) && extension != "") 
		|| extension.CompareNoCase("adf") == 0 || extension.CompareNoCase("dem") == 0
		|| extension.CompareNoCase("ecw") == 0 || extension.CompareNoCase("bil") == 0 
		|| extension.CompareNoCase("sid") == 0 || extension.CompareNoCase("aux") == 0 
		|| extension.CompareNoCase("pix") == 0 || extension.CompareNoCase("dhm") == 0 
		|| extension.CompareNoCase("dt0") == 0 || extension.CompareNoCase("dt1") == 0)))
	{
		_trgrid = new tkGridRaster();
		opened = _trgrid->LoadRaster(_filename, inRam, FileType);
	}
	else if ( forcingGDALUse || FileType == GeoTiff || (FileType == UseExtension && extension.CompareNoCase("tif") == 0))
	{
		_trgrid = new tkGridRaster();
		opened = _trgrid->LoadRaster(_filename, inRam, FileType);
	}
	else if(OpenCustomGrid(DataType, inRam, FileType))
	{
		opened = true;
	}
	else
	{
		_trgrid = new tkGridRaster();
		opened = _trgrid->LoadRaster(_filename, inRam, FileType);
	}

	if (opened)
	{
		SaveProjectionAsWkt();
		_globalCallback = tmpCallback;
	}
	else
	{
		VARIANT_BOOL vb;
		this->Close(&vb);
		_filename = "";
	}

	*retval = opened ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

// ***************************************************
//		MemoryAvailable()
// ***************************************************
bool CGrid::MemoryAvailable(double bytes)
{
	MEMORYSTATUS stat;
	GlobalMemoryStatus (&stat);

	if (stat.dwAvailPhys >= bytes)
	  return true;

	return false;
}

// ***************************************************
//		CreateNew()
// ***************************************************
#pragma warning (disable:4244)
STDMETHODIMP CGrid::CreateNew(BSTR Filename, IGridHeader *Header, GridDataType DataType, VARIANT InitialValue, VARIANT_BOOL InRam, GridFileType FileType, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	ICallback * tmpCallback = _globalCallback;
	if( cBack != NULL )
		_globalCallback = cBack;

	Close(retval);	

	VARIANT_BOOL bInRam = VARIANT_FALSE;
	bool boolInRam = false;
	if(InRam == VARIANT_TRUE)
	{
		boolInRam = true;
		bInRam = VARIANT_TRUE;
	}
	
	activeGridObject = this;

	_filename = OLE2BSTR(Filename);
	CString f_name = OLE2A(Filename);
	CString extension = f_name.Right( f_name.GetLength() - 1 - f_name.ReverseFind('.') );

	ResolveFileType(FileType, extension);

	if (extension == f_name && f_name != "")
		FileType = Esri;

	if (DataType == UnknownDataType || DataType == InvalidDataType)
		DataType = DoubleDataType; // Cannot create a grid with an unknown or invalid data type

	double value; 
	if( dVal(InitialValue,value) == false )
	{	
		*retval = VARIANT_FALSE;
		ErrorMessage(tkINVALID_VARIANT_TYPE);
	}
	else
	{
		// Create new must use our object for ascii grids - gdal doesn't support
		// Create on AAIGrid, only CreateCopy. So, creating using gdal can be worked
		// around, but for now we'll use our object.		
		if ( FileType == Esri || FileType == Flt || FileType == GeoTiff || FileType == Ecw || FileType == Bil || 
			FileType == MrSid || FileType == PAux || FileType == PCIDsk || FileType == DTed ||
			(FileType == UseExtension && 
			(((extension == f_name) && extension != "") || extension.CompareNoCase("adf") == 0 
			|| extension.CompareNoCase("tif") == 0 || extension.CompareNoCase("ecw") == 0 
			|| extension.CompareNoCase("bil") == 0 || extension.CompareNoCase("sid") == 0 
			|| extension.CompareNoCase("aux") == 0 || extension.CompareNoCase("pix") == 0 
			|| extension.CompareNoCase("dhm") == 0 || extension.CompareNoCase("dt0") == 0
			|| extension.CompareNoCase("dt1") == 0)))   // TODO: reuse the code from Open
		{
			_trgrid = new tkGridRaster();

			GridFileType newFileType = FileType;

			ResolveFileType(newFileType, extension);

			if (_trgrid->CanCreate(newFileType))
			{
				double dx;
				Header->get_dX(&dx);
				double dy;
				Header->get_dY(&dy);
				VARIANT ndv;
				VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
				Header->get_NodataValue(&ndv);
				double dndv;
				dVal(ndv,dndv);
				CComBSTR projection;
				Header->get_Projection(&projection);
				long ncols;
				Header->get_NumberCols(&ncols);
				long nrows;
				Header->get_NumberRows(&nrows);
				double xllcenter;
				Header->get_XllCenter(&xllcenter);
				double yllcenter;
				Header->get_YllCenter(&yllcenter);
				
				CComBSTR cTbl;
				Header->get_ColorTable(&cTbl);
				_trgrid->BSTR2ColorTable(cTbl);

				return _trgrid->CreateNew(W2A(Filename), newFileType, dx, dy, xllcenter, yllcenter, 
					dndv, OLE2A(projection), ncols, nrows, DataType, boolInRam, value, true)?S_OK:S_FALSE;
				}
			else
			{
				AfxMessageBox("Writing to this format is not allowed.");
				return S_FALSE;
			}
		}
		else if( DataType == DoubleDataType )
		{	
			_dgrid = new dGrid();
			dHeader dhdr;
			double dx;
			Header->get_dX(&dx);
			dhdr.setDx(dx);
			double dy;
			Header->get_dY(&dy);
			dhdr.setDy(dy);
			VARIANT ndv;
			VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
			Header->get_NodataValue(&ndv);
			double dndv;
			dVal(ndv,dndv);
			dhdr.setNodataValue(dndv);
			CComBSTR notes;
			Header->get_Notes(&notes);
			dhdr.setNotes(OLE2CA(notes));
			CComBSTR projection;
			Header->get_Projection(&projection);
			dhdr.setProjection(OLE2CA(projection));
			long ncols;
			Header->get_NumberCols(&ncols);
			dhdr.setNumberCols(ncols);
			long nrows;
			Header->get_NumberRows(&nrows);
			dhdr.setNumberRows(nrows);
			double xllcenter;
			Header->get_XllCenter(&xllcenter);
			dhdr.setXllcenter(xllcenter);
			double yllcenter;
			Header->get_YllCenter(&yllcenter);
			dhdr.setYllcenter(yllcenter);

			if( _dgrid->initialize(OLE2CA(Filename),dhdr,value,boolInRam) == true )
			{	
				_filename = OLE2W(Filename);
				*retval = VARIANT_TRUE;
			}
			else
			{	
				_dgrid = NULL;
				*retval = VARIANT_FALSE;
				ErrorMessage(_dgrid->LastErrorCode());
			}
			
			VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		}
		else if( DataType == FloatDataType )
		{	_fgrid = new fGrid();
			
			fHeader fhdr;
			double dx;
			Header->get_dX(&dx);
			fhdr.setDx(dx);

			double dy;
			Header->get_dY(&dy);
			fhdr.setDy(dy);
			VARIANT ndv;
			VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
			Header->get_NodataValue(&ndv);
			float fndv;
			fVal(ndv,fndv);
			fhdr.setNodataValue(fndv);
			CComBSTR notes;
			Header->get_Notes(&notes);
			fhdr.setNotes(OLE2CA(notes));
			CComBSTR projection;
			Header->get_Projection(&projection);
			fhdr.setProjection(OLE2CA(projection));
			long ncols;
			Header->get_NumberCols(&ncols);
			fhdr.setNumberCols(ncols);
			long nrows;
			Header->get_NumberRows(&nrows);
			fhdr.setNumberRows(nrows);
			double xllcenter;
			Header->get_XllCenter(&xllcenter);
			fhdr.setXllcenter(xllcenter);
			double yllcenter;
			Header->get_YllCenter(&yllcenter);
			fhdr.setYllcenter(yllcenter);

			if( _fgrid->initialize(OLE2CA(Filename),fhdr,(float)value,boolInRam) == true )
			{	
				_filename = OLE2W(Filename);
				*retval = VARIANT_TRUE;
			}
			else
			{	
				_fgrid = NULL;
				*retval = VARIANT_FALSE;
				ErrorMessage(_fgrid->LastErrorCode());
			}
			VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		}
		else if( DataType == LongDataType )
		{	_lgrid = new lGrid();

			lHeader lhdr;
			double dx;
			Header->get_dX(&dx);
			lhdr.setDx(dx);
			double dy;
			Header->get_dY(&dy);
			lhdr.setDy(dy);
			VARIANT ndv;
			VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
			Header->get_NodataValue(&ndv);
			long lndv;
			lVal(ndv,lndv);
			lhdr.setNodataValue(lndv);
			CComBSTR notes;
			Header->get_Notes(&notes);
			lhdr.setNotes(OLE2CA(notes));
			CComBSTR projection;
			Header->get_Projection(&projection);
			lhdr.setProjection(OLE2CA(projection));
			long ncols;
			Header->get_NumberCols(&ncols);
			lhdr.setNumberCols(ncols);
			long nrows;
			Header->get_NumberRows(&nrows);
			lhdr.setNumberRows(nrows);
			double xllcenter;
			Header->get_XllCenter(&xllcenter);
			lhdr.setXllcenter(xllcenter);
			double yllcenter;
			Header->get_YllCenter(&yllcenter);
			lhdr.setYllcenter(yllcenter);

			if( _lgrid->initialize(OLE2CA(Filename),lhdr,(long)value,boolInRam) == true )
			{	
				_filename = OLE2W(Filename);
				*retval = VARIANT_TRUE;
			}
			else
			{	
				_lgrid = NULL;
				*retval = VARIANT_FALSE;
				ErrorMessage(_lgrid->LastErrorCode());
			}
			VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		}
		else if( DataType == ShortDataType )
		{	_sgrid = new sGrid();

			sHeader shdr;
			double dx;
			Header->get_dX(&dx);
			shdr.setDx(dx);
			double dy;
			Header->get_dY(&dy);
			shdr.setDy(dy);
			VARIANT ndv;
			VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
			Header->get_NodataValue(&ndv);
			short sndv;
			sVal(ndv,sndv);
			shdr.setNodataValue(sndv);
			CComBSTR notes;
			Header->get_Notes(&notes);
			shdr.setNotes(OLE2CA(notes));
			CComBSTR projection;
			Header->get_Projection(&projection);
			shdr.setProjection(OLE2CA(projection));
			long ncols;
			Header->get_NumberCols(&ncols);
			shdr.setNumberCols(ncols);
			long nrows;
			Header->get_NumberRows(&nrows);
			shdr.setNumberRows(nrows);
			double xllcenter;
			Header->get_XllCenter(&xllcenter);
			shdr.setXllcenter(xllcenter);
			double yllcenter;
			Header->get_YllCenter(&yllcenter);
			shdr.setYllcenter(yllcenter);

			if( _sgrid->initialize(OLE2CA(Filename),shdr,(short)value,boolInRam) == true )
			{	
				_filename = OLE2W(Filename);
				*retval = VARIANT_TRUE;
			}
			else
			{	
				_sgrid = NULL;
				*retval = VARIANT_FALSE;
				ErrorMessage(_sgrid->LastErrorCode());
			}
			VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		}
		else
		{	ErrorMessage(tkINVALID_DATA_TYPE);
		}
	}

	_globalCallback = tmpCallback;

	return S_OK;
}
#pragma warning (default:4244)

// ***************************************************
//		Close()
// ***************************************************
STDMETHODIMP CGrid::Close(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*retval = VARIANT_TRUE;
	if ( _trgrid != NULL)
		*retval = _trgrid->Close()?VARIANT_TRUE:VARIANT_FALSE;
	_trgrid = NULL;
	if( _dgrid != NULL )
		*retval = _dgrid->close()?VARIANT_TRUE:VARIANT_FALSE;
	_dgrid = NULL;
	if( _fgrid != NULL )
		*retval = _fgrid->close()?VARIANT_TRUE:VARIANT_FALSE;
	_fgrid = NULL;
	if( _lgrid != NULL )
		*retval = _lgrid->close()?VARIANT_TRUE:VARIANT_FALSE;	
	_lgrid = NULL;
	if( _sgrid != NULL )
		*retval = _sgrid->close()?VARIANT_TRUE:VARIANT_FALSE;
	_sgrid = NULL;
	return S_OK;
}

// ***************************************************
//		Save()
// ***************************************************
STDMETHODIMP CGrid::Save(BSTR Filename, GridFileType  FileType, ICallback * cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	*retval = VARIANT_FALSE;

	if (Filename == L"" || Filename == NULL)
		Filename = OLE2BSTR(_filename);

	ICallback * tmpCallback = _globalCallback;
	if( cBack != NULL )
		_globalCallback = cBack;
	activeGridObject = this;

	CString Filen(W2A(Filename));
	CString extension = Filen.Right( Filen.GetLength() - 1 - Filen.ReverseFind('.') );
	
	ResolveFileType(FileType, extension);

	if (extension == Filen && Filen != "")
		FileType = Esri;

	CString origFilename(W2A(this->_filename));
	CString origExtension = origFilename.Right( origFilename.GetLength() - 1 - origFilename.ReverseFind('.') );
	GridFileType origFileType;
	ResolveFileType(origFileType, origExtension);

	if (origFilename == origExtension && origFilename != "")
		origFileType = Esri;

	// (special conversion case) If they have a GDAL format and wish to save it to BGD, speed it up by writing
	// directly from the tkGridRaster class. (Generic conversion can do anything, but very slowly.)
	if (_trgrid != NULL && FileType == Binary)
	{
		*retval = _trgrid->SaveToBGD(OLE2A(Filename), gridCOMCALLBACK) ? VARIANT_TRUE : VARIANT_FALSE; //? S_OK : S_FALSE;
	}
	// (special conversion case) If they have a BGD and wish to save it to a GDAL format, speed it up by writing
	// directly from the tkGridRaster class. (Generic conversion can do anything, but very slowly.)
	else if (origFileType == Binary && _trgrid == NULL && 
		(_dgrid != NULL || _fgrid != NULL || _sgrid != NULL || _lgrid != NULL) &&
		(FileType == Ecw || FileType == Bil
		|| FileType == Esri || FileType == Flt || FileType == MrSid || FileType == PAux
		|| FileType == PCIDsk || FileType == DTed || FileType == GeoTiff))
	{
		tkGridRaster * tempGrid = new tkGridRaster();
		DATA_TYPE bgdDataType = SHORT_TYPE;

		// Read header info
		bool bRetval = tempGrid->ReadBGDHeader(OLE2A(this->_filename), bgdDataType);
		if (!bRetval) return false;

		GridDataType newDataType = DoubleDataType;
		
		if (bgdDataType == SHORT_TYPE)
			newDataType = ShortDataType;
		else if (bgdDataType == LONG_TYPE)
			newDataType = LongDataType;
		else if (bgdDataType == FLOAT_TYPE)
			newDataType = FloatDataType;
		else if (bgdDataType == DOUBLE_TYPE || bgdDataType == UNKNOWN_TYPE || bgdDataType == INVALID_DATA_TYPE)
			newDataType = DoubleDataType;

		// This looks odd to create a grid with data from itself, but it will have been read in by ReadBGDHeader.
		char * prj = new char[tempGrid->Projection.GetLength()+1];
		strcpy(prj, tempGrid->Projection.GetBuffer());
		bRetval = tempGrid->CreateNew(W2A(Filename), FileType, tempGrid->getDX(), tempGrid->getDY(), tempGrid->getXllCenter(), tempGrid->getYllCenter(), tempGrid->noDataValue, prj, tempGrid->getWidth(), tempGrid->getHeight(), newDataType, true, tempGrid->noDataValue, false);
		delete [] prj;
		if (!bRetval) return S_FALSE;

		// OK, load the data now that the destination grid is created
		bRetval = tempGrid->ReadFromBGD(OLE2A(this->_filename), gridCOMCALLBACK);
		if (!bRetval) return S_FALSE;

		// SAve it again for good measure. Disk based, so will probably just return true -- but will be flushed, so worthwhile
		bRetval = tempGrid->Save(W2A(Filename), FileType);
		if (!bRetval) return S_FALSE;

		tempGrid->Close();
		tempGrid = NULL;
		
		*retval = VARIANT_TRUE;
		//return S_OK;			// Is it needed to
	}
	// Note that ASCII grids are handled by this block only if trgrid is
	// not null; GDAL doesn't support creating ASCII grids, only "CreateCopy" on
	// ascii grids. This can be worked around, but for now use our object
	// to save newly created ascii grids and use GDAL to save from GDAL formats
	// to ascii grids. This block also handles saving existing ascii grids that
	// were opened using the GDAL wrapper.
	else if (_trgrid != NULL && (FileType != Ecw && FileType != Bil
		&& FileType != MrSid && FileType != PAux
		&& FileType != Esri && FileType != Flt && FileType != PCIDsk && FileType != DTed && FileType != GeoTiff)
		||
		(_trgrid == NULL && (FileType == Ecw || FileType == Bil
		|| FileType == Esri || FileType == MrSid || FileType == PAux
		|| FileType == Flt || FileType == PCIDsk || FileType == DTed || FileType == GeoTiff)))

	{
		// Create a generic grid object to capture the desired type, and 
		// copy the data across and save it.

		// Make a temporary grid of the type requested
		IGrid * tempGrid = NULL;
		HRESULT reslt;
		reslt = CoCreateInstance(CLSID_Grid, NULL, CLSCTX_INPROC_SERVER, IID_IGrid, (LPVOID*)(&tempGrid));

		IGridHeader * hdr;
		this->get_Header(&hdr);

		GridDataType dataType;
		this->get_DataType(&dataType);

		VARIANT nodataval;
		VariantInit(&nodataval); //added by Rob Cairns 4-Jan-06
		hdr->get_NodataValue(&nodataval);

		VARIANT_BOOL rslt = VARIANT_FALSE;
		tempGrid->CreateNew(Filename, hdr, dataType, nodataval, false, FileType, NULL, &rslt);

		if (!rslt) return S_FALSE;

		long numCols;
		long numRows;
		hdr->get_NumberCols(&numCols);
		hdr->get_NumberRows(&numRows);

		// Data
		VARIANT vPval;
		VariantInit(&vPval); //added by Rob Cairns 4-Jan-06
		for (int i = 0; i < numCols; i++)
		{
			for (int j = 0; j < numRows; j++)
			{
				this->get_Value(i, j, &vPval);
				tempGrid->put_Value(i, j, vPval); // The order here is correct (i,j) -- this is IGrid.
			}
		}
		VariantClear(&vPval); //added by Rob Cairns 4-Jan-06
		VariantClear(&nodataval); //added by Rob Cairns 4-Jan-06

		// Save the file
		tempGrid->Save(Filename, FileType, cBack, &rslt);

		// Clean up
		VARIANT_BOOL discardResult;
		tempGrid->Close(&discardResult);
		tempGrid = NULL;

		hdr = NULL; // Set this to null so it doesn't
		// get destroyed when it goes out of scope; there
		// are still references to it.

		*retval = rslt;
	}
	else if (_trgrid != NULL)
	{
		if( _trgrid->Save(W2A(Filename), FileType) )
		{	
			_filename = Filename;
			*retval = VARIANT_TRUE;
		}
		else
		{	
			ErrorMessage(tkFAILED_TO_SAVE_GRID);
		}
	}
	else if( _dgrid != NULL )
	{	
		if( _dgrid->save(OLE2CA(Filename), (GRID_TYPE)FileType, gridCOMCALLBACK) == true )
		{	
			_filename = Filename;
			*retval = VARIANT_TRUE;
		}
		else
		{	
			ErrorMessage(_dgrid->LastErrorCode());
		}
	}
	else if( _fgrid != NULL )
	{	
		if( _fgrid->save(OLE2CA(Filename), (GRID_TYPE)FileType, gridCOMCALLBACK) == true )
		{	
			_filename = Filename;
			*retval = VARIANT_TRUE;
		}
		else
		{	
			ErrorMessage(_fgrid->LastErrorCode());
		}
	}
	else if( _lgrid != NULL )
	{	
		if( _lgrid->save(OLE2CA(Filename), (GRID_TYPE)FileType, gridCOMCALLBACK) == true )
		{	
			_filename = Filename;
			*retval = VARIANT_TRUE;
		}
		else
		{	
			ErrorMessage(_lgrid->LastErrorCode());
		}
	}
	else if( _sgrid != NULL )
	{	
		if( _sgrid->save(OLE2CA(Filename), (GRID_TYPE)FileType, gridCOMCALLBACK) == true )
		{	
			_filename = Filename;
			*retval = VARIANT_TRUE;
		}
		else
		{	
			ErrorMessage(_sgrid->LastErrorCode());
		}
	}
	else
	{	
		ErrorMessage(tkGRID_NOT_INITIALIZED);
		return S_OK;
	}

	try
	{
		// Write the projection to the .prj file
		IGridHeader * header = NULL;
		this->get_Header(&header);
		CComBSTR bstrProj = NULL;
		header->get_Projection(&bstrProj);
		header->Release();
		header = NULL;

		if (strcmp(OLE2A(bstrProj), "") != 0)
		{
			SaveProjection(OLE2A(bstrProj));
		}
		_globalCallback = tmpCallback;
	}
	catch(...)
	{
		// No worries - for grids, the .prj is redundant.
	}

	return S_OK;
}

// ***************************************************
//		Clear()
// ***************************************************
STDMETHODIMP CGrid::Clear(VARIANT ClearValue, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	double value; 
	if( dVal(ClearValue,value) == false )
	{	*retval = VARIANT_FALSE;
		ErrorMessage(tkINVALID_VARIANT_TYPE);
	}
	else
	{
		*retval = VARIANT_TRUE;
		if ( _trgrid != NULL)
			_trgrid->clear(value);
		if( _dgrid != NULL )
			_dgrid->clear(value);
		else if( _fgrid != NULL )
			_fgrid->clear((float)value);
		else if( _lgrid != NULL )
			_lgrid->clear((long)value);
		else if( _sgrid != NULL )
			_sgrid->clear((short)value);
		else
		{	*retval = VARIANT_FALSE;
			ErrorMessage(tkGRID_NOT_INITIALIZED);
		}
	}
	return S_OK;
}

// ***************************************************
//		ProjToCell()
// ***************************************************
STDMETHODIMP CGrid::ProjToCell(double x, double y, long *Column, long *Row)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if ( _trgrid != NULL )
		_trgrid->ProjToCell( x, y, *Column, *Row );
	else if( _dgrid != NULL )
		_dgrid->ProjToCell( x, y, *Column, *Row );
	else if( _fgrid != NULL )
		_fgrid->ProjToCell( x, y, *Column, *Row );
	else if( _lgrid != NULL )
		_lgrid->ProjToCell( x, y, *Column, *Row );
	else if( _sgrid != NULL )
		_sgrid->ProjToCell( x, y, *Column, *Row );
	else
	{	*Column = -1;
		*Row = -1;
		ErrorMessage(tkGRID_NOT_INITIALIZED);
	}
	return S_OK;
}

// ***************************************************
//		CellToProj()
// ***************************************************
STDMETHODIMP CGrid::CellToProj(long Column, long Row, double *x, double *y)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_trgrid != NULL)
		_trgrid->CellToProj(Column, Row, *x, *y);
	else if( _dgrid != NULL )
		_dgrid->CellToProj(Column, Row, *x, *y);
	else if( _fgrid != NULL )
		_fgrid->CellToProj(Column, Row, *x, *y);
	else if( _lgrid != NULL )
		_lgrid->CellToProj(Column, Row, *x, *y);
	else if( _sgrid != NULL )
		_sgrid->CellToProj(Column, Row, *x, *y);
	else
	{	*x = 0;
		*y = 0;
		ErrorMessage(tkGRID_NOT_INITIALIZED);
	}
	return S_OK;
}

// ***************************************************
//		get_CdlgFilter()
// ***************************************************
STDMETHODIMP CGrid::get_CdlgFilter(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	USES_CONVERSION;

	//8/16/05 ah -- added GeoTiff (*.tif) to list of supported formats
	// Jan 15 2006 -- cdm -- Split among multiple lines to make it easier to look at.
	// Jan 15 2006 -- cdm -- Added *.arc (Arc/Info Ascii Grid)
	// Jan 15 2006 -- cdm -- Added *.aux (PCI .aux Labelled)
	// Jan 15 2006 -- cdm -- Added *.pix (PCIDSK Database File)
	// Jan 15 2006 -- cdm -- Added *.dhm, *.dt0, *.dt1 (DTED Elevation Raster)
	// Jan 15 2006 -- cdm -- Added *.bil (ESRI HDR/BIL Images)
	// August 18 2009  -- Paul Meems -- Added *.dem (USGS ASCII DEM) and re-ordered the filters

	// CDM 3/1/2006 - can always use ESRI grids now, thanks
	// to GDAL. It is RO however, where ours was RW...but
	// ours was dependent on ESRI DLL's. Dan et al. concur
	// that we should ditch partial RW support in favor of
	// full RO support. Therefore GridManager no longer
	// makes an appearance in this function to test CanUseESRI().

	// NOTE: These need to match the image filter for any duplicates.
		*pVal = A2BSTR( \
			"All Supported Grid Formats|sta.adf;*.bgd;*.asc;*.tif;????cel0.ddf;*.arc;*.aux;*.pix;*.dem;*.dhm;*.dt0;*.img;*.dt1;*.bil;*.nc|" \
			"ASCII Text (ESRI Ascii Grid) (*.asc, *.arc)|*.asc;*.arc|" \
			"BIL (ESRI HDR/BIL Images) (*.bil)|*.bil|" \
			"DTED Elevation (*.dhm, *.dt0, *.dt1)|*.dhm;*.dt0;*.dt1|" \
			"ERDAS Imagine (*.img)|*.img|" \
			"ESRI FLT (*.flt)|*.flt|" \
			"ESRI Grid (sta.adf)|sta.adf|" \
			"GeoTIFF (*.tif)|*.tif|" \
			"PAux (PCI .aux Labelled) (*.aux)|*.aux|" \
			"PIX (PCIDSK Database File) (*.pix)|*.pix|" \
			"USGS ASCII DEM (*.dem)|*.dem|" \
			"USGS SDTS 30m (*.ddf)|????cel0.ddf|" \
			"NetCDF (*.nc)|*.nc|" \
			"USU Binary (*.bgd)|*.bgd");
			
	return S_OK;
}

// ***************************************************
//		ResolveFileType()
// ***************************************************
void CGrid::ResolveFileType(GridFileType &newFileType, CString extension)
{
	if( extension.CompareNoCase("flt") == 0 )
		newFileType = Flt;
	else if( extension.CompareNoCase("asc") == 0 )
		newFileType = Ascii;
	else if( extension.CompareNoCase("bgd") == 0 )
		newFileType = Binary;
	else if( extension.CompareNoCase("ddf") == 0 )
		newFileType = Sdts;
	else if( extension.CompareNoCase("adf") == 0 )
		newFileType = Esri;
	else if (extension.CompareNoCase("ecw") == 0)
		newFileType = Ecw;
	else if (extension.CompareNoCase("bil") == 0)
		newFileType = Bil;
	else if (extension.CompareNoCase("sid") == 0)
		newFileType = MrSid;
	else if (extension.CompareNoCase("arc") == 0)
		newFileType = Ascii;
	else if (extension.CompareNoCase("aux") == 0)
		newFileType = PAux;
	else if (extension.CompareNoCase("pix") == 0)
		newFileType = PCIDsk;
	else if (extension.CompareNoCase("dhm") == 0)
		newFileType = DTed;
	else if (extension.CompareNoCase("dt0") == 0)
		newFileType = DTed;
	else if (extension.CompareNoCase("dt1") == 0)
		newFileType = DTed;
	else if (extension.CompareNoCase("tif") == 0)
		newFileType = GeoTiff;
	else if (extension.CompareNoCase("img") == 0)
		newFileType = Bil; // ERDAS Imagine is usually Bil and is essentially bil (or bsq.. same concept, different byte order)
	else if (extension.CompareNoCase("") == 0)
		newFileType = Binary;
	else
		// Assume Bil so it will attempt opening with tkGridRaster
		newFileType = Bil;
}

// ***************************************************
//		GetFloatWindow()
// ***************************************************
STDMETHODIMP CGrid::GetFloatWindow(long StartRow, long EndRow, long StartCol, long EndCol, float *Vals, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	GetFloatWindowCore(StartRow, EndRow, StartCol, EndCol, (void*)Vals, false, retval);
	return *retval ? S_OK : S_FALSE;
}

// ***************************************************
//		GetFloatWindow2()
// ***************************************************
STDMETHODIMP CGrid::GetFloatWindow2(long StartRow, long EndRow, long StartCol, long EndCol, double *Vals, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	GetFloatWindowCore(StartRow, EndRow, StartCol, EndCol, (void*)Vals, true, retval);
	return *retval ? S_OK : S_FALSE;
}

// ***************************************************
//		GetFloatWindowCore()
// ***************************************************
void CGrid::GetFloatWindowCore(long StartRow, long EndRow, long StartCol, long EndCol, void *Vals, bool useDouble, VARIANT_BOOL * retval)
{
	double* ValsDouble = reinterpret_cast<double*>(Vals);
	float* ValsFloat = reinterpret_cast<float*>(Vals);
	
	if (_trgrid != NULL)
	{
		if (StartRow < 0 || StartRow >= _trgrid->getHeight() || EndRow < 0 || EndRow >= _trgrid->getHeight() )
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			Vals = NULL;
			ValsDouble = NULL;
			*retval = FALSE;
		}

		if (!_trgrid->GetFloatWindow(Vals, StartRow, EndRow, StartCol, EndCol, useDouble))
		{
			*retval = VARIANT_FALSE;
		}
	}
	else if( _dgrid != NULL )
	{	
		long position = 0;
		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					ValsDouble[position++] = _dgrid->getValue(i, j);
				}
				else
				{
					ValsFloat[position++] = static_cast<float>(_dgrid->getValue(i, j));
				}
			}
		}
	}
	else if( _fgrid != NULL )
	{	
		long position = 0;
		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					ValsDouble[position++] = static_cast<double>(_fgrid->getValue(i, j));
				}
				else
				{
					ValsFloat[position++] = _fgrid->getValue(i, j);
				}
			}
		}
	}
	else if( _lgrid != NULL )
	{
		long position = 0;
		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					ValsDouble[position++] = static_cast<double>(_lgrid->getValue(i, j));
				}
				else
				{
					ValsFloat[position++] = static_cast<float>(_lgrid->getValue(i, j));
				}
			}
		}
	}
	else if( _sgrid != NULL )
	{
		long position = 0;
		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					ValsDouble[position++] = static_cast<double>(_sgrid->getValue(i, j));
				}
				else
				{
					ValsFloat[position++] = static_cast<float>(_sgrid->getValue(i, j));
				}
			}
		}
	}
	else
	{	Vals = NULL;
		ErrorMessage(tkGRID_NOT_INITIALIZED);
		*retval = VARIANT_FALSE;
	}

	*retval = VARIANT_TRUE;
}

// ***************************************************
//		PutFloatWindow()
// ***************************************************
STDMETHODIMP CGrid::PutFloatWindow(long StartRow, long EndRow, long StartCol, long EndCol, float *Vals, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	PutFloatWindowCore(StartRow, EndRow, StartCol, EndCol, (void*)Vals, false, retval);
	return *retval ? S_OK : S_FALSE;
}

// ***************************************************
//		PutFloatWindow2()
// ***************************************************
STDMETHODIMP CGrid::PutFloatWindow2(long StartRow, long EndRow, long StartCol, long EndCol, double *Vals, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	PutFloatWindowCore(StartRow, EndRow, StartCol, EndCol, (void*)Vals, true, retval);
	return *retval ? S_OK : S_FALSE;
}

// ******************************************************************
//		PutFloatWindowCore()
// ******************************************************************
void CGrid::PutFloatWindowCore(long StartRow, long EndRow, long StartCol, long EndCol, void *Vals, bool useDouble, VARIANT_BOOL * retval)
{
	double* ValsDouble = reinterpret_cast<double*>(Vals);
	float* ValsFloat = reinterpret_cast<float*>(Vals);
	if (_trgrid != NULL)
	{
 		if (StartRow < 0 || StartRow >= _trgrid->getHeight() || EndRow < 0 || EndRow >= _trgrid->getHeight())
		{
			_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
			Vals = NULL;
			*retval = FALSE;
		}

		if (!_trgrid->PutFloatWindow(Vals, StartRow, EndRow, StartCol, EndCol, useDouble))
		{
			*retval = FALSE;
		}
	}
	else if( _dgrid != NULL )
	{	
		int ncols = _dgrid->getHeader().getNumberCols();
		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					_dgrid->setValue(i, j, ValsDouble[position++]);
				}
				else
				{
					_dgrid->setValue(i, j, static_cast<float>(ValsFloat[position++]));
				}
			}
		}
	}
	else if( _fgrid != NULL )
	{	
		int ncols = _fgrid->getHeader().getNumberCols();
		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					_fgrid->setValue(i, j, static_cast<float>(ValsDouble[position++]));
				}
				else
				{
					_fgrid->setValue(i, j, ValsFloat[position++]);
				}
			}
		}
	}
	else if( _lgrid != NULL )
	{
		int ncols = _lgrid->getHeader().getNumberCols();
		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					_lgrid->setValue(i, j, static_cast<long>(ValsDouble[position++]));
				}
				else
				{
					_lgrid->setValue(i, j, static_cast<long>(ValsFloat[position++]));
				}
			}
		}
	}
	else if( _sgrid != NULL )
	{
		int ncols = _sgrid->getHeader().getNumberCols();
		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					_sgrid->setValue(i, j, static_cast<short>(ValsDouble[position++]));
				}
				else
				{
					_sgrid->setValue(i, j, static_cast<short>(ValsFloat[position++]));
				}
			}
		}
	}
	else
	{
		ErrorMessage(tkGRID_NOT_INITIALIZED);
		*retval = FALSE;
	}

	*retval = TRUE;
}

// ******************************************************************
//		get_Extents()
// ******************************************************************
STDMETHODIMP CGrid::get_Extents(IExtents** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = NULL;

	IGridHeader* header = NULL;
	this->get_Header(&header);

	if (header) {
		long cols, rows;
		double cellWidth, cellHeight, xll, yll;
		header->get_NumberCols(&cols);
		header->get_NumberRows(&rows);
		header->get_dX(&cellWidth);
		header->get_dY(&cellHeight);
		header->get_XllCenter(&xll);
		header->get_YllCenter(&yll);
		header->Release();
		
		double minX = xll - (cellWidth / 2);
		double maxX = xll + (cellWidth * (cols - 1)) + (cellWidth / 2);
		double minY = yll - (cellHeight / 2);
		double maxY = yll + (cellHeight * (rows - 1)) + (cellHeight / 2);

		IExtents* ext = NULL;
		CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)&ext);
		ext->SetBounds(minX, minY, 0.0, maxX, maxY, 0.0);

		*retVal = ext;
	}
	return S_OK;
}

// ******************************************************
//		Clone()
// ******************************************************
IGrid* CGrid::Clone(BSTR newFilename)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	IGridHeader* newHeader = NULL;
	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)&newHeader);
	
	IGridHeader* header = NULL;
	this->get_Header(&header);
	newHeader->CopyFrom(header);

	CComVariant noDataValue;
	header->get_NodataValue(&noDataValue);

	IGrid* newGrid = NULL;
	CoCreateInstance(CLSID_Grid,NULL,CLSCTX_INPROC_SERVER,IID_IGrid,(void**)&newGrid);
	
	GridDataType dataType;
	this->get_DataType(&dataType);

	VARIANT_BOOL vb;
	newGrid->CreateNew(newFilename, newHeader, dataType, noDataValue, false, GridFileType::UseExtension, NULL, &vb);
	if (!vb) {
		newGrid->Close(&vb);
		newGrid->Release();
		newGrid = NULL;
	}

	header->Release();
	newHeader->Release();
	
	return newGrid;
}

// **************************************************************
//		Clip()
// **************************************************************
IGrid* CGrid::Clip(BSTR newFilename, long firstCol, long lastCol, long firstRow, long lastRow)
{
	IGridHeader* newHeader = NULL;
	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)&newHeader);
	
	IGridHeader* header = NULL;
	this->get_Header(&header);
	newHeader->CopyFrom(header);

	double x, y, x2, y2;
	this->CellToProj(firstCol, firstRow, &x, &y);
	this->CellToProj(firstCol, lastRow, &x2, &y2);
	newHeader->put_XllCenter(x);
	newHeader->put_YllCenter(MIN(y, y2));
	newHeader->put_NumberCols(lastCol - firstCol + 1);
	newHeader->put_NumberRows(abs(lastRow - firstRow) + 1);

	CComVariant noDataValue;
	header->get_NodataValue(&noDataValue);

	IGrid* newGrid = NULL;
	CoCreateInstance(CLSID_Grid,NULL,CLSCTX_INPROC_SERVER,IID_IGrid,(void**)&newGrid);
	
	GridDataType dataType;
	this->get_DataType(&dataType);

	VARIANT_BOOL vb;
	newGrid->CreateNew(newFilename, newHeader, dataType, noDataValue, false, GridFileType::UseExtension, NULL, &vb);
	if (!vb) {
		newGrid->Close(&vb);
		newGrid->Release();
		newGrid = NULL;
	}

	header->Release();
	newHeader->Release();
	
	return newGrid;
}

// **************************************************************
//		get_NumBands()
// **************************************************************
STDMETHODIMP CGrid::get_NumBands(int *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_trgrid != NULL)
	{
		*retVal = _trgrid->getNumBands();
	}
	else if (_dgrid || _sgrid || _fgrid || _lgrid)
	{
		*retVal = 1;
	}
	else
	{
		*retVal = 0;	// it's not opened
	}
	return S_OK;
}

// **************************************************************
//		get_ActiveBandIndex()
// **************************************************************
STDMETHODIMP CGrid::get_ActiveBandIndex(int *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _trgrid != NULL ? _trgrid->GetActiveBandIndex() : 1;
	return S_OK;
}

// **************************************************************
//		OpenBand()
// **************************************************************
STDMETHODIMP CGrid::OpenBand(int bandIndex, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (_trgrid != NULL)		 // it works for GDAL-rooted grids
	{
		if (bandIndex < 1 || bandIndex > _trgrid->getNumBands())
		{
			this->ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
		else
		{
			if (_trgrid->OpenBand(bandIndex))
			{
				*retVal = VARIANT_TRUE;
			}
			else
			{
				// GDAL error should be retrieved through GlobalSettings interface
			}
		}
	}
	else
	{
		this->ErrorMessage(tkGDAL_GRID_NOT_OPENED);
	}
	return S_OK;
}

// ****************************************************************
//						ErrorMessage()						         
// ****************************************************************
void CGrid::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	Utility::DisplayErrorMsg(_globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// ****************************************************************
//						get_SourceType()						         
// ****************************************************************
STDMETHODIMP CGrid::get_SourceType (tkGridSourceType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())		
	if (_trgrid)
	{
		*retVal = tkGridSourceType::gstGDALBased;
	}
	else if (_dgrid || _sgrid || _fgrid || _lgrid)
	{
		*retVal = tkGridSourceType::gstNative;
	}
	else
	{
		*retVal = tkGridSourceType::gstUninitialized;
	}
	return S_OK;
}

// ****************************************************************
//		IsRgb()						         
// ****************************************************************
bool CGrid::IsRgb()
{
	return _trgrid ? _trgrid->IsRgb() : false;
}

// ****************************************************************
//						OpenAsImage()						         
// ****************************************************************
STDMETHODIMP CGrid::OpenAsImage(IGridColorScheme* scheme, tkGridProxyMode proxyMode, ICallback* cBack, IImage** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	
	if (!_globalCallback && cBack)
		ComHelper::SetRef(cBack, (IDispatch**)&_globalCallback, false);

	tkGridProxyMode mode = proxyMode;
	if (mode == gpmAuto)
	{
		mode = m_globalSettings.gridProxyMode;
	}
	
	CStringW gridName = GetFilename().MakeLower();

	// ---------------------------------------------
	//	Doing the checks
	// ---------------------------------------------
	if (!GdalHelper::CanOpenWithGdal(gridName) && mode == gpmNoProxy)
	{
		ErrorMessage(tkCANT_DISPLAY_WO_PROXY);
		return S_FALSE;
	}

	bool needsProxy;
	bool isRgb = IsRgb();
	if (mode == gpmAuto && isRgb) {
		needsProxy = false;
	}
	else {
		needsProxy = GridManager::NeedProxyForGrid(gridName, mode, this);
	}

	// ---------------------------------------------
	//	Opening
	// ---------------------------------------------
	if ( !needsProxy)
	{
		OpenAsDirectImage(scheme, _globalCallback, retVal);
	}
	else
	{
		if (GridManager::HasValidProxy(gridName))
		{
			*retVal = OpenImageProxy();
		}
		else
		{
			CreateImageProxy(scheme, retVal);
		}
	}
	return S_OK;
}

// ****************************************************************
//						OpenAsDirectImage()						         
// ****************************************************************
void CGrid::OpenAsDirectImage(IGridColorScheme* scheme, ICallback* cBack, IImage** retVal)
{
	VARIANT_BOOL vb;
	IImage* img = NULL;
	CStringW gridName = GetFilename().MakeLower();

	CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&img);

	CComBSTR bstr(gridName);
	img->Open(bstr, ImageType::USE_FILE_EXTENSION, false, cBack, &vb);
	if (vb)
	{
		*retVal = img;
		img->_pushSchemetkRaster(scheme, &vb);
		if (vb){
			CImageClass* cimg = ((CImageClass*)img);
			if (cimg)
			{
				
				cimg->sourceGridName = gridName;
				cimg->isGridProxy = false;
				
				// grab transparent color from image and save in color scheme
				cimg->get_UseTransparencyColor(&vb);
				if (vb)
				{
					OLE_COLOR transparentColor;
					cimg->get_TransparencyColor(&transparentColor);
					scheme->put_NoDataColor(transparentColor);
				}

				// save the color scheme to open next time
				CStringW legendName = this->GetLegendName();
				int bandIndex;
				this->get_ActiveBandIndex(&bandIndex);
				if (m_globalSettings.saveGridColorSchemeToFile) 
				{
					CComBSTR bstrName(legendName);
					CComBSTR bstrGridName(gridName);
					scheme->WriteToFile(bstrName, bstrGridName, bandIndex, &vb);
				}
			}

			// let's build overviews for direct grid
			GdalHelper::BuildOverviewsIfNeeded(gridName, true, _globalCallback);
		}
		else {
			ErrorMessage(tkNOT_APPLICABLE_TO_BITMAP);
		}
	}
}

// ****************************************************************
//			OpenImageProxy()						         
// ****************************************************************
IImage* CGrid::OpenImageProxy()
{
	VARIANT_BOOL hasProxy;
	this->get_HasValidImageProxy(&hasProxy);
	IImage* iimg= NULL;

	if (hasProxy)
	{
		VARIANT_BOOL vb;
		ComHelper::CreateInstance(tkInterface::idImage, (IDispatch**)&iimg);
		
		CComBSTR bstrName(this->GetProxyName());
		iimg->Open(bstrName, ImageType::USE_FILE_EXTENSION, VARIANT_FALSE, _globalCallback, &vb);
		if (!vb) 
		{
			iimg->Close(&vb);
			iimg->Release();
			iimg = NULL;
		}
	}
	return iimg;
}

// ****************************************************************
//			RemoveColorSchemeFile()						         
// ****************************************************************
bool CGrid::RemoveColorSchemeFile()
{
	return Utility::RemoveFile(this->GetProxyLegendName());
}

// ****************************************************************
//			RemoveImageProxy()						         
// ****************************************************************
STDMETHODIMP CGrid::RemoveImageProxy(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	GridManager::RemoveImageProxy(GetFilename());
	return *retVal ? S_OK: S_FALSE;
}

// ****************************************************************
//			PreferedDisplayMode()						         
// ****************************************************************
STDMETHODIMP CGrid::get_PreferedDisplayMode(tkGridProxyMode *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _preferedDisplayMode;
	return S_OK;
}
STDMETHODIMP CGrid::put_PreferedDisplayMode(tkGridProxyMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_preferedDisplayMode = newVal;
	return S_OK;
}

// ****************************************************************
//			CreateImageProxy()						         
// ****************************************************************
STDMETHODIMP CGrid::CreateImageProxy(IGridColorScheme* colorScheme, IImage** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	VARIANT_BOOL vb;
	RemoveImageProxy(&vb);
	GetUtils()->GridToImage2(this, colorScheme, m_globalSettings.gridProxyFormat, VARIANT_FALSE, this->_globalCallback, retVal);
	return S_OK;
}

// ****************************************************************
//			get_HasValidImageProxy()						         
// ****************************************************************
STDMETHODIMP CGrid::get_HasValidImageProxy(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CStringW gridFilename = GetFilename();
	*retVal = GridManager::HasValidProxy(gridFilename);
	return S_OK;
}

#pragma region Color scheme

// ****************************************************************
//		RetrieveOrGenerateColorScheme()						         
// ****************************************************************
STDMETHODIMP CGrid::RetrieveOrGenerateColorScheme(tkGridSchemeRetrieval retrievalMethod, tkGridSchemeGeneration generateMethod, 
												  PredefinedColorScheme colors, IGridColorScheme** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	RetrieveColorScheme(retrievalMethod, retVal); 
	if (!(*retVal))
	{
		GenerateColorScheme(generateMethod, colors, retVal);
	}
	return S_OK;
}

bool schemeIsValid(IGridColorScheme** scheme)
{
	if (!*scheme) return false;
	long numBreaks;
	(*scheme)->get_NumBreaks(&numBreaks);
	bool valid = numBreaks > 0;
	if (!valid)
	{
		(*scheme)->Release();
		(*scheme) = NULL;
	}
	return true;
}

// ****************************************************************
//		RetrieveColorScheme()						         
// ****************************************************************
STDMETHODIMP CGrid::RetrieveColorScheme(tkGridSchemeRetrieval method, IGridColorScheme** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	IGridColorScheme* scheme = NULL;
	VARIANT_BOOL vb;
	
	// disk based for the grid itself
	if (method == gsrAuto || method == gsrDiskBased)
	{
		CStringW legendName = this->GetLegendName();
		if (Utility::FileExistsW(legendName))
		{
			CoCreateInstance( CLSID_GridColorScheme, NULL, CLSCTX_INPROC_SERVER, IID_IGridColorScheme, (void**)&scheme);
			CComBSTR bstrLegendName(legendName);
			scheme->ReadFromFile(bstrLegendName, m_globalSettings.emptyBstr, &vb);
		}
	}
	
	// disk based for proxy
	if (method == gsrAuto || method == gsrDiskBasedForProxy)
	{
		if (!schemeIsValid(&scheme))
		{
			VARIANT_BOOL hasProxy;
			this->get_HasValidImageProxy(&hasProxy);
			if (hasProxy)
			{
				CStringW legendName = this->GetProxyLegendName();
				CoCreateInstance( CLSID_GridColorScheme, NULL, CLSCTX_INPROC_SERVER, IID_IGridColorScheme, (void**)&scheme);
				CComBSTR bstrLegendName(legendName);
				scheme->ReadFromFile(bstrLegendName, m_globalSettings.emptyBstr, &vb);
			}
		}
	}
	
	// from GDAL color table
	if (method == gsrAuto || method == gsrGdalColorTable)
	{
		if (!schemeIsValid(&scheme))
		{
			this->get_RasterColorTableColoringScheme(&scheme);
		}
	}
	
	*retVal = scheme;
	return S_OK;
}

// ****************************************************************
//			GenerateColorScheme()						         
// ****************************************************************
STDMETHODIMP CGrid::GenerateColorScheme(tkGridSchemeGeneration method, PredefinedColorScheme colors, IGridColorScheme** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	IGridColorScheme* scheme = NULL;
	switch(method)
	{
		case gsgGradient:
			scheme = BuildGradientColorSchemeCore(colors, ColoringType::Hillshade);
			break;
		case gsgUniqueValues:
			this->BuildUniqueColorScheme(INT_MAX, colors, ColoringType::Hillshade, &scheme);
			break;
		case gsgUniqueValuesOrGradient:
			this->BuildUniqueColorScheme(m_globalSettings.maxUniqueValuesCount, colors, ColoringType::Hillshade, &scheme);
			if (!schemeIsValid(&scheme))
			{
				scheme = BuildGradientColorSchemeCore(colors, ColoringType::Hillshade);
			}
			break;
	}
	*retVal = scheme;
	return S_OK;
}

// *************************************************************
//		BuildUniqueColorScheme()
// *************************************************************
bool CGrid::BuildUniqueColorScheme(int maxValuesCount, PredefinedColorScheme colors, ColoringType coloringType, IGridColorScheme** newScheme)
{
	*newScheme = NULL;

	set<CComVariant> values;
	if (!this->GetUniqueValues(values, maxValuesCount))
		return false;

	if (values.size() == 0)    // it's empty, hence no scheme
		return false;

	IGridColorScheme* result = NULL;
	CoCreateInstance(CLSID_GridColorScheme,NULL,CLSCTX_INPROC_SERVER,IID_IGridColorScheme,(void**)&result);
	if (result)
	{
		IColorScheme* scheme = NULL;
		CoCreateInstance(CLSID_ColorScheme,NULL,CLSCTX_INPROC_SERVER,IID_IColorScheme,(void**)&scheme);
		scheme->SetColors4(colors);		
		
		double minValue, maxValue;
		dVal(*values.begin(), minValue);
		dVal(*values.rbegin(), maxValue);

		set<CComVariant>::iterator it = values.begin();
		while (it != values.end())
		{
			double val;
			dVal(*it, val);

			// add break for value
			IGridColorBreak * brk;
			CoCreateInstance(CLSID_GridColorBreak,NULL,CLSCTX_INPROC_SERVER,IID_IGridColorBreak,(void**)&brk);
			brk->put_LowValue( val );
			brk->put_HighValue( val );

			OLE_COLOR color;
			if (maxValue == minValue)
			{
				double rnd = (double)rand()/(double)RAND_MAX;
				scheme->get_RandomColor(rnd, &color);	// any color from the scheme
			}
            else
			{
                double ratio = (val - minValue) / (maxValue - minValue);
				if (coloringType == ColoringType::Random)
				{
					scheme->get_RandomColor(ratio, &color);
				}
				else
				{
					scheme->get_GraduatedColor(ratio, &color);
				}
			}
			
			brk->put_LowColor(color);	 
            brk->put_HighColor(color);
			brk->put_ColoringType(coloringType);

			result->InsertBreak(brk);
			brk->Release();

			++it;
		}
		scheme->Release();
		*newScheme = result;
		return true;
	}
	return false;
}

// *************************************************************
//		GetUniqueValues()
// *************************************************************
// TODO: perhaps can be exposed to API
bool CGrid::GetUniqueValues(set<CComVariant>& values, int maxCount)
{
	IGridHeader* header = NULL;
	this->get_Header(&header);
	if (!header)
		return false;

	CComVariant varNodata;
	long rows = 0, cols = 0;

	header->get_NumberCols(&cols);
	header->get_NumberRows(&rows);
	header->get_NodataValue(&varNodata);
	header->Release();
	
	double noDataValue;
	dVal(varNodata, noDataValue);

	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < cols; j++)
		{
			CComVariant var;
			this->get_Value(j, i, &var);
			if (values.find(var) == values.end())
			{
				values.insert(var);
				if (values.size() > (unsigned int)maxCount)
				{
					// there are too many of them for unique values classification
					return false;		
				}
			}
		}
	}
	return true;
}

// ****************************************************************
//			BuildGradientColorSchemeCore()						         
// ****************************************************************
IGridColorScheme* CGrid::BuildGradientColorSchemeCore(PredefinedColorScheme colors, ColoringType coloringType)
{
	CComVariant max, min;
	this->get_Maximum(&max);
	this->get_Minimum(&min);
	
	IGridColorScheme* scheme = NULL;
	CoCreateInstance(CLSID_GridColorScheme,NULL,CLSCTX_INPROC_SERVER,IID_IGridColorScheme,(void**)&scheme);
	if (scheme)
	{
		double low, high;
		dVal(min, low);
		dVal(max, high);
		scheme->UsePredefined(low, high, colors);
		scheme->ApplyColoringType(coloringType);
	}
	return scheme;
}
#pragma endregion