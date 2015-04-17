// tkRaster.cpp: implementation of the tkRaster class. Replaces tkECW.cpp
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
#include "tkRaster.h"
#include "Vector.h"
#include "gdalwarper.h"
extern "C" 
{
	#include "cq.h"
}

using namespace std;

// *************************************************************
//	  ApplyPredefinedColorScheme()
// *************************************************************
void tkRaster::ApplyPredefinedColorScheme(PredefinedColorScheme colorScheme)
{
	_predefinedColors = colorScheme;
	_predefinedColorScheme->UsePredefined(_dfMin, _dfMax, colorScheme);
}

// *************************************************************
//	  SetActiveBandIndex()
// *************************************************************
bool tkRaster::SetActiveBandIndex(int bandIndex)
{
	if (_nBands < bandIndex || bandIndex < 1)
	{
		return false;
	}
	else
	{
		_activeBandIndex = bandIndex;
		return true;
	}
}

// *************************************************************
//	  GetBand()
// *************************************************************
GDALRasterBand* tkRaster::GetBand(int bandIndex)
{
	return _rasterDataset->GetRasterBand(bandIndex);
}

// *************************************************************
//	  ComputeMinMax()
// *************************************************************
void tkRaster::ComputeBandMinMax()
{
	int bGotMin=false, bGotMax=false;
	_adfMinMax[0] = _poBandR->GetMinimum( &bGotMin );
	_adfMinMax[1] = _poBandR->GetMaximum( &bGotMax );
	
	//GDALComputeRasterMinMax is potentially very slow so only do it once and if needed
	if( ! (bGotMin && bGotMax) )
	{
		if ((_dataType == GDT_Byte) &&  (_imgType != ADF_FILE) && (_hasColorTable==false) && (_imgType != PNG_FILE))
			//Just guess the min and max
			//For some reason GDAL does not seem to pick up the color table of a png binary image
		{
			_dfMin = 0.0;
			_dfMax = 255.0;
		}
		else 
		{	
			GDALComputeRasterMinMax((GDALRasterBandH)_poBandR, FALSE, _adfMinMax);
		}
	}
	_dfMin = _adfMinMax[0];

	if (_adfMinMax[1] > 0 )	{
		_dfMax = _adfMinMax[1];
	}
	else {
		// If GDALComputeRasterMinMax fails
		_dfMax = 255.0;
	}
}

// *************************************************************
//	  LoadRaster()
// *************************************************************
bool tkRaster::LoadRaster(CStringW filename, GDALAccess accessType)
{
	CStringA filenameA = Utility::ConvertToUtf8(filename);
	return LoadRasterCore(filenameA, accessType);
}

// *************************************************************
//	  LoadRasterCore()
// *************************************************************
bool tkRaster::LoadRasterCore(CStringA& filename, GDALAccess accessType)
{
	// Chris M 11/15/2005 - This function doesn't use any AFX calls,
	// and getting a static module state will make this function require unwinding.
	// This will mean that SEH exception handing can't be used, and we'll be unable
	// to catch GDAL crashes on GDALOpen. Therefore, don't use AFX_MANAGE_STATE...
	//	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	bool retVal = false;
	// Chris M 11/15/2005 - Use a slightly stronger form of exception handing (SEH) here,
	// rather than the weaker C++ try/catch. This will catch GDAL's memory exceptions
	// when it cannot, for instance, access it's delay-loaded SID functionality. This
	// will allow us to return false properly, thus displaying "not supported" rather than
	// just doing nothing and leaving a blank map.
	__try
	{
		_warped = false;
		GDALAllRegister();
		
		//Rob JPEG2000 fails at this point with a nasty error if MapWinGIS is 
		//compiled in debug mode. GIF images also crash here		
		
		if (_rasterDataset == NULL)
			_rasterDataset = GdalHelper::OpenRasterDatasetA(filename, accessType);
	
		if( _rasterDataset == NULL ) 
		{
			retVal = false;
			return retVal;
		}

		_origWidth = _rasterDataset->GetRasterXSize();
		_origHeight = _rasterDataset->GetRasterYSize();			
		
		m_globalSettings.SetGdalUtf8(true);		// otherwise there can be problems when reading world file
												// as dataset filename is already stored as UTF8

		double adfGeoTransform[6];
		bool success = _rasterDataset->GetGeoTransform( adfGeoTransform ) == CE_None;

		m_globalSettings.SetGdalUtf8(false);

		if( success )
		{
            _origDx = adfGeoTransform[1];
			_origDy  = adfGeoTransform[5];
			// adfGeoTransform[0]&[3] refer to the corner (not the centre) of the left top pixel
			_origXllCenter = adfGeoTransform[0]+ _origDx/2.0;
			_origYllCenter = adfGeoTransform[3] + _origDy/2.0;
			
			// we got top corner and now we'll get coordinates of the bottom left corner; 
			// dy should be changed respectively as map coordinates will be increasing while moving from bottom to top
			_origYllCenter += _origDy * (_origHeight - 1);
			_origDy *= -1;

			// Check if we need a warped VRT for this file.
			if ( adfGeoTransform[1] < 0.0 || adfGeoTransform[2] != 0.0 || adfGeoTransform[4] != 0.0 || adfGeoTransform[5] > 0.0 )
			{
				GDALDataset* mGdalDataset = NULL;
				mGdalDataset = (GDALDataset *)GDALAutoCreateWarpedVRT( _rasterDataset, NULL, NULL, GRA_NearestNeighbour, 0.2, NULL );  
				if (mGdalDataset)
				{
					_rasterDataset->Dereference();
					_rasterDataset = mGdalDataset;
					_warped = true;

					if ( _rasterDataset->GetGeoTransform( adfGeoTransform )== CE_None)
					{
						_origDx = adfGeoTransform[1];
						_origDy  = adfGeoTransform[5];
						// adfGeoTransform[0]&[3] refer to the corner (not the centre) of the left top pixel
						_origXllCenter = adfGeoTransform[0]+ _origDx/2;
						_origYllCenter = adfGeoTransform[3] + _origDy/2;
						
						_origWidth = _rasterDataset->GetRasterXSize();
						_origHeight = _rasterDataset->GetRasterYSize();			

						// we got top corner and now we'll get coordinates of the bottom left corner; 
						// dy should be changed respectively as map coordinates will be increasing while moving from bottom to top
						_origYllCenter += _origDy * (_origHeight - 1);
						_origDy *= -1;
					}
				}
			}	
		}
		else
		{	
			_origXllCenter = 0;
			_origYllCenter = 0;
			_origDx = 1;
			_origDy = 1;
		}

		/************************** GET THE NUMBER OF BANDS **********************/

		/*What happens if there are more than three bands (ECW SDK)? 
  		A compressed image file may contain from one to any number of bands. Typically a file will
  		contain 1 (grayscale) or 3 (color) bands, but not in every case (e.g. with hyperspectral imagery).
  		If your application is not performing any image processing functions, and is simply designed to
  		display a good image regardless of the number of input bands, we recommend the following
  		approach:
  		 For images with three or less bands, specify the number of bands in the image. For images with
  		more than three bands, specify 3 bands as the number to view.
  		 Select the first bands in the file. For example, use band 0 for a 1 band file, bands 0 and 1 for a
  		2 band file, 0,1, and 2 for a 3 band file, and bands 0, 1, and 2 for a 20 band file.
  		[NB] ... fill all of Red, green and Blue for an input view containing only 1 band, which ensures 
  		that a grayscale view will still appear correctly in your RGB or BGR based bitmap. */
		
		_nBands = _rasterDataset->GetRasterCount();
		//nBands = nBands <= 3 ? nBands : 3;		// older behavior

		/************************ INITIALISE BANDS AND KEEP THEM OPEN **************/

		for (int band = 1; band <= _nBands; band++)
		{
			if ((band == 1) && (_poBandR == NULL))
				_poBandR = _rasterDataset->GetRasterBand(band);
			else if ((band == 2) && (_poBandG == NULL))
				_poBandG = _rasterDataset->GetRasterBand(band);
			else if ((band == 3) && (_poBandB == NULL))
				_poBandB = _rasterDataset->GetRasterBand(band);
		}

		if (_poBandR == NULL)
		{
			retVal = false;
			return retVal;
		}
		
		_activeBandIndex = 1;

		/******************** GET THE DATA TYPE FROM THE FIRST BAND ***************/

		_dataType = _poBandR->GetRasterDataType();

		if(!(	(_dataType == GDT_Byte)||(_dataType == GDT_UInt16)||
		(_dataType == GDT_Int16)||(_dataType == GDT_UInt32)||
		(_dataType == GDT_Int32)||(_dataType == GDT_Float32)||
		(_dataType == GDT_Float64)||(_dataType == GDT_CInt16)||
		(_dataType == GDT_CInt32)||(_dataType == GDT_CFloat32)||
		(_dataType == GDT_CFloat64) ) ) 
			retVal = false;	

		switch (_dataType)
		{
			case GDT_Float32: case GDT_Float64:	
				_genericType=GDT_Float32; break;
			case GDT_Byte: case GDT_Int16: case GDT_UInt16:
				_genericType = GDT_Int32; break;
			default: 
				_genericType = GDT_Int32; break;
		}
		
		/******************** COLOR TABLE AND NUMBER OF BANDS ***************/

		// Initialize
		_palleteInterpretation = GPI_Gray;
		_histogramComputed = false;
		_allowHistogram = true;

		//allowAsGrid = true;				// Allow the image to be read as grid (if appropriate)
		_imageQuality = 100;				// Image quality 10-100
		_predefinedColors = FallLeaves;	// Set the default color scheme if read as grid
		_useHistogram = false;			// Use histogram equalization

		_colorTable = _poBandR->GetColorTable();
		if (_colorTable != NULL)
		{
			_hasColorTable = true;
			_palleteInterpretation = _colorTable->GetPaletteInterpretation();
		}
		else
		{
			_hasColorTable=false;
		}

		/************************** MIN/MAX AND RENDERING METHOD ***********************/
		
		// retrieving max and min values for the band
		ComputeBandMinMax();
		
		// choosing rendering method
		_handleImage = ChooseRenderingMethod();

		/********************* SET TRANSPARENCY COLOR AND GET THE COLOR INTERPRETATION **********************/

		_hasTransparency = false;
		double nDV=0;
		int pbSuccess = NULL;
		nDV = _poBandR->GetNoDataValue(&pbSuccess);
		
		if ((pbSuccess != NULL) && _hasColorTable)
		{
			const GDALColorEntry * poCE = GDALGetColorEntry (_colorTable, (int) nDV);
			if (poCE)
				_transColor = RGB(poCE->c1,poCE->c2,poCE->c3);
			else
			{
				OLE_COLOR clr = (OLE_COLOR)nDV;
				_transColor = RGB(GetRValue(clr), GetGValue(clr), GetBValue(clr));
			}
			_hasTransparency = true;
		}
		else if ((pbSuccess != NULL))
		{
			unsigned char nDVr=0,nDVg=0,nDVb=0;
			if (_nBands == 1 )
			{
				// nDVr = nDVg = 0;
				// nDVb = (unsigned char) nDV;
				_transColor = RGB(255,255,255);
				_hasTransparency = true;
			}
			else
			{
				if (_poBandR != NULL) nDVr = (unsigned char) _poBandR->GetNoDataValue();
				if (_poBandG != NULL) nDVg = (unsigned char) _poBandG->GetNoDataValue();
				if (_poBandB != NULL) nDVb = (unsigned char) _poBandB->GetNoDataValue();
				_transColor = RGB(nDVr,nDVg,nDVb);
				_hasTransparency = true;
			}
		}
		else
		{
			_transColor = RGB(0,0,0);
			_hasTransparency = false;
		}

		// setting extents
		RefreshExtents();
		_adfMinMax[0] = _dfMin;
		_adfMinMax[1] = _dfMax;

		/*********************** END OF INITIALISING ************************************/
		
		retVal = true;

	}
	__except(1)
	{
		CallbackHelper::ErrorMsg("Exception in LoadRaster function.");
		retVal = false;
	}

	return retVal;
}

// *********************************************************
//		RefreshExtents()
// *********************************************************
// When loading buffer the extents are used, so they should be refreshed after each 
// change of orig_XllCenter, orig_YllCenter, orig_dX, orig_dY
void tkRaster::RefreshExtents()
{
	_extents.left = _origXllCenter - _origDx * 0.5;
	_extents.right = _origXllCenter + (_origDx * (_origWidth-1));
	_extents.top = _origYllCenter + (_origDy * (_origHeight-1)); 
	_extents.bottom = _origYllCenter - _origDy * 0.5;
}

// *********************************************************
//		Dereference()
// *********************************************************
int tkRaster::Dereference()
{
	if (_rasterDataset != NULL)
		return _rasterDataset->Dereference();
	else
		return -1;
}

// *********************************************************
//		Close()
// *********************************************************
void tkRaster::Close()
{
	if (_rasterDataset != NULL)
	{
		_rasterDataset->Dereference();
		delete _rasterDataset;
		_rasterDataset = NULL;
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

	_allowAsGrid = grForGridsOnly;
	_activeBandIndex = 1;
	_warped = false;
}

// *********************************************************
//		LoadImageBuffer()
// *********************************************************
bool tkRaster::LoadBuffer(colour ** ImageData, double MinX, double MinY, double MaxX, double MaxY, CStringW filename,
							   tkInterpolationMode downsamplingMode, bool setRGBToGrey, double mapUnitsPerScreenPixel)
{
	if (! _rasterDataset ) 
		_rasterDataset = GdalHelper::OpenRasterDatasetW(filename, GA_ReadOnly);
	
	if (! _rasterDataset ) 
		return false;

	if ((_extents.left > MaxX) || (_extents.right < MinX) || (_extents.top < MinY) || (_extents.bottom > MaxY))
    {	
		//If the image is not in the view area don't waste time drawing it but don't
		//set the height & width to 0 because that would cause a bug in MapWindow - IntegerToColor
		if(*ImageData)
			delete [] (*ImageData);
		(*ImageData) = new colour[1];

		_dX = _origDx;           
		_dY = _origDy; 		

		_width = _height = 1;
	}
    else
    {
		int hiddenPixels;

		_visibleExtents.left = _extents.left;
		_visibleRect.left = 0;
		if (_extents.left < MinX) 
		{	
			hiddenPixels = int((MinX - _extents.left - _origDx/2.0)/_origDx);		// how many pixels are completely hidden: rounding in the lower side
			_visibleExtents.left += hiddenPixels * _origDx;
			_visibleRect.left += hiddenPixels;
		}
	    
		_visibleExtents.bottom = _extents.bottom;
		_visibleRect.bottom = _origHeight - 1;
		if (_extents.bottom < MinY)
		{	
			hiddenPixels = int((MinY - _extents.bottom)/_origDy);	// how many pixels are completely hidden: rounding in the lower side
			_visibleExtents.bottom +=  hiddenPixels * _origDy;
			_visibleRect.bottom -= hiddenPixels;
		}
		
		_visibleExtents.right = _extents.right;
		_visibleRect.right = _origWidth - 1;
		if (_extents.right > MaxX) 
		{	
			hiddenPixels = int((_extents.right - MaxX)/_origDx);	// how many pixels are completely hidden: rounding in the lower side
			_visibleExtents.right -= hiddenPixels * _origDx;
			_visibleRect.right -= hiddenPixels;
		}

		_visibleExtents.top = _extents.top;
		_visibleRect.top = 0;
		if (_extents.top > MaxY)
		{	
			hiddenPixels = int((_extents.top-MaxY)/_origDy);		// how many pixels are completely hidden: rounding in the lower side
			_visibleExtents.top -=  hiddenPixels * _origDy;
			_visibleRect.top += hiddenPixels;
		}
		
		// size of image buffer without scaling
		_dX = _origDx;	// map units per image pixel
		_dY = _origDy;
		_xllCenter = _visibleExtents.left + _dX * 0.5;
		_yllCenter = _visibleExtents.bottom + _dY * 0.5;
		_width = _visibleRect.right - _visibleRect.left + 1;
		_height = _visibleRect.bottom - _visibleRect.top + 1;
		
		int xBuff = _width;
		int yBuff = _height;

		for (int i = 0; ; i++)
		{
			// In case of large images and down sampling it makes sense to reduce the size of buffer,
			// as additional details will be lost all the same but memory usage can be unacceptable.
			
			double mapUnitsPerImagePixel = (_dX + _dY);			// we intentionally don't divide by 2 here, as we'll check the possibility of decreasing buffer by 2
			
			if (downsamplingMode == imBicubic || downsamplingMode == imBilinear)		// In case of interpolation algorithm different form nearest neigbour is used 
				mapUnitsPerImagePixel *= 2.0;											// the buffer should be noticeably bigger than screen dimensions to preserve details
			
			if (downsamplingMode == imHighQualityBicubic || downsamplingMode == imHighQualityBilinear)
				mapUnitsPerImagePixel *= 3.0;

			if ( mapUnitsPerImagePixel < mapUnitsPerScreenPixel )		// it's down sampling
			{
				int xTemp = xBuff/2;
				int yTemp = yBuff/2;
				_dX = _dX * double(xBuff)/double(xTemp);
				_dY = _dY * double(yBuff)/double(yTemp);
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
		
		// the image of reduced quality was requested
		if ( _imageQuality != 100 && _width > 10 && _height > 10)
		{
			int xTemp = int((double)xBuff * double(_imageQuality)/100.0);
			int yTemp = int((double)yBuff * double(_imageQuality)/100.0);
			_dX = _dX * double(xBuff)/double(xTemp);
			_dY = _dY * double(yBuff)/double(yTemp);
			xBuff = xTemp;
			yBuff = yTemp;
			_xllCenter = _visibleExtents.left + _dX * 0.5;
			_yllCenter = _visibleExtents.bottom + _dY * 0.5;
		}

		// ----------------------------------------------------
		//   Building a histogram
		// ----------------------------------------------------
		if ((_useHistogram) && (_allowHistogram))
		{
			//Check all the conditions are met for histogram equalization compute the histogram once
			if (_nBands == 1 && !_hasColorTable && !(WillBeRenderedAsGrid()))    // handleImage == asGrid && allowAsGrid
			{
				if (!_histogramComputed)
				{
					_nLUTBins = 256;
					if(ComputeEqualizationLUTs(filename, &_padfScaleMin, &_padfScaleMax, &_papanLUTs))
						_histogramComputed = true;	
					else 
					{
						_histogramComputed = false;	//Failed - don't try again 
						_allowHistogram = false;
					}
				}
			}
			else 
				_histogramComputed = false;
		}
		
		// ---------------------------------------------------------
		//		Reading data to the buffer
		// ---------------------------------------------------------
		bool success;
		if ( this->WillBeRenderedAsGrid() )
		{
			// if user passed a color scheme, image will be opened as grid using the first band
			if (_genericType == GDT_Int32)
				success =  ReadGridAsImage<_int32>(ImageData, _visibleRect.left, _visibleRect.top, _width, _height, xBuff, yBuff, setRGBToGrey);
			else
				success =  ReadGridAsImage<float>(ImageData, _visibleRect.left, _visibleRect.top, _width, _height, xBuff, yBuff, setRGBToGrey);
		}
		else
			success = ReadImage(ImageData, _visibleRect.left, _visibleRect.top, _width, _height, xBuff, yBuff);
		
		if ( success )
		{
			_width = xBuff;
			_height = yBuff;
		}
		else
		{
			// TODO: clear the buffer and parameters
		}
	}
	return true;
}

// *********************************************************
//		LoadBufferFull()
// *********************************************************
// maxBufferSize - the maximum size of buffer in megabytes if non-positive value is specified, 
// the whole image will be loaded
bool tkRaster::LoadBufferFull(colour** ImageData, CStringW filename, double maxBufferSize)
{
	if (! _rasterDataset ) 
		_rasterDataset = GdalHelper::OpenRasterDatasetW(filename, GA_ReadOnly);
	
	if (! _rasterDataset ) 
		return false;
	
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

	return ReadImage(ImageData, _visibleRect.left, _visibleRect.top, _origWidth, _origHeight, _width, _height);
}

// *********************************************************
//		ReadImage()
// *********************************************************
bool tkRaster::ReadImage(colour ** ImageData, int xOffset, int yOffset, int width, int height, int xBuff, int yBuff)  //, bool useHistogram, bool clearGDALCache)
{
    // -----------------------------------------------
	//  allocating memory for initial buffer
	// -----------------------------------------------
	unsigned char * srcDataChar = NULL;
	_int32 * srcDataInt = NULL;
	float * srcDataFloat = NULL;
	
	if (_handleImage == asComplex)
	{
		if (_genericType == GDT_Int32)
			srcDataInt =  (_int32 *) CPLMalloc( sizeof(_int32)*xBuff*yBuff );
		else
			srcDataFloat =  (float *) CPLMalloc( sizeof(float)*xBuff*yBuff );
	}
	else
		srcDataChar =  (unsigned char *) CPLMalloc( sizeof(unsigned char)*xBuff*yBuff );
	
	// -----------------------------------------------
	//  allocating memory for second buffer
	// -----------------------------------------------
	if(*ImageData != NULL) delete [] (*ImageData);
    try
	{
		(*ImageData) = new colour[xBuff*yBuff];
	}
	catch(...)
	{
		return false;
	}
	
	// -----------------------------------------------
	//    preparing to read
	// -----------------------------------------------
	GDALRasterBand* poBand = NULL;
	const GDALColorEntry * poCE = NULL;	

	double shift = 0, range = 0;
	if (_handleImage == asComplex)
	{
		double dfMin = _adfMinMax[0];
		double dfMax = _adfMinMax[1];
		range = dfMax - dfMin;		
		shift = 0 - dfMin;
	}
	else
	{
		range=255;
		shift=0;
	}

    for (int band = 1; band <= _nBands; band++)
    {
		// -----------------------------------------------
		//   reading the band
		// -----------------------------------------------
		if (band == 1) poBand = _poBandR; 
		else if (band == 2) poBand = _poBandG;
		else if (band == 3) poBand = _poBandB;
		
		if (poBand == NULL)
		{
			poBand = _rasterDataset->GetRasterBand(band);	// Keep it open until the dataset is destroyed
		}
		else
		{
			if (_handleImage == asComplex)
			{
				if (_genericType == GDT_Int32)
				{
					poBand->AdviseRead ( xOffset, yOffset, width, height, xBuff, yBuff, GDT_Int32, NULL);
					poBand->RasterIO( GF_Read, xOffset, yOffset, width, height, srcDataInt, xBuff, yBuff, GDT_Int32, 0, 0 );
					
				}
				else
				{
					poBand->AdviseRead ( xOffset, yOffset, width, height, xBuff, height, GDT_Float32, NULL);
					poBand->RasterIO( GF_Read, xOffset, yOffset, width, height, srcDataFloat, xBuff, yBuff, GDT_Float32, 0, 0 );
				}
			}
			else
			{
				poBand->AdviseRead ( xOffset, yOffset, width, height, xBuff, yBuff, GDT_Byte, NULL);
				poBand->RasterIO( GF_Read, xOffset, yOffset, width, height, srcDataChar, xBuff, yBuff, GDT_Byte, 0, 0 );
			}
		}
		double noDataValue = poBand->GetNoDataValue();
		_cInterp = poBand->GetColorInterpretation();

		// -----------------------------------------------------------
		//   caching the color table; adding data to the second buffer
		// -----------------------------------------------------------
		int tableCount = 0;
		if (_hasColorTable)
			tableCount = _colorTable->GetColorEntryCount();
		
		if (_hasColorTable && tableCount > 0)
		{
			// caching the table in the array for faster access
			colour* colorTable = new colour[tableCount];
			for (int i = 0; i < tableCount; i++ )
			{
				poCE = _colorTable->GetColorEntry(i);
				GDALColorEntry2Colour(band, i, shift, range, noDataValue, poCE, _useHistogram, colorTable + i);	// possible to optimize further: no need to check all values
			}
			
			// reading and decoding the bitmap values
			colour* dstRow;	int* srcRow; int index;
			for (int i = 0; i < yBuff; i++)
			{
				dstRow = (*ImageData) + i * xBuff;
				srcRow = srcDataInt + (yBuff-i-1) * xBuff;		// lsu: I assume that color table is present for _int32 images only, isn't it?
				for (int j = 0; j < xBuff; j++)
				{
					index =	*(srcRow + j);								
					memcpy(dstRow + j, colorTable + index, sizeof(colour));
				}
			}

			delete[] colorTable;
		}
		
		// ------------------------------------------------------------
		//   passing non-indexed data to the second buffer
		// ------------------------------------------------------------
		else
		{
			if (_handleImage == asComplex)
			{
				if (_genericType == GDT_Int32)
					AddToBufferAlt<_int32>(ImageData, srcDataInt, xBuff, yBuff, band, shift, range, noDataValue, poCE, _useHistogram);	
				else
					AddToBufferAlt<float>(ImageData, srcDataFloat, xBuff, yBuff, band, shift, range, noDataValue, poCE, _useHistogram);
			}
			else
				AddToBufferAlt<unsigned char>(ImageData, srcDataChar, xBuff, yBuff, band, shift, range, noDataValue, poCE, _useHistogram);
		}
    }
	
	// --------------------------------------------------------
	//	  releasing GDAL cache
	// --------------------------------------------------------
	if (clearGDALCache)
	{
		if (_poBandR != NULL) _poBandR->FlushCache();	
		if (_poBandG != NULL) _poBandG->FlushCache();	
		if (_poBandB != NULL) _poBandB->FlushCache();	
	}

	// --------------------------------------------------------
	//		cleaning
	// --------------------------------------------------------
	if (_handleImage == asComplex)
	{
		if (_genericType == GDT_Int32) {
			if( srcDataInt != NULL ) {
				CPLFree( srcDataInt );
				srcDataInt = NULL;
			}
		}
		else {
			if( srcDataFloat != NULL ) {
				CPLFree( srcDataFloat );
				srcDataFloat = NULL;
			} 
		}
	}
	if( srcDataChar != NULL ) {
		CPLFree( srcDataChar );
		srcDataChar = NULL;
	}
	return true;
}

// *************************************************************
//	  AddToBufferAlt
// *************************************************************
template <typename T>
bool tkRaster::AddToBufferAlt(colour ** ImageData, T* data, int xBuff, int yBuff,
					int band, double shift, double range, double noDataValue, const GDALColorEntry * poCE, bool useHistogram)
{
	T val = 0;		// value from source buffer	(_int32, float, unsigned char)
	colour* dst;	// position in the resulting buffer

	// histogram image
	if (useHistogram && _nBands == 1 && !_hasColorTable)
	{
		for (int i = 0; i < yBuff; i++) {
			for (int j = 0; j < xBuff; j++)	
			{
				val = data[(yBuff-i-1) * xBuff + j];	
				dst = (*ImageData) + i * xBuff + j;
				
				if ((double)val == noDataValue)
				{
					dst->blue = _transColor.b;
					dst->green = _transColor.g;
					dst->red = _transColor.r;
					//memcpy(dst, &transColor, sizeof(colour));
				}
				else
				{
					double dfScale = _nLUTBins / (_padfScaleMax[band-1] - _padfScaleMin[band-1]);
					int iBin = (int) (((double)val - _padfScaleMin[band-1]) * dfScale);
					iBin = MAX(0,MIN(_nLUTBins-1,iBin));
					const int * panLUT = _papanLUTs[band-1];
					if( panLUT )
					{
						dst->red = (unsigned char) panLUT[iBin];
						dst->green = (unsigned char) panLUT[iBin];
						dst->blue = (unsigned char) panLUT[iBin];
					}
					else
					{
						dst->red = (unsigned char) iBin;
						dst->green = (unsigned char) iBin;
						dst->blue = (unsigned char) iBin;
					}
				}
			}
		}
	}
	
	// complex image
	else if(_handleImage == asComplex)
	{
		double ratio = 255.0/(double)range;
		
		for (int i = 0; i < yBuff; i++) {
			for (int j = 0; j < xBuff; j++)	
			{
				val = data[(yBuff-i-1) * xBuff + j];	
				dst = (*ImageData) + i * xBuff + j;
				
				if (_nBands == 1)
				{
					if (val == noDataValue)
					{
						dst->blue = _transColor.b;
						dst->green = _transColor.g;
						dst->red = _transColor.r;
						//memcpy(dst, &transColor, sizeof(colour));
					}
					else
					{
						dst->red = static_cast<unsigned char>(double(val + shift) * ratio);
						dst->green = static_cast<unsigned char>(double(val + shift) * ratio);
						dst->blue = static_cast<unsigned char>(double(val + shift) * ratio);				
					}
				}
				else
				{
					if (val == noDataValue)
					{
						if (band == 1)		dst->red = _transColor.r;
						else if (band == 2)	dst->green = _transColor.g;
						else if (band == 3)	dst->blue = _transColor.b;
					}
					else
					{
						if (band == 1)      dst->red = static_cast<unsigned char>(double(val + shift)  * ratio);
						else if (band == 2) dst->green = static_cast<unsigned char>(double(val + shift) * ratio);
						else if (band == 3) dst->blue = static_cast<unsigned char>(double(val + shift)  * ratio);
					}
				}
			}
		}
	}
	
	// common image
	else
	{
		for (int i = 0; i < yBuff; i++) {
			for (int j = 0; j < xBuff; j++)	
			{
				val = (unsigned char)data[(yBuff-i-1) * xBuff + j];	
				dst = (*ImageData) + i * xBuff + j;
				
				if (_nBands == 1)
				{
					if (val == noDataValue)
					{
						dst->blue = _transColor.b;
						dst->green = _transColor.g;
						dst->red = _transColor.r;
						//memcpy(dst, &transColor, sizeof(colour));
					}
					else
					{
						dst->red = static_cast<unsigned char>(val);
						dst->green = static_cast<unsigned char>(val);
						dst->blue = static_cast<unsigned char>(val);
					}
				}
				else
				{
					if (val == noDataValue)
					{
						if (band == 1)		dst->red = _transColor.r;
						else if (band == 2)	dst->green = _transColor.g;
						else if (band == 3)	dst->blue = _transColor.b;
					}
					else
					{
						if (band == 1)      dst->red = static_cast<unsigned char>(val);
						else if (band == 2) dst->green = static_cast<unsigned char>(val);
						else if (band == 3) dst->blue = static_cast<unsigned char>(val);
					}
				}
			}
		}
	}
	return true;
}

// ****************************************************************
//		Filling colour structure from the GDAL colour value
// ****************************************************************
// Is used for caching colours when color table exists
// lsu:  are there all options should be considered here?
void tkRaster::GDALColorEntry2Colour(int band, double colorValue, double shift, double range, double noDataValue, const GDALColorEntry * poCE, bool useHistogram, colour* result)
{
	bool colorEntryExists = (poCE != NULL)?true:false;
	
	// transparent color
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
	}
	else if (useHistogram && _nBands == 1 && !_hasColorTable)
	{
		double dfScale = _nLUTBins / (_padfScaleMax[band-1] - _padfScaleMin[band-1]);
		int iBin = (int) ((colorValue - _padfScaleMin[band-1]) * dfScale);
		iBin = MAX(0,MIN(_nLUTBins-1,iBin));
		const int * panLUT = _papanLUTs[band-1];
		
		if( panLUT )
		{
			result->red = (unsigned char) panLUT[iBin];
			result->green = (unsigned char) panLUT[iBin];
			result->blue = (unsigned char) panLUT[iBin];
		}
		else
		{
			result->red = (unsigned char) iBin;
			result->green = (unsigned char) iBin;
			result->blue = (unsigned char) iBin;
		}
	}
	else if (_hasColorTable && colorEntryExists)
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
	} 

	else if (_handleImage == asComplex)
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
	}
	else if (_nBands == 1)
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
	return;
}

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
//		ReadGridAsImage()
// *********************************************************
template <typename DataType>
bool tkRaster::ReadGridAsImage(colour** ImageData, int xOff, int yOff, int width, int height, int xBuff, int yBuff, bool setRGBToGrey) 
{
	return ReadGridAsImage2<DataType>(ImageData, xOff, yOff, width, height, xBuff, yBuff, setRGBToGrey);
}

template <typename DataType>
bool tkRaster::ReadGridAsImage2(colour** ImageData, int xOff, int yOff, int width, int height, int xBuff, int yBuff, bool setRGBToGrey) 
{
	_poBandR = _rasterDataset->GetRasterBand(_activeBandIndex);
	if (_poBandR == NULL) return false;

	float noDataValue = 0;
	try
	{
		// Some IMG files have no nodata value -- no way to tell except
		// to try. A non-SEH exception is thrown if nodata isn't available
		noDataValue = static_cast<float>(_poBandR->GetNoDataValue());
	}
	catch(...)
	{
	}	

	const float dfMin = static_cast<float>(_adfMinMax[0]);
    const float dfMax = static_cast<float>(_adfMinMax[1]);
	const float range = static_cast<float>(dfMax - dfMin);
	const float shift = static_cast<float>(0 - dfMin);	

	const float radiansToDegrees = (float)(180.0 / 3.14159);  
	const float degreesToRadians = (float)(3.14159 / 180.0); 

	const float z = 1.0; 
	const float scale = 1.0;
	const float az = 315.0;
	const float alt = 45.0;
	
	if(*ImageData)
		delete[] (*ImageData);
    (*ImageData) = new colour[xBuff*yBuff];


	DataType* pafScanArea = (DataType*)CPLMalloc( sizeof(DataType)*xBuff*yBuff );

	const float xll = static_cast<float>(_xllCenter);
	const float yll = static_cast<float>(_yllCenter);

	//Hard code csize, so that the vectors are normal
	const float csize = 30;

	IGridColorScheme* gridColorScheme = GetColorSchemeForRendering();
	long numBreaks = 0;
	gridColorScheme->get_NumBreaks(&numBreaks);
	
	std::vector<BreakVal> bvals;	
	for(int i = 0; i < numBreaks; i++ )
	{	
		IGridColorBreak * bi = NULL;
		gridColorScheme->get_Break(i, &bi);
		BreakVal bv(bi);

		bvals.push_back( bv );
	}

	//Bug 1389 Make sure the incoming gridColorScheme from _pushSchemetkRaster has the same no-data color
	gridColorScheme->put_NoDataColor(_transColor);

	float ai = 0.0;
	float li = 0.0;
	{
		const double ka = .7;
		const double kd = .8;

		double val;
		gridColorScheme->get_AmbientIntensity(&val);
		ai = static_cast<float>(val * ka);

		gridColorScheme->get_LightSourceIntensity(&val);
		li = static_cast<float>(val * kd);
	}

	double lsi, lsj, lsk;					
	IVector * v = NULL;
	gridColorScheme->GetLightSource(&v);
	v->get_i(&lsi);
	v->get_j(&lsj);
	v->get_k(&lsk);
	v->Release();
	cppVector lightsource(lsi,lsj,lsk);

	const float total = static_cast<float>(yBuff * xBuff);

	if (_genericType == GDT_Int32)
	{
		_poBandR->AdviseRead ( xOff, yOff,width, height, xBuff, yBuff, GDT_Int32, NULL);
		_poBandR->RasterIO( GF_Read, xOff, yOff,width, height, pafScanArea, xBuff, yBuff, GDT_Int32,0, 0 );
	}
	else
	{
		_poBandR->AdviseRead ( xOff, yOff, width, height, xBuff, yBuff, GDT_Float32, NULL);
		_poBandR->RasterIO( GF_Read, xOff, yOff,width, height, pafScanArea, xBuff, yBuff, GDT_Float32,0, 0 );
	}

	for (int i = 0; i < yBuff; i++)
	{
		for (int j = 0; j < xBuff; j++)
		{
			float tmp = (float)pafScanArea[(yBuff-i-1) * xBuff + j];
			
			if (tmp == noDataValue)
			{
				(*ImageData)[i * xBuff + j].red =  (unsigned char) _transColor.r;
				(*ImageData)[i * xBuff + j].green = (unsigned char) _transColor.g;
				(*ImageData)[i * xBuff + j].blue =  (unsigned char) _transColor.b;			
			} 
			else if ( setRGBToGrey == true )
			//Use Matt Perry's Method
			{
				int containsNull = 0;
				// Exclude the edges 
				if (i == 0 || j == 0 || i == yBuff-1 || j == xBuff-1 ) 
				{
					// We are at the edge so write doDataValue and move on
					(*ImageData)[i * xBuff + j].red =  (unsigned char) _transColor.r;
					(*ImageData)[i * xBuff + j].green = (unsigned char) _transColor.g;
					(*ImageData)[i * xBuff + j].blue =  (unsigned char) _transColor.b;
					//shadeBuf[j] = noDataValue;
					continue;
				}

				float win[9];
				// Read in 3x3 window
				win[0] = (float)( pafScanArea[(yBuff-i-1-1) * xBuff + j-1]);
				win[1] = (float)( pafScanArea[(yBuff-i-1  ) * xBuff + j  ]);
				win[2] = (float)( pafScanArea[(yBuff-i-1+1) * xBuff + j+1]);
				win[3] = (float)( pafScanArea[(yBuff-i-1-1) * xBuff + j-1]);
				win[4] = (float)( pafScanArea[(yBuff-i-1  ) * xBuff + j  ]);
				win[5] = (float)( pafScanArea[(yBuff-i-1+1) * xBuff + j+1]);
				win[6] = (float)( pafScanArea[(yBuff-i-1-1) * xBuff + j-1]);
				win[7] = (float)( pafScanArea[(yBuff-i-1  ) * xBuff + j  ]);
				win[8] = (float)( pafScanArea[(yBuff-i-1+1) * xBuff + j+1]);
				
				// Check if window has null value
				for (int n = 0; n <= 8; n++) 
				{
					if(win[n] == noDataValue) 
					{
						containsNull = 1;
						break;
					}
				}
				if (containsNull == 1) 
				{
					// We have nulls so write nullValue and move on
					(*ImageData)[i * xBuff + j].red =  (unsigned char) _transColor.r;
					(*ImageData)[i * xBuff + j].green = (unsigned char) _transColor.g;
					(*ImageData)[i * xBuff + j].blue =  (unsigned char) _transColor.b;
					//shadeBuf[j] = noDataValue;
					continue;
				} 
				else 
				{
					// We have a valid 3x3 window. Compute Hillshade
					// First Slope ...
					float x = (float)(((z*win[0] + z*win[3] + z*win[3] + z*win[6]) - 
						(z*win[2] + z*win[5] + z*win[5] + z*win[8])) /
						(8.0 * _dX * scale));

					float y = (float)(((z*win[6] + z*win[7] + z*win[7] + z*win[8]) - 
						(z*win[0] + z*win[1] + z*win[1] + z*win[2])) /
						(8.0 * _dY * scale));

					float slope = (float)(90.0 - atan(sqrt(x*x + y*y))*radiansToDegrees);
					
					// ... then aspect...
					float aspect = atan2(x,y);			
					
					// ... then the shade value
					float cang = (float)(sin(alt*degreesToRadians) * sin(slope*degreesToRadians) + 
						cos(alt*degreesToRadians) * cos(slope*degreesToRadians) * 
						cos((az-90.0)*degreesToRadians - aspect));

					if (cang <= 0.0) 
					{
						(*ImageData)[i * xBuff + j].red = (unsigned char) (1);
						(*ImageData)[i * xBuff + j].green = (unsigned char) (0);
						(*ImageData)[i * xBuff + j].blue = (unsigned char) (0);					
						//cang = noDataValue;
					}
					else
					{
						cang = (float)(255.0 * cang);
						(*ImageData)[i * xBuff + j].red = (unsigned char) (cang);
						(*ImageData)[i * xBuff + j].green = (unsigned char) (cang);
						(*ImageData)[i * xBuff + j].blue = (unsigned char) (cang);
					}
				}
			}
			else
			{ 
				// Use the normal hillshade method				
				auto colorBreak = FindBreak( bvals, tmp );

				if (colorBreak == nullptr) //A break is not defined for this value
				{
					(*ImageData)[i * xBuff + j].red = (unsigned char)_transColor.r;
					(*ImageData)[i * xBuff + j].green = (unsigned char)_transColor.g;
					(*ImageData)[i * xBuff + j].blue = (unsigned char)_transColor.b;
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

				colort ct;
				if( colortype == Hillshade )
				{
					float yone = 0, ytwo = 0, ythree = 0;
					//Cannot Compute Polygon ... Make the best guess
					if( j >= xBuff - 1 && i >= yBuff-1)
					{	
						yone = tmp;
						ytwo = tmp;
						ythree = tmp;
					}
					else if( j >= xBuff - 1 )
					{	
	
						yone =   (float)( pafScanArea[(yBuff-i-1  ) * xBuff + j-1]);
						ytwo =   (float)( pafScanArea[(yBuff-i-1-1) * xBuff + j  ]);
						ythree = (float)( pafScanArea[(yBuff-i-1  ) * xBuff + j-1]);
					}
					else if( i >= yBuff-1 )
					{	

						yone = (float)( pafScanArea[(yBuff-i-1+1) * xBuff + j  ]);
						ytwo = (float)( pafScanArea[(yBuff-i-1  ) * xBuff + j+1]);	
						ythree = tmp;
					}
					else
					{	
						yone = tmp;
						ytwo =   (float)( pafScanArea[(yBuff-i-1-1) * xBuff + j+1]);
						ythree = (float)( pafScanArea[(yBuff-i-1-1) * xBuff + j  ]);			
					}
				
					float xone = xll + csize * (yBuff-i-1);
					float xtwo = xone + csize;
					float xthree = xone;
					
					float zone = yll + csize * j;
					float ztwo = zone;
					float zthree = zone - csize;

					//check for nodata on triangle corners
					if( yone == noDataValue || ytwo == noDataValue || ythree == noDataValue )
					{	
						(*ImageData)[i * xBuff + j].red =  (unsigned char) _transColor.r;
						(*ImageData)[i * xBuff + j].green = (unsigned char) _transColor.g;
						(*ImageData)[i * xBuff + j].blue =  (unsigned char) _transColor.b;
						continue;
					}
					else
					{
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
							float I = ai + li * lightsource.dot(normal);
							if( I > 1.0f )
								I = 1.0f;

							GradientPercent gradient = computeGradient(tmp, lowVal, biRange, gradmodel);

							(*ImageData)[i * xBuff + j].red =  (unsigned char) (((double)GetRValue(lowColor)*gradient.left + (double)GetRValue(hiColor)*gradient.right )*I) %256;
							(*ImageData)[i * xBuff + j].green = (unsigned char) (((double)GetGValue(lowColor)*gradient.left + (double)GetGValue(hiColor)*gradient.right )*I) %256;
							(*ImageData)[i * xBuff + j].blue =  (unsigned char) (((double)GetBValue(lowColor)*gradient.left + (double)GetBValue(hiColor)*gradient.right )*I) %256;
						}
						continue;
					}
				}
				else if( colortype == Gradient )
				{
					GradientPercent gradient = computeGradient(tmp, lowVal, biRange, gradmodel);

					ct = RGB(
						(int)((float)GetRValue(lowColor)*gradient.left + (float)GetRValue(hiColor)*gradient.right ) %256,
						(int)((float)GetGValue(lowColor)*gradient.left + (float)GetGValue(hiColor)*gradient.right ) %256,
						(int)((float)GetBValue(lowColor)*gradient.left + (float)GetBValue(hiColor)*gradient.right ) %256);
				}
				else if( colortype == Random )
				{
					ct = RGB(GetRValue(lowColor), GetGValue(lowColor), GetBValue(lowColor));
				}

				(*ImageData)[i * xBuff + j].red =  (unsigned char) (ct.r);
				(*ImageData)[i * xBuff + j].green = (unsigned char) (ct.g);
				(*ImageData)[i * xBuff + j].blue =  (unsigned char) (ct.b);				
			}

		} 
	}
	
	// lsu: clearing cache    
	if (clearGDALCache)
		_poBandR->FlushCache();		 
	
	CPLFree( pafScanArea );
	return true;
}

// *************************************************************
//	  findBreak
// *************************************************************
inline const tkRaster::BreakVal* tkRaster::FindBreak(const std::vector<BreakVal> & bvals, double val) const
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

/************************************************************************/
/*                      ComputeEqualizationLUTs()                       */
/*                                                                      */
/*      Get an image histogram, and compute equalization luts from      */
/*      it.                                                             */
/************************************************************************/

bool tkRaster::ComputeEqualizationLUTs( CStringW filename,
                         double **ppadfScaleMin, double **ppadfScaleMax, 
                         int ***ppapanLUTs)

{
	int iBand;
    int nHistSize = 0;
    int *panHistogram = NULL;
	if (_rasterDataset == NULL) 
		_rasterDataset = GdalHelper::OpenRasterDatasetW(filename, GA_ReadOnly);

	GDALRasterBand * poBand;
    
	// For now we always compute min/max
    *ppadfScaleMin = (double *) CPLCalloc(sizeof(double),_nBands);
    *ppadfScaleMax = (double *) CPLCalloc(sizeof(double),_nBands);

    *ppapanLUTs = (int **) CPLCalloc(sizeof(int *),_nBands);

/* ==================================================================== */
/*      Process all bands.                                              */
/* ==================================================================== */
    for( iBand = 0; iBand < _nBands; iBand++ )
    {
		GDALColorInterp cInt; 
		poBand = _rasterDataset->GetRasterBand(iBand+1);
		cInt = poBand->GetColorInterpretation();
        CPLErr eErr;

/* -------------------------------------------------------------------- */
/*      Get a reasonable histogram.                                     */
/* -------------------------------------------------------------------- */
        eErr =
            poBand->GetDefaultHistogram( *ppadfScaleMin + iBand,
                                     *ppadfScaleMax + iBand,
                                     &nHistSize, &panHistogram, 
                                     TRUE, NULL, NULL );

        if( eErr != CE_None )
            return false;

        panHistogram[0] = 0; // zero out extremes (nodata, etc)
        panHistogram[nHistSize-1] = 0;

/* -------------------------------------------------------------------- */
/*      Total histogram count, and build cumulative histogram.          */
/*      We take care to use big integers as there may be more than 4    */
/*      Gigapixels.                                                     */
/* -------------------------------------------------------------------- */
        GIntBig *panCumHist = (GIntBig *) CPLCalloc(sizeof(GIntBig),nHistSize);
        GIntBig nTotal = 0;
        int iHist;

        for( iHist = 0; iHist < nHistSize; iHist++ )
        {
            panCumHist[iHist] = nTotal + panHistogram[iHist]/2;
            nTotal += panHistogram[iHist];
        }

        CPLFree( panHistogram );

        if( nTotal == 0 ) nTotal = 1;

/* -------------------------------------------------------------------- */
/*      Now compute a LUT from the cumulative histogram.                */
/* -------------------------------------------------------------------- */
        int *panLUT = (int *) CPLCalloc(sizeof(int),_nLUTBins);
        int iLUT;

        for( iLUT = 0; iLUT < _nLUTBins; iLUT++ )
        {
            iHist = (iLUT * nHistSize) / _nLUTBins;
            int nValue = (int) ((panCumHist[iHist] * _nLUTBins) / nTotal);
            panLUT[iLUT] = MAX(0,MIN(_nLUTBins-1,nValue));
        } 
		(*ppapanLUTs)[iBand] = panLUT;
    }
    return true;
}

// ***********************************************************
//		SetNoDataValue()
// ***********************************************************
bool tkRaster::SetNoDataValue(double Value)
{
	if (_rasterDataset)
	{
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
		
		// TODO: should we consider some other specific cases
		else
		{
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
	}
	else
		return false;
}

#pragma region Choose rendering
// *********************************************************
//		GetColorScheme()
// *********************************************************
// Which of the 2 color schemes can we use
IGridColorScheme* tkRaster::GetColorSchemeForRendering()
{
	long numBreaks;	
	bool hasCustomScheme = false;
	if (_customColorScheme) {
		_customColorScheme->get_NumBreaks(&numBreaks);
		if (numBreaks > 0) {
			hasCustomScheme = true;
		}
	}
	
	if (hasCustomScheme) {
		return _customColorScheme;
	}
	else
	{
		// make sure that at least everything is all right with predefined one
		_predefinedColorScheme->get_NumBreaks(&numBreaks);
		if (numBreaks == 0) {
			_predefinedColorScheme->UsePredefined(_dfMin, _dfMax, _predefinedColors);
		}
		return _predefinedColorScheme;
	}
}

// *************************************************************
//	  CanUseExternalColorScheme()
// *************************************************************
bool tkRaster::CanUseExternalColorScheme()
{
	// TODO: revisit; probably in some cases it's not possible after all
	//return dataType == GDT_Int32 || dataType == GDT_Float32;
	return true;
}

// *************************************************************
//	  ChooseRenderingMethod()
// *************************************************************
HandleImage tkRaster::ChooseRenderingMethod()
{
	HandleImage method;
	if (_imgType == IMG_FILE || _imgType == KAP_FILE || (_imgType == TIFF_FILE && _dataType == GDT_UInt16) || _hasColorTable )
	{
		method = asComplex;
	}
	else if	( _dataType == GDT_Byte && _imgType != ADF_FILE && _dfMax > 15)
	{
		method = asRGB;
	}
	else if (_dfMax > 1 &&( _nBands == 1 || _imgType == ADF_FILE || _imgType == ASC_FILE || _imgType == DEM_FILE)) 
	{
		method = asGrid;
	}
	else if (!IsRgb())
	{
		method = asGrid;
	}
	else
	{
		method = asComplex;
	}
	return method;
}

// *************************************************************
//	  WillBeRenderedAsGrid()
// *************************************************************
bool tkRaster::WillBeRenderedAsGrid()
{
	return (_allowAsGrid == tkGridRendering::grForceForAllFormats ||
		   (_allowAsGrid == tkGridRendering::grForGridsOnly && _handleImage == asGrid));
}
#pragma endregion

// *************************************************************
//	  IsRgb()
// *************************************************************
bool tkRaster::IsRgb()
{
	return GdalHelper::IsRgb(this->GetDataset());
}

// *************************************************************
//	  ApplyCustomColorScheme()
// *************************************************************
void tkRaster::ApplyCustomColorScheme(IGridColorScheme * scheme)
{
	ComHelper::SetRef((IDispatch*)scheme, (IDispatch**)&_customColorScheme, true);
}