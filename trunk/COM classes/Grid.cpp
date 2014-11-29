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
	Utility::DisplayProgress(globalCallback, percent, message, key);
}

// ***************************************************
//		Ctor
// ***************************************************
CGrid::CGrid()
{	
	dgrid = NULL;
	fgrid = NULL;
	lgrid = NULL;
	sgrid = NULL;
	trgrid = NULL;

	globalCallback = NULL;
	lastErrorCode = tkNO_ERROR;
	key = A2BSTR("");
	filename = L"";
	preferedDisplayMode = gpmAuto;
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

	::SysFreeString(key);

	if (globalCallback)
		globalCallback->Release();
}

// ***************************************************
//		get_RasterColorTableColoringScheme()
// ***************************************************
// See comments at the top of tkGridRaster::GetIntValueGridColorTable
// Builds unique values color scheme for GDAL integer grids
STDMETHODIMP CGrid::get_RasterColorTableColoringScheme(IGridColorScheme **pVal)
{
	if (trgrid == NULL) 	return S_FALSE;
	return trgrid->GetIntValueGridColorTable(pVal) ? S_OK : S_FALSE;
}


// ***************************************************
//		get_Header()
// ***************************************************
STDMETHODIMP CGrid::get_Header(IGridHeader **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if (trgrid != NULL)
	{
		// Make grid header
		CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)pVal);

		double dX = trgrid->getDX();
		double dY = trgrid->getDY();
		(*pVal)->put_dX(dX);
		(*pVal)->put_dY(dY);
		
		VARIANT ndv;
		VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
		ndv.vt = VT_R8;
		ndv.dblVal = trgrid->noDataValue;
		(*pVal)->put_NodataValue(ndv);
		(*pVal)->put_Notes(A2BSTR(""));
		(*pVal)->put_NumberCols(trgrid->getWidth());
		(*pVal)->put_NumberRows(trgrid->getHeight());
		(*pVal)->put_XllCenter(trgrid->getXllCenter());
		(*pVal)->put_YllCenter(trgrid->getYllCenter());
		(*pVal)->put_Projection(trgrid->Projection.AllocSysString());
		
		BSTR cTbl;
		if (trgrid->ColorTable2BSTR(&cTbl))
			(*pVal)->put_ColorTable(cTbl);
		::SysFreeString(cTbl);

		VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		(*pVal)->put_Owner((int*)(void*)trgrid, (int*)NULL, (int*)NULL, (int*)NULL, (int*)NULL);
	}
	else if( dgrid != NULL )
	{	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)pVal);
		(*pVal)->put_dX(dgrid->getHeader().getDx());
		(*pVal)->put_dY(dgrid->getHeader().getDy());
		VARIANT ndv;
		VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
		ndv.vt = VT_R8;
		ndv.dblVal = dgrid->getHeader().getNodataValue();
		(*pVal)->put_NodataValue(ndv);
		(*pVal)->put_Notes(A2BSTR(dgrid->getHeader().getNotes()));
		(*pVal)->put_Projection(A2BSTR(dgrid->getHeader().getProjection()));
		(*pVal)->put_NumberCols(dgrid->getHeader().getNumberCols());
		(*pVal)->put_NumberRows(dgrid->getHeader().getNumberRows());
		(*pVal)->put_XllCenter(dgrid->getHeader().getXllcenter());
		(*pVal)->put_YllCenter(dgrid->getHeader().getYllcenter());
		VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		(*pVal)->put_Owner((int*)NULL, (int*)dgrid, (int*)NULL, (int*)NULL, (int*)NULL);
	}
	else if( fgrid != NULL )
	{	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)pVal);
		(*pVal)->put_dX(fgrid->getHeader().getDx());
		(*pVal)->put_dY(fgrid->getHeader().getDy());
		VARIANT ndv;
		VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
		ndv.vt = VT_R4;
		ndv.fltVal = fgrid->getHeader().getNodataValue();
		(*pVal)->put_NodataValue(ndv);
		(*pVal)->put_Notes(A2BSTR(fgrid->getHeader().getNotes()));
		(*pVal)->put_Projection(A2BSTR(fgrid->getHeader().getProjection()));
		(*pVal)->put_NumberCols(fgrid->getHeader().getNumberCols());
		(*pVal)->put_NumberRows(fgrid->getHeader().getNumberRows());
		(*pVal)->put_XllCenter(fgrid->getHeader().getXllcenter());
		(*pVal)->put_YllCenter(fgrid->getHeader().getYllcenter());
		VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		(*pVal)->put_Owner((int*)NULL, (int*)NULL, (int*)NULL, (int*)NULL, (int*)fgrid);
	}
	else if( lgrid != NULL )
	{	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)pVal);
		(*pVal)->put_dX(lgrid->getHeader().getDx());
		(*pVal)->put_dY(lgrid->getHeader().getDy());
		VARIANT ndv;
		VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
		ndv.vt = VT_I4;
		ndv.lVal = lgrid->getHeader().getNodataValue();
		(*pVal)->put_NodataValue(ndv);
		(*pVal)->put_Notes(A2BSTR(lgrid->getHeader().getNotes()));
		(*pVal)->put_Projection(A2BSTR(lgrid->getHeader().getProjection()));
		(*pVal)->put_NumberCols(lgrid->getHeader().getNumberCols());
		(*pVal)->put_NumberRows(lgrid->getHeader().getNumberRows());
		(*pVal)->put_XllCenter(lgrid->getHeader().getXllcenter());
		(*pVal)->put_YllCenter(lgrid->getHeader().getYllcenter());
		VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		(*pVal)->put_Owner((int*)NULL, (int*)NULL, (int*)NULL, (int*)lgrid, (int*)NULL);
	}
	else if( sgrid != NULL )
	{	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)pVal);
		(*pVal)->put_dX(sgrid->getHeader().getDx());
		(*pVal)->put_dY(sgrid->getHeader().getDy());
		VARIANT ndv;
		VariantInit(&ndv); //added by Rob Cairns 4-Jan-06
		ndv.vt = VT_I2;
		ndv.iVal = sgrid->getHeader().getNodataValue();
		(*pVal)->put_NodataValue(ndv);
		(*pVal)->put_Notes(A2BSTR(sgrid->getHeader().getNotes()));
		(*pVal)->put_Projection(A2BSTR(sgrid->getHeader().getProjection()));
		(*pVal)->put_NumberCols(sgrid->getHeader().getNumberCols());
		(*pVal)->put_NumberRows(sgrid->getHeader().getNumberRows());
		(*pVal)->put_XllCenter(sgrid->getHeader().getXllcenter());
		(*pVal)->put_YllCenter(sgrid->getHeader().getYllcenter());
		VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		(*pVal)->put_Owner((int*)NULL, (int*)NULL, (int*)sgrid, (int*)NULL, (int*)NULL);
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

	if ( trgrid != NULL)
	{
		trgrid->Projection = A2BSTR(projection);
	}
	else if( dgrid != NULL )
	{	
		dHeader hdr = dgrid->getHeader();
		hdr.setProjection(projection);
		dgrid->setHeader(hdr);
	}
	else if( fgrid != NULL )
	{	
		fHeader hdr = fgrid->getHeader();	
		hdr.setProjection(projection);
		fgrid->setHeader(hdr);
	}
	else if( lgrid != NULL )
	{	
		lHeader hdr = lgrid->getHeader();
		hdr.setProjection(projection);
		lgrid->setHeader(hdr);
	}
	else if( sgrid != NULL )
	{	
		sHeader hdr = sgrid->getHeader();
		hdr.setProjection(projection);
		sgrid->setHeader(hdr);
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

	if (trgrid != NULL)
	{
		if (Row < 0 || Row >= trgrid->getHeight() )
		{
			lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
			Vals = NULL;
			*retval = FALSE;
		}

		int ncols = trgrid->getWidth();

		if (!trgrid->isInRam())
		{
			trgrid->GetFloatWindow(Vals, Row, Row, 0, ncols-1, useDouble);
		}
		else
		{
			for (int i = 0; i < ncols; i++)
			{
				if (useDouble) {
					ValsDouble[i] = trgrid->getValue(Row, i);
				}
				else {
					ValsFloat[i] = static_cast<float>(trgrid->getValue(Row, i));
				}
			}
		}
	}
	else if( dgrid != NULL )
	{	
		int ncols = dgrid->getHeader().getNumberCols();

		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				ValsDouble[i] = dgrid->getValue(i, Row);
			}
			else {
				ValsFloat[i] = static_cast<float>(dgrid->getValue(i, Row));
			}
		}
	}
	else if( fgrid != NULL )
	{	
		int ncols = fgrid->getHeader().getNumberCols();

		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				ValsDouble[i] = static_cast<double>(fgrid->getValue(i, Row));
			}
			else {
				ValsFloat[i] = fgrid->getValue(i, Row);
			}
		}
	}
	else if( lgrid != NULL )
	{	int ncols = lgrid->getHeader().getNumberCols();

		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				ValsDouble[i] = static_cast<double>(lgrid->getValue(i, Row));
			}
			else {
				ValsFloat[i] = static_cast<float>(lgrid->getValue(i, Row));
			}
		}
	}
	else if( sgrid != NULL )
	{	int ncols = sgrid->getHeader().getNumberCols();

		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				ValsDouble[i] = static_cast<double>(sgrid->getValue(i, Row));
			}
			else {
				ValsFloat[i] = static_cast<float>(sgrid->getValue(i, Row));
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

	if (trgrid != NULL)
	{
		if (Row < 0 || Row >= trgrid->getHeight() )
		{
			lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
			Vals = NULL;
			*retval = FALSE;
		}

		long ncols = trgrid->getWidth();
		
		if (!trgrid->isInRam())
		{
			trgrid->PutFloatWindow(Vals, Row, Row, 0, ncols-1, useDouble);
		}
		else
		{
			for (long i = 0; i < ncols; i++)
			{
				if (useDouble) {
					trgrid->putValue(Row, i, ValsDouble[i]);
				}
				else
				{
					trgrid->putValue(Row, i, static_cast<double>(ValsFloat[i]));
				}
			}
		}
	}
	else if( dgrid != NULL )
	{	
		int ncols = dgrid->getHeader().getNumberCols();
		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				dgrid->setValue(i, Row, ValsDouble[i]);
			}
			else {
				dgrid->setValue(i, Row, static_cast<double>(ValsFloat[i]));
			}
		}
	}
	else if( fgrid != NULL )
	{	
		int ncols = fgrid->getHeader().getNumberCols();
		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				fgrid->setValue(i, Row, static_cast<float>(ValsDouble[i]));
			}
			else {
				fgrid->setValue(i, Row, ValsFloat[i]);
			}
		}
	}
	else if( lgrid != NULL )
	{	
		int ncols = lgrid->getHeader().getNumberCols();
		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				double val = ValsDouble[i];
				long lval = static_cast<long>(ValsDouble[i]);
				lgrid->setValue(i, Row, static_cast<long>(ValsDouble[i]));
			}
			else {
				lgrid->setValue(i, Row, static_cast<long>(ValsFloat[i]));
			}
		}
	}
	else if( sgrid != NULL )
	{	
		int ncols = sgrid->getHeader().getNumberCols();
		for (int i = 0; i < ncols; i++)
		{
			if (useDouble) {
				sgrid->setValue(i, Row, static_cast<short>(ValsDouble[i]));
			}
			else {
				sgrid->setValue(i, Row, static_cast<short>(ValsFloat[i]));
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

	if (trgrid != NULL)
	{
		if (Column < 0 || Column >= trgrid->getWidth() || Row< 0 || Row >= trgrid->getHeight() )
		{
			lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
			pVal->vt = VT_R8;
			pVal->dblVal = trgrid->noDataValue;
			return S_OK;
		}

		pVal->vt = VT_R8;
		pVal->dblVal = trgrid->getValue(Row, Column);
	}
	else if( dgrid != NULL )
	{	pVal->vt = VT_R8;
		pVal->dblVal = dgrid->getValue( Column, Row );
	}
	else if( fgrid != NULL )
	{	pVal->vt = VT_R4;
		pVal->fltVal = fgrid->getValue( Column, Row );
	}
	else if( lgrid != NULL )
	{	pVal->vt = VT_I4;
		pVal->lVal = lgrid->getValue( Column, Row );
	}
	else if( sgrid != NULL )
	{	pVal->vt = VT_I2;
		pVal->iVal = sgrid->getValue( Column, Row );
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
		if ( trgrid != NULL)
		{
			if (Column < 0 || Column >= trgrid->getWidth() || Row< 0 || Row >= trgrid->getHeight() )
			{
				lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
				return S_FALSE;
			}
			if (trgrid->CanCreate()) // Can Create == Can Write
			{
				trgrid->putValue(Row, Column, Value);
				return S_OK;
			}
			else
			{
				AfxMessageBox("Writing to this format is not allowed.");
				return S_FALSE;
			}
		}
		else if( dgrid != NULL )
			dgrid->setValue( Column, Row, Value );
		else if( fgrid != NULL )
			fgrid->setValue( Column, Row, (float)Value );
		else if( lgrid != NULL )
			lgrid->setValue( Column, Row, (long)Value );
		else if( sgrid != NULL )
			sgrid->setValue( Column, Row, (short)Value );
	}
	return S_OK;
}

// ***************************************************
//		get_InRam()
// ***************************************************
STDMETHODIMP CGrid::get_InRam(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if ( trgrid != NULL)
		*pVal = trgrid->isInRam()?VARIANT_TRUE:VARIANT_FALSE;
	else if( dgrid != NULL )
		*pVal = dgrid->inRam()?VARIANT_TRUE:VARIANT_FALSE;
	else if( fgrid != NULL )
		*pVal = fgrid->inRam()?VARIANT_TRUE:VARIANT_FALSE;
	else if( lgrid != NULL )
		*pVal = lgrid->inRam()?VARIANT_TRUE:VARIANT_FALSE;
	else if( sgrid != NULL )
		*pVal = sgrid->inRam()?VARIANT_TRUE:VARIANT_FALSE;
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

	if (trgrid != NULL)
	{
		pVal->vt = VT_R8;
		pVal->dblVal = trgrid->GetMaximum();
	}
	else if( dgrid != NULL )
	{	pVal->vt = VT_R8;
		pVal->dblVal = dgrid->maximum();
	}
	else if( fgrid != NULL )
	{	pVal->vt = VT_R4;
		pVal->fltVal = fgrid->maximum();
	}
	else if( lgrid != NULL )
	{	pVal->vt = VT_I4;
		pVal->lVal = lgrid->maximum();
	}
	else if( sgrid != NULL )
	{	pVal->vt = VT_I2;
		pVal->iVal = sgrid->maximum();
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
		
	if (trgrid != NULL)
	{
		// GDAL doesn't always exclude nodata values.
		// If nodata == minimum generate by brute force.
		// This code isn't present on getmax -- only appears
		// to be a problem with ESRI tiffs (i.e., nodata is
		// the default large negative)

		double bruteForceResult = trgrid->GetMinimum();
		float f1 = Utility::FloatRound(trgrid->GetMinimum(), 4);
		float f2 = Utility::FloatRound(trgrid->noDataValue, 4);
		float v1;
		float ndv = Utility::FloatRound(trgrid->noDataValue, 4);
		if (FloatsEqual(f1, f2))
		{
			double currentMin = 9999999;
			long w = trgrid->getWidth();
			long h = trgrid->getHeight();
			for (register long i = 0; i < w; i++)
			{
				for (register long j = 0; j < h; j++)
				{
					if (trgrid->getValue(j, i) < currentMin)
					{
						v1 = Utility::FloatRound(trgrid->getValue(j, i), 4);
						if (!FloatsEqual(v1, ndv))
							currentMin = trgrid->getValue(j, i);
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
			pVal->dblVal = trgrid->GetMinimum();
		}
	}
	else if( dgrid != NULL )
	{	pVal->vt = VT_R8;
		pVal->dblVal = dgrid->minimum();
	}
	else if( fgrid != NULL )
	{	pVal->vt = VT_R4;
		pVal->fltVal = fgrid->minimum();
	}
	else if( lgrid != NULL )
	{	pVal->vt = VT_I4;
		pVal->lVal = lgrid->minimum();
	}
	else if( sgrid != NULL )
	{	pVal->vt = VT_I2;
		pVal->iVal = sgrid->minimum();
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

	if ( trgrid != NULL)
		*pVal = trgrid->GetDataType();
	else if( dgrid != NULL )
		*pVal = DoubleDataType;
	else if( fgrid != NULL )
		*pVal = FloatDataType;
	else if( lgrid != NULL )
		*pVal = LongDataType;
	else if( sgrid != NULL )
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
	*pVal = W2BSTR(filename);
	return S_OK;
}

// ***************************************************
//		get_LastErrorCode()
// ***************************************************
STDMETHODIMP CGrid::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = lastErrorCode;

	if( *pVal != tkNO_ERROR )
	{	if ( trgrid != NULL)
			*pVal = 0; // todo -- trgrid ought to keep track of an error code
		else if( dgrid != NULL )
			*pVal = dgrid->LastErrorCode();
		else if( fgrid != NULL )
			*pVal = fgrid->LastErrorCode();
		else if( lgrid != NULL )
			*pVal = lgrid->LastErrorCode();
		else if( sgrid != NULL )
			*pVal = sgrid->LastErrorCode();
	}

	lastErrorCode = tkNO_ERROR;
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
	*pVal = globalCallback;
	if( globalCallback != NULL )
		globalCallback->AddRef();
	return S_OK;
}
STDMETHODIMP CGrid::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&globalCallback);
	return S_OK;
}

// ***************************************************
//		Key()
// ***************************************************
STDMETHODIMP CGrid::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(key);
	return S_OK;
}
STDMETHODIMP CGrid::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(key);
	key = OLE2BSTR(newVal);
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
		GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&proj);
		if (proj)
		{
			VARIANT_BOOL vb;
			proj->ReadFromFile(W2BSTR(prjFilename), &vb);
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
		dgrid = new dGrid();
		result = dgrid->open( W2A(filename), inRam, (GRID_TYPE)FileType, gridCOMCALLBACK );		// TODO: use Unicode
		if (!result)
		{	
			ErrorMessage(dgrid->LastErrorCode());
			delete dgrid;
			dgrid = NULL;
		}
	}
	else if( DataType == FloatDataType )
	{	
		fgrid = new fGrid();
		result = fgrid->open( W2A(filename), inRam, (GRID_TYPE)FileType, gridCOMCALLBACK );		// TODO: use Unicode
		if (!result)
		{
			ErrorMessage(fgrid->LastErrorCode());
			delete fgrid;
			fgrid = NULL;
		}
	}
	else if( DataType == LongDataType )
	{	
		lgrid = new lGrid();
		result = lgrid->open( W2A(filename), inRam, (GRID_TYPE)FileType, gridCOMCALLBACK );		// TODO: use Unicode
		if (!result)
		{	
			ErrorMessage(lgrid->LastErrorCode());
			delete lgrid;
			lgrid = NULL;
		}
	}
	else if( DataType == ShortDataType )
	{	
		sgrid = new sGrid();
		result = sgrid->open( W2A(filename), inRam, (GRID_TYPE)FileType, gridCOMCALLBACK) ;		// TODO: use Unicode
		if (!result)
		{	
			ErrorMessage(sgrid->LastErrorCode());
			delete sgrid;
			sgrid = NULL;
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
		dgrid = new dGrid();
		ifstream fin(filename);
		dgrid->asciiReadHeader(fin);
		dHeader dhd = dgrid->getHeader();
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
		delete dgrid;
		dgrid = NULL;
	}
	else if (DataType == FloatDataType)
	{
		fgrid = new fGrid();
		ifstream fin(filename);
		fgrid->asciiReadHeader(fin);
		fHeader fhd = fgrid->getHeader();
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
		delete fgrid;
		fgrid = NULL;
	}
	else if (DataType == ShortDataType)
	{
		sgrid = new sGrid();
		ifstream fin(filename);
		sgrid->asciiReadHeader(fin);
		sHeader shd = sgrid->getHeader();
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
		delete sgrid;
		sgrid = NULL;
	}
	else if (DataType == LongDataType)
	{
		lgrid = new lGrid();
		ifstream fin(filename);
		lgrid->asciiReadHeader(fin);
		lHeader lhd = lgrid->getHeader();
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
		delete lgrid;
		lgrid = NULL;
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

	if (Filename == A2BSTR(""))
	{
		ErrorMessage(tkINVALID_FILE);
		return S_FALSE;
	}

	filename = OLE2BSTR(Filename);
	ICallback * tmpCallback = globalCallback;
	if( cBack != NULL )	globalCallback = cBack;
	bool inRam = InRam == VARIANT_TRUE ? true: false;
	
	// Are we handling ECWP imagery? 
	CStringW ecwpTest = filename;
	if (ecwpTest.MakeLower().Left(4) == L"ecwp") 
	{
		// e.g. ecwp://imagery.oregonexplorer.info/ecwimages/2005orthoimagery.ecw
		trgrid = new tkGridRaster();
		*retval = trgrid->LoadRaster(filename, true, FileType)?VARIANT_TRUE:VARIANT_FALSE;
		return S_OK;
	}
	
	// check or sta.adf file
	bool isAuxHeader = OpenAuxHeader(filename);

	activeGridObject = this;
	bool opened = false;

	// get extension
	CString extension = W2A(filename.Right( filename.GetLength() - 1 - filename.ReverseFind('.') ));
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
	GetDataTypeFromBinaryGrid(DataType, FileType, filename);

	// If opening an existing ASCII grid, this can be done with GDAL. GDAL doesn't
	// support creation of ASCII grids however, so this will be done with our object
	// for now. (This can be worked around using a virtual raster, but this will do for now)
	if ( FileType == Esri || FileType == Flt || FileType == Ecw || FileType == Bil || FileType == MrSid || 
		FileType == PAux || FileType == PCIDsk || FileType == DTed ||
		(FileType == UseExtension && (((A2W(extension) == filename) && extension != "") 
		|| extension.CompareNoCase("adf") == 0 || extension.CompareNoCase("dem") == 0
		|| extension.CompareNoCase("ecw") == 0 || extension.CompareNoCase("bil") == 0 
		|| extension.CompareNoCase("sid") == 0 || extension.CompareNoCase("aux") == 0 
		|| extension.CompareNoCase("pix") == 0 || extension.CompareNoCase("dhm") == 0 
		|| extension.CompareNoCase("dt0") == 0 || extension.CompareNoCase("dt1") == 0)))
	{
		trgrid = new tkGridRaster();
		opened = trgrid->LoadRaster(filename, inRam, FileType);
	}
	else if ( forcingGDALUse || FileType == GeoTiff || (FileType == UseExtension && extension.CompareNoCase("tif") == 0))
	{
		trgrid = new tkGridRaster();
		opened = trgrid->LoadRaster(filename, inRam, FileType);
	}
	else if(OpenCustomGrid(DataType, inRam, FileType))
	{
		opened = true;
	}
	else
	{
		trgrid = new tkGridRaster();
		opened = trgrid->LoadRaster(filename, inRam, FileType);
	}

	if (opened)
	{
		SaveProjectionAsWkt();
		globalCallback = tmpCallback;
	}
	else
	{
		VARIANT_BOOL vb;
		this->Close(&vb);
		filename = "";
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

	ICallback * tmpCallback = globalCallback;
	if( cBack != NULL )
		globalCallback = cBack;

	Close(retval);	

	VARIANT_BOOL bInRam = VARIANT_FALSE;
	bool boolInRam = false;
	if(InRam == VARIANT_TRUE)
	{
		boolInRam = true;
		bInRam = VARIANT_TRUE;
	}
	
	activeGridObject = this;

	filename = OLE2BSTR(Filename);
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
			trgrid = new tkGridRaster();

			GridFileType newFileType = FileType;

			ResolveFileType(newFileType, extension);

			if (trgrid->CanCreate(newFileType))
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
				BSTR projection;
				Header->get_Projection(&projection);
				long ncols;
				Header->get_NumberCols(&ncols);
				long nrows;
				Header->get_NumberRows(&nrows);
				double xllcenter;
				Header->get_XllCenter(&xllcenter);
				double yllcenter;
				Header->get_YllCenter(&yllcenter);
				
				BSTR cTbl;
				Header->get_ColorTable(&cTbl);
				trgrid->BSTR2ColorTable(cTbl);
				::SysFreeString(cTbl);

				return trgrid->CreateNew(W2A(Filename), newFileType, dx, dy, xllcenter, yllcenter, 
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
			dgrid = new dGrid();
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
			BSTR notes;
			Header->get_Notes(&notes);
			dhdr.setNotes(OLE2CA(notes));
			BSTR projection;
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

			if( dgrid->initialize(OLE2CA(Filename),dhdr,value,boolInRam) == true )
			{	
				filename = OLE2W(Filename);
				*retval = VARIANT_TRUE;
			}
			else
			{	
				dgrid = NULL;
				*retval = VARIANT_FALSE;
				ErrorMessage(dgrid->LastErrorCode());
			}
			
			VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
		    ::SysFreeString(notes);  //Lailin Chen 12/20/2005 
			::SysFreeString(projection);  //Lailin Chen 12/20/2005 
		}
		else if( DataType == FloatDataType )
		{	fgrid = new fGrid();
			
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
			BSTR notes;
			Header->get_Notes(&notes);
			fhdr.setNotes(OLE2CA(notes));
			BSTR projection;
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

			if( fgrid->initialize(OLE2CA(Filename),fhdr,(float)value,boolInRam) == true )
			{	
				filename = OLE2W(Filename);
				*retval = VARIANT_TRUE;
			}
			else
			{	
				fgrid = NULL;
				*retval = VARIANT_FALSE;
				ErrorMessage(fgrid->LastErrorCode());
			}
			VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
			::SysFreeString(notes);  //Lailin Chen 12/20/2005 
			::SysFreeString(projection);  //Lailin Chen 12/20/2005 
		}
		else if( DataType == LongDataType )
		{	lgrid = new lGrid();

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
			BSTR notes;
			Header->get_Notes(&notes);
			lhdr.setNotes(OLE2CA(notes));
			BSTR projection;
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

			if( lgrid->initialize(OLE2CA(Filename),lhdr,(long)value,boolInRam) == true )
			{	
				filename = OLE2W(Filename);
				*retval = VARIANT_TRUE;
			}
			else
			{	
				lgrid = NULL;
				*retval = VARIANT_FALSE;
				ErrorMessage(lgrid->LastErrorCode());
			}
			VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
			::SysFreeString(notes);  //Lailin Chen 12/20/2005 
			::SysFreeString(projection);  //Lailin Chen 12/20/2005 
		}
		else if( DataType == ShortDataType )
		{	sgrid = new sGrid();

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
			BSTR notes;
			Header->get_Notes(&notes);
			shdr.setNotes(OLE2CA(notes));
			BSTR projection;
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

			if( sgrid->initialize(OLE2CA(Filename),shdr,(short)value,boolInRam) == true )
			{	
				filename = OLE2W(Filename);
				*retval = VARIANT_TRUE;
			}
			else
			{	
				sgrid = NULL;
				*retval = VARIANT_FALSE;
				ErrorMessage(sgrid->LastErrorCode());
			}
			VariantClear(&ndv); //added by Rob Cairns 4-Jan-06
			::SysFreeString(notes);  //Lailin Chen 12/20/2005 
			::SysFreeString(projection);  //Lailin Chen 12/20/2005 
		}
		else
		{	ErrorMessage(tkINVALID_DATA_TYPE);
		}
	}

	globalCallback = tmpCallback;

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
	if ( trgrid != NULL)
		*retval = trgrid->Close()?VARIANT_TRUE:VARIANT_FALSE;
	trgrid = NULL;
	if( dgrid != NULL )
		*retval = dgrid->close()?VARIANT_TRUE:VARIANT_FALSE;
	dgrid = NULL;
	if( fgrid != NULL )
		*retval = fgrid->close()?VARIANT_TRUE:VARIANT_FALSE;
	fgrid = NULL;
	if( lgrid != NULL )
		*retval = lgrid->close()?VARIANT_TRUE:VARIANT_FALSE;	
	lgrid = NULL;
	if( sgrid != NULL )
		*retval = sgrid->close()?VARIANT_TRUE:VARIANT_FALSE;
	sgrid = NULL;
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
		Filename = OLE2BSTR(filename);

	ICallback * tmpCallback = globalCallback;
	if( cBack != NULL )
		globalCallback = cBack;
	activeGridObject = this;

	CString Filen(W2A(Filename));
	CString extension = Filen.Right( Filen.GetLength() - 1 - Filen.ReverseFind('.') );
	
	ResolveFileType(FileType, extension);

	if (extension == Filen && Filen != "")
		FileType = Esri;

	CString origFilename(W2A(this->filename));
	CString origExtension = origFilename.Right( origFilename.GetLength() - 1 - origFilename.ReverseFind('.') );
	GridFileType origFileType;
	ResolveFileType(origFileType, origExtension);

	if (origFilename == origExtension && origFilename != "")
		origFileType = Esri;

	// (special conversion case) If they have a GDAL format and wish to save it to BGD, speed it up by writing
	// directly from the tkGridRaster class. (Generic conversion can do anything, but very slowly.)
	if (trgrid != NULL && FileType == Binary)
	{
		*retval = trgrid->SaveToBGD(OLE2A(Filename), gridCOMCALLBACK) ? VARIANT_TRUE : VARIANT_FALSE; //? S_OK : S_FALSE;
	}
	// (special conversion case) If they have a BGD and wish to save it to a GDAL format, speed it up by writing
	// directly from the tkGridRaster class. (Generic conversion can do anything, but very slowly.)
	else if (origFileType == Binary && trgrid == NULL && 
		(dgrid != NULL || fgrid != NULL || sgrid != NULL || lgrid != NULL) &&
		(FileType == Ecw || FileType == Bil
		|| FileType == Esri || FileType == Flt || FileType == MrSid || FileType == PAux
		|| FileType == PCIDsk || FileType == DTed || FileType == GeoTiff))
	{
		tkGridRaster * tempGrid = new tkGridRaster();
		DATA_TYPE bgdDataType = SHORT_TYPE;

		// Read header info
		bool bRetval = tempGrid->ReadBGDHeader(OLE2A(this->filename), bgdDataType);
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
		bRetval = tempGrid->ReadFromBGD(OLE2A(this->filename), gridCOMCALLBACK);
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
	else if (trgrid != NULL && (FileType != Ecw && FileType != Bil
		&& FileType != MrSid && FileType != PAux
		&& FileType != Esri && FileType != Flt && FileType != PCIDsk && FileType != DTed && FileType != GeoTiff)
		||
		(trgrid == NULL && (FileType == Ecw || FileType == Bil
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
	else if (trgrid != NULL)
	{
		if( trgrid->Save(W2A(Filename), FileType) )
		{	
			filename = Filename;
			*retval = VARIANT_TRUE;
		}
		else
		{	
			ErrorMessage(tkFAILED_TO_SAVE_GRID);
		}
	}
	else if( dgrid != NULL )
	{	
		if( dgrid->save(OLE2CA(Filename), (GRID_TYPE)FileType, gridCOMCALLBACK) == true )
		{	
			filename = Filename;
			*retval = VARIANT_TRUE;
		}
		else
		{	
			ErrorMessage(dgrid->LastErrorCode());
		}
	}
	else if( fgrid != NULL )
	{	
		if( fgrid->save(OLE2CA(Filename), (GRID_TYPE)FileType, gridCOMCALLBACK) == true )
		{	
			filename = Filename;
			*retval = VARIANT_TRUE;
		}
		else
		{	
			ErrorMessage(fgrid->LastErrorCode());
		}
	}
	else if( lgrid != NULL )
	{	
		if( lgrid->save(OLE2CA(Filename), (GRID_TYPE)FileType, gridCOMCALLBACK) == true )
		{	
			filename = Filename;
			*retval = VARIANT_TRUE;
		}
		else
		{	
			ErrorMessage(lgrid->LastErrorCode());
		}
	}
	else if( sgrid != NULL )
	{	
		if( sgrid->save(OLE2CA(Filename), (GRID_TYPE)FileType, gridCOMCALLBACK) == true )
		{	
			filename = Filename;
			*retval = VARIANT_TRUE;
		}
		else
		{	
			ErrorMessage(sgrid->LastErrorCode());
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

		if (strcmp(W2A(bstrProj), "") != 0)
		{
			SaveProjection(W2A(bstrProj));
		}
		globalCallback = tmpCallback;
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
		if ( trgrid != NULL)
			trgrid->clear(value);
		if( dgrid != NULL )
			dgrid->clear(value);
		else if( fgrid != NULL )
			fgrid->clear((float)value);
		else if( lgrid != NULL )
			lgrid->clear((long)value);
		else if( sgrid != NULL )
			sgrid->clear((short)value);
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

	if ( trgrid != NULL )
		trgrid->ProjToCell( x, y, *Column, *Row );
	else if( dgrid != NULL )
		dgrid->ProjToCell( x, y, *Column, *Row );
	else if( fgrid != NULL )
		fgrid->ProjToCell( x, y, *Column, *Row );
	else if( lgrid != NULL )
		lgrid->ProjToCell( x, y, *Column, *Row );
	else if( sgrid != NULL )
		sgrid->ProjToCell( x, y, *Column, *Row );
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

	if (trgrid != NULL)
		trgrid->CellToProj(Column, Row, *x, *y);
	else if( dgrid != NULL )
		dgrid->CellToProj(Column, Row, *x, *y);
	else if( fgrid != NULL )
		fgrid->CellToProj(Column, Row, *x, *y);
	else if( lgrid != NULL )
		lgrid->CellToProj(Column, Row, *x, *y);
	else if( sgrid != NULL )
		sgrid->CellToProj(Column, Row, *x, *y);
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
	
	if (trgrid != NULL)
	{
		if (StartRow < 0 || StartRow >= trgrid->getHeight() || EndRow < 0 || EndRow >= trgrid->getHeight() )
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			Vals = NULL;
			ValsDouble = NULL;
			*retval = FALSE;
		}

		if (!trgrid->GetFloatWindow(Vals, StartRow, EndRow, StartCol, EndCol, useDouble))
		{
			*retval = VARIANT_FALSE;
		}
	}
	else if( dgrid != NULL )
	{	
		long position = 0;
		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					ValsDouble[position++] = dgrid->getValue(i, j);
				}
				else
				{
					ValsFloat[position++] = static_cast<float>(dgrid->getValue(i, j));
				}
			}
		}
	}
	else if( fgrid != NULL )
	{	
		long position = 0;
		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					ValsDouble[position++] = static_cast<double>(fgrid->getValue(i, j));
				}
				else
				{
					ValsFloat[position++] = fgrid->getValue(i, j);
				}
			}
		}
	}
	else if( lgrid != NULL )
	{
		long position = 0;
		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					ValsDouble[position++] = static_cast<double>(lgrid->getValue(i, j));
				}
				else
				{
					ValsFloat[position++] = static_cast<float>(lgrid->getValue(i, j));
				}
			}
		}
	}
	else if( sgrid != NULL )
	{
		long position = 0;
		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					ValsDouble[position++] = static_cast<double>(sgrid->getValue(i, j));
				}
				else
				{
					ValsFloat[position++] = static_cast<float>(sgrid->getValue(i, j));
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
	if (trgrid != NULL)
	{
 		if (StartRow < 0 || StartRow >= trgrid->getHeight() || EndRow < 0 || EndRow >= trgrid->getHeight())
		{
			lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
			Vals = NULL;
			*retval = FALSE;
		}

		if (!trgrid->PutFloatWindow(Vals, StartRow, EndRow, StartCol, EndCol, useDouble))
		{
			*retval = FALSE;
		}
	}
	else if( dgrid != NULL )
	{	
		int ncols = dgrid->getHeader().getNumberCols();
		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					dgrid->setValue(i, j, ValsDouble[position++]);
				}
				else
				{
					dgrid->setValue(i, j, static_cast<float>(ValsFloat[position++]));
				}
			}
		}
	}
	else if( fgrid != NULL )
	{	
		int ncols = fgrid->getHeader().getNumberCols();
		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					fgrid->setValue(i, j, static_cast<float>(ValsDouble[position++]));
				}
				else
				{
					fgrid->setValue(i, j, ValsFloat[position++]);
				}
			}
		}
	}
	else if( lgrid != NULL )
	{
		int ncols = lgrid->getHeader().getNumberCols();
		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					lgrid->setValue(i, j, static_cast<long>(ValsDouble[position++]));
				}
				else
				{
					lgrid->setValue(i, j, static_cast<long>(ValsFloat[position++]));
				}
			}
		}
	}
	else if( sgrid != NULL )
	{
		int ncols = sgrid->getHeader().getNumberCols();
		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				if (useDouble)
				{
					sgrid->setValue(i, j, static_cast<short>(ValsDouble[position++]));
				}
				else
				{
					sgrid->setValue(i, j, static_cast<short>(ValsFloat[position++]));
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
	if (trgrid != NULL)
	{
		*retVal = trgrid->getNumBands();
	}
	else if (dgrid || sgrid || fgrid || lgrid)
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
	*retVal = trgrid != NULL ? trgrid->GetActiveBandIndex() : 1;
	return S_OK;
}

// **************************************************************
//		OpenBand()
// **************************************************************
STDMETHODIMP CGrid::OpenBand(int bandIndex, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (trgrid != NULL)		 // it works for GDAL-rooted grids
	{
		if (bandIndex < 1 || bandIndex > trgrid->getNumBands())
		{
			this->ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
		else
		{
			if (trgrid->OpenBand(bandIndex))
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
	lastErrorCode = ErrorCode;
	Utility::DisplayErrorMsg(globalCallback, key, ErrorMsg(lastErrorCode));
}

// ****************************************************************
//						get_SourceType()						         
// ****************************************************************
STDMETHODIMP CGrid::get_SourceType (tkGridSourceType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())		
	if (trgrid)
	{
		*retVal = tkGridSourceType::gstGDALBased;
	}
	else if (dgrid || sgrid || fgrid || lgrid)
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
	return trgrid ? trgrid->IsRgb() : false;
}

// ****************************************************************
//						OpenAsImage()						         
// ****************************************************************
STDMETHODIMP CGrid::OpenAsImage(IGridColorScheme* scheme, tkGridProxyMode proxyMode, ICallback* cBack, IImage** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	
	if (!globalCallback && cBack)
		Utility::put_ComReference(cBack, (IDispatch**)&globalCallback, false);

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
		OpenAsDirectImage(scheme, globalCallback, retVal);
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
				if (m_globalSettings.saveGridColorSchemeToFile) {
					scheme->WriteToFile(W2BSTR(legendName), W2BSTR(GetFilename()), bandIndex, &vb);
				}
			}

			// let's build overviews for direct grid
			GdalHelper::BuildOverviewsIfNeeded(gridName, true, globalCallback);
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
		GetUtils()->CreateInstance(tkInterface::idImage, (IDispatch**)&iimg);
		
		iimg->Open(OLE2BSTR(this->GetProxyName()), ImageType::USE_FILE_EXTENSION, VARIANT_FALSE, globalCallback, &vb);
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
	*retVal = preferedDisplayMode;
	return S_OK;
}
STDMETHODIMP CGrid::put_PreferedDisplayMode(tkGridProxyMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	preferedDisplayMode = newVal;
	return S_OK;
}

// ****************************************************************
//			get_CanDisplayWithoutProxy()						         
// ****************************************************************
//STDMETHODIMP CGrid::get_CanDisplayWithoutProxy(tkCanDisplayGridWoProxy* retVal)
//{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState())
//	CStringW gridName = GetFilename().MakeLower();
//
//	// ignore formats, it seems that almost any grid can be displayed directly, in some way or another
//	//Utility::EndsWith(gridName, ".tif") || Utility::EndsWith(gridName, ".tiff") || 
//	//Utility::EndsWith(gridName, ".img") || Utility::EndsWith(gridName, ".bil")
//
//	bool gdalFormat = GdalHelper::CanOpenWithGdal(GetFilename());
//
//	if (gdalFormat)
//	{
//		if (IsRgb())
//		{
//			*retVal = cdwYes;
//		}
//		else
//		{
//			if (m_globalSettings.MaxDirectGridSizeMb <= 0)	{
//				// there is no limitation
//				*retVal = cdwYes;
//			}
//			else
//			{
//				long size = Utility::get_FileSize(gridName) / (0x1 << 20);
//				*retVal = size > m_globalSettings.MaxDirectGridSizeMb ? cdwSizeLimitation : cdwYes;
//			}
//		}
//	}
//	else
//	{
//		*retVal = cdwUnsupportedFormat;
//	}
//	return S_OK;
//}

// ****************************************************************
//			CreateImageProxy()						         
// ****************************************************************
STDMETHODIMP CGrid::CreateImageProxy(IGridColorScheme* colorScheme, IImage** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	VARIANT_BOOL vb;
	RemoveImageProxy(&vb);
	GetUtils()->GridToImage2(this, colorScheme, m_globalSettings.gridProxyFormat, VARIANT_FALSE, this->globalCallback, retVal);
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
			scheme->ReadFromFile(OLE2BSTR(legendName), A2BSTR(""), &vb);
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
				scheme->ReadFromFile(OLE2BSTR(legendName), A2BSTR(""), &vb);
			}
		}
	}
	
	// from Gdal color table
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