# ifndef GRID_TYPES_H
# define GRID_TYPES_H


# ifndef MAX_STRING_LENGTH
# define MAX_STRING_LENGTH 255
# endif

# ifndef GRID_TYPES
# define GRID_TYPES

#include "stdafx.h"

	enum GRID_TYPE	
	{ ASCII_GRID,
	  BINARY_GRID,
	  ESRI_GRID,
	  GEOTIFF_GRID, //added 8/15/05 -- ah
	  SDTS_GRID,
	  PAUX,
	  PCIDSK,
	  DTED,
	  BIL,
	  ECW,
	  MRSID,
	  FLT,
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
	  INVALID_DATA_TYPE = -1,
	  BYTE_TYPE
	};

# endif

# ifndef CONV_TO_SHAPES
# define CONV_TO_SHAPES

# define BORDER             -2225
# define DOUBLE_BORDER      -2226
# define DECISION           -2227
# define TRACE_BORDER       -2228
# define CURRENT_POLYGON	-2229

	/*enum DIRECTION
	{ NONE,
	  RIGHT,
	  UPRIGHT,
	  UP,
	  UPLEFT,
	  LEFT,
	  DOWNLEFT,
	  DOWN,
	  DOWNRIGHT
	};*/

# endif

# ifndef CINFO_COLOR_TYPE
# define CINFO_COLOR_TYPE

	enum ctColoringType
	{
		ctHILLSHADE,	// ctHILLSHADE = 0,
		ctGRADIENT,		// ctGRADIENT = 1,
		ctRANDOM,		// ctRANDOM = 2,
	};

	enum gmGradientModel
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