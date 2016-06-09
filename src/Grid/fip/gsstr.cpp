# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: G123SSTR
**
**    PURPOSE: TO RETRIEVE A SUB STRING INTO A CHARACTER BUFFER
**
**    INVOCATION METHOD: G123SSTR(IN_STR, BUF_STR, STR_LEN)
**
**    ARGUMENT LIST:
**     NAME       TYPE      USE      DESCRIPTION
**     IN_STR[]   PTR        I       POINTER TO AN INPUT STRING
**     BUF_STR[]  PTR        O       POINTER TO A BUFFER 
**     STR_LEN    LONG       I       LENGTH OF STRING TO BE RETRIEVED
**     G123SSTR() LOGICAL    O       SUCCESS FLAG 
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     FTPOS      LONG               POSITION OF FIELD TERMITATOR
**     IN_STR_LEN LONG               LENGTH OF INPUT STRING
**
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
**  
**    GLOBAL CONSTANTS:
**     NAME       TYPE           DESCRIPTION
**     FT         CHAR           FIELD TERMINATOR
**     FT_STR     CHAR[2]        FIELD TERMINATOR STRING
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
**     J. TAYLOR     93DR023     04/16/93  REDESIGNED FROM G123STR() 
**                                          TO RETRIEVE STRING
**                                          FROM INPUT STRING RATHER THAN
**                                          FILE
**     L. MCMILLION  93DR023     05/13/93  INSERTED SIZE_T CAST FOR STRNCPY
**                                          ARGUMENT
**
**    PDL:
**
**     IF THERE IS A FIELD TERMINATOR IN THE INPUT STRING
**     THEN
**        SET FTPOS TO POSITION OF FIELD TERMINATOR
**     ELSE
**        SET FTPOS TO ZERO
**     ENDIF
**     SET IN_STR_LEN TO INPUT STRING LENGTH
**     IF STRING TERMINATED BY A FIELD TERMINATOR IS SHORTER THAN STRING
**      TO BE RETRIEVED
**     THEN
**        RETURN FAILURE
**     ELSE IF STRING TERMINATED BY A FIELD TERMINATOR IS SAME LENGTH AS
**      STRING TO BE RETRIEVED
**     THEN
**        COPY STRING TERMINATED BY FIELD TERMINATOR TO BUF_STR
**        MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR
**        APPEND NULL CHARACTER TO BUF_STR
**     ELSE IF INPUT STRING IS SHORTER THAN STRING TO BE RETRIEVED
**     THEN
**        RETURN FAILURE
**     ELSE IF INPUT STRING IS LONG ENOUGH 
**     THEN
**        COPY SUB STRING FROM INPUT STRING TO BUF_STR
**        MOVE BEGINNING OF INPUT STRING PAST SUB STRING RETRIEVED
**        APPEND NULL CHARACTER TO BUF_STR
**     ENDIF
**
**     RETURN SUCCESS
**    
*****************************************************************************
**     CODE SECTION
**
*****************************************************************************/ 
#include "stc123.h"

int g123sstr(char **in_str,char *buf_str,long str_len)
{
   /* INTERNAL VARIABLES */
   long ftpos;
   long in_str_len;

   /* IF THERE IS A FIELD TERMINATOR IN THE INPUT STRING */
   if (strchr(*in_str,FT))  

      /* SET FTPOS TO POSITION OF FIELD TERMINATOR */
      ftpos = strcspn(*in_str,FT_STR) + 1L;

   /* ELSE SET FTPOS TO ZERO */
   else ftpos = 0;

   /* SET IN_STR_LEN TO INPUT STRING LENGTH */
   in_str_len = _tcslen(*in_str);

   /* IF STRING TERMINATED BY A FIELD TERMINATOR IS SHORTER THAN STRING
       TO BE RETRIEVED */
   if ((ftpos) && (ftpos < str_len)) {

      /* RETURN FAILURE */
      return(0);
   }

   /* ELSE IF STRING TERMINATED BY A FIELD TERMINATOR IS SAME LENGTH AS
       STRING TO BE RETRIEVED */
   else if ((ftpos) && (ftpos == str_len)) {

      /* COPY STRING TERMINATED BY FIELD TERMINATOR TO BUF_STR */
      strncpy(buf_str,*in_str,(size_t)ftpos-1);

      /* MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR */
      *in_str = *in_str + ftpos;

      /* APPEND NULL CHARACTER TO BUF_STR */
      buf_str[ftpos-1] = NC;
   }

   /* ELSE IF INPUT STRING IS SHORTER THAN STRING TO BE RETRIEVED */
   else if (in_str_len < str_len) {

      /* RETURN FAILURE */
      return(0);
   }

   /* ELSE IF INPUT STRING IS LONG ENOUGH */
   else if (in_str_len >= str_len) {

      /* COPY SUB STRING FROM INPUT STRING TO BUF_STR */
      strncpy(buf_str,*in_str,(size_t)str_len);

      /* MOVE BEGINNING OF INPUT STRING PAST SUB STRING RETRIEVED */
      *in_str = *in_str + str_len;

      /* APPEND NULL CHARACTER TO BUF_STR */
      buf_str[str_len] = NC;
   }

   /*RETURN SUCCESS */
   return(1);
}
