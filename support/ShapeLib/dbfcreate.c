/******************************************************************************
 *
 * Project:  Shapelib
 * Purpose:  Sample application for creating a new .dbf file.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shapefil.h"

SHP_CVSID("$Id$")

int main(int argc, char ** argv) {
    // Display a usage message.
    if( argc < 2 ) {
        printf("dbfcreate xbase_file [[-s field_name width], "
               "[-n field_name width decimals]]...\n");
        return 1;
    }

    // Create the database.
    DBFHandle hDBF = DBFCreate( argv[1] );
    if( hDBF == NULL ) {
        printf("DBFCreate(%s) failed.\n", argv[1]);
        return 2;
    }

    // Loop over the field definitions adding new fields.
    for( int i = 2; i < argc; i++ ) {
        if(i < argc - 2 && strcmp(argv[i], "-s") == 0) {
            const char *field = argv[i+1];
            const int width = atoi(argv[i+2]);
            const int decimals = 0;
            if(DBFAddField(hDBF, field, FTString, width, decimals) == -1) {
                printf("DBFAddField(%s,FTString,%d,0) failed.\n", field, width);
                DBFClose(hDBF);
                return 4;
            }
            i += 2;
        } else if(i < argc - 3 && strcmp(argv[i], "-n") == 0) {
            const char *field = argv[i+1];
            const int width = atoi(argv[i+2]);
            const int decimals = atoi(argv[i+3]);
            if(DBFAddField(hDBF, field, FTDouble, width, decimals) == -1) {
                printf("DBFAddField(%s,FTDouble,%d,%d) failed.\n", field, width, decimals);
                DBFClose(hDBF);
                return 4;
            }
            i += 3;
        } else {
            printf("Argument incomplete, or unrecognised: %s\n", argv[i]);
            DBFClose(hDBF);
            return 3;
        }
    }

    DBFClose(hDBF);
    return 0;
}
