# include "stdafx.h"
/****************************************************************************
** 
**    INVOCATION NAME: IS123INTGR
** 
**    PURPOSE: TO DETERMINE IF A STRING IS AN INTEGER
** 
**    INVOCATION METHOD: IS123INTGR(STRING)
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION
**     STRING[]      PTR        I       CHARACTER STRING
**     IS123INTGR()  LOGICAL    O       SUCCESS FLAG
**      
**    EXTERNAL FUNCTION REFERENCES: NONE
** 
**    INTERNAL VARIABLES:
**     NAME          TYPE               DESCRIPTION
**     NUM           INT                STRING IN INTEGER FORMAT
**   
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
** 
**    GLOBAL CONSTANTS:
**     NAME          TYPE               DESCRIPTION
**     NC            CHAR               NULL CHARACTER
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     A. DEWITT                 06/18/90  INITIAL PROLOG
**     A. DEWITT                 06/18/90  INITIAL PDL
**     A. DEWITT                 06/18/90  INITIAL CODE
**     A. DEWITT                 08/15/90  ADDED THIRD ARGUMENT TO SSCANF
**                                          FUNCTION CALL
** 
**    PDL: 
**
**     IF STRING NULL
**     THEN
**        RETURN FAILURE
**     ENDIF
**     READ STRING INTO NUMBER
**     IF NUMBER LESS THAN OR EQUAL TO ZERO
**     THEN
**        RETURN FAILURE
**     ENDIF
**
**     RETURN SUCCESS 
** 
****************************************************************************** 
**    CODE SECTION 
**
*****************************************************************************/
#include "stc123.h"

int is123intgr(char *string)
{
   /* INTERNAL VARIABLES */
   long num;

   /* IF STRING NULL, RETURN FAILURE */
   if (*string == NC) return(0);

   /* READ STRING INTO NUMBER */
   if (sscanf(string,"%ld",&num) <= 0) return(0);

   /* RETURN SUCCESS */
   return(1);
}
