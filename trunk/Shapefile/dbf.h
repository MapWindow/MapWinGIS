#include "shapefil.h"
#include <stdio.h>

#ifdef USE_DBMALLOC
#include <dbmalloc.h>
#endif

#ifdef USE_CPL
#include "cpl_error.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

const wchar_t* Utf8ToWideChar( const char *pszFilename );

DBFHandle DBFOpen_MW( CStringW pszDBFFile, const char * pszAccess );
DBFHandle DBFCreate_MW( CStringW nameW );
void * SfRealloc( void * pMem, int nNewSize );

#ifdef __cplusplus
}
#endif

/******************** END OF CODE ***************************/