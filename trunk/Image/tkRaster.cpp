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
		return false;
	else
	{
		activeBandIndex = bandIndex;
		return true;
	}
}

// *************************************************************
//	  get_RasterBand()
// *************************************************************
GDALRasterBand* tkRaster::get_RasterBand(int BandIndex)
{
	if (BandIndex == 1) return _poBandR;
	else if (BandIndex == 1) return _poBandG;
	else if (BandIndex == 1) return _poBandB;
	else return NULL;
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
		if ((_dataType == GDT_Byte) &&  (_imgType != ADF_FILE) && (hasColorTable==false) && (_imgType != PNG_FILE))
			//Just guess the min and max
			//For some reason gdal does not seem to pick up the color table of a png binary image
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
	//AfxMessageBox("In LoadRaster");
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
		warped = false;
		GDALAllRegister();
		
		//Rob JPEG2000 fails at this point with a nasty error if MapWinGIS is 
		//compiled in debug mode. GIF images also crash here		
		
		if (_rasterDataset == NULL)
			_rasterDataset = GdalHelper::OpenDatasetA(filename, accessType);
	
		if( _rasterDataset == NULL ) 
		{
			retVal = false;
			return retVal;
		}

		orig_Width = _rasterDataset->GetRasterXSize();
		orig_Height = _rasterDataset->GetRasterYSize();			
		
		m_globalSettings.SetGdalUtf8(true);		// otherwise there can be problems when reading world file
												// as dataset filename is already stored as UTF8

		double adfGeoTransform[6];
		bool success = _rasterDataset->GetGeoTransform( adfGeoTransform ) == CE_None;

		m_globalSettings.SetGdalUtf8(false);

		if( success )
		{
            orig_dX = adfGeoTransform[1];
			orig_dY  = adfGeoTransform[5];
			// adfGeoTransform[0]&[3] refer to the corner (not the centre) of the left top pixel
			orig_XllCenter = adfGeoTransform[0]+ orig_dX/2.0;
			orig_YllCenter = adfGeoTransform[3] + orig_dY/2.0;
			
			// we got top corner and now we'll get coordinates of the bottom left corner; 
			// dy should be changed respectively as map coordinates will be increasing while moving from bottom to top
			orig_YllCenter += orig_dY * (orig_Height - 1);
			orig_dY *= -1;

			// Check if we need a warped VRT for this file.
			if ( adfGeoTransform[1] < 0.0 || adfGeoTransform[2] != 0.0 || adfGeoTransform[4] != 0.0 || adfGeoTransform[5] > 0.0 )
			{
				GDALDataset* mGdalDataset = NULL;
				mGdalDataset = (GDALDataset *)GDALAutoCreateWarpedVRT( _rasterDataset, NULL, NULL, GRA_NearestNeighbour, 0.2, NULL );  
				if (mGdalDataset)
				{
					_rasterDataset->Dereference();
					_rasterDataset = mGdalDataset;
					warped = true;

					if ( _rasterDataset->GetGeoTransform( adfGeoTransform )== CE_None)
					{
						orig_dX = adfGeoTransform[1];
						orig_dY  = adfGeoTransform[5];
						// adfGeoTransform[0]&[3] refer to the corner (not the centre) of the left top pixel
						orig_XllCenter = adfGeoTransform[0]+ orig_dX/2;
						orig_YllCenter = adfGeoTransform[3] + orig_dY/2;
						
						orig_Width = _rasterDataset->GetRasterXSize();
						orig_Height = _rasterDataset->GetRasterYSize();			

						// we got top corner and now we'll get coordinates of the bottom left corner; 
						// dy should be changed respectively as map coordinates will be increasing while moving from bottom to top
						orig_YllCenter += orig_dY * (orig_Height - 1);
						orig_dY *= -1;
					}
				}
			}	
		}
		else
		{	
			orig_XllCenter = 0;
			orig_YllCenter = 0;
			orig_dX = 1;
			orig_dY = 1;
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
		
		activeBandIndex = 1;

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

		//Initialise
		_cPI = GPI_Gray;
		_histogramComputed = false;
		_allowHistogram = true;

		//allowAsGrid = true;				// Allow the image to be read as grid (if appropriate)
		imageQuality = 100;				// Image quality 10-100
		_predefinedColors = FallLeaves;	// Set the default color scheme if read as grid
		useHistogram = false;			// Use histogram equalization

		_poCT = _poBandR->GetColorTable();
		if (_poCT != NULL)
		{
			hasColorTable = true;
			_cPI = _poCT->GetPaletteInterpretation();
			_ciName = GDALGetPaletteInterpretationName(paletteInterp);
		}
		else 
			hasColorTable=false;

		/************************** MIN/MAX AND RENDERING METHOD ***********************/
		
		// retrieving max and min values for the band
		ComputeBandMinMax();
		
		// choosing rendering method
		_handleImage = ChooseRenderingMethod();

		/********************* SET TRANSPARENCY COLOR AND GET THE COLOR INTERPRETATION **********************/

		hasTransparency = false;
		double nDV=0;
		int pbSuccess = NULL;
		nDV = _poBandR->GetNoDataValue(&pbSuccess);
		
		if ((pbSuccess != NULL) && hasColorTable)
		{
			const GDALColorEntry * poCE = GDALGetColorEntry (_poCT, (int) nDV);
			if (poCE)
				transColor = RGB(poCE->c1,poCE->c2,poCE->c3);
			else
			{
				OLE_COLOR clr = (OLE_COLOR)nDV;
				transColor = RGB(GetRValue(clr), GetGValue(clr), GetBValue(clr));
			}
			hasTransparency = true;
		}
		else if ((pbSuccess != NULL))
		{
			unsigned char nDVr=0,nDVg=0,nDVb=0;
			if (_nBands == 1 )
			{
				// nDVr = nDVg = 0;
				// nDVb = (unsigned char) nDV;
				transColor = RGB(255,255,255);
				hasTransparency = true;
			}
			else
			{
				if (_poBandR != NULL) nDVr = (unsigned char) _poBandR->GetNoDataValue();
				if (_poBandG != NULL) nDVg = (unsigned char) _poBandG->GetNoDataValue();
				if (_poBandB != NULL) nDVb = (unsigned char) _poBandB->GetNoDataValue();
				transColor = RGB(nDVr,nDVg,nDVb);
				hasTransparency = true;
			}
		}
		else
		{
			transColor = RGB(0,0,0);	// lsu 30-apr-2010: changed from RGB(0,0,1) as I see no logic in this
			hasTransparency = false;
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
		AfxMessageBox("Exception in LoadRaster function");
		retVal = false;
	}

	return retVal;
}

// *********************************************************
//		RefreshExtents()
// *********************************************************
// When loading buffer the extents are used, so they should be refreshed after each 
// chage of orig_XllCenter, orig_YllCenter, orig_dX, orig_dY
void tkRaster::RefreshExtents()
{
	_extents.left = orig_XllCenter - orig_dX * 0.5;
	_extents.right = orig_XllCenter + (orig_dX * (orig_Width-1));
	_extents.top = orig_YllCenter + (orig_dY * (orig_Height-1)); 
	_extents.bottom = orig_YllCenter - orig_dY * 0.5;
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
		_poBandR = NULL;		// lsu 28-may-2010 - we can't use this pointers any more
		_poBandG = NULL;		// if somebody will want to open new dataset
		_poBandB = NULL;
	}
	if (_predefinedColorScheme)
	{
		_predefinedColorScheme->Clear();
		_predefinedColorScheme->Release();
		_predefinedColorScheme = NULL;
	}
	allowAsGrid = grForGridsOnly;
	activeBandIndex = 1;
	warped = false;
}

// *********************************************************
//		LoadImageBuffer()
// *********************************************************
bool tkRaster::LoadBuffer(colour ** ImageData, double MinX, double MinY, double MaxX, double MaxY, CStringW filename,
							   tkInterpolationMode downsamplingMode, bool setRGBToGrey, double mapUnitsPerScreenPixel)
{
	if (! _rasterDataset ) 
		_rasterDataset = GdalHelper::OpenDatasetW(filename, GA_ReadOnly);
	
	if (! _rasterDataset ) 
		return false;

	if ((_extents.left > MaxX) || (_extents.right < MinX) || (_extents.top < MinY) || (_extents.bottom > MaxY))
    {	
		//If the image is not in the view area don't waste time drawing it but don't
		//set the height & width to 0 because that would cause a bug in MapWindow - IntegerToColor
		if(*ImageData)
			delete [] (*ImageData);
		(*ImageData) = new colour[1];

		dX = orig_dX;           
		dY = orig_dY; 		

		width = height = 1;
	}
    else
    {
		int hiddenPixels;

		_visibleExtents.left = _extents.left;
		visibleRect.left = 0;
		if (_extents.left < MinX) 
		{	
			hiddenPixels = int((MinX - _extents.left - orig_dX/2.0)/orig_dX);		// how many pixels are completely hidden: rounding in the lower side
			_visibleExtents.left += hiddenPixels * orig_dX;
			visibleRect.left += hiddenPixels;
		}
	    
		_visibleExtents.bottom = _extents.bottom;
		visibleRect.bottom = orig_Height - 1;
		if (_extents.bottom < MinY)
		{	
			hiddenPixels = int((MinY - _extents.bottom)/orig_dY);	// how many pixels are completely hidden: rounding in the lower side
			_visibleExtents.bottom +=  hiddenPixels * orig_dY;
			visibleRect.bottom -= hiddenPixels;
		}
		
		_visibleExtents.right = _extents.right;
		visibleRect.right = orig_Width - 1;
		if (_extents.right > MaxX) 
		{	
			hiddenPixels = int((_extents.right - MaxX)/orig_dX);	// how many pixels are completely hidden: rounding in the lower side
			_visibleExtents.right -= hiddenPixels * orig_dX;
			visibleRect.right -= hiddenPixels;
		}

		_visibleExtents.top = _extents.top;
		visibleRect.top = 0;
		if (_extents.top > MaxY)
		{	
			hiddenPixels = int((_extents.top-MaxY)/orig_dY);		// how many pixels are completely hidden: rounding in the lower side
			_visibleExtents.top -=  hiddenPixels * orig_dY;
			visibleRect.top += hiddenPixels;
		}
		
		// size of image buffer without scaling
		dX = orig_dX;	// map units per image pixel
		dY = orig_dY;
		XllCenter = _visibleExtents.left + dX * 0.5;
		YllCenter = _visibleExtents.bottom + dY * 0.5;
		width = visibleRect.right - visibleRect.left + 1;
		height = visibleRect.bottom - visibleRect.top + 1;
		
		int xBuff = width;
		int yBuff = height;

		for (int i = 0; ; i++)
		{
			// In case of large images and down sampling it makes sense to reduce the size of buffer,
			// as additional details will be lost all the same but memory usage can be unacceptable.
			
			double mapUnitsPerImagePixel = (dX + dY);			// we intentionally don't divide by 2 here, as we'll check the possibility of decreasing buffer by 2
			
			if (downsamplingMode == imBicubic || downsamplingMode == imBilinear)		// In case of interpolation algorithm different form nearest neigbour is used 
				mapUnitsPerImagePixel *= 2.0;											// the buffer should be noticeably bigger than screen dimensions to preserve details
			
			if (downsamplingMode == imHighQualityBicubic || downsamplingMode == imHighQualityBilinear)
				mapUnitsPerImagePixel *= 3.0;

			if ( mapUnitsPerImagePixel < mapUnitsPerScreenPixel )		// it's down sampling
			{
				int xTemp = xBuff/2;
				int yTemp = yBuff/2;
				dX = dX * double(xBuff)/double(xTemp);
				dY = dY * double(yBuff)/double(yTemp);
				xBuff = xTemp;
				yBuff = yTemp;
				//xBuff /= 2;
				//yBuff /= 2;
				//dX *= 2.0;
				//dY *= 2.0;
			}
			else
			{
				XllCenter = _visibleExtents.left + dX * 0.5;
				YllCenter = _visibleExtents.bottom + dY * 0.5;
				break;
			}
		}
		
		// the image of reduced quality was requested
		if ( imageQuality != 100 && width > 10 && height > 10)
		{
			int xTemp = int((double)xBuff * double(imageQuality)/100.0);
			int yTemp = int((double)yBuff * double(imageQuality)/100.0);
			dX = dX * double(xBuff)/double(xTemp);
			dY = dY * double(yBuff)/double(yTemp);
			xBuff = xTemp;
			yBuff = yTemp;
			XllCenter = _visibleExtents.left + dX * 0.5;
			YllCenter = _visibleExtents.bottom + dY * 0.5;
		}

		// ----------------------------------------------------
		//   Building a histogram
		// ----------------------------------------------------
		if ((useHistogram) && (_allowHistogram))
		{
			//Check all the conditions are met for histogram equalization compute the histogram once
			if (_nBands == 1 && !hasColorTable && !(WillBeRenderedAsGrid()))    // handleImage == asGrid && allowAsGrid
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
			success = ReadGridAsImage(ImageData, visibleRect.left, visibleRect.top, width, height, xBuff, yBuff, setRGBToGrey);
		}
		else
			success = ReadImage(ImageData, visibleRect.left, visibleRect.top, width, height, xBuff, yBuff);
		
		if ( success )
		{
			width = xBuff;
			height = yBuff;
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
		_rasterDataset = GdalHelper::OpenDatasetW(filename, GA_ReadOnly);
	
	if (! _rasterDataset ) 
		return false;
	
	// size of image buffer without scaling
	dX = orig_dX;	// map units per image pixel
	dY = orig_dY;
	XllCenter = orig_XllCenter;
	YllCenter = orig_YllCenter;
	width = orig_Width;
	height = orig_Height;

	int screenX = GetSystemMetrics(SM_CXFULLSCREEN);
	int screenY = GetSystemMetrics(SM_CYFULLSCREEN);
	
	double size = (maxBufferSize * (0x1 << 20)) / 3;  
	
	width = orig_Width;
	height = orig_Height;
	
	if ( maxBufferSize > 0 )
	{
		for (int i = 0; ; i++)
		{
			if (width * height / 4 > size)
			{
				width /= 2;
				height /= 2;
				dX *= 2.0;
				dY *= 2.0;
			}
			else
				break;
		}
	}
	
	// setting rectangles for consistency
	visibleRect.left = 0;
	visibleRect.right = orig_Width - 1;
	visibleRect.top = 0;
	visibleRect.bottom = orig_Height - 1;

	_visibleExtents.left = _extents.left;
	_visibleExtents.right = _extents.right;
	_visibleExtents.top = _extents.top;
	_visibleExtents.bottom = _extents.bottom;

	return ReadImage(ImageData, visibleRect.left, visibleRect.top, orig_Width, orig_Height, width, height);
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
		if (hasColorTable)
			tableCount = _poCT->GetColorEntryCount();
		
		if (hasColorTable && tableCount > 0)
		{
			// caching the table in the array for faster access
			colour* colorTable = new colour[tableCount];
			for (int i = 0; i < tableCount; i++ )
			{
				poCE = _poCT->GetColorEntry(i);
				GDALColorEntry2Colour(band, i, shift, range, noDataValue, poCE, useHistogram, colorTable + i);	// possible to optimize further: no need to check all values
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
					AddToBufferAlt<_int32>(ImageData, srcDataInt, xBuff, yBuff, band, shift, range, noDataValue, poCE, useHistogram);	
				else
					AddToBufferAlt<float>(ImageData, srcDataFloat, xBuff, yBuff, band, shift, range, noDataValue, poCE, useHistogram);
			}
			else
				AddToBufferAlt<unsigned char>(ImageData, srcDataChar, xBuff, yBuff, band, shift, range, noDataValue, poCE, useHistogram);
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
	if (useHistogram && _nBands == 1 && !hasColorTable)
	{
		for (int i = 0; i < yBuff; i++) {
			for (int j = 0; j < xBuff; j++)	
			{
				val = data[(yBuff-i-1) * xBuff + j];	
				dst = (*ImageData) + i * xBuff + j;
				
				if ((double)val == noDataValue)
				{
					dst->blue = transColor.b;
					dst->green = transColor.g;
					dst->red = transColor.r;
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
	
	// comlex image
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
						dst->blue = transColor.b;
						dst->green = transColor.g;
						dst->red = transColor.r;
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
						if (band == 1)		dst->red = transColor.r;
						else if (band == 2)	dst->green = transColor.g;
						else if (band == 3)	dst->blue = transColor.b;
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
						dst->blue = transColor.b;
						dst->green = transColor.g;
						dst->red = transColor.r;
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
						if (band == 1)		dst->red = transColor.r;
						else if (band == 2)	dst->green = transColor.g;
						else if (band == 3)	dst->blue = transColor.b;
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
			if (band == 1)		result->red = transColor.r;
			else if (band == 2)	result->green = transColor.g;
			else if (band == 3)	result->blue = transColor.b;
		}
	}
	else if (useHistogram && _nBands == 1 && !hasColorTable)
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
	else if (hasColorTable && colorEntryExists)
	{
		if (_cInterp == GCI_AlphaBand)
		{
			result->red = (unsigned char)poCE->c4;  
			result->green = (unsigned char)poCE->c4;
			result->blue = (unsigned char)poCE->c4; 
		}
		else if ( (_cPI == GPI_Gray) || (_cInterp == GCI_GrayIndex) )
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

// *********************************************************
//		ReadGridAsImage()
// *********************************************************
bool tkRaster::ReadGridAsImage(colour** ImageData, int xOff, int yOff, int width, int height, int xBuff, int yBuff, bool setRGBToGrey) 
{
	double      dfMin = 0.0, dfMax = 255.0, shift=0.0, range=255.0;
    int         bGotMin=false, bGotMax=false;
	
	double noDataValue = 0;
	bool makeTrPixel=false;
	double tmp=0;
	_int32 * pafScanAreaInt = NULL;
	float * pafScanAreaFloat = NULL;

	//if (poBandR == NULL)
	//{
	// poBandR = rasterDataset->GetRasterBand(1);
	// Keep them open until the dataset is destroyed
	//}
	
	Debug::WriteLine("ActiveBand: %d", activeBandIndex);
	_poBandR = _rasterDataset->GetRasterBand(activeBandIndex);
	if (_poBandR == NULL) return false;

	try
	{
		// Some IMG files have no nodata value -- no way to tell except
		// to try. A non-SEH exception is thrown if nodata isn't available
		noDataValue = _poBandR->GetNoDataValue();
	}
	catch(...)
	{
	}
    
	dfMin = _adfMinMax[0];
    dfMax = _adfMinMax[1];
	range = dfMax - dfMin;		
	shift = 0 - dfMin;
	
	/* Hillshade variables */

	const double radiansToDegrees = (double)(180.0 / 3.14159);  
	const double degreesToRadians = (double)(3.14159 / 180.0); 
	double  *win;
	double  x;
	double  y;
	double  aspect;
	double  slope;
	double  cang; 
	int    n;
	int    containsNull;
	win = (double *) CPLMalloc(sizeof(double)*9);

	double z = 1.0; 
	double scale = 1.0;
	double az = 315.0;
	double alt = 45.0;
	
	if(*ImageData)
		delete [] (*ImageData);
    (*ImageData) = new colour[xBuff*yBuff];


	if (_genericType == GDT_Int32)
		pafScanAreaInt =  (_int32 *) CPLMalloc( sizeof(_int32)*xBuff*yBuff );
	else
		pafScanAreaFloat =  (float *) CPLMalloc( sizeof(float)*xBuff*yBuff );

	double xll=XllCenter, yll=YllCenter;

	//Hard code csize, so that the vectors are normal
	double csize = 30;
	long break_index = 0;

	//Hillshade code
	double leftPercent = 0.0;
	double rightPercent = 0.0;
	long cnt = 0;

	double ka = .7;
	double kd = .8;
	
	IGridColorScheme* gridColorScheme = GetColorSchemeForRendering();
	long numBreaks = 0;
	gridColorScheme->get_NumBreaks(&numBreaks);
	
	//Bug 1389 Make sure the incoming gridColorScheme from _pushSchemetkRaster has the same no-data color
	gridColorScheme->put_NoDataColor(transColor);

	double ai = 0.0, li = 0.0;
	gridColorScheme->get_AmbientIntensity(&ai);
	gridColorScheme->get_LightSourceIntensity(&li);
	double lsi, lsj, lsk;					
	IVector * v = NULL;
	gridColorScheme->GetLightSource(&v);
	v->get_i(&lsi);
	v->get_j(&lsj);
	v->get_k(&lsk);
	v->Release();
	cppVector lightsource(lsi,lsj,lsk);

	std::deque<BreakVal> bvals;
	
	double lowval, highval;
	for( int i = 0; i < numBreaks; i++ )
	{	
		IGridColorBreak * bi = NULL;
		gridColorScheme->get_Break(i, &bi);
		bi->get_LowValue(&lowval);
		bi->get_HighValue(&highval);
		bi->Release();

		BreakVal bv;
		bv.lowVal = lowval;
		bv.highVal = highval;
		bvals.push_back( bv );
	}

	double total = yBuff * xBuff;
	long newpercent = 0, percent = 0;
	
	colort ct;

	if (_genericType == GDT_Int32)
	{
		_poBandR->AdviseRead ( xOff, yOff,width, height, xBuff, yBuff, GDT_Int32, NULL);
		_poBandR->RasterIO( GF_Read, xOff, yOff,width, height, pafScanAreaInt, xBuff, yBuff, GDT_Int32,0, 0 );
	}
	else
	{
		_poBandR->AdviseRead ( xOff, yOff, width, height, xBuff, yBuff, GDT_Float32, NULL);
		_poBandR->RasterIO( GF_Read, xOff, yOff,width, height, pafScanAreaFloat, xBuff, yBuff, GDT_Float32,0, 0 );
	}

	for (int i = 0; i < yBuff; i++)
	{
		for (int j = 0; j < xBuff; j++)
		{
			if (_genericType == GDT_Int32)
				tmp = (_int32)( pafScanAreaInt[(yBuff-i-1) * xBuff + j]);
			else
				tmp = (double)(pafScanAreaFloat[(yBuff-i-1) * xBuff + j]);
			
			//13-Nov-08 Rob Cairns: crude image transparency
			//if (pctTrans > 0)
			//	makeTrPixel = UseNoDataValue(i,j);
			//if ((makeTrPixel) || (tmp == noDataValue))
			if (tmp == noDataValue)
			{
				(*ImageData)[i * xBuff + j].red =  (unsigned char) transColor.r;
				(*ImageData)[i * xBuff + j].green = (unsigned char) transColor.g;
				(*ImageData)[i * xBuff + j].blue =  (unsigned char) transColor.b;			
			} 
			else if ( setRGBToGrey == true )
			//Use Matt Perry's Method
			{
				containsNull = 0;				
				// Exclude the edges 
				if (i == 0 || j == 0 || i == yBuff-1 || j == xBuff-1 ) 
				{
					// We are at the edge so write doDataValue and move on
					(*ImageData)[i * xBuff + j].red =  (unsigned char) transColor.r;
					(*ImageData)[i * xBuff + j].green = (unsigned char) transColor.g;
					(*ImageData)[i * xBuff + j].blue =  (unsigned char) transColor.b;
					//shadeBuf[j] = noDataValue;
					continue;
				}

				if (_genericType == GDT_Int32)
				{					// Read in 3x3 window
					win[0] = (double)( pafScanAreaInt[(yBuff-i-1-1) * xBuff + j-1]);
					win[1] = (double)( pafScanAreaInt[(yBuff-i-1) * xBuff + j]);
					win[2] = (double)( pafScanAreaInt[(yBuff-i-1+1) * xBuff + j+1]);
					win[3] = (double)( pafScanAreaInt[(yBuff-i-1-1) * xBuff + j-1]);
					win[4] = (double)( pafScanAreaInt[(yBuff-i-1) * xBuff + j]);
					win[5] = (double)( pafScanAreaInt[(yBuff-i-1+1) * xBuff + j+1]);
					win[6] = (double)( pafScanAreaInt[(yBuff-i-1-1) * xBuff + j-1]);
					win[7] = (double)( pafScanAreaInt[(yBuff-i-1) * xBuff + j]);
					win[8] = (double)( pafScanAreaInt[(yBuff-i-1+1) * xBuff + j+1]);
				}
				else
				{
					win[0] = (double)( pafScanAreaFloat[(yBuff-i-1-1) * xBuff + j-1]);
					win[1] = (double)( pafScanAreaFloat[(yBuff-i-1) * xBuff + j]);
					win[2] = (double)( pafScanAreaFloat[(yBuff-i-1+1) * xBuff + j+1]);
					win[3] = (double)( pafScanAreaFloat[(yBuff-i-1-1) * xBuff + j-1]);
					win[4] = (double)( pafScanAreaFloat[(yBuff-i-1) * xBuff + j]);
					win[5] = (double)( pafScanAreaFloat[(yBuff-i-1+1) * xBuff + j+1]);
					win[6] = (double)( pafScanAreaFloat[(yBuff-i-1-1) * xBuff + j-1]);
					win[7] = (double)( pafScanAreaFloat[(yBuff-i-1) * xBuff + j]);
					win[8] = (double)( pafScanAreaFloat[(yBuff-i-1+1) * xBuff + j+1]);
				}
				// Check if window has null value
				for ( n = 0; n <= 8; n++) 
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
					(*ImageData)[i * xBuff + j].red =  (unsigned char) transColor.r;
					(*ImageData)[i * xBuff + j].green = (unsigned char) transColor.g;
					(*ImageData)[i * xBuff + j].blue =  (unsigned char) transColor.b;
					//shadeBuf[j] = noDataValue;
					continue;
				} 
				else 
				{
					// We have a valid 3x3 window. Compute Hillshade
					// First Slope ...
					x = (double)(((z*win[0] + z*win[3] + z*win[3] + z*win[6]) - 
						(z*win[2] + z*win[5] + z*win[5] + z*win[8])) /
						(8.0 * dX * scale));

					y = (double)(((z*win[6] + z*win[7] + z*win[7] + z*win[8]) - 
						(z*win[0] + z*win[1] + z*win[1] + z*win[2])) /
						(8.0 * dY * scale));

					slope = (double)(90.0 - atan(sqrt(x*x + y*y))*radiansToDegrees);
					
					// ... then aspect...
					aspect = atan2(x,y);			
					
					// ... then the shade value
					cang = (double)(sin(alt*degreesToRadians) * sin(slope*degreesToRadians) + 
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
						cang = (double)(255.0 * cang);
						(*ImageData)[i * xBuff + j].red = (unsigned char) (cang);
						(*ImageData)[i * xBuff + j].green = (unsigned char) (cang);
						(*ImageData)[i * xBuff + j].blue = (unsigned char) (cang);
					}
				}
			}

			else
			{ 
				
				
				// Use the normal hillshade method
				newpercent = (long)(((i * xBuff + j)/total)*100);
				//Find the break
				break_index = findBreak( bvals, tmp );

				if (break_index < 0) //A break is not defined for this value
				{
					(*ImageData)[i * xBuff + j].red = (unsigned char)transColor.r;
					(*ImageData)[i * xBuff + j].green = (unsigned char)transColor.g;
					(*ImageData)[i * xBuff + j].blue = (unsigned char)transColor.b;
					continue;
				}

 			    IGridColorBreak * bi = NULL;
				gridColorScheme->get_Break( break_index, &bi );
				
				OLE_COLOR hiColor, lowColor;
				bi->get_HighColor(&hiColor);
				bi->get_LowColor(&lowColor);
				double hiVal, lowVal;
				bi->get_HighValue(&hiVal);
				bi->get_LowValue(&lowVal);
				double biRange = hiVal - lowVal;
				if( biRange <= 0.0 )
					biRange = 1.0;

				ColoringType colortype;
				bi->get_ColoringType(&colortype);
				GradientModel gradmodel;
				bi->get_GradientModel(&gradmodel);
				bi->Release();
				
				if (!allowHillshade && colortype == Hillshade)
					colortype = Gradient;

				if( colortype == Hillshade )
				{
					double yone = 0, ytwo = 0, ythree = 0;
					//Cannot Compute Polygon ... Make the best guess
					if( j >= xBuff - 1 || i >= yBuff-1 )
					{	
						if( j >= xBuff - 1 && i >= yBuff-1)
						{	
							yone = tmp;
							ytwo = tmp;
							ythree = tmp;
						}
						else if( j >= xBuff - 1 )
						{	
	
							if (_genericType == GDT_Int32)
							{
								yone = (double)( pafScanAreaInt[(yBuff-i-1) * xBuff + j-1]);
								ytwo = (double)( pafScanAreaInt[(yBuff-i-1-1) * xBuff + j]);
								ythree = (double)( pafScanAreaInt[(yBuff-i-1) * xBuff + j-1]);
							}
							else
							{
								yone = (double)(pafScanAreaFloat[(yBuff-i-1) * xBuff + j-1]);
								ytwo = (double)(pafScanAreaFloat[(yBuff-i-1-1) * xBuff + j]);
								ythree = (double)(pafScanAreaFloat[(yBuff-i-1) * xBuff + j-1]);
							}	
						}
						else if( i >= yBuff-1 )
						{	

							if (_genericType == GDT_Int32)
							{
								yone = (double)( pafScanAreaInt[(yBuff-i-1+1) * xBuff + j]);
								ytwo = (double)( pafScanAreaInt[(yBuff-i-1) * xBuff + j+1]);
							}
							else
							{
								yone = (double)(pafScanAreaFloat[(yBuff-i-1+1) * xBuff + j]);
								ytwo = (double)(pafScanAreaFloat[(yBuff-i-1) * xBuff + j+1]);
							}	
							ythree = tmp;
						}
					}
					else
					{	
						yone = tmp;
						if (_genericType == GDT_Int32)
						{
							ytwo = (double)( pafScanAreaInt[(yBuff-i-1-1) * xBuff + j+1]);
							ythree = (double)( pafScanAreaInt[(yBuff-i-1-1) * xBuff + j]);
						}
						else
						{
							ytwo = (double)(pafScanAreaFloat[(yBuff-i-1-1) * xBuff + j+1]);
							ythree = (double)(pafScanAreaFloat[(yBuff-i-1-1) * xBuff + j]);
						}			
					}
				
					double xone = xll + csize * (yBuff-i-1);
					double xtwo = xone + csize;
					double xthree = xone;
					
					double zone = yll + csize * j;
					double ztwo = zone;
					double zthree = zone - csize;

					//check for nodata on triangle corners
					if( yone == noDataValue || ytwo == noDataValue || ythree == noDataValue )
					{	
						(*ImageData)[i * xBuff + j].red =  (unsigned char) transColor.r;
						(*ImageData)[i * xBuff + j].green = (unsigned char) transColor.g;
						(*ImageData)[i * xBuff + j].blue =  (unsigned char) transColor.b;
						continue;
					}
					else
					{
						//Make Two Vectors
						cppVector one;
						one.seti( xone - xtwo );
						one.setj( yone - ytwo );
						one.setk( zone - ztwo );
						one.Normalize();
						cppVector two;
						two.seti( xone - xthree );
						two.setj( yone - ythree );
						two.setk( zone - zthree );
						two.Normalize();

						//Compute Normal
						cppVector normal = two.crossProduct( one );					

						//Compute I
						double I = ai*ka + li*kd*( lightsource.dot( normal ) );
						//Compute Gradient * I
						if( I > 1.0 )
							I = 1.0;
						//Two Color Gradient					
						//Linear
						if( gradmodel == Linear )
						{	
							rightPercent = ( ( tmp - lowVal ) / biRange );
							leftPercent = 1.0 - rightPercent;					
						}
						//Log
						else if( gradmodel == Logorithmic )
						{	
							double dLog = 0.0;
							double ht = tmp;
							if( ht < 1 )
								ht = 1.0;
							if( biRange > 1.0 && ht - lowVal > 1.0 )
							{	rightPercent = ( log( ht - lowVal)/log(biRange) );
								leftPercent = 1.0 - rightPercent;							
							}					
							else
							{	rightPercent = 0.0;
								leftPercent = 1.0;							
							}
								
						}
						//Exp
						else if( gradmodel == Exponential )
						{	
							double dLog = 0.0;
							double ht = tmp;
							if( ht < 1 )
								ht = 1.0;
							if( biRange > 1.0 )
							{	rightPercent = ( pow( ht - lowVal, 2)/pow(biRange, 2) );
								leftPercent = 1.0 - rightPercent;						
							}					
							else
							{	rightPercent = 0.0;
								leftPercent = 1.0;							
							}		
						}

						if (lowColor == hiColor)
						{
							(*ImageData)[i * xBuff + j].red = (unsigned char)(GetRValue(lowColor) % 256);
							(*ImageData)[i * xBuff + j].green = (unsigned char)(GetGValue(lowColor) % 256);
							(*ImageData)[i * xBuff + j].blue = (unsigned char)(GetBValue(lowColor) % 256);
						}
						else
						{
							(*ImageData)[i * xBuff + j].red =  (unsigned char) (((double)GetRValue(lowColor)*leftPercent + (double)GetRValue(hiColor)*rightPercent )*I) %256;
							(*ImageData)[i * xBuff + j].green = (unsigned char) (((double)GetGValue(lowColor)*leftPercent + (double)GetGValue(hiColor)*rightPercent )*I) %256;
							(*ImageData)[i * xBuff + j].blue =  (unsigned char) (((double)GetBValue(lowColor)*leftPercent + (double)GetBValue(hiColor)*rightPercent )*I) %256;	
						}
						continue;
						//ct = RGB((int)(((double)GetRValue(lowColor)*leftPercent + (double)GetRValue(hiColor)*rightPercent )*I) %256, (int)(((double)GetGValue(lowColor)*leftPercent + (double)GetGValue(hiColor)*rightPercent )*I) %256, (int)(((double)GetBValue(lowColor)*leftPercent + (double)GetBValue(hiColor)*rightPercent )*I) %256);
					}
				}
				else if( colortype == Gradient )
				{
					//Linear
					if( gradmodel == Linear )
					{	
						rightPercent = ( ( tmp - lowVal ) / biRange );
						leftPercent = 1.0 - rightPercent;			
					}
					//Log
					else if( gradmodel == Logorithmic )
					{	
						double dLog = 0.0;
						double ht = tmp;
						if( ht < 1 )
							ht = 1.0;
						if( biRange > 1.0 && ht - lowVal > 1.0 )
						{	rightPercent = ( log( ht - lowVal)/log(biRange) );
							leftPercent = 1.0 - rightPercent;					
						}					
						else
						{	
							rightPercent = 0.0;
							leftPercent = 1.0;						
						}						
					}
					//Exp
					else if( gradmodel == Exponential )
					{	
						double dLog = 0.0;
						double ht = tmp;
						if( ht < 1 )
							ht = 1.0;
						if( biRange > 1.0 )
						{
							rightPercent = ( pow( ht - lowVal, 2)/pow(biRange, 2) );
							leftPercent = 1.0 - rightPercent;
						}					
						else
						{
							rightPercent = 0.0;
							leftPercent = 1.0;						
						}		
					}
					ct = RGB((int)((double)GetRValue(lowColor)*leftPercent + (double)GetRValue(hiColor)*rightPercent ) %256, (int)((double)GetGValue(lowColor)*leftPercent + (double)GetGValue(hiColor)*rightPercent ) %256, (int)((double)GetBValue(lowColor)*leftPercent + (double)GetBValue(hiColor)*rightPercent ) %256);
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
	
	if (_genericType == GDT_Int32)
	{
		if( pafScanAreaInt != NULL )
		{
			CPLFree( pafScanAreaInt );
			pafScanAreaInt = NULL;
		}
	}
	else
	{
		if( pafScanAreaFloat != NULL )
		{
			CPLFree( pafScanAreaFloat );
			pafScanAreaFloat = NULL;
		} 
	}	
	return true;
}

// *************************************************************
//	  findBreak
// *************************************************************
inline long tkRaster::findBreak( std::deque<BreakVal> & bVals, double val )
{
	register int sizeBVals = (int)bVals.size();
	for(register int i = 0; i < sizeBVals; i++ )
	{	
		if( val >= bVals[i].lowVal &&
			val <= bVals[i].highVal )
			return i;
	}

	return -1;
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
		_rasterDataset = GdalHelper::OpenDatasetW(filename, GA_ReadOnly);

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
	Debug::WriteLine("Data type: %d", _dataType);
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
	if (_imgType == IMG_FILE || _imgType == KAP_FILE || (_imgType == TIFF_FILE && _dataType == GDT_UInt16) || hasColorTable )
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
	return (allowAsGrid == tkGridRendering::grForceForAllFormats ||
		   (allowAsGrid == tkGridRendering::grForGridsOnly && _handleImage == asGrid));
}
#pragma endregion

// *************************************************************
//	  IsRgb()
// *************************************************************
bool tkRaster::IsRgb()
{
	return GdalHelper::IsRgb(this->get_Dataset());
}
