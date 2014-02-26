//********************************************************************************************************
//File name: tkGridRaster.cpp
//Description:  Generic grid wrapper class to allow MapWinGIs to utilize formats supported by GDAL
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Christopher Michaelis.
//
//Contributor(s): (Open source contributors should list themselves and their modifications here).
//1-10-2006 - 1-18-2006 - cdm -- Initial Revision
//1-26-2006 - 1-18-2006 - cdm -- Double scanline buffering
//********************************************************************************************************

#include "stdafx.h"
#include <math.h>
#include <cassert>
#include <fstream>
#include <exception>

#include "grdTypes.h"
#include "tkGridRaster.h"
#include "ogr_srs_api.h"
#include "cpl_string.h"
#include "ogr_spatialref.h"

#include "projections.h"


extern "C" 
{
	#include "cq.h"
}

using namespace std;

#define SWAP(a, b) { \
    (a) ^= (b); \
    (b) ^= (a); \
    (a) ^= (b); \
}
   

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int MASKTABLE[] = { 0x0000, 0x0001, 0x0003, 0x0007,
                      0x000f, 0x001f, 0x003f, 0x007f,
					  0x00ff, 0x01ff, 0x03ff, 0x07ff,
					  0x0fff, 0x1fff, 0x3fff, 0x7fff };
const int SHIFTINC[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5 };
const int BITSLEFT[] = { 8, 7, 6, 5, 4, 3, 2, 1, 0 };
const int NUMREADINC[] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 };


void tkGridRaster::ProjToCell( double x, double y, long & column, long & row )
{	
	if( dX != 0.0 && dY != 0.0 )
	{	column = round( ( x - XllCenter )/dX );
		row = height - round( ( y - YllCenter )/dY ) - 1;
	}
}

void tkGridRaster::CellToProj( long column, long row, double & x, double & y )
{	
	x = XllCenter + column*dX;
	y = YllCenter + ( ( height - row - 1)*dY );
}

inline int tkGridRaster::round( double d )
{	if( ceil(d) - d <= .5 )
		return (int)ceil(d);
	else
		return (int)floor(d);
}

// Retrieve the color table, assuming that there is a color entry
// for each integer value between min and max.
// Eventual todo: Create one that gets unique values (could be time consuming)
// that would make it work for float values too.
// Chris Michaelis 2/2/2006
bool tkGridRaster::GetIntValueGridColorTable(IGridColorScheme ** newscheme)
{
	if (genericType == GDT_Float32)
		return false; // Only try for int grids.

	GDALColorTable * poCT = NULL;
	GDALColorInterp cInterp;
	const GDALColorEntry * poCE;
	double tmp=0;
	bool colorTableExists = false;
	bool colorEntryExists = false;
	int bGotMin = false;
	int bGotMax = false;
    int iMin = static_cast<int>(GDALGetRasterMinimum( poBand, &bGotMin ));
    int iMax = static_cast<int>(GDALGetRasterMaximum( poBand, &bGotMax ));

	double range = GDALGetRasterMaximum( poBand, &bGotMax ) - GDALGetRasterMinimum( poBand, &bGotMin );		
	double shift = 0 - GDALGetRasterMinimum( poBand, &bGotMin );

	poCT = poBand->GetColorTable();
	if (poCT != NULL)
		colorTableExists = true;

	if (!colorTableExists)
		return false;

	CoCreateInstance(CLSID_GridColorScheme,NULL,CLSCTX_INPROC_SERVER,IID_IGridColorScheme,(void**)newscheme);
	(*newscheme)->put_AmbientIntensity(0.7);
	(*newscheme)->put_LightSourceIntensity(0.7);
	(*newscheme)->SetLightSource(0,1);
	(*newscheme)->put_LightSourceIntensity(0.7);
	(*newscheme)->put_NoDataColor(0);

	cInterp = poBand->GetColorInterpretation(); 

	OLE_COLOR lastColor = RGB(0,0,0);
	int boundA = iMin;
	int boundB = iMin;

	for (int i = iMin; i < iMax; i++)
	{	
		poCE = GDALGetColorEntry (poCT, i);
		colorEntryExists = poCE != NULL;
		
		OLE_COLOR newColor = RGB(0,0,0);

		if (((colorEntryExists)&&((cInterp == GCI_GrayIndex)||(cInterp == GCI_AlphaBand)) ))
		{
			newColor = RGB( (unsigned char)poCE->c4, (unsigned char)poCE->c4, (unsigned char)poCE->c4);
		}
		else if (colorEntryExists)
		{
			newColor = RGB((unsigned char)poCE->c1, (unsigned char)poCE->c2, (unsigned char)poCE->c3);
		}
		else if (nBands == 1)		// lsu: 13-apr-13 - the behavior may change as nBands variable was never initialized
		{
			newColor = RGB((unsigned char)((tmp + shift) * 255/range),(unsigned char)((tmp + shift) * 255/range),(unsigned char)((tmp + shift) * 255/range));
		}
		
		if (newColor != lastColor && i != iMin)
		{
			// Put a break for the last chunk of color bands
			IGridColorBreak * newbreak;
			CoCreateInstance(CLSID_GridColorBreak,NULL,CLSCTX_INPROC_SERVER,IID_IGridColorBreak,(void**)&newbreak);
			newbreak->put_LowValue( boundA );
			newbreak->put_HighValue( boundB );
			newbreak->put_LowColor(lastColor);
			newbreak->put_HighColor(lastColor);
			(*newscheme)->InsertBreak(newbreak);
			newbreak->Release();

			// Set the new bounds and the new "last" color
			lastColor = newColor;
			boundA = i;
			boundB = i;
		}
		else if (newColor == lastColor)
		{
			boundB = i;
		}
	}

	// Write the last one
	IGridColorBreak * newbreak;
	CoCreateInstance(CLSID_GridColorBreak,NULL,CLSCTX_INPROC_SERVER,IID_IGridColorBreak,(void**)&newbreak);
	newbreak->put_LowValue( boundA );
	newbreak->put_HighValue( boundB );
	newbreak->put_LowColor(lastColor);
	newbreak->put_HighColor(lastColor);
	(*newscheme)->InsertBreak(newbreak);
	newbreak->Release();

	return true;
}



void tkGridRaster::SaveHeaderInfo()
{
	if (rasterDataset == NULL)
		return;

	if (poBand == NULL)
		return;

	poBand->SetNoDataValue(noDataValue);
	
	double adfGeoTransform[6];
	adfGeoTransform[0] = XllCenter - (dX / 2);
	adfGeoTransform[1] = dX;
	adfGeoTransform[2] = 0;
	adfGeoTransform[3] = (height * dY) + YllCenter + ((dY * -1) / 2);
	adfGeoTransform[4] = 0;
	adfGeoTransform[5] = dY * -1;

    rasterDataset->SetGeoTransform( adfGeoTransform );

	if (Projection.GetLength() != 0)
	{
		// SetProjection expects WKT
		if (startsWith(Projection.GetBuffer(), "+proj"))
		{
			char * wkt = NULL;

			ProjectionTools * p = new ProjectionTools();
			p->ToESRIWKTFromProj4(&wkt, Projection.GetBuffer());

			if (wkt != NULL && strcmp(wkt, "") != 0)
			{
				rasterDataset->SetProjection(wkt);
			}

			delete p; //added by Lailin Chen 12/30/2005, don't delete wkt
		}
		else
		{
			// Reasonably good indicator it's WKT
			if (contains(Projection.GetBuffer(), '[') && contains(Projection.GetBuffer(), ']'))
			{
				rasterDataset->SetProjection( Projection.GetBuffer() );
			}
		}
	}

	// Force reopen to update the nodata value
	poBand->FlushCache();
	rasterDataset->FlushCache();

	delete rasterDataset;
	poBand = NULL;
	
	rasterDataset = (GDALDataset *) GDALOpen(mFilename, GA_Update );
	if (rasterDataset == NULL)  // Attempt ro open
		rasterDataset = (GDALDataset *) GDALOpen(mFilename, GA_ReadOnly );

	if( rasterDataset != NULL )
	{
		poBand = rasterDataset->GetRasterBand(1);
	}
}

// *****************************************************
//		LoadRaster()
// *****************************************************
bool tkGridRaster::LoadRaster(const char * filename, bool InRam, GridFileType fileType)
{
	__try
	{
		CanScanlineBuffer = false;
		currentFileType = fileType;
		mFilename = filename;

		GDALAllRegister();
		rasterDataset = (GDALDataset *) GDALOpen(filename, GA_Update );

		if (rasterDataset == NULL)  // Attempt ro open
			rasterDataset = (GDALDataset *) GDALOpen(filename, GA_ReadOnly );
		
		if (!rasterDataset) {
			return false;
		}
		
		nBands = rasterDataset->GetRasterCount();   // lsu: added 13-apr-13
		width = rasterDataset->GetRasterXSize();
		height = rasterDataset->GetRasterYSize();			
		double adfGeoTransform[6];

		if( rasterDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
		{
			// CDM March 18 for bug #100
			// GetGeoTransform will always return the corner cell whether it's
			// PixelIsArea or PixelIsPoint. So, add a half-cell offset to make 
			// it PixelIsPoint for MapWinGIS compatibility. Saving out won't hurt 
			// anything, since the tiepoints won't change. CreateNew will default
			// to PixelIsArea by applying the reverse of this adjustment.
			const char * pszCellRegistration = rasterDataset->GetMetadataItem("AREA_OR_POINT", NULL);
			
			// The IF statement is no longer needed; see comments above.
			//if (pszCellRegistration == NULL || strcmp(pszCellRegistration, "Area") == 0 || strcmp(pszCellRegistration, "AREA") == 0)
			//{
				// PixelIsArea. Use half-cell offset fix. (Shift Inward)
                dX = adfGeoTransform[1];
				dY  = -1 * adfGeoTransform[5];
				XllCenter = adfGeoTransform[0];
				XllCenter += (dX / 2);
				YllCenter = adfGeoTransform[3] - height*dY;
				YllCenter += (dY / 2);
			//}
			//else // Old code from when this was assuming the corner coordinate.
			//{
			//	// PixelIsPoint
   //             dX = adfGeoTransform[1];
			//	dY  = -1 * adfGeoTransform[5];
			//	XllCenter = adfGeoTransform[0];
			//	YllCenter = adfGeoTransform[3] - height*dY;
			//}
		}
		else
		{	
			//Try to display images without headers			
			XllCenter = 0;
			YllCenter = 0;
			dX = 1;
			dY = -1;
		}

		// the first one is opened by default
		if (!this->OpenBand(1))	
		{
			return false;
		}
		
		this->ReadProjection();		

        inRam = InRam;
		if (inRam)
		{
			LoadFullBuffer();
		}

		// Determine if scanline buffer can be done in case
		// of inram==false, whether passed or forced by LoadFullBuffer
		if (genericType == GDT_Int32 || genericType == GDT_Byte)
		{
			// Chris M 1/28/2007 -- multiply this by two, since we use two buffers
			if (MemoryAvailable(2 * width * sizeof(_int32)))
				CanScanlineBuffer = true;
		}
		else
		{
			if (MemoryAvailable(2 * width * sizeof(float)))
				CanScanlineBuffer = true;
		}
	}
	__except(1)
	{
		// Meh - doesn't particularly matter if the dataset loaded.
	}

	return (rasterDataset != NULL);
}

int tkGridRaster::getNumBands()
{
	return this->nBands;
}

// *****************************************************
//		OpenBand()
// *****************************************************
bool tkGridRaster::OpenBand(int bandIndex)
{
	poBand = rasterDataset->GetRasterBand(bandIndex);   // was 1 by default
	if (!poBand) {
		return false;
	}
	
	dataType = poBand->GetRasterDataType();
	if(!(	(dataType == GDT_Byte)||(dataType == GDT_UInt16)||
			(dataType == GDT_Int16)||(dataType == GDT_UInt32)||
			(dataType == GDT_Int32)||(dataType == GDT_Float32)||
			(dataType == GDT_Float64)||(dataType == GDT_CInt16)||
			(dataType == GDT_CInt32)||(dataType == GDT_CFloat32)||
			(dataType == GDT_CFloat64) ) ) 
	{
		return false;	
	}

	// Assuming one band -- "grid" is only used for one band.
	// 3 band and other colorized rasters are used through tkRaster
	dataType = poBand->GetRasterDataType();
	cIntp = poBand->GetColorInterpretation(); 
	poColorT = poBand->GetColorTable();
	if (poColorT != NULL)
	{
		hasColorTable = true;
	}
	
	//Route the image to the right reader
	//Note: we are assuming here that the data type does not change across bands.
	//However in some complex formats this is not necessarily the case
	switch (dataType)
	{
		case GDT_Float32: 
		case GDT_Float64:
			genericType=GDT_Float32;
			break;
		case GDT_Byte:
			genericType = GDT_Byte;
			break;
        case GDT_Int16:
		case GDT_UInt16:
			genericType = GDT_Int32;
			break;
		default:
			genericType = GDT_Int32;
			break;
	}

	// Update width and height
    long nWidth = poBand->GetXSize();
	long nHeight = poBand->GetYSize();
	if (nWidth != width)
	{
		width = nWidth;
	}
	if (nHeight != height)
	{
		height = nHeight;
	}

	int success = -1;
	double candidateNoDataValue = poBand->GetNoDataValue(&success);
	if (success)
	{
		noDataValue = candidateNoDataValue;
	}
	else
	{
		// Write the assumed nodata value (ArCMapView) so that it doesn't die on GetMin.
		poBand->SetNoDataValue(noDataValue);
	}

	return true;
}

// *****************************************************
//		ReadProjection()
// *****************************************************
void tkGridRaster::ReadProjection()
{
	Projection = "";
	__try
	{
		const char * wkt = (char *)rasterDataset->GetProjectionRef();
		int length = _tcslen(wkt);

		if (wkt != NULL && length > 0)
		{
         char * temp = new char[length+1];
         strcpy(temp, wkt);

			OGRSpatialReferenceH  hSRS;
			hSRS = OSRNewSpatialReference(NULL);

			__try
			{
				if( OSRImportFromESRI( hSRS, &temp ) == CE_None ) 
				{	
					char * pszProj4 = NULL;				
					OSRExportToProj4(hSRS, &pszProj4);
					if (pszProj4 != NULL)	
					{
						Projection = pszProj4;
						CPLFree(pszProj4);
					}
				} 
				else {
					if( OSRImportFromWkt( hSRS, &temp ) == CE_None )
					{
						char * pszProj4 = NULL;
						OSRExportToProj4(hSRS, &pszProj4);
						if (pszProj4 != NULL)	
						{
							Projection = pszProj4;
							CPLFree(pszProj4);
						}
					}
				}
			}
			__except(1)
			{
			}
			OSRDestroySpatialReference( hSRS );
         delete temp;
		}
	}
	__except(1)
	{
	}
}

GridDataType tkGridRaster::GetDataType()
{
	if (genericType == GDT_Int32)
		return LongDataType;
	else if (genericType == GDT_Byte)
		return ByteDataType;
	else
		return FloatDataType;
}

bool tkGridRaster::CreateNew(char * filename, GridFileType newFileType, double dx, double dy, 
			   double xllcenter, double yllcenter, double nodataval, char * projection,
			   long ncols, long nrows, GridDataType DataType, bool CreateInRam, double initialValue, bool applyInitialValue)
{      
	GDALAllRegister();


	if (DataType == ShortDataType || DataType == LongDataType)
		genericType = GDT_Int32;
	if (DataType == FloatDataType || DataType == DoubleDataType || DataType == InvalidDataType || DataType == UnknownDataType)
		genericType = GDT_Float32;
	if (DataType == ByteDataType)
		genericType = GDT_Byte;

	char **papszOptions = NULL;
	char *pszFormat;

	if (newFileType == Ecw)
		pszFormat = T2A("ECW");
	if (newFileType == Bil)
		pszFormat = T2A("EHdr");
	if (newFileType == MrSid)
		return false; // License restrictions forbid this
	if (newFileType == Ascii)
		pszFormat = T2A("AAIGrid");
	if (newFileType == PAux)
		pszFormat = T2A("PAux");
	if (newFileType == PCIDsk)
		pszFormat = T2A("PCIDSK");
	if (newFileType == DTed)
		pszFormat = T2A("DTED");
	if (newFileType == GeoTiff)
		pszFormat = T2A("GTiff");

    GDALDriver *poDriver;

    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

    if( poDriver == NULL )
        return false;

	// Ensure the filename is not null.
	if (filename == NULL)
		return false;

	try
	{
		_unlink(filename);
	}
	catch(...)
	{}

    rasterDataset = poDriver->Create( filename, ncols, nrows, 1, genericType, 
                                papszOptions );

	currentFileType = newFileType;

	width = ncols;
	height = nrows;
	XllCenter = xllcenter;
	YllCenter = yllcenter;
	dX = dx;
	dY = dy;
	Projection = projection;

	// Adjust by one half-cell offset to make this PixelIsArea.
	// (Shift Outward)
	// Don't bother trying to write this tag out -- GDAL doesn't
	// seem to write that tag no matter how we try. It can read it
	// fine, and it defaults to PixelIsArea if missing... so no problem.

	double adfGeoTransform[6];
	adfGeoTransform[0] = XllCenter - (dX / 2);
	adfGeoTransform[1] = dX;
	adfGeoTransform[2] = 0;
	adfGeoTransform[3] = (height * dy) + YllCenter + ((dY * -1) / 2);
	adfGeoTransform[4] = 0;
	adfGeoTransform[5] = dY * -1;

    rasterDataset->SetGeoTransform( adfGeoTransform );
	    
	if (rasterDataset == NULL)
		return false;

    poBand = rasterDataset->GetRasterBand(1);

	if (hasColorTable)
	{
		poBand->SetColorTable(poColorT);
	}

	poBand->SetNoDataValue(nodataval);
	noDataValue = nodataval;
    
	mFilename = filename;
	inRam = CreateInRam;

	if (inRam)
	{
		// Make a buffer
		if (genericType == GDT_Int32 || genericType == GDT_Byte)
		{
			_int32buffer = (_int32 *) CPLMalloc( sizeof(_int32)*width*height );
			if (!_int32buffer)
			{
				inRam = false; // Force the user to use disk-based;
				// not enough memory likely to load into memory.
			}		
		}
		else
		{
			floatbuffer = (float *) CPLMalloc( sizeof(float)*width*height );
			if (!floatbuffer)
			{
				inRam = false; // Force the user to use disk-based;
				// not enough memory likely to load into memory.
			}
		}
	}

	// Write the initial value
	if (applyInitialValue)
		clear(initialValue);

	if (strcmp(projection, "") != 0 && projection != NULL)
	{
		// SetProjection expects WKT
		if (startsWith(projection, "+proj"))
		{
			char * wkt = NULL;

			ProjectionTools * p = new ProjectionTools();
			p->ToESRIWKTFromProj4(&wkt, projection);

			if (wkt != NULL && strcmp(wkt, "") != 0)
			{
				rasterDataset->SetProjection(wkt);
			}

			delete p; //added by Lailin Chen 12/30/2005, don't delete wkt
		}
		else
		{
			// Reasonably good indicator it's WKT
			if (contains(projection, '[') && contains(projection, ']'))
			{
				rasterDataset->SetProjection( projection );
			}
		}
	}

	return true;
}

bool tkGridRaster::startsWith(const char* compare, const char* starts) const
{
	bool cc = false;
	if(!starts && !compare)
	{
		return true;
	}
	if(!*starts)
	{
		return (::_tcslen(compare) == 0);
	}

	size_t arglen = ::_tcslen(starts);
	if(arglen <= ::_tcslen(compare))
	{
		cc = (_strnicmp(compare, starts, arglen) == 0);
	}
	return cc;
}

bool tkGridRaster::CanCreate(GridFileType newFileType)
{
	GDALAllRegister();

	char *pszFormat;

	if (newFileType == Ecw)
		pszFormat = T2A("ECW");
	if (newFileType == Bil)
		pszFormat = T2A("EHdr");
	if (newFileType == MrSid)
		return false; // License restrictions forbid this
	if (newFileType == Ascii)
		pszFormat = T2A("AAIGrid");
	if (newFileType == PAux)
		pszFormat = T2A("PAux");
	if (newFileType == PCIDsk)
		pszFormat = T2A("PCIDSK");
	if (newFileType == DTed)
		pszFormat = T2A("DTED");
	if (newFileType == GeoTiff)
		pszFormat = T2A("GTiff");

    GDALDriver *poDriver;
    char **papszMetadata;

    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	// Don't delete poDriver - metadata will be reference counted -- don't free it.
	// reference counted.

    if( poDriver == NULL )
        return false;

    papszMetadata = poDriver->GetMetadata();
    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
		return true;
	else
		return false;
}

bool tkGridRaster::CanCreate()
{
	static bool haveDetermined = false;
	static bool priorDecision = false;

	if (haveDetermined) return priorDecision;

	bool retval;

	GDALAllRegister();

	GDALDriver *poDriver;
    char **papszMetadata;

	poDriver = rasterDataset->GetDriver();
	// Don't delete poDriver - metadata will be reference counted -- don't free it.

    if( poDriver == NULL )
        return false; // No driver == no dataset == no write.

    papszMetadata = poDriver->GetMetadata();
	haveDetermined = true;

    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
	{
		priorDecision = true;
        return true;
	}
	else
	{
		priorDecision = false;
		return false;
	}

	return retval;
}

bool tkGridRaster::Close()
{
	try
	{
		if (rasterDataset != NULL)
		{
			delete rasterDataset;
			rasterDataset=NULL;
		}
		
		if( _int32buffer != NULL )
		{
			CPLFree( _int32buffer );
			_int32buffer = NULL;
		}
		
		if( floatbuffer != NULL )
		{
			CPLFree( floatbuffer );
			floatbuffer = NULL;
		}

		if( _int32ScanlineBufferA != NULL )
		{
			CPLFree( _int32ScanlineBufferA );
			_int32ScanlineBufferA = NULL;
		}
		
		if( floatScanlineBufferA != NULL )
		{
			CPLFree( floatScanlineBufferA );
			floatScanlineBufferA = NULL;
		}
		
		if( _int32ScanlineBufferB != NULL )
		{
			CPLFree( _int32ScanlineBufferB );
			_int32ScanlineBufferB = NULL;
		}
		
		if( floatScanlineBufferB != NULL )
		{
			CPLFree( floatScanlineBufferB );
			floatScanlineBufferB = NULL;
		}
	}
	catch(...)
	{
		return false;
	}

	return true;
}

double tkGridRaster::GetMaximum()
{
	if (cachedMax != -9999) return cachedMax;

	int bGotMin, bGotMax;
	double adfMinMax[2];
	adfMinMax[0] = poBand->GetMinimum( &bGotMin );
    adfMinMax[1] = poBand->GetMaximum( &bGotMax );
    if( ! (bGotMin && bGotMax) )
       GDALComputeRasterMinMax((GDALRasterBandH)poBand, false, adfMinMax);

	cachedMax = adfMinMax[1];
	cachedMin = adfMinMax[0];

	return adfMinMax[1];
}

double tkGridRaster::GetMinimum()
{
	if (cachedMin != 9999) return cachedMin;

	int bGotMin, bGotMax;
	double adfMinMax[2];
	adfMinMax[0] = poBand->GetMinimum( &bGotMin );
    adfMinMax[1] = poBand->GetMaximum( &bGotMax );
    if( ! (bGotMin && bGotMax) )
       GDALComputeRasterMinMax((GDALRasterBandH)poBand, false, adfMinMax);

	cachedMax = adfMinMax[1];
	cachedMin = adfMinMax[0];

	return adfMinMax[0];
}

void tkGridRaster::LoadFullBuffer()
{
	try
	{
	if (genericType == GDT_Int32 || genericType == GDT_Byte)
	{
		// Is there enough memory available? 
		// CDM 1/6/2007: On one really extreme case, the amount
		// of memory required would have caused the size of the largest
		// datatype to overflow repeatedly. So check in a stepwise
		// fashion "up to" the size we really want
        if (!MemoryAvailable(width*height))
		{
			inRam = false;
			return;
		}
		if (!MemoryAvailable(width*height*2))
		{
			inRam = false;
			return;
		}
		if (!MemoryAvailable(width*height*sizeof(_int32)*2))
		{
			inRam = false;
			return;
		}

		_int32buffer = (_int32 *) CPLMalloc( sizeof(_int32)*width*height );
		if (!_int32buffer)
		{
			inRam = false; // Force the user to use disk-based;
			// not enough memory likely to load into memory.
			return;
		}

		poBand->AdviseRead ( 0, 0, width, height, width, height, GDT_Int32, NULL);
		poBand->RasterIO( GF_Read, 0, 0, width, height,
							_int32buffer, width, height, GDT_Int32,0, 0 );
		
	}
	else
	{
		// Is there enough memory available?
		// CDM 1/6/2007: On one really extreme case, the amount
		// of memory required would have caused the size of the largest
		// datatype to overflow repeatedly. So check in a stepwise
		// fashion "up to" the size we really want
        if (!MemoryAvailable(width*height))
		{
			inRam = false;
			return;
		}
		if (!MemoryAvailable(width*height*2))
		{
			inRam = false;
			return;
		}
		if (!MemoryAvailable(width*height*sizeof(float)*2))
		{
			inRam = false;
			return;
		}

		floatbuffer = (float *) CPLMalloc( sizeof(float)*width*height );
		if (!floatbuffer)
		{
			inRam = false; // Force the user to use disk-based;
			// not enough memory likely to load into memory.
			return;
		}

		poBand->AdviseRead ( 0, 0, width, height, width, height, GDT_Float32, NULL);
		poBand->RasterIO( GF_Read, 0, 0, width, height,
						floatbuffer, width, height, GDT_Float32,0, 0 );
	}
	}
	catch(...)
	{
		inRam = false;
	}
}

bool tkGridRaster::GetFloatWindow(float *Vals, long StartRow, long EndRow, long StartCol, long EndCol)
{
	if (poBand == NULL) return false;

	// Load from buffer if it exists, otherwise use RasterIO.
	// Since there are lots of potential buffers, just call getValue
	if (_int32buffer != NULL || floatbuffer != NULL || _int32ScanlineBufferB != NULL || floatScanlineBufferB != NULL || _int32ScanlineBufferA != NULL || floatScanlineBufferA != NULL)
	{
		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				// Just use getvalue -- it will be memory-based,
				// and will account for the correct buffer.
				Vals[position++] = static_cast<float>(getValue(j, i));
			}
		}
	}
	else
	{
		// Use Rasterio Directly from disk -- faster for large grids,
		// which is likely when this function will be called anyway.
		try
		{
			poBand->AdviseRead ( StartCol, StartRow, (EndCol - StartCol)+1, (EndRow - StartRow)+1, (EndCol - StartCol)+1, (EndRow - StartRow)+1, GDT_Float32, NULL);
			poBand->RasterIO( GF_Read, StartCol, StartRow, (EndCol - StartCol)+1, (EndRow - StartRow)+1,
								Vals, (EndCol - StartCol)+1, (EndRow - StartRow)+1, GDT_Float32, 0, 0 );
		}
		catch(...)
		{
			return false;
		}
	}

	return true;
}


bool tkGridRaster::PutFloatWindow(float *Vals, long StartRow, long EndRow, long StartCol, long EndCol)
{
	if (poBand == NULL) return false;

	// Reset our cached max/min values to our "unset" flags
	cachedMin = 9999;
	cachedMax = -9999;

	// Save to buffer if it exists, otherwise use RasterIO.
	if (_int32buffer != NULL || floatbuffer != NULL || _int32ScanlineBufferB != NULL || floatScanlineBufferB != NULL || _int32ScanlineBufferA != NULL || floatScanlineBufferA != NULL)
	{
		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				// Just use putvalue -- it will be memory-based,
				// and will account for the correct buffer.
				putValue(j, i, static_cast<double>(Vals[position++]));
			}
		}
	}
	else
	{
		// Use Rasterio Directly to disk -- faster for large grids,
		// which is likely when this function will be called anyway.
		try
		{
			poBand->RasterIO( GF_Write, StartCol, StartRow, (EndCol - StartCol)+1, (EndRow - StartRow)+1,
								Vals, (EndCol - StartCol)+1, (EndRow - StartRow)+1, GDT_Float32, 0, 0 );
		}
		catch(...)
		{
			return false;
		}
	}

	return true;
}

void tkGridRaster::clear(double value)
{
	if (inRam && _int32buffer != NULL)
	{
		for (int i = 0; i < width * height; i++)
			_int32buffer[i] = static_cast<_int32>(value);
	}
	else if (inRam && floatbuffer != NULL)
	{
		for (int i = 0; i < width * height; i++)
			floatbuffer[i] = static_cast<float>(value);
	}
	else if (!inRam)
	{
		if (poBand != NULL)
			poBand->Fill(value);
	}
}

bool tkGridRaster::SaveFullBuffer()
{
	// First, check to see if scanline buffering has been used. This will likely be the most common.
	// If not, drop down to "buffer save"
	bool retVal = false;

	// Chris Michaelis 1/28/2007
	// Do not use ELSE's here -- otherwise only every other line will be saved
	
	if (genericType != GDT_Int32 && scanlineADataChanged && floatScanlineBufferA != NULL)
	{
		poBand->RasterIO( GF_Write, 0, scanlineBufferNumberA, width, 1,
									floatScanlineBufferA, width, 1, GDT_Float32,0, 0 );
		retVal = true;
		scanlineADataChanged = false;
	}
	
	if ((genericType == GDT_Int32 || genericType == GDT_Byte) && scanlineADataChanged && _int32ScanlineBufferA != NULL)
	{
		poBand->RasterIO( GF_Write, 0, scanlineBufferNumberA, width, 1,
									_int32ScanlineBufferA, width, 1, GDT_Int32,0, 0 );
		retVal = true;
		scanlineADataChanged = false;
	}
	
	if (genericType != GDT_Int32 && scanlineBDataChanged && floatScanlineBufferB != NULL)
	{
		poBand->RasterIO( GF_Write, 0, scanlineBufferNumberB, width, 1,
									floatScanlineBufferB, width, 1, GDT_Float32,0, 0 );
		retVal = true;
		scanlineBDataChanged = false;
	}
	
	if ((genericType == GDT_Int32 || genericType == GDT_Byte) && scanlineBDataChanged && _int32ScanlineBufferB != NULL)
	{
		poBand->RasterIO( GF_Write, 0, scanlineBufferNumberB, width, 1,
									_int32ScanlineBufferB, width, 1, GDT_Int32,0, 0 );
		retVal = true;
		scanlineBDataChanged = false;
	}

	// Buffer save -- if not scanlining.
	if ((genericType == GDT_Int32 || genericType == GDT_Byte) && _int32buffer != NULL)
	{
		poBand->RasterIO( GF_Write, 0, 0, width, height,
							_int32buffer, width, height, GDT_Int32,0, 0 );
		retVal = true;		
	}
	
	if (genericType == GDT_Float32 && floatbuffer != NULL)
	{
		poBand->RasterIO( GF_Write, 0, 0, width, height,
						floatbuffer, width, height, GDT_Float32,0, 0 );
		retVal = true;
	}

	////08-Aug-09 Rob Cairns if using clip grid with polygon then genericType is not set here
	////if (genericType != GDT_Float32 && genericType != GDT_Int32)
	//if (genericType == GDT_Unknown)
	//{
	//	if (_int32buffer != NULL)
	//	{
	//		poBand->RasterIO( GF_Write, 0, 0, width, height,
	//							_int32buffer, width, height, GDT_Int32,0, 0 );
	//		retVal = true;		
	//	}
	//	if (floatbuffer != NULL)
	//	{
	//		poBand->RasterIO( GF_Write, 0, 0, width, height,
	//						floatbuffer, width, height, GDT_Float32,0, 0 );
	//		retVal = true;
	//	}
	//}

	retVal = true;// not in ram

	rasterDataset->FlushCache();

	return retVal;
}

bool tkGridRaster::Save(char * saveToFilename, GridFileType newFileFormat)
{

	// Write the .prj file
	if (Projection.GetLength() > 0)
	{
		CString prjFilename = saveToFilename;
		prjFilename = prjFilename.Left(prjFilename.GetLength() - 3) + "prj";
		FILE * prjFile = NULL;
		prjFile = fopen(prjFilename, "wb");
		if (prjFile)
		{
			char * wkt;

			ProjectionTools * p = new ProjectionTools();
			p->ToESRIWKTFromProj4(&wkt, Projection.GetBuffer());

			fprintf(prjFile, "%s", wkt);
			fclose(prjFile);
			prjFile = NULL;
			delete p; //added by Lailin Chen 12/30/2005
		}

	}

	if (mFilename == saveToFilename && newFileFormat == currentFileType)
	{
		return SaveFullBuffer();
	}
	else if (newFileFormat == currentFileType)
	{
		// Before saving to a different file type,
		// save out any scanline memory buffers to the current file
		if (scanlineADataChanged || scanlineBDataChanged)
			SaveFullBuffer();
		// Note that if it's inram fully, the above isn't necessary;
		// it can be written directly from our memory buffer by GDAL below.

		GDALDataset *poDstDS;

		poDstDS = rasterDataset->GetDriver()->CreateCopy( saveToFilename, rasterDataset, FALSE, NULL, NULL, NULL );

		if (poDstDS == NULL)
			return false;

		// If in ram, save any changes also.
		if (inRam)
		{
			if ((genericType == GDT_Int32 || genericType == GDT_Byte) && _int32buffer != NULL)
			{
				rasterDataset->GetRasterBand(1)->RasterIO( GF_Write, 0, 0, width, height,
								_int32buffer, width, height, GDT_Int32, 0, 0 );
			}
			else if (genericType ==  GDT_Float32 && floatbuffer != NULL)
			{
				rasterDataset->GetRasterBand(1)->RasterIO( GF_Write, 0, 0, width, height,
							floatbuffer, width, height, GDT_Float32,0, 0 );
			}
		}

		poDstDS->FlushCache();

		// Clean up the destination dataset
		if( poDstDS != NULL )
			delete poDstDS;

		return true;
	}
	else // Different file type and/or different filename
	{
		char **papszOptions = NULL;
		char *pszFormat;

		if (newFileFormat == Ecw)
			pszFormat = T2A("ECW");
		if (newFileFormat == Bil)
			pszFormat = T2A("EHdr");
		if (newFileFormat == MrSid)
			return false; // License restrictions forbid this
		if (newFileFormat == Ascii)
			pszFormat = T2A("Ascii"); //"AAIGrid");
		if (newFileFormat == PAux)
			pszFormat = T2A("PAux");
		if (newFileFormat == PCIDsk)
			pszFormat = T2A("PCIDSK");
		if (newFileFormat == DTed)
			pszFormat = T2A("DTED");
		if (newFileFormat == GeoTiff)
			pszFormat = T2A("GTiff");

		GDALDriver *poDriver;

		poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

		if( poDriver == NULL )
			return false;

		// Create a new file and dump to it
		tkGridRaster * outFile = new tkGridRaster();
		outFile->CreateNew(saveToFilename, newFileFormat, dX, dY, XllCenter, YllCenter, noDataValue, Projection.GetBuffer(), width, height, GetDataType(), false, noDataValue, false);
		for (int j = 0; j < height; j++)
			for (int i = 0; i < width; i++)
				outFile->putValue(j, i, getValue(j, i));
		outFile->Save(saveToFilename, newFileFormat);
		outFile->Close();
		outFile = NULL;

		return true;
	}
}

double tkGridRaster::getValue( long Row, long Column )
{
	if (inRam == true && _int32buffer != NULL)
			return static_cast<double>(_int32buffer[Column + Row * width]);
	else if (inRam == true && floatbuffer != NULL)
			return static_cast<double>(floatbuffer[Column + Row * width]);
	else
	{
		// Read from disk - buffering one line at a time if possible
		if (Row == scanlineBufferNumberA)
		{
			// Return from one-row buffer	

			scanlineLastAccessed = 'A';
			if ((genericType == GDT_Int32 || genericType == GDT_Byte) && _int32ScanlineBufferA != NULL)
				return static_cast<double>(_int32ScanlineBufferA[Column]);
			else if (floatScanlineBufferA != NULL)
				return static_cast<double>(floatScanlineBufferA[Column]);
		}
		else if (Row == scanlineBufferNumberB)
		{
			// Return from one-row buffer	

			scanlineLastAccessed = 'B';
			if ((genericType == GDT_Int32 || genericType == GDT_Byte) && _int32ScanlineBufferB != NULL)
				return static_cast<double>(_int32ScanlineBufferB[Column]);
			else if (floatScanlineBufferB != NULL)
				return static_cast<double>(floatScanlineBufferB[Column]);
		}
		
		// If we're still in the function, need to load a buffer
		// or get a single value
		
		if (CanScanlineBuffer)
		{
			if (scanlineLastAccessed == 'A')
			{
				// Load B

				if (genericType == GDT_Int32 || genericType == GDT_Byte)
				{
					// If put_Value was called and wrote into the memory buffer,
					// save that before dumping the old buffer data.
					if (scanlineBDataChanged)
						SaveFullBuffer();

					// Chris M 1/28/2007 -- Avoid releasing and reallocating
					// memory on every buffer shift. Rather, ensure that
					// the datatypes are checked so I know which buffer to
					// look at (in getvalue and putvalue and savefullbuffer)
					// if (_int32ScanlineBufferB != NULL)
					// {
					// 	CPLFree(_int32ScanlineBufferB);
					// 	_int32ScanlineBufferB = NULL;
					// }
					// if (floatScanlineBufferB != NULL)
					// {
					// 	CPLFree( floatScanlineBufferB);
					// 	floatScanlineBufferB = NULL;
					// }

					if (_int32ScanlineBufferB == NULL)
						_int32ScanlineBufferB = (_int32*) CPLMalloc( sizeof(_int32)*width);

					// If we get here and the buffer is still null, apparently
					// there's not really enough memory to do this...!
					if (_int32ScanlineBufferB == NULL)
					{
						// Clear and prevent buffering
						scanlineBufferNumberA = -1;
						scanlineBufferNumberB = -1;
						CanScanlineBuffer = false;

						// Return the requested value with a recursive call
						return getValue(Row, Column);
					}

					poBand->RasterIO( GF_Read, 0, Row, width, 1,
								_int32ScanlineBufferB, width, 1, GDT_Int32,0, 0 );
					scanlineBufferNumberB = Row;
					scanlineLastAccessed = 'B';
					scanlineBDataChanged = false;
					return static_cast<double>(_int32ScanlineBufferB[Column]);
				}
				else
				{
					// If put_Value was called and wrote into the memory buffer,
					// save that before dumping the old buffer data.
					if (scanlineBDataChanged)
						SaveFullBuffer();

					// see comment above
					//if (_int32ScanlineBufferB != NULL)
					//{
					//	CPLFree(_int32ScanlineBufferB);
					//	_int32ScanlineBufferB = NULL;
					//}
					//if (floatScanlineBufferB != NULL)
					//{
					//	CPLFree( floatScanlineBufferB);
					//	floatScanlineBufferB = NULL;
					//}
	
					if (floatScanlineBufferB == NULL)
						floatScanlineBufferB = (float *) CPLMalloc( sizeof(float)*width);

					// If we get here and the buffer is still null, apparently
					// there's not really enough memory to do this...!
					if (floatScanlineBufferB == NULL)
					{
						// Clear and prevent buffering
						scanlineBufferNumberA = -1;
						scanlineBufferNumberB = -1;
						CanScanlineBuffer = false;

						// Return the requested value with a recursive call
						return getValue(Row, Column);
					}

					poBand->RasterIO( GF_Read, 0, Row, width, 1,
									floatScanlineBufferB, width, 1, GDT_Float32,0, 0 );
					scanlineBufferNumberB = Row;
					scanlineLastAccessed = 'B';
					scanlineBDataChanged = false;
					return static_cast<double>(floatScanlineBufferB[Column]);
				}
			}
			else
			{
				// Load A

				if (genericType == GDT_Int32 || genericType == GDT_Byte)
				{
					// If put_Value was called and wrote into the memory buffer,
					// save that before dumping the old buffer data.
					if (scanlineADataChanged)
						SaveFullBuffer();

					// See comment above
					//if (_int32ScanlineBufferA != NULL)
					//{
					//	CPLFree(_int32ScanlineBufferA);
					//	_int32ScanlineBufferA = NULL;
					//}
					//if (floatScanlineBufferA != NULL)
					//{
					//	CPLFree( floatScanlineBufferA);
					//	floatScanlineBufferA = NULL;
					//}

					if (_int32ScanlineBufferA == NULL)
						_int32ScanlineBufferA = (_int32*) CPLMalloc( sizeof(_int32)*width);

					// If we get here and the buffer is still null, apparently
					// there's not really enough memory to do this...!
					if (_int32ScanlineBufferA == NULL)
					{
						// Clear and prevent buffering
						scanlineBufferNumberA = -1;
						scanlineBufferNumberB = -1;
						CanScanlineBuffer = false;

						// Return the requested value with a recursive call
						return getValue(Row, Column);
					}

					poBand->RasterIO( GF_Read, 0, Row, width, 1,
								_int32ScanlineBufferA, width, 1, GDT_Int32,0, 0 );
					scanlineBufferNumberA = Row;
					scanlineLastAccessed = 'A';
					scanlineADataChanged = false;
					return static_cast<double>(_int32ScanlineBufferA[Column]);
				}
				else
				{
					// If put_Value was called and wrote into the memory buffer,
					// save that before dumping the old buffer data.
					if (scanlineADataChanged)
						SaveFullBuffer();

					// see comment above
					//if (_int32ScanlineBufferA != NULL)
					//{
					//	CPLFree(_int32ScanlineBufferA);
					//	_int32ScanlineBufferA = NULL;
					//}
					//if (floatScanlineBufferA != NULL)
					//{
					//	CPLFree( floatScanlineBufferA);
					//	floatScanlineBufferA = NULL;
					//}
	
					if (floatScanlineBufferA == NULL)
						floatScanlineBufferA = (float *) CPLMalloc( sizeof(float)*width);

					// If we get here and the buffer is still null, apparently
					// there's not really enough memory to do this...!
					if (floatScanlineBufferA == NULL)
					{
						// Clear and prevent buffering
						scanlineBufferNumberA = -1;
						scanlineBufferNumberB = -1;
						CanScanlineBuffer = false;

						// Return the requested value with a recursive call
						return getValue(Row, Column);
					}

					poBand->RasterIO( GF_Read, 0, Row, width, 1,
									floatScanlineBufferA, width, 1, GDT_Float32,0, 0 );
					scanlineBufferNumberA = Row;
					scanlineLastAccessed = 'A';
					scanlineADataChanged = false;
					return static_cast<double>(floatScanlineBufferA[Column]);
				}

			}
		}
		else
		{
			// Read one value at a time. Performance hit...

			_int32 pafScanAreaInt;
			float pafScanAreaFloat;
				
			if (genericType == GDT_Int32 || genericType == GDT_Byte)
			{
				//poBand->AdviseRead ( Column, Row, 1, 1, 1, 1, GDT_Int32, NULL);
				poBand->RasterIO( GF_Read, Column, Row, 1, 1,
									&pafScanAreaInt, 1, 1, GDT_Int32,0, 0 );
			}
			else
			{
				//poBand->AdviseRead ( Column, Row, 1, 1, 1, 1, GDT_Float32, NULL);
				poBand->RasterIO( GF_Read, Column, Row, 1, 1,
								&pafScanAreaFloat, 1, 1, GDT_Float32,0, 0 );
			}

			if (genericType == GDT_Int32 || genericType == GDT_Byte)
				return static_cast<double>(pafScanAreaInt);
			else
				return static_cast<double>(pafScanAreaFloat);
		}
	}
}

void tkGridRaster::putValue( long Row, long Column, double Value )
{
	// Reset our cached max/min values to our "unset" flags
	cachedMin = 9999;
	cachedMax = -9999;

	if (inRam == true && _int32buffer != NULL)
			_int32buffer[Column + Row * width] = static_cast<_int32>(Value);
	else if (inRam == true && floatbuffer != NULL)
			floatbuffer[Column + Row * width] = static_cast<float>(Value);

	// If there is a loaded scanline buffer for this row, use it and set the modified flag.
	else if ((genericType == GDT_Int32 || genericType == GDT_Byte) && scanlineBufferNumberB == Row && _int32ScanlineBufferB != NULL)
	{
		_int32ScanlineBufferB[Column] = static_cast<_int32>(Value);
		scanlineBDataChanged = true;
	}
	else if (genericType != GDT_Int32 && scanlineBufferNumberB == Row && floatScanlineBufferB != NULL)
	{
		floatScanlineBufferB[Column] = static_cast<float>(Value);
		scanlineBDataChanged = true;
	}
	else if ((genericType == GDT_Int32 || genericType == GDT_Byte) && scanlineBufferNumberA == Row && _int32ScanlineBufferA != NULL)
	{
		_int32ScanlineBufferA[Column] = static_cast<_int32>(Value);
		scanlineADataChanged = true;
	}
	else if (genericType != GDT_Int32 && scanlineBufferNumberA == Row && floatScanlineBufferA != NULL)
	{
		floatScanlineBufferA[Column] = static_cast<float>(Value);
		scanlineADataChanged = true;
	}
	else if (CanScanlineBuffer)
	{
		// We can use scanline buffering but the row we're writing to isn't loaded.
		// Request a value from this row to force it to load into one of the buffers.
		// If existing data was changed, it will be saved by getValue before the buffer is loaded.
		getValue(Row, Column);  // Return value is junked (not assigned to anything)

		// It is loaded now -- the else blocks above can now catch it and write it. So, call myself.
		putValue(Row, Column, Value);
	}
	else
	{
		// Write directly to disk. Big performance hit.
			
		if (genericType == GDT_Int32 || genericType == GDT_Byte)
		{
			_int32 pafScanAreaInt = static_cast<_int32>(Value);
			poBand->RasterIO( GF_Write, Column, Row, 1, 1,
								&pafScanAreaInt, 1, 1, GDT_Int32,0, 0 );
		}
		else
		{
			float pafScanAreaFloat = static_cast<float>(Value);
			poBand->RasterIO( GF_Write, Column, Row, 1, 1,
							&pafScanAreaFloat, 1, 1, GDT_Float32,0, 0 );
		}
	}
}

bool tkGridRaster::SaveToBGD(CString filename, void(*callback)(int number, const char * message))
{
	try
	{
		FILE * out = fopen( filename, "w+b" );
		
		if( !out )
			return false;
		else
		{	
			// Write header
			fwrite( &width, sizeof(int),1,out);
			fwrite( &height, sizeof(int),1,out);
			fwrite( &dX, sizeof(double),1,out);
			fwrite( &dY, sizeof(double),1,out);
			fwrite( &XllCenter, sizeof(double),1,out);
			fwrite( &YllCenter, sizeof(double),1,out);

			DATA_TYPE type = FLOAT_TYPE;
			if (genericType == GDT_Int32 || genericType == GDT_Byte)
				type = LONG_TYPE;

			fwrite( &type, sizeof(DATA_TYPE),1,out);

			if (genericType == GDT_Int32 || genericType == GDT_Byte)
			{
				long nodata = static_cast<long>(noDataValue);
				fwrite( &nodata, sizeof(long),1,out);
			}
			else
			{
				double nodata = noDataValue;
				fwrite( &nodata, sizeof(double),1,out);
			}
			
			char * projection = new char[MAX_STRING_LENGTH + 1];
			if (Projection.GetLength() <= 255)
			{
				strcpy( projection, Projection.GetBuffer());
			}

			if( _tcslen( projection ) > 0 )
				fwrite( projection, sizeof(char), _tcslen(projection),out);
			
			if( _tcslen( projection ) < MAX_STRING_LENGTH )
			{	
				int size_of_pad = MAX_STRING_LENGTH - _tcslen(projection);
				char * pad = new char[size_of_pad];
				for( int p = 0; p < size_of_pad; p++ )
					pad[p] = 0;
				fwrite(pad, sizeof(char), size_of_pad, out );
			}

			CString prjFilename = filename.Left(filename.GetLength() - 3) + "prj";
			FILE * prjFile = NULL;
			prjFile = fopen(prjFilename, "wb");
			if (prjFile)
			{
				try
				{ 
					char * wkt;

					ProjectionTools * p = new ProjectionTools();
					p->ToESRIWKTFromProj4(&wkt, projection);

					fprintf(prjFile, "%s", wkt);
					fclose(prjFile);
					prjFile = NULL;
					delete p; //added by Lailin Chen 12/30/2005
					}
				catch(...)
				{
					// Not critical - the .prj file is supplemental, as the projection
					// is stored in the grid header also.
				}
			}

			// We have no notes. Write emptiness.
			int size_of_pad = MAX_STRING_LENGTH;
			char * pad = new char[size_of_pad];
			for( int p = 0; p < size_of_pad; p++ )
				pad[p] = 0;
			fwrite(pad, sizeof(char), size_of_pad, out );
			// Done with header

			double total = height * width;
			int percent = 0;
			long num_written = 0;

			// Duplicate the entire loop for this decision, rather than placing this
			// where it makes sense inside the loop. This means the "if" isn't evaluated
			// width*height times.
			if (genericType == GDT_Int32 || genericType == GDT_Byte)
			{
				long lValue;
				for( int j = 0; j < height; j++ )
				{	for( int i = 0; i < width; i++ )
					{	num_written++;
						lValue = static_cast<long>(getValue( j, i ));
						fwrite( &lValue,sizeof(long),1,out);

						if( callback != NULL )
						{
							int newpercent = (int)((num_written/total)*100);
							if( newpercent > percent )
							{	percent = newpercent;
								callback( percent, "Binary Grid Write");
							}											
						}
					}				
				}
			}
			else
			{
				float fValue;
				for( int j = 0; j < height; j++ )
				{	for( int i = 0; i < width; i++ )
					{	num_written++;
						fValue = static_cast<float>(getValue( j, i ));
						fwrite( &fValue,sizeof(float),1,out);

						if( callback != NULL )
						{
							int newpercent = (int)((num_written/total)*100);
							if( newpercent > percent )
							{	percent = newpercent;
								callback( percent, "Binary Grid Write");
							}											
						}
					}				
				}
			}
			fflush(out);
			fclose( out );	
			out = NULL;
			return true;
	}
	return true;
	}
	catch(...)
	{
		return false;
	}
}

bool tkGridRaster::ReadFromBGD(CString filename, void (*callback)(int number, const char * message))
{	
	// Reset our cached max/min values to our "unset" flags
	cachedMin = 9999;
	cachedMax = -9999;

		FILE * in = fopen( filename, "rb" );

		if( !in )
		{
			return false;
		}
		else
		{	
			// Reread the header. This will have been read by whoever called ReadFromBGD in order
			// to create the output file prior to reading in the BGD into that file.
			// Reread it to position the file only.
			DATA_TYPE bgdDataType;
			ReadBGDHeader(filename, in, bgdDataType );

			double total = height * width;

			int percent = 0;
			long num_read = 0;

			float fData;
			long lData;
			short sData;
			double dData;
			
			for( int j = 0; j < height; j++ )
			{	for( int i = 0; i < width; i++ )
				{	num_read++;
					if( feof(in) )
					{
						return false; // premature end of data
					}

					if (bgdDataType == LONG_TYPE)
					{
						fread( &lData,sizeof(long),1,in );
						putValue(j, i, static_cast<double>(lData));
					}
					else if (bgdDataType == SHORT_TYPE)
					{
						fread( &sData,sizeof(short),1,in );
						putValue(j, i, static_cast<double>(sData));
					}
					else if (bgdDataType == FLOAT_TYPE)
					{
						fread( &fData,sizeof(float),1,in );
						putValue(j, i, static_cast<double>(fData));
					}
					else if (bgdDataType == DOUBLE_TYPE)
					{
						fread( &dData,sizeof(double),1,in );
						putValue(j, i, dData);
					}
													
					if( callback != NULL )
					{
						int newpercent = (int)(((num_read)/total)*100);
						if( newpercent > percent )
						{	percent = newpercent;
							callback( percent, "Reading Binary Grid" );
						}
					}
				}
			}	
			
			fclose(in);
			return true;			
		}
}

bool tkGridRaster::ReadBGDHeader(CString filename, DATA_TYPE &bgdDataType)
{
	FILE * in = fopen( filename, "rb" );
	if (!in) return false;
	ReadBGDHeader(filename, in, bgdDataType);
	fclose(in);
	return true;
}

void tkGridRaster::ReadBGDHeader( CString filename, FILE * in, DATA_TYPE &bgdDataType )
{	
		rewind(in);
		long ncols;
		fread( &ncols, sizeof(int),1,in);
		width = ncols;

		long nrows;
		fread( &nrows, sizeof(int),1,in);
		height = nrows;

		fread( &dX, sizeof(double),1,in);

		fread( &dY, sizeof(double),1,in);

		fread( &XllCenter, sizeof(double),1,in);

		fread( &YllCenter, sizeof(double),1,in);

		fread( &bgdDataType, sizeof(DATA_TYPE),1,in);
		
		if (bgdDataType == LONG_TYPE)
		{
			long nodata_value;
			fread( &nodata_value, sizeof(long),1,in);		
			noDataValue = static_cast<double>(nodata_value);
		}
		else if (bgdDataType == SHORT_TYPE)
		{
			short nodata_value;
			fread( &nodata_value, sizeof(short),1,in);		
			noDataValue = static_cast<double>(nodata_value);
		}
		else if (bgdDataType == FLOAT_TYPE)
		{
			float nodata_value;
			fread( &nodata_value, sizeof(float),1,in);		
			noDataValue = static_cast<double>(nodata_value);
		}
		else if (bgdDataType == DOUBLE_TYPE)
		{
			double nodata_value;
			fread( &nodata_value, sizeof(double),1,in);		
			noDataValue = nodata_value;	
		}

		char * projection = new char[MAX_STRING_LENGTH + 1];
		fread( projection, sizeof(char), MAX_STRING_LENGTH,in);

		// a .prj file will override what's in the header
		try
		{
			char * newProj = NULL;
			ProjectionTools * p = new ProjectionTools();
			CString prjFilename = filename.Left(filename.GetLength() - 3) + "prj";
			p->GetProj4FromPRJFile(prjFilename.GetBuffer(), &newProj);
			delete p;

			if (newProj != NULL && _tcslen(newProj) > 0)
			{
				strncpy(projection, newProj, MAX_STRING_LENGTH);
				CPLFree(newProj);
			}
 
			if (strcmp(projection, "") != 0 && projection != NULL)
			{
					try
					{
						if (strcmp(projection, "") != 0 && projection != NULL)
						{
							// SetProjection expects WKT
							if (startsWith(projection, "+proj"))
							{
								char * wkt = NULL;

								ProjectionTools * p = new ProjectionTools();
								p->ToESRIWKTFromProj4(&wkt, projection);

								if (wkt != NULL && strcmp(wkt, "") != 0)
								{
									if (rasterDataset) rasterDataset->SetProjection(wkt);
								}

								delete p; //added by Lailin Chen 12/30/2005, don't delete wkt
							}
							else
							{
								// Reasonably good indicator it's WKT
								if (contains(projection, '[') && contains(projection, ']'))
								{
									if (rasterDataset) rasterDataset->SetProjection( projection );
								}
							}
						}
					}
					catch(...)
					{
					}
				Projection = projection;
			}
		}
		catch(...)
		{}

		char * notes = new char[MAX_STRING_LENGTH + 1];
		fread( notes, sizeof(char), MAX_STRING_LENGTH, in);
		// Don't do anything with the notes
}

bool tkGridRaster::contains(char * haystack, char needle) const
{
	for (unsigned int i = 0; i < _tcslen(haystack); i++)
	{
		if (haystack[i] == needle) return true;
	}

	return false;
}

bool tkGridRaster::MemoryAvailable(double bytes)
{
  if (bytes > MAX_INRAM_SIZE) return false;

  MEMORYSTATUS stat;

  GlobalMemoryStatus (&stat);

  if (stat.dwAvailPhys >= bytes)
	  return true;

  return false;
}

bool tkGridRaster::ColorTable2BSTR(BSTR *pVal)
{
	USES_CONVERSION;
	*pVal = A2BSTR("");
	CString stCT, stCPI, s;
	int numColors = 0;
	int cPI = 0;
	if (!hasColorTable)
		return false;
	GDALPaletteInterp cInt;
	const GDALColorEntry * poCE;
	cInt = poColorT->GetPaletteInterpretation();
	numColors = poColorT->GetColorEntryCount();
	if (numColors == 0)
		return false;
	switch (cInt)
	{
		case GPI_Gray: 
			stCPI = "0";break;
		case GPI_RGB: 
			stCPI = "1";break;
		case GPI_CMYK: 
			stCPI = "2";break;
		case GPI_HLS: 
			stCPI = "3";break;
		default:
			stCPI = "0";
	}
	stCT = stCPI + ":0:0:0:";
	for (int i = 0; i < numColors; i++)
	{
		poCE = poColorT->GetColorEntry(i);
		for (int j = 0; j <= 3; j++)
		{
			if (j == 0) s.Format("%d%s",poCE->c1,":");
			else if (j == 1) s.Format("%d%s",poCE->c2,":");
			else if (j == 2) s.Format("%d%s",poCE->c3,":");
			else if (j == 3) s.Format("%d%s",poCE->c4,":");
			stCT = stCT+s;
		}
	}
	stCT = stCT.Mid(0,stCT.GetLength()-1);
	*pVal = stCT.AllocSysString();
	return true;
}

bool tkGridRaster::BSTR2ColorTable(BSTR cTbl)
{
	USES_CONVERSION;
	int numColors = 0;
	CString str (cTbl == NULL ? L"" : cTbl);
	if (str.IsEmpty())
	{
		hasColorTable=false;
		return false;
	}
	CString resToken;
	vector<CString> vCT;
	int curPos = 0;
	resToken= str.Tokenize(_T(":"),curPos);
	while (resToken != _T(""))
	{
	   vCT.push_back(resToken);
	   resToken = str.Tokenize(_T(":"), curPos);
	}
	numColors=vCT.size()/4;
	GDALPaletteInterp cInt;
	int iInt = atoi(vCT[0]);
	switch (iInt)
	{
		case 0:
			cInt=GPI_Gray;break;
		case 1:
			cInt=GPI_RGB;break;
		case 2:
			cInt=GPI_CMYK;break;
		case 3:
			cInt=GPI_HLS;break;
		default:
			cInt=GPI_RGB;
	}
	
	poColorT = new GDALColorTable(cInt);
	GDALColorEntry poCE;

	for (int i = 0; i < numColors-1; i++)
	{
		//DEBUG
		//if (i < 10)
		//{
		//	CString tmp;
		//	tmp.Format("%d%s%s%s%s",i,vCT[(i+1)*4+0],vCT[(i+1)*4+1],vCT[(i+1)*4+2],vCT[(i+1)*4+3]);
		//	AfxMessageBox((LPCTSTR)tmp);
		//}
		poCE.c1=atoi(vCT[(i+1)*4+0]);
		poCE.c2=atoi(vCT[(i+1)*4+1]);
		poCE.c3=atoi(vCT[(i+1)*4+2]);
		poCE.c4=atoi(vCT[(i+1)*4+3]);
		poColorT->SetColorEntry(i,&poCE);
	}
	hasColorTable = true;
	return true;
}

