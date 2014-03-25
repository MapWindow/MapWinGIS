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

DBFHandle SHPAPI_CALL DBFOpen_MW( CStringW pszFilename, const char * pszAccess )
{
	CStringA nameA = Utility::ConvertToUtf8(pszFilename);
	m_globalSettings.SetGdalUtf8(true);
	DBFHandle handle = DBFOpen(nameA, pszAccess);
	m_globalSettings.SetGdalUtf8(false);
	return handle;
}

DBFHandle SHPAPI_CALL DBFCreate_MW( CStringW nameW )
{
	CStringA nameA = Utility::ConvertToUtf8(nameW);
	m_globalSettings.SetGdalUtf8(true);
	DBFHandle handle = DBFCreate(nameA);
	m_globalSettings.SetGdalUtf8(false);
	return handle;
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
