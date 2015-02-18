# ifndef DBF_TYPES_H
# define DBF_TYPES_H

# ifndef DBF_FIELDTYPE_STRUCT
# define DBF_FIELDTYPE_STRUCT

	typedef enum
	{
	  FTString,
	  FTInteger,
	  FTDouble,
	  FTInvalid
	} DBFFieldType;

# endif

# ifndef DBF_NUMBER_NODATA_VALUE
# define DBF_NUMBER_NODATA_VALUE -99999
# endif

# ifndef MAX_FIELD_NAME_SIZE
# define MAX_FIELD_NAME_SIZE 12
# endif

# endif