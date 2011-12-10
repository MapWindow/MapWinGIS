# include "stdafx.h"
/********************+*******************************************************
** 
**    INVOCATION NAME: G123ORDER
** 
**    PURPOSE: RETURN THE BYTE ORDER OF THE CURRENT MACHINE       
** 
**    INVOCATION METHOD: G123ORDER(ORDER)
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION
**     ORDER         PTR        O       BYTE ORDER            
**                                       0 - "LITTLE ENDIAN"
**                                            LEAST SIGNIFICANT BYTE FIRST
**                                       1 - "BIG ENDIAN"
**                                            MOST SIGNIFICANT BYTE FIRST
**                                       2 - "MIDDLE ENDIAN"
**     G123ORDER()   LOGICAL    O       SUCCESS FLAG
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
**     SET LONG INTEGER TO TEST VALUE
**     IF RIGHT MOST BYTE IS MOST SIGNIFICANT BYTE
**     THEN
**        SET ORDER TO LITTLE ENDIAN
**     ELSE IF LEFT MOST BYTE IS MOST SIGNIFICANT BYTE
**     THEN
**        SET ORDER TO BIG ENDIAN
**     ELSE
**        SET ORDER TO MIDDLE ENDIAN
**     ENDIF
**       
**     RETURN SUCCESS 
** 
****************************************************************************** 
**    CODE SECTION 
**
*****************************************************************************/
#include "stc123.h"

int g123order(int *order)
{
   /* INTERNAL VARIABLES */
   union { long wnum; char wstr[sizeof(long)]; } u;
   
   /* SET LONG INTEGER TO TEST VALUE */
   u.wnum = 0x04030201;

   /* IF RIGHT MOST BYTE IS MOST SIGNIFICANT BYTE */
   if (u.wstr[3] == 0x04) {
   
      /* SET ORDER TO LITTLE ENDIAN */
      *order = 0;
   }
   
   /* ELSE IF LEFT MOST BYTE IS MOST SIGNIFICANT BYTE */
   else if (u.wstr[0] == 0x04) {
 
      /* SET ORDER TO BIG ENDIAN */
      *order = 1;
   }

   /* ELSE */
   else {

      /* SET ORDER TO MIDDLE ENDIAN */
      *order = 2;

   }
      
   /* RETURN SUCCESS */
   return(1);
}
