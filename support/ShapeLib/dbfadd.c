/******************************************************************************
 *
 * Project:  Shapelib
 * Purpose:  Sample application for adding a record to an existing .dbf file.
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
 */

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "shapefil.h"

SHP_CVSID("$Id$")

int main(int argc, char ** argv) {
    if (argc < 3) {
        printf("dbfadd xbase_file field_values\n");
        return EXIT_FAILURE;
    }

    DBFHandle hDBF = DBFOpen(argv[1], "r+b");
    if (hDBF == NULL) {
        printf("DBFOpen(%s,\"rb+\") failed.\n", argv[1]);
        return 2;
    }

    // Do we have the correct number of arguments?
    if (DBFGetFieldCount(hDBF) != argc - 2) {
        printf("Got %d fields, but require %d\n",
               argc - 2, DBFGetFieldCount(hDBF));
        DBFClose(hDBF);
        return 3;
    }

    const int iRecord = DBFGetRecordCount(hDBF);

    // Loop assigning the new field values.
    for (int i = 0; i < DBFGetFieldCount(hDBF); i++) {
        if (strcmp( argv[i+2], "" ) == 0)
            DBFWriteNULLAttribute(hDBF, iRecord, i);
        else if (DBFGetFieldInfo( hDBF, i, NULL, NULL, NULL ) == FTString)
            DBFWriteStringAttribute(hDBF, iRecord, i, argv[i+2]);
        else
            DBFWriteDoubleAttribute(hDBF, iRecord, i, atof(argv[i+2]));
    }

    DBFClose(hDBF);

    return 0 ;
}
