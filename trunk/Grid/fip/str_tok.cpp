# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: STR123TOK
**
**    PURPOSE: TO READ STRING AS A SERIES OF ZERO OR MORE TOKENS USING DELIMS
**              AS THE SET OF CHARACTERS SERVING AS DELIMITERS OF THE TOKENS.
**              ALSO TO RETURN A POINTER TO THE FIRST TOKEN ON THE FIRST CALL,
**              AND ON SUBSEQUENT CALLS RETURN EACH OF THE OTHER TOKENS UNTIL
**              THERE ARE NO MORE TOKENS.
**
**    INVOCATION METHOD: STR123TOK(STRING,DELIMS,STR_LEN)
**
**    ARGUMENT LIST:
**     NAME         TYPE      USE      DESCRIPTION
**     STRING[]     PTR       I/O      STRING POINTER
**     DELIMS[]     PTR        I       CHARACTER STRING CONTAINING DELIMITERS
**     STR_LEN      LONG       O       LENGTH OF STRING
**     STR123TOK()  PTR        O       POINTER TO TOKEN STRING
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     I          INT                INDEX COUNTER
**     ST_STR[]   PTR                POINTER TO STARTING LOCATION OF INPUT
**                                    STRING
**     TMP_STR[]  PTR                TEMPORARY CHARACTER STRING POINTER
**
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
**  
**    GLOBAL CONSTANTS:
**     NAME       TYPE           DESCRIPTION
**     NC         CHAR           NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     L. MCMILLION              09/21/90  INITIAL PROLOG
**     L. MCMILLION              09/21/90  INITIAL PDL
**     L. MCMILLION              09/21/90  INITIAL CODE
**     J. TAYLOR     92DR005     05/17/92  CHANGED CALLING SEQUENCE TO OUTPUT
**                                          STRING LENGTH
**     L. MCMILLION  TASK #40    12/16/92  UPDATED PROLOG PER QC
**
**    PDL:
**     SET INPUT STRING POINTER PAST LEADING DELIMITERS
**
**     IF INPUT STRING POINTER IS NULL CHARACTER
**     THEN
**        RETURN NULL
**     ENDIF
**
**     SET START STRING POINTER TO INPUT STRING POINTER
**     CALL LIBRARY FUNCTION STRPBRK() TO FIND THE FIRST OCCURRENCE OF A
**      DELIMITER IN INPUT STRING
**     SET TEMPORARY STRING POINTER TO POINTER RETURNED FROM LIBRARY FUNCTION
**      STRPBRK()
**     IF TEMPORARY STRING POINTER IS NULL
**     THEN
**        SET INPUT STRING POINTER TO END OF STRING
**     ELSE
**        SET TEMPORARY STRING POINTER TO NULL CHARACTER
**        IF POSITION FOLLOWING TEMPORARY STRING POINTER IS NOT NULL CHARACTER
**        THEN
**           INCREMENT TEMPORARY STRING POINTER
**        ENDIF
**        SET INPUT STRING POINTER TO TEMPORARY STRING POINTER
**     ENDIF
**
**     RETURN START OF STRING POINTER
**    
*****************************************************************************
**     CODE SECTION
**
*****************************************************************************/ 
#include "stc123.h"

char *str123tok(char **string,char *delims,long *str_len)
{
   /* INTERNAL VARIABLES */
   long i;
   char *st_str;
   char *tmp_str;

   /* SET INPUT STRING POINTER PAST LEADING DELIMITERS */
   *string += strspn(*string,delims);

   /* IF INPUT STRING POINTER IS NULL CHARACTER, RETURN NULL */
   if (**string == NC) return(NULL);

   /* SET START STRING POINTER TO INPUT STRING POINTER */
   st_str = *string;

   /* CALL LIBRARY FUNCTION STRPBRK() TO FIND THE FIRST OCCURRENCE OF A
       DELIMITER IN INPUT STRING
   */
   tmp_str = strpbrk(*string,delims);

   /* IF TEMPORARY STRING POINTER IS NULL */
   if (tmp_str == NULL) {

      /* SET INPUT STRING POINTER TO END OF STRING */
      for(i = 0, *str_len = _tcslen(st_str); i < *str_len; (*string)++, i++);
   }
   else {

      /* SET TEMPORARY STRING POINTER TO NULL CHARACTER */
      *tmp_str = NC;

      /* IF POSITION FOLLOWING TEMPORARY STRING POINTER IS NOT NULL CHARACTER */
      if (*(tmp_str + 1) != NC) {

         /* INCREMENT TEMPORARY STRING POINTER */
         ++tmp_str;
      }

      /* SET INPUT STRING POINTER TO TEMPORARY STRING POINTER */
      *string = tmp_str;
   }

   /* RETURN START OF STRING POINTER */
   return(st_str);
}

