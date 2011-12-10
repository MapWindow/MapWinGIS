#include "stdafx.h"
#include "dbf.h"
#include "shapefil.h"

#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#ifndef FALSE
#  define FALSE		0
#  define TRUE		1
#endif

DBFHandle SHPAPI_CALL
DBFOpen_MW( const char * pszFilename, const char * pszAccess )
{
	return DBFOpen(pszFilename, pszAccess);
}

/************************************************************************/
/*                             SfRealloc()                              */
/*                                                                      */
/*      A realloc cover function that will access a NULL pointer as     */
/*      a valid input.                                                  */
/************************************************************************/

static void * SfRealloc( void * pMem, int nNewSize )

{
    if( pMem == NULL )
        return( (void *) malloc(nNewSize) );
    else
        return( (void *) realloc(pMem,nNewSize) );
}
