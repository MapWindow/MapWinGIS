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

	xmin = xMin;
	xmax = xMax;
	ymin = yMin;
	ymax = yMax;
	zmin = zMin;
	zmax = zMax;

	double tmp;
	if( xmin > xmax )
	{	tmp = xmax;
		xmax = xmin;
		xmin = tmp;
	}
	if( ymin > ymax )
	{	tmp = ymax;
		ymax = ymin;
		ymin = tmp;
	}
	if( zmin > zmax )
	{	tmp = zmax;
	zmax = zmin;
		zmin = tmp;
	}

	return S_OK;
}

STDMETHODIMP CExtents::GetBounds(double * xMin, double * yMin, double * zMin, double * xMax, double * yMax, double * zMax)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*xMin = xmin;
	*xMax = xmax;
	*yMin = ymin;
	*yMax = ymax;
	*zMin = zmin;
	*zMax = zmax;

	return S_OK;
}


STDMETHODIMP CExtents::get_xMin(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = xmin;
	return S_OK;
}

STDMETHODIMP CExtents::get_xMax(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = xmax;
	return S_OK;
}

STDMETHODIMP CExtents::get_yMin(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = ymin;
	return S_OK;
}

STDMETHODIMP CExtents::get_yMax(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = ymax;
	return S_OK;
}

STDMETHODIMP CExtents::get_zMin(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = zmin;
	return S_OK;
}

STDMETHODIMP CExtents::get_zMax(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = zmax;
	return S_OK;
}

STDMETHODIMP CExtents::get_mMin(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = mmin;

	return S_OK;
}

STDMETHODIMP CExtents::get_mMax(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = mmax;

	return S_OK;
}

STDMETHODIMP CExtents::GetMeasureBounds(double *mMin, double *mMax)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*mMin = mmin;
	*mMax = mmax;

	return S_OK;
}

STDMETHODIMP CExtents::SetMeasureBounds(double mMin, double mMax)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	mmin = mMin;
	mmax = mMax;

	double tmp;
	if( mmin > mmax )
	{	tmp = mmax;
		mmax = mmin;
		mmin = tmp;
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
			pnt->put_X(this->xmin);
			pnt->put_Y(this->ymin);
		}
		else if (i ==1)
		{
			pnt->put_X(this->xmin);
			pnt->put_Y(this->ymax);
			
		}
		else if (i ==2)
		{
			pnt->put_X(this->xmax);
			pnt->put_Y(this->ymax);
		}
		else if (i ==3)
		{
			pnt->put_X(this->xmax);
			pnt->put_Y(this->ymin);
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
		*retVal = (this->xmax < xMin || this->xmin > xMax || this->ymax < yMax || this->ymin > yMax);
	}
	return S_OK;
}

STDMETHODIMP CExtents::Union(IExtents* ext)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (ext) {
		double xMin, yMin, zMin, xMax, yMax, zMax;
		ext->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		this->xmin = MIN(xMin, this->xmin);
		this->ymin = MIN(yMin, this->ymin);
		this->zmin = MIN(zMin, this->zmin);
		this->xmax = MAX(xMax, this->xmax);
		this->ymax = MAX(yMax, this->ymax);
		this->zmax = MAX(zMax, this->zmax);
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
		xMin = MAX(xMin, this->xmin);
		yMin = MAX(yMin, this->ymin);
		zMin = MAX(zMin, this->zmin);
		xMax = MIN(xMax, this->xmax);
		yMax = MIN(yMax, this->ymax);
		zMax = MIN(zMax, this->zmax);
		
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
		xMin = MAX(xMin, this->xmin);
		yMin = MAX(yMin, this->ymin);
		zMin = MAX(zMin, this->zmin);
		xMax = MIN(xMax, this->xmax);
		yMax = MIN(yMax, this->ymax);
		zMax = MIN(zMax, this->zmax);
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
	s.Format("xMin=%f; xMax=%f; yMin=%f; yMax=%f", xmin, xmax, ymin, ymax);
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
	*retVal =  x >= xmin && x <= xmax && y >= ymin && y <= ymax ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

// ******************************************************
//		GetCenter()
// ******************************************************
STDMETHODIMP CExtents::get_Center(IPoint** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	GetUtils()->CreateInstance(idPoint, (IDispatch**)retVal);
	(*retVal)->put_X((xmin + xmax) / 2);
	(*retVal)->put_Y((ymin + ymax) / 2);
	return S_OK;
}

// ******************************************************
//		MoveTo()
// ******************************************************
STDMETHODIMP CExtents::MoveTo(double x, double y)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	double dx = xmax - xmin;
	double dy = ymax - ymin;
	xmin = x - dx / 2.0;
	xmax = x + dx / 2.0;
	ymin = y - dy / 2.0;
	ymax = y + dy / 2.0;
	return S_OK;
}


