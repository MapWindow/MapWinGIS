// tkRaster.cpp: implementation of the tkRaster class.
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
//11-4-2005 Rob Cairns. Changed class to cater for more generic formats
//10-25-2005 Rob Cairns. Made changes to LoadECW and LoadImageBuffer
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GdalRaster.h"
#include "Vector.h"
#include "gdalwarper.h"
#include "GridColorScheme.h"

using namespace std;

// *************************************************************
//	  Open()
// *************************************************************
bool GdalRaster::Open(CStringW filename, GDALAccess accessType)
{
	CStringA filenameA = Utility::ConvertToUtf8(filename);
	return OpenCore(filenameA, accessType);
}

// *************************************************************
//	  OpenCore()
// *************************************************************
bool GdalRaster::OpenCore(CStringA& filename, GDALAccess accessType)
{
	// Chris M 11/15/2005 - This function doesn't use any AFX calls,
	// and getting a static module state will make this function require unwinding.
	// This will mean that SEH exception handing can't be used, and we'll be unable
	// to catch GDAL crashes on GDALOpen. Therefore, don't use AFX_MANAGE_STATE...
	//	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// Chris M 11/15/2005 - Use a slightly stronger form of exception handing (SEH) here,
	// rather than the weaker C++ try/catch. This will catch GDAL's memory exceptions
	// when it cannot, for instance, access it's delay-loaded SID functionality. This
	// will allow us to return false properly, thus displaying "not supported" rather than
	// just doing nothing and leaving a blank map.

	bool retVal = false;
	_warped = false;

	__try
	{
		GDALAllRegister();
		
		if (_dataset == NULL)
			_dataset = GdalHelper::OpenRasterDatasetA(filename, accessType);
	
		if( _dataset == NULL ) 
		{
			return false;
		}

		_origWidth = _dataset->GetRasterXSize();
		_origHeight = _dataset->GetRasterYSize();			
		
		if(!ReadGeoTransform())
		{ 
			_origXllCenter = 0;
			_origYllCenter = 0;
			_origDx = 1;
			_origDy = 1;
		}

		_bandMinMax.clear();
		_bandMinMax.resize(_dataset->GetRasterCount());

		OpenDefaultBands();

		GDALRasterBand* band = _dataset->GetRasterBand(1);
		if (!band) {
			return false;
		}

		InitDataType(band);

		InitSettings(band);

		InitNoDataValue();

		RefreshExtents();

		return true;
	}
	__except(1)
	{
		CallbackHelper::ErrorMsg("Exception in LoadRaster function.");
	}

	return false;
}

// *************************************************************
//	  ApplyCustomColorScheme()
// *************************************************************
void GdalRaster::ApplyCustomColorScheme(IGridColorScheme * scheme)
{
	ComHelper::SetRef((IDispatch*)scheme, (IDispatch**)&_customColorScheme, true);
}

// *************************************************************
//	  ApplyPredefinedColorScheme()
// *************************************************************
void GdalRaster::ApplyPredefinedColorScheme(PredefinedColorScheme colorScheme)
{
	_predefinedColors = colorScheme;
	UpdatePredefinedColorScheme();
}

// *************************************************************
//	  SetActiveBandIndex()
// *************************************************************
bool GdalRaster::SetActiveBandIndex(int bandIndex)
{
	if (!ValidateBandIndex(bandIndex))
		return false;
	
	_activeBandIndex = bandIndex;
	UpdatePredefinedColorScheme();
	return true;
}

// *************************************************************
//	  ValidateBandIndex()
// *************************************************************
bool GdalRaster::ValidateBandIndex(int bandIndex)
{
	return bandIndex <= _nBands && bandIndex > 0;
}

// *************************************************************
//	  GetBand()
// *************************************************************
GDALRasterBand* GdalRaster::GetBand(int bandIndex)
{
	return _dataset->GetRasterBand(bandIndex);
}

// *************************************************************
//	  ComputeBandMinMax()
// *************************************************************
void GdalRaster::ComputeBandMinMax(GDALRasterBand* band, BandMinMax& minMax, bool force)
{
	if ((minMax.Calculated && !force) || !band) return;

	int gotMin = false;
	int gotMax = false;
	minMax.Min = band->GetMinimum(&gotMin);
	minMax.Max = band->GetMaximum(&gotMax);

	if (!gotMin || !gotMax)
	{
		if (_dataType == GDT_Byte && _imgType != PNG_FILE && _imgType != ADF_FILE && !_hasColorTable == false)
		{
			//For some reason GDAL does not seem to pick up the color table of a PNG binary image		
			minMax.Min = 0;
			minMax.Max = 255;
			CallbackHelper::AssertionFailed("Default min max values for the band are used.");
		}
		else
		{
			double adfMinMax[2];
			CPLErr result = band->ComputeRasterMinMax(FALSE, adfMinMax);
			minMax.Min = adfMinMax[0];
			minMax.Max = adfMinMax[1];
		}
	}

	minMax.Calculated = true;	
}

// *************************************************************
//	  InitNoDataValue()
// *************************************************************
void GdalRaster::InitNoDataValue()
{
	_hasTransparency = false;
	double nDV = 0;
	int pbSuccess = NULL;
	nDV = _poBandR->GetNoDataValue(&pbSuccess);

	if ((pbSuccess != NULL) && _hasColorTable)
	{
		const GDALColorEntry * poCE = GDALGetColorEntry(_colorTable, (int)nDV);
		if (poCE) {
			_transColor = RGB(poCE->c1, poCE->c2, poCE->c3);
		}
		else
		{
			OLE_COLOR clr = (OLE_COLOR)nDV;
			_transColor = RGB(GetRValue(clr), GetGValue(clr), GetBValue(clr));
		}
		_hasTransparency = true;
	}
	else if ((pbSuccess != NULL))
	{
		unsigned char nDVr = 0, nDVg = 0, nDVb = 0;
		if (_nBands == 1)
		{
			_transColor = RGB(255, 255, 255);
			_hasTransparency = true;
		}
		else
		{
			if (_poBandR != NULL) nDVr = (unsigned char)_poBandR->GetNoDataValue();
			if (_poBandG != NULL) nDVg = (unsigned char)_poBandG->GetNoDataValue();
			if (_poBandB != NULL) nDVb = (unsigned char)_poBandB->GetNoDataValue();
			_transColor = RGB(nDVr, nDVg, nDVb);
			_hasTransparency = true;
		}
	}
	else
	{
		_transColor = RGB(0, 0, 0);
		_hasTransparency = false;
	}
}

// *************************************************************
//	  InitDataType()
// *************************************************************
bool GdalRaster::InitDataType(GDALRasterBand* band)
{
	if (!band) return false;
	

	_dataType = band->GetRasterDataType();

	if (!((_dataType == GDT_Byte) || (_dataType == GDT_UInt16) ||
		(_dataType == GDT_Int16) || (_dataType == GDT_UInt32) ||
		(_dataType == GDT_Int32) || (_dataType == GDT_Float32) ||
		(_dataType == GDT_Float64) || (_dataType == GDT_CInt16) ||
		(_dataType == GDT_CInt32) || (_dataType == GDT_CFloat32) ||
		(_dataType == GDT_CFloat64)))
	{
		return false;
	}

	_genericType = GetSimplifiedDataType(band);

	return true;
}

// *************************************************************
//	  InitSettings()
// *************************************************************
GDALDataType GdalRaster::GetSimplifiedDataType(GDALRasterBand* band)
{
	if (!band) return GDT_Unknown;

	GDALDataType dataType = band->GetRasterDataType();
	switch (dataType)
	{
		case GDT_Byte:
			return GDT_Byte;
		case GDT_Float32:
		case GDT_Float64:
		case GDT_CFloat32:
		case GDT_CFloat64:
			return GDT_Float32;
		case GDT_CInt16:
		case GDT_CInt32:
		case GDT_Int16:
		case GDT_Int32:
		case GDT_UInt16:
		case GDT_UInt32:
			return GDT_Int32;
	}
	
	return GDT_Unknown;
}

// *************************************************************
//	  InitSettings()
// *************************************************************
void GdalRaster::InitSettings(GDALRasterBand* band)
{
	_activeBandIndex = 1;
	_histogram.Clear();
	_useHistogram = false;
	_palleteInterpretation = GPI_Gray;
	_imageQuality = 100;
	_predefinedColors = FallLeaves;

	_colorTable = band->GetColorTable();

	if (_colorTable != NULL)
	{
		_hasColorTable = true;
		_palleteInterpretation = _colorTable->GetPaletteInterpretation();
	}
	else
	{
		_hasColorTable = false;
	}
}

// *************************************************************
//	  OpenDefaultBands()
// *************************************************************
void GdalRaster::OpenDefaultBands()
{
	_nBands = _dataset->GetRasterCount();

	for (int band = 1; band <= _nBands; band++)
	{
		if (band == 1)
		{
			if (_poBandR) {
				CallbackHelper::AssertionFailed("Red band is supposed to NULL.");
			}
			else {
				_poBandR = _dataset->GetRasterBand(band);
			}
		}

		if (band == 2)
		{
			if (_poBandG) {
				CallbackHelper::AssertionFailed("Green band is supposed to NULL.");
			}
			else {
				_poBandG = _dataset->GetRasterBand(band);
			}
		}

		if (band == 3)
		{
			if (_poBandB) {
				CallbackHelper::AssertionFailed("Blue band is supposed to NULL.");
			}
			else {
				_poBandB = _dataset->GetRasterBand(band);
			}
		}
	}
}

// *************************************************************
//	  ReadGeoTransform()
// *************************************************************
bool GdalRaster::ReadGeoTransform()
{
	m_globalSettings.SetGdalUtf8(true);		// otherwise there can be problems when reading world file
	// as dataset filename is already stored as UTF8

	double adfGeoTransform[6];
	bool success = _dataset->GetGeoTransform(adfGeoTransform) == CE_None;

	m_globalSettings.SetGdalUtf8(false);

	if (!success) return false;

	_origDx = adfGeoTransform[1];
	_origDy = adfGeoTransform[5];
	
	// adfGeoTransform[0]&[3] refer to the corner (not the center) of the left top pixel
	_origXllCenter = adfGeoTransform[0] + _origDx / 2.0;
	_origYllCenter = adfGeoTransform[3] + _origDy / 2.0;

	// we got top corner and now we'll get coordinates of the bottom left corner; 
	// dy should be changed respectively as map coordinates will be increasing while moving from bottom to top
	_origYllCenter += _origDy * (_origHeight - 1);
	_origDy *= -1;

	// Check if we need a warped VRT for this file.
	if (adfGeoTransform[1] < 0.0 || adfGeoTransform[2] != 0.0 || adfGeoTransform[4] != 0.0 || adfGeoTransform[5] > 0.0)
	{
		GDALDataset* mGdalDataset = NULL;
		mGdalDataset = (GDALDataset *)GDALAutoCreateWarpedVRT(_dataset, NULL, NULL, GRA_NearestNeighbour, 0.2, NULL);
		if (mGdalDataset)
		{
			_dataset->Dereference();
			_dataset = mGdalDataset;
			_warped = true;

			if (_dataset->GetGeoTransform(adfGeoTransform) == CE_None)
			{
				_origDx = adfGeoTransform[1];
				_origDy = adfGeoTransform[5];
				
				// adfGeoTransform[0]&[3] refer to the corner (not the center) of the left top pixel
				_origXllCenter = adfGeoTransform[0] + _origDx / 2;
				_origYllCenter = adfGeoTransform[3] + _origDy / 2;

				_origWidth = _dataset->GetRasterXSize();
				_origHeight = _dataset->GetRasterYSize();

				// we got top corner and now we'll get coordinates of the bottom left corner; 
				// dy should be changed respectively as map coordinates will be increasing while moving from bottom to top
				_origYllCenter += _origDy * (_origHeight - 1);
				_origDy *= -1;
			}
		}
	}
	
	return true;	
}

// *********************************************************
//		RefreshExtents()
// *********************************************************
// When loading buffer the extents are used, so they should be refreshed after each 
// change of orig_XllCenter, orig_YllCenter, orig_dX, orig_dY
void GdalRaster::RefreshExtents()
{
	_extents.left = _origXllCenter - _origDx * 0.5;
	_extents.right = _origXllCenter + (_origDx * (_origWidth-1));
	_extents.top = _origYllCenter + (_origDy * (_origHeight-1)); 
	_extents.bottom = _origYllCenter - _origDy * 0.5;
}

// *********************************************************
//		Dereference()
// *********************************************************
int GdalRaster::Dereference()
{
	if (_dataset != NULL)
		return _dataset->Dereference();
	else
		return -1;
}

// *********************************************************
//		Close()
// *********************************************************
void GdalRaster::Close()
{
	if (_dataset != NULL)
	{
		_dataset->Dereference();
		delete _dataset;
		_dataset = NULL;
		_poBandR = NULL;
		_poBandG = NULL;
		_poBandB = NULL;
	}

	if (_predefinedColorScheme)
	{
		_predefinedColorScheme->Clear();
		_predefinedColorScheme->Release();
		_predefinedColorScheme = NULL;
	}

	if (_customColorScheme) {
		_customColorScheme->Clear();
		_customColorScheme->Release();
		_customColorScheme = NULL;
	}

	_histogram.Clear();

	_allowAsGrid = grForGridsOnly;
	_activeBandIndex = 1;
	_warped = false;
}

// *********************************************************
//		LoadBuffer()
// *********************************************************
bool GdalRaster::LoadBuffer(colour ** ImageData, Extent& screenExtents, CStringW filename,
							   tkInterpolationMode downsamplingMode, bool setToGrey, double mapUnitsPerPixel)
{
	if (!ReopenDatasetIfNeeded(filename)) {
		return false;
	}

	if (_extents.left > screenExtents.right || 
		_extents.right < screenExtents.left || 
		_extents.top < screenExtents.bottom || 
		_extents.bottom > screenExtents.top)
    {	
		// If the image is not in the view area don't waste time drawing it but don't
		// set the height & width to 0 because that would cause a bug in MapWindow - IntegerToColor
		SetEmptyBuffer(ImageData);
		return true;
	}

	UpdateVisibleExtents(screenExtents.left, screenExtents.bottom, screenExtents.right, screenExtents.top);

	int xBuff = _width;
	int yBuff = _height;

	TryDecreaseBufferSize(downsamplingMode, mapUnitsPerPixel, xBuff, yBuff);
		
	ApplyBufferQuality(xBuff, yBuff);

	// actual reading		
	if ( ReadDataGeneric(ImageData, xBuff, yBuff, setToGrey) )   
	{
		_width = xBuff;
		_height = yBuff;
		return true;
	}

	return false;
}

// *********************************************************
//		ReopenDatasetIfNeeded()
// *********************************************************
bool GdalRaster::ReopenDatasetIfNeeded(CStringW filename)
{
	if (!_dataset) {
		_dataset = GdalHelper::OpenRasterDatasetW(filename, GA_ReadOnly);
	}

	return _dataset != NULL;
}

// *********************************************************
//		ApplyBufferQuality()
// *********************************************************
void GdalRaster::ApplyBufferQuality(int& xBuff, int& yBuff)
{
	// the image of reduced quality was requested
	if (_imageQuality != 100 && _width > 10 && _height > 10)
	{
		int xTemp = int((double)xBuff * double(_imageQuality) / 100.0);
		int yTemp = int((double)yBuff * double(_imageQuality) / 100.0);
		_dX = _dX * double(xBuff) / double(xTemp);
		_dY = _dY * double(yBuff) / double(yTemp);
		xBuff = xTemp;
		yBuff = yTemp;
		_xllCenter = _visibleExtents.left + _dX * 0.5;
		_yllCenter = _visibleExtents.bottom + _dY * 0.5;
	}
}

// *********************************************************
//		ReadData()
// *********************************************************
bool GdalRaster::ReadDataGeneric(colour ** ImageData, int& xBuff, int& yBuff, bool setToGrey)
{
	if (!WillBeRenderedAsGrid())
	{
		return ReadBandData(ImageData, _visibleRect.left, _visibleRect.top, _width, _height, xBuff, yBuff);
	}

	_rendering = rrColorScheme;

	if (_genericType == GDT_Int32) 
	{
		// if user passed a color scheme, image will be opened as grid using the first band			
		return ReadBandDataAsGrid<_int32>(ImageData, _visibleRect.left, _visibleRect.top, _width, _height, xBuff, yBuff, setToGrey);
	}
		
	return ReadBandDataAsGrid<float>(ImageData, _visibleRect.left, _visibleRect.top, _width, _height, xBuff, yBuff, setToGrey);
}

// *********************************************************
//		AllocateImageData()
// *********************************************************
bool GdalRaster::AllocateImageData(colour ** imageData, int size)
{
	if (*imageData)
	{
		delete[](*imageData);
	}

	try
	{
		(*imageData) = new colour[size];
		return true;
	}
	catch (...)
	{
		CallbackHelper::ErrorMsg("Failed to allocated memory for image buffer.");
	}

	return false;
}

// *********************************************************
//		SetEmptyBuffer()
// *********************************************************
void GdalRaster::SetEmptyBuffer(colour ** ImageData)
{
	AllocateImageData(ImageData, 1);

	_dX = _origDx;
	_dY = _origDy;

	_width = _height = 1;
}

// *********************************************************
//		TryDecreaseBufferSize()
// *********************************************************
// In case of large images and down sampling it makes sense to reduce the size of buffer,
// as additional details will be lost all the same but memory usage can be unacceptable.
void GdalRaster::TryDecreaseBufferSize(tkInterpolationMode downsamplingMode, double mapUnitsPerScreenPixel, int& xBuff, int& yBuff)
{
	for (int i = 0;; i++)
	{
		// we intentionally don't divide by 2 here, as we'll check the possibility of decreasing buffer by 2		
		double mapUnitsPerImagePixel = (_dX + _dY);			

		// In case of interpolation algorithm different form nearest neighbor is used 		
		// the buffer should be noticeably bigger than screen dimensions to preserve details
		if (downsamplingMode == imBicubic || downsamplingMode == imBilinear)		
			mapUnitsPerImagePixel *= 2.0;											

		if (downsamplingMode == imHighQualityBicubic || downsamplingMode == imHighQualityBilinear)
			mapUnitsPerImagePixel *= 3.0;

		// it's down sampling		
		if (mapUnitsPerImagePixel < mapUnitsPerScreenPixel)		
		{
			int xTemp = xBuff / 2;
			int yTemp = yBuff / 2;
			_dX = _dX * double(xBuff) / double(xTemp);
			_dY = _dY * double(yBuff) / double(yTemp);
			xBuff = xTemp;
			yBuff = yTemp;
		}
		else
		{
			_xllCenter = _visibleExtents.left + _dX * 0.5;
			_yllCenter = _visibleExtents.bottom + _dY * 0.5;
			break;
		}
	}
}

// *********************************************************
//		UpdateVisibleExtents()
// *********************************************************
void GdalRaster::UpdateVisibleExtents(double minX, double minY, double maxX, double maxY)
{
	_visibleExtents.left = _extents.left;
	_visibleRect.left = 0;
	if (_extents.left < minX)
	{
		// how many pixels are completely hidden: rounding in the lower side		
		int hiddenPixels = int((minX - _extents.left - _origDx / 2.0) / _origDx);
		_visibleExtents.left += hiddenPixels * _origDx;
		_visibleRect.left += hiddenPixels;
	}

	_visibleExtents.bottom = _extents.bottom;
	_visibleRect.bottom = _origHeight - 1;
	if (_extents.bottom < minY)
	{
		// how many pixels are completely hidden: rounding in the lower side		
		int hiddenPixels = int((minY - _extents.bottom) / _origDy);
		_visibleExtents.bottom += hiddenPixels * _origDy;
		_visibleRect.bottom -= hiddenPixels;
	}

	_visibleExtents.right = _extents.right;
	_visibleRect.right = _origWidth - 1;
	if (_extents.right > maxX)
	{
		// how many pixels are completely hidden: rounding in the lower side		
		int hiddenPixels = int((_extents.right - maxX) / _origDx);
		_visibleExtents.right -= hiddenPixels * _origDx;
		_visibleRect.right -= hiddenPixels;
	}

	_visibleExtents.top = _extents.top;
	_visibleRect.top = 0;
	if (_extents.top > maxY)
	{
		// how many pixels are completely hidden: rounding in the lower side		
		int hiddenPixels = int((_extents.top - maxY) / _origDy);
		_visibleExtents.top -= hiddenPixels * _origDy;
		_visibleRect.top += hiddenPixels;
	}

	// size of image buffer without scaling
	_dX = _origDx;	// map units per image pixel
	_dY = _origDy;
	_xllCenter = _visibleExtents.left + _dX * 0.5;
	_yllCenter = _visibleExtents.bottom + _dY * 0.5;
	_width = _visibleRect.right - _visibleRect.left + 1;
	_height = _visibleRect.bottom - _visibleRect.top + 1;
}

// *********************************************************
//		LoadBufferFull()
// *********************************************************
// maxBufferSize - the maximum size of buffer in megabytes if non-positive value is specified, 
// the whole image will be loaded
bool GdalRaster::LoadBufferFull(colour** ImageData, CStringW filename, double maxBufferSize)
{
	if (!ReopenDatasetIfNeeded(filename)) {
		return false;
	}
	
	// size of image buffer without scaling
	_dX = _origDx;	// map units per image pixel
	_dY = _origDy;
	_xllCenter = _origXllCenter;
	_yllCenter = _origYllCenter;
	_width = _origWidth;
	_height = _origHeight;

	int screenX = GetSystemMetrics(SM_CXFULLSCREEN);
	int screenY = GetSystemMetrics(SM_CYFULLSCREEN);
	
	double size = (maxBufferSize * (0x1 << 20)) / 3;  
	
	if ( maxBufferSize > 0 )
	{
		for (int i = 0; ; i++)
		{
			if (_width * _height / 4 > size)
			{
				_width /= 2;
				_height /= 2;
				_dX *= 2.0;
				_dY *= 2.0;
			}
			else
				break;
		}
	}
	
	// setting rectangles for consistency
	_visibleRect.left = 0;
	_visibleRect.right = _origWidth - 1;
	_visibleRect.top = 0;
	_visibleRect.bottom = _origHeight - 1;

	_visibleExtents.left = _extents.left;
	_visibleExtents.right = _extents.right;
	_visibleExtents.top = _extents.top;
	_visibleExtents.bottom = _extents.bottom;

	return ReadBandData(ImageData, _visibleRect.left, _visibleRect.top, _origWidth, _origHeight, _width, _height);
}

// *********************************************************
//		GetMappedBandIndex()
// *********************************************************
int GdalRaster::GetMappedBandIndex(int bandIndex)
{
	if (_useRgbBandMapping)
	{
		if (bandIndex == 1)
		{
			return _redBandIndex;
		}

		if (bandIndex == 2)
		{
			return _greenBandIndex;
		}

		if (bandIndex == 3)
		{
			return _blueBandIndex;
		}

		if (bandIndex == 4)
		{
			return _alphaBandIndex;
		}

		return NULL;
	}

	if (_nBands == 1 || _forceSingleBandRendering)
	{
		return _activeBandIndex;
	}
	else
	{
		return bandIndex;
	}
}

// *********************************************************
//		GetMappedBand()
// *********************************************************
GDALRasterBand* GdalRaster::GetMappedBand(int bandIndex)
{
	if (_useRgbBandMapping)
	{
		if (bandIndex == 1)
		{
			return _dataset->GetRasterBand(_redBandIndex);
		}

		if (bandIndex == 2)
		{
			return _dataset->GetRasterBand(_greenBandIndex);
		}

		if (bandIndex == 3)
		{
			return _dataset->GetRasterBand(_blueBandIndex);
		}

		return NULL;
	}
	
	if (_nBands == 1 || _forceSingleBandRendering)
	{
		return _dataset->GetRasterBand(_activeBandIndex);
	}
	else
	{
		return GetDefaultRgbBand(bandIndex);
	}
}

// *********************************************************
//		ReadBandData()
// *********************************************************
bool GdalRaster::ReadBandData(colour ** imageData, int xOffset, int yOffset, int width, int height, int xBuff, int yBuff)
{
    // -----------------------------------------------
	//  allocating memory for first buffer (GDAL reading)
	// -----------------------------------------------
	unsigned char * srcDataChar = NULL;
	_int32 * srcDataInt = NULL;
	float * srcDataFloat = NULL;

	GDALDataType dataType = _genericType;
	if (_hasColorTable)
	{
		dataType = GDT_Int32;
	}

	if (dataType == GDT_Byte)
	{
		srcDataChar = (unsigned char *)CPLMalloc(sizeof(unsigned char)*xBuff*yBuff);
	}
	else if (dataType == GDT_Int32)
	{
		srcDataInt = (_int32 *)CPLMalloc(sizeof(_int32)*xBuff*yBuff);
	}
	else
	{
		srcDataFloat = (float *)CPLMalloc(sizeof(float)*xBuff*yBuff);
	}
	
	// -----------------------------------------------
	//  allocating memory for second buffer (in-memory colour array)
	// -----------------------------------------------
	if (!AllocateImageData(imageData, xBuff * yBuff)) {
		return false;
	}

	bool singleBand = _nBands == 1 || _forceSingleBandRendering;
	_rendering = singleBand ? rrSingleBand : rrRGB;

	// -----------------------------------------------
	//    reading data
	// -----------------------------------------------
    for (int bandIndex = 1; bandIndex <= MAX(_nBands, 4); bandIndex++)
    {
		int realBandIndex = GetMappedBandIndex(bandIndex);
		if (realBandIndex == 0 || realBandIndex > _nBands)
		{
			continue;
		}

		GDALRasterBand* poBand = _dataset->GetRasterBand(realBandIndex);

		if (!poBand)
		{
			// TODO: what if we are missing some band?
			// does the appropriate color component will be initialized with 0?
			continue;
		}

		double min = GetBandMinMax(realBandIndex, true);
		double max = GetBandMinMax(realBandIndex, false);
		double range = max - min;
		double shift = 0 - min;

		// images with color table are classified as complex with GDT_Int32 data type
		if (dataType == GDT_Byte)
		{
			poBand->AdviseRead(xOffset, yOffset, width, height, xBuff, yBuff, GDT_Byte, NULL);
			poBand->RasterIO(GF_Read, xOffset, yOffset, width, height, srcDataChar, xBuff, yBuff, GDT_Byte, 0, 0);
		}
		else if (dataType == GDT_Int32)
		{
			poBand->AdviseRead(xOffset, yOffset, width, height, xBuff, yBuff, GDT_Int32, NULL);
			poBand->RasterIO(GF_Read, xOffset, yOffset, width, height, srcDataInt, xBuff, yBuff, GDT_Int32, 0, 0);
		}
		else
		{
			poBand->AdviseRead(xOffset, yOffset, width, height, xBuff, height, GDT_Float32, NULL);
			poBand->RasterIO(GF_Read, xOffset, yOffset, width, height, srcDataFloat, xBuff, yBuff, GDT_Float32, 0, 0);
		}

		double noDataValue = poBand->GetNoDataValue();
		_cInterp = poBand->GetColorInterpretation();

		if (!ReadColorTableToMemoryBuffer(imageData, srcDataInt, realBandIndex, xBuff, yBuff, noDataValue, shift, range))
		{
			int rgbBandIndex = bandIndex;

			if (dataType == GDT_Byte)
			{
				GdalBufferToMemoryBuffer<unsigned char>(imageData, srcDataChar, xBuff, yBuff, rgbBandIndex, realBandIndex, shift, range, 
					noDataValue, min, max);
			}
			else if (dataType == GDT_Int32)
			{
				GdalBufferToMemoryBuffer<_int32>(imageData, srcDataInt, xBuff, yBuff, rgbBandIndex, realBandIndex, shift, range, 
					noDataValue, min, max);
			}
			else
			{
				GdalBufferToMemoryBuffer<float>(imageData, srcDataFloat, xBuff, yBuff, rgbBandIndex, realBandIndex, shift, range, 
					noDataValue, min, max);
			}
		}
		else
		{
			_rendering = rrBuiltInColorTable;
		}

		if (_clearGDALCache) {
			poBand->FlushCache();
		}

		if (_forceSingleBandRendering) {
			break;
		}
    }

	// --------------------------------------------------------
	//		cleaning
	// --------------------------------------------------------
	if (dataType == GDT_Byte)
	{
		if (srcDataChar != NULL) 
		{
			CPLFree(srcDataChar);
			srcDataChar = NULL;
		}
	}
	else if (dataType == GDT_Int32)
	{
		if (srcDataInt != NULL) {
			CPLFree(srcDataInt);
			srcDataInt = NULL;
		}
	}
	else 
	{
		if (srcDataFloat != NULL) {
			CPLFree(srcDataFloat);
			srcDataFloat = NULL;
		}
	}
	return true;
}

// *********************************************************
//		GetRgbBand()
// *********************************************************
GDALRasterBand* GdalRaster::GetDefaultRgbBand(int bandIndex)
{
	if (bandIndex == 1)  {
		return _poBandR;
	}
	if (bandIndex == 2) {
		return _poBandG;
	}
	if (bandIndex == 3)  {
		return _poBandB;
	}

	return NULL;
}

// *************************************************************
//	  ReadColorTableToBuffer
// *************************************************************
bool GdalRaster::ReadColorTableToMemoryBuffer(colour ** imageData, int* srcDataInt, int bandIndex, int xBuff, int yBuff, 
									double noDataValue, double shift, double range)
{
	// caching the color table; adding data to the second buffer	
	if (!_hasColorTable || _ignoreColorTable) return false;
	
	if (!srcDataInt) 
	{
		CallbackHelper::ErrorMsg("Integer buffer must be used to display built-in color table");
		return false;
	}

	int tableCount = _colorTable->GetColorEntryCount();
	if (tableCount == 0)
	{
		return false;
	}

	// caching the table in the array for faster access
	const GDALColorEntry * poCE = NULL;
	colour* colorTable = new colour[tableCount];

	for (int i = 0; i < tableCount; i++)
	{
		poCE = _colorTable->GetColorEntry(i);
		
		// possible to optimize further: no need to check all values		
		GDALColorEntry2Colour(bandIndex, i, shift, range, noDataValue, poCE, _useHistogram, colorTable + i);	
	}

	// reading and decoding the bitmap values
	colour* dstRow;	int* srcRow; int index;
	for (int i = 0; i < yBuff; i++)
	{
		dstRow = (*imageData) + i * xBuff;
		srcRow = srcDataInt + i * xBuff;

		for (int j = 0; j < xBuff; j++)
		{
			index = *(srcRow + j);
			memcpy(dstRow + j, colorTable + index, sizeof(colour));
		}
	}

	delete[] colorTable;

	return true;
}

// *************************************************************
//	  GdalBufferToMemoryBuffer
// *************************************************************
template <typename T>
bool GdalRaster::GdalBufferToMemoryBuffer(colour ** dst, T* src, int xBuff, int yBuff,
	int nominalRgbBand, int realBandIndex, double shift, double range, double noDataValue, double min, double max)
{
	T val = 0;		// value from source buffer	(_int32, float, unsigned char)
	colour* color;	// position in the resulting buffer

	bool singleBand = _nBands == 1 || _forceSingleBandRendering;

	bool histogram = /*singleBand &&*/ _useHistogram && _histogram.CanUse();
	bool floatOrInt = _genericType == GDT_Float32 || _genericType == GDT_Int32;
	double ratio = 255.0 / (double)range;

	unsigned char b;
	for (int i = 0; i < yBuff; i++) 
	{
		for (int j = 0; j < xBuff; j++)	
		{
			val = src[i * xBuff + j];	
			color = (*dst) + i * xBuff + j;
				
			if (val == noDataValue || val < min || val > max)
			{
				color->blue = _transColor.b;
				color->green = _transColor.g;
				color->red = _transColor.r;
				color->alpha = 0;		// in fact the rest isn't much needed
				continue;
			}

			if (histogram)
			{
				b = static_cast<unsigned char>(_histogram.GetColorValue(realBandIndex, static_cast<double>(val)));
			}
			else if (floatOrInt)
			{
				b = static_cast<unsigned char>(double(val + shift) * ratio);
			}
			else
			{
				b = static_cast<unsigned char>(val);
			}

			if (_reverseGreyscale) 
			{
				b = 255 - b;
			}

			if (singleBand)
			{
				color->red = b;
				color->green = b;
				color->blue = b;

				if (_alphaRendering) 
				{
					if (_reverseGreyscale) {
						b = 255 - b;		// alpha band must not be reversed
					}
					color->alpha = b;
				}
			}
			else
			{
				if (nominalRgbBand == 1)      color->red = b;
				else if (nominalRgbBand == 2) color->green = b;
				else if (nominalRgbBand == 3) color->blue = b;
				else if (nominalRgbBand == 4) 
				{
					if (_reverseGreyscale) {
						b = 255 - b;		// alpha band must not be reversed
					}

					// to honor no data values previously set
					// TODO: in fact better to process the alpha band first
					if (color->alpha != 0)
					{
						color->alpha = b;
					}
				}
			}
		}
	}

	return true;
}

// ****************************************************************
//		GDALColorEntry2Colour()
// ****************************************************************
// Filling colour structure from the GDAL colour value.
// Only when built-in color table is specified for datasource.
void GdalRaster::GDALColorEntry2Colour(int band, double colorValue, double shift, double range, double noDataValue, 
					const GDALColorEntry * poCE, bool useHistogram, colour* result)
{
	bool colorEntryExists = (poCE != NULL)?true:false;
	
	// TODO: should we use _forceSingleBandRendering here?

	if (colorValue == noDataValue)
	{
		if (_nBands == 1)
		{
			result->red = (unsigned char)	poCE->c1;
			result->green = (unsigned char)	poCE->c2;
			result->blue = (unsigned char)	poCE->c3;
		}
		else
		{
			if (band == 1)		result->red = _transColor.r;
			else if (band == 2)	result->green = _transColor.g;
			else if (band == 3)	result->blue = _transColor.b;
		}

		result->alpha = 0;

		return;
	}
	
	if (useHistogram && _nBands == 1 && !_hasColorTable)
	{
		unsigned char b = _histogram.GetColorValue(band, colorValue);
	
		result->red = b;
		result->green = b;
		result->blue = b;

		return;
	}

	if (_hasColorTable && colorEntryExists)
	{
		if (_cInterp == GCI_AlphaBand)
		{
			result->red = (unsigned char)poCE->c4;  
			result->green = (unsigned char)poCE->c4;
			result->blue = (unsigned char)poCE->c4; 
		}
		else if ( (_palleteInterpretation == GPI_Gray) || (_cInterp == GCI_GrayIndex) )
		{
			result->red = (unsigned char)poCE->c1;  
			result->green = (unsigned char)poCE->c1;
			result->blue = (unsigned char)poCE->c1; 
		}
		else 
		{
			result->red = (unsigned char)poCE->c1;
			result->green = (unsigned char)poCE->c2;
			result->blue = (unsigned char)poCE->c3;
		}
		
		return;
	} 

	if (_genericType == GDT_Float32 || _genericType == GDT_Int32)
	{
		if (_nBands == 1)
		{
			result->red = (unsigned char)((colorValue + shift) * 255/range);
			result->green = (unsigned char)((colorValue + shift) * 255/range);
			result->blue = (unsigned char)((colorValue + shift) * 255/range);				
		}
		else
		{
			if (band == 1)      result->red = (unsigned char) ((colorValue + shift)  * 255/range);
			else if (band == 2) result->green = (unsigned char) ((colorValue + shift) * 255/range);
			else if (band == 3) result->blue = (unsigned char) ((colorValue + shift)  * 255/range);
		}

		return;
	}
	
	if (_nBands == 1)
	{
		result->red = (unsigned char)(colorValue);
		result->green = (unsigned char)(colorValue);
		result->blue = (unsigned char)(colorValue);
	}
	else
	{
		if (band == 1)      result->red = (unsigned char)(colorValue);
		else if (band == 2) result->green = (unsigned char)(colorValue);
		else if (band == 3) result->blue = (unsigned char)(colorValue);
	}
}

// *********************************************************
//		computeGradient()
// *********************************************************
namespace {
	struct GradientPercent
	{
		float left;
		float right;
	};

	GradientPercent computeGradient(float val, float lowVal, float biRange, GradientModel gradmodel)
	{
		GradientPercent result;

		//Linear
		if( gradmodel == Linear )
		{	
			result.right = ( ( val - lowVal ) / biRange );
			result.left = 1.0f - result.right;			
		}
		//Log
		else if( gradmodel == Logorithmic )
		{	
			float dLog = 0.0f;
			if( val < 1.0f )
				val = 1.0f;
			if( biRange > 1.0f && val - lowVal > 1.0f )
			{
				result.right = ( log( val - lowVal)/log(biRange) );
				result.left = 1.0f - result.right;					
			}					
			else
			{	
				result.right = 0.0f;
				result.left = 1.0f;						
			}						
		}
		//Exp
		else if( gradmodel == Exponential )
		{	
			float dLog = 0.0f;
			if( val < 1.0f )
				val = 1.0f;
			if( biRange > 1.0f )
			{
				result.right = ( pow(val - lowVal, 2)/pow(biRange, 2) );
				result.left = 1.0f - result.right;
			}					
			else
			{
				result.right = 0.0f;
				result.left = 1.0f;						
			}		
		}
		return result;
	}
}

// *********************************************************
//		SetTransparentColor()
// *********************************************************
inline void GdalRaster::SetTransparentColor(colour* ImageData)
{
	ImageData->red = (unsigned char)_transColor.r;
	ImageData->green = (unsigned char)_transColor.g;
	ImageData->blue = (unsigned char)_transColor.b;
	ImageData->alpha = 0;
}

// *********************************************************
//		ReadGridAsImage()
// *********************************************************
template <typename DataType>
bool GdalRaster::ReadBandDataAsGrid(colour** ImageData, int xOff, int yOff, int width, int height, int xBuff, int yBuff, bool setRGBToGrey) 
{
	return ReadBandDataAsGridCore<DataType>(ImageData, xOff, yOff, width, height, xBuff, yBuff, setRGBToGrey);
}

// *********************************************************
//		GetLightSource()
// *********************************************************
void GdalRaster::GetLightSource(IGridColorScheme* gridColorScheme, cppVector& result)
{
	double lsi, lsj, lsk;
	IVector * v = NULL;
	gridColorScheme->GetLightSource(&v);
	v->get_i(&lsi);
	v->get_j(&lsj);
	v->get_k(&lsk);
	v->Release();

	result.seti(lsi);
	result.setj(lsj);
	result.setk(lsk);
}

// *********************************************************
//		ReadColorScheme()
// *********************************************************
void GdalRaster::ReadColorScheme(std::vector<BreakVal>& bvals, float& ai, float& li, cppVector& lightSource)
{
	IGridColorScheme* gridColorScheme = GetColorSchemeForRendering();
	long numBreaks = 0;
	gridColorScheme->get_NumBreaks(&numBreaks);

	for (int i = 0; i < numBreaks; i++)
	{
		CComPtr<IGridColorBreak> bi = NULL;
		gridColorScheme->get_Break(i, &bi);
		VARIANT_BOOL visible;
		bi->get_Visible(&visible);
		
		if (visible)
		{
			BreakVal bv(bi);
			bvals.push_back(bv);
		}
	}

	//Bug 1389 Make sure the incoming gridColorScheme from _pushSchemetkRaster has the same no-data color
	gridColorScheme->put_NoDataColor(_transColor);

	const double ka = .7;
	const double kd = .8;

	double val;
	gridColorScheme->get_AmbientIntensity(&val);
	ai = static_cast<float>(val * ka);

	gridColorScheme->get_LightSourceIntensity(&val);
	li = static_cast<float>(val * kd);

	GetLightSource(gridColorScheme, lightSource);
}

// *********************************************************
//		ReadGridAsImage2()
// *********************************************************
template <typename DataType>
bool GdalRaster::ReadBandDataAsGridCore(colour** ImageData, int xOff, int yOff, int width, int height, int xBuff, int yBuff, bool setRGBToGrey) 
{
	GDALRasterBand* band = _dataset->GetRasterBand(_activeBandIndex);
	if (band == NULL) return false;

	float noDataValue = static_cast<float>(band->GetNoDataValue());

	AllocateImageData(ImageData, xBuff * yBuff);

	DataType* pafScanArea = (DataType*)CPLMalloc( sizeof(DataType)*xBuff*yBuff );

	if (_genericType == GDT_Int32)
	{
		band->AdviseRead(xOff, yOff, width, height, xBuff, yBuff, GDT_Int32, NULL);
		band->RasterIO(GF_Read, xOff, yOff, width, height, pafScanArea, xBuff, yBuff, GDT_Int32, 0, 0);
	}
	else
	{
		band->AdviseRead(xOff, yOff, width, height, xBuff, yBuff, GDT_Float32, NULL);
		band->RasterIO(GF_Read, xOff, yOff, width, height, pafScanArea, xBuff, yBuff, GDT_Float32, 0, 0);
	}

	const float total = static_cast<float>(yBuff * xBuff);
	const float xll = static_cast<float>(_xllCenter);
	const float yll = static_cast<float>(_yllCenter);

	const float csize = 30;	  // hard code csize, so that the vectors are normal
	std::vector<BreakVal> bvals;
	float ai = 0.0f, li = 0.0f;
	cppVector lightSource;
	ReadColorScheme(bvals, ai, li, lightSource);

	for (int i = 0; i < yBuff; i++)
	{
		for (int j = 0; j < xBuff; j++)
		{
			float tmp = (float)pafScanArea[i * xBuff + j];

			if (tmp == noDataValue)
			{
				SetTransparentColor((*ImageData) + i * xBuff + j);
				continue;
			} 

			auto colorBreak = FindBreak( bvals, tmp );

			if (colorBreak == nullptr) //A break is not defined for this value
			{
				SetTransparentColor((*ImageData) + i * xBuff + j);
				continue;
			}

 			OLE_COLOR hiColor = colorBreak->hiColor;
			OLE_COLOR lowColor = colorBreak->lowColor;

			float hiVal = colorBreak->highVal;
			float lowVal = colorBreak->lowVal;
			float biRange = hiVal - lowVal;
			if( biRange <= 0.0 )
				biRange = 1.0;

			ColoringType colortype = colorBreak->colortype;
			GradientModel gradmodel = colorBreak->gradmodel;
				
			if (!_allowHillshade && colortype == Hillshade)
				colortype = Gradient;

			if( colortype == Hillshade )
			{
				float yone = 0, ytwo = 0, ythree = 0;

				// Exclude the edges 
				if (i == 0 || j == 0 || i == yBuff - 1 || j == xBuff - 1)
				{
					// We are at the edge so write doDataValue and move on
					yone = tmp;
					ytwo = tmp;
					ythree = tmp;
				}
				else
				{
					yone = tmp;
					ytwo = (float)(pafScanArea[(i - 1) * xBuff + j + 1]);
					ythree = (float)(pafScanArea[(i - 1) * xBuff + j]);
				}
				
				float xone = xll + csize * (i);
				float xtwo = xone + csize;
				float xthree = xone;
					
				float zone = yll + csize * j;
				float ztwo = zone;
				float zthree = zone - csize;

				//check for nodata on triangle corners
				if( yone == noDataValue || ytwo == noDataValue || ythree == noDataValue )
				{	
					SetTransparentColor((*ImageData) + i * xBuff + j);
					continue;
				}
				
				if (lowColor == hiColor)
				{
					(*ImageData)[i * xBuff + j].red = (unsigned char)(GetRValue(lowColor) % 256);
					(*ImageData)[i * xBuff + j].green = (unsigned char)(GetGValue(lowColor) % 256);
					(*ImageData)[i * xBuff + j].blue = (unsigned char)(GetBValue(lowColor) % 256);
				}
				else
				{
					//Make Two Vectors
					cppVector one(
						xone - xtwo,
						yone - ytwo,
						zone - ztwo );
					cppVector two(
						xone - xthree,
						yone - ythree,
						zone - zthree );
						
					//Compute Normal
					cppVector normal = two.crossProduct( one );

					//Compute I
					double I = ai + li * lightSource.dot(normal);
					if( I > 1.0 )
						I = 1.0;

					GradientPercent gradient = computeGradient(tmp, lowVal, biRange, gradmodel);

					(*ImageData)[i * xBuff + j].red =  (unsigned char) (((double)GetRValue(lowColor)*gradient.left + (double)GetRValue(hiColor)*gradient.right )*I) %256;
					(*ImageData)[i * xBuff + j].green = (unsigned char) (((double)GetGValue(lowColor)*gradient.left + (double)GetGValue(hiColor)*gradient.right )*I) %256;
					(*ImageData)[i * xBuff + j].blue =  (unsigned char) (((double)GetBValue(lowColor)*gradient.left + (double)GetBValue(hiColor)*gradient.right )*I) %256;
				}
			}
			else if( colortype == Gradient )
			{
				GradientPercent gradient = computeGradient(tmp, lowVal, biRange, gradmodel);

				(*ImageData)[i * xBuff + j].red = (int)((float)GetRValue(lowColor)*gradient.left + (float)GetRValue(hiColor)*gradient.right) % 256;
				(*ImageData)[i * xBuff + j].green = (int)((float)GetGValue(lowColor)*gradient.left + (float)GetGValue(hiColor)*gradient.right) % 256;
				(*ImageData)[i * xBuff + j].blue = (int)((float)GetBValue(lowColor)*gradient.left + (float)GetBValue(hiColor)*gradient.right) % 256;
			}
			else if( colortype == Random )
			{
				(*ImageData)[i * xBuff + j].red = GetRValue(lowColor);
				(*ImageData)[i * xBuff + j].green = GetGValue(lowColor);
				(*ImageData)[i * xBuff + j].blue = GetBValue(lowColor);
			}
		} 
	}
	
	if (_clearGDALCache)
		band->FlushCache();
	
	CPLFree( pafScanArea );
	return true;
}


// *************************************************************
//	  findBreak
// *************************************************************
inline const GdalRaster::BreakVal* GdalRaster::FindBreak(const std::vector<BreakVal> & bvals, double val) const
{
	register int sizeBVals = (int)bvals.size();
	for(register int i = 0; i < sizeBVals; i++ )
	{	
		if( val >= bvals[i].lowVal &&
			val <= bvals[i].highVal )
			return &bvals[i];
	}

	return nullptr;
}

// *********************************************************
//		CompuateHistogram()
// *********************************************************
void GdalRaster::CompuateHistogram()
{
	if (_histogram.GetState() != HistogramNotComputed) {
		return;		// we've tried once
	}

	_histogram.ComputeHistogram(_dataset, 256);
}

// ***********************************************************
//		SetNoDataValue()
// ***********************************************************
bool GdalRaster::SetNoDataValue(double Value)
{
	if (!_dataset) return false;
	
	if (_nBands == 3 && _genericType == GDT_Int32)
	{
		OLE_COLOR val = (OLE_COLOR)Value;
		unsigned char r = GetRValue(val);
		unsigned char g = GetGValue(val);
		unsigned char b = GetBValue(val);
		if (_poBandR->SetNoDataValue(double(r)) != CE_None) return false;
		if (_poBandG->SetNoDataValue(double(g)) != CE_None) return false;
		if (_poBandB->SetNoDataValue(double(b)) != CE_None) return false;
		return true;
	}
		
	if (_nBands >= 1)
	{
		if (_poBandR->SetNoDataValue(Value) != CE_None) return false;
	}

	if (_nBands >= 2)
	{
		if (_poBandG->SetNoDataValue(Value) != CE_None) return false;
	}

	if (_nBands == 3)
	{
		if (_poBandB->SetNoDataValue(Value) != CE_None) return false;
	}

	return true;
}

// *********************************************************
//		GetColorScheme()
// *********************************************************
// Which of the 2 color schemes can we use
IGridColorScheme* GdalRaster::GetColorSchemeForRendering()
{
	long numBreaks;	

	if (_customColorScheme)
	{
		_customColorScheme->get_NumBreaks(&numBreaks);
		if (numBreaks > 0) {
			return _customColorScheme;
		}
	}

	_predefinedColorScheme->get_NumBreaks(&numBreaks);
	if (numBreaks == 0)
	{
		UpdatePredefinedColorScheme();
	}

	return _predefinedColorScheme;
}

// *************************************************************
//	  GetActiveBand()
// *************************************************************
GDALRasterBand* GdalRaster::GetActiveBand()
{
	return _dataset->GetRasterBand(_activeBandIndex);
}

// *************************************************************
//	  UpdatePredefinedColorScheme()
// *************************************************************
void GdalRaster::UpdatePredefinedColorScheme()
{
	double min = GetBandMinMax(_activeBandIndex, true);
	double max = GetBandMinMax(_activeBandIndex, false);
	_predefinedColorScheme->UsePredefined(min, max, _predefinedColors);
}

// *************************************************************
//	  CanUseExternalColorScheme()
// *************************************************************
bool GdalRaster::CanUseExternalColorScheme()
{
	// TODO: revisit; probably in some cases it's not possible after all
	//return dataType == GDT_Int32 || dataType == GDT_Float32;
	return true;
}

// *************************************************************
//	  NeedsGridRendering()
// *************************************************************
bool GdalRaster::NeedsGridRendering()
{
	if (_imgType == IMG_FILE || _imgType == KAP_FILE || (_imgType == TIFF_FILE && _dataType == GDT_UInt16) || _hasColorTable)
	{
		return false;
	}
	else if (_dataType == GDT_Byte && _imgType != ADF_FILE /*&& _dfMax > 15*/)
	{
		return false;
	}
	else if (/*_dfMax > 1 &&*/ _nBands == 1 || _imgType == ADF_FILE || _imgType == ASC_FILE || _imgType == DEM_FILE)
	{
		return true;
	}
	else if (!IsRgb())
	{
		return true;
	}
	else
	{
		return false;
	}
}

// *************************************************************
//	  WillBeRenderedAsGrid()
// *************************************************************
bool GdalRaster::WillBeRenderedAsGrid()
{
	if (_allowAsGrid == grNever)  {
		return false;
	}
	
	return _allowAsGrid == grForceForAllFormats || (_allowAsGrid == grForGridsOnly && NeedsGridRendering());
}

// *************************************************************
//	  IsRgb()
// *************************************************************
bool GdalRaster::IsRgb()
{
	return GdalHelper::IsRgb(_dataset);
}

// *************************************************************
//	  GetRgbBandIndex()
// *************************************************************
int GdalRaster::GetRgbBandIndex(BandChannel color)
{
	switch (color)
	{
		case BandChannelRed:
			return _redBandIndex;
		case BandChannelGreen:
			return _greenBandIndex;	
		case BandChannelBlue:
			return _blueBandIndex;
		case BandChannelAlpha:
			return _alphaBandIndex;
	}

	return 0;
}

// *************************************************************
//	  SetRgbBandIndex()
// *************************************************************
void GdalRaster::SetRgbBandIndex(BandChannel color, int bandIndex)
{
	switch (color)
	{
		case BandChannelRed:
			_redBandIndex = bandIndex;
			return;
		case BandChannelGreen:
			_greenBandIndex = bandIndex;
			return;
		case BandChannelBlue:
			_blueBandIndex = bandIndex;
			return;
		case BandChannelAlpha:
			_alphaBandIndex = bandIndex;
			return;
	}

	CallbackHelper::ErrorMsg("Invalid band index passed to GdalRaster::SetRgbBandIndex");
}

// *************************************************************
//	  SetUseHistogram()
// *************************************************************
void GdalRaster::SetUseHistogram(bool value)
{
	_useHistogram = value;
	if (_useHistogram && _histogram.GetState() == HistogramNotComputed)
	{
		this->CompuateHistogram();
	}
}

// *************************************************************
//	  GetBandMinMax()
// *************************************************************
double GdalRaster::GetBandMinMax(int bandIndex, bool min)
{
	if (!ValidateBandIndex(bandIndex)) {
		return 0.0;
	}

	if (!_bandMinMax[bandIndex - 1].Calculated)
	{
		ComputeBandMinMax(_dataset->GetRasterBand(bandIndex), _bandMinMax[bandIndex - 1], false);
	}

	return min ? _bandMinMax[bandIndex - 1].Min : _bandMinMax[bandIndex - 1].Max;
}

// *************************************************************
//	  GetBandMinMax()
// *************************************************************
void GdalRaster::SetBandMinMax(int bandIndex, double min, double max)
{
	if (!ValidateBandIndex(bandIndex)) return;

	bandIndex--;		

	_bandMinMax[bandIndex].Min = min;
	_bandMinMax[bandIndex].Max = max;
	_bandMinMax[bandIndex].Calculated = true;
}

// *************************************************************
//	  SetDefaultMinMax()
// *************************************************************
void GdalRaster::SetDefaultMinMax(int bandIndex)
{
	ComputeBandMinMax(_dataset->GetRasterBand(bandIndex), _bandMinMax[bandIndex - 1], true);
}

// *************************************************************
//	  GuessRenderingMode()
// *************************************************************
tkRasterRendering GdalRaster::GuessRenderingMode()
{
	if (WillBeRenderedAsGrid())
	{
		return rrColorScheme;
	}

	if (_hasColorTable && !_ignoreColorTable)
	{
		return rrBuiltInColorTable;
	}

	if (_nBands == 1 || _forceSingleBandRendering)
	{
		return rrSingleBand;
	}

	return rrRGB;
}

// *************************************************************
//	  Serialize()
// *************************************************************
void GdalRaster::Serialize(CPLXMLNode* psTree)
{
	SerializeCore(psTree);

	SaveBandsMinMax(psTree);

	if (_customColorScheme)
	{
		CPLXMLNode* node = ((CGridColorScheme*)_customColorScheme)->SerializeCore("GridColorScheme");
		if (node) 
		{
			CPLAddXMLChild(psTree, node);
		}
	}
}

// *************************************************************
//	  SerializeCore()
// *************************************************************
void GdalRaster::SerializeCore(CPLXMLNode* psTree)
{
	if (_predefinedColors != FallLeaves)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ImageColorScheme", CPLString().Printf("%d", (int)_predefinedColors));

	if (_allowAsGrid != tkGridRendering::grForGridsOnly)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AllowGridRendering", CPLString().Printf("%d", (int)_allowAsGrid));

	if (_allowHillshade != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AllowHillshade", CPLString().Printf("%d", (int)_allowHillshade));

	if (_buffSize != 100.0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "BufferSize", CPLString().Printf("%d", _buffSize));

	if (_clearGDALCache != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ClearGdalCache", CPLString().Printf("%d", (int)_clearGDALCache));

	if (_activeBandIndex != -1)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "SourceGridBandIndex", CPLString().Printf("%d", _activeBandIndex));

	if (_useHistogram != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseHistogram", CPLString().Printf("%d", (int)_useHistogram));

	if (_redBandIndex != 1)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "RedBandIndex", CPLString().Printf("%d", (int)_redBandIndex));

	if (_greenBandIndex != 2)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "GreenBandIndex", CPLString().Printf("%d", (int)_greenBandIndex));

	if (_blueBandIndex != 3)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "BlueBandIndex", CPLString().Printf("%d", (int)_blueBandIndex));

	if (_alphaBandIndex != 4)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AlphaBandIndex", CPLString().Printf("%d", (int)_alphaBandIndex));

	if (_useRgbBandMapping != false)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseRgbBandMapping", CPLString().Printf("%d", (int)_useRgbBandMapping));

	if (_forceSingleBandRendering != false)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ForceSingleBandRendering", CPLString().Printf("%d", (int)_forceSingleBandRendering));

	if (_reverseGreyscale != false)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ReverseGreyscale", CPLString().Printf("%d", (int)_reverseGreyscale));

	if (_ignoreColorTable != false)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "IgnoreColorTable", CPLString().Printf("%d", (int)_ignoreColorTable));

	if (_alphaRendering != false)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseActiveBandAsAlpha", CPLString().Printf("%d", (int)_alphaRendering));

}

// *************************************************************
//	  SaveBandsMinMax()
// *************************************************************
void GdalRaster::SaveBandsMinMax(CPLXMLNode* psTree)
{
	CPLXMLNode* nodeBands = CPLCreateXMLNode(psTree, CPLXMLNodeType::CXT_Element, "Bands");

	for (size_t i = 0; i < _bandMinMax.size(); i++)
	{
		CPLXMLNode* nodeBand = CPLCreateXMLNode(nodeBands, CPLXMLNodeType::CXT_Element, "Band");

		Utility::CPLCreateXMLAttributeAndValue(nodeBand, "Min", _bandMinMax[i].Min);
		Utility::CPLCreateXMLAttributeAndValue(nodeBand, "Max", _bandMinMax[i].Max);
		Utility::CPLCreateXMLAttributeAndValue(nodeBand, "Valid", _bandMinMax[i].Calculated);
	}
}

// *************************************************************
//	  Deserialize()
// *************************************************************
void GdalRaster::Deserialize(CPLXMLNode* node)
{
	if (!node) return;

	DeserializeCore(node);

	DeserializeBandMinMax(node);

	CPLXMLNode* nodeScheme = CPLGetXMLNode(node, "GridColorScheme");
	if (nodeScheme)
	{
		if (!_customColorScheme) 
		{
			ComHelper::CreateInstance(idGridColorScheme, (IDispatch**)&_customColorScheme);
		}

		((CGridColorScheme*)_customColorScheme)->DeserializeCore(nodeScheme);
	}
}

// *************************************************************
//	  DeserializeBandMinMax()
// *************************************************************
void GdalRaster::DeserializeBandMinMax(CPLXMLNode* node)
{
	if (!node) return;

	node = CPLGetXMLNode(node, "Bands");
	if (!node) return;
	
	int count = Utility::CPLXMLChildrentCount(node);
	if (count != _bandMinMax.size()) {
		return;
	}

	CString s;
	int i = 0;

	node = node->psChild;
	while (node)
	{
		if (_stricmp(node->pszValue, "Band") == 0)
		{
			s = CPLGetXMLValue(node, "Valid", "0");
			bool valid = atoi(s) != 0;

			if (valid)
			{
				s = CPLGetXMLValue(node, "Min", "");
				if (s != "") _bandMinMax[i].Min = Utility::atof_custom(s);

				s = CPLGetXMLValue(node, "Max", "");
				if (s != "") _bandMinMax[i].Max = Utility::atof_custom(s);
			}

			i++;
		}

		node = node->psNext;
	}
}

// *************************************************************
//	  DeserializeCore()
// *************************************************************
void GdalRaster::DeserializeCore(CPLXMLNode* node)
{
	CString s = CPLGetXMLValue(node, "AllowGridRendering", "1");		// 1 = grForGridsOnly
	if (s != "") SetAllowAsGrid((tkGridRendering)atoi(s));

	s = CPLGetXMLValue(node, "AllowHillshade", "1");
	if (s != "") SetAllowHillshade((bool)(atoi(s) != 0));

	s = CPLGetXMLValue(node, "BufferSize", "100");
	if (s != "") _buffSize = atoi(s);

	s = CPLGetXMLValue(node, "ClearGdalCache", "1");
	if (s != "") SetClearGdalCache((bool)(atoi(s) != 0));

	s = CPLGetXMLValue(node, "SourceGridBandIndex", "1");
	if (s != "") SetActiveBandIndex(atoi(s));

	s = CPLGetXMLValue(node, "UseHistogram", "0");
	if (s != "") SetUseHistogram((bool)(atoi(s) != 0));

	s = CPLGetXMLValue(node, "ImageColorScheme", "0");
	if (s != "") ApplyPredefinedColorScheme((PredefinedColorScheme)atoi(s));

	s = CPLGetXMLValue(node, "RedBandIndex", "1");
	if (s != "") SetRgbBandIndex(BandChannelRed, atoi(s));

	s = CPLGetXMLValue(node, "GreenBandIndex", "2");
	if (s != "") SetRgbBandIndex(BandChannelGreen, atoi(s));

	s = CPLGetXMLValue(node, "BlueBandIndex", "3");
	if (s != "") SetRgbBandIndex(BandChannelBlue, atoi(s));

	s = CPLGetXMLValue(node, "AlphaBandIndex", "4");
	if (s != "") SetRgbBandIndex(BandChannelAlpha, atoi(s));

	s = CPLGetXMLValue(node, "UseRgbBandMapping", "0");
	if (s != "") SetUseRgbBandMapping((bool)(atoi(s) != 0));

	s = CPLGetXMLValue(node, "ForceSingleBandRendering", "0");
	if (s != "") SetForceSingleBandRendering((bool)(atoi(s) != 0));

	s = CPLGetXMLValue(node, "ReverseGreyscale", "0");
	if (s != "") SetReverseGreyscale((bool)(atoi(s) != 0));

	s = CPLGetXMLValue(node, "IgnoreColorTable", "0");
	if (s != "") SetIgnoreColorTable((bool)(atoi(s) != 0));

	s = CPLGetXMLValue(node, "UseActiveBandAsAlpha", "0");
	if (s != "") SetAlphaBandRendering((bool)(atoi(s) != 0));
}

// *************************************************************
//	  GetActiveColorScheme()
// *************************************************************
IGridColorScheme* GdalRaster::GetActiveColorScheme()
{ 
	// caller won't be able to distinguish whether it's predefined or custom color scheme;
	// however if there is need to force:
	// a) predefined color scheme
	// -set Image.CustomColorScheme to NULL,
	// -set new value of Image.ImageColorScheme (predefined colors enumeration; _predefinedColorScheme will be generated on the next redraw)
	// b) custom color scheme:
	// - just set Image.CustomColorScheme
	return _customColorScheme != NULL ? _customColorScheme : _predefinedColorScheme; 
}