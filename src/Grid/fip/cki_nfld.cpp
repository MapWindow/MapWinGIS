# include "stdafx.h"
/***************************************************************************
**
**     INVOCATION NAME: CKI123NFLD
**     
**     PURPOSE: TO CHECK INPUT STRING FOR NULL FIELDS AND ENSURE DELIMITERS
**               ARE NOT ADJACENT.
**
**     INVOCATION METHOD: CKI123NFLD(OLD_STR)
**
**     ARGUMENT LIST: 
**      NAME            TYPE     USE   DESCRIPTION
**      OLD_STR[]       PTR       I    OLD STRING TO BE CHECKED
**      CKI123NFLD()    LOGICAL   O    SUCCESS FLAG
**
**     EXTERNAL FUNCTION REFERENCES: NONE
**
**     INTERNAL VARIABLES:
**      NAME            TYPE         DESCRIPTION
**      I               INT          INDEX INTO INPUT STRING
**      NEW_STR[]       PTR          NEW CHARACTER STRING AFTER PROCESSING
**      SLEN            INT          STRING LENGTH
**      TMP_STR[]       PTR          TEMPORARY CHARACTER STRING POINTER
**
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
**
**    GLOBAL CONSTANTS:
**     NAME          TYPE      DESCRIPTION
**     FT            CHAR      FIELD TERMINATOR (RS) 1/14
**     NC            CHAR      NULL CHARACTER
**     SEP_STR[2]    CHAR      CHARACTER STRING CONTAINING DELIMITER SEPARATOR
**     UT            CHAR      UNIT TERMINATOR (US) 1/15
**
**     CHANGE HISTORY:
**      AUTHOR         CHANGE-ID     DATE     CHANGE SUMMARY
**      A. DEWITT                  07/24/90   INITIAL PROLOG
**      A. DEWITT                  07/24/90   INITIAL PDL
**      A. DEWITT                  07/24/90   INITIAL CODE
**      L. MCMILLION   92DR009     04/14/92   REPLACED BLANK SEPARATOR WITH
**                                             NEW SEPARATOR
**      L. MCMILLION   TASK #40    12/30/92   UPDATED PROLOG PER QC
**
**     PDL:
**
**      SET TEMPORARY STRING TO OLD STRING
**      SET STRING LENGTH TO LENGTH OF TEMPORARY STRING
**      SET NEW STRING POINTER TO TEMPORARY STRING POINTER
**      IF FIRST CHARACTER INDICATES A NULL FIELD
**      THEN
**         INCREMENT STRING LENGTH
**      ENDIF
**
**      WHILE NEW STRING NOT POINTING TO NULL CHARACTER DO
**         IF NEW STRING IS POINTING TO UNIT TERMINATOR
**         THEN
**            IF NEXT CHARACTER OF NEW STRING EQUALS A UNIT TERMINATOR OR A
**             FIELD TERMINATOR
**            THEN
**               INCREMENT STRING LENGTH
**            ENDIF
**         ENDIF
**         MOVE POINTER TO NEXT CHARACTER
**      END WHILE
**      INCREMENT SLEN FOR NC SPACE
**      INITIALIZE INDEX TO ZERO
**      ALLOCATE SPACE FOR NEW STRING
**      SET NEW STRING TO NULL CHARACTER
**      IF FIRST CHARACTER INDICATES NULL COLUMN VALUES OF TWO OR MORE
**       DIMENSIONS
**      THEN
**         APPEND SEPARATOR TO NEW STRING
**      ENDIF
**      WHILE TEMPORARY STRING NOT POINTING TO NULL CHARACTER DO
**         IF TEMPORARY STRING IS POINTING TO UNIT TERMINATOR
**         THEN
**            APPEND CHARACTER TO NEW STRING
**            IF NEXT CHARACTER OF STRING EQUALS UNIT TERMINATOR OR FIELD
**             TERMINATOR
**            THEN
**               APPEND A SEPARATOR TO NEW STRING
**            ENDIF
**         ELSE
**            COPY CHARACTER TO NEW STRING
**         ENDIF
**         UPDATE INDEX TO NEXT CHARACTER
**      END WHILE
**
**      SET PARAMETER VARIABLE OLD STRING TO NEW STRING
**      FREE TEMPORARY STRING POINTER
**
**      RETURN SUCCESS
**
****************************************************************************
**      CODE SECTION
**
****************************************************************************/
#include "stc123.h"

int cki123nfld(char **old_str)

{
   /* INTERNAL VARIABLES */
   long i;
   char *new_str;
   size_t slen;
   char *tmp_str;


   /* SET TEMPORARY STRING TO OLD STRING */
   tmp_str = *old_str;

   /* SET STRING LENGTH TO LENGTH OF TEMPORARY STRING */
   slen = (size_t) _tcslen(tmp_str);

   /* SET NEW STRING POINTER TO TEMPORARY STRING POINTER */
   new_str = tmp_str;
   
   /* IF FIRST CHARACTER INDICATES A NULL FIELD */
   if ((*new_str == FT) || (*new_str == UT)) slen++;
   
   /* WHILE NEW STRING NOT POINTING TO NULL CHARACTER DO */
   while (*new_str != NC) {
      
      /* IF NEW STRING IS POINTING TO UNIT TERMINATOR */
      if (*new_str == UT) {
         
         /* IF NEXT CHARACTER OF NEW STRING EQUALS A UNIT TERMINATOR OR A
             FIELD TERMINATOR -- A NULL FIELD HAS BEEN FOUND, INCREMENT STRING
             LENGTH
         */
         if ((new_str[1] == UT) || (new_str[1] == FT)) slen++;
      }
      
      /* MOVE POINTER TO NEXT CHARACTER */
      new_str++;
   }         

   /* INCREMENT SLEN FOR NC SPACE */
   slen++;
   
   /* INITIALIZE INDEX TO ZERO */
   i = 0;
   
   /* ALLOCATE SPACE FOR NEW STRING */
   if ((new_str = (char *) malloc(slen * sizeof(char))) == NULL) return(0);

   /* INITIALIZE NEW STRING TO NULL CHARACTER */
   *new_str = NC;
   
   /* IF FIRST CHARACTER INDICATES NULL COLUMN VALUES OF TWO OR MORE 
      DIMENSIONS, APPEND SEPARATOR TO NEW STRING
   */
   if ((tmp_str[i] == FT) || (tmp_str[i] == UT)) strcat(new_str,SEP_STR);
   
   /* WHILE TEMPORARY STRING NOT POINTING TO NULL CHARACTER DO */
   while (tmp_str[i] != NC) {
      
      /* IF TEMPORARY STRING IS POINTING TO UNIT TERMINATOR */
      if (tmp_str[i] == UT) {
         
         /* APPEND CHARACTER TO NEW STRING */
         strncat(new_str,&tmp_str[i],1);
         
         /* IF NEXT CHARACTER OF STRING EQUALS UNIT TERMINATOR OR FIELD
             TERMINATOR -- A NULL DIMENSION, APPEND A SEPARATOR TO NEW STRING
         */
         if (tmp_str[i+1] == UT || tmp_str[i+1] == FT) strcat(new_str,SEP_STR);
      }
      
      /* ELSE COPY CHARACTER TO NEW STRING */
      else {
         strncat(new_str,&tmp_str[i],1);
      }
         
      /* UPDATE INDEX TO NEXT CHARACTER */
      i++;
   }

   /* SET PARAMETER VARIABLE OLD STRING TO NEW STRING */
   *old_str = new_str;

   /* FREE TEMPORARY STRING POINTER */
   free(tmp_str);

   /* RETURN SUCCESS */
   return(1);
}
