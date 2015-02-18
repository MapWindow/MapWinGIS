# include <afx.h>
# include <iostream>

//******************************************************************
//we want to disable this warning because it is not important to us
#pragma warning(disable:4251)
//******************************************************************

//-------------------------------------------
//-- grdtypes.h
//-------------------------------------------
# ifndef GRID_TYPES_H
# define GRID_TYPES_H


# ifndef MAX_STRING_LENGTH
# define MAX_STRING_LENGTH 255
# endif

# ifndef GRID_TYPES
# define GRID_TYPES
	
	enum GRID_TYPE	
	{ ASCII_GRID,
	  BINARY_GRID,
	  ESRI_GRID,
	  SDTS_GRID,
	  USE_EXTENSION,
	  INVALID_GRID_TYPE = -1
	};

# endif

# ifndef GRID_DATA_TYPE
# define GRID_DATA_TYPE
	
	enum DATA_TYPE
	{ SHORT_TYPE,
	  LONG_TYPE,
	  FLOAT_TYPE,
	  DOUBLE_TYPE,	 
	  UNKNOWN_TYPE,
	  INVALID_DATA_TYPE = -1
	};

# endif

# ifndef CONV_TO_SHAPES
# define CONV_TO_SHAPES

# define BORDER             -2225
# define DOUBLE_BORDER      -2226
# define DECISION           -2227
# define TRACE_BORDER       -2228
# define CURRENT_POLYGON	-2229

	enum DIRECTION
	{ NONE,
	  RIGHT,
	  UPRIGHT,
	  UP,
	  UPLEFT,
	  LEFT,
	  DOWNLEFT,
	  DOWN,
	  DOWNRIGHT
	};

# endif

# ifndef CINFO_COLOR_TYPE
# define CINFO_COLOR_TYPE

	enum ColoringType
	{
		ctHILLSHADE,	// ctHILLSHADE = 0,
		ctGRADIENT,		// ctGRADIENT = 1,
		ctRANDOM,		// ctRANDOM = 2,
	};

	enum GradientModel
	{	
		gmLOG,
		gmLINEAR,		
		gmEXPONENTIAL
	};

	enum PredefinedColors
	{
		ciFALL_LEAVES,
		ciSUMMER_MOUNTAINS,
		ciDESERT,
		ciGLACIERS,
		ciMEADOW,
		ciVALLEY_FIRES,
		ciDEAD_SEA,
		ciHIGHWAY_1		
	};

# endif

# endif
//-------------------------------------------
//-- grdtypes.h
//-------------------------------------------
//-------------------------------------------
//-- sHeader.h
//-------------------------------------------
# ifndef SHEADER_H
# define SHEADER_H



class __declspec( dllimport ) sHeader
{
	public:		
		//CONSTRUCTORS
		sHeader();
		sHeader( const sHeader & h );

		//DESTRUCTOR
		~sHeader();

		//OPERATORS
		sHeader operator=( const sHeader & h );

		//DATA ACCESS MEMBERS
		inline long getNumberCols();
		inline long getNumberRows();
		inline short getNodataValue();
		inline double getDx();
		inline double getDy();
		inline double getXllcenter();
		inline double getYllcenter();
		char * getProjection();
		char * getNotes();

		void setNumberCols( long p_ncols );
		void setNumberRows( long p_nrows );
		void setNodataValue( short p_nodata_value );
		void setDx( double p_dx );
		void setDy( double p_dy );
		void setXllcenter( double p_xllcenter );
		void setYllcenter( double p_yllcenter );
		void setProjection( const char * p_projection );
		void setNotes( const char * p_notes );

	private:
		long ncols;
		long nrows;
		short nodataValue;
		double dx;
		double dy;
		double xllcenter;
		double yllcenter;
		CString projection;
		CString notes;

};

# endif
//-------------------------------------------
//-- sHeader.h
//-------------------------------------------
//-------------------------------------------
//-- lHeader.h
//-------------------------------------------
# ifndef LHEADER_H
# define LHEADER_H


class __declspec( dllimport ) lHeader
{
	public:		
		//CONSTRUCTORS
		lHeader();
		lHeader( const lHeader & h );

		//OPERATORS
		lHeader operator=( const lHeader & h );

		//DESTRUCTOR
		~lHeader();

		//DATA ACCESS MEMBERS
		inline long getNumberCols();
		inline long getNumberRows();
		inline long getNodataValue();
		inline double getDx();
		inline double getDy();
		inline double getXllcenter();
		inline double getYllcenter();
		char * getProjection();
		char * getNotes();

		void setNumberCols( long p_ncols );
		void setNumberRows( long p_nrows );
		void setNodataValue( long p_nodata_value );
		void setDx( double p_dx );
		void setDy( double p_dy );
		void setXllcenter( double p_xllcenter );
		void setYllcenter( double p_yllcenter );
		void setProjection( const char * p_projection );
		void setNotes( const char * p_notes );

	private:
		long ncols;
		long nrows;
		long nodataValue;
		double dx;
		double dy;
		double xllcenter;
		double yllcenter;
		CString projection;
		CString notes;

};

# endif
//-------------------------------------------
//-- lHeader.h
//-------------------------------------------
//-------------------------------------------
//-- fHeader.h
//-------------------------------------------
# ifndef FHEADER_H
# define FHEADER_H


class __declspec( dllimport ) fHeader
{
	public:		
		//CONSTRUCTORS
		fHeader();
		fHeader( const fHeader & h );

		//Destructor
		~fHeader();


		//OPERATORS
		fHeader operator=( const fHeader & h );

		//DATA ACCESS MEMBERS
		inline long getNumberCols();
		inline long getNumberRows();
		inline float getNodataValue();
		inline double getDx();
		inline double getDy();
		inline double getXllcenter();
		inline double getYllcenter();
		char * getProjection();
		char * getNotes();

		void setNumberCols( long p_ncols );
		void setNumberRows( long p_nrows );
		void setNodataValue( float p_nodata_value );
		void setDx( double p_dx );
		void setDy( double p_dy );
		void setXllcenter( double p_xllcenter );
		void setYllcenter( double p_yllcenter );
		void setProjection( const char * p_projection );
		void setNotes( const char * p_notes );

	private:
		long ncols;
		long nrows;
		float nodataValue;
		double dx;
		double dy;
		double xllcenter;
		double yllcenter;
		CString projection;
		CString notes;

};

# endif
//-------------------------------------------
//-- fHeader.h
//-------------------------------------------
//-------------------------------------------
//-- dHeader.h
//-------------------------------------------
# ifndef DHEADER_H
# define DHEADER_H


class __declspec( dllimport ) dHeader
{
	public:		
		//CONSTRUCTORS
		dHeader();
		dHeader( const dHeader & h );

		//Destructor
		~dHeader();

		//OPERATORS
		dHeader operator=( const dHeader & h );

		//DATA ACCESS MEMBERS
		inline long getNumberCols();
		inline long getNumberRows();
		inline double getNodataValue();
		inline double getDx();
		inline double getDy();
		inline double getXllcenter();
		inline double getYllcenter();
		char * getProjection();
		char * getNotes();

		void setNumberCols( long p_ncols );
		void setNumberRows( long p_nrows );
		void setNodataValue( double p_nodata_value );
		void setDx( double p_dx );
		void setDy( double p_dy );
		void setXllcenter( double p_xllcenter );
		void setYllcenter( double p_yllcenter );
		void setProjection( const char * p_projection );
		void setNotes( const char * p_notes );

	private:
		long ncols;
		long nrows;
		double nodataValue;
		double dx;
		double dy;
		double xllcenter;
		double yllcenter;
		CString projection;
		CString notes;

};

# endif
//-------------------------------------------
//-- dHeader.h
//-------------------------------------------
//-------------------------------------------
//-- sGrid.h
//-------------------------------------------
# ifndef SGRID_H
# define SGRID_H


class __declspec( dllimport ) sGrid
{
	public:
		
		//CONSTRUCTORS
		sGrid();
		~sGrid();
		
		//OPERATORS
		short operator()( int Column, int Row );
				
		//FUNCTIONS
		bool open( const char * Filename, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION, void (*callback)( int number, const char * message ) = NULL );
		bool initialize( const char * Filename, sHeader Header, short initialValue, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION );		
		bool save( const char * Filename = "", GRID_TYPE GridType = USE_EXTENSION, void (*callback)(int number, const char * message ) = NULL );
		bool close();
		void clear(short clearValue);
	
		//MAPPING FUNCTIONS
		void ProjToCell( double x, double y, long & column, long & row );
		void CellToProj( long column, long row, double & x, double & y );
					
		//DATA MEMBER ACCESS
		inline sHeader getHeader();
		void setHeader( sHeader h );
		inline short getValue( int Column, int Row );
		inline void setValue( int Column, int Row, short Value );
		bool inRam();		
		short maximum();
		short minimum();
		long LastErrorCode();

		short ** getArrayPtr();

	private:

		inline int round( double d );
		GRID_TYPE getGridType( const char * filename );	
		void dealloc();
		void alloc();		
		inline bool inGrid( long column, long row );
		inline short getValueDisk( int column, int row );
		inline void setValueDisk( int column, int row, short value );
		void clearDisk(short clearValue);

		bool readDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool readDiskToDisk( void (*callback)(int number, const char * message ) = NULL );
		bool writeMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool writeDiskToDisk();

		//ASCII GRID FUNCTIONS		
		bool asciiReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool asciiReadDiskToDisk( void (*callback)(int number, const char * message ) = NULL );
		bool asciiWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool asciiWriteDiskToDisk();		
		void asciiReadHeader( std::istream & in );
		void asciiReadFooter( std::istream & in );
		void asciiWriteHeader( std::ostream & out );
		void asciiWriteFooter( std::ostream & out );
		bool asciiIsHeaderValue( CString headerValue, std::istream & in );
		bool asciiSaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//BINARY GRID FUNCTIONS
		bool binaryReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool binaryReadDiskToDisk();
		bool binaryWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool binaryWriteDiskToDisk();		
		bool binaryInitializeDisk( short InitialValue );			
		void binaryReadHeader( FILE * in );
		void binaryWriteHeader( FILE * out );
		short binaryGetValueDisk( int Column, int Row );
		void binarySetValueDisk( int Column, int Row, short Value );
		void binaryClearDisk(short clearValue);
		void binaryBufferRows( int CenterRow );
		bool binarySaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//ESRI GRID FUNCTIONS
		bool esriReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool esriReadDiskToDisk();
		bool esriWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool esriWriteDiskToDisk();		
		bool esriInitializeDisk( short InitialValue );			
		short esriGetValueDisk( int Column, int Row );
		void esriSetValueDisk( int Column, int Row, short Value );
		void esriClearDisk(short clearValue);
		void esriBufferRows( int CenterRow );
		bool esriSaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//SDTS GRID FUNCTIONS
		bool sdtsReadDiskToMemory( void(*callback)(int number, const char * message ) = NULL );
		void get_iref(double & xhrs, double & yhrs);
		void get_xref(void);
		void dem_rc(int status, int & ncol, int & nrow);
		void dem_head(int);
		void dem_mbr(int status, double & SWX, double & SWY, double & NWX, double & NWY, double & NEX, double & NEY, double & SEX, double & SEY);
		void cell_range(int status, long & voidvalue, long & fillvalue);
		void cells_out(const char * filename, int status, long fillvalue, void (*callback)(int number, const char * message ) = NULL );
		void get_nw_corner(double & upperlx, double & upperly);
		int  s123tol2(char *,long *,int);
		bool read_sdts_header(char * filename, sHeader & h, long & fillvalue);
		void baseAndId();
		char FeetOrMeters();

	private:

		sHeader gridHeader;
		bool isInRam;
		short ** data;
		short max;
		short min;
		GRID_TYPE gridType;
		CString gridFilename;

		//ASCII Specific Variables
			//Flags used to adjust reference to cell center
			bool xllcorner;
			bool yllcorner;
		//BINARY Specific Variables
			short * row_one;
			short * row_two;	//Current row
			short * row_three;
			long current_row;
			FILE * file_in_out;
			long file_position_beg_of_data;			
			DATA_TYPE data_type;
		//ESRI Specific Variables
			int grid_layer;		
			void * row_buf1;		
			void * row_buf2;
			void * row_buf3;					
		//SDTS Specific Variables
			FILE *fpin;
			long int_level;
			double sfax, sfay, xorg, yorg;
			double x[5];
			double y[5];
			long nxy;
			long str_len;
			long li;
			int stat2;
			int status;
			char temp[100];
			char ice;
			char leadid;
			char ccs[4];
			char tag[10];
			char fdlen[10];
			char *fdname;
			char mod_name[10];
			char base_name[MAX_STRING_LENGTH];
			char file_name[MAX_STRING_LENGTH];
			char string[5000];
			char descr[5000];
			char frmts[500];
			int order;
			char rsnm[5];

		//Flags used to indicate if the max or min value has been overwritten
		bool findNewMax;		
		bool findNewMin;	
		
		long lastErrorCode;
};

# endif

//-------------------------------------------
//-- sGrid.h
//-------------------------------------------
//-------------------------------------------
//-- lGrid.h
//-------------------------------------------
# ifndef LGRID_H
# define LGRID_H


class __declspec( dllimport ) lGrid
{
	public:
		
		//CONSTRUCTORS
		lGrid();
		~lGrid();
		
		//OPERATORS
		long operator()( int Column, int Row );
				
		//FUNCTIONS
		bool open( const char * Filename, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION, void (*callback)( int number, const char * message ) = NULL );
		bool initialize( const char * Filename, lHeader Header, long initialValue, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION );		
		bool save( const char * Filename = "", GRID_TYPE GridType = USE_EXTENSION, void (*callback)(int number, const char * message ) = NULL );
		bool close();
		void clear(long clearValue);

		//MAPPING FUNCTIONS
		void ProjToCell( double x, double y, long & column, long & row );
		void CellToProj( long column, long row, double & x, double & y );
				
		//DATA MEMBER ACCESS
		inline lHeader getHeader();
		void setHeader( lHeader h );
		inline long getValue( int Column, int Row );		
		inline void setValue( int Column, int Row, long Value );
		bool inRam();		
		long maximum();
		long minimum();
		long LastErrorCode();

		long ** getArrayPtr();

	private:

		inline int round( double d );
		GRID_TYPE getGridType( const char * filename );	
		void dealloc();
		void alloc();		
		inline bool inGrid( long column, long row );
		inline long getValueDisk( int column, int row );
		inline void setValueDisk( int column, int row, long value );
		void clearDisk(long clearValue);

		bool readDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool readDiskToDisk( void (*callback)(int number, const char * message ) = NULL );
		bool writeMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool writeDiskToDisk();

		//ASCII GRID FUNCTIONS		
		bool asciiReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool asciiReadDiskToDisk( void (*callback)(int number, const char * message ) = NULL );
		bool asciiWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool asciiWriteDiskToDisk();		
		void asciiReadHeader( std::istream & in );
		void asciiReadFooter( std::istream & in );
		void asciiWriteHeader( std::ostream & out );
		void asciiWriteFooter( std::ostream & out );
		bool asciiIsHeaderValue( CString headerValue, std::istream & in );
		bool asciiSaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//BINARY GRID FUNCTIONS
		bool binaryReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool binaryReadDiskToDisk();
		bool binaryWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool binaryWriteDiskToDisk();		
		bool binaryInitializeDisk( long InitialValue );			
		void binaryReadHeader( FILE * in );
		void binaryWriteHeader( FILE * out );
		long binaryGetValueDisk( int Column, int Row );
		void binarySetValueDisk( int Column, int Row, long Value );
		void binaryClearDisk(long clearValue);
		void binaryBufferRows( int CenterRow );
		bool binarySaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//ESRI GRID FUNCTIONS
		bool esriReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool esriReadDiskToDisk();
		bool esriWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool esriWriteDiskToDisk();		
		bool esriInitializeDisk( long InitialValue );			
		long esriGetValueDisk( int Column, int Row );
		void esriSetValueDisk( int Column, int Row, long Value );
		void esriClearDisk(long clearValue);
		void esriBufferRows( int CenterRow );
		bool esriSaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//SDTS GRID FUNCTIONS
		bool sdtsReadDiskToMemory( void(*callback)(int number, const char * message ) = NULL );
		void get_iref(double & xhrs, double & yhrs);
		void get_xref(void);
		void dem_rc(int status, int & ncol, int & nrow);
		void dem_head(int);
		void dem_mbr(int status, double & SWX, double & SWY, double & NWX, double & NWY, double & NEX, double & NEY, double & SEX, double & SEY);
		void cell_range(int status, long & voidvalue, long & fillvalue);
		void cells_out(const char * filename, int status, long fillvalue, void (*callback)(int number, const char * message ) = NULL );
		void get_nw_corner(double & upperlx, double & upperly);
		int  s123tol2(char *,long *,int);
		bool read_sdts_header(char * filename, lHeader & h, long & fillvalue);
		void baseAndId();
		char FeetOrMeters();

	private:

		lHeader gridHeader;
		bool isInRam;
		long ** data;
		long max;
		long min;
		GRID_TYPE gridType;
		CString gridFilename;

		//ASCII Specific Variables
			//Flags used to adjust reference to cell center
			bool xllcorner;
			bool yllcorner;
		//BINARY Specific Variables
			long * row_one;
			long * row_two;	//Current row
			long * row_three;
			long current_row;
			FILE * file_in_out;
			long file_position_beg_of_data;			
			DATA_TYPE data_type;
		//ESRI Specific Variables
			int grid_layer;		
			void * row_buf1;		
			void * row_buf2;
			void * row_buf3;					
		//SDTS Specific Variables
			FILE *fpin;
			long int_level;
			double sfax, sfay, xorg, yorg;
			double x[5];
			double y[5];
			long nxy;
			long str_len;
			long li;
			int stat2;
			int status;
			char temp[100];
			char ice;
			char leadid;
			char ccs[4];
			char tag[10];
			char fdlen[10];
			char *fdname;
			char mod_name[10];
			char base_name[MAX_STRING_LENGTH];
			char file_name[MAX_STRING_LENGTH];
			char string[5000];
			char descr[5000];
			char frmts[500];
			int order;
			char rsnm[5];

		//Flags used to indicate if the max or min value has been overwritten
		bool findNewMax;		
		bool findNewMin;		

		long lastErrorCode;
};

# endif
//-------------------------------------------
//-- lGrid.h
//-------------------------------------------
//-------------------------------------------
//-- fGrid.h
//-------------------------------------------
# ifndef FGRID_H
# define FGRID_H


class __declspec( dllimport ) fGrid
{
	public:
		
		//CONSTRUCTORS
		fGrid();
		~fGrid();
		
		//OPERATORS
		float operator()( int Column, int Row );
				
		//FUNCTIONS
		bool open( const char * Filename, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION, void (*callback)( int number, const char * message ) = NULL );
		bool initialize( const char * Filename, fHeader Header, float initialValue, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION );		
		bool save( const char * Filename = "", GRID_TYPE GridType = USE_EXTENSION, void (*callback)(int number, const char * message ) = NULL );
		bool close();
		void clear(float clearValue);

		//MAPPING FUNCTIONS
		void ProjToCell( double x, double y, long & column, long & row );
		void CellToProj( long column, long row, double & x, double & y );
				
		//DATA MEMBER ACCESS
		inline fHeader getHeader();
		void setHeader( fHeader h );
		inline float getValue( int Column, int Row );
		inline void setValue( int Column, int Row, float Value );
		bool inRam();		
		float maximum();
		float minimum();
		long LastErrorCode();

		float ** getArrayPtr();

	private:

		inline int round( double d );
		GRID_TYPE getGridType( const char * filename );	
		void dealloc();
		void alloc();		
		inline bool inGrid( long column, long row );
		inline float getValueDisk( int column, int row );
		inline void setValueDisk( int column, int row, float value );
		void clearDisk(float clearValue);

		bool readDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool readDiskToDisk( void (*callback)(int number, const char * message ) = NULL );
		bool writeMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool writeDiskToDisk();

		//ASCII GRID FUNCTIONS		
		bool asciiReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool asciiReadDiskToDisk( void (*callback)(int number, const char * message ) = NULL );
		bool asciiWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool asciiWriteDiskToDisk();		
		void asciiReadHeader( std::istream & in );
		void asciiReadFooter( std::istream & in );
		void asciiWriteHeader( std::ostream & out );
		void asciiWriteFooter( std::ostream & out );
		bool asciiIsHeaderValue( CString headerValue, std::istream & in );
		bool asciiSaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//BINARY GRID FUNCTIONS
		bool binaryReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool binaryReadDiskToDisk();
		bool binaryWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool binaryWriteDiskToDisk();		
		bool binaryInitializeDisk( float InitialValue );			
		void binaryReadHeader( FILE * in );
		void binaryWriteHeader( FILE * out );
		float binaryGetValueDisk( int Column, int Row );
		void binarySetValueDisk( int Column, int Row, float Value );
		void binaryClearDisk(float clearValue);
		void binaryBufferRows( int CenterRow );
		bool binarySaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//ESRI GRID FUNCTIONS
		bool esriReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool esriReadDiskToDisk();
		bool esriWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool esriWriteDiskToDisk();		
		bool esriInitializeDisk( float InitialValue );			
		float esriGetValueDisk( int Column, int Row );
		void esriSetValueDisk( int Column, int Row, float Value );
		void esriClearDisk(float clearValue);
		void esriBufferRows( int CenterRow );
		bool esriSaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//SDTS GRID FUNCTIONS
		bool sdtsReadDiskToMemory( void(*callback)(int number, const char * message ) = NULL );
		void get_iref(double & xhrs, double & yhrs);
		void get_xref(void);
		void dem_rc(int status, int & ncol, int & nrow);
		void dem_head(int);
		void dem_mbr(int status, double & SWX, double & SWY, double & NWX, double & NWY, double & NEX, double & NEY, double & SEX, double & SEY);
		void cell_range(int status, long & voidvalue, long & fillvalue);
		void cells_out(const char * filename, int status, long fillvalue, void (*callback)(int number, const char * message ) = NULL );
		void get_nw_corner(double & upperlx, double & upperly);
		int  s123tol2(char *,long *,int);
		bool read_sdts_header(char * filename, fHeader & h, long & fillvalue);
		void baseAndId();
		char FeetOrMeters();

	private:

		fHeader gridHeader;
		bool isInRam;
		float ** data;
		float max;
		float min;
		GRID_TYPE gridType;
		CString gridFilename;

		//ASCII Specific Variables
			//Flags used to adjust reference to cell center
			bool xllcorner;
			bool yllcorner;
		//BINARY Specific Variables
			float * row_one;
			float * row_two;	//Current row
			float * row_three;
			long current_row;
			FILE * file_in_out;
			long file_position_beg_of_data;			
			DATA_TYPE data_type;
		//ESRI Specific Variables
			int grid_layer;		
			void * row_buf1;		
			void * row_buf2;
			void * row_buf3;	
			float float_null;
		//SDTS Specific Variables
			FILE *fpin;
			long int_level;
			double sfax, sfay, xorg, yorg;
			double x[5];
			double y[5];
			long nxy;
			long str_len;
			long li;
			int stat2;
			int status;
			char temp[100];
			char ice;
			char leadid;
			char ccs[4];
			char tag[10];
			char fdlen[10];
			char *fdname;
			char mod_name[10];
			char base_name[MAX_STRING_LENGTH];
			char file_name[MAX_STRING_LENGTH];
			char string[5000];
			char descr[5000];
			char frmts[500];
			int order;
			char rsnm[5];

		//Flags used to indicate if the max or min value has been overwritten
		bool findNewMax;		
		bool findNewMin;
		
		long lastErrorCode;
};

# endif
//-------------------------------------------
//-- fGrid.h
//-------------------------------------------
//-------------------------------------------
//-- dGrid.h
//-------------------------------------------
# ifndef DGRID_H
# define DGRID_H

class __declspec( dllimport ) dGrid
{
	public:
		
		//CONSTRUCTORS
		dGrid();
		~dGrid();
		
		//OPERATORS
		double operator()( int Column, int Row );
				
		//FUNCTIONS
		bool open( const char * Filename, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION, void (*callback)( int number, const char * message ) = NULL );
		bool initialize( const char * Filename, dHeader Header, double initialValue, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION );		
		bool save( const char * Filename = "", GRID_TYPE GridType = USE_EXTENSION, void (*callback)(int number, const char * message ) = NULL );
		bool close();
		void clear(double clearValue);

		//MAPPING FUNCTIONS
		void ProjToCell( double x, double y, long & column, long & row );
		void CellToProj( long column, long row, double & x, double & y );
				
		//DATA MEMBER ACCESS
		inline dHeader getHeader();
		void setHeader( dHeader h );
		inline double getValue( int Column, int Row );
		inline void setValue( int Column, int Row, double Value );
		bool inRam();		
		double maximum();
		double minimum();
		long LastErrorCode();

		double ** getArrayPtr();

	private:
		inline int round( double d );
		GRID_TYPE getGridType( const char * filename );	
		void dealloc();
		void alloc();		
		inline bool inGrid( long column, long row );
		inline double getValueDisk( int column, int row );
		inline void setValueDisk( int column, int row, double value );
		void clearDisk(double clearValue);

		bool readDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool readDiskToDisk( void (*callback)(int number, const char * message ) = NULL );
		bool writeMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool writeDiskToDisk();

		//ASCII GRID FUNCTIONS		
		bool asciiReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool asciiReadDiskToDisk( void (*callback)(int number, const char * message ) = NULL );
		bool asciiWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool asciiWriteDiskToDisk();		
		void asciiReadHeader( std::istream & in );
		void asciiReadFooter( std::istream & in );
		void asciiWriteHeader( std::ostream & out );
		void asciiWriteFooter( std::ostream & out );
		bool asciiIsHeaderValue( CString headerValue, std::istream & in );
		bool asciiSaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//BINARY GRID FUNCTIONS
		bool binaryReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool binaryReadDiskToDisk();
		bool binaryWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool binaryWriteDiskToDisk();		
		bool binaryInitializeDisk( double InitialValue );			
		void binaryReadHeader( FILE * in );
		void binaryWriteHeader( FILE * out );
		double binaryGetValueDisk( int Column, int Row );
		void binarySetValueDisk( int Column, int Row, double Value );
		void binaryClearDisk(double clearValue);
		void binaryBufferRows( int CenterRow );
		bool binarySaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//ESRI GRID FUNCTIONS
		bool esriReadDiskToMemory( void (*callback)(int number, const char * message ) = NULL );
		bool esriReadDiskToDisk();
		bool esriWriteMemoryToDisk( void(*callback)(int number, const char * message ) = NULL );
		bool esriWriteDiskToDisk();		
		bool esriInitializeDisk( double InitialValue );			
		double esriGetValueDisk( int Column, int Row );
		void esriSetValueDisk( int Column, int Row, double Value );
		void esriClearDisk(double clearValue);
		void esriBufferRows( int CenterRow );
		bool esriSaveAs( CString filename, void(*callback)(int number, const char * message) = NULL );

		//SDTS GRID FUNCTIONS
		bool sdtsReadDiskToMemory( void(*callback)(int number, const char * message ) = NULL );
		void get_iref(double & xhrs, double & yhrs);
		void get_xref(void);
		void dem_rc(int status, int & ncol, int & nrow);
		void dem_head(int);
		void dem_mbr(int status, double & SWX, double & SWY, double & NWX, double & NWY, double & NEX, double & NEY, double & SEX, double & SEY);
		void cell_range(int status, long & voidvalue, long & fillvalue);
		void cells_out(const char * filename, int status, long fillvalue, void (*callback)(int number, const char * message ) = NULL );
		void get_nw_corner(double & upperlx, double & upperly);
		int  s123tol2(char *,long *,int);
		bool read_sdts_header(char * filename, dHeader & h, long & fillvalue);
		void baseAndId();
		char FeetOrMeters();
		

	private:

		dHeader gridHeader;
		bool isInRam;
		double ** data;
		double max;
		double min;
		GRID_TYPE gridType;
		CString gridFilename;

		//ASCII Specific Variables
			//Flags used to adjust reference to cell center
			bool xllcorner;
			bool yllcorner;
		//BINARY Specific Variables
			double * row_one;
			double * row_two;	//Current row
			double * row_three;
			long current_row;
			FILE * file_in_out;
			long file_position_beg_of_data;			
			DATA_TYPE data_type;
		//ESRI Specific Variables
			int grid_layer;		
			void * row_buf1;		
			void * row_buf2;
			void * row_buf3;
			float float_null;
		//SDTS Specific Variables
			FILE *fpin;
			long int_level;
			double sfax, sfay, xorg, yorg;
			double x[5];
			double y[5];
			long nxy;
			long str_len;
			long li;
			int stat2;
			int status;
			char temp[100];
			char ice;
			char leadid;
			char ccs[4];
			char tag[10];
			char fdlen[10];
			char *fdname;
			char mod_name[10];
			char base_name[MAX_STRING_LENGTH];
			char file_name[MAX_STRING_LENGTH];
			char string[5000];
			char descr[5000];
			char frmts[500];
			int order;
			char rsnm[5];
		
		//Flags used to indicate if the max or min value has been overwritten
		bool findNewMax;		
		bool findNewMin;
		
		long lastErrorCode;
};

# endif
//-------------------------------------------
//-- dGrid.h
//-------------------------------------------
//-------------------------------------------
//-- GridManager.h
//-------------------------------------------
# ifndef GRIDMANAGER_H
# define GRIDMANAGER_H

class __declspec( dllimport ) GridManager
{
public:
	GridManager();
	~GridManager();
	
	DATA_TYPE getGridDataType( const char * filename, GRID_TYPE GridType = USE_EXTENSION );
	GRID_TYPE getGridType( const char * filename );
	bool canUseEsriGrids();
	bool deleteGrid( const char * filename, GRID_TYPE GridType = USE_EXTENSION );
};

# endif
//-------------------------------------------
//-- GridManager.h
//-------------------------------------------

//-------------------------------------------
//-- header.h
//-------------------------------------------
# ifndef HEADER_H
# define HEADER_H

class __declspec( dllimport ) header
{
	public:		
		//CONSTRUCTORS
		header();
		header( const header & h );

		//Destructor
		~header();

		//OPERATORS
		header operator=( const header & h );

		//DATA ACCESS MEMBERS
		inline long getNumberCols();
		inline long getNumberRows();
		inline double getNodataValue();
		inline double getDx();
		inline double getDy();
		inline double getXllcenter();
		inline double getYllcenter();
		char * getProjection();
		char * getNotes();

		void setNumberCols( long p_ncols );
		void setNumberRows( long p_nrows );
		void setNodataValue( double p_nodata_value );
		void setDx( double p_dx );
		void setDy( double p_dy );
		void setXllcenter( double p_xllcenter );
		void setYllcenter( double p_yllcenter );
		void setProjection( const char * p_projection );
		void setNotes( const char * p_notes );

	private:
		long ncols;
		long nrows;
		double nodataValue;
		double dx;
		double dy;
		double xllcenter;
		double yllcenter;
		CString projection;
		CString notes;

};

# endif
//-------------------------------------------
//-- header.h
//-------------------------------------------

//-------------------------------------------
//-- Grid.h
//-------------------------------------------
# ifndef GRID_H
# define GRID_H

class __declspec( dllimport ) grid
{
	public:
		
		//CONSTRUCTORS
		grid();
		~grid();
		
		//OPERATORS
		double operator()( int Column, int Row );
				
		//FUNCTIONS
		bool open( const char * Filename, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION, void (*callback)( int number, const char * message ) = NULL );
		bool initialize( const char * Filename, header pheader, double initialValue, DATA_TYPE DataType = DOUBLE_TYPE, bool InRam = true, GRID_TYPE GridType = USE_EXTENSION );		
		bool save( const char * Filename = "", GRID_TYPE GridType = USE_EXTENSION, void (*callback)(int number, const char * message ) = NULL );
		bool close();
		void clear(double clearValue);

		//MAPPING FUNCTIONS
		void ProjToCell( double x, double y, long & column, long & row );
		void CellToProj( long column, long row, double & x, double & y );
				
		//DATA MEMBER ACCESS
		inline header getHeader();
		void setHeader( header h );
		inline double getValue( int Column, int Row );
		inline void setValue( int Column, int Row, double Value );
		bool inRam();		
		double maximum();
		double minimum();
		long LastErrorCode();

		DATA_TYPE getDataType();
	private:

		header copyToHeader();
		dHeader dh_copyFromHeader( header & h );
		fHeader fh_copyFromHeader( header & h );
		lHeader lh_copyFromHeader( header & h );
		sHeader sh_copyFromHeader( header & h );

		dGrid * dgrid;
		fGrid * fgrid;
		lGrid * lgrid;
		sGrid * sgrid;

		long lastErrorCode;
};

# endif
//-------------------------------------------
//-- Grid.h
//-------------------------------------------
