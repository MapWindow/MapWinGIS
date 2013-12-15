#pragma region Include
#include "stdafx.h"
#include "MapWinGis.h"
#include "Map.h"
#include "Image.h"
#include "Projections.h"
#include "Shapefile.h"
#include "Labels.h"
#include "Tiles.h"

#include "xtiffio.h"  /* for TIFF */
#include "geotiffio.h" /* for GeoTIFF */
#include "Gdipluspixelformats.h"
#include "tiff.h"
#include "geotiff.h"
#include "geo_normalize.h"
#include "geovalues.h"
#include "tiffio.h"
#include "tiffiop.h"

#pragma endregion

#pragma region REGION Rotation
//ajp (June 2010)
IPoint* CMapView::GetBaseProjectionPoint(double rotPixX, double rotPixY)
{
  IPoint *curPoint = NULL;
  long basePixX = 0, basePixY = 0; 
  double baseProjX = 0, baseProjY = 0; 
	
  CoCreateInstance( CLSID_Point, NULL, CLSCTX_INPROC_SERVER, IID_IPoint, (void**)&curPoint);

	if (m_Rotate == NULL || m_Rotate->degAngle == 0.0)
  {
    basePixX = (long)rotPixX;
    basePixY = (long)rotPixY;
  }
  else
  {
     m_Rotate->getOriginalPixelPoint((long) rotPixX, (long) rotPixY, &basePixX, &basePixY);
  }
  PixelToProjection( basePixX, basePixY, baseProjX, baseProjY);
  
  curPoint->put_X(baseProjX);
  curPoint->put_Y(baseProjY);
  return curPoint;
}

//ajp (June 2010)
IExtents* CMapView::GetRotatedExtent()
{
  Extent rotExtent;
	IExtents * box = NULL;

	rotExtent = extents;
	CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&box);
  box->SetBounds( rotExtent.left, rotExtent.bottom, 0, rotExtent.right, rotExtent.top, 0 );

	if (m_RotateAngle == 0)
	  return box;

	if (m_Rotate == NULL)
		m_Rotate = new Rotate();

	m_Rotate->calcRotatedExtent(m_viewWidth, m_viewHeight);
  rotExtent.right += (m_Rotate->xAxisDiff * m_inversePixelPerProjectionX);
  rotExtent.bottom -= (m_Rotate->yAxisDiff * m_inversePixelPerProjectionY);
  rotExtent.left -= (m_Rotate->xAxisDiff * m_inversePixelPerProjectionX);
  rotExtent.top += (m_Rotate->yAxisDiff * m_inversePixelPerProjectionY);
  box->SetBounds( rotExtent.left, rotExtent.bottom, 0, rotExtent.right, rotExtent.top, 0 );

	return box;
}
#pragma endregion

#pragma region REGION Images
// ****************************************************************
//		ReloadImageBuffers()
// ****************************************************************
void CMapView::ReloadImageBuffers()
{
	IImage * iimg = NULL;
	for(size_t i = 0; i < m_activeLayers.size(); i++ )
	{
		Layer * l = m_allLayers[m_activeLayers[i]];

		if ((l->type == ImageLayer) && (l->flags & Visible))
		{
			//l->object->QueryInterface(IID_IImage,(void**)&iimg);
			//if( iimg != NULL ) 
			if (l->QueryImage(&iimg))
			{	
				((CImageClass*)iimg)->_bufferReloadIsNeeded = true;
				iimg->Release();
			}
		}
	}
}

// ***************************************************
//  GetImageLayerPercentTransparent
// ***************************************************
// Will be deprecated
float CMapView::GetImageLayerPercentTransparent(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,m_allLayers) )
	{
		Layer * l = m_allLayers[LayerHandle];
		if( l->type == ImageLayer )
		{
			IImage * iimg = NULL;
			//l->object->QueryInterface(IID_IImage,(void**)&iimg);
			//if( iimg == NULL )	
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

	if( IS_VALID_LAYER(LayerHandle,m_allLayers) )
	{
		Layer * l = m_allLayers[LayerHandle];
		if( l->type == ImageLayer )
		{	
			IImage * iimg = NULL;
			//l->object->QueryInterface(IID_IImage,(void**)&iimg);
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

	if(IS_VALID_LAYER(LayerHandle,m_allLayers))
	{
		Layer * l = m_allLayers[LayerHandle];
		if(l->type == ImageLayer)
		{
			ImageLayerInfo * ili = (ImageLayerInfo*)(l->addInfo);

			if(ili->ColorScheme != NULL)
				ili->ColorScheme->Release();

			ili->ColorScheme = ColorScheme;

			if(ColorScheme!= NULL)
				ColorScheme->AddRef();

			return VARIANT_TRUE;
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
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// ***************************************************************
//		AdjustLayerExtents()
// ***************************************************************
BOOL CMapView::AdjustLayerExtents(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,m_allLayers) )
	{	
		Layer * l = m_allLayers[LayerHandle];
		if(l->object == NULL) return FALSE;
		
		if (l->type == ImageLayer)
		{
			IImage * iimg = NULL;
			//l->object->QueryInterface(IID_IImage,(void**)&iimg);
			//if( iimg == NULL )return FALSE;
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
			//l->object->QueryInterface(IID_IShapefile,(void**)&ishp);
			//if (ishp == NULL) return FALSE;
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

#pragma region REGION Methods
void CMapView::LockWindow(short LockMode)
{
	if( LockMode == lmUnlock )
	{
		m_lockCount--;
		if( m_lockCount <= 0 )
		{
			m_lockCount = 0;
			((CTiles*)m_tiles)->LoadTiles(this);
			ReloadImageBuffers();
			InvalidateControl();
		}
	}
	else if( LockMode == lmLock )
		m_lockCount++;
}

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

void CMapView::Redraw()
{
	m_canbitblt = FALSE;	// layers buffer won't be used
	
	((CTiles*)m_tiles)->LoadTiles((void*)this);
	
	ReloadImageBuffers();
	InvalidateControl();
}

void CMapView::ShowToolTip(LPCTSTR Text, long Milliseconds)
{
	m_ttip.UpdateTipText(Text,this,IDC_TTBTN);
	m_ttip.SetDelayTime(TTDT_AUTOPOP,Milliseconds);
	KillTimer(HIDETEXT);
	SetTimer(SHOWTEXT,0,NULL);

	if( Milliseconds < 0 )
		Milliseconds = 0;

	SetTimer(HIDETEXT,Milliseconds,NULL);
}

BSTR CMapView::GetErrorMsg(long ErrorCode)
{
	return A2BSTR(ErrorMsg(ErrorCode));
}

long CMapView::GetLastErrorCode()
{
	long lec = m_lastErrorCode;
	m_lastErrorCode = tkNO_ERROR;
	return lec;
}

/***********************************************************************/
/*						ErrorMessage()						           */
/***********************************************************************/
inline void CMapView::ErrorMessage(long ErrorCode)
{
	USES_CONVERSION;
	m_lastErrorCode = ErrorCode;
	if( m_globalCallback != NULL) 
		m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
	return;
}

BOOL CMapView::IsSameProjection(LPCTSTR proj4_a, LPCTSTR proj4_b)
{
	ProjectionTools * pt = new ProjectionTools();
	bool rt = pt->IsSameProjection(proj4_a, proj4_b);
	delete pt;

	return (rt ? TRUE : FALSE);
}

BOOL CMapView::IsTIFFGrid(LPCTSTR Filename)
{
	try
	{
		TIFF 	*tiff=(TIFF*)0;  // TIFF-level descriptor 
		const TIFFCodec *tCodec=(TIFFCodec*)0;

		tiff = XTIFFOpen((char *)Filename, "r"); 
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

#pragma region REGION LayerUpdate
// **********************************************************
//			GetDrawingLabels()
// **********************************************************
// Deletes dynamically alocated frames info for all layers; drops isDrawn flag
void CMapView::ClearLabelFrames()
{
	// clear frames for regular labels
	for (int i = 0; i < (int)m_activeLayers.size(); i++)
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		if( l != NULL )
		{	
			// charts
			if (l->type == ShapefileLayer)
			{
				IShapefile * sf = NULL;
				//l->object->QueryInterface(IID_IShapefile, (void**)&sf);
				//if (sf != NULL)
				if (l->QueryShapefile(&sf))
				{
					((CShapefile*)sf)->ClearChartFrames();
					sf->Release();
				}
			}
			
			// labels
			ILabels* LabelsClass = l->get_Labels();
			if (LabelsClass == NULL) continue;
			
			CLabels* coLabels = static_cast<CLabels*>(LabelsClass);
			coLabels->ClearLabelFrames();
			LabelsClass->Release(); LabelsClass = NULL;
		}
	}

	// clear frames for drawing labels
	for(size_t j = 0; j < m_activeDrawLists.size(); j++ )
	{
		bool isSkip = false;
		for (size_t i = 0; i < DrawingLayerInVisilbe.size(); i++)
		{
			if (DrawingLayerInVisilbe[i] == j)
			{
				isSkip = true;	// skip if this layer is set invisiable
				break;  
			}
		}
		if(isSkip) 
			continue;

		DrawList * dlist = m_allDrawLists[m_activeDrawLists[j]];
		if( IS_VALID_PTR(dlist) )
		{
			if (dlist->listType == dlSpatiallyReferencedList)
			{
				CLabels* coLabels = static_cast<CLabels*>(dlist->m_labels);
				coLabels->ClearLabelFrames();
			}
		}
	}
}

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
#pragma endregion

#pragma region REGION Others
BOOL CMapView::ApplyLegendColors(LPDISPATCH pLegend)
{
	this->ErrorMessage(tkPROPERTY_DEPRECATED);
	return FALSE;	// deprecated
}

LPDISPATCH CMapView::GetColorScheme(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,m_allLayers) )
	{	
		Layer * l = m_allLayers[LayerHandle];
		if( l->type == ShapefileLayer )
		{	
			return NULL;	// probably return ShapeDrawingOptions ?
		}
		else if(l->type == ImageLayer)
		{
			ImageLayerInfo* ili = (ImageLayerInfo*)(l->addInfo);
			if( ili->ColorScheme != NULL)
				ili->ColorScheme->AddRef();

			return (ili->ColorScheme);
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

inline double CMapView::makeVal( const char * sVal )
{
	double val = 0.0;

	if( sVal != NULL )
	{	
		for( size_t i = 0; i < _tcslen( sVal ); i++ )
		{
			char c = sVal[i];
			if( isalpha(c) || isdigit(c) )
			{	c = toupper(c);

				//Adjust by the ASCII value of 0
				c = c - 48;

				//90-48 = range
				val += ((double)c)*(42*pow((double)10, (double)i));
			}
		}
	}
	return val;
}


BSTR CMapView::GetGridFileName(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString retval;

	if(IS_VALID_LAYER(LayerHandle,m_allLayers))
	{
		Layer * l = m_allLayers[LayerHandle];
		if(l->type == ImageLayer)
		{
			ImageLayerInfo * ili = (ImageLayerInfo*)(l->addInfo);
			return ili->GridFileName.AllocSysString();
		}
		else
		{
			m_lastErrorCode = tkUNEXPECTED_LAYER_TYPE;
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
			return retval.AllocSysString();
		}
	}
	else
	{
		m_lastErrorCode = tkINVALID_LAYER_HANDLE;
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
		return retval.AllocSysString();
	}
}

void CMapView::SetGridFileName(LONG LayerHandle, LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(IS_VALID_LAYER(LayerHandle,m_allLayers))
	{
		Layer * l = m_allLayers[LayerHandle];
		if(l->type == ImageLayer)
		{
			ImageLayerInfo * ili = (ImageLayerInfo*)(l->addInfo);

			ili->GridFileName = newVal;
			return;
		}
		else
		{
			m_lastErrorCode = tkUNEXPECTED_LAYER_TYPE;
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
			return;
		}
	}
	else
	{
		m_lastErrorCode = tkINVALID_LAYER_HANDLE;
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
		return;
	}
}

#pragma endregion

// *********************************************************
//		SetWaitCursor()
// *********************************************************
HCURSOR CMapView::SetWaitCursor()
{
   if (m_DisableWaitCursor)
		return NULL;

	HCURSOR oldCursor = ::GetCursor();
   
	CPoint cpos;
	GetCursorPos(&cpos);
	CRect wrect;
	GetWindowRect(&wrect);
	
	HWND wndActive = ::GetActiveWindow();
	if ((wndActive == this->GetSafeHwnd()) || (wndActive == this->GetParentOwner()->GetSafeHwnd()))
	{
		if( wrect.PtInRect(cpos) && (m_mapCursor != crsrUserDefined) && !m_DisableWaitCursor)
		{
			::SetCursor(LoadCursor(NULL, IDC_WAIT) );
		}
	}

   return oldCursor;
}

#pragma region REGION Serial number
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
		curPosition %= valsLen; // wrap around the "end" of the wheel if needed
		tmp.AppendChar(vals[curPosition]); // read what the magic wheel says
	}

	return tmp;
}

bool CMapView::VerifySerial(CString str)
{
	if (Crypt(str) == str)
		return true;
	else
		return false;
}

#pragma endregion
