# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: GVD123.H
**
**    PURPOSE: TO DECLARE GLOBAL VARIABLES USED IN SELECTED FUNCTIONS
**
**    INVOCATION METHOD: #INCLUDE "GVD123.H"
**
**    ARGUMENT LIST: NONE
**     
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES: NONE
**  
**    GLOBAL VARIABLES:
**     NAME             TYPE     USE   DESCRIPTION
**     ASCII            INT      I/O   ASCII CONVERSION FLAG
**     CUR_FM           PTR      I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                      STRUCTURE ENTRY
**     EBCDIC           INT      I/O   EBCDIC CONVERSION FLAG
**     FM_HD            PTR      I/O   HEAD POINTER TO LIST OF OPEN FILES
**     GLB_STR[MAXSIZ]  CHAR     I/O   GLOBAL CHARACTER STRING USED FOR
**                                      PROCESSING
**     GLB_STR2[MAXSIZ] CHAR     I/O   SECOND GLOBAL CHARACTER STRING USED FOR
**                                      PROCESSING
**     INIT_DONE        LOGICAL  I/O   INITIALIZATION FLAG OF FM STRUCTURE
**
**    GLOBAL CONSTANTS:
**     NAME          TYPE      DESCRIPTION
**     DEL_STR[3]    CHAR      CHARACTER STRING CONTAINING THE FIELD AND UNIT
**                              TERMINATORS
**     FT            CHAR      FIELD TERMINATOR (RS) 1/14
**     FT_STR[2]     CHAR      CHARACTER STRING CONTAINING THE FIELD
**                              TERMINATOR
**     SEP           CHAR      SEPARATOR CHARACTER--ASCII DECIMAL CODE 255
**     SEP_STR[2]    CHAR      CHARACTER STRING CONTAINING SEPARATOR FOR
**                              ADJACENT DELIMITERS
**     UT            CHAR      UNIT TERMINATOR (US) 1/15
**     UT_STR[2]     CHAR      CHARACTER STRING CONTAINING THE UNIT
**                              TERMINATOR
**     MAXSIZ        INT       MAXIMUM AMOUNT OF CONTIGUOUS MEMORY SPACE
**
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     L. MCMILLION              08/02/90  INITIAL PROLOG
**     A. DEWITT                 08/02/90  INTIAL CODE
**     L. MCMILLION              10/02/90  ADDED SECOND GLOBAL STRING FOR
**                                          PROCESSING
**     L. MCMILLION  92DR009     04/14/92  ADDED SEPARATOR FOR ADJACENT
**                                          DELIMITERS
**     J. TAYLOR     TASK 55     01/12/93  ADDED ASCII/EBCDIC CONVERSION
**                                          FLAGS
**     J. TAYLOR     TASK 55     02/02/93  CHANGED INITIALIZATION OF GLOBAL
**                                          CHARACTER STRING CONSTANTS WITH   
**                                          THE NULL CHARACTER TO A ZERO VALUE
**                                          FOR PORTABILTY                  
**
****************************************************************************
**    CODE SECTION
**
****************************************************************************/

char DEL_STR[3] = {30,31,0};
char FT = 30;
char FT_STR[2] = {30,0};
char SEP = (char)255;
char SEP_STR[2] = {(char)255,0};
char UT = 31;
char UT_STR[2] = {31,0};

int ASCII = 0;
int EBCDIC = 0;
struct fm *cur_fm             ;
struct fm *fm_hd              ;
char glb_str[MAXSIZ]          ;
char glb_str2[MAXSIZ]         ;
static int init_done = 0      ;
