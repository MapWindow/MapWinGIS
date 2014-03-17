#include "stdafx.h"
#include <gdiplus.h>
#include "MapWinGis.h"
#include "Map.h"
#include "GeoProjection.h"
#include "Shapefile.h"
# include "Tiles.h"
#include "Measuring.h"

#pragma region Scale

// ****************************************************
//	   ZoomToTileLevel()
// ****************************************************
VARIANT_BOOL CMapView::ZoomToTileLevel(int zoom)
{
	// we shall make all the calculations in the Google Mercator (EPSG: 3857)
	// and then transform bounds to the current coordinate system
	double xCent = (extents.left + extents.right)/2.0;
	double yCent = (extents.bottom + extents.top)/2.0;
	
	double minX, maxX, minY, maxY;	// size of control in pixels
	PROJECTION_TO_PIXEL(extents.left, extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(extents.right, extents.top, maxX, maxY);
	
	// getting screen size
	double screenHeight = abs(maxY - minY); ///96.0;
	double screenWidth =  abs(maxX - minX); ///96.0;

	double ratio = 1.2;		// multiplication ratio
	double projWidth = 20037508.342789244 * 2.0;	// half of equator circumference (width and height for mercator projection)
	double pxWidth = ratio * 512.0 * pow(2.0, zoom - 1);	// width of map in pixels at the requested zoom

	double w = screenWidth / pxWidth * projWidth;		// requested width in meters (EPSG:3857)
	double h = screenHeight / pxWidth * projWidth;		// requested width in meters (EPSG:3857)

	VARIANT_BOOL vb, vb2;
	IGeoProjection* projMap = this->GetGeoProjection();
	if (projMap)
	{
		VARIANT_BOOL projMatch;
		projMap->get_IsSame(m_GMercProjection, &projMatch);
		if (projMatch)
		{
			minX = xCent - w/2.0;
			maxX = xCent + w/2.0;
			minY = yCent - h/2.0;
			maxY = yCent + h/2.0;
			this->SetExtentsCore(Extent( minX, maxX, minY, maxY ), true);
			projMap->Release();
			return VARIANT_TRUE;
		}
		else
		{
			// get center in GMercator
			projMap->StartTransform(m_GMercProjection, &vb);
			if (vb)
			{
				projMap->Transform(&xCent, &yCent, &vb);
				projMap->StopTransform();
				if (!vb)
				{
					ErrorMsg(tkFAILED_TO_REPROJECT);
				}
			}
			else
			{
				ErrorMsg(tkFAILED_TO_REPROJECT);
			}
			
			// return back to map projection
			if (vb)
			{
				m_GMercProjection->StartTransform(projMap, &vb);
				if (vb)
				{
					minX = xCent - w/2.0;
					maxX = xCent + w/2.0;
					minY = yCent - h/2.0;
					maxY = yCent + h/2.0;
					m_GMercProjection->Transform(&minX, &minY, &vb);
					m_GMercProjection->Transform(&maxX, &maxY, &vb2);
					m_GMercProjection->StopTransform();
					if (!vb || !vb2)
					{
						ErrorMsg(tkFAILED_TO_REPROJECT);
					}
					else
					{
						Debug::WriteLine("Zoom to tile level: %d; extents: xMin:%f; xMax: %f; yMin: %f; yMax: %f", zoom, minX, maxX, minY, maxY);
						this->SetExtentsCore(Extent( minX, maxX, minY, maxY ), true);
						projMap->Release();
						return VARIANT_TRUE;
					}
				}
				else
				{
					ErrorMsg(tkFAILED_TO_REPROJECT);
				}
			}
		}
		projMap->Release();
	}
	else
	{
		ErrorMsg(tkMAP_PROJECTION_NOT_SET);
	}
	return VARIANT_FALSE;
}

// ****************************************************
//	   SetExtentsCore()
// ****************************************************
void CMapView::SetExtentsCore(Extent ext, bool logExtents, bool mapSizeChanged)
{
	this->CalculateVisibleExtents(ext, logExtents, mapSizeChanged);
	m_canbitblt = FALSE;

	this->FireExtentsChanged();
	this->ReloadImageBuffers();
	
	this->m_lastWidthMeters = 0.0;	// extents has changed it must be recalculated

	if( !m_lockCount )
	{
		((CTiles*)m_tiles)->LoadTiles((void*)this);
		this->InvalidateControl();
	}
}

// **********************************************************
//			CurrentScale()
// **********************************************************
DOUBLE CMapView::GetCurrentScale(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	 
	double minX, maxX, minY, maxY;	// size of ap control in pixels
    PROJECTION_TO_PIXEL(extents.left, extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(extents.right, extents.top, maxX, maxY);
	if (minX == maxX && minY == maxY)
	{
		return 0.0;
	}
	else
	{
		//TODO: reimplement
		long pixX = 96; //m_layerDC->GetDeviceCaps(LOGPIXELSX);	// Number of pixels per logical inch along the screen width.
		long pixY = 96; //m_layerDC->GetDeviceCaps(LOGPIXELSY);	// Number of pixels per logical inch along the screen height
		if (pixX == 0.0 || pixY == 0.0)	return 0.0;
		
		// logical size of screen, inches
		double screenHeigth = fabs(maxY - minY)/(double)pixY;	//96.0
		double screenWidth =  fabs(maxX - minX)/(double)pixX;	//96.0
		
		// size of map being displayed, inches
		double convFact = Utility::getConversionFactor(m_unitsOfMeasure);	
		if (convFact == 0) return 0.0;
		double mapHeight = (extents.top - extents.bottom)*convFact;
		double mapWidth = (extents.right - extents.left)*convFact;
		
		// calculate it as diagonal
		return sqrt(pow(mapWidth, 2) + pow(mapHeight, 2)) / sqrt(pow(screenWidth,2) + pow(screenHeigth,2));
	}
}

// **********************************************************
//			SetCurrentScale()
// **********************************************************
void CMapView::SetCurrentScale(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal == 0.0) return;

	double xCent = (extents.left + extents.right)/2.0;
	double yCent = (extents.bottom + extents.top)/2.0;
	
	double minX, maxX, minY, maxY;	// size of ap control in pixels
    PROJECTION_TO_PIXEL(extents.left, extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(extents.right, extents.top, maxX, maxY);
	
	//TODO: reimplement

	// getting screen size
	long pixX = 96; //m_layerDC->GetDeviceCaps(LOGPIXELSX);
	long pixY = 96; //m_layerDC->GetDeviceCaps(LOGPIXELSY);
	if (pixX == 0.0 || pixY == 0.0)	
		return;
	double screenHeight = (maxY - minY)/pixY;	//96.0
	double screenWidth =  (maxX - minX)/pixX;	//96.0
	
	// diagonal of map extents in inches
	double mapDiag = newVal * sqrt(pow(screenWidth,2) + pow(screenHeight,2));
	
	if (screenHeight == 0.0) return;
	double a = screenWidth/screenHeight;						// we need width and height, but have diagonal and the sides ratio of triangle only; this makes 2 equations:
	double mapHeight = sqrt(pow(mapDiag,2)/(pow(a,2) + 1));		// x/y = a
	double mapWidth = mapHeight * a;							// x^2 + y^2 = b^2		// where b - mapDiag
																// Taking x from first:
																// y^2*a^2 + y^2 = b^2
																// y = sqrt(b^2/(a^2 + 1))
																// x = y*a
	// converting to the map units
	mapHeight /= Utility::getConversionFactor(m_unitsOfMeasure);
	mapWidth /= Utility::getConversionFactor(m_unitsOfMeasure);

	IExtents* box = NULL;
	CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)&box);
	box->SetBounds(xCent - mapWidth/2.0, yCent - mapHeight/2.0, 0.0, xCent + mapWidth/2.0, yCent + mapHeight/2.0, 0.0);
	this->SetExtents((LPDISPATCH)box);
	box->Release(); box = NULL;
	return;
}
#pragma endregion

#pragma region Extents
// ****************************************************
//	   GetExtents()
// ****************************************************
LPDISPATCH CMapView::GetExtents()
{
	IExtents * box = NULL;
	CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&box);
	box->SetBounds( extents.left, extents.bottom, 0, extents.right, extents.top, 0 );
	return box;
}

// ****************************************************
//	   SetExtents()
// ****************************************************
void CMapView::SetExtents(LPDISPATCH newValue)
{
	if( !newValue )
	{	
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
	}
	else
	{
		IExtents * box = NULL;
		newValue->QueryInterface(IID_IExtents, (void**)&box);

		if( box != NULL )
		{	
			double nv = 0;
			Extent ext;
			box->GetBounds(&ext.left, &ext.bottom, &nv, &ext.right, &ext.top, &nv);
			box->Release();

			this->SetExtentsCore(ext);
		}
		else
			ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
	}
}
#pragma endregion

#pragma region GeographicExtents
// *****************************************************
//		SetGeographicExtents()
// *****************************************************
VARIANT_BOOL CMapView::SetGeographicExtents(IExtents* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!pVal)
	{
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return VARIANT_FALSE;
	}
	else
	{
		if (m_transformationMode == tmWgs84Complied)  //||((CGeoProjection*)m_projection)->get_IsSame(m_wgsProjection))
		{
			this->SetExtents(pVal);
		}
		else if (m_transformationMode == tmDoTransformation)
		{
			double xMin, xMax, yMin, yMax, zMin, zMax;
			pVal->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
			
			VARIANT_BOOL vbretval;
			m_wgsProjection->Transform(&xMin, &yMin, &vbretval);	 if (!vbretval) return VARIANT_FALSE;
			m_wgsProjection->Transform(&xMax, &yMax, &vbretval);	 if (!vbretval) return VARIANT_FALSE;

			pVal->SetBounds(xMin, yMin, zMin, xMax, yMax, zMax);
			this->SetExtents(pVal); 
		}
		return VARIANT_TRUE;
	}
}

// *****************************************************
//		GetMeasuring()
// *****************************************************
IMeasuring* CMapView::GetMeasuring() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(m_measuring) {
		m_measuring->AddRef();
	}
	return m_measuring;
}

// *****************************************************
//		GetGeographicExtents()
// *****************************************************
IExtents* CMapView::GetGeographicExtents()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return GetGeographicExtentsCore(false);
}

// ***************************************************************
//	   GetGeographicExtentsCore
// ***************************************************************
IExtents* CMapView::GetGeographicExtentsCore(bool clipForTiles, Extent* clipExtents)
{
	IExtents * box = NULL;

	if (m_projection)
	{
		//if (((CGeoProjection*)m_projection)->get_IsSame(m_wgsProjection))
		if (m_transformationMode == tkTransformationMode::tmWgs84Complied)
		{
			CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&box);
			box->SetBounds( extents.left, extents.bottom, 0, extents.right, extents.top, 0 );
		}
		else if (m_transformationMode == tkTransformationMode::tmDoTransformation)
		{
			VARIANT_BOOL vbretval;
			m_projection->StartTransform(m_wgsProjection, &vbretval);
			if (vbretval)
			{
				Extent ext;
				bool clip = clipForTiles && clipExtents;
				//Debug::WriteLine("GetGeographicExtentsCore extents: left = %f; right = %f; bottom = %f; top = %f", extents.left, extents.right, extents.bottom, extents.top);
				ext.left = clip ? MAX(extents.left, clipExtents->left) : extents.left;
				ext.right = clip ? MIN(extents.right, clipExtents->right) : extents.right;
				ext.top = clip ? MIN(extents.top, clipExtents->top) : extents.top;
				ext.bottom = clip ? MAX(extents.bottom, clipExtents->bottom) : extents.bottom;
				//Debug::WriteLine("GetGeographicExtentsCore clipped extents: left = %f; right = %f; bottom = %f; top = %f", ext.left, ext.right, ext.bottom, ext.top);
				
				double xBL, yBL, xTL, yTL, xBR, yBR, xTR, yTR;
				
				xBL = ext.left;
				yBL = ext.bottom;
				
				xTL = ext.left;
				yTL = ext.top;

				xBR = ext.right;
				yBR = ext.bottom;
				
				xTR = ext.right;
				yTR = ext.top;

				m_projection->Transform(&xBL, &yBL, &vbretval);	 if (!vbretval) goto cleaning;
				m_projection->Transform(&xTL, &yTL, &vbretval);	 if (!vbretval) goto cleaning;
				m_projection->Transform(&xBR, &yBR, &vbretval);  if (!vbretval) goto cleaning;
				m_projection->Transform(&xTR, &yTR, &vbretval);  if (!vbretval) goto cleaning;
				
				double degreePerMapUnit = this->DegreesPerMapUnit();

				bool checkBounds = true;
				if (checkBounds && !clipForTiles)		// TODO: is it needed
				{
					double xMinTest = xTL; 
					double yMinTest = yBR;
					double xMaxTest = xBR; 
					double yMaxTest = yTL;
					m_wgsProjection->Transform(&xMinTest, &yMinTest, &vbretval);	if (!vbretval) goto cleaning;
					m_wgsProjection->Transform(&xMaxTest, &yMaxTest, &vbretval);    if (!vbretval) goto cleaning;
					
					double x1 = fabs(xMinTest - extents.left);
					double x2 = fabs(xMaxTest - extents.right);
					double y1 = fabs(yMinTest - extents.bottom);
					double y2 = fabs(yMaxTest - extents.top);
					
					VARIANT_BOOL projected;
					m_projection->get_IsGeographic(&projected);
					if (projected)
					{
						if (x1 > 500.0)
							xTL = -180.0;

						if (x2 > 500.0)
							xBR = 180.0;
					}
				}

				CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&box);
				box->SetBounds( xTL, yBR, 0, xBR, yTL, 0 );		// TODO: return 4 point geographical extents as projections other that equirectangular can be used

cleaning:
				m_projection->StopTransform();
			}
		}
	}
	return box;
}
#pragma endregion

#pragma region GeoProjection
// *****************************************************
//		SetProjection()
// *****************************************************
void CMapView::SetGeoProjection(IGeoProjection* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Utility::put_ComReference(pVal, (IDispatch**)&m_projection, false);
	
	if (m_transformationMode == tmDoTransformation)
		m_wgsProjection->StopTransform();
	
	VARIANT_BOOL isSame;
	m_wgsProjection->get_IsSame(m_projection, &isSame);
	if (isSame)
	{
		m_transformationMode = tmWgs84Complied;
	}
	else
	{
		VARIANT_BOOL vbretval;
		m_wgsProjection->StartTransform(m_projection, &vbretval);
		m_transformationMode = vbretval ? tmDoTransformation : tmNotDefined;
	}

	VARIANT_BOOL geographic;
	m_projection->get_IsGeographic(&geographic);
	m_unitsOfMeasure = geographic ? umDecimalDegrees : umMeters;
	((CMeasuring*)m_measuring)->SetProjection(m_projection, m_wgsProjection, m_transformationMode);

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
#pragma endregion

#pragma region Unit conversion
// *****************************************************
//		GetPixelsPerDegree
// *****************************************************
DOUBLE CMapView::GetPixelsPerDegree(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	double val = 1.0;
	
	if (this->m_unitsOfMeasure != umDecimalDegrees)
	{
		if (!Utility::ConvertDistance(umDecimalDegrees, this->m_unitsOfMeasure, val))
		{
			return 0.0;
		}
	}

	double x, y;
    x = y = 0.0;
    double screenX = 0.0, screenY = 0.0;
    this->ProjToPixel(x, y, &screenX, &screenY);
    double x1 = screenX;
	double y1 = screenY;

    x = y = val;
    this->ProjToPixel(x, y, &screenX, &screenY);
    return (abs(screenX - x1) + abs(screenY - y1))/2.0;
}

// *****************************************************
//		GetPixelsPerMapUnit
// *****************************************************
// Without conversion to decimal degrees
DOUBLE CMapView::PixelsPerMapUnit(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	double val = 1.0;
	
	double x, y;
    x = y = 0.0;
    double screenX = 0.0, screenY = 0.0;
    this->ProjToPixel(x, y, &screenX, &screenY);
    double x1 = screenX;

    x = y = val;
    this->ProjToPixel(x, y, &screenX, &screenY);
    return abs(screenX - x1);
}

// *****************************************************
//		DegreesPerMapUnit
// *****************************************************
DOUBLE CMapView::DegreesPerMapUnit(void)
{
	double val = 1.0;
	if (m_unitsOfMeasure == umDecimalDegrees)
	{
		return val;
	}
	else
	{
		if (Utility::ConvertDistance(this->m_unitsOfMeasure, umDecimalDegrees, val))
		{
			return val;
		}
		else
			return 1.0;
	}
}

// *****************************************************
//		SetPixelsPerDegree
// *****************************************************
void CMapView::SetPixelsPerDegree(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SetNotSupported();
}

void CMapView::ProjToPixel(double projX, double projY, double FAR* pixelX, double FAR* pixelY)
{
	ProjectionToPixel(projX,projY,*pixelX,*pixelY);
}

void CMapView::PixelToProj(double pixelX, double pixelY, double FAR* projX, double FAR* projY)
{
	PixelToProjection(pixelX,pixelY,*projX,*projY);
}

inline void CMapView::PixelToProjection( double piX, double piY, double & prX, double & prY )
{
	prX = extents.left + piX*m_inversePixelPerProjectionX;
	prY = extents.top - piY*m_inversePixelPerProjectionY;
}

inline void CMapView::ProjectionToPixel( double prX, double prY, double & piX, double & piY )
{
	piX = (prX - extents.left)*m_pixelPerProjectionX;
	piY = (extents.top - prY) * m_pixelPerProjectionY;
}

// ***********************************************************
//		UnitsPerPixel
// ***********************************************************
// Returns number of map units per pixel of map
double CMapView::UnitsPerPixel()
{
    double minX, maxX, minY, maxY;
    PROJECTION_TO_PIXEL(extents.left, extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(extents.right, extents.top, maxX, maxY);
	if (minX == maxX && minY == maxY)
	{
		return 0.0;
	}
	else
	{
		return sqrt(pow(extents.right - extents.left, 2) + pow(extents.top - extents.bottom, 2)) / 
			   sqrt(pow(maxX - minX,2) + pow(maxY - minY,2));
	}
}
#pragma endregion

#pragma region Zooming
// *****************************************************
//		ZoomToSelected()
// *****************************************************
VARIANT_BOOL CMapView::ZoomToSelected(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long numSelected = 0;

	IShapefile* sf = this->GetShapefile(LayerHandle);
	if (sf)
	{
		long numShapes;
		sf->get_NumShapes(&numShapes);
		sf->get_NumSelected(&numSelected);
		
		if (numSelected > 0)
		{
			double xMin, xMax, yMin, yMax;
			double _minX, _maxX, _minY, _maxY;
			bool first = true;
			for (int i = 0; i < numShapes; i++)
			{
				VARIANT_BOOL selected;
				sf->get_ShapeSelected(i, &selected);
				if (selected)
				{
					if (((CShapefile*)sf)->QuickExtentsCore(i, &xMin, &yMin, &xMax, &yMax))
					{					
						if (first)
						{
							_minX = xMin, _maxX = xMax;
							_minY = yMin, _maxY = yMax;
							first = false;
						}
						else	
						{	if( xMin < _minX )	_minX = xMin; 
							if( xMax > _maxX )	_maxX = xMax;
							if( yMin < _minY )	_minY = yMin;
							if( yMax > _maxY )	_maxY = yMax;
						}
					}
				}
			}
			
			IExtents* bounds = NULL;
			CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)&bounds);
			bounds->SetBounds(_minX, _minY, 0.0, _maxX, _maxY, 0.0);
			this->SetExtents(bounds);
			bounds->Release();
		}
		sf->Release();
	}
	return numSelected > 0 ? VARIANT_TRUE : VARIANT_FALSE;

}

// ****************************************************************
//		ZoomToMaxExtents()
// ****************************************************************
void CMapView::ZoomToMaxExtents()
{
	bool extentsSet = false;

	if( m_activeLayers.size() > 0 )
		LogPrevExtent();

	long endcondition = m_activeLayers.size();
	for(int i = 0; i < endcondition; i++ )
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		this->AdjustLayerExtents(m_activeLayers[i]);

		if( extentsSet == false )
		{	
			double xrange = l->extents.right - l->extents.left;
			double yrange = l->extents.top - l->extents.bottom;

			//Neio 2009-07-02 for empty layer will cause the caculation error
			if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
				continue;

			extents.left = l->extents.left - xrange*m_extentPad;
			extents.right = l->extents.right + xrange*m_extentPad;
			extents.top = l->extents.top + yrange*m_extentPad;
			extents.bottom = l->extents.bottom - yrange*m_extentPad;
			extentsSet = true;
		}
		else
		{	double xrange = l->extents.right - l->extents.left;
			double yrange = l->extents.top - l->extents.bottom;

			//Neio 2009-07-02 for empty layer will cause the caculation error
			if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
				continue;

			if( l->extents.left - xrange*m_extentPad < extents.left )
				extents.left = l->extents.left - xrange*m_extentPad;
			if( l->extents.right + xrange*m_extentPad > extents.right )
				extents.right = l->extents.right + xrange*m_extentPad;
			if( l->extents.bottom - yrange*m_extentPad < extents.bottom )
				extents.bottom = l->extents.bottom - yrange*m_extentPad;
			if( l->extents.top + yrange*m_extentPad > extents.top )
				extents.top = l->extents.top + yrange*m_extentPad;
		}
	}

	if( !extentsSet )
		extents = Extent(0,0,0,0);

	this->SetExtentsCore(extents);

}

// ****************************************************************
//		ZoomToMaxVisibleExtents()
// ****************************************************************
//Rob Cairns - this is a suggested fix for gdal images
void CMapView::ZoomToMaxVisibleExtents(void)
{
	bool extentsSet = false;

	if( m_activeLayers.size() > 0 )
		LogPrevExtent();

	register int i;
	long endcondition = m_activeLayers.size();
	for( i = 0; i < endcondition; i++ )
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		this->AdjustLayerExtents(m_activeLayers[i]);
		if( l->flags & Visible)
		{
			if( extentsSet == false )
			{	double xrange = l->extents.right - l->extents.left;
				double yrange = l->extents.top - l->extents.bottom;

				//Neio 2009-07-02 for empty layer will cause the caculation error
				if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
					continue;

				extents.left = l->extents.left - xrange*m_extentPad;
				extents.right = l->extents.right + xrange*m_extentPad;
				extents.top = l->extents.top + yrange*m_extentPad;
				extents.bottom = l->extents.bottom - yrange*m_extentPad;
				extentsSet = true;
			}
			else
			{	double xrange = l->extents.right - l->extents.left;
				double yrange = l->extents.top - l->extents.bottom;

				//Neio 2009-07-02 for empty layer will cause the caculation error
				if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
					continue;

				if( l->extents.left - xrange*m_extentPad < extents.left )
					extents.left = l->extents.left - xrange*m_extentPad;
				if( l->extents.right + xrange*m_extentPad > extents.right )
					extents.right = l->extents.right + xrange*m_extentPad;
				if( l->extents.bottom - yrange*m_extentPad < extents.bottom )
					extents.bottom = l->extents.bottom - yrange*m_extentPad;
				if( l->extents.top + yrange*m_extentPad > extents.top )
					extents.top = l->extents.top + yrange*m_extentPad;
			}
		}

	}

	if( ! extentsSet )
		extents = Extent(0,0,0,0);

	this->SetExtentsCore(extents);
}

// **************************************************************
//		ZoomToLayer()
// **************************************************************
void CMapView::ZoomToLayer(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,m_allLayers) )
	{	
		this->AdjustLayerExtents(LayerHandle);
		
		this->LogPrevExtent();
		Layer * l = m_allLayers[LayerHandle];
		double xrange = l->extents.right - l->extents.left;
		double yrange = l->extents.top - l->extents.bottom;
		extents.left = l->extents.left - xrange*m_extentPad;
		extents.right = l->extents.right + xrange*m_extentPad;
		extents.top = l->extents.top + yrange*m_extentPad;
		extents.bottom = l->extents.bottom - yrange*m_extentPad;
		
		this->SetExtentsCore(extents);
	}
	else
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
}

// ****************************************************************
//		ZoomToShape()
// ****************************************************************
void CMapView::ZoomToShape(long LayerHandle, long Shape)
{
	if( IsValidShape(LayerHandle, Shape) )
	{	
		IShapefile * ishp = NULL;
		Layer * l = m_allLayers[LayerHandle];
		//l->object->QueryInterface(IID_IShapefile,(void**)&ishp);
		//if( ishp == NULL )
		if (!l->QueryShapefile(&ishp))
		{	
			this->ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
			return;
		}

		this->LogPrevExtent();
		
		double left, right, top, bottom;
		((CShapefile*)ishp)->QuickExtentsCore(Shape, &left, &bottom, &right, &top );
		ishp->Release();
		
		double xrange = right - left;
		double yrange = top - bottom;

		//if the minimum and maximum extents are the same, use a range of 1 for
		// xrange and yrange to allow a point to be centered in the map window
		// when ZoomToShape is used on a point shapefile.
		if(xrange == 0)
			xrange = 1;
		
		if(yrange == 0)
			yrange = 1;

		extents.left = left - xrange*m_extentPad;
		extents.right = right + xrange*m_extentPad;
		extents.top = top + yrange*m_extentPad;
		extents.bottom = bottom - yrange*m_extentPad;

		this->SetExtentsCore(extents);
	}
	else
	{	
		//Error Code set in func
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
	}
}

#pragma endregion

#pragma region Calculate visible extents
// ***************************************************************
//		CalculateVisibleExtents()
// ***************************************************************
void CMapView::CalculateVisibleExtents( Extent e, bool LogPrev, bool MapSizeChanged )
{
	if( LogPrev == true )
		LogPrevExtent();

	double left = MIN( e.left, e.right );
	double right = MAX( e.left, e.right );
	double bottom = MIN( e.bottom, e.top );
	double top = MAX( e.bottom, e.top );
	
	if (left == right)	// lsu 26 jul 2009 for zooming to single point
	{	
		left -= 0.5;  
		right +=0.5;
	}
	if (bottom == top)
	{	
		bottom -=0.5;
		top +=0.5;
	}
	double xrange = right - left;
	double yrange = top - bottom;

	double yextent, xextent;
	double xadjust = 0, yadjust = 0;
	
	if (!MapSizeChanged && rbMapResizeBehavior != rbWarp)
	{
		// size of control is the same, we need just to apply new extents
		if (m_viewHeight!= 0 && m_viewWidth != 0 && xrange != 0.0 && yrange != 0.0)
		{	 
			// make extents proportional to screen sides ratio
			double ratio = ((double)m_viewWidth/(double)m_viewHeight)/(xrange/yrange);
			if (ratio > 1 )
			{	
				left = left - xrange * (ratio - 1)/2;
				right = left + xrange * (ratio);
				xrange *= ratio;
			}
			else if (ratio < 1)
			{	
				top = top + yrange * ((1/ratio) - 1)/2;
				bottom = top - yrange * (1/ratio);
				yrange *= (1/ratio);	
			}
		}
	}
	else
	{	
		// size of control changed; we need to calculate new extents
		if ( (rbMapResizeBehavior == rbClassic) || (rbMapResizeBehavior == rbIntuitive) )
		{
			if( xrange > yrange )
			{
				yextent = (xrange / m_viewWidth) * m_viewHeight;
				xextent = xrange;
				yadjust = (yextent - yrange) * .5;
				xadjust = 0;
				
				if (rbMapResizeBehavior == rbClassic)
				{
					if( yextent < yrange )
					{
						yextent = yrange;
						xextent = (yrange / m_viewHeight) * m_viewWidth;
						yadjust = 0;
						xadjust = (xextent - xrange) * .5;
					}
				}
			}
			else
			{
				xextent = (yrange / m_viewHeight) * m_viewWidth;
				yextent = yrange;
				xadjust = (xextent - xrange) * .5;
				yadjust = 0;
				
				if (rbMapResizeBehavior == rbClassic)
				{
					if( xextent < xrange )
					{
						xextent = xrange;
						yextent = (xrange / m_viewWidth) * m_viewHeight;
						xadjust = 0;
						yadjust = (yextent - yrange) * .5;
					}
				}
			}
		}
		else if (rbMapResizeBehavior == rbModern)
		{   
			//sizeOption is "modern" - this leaves scale on dX but adjusts scale on dY
			xextent = (yrange / m_viewHeight) * m_viewWidth;
		    yextent = yrange;
		}
		
		else if (rbMapResizeBehavior == rbKeepScale)
		{   
			// lsu (07/03/09) sizeOption is "keep scale", no scale adjustments for both axes
			if (m_pixelPerProjectionX == 0 || m_pixelPerProjectionY == 0)
			{	xextent = xrange;
				yextent = yrange;
			}
			else
			{	xextent = m_viewWidth/m_pixelPerProjectionX;
				yextent = m_viewHeight/m_pixelPerProjectionY;
			}
		}
		else if (rbMapResizeBehavior == rbWarp)
		{
			//sizeOption is "Warp" - this does not scale
			xextent = xrange;
			yextent = yrange;
			xadjust = 0;
			yadjust = 0;
		}
		
		left = left - xadjust;
		right = left + xextent;
		bottom = bottom - yadjust;
		if (rbMapResizeBehavior == rbKeepScale)
			bottom = top - yextent;
		else
			top = bottom + yextent;	

		xrange = right - left;
		yrange = top - bottom;
	}
	
	// save new extents and recalculate scale
	extents.left = left;
	extents.right = right;
	extents.bottom = bottom;
	extents.top = top;
	
	if (xrange == 0)
	{
		m_pixelPerProjectionX = 0;
		m_inversePixelPerProjectionX = 0;
	}
	else
	{
		m_pixelPerProjectionX = m_viewWidth/xrange;
		m_inversePixelPerProjectionX = 1.0/m_pixelPerProjectionX;
	}

	if (yrange == 0)
	{
		m_pixelPerProjectionY = 0;
		m_inversePixelPerProjectionY = 0;
	}
	else
	{
		m_pixelPerProjectionY = m_viewHeight/yrange;
		m_inversePixelPerProjectionY = 1.0/m_pixelPerProjectionY;
	}
}
#pragma endregion

#pragma region Max extents
// ****************************************************************
//		GetMaxExtents()
// ****************************************************************
IExtents* CMapView::GetMaxExtents(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	bool extentsSet = false;
	Extent maxExtents;

	for(size_t i = 0; i <  m_activeLayers.size(); i++ )
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		this->AdjustLayerExtents(m_activeLayers[i]);

		if( !extentsSet )
		{	
			maxExtents.left = l->extents.left;
			maxExtents.right = l->extents.right;
			maxExtents.top = l->extents.top;
			maxExtents.bottom = l->extents.bottom;
			extentsSet = true;
		}
		else
		{	

			if( l->extents.left < maxExtents.left )
				maxExtents.left = l->extents.left;
			if( l->extents.right > maxExtents.right )
				maxExtents.right = l->extents.right;
			if( l->extents.bottom < maxExtents.bottom )
				maxExtents.bottom = l->extents.bottom;
			if( l->extents.top > maxExtents.top )
				maxExtents.top = l->extents.top;
		}
	}

	if( !extentsSet )
	{
		maxExtents = Extent(0,0,0,0);
	}
	
	IExtents* ext = NULL;
	CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)&ext);
	ext->SetBounds(maxExtents.left, maxExtents.bottom, 0.0, maxExtents.right, maxExtents.top, 0.0);
	return ext;
}

// ****************************************************************
//		SetMaxExtents()
// ****************************************************************
void CMapView::SetMaxExtents(IExtents* pVal)
{
	SetNotSupported();
}
#pragma endregion

#pragma region Zoom in/zoom out
bool tiledZoom = false;
// ***************************************************
//		ZoomIn()
// ***************************************************
void CMapView::ZoomIn(double Percent)
{
	if (tiledZoom)
	{
		// An attempt to use discrete zoom levels from tiles; unfinished
		int zoom;
		this->m_tiles->get_CurrentZoom(&zoom);
		Debug::WriteLine("Current zoom: %d", zoom);
		if (zoom < 18) {
			this->ZoomToTileLevel(zoom + 1);
			this->m_tiles->get_CurrentZoom(&zoom);
			Debug::WriteLine("After zoom in: %d", zoom);
		}
	}
	else {
		double xzin = ((extents.right - extents.left)*( 1.0 - Percent ))*.5;
		double xmiddle = extents.left + (extents.right - extents.left)*.5;

		double cLeft = xmiddle - xzin;
		double cRight = xmiddle + xzin;

		double yzin = ((extents.top - extents.bottom)*( 1.0 - Percent ))*.5;
		double ymiddle = extents.bottom + (extents.top - extents.bottom)*.5;

		double cBottom = ymiddle - yzin;
		double cTop = ymiddle + yzin;

		this->SetExtentsCore(Extent(cLeft, cRight, cBottom, cTop), true);
	}
}

// ***************************************************
//		ZoomOut()
// ***************************************************
void CMapView::ZoomOut(double Percent)
{
	if (tiledZoom)
	{
		int zoom;
		this->m_tiles->get_CurrentZoom(&zoom);
		Debug::WriteLine("Current zoom: %d", zoom);
		if (zoom > 1) {
			this->ZoomToTileLevel(zoom - 1);
			Debug::WriteLine("After zoom out: %d", zoom);
		}
		// An attempt to use discrete zoom levels from tiles; unfinished
		//this->m_tiles->ZoomOut();
	}
	else 
	{
		double xzout = ((extents.right - extents.left)*( 1.0 + Percent ))*.5;
		double xmiddle = extents.left + (extents.right - extents.left)*.5;

		double cLeft = xmiddle - xzout;
		double cRight = xmiddle + xzout;

		double yzout = ((extents.top - extents.bottom)*( 1.0 + Percent ))*.5;
		double ymiddle = extents.bottom + (extents.top - extents.bottom)*.5;

		double cBottom = ymiddle - yzout;
		double cTop = ymiddle + yzout;

		this->SetExtentsCore(Extent( cLeft, cRight, cBottom, cTop ), true);
	}
}

void CMapView::LogPrevExtent()
{
	m_prevExtents.push_back( extents );
	if( m_prevExtents.size() > (size_t)m_extentHistory )
		m_prevExtents.pop_front();
}

long CMapView::ZoomToPrev()
{
	if( m_prevExtents.size() > 0 )
	{	
		this->SetExtentsCore(m_prevExtents[m_prevExtents.size() - 1], false);
		m_prevExtents.pop_back();
	}
	return m_prevExtents.size();
}

// ****************************************************************
//		ZoomToWorld()
// ****************************************************************
VARIANT_BOOL CMapView::ZoomToWorld()
{
	VARIANT_BOOL vb;
	m_projection->get_IsEmpty(&vb);
	if (!vb) {
		IExtents* ext = NULL;
		CoCreateInstance(CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&ext);
		ext->SetBounds(-180.0, -85.0, 0.0, 180.0, 85.0, 0.0);
		vb = this->SetGeographicExtents(ext);
		if (vb)  {
			this->Redraw();
		}
		return vb;
	}
	else {
		this->ErrorMessage(tkMAP_PROJECTION_NOT_SET);
	}
	return false;
}
