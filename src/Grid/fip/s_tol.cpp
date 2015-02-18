# include "stdafx.h"
/********************+*******************************************************
** 
**    INVOCATION NAME: S123TOL
** 
**    PURPOSE: TO CONVERT A CHARACTER STRING TO A LONG INTEGER AND
**              CHANGE ITS BYTE ORDERING WHEN SPECIFIED
** 
**    INVOCATION METHOD: S123TOL(STRING,NUM,REVERSE)
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION
**     STRING[]      PTR        I       CHARACTER STRING
**     NUM           LONG       O       NUMBER TO BE CONVERTED
**     REVERSE       LOGICAL    I       REVERSE BYTE ORDER FLAG
**                                       1 - REVERSE
**                                       0 - DO NOT REVERSE
**     S123TOL()     LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES: NONE
** 
**    INTERNAL VARIABLES:
**     NAME          TYPE               DESCRIPTION
**     U             UNION              UNION OF LONG INTEGER AND WORK STRING
**     WNUM          LONG               WORK LONG
**     WSTR[]        CHAR               WORK STRING
**   
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
** 
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     J. TAYLOR     92DR018     12/09/92  INITIAL PROLOG
**     J. TAYLOR     92DR018     12/09/92  INITIAL PDL
**     J. TAYLOR     92DR018     12/09/92  INITIAL CODE
** 
**    PDL: 
**
**     IF BYTE ORDER TO BE REVERSED
**     THEN
**        MOVE INPUT STRING TO WORK STRING IN REVERSE BYTE ORDER
**     ELSE
**        MOVE INPUT STRING TO WORK STRING WITH NO CHANGE IN BYTE ORDER
**     ENDIF
**     MOVE WORK LONG INTEGER TO OUTPUT LONG INTEGER
**       
**     RETURN SUCCESS 
** 
****************************************************************************** 
**    CODE SECTION 
**
*****************************************************************************/
#include "stc123.h"

int s123tol(char *string,long *num,int reverse)

{
   /* INTERNAL VARIABLES */
   union { long wnum; char wstr[sizeof(long)]; } u;
      

   /* IF BYTE ORDER TO BE REVERSED */
   if (reverse) {

      /* MOVE INPUT STRING TO WORK STRING IN REVERSE BYTE ORDER */
      u.wstr[0] = string[3];
      u.wstr[1] = string[2];
      u.wstr[2] = string[1];
      u.wstr[3] = string[0];
   }

   /* ELSE */
   else {

      /* MOVE INPUT STRING TO WORK STRING WITH NO CHANGE IN BYTE ORDER */
      u.wstr[0] = string[0];
      u.wstr[1] = string[1];
      u.wstr[2] = string[2];
      u.wstr[3] = string[3];
   }
   
   /* MOVE WORK LONG INTEGER TO OUTPUT LONG INTEGER */
   *num = u.wnum;
      
   
   /* RETURN SUCCESS */
   return(1);
}
