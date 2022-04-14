# include "StdAfx.h"
# include "GridManager.h"
#undef HOST_FILLORDER /* MWGIS-182 */
# include "xtiffio.h"  /* for TIFF */
# include "tiff.h"
# include "Grid.h"

GridManager::GridManager()
{
	initialize_esri();
}

GridManager::~GridManager()
{	shutdown_esri();
}

DATA_TYPE GridManager::getGridDataType( const char * cfilename, GRID_TYPE GridType )
{	
	CString filename = cfilename;

	GRID_TYPE grid_type = GridType;
	if( GridType == USE_EXTENSION )
		grid_type = getGridType( filename );
	
	if( grid_type == GEOTIFF_GRID) //added 8/26/05 --ah
	{
		DATA_TYPE type = UNKNOWN_TYPE;
		TIFF *tiff = XTIFFOpen((char *)cfilename, "r"); // TIFF-level descriptor
		if(tiff)
		{
			uint16_t sampleFormat, bitsPerSample;

			TIFFGetField(tiff,TIFFTAG_SAMPLEFORMAT, &sampleFormat);
			TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
			if(sampleFormat == 1)
			{//type is unsigned int
				type = SHORT_TYPE;
			}
			else if(sampleFormat == 2)
			{//type is signed int
				type = LONG_TYPE;
			}
			else if(sampleFormat == 3)
			{//IEEE floating-point value
				if(bitsPerSample == 32)
				{
					type = FLOAT_TYPE;
				}
				else if(bitsPerSample == 64)
				{
					type = DOUBLE_TYPE;
				}
			}
			//TODO: may want to check for other sample formats:
			//4 = unknown, 5 = complex int, 6 = complex float or double
			//0 = undefined -> check bits per sample and samples per pixel
			//to determine size.
			else
			{
				type = UNKNOWN_TYPE;
			}			
		}
		XTIFFClose(tiff);

		return type;
	}
	else if( grid_type == ASCII_GRID )
		return DOUBLE_TYPE;
	else if( grid_type == BINARY_GRID )
	{	
		FILE * in = fopen( filename, "rb" );
		if( !in )
			return INVALID_DATA_TYPE;
		else
		{	long ncols;
			fread( &ncols, sizeof(int),1,in);
			long nrows;
			fread( &nrows, sizeof(int),1,in);
			double dx;
			fread( &dx, sizeof(double),1,in);
			double dy;
			fread( &dy, sizeof(double),1,in);
			double xllcenter;
			fread( &xllcenter, sizeof(double),1,in);
			double yllcenter;
			fread( &yllcenter, sizeof(double),1,in);
			DATA_TYPE data_type;
			fread( &data_type, sizeof(DATA_TYPE),1,in);
			fclose( in );
			return data_type;
		}
	}
	else if( grid_type == SDTS_GRID )
		return DOUBLE_TYPE;
	else if( grid_type == ESRI_GRID )
	{	if( filename.GetLength() <= 0 )
			return INVALID_DATA_TYPE;		
		else
		{	DATA_TYPE data_type = esri_data_type(filename);
			return data_type;
		}
	}
	
	return INVALID_DATA_TYPE;
}

GRID_TYPE GridManager::getGridType( const char * cfilename )
{	
	CString filename = cfilename;

	GRID_TYPE grid_type = INVALID_GRID_TYPE;

	if( filename.GetLength() > 0 )
	{	
		char * clean_filename = new char[ filename.GetLength() + 1];
		strcpy( clean_filename, filename );
		for( int i = _tcslen( clean_filename ) - 1; i >= 0; i-- )
		{	if( clean_filename[i] == '\\' || clean_filename[i] == '/' )
				clean_filename[i] = '\0';
			else
				break;
		}

		CFileFind cff;
		if( cff.FindFile( clean_filename ) == TRUE )
		{
			cff.FindNextFile();

			if( cff.IsDirectory() )
				grid_type = ESRI_GRID;
			else
			{	CString f_name = cff.GetFileName();
				CString extension = f_name.Right( f_name.GetLength() - 1 - f_name.ReverseFind('.') );
				if(extension.CompareNoCase("TIF") == 0)//added 8/15/05 -- ah
					grid_type = GEOTIFF_GRID;
				else if( extension.CompareNoCase("ASC") == 0 )
					grid_type = ASCII_GRID;
				else if( extension.CompareNoCase("BGD") == 0 )
					grid_type = BINARY_GRID;
				else if( extension.CompareNoCase("DDF") == 0 )
					grid_type = SDTS_GRID;
			}
			cff.Close();
			return grid_type;
		}
		cff.Close();

		//File does not exist so parse it out
		int length = _tcslen(filename );
		bool foundPeriod = false;
		for( int e = length-1; e >= 0; e-- )
		{	if( filename[e] == '\\' || filename[e] == '/' )
				return ESRI_GRID;
			else if( filename[e] == '.' )
			{
				foundPeriod = true;
				break;
			}
		}

		if ( foundPeriod == false ) 
			return ESRI_GRID;
		
		if( length > 4 )
		{
			char ext[4];
			ext[0] = filename[length - 3];
			ext[1] = filename[length - 2];
			ext[2] = filename[length - 1];
			ext[3] = '\0';
			
			if( islower( ext[0] ) )
				ext[0] = toupper( ext[0] );
			if( islower( ext[1] ) )
				ext[1] = toupper( ext[1] );
			if( islower( ext[2] ) )
				ext[2] = toupper( ext[2] );

			if( !strcmp( ext, "TIF" ) )//added 8/15/05 -- ah
				return GEOTIFF_GRID;
			else if( !strcmp( ext, "ASC" ) )
				return ASCII_GRID;
			else if( !strcmp( ext, "BGD" ) )
				return BINARY_GRID;
			else if( !strcmp( ext, "DDF" ) )
				return SDTS_GRID;
		}
	}
	
	return grid_type;
}

bool GridManager::canUseEsriGrids()
{	return can_use_esri_grids();
}

bool GridManager::deleteGrid( const char * cfilename, GRID_TYPE GridType )
{
	CString filename = cfilename;

	GRID_TYPE type = GridType;
	if( type == USE_EXTENSION )
		type = getGridType( filename );

	if( type == GEOTIFF_GRID)//added 8/15/05 -- ah, might need to change to delete
		_unlink(filename);
	else if( type == ASCII_GRID )
		_unlink( filename );			
	else if( type == BINARY_GRID )
		_unlink( filename );			
	else if( type == ESRI_GRID )
		delete_esri_grid( filename );			
	else if( type == SDTS_GRID )
	{
		CString prefix = filename.Left( 4 );
		if( prefix.GetLength() != 4 )
			return false;

		CFileFind cff;
		if( cff.FindFile( filename ) == TRUE )
		{
			cff.FindNextFile();

			CString f_name = cff.GetFileName();
			CString extension = f_name.Right( f_name.GetLength() - 1 - f_name.ReverseFind('.') );
			
			if( extension.CompareNoCase("DDF") == 0 )
			{	CString fprefix = f_name.Left( 4 );
				if( prefix.CompareNoCase( fprefix ) == 0 )
					_unlink( f_name );
			}			
		}
		cff.Close();
	}
	else
		return false;

	return true;
}

// ****************************************************************
//						NeedProxyForGrid()						         
// ****************************************************************
bool GridManager::NeedProxyForGrid(CStringW filename, tkGridProxyMode proxyMode, IGrid* grid)
{
	if (proxyMode == gpmNoProxy )
		return false;

	if (proxyMode == gpmUseProxy )
		return true;

	// so we are in the autochoose mode: to render it quickly we need either overviews or image proxy
	// first check overviews
	if(GdalHelper::HasOverviews(filename))
		return false;

	bool tempGridNeeded = grid == NULL;

	// then if there is a proxy
	bool hasProxy = GridManager::HasValidProxy(filename);
	if (hasProxy) return true;

	// so, we have none of the two; if grid is small - nevermind - open directly
	if (m_globalSettings.MaxDirectGridSizeMb > 0)	
	{
		long size = Utility::GetFileSize(filename) / (0x1 << 20);
		if( size < m_globalSettings.MaxDirectGridSizeMb)
			return false;
	}

	bool canBuildOverviews = m_globalSettings.rasterOverviewCreation != rocNo;
	if (!GdalHelper::SupportsOverviews(filename, NULL))
		canBuildOverviews = false;
	return !canBuildOverviews;
}

// ****************************************************************** 
//		GetProxyLegendName
// ****************************************************************** 
CStringW GridManager::GetProxyLegendName(CStringW filename)
{
	tkGridProxyFormat format = m_globalSettings.gridProxyFormat;
	switch(format) {
		case gpfTiffProxy:
			return Utility::GetPathWOExtension(filename) + L"_proxy.tif.mwleg";
		case gpfBmpProxy:
		default:
			return Utility::GetPathWOExtension(filename) + L"_proxy.bmp.mwleg";
	}
}

// ****************************************************************** 
//		GetProxyName
// ****************************************************************** 
CStringW GridManager::GetProxyName(CStringW filename)
{
	tkGridProxyFormat format = m_globalSettings.gridProxyFormat;
	switch(format) {
		case gpfTiffProxy:
			return Utility::GetPathWOExtension(filename) + L"_proxy.tif";
		case gpfBmpProxy:
		default:
			return Utility::GetPathWOExtension(filename) + L"_proxy.bmp";
	}
}

// ****************************************************************** 
//		GetProxyWorldFileName
// ****************************************************************** 
CStringW GridManager::GetProxyWorldFileName(CStringW filename)
{
	tkGridProxyFormat format = m_globalSettings.gridProxyFormat;
	switch(format) {
		case gpfTiffProxy:
			return L"";
		case gpfBmpProxy:
		default:
			return Utility::GetPathWOExtension(filename) + L"_proxy.bpw";
	}
}

// ****************************************************************** 
//		GetOverviewsFilename
// ****************************************************************** 
CStringW GridManager::GetOverviewsFilename(CStringW filename)
{
	tkGridProxyFormat format = m_globalSettings.gridProxyFormat;
	switch(format) {
		case gpfTiffProxy:
			return Utility::GetPathWOExtension(filename) + L"_proxy.tif.ovr";
		case gpfBmpProxy:
		default:
			return L"";
	}
}

// ****************************************************************** 
//		HasValidProxy
// ****************************************************************** 
bool GridManager::HasValidProxy(CStringW gridFilename)
{
	CStringW proxyName = GetProxyName(gridFilename);
	CStringW legendName = GetProxyLegendName(gridFilename);
	return (Utility::FileExistsW(proxyName) && Utility::IsFileYounger(proxyName, gridFilename) &&
		    Utility::FileExistsW(legendName) && Utility::IsFileYounger(legendName, gridFilename));
}

// ****************************************************************** 
//		RemoveImageProxy
// ****************************************************************** 
bool GridManager::RemoveImageProxy(CStringW gridFilename)
{
	Utility::RemoveFile(GetProxyName(gridFilename));
	Utility::RemoveFile(GetProxyLegendName(gridFilename));
	Utility::RemoveFile(GetProxyWorldFileName(gridFilename));
	Utility::RemoveFile(GetOverviewsFilename(gridFilename));
	return true;
}