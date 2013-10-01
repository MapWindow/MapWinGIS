# include "stdafx.h"
/********************+*******************************************************
** 
**    INVOCATION NAME: L123TOS
** 
**    PURPOSE: TO CONVERT A LONG INTEGER TO A CHARACTER STRING AND
**              CHANGE ITS BYTE ORDERING WHEN SPECIFIED
** 
**    INVOCATION METHOD: L123TOS(NUM,STRING,REVERSE)
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION
**     NUM           LONG       I       NUMBER TO BE CONVERTED
**     REVERSE       LOGICAL    I       REVERSE BYTE ORDER FLAG
**                                       1 - REVERSE
**                                       0 - DO NOT REVERSE
**     STRING[]      PTR        O       CHARACTER STRING
**     L123TOS()     LOGICAL    O       SUCCESS FLAG
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
**     MOVE INPUT LONG INTEGER TO WORK LONG INTEGER
**     IF BYTE ORDER TO BE REVERSED
**     THEN
**        MOVE WORK STRING TO OUTPUT STRING IN REVERSE BYTE ORDER
**     ELSE
**        MOVE WORK STRING TO OUTPUT STRING WITH NO CHANGE IN BYTE ORDER
**     ENDIF
**       
**     RETURN SUCCESS 
** 
****************************************************************************** 
**    CODE SECTION 
**
*****************************************************************************/
#include "stc123.h"

int l123tos(long num,char *string,int reverse)
{
   /* INTERNAL VARIABLES */
   union { long wnum; char wstr[sizeof(long)]; } u;
      
   /* MOVE INPUT LONG INTEGER TO WORK LONG INTEGER */
   u.wnum = num;
      

   /* IF BYTE ORDER TO BE REVERSED */
   if (reverse) {

      /* MOVE WORK STRING TO OUTPUT STRING IN REVERSE BYTE ORDER */
      string[0] = u.wstr[3];
      string[1] = u.wstr[2];
      string[2] = u.wstr[1];
      string[3] = u.wstr[0];
   }

   /* ELSE */
   else {

      /* MOVE WORK STRING TO OUTPUT STRING WITH NO CHANGE IN BYTE ORDER */
      string[0] = u.wstr[0];
      string[1] = u.wstr[1];
      string[2] = u.wstr[2];
      string[3] = u.wstr[3];
   }
   
   /* RETURN SUCCESS */
   return(1);
}
