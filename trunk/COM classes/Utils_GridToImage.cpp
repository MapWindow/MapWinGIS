#include "stdafx.h"
#include "Utils.h"
#include "colour.h"
#include "Image.h"
#include "Grid.h"
#include "Vector.h"

#define MAX_BMP_SIZE_PIXELS 10000000

// ****************************************************************
//		ReadBreaks()
// ****************************************************************
void ReadBreaks(IGridColorScheme* ci, std::deque<BreakVal>& result)
{
	long numBreaks = 0;
	ci->get_NumBreaks(&numBreaks);
	double lowval, highval;
	for (int i = 0; i < numBreaks; i++)
	{
		IGridColorBreak * bi = NULL;
		ci->get_Break(i, &bi);
		bi->get_LowValue(&lowval);
		bi->get_HighValue(&highval);
		bi->Release();

		BreakVal bv;
		bv.lowVal = lowval;
		bv.highVal = highval;
		result.push_back(bv);
	}
}

// ****************************************************************
//		findBreak()
// ****************************************************************
long CUtils::findBreak(std::deque<BreakVal> & bVals, double val)
{
	int sizeBVals = (int)bVals.size();
	for (int i = 0; i < sizeBVals; i++)
	{
		if (val >= bVals[i].lowVal &&
			val <= bVals[i].highVal)
			return i;
	}
	return -1;
}

// ****************************************************************
//		GetLightSource()
// ****************************************************************
void GetLightSource(IGridColorScheme* ci, cppVector& lightsource)
{
	// TODO: can be moved to IGridColorScheme
	double lsi, lsj, lsk;
	IVector * v = NULL;
	ci->GetLightSource(&v);
	v->get_i(&lsi);
	v->get_j(&lsj);
	v->get_k(&lsk);
	v->Release();

	lightsource.seti(lsi);
	lightsource.setj(lsj);
	lightsource.setk(lsk);
}

// *******************************************************
//		MemoryAvailable()
// *******************************************************
bool CUtils::MemoryAvailable(double bytes)
{
	MEMORYSTATUS stat;

	GlobalMemoryStatus(&stat);

	return (stat.dwAvailPhys >= bytes);
}

// ****************************************************************
//		WritePixel()
// ****************************************************************
inline void CUtils::WritePixel(IImage* img, int row, int col, OLE_COLOR color,
	int nodataColor_R, int nodataColor_G, int nodataColor_B, int ncols, bool inRam)
{
	if (inRam)
	{
		img->put_Value(row, col, color);
	}
	else
	{
		PutBitmapValue(col, row, nodataColor_R, nodataColor_G, nodataColor_B, ncols);
	}
}

// *************************************************************************
//		GridToImage()
// *************************************************************************
STDMETHODIMP CUtils::GridToImage(IGrid *Grid, IGridColorScheme *ci, ICallback *cBack, IImage ** retval)
{
	// choosing inRam (logic preserved from older versions for backward compatibility)
	IGridHeader * gridheader = NULL;
	Grid->get_Header(&gridheader);
	long ncols, nrows;
	gridheader->get_NumberCols(&ncols);
	gridheader->get_NumberRows(&nrows);
	gridheader->Release();
	bool inRam = MemoryAvailable(ncols * nrows) && MemoryAvailable(ncols * nrows * 2) && MemoryAvailable(static_cast<long>((ncols * nrows * sizeof(colour))));
	return RunGridToImage(Grid, ci, gpfBmpProxy, inRam, false, cBack, retval);
}

// *************************************************************************
//		GridToImage2()
//		Overload with imageFormat and inRam parameters
// *************************************************************************
STDMETHODIMP CUtils::GridToImage2(IGrid * Grid, IGridColorScheme * ci, tkGridProxyFormat imageFormat, VARIANT_BOOL inRam, ICallback* cBack, IImage ** retval)
{
	return RunGridToImage(Grid, ci, imageFormat, inRam == VARIANT_TRUE, true, cBack, retval);
}

// *************************************************************************
//		RunGridToImage()
// *************************************************************************
HRESULT CUtils::RunGridToImage(IGrid * Grid, IGridColorScheme * ci, tkGridProxyFormat imageFormat, 
							   bool inRam, bool checkMemory, ICallback* callback, IImage ** retval)
{
	if( Grid == NULL || ci == NULL )
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_FALSE;
	}

	CComPtr<IGridHeader> gridheader = NULL;
	Grid->get_Header(&gridheader);
	long ncols, nrows;
	gridheader->get_NumberCols(&ncols);
	gridheader->get_NumberRows(&nrows);
	double xll, yll;
	double dx = 1.0, dy = 1.0;
	gridheader->get_XllCenter(&xll);
	gridheader->get_YllCenter(&yll);
	gridheader->get_dX(&dx);
	gridheader->get_dY(&dy);

	if (inRam && checkMemory)
	{
		// Required memory -- columns times rows times size of colour struct	
		// Check against increasing sizes to prevent double wraparound
		bool hasMemory = (MemoryAvailable(ncols * nrows) && MemoryAvailable(ncols * nrows * 2) &&
			MemoryAvailable(static_cast<long>((ncols * nrows * sizeof(colour)))));

		if (!hasMemory) {
			// inRam version was requested explicitly, so we return error 
			// rather than substitute it with the disk version
			ErrorMessage(tkFAILED_TO_ALLOCATE_MEMORY);
			return S_FALSE;
		}
	}
		
	CGrid* g = (CGrid*)Grid;
	CStringW gridName = g->GetFilename();
	CStringW imageFile = g->GetProxyName();
	
	USES_CONVERSION;
	VARIANT_BOOL vb;
	CreateBitmap(imageFile, ncols, nrows, imageFormat, &vb);

	CComBSTR bstr;
	gridheader->get_Projection(&bstr);
	_rasterDataset->SetProjection(OLE2A(bstr));		// TODO: perhaps it should be converted into WKT format

	if (imageFormat == gpfTiffProxy)
	{
		// TODO: perhaps the rotation parameters should also be copied; though currently it seems there is no API to do
		double adfGeoTransform[6];
		adfGeoTransform[GEOTRSFRM_TOPLEFT_X] = xll - dx /2.0;
		adfGeoTransform[GEOTRSFRM_WE_RES] = dx;
		adfGeoTransform[GEOTRSFRM_ROTATION_PARAM1] = 0;
		adfGeoTransform[GEOTRSFRM_TOPLEFT_Y] = yll + ((nrows - 0.5) * dy);
		adfGeoTransform[GEOTRSFRM_ROTATION_PARAM2] = 0;
		adfGeoTransform[GEOTRSFRM_NS_RES] = -dy;
		_rasterDataset->SetGeoTransform(adfGeoTransform);
	}

	// the main processing
	GridToImageCore(Grid, ci, callback, inRam, retval);

	if (inRam)
	{
		(*retval)->put_XllCenter(xll);
		(*retval)->put_YllCenter(yll);
		(*retval)->put_dX(dx);
		(*retval)->put_dY(dy);
		
		// marking that it's a proxy
		CStringW gridName = ((CGrid*)Grid)->GetFilename();
		CImageClass* img = ((CImageClass*)(*retval));
		img->sourceGridName = gridName;
		img->isGridProxy = true;
	}
	else
	{
		// rewriting the legend (must be done before opening image so the legend is applied to it)
		CStringW legendName = ((CGrid*)Grid)->GetProxyLegendName();
		if (Utility::RemoveFile(legendName)) 
		{
			int bandIndex = 1;
			g->get_ActiveBandIndex(&bandIndex);
			CComBSTR bstrName(legendName);
			CComBSTR bstrGridName(gridName);
			ci->WriteToFile(bstrName, bstrGridName, bandIndex, &vb);
		}
		
		// open the created file
		CoCreateInstance(CLSID_Image, NULL, CLSCTX_INPROC_SERVER, IID_IImage, (void**)retval);
		if (*retval) {
			CImageClass* img = (CImageClass*)*retval;
			CComBSTR bstrName(imageFile);
			(*retval)->Open(bstrName, ImageType::USE_FILE_EXTENSION, false, NULL, &vb);
			if (!vb) {
				(*retval)->Release();
				(*retval) = NULL;
				return S_FALSE;
			}
		}
		
		(*retval)->put_XllCenter(xll);
		(*retval)->put_YllCenter(yll);
		(*retval)->put_dX(dx);
		(*retval)->put_dY(dy);

		// saving the world file
		CStringW WorldFile;
		if (imageFormat == gpfBmpProxy) 
		{
			WorldFile = Utility::GetPathWOExtension(imageFile) + ".bpw";
			
			if (WorldFile.GetLength() > 0) 
			{
				WriteWorldFile(WorldFile, imageFile, dx, dy, xll, yll, nrows);
			}
		}
		else 
		{
			GdalHelper::BuildOverviewsIfNeeded(imageFile, false, _globalCallback);		// built-in overviews
		}
	}
	return S_OK;
}

// *************************************************************************
//		Converts grid to image
// *************************************************************************
void CUtils::GridToImageCore(IGrid *Grid, IGridColorScheme *ci, ICallback *cBack, bool inRam, IImage ** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retval = NULL;

	if (_globalCallback == NULL && cBack != NULL)
	{
		_globalCallback = cBack;
		_globalCallback->AddRef();
	}
	
	CComPtr<IGridHeader> gridheader = NULL;
	Grid->get_Header(&gridheader);

	long ncols, nrows;
	gridheader->get_NumberCols(&ncols);
	gridheader->get_NumberRows(&nrows);

	if( ncols <= 0 || nrows <= 0 )
	{	
		ErrorMessage(tkZERO_ROWS_OR_COLS);
		return;
	}

	double xll, yll, nodata;
	CComVariant vndv;

	gridheader->get_XllCenter(&xll);
	gridheader->get_YllCenter(&yll);
	gridheader->get_NodataValue(&vndv);
	dVal(vndv, nodata);	
	
	//Hard code csize, so that the vectors are normal
	double csize = 30;
	double val = nodata;
	CComVariant vval;
	long break_index = 0;

	//Hillshade code
	double leftPercent = 0.0;
	double rightPercent = 0.0;
	long cnt = 0;

	OLE_COLOR nodataColor;
	ci->get_NoDataColor(&nodataColor);

	int nodataColor_R = GetRValue(nodataColor);
	int nodataColor_G = GetGValue(nodataColor);
	int nodataColor_B = GetBValue(nodataColor);

	double ka = .7;
	double kd = .8;

	double ai = 0.0, li = 0.0;
	ci->get_AmbientIntensity(&ai);
	ci->get_LightSourceIntensity(&li);

	cppVector lightsource(0.0,0.0,0.0);
	GetLightSource(ci, lightsource);
	
	VARIANT_BOOL vbretval;
	if (inRam)
	{
		CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)retval);
		(*retval)->CreateNew( ncols, nrows, &vbretval );
	}
	else
	{
		_canScanlineBuffer = (MemoryAvailable(ncols * (sizeof(_int32)* 3)));
	}

	std::deque<BreakVal> bvals;
	ReadBreaks(ci, bvals);

	double total = nrows * ncols;
	long newpercent = 0, percent = 0;

	for( int j = nrows-1; j >= 0; j-- )
	{				
		// it could be more smooth in the nested cycle but better to spare performance 
		Utility::DisplayProgress(_globalCallback, (nrows - j - 1)*ncols - 1, (int)total, "GridToImage", _key, percent);

		for( int i = 0; i < ncols; i++ )
		{	
			Grid->get_Value( i, j, &vval );
			dVal( vval, val );
			
			// the value is nodata
			if( val == nodata )
			{	
				WritePixel(*retval, j, i, nodataColor, nodataColor_R, nodataColor_G, nodataColor_B, ncols, inRam);
				continue;
			}

													  // if color scheme is large it may take time O(N^3)
			break_index = findBreak( bvals, val );    // can be reduced to O(N^2*log(N)) with a tree

			// a break is not defined for this value
			if( break_index < 0 )
			{	
				WritePixel(*retval, j, i, nodataColor, nodataColor_R, nodataColor_G, nodataColor_B, ncols, inRam);
				continue;
			}

			IGridColorBreak * bi = NULL;
			ci->get_Break( break_index, &bi );

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

			if( colortype == Hillshade )
			{
				double yone = 0, ytwo = 0, ythree = 0;
				CComVariant vyone, vytwo, vythree;

				//Cannot Compute Polygon ... Make the best guess
				if( i >= ncols - 1 || j <= 0 )
				{	
					if( i >= ncols - 1 && j <= 0)
					{	
						yone = val;
						ytwo = val;
						ythree = val;
					}
					else if( i >= ncols - 1 )
					{	
						Grid->get_Value( i - 1, j, &vyone );
						Grid->get_Value( i, j - 1, &vytwo );
						Grid->get_Value( i - 1, j - 1, &vythree );
						dVal(vyone,yone);
						dVal(vytwo,ytwo);
						dVal(vythree,ythree);
					}
					else if( j <= 0 )
					{	
						Grid->get_Value( i, j + 1, &vyone );
						Grid->get_Value( i + 1, j, &vytwo );						
						dVal(vyone,yone);
						dVal(vytwo,ytwo);						
						ythree = val;
					}
				}
				else
				{	
					yone = val;
					Grid->get_Value( i + 1, j - 1, &vytwo );
					Grid->get_Value( i, j - 1, &vythree );
					dVal(vytwo,ytwo);
					dVal(vythree,ythree);					
				}

				double xone = xll + csize*j;
				double xtwo = xone + csize;
				double xthree = xone;
				
				double zone = yll + csize*i;
				double ztwo = zone;
				double zthree = zone - csize;

				//check for nodata on triangle corners
				if( yone == nodata || ytwo == nodata || ythree == nodata )
				{	
					WritePixel(*retval, j, i, nodataColor, nodataColor_R, nodataColor_G, nodataColor_B, ncols, inRam);
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
					if( gradmodel == Linear )
					{	
						rightPercent = ( ( val - lowVal ) / biRange );
						leftPercent = 1.0 - rightPercent;					
					}
					else if( gradmodel == Logorithmic )
					{	
						double dLog = 0.0;
						double ht = val;
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
					else if( gradmodel == Exponential )
					{	
						double dLog = 0.0;
						double ht = val;
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

					int finalColorR = (int)(((double)GetRValue(lowColor)*leftPercent + (double)GetRValue(hiColor)*rightPercent )*I) %256;
					int finalColorG = (int)(((double)GetGValue(lowColor)*leftPercent + (double)GetGValue(hiColor)*rightPercent )*I) %256;
					int finalColorB = (int)(((double)GetBValue(lowColor)*leftPercent + (double)GetBValue(hiColor)*rightPercent )*I) %256;
					
					WritePixel(*retval, j, i, RGB(finalColorR, finalColorG, finalColorB), finalColorR, finalColorG, finalColorB, ncols, inRam);
				}
			}			
			else if( colortype == Gradient )
			{
				if( gradmodel == Linear )
				{	
					rightPercent = ( ( val - lowVal ) / biRange );
					leftPercent = 1.0 - rightPercent;			
				}
				else if( gradmodel == Logorithmic )
				{	
					double dLog = 0.0;
					double ht = val;
					if( ht < 1 )
						ht = 1.0;
					if( biRange > 1.0 && ht - lowVal > 1.0 )
					{	
						rightPercent = ( log( ht - lowVal)/log(biRange) );
						leftPercent = 1.0 - rightPercent;					
					}					
					else
					{	
						rightPercent = 0.0;
						leftPercent = 1.0;						
					}						
				}
				else if( gradmodel == Exponential )
				{	
					double dLog = 0.0;
					double ht = val;
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

				int finalColorR = (int)((double)GetRValue(lowColor)*leftPercent + (double)GetRValue(hiColor)*rightPercent ) %256;
				int finalColorG = (int)((double)GetGValue(lowColor)*leftPercent + (double)GetGValue(hiColor)*rightPercent ) %256;
				int finalColorB = (int)((double)GetBValue(lowColor)*leftPercent + (double)GetBValue(hiColor)*rightPercent ) %256;
					
				WritePixel(*retval, j, i, RGB(finalColorR, finalColorG, finalColorB), finalColorR, finalColorG, finalColorB, ncols, inRam);
			}
			else if( colortype == Random )
			{
				if (inRam)
				{
					(*retval)->put_Value( j, i, lowColor );
				}
				else
				{
					PutBitmapValue(i, j, GetRValue(lowColor), GetGValue(lowColor), GetBValue(lowColor), ncols);
				}
			}
		}		
	}
	
	if (!inRam)
	{
		

		FinalizeAndCloseBitmap(ncols);

		if (_rasterDataset != NULL)
		{
			_rasterDataset->FlushCache();
			GDALClose(_rasterDataset);
			_rasterDataset = NULL;
		}
	}

	Utility::DisplayProgressCompleted(_globalCallback, _key);
}

#pragma region Write disk-based

// *******************************************************
//		CreateBitmap()
// *******************************************************
void CUtils::CreateBitmap(CStringW filename, long cols, long rows, tkGridProxyFormat format, VARIANT_BOOL * retval)
{
	*retval = S_FALSE;

	GDALAllRegister();

    GDALDriver *poDriver;
    char **papszOptions = NULL;

	poDriver = GetGDALDriverManager()->GetDriverByName(format == gpfTiffProxy ? "GTiff" : "BMP");

	if( poDriver == NULL )
		return;

	bool hasOptions = false;
	if (format == gpfTiffProxy) {
		papszOptions = CSLSetNameValue( papszOptions, "COMPRESS", m_globalSettings.GetTiffCompression() );
		hasOptions = true;
	}

	m_globalSettings.SetGdalUtf8(true);
	_rasterDataset = poDriver->Create(Utility::ConvertToUtf8(filename), cols, rows, 3, GDT_Byte, papszOptions);
	m_globalSettings.SetGdalUtf8(false);

	if (hasOptions) {
		CSLDestroy( papszOptions );
	}

	if (_rasterDataset)
	{
		_poBandR = _rasterDataset->GetRasterBand(1);
		_poBandG = _rasterDataset->GetRasterBand(2);
		_poBandB = _rasterDataset->GetRasterBand(3);
		if (_poBandR != NULL && _poBandG != NULL && _poBandB != NULL)
		{
			*retval = S_OK;
			return;
		}
	}
	*retval = S_FALSE;
}

// *************************************************************
//	   PutBitmapValue()
// *************************************************************
inline void CUtils::PutBitmapValue(long col, long row, _int32 Rvalue, _int32 Gvalue, _int32 Bvalue, long totalWidth)
{
	if (_bufferANum == row)
	{
		_bufferA_R[col] = Rvalue;
		_bufferA_G[col] = Gvalue;
		_bufferA_B[col] = Bvalue;
		_bufferLastUsed = 'A';
	}
	else if (_bufferBNum == row)
	{
		_bufferB_R[col] = Rvalue;
		_bufferB_G[col] = Gvalue;
		_bufferB_B[col] = Bvalue;
		_bufferLastUsed = 'B';
	}
	else if (_canScanlineBuffer && _bufferLastUsed != 'A') // If A wasn't the last used, replace it.
	{
		_bufferLastUsed = 'A';
		// Write to row BufferANum
		_poBandR->RasterIO( GF_Write, 0, _bufferANum, totalWidth, 1, _bufferA_R, totalWidth, 1, GDT_Int32, 0, 0 );
		_poBandG->RasterIO( GF_Write, 0, _bufferANum, totalWidth, 1, _bufferA_G, totalWidth, 1, GDT_Int32, 0, 0 );
		_poBandB->RasterIO( GF_Write, 0, _bufferANum, totalWidth, 1, _bufferA_B, totalWidth, 1, GDT_Int32, 0, 0 );

		// Now that we're loading a different number into buffer,
		// reset BufferANum
		_bufferANum = row;

		// Fetch the buffer rather than creating anew; data may have been written to it out of order.
		if (_bufferA_R != NULL)
		{
			CPLFree(_bufferA_R);
			_bufferA_R = NULL;
		}

		_bufferA_R = (_int32*) CPLMalloc( sizeof(_int32)*totalWidth);
		_poBandR->RasterIO( GF_Read, 0, row, totalWidth, 1, _bufferA_R, totalWidth, 1, GDT_Int32, 0, 0 );

		if (_bufferA_G != NULL)
		{
			CPLFree(_bufferA_G);
			_bufferA_G = NULL;
		}

		_bufferA_G = (_int32*) CPLMalloc( sizeof(_int32)*totalWidth);
		_poBandG->RasterIO( GF_Read, 0, row, totalWidth, 1, _bufferA_G, totalWidth, 1, GDT_Int32, 0, 0 );

		if (_bufferA_B != NULL)
		{
			CPLFree(_bufferA_B);
			_bufferA_B = NULL;
		}

		_bufferA_B = (_int32*) CPLMalloc( sizeof(_int32)*totalWidth);
		_poBandB->RasterIO( GF_Read, 0, row, totalWidth, 1, _bufferA_B, totalWidth, 1, GDT_Int32, 0, 0 );

		// Finally, put the value.
		_bufferA_R[col] = Rvalue;
		_bufferA_G[col] = Gvalue;
		_bufferA_B[col] = Bvalue;
	}
	else if (_canScanlineBuffer && _bufferLastUsed != 'B') // If B wasn't the last used, replace it.
	{
		_bufferLastUsed = 'B';
		// Write to row BufferANum
		_poBandR->RasterIO( GF_Write, 0, _bufferBNum, totalWidth, 1, _bufferB_R, totalWidth, 1, GDT_Int32, 0, 0 );
		_poBandG->RasterIO( GF_Write, 0, _bufferBNum, totalWidth, 1, _bufferB_G, totalWidth, 1, GDT_Int32, 0, 0 );
		_poBandB->RasterIO( GF_Write, 0, _bufferBNum, totalWidth, 1, _bufferB_B, totalWidth, 1, GDT_Int32, 0, 0 );

		// Now that we're loading a different number into buffer,
		// reset BufferBNum
		_bufferBNum = row;

		// Fetch the buffer rather than creating anew; data may have been written to it out of order.
		if (_bufferB_R != NULL)
		{
			CPLFree(_bufferB_R);
			_bufferB_R = NULL;
		}

		_bufferB_R = (_int32*) CPLMalloc( sizeof(_int32)*totalWidth);
		_poBandR->RasterIO( GF_Read, 0, row, totalWidth, 1, _bufferB_R, totalWidth, 1, GDT_Int32, 0, 0 );

		if (_bufferB_G != NULL)
		{
			CPLFree(_bufferB_G);
			_bufferB_G = NULL;
		}

		_bufferB_G = (_int32*) CPLMalloc( sizeof(_int32)*totalWidth);
		_poBandG->RasterIO( GF_Read, 0, row, totalWidth, 1, _bufferB_G, totalWidth, 1, GDT_Int32, 0, 0 );

		if (_bufferB_B != NULL)
		{
			CPLFree(_bufferB_B);
			_bufferB_B = NULL;
		}

		_bufferB_B = (_int32*) CPLMalloc( sizeof(_int32)*totalWidth);
		_poBandB->RasterIO( GF_Read, 0, row, totalWidth, 1, _bufferB_B, totalWidth, 1, GDT_Int32, 0, 0 );

		// Finally, put the value.
		_bufferB_R[col] = Rvalue;
		_bufferB_G[col] = Gvalue;
		_bufferB_B[col] = Bvalue;
	}
	else
	{
		// Write directly to file
		_poBandR->RasterIO( GF_Write, col, row, 1, 1, &Rvalue, 1, 1, GDT_Int32, 0, 0 );
		_poBandG->RasterIO( GF_Write, col, row, 1, 1, &Gvalue, 1, 1, GDT_Int32, 0, 0 );
		_poBandB->RasterIO( GF_Write, col, row, 1, 1, &Bvalue, 1, 1, GDT_Int32, 0, 0 );
	}
}

// *************************************************************
//	   FinalizeAndCloseBitmap()
// *************************************************************
void CUtils::FinalizeAndCloseBitmap(int totalWidth)
{
	if (_bufferA_R != NULL && _bufferA_G != NULL && _bufferA_B != NULL && _bufferANum != -1)
	{
		_poBandR->RasterIO( GF_Write, 0, _bufferANum, totalWidth, 1, _bufferA_R, totalWidth, 1, GDT_Int32, 0, 0 );
		_poBandG->RasterIO( GF_Write, 0, _bufferANum, totalWidth, 1, _bufferA_G, totalWidth, 1, GDT_Int32, 0, 0 );
		_poBandB->RasterIO( GF_Write, 0, _bufferANum, totalWidth, 1, _bufferA_B, totalWidth, 1, GDT_Int32, 0, 0 );
	}
	if (_bufferB_R != NULL && _bufferB_G != NULL && _bufferB_B != NULL && _bufferBNum != -1)
	{
		_poBandR->RasterIO( GF_Write, 0, _bufferBNum, totalWidth, 1, _bufferB_R, totalWidth, 1, GDT_Int32, 0, 0 );
		_poBandG->RasterIO( GF_Write, 0, _bufferBNum, totalWidth, 1, _bufferB_G, totalWidth, 1, GDT_Int32, 0, 0 );
		_poBandB->RasterIO( GF_Write, 0, _bufferBNum, totalWidth, 1, _bufferB_B, totalWidth, 1, GDT_Int32, 0, 0 );
	}

	if (_bufferA_R != NULL)
	{
		CPLFree(_bufferA_R);
		_bufferA_R = NULL;
	}
	if (_bufferA_G != NULL)
	{
		CPLFree(_bufferA_G);
		_bufferA_G = NULL;
	}
	if (_bufferA_B != NULL)
	{
		CPLFree(_bufferA_B);
		_bufferA_B = NULL;
	}
	if (_bufferB_R != NULL)
	{
		CPLFree(_bufferB_R);
		_bufferB_R = NULL;
	}
	if (_bufferB_G != NULL)
	{
		CPLFree(_bufferB_G);
		_bufferB_G = NULL;
	}
	if (_bufferB_B != NULL)
	{
		CPLFree(_bufferB_B);
		_bufferB_B = NULL;
	}

	if (_rasterDataset != NULL)
	{
		delete _rasterDataset;
		_rasterDataset = NULL;
	}
}

// *******************************************************
//		WriteWorldFile()
// *******************************************************
void CUtils::WriteWorldFile(CStringW worldFile, CStringW imageFile, double dx, double dy, double xll, double yll, int nrows)
{
	FILE* fout = _wfopen(worldFile, L"w");
		
	if( !fout )
	{	
		ErrorMessage(tkCANT_WRITE_WORLD_FILE);
	}
	
	fprintf(fout,"%.14f\n",dx);
	fprintf(fout,"%.14f\n",0.0);
	fprintf(fout,"%.14f\n",0.0);
	fprintf(fout,"%.14f\n",dy*-1.0);
	
	//convert lower left to upper left pixel
	double xupLeft = xll;
	double yupLeft = yll + ( dy*(nrows-1));
	
	fprintf(fout,"%.14f\n",xupLeft);
	fprintf(fout,"%.14f\n",yupLeft);
	
	fprintf(fout,"%s\n","[tkImageCom]");
	fprintf(fout,"%s %s\n","ImageFile",imageFile);
	fflush(fout);
	fclose(fout);
}
#pragma endregion
