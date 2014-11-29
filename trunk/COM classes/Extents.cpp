//********************************************************************************************************
//File name: Extents.cpp
//Description:  Implementation of CExtents.
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
#include "Extents.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CExtents
STDMETHODIMP CExtents::SetBounds(double xMin, double yMin, double zMin, double xMax, double yMax, double zMax)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	_xmin = xMin;
	_max = xMax;
	_ymin = yMin;
	_ymax = yMax;
	_zmin = zMin;
	_zmax = zMax;

	double tmp;
	if( _xmin > _max )
	{	tmp = _max;
		_max = _xmin;
		_xmin = tmp;
	}
	if( _ymin > _ymax )
	{	tmp = _ymax;
		_ymax = _ymin;
		_ymin = tmp;
	}
	if( _zmin > _zmax )
	{	tmp = _zmax;
	_zmax = _zmin;
		_zmin = tmp;
	}

	return S_OK;
}

STDMETHODIMP CExtents::GetBounds(double * xMin, double * yMin, double * zMin, double * xMax, double * yMax, double * zMax)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*xMin = _xmin;
	*xMax = _max;
	*yMin = _ymin;
	*yMax = _ymax;
	*zMin = _zmin;
	*zMax = _zmax;

	return S_OK;
}


STDMETHODIMP CExtents::get_xMin(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _xmin;
	return S_OK;
}

STDMETHODIMP CExtents::get_xMax(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _max;
	return S_OK;
}

STDMETHODIMP CExtents::get_yMin(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _ymin;
	return S_OK;
}

STDMETHODIMP CExtents::get_yMax(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _ymax;
	return S_OK;
}

STDMETHODIMP CExtents::get_zMin(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _zmin;
	return S_OK;
}

STDMETHODIMP CExtents::get_zMax(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _zmax;
	return S_OK;
}

STDMETHODIMP CExtents::get_mMin(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _mmin;

	return S_OK;
}

STDMETHODIMP CExtents::get_mMax(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _mmax;

	return S_OK;
}

STDMETHODIMP CExtents::GetMeasureBounds(double *mMin, double *mMax)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*mMin = _mmin;
	*mMax = _mmax;

	return S_OK;
}

STDMETHODIMP CExtents::SetMeasureBounds(double mMin, double mMax)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	_mmin = mMin;
	_mmax = mMax;

	double tmp;
	if( _mmin > _mmax )
	{	tmp = _mmax;
		_mmax = _mmin;
		_mmin = tmp;
	}

	return S_OK;
}

//*********************************************************************
//*	 Extents2Shape()				              
//*********************************************************************
// Creates polygon shape which is equivalent to the input extents
// return shape pointer on success, or NULL otherwise
 
STDMETHODIMP CExtents::ToShape(IShape** retVal)
{
	IShape* shp = NULL;
	long PartIndex = 0;
	VARIANT_BOOL vbretval;
	
	CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
	shp->put_ShapeType(SHP_POLYGON);
	shp->InsertPart(0, &PartIndex, &vbretval);
	
	IPoint* pnt = NULL;
	
	for (long i = 0; i<=4; i++)
	{
		m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);

		if (i == 0 || i ==4)
		{	
			pnt->put_X(this->_xmin);
			pnt->put_Y(this->_ymin);
		}
		else if (i ==1)
		{
			pnt->put_X(this->_xmin);
			pnt->put_Y(this->_ymax);
			
		}
		else if (i ==2)
		{
			pnt->put_X(this->_max);
			pnt->put_Y(this->_ymax);
		}
		else if (i ==3)
		{
			pnt->put_X(this->_max);
			pnt->put_Y(this->_ymin);
		}

		shp->InsertPoint(pnt, &i, &vbretval);
		pnt->Release();
	}

	*retVal = shp;
	return S_OK;
}

STDMETHODIMP CExtents::Disjoint(IExtents* ext, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_TRUE;
	if (ext) {
		double xMin, yMin, zMin, xMax, yMax, zMax;
		ext->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		*retVal = (this->_max < xMin || this->_xmin > xMax || this->_ymax < yMax || this->_ymin > yMax);
	}
	return S_OK;
}

STDMETHODIMP CExtents::Union(IExtents* ext)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (ext) {
		double xMin, yMin, zMin, xMax, yMax, zMax;
		ext->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		this->_xmin = MIN(xMin, this->_xmin);
		this->_ymin = MIN(yMin, this->_ymin);
		this->_zmin = MIN(zMin, this->_zmin);
		this->_max = MAX(xMax, this->_max);
		this->_ymax = MAX(yMax, this->_ymax);
		this->_zmax = MAX(zMax, this->_zmax);
	}
	return S_OK;
}

STDMETHODIMP CExtents::GetIntersection(IExtents* ext, IExtents** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	if (ext) {
		double xMin, yMin, zMin, xMax, yMax, zMax;
		ext->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		xMin = MAX(xMin, this->_xmin);
		yMin = MAX(yMin, this->_ymin);
		zMin = MAX(zMin, this->_zmin);
		xMax = MIN(xMax, this->_max);
		yMax = MIN(yMax, this->_ymax);
		zMax = MIN(zMax, this->_zmax);
		
		if (xMin <= xMax && yMin <= yMax && zMin <= zMax)
		{
			CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)retVal);
			(*retVal)->SetBounds(xMin, yMin, zMin, xMax, yMax, zMax);
		}
	}
	return S_OK;
}

STDMETHODIMP CExtents::Intersects(IExtents* ext, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	if (ext) {
		double xMin, yMin, zMin, xMax, yMax, zMax;
		ext->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		xMin = MAX(xMin, this->_xmin);
		yMin = MAX(yMin, this->_ymin);
		zMin = MAX(zMin, this->_zmin);
		xMax = MIN(xMax, this->_max);
		yMax = MIN(yMax, this->_ymax);
		zMax = MIN(zMax, this->_zmax);
		*retVal = (xMin <= xMax && yMin <= yMax && zMin <= zMax) ? VARIANT_TRUE : VARIANT_FALSE;
	}
	return S_OK;
}

// ******************************************************
//		ToDebugString()
// ******************************************************
STDMETHODIMP CExtents::ToDebugString(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CString s;
	s.Format("xMin=%f; xMax=%f; yMin=%f; yMax=%f", _xmin, _max, _ymin, _ymax);
	USES_CONVERSION;
	*retVal = A2BSTR(s);
	return S_OK;
}

// ******************************************************
//		PointIsWithin()
// ******************************************************
STDMETHODIMP CExtents::PointIsWithin(double x, double y, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal =  x >= _xmin && x <= _max && y >= _ymin && y <= _ymax ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

// ******************************************************
//		GetCenter()
// ******************************************************
STDMETHODIMP CExtents::get_Center(IPoint** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	GetUtils()->CreateInstance(idPoint, (IDispatch**)retVal);
	(*retVal)->put_X((_xmin + _max) / 2);
	(*retVal)->put_Y((_ymin + _ymax) / 2);
	return S_OK;
}

// ******************************************************
//		MoveTo()
// ******************************************************
STDMETHODIMP CExtents::MoveTo(double x, double y)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	double dx = _max - _xmin;
	double dy = _ymax - _ymin;
	_xmin = x - dx / 2.0;
	_max = x + dx / 2.0;
	_ymin = y - dy / 2.0;
	_ymax = y + dy / 2.0;
	return S_OK;
}


