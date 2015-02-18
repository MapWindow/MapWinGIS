# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: G123STR
**
**    PURPOSE: TO READ A STRING INTO A CHARACTER BUFFER
**
**    INVOCATION METHOD: G123STR(FP, BUF_STR, STR_LEN)
**
**    ARGUMENT LIST:
**     NAME       TYPE      USE      DESCRIPTION
**     FP         PTR        I       FILE POINTER
**     BUF_STR[]  PTR        O       POINTER TO A BUFFER 
**     STR_LEN    INT        I       LENGTH OF STRING TO BE READ IN
**     G123STR()  LOGICAL    O       SUCCESS FLAG 
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     I          INT                LOOP CONTROL VARIABLE
**     CH         INT                INTEGER REPRESENTATION OF INPUT CHARACTER
**
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
**  
**    GLOBAL CONSTANTS:
**     NAME       TYPE           DESCRIPTION
**     FT         CHAR           FIELD TERMINATOR
**     NC         CHAR           NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     L. MCMILLION              05/09/90  INITIAL CODE
**     A. DEWITT                 08/14/90  REMOVED CALL TO FUNCTION FEOF
**                                          IN WHILE CONDITION
**     L. MCMILLION  TASK #40    12/22/92  UPDATED PROLOG/PDL PER QC
**
**    PDL:
**
**     INITIALIZE I TO ZERO
**     INITIALIZE CH TO NULL CHARACTER
**     WHILE STRING LENGTH LESS THAN INPUT STRING LENGTH AND FIELD
**      TERMINATOR NOT ENCOUNTERED AND NOT EOF DO
**        READ CH
**        STORE CH IN BUFFER
**        INCREMENT I
**     ENDWHILE
**     IF EOF ENCOUNTERED
**     THEN
**        RETURN FAILURE
**     ENDIF
**     APPEND NULL CHARACTER TO STRING
**     IF CH EQUALS FIELD TERMINATOR
**     THEN
**        INCREMENT I
**     ENDIF
**
**     IF STRING IS NOT COMPLETE 
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

int g123str(FILE *fp,char *buf_str,long str_len)
{
   /* INTERNAL VARIABLES */
   int ch;
   long i = 0;

   /* INITIALIZE CH TO NULL CHARACTER */
   ch = NC;
   
   /* READ CHARACTER. WHILE STRING LENGTH LESS THAN INPUT STRING LENGTH 
       AND FIELD TERMINATOR NOT ENCOUNTERED AND NOT EOF DO
   */
   while((i < str_len) && ((ch = getc(fp)) != EOF) && (ch != FT)) {

         /* STORE CH IN BUFFER */
         buf_str[i++] = (char) ch;
   }

   /* IF EOF ENCOUNTERED, RETURN FAILURE */
   if (ch == EOF) return(0);

   /* APPEND NULL CHARACTER TO STRING */
   buf_str[i] = NC;

   /* IF FIELD TERMINATOR ENCOUNTERED, INCREMENT INDEX I */
   if (ch == FT) i++;

   /* IF STRING IS NOT COMPLETE, RETURN FAILURE */
   if (i != str_len) return(0);

   /*RETURN SUCCESS */
   return(1);
}
