# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: G123FSTR
**
**    PURPOSE: TO READ A FULL STRING INTO AN CHARACTER BUFFER
**
**    INVOCATION METHOD: G123FSTR(FP, BUF_STR, STR_LEN)
**
**    ARGUMENT LIST:
**     NAME       TYPE      USE      DESCRIPTION
**     FP         PTR        I       FILE POINTER
**     BUF_STR[]  PTR       I/O      POINTER TO A BUFFER 
**     STR_LEN    INT        I       LENGTH OF STRING TO BE READ IN
**     G123FSTR() LOGICAL    O       SUCCESS FLAG 
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     DIV_RESULT L_DIV              NUMBER OF TIMES TO READ FROM FILE
**     I          INT                LOOP CONTROL VARIABLE
**     LEN        INT                INDEX OF BUFFER POINTER
**     TMPCPTR[]  PTR                TEMPORARY CHARACTER POINTER
**
**    C LIBRARY STRUCTURE:
**     LDIV_T STRUCTURE
**      NAME      TYPE               DESCRIPTION
**      QUOT      INT                RESULTING QUOTIENT FROM INTEGER DIVISION
**      REM       INT                RESULTING REMAINDER FROM INTEGER DIVISION
**
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
**  
**    GLOBAL CONSTANTS:
**    NAME          TYPE             DESCRIPTION
**    MAXINT        INT              MAXIMUM VALUE OF TWO-BYTE INTEGER
**    NC            CHAR             NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     L. MCMILLION              05/09/90  INITIAL CODE
**     A. DEWITT                 08/14/90  REMOVED CALL TO FUNCTION FEOF
**                                          IN WHILE CONDITION
**     A. DEWITT      92DR005    04/02/92  IMPLEMENT BINARY DATA
**     L. MCMILLION  TASK #40    12/15/92  UPDATED PROLOG PER QC
**     J. TAYLOR      93DR037    07/28/93  REMOVED MEMSET CALLS
**     J. TAYLOR      93DR039    07/28/93  REMOVED MEMSET CALLS
**
**    PDL:
**
**     INITIALIZE LOOP CONTROL VARIABLE TO ONE
**     INITIALIZE LEN
**     COMPUTE NUMBER OF TIMES TO READ STRING 
**     INITIALIZE STRING TO EMPTY 
**     SET TEMPORARY CHARACTER POINTER TO BUF_PTR 
**     WHILE HAVE MORE READS DO 
**        IF QUOTIENT IS GREATER THAN ZERO { STR_LEN > MAXINT } 
**        THEN
**           READ IN CHARACTER STRING 
**           TERMINATE TMPCPTR WITH NULL CHARACTER
**           INCREMENT STRING POINTER 
**        ELSE
**           READ IN CHARACTER STRING 
**           TERMINATE TMPCPTR WITH NULL CHARACTER
**        ENDIF
**        DECREMENT QUOTIENT COUNTER 
**     END WHILE
**     IF LEN NOT EQUAL TO STR_LEN
**     THEN
**        RETURN FAILURE 
**     ENDIF
**
**     RETURN SUCCESS 
**    
*****************************************************************************
**     CODE SECTION
**
*****************************************************************************/ 
#include "stc123.h"

int g123fstr(FILE* fp, char* buf_str, long str_len)
{
	/* LOCAL VARIABLES */
	char* tmpCptr;
	long remaining;

	/* BASIC VALIDATION */
	if (fp == nullptr || buf_str == nullptr || str_len < 0)
		return(0);

	/* INITIALIZE POINTERS */
	tmpCptr = buf_str;
	remaining = str_len;

	/* READ THE FULL STRING IN CHUNKS */
	while(remaining > 0) {
		size_t chunk = (remaining > MAXINT) ? static_cast<size_t>(MAXINT) : static_cast<size_t>(remaining);
		size_t read_len = fread(tmpCptr, sizeof(char), chunk, fp);

		if (read_len != chunk) return(0);

		tmpCptr += read_len;
		remaining -= static_cast<long>(read_len);
	}

	/* APPEND NULL CHARACTER ONCE, AT THE END */
	*tmpCptr = NC;

	/* RETURN SUCCESS */
	return(1);
}