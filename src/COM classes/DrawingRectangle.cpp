// DrawingRectangle.cpp : Implementation of CDrawingRectangle

#include "stdafx.h"
#include "DrawingRectangle.h"

// *****************************************************************
//		X
// *****************************************************************
STDMETHODIMP CDrawingRectangle::get_X(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _x;
	return S_OK;
}
STDMETHODIMP CDrawingRectangle::put_X(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_x = newVal;
	return S_OK;
}

// *****************************************************************
//		Y
// *****************************************************************
STDMETHODIMP CDrawingRectangle::get_Y(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _y;
	return S_OK;
}
STDMETHODIMP CDrawingRectangle::put_Y(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_y = newVal;
	return S_OK;
}

// *****************************************************************
//		Width
// *****************************************************************
STDMETHODIMP CDrawingRectangle::get_Width(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _width;
	return S_OK;
}
STDMETHODIMP CDrawingRectangle::put_Width(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal >= 0.0)
	{	
		_width = newVal;
	}
	return S_OK;
}

// *****************************************************************
//		Height
// *****************************************************************
STDMETHODIMP CDrawingRectangle::get_Height(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _height;
	return S_OK;
}
STDMETHODIMP CDrawingRectangle::put_Height(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal >= 0.0)
	{
		_height = newVal;
	}
	return S_OK;
}

// *****************************************************************
//		Visible
// *****************************************************************
STDMETHODIMP CDrawingRectangle::get_Visible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _visible;
	return S_OK;
}
STDMETHODIMP CDrawingRectangle::put_Visible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_visible = newVal;
	return S_OK;
}

// *****************************************************************
//		ReferenceType
// *****************************************************************
STDMETHODIMP CDrawingRectangle::get_ReferenceType(tkDrawReferenceList* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _referenceType;
	return S_OK;
}
STDMETHODIMP CDrawingRectangle::put_ReferenceType(tkDrawReferenceList newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_referenceType = newVal;
	return S_OK;
}

