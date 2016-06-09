# include "stdafx.h"
/****************************************************************************
** 
**    INVOCATION NAME: CONV123CAT
** 
**    PURPOSE: TO CONVERT A SOURCE CHARACTER STRING BETWEEN THE ASCII CHARACTER
**              SET AND THE EBCDIC CHARACTER SET AND CONCATENATE IT ONTO
**              A DESTINATION STRING
** 
**    INVOCATION METHOD: CONV123CAT(DEST_STR,SOURCE_STR)
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION
**     DEST_STR[]    PTR       I/O      DESTINATION CHARACTER STRING
**     SOURCE_STR[]  PTR        I       SOURCE CHARACTER STRING
**     CONV123CAT()  LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     A123TOE()        CONVERTS AN ASCII CHARACTER STRING TO EBCDIC
**     E123TOA()        CONVERTS AN EBCDIC CHARACTER STRING TO ASCII
** 
**    INTERNAL VARIABLES: NONE
**   
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES:
**     NAME          TYPE      USE      DESCRIPTION
**     GLB_STR[]     CHAR       I       TEMPORARY GLOBAL STRING
**     ASCII         INT        I       ASCII CONVERSION FLAG
**     EBCDIC        INT        I       EBCDIC CONVERSION FLAG
** 
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     J. TAYLOR     TASK 55     01/13/93  INITIAL PROLOG
**     J. TAYLOR     TASK 55     01/13/93  INITIAL PDL
**     J. TAYLOR     TASK 55     01/13/93  INITIAL CODE
** 
**    PDL: 
**
**     COPY SOURCE STRING TO GLOBAL STRING
**     IF ASCII CONVERSION
**     THEN
**        CONVERT GLOBAL STRING FROM EBCDIC TO ASCII
**     ELSE IF EBCDIC CONVERSION
**     THEN
**        CONVERT GLOBAL STRING FROM ASCII TO EBCDIC
**     ENDIF
**     CONCATENATE GLOBAL STRING TO DESTINATION STRING
**       
**     RETURN SUCCESS 
** 
****************************************************************************** 
**    CODE SECTION 
**
*****************************************************************************/
#include "stc123.h"

int conv123cat(char *dest_str,char *source_str)
{
   /* COPY SOURCE STRING TO GLOBAL STRING */
   strcpy(glb_str,source_str);

   /* IF ASCII CONVERSION */
   if (ASCII) {

      /* CONVERT GLOBAL STRING FROM EBCDIC TO ASCII */
      e123toa(glb_str);
   }

   /* ELSE IF EBCDIC CONVERSION */
   else if (EBCDIC) {

      /* CONVERT GLOBAL STRING FROM ASCII TO EBCDIC */
      a123toe(glb_str);
   }

   /* CONCATENATE GLOBAL STRING TO DESTINATION STRING */
   strcat(dest_str,glb_str);

   /* RETURN SUCCESS */
   return(1);
}
