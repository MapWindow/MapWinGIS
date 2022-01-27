/******************************************************************************
 *
 * Project:  Shapelib
 * Purpose:  Sample application for dumping .dbf files to the terminal.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 1999, Frank Warmerdam
 *
 * This software is available under the following "MIT Style" license,
 * or at the option of the licensee under the LGPL (see COPYING).  This
 * option is discussed in more detail in shapelib.html.
 *
 * --
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shapefil.h"

SHP_CVSID("$Id$")

int main( int argc, char ** argv ) {
/* -------------------------------------------------------------------- */
/*      Handle arguments.                                               */
/* -------------------------------------------------------------------- */
    bool bHeader = false;
    bool bRaw = false;
    bool bMultiLine = false;
    char *pszFilename = NULL;

    for( int i = 1; i < argc; i++ )
    {
        if( strcmp(argv[i],"-h") == 0 )
            bHeader = true;
        else if( strcmp(argv[i],"-r") == 0 )
            bRaw = true;
        else if( strcmp(argv[i],"-m") == 0 )
            bMultiLine = true;
        else
            pszFilename = argv[i];
    }

/* -------------------------------------------------------------------- */
/*      Display a usage message.                                        */
/* -------------------------------------------------------------------- */
    if( pszFilename == NULL )
    {
	printf( "dbfdump [-h] [-r] [-m] xbase_file\n" );
        printf( "        -h: Write header info (field descriptions)\n" );
        printf( "        -r: Write raw field info, numeric values not reformatted\n" );
        printf( "        -m: Multiline, one line per field.\n" );
	exit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Open the file.                                                  */
/* -------------------------------------------------------------------- */
    DBFHandle hDBF = DBFOpen( pszFilename, "rb" );
    if( hDBF == NULL )
    {
	printf( "DBFOpen(%s,\"r\") failed.\n", argv[1] );
	exit( 2 );
    }

/* -------------------------------------------------------------------- */
/*	If there is no data in this file let the user know.		*/
/* -------------------------------------------------------------------- */
    if( DBFGetFieldCount(hDBF) == 0 )
    {
	printf( "There are no fields in this table!\n" );
        DBFClose(hDBF);
	exit( 3 );
    }

/* -------------------------------------------------------------------- */
/*	Dump header definitions.					*/
/* -------------------------------------------------------------------- */
    char szTitle[12];
    int nWidth;
    int nDecimals;

    if( bHeader )
    {
        for( int i = 0; i < DBFGetFieldCount(hDBF); i++ )
        {
            const char chNativeType = DBFGetNativeFieldType( hDBF, i );
            const DBFFieldType eType = DBFGetFieldInfo( hDBF, i, szTitle, &nWidth, &nDecimals );

            const char *pszTypeName = NULL;
            if( eType == FTString )
                pszTypeName = "String";
            else if( eType == FTInteger )
                pszTypeName = "Integer";
            else if( eType == FTDouble )
                pszTypeName = "Double";
            else if( eType == FTInvalid )
                pszTypeName = "Invalid";
            // TODO(schwehr): else?

            printf( "Field %d: Type=%c/%s, Title=`%s', Width=%d, Decimals=%d\n",
                    i, chNativeType, pszTypeName, szTitle, nWidth, nDecimals );
        }
    }

/* -------------------------------------------------------------------- */
/*	Compute offsets to use when printing each of the field 		*/
/*	values. We make each field as wide as the field title+1, or 	*/
/*	the field value + 1. 						*/
/* -------------------------------------------------------------------- */
    int *panWidth = (int *) malloc( DBFGetFieldCount( hDBF ) * sizeof(int) );
    char szFormat[32];

    for( int i = 0; i < DBFGetFieldCount(hDBF) && !bMultiLine; i++ )
    {
	const DBFFieldType eType = DBFGetFieldInfo( hDBF, i, szTitle, &nWidth, &nDecimals );
	if( (int) strlen(szTitle) > nWidth )
	    panWidth[i] = strlen(szTitle);
	else
	    panWidth[i] = nWidth;

	if( eType == FTString )
	    sprintf( szFormat, "%%-%ds ", panWidth[i] );
	else
	    sprintf( szFormat, "%%%ds ", panWidth[i] );
	printf( szFormat, szTitle );
    }
    printf( "\n" );

/* -------------------------------------------------------------------- */
/*	Read all the records 						*/
/* -------------------------------------------------------------------- */
    for( int iRecord = 0; iRecord < DBFGetRecordCount(hDBF); iRecord++ )
    {
        if( bMultiLine )
            printf( "Record: %d\n", iRecord );

	for( int i = 0; i < DBFGetFieldCount(hDBF); i++ )
	{
            const DBFFieldType eType = DBFGetFieldInfo( hDBF, i, szTitle, &nWidth, &nDecimals );

            if( bMultiLine )
            {
                printf( "%s: ", szTitle );
            }

/* -------------------------------------------------------------------- */
/*      Print the record according to the type and formatting           */
/*      information implicit in the DBF field description.              */
/* -------------------------------------------------------------------- */
            if( !bRaw )
            {
                if( DBFIsAttributeNULL( hDBF, iRecord, i ) )
                {
                    if( eType == FTString )
                        sprintf( szFormat, "%%-%ds", nWidth );
                    else
                        sprintf( szFormat, "%%%ds", nWidth );

                    printf( szFormat, "(NULL)" );
                }
                else
                {
                    switch( eType )
                    {
                      case FTString:
                        sprintf( szFormat, "%%-%ds", nWidth );
                        printf( szFormat,
                                DBFReadStringAttribute( hDBF, iRecord, i ) );
                        break;

                      case FTInteger:
                        sprintf( szFormat, "%%%dd", nWidth );
                        printf( szFormat,
                                DBFReadIntegerAttribute( hDBF, iRecord, i ) );
                        break;

                      case FTDouble:
                        sprintf( szFormat, "%%%d.%dlf", nWidth, nDecimals );
                        printf( szFormat,
                                DBFReadDoubleAttribute( hDBF, iRecord, i ) );
                        break;

                      default:
                        break;
                    }
                }
            }

/* -------------------------------------------------------------------- */
/*      Just dump in raw form (as formatted in the file).               */
/* -------------------------------------------------------------------- */
            else
            {
                sprintf( szFormat, "%%-%ds", nWidth );
                printf( szFormat,
                        DBFReadStringAttribute( hDBF, iRecord, i ) );
            }

/* -------------------------------------------------------------------- */
/*      Write out any extra spaces required to pad out the field        */
/*      width.                                                          */
/* -------------------------------------------------------------------- */
            if( bMultiLine ) {
                printf( "\n" );
            } else {
		sprintf( szFormat, "%%%ds", panWidth[i] - nWidth + 1 );
		printf( szFormat, "" );
	    }

	    fflush( stdout );
	}

        if( DBFIsRecordDeleted(hDBF, iRecord) )
            printf( "(DELETED)" );

	printf( "\n" );
    }

    DBFClose( hDBF );
    free( panWidth );

    return( 0 );
}
