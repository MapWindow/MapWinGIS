#include "stdafx.h"
#include "Map.h"
#include "Projections.h"
#include "xtiffio.h"  /* for TIFF */
#include "geotiffio.h" /* for GeoTIFF */
#include "Gdipluspixelformats.h"
#include "tiff.h"
#include "geotiff.h"
#include "geo_normalize.h"
#include "geovalues.h"
#include "tiffio.h"
#include "tiffiop.h"

#pragma region Rotation
// ****************************************************************
//		GetRotatedExtent()
// ****************************************************************
//ajp (June 2010)
IPoint* CMapView::GetBaseProjectionPoint(double rotPixX, double rotPixY)
{
  IPoint *curPoint = NULL;
  long basePixX = 0, basePixY = 0; 
  double baseProjX = 0, baseProjY = 0; 
	
  CoCreateInstance( CLSID_Point, NULL, CLSCTX_INPROC_SERVER, IID_IPoint, (void**)&curPoint);

	if (_rotate == NULL || _rotate->degAngle == 0.0)
  {
    basePixX = (long)rotPixX;
    basePixY = (long)rotPixY;
  }
  else
  {
     _rotate->getOriginalPixelPoint((long) rotPixX, (long) rotPixY, &basePixX, &basePixY);
  }
  PixelToProjection( basePixX, basePixY, baseProjX, baseProjY);
  
  curPoint->put_X(baseProjX);
  curPoint->put_Y(baseProjY);
  return curPoint;
}

// ****************************************************************
//		GetRotatedExtent()
// ****************************************************************
//ajp (June 2010)
IExtents* CMapView::GetRotatedExtent()
{
  Extent rotExtent;
	IExtents * box = NULL;

	rotExtent = _extents;
	CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&box);
  box->SetBounds( rotExtent.left, rotExtent.bottom, 0, rotExtent.right, rotExtent.top, 0 );

	if (_rotateAngle == 0)
	  return box;

	if (_rotate == NULL)
		_rotate = new Rotate();

	_rotate->calcRotatedExtent(_viewWidth, _viewHeight);
  rotExtent.right += (_rotate->xAxisDiff * _inversePixelPerProjectionX);
  rotExtent.bottom -= (_rotate->yAxisDiff * _inversePixelPerProjectionY);
  rotExtent.left -= (_rotate->xAxisDiff * _inversePixelPerProjectionX);
  rotExtent.top += (_rotate->yAxisDiff * _inversePixelPerProjectionY);
  box->SetBounds( rotExtent.left, rotExtent.bottom, 0, rotExtent.right, rotExtent.top, 0 );

	return box;
}
#pragma endregion

#pragma region Images

// ***************************************************
//  GetImageLayerPercentTransparent
// ***************************************************
float CMapView::GetImageLayerPercentTransparent(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,_allLayers) )
	{
		Layer * l = _allLayers[LayerHandle];
		if( l->type == ImageLayer )
		{
			IImage * iimg = NULL;
			
			if (!l->QueryImage(&iimg))
				return 1.0;
			
			double val;
			iimg->get_TransparencyPercent(&val);
			iimg->Release(); iimg = NULL;
			return static_cast<float>(val);
		}
		else
		{	
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
			return 0.0f;
		}
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return 0.0f;
	}
}

// ***************************************************
//  GetImageLayerPercentTransparent
// ***************************************************
// Will be deprecated
void CMapView::SetImageLayerPercentTransparent(long LayerHandle, float newValue)
{
	if( newValue < 0.0 )		newValue = 0.0;
	else if( newValue > 1.0 )	newValue = 1.0;

	if( IS_VALID_LAYER(LayerHandle,_allLayers) )
	{
		Layer * l = _allLayers[LayerHandle];
		if( l->type == ImageLayer )
		{	
			IImage * iimg = NULL;
			
			//if( iimg == NULL )	return;
			if (!l->QueryImage(&iimg))
				return;
			iimg->put_TransparencyPercent(newValue);
			iimg->Release(); iimg = NULL;
		}
		else
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
	}
	else
		ErrorMessage(tkINVALID_LAYER_HANDLE);
}

// ***************************************************
//  SetImageLayerColorScheme
// ***************************************************
// Will be deprecated
VARIANT_BOOL CMapView::SetImageLayerColorScheme(LONG LayerHandle, IDispatch* ColorScheme)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(IS_VALID_LAYER(LayerHandle,_allLayers))
	{
		// redirected to image class for backward compatibility
		IGridColorScheme* scheme = NULL;
		ColorScheme->QueryInterface(IID_IGridColorScheme,(void**)&scheme);
		if (scheme)
		{
			IImage* img = this->GetImage(LayerHandle);
			if (img != NULL)
			{
				img->put_CustomColorScheme(scheme);
				img->Release();
				return VARIANT_TRUE;
			}
			else
			{
				ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
				return VARIANT_FALSE;
			}
			scheme->Release();
		}
		else
		{
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
			return VARIANT_FALSE;
		}
	}
	else
	{
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return VARIANT_FALSE;
	}
}

// deprecated
void CMapView::UpdateImage(LONG LayerHandle)
{
	ErrorMessage(tkMETHOD_DEPRECATED);
}

// ***************************************************************
//		LayerIsEmpty()
// ***************************************************************
bool CMapView::LayerIsEmpty(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,_allLayers) )
	{	
		Layer * l = _allLayers[LayerHandle];
		if(l->object == NULL) return true;
		if (l->type == ShapefileLayer)
		{
			IShapefile * ishp = NULL;
			if (!l->QueryShapefile(&ishp)) return true;

			long numShapes;
			ishp->get_NumShapes(&numShapes);
			ishp->Release();
			if (numShapes == 0) return true;
		}
		return false;
	}
	return true;
}

// ***************************************************************
//		AdjustLayerExtents()
// ***************************************************************
BOOL CMapView::AdjustLayerExtents(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,_allLayers) )
	{	
		Layer * l = _allLayers[LayerHandle];
		if(l->object == NULL) return FALSE;
		
		if (l->type == ImageLayer)
		{
			IImage * iimg = NULL;
			if (!l->QueryImage(&iimg)) return FALSE;
			double xllCenter=0, yllCenter=0, dx=0, dy=0;
			long width=0, height=0;
			
			iimg->get_OriginalXllCenter(&xllCenter);
			iimg->get_OriginalYllCenter(&yllCenter);
			iimg->get_OriginalDX(&dx);
			iimg->get_OriginalDY(&dy);
			iimg->get_OriginalWidth(&width);
			iimg->get_OriginalHeight(&height);	
			l->extents = Extent( xllCenter, xllCenter + dx*width, yllCenter, yllCenter + dy*height );
			iimg->Release();
			iimg = NULL;
			return TRUE;
		}
		else if (l->type == ShapefileLayer)
		{
			IShapefile * ishp = NULL;
			if (!l->QueryShapefile(&ishp)) return FALSE;
			IExtents * box = NULL;
			ishp->get_Extents(&box);
			double xm,ym,zm,xM,yM,zM;
			box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
			l->extents = Extent(xm,xM,ym,yM);
			box->Release();
			box = NULL;
			ishp->Release();
			ishp=NULL;
			return TRUE;
		}
		else return FALSE;

	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return FALSE;
	}
}

#pragma endregion

#pragma region Methods
// *************************************************
//			LockWindow()						  
// *************************************************
void CMapView::LockWindow(short LockMode)
{
	if( LockMode == lmUnlock )
	{
		_lockCount--;
		if( _lockCount <= 0 )
		{
			_lockCount = 0;
			DoUpdateTiles();
			ReloadImageBuffers();
			InvalidateControl();
		}
	}
	else if( LockMode == lmLock )
		_lockCount++;
}

// *************************************************
//			Resize()						  
// *************************************************
void CMapView::Resize(long Width, long Height)
{
	POINTL pl;
	pl.x = Width;
	pl.y = Height;

	POINTF pf;
	pf.x = (float)Width;
	pf.y = (float)Height;

	TransformCoords( &pl, &pf, XFORMCOORDS_SIZE | XFORMCOORDS_CONTAINERTOHIMETRIC );

	CSize size;
	size.cx = pl.x;
	size.cy = pl.y;
	CDC *dc = GetDC();
	dc->HIMETRICtoDP( &size );
	ReleaseDC( dc );

	CRect rect;
	GetRectInContainer( rect );
	rect.right = rect.left + size.cx;
	rect.bottom = rect.top + size.cy;

	SetRectInContainer( rect );

	OnSize( SIZE_RESTORED, size.cx, size.cy );
}

// *************************************************
//			Redraw2()						  
// *************************************************
void CMapView::Redraw2(tkRedrawType redrawType)
{
	RedrawCore(redrawType, redrawType != RedrawTempObjectsOnly, false);
}

// *************************************************
//			Redraw3()						  
// *************************************************
void CMapView::RedrawCore( tkRedrawType redrawType, bool updateTiles, bool atOnce )
{
	if (updateTiles) {
		DoUpdateTiles();
	}

	switch (redrawType)
	{
		case tkRedrawType::RedrawAll:
			_canUseLayerBuffer = FALSE;
			ReloadImageBuffers();
			break;
		case tkRedrawType::RedrawSkipDataLayers:
			_canUseMainBuffer = false;
			break;
		case tkRedrawType::RedrawTempObjectsOnly:
			// do nothing, simply invalidate control
			break;
	}
	if (atOnce){
		this->Refresh();
	}
	else {
		InvalidateControl();
	}
}

// *************************************************
//			Redraw()						  
// *************************************************
void CMapView::Redraw()
{
	Redraw2(tkRedrawType::RedrawAll);
}

// *************************************************
//			ShowToolTip()						  
// *************************************************
void CMapView::ShowToolTip(LPCTSTR Text, long Milliseconds)
{
	_ttip.UpdateTipText(Text,this,IDC_TTBTN);
	_ttip.SetDelayTime(TTDT_AUTOPOP,Milliseconds);
	KillTimer(HIDETEXT);
	SetTimer(SHOWTEXT,0,NULL);

	if( Milliseconds < 0 )
		Milliseconds = 0;

	SetTimer(HIDETEXT,Milliseconds,NULL);
}

// *************************************************
//			GetErrorMsg()						  
// *************************************************
BSTR CMapView::GetErrorMsg(long ErrorCode)
{
	return A2BSTR(ErrorMsg(ErrorCode));
}

// *************************************************
//			GetLastErrorCode()						  
// *************************************************
long CMapView::GetLastErrorCode()
{
	long lec = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return lec;
}

// *************************************************
//			ErrorMessage()						  
// *************************************************
inline void CMapView::ErrorMessage(long ErrorCode)
{
	USES_CONVERSION;
	_lastErrorCode = ErrorCode;
	if( _globalCallback != NULL) 
		_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(_lastErrorCode)));
	return;
}

// *************************************************
//			IsSameProjection()						  
// *************************************************
BOOL CMapView::IsSameProjection(LPCTSTR proj4_a, LPCTSTR proj4_b)
{
	ProjectionTools * pt = new ProjectionTools();
	bool rt = pt->IsSameProjection(proj4_a, proj4_b);
	delete pt;

	return (rt ? TRUE : FALSE);
}

// *************************************************
//			IsTIFFGrid()						  
// *************************************************
BOOL CMapView::IsTIFFGrid(LPCTSTR Filename)
{
	try
	{
		TIFF *tiff = XTIFFOpen((char *)Filename, "r"); // TIFF-level descriptor
		if (tiff)
		{
			int w=0, h=0;

			tdir_t d = 0;
			TIFFSetDirectory(tiff,d);

			uint32 SamplesPerPixel = 0;

			TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH, &w);
			TIFFGetField(tiff,TIFFTAG_IMAGELENGTH, &h);
			TIFFGetField(tiff,TIFFTAG_SAMPLESPERPIXEL, &SamplesPerPixel);

			uint16 photo = 0;
			// If it's a color-mapped palette, consider it an image --
			// it's probably an image (USGS DLG or USGS Quad Map most commonly)
			TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &photo);

			XTIFFClose(tiff);

			if(photo == PHOTOMETRIC_PALETTE) // && SamplesPerPixel != 1)
			{
				return false;
			}

			else if (SamplesPerPixel == 1)
				return true;
			else
				return false;
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}
#pragma endregion

#pragma region LayerUpdate

#pragma endregion

#pragma region Obsolete

// ***************************************************************
//		AlignShapeLayerAndShapes()
// ***************************************************************
// Refreshes drawing options for shapes (old implementation), creates
// or deletes if necessary. If the shape count was changed, the options
// will be initialize with default values
void CMapView::AlignShapeLayerAndShapes(Layer * layer)
{
	return;
}

// *********************************************************
//		ApplyLegendColors()
// *********************************************************
BOOL CMapView::ApplyLegendColors(LPDISPATCH pLegend)
{
	this->ErrorMessage(tkMETHOD_NOT_IMPLEMENTED);
	return FALSE;
}

// *********************************************************
//		GetColorScheme()
// *********************************************************
LPDISPATCH CMapView::GetColorScheme(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,_allLayers) )
	{	
		Layer * l = _allLayers[LayerHandle];
		if( l->type == ShapefileLayer )
		{	
			return NULL;	// probably return ShapeDrawingOptions ?
		}
		else if(l->type == ImageLayer)
		{
 			// redirected to image color scheme for backward compatibility
			IGridColorScheme* scheme = NULL;
			IImage* img = this->GetImage(LayerHandle);
			if (img != NULL)
			{
				img->get_CustomColorScheme(&scheme);
				img->Release();
			}
			else
			{
				ErrorMessage(tkUNEXPECTED_LAYER_TYPE);	
			}
			return scheme;
		}
		else
		{
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);	
			return NULL;
		}
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return NULL;
	}
}

// *********************************************************
//		GetGridFileName()
// *********************************************************
BSTR CMapView::GetGridFileName(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString retval;

	if(IS_VALID_LAYER(LayerHandle,_allLayers))
	{
		// redirected to image class for backward compatibility
		IImage* img = this->GetImage(LayerHandle);
		if (img != NULL)
		{	
			BSTR gridName;
			img->get_SourceGridName(&gridName);
			img->Release();
			return gridName;
		}
		else
		{
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
			return retval.AllocSysString();
		}
	}
	else
	{
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return retval.AllocSysString();
	}
}



#pragma endregion

#pragma region Serial number
// *********************************************************
//		Crypt()
// *********************************************************
CString CMapView::Crypt(CString str)
{
	CString tmp;

	int len = str.GetLength();
	int end = len;
	int cur = 0;

	str.MakeUpper();

	// convert to uppercase and remove non alpha chars
	for (int i = 0; i < end && cur < 7; i++)
	{
		char t = str[i];
		if (t >= 'A' && t <= 'Z')
		{
			tmp.AppendChar(t);
			cur++;
		}
	}

	for (int j = cur; j < 7; j++)
		tmp.AppendChar((char)((rand() % 26) + 64));

	//	PrepString(str, tmp);

	int curPosition = 0;
	int offset = 0;
	for (int i = 0; i < 7; i++)
	{
		offset = (int)(tmp[6 - i] - 'A') + 7;
		curPosition += offset; // spin the decoder wheel to get then encoded character
		curPosition %= _valsLen; // wrap around the "end" of the wheel if needed
		tmp.AppendChar(_vals[curPosition]); // read what the magic wheel says
	}

	return tmp;
}

// *********************************************************
//		VerifySerial()
// *********************************************************
bool CMapView::VerifySerial(CString str)
{
	return Crypt(str) == str;
}
#pragma endregion

