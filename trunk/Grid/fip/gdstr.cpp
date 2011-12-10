# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: G123DSTR
**
**    PURPOSE: TO RETRIEVE A STRING INTO A CHARACTER BUFFER BASED ON DELIMITOR
**
**    INVOCATION METHOD: G123DSTR(IN_STR, BUF_STR, DELIM)
**
**    ARGUMENT LIST:
**     NAME        TYPE      USE      DESCRIPTION
**     IN_STR[]    PTR        I       POINTER TO A INPUT STRING
**     BUF_STR[]   PTR       I/O      POINTER TO A BUFFER
**     DELIM       CHAR       I       DELIMITOR CHARACTER
**     G123DSTR()  LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     FTPOS      LONG               POSITION OF FIELD TERMITATOR
**     DLPOS      LONG               POSITION OF DELIMITOR
**     DLSTR[2]   CHAR               DELIMITOR CHARACTER STRING
**  
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
**
**    GLOBAL CONSTANTS:
**     NAME       TYPE            DESCRIPTION
**     FT         CHAR            FIELD TERMINATOR
**     FT_STR     CHAR[2]         FIELD TERMINATOR STRING
**     NC         CHAR            NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     L. MCMILLION              05/09/90  INITIAL CODE
**     A. DEWITT      92DR005    04/08/92  ADD FEOF() CALL FOR BINARY FILE
**     L. MCMILLION  TASK #40    12/15/92  PROLOG/PDL/INLINE COMMENT UPDATED
**     J. TAYLOR     93DR023     04/16/93  REDESIGNED TO RETRIEVE STRING
**                                          FROM INPUT STRING RATHER THAN
**                                          FILE
**     L. MCMILLION  93DR023     05/13/93  INSERTED SIZE_T CAST FOR STRNCPY
**                                          ARGUMENT
**     J. TAYLOR     93DR031     06/16/93  MODIFIED TO STORE DELIMITER IN 
**                                          CHARACTER STRING BEFORE CALL TO
**                                          STRCSPN()
**
**    PDL:
**
**     IF THERE IS A FIELD TERMINATOR IN THE INPUT STRING
**     THEN
**        SET FTPOS TO POSITION OF FIELD TERMINATOR
**     ELSE
**        SET FTPOS TO ZERO
**     ENDIF
**     IF THERE IS A DELIMITOR IN THE INPUT STRING
**     THEN
**        STORE DELIMITOR IN CHARACTER STRING
**        TERMINATE CHARACTER STRING WITH NULL CHARACTER
**        SET DLPOS TO POSITION OF DELIMITOR
**     ELSE
**        SET DLPOS TO ZERO
**     ENDIF
**     IF DELIMITOR OCCURS BEFORE OR IS FIELD TERMINATOR 
**      OR DELIMITOR OCCURS BUT FIELD TERMINATOR DOES NOT
**     THEN
**        COPY DELIMITED STRING FROM INPUT STRING TO BUF_STR
**        MOVE BEGINNING OF INPUT STRING PAST DELIMITOR
**        APPEND NULL CHARACTER TO BUF_STR
**     ELSE IF FIELD TERMINATOR OCCURS BEFORE DELIMITOR
**      OR FIELD TERMINATOR OCCURS BUT DELIMITOR DOES NOT
**     THEN
**        COPY TERMINATED STRING FROM INPUT STRING TO BUF_STR
**        MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR
**        APPEND NULL CHARACTER TO BUF_STR
**     ELSE IF THERE IS NEITHER A DELIMITOR OR FIELD TERMINATOR
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

int g123dstr(char **in_str,char *buf_str,int delim)
{
   /* LOCAL VARIABLES */
   long dlpos;
   long ftpos;
   char dlstr[2];

   /* IF THERE IS A FIELD TERMINATOR IN THE INPUT STRING */
   if (strchr(*in_str,FT))  

      /* SET FTPOS TO POSITION OF FIELD TERMINATOR */
      ftpos = strcspn(*in_str,FT_STR) + 1L;

   /* ELSE SET FTPOS TO ZERO */
   else ftpos = 0;

   /* IF THERE IS A DELIMITOR IN THE INPUT STRING */
   if (strchr(*in_str,delim))  {

      /* STORE DELIMITOR IN CHARACTER STRING */
      dlstr[0] = delim;

      /* TERMINATE CHARACTER STRING WITH NULL CHARACTER */
      dlstr[1] = NC;

      /* SET DLPOS TO POSITION OF DELIMITOR */
      dlpos = strcspn(*in_str,dlstr) + 1L;
   }
   /* ELSE SET DLPOS TO ZERO */
   else dlpos = 0;

   /* IF DELIMITOR OCCURS BEFORE OR IS FIELD TEMRINATOR 
       OR DELIMITOR OCCURS BUT FIELD TERMINATOR DOES NOT */
   if (((dlpos) && (dlpos <= ftpos))
       || ((dlpos) && (!ftpos))) {

      /* COPY DELIMITED STRING FROM INPUT STRING TO BUF_STR */
      strncpy(buf_str,*in_str,(size_t)dlpos-1);

      /* MOVE BEGINNING OF INPUT STRING PAST DELIMITOR */
      *in_str = *in_str + dlpos;

      /* APPEND NULL CHARACTER TO BUF_STR */
      buf_str[dlpos-1] = NC;
   }

   /* ELSE IF FIELD TERMINATOR OCCURS BEFORE DELIMITOR
       OR FIELD TERMINATOR OCCURS BUT DELIMITOR DOES NOT */
   else if (((ftpos) && (ftpos < dlpos)) 
            || ((ftpos) && (!dlpos))) {

      /* COPY TERMINATED STRING FROM INPUT STRING TO BUF_STR */
      strncpy(buf_str,*in_str,(size_t)ftpos);

      /* MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR */
      *in_str = *in_str + ftpos;

      /* APPEND NULL CHARACTER TO BUF_STR */
      buf_str[ftpos] = NC;
   }

   /* ELSE IF THERE IS NEITHER A DELIMITOR OR FIELD TERMINATOR */
   else if ((!dlpos) && (!ftpos)) {

      /* RETURN FAILURE */
      return(0);
   }

   /* RETURN SUCCESS */
   return(1);
}  
