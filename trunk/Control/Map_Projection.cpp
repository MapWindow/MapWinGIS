#include "stdafx.h"
#include "map.h"
#include "GeoProjection.h"
#include "Tiles.h"

// some simple incapsulation for readability of code
IGeoProjection* CMapView::GetMapToWgs84Transform() 
{ 
	VARIANT_BOOL vb;
	_projection->get_HasTransformation(&vb);
	return vb ?  _projection : NULL; 
}
IGeoProjection* CMapView::GetMapToGMercTransform() 
{
	VARIANT_BOOL vb;
	_projectionToGMerc->get_HasTransformation(&vb);
	return vb ?  _projectionToGMerc : NULL; 
}
IGeoProjection* CMapView::GetWgs84ToMapTransform() 
{ 
	VARIANT_BOOL vb;
	_wgsProjection->get_HasTransformation(&vb);
	return vb ?  _wgsProjection : NULL; 
}
IGeoProjection* CMapView::GetGMercToMapTransform() 
{ 
	VARIANT_BOOL vb;
	_gmercProjection->get_HasTransformation(&vb);
	return vb ?  _gmercProjection : NULL; 
}
IGeoProjection* CMapView::GetWgs84Projection() { return _wgsProjection; }
IGeoProjection* CMapView::GetGMercProjection() { return _gmercProjection; }
IGeoProjection* CMapView::GetMapProjection() { return _projection; }

// *****************************************************
//		ReleaseProjections()
// *****************************************************
void CMapView::ReleaseProjections()
{
	VARIANT_BOOL vb;
	_projection->Clear(&vb);
	_projection->Release();

	_projectionToGMerc->Clear(&vb);
	_projectionToGMerc->Release();
	
	_wgsProjection->Clear(&vb);
	_wgsProjection->Release();
	
	_gmercProjection->Clear(&vb);
	_gmercProjection->Release();
}

// *****************************************************
//		InitProjections()
// *****************************************************
void CMapView::InitProjections()
{
	GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&_projectionToGMerc);
	GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&_wgsProjection);
	GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&_gmercProjection);
	_transformationMode = tmNotDefined;
	VARIANT_BOOL vb;
	_wgsProjection->SetWgs84(&vb);		// EPSG:4326
	_gmercProjection->SetGoogleMercator(&vb);	// EPSG:3857
	
	_projection = NULL;
	IGeoProjection* p = NULL;
	GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&p);
	SetGeoProjection(p);
}

// *****************************************************
//		SetGeoProjection()
// *****************************************************
void CMapView::SetGeoProjection(IGeoProjection* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IGeoProjection* last = NULL;
	if (pVal)
	{
		last = _projection;
		if (last)
			last->AddRef();		// add temp reference; as it ca be deleted in the next line
	}
	
	Utility::put_ComReference(pVal, (IDispatch**)&_projection, false);
	
	if (last)
	{
		if (last != _projection)
		{
			((CGeoProjection*)last)->SetIsFrozen(false);
		}
		last->Release();
		last = NULL;
	}

	if (_transformationMode == tmDoTransformation)
		_wgsProjection->StopTransform();
	
	VARIANT_BOOL isSame, vb;
	_wgsProjection->get_IsSame(_projection, &isSame);
	if (isSame)
	{
		_transformationMode = tmWgs84Complied;
	}
	else
	{
		_wgsProjection->StartTransform(_projection, &vb);	// must always have transformation to current projection
		_transformationMode = vb ? tmDoTransformation : tmNotDefined;
		if (!vb) {
			Utility::DisplayErrorMsg(_globalCallback, m_key, "Failed to start WGS84 to map transformation.");
		}

		_projection->StartTransform(_wgsProjection, &vb);	// must always have transformation to WGS84
		if (!vb) {
			Utility::DisplayErrorMsg(_globalCallback, m_key, "Failed to start map to WGS84 transformation.");
		}
	}

	if (_projection)
	{
		((CGeoProjection*)_projection)->SetIsFrozen(true);
	}

	// init Google Mercator transforms (for tiles)
	_gmercProjection->StartTransform(_projection, &vb);		// must always have transformation to current projection
	if (!vb) {
		Utility::DisplayErrorMsg(_globalCallback, m_key, "Failed to start GMercator to map transformation.");
	}
	
	_projection->Clone(&_projectionToGMerc);
	_projectionToGMerc->StartTransform(_gmercProjection, &vb);
	if (!vb) {
		Utility::DisplayErrorMsg(_globalCallback, m_key, "Failed to start map to GMercator transformation.");
	}

	VARIANT_BOOL geographic;
	_projection->get_IsGeographic(&geographic);
	_unitsOfMeasure = geographic ? umDecimalDegrees : umMeters;

	((CTiles*)_tiles)->UpdateProjection();
}

// *****************************************************
//		GetProjection()
// *****************************************************
IGeoProjection* CMapView::GetGeoProjection (void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_projection)
		_projection->AddRef();
	return _projection;
}

// ***************************************************************
//		ClearMapProjectionWithLastLayer()
// ***************************************************************
void CMapView::ClearMapProjectionWithLastLayer()
{
	if (_grabProjectionFromData)
	{
		VARIANT_BOOL isEmpty;
		_projection->get_IsEmpty(&isEmpty);
		if (!isEmpty)
		{
			IGeoProjection* proj = NULL;
			GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&proj);
			SetGeoProjection(proj);
		}
	}
}