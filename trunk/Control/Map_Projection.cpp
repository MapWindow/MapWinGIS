#include "stdafx.h"
#include "map.h"
#include "GeoProjection.h"
#include "Tiles.h"

// some simple incapsulation for readability of code
IGeoProjection* CMapView::GetMapToWgs84Transform() 
{ 
	VARIANT_BOOL vb;
	m_projection->get_HasTransformation(&vb);
	return vb ?  m_projection : NULL; 
}
IGeoProjection* CMapView::GetMapToGMercTransform() 
{
	VARIANT_BOOL vb;
	m_projectionToGMerc->get_HasTransformation(&vb);
	return vb ?  m_projectionToGMerc : NULL; 
}
IGeoProjection* CMapView::GetWgs84ToMapTransform() 
{ 
	VARIANT_BOOL vb;
	m_wgsProjection->get_HasTransformation(&vb);
	return vb ?  m_wgsProjection : NULL; 
}
IGeoProjection* CMapView::GetGMercToMapTransform() 
{ 
	VARIANT_BOOL vb;
	m_GMercProjection->get_HasTransformation(&vb);
	return vb ?  m_GMercProjection : NULL; 
}
IGeoProjection* CMapView::GetWgs84Projection() { return m_wgsProjection; }
IGeoProjection* CMapView::GetGMercProjection() { return m_GMercProjection; }
IGeoProjection* CMapView::GetMapProjection() { return m_projection; }

// *****************************************************
//		ReleaseProjections()
// *****************************************************
void CMapView::ReleaseProjections()
{
	VARIANT_BOOL vb;
	m_projection->Clear(&vb);
	m_projection->Release();

	m_projectionToGMerc->Clear(&vb);
	m_projectionToGMerc->Release();
	
	m_wgsProjection->Clear(&vb);
	m_wgsProjection->Release();
	
	m_GMercProjection->Clear(&vb);
	m_GMercProjection->Release();
}

// *****************************************************
//		InitProjections()
// *****************************************************
void CMapView::InitProjections()
{
	GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&m_projectionToGMerc);
	GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&m_wgsProjection);
	GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&m_GMercProjection);
	m_transformationMode = tmNotDefined;
	VARIANT_BOOL vb;
	m_wgsProjection->SetWgs84(&vb);		// EPSG:4326
	m_GMercProjection->SetGoogleMercator(&vb);	// EPSG:3857
	
	m_projection = NULL;
	IGeoProjection* p = NULL;
	GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&p);
	SetGeoProjection(p);
}

// *****************************************************
//		SetProjection()
// *****************************************************
void CMapView::SetGeoProjection(IGeoProjection* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IGeoProjection* last = NULL;
	if (pVal)
	{
		last = m_projection;
		if (last)
			last->AddRef();		// add temp reference; as it ca be deleted in the next line
	}
	
	Utility::put_ComReference(pVal, (IDispatch**)&m_projection, false);
	
	if (last)
	{
		if (last != m_projection)
		{
			((CGeoProjection*)last)->SetIsFrozen(false);
		}
		last->Release();
		last = NULL;
	}

	if (m_transformationMode == tmDoTransformation)
		m_wgsProjection->StopTransform();
	
	VARIANT_BOOL isSame, vb;
	m_wgsProjection->get_IsSame(m_projection, &isSame);
	if (isSame)
	{
		m_transformationMode = tmWgs84Complied;
	}
	else
	{
		m_wgsProjection->StartTransform(m_projection, &vb);	// must always have transformation to current projection
		m_transformationMode = vb ? tmDoTransformation : tmNotDefined;
		if (!vb) {
			Utility::DisplayErrorMsg(m_globalCallback, m_key, "Failed to start WGS84 to map transformation.");
		}

		m_projection->StartTransform(m_wgsProjection, &vb);	// must always have transformation to WGS84
		if (!vb) {
			Utility::DisplayErrorMsg(m_globalCallback, m_key, "Failed to start map to WGS84 transformation.");
		}
	}

	if (m_projection)
	{
		((CGeoProjection*)m_projection)->SetIsFrozen(true);
	}

	// init Google Mercator transforms (for tiles)
	m_GMercProjection->StartTransform(m_projection, &vb);		// must always have transformation to current projection
	if (!vb) {
		Utility::DisplayErrorMsg(m_globalCallback, m_key, "Failed to start GMercator to map transformation.");
	}
	
	m_projection->Clone(&m_projectionToGMerc);
	m_projectionToGMerc->StartTransform(m_GMercProjection, &vb);
	if (!vb) {
		Utility::DisplayErrorMsg(m_globalCallback, m_key, "Failed to start map to GMercator transformation.");
	}

	VARIANT_BOOL geographic;
	m_projection->get_IsGeographic(&geographic);
	m_unitsOfMeasure = geographic ? umDecimalDegrees : umMeters;

	((CTiles*)m_tiles)->UpdateProjection();
}

// *****************************************************
//		GetProjection()
// *****************************************************
IGeoProjection* CMapView::GetGeoProjection (void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_projection)
		m_projection->AddRef();
	return m_projection;
}

// ***************************************************************
//		ClearMapProjectionWithLastLayer()
// ***************************************************************
void CMapView::ClearMapProjectionWithLastLayer()
{
	if (_grabProjectionFromData)
	{
		VARIANT_BOOL isEmpty;
		m_projection->get_IsEmpty(&isEmpty);
		if (!isEmpty)
		{
			IGeoProjection* proj = NULL;
			GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&proj);
			SetGeoProjection(proj);
		}
	}
}