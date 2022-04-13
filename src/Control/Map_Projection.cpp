#include "StdAfx.h"
#include "Map.h"
#include "GeoProjection.h"
#include "Tiles.h"
#include "ExtentsHelper.h"
#include "ShapeHelper.h"

// some simple encapsulation for readability of code
IGeoProjection* CMapView::GetMapToWgs84Transform()
{
	VARIANT_BOOL vb;
	_projection->get_HasTransformation(&vb);
	return vb ? _projection : nullptr;
}
IGeoProjection* CMapView::GetWgs84ToMapTransform()
{
	VARIANT_BOOL vb;
	_wgsProjection->get_HasTransformation(&vb);
	return vb ? _wgsProjection : nullptr;
}
IGeoProjection* CMapView::GetTilesToMapTransform()
{
	VARIANT_BOOL vb;
	_tileProjection->get_HasTransformation(&vb);
	return vb ? _tileProjection : nullptr;
}

IGeoProjection* CMapView::GetMapToTilesTransform()
{
	VARIANT_BOOL vb;
	_tileReverseProjection->get_HasTransformation(&vb);
	return vb ? _tileReverseProjection : nullptr;
}

IGeoProjection* CMapView::GetWgs84Projection() { return _wgsProjection; }
IGeoProjection* CMapView::GetGMercProjection() { return _gmercProjection; }
IGeoProjection* CMapView::GetMapProjection() { return _projection; }

// *****************************************************
//		ReleaseProjections()
// *****************************************************
void CMapView::ReleaseProjections()
{
	dynamic_cast<CGeoProjection*>(_projection)->SetIsFrozen(false);
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
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_tileProjection);  // TODO: Don't use C-style cast
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_tileReverseProjection);
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_wgsProjection);
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_gmercProjection);
	_transformationMode = tmNotDefined;
	VARIANT_BOOL vb;
	_wgsProjection->SetWgs84(&vb);				// EPSG:4326
	_gmercProjection->SetGoogleMercator(&vb);	// EPSG:3857

	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_projection);

	CComPtr<IGeoProjection> p = nullptr;
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&p);
	SetGeoProjection(p);
}

// *****************************************************
//		SetGeoProjection()
// *****************************************************
void CMapView::SetGeoProjection(IGeoProjection* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!pVal || pVal == _projection) {
		return;
	}

	bool preserveExtents = _activeLayers.size() == 0;
	IExtents* ext = preserveExtents ? GetGeographicExtents() : nullptr;	// try to preserve extents

	((CGeoProjection*)_projection)->SetIsFrozen(false);
	_projection->StopTransform();

	ComHelper::SetRef(pVal, (IDispatch**)&_projection);

#ifndef RELEASE_MODE
	CComBSTR bStr;
	_projection->ExportToProj4(&bStr);
	Debug::WriteLine("MAP PROJECTION SET: %s", static_cast<LPWSTR>(bStr));
#endif

	_projectionChangeCount++;

	UpdateMapTranformation();

	UpdateTileProjection();

	((CGeoProjection*)_projection)->SetIsFrozen(true);

	_projection->get_LinearUnits(&_unitsOfMeasure);

	ClearExtentHistory();

	// restore extents
	if (ext)
	{
		if (preserveExtents) {
			SetGeographicExtents(ext);
		}

		ext->Release();
	}

	FireProjectionChanged();
}

// *****************************************************
//		UpdateMapTranformation()
// *****************************************************
void CMapView::UpdateMapTranformation()
{
	VARIANT_BOOL isEmpty;
	_projection->get_IsEmpty(&isEmpty);

	IGeoProjection* gp = nullptr;

	_wgsProjection->StopTransform();

	if (isEmpty)
	{
		VARIANT_BOOL isSame, vb;
		CComPtr<IExtents> box = nullptr;
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
		_transformationMode = tmNotDefined;
	}
}



// *****************************************************
//		GetProjection()
// *****************************************************
IGeoProjection* CMapView::GetGeoProjection(void)
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
			CComPtr<IGeoProjection> proj = nullptr;
			ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&proj);
			if (proj)
			{
				SetGeoProjection(proj);
			}
		}
	}
}

// ***************************************************************
//		GeodesicLength()
// ***************************************************************
DOUBLE CMapView::GeodesicLength(IShape* polyline)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!polyline)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return 0.0;
	}

	ShpfileType shpType = ShapeHelper::GetShapeType2D(polyline);
	if (shpType != SHP_POLYLINE && shpType != SHP_POLYGON)
	{
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return 0.0;
	}

	VARIANT_BOOL ellipsoid;
	_geodesicShape->get_IsUsingEllipsoid(&ellipsoid);
	if (!ellipsoid)
	{
		ErrorMessage(tkFAILED_TRANSFORM_MAP_TO_WGS84);
		return 0.0;   // Euclidean area can be calculated using Shape.Area, so better to return 0.0
	}

	_geodesicShape->SetShape(polyline);

	double length = 0.0;
	_geodesicShape->get_Length(&length);

	return length;
}

// ***************************************************************
//		GeodesicArea()
// ***************************************************************
DOUBLE CMapView::GeodesicArea(IShape* polygon)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!polygon)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return 0.0;
	}

	ShpfileType shpType = ShapeHelper::GetShapeType2D(polygon);
	if (shpType != SHP_POLYGON)
	{
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return 0.0;
	}

	VARIANT_BOOL ellipsoid;
	_geodesicShape->get_IsUsingEllipsoid(&ellipsoid);
	if (!ellipsoid)
	{
		ErrorMessage(tkFAILED_TRANSFORM_MAP_TO_WGS84);
		return 0.0;   // Euclidean area can be calculated using Shape.Area, so better to return 0.0
	}

	_geodesicShape->SetShape(polygon);

	double area = 0.0;
	_geodesicShape->get_Area(&area);

	return area;
}

// ***************************************************************
//		GeodesicDistance()
// ***************************************************************
DOUBLE CMapView::GeodesicDistance(DOUBLE projX1, DOUBLE projY1, DOUBLE projX2, DOUBLE projY2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IGeoProjection* gp = GetMapToWgs84Transform();
	if (!gp) {
		ErrorMessage(tkFAILED_TRANSFORM_MAP_TO_WGS84);
		return 0.0;
	}

	VARIANT_BOOL transform;
	gp->get_HasTransformation(&transform);
	if (_transformationMode == tmNotDefined || !transform) {
		ErrorMessage(tkFAILED_TRANSFORM_MAP_TO_WGS84);
		return 0.0;
	}

	VARIANT_BOOL vb, vb2;
	gp->Transform(&projX1, &projY1, &vb);
	gp->Transform(&projX2, &projY2, &vb2);

	if (!vb || !vb2) {
		ErrorMessage(tkFAILED_TRANSFORM_MAP_TO_WGS84);
		return 0.0;
	}

	double distance = 0.0;
	GetUtils()->GeodesicDistance(projY1, projX1, projY2, projX2, &distance);
	return distance;
}

