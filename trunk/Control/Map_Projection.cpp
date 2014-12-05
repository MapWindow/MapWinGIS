#include "stdafx.h"
#include "map.h"
#include "GeoProjection.h"
#include "Tiles.h"
#include "ExtentsHelper.h"
#include "ProjectionHelper.h"

// some simple encapsulation for readability of code
IGeoProjection* CMapView::GetMapToWgs84Transform() 
{ 
	VARIANT_BOOL vb;
	_projection->get_HasTransformation(&vb);
	return vb ?  _projection : NULL; 
}
IGeoProjection* CMapView::GetWgs84ToMapTransform() 
{ 
	VARIANT_BOOL vb;
	_wgsProjection->get_HasTransformation(&vb);
	return vb ?  _wgsProjection : NULL; 
}
IGeoProjection* CMapView::GetTilesToMapTransform() 
{ 
	VARIANT_BOOL vb;
	_tileProjection->get_HasTransformation(&vb);
	return vb ?  _tileProjection : NULL; 
}

IGeoProjection* CMapView::GetMapToTilesTransform() 
{
	VARIANT_BOOL vb;
	_tileReverseProjection->get_HasTransformation(&vb);
	return vb ?  _tileReverseProjection : NULL; 
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

	_tileProjection->Clear(&vb);
	_tileProjection->Release();

	_tileReverseProjection->Clear(&vb);
	_tileReverseProjection->Release();
	
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
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_tileProjection);
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_tileReverseProjection);
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_wgsProjection);
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_gmercProjection);
	_transformationMode = tmNotDefined;
	VARIANT_BOOL vb;
	_wgsProjection->SetWgs84(&vb);				// EPSG:4326
	_gmercProjection->SetGoogleMercator(&vb);	// EPSG:3857
	
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_projection);

	CComPtr<IGeoProjection> p = NULL;
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&p);
	SetGeoProjection(p);
}

// *****************************************************
//		SetGeoProjection()
// *****************************************************
void CMapView::SetGeoProjection(IGeoProjection* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!pVal || pVal == _projection)
		return;

	bool preserveExtents = _activeLayers.size() == 0;
	IExtents* ext = GetGeographicExtents();	// try to preserve extents

	((CGeoProjection*)_projection)->SetIsFrozen(false);
	_projection->StopTransform();

	ComHelper::SetRef(pVal, (IDispatch**)&_projection);
	VARIANT_BOOL isEmpty;
	_projection->get_IsEmpty(&isEmpty);

#ifndef RELEASE_MODE

	CComBSTR str;
	if (!isEmpty) {
		_projection->ExportToProj4(&str);
	}
	else {
		str = L"";
	}
	USES_CONVERSION;
	CString s = str.Length() > 0 ? OLE2A(str) : "<empty>";
	Debug::WriteLine("MAP PROJECTION SET: %s", s);

#endif

	_projectionChangeCount++;
	IGeoProjection* gp = NULL;
	
	_wgsProjection->StopTransform();
	
	if (!isEmpty)
	{
		VARIANT_BOOL isSame, vb;
		CComPtr<IExtents> box = NULL;
		box.Attach(ExtentsHelper::GetWorldBounds());
		_wgsProjection->get_IsSameExt(_projection, box, 20, &isSame);
		if (isSame)
		{
			_transformationMode = tmWgs84Complied;
		}
		else
		{
			_wgsProjection->StartTransform(_projection, &vb);	// must always have transformation to current projection
			_transformationMode = vb ? tmDoTransformation : tmNotDefined;
			if (!vb) {
				ErrorMessage(tkFAILED_TRANSFORM_WGS84_TO_MAP);
			}

			_projection->StartTransform(_wgsProjection, &vb);	// must always have transformation to WGS84
			if (!vb) {
				ErrorMessage(tkFAILED_TRANSFORM_MAP_TO_WGS84);
			}
		}
	}
	else
	{
		_transformationMode= tmNotDefined;
	}

	if (_projection)
	{
		((CGeoProjection*)_projection)->SetIsFrozen(true);
	}

	VARIANT_BOOL geographic;
	_projection->get_IsGeographic(&geographic);
	_unitsOfMeasure = geographic ? umDecimalDegrees : umMeters;

	((CTiles*)_tiles)->UpdateProjection();

	if (ext) {
		if (preserveExtents)
			SetGeographicExtents(ext);
		ext->Release();
	}

	if (_projection == pVal)
		FireProjectionChanged();
}

// *****************************************************
//		UpdateTileProjection()
// *****************************************************
void CMapView::UpdateTileProjection()
{
	VARIANT_BOOL vb;
	_tileProjection->Clear(&vb);
	_tileReverseProjection->Clear(&vb);
	
	tkTileProjection tp;
	_tiles->get_ServerProjection(&tp);
	
	switch(tp)
	{
		case SphericalMercator:
			_tileProjection->SetGoogleMercator(&vb);
			break;
		case Amersfoort:
			_tileProjection->ImportFromEPSG(EPSG_AMERSFOORT, &vb);
			break;
	}
	_tileReverseProjection->CopyFrom(_projection, &vb);

	if (ProjectionHelper::IsEmpty(_projection)) 
	{
		_tileProjectionState = ProjectionDoTransform;
		return;
	}

	_tileProjection->get_IsSame(_projection, &vb);
	_tileProjectionState = vb ? ProjectionMatch : ProjectionDoTransform;

	if (_transformationMode == tmWgs84Complied && tp == SphericalMercator)
	{
		// transformation is needed, but it leads only to some vertical scaling which is quite acceptable
		_tileProjectionState = ProjectionCompatible;
	}

	if (_tileProjectionState == ProjectionDoTransform || _tileProjectionState == ProjectionCompatible)
	{
		_tileProjection->StartTransform(_projection, &vb);
		if (!vb) {
			ErrorMessage(tkTILES_MAP_TRANSFORM_FAILED);
		}
		
		_tileReverseProjection->StartTransform(_tileProjection, &vb);
		if (!vb) {
			ErrorMessage(tkMAP_TILES_TRANSFORM_FAILED);
		}
	}
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
			ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&proj);
			if (proj)
			{
				SetGeoProjection(proj);
				proj->Release();
			}
		}
	}
}

