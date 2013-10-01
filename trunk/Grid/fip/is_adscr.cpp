# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: IS123ADSCR
**
**    PURPOSE: TO DETERMINE IF THE INPUT STRING IS A DDR ARRAY DESCRIPTOR
**              INSTEAD OF LABELS
**
**    INVOCATION METHOD: IS123ADSCR(STRNG)
**
**    ARGUMENT LIST:
**     NAME            TYPE      USE      DESCRIPTION
**     STRNG[]         PTR        I       POINTER TO INPUT STRING
**     IS123ADSCR()    LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     DIGCOM     LOGICAL            DIGIT OR COMMA FLAG
**     I          INT                INDEX
**     LEN        SIZE_T             LENGTH OF CHARACTER STRING
**
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION       
**     ARYD_DLM         CHAR         DDR ARRAY DESCRIPTOR DELIMITER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     L. MCMILLION   93DR033    06/16/93  INITIAL PROLOG
**     L. MCMILLION   93DR033    06/16/93  INITIAL PDL
**     L. MCMILLION   93DR033    06/16/93  INITIAL CODE
**
**    PDL:
**
**     INITIALIZE DIGIT OR COMMA FLAG TO TRUE
**     INITIALIZE INDEX TO ZERO
**     SET LENGTH TO STRING LENGTH
**     WHILE INDEX LESS THAN STRING LENGTH AND DIGIT OR COMMA FLAG TRUE DO
**        IF CURRENT CHARACTER IN STRING IS A DIGIT OR COMMA
**        THEN
**           SET DIGIT OR COMMA FLAG TO TRUE
**        ELSE
**           SET DIGIT OR COMMA FLAG TO FALSE
**        ENDIF
**        INCREMENT INDEX
**     ENDWHILE
**     
**     RETURN DIGIT OR COMMA FLAG
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int is123adscr(char *strng)

{
   /* INTERNAL VARIABLES */
   int digcom = 1;
   int i = 0;
   size_t len;

   /* SET LENGTH TO STRING LENGTH */
   len = _tcslen(strng);

   /* WHILE INDEX LESS THAN STRING LENGTH AND DIGIT OR COMMA FLAG TRUE DO */
   while (i < (int)len && digcom) {

      /* IF CURRENT CHARACTER IN STRING IS A DIGIT OR COMMA, SET DIGIT OR
          COMMA FLAG TO TRUE; OTHERWISE FALSE
      */
      digcom = (isdigit((int) strng[i]) || strng[i] == ARYD_DLM);

      /* INCREMENT INDEX */
      i++;
   }

   /*  RETURN DIGIT OR COMMA FLAG */
   return(digcom);
}
