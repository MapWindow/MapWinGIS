#include "StdAfx.h"
#include "Map.h"
#undef HOST_FILLORDER /* MWGIS-182 */
#include "Projections.h"
#include "xtiffio.h"  /* for TIFF */
#undef HOST_FILLORDER /* MWGIS-182 */
#include "geotiffio.h" /* for GeoTIFF */
#include "Gdipluspixelformats.h"
#include "tiff.h"
#include "geotiff.h"
#include "geo_normalize.h"
#include "geovalues.h"
#include "tiffio.h"

#if GDAL_VERSION_MAJOR < 3
#include "tiffiop.h"
#endif

// ReSharper disable CppUseAuto

#pragma region Rotation
// ****************************************************************
//		GetRotatedExtent()
// ****************************************************************
//ajp (June 2010)
IPoint* CMapView::GetBaseProjectionPoint(double rotPixX, double rotPixY)
{
	IPoint* curPoint = nullptr;
	long basePixX = 0, basePixY = 0;
	double baseProjX = 0, baseProjY = 0;

	CoCreateInstance(CLSID_Point, nullptr, CLSCTX_INPROC_SERVER, IID_IPoint, (void**)&curPoint);

	if (_rotate == nullptr || _rotate->degAngle == 0.0)
	{
		basePixX = (long)rotPixX;
		basePixY = (long)rotPixY;
	}
	else
	{
		_rotate->getOriginalPixelPoint((long)rotPixX, (long)rotPixY, &basePixX, &basePixY);
	}
	PixelToProjection(basePixX, basePixY, baseProjX, baseProjY);

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
	IExtents* box = nullptr;

	rotExtent = _extents;
	ComHelper::CreateExtents(&box);
	box->SetBounds(rotExtent.left, rotExtent.bottom, 0, rotExtent.right, rotExtent.top, 0);

	if (_rotateAngle == 0)
		return box;

	if (_rotate == nullptr)
		_rotate = new Rotate();

	_rotate->calcRotatedExtent(_viewWidth, _viewHeight);
	rotExtent.right += (_rotate->xAxisDiff * _inversePixelPerProjectionX);
	rotExtent.bottom -= (_rotate->yAxisDiff * _inversePixelPerProjectionY);
	rotExtent.left -= (_rotate->xAxisDiff * _inversePixelPerProjectionX);
	rotExtent.top += (_rotate->yAxisDiff * _inversePixelPerProjectionY);
	box->SetBounds(rotExtent.left, rotExtent.bottom, 0, rotExtent.right, rotExtent.top, 0);

	return box;
}
#pragma endregion

#pragma region Images

// ***************************************************
//  GetImageLayerPercentTransparent
// ***************************************************
float CMapView::GetImageLayerPercentTransparent(long LayerHandle)
{
	if (IS_VALID_LAYER(LayerHandle, _allLayers))
	{
		Layer* l = _allLayers[LayerHandle];
		if (l->IsImage())
		{
			IImage* iimg = nullptr;

			if (!l->QueryImage(&iimg))
				return 1.0;

			double val;
			iimg->get_TransparencyPercent(&val);
			iimg->Release(); iimg = nullptr;
			return static_cast<float>(val);
		}

		ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
		return 0.0f;
	}

	ErrorMessage(tkINVALID_LAYER_HANDLE);
	return 0.0f;
}

// ***************************************************
//  GetImageLayerPercentTransparent
// ***************************************************
// Will be deprecated
void CMapView::SetImageLayerPercentTransparent(long LayerHandle, float newValue)
{
	if (newValue < 0.0)		newValue = 0.0;
	else if (newValue > 1.0)	newValue = 1.0;

	if (IS_VALID_LAYER(LayerHandle, _allLayers))
	{
		Layer* l = _allLayers[LayerHandle];
		if (l->IsImage())
		{
			IImage* iimg = nullptr;

			//if( iimg == NULL )	return;
			if (!l->QueryImage(&iimg))
				return;
			iimg->put_TransparencyPercent(newValue);
			iimg->Release(); iimg = nullptr;
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
VARIANT_BOOL CMapView::SetImageLayerColorScheme(LONG layerHandle, IDispatch* colorScheme)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (IS_VALID_LAYER(layerHandle, _allLayers))
	{
		// redirected to image class for backward compatibility
		IGridColorScheme* scheme = nullptr;
		colorScheme->QueryInterface(IID_IGridColorScheme, (void**)&scheme);
		if (scheme)
		{
			IImage* img = this->GetImage(layerHandle);
			if (img != nullptr)
			{
				img->put_CustomColorScheme(scheme);
				img->Release();
				scheme->Release();
				return VARIANT_TRUE;
			}

			scheme->Release();
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
			return VARIANT_FALSE;
		}

		ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
		return VARIANT_FALSE;
	}

	ErrorMessage(tkINVALID_LAYER_HANDLE);
	return VARIANT_FALSE;
}

// deprecated
void CMapView::UpdateImage(LONG layerHandle)
{
	ErrorMessage(tkMETHOD_DEPRECATED);
}

// ***************************************************************
//		LayerIsEmpty()
// ***************************************************************
bool CMapView::LayerIsEmpty(long layerHandle)
{
	if (IS_VALID_LAYER(layerHandle, _allLayers))
	{
		Layer* l = _allLayers[layerHandle];
		if (!l) return true;
		return l->IsEmpty();
	}
	return true;
}

// ***************************************************************
//		AdjustLayerExtents()
// ***************************************************************
BOOL CMapView::AdjustLayerExtents(long layerHandle)
{
	if (IS_VALID_LAYER(layerHandle, _allLayers))
	{
		Layer* l = _allLayers[layerHandle];
		if (!l->get_Object()) return FALSE;
		return l->UpdateExtentsFromDatasource() ? TRUE : FALSE;
	}

	ErrorMessage(tkINVALID_LAYER_HANDLE);
	return FALSE;
}

#pragma endregion

#pragma region Methods
// *************************************************
//			LockWindow()						  
// *************************************************
void CMapView::LockWindow(short LockMode)
{
	if (LockMode == lmUnlock)
	{
		if (_lockCount == 0)
		{
			return;    // no need to schedule more buffer reloads
		}

		_lockCount--;

		if (_lockCount == 0)
		{
			_lockCount = 0;
			RedrawCore(RedrawAll, false, true);
		}
	}
	else if (LockMode == lmLock)
	{
		_lockCount++;
	}
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

	TransformCoords(&pl, &pf, XFORMCOORDS_SIZE | XFORMCOORDS_CONTAINERTOHIMETRIC);

	CSize size;
	size.cx = pl.x;
	size.cy = pl.y;
	CDC* dc = GetDC();
	dc->HIMETRICtoDP(&size);
	ReleaseDC(dc);

	CRect rect;
	GetRectInContainer(rect);
	rect.right = rect.left + size.cx;
	rect.bottom = rect.top + size.cy;

	SetRectInContainer(rect);

	OnSize(SIZE_RESTORED, size.cx, size.cy);
}

// *************************************************
//			Redraw2()						  
// *************************************************
void CMapView::Redraw2(tkRedrawType redrawType)
{
	RedrawCore(redrawType, false);
}

// *************************************************
//			Redraw3()						  
// *************************************************
void CMapView::Redraw3(tkRedrawType redrawType, VARIANT_BOOL reloadTiles)
{
	RedrawCore(redrawType, false, reloadTiles != 0);
}

// *************************************************
//			ScheduleVolatileRedraw()						  
// *************************************************
void CMapView::ScheduleVolatileRedraw()
{
	_canUseVolatileBuffer = false;
	_canUseMainBuffer = false;
}

// *************************************************
//			ScheduleLayerRedraw()						  
// *************************************************
void CMapView::ScheduleLayerRedraw()
{
	_canUseLayerBuffer = false;
	_canUseVolatileBuffer = false;
	_canUseMainBuffer = false;
}

// *************************************************
//			RedrawCore()						  
// *************************************************
void CMapView::RedrawCore(tkRedrawType redrawType, bool atOnce, bool forceReloadTiles /*= false */)
{
	bool reloaded = ReloadTiles(forceReloadTiles || redrawType == RedrawAll);

	if (reloaded && redrawType == RedrawMinimal)
	{
		// we need at least this much to see that tiles are updated
		redrawType = RedrawSkipDataLayers;
	}

	// no breaks are needed; it's intentional; redraw type of higher order leads to redraw of lower levels
	switch (redrawType)
	{
	case tkRedrawType::RedrawAll:
		_canUseLayerBuffer = false;
		ReloadBuffers();

	case tkRedrawType::RedrawSkipDataLayers:
		_canUseVolatileBuffer = false;

	case tkRedrawType::RedrawSkipAllLayers:
		_canUseMainBuffer = false;

	case tkRedrawType::RedrawMinimal:
		// do nothing, simply invalidate control
		break;
	}

	if (atOnce) {
		Refresh();
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
	_ttip.UpdateTipText(Text, this, IDC_TTBTN);
	_ttip.SetDelayTime(TTDT_AUTOPOP, Milliseconds);
	KillTimer(HIDETEXT);
	SetTimer(SHOWTEXT, 0, nullptr);

	if (Milliseconds < 0)
		Milliseconds = 0;

	SetTimer(HIDETEXT, Milliseconds, nullptr);
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
inline void CMapView::ErrorMessage(long ErrorCode, tkCallbackVerbosity verbosity)
{
	if (verbosity < m_globalSettings.callbackVerbosity) {
		return;
	}

	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("Map", _globalCallback, m_key, ErrorMsg(_lastErrorCode));
}

// *************************************************
//			IsSameProjection()						  
// *************************************************
BOOL CMapView::IsSameProjection(LPCTSTR proj4A, LPCTSTR proj4B)
{
	ProjectionTools* pt = new ProjectionTools();
	bool rt = pt->IsSameProjection(proj4A, proj4B);
	delete pt;

	return (rt ? TRUE : FALSE);
}

// *************************************************
//			IsTIFFGrid()						  
// *************************************************
BOOL CMapView::IsTIFFGrid(LPCTSTR filename)
{
	CComBSTR bstrName(filename);
	VARIANT_BOOL vb;
	GetUtils()->IsTiffGrid(bstrName, &vb);
	return vb ? TRUE : FALSE;
}
#pragma endregion

#pragma region Obsolete

// ***************************************************************
//		AlignShapeLayerAndShapes()
// ***************************************************************
// Refreshes drawing options for shapes (old implementation), creates
// or deletes if necessary. If the shape count was changed, the options
// will be initialize with default values
void CMapView::AlignShapeLayerAndShapes(Layer* layer)
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
	if (IS_VALID_LAYER(LayerHandle, _allLayers))
	{
		Layer* l = _allLayers[LayerHandle];
		if (l->IsShapefile())
		{
			return nullptr;	// probably return ShapeDrawingOptions ?
		}
		else if (l->IsImage())
		{
			// redirected to image color scheme for backward compatibility
			IGridColorScheme* scheme = nullptr;
			IImage* img = this->GetImage(LayerHandle);
			if (img != nullptr)
			{
				img->get_CustomColorScheme(&scheme);
				img->Release();
				if (!scheme)
				{
					VARIANT_BOOL vb;
					img->get_IsGridProxy(&vb);
					if (vb) {
						img->get_GridProxyColorScheme(&scheme);
					}
				}
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
			return nullptr;
		}
	}
	else
	{
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return nullptr;
	}
}

// *********************************************************
//		GetGridFileName()
// *********************************************************
BSTR CMapView::GetGridFileName(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString retval;

	if (IS_VALID_LAYER(LayerHandle, _allLayers))
	{
		// redirected to image class for backward compatibility
		IImage* img = this->GetImage(LayerHandle);
		if (img != nullptr)
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

// ReSharper restore CppUseAuto