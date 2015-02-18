#include "stdafx.h"
#include "dGrid.h"
#include "EsriDll.h"
#include "gioapi.h"
#include "stc123.h"
#include <iomanip>

extern ESRI_PUTWINDOWROW_PROC putwindowrow;
extern ESRI_CELLLYRCLOSE_PROC celllyrclose;
extern ESRI_GRIDCOPY_PROC gridcopy;
extern ESRI_CELLLAYEROPEN_PROC celllayeropen;
extern ESRI_BNDCELLREAD_PROC bndcellread;
extern ESRI_PRIVATEACCESSWINDOWSET_PROC privateaccesswindowset;
extern ESRI_GETMISSINGFLOAT_PROC getmissingfloat;
extern ESRI_PRIVATEWINDOWCOLS_PROC privatewindowcols;
extern ESRI_PRIVATEWINDOWROWS_PROC privatewindowrows;
extern ESRI_GETWINDOWROW_PROC getwindowrow;
extern ESRI_CELLLYREXISTS_PROC celllyrexists;
extern ESRI_GRIDDELETE_PROC griddelete;
extern ESRI_CELLLAYERCREATE_PROC celllayercreate;

dGrid::dGrid()
{	data = NULL;
	isInRam = true;
	findNewMax = false;
	findNewMin = false;

	//ASCII
	xllcorner = false;
	yllcorner = false;

	//Disk-Based
	current_row = -1;

	//BINARY
	row_one = NULL;
	row_two = NULL;
	row_three = NULL;
	file_in_out = NULL;

	//ESRI
	grid_layer = -1;	
	row_buf1 = NULL;	
	row_buf2 = NULL;
	row_buf3 = NULL;	
	
	leadid = 0;
	initialize_esri();


	lastErrorCode = tkNO_ERROR;
}

dGrid::~dGrid()
{	
	close();

	shutdown_esri();
}

long dGrid::LastErrorCode()
{	long tmpec = lastErrorCode;
	lastErrorCode = tkNO_ERROR;
	return tmpec;
}
		
//OPERATORS
double dGrid::operator()( int Column, int Row )
{	
	if( inGrid( Column, Row ) )
	{
		if( isInRam )
			return data[Row][Column];			
		else
			return getValueDisk( Column, Row );			
	}
	else
		return gridHeader.getNodataValue();
}

//FUNCTIONS
bool dGrid::open( const char * cfilename, bool InRam, GRID_TYPE GridType, void (*callback)( int number, const char * message ) )
{	
	CString filename = cfilename;

	if( data != NULL || file_in_out != NULL )
		close();

	isInRam = InRam;
	
	if( filename.GetLength() <= 0 )
	{	lastErrorCode = tkINVALID_FILENAME;
		return false;
	}
	else
		gridFilename = filename;

	gridType = GridType;
	if( gridType == USE_EXTENSION )
		gridType = getGridType( filename );
	
	if( isInRam == true )
		return readDiskToMemory( callback );				
	else
	{	//1. Read the header information
		//2. Find the min and max
		//3. Leave the file open
		return readDiskToDisk( callback );
	}
}

bool dGrid::readDiskToMemory( void (*callback)(int number, const char * message ) )
{	
	if( gridType == ASCII_GRID )
		return asciiReadDiskToMemory( callback );
	else if( gridType == BINARY_GRID )
		return binaryReadDiskToMemory( callback );
	else if( gridType == ESRI_GRID )
		return esriReadDiskToMemory( callback );
	else if( gridType == SDTS_GRID )
		return sdtsReadDiskToMemory( callback );
	lastErrorCode = tkINVALID_GRID_FILE_TYPE;
	return false;	
}

bool dGrid::readDiskToDisk( void (*callback)(int number, const char * message ) )
{
	if( gridType == ASCII_GRID )
		return asciiReadDiskToDisk( callback );
	else if( gridType == BINARY_GRID )
		return binaryReadDiskToDisk();
	else if( gridType == ESRI_GRID )
		return esriReadDiskToDisk();
	/*
	else if( gridType == SDTS_GRID )
		return sdtsReadDiskToDisk( callback );
	*/
	lastErrorCode = tkINVALID_GRID_FILE_TYPE;
	return false;
}	

bool dGrid::writeMemoryToDisk( void(*callback)(int number, const char * message ) )
{
	if( gridType == ASCII_GRID )
		return asciiWriteMemoryToDisk( callback );
	else if( gridType == BINARY_GRID )
		return binaryWriteMemoryToDisk( callback );	
	else if( gridType == ESRI_GRID )
		return esriWriteMemoryToDisk( callback );
	/*
	else if( gridType == SDTS_GRID )
		return sdtsWriteMemoryToDisk( callback );
	*/
	lastErrorCode = tkINVALID_GRID_FILE_TYPE;
	return false;	
}

bool dGrid::writeDiskToDisk()
{	
	if( gridType == ASCII_GRID )
		return asciiWriteDiskToDisk();
	else if( gridType == BINARY_GRID )
		return binaryWriteDiskToDisk();	
	else if( gridType == ESRI_GRID )
		return esriWriteDiskToDisk();
	/*
	else if( gridType == SDTS_GRID )
		return sdtsWriteDiskToDisk( callback );
	*/
	lastErrorCode = tkINVALID_GRID_FILE_TYPE;
	return false;	
}

bool dGrid::initialize( const char * cfilename, dHeader header, double initialValue, bool InRam, GRID_TYPE GridType )
{	
	CString filename = cfilename;
	close();

	isInRam = InRam;	
	gridFilename = filename;
	gridHeader = header;
	
	gridType = GridType;
	if( gridType == USE_EXTENSION )
		gridType = getGridType( filename );

	if( gridHeader.getNumberRows() <= 0 || gridHeader.getNumberCols() <= 0 )
	{	lastErrorCode = tkZERO_ROWS_OR_COLS;
		return false;
	}
	
	if (gridType == ASCII_GRID)
		isInRam = true; // Force inram true for ascii grids; no support for disk-based ascii grids.

	if( isInRam == true )
	{	
		if (data != NULL)
		{
			delete [] data;
			data = NULL;
		}
		data = new double*[ gridHeader.getNumberRows() ];
		for( int y = 0; y < gridHeader.getNumberRows(); y++ )
		{	data[y] = new double[gridHeader.getNumberCols()];
			for( int x = 0; x < gridHeader.getNumberCols(); x++ )
			{	data[y][x] = initialValue;
			}
		}

		min = initialValue;
		max = initialValue;

		return true;
	}
	else
	{	
		if( gridType == BINARY_GRID )
			return binaryInitializeDisk( initialValue );
		else if( gridType == ESRI_GRID )
		{	//make sure that the filename has a final '\' for esri grids
			if( gridFilename[gridFilename.GetLength() -1] != '\\' ||
				gridFilename[gridFilename.GetLength() -1] != '/')
					gridFilename += "\\";
			return esriInitializeDisk( initialValue );
		}
		/*
		else if( gridType == SDTS_GRID )
			return sdtsInitialize( callback );
		*/
		lastErrorCode = tkINVALID_GRID_FILE_TYPE;
	}
	return false;
}

#pragma optimize("", off)
bool dGrid::close()
{	
	if( isInRam == true )
	{	dealloc();
		return true;
	}
	else
	{	
		// bool result = writeDiskToDisk();

		// Chris Michaelis July 02 2003 - every time this is called,
		// it rewrites the header. Dan Ames and I don't see a reason for this,
		// seeing as all writes are done immediately when changes are made,
		// and it's touching the grid file to the current date, thus invalidating
		// our test by date to see if the image corresponding to the grid needs to
		// be updated.
		// We've opted to comment it out until we see a reason for its being here.
		// This change is made in sGrid, dGrid, fGrid, and lGrid.
		bool result = true;

		if( file_in_out != NULL )
		{	fclose( file_in_out );
			file_in_out = NULL;
		}
		if( row_one != NULL )
		{	delete [] row_one;
			row_one = NULL;
		}
		if( row_two != NULL )
		{	delete [] row_two;
			row_two = NULL;
		}
		if( row_three != NULL )
		{	delete [] row_three;
			row_three = NULL;
		}
		if( row_buf1 != NULL )
		{	CFree1((char *)row_buf1);
			row_buf1 = NULL;
		}
		if( row_buf2 != NULL )
		{	CFree1((char *)row_buf2);
			row_buf2 = NULL;
		}
		if( row_buf3 != NULL )
		{	CFree1((char *)row_buf3);
			row_buf3 = NULL;
		}
		if( grid_layer >= 0 )
		{	
			if( celllyrclose != NULL )
				celllyrclose(grid_layer);
			grid_layer = -1;
		}
		return result;
	}
}
#pragma optimize("", on)

bool dGrid::save( const char * cfilename, GRID_TYPE GridType, void (*callback)(int number, const char * message ) )
{	
	CString filename = cfilename;

	if( isInRam == true )
	{
		if( filename.GetLength() > 0 )
			gridFilename = filename;

		if( GridType == USE_EXTENSION )
			GridType = getGridType( gridFilename );
			
		// Save was successful.  Update my grid type to be the new grid type.
		gridType = GridType;

		//make sure that the filename has a final '\' for esri grids
		if( GridType == ESRI_GRID )
		{
			if( gridFilename[gridFilename.GetLength() -1] != '\\' &&
				gridFilename[gridFilename.GetLength() -1] != '/')

				gridFilename += "\\";
		}
		if ( writeMemoryToDisk( callback ) )
		{
			return true;
		}
	}
	else
	{	
		if( filename.GetLength() <= 0 )
			return writeDiskToDisk();		
		//Convert the Grid
		else
		{	if( GridType == USE_EXTENSION )
				GridType = getGridType( filename );

			//make sure that the filename has a final '\' for esri grids
			if( GridType == ESRI_GRID ){
				if( gridFilename[gridFilename.GetLength() -1] != '\\' ||
					gridFilename[gridFilename.GetLength() -1] != '/')
					
					gridFilename += "\\";
			}
		
			if( GridType == ASCII_GRID )
			{
				if ( asciiSaveAs( filename, callback ) )
				{	
					// Save was successful.  Update my grid type to be the new grid type.
					gridType = GridType;
					return true;
				}
			}
			else if( GridType == BINARY_GRID )
			{
				if ( binarySaveAs( filename, callback ) )
				{	
					// Save was successful.  Update my grid type to be the new grid type.
					gridType = GridType;	
					return true;
				}
			}
			else if( GridType == ESRI_GRID )
			{
				if ( esriSaveAs( filename, callback ) )
				{	
					// Save was successful.  Update my grid type to be the new grid type.
					gridType = GridType;
					return true;
				}
			}
			lastErrorCode = tkINVALID_GRID_FILE_TYPE;
		}
	}
	return false;
}

inline double dGrid::getValue( int Column, int Row )
{
	if( inGrid( Column ,Row ) )
	{
		if( isInRam )
			return data[Row][Column];			
		else
			return getValueDisk( Column, Row );			
	}
	else
	{	lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return gridHeader.getNodataValue();
	}
}

inline double dGrid::getValueDisk( int Column, int Row )
{	if( gridType == ASCII_GRID )
		return gridHeader.getNodataValue();
	else if( gridType == BINARY_GRID )
		return binaryGetValueDisk( Column, Row );
	else if( gridType == ESRI_GRID )
		return esriGetValueDisk( Column, Row );
	else if( gridType == SDTS_GRID )
		return gridHeader.getNodataValue();
	
	return gridHeader.getNodataValue();
}

void dGrid::setValue( int Column, int Row, double Value )
{		
	if( inGrid( Column, Row ) )
	{	
		if( max == gridHeader.getNodataValue() )
			max = Value;
		else if( Value > max && Value != gridHeader.getNodataValue() )
			max = Value;
		else if( getValue( Column, Row ) == max )
			findNewMax = true;

		if( min == gridHeader.getNodataValue() )
			min = Value;
		else if( Value < min && Value != gridHeader.getNodataValue() )
			min = Value;
		else if( getValue( Column, Row ) == min )
			findNewMin = true;

		if( isInRam )
			data[Row][Column] = Value;
		else
			setValueDisk( Column, Row, Value );		
	}
	else
		lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
}

inline void dGrid::setValueDisk( int Column, int Row, double Value )
{	if( gridType == ASCII_GRID )
		return;
	else if( gridType == BINARY_GRID )
		binarySetValueDisk( Column, Row, Value );
	else if( gridType == ESRI_GRID )
		esriSetValueDisk( Column, Row, Value );
	else if( gridType == SDTS_GRID )
		return;	
}

void dGrid::dealloc()
{	if( isInRam && data != NULL )
	{	for( int y = 0; y < gridHeader.getNumberRows(); y++ )
			delete [] data[y];		
		delete [] data;				
		data = NULL;
	}
	gridHeader.setNumberRows( 0 );
	gridHeader.setNumberCols( 0 );
	min = gridHeader.getNodataValue();
	max = gridHeader.getNodataValue();
}

void dGrid::alloc()
{	if( isInRam )
	{
		if( gridHeader.getNumberCols() > 0 && gridHeader.getNumberRows() > 0 )
		{
			if (data != NULL)
			{
				delete [] data;
				data = NULL;
			}
			data = new double *[gridHeader.getNumberRows()];
			for( int y = 0; y < gridHeader.getNumberRows(); y++ )
				data[y] = new double[gridHeader.getNumberCols()];		
		}		
	}
}

//DATA MEMBER ACCESS
dHeader dGrid::getHeader()
{	return gridHeader;
}

void dGrid::setHeader( dHeader h )
{	
	//Don't allow the Rows and Columns to Change
	gridHeader.setDx( h.getDx() );
	gridHeader.setDy( h.getDy() );
	gridHeader.setNodataValue( h.getNodataValue() );
	gridHeader.setNotes( h.getNotes() );
	gridHeader.setProjection( h.getProjection() );
	gridHeader.setXllcenter( h.getXllcenter() );
	gridHeader.setYllcenter( h.getYllcenter() );
}


//MAPPING FUNCTIONS
void dGrid::ProjToCell( double x, double y, long & column, long & row )
{	if( gridHeader.getDx() != 0.0 && gridHeader.getDy() != 0.0 )
	{	column = round( ( x - gridHeader.getXllcenter() )/gridHeader.getDx() );
		row = gridHeader.getNumberRows() - round( ( y - gridHeader.getYllcenter() )/gridHeader.getDy() ) - 1;
	}
}

void dGrid::CellToProj( long column, long row, double & x, double & y )
{	x = gridHeader.getXllcenter() + column*gridHeader.getDx();
	y = gridHeader.getYllcenter() + ( ( gridHeader.getNumberRows() - row - 1)*gridHeader.getDy() );
}

inline int dGrid::round( double d )
{	if( ceil(d) - d <= .5 )
		return (int)ceil(d);
	else
		return (int)floor(d);
}

void dGrid::clear(double clearValue)
{
	if( isInRam )
	{
		int ncols = gridHeader.getNumberCols();
		int nrows= gridHeader.getNumberRows();
		for( int j = 0; j < nrows; j++ )
		{	for( int i = 0; i < ncols; i++ )
				data[j][i] = clearValue;					
		}
	}
	else
	{	clearDisk(clearValue);
	}
}

void dGrid::clearDisk(double clearValue)
{	if( gridType == ASCII_GRID )
		return;
	else if( gridType == BINARY_GRID )
		binaryClearDisk(clearValue);
	else if( gridType == ESRI_GRID )
		esriClearDisk(clearValue);
	/*
	else if( gridType == SDTS_GRID )
		return sdtsInitialize( callback );
	*/
}

inline bool dGrid::inGrid( long column, long row )
{
	if( column < 0 || column >= gridHeader.getNumberCols() )
		return false;
	if( row < 0 || row >= gridHeader.getNumberRows() )
		return false;

	return true;
}

double dGrid::maximum()
{	//Darrel Brown 10/10/2003
	// I changed this code so that the min/max are both calculated
	// at the same time, the first time anyone requests either.
	// This is a result of me changing the open code for disk-
	// based grids.
	/*if( findNewMax )
	{	double nodata_value = gridHeader.getNodataValue();
		max = nodata_value;
		
		for( int j = 0; j < gridHeader.getNumberRows(); j++ )
		{
			for( int i = 0; i < gridHeader.getNumberCols(); i++ )
			{	double val = getValue( i, j );
				if( max == nodata_value )
					max = val;
				else if( val > max && val != nodata_value )
					max = val;
			}
		}
		findNewMax = false;
	}*/
	if( findNewMin || findNewMax )
	{	double nodata_value = gridHeader.getNodataValue();
		min = nodata_value;
		max = nodata_value;

		for( int j = 0; j < gridHeader.getNumberRows(); j++ )
		{
			for( int i = 0; i < gridHeader.getNumberCols(); i++ )
			{	double val = getValue( i, j );
				// find both min and max at the same time	
				if( min == nodata_value )
					min = val;
				else if( val < min && val != nodata_value )
					min = val;

				if( max == nodata_value )
					max = val;
				else if( val > max && val != nodata_value )
					max = val;
			}
		}
		findNewMax = false;
		findNewMin = false;
	}
	return max;
}

double dGrid::minimum()
{	if( findNewMin || findNewMax )
	{	double nodata_value = gridHeader.getNodataValue();
		min = nodata_value;
		max = nodata_value;

		for( int j = 0; j < gridHeader.getNumberRows(); j++ )
		{
			for( int i = 0; i < gridHeader.getNumberCols(); i++ )
			{	double val = getValue( i, j );
				// find both min and max at the same time	
				if( min == nodata_value )
					min = val;
				else if( val < min && val != nodata_value )
					min = val;

				if( max == nodata_value )
					max = val;
				else if( val > max && val != nodata_value )
					max = val;
			}
		}
		findNewMax = false;
		findNewMin = false;
	}
	return min;
}

bool dGrid::inRam()
{	return isInRam;
}

double ** dGrid::getArrayPtr()
{	return data;
}

GRID_TYPE dGrid::getGridType( const char * filename )
{	
	GRID_TYPE grid_type = INVALID_GRID_TYPE;

	if( filename != NULL && _tcslen( filename ) > 0 )
	{	
		char * clean_filename = new char[_tcslen( filename ) + 1];
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
				if( extension.CompareNoCase("ASC") == 0 || extension.CompareNoCase("ARC") == 0)
					grid_type = ASCII_GRID;
				else if( extension.CompareNoCase("BGD") == 0 )
					grid_type = BINARY_GRID;
				else if( extension.CompareNoCase("DDF") == 0 )
					grid_type = SDTS_GRID;
			}
			cff.Close();
			delete [] clean_filename;
			return grid_type;
		}
		cff.Close();

		//File does not exist so parse it out
		int length = _tcslen(filename );
		bool foundPeriod = false;
		for( int e = length-1; e >= 0; e-- )
		{	if( filename[e] == '\\' || filename[e] == '/' )
			{
				delete [] clean_filename;
				return ESRI_GRID;
			}
			else if( filename[e] == '.' )
			{
				foundPeriod = true;
				break;
			}
		}

		if ( foundPeriod == false ) 
		{
			delete [] clean_filename;
			return ESRI_GRID;
		}
		
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

			if( !strcmp( ext, "ASC" ) )
			{
				delete [] clean_filename;
				return ASCII_GRID;
			}
			else if( !strcmp( ext, "ARC" ) )
			{
				delete [] clean_filename;
				return ASCII_GRID;
			}
			else if( !strcmp( ext, "BGD" ) )
			{
				delete [] clean_filename;
				return BINARY_GRID;
			}
			else if( !strcmp( ext, "DDF" ) )
			{
				delete [] clean_filename;
				return SDTS_GRID;
			}
		}
	}
	
	return grid_type;
}


///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//			ASCII GRID FUNCTIONS
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	bool dGrid::asciiReadDiskToMemory( void (*callback)(int number, const char * message ) )
	{	
		ifstream in(gridFilename);

		if( !in )
		{	lastErrorCode = tkCANT_OPEN_FILE;
			return false;
		}
		else
		{	asciiReadHeader( in );
		
			int percent = 0;
			long num_read = 0;
			double total = gridHeader.getNumberRows()*gridHeader.getNumberCols();
			double nodata = gridHeader.getNodataValue();

			min = nodata;
			max = nodata;

			if( gridHeader.getNumberRows() < 0 || gridHeader.getNumberCols() < 0 || 
				gridHeader.getDx() < 0 || gridHeader.getDy() < 0 )
				{	lastErrorCode = tkZERO_ROWS_OR_COLS;
					return false;
				}
			else
			{	if( callback != NULL )
					callback( 0, "Allocating and Initializing Memory" );

				alloc();
			}
						
			for( int j = 0; j < gridHeader.getNumberRows(); j++ )
			{	for( int i = 0; i < gridHeader.getNumberCols(); i++ )
				{	num_read++;
					if( !in )
					{	dealloc();
						return false;						
					}

					in>>data[j][i];				
					if( min == nodata )
					{	min = data[j][i];
						max = data[j][i];
					}
					else
					{	if( data[j][i] != nodata )
						{
							if( data[j][i] < min )
								min = data[j][i];
							else if( data[j][i] > max )
								max = data[j][i];
						}
					}
					
					if( callback != NULL )
					{	int newpercent = (int)(((num_read)/total)*100);
						if( newpercent > percent )
						{	percent = newpercent;
							callback( percent, "Reading Ascii Grid" );											
						}
					}				
				}
			}
			asciiReadFooter( in );
			in.close();
			return true;
		}		
	}

	bool dGrid::asciiReadDiskToDisk( void(*callback)(int number, const char * message ) )
	{	isInRam = true;
		return asciiReadDiskToMemory( callback );		
	}

	bool dGrid::asciiWriteMemoryToDisk( void(*callback)(int number, const char * message ) )
	{
		ofstream out( gridFilename );

		if( !out )
		{	lastErrorCode = tkCANT_CREATE_FILE;
			return false;
		}
		else
		{	
			asciiWriteHeader(out);
			double total = gridHeader.getNumberRows()*gridHeader.getNumberCols();
			int percent = 0;
			long num_written = 0;

			for( int j = 0; j < gridHeader.getNumberRows(); j++ )
			{	for( int i = 0; i < gridHeader.getNumberCols(); i++ )
				{	num_written++;
					
					out<<data[j][i]<<" ";
					
					if( callback != NULL )
					{
						int newpercent = (int)((num_written/total)*100);
						if( newpercent > percent )
						{	percent = newpercent;
							callback( percent, "Writing Ascii Grid" );
						}						
					}
				}
				out<<endl;
			}
			asciiWriteFooter(out);
			out.close();
			return true;			
		}		
	}

	bool dGrid::asciiWriteDiskToDisk()
	{	return false;
	}

	void dGrid::asciiReadHeader( istream & in )
	{
		char * header_value = new char[MAX_STRING_LENGTH];
		in>>header_value;

		streampos filePosition = in.tellg();
		while( asciiIsHeaderValue( (TCHAR*)header_value, in ) )
		{	filePosition = in.tellg();
			in>>header_value;
		}
		
		//Read past a header value, so put it back
		in.seekg( filePosition, ios::beg );

		if( xllcorner )
		{	gridHeader.setXllcenter( gridHeader.getXllcenter() + gridHeader.getDx()/2 );
			xllcorner = false;
		}
		if( yllcorner )
		{	gridHeader.setYllcenter( gridHeader.getYllcenter() + gridHeader.getDy()/2 );
			yllcorner = false;
		}

		delete [] header_value;
	}

	void dGrid::asciiReadFooter( istream & in )
	{	char * header_value = new char[MAX_STRING_LENGTH];
		in>>header_value;
		while( in )
		{	asciiIsHeaderValue( (TCHAR*)header_value, in );
			in>>header_value;	
		}

		delete [] header_value;
	}

	void dGrid::asciiWriteHeader( ostream & out )
	{	out<<"NCOLS "<<gridHeader.getNumberCols()<<endl;
		out<<"NROWS "<<gridHeader.getNumberRows()<<endl;
	
		out.setf(ios::fixed);
		out<<"XLLCENTER "<<(double)gridHeader.getXllcenter()<<endl;
		out<<"YLLCENTER "<<(double)gridHeader.getYllcenter()<<endl;
		out.unsetf(ios::fixed);
	
		//dx and dy should be the same
		// Chris M May 2006 -- Not necessarily the same now; take converted
		// surfer grids for instance, these are rarely the same.
		// out<<"CELLSIZE "<<(double)gridHeader.getDx()<<endl;
		out << "DX "<< (double)gridHeader.getDx() << endl;
		out << "DY "<< (double)gridHeader.getDy() << endl;

		out<<"NODATA_VALUE "<<gridHeader.getNodataValue()<<endl;
	}

	void dGrid::asciiWriteFooter( ostream & out )
	{	if( gridHeader.getProjection() != NULL )
			out<<"PROJECTION "<<gridHeader.getProjection()<<endl;
		if( gridHeader.getNotes() != NULL )
			out<<"NOTES "<<gridHeader.getNotes()<<endl;
	}

	bool dGrid::asciiIsHeaderValue( CString headerValue, istream & in )
	{
		if( headerValue.GetLength() <= 0 )
			return false;

		if( headerValue.CompareNoCase( "NCOLS" ) == 0 )
		{	long ncols;
			in>>ncols;
			gridHeader.setNumberCols( ncols );
			return true;
		}
		else if( headerValue.CompareNoCase( "NROWS" ) == 0 )
		{	long nrows;
			in>>nrows;
			gridHeader.setNumberRows( nrows );
			return true;
		}
		else if( headerValue.CompareNoCase( "XLLCENTER" ) == 0 )
		{	xllcorner = false;
			double xllcenter;
			in>>xllcenter;
			gridHeader.setXllcenter( xllcenter );
			return true;
		}
		else if( headerValue.CompareNoCase( "XLL" ) == 0 )
		{	xllcorner = false;
			double xllcenter;
			in>>xllcenter;
			gridHeader.setXllcenter( xllcenter );
			return true;
		}
		else if( headerValue.CompareNoCase( "XLLCORNER" ) == 0 )
		{	//Set Flag
			xllcorner = true;
			double xllcenter;
			in>>xllcenter;
			gridHeader.setXllcenter( xllcenter );
			return true;
		}
		else if( headerValue.CompareNoCase( "YLLCENTER" ) == 0 )
		{	yllcorner = false;
			double yllcenter;
			in>>yllcenter;
			gridHeader.setYllcenter( yllcenter );
			return true;
		}
		else if( headerValue.CompareNoCase( "YLL" ) == 0 )
		{	yllcorner = false;
			double yllcenter;
			in>>yllcenter;
			gridHeader.setYllcenter( yllcenter );
			return true;
		}
		else if( headerValue.CompareNoCase( "YLLCORNER" ) == 0 )
		{	//Set Flag
			yllcorner = true;
			double yllcenter;
			in>>yllcenter;		
			gridHeader.setYllcenter( yllcenter );
			return true;
		}
		else if( headerValue.CompareNoCase( "CELLSIZE" ) == 0 )
		{	double csize;
			in>>csize;
			gridHeader.setDy( csize );
			gridHeader.setDx( csize );
			return true;
		}
		else if( headerValue.CompareNoCase( "NODATA_VALUE" ) == 0 )
		{	double nodata_value;
			in>>nodata_value;
			gridHeader.setNodataValue( nodata_value );
			return true;
		}
		else if( headerValue.CompareNoCase( "DX" ) == 0 )
		{	double dx;
			in>>dx;
			gridHeader.setDx( dx );
			return true;
		}
		else if( headerValue.CompareNoCase( "DY" ) == 0 )
		{	double dy;			
			in>>dy;
			gridHeader.setDy( dy );
			return true;
		}
		else if( headerValue.CompareNoCase( "PROJECTION" ) == 0 )
		{	char next;
			while( in.peek() == ' ' || in.peek() == '\t' )
				in.get(next);

			char * projection = new char[MAX_STRING_LENGTH + 1];
			in.getline( projection, MAX_STRING_LENGTH );
			gridHeader.setProjection( (TCHAR*)projection );

			delete [] projection;
			return true;
		}
		else if( headerValue.CompareNoCase( "NOTES" ) == 0 )
		{	char next;
			while( in.peek() == ' ' || in.peek() == '\t' )
				in.get(next);

			char * notes = new char[MAX_STRING_LENGTH];
			in.getline( notes, MAX_STRING_LENGTH );
			gridHeader.setNotes( (TCHAR*)notes );

			delete [] notes;
			return true;
		}
		return false;
	}

	bool dGrid::asciiSaveAs( CString filename, void(*callback)(int number, const char * message) )
	{
		ofstream out( filename );

		if( !out )
		{	lastErrorCode = tkCANT_CREATE_FILE;
			return false;
		}
		else
		{	
			asciiWriteHeader(out);
			double total = gridHeader.getNumberRows()*gridHeader.getNumberCols();
			int percent = 0;
			long num_written = 0;

			for( int j = 0; j < gridHeader.getNumberRows(); j++ )
			{	for( int i = 0; i < gridHeader.getNumberCols(); i++ )
				{	num_written++;
					
					out<<getValue( i, j )<<" ";
					
					if( callback != NULL )
					{
						int newpercent = (int)((num_written/total)*100);
						if( newpercent > percent )
						{	percent = newpercent;
							callback( percent, "Writing Ascii Grid" );
						}						
					}
				}
				out<<endl;
			}
			asciiWriteFooter(out);
			out.close();

			if( isInRam == true )
				return true;			
			else
			{	close();				
				return open( filename, true, ASCII_GRID, callback );
			}
		}		
	}

///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//			ASCII GRID FUNCTIONS
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//			BINARY GRID FUNCTIONS
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	bool dGrid::binaryReadDiskToMemory( void (*callback)(int number, const char * message ) )
	{	FILE * in = fopen( gridFilename, "rb" );
		
		if( !in )
		{	lastErrorCode = tkCANT_OPEN_FILE;
			return false;
		}
		else
		{	binaryReadHeader( in );
			double total = gridHeader.getNumberRows()* gridHeader.getNumberCols();
			double nodata = gridHeader.getNodataValue();
			min = nodata;
			max = nodata;

			if( gridHeader.getNumberCols() < 0 || gridHeader.getNumberRows() < 0 || 
				gridHeader.getDx() < 0 || gridHeader.getDy() < 0 ||
				data_type != DOUBLE_TYPE )
				{	dealloc();
					fclose( in );
					lastErrorCode = tkINCOMPATIBLE_DATA_TYPE;
					return false;
				}
			
			alloc();

			int percent = 0;
			long num_read = 0;
						
			for( int j = 0; j < gridHeader.getNumberRows(); j++ )
			{	for( int i = 0; i < gridHeader.getNumberCols(); i++ )
				{	num_read++;
					if( feof(in) )
					{	dealloc();
						fclose( in );
						return false;
					}

					fread( &data[j][i],sizeof(double),1,in );
								
					if( min == nodata )
					{	min = data[j][i];
						max = data[j][i];
					}
					else
					{	if( data[j][i] != nodata )
						{
							if( data[j][i] < min )
								min = data[j][i];
							else if( data[j][i] > max )
								max = data[j][i];
						}
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

	bool dGrid::binaryReadDiskToDisk()
	{	
		file_in_out = fopen( gridFilename, "r+b" );
		
		if( !file_in_out )
		{	lastErrorCode = tkCANT_CREATE_FILE;
			return false;
		}
		else
		{	
			binaryReadHeader(file_in_out);
			
			if( gridHeader.getNumberCols() < 0 || gridHeader.getNumberRows() < 0 || 
				gridHeader.getDx() < 0 || gridHeader.getDy() < 0 ||
				data_type != DOUBLE_TYPE )
				{	dealloc();
					fclose( file_in_out );
					file_in_out = NULL;
					lastErrorCode = tkINCOMPATIBLE_DATA_TYPE;
					return false;
				}

			file_position_beg_of_data = ftell( file_in_out);


			//Darrel Brown, 10/10/2003
			//I removed this code to improve loading performance of disk
			// based binary grids.  Now the min/max are calculated when first
			// requested, not at load time.
			//
			//double value;
			//double nodata = gridHeader.getNodataValue();
			//min = nodata;
			//max = nodata;
			//for( int j = 0; j < gridHeader.getNumberRows(); j++ )
			//{	for( int i = 0; i < gridHeader.getNumberCols(); i++ )
			//	{	
			//		if( feof(file_in_out) )
			//		{	dealloc();
			//			fclose( file_in_out );
			//			file_in_out = NULL;
			//			lastErrorCode = tkINVALID_FILE;
			//			return false;
			//		}
			//				

			//		fread( &value,sizeof(double),1,file_in_out );

			//		if( min == nodata )
			//		{	min = value;
			//			max = value;
			//		}
			//		else
			//		{	if( value != nodata )
			//			{
			//				if( value < min )
			//					min = value;
			//				else if( value > max )
			//					max = value;
			//			}
			//		}
			//	}
			//}
			
			//Darrel Brown, 10/10/2003
			//set flags in the grid to make sure the min/max get calculated when asked for
			// This is done because the above code was commented
			findNewMax = findNewMin = true;

			if( gridHeader.getNumberCols() > 0 )
			{	
				row_one = new double[gridHeader.getNumberCols()];
				row_two = new double[gridHeader.getNumberCols()];
				row_three = new double[gridHeader.getNumberCols()];
					
				binaryBufferRows( 1 );				
			}				
			return true;		
		}		
	}

	bool dGrid::binaryWriteMemoryToDisk( void(*callback)(int number, const char * message ) )
	{	
		FILE * out = fopen( gridFilename, "wb" );
		
		if( !out )
		{	lastErrorCode = tkCANT_CREATE_FILE;
			return false;
		}
		else
		{	
			binaryWriteHeader(out);
			double total = gridHeader.getNumberRows() * gridHeader.getNumberCols();
			int percent = 0;
			long num_written = 0;

			for( int j = 0; j < gridHeader.getNumberRows(); j++ )
			{	for( int i = 0; i < gridHeader.getNumberCols(); i++ )
				{	num_written++;
					
					fwrite( &data[j][i],sizeof(double),1,out);

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
			fclose( out );
			return true;			
		}		
	}

	bool dGrid::binaryWriteDiskToDisk()
	{
		if( !file_in_out )
			return false;
		else
		{	rewind( file_in_out );
			binaryWriteHeader(file_in_out);
			return true;
		}	
	
	}

	bool dGrid::binaryInitializeDisk( double initialValue )
	{			
		file_in_out = fopen( gridFilename, "w+b" );

		if( !file_in_out )
		{	lastErrorCode = tkCANT_CREATE_FILE;
			return false;
		}
		else
		{				
			binaryWriteHeader(file_in_out);
			file_position_beg_of_data = ftell( file_in_out);
			
			for( int j = gridHeader.getNumberRows() - 1; j >= 0; j-- )
			{	for( int i = 0; i < gridHeader.getNumberCols(); i++ )
				{	
					fwrite( &initialValue,sizeof(double),1,file_in_out);																						
				}				
			}
			
			min = initialValue;
			max = initialValue;
			
			current_row = 0;
			row_one = new double[gridHeader.getNumberCols()];
			row_two = new double[gridHeader.getNumberCols()];
			row_three = new double[gridHeader.getNumberCols()];

			for( int i = 0; i < gridHeader.getNumberCols(); i++ )
			{	row_one[i] = initialValue;
				row_two[i] = initialValue;
				row_three[i] = initialValue;
			}
		}
		return true;		
	}

	void dGrid::binaryReadHeader( FILE * in )
	{	
		rewind(in);
		long ncols;
		fread( &ncols, sizeof(int),1,in);
		gridHeader.setNumberCols( ncols );

		long nrows;
		fread( &nrows, sizeof(int),1,in);
		gridHeader.setNumberRows( nrows );

		double dx;
		fread( &dx, sizeof(double),1,in);
		gridHeader.setDx( dx );

		double dy;
		fread( &dy, sizeof(double),1,in);
		gridHeader.setDy( dy );

		double xllcenter;
		fread( &xllcenter, sizeof(double),1,in);
		gridHeader.setXllcenter( xllcenter );

		double yllcenter;
		fread( &yllcenter, sizeof(double),1,in);
		gridHeader.setYllcenter( yllcenter );

		fread( &data_type, sizeof(DATA_TYPE),1,in);
		
		double nodata_value;
		fread( &nodata_value, sizeof(double),1,in);		
		gridHeader.setNodataValue( nodata_value );

		char * projection = new char[MAX_STRING_LENGTH + 1];
		fread( projection, sizeof(char), MAX_STRING_LENGTH,in);
		gridHeader.setProjection( (TCHAR*)projection );
		delete [] projection;

		char * notes = new char[MAX_STRING_LENGTH + 1];
		fread( notes, sizeof(char), MAX_STRING_LENGTH, in);
		gridHeader.setNotes( (TCHAR*)notes );
		delete [] notes;
	}

	void dGrid::binaryWriteHeader( FILE * out )
	{
		rewind(out);
		long ncols = gridHeader.getNumberCols();
		fwrite( &ncols, sizeof(int),1,out);
		long nrows = gridHeader.getNumberRows();
		fwrite( &nrows, sizeof(int),1,out);
		double dx = gridHeader.getDx();
		fwrite( &dx, sizeof(double),1,out);
		double dy = gridHeader.getDy();
		fwrite( &dy, sizeof(double),1,out);
		double xllcenter = gridHeader.getXllcenter();
		fwrite( &xllcenter, sizeof(double),1,out);
		double yllcenter = gridHeader.getYllcenter();
		fwrite( &yllcenter, sizeof(double),1,out);
		DATA_TYPE type = DOUBLE_TYPE;
		fwrite( &type, sizeof(DATA_TYPE),1,out);
		double nodata = gridHeader.getNodataValue();
		fwrite( &nodata, sizeof(double),1,out);
		
		char * projection = new char[MAX_STRING_LENGTH + 1];
		strcpy( projection, gridHeader.getProjection() );
		if( _tcslen( projection ) > 0 )
			fwrite( projection, sizeof(char), _tcslen(projection),out);
		if( _tcslen( projection ) < MAX_STRING_LENGTH )
		{	int size_of_pad = MAX_STRING_LENGTH - _tcslen(projection);
			char * pad = new char[size_of_pad];
			for( int p = 0; p < size_of_pad; p++ )
				pad[p] = 0;
			fwrite(pad, sizeof(char), size_of_pad, out );
			delete [] pad;
		}
		delete [] projection;

		char * notes = new char[MAX_STRING_LENGTH + 1];
		strcpy( notes, gridHeader.getNotes() );
		if( _tcslen( notes ) > 0 )
			fwrite( notes, sizeof(char), _tcslen(notes), out);
		if( _tcslen( notes ) < MAX_STRING_LENGTH )
		{	int size_of_pad = MAX_STRING_LENGTH - _tcslen(notes);
			char * pad = new char[size_of_pad];
			for( int p = 0; p < size_of_pad; p++ )
				pad[p] = 0;
			fwrite(pad, sizeof(char), size_of_pad, out );
			delete [] pad;
		}	
		delete [] notes;
	}

	double dGrid::binaryGetValueDisk( int Column, int Row )
	{
		if( Row == current_row - 1 )
			return row_one[ Column ];
		else if( Row == current_row )
			return row_two[ Column ];
		else if( Row == current_row + 1 )
			return row_three[ Column ];
		else
		{	binaryBufferRows( Row );
			return row_two[ Column ];
		}	
	}

	void dGrid::binarySetValueDisk( int Column, int Row, double Value )
	{
		long file_position = file_position_beg_of_data + sizeof(double)*Row*gridHeader.getNumberCols() + sizeof(double)*Column;
		
		if( fseek( file_in_out, file_position, SEEK_SET ) == -1L )
		{}
		else
		{	
			if( fwrite( &Value, sizeof(double), 1, file_in_out ) < 1 )
			{}								
			else
			{	if( Row == current_row - 1 )
					row_one[ Column ] = Value;
				else if( Row == current_row )
					row_two[ Column ] = Value;
				else if( Row == current_row + 1 )
					row_three[ Column ] = Value;					
			}
		}		
	}
	
	void dGrid::binaryClearDisk(double clearValue)
	{
		if( fseek( file_in_out, file_position_beg_of_data, SEEK_SET ) == -1L )
			return;
		else
		{	
			for( int j = 0; j < gridHeader.getNumberRows(); j++ )
			{	for( int i = 0; i < gridHeader.getNumberCols(); i++ )
				{	if( fwrite( &clearValue, sizeof(double), 1, file_in_out ) < 1 )
						return;
				}
			}					
		}		
	}

	void dGrid::binaryBufferRows( int center_row )
	{
		bool nd_fill_one = false;
		bool nd_fill_two = false;
		bool nd_fill_three = false;

		if( gridHeader.getNumberRows() >= center_row - 1 && center_row - 1 >= 0 )
		{	long file_position = file_position_beg_of_data + sizeof(double)*(center_row-1)*gridHeader.getNumberCols();

			if( fseek( file_in_out, file_position, SEEK_SET ) != -1L )
				fread( row_one, sizeof(double), gridHeader.getNumberCols(), file_in_out );
			else
				nd_fill_one = true;
		}
		else
			nd_fill_one = true;
				
		if( gridHeader.getNumberRows() >= center_row && center_row >= 0 )
		{	long file_position = file_position_beg_of_data + sizeof(double)*(center_row)*gridHeader.getNumberCols();

			if( fseek( file_in_out, file_position, SEEK_SET ) != -1L )
				fread( row_two, sizeof(double), gridHeader.getNumberCols(), file_in_out );
			else
				nd_fill_two = true;
		}
		else
			nd_fill_two = true;

		if( gridHeader.getNumberRows() >= center_row + 1 && center_row + 1 >= 0 )
		{	long file_position = file_position_beg_of_data + sizeof(double)*(center_row+1)*gridHeader.getNumberCols();

			if( fseek( file_in_out, file_position, SEEK_SET ) != -1L )
				fread( row_three, sizeof(double), gridHeader.getNumberCols(), file_in_out );
			else
				nd_fill_three = true;
		}
		else
			nd_fill_three = true;
			
		current_row = center_row;

		if( nd_fill_one || nd_fill_two || nd_fill_three )
		{
			//Initialize to nodata
			for( int i = 0; i < gridHeader.getNumberCols(); i++ )
			{	if( nd_fill_one )
					row_one[i] = gridHeader.getNodataValue();
				if( nd_fill_two )
					row_two[i] = gridHeader.getNodataValue();
				if( nd_fill_three )
					row_three[i] = gridHeader.getNodataValue();	
			}	
		}
	}


	bool dGrid::binarySaveAs( CString filename, void(*callback)(int number, const char * message) )
	{	
		// Write to a temporary file first.
		char tempName[ FILENAME_MAX ] = {0};
		tmpnam(tempName);
		FILE * out = fopen( tempName, "wb" );
		
		if( !out )
			return false;

		binaryWriteHeader(out);

		double total = gridHeader.getNumberRows() * gridHeader.getNumberCols();
		int percent = 0;
		long num_written = 0;
		double value;

		for( int j = 0; j < gridHeader.getNumberRows(); j++ )
		{	for( int i = 0; i < gridHeader.getNumberCols(); i++ )
			{	num_written++;
				value = getValue( i, j );
				fwrite( &value,sizeof(double),1,out);

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
		fclose( out );

		if( isInRam == true )
		{
			if( !MoveFileEx(tempName, filename,
				MOVEFILE_REPLACE_EXISTING |
				MOVEFILE_COPY_ALLOWED) )
				return false;

			return true;
		}
		else
		{	close();

			if( !MoveFileEx(tempName, filename,
				MOVEFILE_REPLACE_EXISTING |
				MOVEFILE_COPY_ALLOWED) )
				return false;

			return open( filename, false, BINARY_GRID, callback );
		}
	}

///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//			BINARY GRID FUNCTIONS
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//			ESRI GRID FUNCTIONS
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	//ESRI GRID FUNCTIONS
	#pragma optimize("", off)
	bool dGrid::esriReadDiskToMemory( void (*callback)(int number, const char * message ) )
	{	
		if( celllayeropen == NULL ||
			celllyrclose == NULL ||
			bndcellread == NULL ||
			privateaccesswindowset == NULL ||
			privatewindowcols == NULL ||
			privatewindowrows == NULL ||
			getwindowrow == NULL ||
			getmissingfloat == NULL )
			{	grid_layer = -1;
				lastErrorCode = tkESRI_DLL_NOT_INITIALIZED;
				return false;
			}

		double csize;	
		double bndbox[4];
		double adjbndbox[4];
		double nodata = -1;
		int cell_type;
		
		char * fname = new char[_MAX_PATH+1];		
		if( GetShortPathName(gridFilename,fname,_MAX_PATH) == 0 )
			strcpy( fname, gridFilename );	
		grid_layer = celllayeropen(fname,READONLY,ROWIO,&cell_type,&csize);

		if( grid_layer >= 0 )
		{			
			//Get the bounding box of the input cell layer 
			//Bounding box is xllcorner, yllcorner, xurcorner, yurcorner
			if( bndcellread(fname,bndbox) < 0 )
			{	dealloc();
				//Close handle
				celllyrclose(grid_layer);
				grid_layer = -1;
				lastErrorCode = tkESRI_INVALID_BOUNDS;
				return false;
			}			
			
			//Need to find cell_type
			if( cell_type == CELLFLOAT )
			{	getmissingfloat(&float_null);	
				nodata = float_null;
			}
			if( cell_type == CELLINT )
			{	dealloc();
				//Close handle
				celllyrclose(grid_layer);
				grid_layer = -1;
				lastErrorCode = tkINVALID_GRID_FILE_TYPE;
				return false;				
			}
			gridHeader.setXllcenter( bndbox[0] + .5*csize );
			gridHeader.setYllcenter( bndbox[1] + .5*csize );
			gridHeader.setDx( csize );
			gridHeader.setDy( csize );
			gridHeader.setNodataValue( nodata );
				
			//Set the Window to the output bounding box and cellsize        
			if( privateaccesswindowset(grid_layer,bndbox,csize,adjbndbox) < 0)
			{	dealloc();
				//Close handle
				celllyrclose(grid_layer);
				grid_layer = -1;
				lastErrorCode = tkESRI_ACCESS_WINDOW_SET;
				return false;
			}
   
			//Get the number of rows and columns in the window
			gridHeader.setNumberCols( privatewindowcols(grid_layer) );
			gridHeader.setNumberRows( privatewindowrows(grid_layer) );

			if( gridHeader.getNumberCols() <= 0 || gridHeader.getNumberRows() <= 0 )
			{	dealloc();
				//Close handle
				celllyrclose(grid_layer);
				grid_layer = -1;
				lastErrorCode = tkZERO_ROWS_OR_COLS;
				return false;
			}

			//Create space for the data
			alloc();

			//Now copy row major into array
			//Allocate row buffer				
			row_buf1 = (CELLTYPE*)CAllocate1(gridHeader.getNumberCols() + 1, sizeof(CELLTYPE));
			if ( row_buf1 == NULL )
			{	dealloc();
				//Close handle
				celllyrclose(grid_layer);
				grid_layer = -1;
				lastErrorCode = tkCANT_ALLOC_MEMORY;
				return false;
			}

			int percent = 0;
			double total = gridHeader.getNumberRows();
			
			//Find the min and max
			double nodata = gridHeader.getNodataValue();
			min = nodata;
			max = nodata;
			for ( int j = 0; j < gridHeader.getNumberRows(); j++)
			{	
				getwindowrow(grid_layer, j, (CELLTYPE*)row_buf1);

				register float *buf = (float *)row_buf1;
				for( int i = 0; i < gridHeader.getNumberCols(); i++)
				{	
					if( buf[i] == float_null )
						data[j][i] = nodata;
					else
					{	data[j][i] = buf[i];
						
						if( min == nodata )
							min = buf[i];
						else if( buf[i] < min )
							min = buf[i];

						if( max == nodata )
							max = buf[i];
						else if( buf[i] > max )
							max = buf[i];							
					}
				}             
							 

				int newpercent = (int)((j/total)*100);
				if( newpercent > percent )
				{   percent = newpercent;
					if( callback != NULL )
						callback( percent, "Reading Esri Grid" );
				}

			}

			//Free row buffer
			CFree1((char *)row_buf1);
			row_buf1 = NULL;
  
			//Close handle
			celllyrclose(grid_layer);
			grid_layer = -1;
			return true;
		}							
		else
		{	dealloc();
			grid_layer = -1;
			lastErrorCode = tkESRI_LAYER_OPEN;
			return false;
		}			
	}
	#pragma optimize("", on)

	#pragma optimize("", off)
	bool dGrid::esriReadDiskToDisk()
	{	
		//Check for the needed functions
		if( bndcellread == NULL ||
			celllyrclose == NULL || 
			celllayeropen == NULL ||
			privateaccesswindowset == NULL ||
			privatewindowcols == NULL ||
			privatewindowrows == NULL ||
			getwindowrow == NULL ||
			getmissingfloat == NULL )
			{	grid_layer = -1;
				lastErrorCode = tkESRI_DLL_NOT_INITIALIZED;
				return false;
			}

		double csize;	
		double bndbox[4];
		double adjbndbox[4];
		double nodata = -1;
		int cell_type;

		char * fname = new char[_MAX_PATH+1];		
		if( GetShortPathName(gridFilename,fname,_MAX_PATH) == 0 )
			strcpy( fname, gridFilename );	
		grid_layer = celllayeropen(fname,READWRITE,ROWIO,&cell_type,&csize);

		if( grid_layer >= 0 )
		{					
			//Get the bounding box of the input cell layer 
			//Bounding box is xllcorner, yllcorner, xurcorner, yurcorner
			
			if( bndcellread(fname,bndbox) < 0 )
			{	dealloc();
				//Close handle
				celllyrclose(grid_layer);
				grid_layer = -1;
				lastErrorCode = tkESRI_INVALID_BOUNDS;
				delete [] fname;
				return false;
			}
			
			//Needed to find type
			if( cell_type == CELLFLOAT )
			{	getmissingfloat(&float_null);
				nodata = float_null;
			}
			if( cell_type == CELLINT )
			{	dealloc();
				//Close handle
				celllyrclose(grid_layer);
				grid_layer = -1;
				lastErrorCode = tkINVALID_GRID_FILE_TYPE;
				delete [] fname;
				return false;				
			}

			gridHeader.setXllcenter( bndbox[0] + .5*csize );
			gridHeader.setYllcenter( bndbox[1] + .5*csize );
			gridHeader.setDx( csize );
			gridHeader.setDy( csize );
			gridHeader.setNodataValue( nodata );
			
			//Set the Window to the output bounding box and cellsize        
			if( privateaccesswindowset(grid_layer,bndbox,csize,adjbndbox) < 0)
			{	dealloc();
				//Close handle
				celllyrclose(grid_layer);
				grid_layer = -1;
				lastErrorCode = tkESRI_ACCESS_WINDOW_SET;
				delete [] fname;
				return false;
			}

			//Get the number of rows and columns in the window
			gridHeader.setNumberCols( privatewindowcols(grid_layer) );
			gridHeader.setNumberRows( privatewindowrows(grid_layer) );

			if( gridHeader.getNumberCols() <= 0 || gridHeader.getNumberRows() <= 0 )
			{	dealloc();
				//Close handle
				celllyrclose(grid_layer);
				grid_layer = -1;
				lastErrorCode = tkZERO_ROWS_OR_COLS;
				delete [] fname;
				return false;
			}

			//Allocate row buffer				
			row_buf1 = (CELLTYPE*)CAllocate1(gridHeader.getNumberCols() + 1, sizeof(CELLTYPE));
			row_buf2 = (CELLTYPE*)CAllocate1(gridHeader.getNumberCols() + 1, sizeof(CELLTYPE));				
			row_buf3 = (CELLTYPE*)CAllocate1(gridHeader.getNumberCols() + 1, sizeof(CELLTYPE));				
			if ( row_buf1 == NULL || row_buf2 == NULL || row_buf3 == NULL )
			{	dealloc();
				//Close handle
				celllyrclose(grid_layer);
				grid_layer = -1;
				lastErrorCode = tkCANT_ALLOC_MEMORY;
				delete [] fname;
				return false;
			}
			
			//Find the min and max
			double nodata = gridHeader.getNodataValue();
			min = nodata;
			max = nodata;
			findNewMin = true;
			findNewMax = true;
			/*for ( int j = 0; j < gridHeader.getNumberRows(); j++)
			{	
				getwindowrow(grid_layer, j, (CELLTYPE*)row_buf1);

				register float *buf = (float *)row_buf1;
				for( int i = 0; i < gridHeader.getNumberCols(); i++)
				{	
					if( buf[i] != float_null )
					{	if( min == nodata )
							min = buf[i];
						else if( buf[i] < min )
							min = buf[i];

						if( max == nodata )
							max = buf[i];
						else if( buf[i] > max )
							max = buf[i];
					}
				}						
				 
			}				*/
			esriBufferRows( 0 );
			delete [] fname;
			return true;								
		}
		else
		{	dealloc();
			grid_layer = -1;
			lastErrorCode = tkESRI_LAYER_OPEN;
			delete [] fname;
			return false;
		}
	}
	#pragma optimize("", on)

	#pragma optimize("", off)
	bool dGrid::esriWriteMemoryToDisk( void(*callback)(int number, const char * message ) )
	{
		//Check the needed functions
		if( celllyrclose == NULL ||
			celllyrexists == NULL ||
			celllayercreate == NULL ||
			griddelete == NULL ||			
			privateaccesswindowset == NULL ||
			getmissingfloat == NULL ||
			putwindowrow == NULL )
			{	grid_layer = -1;
				lastErrorCode = tkESRI_DLL_NOT_INITIALIZED;
				return false;
			}

		int cell_type = CELLFLOAT;
		
		double csize = gridHeader.getDx();
		//Bounding box is xllcorner, yllcorner, xurcorner, yurcorner
		double bndbox[4];
		bndbox[0] = gridHeader.getXllcenter() - gridHeader.getDx()*.5;
		bndbox[1] = gridHeader.getYllcenter() - gridHeader.getDy()*.5;
		bndbox[2] = gridHeader.getXllcenter() + gridHeader.getNumberCols()*gridHeader.getDx() - gridHeader.getDx()*.5;
		bndbox[3] = gridHeader.getYllcenter() + gridHeader.getNumberRows()*gridHeader.getDy() - gridHeader.getDy()*.5;
		double adjbndbox[4];
	
		char * fname = new char[gridFilename.GetLength()+1];
		strcpy( fname, gridFilename );		
		if( celllyrexists( fname ) != 0 )
			griddelete( fname );
					
		grid_layer = celllayercreate( fname, WRITEONLY, ROWIO, cell_type, csize, bndbox);
		if( grid_layer < 0 )
		{	grid_layer = -1;
			lastErrorCode = tkESRI_LAYER_CREATE;
			return false;
		}

		if( privateaccesswindowset( grid_layer, bndbox, csize, adjbndbox) < 0 )
		{	celllyrclose(grid_layer);
			if( celllyrexists( fname ) )
				griddelete( fname );						
			grid_layer = -1;
			lastErrorCode = tkESRI_ACCESS_WINDOW_SET;
			return false;
		}
		
		getmissingfloat(&float_null);				

		//Allocate row buffer				
		row_buf1 = (CELLTYPE*)CAllocate1(gridHeader.getNumberCols() + 1, sizeof(CELLTYPE));
		if ( row_buf1 == NULL )
		{	celllyrclose(grid_layer);
			if( celllyrexists( fname ) )
				griddelete( fname );						
			grid_layer = -1;
			lastErrorCode = tkCANT_ALLOC_MEMORY;
			return false;
		}

		double total = gridHeader.getNumberRows();
		int percent = 0;

		double nodata = gridHeader.getNodataValue();
		register float *buf = (float *)row_buf1;					
		for( int j = 0; j < gridHeader.getNumberRows(); j++)
		{
			for( int i = 0; i < gridHeader.getNumberCols(); i++)
			{
				buf[i] = (float)data[j][i];
				if(buf[i] == nodata)
					buf[i] = float_null;
			}
			putwindowrow( grid_layer, j, (CELLTYPE*)row_buf1);				

			int newpercent = (int)((j/total)*100);
			if( newpercent > percent )
			{	percent = newpercent;
				if( callback != NULL )
					callback( percent, "Writing Esri Grid" );
			}
		}
	
		CFree1 ((char *)row_buf1);	
		row_buf1 = NULL;

		//Close handle					
		celllyrclose(grid_layer);
		grid_layer = -1;
		return true;					
	}
	#pragma optimize("", on)

	bool dGrid::esriWriteDiskToDisk()
	{	//Header info cannot be changed!!!
		return true;
	}

	#pragma optimize("", off)
	bool dGrid::esriInitializeDisk( double InitialValue )
	{	
		if( celllyrexists == NULL ||
			celllyrclose == NULL ||
			griddelete == NULL ||
			celllayercreate == NULL ||
			privateaccesswindowset == NULL ||
			putwindowrow == NULL ||
			getmissingfloat == NULL )	
			{	grid_layer = -1;
				lastErrorCode = tkESRI_DLL_NOT_INITIALIZED;
				return false;
			}

		int cell_type = CELLFLOAT;
		
		double csize = gridHeader.getDx();
		//Bounding box is xllcorner, yllcorner, xurcorner, yurcorner
		double bndbox[4];
		bndbox[0] = gridHeader.getXllcenter() - gridHeader.getDx()*.5;
		bndbox[1] = gridHeader.getYllcenter() - gridHeader.getDy()*.5;
		bndbox[2] = gridHeader.getXllcenter() + gridHeader.getNumberCols()*gridHeader.getDx() - gridHeader.getDx()*.5;
		bndbox[3] = gridHeader.getYllcenter() + gridHeader.getNumberRows()*gridHeader.getDy() - gridHeader.getDy()*.5;
		
		char * fname = new char[gridFilename.GetLength()+1];
		strcpy( fname, gridFilename );
		
		if( celllyrexists( fname ) != 0 )
		{
			if( griddelete( fname ) == 0 )
			{	grid_layer = -1;
				lastErrorCode = tkESRI_CANT_DELETE_FILE;
				delete [] fname;
				return false;
			}
		}
	
		grid_layer = celllayercreate( fname, WRITEONLY, ROWIO, cell_type, csize, bndbox);

		if( grid_layer < 0 )
		{	grid_layer = -1;
			lastErrorCode = tkESRI_LAYER_CREATE;
			delete [] fname;
			return false;
		}
		
		getmissingfloat(&float_null);

		double adjbndbox[4];
		if( privateaccesswindowset( grid_layer, bndbox, csize, adjbndbox) < 0 )
		{	celllyrclose(grid_layer);
			if( celllyrexists( fname ) )
				griddelete( fname );						
			grid_layer = -1;
			lastErrorCode = tkESRI_ACCESS_WINDOW_SET;
			delete [] fname;
			return false;
		}

		//Allocate row buffer				
		row_buf1 = (CELLTYPE*)CAllocate1(gridHeader.getNumberCols() + 1, sizeof(CELLTYPE));
		if( row_buf1 == NULL )
		{	
			celllyrclose(grid_layer);						
			if( celllyrexists( fname ) )
				griddelete( fname );
			grid_layer = -1;
			lastErrorCode = tkCANT_ALLOC_MEMORY;
			delete [] fname;
			return false;
		}	

		double nodata = gridHeader.getNodataValue();	
		max = nodata;
		min = nodata;

		float *buf = (float *)row_buf1;					
		
		if( InitialValue == nodata )
		{
			for( int i = 0; i < gridHeader.getNumberCols(); i++)
				buf[i] = float_null;
		}
		else
		{	for( int i = 0; i < gridHeader.getNumberCols(); i++)
				//casting added by dpa 6/7/05
				buf[i] = (float) InitialValue;
		}
			
		for( int j = 0; j < gridHeader.getNumberRows(); j++)
			putwindowrow( grid_layer, j, (CELLTYPE*)row_buf1);			
								
		//Close and Reopen so VAT Table is Written		
		celllyrclose(grid_layer);
		grid_layer = -1;
		CFree1 ((char *)row_buf1);
		row_buf1 = NULL;

		delete [] fname;
		return esriReadDiskToDisk();								
	}
	#pragma optimize("", on)

	#pragma optimize("", off)
	double dGrid::esriGetValueDisk( int Column, int Row )
	{			
		if( Column < 0 || Column >= gridHeader.getNumberCols() )
			return gridHeader.getNodataValue();
		if( Row < 0 || Row >= gridHeader.getNumberRows() )
			return gridHeader.getNodataValue();

		if( Row == current_row - 1 )
		{	
			if( ((float *)row_buf1)[Column] == float_null )
				return gridHeader.getNodataValue();
			return double((((float *)row_buf1)[Column]));						
		}
		else if( Row == current_row )
		{	
			if( ((float *)row_buf2)[Column] == float_null )
				return gridHeader.getNodataValue();
			return double((((float *)row_buf2)[Column]));						
		}
		else if( Row == current_row + 1 )
		{	
			if( ((float *)row_buf3)[Column] == float_null )
				return gridHeader.getNodataValue();
			return double((((float *)row_buf3)[Column]));						
		}
		else
		{	esriBufferRows( Row );
			
			if( ((float *)row_buf2)[Column] == float_null )
				return gridHeader.getNodataValue();
			return double((((float *)row_buf2)[Column]));								
		}
		
		return gridHeader.getNodataValue();
	}
	#pragma optimize("", on)

	#pragma optimize("", off)
	void dGrid::esriSetValueDisk( int Column, int Row, double Value )
	{
		if( putwindowrow == NULL )
			return;

		double value = Value;
		//Load the buffers with the current row
		getValueDisk( Column, Row );
		if( Row == current_row - 1 )
		{
			if( value == gridHeader.getNodataValue() )
				value = float_null;

			register float *buf = (float *)row_buf1;			
			//casting added by dpa 6/7/05
			buf[Column] = (float) value;					
			putwindowrow( grid_layer, Row, (CELLTYPE*)row_buf1);
		}
		else if( Row == current_row )
		{
			if( value == gridHeader.getNodataValue() )
				value = float_null;
				
			register float *buf = (float *)row_buf2;			
     		//casting added by dpa 6/7/05
			buf[Column] = (float) value;
			putwindowrow( grid_layer, Row, (CELLTYPE*)row_buf2);			
		}
		else if( Row == current_row + 1 )
		{
			if( value == gridHeader.getNodataValue() )
				value = float_null;

			register float *buf = (float *)row_buf3;			
			//casting added by dpa 6/7/05
			buf[Column] = (float) value;
			putwindowrow( grid_layer, Row, (CELLTYPE*)row_buf3);			
		}		
	}
	#pragma optimize("", on)

	#pragma optimize("", off)
	void dGrid::esriClearDisk(double clearValue)
	{	
		if( putwindowrow == NULL )
			return;

		register float *buf = (float *)row_buf1;					
		for( int j = 0; j < gridHeader.getNumberRows(); j++)
		{	
			double val = clearValue;
			if( val == gridHeader.getNodataValue() )
				val = float_null;
			for( int i = 0; i < gridHeader.getNumberCols(); i++)
			{
				//casting added by dpa 6/7/05
				buf[i]=(float) val;
			}
			putwindowrow( grid_layer, j, (CELLTYPE*)row_buf1);								
		}
		for( int i = 0; i < gridHeader.getNumberCols(); i++)
		{
			//casting added by dpa 6/7/05
			buf[i]=(float) clearValue;
		}					

		esriBufferRows( 0 );				
	}
	#pragma optimize("", on)

	#pragma optimize("", off)
	void dGrid::esriBufferRows( int center_row )
	{
		if( getwindowrow == NULL )
		{	register float *ibuf;
		
			for( int i = 0; i < gridHeader.getNumberCols(); i++ )
			{	
				ibuf = (float *)row_buf1;
				ibuf[i] = float_null;
				ibuf = (float *)row_buf2;
				ibuf[i] = float_null;
				ibuf = (float *)row_buf3;
				ibuf[i] = float_null;																	
			}	
			return;
		}

		bool nd_fill_one = false;
		bool nd_fill_two = false;
		bool nd_fill_three = false;

		if( gridHeader.getNumberRows() >= center_row -1 && center_row - 1 >= 0 )
		{					
			getwindowrow(grid_layer, center_row - 1, (CELLTYPE*)row_buf1);		
			nd_fill_one = false;
		}
		else
			nd_fill_one = true;

		if( gridHeader.getNumberRows() >= center_row && center_row >= 0 )
		{			
			getwindowrow(grid_layer, center_row, (CELLTYPE*)row_buf2);		
			nd_fill_two = false;
		}
		else
			nd_fill_two = true;

		if( gridHeader.getNumberRows() >= center_row + 1 && center_row + 1 >= 0 )
		{
			getwindowrow(grid_layer, center_row + 1, (CELLTYPE*)row_buf3);		
			nd_fill_three = false;
		}
		else
			nd_fill_three = true;

		current_row = center_row;

		//Initialize row to nodata
		if( nd_fill_one || nd_fill_two || nd_fill_three )
		{	
			register float *ibuf;
			
			for( int i = 0; i < gridHeader.getNumberCols(); i++ )
			{	
				if( nd_fill_one )
				{	ibuf = (float *)row_buf1;
					ibuf[i] = float_null;
				}
				if( nd_fill_two )
				{	ibuf = (float *)row_buf2;
					ibuf[i] = float_null;
				}
				if( nd_fill_three )
				{	ibuf = (float *)row_buf3;
					ibuf[i] = float_null;
				}												
			}			
		}
	}
	#pragma optimize("", on)

	#pragma optimize("", off)
	bool dGrid::esriSaveAs( CString filename, void(*callback)(int number, const char * message) )
	{
		long temp_grid_layer = -1;
		//Check the needed functions
		if( celllyrclose == NULL ||
			celllyrexists == NULL ||
			celllayercreate == NULL ||
			griddelete == NULL ||			
			privateaccesswindowset == NULL ||
			getmissingfloat == NULL ||
			putwindowrow == NULL )
			{	temp_grid_layer = -1;
				lastErrorCode = tkESRI_DLL_NOT_INITIALIZED;
				return false;
			}

		int cell_type = CELLFLOAT;
		
		double csize = gridHeader.getDx();
		//Bounding box is xllcorner, yllcorner, xurcorner, yurcorner
		double bndbox[4];
		bndbox[0] = gridHeader.getXllcenter() - gridHeader.getDx()*.5;
		bndbox[1] = gridHeader.getYllcenter() - gridHeader.getDy()*.5;
		bndbox[2] = gridHeader.getXllcenter() + gridHeader.getNumberCols()*gridHeader.getDx() - gridHeader.getDx()*.5;
		bndbox[3] = gridHeader.getYllcenter() + gridHeader.getNumberRows()*gridHeader.getDy() - gridHeader.getDy()*.5;
		double adjbndbox[4];
	
		char * fname = new char[filename.GetLength()+1];
		strcpy( fname, filename );		
		if( celllyrexists( fname ) != 0 )
			griddelete( fname );
					
		temp_grid_layer = celllayercreate( fname, WRITEONLY, ROWIO, cell_type, csize, bndbox);
		if( temp_grid_layer < 0 )
		{	temp_grid_layer = -1;
			lastErrorCode = tkESRI_LAYER_CREATE;
			delete [] fname;
			return false;
		}

		if( privateaccesswindowset( temp_grid_layer, bndbox, csize, adjbndbox) < 0 )
		{	celllyrclose(temp_grid_layer);
			if( celllyrexists( fname ) )
				griddelete( fname );						
			temp_grid_layer = -1;
			lastErrorCode = tkESRI_ACCESS_WINDOW_SET;
			delete [] fname;
			return false;
		}

		getmissingfloat(&float_null);
									
		//Allocate row buffer				
		void * temp_row_buf = (CELLTYPE*)CAllocate1(gridHeader.getNumberCols() + 1, sizeof(CELLTYPE));
		if ( temp_row_buf == NULL )
		{	celllyrclose(temp_grid_layer);
			if( celllyrexists( fname ) )
				griddelete( fname );						
			temp_grid_layer = -1;
			lastErrorCode = tkCANT_ALLOC_MEMORY;
			delete [] fname;
			return false;
		}

		double total = gridHeader.getNumberRows();
		int percent = 0;

		double nodata = gridHeader.getNodataValue();
		register float *buf = (float *)temp_row_buf;					
		for( int j = 0; j < gridHeader.getNumberRows(); j++)
		{
			for( int i = 0; i < gridHeader.getNumberCols(); i++)
			{
				buf[i] = (float)getValue( i, j );
				if(buf[i] == nodata)
					buf[i] = float_null;
			}
			putwindowrow( temp_grid_layer, j, (CELLTYPE*)temp_row_buf);				

			int newpercent = (int)((j/total)*100);
			if( newpercent > percent )
			{	percent = newpercent;
				if( callback != NULL )
					callback( percent, "Writing Esri Grid" );
			}
		}
	
		CFree1 ((char *)temp_row_buf);	
		temp_row_buf = NULL;

		if( isInRam == true )
		{	gridFilename = filename;
			//Close handle					
			celllyrclose(temp_grid_layer);
			temp_grid_layer = -1;
			delete [] fname;
			return true;					
		}
		else
		{	celllyrclose(temp_grid_layer);
			temp_grid_layer = -1;
			close();
			delete [] fname;
			return open( filename, false, ESRI_GRID, callback );
		}
	}
	#pragma optimize("", on)

///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//			ESRI GRID FUNCTIONS
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//			SDTS GRID FUNCTIONS
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	# define null 0
	bool dGrid::sdtsReadDiskToMemory( void(*callback)(int number, const char * message ) )
	{	
		
		//Initialize the grid
		strcpy( file_name, gridFilename );

		//Find the byte order of the machine		
		g123order(&order);
					
		char * fname = new char[ gridFilename.GetLength() + 1];
		strcpy( fname, gridFilename );
		long fillvalue;
		if( !read_sdts_header( fname, gridHeader, fillvalue ) )
		{	lastErrorCode = tkSDTS_BAD_FILE_HEADER;
			return false;
		}
		
		alloc();
					
		cells_out( gridFilename, status, fillvalue, callback );

		//Check for -255 Value that can be produced on extraction
		double nodata_value = gridHeader.getNodataValue();
		double value = gridHeader.getNodataValue();
		double average = 0;
		int cnt = 0;
		
		for( int row = 0; row < gridHeader.getNumberRows(); row++ )
		{	
			for( int column = 0; column < gridHeader.getNumberCols(); column++ )
			{	
				value = getValue( column, row );
				average = 0;
				cnt = 0;
				if( value == -255 )
				{				
					double up = getValue( column, row + 1 );
					if( up != nodata_value && up != -255 )
					{	average += up;
						cnt++;
					}
					double up_left = getValue( column - 1, row + 1 );
					if( up_left != nodata_value && up_left != -255 )
					{	average += up_left;
						cnt++;
					}
					double left = getValue( column - 1, row );
					if( left != nodata_value && left != -255 )
					{	average += left;
						cnt++;
					}
					double down_left = getValue( column - 1, row - 1 );
					if( down_left != nodata_value && down_left != -255 )
					{	average += down_left;
						cnt++;
					}
					double down = getValue( column, row - 1 );
					if( down != nodata_value && down != -255 )
					{	average += down;
						cnt++;
					}
					double down_right = getValue( column + 1, row - 1 );
					if( down_right != nodata_value && down_right != -255 )
					{	average += down_right;
						cnt++;
					}
					double right = getValue( column + 1, row );
					if( right != nodata_value && right != -255 )
					{	average += right;
						cnt++;
					}
					double up_right = getValue( column + 1, row + 1 );
					if( up_right != nodata_value && up_right != -255 )
					{	average += up_right;
						cnt++;
					}

					if( cnt != 0 )
						average = average/cnt;
					else
						average = nodata_value;

					setValue( column, row, (double)average );
				}
			}
		}
							
		return true;	
	}

	bool dGrid::read_sdts_header(char * filename, dHeader & h, long & fillvalue)
	{	
		strcpy( file_name, filename );
		baseAndId();
		if (!beg123file (filename,'R',&int_level,&ice,ccs,&fpin))
			return false;
		
		stat2 = end123file (&fpin);

		dem_head(status);
		long voidvalue;
		cell_range(status, voidvalue, fillvalue);
		h.setNodataValue( voidvalue );

		double xhrs, yhrs;
		get_iref(xhrs, yhrs);
		h.setDx( (int)xhrs );
		h.setDy( (int)xhrs );

		get_xref();
		
		double SWX, NWX, SWY, SEY, dv;
		dem_mbr(status, SWX, SWY, NWX, dv,dv,dv,dv, SEY);

		int ncol, nrow;
		dem_rc(status, ncol, nrow);
		h.setNumberCols( ncol );
		h.setNumberRows( nrow );
		
		double upperlx, upperly;
		get_nw_corner(upperlx, upperly);

		double miny = upperly  - ( nrow * yhrs) ; 
		double minx = upperlx ; 

		if( SWX < NWX )
			minx = SWX;
		else
			minx = NWX;
		if( SWY < SEY )
			miny = SWY;
		else
			miny = SEY;

		double mydouble = (int)minx;
		h.setXllcenter( ((int)(mydouble/xhrs + 0.99999)*xhrs) );
		mydouble = (int)miny;
		h.setYllcenter( ((int)(mydouble/yhrs + 0.99999)*yhrs) );

		return true;
	}

	void dGrid::baseAndId()
	{
		int len,j;
		//parse out base_name 
		len = _tcslen(file_name);
		for(j=0;j<len-8;j++)
			base_name[j] = file_name[j];
		base_name[j] = '\0';
	}

	void dGrid::dem_rc(int status, int & ncol, int & nrow)
	{

	  strcpy (file_name,base_name);
	  strcat (file_name,"LDEF.DDF");
	  if (! beg123file (file_name,'R',&int_level,&ice,ccs,&fpin))
			 {
			  printf ("\nCAN'T OPEN 'dem' LDEF FILE %s",file_name);
			  exit(0);
			  }
	  /*      Read data descriptive record (DDR)      */

	  if (! rd123ddrec 
			(fpin,          /* file pointer */
			string,         /* DDR record returned */
			&status))       /* status returned */
			  {
				printf ("\n*** ERROR READING DDR *** %d",status);
				//fprintf (fpdem,"\n*** ERROR READING DDR *** %d",status);
				goto done;
			 }

	  status = -1;
  
	  /*       Loop to process each subfield             */

	  do 
	  {

	  /*      Read data record subfield    */

		 if (! rd123sfld 
			(fpin,          /* file pointer */
			tag,            /* field tag returned */
			&leadid,        /* leader identifier returned */
			string,         /* subfield contents returned */
			&str_len,       /* length of subfield */
			&status))        /* status returned */
			 {
			 printf ("\nERROR READING DATA RECORD SUBFIELD");
			 //fprintf (fpdem,"\nERROR READING DATA RECORD SUBFIELD");
			 goto done;
			 }

	  /*      Retrieve description of current subfield        */


		 if (! chk123sfld 
			(fpin,          /* file pointer */
			tag,            /* tag output */
			descr,          /* subfield descriptions output */
			frmts))          /* subfield format control */
			 {
			 printf ("\nERROR CHECKING DATA RECORD SUBFIELD");
			 //fprintf (fpdem,"\nERROR CHECKING DATA RECORD SUBFIELD");
			 goto done;
			 }

	  /*       Process based on field and subfield tags           */

		if ( !strcmp(tag,"LDEF") && !strcmp(descr,"NROW"))
			 {
			  nrow = atol (string);
			 }
		else if ( !strcmp(tag,"LDEF") && !strcmp(descr,"NCOL"))
			 {
			  ncol = atol (string);
			 }
	/*    If end of record, write out record and reinitialize        */

		if (status == 3 || status == 4)
		  {
	/*       printf ("dem _rc nrow = %ld, ncol= %ld \n",nrow,ncol);*/
		  }
	   } while (status != 4);   /* Break out of loop at end of file */

	  done:

	  stat2 = end123file (&fpin);
	  return;
	}
	/*********************************************/
	void dGrid::get_iref(double & xhrs, double & yhrs)
	{
	/* set some default values */
	sfax = 1.0;
	sfay = 1.0;
	xorg = 0.0;
	yorg = 0.0;

	strcpy (file_name,base_name);
	strcat (file_name,"IREF.DDF");

	if (! beg123file (file_name,'R',&int_level,&ice,ccs,&fpin))
			 {
			 printf ("\nERROR OPENING FILE %s",file_name);
			 exit(0);
			 }

	if (! rd123ddrec 
			(fpin,          /* file pointer */
			string,         /* DDR record returned */
			&status))       /* status returned */
			 {
			 printf ("\n*** ERROR READING DDR ***");

			 goto done;
			 }

	status = -1;

	/*       Loop to process each subfield in Identification module            */

	do {

	/*      Read data record subfield    */

	 if (! rd123sfld 
			(fpin,          /* file pointer */
			tag,            /* field tag returned */
			&leadid,        /* leader identifier returned */
			string,         /* subfield contents returned */
			&str_len,       /* string length */
			&status))       /* status returned */
			 {
			 printf ("\nERROR READING DATA RECORD SUBFIELD (IDEN MODULE)");

			 goto done;
			 }

	/*      Retrieve description of current subfield        */


	 if (! chk123sfld 
			(fpin,          /* file pointer */
			tag,            /* tag output */
			descr,          /* subfield descriptions output */
			frmts))          /* subfield format control */
			 {
			 printf ("\nERROR CHECKING DATA RECORD SUBFIELD");

			 goto done;
			 }

	/*    check subfield name and extract contents for each subfield       */

		  if (!strcmp (tag, "IREF") && !strcmp (descr, "SFAX"))
			sfax = atof(string)  ;
		  else if (!strcmp (tag, "IREF") && !strcmp (descr, "SFAY"))
			sfay = atof(string)  ;
		  else if (!strcmp (tag, "IREF") && !strcmp (descr, "XORG"))
			xorg = atof(string)  ;
		  else if (!strcmp (tag, "IREF") && !strcmp (descr, "YORG"))
			yorg = atof(string)  ;
		  else if (!strcmp (tag, "IREF") && !strcmp (descr, "XHRS"))
			xhrs = atof(string)  ;
		  else if (!strcmp (tag, "IREF") && !strcmp (descr, "YHRS"))
			yhrs = atof(string)  ;

	 } while (status != 4);   /* Break out of loop at end of file */

	 done:
	 stat2 = end123file (&fpin);

	/*
	printf ("\nsfax:                              %f",sfax);
	printf ("\nsfay:                              %f",sfay);
	printf ("\nxorg:                              %f",xorg);
	printf ("\nyorg:                              %f",yorg);

	 printf ("\ncell width:                       %f",width);
	 printf ("\ncell height:                      %f",height);
	*/
	return;
	}


	/*********************************************/
	void dGrid::get_xref(void)
	{
	/* set some default values */
	strcpy(rsnm,"??1")  ;
	/*strcpy(hdat,"??2")  ;*/
	/*strcpy(rdoc,"??3")  ;*/

	strcpy (file_name,base_name);
	strcat (file_name,"XREF.DDF");

	if (! beg123file (file_name,'R',&int_level,&ice,ccs,&fpin))
			 {
			 printf ("\nERROR OPENING FILE %s",file_name);
			 exit(0);
			 }

	if (! rd123ddrec 
			(fpin,          /* file pointer */
			string,         /* DDR record returned */
			&status))       /* status returned */
			 {
			 printf ("\n*** ERROR READING DDR ***");

			 goto done;
			 }

	status = -1;

	/*       Loop to process each subfield in Identification module            */

	do {

	/*      Read data record subfield    */

	 if (! rd123sfld 
			(fpin,          /* file pointer */
			tag,            /* field tag returned */
			&leadid,        /* leader identifier returned */
			string,         /* subfield contents returned */
			&str_len,       /* string length */
			&status))       /* status returned */
			 {
			 printf ("\nERROR READING DATA RECORD SUBFIELD (IDEN MODULE)");

			 goto done;
			 }

	/*      Retrieve description of current subfield        */


	 if (! chk123sfld 
			(fpin,          /* file pointer */
			tag,            /* tag output */
			descr,          /* subfield descriptions output */
			frmts))         /* subfield format control */
			 {
			 printf ("\nERROR CHECKING DATA RECORD SUBFIELD");

			 goto done;
			 }

	/*    check subfield name and extract contents for each subfield       */

		  if (!strcmp      (tag, "XREF") && !strcmp (descr, "RSNM"))
			strcpy(rsnm,string)  ;
	/*
		  else if (!strcmp (tag, "XREF") && !strcmp (descr, "ZONE"))
			zone = atoi(string)  ;
		  else if (!strcmp (tag, "XREF") && !strcmp (descr, "HDAT"))
			strcpy(hdat,string)  ;
		  else if (!strcmp (tag, "XREF") && !strcmp (descr, "RDOC"))
			strcpy(rdoc,string)  ;
	*/

	 } while (status != 4);   /* Break out of loop at end of file */

	/*
	printf ("hdat:                              %s\n",hdat);
	printf ("rdoc:                              %s\n",rdoc);


	printf ("\nrsnm:                              %s\n",rsnm);
	printf (  "zone:                              %d\n",zone);
	*/
	done:
	stat2 = end123file (&fpin);

	return;
	}

	/***************************************/

	void dGrid::dem_head(int status)
	{
	/*      Open Identification module */

	strcpy (file_name,base_name);
	strcat (file_name,"IDEN.DDF");

	if (! beg123file (file_name,'R',&int_level,&ice,ccs,&fpin))
			 {
			 printf ("\nERROR OPENING FILE %s",file_name);
			 exit(0);
			 }

	/*printf ("\n\nIdentification module:  %s\n",file_name);*/

	/*      Read Identification module data descriptive record (DDR)      */

	if (! rd123ddrec 
			(fpin,          /* file pointer */
			string,         /* DDR record returned */
			&status))       /* status returned */
			 {
			 printf ("\n*** ERROR READING DDR ***");

			 goto done;
			 }

	status = -1;

	/*       Loop to process each subfield in Identification module            */

	do {

	/*      Read data record subfield    */

	 if (! rd123sfld 
			(fpin,          /* file pointer */
			tag,            /* field tag returned */
			&leadid,        /* leader identifier returned */
			string,         /* subfield contents returned */
			&str_len,       /* string length */
			&status))       /* status returned */
			 {
			 printf ("\nERROR READING DATA RECORD SUBFIELD (IDEN MODULE)");

			 goto done;
			 }

	/*      Retrieve description of current subfield        */


	 if (! chk123sfld 
			(fpin,          /* file pointer */
			tag,            /* tag output */
			descr,          /* subfield descriptions output */
			frmts))          /* subfield format control */
			 {
			 printf ("\nERROR CHECKING DATA RECORD SUBFIELD");

			 goto done;
			 }

	/*    Display subfield name and contents for each subfield       */
			
	/*
		  if (!strcmp (tag, "IDEN") && !strcmp (descr, "TITL"))
			// title 
			  printf ("\nTitle:            %-41s\n",string);
		  else if (!strcmp (tag, "IDEN") && !strcmp (descr, "SCAL"))
		  {
			 // scale 
			 scale = atol(string);
			 printf ("scale:                   %8s\n",string);
		  }
		  else if (!strcmp (tag, "IDEN") && !strcmp (descr, "DAID"))
			printf ("Data ID:     %s\n",string);
		  else if (!strcmp (tag, "IDEN") && !strcmp (descr, "MPDT"))
			printf ("Map Date:                  %s\n",string);
		  else if (!strcmp (tag, "IDEN") && !strcmp (descr, "DCDT"))
			printf ("Data set creation date:    %s\n",string);
	*/
	 } while (status != 4);   /* Break out of loop at end of file */

	done:

	/*       Close input Identification module           */

	status = end123file (&fpin);

	}


	/*********************************************/

	void dGrid::dem_mbr(int status, double & SWX, double & SWY, double & NWX, double & NWY, double & NEX, double & NEY, double & SEX, double & SEY)
	{
	  int i;
	  int seq=0;
	  double fl;
	  double x[5], y[5];
	 /* int label ;*/
	  strcpy (file_name,base_name);
	  strcat (file_name,"SPDM.DDF");
	/*  strcat (file_name,module);*/
	  if (! beg123file (file_name,'R',&int_level,&ice,ccs,&fpin))
			 {
			  printf ("\nCAN'T OPEN 'dem' SPDM FILE %s",file_name);
			  exit(0);
			  }
	  /*      Read data descriptive record (DDR)      */

	  if (! rd123ddrec 
			(fpin,          /* file pointer */
			string,         /* DDR record returned */
			&status))       /* status returned */
			  {
				printf ("\n*** ERROR READING DDR *** %d",status);
				//fprintf (fpdem,"\n*** ERROR READING DDR *** %d",status);
				goto done;
			 }

	  status = -1;
	  nxy = 1;         /* number of coordinate pairs */
  
	  /*       Loop to process each subfield             */

	  do {
	  /*      Read data record subfield    */

	  if (! rd123sfld 
			(fpin,          /* file pointer */
			tag,            /* field tag returned */
			&leadid,        /* leader identifier returned */
			string,         /* subfield contents returned */
			&str_len,       /* length of subfield */
			&status))        /* status returned */
			 {
			 printf ("\nERROR READING DATA RECORD SUBFIELD");
			 //fprintf (fpdem,"\nERROR READING DATA RECORD SUBFIELD");
			 goto done;
			 }

	  /*      Retrieve description of current subfield        */


	   if (! chk123sfld 
			(fpin,          /* file pointer */
			tag,            /* tag output */
			descr,          /* subfield descriptions output */
			frmts))         /* subfield format control */
			 {
			 printf ("\nERROR CHECKING DATA RECORD SUBFIELD");
			 //fprintf (fpdem,"\nERROR CHECKING DATA RECORD SUBFIELD");
			 goto done;
			 }

	   /* Process based on field and subfield tags */
		if (!strcmp(tag, "DMSA"))
		{
			fl = atof(string);
			if (!strcmp(descr, "X") || !strcmp(descr, "!X"))
			{
				x[nxy] = fl;		/* X spatial address */
			} 
			else if (!strcmp(descr, "Y") || !strcmp(descr, "!Y"))
			{
				y[nxy] = fl;		/* Y spatial address */
				nxy++;
			}
		}

	/*    If end of record, write out record and reinitialize        */

		if (status == 3 || status == 4)
		{
		   seq++;

		   for (i = 1; i < nxy; i++)
		   {
				if (i == 1)
				{
					SWX = x[i];
					SWY = y[i];
				}
				else if (i == 2)
				{
					NWX = x[i];
					NWY = y[i];
				}
				else if (i == 3)
				{
					NEX = x[i];
					NEY = y[i];
				}
				else if (i == 4)
				{
					SEX = x[i];
					SEY = y[i];
				}
		   }
		   nxy=0;
		}
	  } while (status != 4);   /* Break out of loop at end of file */

	done:

	  stat2 = end123file (&fpin);
	  return;
	}

	/*********************************************/
	void dGrid::cell_range(int status, long & voidvalue, long & fillvalue)
	{
	int seq=0;
	int recid;

	strcpy (file_name,base_name);
	strcat (file_name,"DDOM.DDF");
	/*printf( "looking for cell range info in DDOM file\n");*/

	if (! beg123file (file_name,'R',&int_level,&ice,ccs,&fpin))
			 {
			 printf ("\nERROR OPENING FILE %s",file_name);
			 exit(0);
			 }

	if (! rd123ddrec 
			(fpin,          /* file pointer */
			string,         /* DDR record returned */
			&status))       /* status returned */
			 {
			 printf ("\n*** ERROR READING DDR ***");

			 goto done;
			 }

	status = -1;

	/*       Loop to process each subfield in CATS module            */

	do {  

	/*      Read data record subfield    */

	 if (! rd123sfld 
			(fpin,          /* file pointer */
			tag,            /* field tag returned */
			&leadid,        /* leader identifier returned */
			string,         /* subfield contents returned */
			&str_len,       /* string length */
			&status))       /* status returned */
			 {
			 printf ("\nERROR READING DATA RECORD SUBFIELD (AHDR MODULE)");

			 goto done;
			 }

	/*      Retrieve description of current subfield        */


	 if (! chk123sfld 
			(fpin,          /* file pointer */
			tag,            /* tag output */
			descr,          /* subfield descriptions output */
			frmts))         /* subfield format control */
			 {
			 printf ("\nERROR CHECKING DATA RECORD SUBFIELD");

			 goto done;
			 }
	/*    check subfield name and extract contents for each subfield       */

		  if      (!strcmp (tag, "DDOM") && !strcmp (descr, "RCID"))
			{
	/*             printf("RECID as string %s \n", string);*/
				 recid = atoi(string); 
	/*             printf ("rec id as number= %d \n",recid);*/
			}      
		  else if (recid == 1  && !strcmp ( descr,"DVAL"))
		  {
			voidvalue = atoi (string)   ;
			//if( voidvalue == -32767 )
			//	voidvalue = -1;
	/*        printf ("void %d %s %d\n",recid,string, voidvalue);*/
		  }
		  else if (recid == 2  && !strcmp ( descr,"DVAL"))
		  {
			fillvalue = atoi(string)   ;		
			/*        printf ("fill %d %s %d\n",recid,string, fillvalue);*/
		  }
		  else if (recid == 3  && !strcmp ( descr,"DVAL"))
		  {
			//minvalue = atoi (string)   ;
	/*        printf ("min %d %s %d\n",recid,string, minvalue);*/
		  }
		  else if (recid == 4  && !strcmp ( descr,"DVAL"))
		  {
			//maxvalue = atoi (string)   ;
	/*        printf ("max %d %s %d\n",recid,string, maxvalue);*/
		  }

	 } while (status != 4);   /* Break out of loop at end of file */

	done:
	stat2 = end123file (&fpin);

	return;
	}

	/***************************************************************************
	** 
	**    INVOCATION NAME: S123TOL2
	** 
	**    PURPOSE: TO CONVERT A 2 CHARACTER STRING TO A LONG INTEGER AND
	**              CHANGE ITS BYTE ORDERING WHEN SPECIFIED
	** 
	**    INVOCATION METHOD: S123TOL(STRING,NUM,REVERSE)
	** 
	**    ARGUMENT LIST: 
	**     NAME          TYPE      USE      DESCRIPTION
	**     STRING[]      PTR        I       CHARACTER STRING
	**     NUM           LONG       O       NUMBER TO BE CONVERTED
	**     REVERSE       LOGICAL    I       REVERSE BYTE ORDER FLAG
	**                                       1 - REVERSE
	**                                       0 - DO NOT REVERSE
	**     S123TOL2()     LOGICAL    O       SUCCESS FLAG
	**
	**    EXTERNAL FUNCTION REFERENCES: NONE
	** 
	**    INTERNAL VARIABLES:
	**     NAME          TYPE               DESCRIPTION
	**     MSB           char               most significant digit
	**     LSB           char               lest significant digit
	**     I4            long int           temp variable
	**   
	**    GLOBAL REFERENCES: NONE
	**
	**    GLOBAL VARIABLES: NONE
	** 
	**    GLOBAL CONSTANTS: NONE
	**
	**    CHANGE HISTORY: 
	**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
	**     sol katz(blm)               3/20/98 created 
	** 
	** 
	****************************************************************************** 
	**    CODE SECTION 
	**
	*****************************************************************************/
	/*#include "stc123.h"*/

	int dGrid::s123tol2(char* string,long* num,int reverse)
	{
	   /* INTERNAL VARIABLES */
				char MSB; 
	   unsigned char LSB; 
	   long i4;   
	   int I256 = 256;

	   if (reverse) {    /* IF BYTE ORDER TO BE REVERSED */

		  /* MOVE INPUT STRING TO WORK STRING IN REVERSE BYTE ORDER */
		  MSB = string[0];
		  LSB = string[1];
	/*      printf("\nreverse  %2.2x %2.2x %d %d\n", MSB, LSB, MSB, LSB); */
	   }

	   else {

		  /* MOVE INPUT STRING TO WORK STRING WITH NO CHANGE IN BYTE ORDER */
		  MSB = string[1];
		  LSB = string[0];
	/*      printf("\nstandard %2.2x %2.2x \n", MSB, LSB);*/

	   }
   
	   /* MOVE WORK INTEGER TO OUTPUT LONG INTEGER */
		  i4 = (long) (MSB * I256) ;
		  i4 = i4 + LSB;
		  *num = i4;
	   /* RETURN SUCCESS */
	   return(1);
	}

	/********************************************/
	void dGrid::cells_out(const char * filename, int status, long fillvalue, void (*callback)(int number, const char * message ) )
	{
		int i = 0;
		int j = 0;

		int percent = 0;
		int cnt = 0;
		double total = gridHeader.getNumberCols()*gridHeader.getNumberRows();
		double nodata_value = gridHeader.getNodataValue();
		float vscale;

		char fom = FeetOrMeters();

		if( fom == 'm' && gridHeader.getDx() == 10 && gridHeader.getDy() == 10 )
			vscale = 10.0;
		else
			vscale = 1.0;

		strcpy( file_name, filename );
		baseAndId();
		if (! beg123file (file_name,'R',&int_level,&ice,ccs,&fpin))
		{
			printf ("\nCAN'T OPEN CELL VALUES FILE %s...",file_name);
			return ;
		}

		if (! rd123ddrec(fpin,string,&status))      
		{
			 printf ("\n*** ERROR READING DDR ***");
	   //      fprintf (fpdem,"\n*** ERROR READING DDR ***");
			 goto done;
		}
		status = -1;

		/*       Loop to process each subfield             */
		do {
			/*      Read data record subfield    */
			if (! rd123sfld (fpin,tag,&leadid,string,&str_len,&status))    
			{
				printf ("\nERROR READING DATA RECORD SUBFIELD");
				goto done;
			}
			/*      Retrieve description of current subfield        */
			if (! chk123sfld(fpin,tag,descr,frmts))          
			{
				printf ("\nERROR CHECKING DATA RECORD SUBFIELD");
		//		fprintf (fpdem,"\nERROR CHECKING DATA RECORD SUBFIELD");
				goto done;
			}

			if (strstr (frmts, "B") != NULL)
			{	
				/*
				strncpy (temp, string, (int)str_len);
				if (str_len == 2)
				{
					if (!order)
						s123tol2 (string, &li, 1);
					else
						s123tol2 (string, &li, 0);
				
					//skip if it's a nodata cell
					if(li != nodata_value && li != fillvalue) 
					{
						if(fom == 'f')
						{
							li = (double)li/3.2808;
						}

						if( li == 32767 || li == -32767 )
							setValue( i, j, -255 );
						else
							setValue( i, j, (double)li );

						int newpercent = (cnt/total)*100;
						if( newpercent > percent )
						{	percent = newpercent;
							if( callback )
								callback( percent, "Reading SDTS Grid" );
						}
					}
					else
						setValue( i, j, nodata_value );

					cnt++;
					i++;
				//	fprintf (fpdem,"%7ld",li);
					
				}
				*/
				if (str_len == 2)
				{
					li = (string[0] << 8) | (string[1] & 0xFF);
				}
				else if (str_len == 4)
				{
					union
					{
						long i;
						float f;
					} u;
					s123tol(string, &u.i, !order);
					li = (int) (vscale * u.f);					
				}
				else
				{
					li = (long)nodata_value;
				}

				//BIAS FOR ELEVATION
				if( li < -1000 )
					li = (long)nodata_value;

				//Set the cell value
				if(li != nodata_value && li != fillvalue) 
				{
					if(fom == 'f')
					{
						li = (long)(li/3.2808);
					}

					if( li == 32767 || li == -32767 )
						setValue( i, j, -255 );
					else
						setValue( i, j, (double)li );

					int newpercent = (int)((cnt/total)*100);
					if( newpercent > percent )
					{	percent = newpercent;
						if( callback )
							callback( percent, "Reading SDTS Grid" );
					}
				}
				else
					setValue( i, j, nodata_value );
				cnt++;
				i++;
			}

	  /*       Output record/end of file delimeters                */
			if (status == 3)   /* subfield is at end of record */
			{
				//	fprintf (fpdem,"\n");
				j++;
				i = 0;
			}
		} while (status != 4);   /* Break out of loop at end of file */

	done:

	status = end123file (&fpin);
	}


	/*********************************************/
	void dGrid::get_nw_corner(double & upperlx, double & upperly)
	{
	long sadr_x,sadr_y;

	strcpy (file_name,base_name);
	strcat (file_name,"RSDF.DDF");
	if (! beg123file (file_name,'R',&int_level,&ice,ccs,&fpin))
			 {
			 printf ("\nERROR OPENING FILE %s",file_name);
			 exit(0);
			 }
	if (! rd123ddrec      
			(fpin,          /* file pointer */
			string,         /* DDR record returned */
			&status))       /* status returned */
			 {
			 printf ("\n*** ERROR READING DDR ***");
       
	  goto done;
  
		   }
	status = -1;
	/*  
		 Loop to process each subfield in Raster Definition module            */
	do 
	{
	/*      Read data record subfield    */

	 if (! rd123sfld 
			(fpin,          /* file pointer */
			tag,            /* field tag returned */
			&leadid,        /* leader identifier returned */
			string,      
	   /* subfield contents returned */
			&str_len,       /* string length */
			&status))       /* status returned */
			 {
   
		  printf ("\nERROR READING DATA RECORD SUBFIELD (IDEN MODULE)");
		  goto done;
   
		  }
	/*      Retrieve description of current subfield        */
	 if (! chk123sfld 
			(fpin,          /* file pointer */
			tag,            /* tag output */
			descr,          /* subfield descriptions output */
			frmts))          /* subfield format control */
			 {
			 printf ("\nERROR CHECKING DATA RECORD SUBFIELD");
			 goto done;
			 }
	/*    check subfield name and extract contents for each subfield       */
		  if (!strcmp (tag, "SADR") && !strcmp (descr, "X"))
		  {   

	/*         Binary data, convert character string returned by rd123sfld to a
			   long integer, changing bit order if necessary     */

			   if (strstr (frmts,"B") != null)
			   {
				 if (!order)
					s123tol (string, &sadr_x, 1);
				 else
					s123tol (string, &sadr_x, 0);
			   }
			   else if (strstr(frmts, "R"))
			   {
					sadr_x = (long) atof(string);
			   }
		  }
		  else if (!strcmp (tag, "SADR") && !strcmp (descr, "Y"))
		  {
	/*         Binary data, convert character string returned by rd123sfld to a
			   long integer, changing bit order if necessary     */

			   if (strstr (frmts,"B") != null)
			   {
				 if (!order)
					s123tol (string, &sadr_y, 1);
				 else
					s123tol (string, &sadr_y, 0);
			   }
			   else if (strstr(frmts, "R"))
			   {
					sadr_y = (long) atof(string);
			   }
		  }  
	 } while (status != 4);   /* Break out of loop at end of file */
	done:
	stat2 = end123file (&fpin);

	upperlx = (sadr_x * sfax ) + xorg;
	upperly = (sadr_y * sfay ) + yorg;
	/*
	printf ("\nsadr_x:            %ld",sadr_x);
	printf ("\nsadr_y:            %ld",sadr_y);
	printf ("\nupperlx:           %f",upperlx);
	printf ("\nupperly:           %f",upperly);
	*/
	return;
	}

	char dGrid::FeetOrMeters()
	{
		const int	MAX = 1000;
		char		data[1000];
		FILE*		fp;
		int			len;
		char*		index = NULL;

		baseAndId();
		strcpy (file_name,base_name);
		strcat (file_name,"DDOM.DDF");
		
		fp = fopen(file_name,"r");
		if(!fp)
			return 0;

		len = fread(data,sizeof(char),MAX-1,fp);
		fclose(fp);
		data[MAX-1] = '\0';

		index = strstr(data,"meter");
		if(index)
			return 'm';

		index = strstr(data,"METER");
		if(index)
			return 'm';

		index = strstr(data,"feet");
		if(index) 
			return 'f';

		index = strstr(data,"FEET");
		if(index)
			return 'f';

		return 0;
	}

///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//			SDTS GRID FUNCTIONS
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/