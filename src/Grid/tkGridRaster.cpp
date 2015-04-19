//********************************************************************************************************
//File name: tkGridRaster.cpp
//Description:  Generic grid wrapper class to allow MapWinGIs to utilize formats supported by GDAL
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License");
//you may not use this file except in compliance with the License. You may obtain a copy of the License at
//http://www.mozilla.org/MPL/
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND, either express or implied. See the License for the specific language governing rights and
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
#include <cassert>
#include <exception>
#include "grdTypes.h"
#include "tkGridRaster.h"
#include "cpl_string.h"
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
	if( _dX != 0.0 && _dY != 0.0 )
	{	column = round( ( x - _xllCenter )/_dX );
		row = _height - round( ( y - _yllCenter )/_dY ) - 1;
	}
}

void tkGridRaster::CellToProj( long column, long row, double & x, double & y )
{
	x = _xllCenter + column*_dX;
	y = _yllCenter + ( ( _height - row - 1)*_dY );
}

inline int tkGridRaster::round( double d )
{	if( ceil(d) - d <= .5 )
		return (int)ceil(d);
	else
		return (int)floor(d);
}

void tkGridRaster::SaveHeaderInfo()
{
	if (_rasterDataset == NULL)
		return;

	if (_poBand == NULL)
		return;

	_poBand->SetNoDataValue(_noDataValue);

	double adfGeoTransform[6];
	adfGeoTransform[0] = _xllCenter - (_dX / 2);
	adfGeoTransform[1] = _dX;
	adfGeoTransform[2] = 0;
	adfGeoTransform[3] = (_height * _dY) + _yllCenter + ((_dY * -1) / 2);
	adfGeoTransform[4] = 0;
	adfGeoTransform[5] = _dY * -1;

    _rasterDataset->SetGeoTransform( adfGeoTransform );

	if (_projection.GetLength() != 0)
	{
		// SetProjection expects WKT
		if (startsWith(_projection.GetBuffer(), "+proj"))
		{
			char * wkt = NULL;

			ProjectionTools * p = new ProjectionTools();
			p->ToESRIWKTFromProj4(&wkt, _projection.GetBuffer());

			if (wkt != NULL && strcmp(wkt, "") != 0)
			{
				_rasterDataset->SetProjection(wkt);
			}

			delete p; //added by Lailin Chen 12/30/2005, don't delete wkt
		}
		else
		{
			// Reasonably good indicator it's WKT
			if (contains(_projection.GetBuffer(), '[') && contains(_projection.GetBuffer(), ']'))
			{
				_rasterDataset->SetProjection( _projection.GetBuffer() );
			}
		}
	}

	// Force reopen to update the nodata value
	_poBand->FlushCache();
	_rasterDataset->FlushCache();

	delete _rasterDataset;
	_poBand = NULL;

	_rasterDataset = GdalHelper::OpenRasterDatasetW(_filename);

	if( _rasterDataset != NULL )
	{
		_poBand = _rasterDataset->GetRasterBand(1);
	}
}

// *****************************************************
//		LoadRaster()
// *****************************************************

bool tkGridRaster::LoadRaster(CStringW filename, bool InRam, GridFileType fileType)
{
	_filename = filename;
	CStringA filenameA = Utility::ConvertToUtf8(filename);
	return LoadRasterCore(filenameA.GetBuffer(), InRam, fileType);
}

bool tkGridRaster::LoadRasterCore(char* filenameA, bool InRam, GridFileType fileType)
{
	__try
	{
		_canScanlineBuffer = false;
		_currentFileType = fileType;

		GDALAllRegister();
		
		_rasterDataset = GdalHelper::OpenRasterDatasetA(filenameA);

		if (!_rasterDataset) {
			return false;
		}

		_nBands = _rasterDataset->GetRasterCount();
		_width = _rasterDataset->GetRasterXSize();
		_height = _rasterDataset->GetRasterYSize();
		double adfGeoTransform[6];

		if( _rasterDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
		{
			// CDM March 18 for bug #100
			// GetGeoTransform will always return the corner cell whether it's
			// PixelIsArea or PixelIsPoint. So, add a half-cell offset to make
			// it PixelIsPoint for MapWinGIS compatibility. Saving out won't hurt
			// anything, since the tiepoints won't change. CreateNew will default
			// to PixelIsArea by applying the reverse of this adjustment.
			const char * pszCellRegistration = _rasterDataset->GetMetadataItem("AREA_OR_POINT", NULL);
		
			// PixelIsArea. Use half-cell offset fix. (Shift Inward)
            _dX = adfGeoTransform[1];
			_dY  = -1 * adfGeoTransform[5];
			_xllCenter = adfGeoTransform[0];
			_xllCenter += (_dX / 2);
			_yllCenter = adfGeoTransform[3] - _height*_dY;
			_yllCenter += (_dY / 2);
		}
		else
		{
			//Try to display images without headers
			_xllCenter = 0;
			_yllCenter = 0;
			_dX = 1;
			_dY = -1;
		}

		if (!this->OpenBand(_activeBandIndex))
		{
			return false;
		}

		this->ReadProjection();

        _inRam = InRam;
		if (_inRam)
		{
			LoadFullBuffer();
		}

		// Determine if scanline buffer can be done in case
		// of inram==false, whether passed or forced by LoadFullBuffer
		if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
		{
			// Chris M 1/28/2007 -- multiply this by two, since we use two buffers
			if (MemoryAvailable(2 * _width * sizeof(_int32)))
				_canScanlineBuffer = true;
		}
		else
		{
			if (MemoryAvailable(2 * _width * sizeof(float)))
				_canScanlineBuffer = true;
		}
	}
	__except(1)
	{
		// Meh - doesn't particularly matter if the dataset loaded.
	}

	return (_rasterDataset != NULL);
}

int tkGridRaster::getNumBands()
{
	return this->_nBands;
}

// *****************************************************
//		OpenBand()
// *****************************************************
bool tkGridRaster::OpenBand(int bandIndex)
{
	int count = _rasterDataset->GetRasterCount();
	
	_poBand = _rasterDataset->GetRasterBand(bandIndex);
	if (!_poBand) {
		return false;
	}

	_cachedMax = -9999.0;
	_cachedMin = 9999.0;

	_activeBandIndex = bandIndex;

	_dataType = _poBand->GetRasterDataType();
	if(!(	(_dataType == GDT_Byte)||(_dataType == GDT_UInt16)||
			(_dataType == GDT_Int16)||(_dataType == GDT_UInt32)||
			(_dataType == GDT_Int32)||(_dataType == GDT_Float32)||
			(_dataType == GDT_Float64)||(_dataType == GDT_CInt16)||
			(_dataType == GDT_CInt32)||(_dataType == GDT_CFloat32)||
			(_dataType == GDT_CFloat64) ) )
	{
		return false;
	}

	// Assuming one band -- "grid" is only used for one band.
	// 3 band and other colorized rasters are used through tkRaster
	_dataType = _poBand->GetRasterDataType();
	_cIntp = _poBand->GetColorInterpretation();
	_poColorT = _poBand->GetColorTable();
	if (_poColorT != NULL)
	{
		_hasColorTable = true;
	}

	//Route the image to the right reader
	//Note: we are assuming here that the data type does not change across bands.
	//However in some complex formats this is not necessarily the case
	switch (_dataType)
	{
		case GDT_Float32:
		case GDT_Float64:
			_genericType=GDT_Float32;
			break;
		case GDT_Byte:
			_genericType = GDT_Byte;
			break;
        case GDT_Int16:
		case GDT_UInt16:
			_genericType = GDT_Int32;
			break;
		default:
			_genericType = GDT_Int32;
			break;
	}

	// Update width and height
    long nWidth = _poBand->GetXSize();
	long nHeight = _poBand->GetYSize();
	if (nWidth != _width)
	{
		_width = nWidth;
	}
	if (nHeight != _height)
	{
		_height = nHeight;
	}

	int success = -1;
	double candidateNoDataValue = _poBand->GetNoDataValue(&success);
	if (success)
	{
		_noDataValue = candidateNoDataValue;
	}
	else
	{
		// Write the assumed nodata value (ArCMapView) so that it doesn't die on GetMin.
		_poBand->SetNoDataValue(_noDataValue);
	}

	return true;
}

// *****************************************************
//		ReadProjection()
// *****************************************************
void tkGridRaster::ReadProjection()
{
	_projection = "";

	const char * wkt = _rasterDataset->GetProjectionRef();
	if (wkt)
	{
		IGeoProjection* proj = NULL;
		ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&proj);
		if (proj)
		{
			USES_CONVERSION;
			VARIANT_BOOL vb;
			CComBSTR bstrWkt(wkt);
			proj->ImportFromAutoDetect(bstrWkt, &vb);
			if (vb)
			{
				CComBSTR bstr;
				proj->ExportToProj4(&bstr);
				_projection = OLE2A(bstr);
			}
			proj->Release();
		}
	}
}

GridDataType tkGridRaster::GetDataType()
{
	if (_genericType == GDT_Int32)
		return LongDataType;
	else if (_genericType == GDT_Byte)
		return ByteDataType;
	else
		return FloatDataType;
}

bool tkGridRaster::CreateNew(CStringW filename, GridFileType newFileType, double dx, double dy, 
			   double xllcenter, double yllcenter, double nodataval, char * projection,
			   long ncols, long nrows, GridDataType DataType, bool CreateInRam, double initialValue, bool applyInitialValue)
{
	GDALAllRegister();


	if (DataType == ShortDataType || DataType == LongDataType)
		_genericType = GDT_Int32;
	if (DataType == FloatDataType || DataType == DoubleDataType || DataType == InvalidDataType || DataType == UnknownDataType)
		_genericType = GDT_Float32;
	if (DataType == ByteDataType)
		_genericType = GDT_Byte;

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

	if (filename.GetLength() == 0)
		return false;
	//if (filename == NULL) return false;

	try
	{
		_wunlink(filename);
	}
	catch(...)
	{}

	m_globalSettings.SetGdalUtf8(true);
	_rasterDataset = poDriver->Create( Utility::ConvertToUtf8(filename), ncols, nrows, 1, _genericType, papszOptions );
	m_globalSettings.SetGdalUtf8(false);

	_currentFileType = newFileType;

	_width = ncols;
	_height = nrows;
	_xllCenter = xllcenter;
	_yllCenter = yllcenter;
	_dX = dx;
	_dY = dy;
	_projection = projection;

	// Adjust by one half-cell offset to make this PixelIsArea.
	// (Shift Outward)
	// Don't bother trying to write this tag out -- GDAL doesn't
	// seem to write that tag no matter how we try. It can read it
	// fine, and it defaults to PixelIsArea if missing... so no problem.

	double adfGeoTransform[6];
	adfGeoTransform[0] = _xllCenter - (_dX / 2);
	adfGeoTransform[1] = _dX;
	adfGeoTransform[2] = 0;
	adfGeoTransform[3] = (_height * dy) + _yllCenter + ((_dY * -1) / 2);
	adfGeoTransform[4] = 0;
	adfGeoTransform[5] = _dY * -1;

    _rasterDataset->SetGeoTransform( adfGeoTransform );

	if (_rasterDataset == NULL)
		return false;

    _poBand = _rasterDataset->GetRasterBand(1);		// to open the first band is ok here;

	if (_hasColorTable)
	{
		_poBand->SetColorTable(_poColorT);
	}

	_poBand->SetNoDataValue(nodataval);
	_noDataValue = nodataval;

	_filename = filename;
	_inRam = CreateInRam;

	if (_inRam)
	{
		// Make a buffer
		if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
		{
			_int32buffer = (_int32 *) CPLMalloc( sizeof(_int32)*_width*_height );
			if (!_int32buffer)
			{
				_inRam = false; // Force the user to use disk-based;
				// not enough memory likely to load into memory.
			}
		}
		else
		{
			_floatbuffer = (float *) CPLMalloc( sizeof(float)*_width*_height );
			if (!_floatbuffer)
			{
				_inRam = false; // Force the user to use disk-based;
				// not enough memory likely to load into memory.
			}
		}
	}

	// Write the initial value
	if (applyInitialValue)
		clear(initialValue);

	if (projection && strcmp(projection, "") != 0)
	{
		// SetProjection expects WKT
		if (startsWith(projection, "+proj"))
		{
			char * wkt = NULL;

			ProjectionTools * p = new ProjectionTools();
			p->ToESRIWKTFromProj4(&wkt, projection);

			if (wkt != NULL && strcmp(wkt, "") != 0)
			{
				_rasterDataset->SetProjection(wkt);
			}

			delete p; //added by Lailin Chen 12/30/2005, don't delete wkt
		}
		else
		{
			// Reasonably good indicator it's WKT
			if (contains(projection, '[') && contains(projection, ']'))
			{
				_rasterDataset->SetProjection( projection );
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

	GDALAllRegister();

	GDALDriver *poDriver;
    char **papszMetadata;

	poDriver = _rasterDataset->GetDriver();
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
}

bool tkGridRaster::Close()
{
	try
	{
		if (_rasterDataset != NULL)
		{
			delete _rasterDataset;
			_rasterDataset=NULL;
		}

		if( _int32buffer != NULL )
		{
			CPLFree( _int32buffer );
			_int32buffer = NULL;
		}

		if( _floatbuffer != NULL )
		{
			CPLFree( _floatbuffer );
			_floatbuffer = NULL;
		}

		if( _int32ScanlineBufferA != NULL )
		{
			CPLFree( _int32ScanlineBufferA );
			_int32ScanlineBufferA = NULL;
		}

		if( _floatScanlineBufferA != NULL )
		{
			CPLFree( _floatScanlineBufferA );
			_floatScanlineBufferA = NULL;
		}

		if( _int32ScanlineBufferB != NULL )
		{
			CPLFree( _int32ScanlineBufferB );
			_int32ScanlineBufferB = NULL;
		}

		if( _floatScanlineBufferB != NULL )
		{
			CPLFree( _floatScanlineBufferB );
			_floatScanlineBufferB = NULL;
		}

		_cachedMax = -9999;
		_cachedMin = 9999;
		_genericType = GDT_Unknown;
		_hasColorTable = false;
		_activeBandIndex = 1;
	}
	catch(...)
	{
		return false;
	}

	return true;
}

double tkGridRaster::GetMaximum()
{
	if (_cachedMax != -9999) return _cachedMax;

	int bGotMin, bGotMax;
	double adfMinMax[2];
	adfMinMax[0] = _poBand->GetMinimum( &bGotMin );
    adfMinMax[1] = _poBand->GetMaximum( &bGotMax );
    if( ! (bGotMin && bGotMax) )
	{
		GDALComputeRasterMinMax((GDALRasterBandH)_poBand, false, adfMinMax);
	}

	_cachedMax = adfMinMax[1];
	_cachedMin = adfMinMax[0];

	return adfMinMax[1];
}

double tkGridRaster::GetMinimum()
{
	if (_cachedMin != 9999) return _cachedMin;

	int bGotMin, bGotMax;
	double adfMinMax[2];
	adfMinMax[0] = _poBand->GetMinimum( &bGotMin );
    adfMinMax[1] = _poBand->GetMaximum( &bGotMax );
    if( ! (bGotMin && bGotMax) )
	{
	   GDALComputeRasterMinMax((GDALRasterBandH)_poBand, false, adfMinMax);
	}

	_cachedMax = adfMinMax[1];
	_cachedMin = adfMinMax[0];

	return adfMinMax[0];
}

void tkGridRaster::LoadFullBuffer()
{
	try
	{
	if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
	{
		// Is there enough memory available?
		// CDM 1/6/2007: On one really extreme case, the amount
		// of memory required would have caused the size of the largest
		// datatype to overflow repeatedly. So check in a stepwise
		// fashion "up to" the size we really want
        if (!MemoryAvailable(_width*_height))
		{
			_inRam = false;
			return;
		}
		if (!MemoryAvailable(_width*_height*2))
		{
			_inRam = false;
			return;
		}
		if (!MemoryAvailable(_width*_height*sizeof(_int32)*2))
		{
			_inRam = false;
			return;
		}

		_int32buffer = (_int32 *) CPLMalloc( sizeof(_int32)*_width*_height );
		if (!_int32buffer)
		{
			_inRam = false; // Force the user to use disk-based;
			// not enough memory likely to load into memory.
			return;
		}

		_poBand->AdviseRead ( 0, 0, _width, _height, _width, _height, GDT_Int32, NULL);
		_poBand->RasterIO( GF_Read, 0, 0, _width, _height,
							_int32buffer, _width, _height, GDT_Int32,0, 0 );

	}
	else
	{
		// Is there enough memory available?
		// CDM 1/6/2007: On one really extreme case, the amount
		// of memory required would have caused the size of the largest
		// datatype to overflow repeatedly. So check in a stepwise
		// fashion "up to" the size we really want
        if (!MemoryAvailable(_width*_height))
		{
			_inRam = false;
			return;
		}
		if (!MemoryAvailable(_width*_height*2))
		{
			_inRam = false;
			return;
		}
		if (!MemoryAvailable(_width*_height*sizeof(float)*2))
		{
			_inRam = false;
			return;
		}

		_floatbuffer = (float *) CPLMalloc( sizeof(float)*_width*_height );
		if (!_floatbuffer)
		{
			_inRam = false; // Force the user to use disk-based;
			// not enough memory likely to load into memory.
			return;
		}

		_poBand->AdviseRead ( 0, 0, _width, _height, _width, _height, GDT_Float32, NULL);
		_poBand->RasterIO( GF_Read, 0, 0, _width, _height,
						_floatbuffer, _width, _height, GDT_Float32,0, 0 );
	}
	}
	catch(...)
	{
		_inRam = false;
	}
}

bool tkGridRaster::GetFloatWindow(void *Vals, long StartRow, long EndRow, long StartCol, long EndCol, bool useDouble)
{
	if (_poBand == NULL) return false;

	// Load from buffer if it exists, otherwise use RasterIO.
	// Since there are lots of potential buffers, just call getValue
	if (_int32buffer != NULL || _floatbuffer != NULL || _int32ScanlineBufferB != NULL || _floatScanlineBufferB != NULL || _int32ScanlineBufferA != NULL || _floatScanlineBufferA != NULL)
	{
		long position = 0;

		double* ValsDouble = reinterpret_cast<double*>(Vals);
		float* ValsFloat = reinterpret_cast<float*>(Vals);

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				// Just use getvalue -- it will be memory-based,
				// and will account for the correct buffer.
				if (useDouble)
				{
					ValsDouble[position++] = getValue(j, i);
				}
				else
				{
					ValsFloat[position++] = static_cast<float>(getValue(j, i));
				}
			}
		}
	}
	else
	{
		// Use Rasterio Directly from disk -- faster for large grids,
		// which is likely when this function will be called anyway.
		try
		{
			GDALDataType type = useDouble ? GDT_Float64 : GDT_Float32;
			_poBand->AdviseRead ( StartCol, StartRow, (EndCol - StartCol)+1, (EndRow - StartRow)+1, (EndCol - StartCol)+1, (EndRow - StartRow)+1, type, NULL);
			_poBand->RasterIO( GF_Read, StartCol, StartRow, (EndCol - StartCol)+1, (EndRow - StartRow)+1, Vals, (EndCol - StartCol)+1, (EndRow - StartRow)+1, type, 0, 0 );
		}
		catch(...)
		{
			return false;
		}
	}

	return true;
}


bool tkGridRaster::PutFloatWindow(void *Vals, long StartRow, long EndRow, long StartCol, long EndCol, bool useDouble)
{
	if (_poBand == NULL) return false;

	// Reset our cached max/min values to our "unset" flags
	_cachedMin = 9999;
	_cachedMax = -9999;

	// Save to buffer if it exists, otherwise use RasterIO.
	if (_int32buffer != NULL || _floatbuffer != NULL || _int32ScanlineBufferB != NULL || 
		_floatScanlineBufferB != NULL || _int32ScanlineBufferA != NULL || _floatScanlineBufferA != NULL)
	{
		
		double* ValsDouble = reinterpret_cast<double*>(Vals);
		float* ValsFloat = reinterpret_cast<float*>(Vals);

		long position = 0;

		for (long j = StartRow; j <= EndRow; j++)
		{
			for (long i = StartCol; i <= EndCol; i++)
			{
				// Just use putvalue -- it will be memory-based,
				// and will account for the correct buffer.
				if (useDouble)
				{
					putValue(j, i, ValsDouble[position++]);
					
				}
				else
				{
					putValue(j, i,  static_cast<float>(ValsFloat[position++]));
				}
			}
		}
	}
	else
	{
		// Use Rasterio Directly to disk -- faster for large grids,
		// which is likely when this function will be called anyway.
		try
		{
			GDALDataType type = useDouble ? GDT_Float64 : GDT_Float32;
			_poBand->RasterIO( GF_Write, StartCol, StartRow, (EndCol - StartCol)+1, (EndRow - StartRow)+1,
								Vals, (EndCol - StartCol)+1, (EndRow - StartRow)+1, type, 0, 0 );
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
	if (_inRam && _int32buffer != NULL)
	{
		for (int i = 0; i < _width * _height; i++)
			_int32buffer[i] = static_cast<_int32>(value);
	}
	else if (_inRam && _floatbuffer != NULL)
	{
		for (int i = 0; i < _width * _height; i++)
			_floatbuffer[i] = static_cast<float>(value);
	}
	else if (!_inRam)
	{
		if (_poBand != NULL)
			_poBand->Fill(value);
	}
}

bool tkGridRaster::SaveFullBuffer()
{
	// First, check to see if scanline buffering has been used. This will likely be the most common.
	// If not, drop down to "buffer save"
	bool retVal = false;

	// Chris Michaelis 1/28/2007
	// Do not use ELSE's here -- otherwise only every other line will be saved

	if (_genericType != GDT_Int32 && _scanlineADataChanged && _floatScanlineBufferA != NULL)
	{
		_poBand->RasterIO( GF_Write, 0, _scanlineBufferNumberA, _width, 1,
									_floatScanlineBufferA, _width, 1, GDT_Float32,0, 0 );
		retVal = true;
		_scanlineADataChanged = false;
	}

	if ((_genericType == GDT_Int32 || _genericType == GDT_Byte) && _scanlineADataChanged && _int32ScanlineBufferA != NULL)
	{
		_poBand->RasterIO( GF_Write, 0, _scanlineBufferNumberA, _width, 1,
									_int32ScanlineBufferA, _width, 1, GDT_Int32,0, 0 );
		retVal = true;
		_scanlineADataChanged = false;
	}

	if (_genericType != GDT_Int32 && _scanlineBDataChanged && _floatScanlineBufferB != NULL)
	{
		_poBand->RasterIO( GF_Write, 0, _scanlineBufferNumberB, _width, 1,
									_floatScanlineBufferB, _width, 1, GDT_Float32,0, 0 );
		retVal = true;
		_scanlineBDataChanged = false;
	}

	if ((_genericType == GDT_Int32 || _genericType == GDT_Byte) && _scanlineBDataChanged && _int32ScanlineBufferB != NULL)
	{
		_poBand->RasterIO( GF_Write, 0, _scanlineBufferNumberB, _width, 1,
									_int32ScanlineBufferB, _width, 1, GDT_Int32,0, 0 );
		retVal = true;
		_scanlineBDataChanged = false;
	}

	// Buffer save -- if not scanlining.
	if ((_genericType == GDT_Int32 || _genericType == GDT_Byte) && _int32buffer != NULL)
	{
		_poBand->RasterIO( GF_Write, 0, 0, _width, _height,
							_int32buffer, _width, _height, GDT_Int32,0, 0 );
		retVal = true;
	}

	if (_genericType == GDT_Float32 && _floatbuffer != NULL)
	{
		_poBand->RasterIO( GF_Write, 0, 0, _width, _height,
						_floatbuffer, _width, _height, GDT_Float32,0, 0 );
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

	_rasterDataset->FlushCache();

	return retVal;
}

bool tkGridRaster::Save(CStringW saveToFilename, GridFileType newFileFormat)
{
	// Write the .prj file
	if (_projection.GetLength() > 0)
	{
		CStringW prjFilename = saveToFilename;
		prjFilename = prjFilename.Left(prjFilename.GetLength() - 3) + L"prj";
		FILE * prjFile = NULL;
		prjFile = _wfopen(prjFilename, L"wb");
		if (prjFile)
		{
			char * wkt;
			ProjectionTools * p = new ProjectionTools();
			p->ToESRIWKTFromProj4(&wkt, _projection.GetBuffer());

			fprintf(prjFile, "%s", wkt);
			fclose(prjFile);
			prjFile = NULL;
			delete p; //added by Lailin Chen 12/30/2005
		}
	}

	if (_filename == saveToFilename && newFileFormat == _currentFileType)
	{
		return SaveFullBuffer();
	}
	else if (newFileFormat == _currentFileType)
	{
		// Before saving to a different file type,
		// save out any scanline memory buffers to the current file
		if (_scanlineADataChanged || _scanlineBDataChanged)
			SaveFullBuffer();
		
		// Note that if it's inram fully, the above isn't necessary;
		// it can be written directly from our memory buffer by GDAL below.
		GDALDataset *poDstDS;
		
		CStringA saveFilenameA = Utility::ConvertToUtf8(saveToFilename);
		m_globalSettings.SetGdalUtf8(true);
		poDstDS = _rasterDataset->GetDriver()->CreateCopy( saveFilenameA, _rasterDataset, FALSE, NULL, NULL, NULL );
		m_globalSettings.SetGdalUtf8(false);

		if (poDstDS == NULL)
			return false;

		// If in ram, save any changes also.
		if (_inRam)
		{
			if ((_genericType == GDT_Int32 || _genericType == GDT_Byte) && _int32buffer != NULL)
			{
				_rasterDataset->GetRasterBand(1)->RasterIO( GF_Write, 0, 0, _width, _height,
								_int32buffer, _width, _height, GDT_Int32, 0, 0 );
			}
			else if (_genericType ==  GDT_Float32 && _floatbuffer != NULL)
			{
				_rasterDataset->GetRasterBand(1)->RasterIO( GF_Write, 0, 0, _width, _height,
							_floatbuffer, _width, _height, GDT_Float32,0, 0 );
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
		outFile->CreateNew(saveToFilename, newFileFormat, _dX, _dY, _xllCenter, _yllCenter, _noDataValue, _projection.GetBuffer(), _width, _height, GetDataType(), false, _noDataValue, false);
		for (int j = 0; j < _height; j++)
			for (int i = 0; i < _width; i++)
				outFile->putValue(j, i, getValue(j, i));
		outFile->Save(saveToFilename, newFileFormat);
		outFile->Close();
		outFile = NULL;

		return true;
	}
}

double tkGridRaster::getValue( long Row, long Column )
{
	if (_inRam == true && _int32buffer != NULL)
			return static_cast<double>(_int32buffer[Column + Row * _width]);
	else if (_inRam == true && _floatbuffer != NULL)
			return static_cast<double>(_floatbuffer[Column + Row * _width]);
	else
	{
		// Read from disk - buffering one line at a time if possible
		if (Row == _scanlineBufferNumberA)
		{
			// Return from one-row buffer
			_scanlineLastAccessed = 'A';
			if ((_genericType == GDT_Int32 || _genericType == GDT_Byte) && _int32ScanlineBufferA != NULL)
				return static_cast<double>(_int32ScanlineBufferA[Column]);
			else if (_floatScanlineBufferA != NULL)
				return static_cast<double>(_floatScanlineBufferA[Column]);
		}
		else if (Row == _scanlineBufferNumberB)
		{
			// Return from one-row buffer
			_scanlineLastAccessed = 'B';
			if ((_genericType == GDT_Int32 || _genericType == GDT_Byte) && _int32ScanlineBufferB != NULL)
				return static_cast<double>(_int32ScanlineBufferB[Column]);
			else if (_floatScanlineBufferB != NULL)
				return static_cast<double>(_floatScanlineBufferB[Column]);
		}

		// If we're still in the function, need to load a buffer
		// or get a single value
		if (_canScanlineBuffer)
		{
			if (_scanlineLastAccessed == 'A')
			{
				// Load B
				if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
				{
					// If put_Value was called and wrote into the memory buffer,
					// save that before dumping the old buffer data.
					if (_scanlineBDataChanged)
						SaveFullBuffer();

					if (_int32ScanlineBufferB == NULL)
						_int32ScanlineBufferB = (_int32*) CPLMalloc( sizeof(_int32)*_width);

					// If we get here and the buffer is still null, apparently
					// there's not really enough memory to do this...!
					if (_int32ScanlineBufferB == NULL)
					{
						// Clear and prevent buffering
						_scanlineBufferNumberA = -1;
						_scanlineBufferNumberB = -1;
						_canScanlineBuffer = false;

						// Return the requested value with a recursive call
						return getValue(Row, Column);
					}

					_poBand->RasterIO( GF_Read, 0, Row, _width, 1,
								_int32ScanlineBufferB, _width, 1, GDT_Int32,0, 0 );
					_scanlineBufferNumberB = Row;
					_scanlineLastAccessed = 'B';
					_scanlineBDataChanged = false;
					return static_cast<double>(_int32ScanlineBufferB[Column]);
				}
				else
				{
					// If put_Value was called and wrote into the memory buffer,
					// save that before dumping the old buffer data.
					if (_scanlineBDataChanged)
						SaveFullBuffer();

					if (_floatScanlineBufferB == NULL)
						_floatScanlineBufferB = (float *) CPLMalloc( sizeof(float)*_width);

					// If we get here and the buffer is still null, apparently
					// there's not really enough memory to do this...!
					if (_floatScanlineBufferB == NULL)
					{
						// Clear and prevent buffering
						_scanlineBufferNumberA = -1;
						_scanlineBufferNumberB = -1;
						_canScanlineBuffer = false;

						// Return the requested value with a recursive call
						return getValue(Row, Column);
					}

					_poBand->RasterIO( GF_Read, 0, Row, _width, 1,
									_floatScanlineBufferB, _width, 1, GDT_Float32,0, 0 );
					_scanlineBufferNumberB = Row;
					_scanlineLastAccessed = 'B';
					_scanlineBDataChanged = false;
					return static_cast<double>(_floatScanlineBufferB[Column]);
				}
			}
			else
			{
				// Load A
				if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
				{
					// If put_Value was called and wrote into the memory buffer,
					// save that before dumping the old buffer data.
					if (_scanlineADataChanged)
						SaveFullBuffer();

					if (_int32ScanlineBufferA == NULL)
						_int32ScanlineBufferA = (_int32*) CPLMalloc( sizeof(_int32)*_width);

					// If we get here and the buffer is still null, apparently
					// there's not really enough memory to do this...!
					if (_int32ScanlineBufferA == NULL)
					{
						// Clear and prevent buffering
						_scanlineBufferNumberA = -1;
						_scanlineBufferNumberB = -1;
						_canScanlineBuffer = false;

						// Return the requested value with a recursive call
						return getValue(Row, Column);
					}

					_poBand->RasterIO( GF_Read, 0, Row, _width, 1,
								_int32ScanlineBufferA, _width, 1, GDT_Int32,0, 0 );
					_scanlineBufferNumberA = Row;
					_scanlineLastAccessed = 'A';
					_scanlineADataChanged = false;
					return static_cast<double>(_int32ScanlineBufferA[Column]);
				}
				else
				{
					// If put_Value was called and wrote into the memory buffer,
					// save that before dumping the old buffer data.
					if (_scanlineADataChanged)
						SaveFullBuffer();

					if (_floatScanlineBufferA == NULL)
						_floatScanlineBufferA = (float *) CPLMalloc( sizeof(float)*_width);

					// If we get here and the buffer is still null, apparently
					// there's not really enough memory to do this...!
					if (_floatScanlineBufferA == NULL)
					{
						// Clear and prevent buffering
						_scanlineBufferNumberA = -1;
						_scanlineBufferNumberB = -1;
						_canScanlineBuffer = false;

						// Return the requested value with a recursive call
						return getValue(Row, Column);
					}

					_poBand->RasterIO( GF_Read, 0, Row, _width, 1,
									_floatScanlineBufferA, _width, 1, GDT_Float32,0, 0 );
					_scanlineBufferNumberA = Row;
					_scanlineLastAccessed = 'A';
					_scanlineADataChanged = false;
					return static_cast<double>(_floatScanlineBufferA[Column]);
				}

			}
		}
		else
		{
			// Read one value at a time. Performance hit...
			_int32 pafScanAreaInt;
			float pafScanAreaFloat;

			if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
			{
				//poBand->AdviseRead ( Column, Row, 1, 1, 1, 1, GDT_Int32, NULL);
				_poBand->RasterIO( GF_Read, Column, Row, 1, 1,
									&pafScanAreaInt, 1, 1, GDT_Int32,0, 0 );
			}
			else
			{
				//poBand->AdviseRead ( Column, Row, 1, 1, 1, 1, GDT_Float32, NULL);
				_poBand->RasterIO( GF_Read, Column, Row, 1, 1,
								&pafScanAreaFloat, 1, 1, GDT_Float32,0, 0 );
			}

			if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
				return static_cast<double>(pafScanAreaInt);
			else
				return static_cast<double>(pafScanAreaFloat);
		}
	}
}

void tkGridRaster::putValue( long Row, long Column, double Value )
{
	// Reset our cached max/min values to our "unset" flags
	_cachedMin = 9999;
	_cachedMax = -9999;

	if (_inRam == true && _int32buffer != NULL)
			_int32buffer[Column + Row * _width] = static_cast<_int32>(Value);
	else if (_inRam == true && _floatbuffer != NULL)
			_floatbuffer[Column + Row * _width] = static_cast<float>(Value);

	// If there is a loaded scanline buffer for this row, use it and set the modified flag.
	else if ((_genericType == GDT_Int32 || _genericType == GDT_Byte) && _scanlineBufferNumberB == Row && _int32ScanlineBufferB != NULL)
	{
		_int32ScanlineBufferB[Column] = static_cast<_int32>(Value);
		_scanlineBDataChanged = true;
	}
	else if (_genericType != GDT_Int32 && _scanlineBufferNumberB == Row && _floatScanlineBufferB != NULL)
	{
		_floatScanlineBufferB[Column] = static_cast<float>(Value);
		_scanlineBDataChanged = true;
	}
	else if ((_genericType == GDT_Int32 || _genericType == GDT_Byte) && _scanlineBufferNumberA == Row && _int32ScanlineBufferA != NULL)
	{
		_int32ScanlineBufferA[Column] = static_cast<_int32>(Value);
		_scanlineADataChanged = true;
	}
	else if (_genericType != GDT_Int32 && _scanlineBufferNumberA == Row && _floatScanlineBufferA != NULL)
	{
		_floatScanlineBufferA[Column] = static_cast<float>(Value);
		_scanlineADataChanged = true;
	}
	else if (_canScanlineBuffer)
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

		if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
		{
			_int32 pafScanAreaInt = static_cast<_int32>(Value);
			_poBand->RasterIO( GF_Write, Column, Row, 1, 1,
								&pafScanAreaInt, 1, 1, GDT_Int32,0, 0 );
		}
		else
		{
			float pafScanAreaFloat = static_cast<float>(Value);
			_poBand->RasterIO( GF_Write, Column, Row, 1, 1,
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
			fwrite( &_width, sizeof(int),1,out);
			fwrite( &_height, sizeof(int),1,out);
			fwrite( &_dX, sizeof(double),1,out);
			fwrite( &_dY, sizeof(double),1,out);
			fwrite( &_xllCenter, sizeof(double),1,out);
			fwrite( &_yllCenter, sizeof(double),1,out);

			DATA_TYPE type = FLOAT_TYPE;
			if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
				type = LONG_TYPE;

			fwrite( &type, sizeof(DATA_TYPE),1,out);

			if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
			{
				long nodata = static_cast<long>(_noDataValue);
				fwrite( &nodata, sizeof(long),1,out);
			}
			else
			{
				double nodata = _noDataValue;
				fwrite( &nodata, sizeof(double),1,out);
			}

			char * projection = new char[MAX_STRING_LENGTH + 1];
			if (_projection.GetLength() <= 255)
			{
				strcpy( projection, _projection.GetBuffer());
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
				delete[] pad;
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
			delete[] pad;
			// Done with header

			double total = _height * _width;
			int percent = 0;
			long num_written = 0;

			// Duplicate the entire loop for this decision, rather than placing this
			// where it makes sense inside the loop. This means the "if" isn't evaluated
			// width*height times.
			if (_genericType == GDT_Int32 || _genericType == GDT_Byte)
			{
				long lValue;
				for( int j = 0; j < _height; j++ )
				{	for( int i = 0; i < _width; i++ )
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
				for( int j = 0; j < _height; j++ )
				{	for( int i = 0; i < _width; i++ )
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
	_cachedMin = 9999;
	_cachedMax = -9999;

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

			double total = _height * _width;

			int percent = 0;
			long num_read = 0;

			float fData;
			long lData;
			short sData;
			double dData;

			for( int j = 0; j < _height; j++ )
			{	for( int i = 0; i < _width; i++ )
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
		_width = ncols;

		long nrows;
		fread( &nrows, sizeof(int),1,in);
		_height = nrows;

		fread( &_dX, sizeof(double),1,in);

		fread( &_dY, sizeof(double),1,in);

		fread( &_xllCenter, sizeof(double),1,in);

		fread( &_yllCenter, sizeof(double),1,in);

		fread( &bgdDataType, sizeof(DATA_TYPE),1,in);

		if (bgdDataType == LONG_TYPE)
		{
			long nodata_value;
			fread( &nodata_value, sizeof(long),1,in);
			_noDataValue = static_cast<double>(nodata_value);
		}
		else if (bgdDataType == SHORT_TYPE)
		{
			short nodata_value;
			fread( &nodata_value, sizeof(short),1,in);
			_noDataValue = static_cast<double>(nodata_value);
		}
		else if (bgdDataType == FLOAT_TYPE)
		{
			float nodata_value;
			fread( &nodata_value, sizeof(float),1,in);
			_noDataValue = static_cast<double>(nodata_value);
		}
		else if (bgdDataType == DOUBLE_TYPE)
		{
			double nodata_value;
			fread( &nodata_value, sizeof(double),1,in);
			_noDataValue = nodata_value;
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

			if (projection && strcmp(projection, "") != 0)
			{
					try
					{
						if (projection && strcmp(projection, "") != 0 )
						{
							// SetProjection expects WKT
							if (startsWith(projection, "+proj"))
							{
								char * wkt = NULL;

								ProjectionTools * p = new ProjectionTools();
								p->ToESRIWKTFromProj4(&wkt, projection);

								if (wkt != NULL && strcmp(wkt, "") != 0)
								{
									if (_rasterDataset) _rasterDataset->SetProjection(wkt);
								}

								delete p; //added by Lailin Chen 12/30/2005, don't delete wkt
							}
							else
							{
								// Reasonably good indicator it's WKT
								if (contains(projection, '[') && contains(projection, ']'))
								{
									if (_rasterDataset) _rasterDataset->SetProjection( projection );
								}
							}
						}
					}
					catch(...)
					{
					}
				_projection = projection;
			}
		}
		catch(...)
		{}

		char * notes = new char[MAX_STRING_LENGTH + 1];
		fread( notes, sizeof(char), MAX_STRING_LENGTH, in);
		delete[] notes;
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
	if (!_hasColorTable)
		return false;
	GDALPaletteInterp cInt;
	const GDALColorEntry * poCE;
	cInt = _poColorT->GetPaletteInterpretation();
	numColors = _poColorT->GetColorEntryCount();
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
		poCE = _poColorT->GetColorEntry(i);
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
	SysFreeString(*pVal);
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
		_hasColorTable=false;
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

	_poColorT = new GDALColorTable(cInt);
	GDALColorEntry poCE;

	for (int i = 0; i < numColors-1; i++)
	{
		poCE.c1=atoi(vCT[(i+1)*4+0]);
		poCE.c2=atoi(vCT[(i+1)*4+1]);
		poCE.c3=atoi(vCT[(i+1)*4+2]);
		poCE.c4=atoi(vCT[(i+1)*4+3]);
		_poColorT->SetColorEntry(i,&poCE);
	}
	_hasColorTable = true;
	return true;
}

// ***********************************************
//	    IsRgb
// ***********************************************
bool tkGridRaster::IsRgb()
{ 
	return GdalHelper::IsRgb(_rasterDataset);
}

// ***********************************************
//	    GetBand
// ***********************************************
GDALRasterBand* tkGridRaster::GetBand(int index)
{
	return _rasterDataset->GetRasterBand(index);
}

