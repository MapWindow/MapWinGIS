# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: WR123REC
**
**    PURPOSE: TO WRITE THE NEXT DATA RECORD
**
**    INVOCATION METHOD: WR123REC(FP,STRING,STR_LEN,STATUS)
**
**    ARGUMENT LIST:
**     NAME          TYPE      USE      DESCRIPTION
**     FP            PTR        I       FILE POINTER
**     STRING[]      PTR        I       DATA RECORD 
**     STR_LEN       LONG       I       LENGTH OF STRING 
**     STATUS        INT       I/O      STATUS
**                                       0 = FAILURE
**                                       1 = OKAY
**                                       2 = START OF RECORD
**                                       3 = END OF RECORD
**                                       4 = END OF FILE
**                                       5 = END OF FIELD
**                                       6 = START OF FIELD
**     WR123REC()    LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     END123REC()      COMPLETE PREVIOUS RECORD IF DATA IN BUFFER
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     LD123REC()       LOADS DATA RECORD INTO STRUCTURES
**
**    INTERNAL VARIABLES: 
**     NAME          TYPE              DESCRIPTION
**     INSTATUS      INT               INPUT STATUS
**     PREV_POS      INT               PREVIOUS POSITION OF FILE POINTER
**  
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/17/90  INITIAL PROLOG
**     P. HODGES                 05/17/90  INITIAL PDL
**     A. DEWITT                 06/01/90  INITIAL CODE
**     A. DEWITT                 06/11/90  MODIFY PDL/CODE TO IMPLEMENT STATUS
**     L. MCMILLION              12/11/90  MODIFIED LOGIC TO LOAD DR STRUCTURE
**                                          REGARDLESS OF LEADER ID
**     J. TAYLOR     92DR005     05/16/92  MODIFIED FOR BIT FIELD WRITING
**     J. TAYLOR     92DR005     05/17/92  CHANGED CALLING SEQUENCE TO INPUT
**                                          STRING LENGTH
**     L. MCMILLION  TASK #40    12/16/92  UPDATED PROLOG PER QC
**
**    PDL:
**
**     SET INSTATUS TO INPUT STATUS
**     INITIALIZE STATUS TO FAILURE
**     CALL GET123LEVEL TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER
**     CALL END123REC() TO WRITE ANY PREVIOUS RECORD TO FILE
**     SET PREV_POS TO CURRENT POSITION OF FILE POINTER
**     WRITE RECORD TO FP
**     MOVE FILE POINTER TO BEGINING OF RECORD WRITTEN
**     CALL LD123REC() TO LOAD RECORD INTO THE STRUCTURE
**     REPOSITION FILE POINTER BACK TO BEGINNING OF RECORD WRITTEN TO PREPARE
**      FILE FOR WHEN THE DATA RECORD IS ENDED
**     
**     SET STATUS TO INSTATUS
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int wr123rec(FILE *fp,char *string,long str_len,int *status)

{
   int  instatus    ;
   long prev_pos    ;
   
   /* SET INSTATUS TO INPUT STATUS */
   instatus = *status;
   
   /* INITIALIZE STATUS TO FAILURE */
   *status = 0;
   
   /* CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return (0);
   
   /* CALL END123REC() TO WRITE ANY PREVIOUS RECORD TO FILE */
   if (!end123rec(fp)) return (0);
   
   /* SET PREV_POS TO CURRENT POSITION OF FILE POINTER */
   if ((prev_pos = ftell(fp)) == -1L) return(0);
   
   /* WRITE RECORD TO FP */
   fwrite(string,sizeof(char),(size_t) str_len,fp);
   
   /* MOVE FILE POINTER TO BEGINING OF RECORD WRITTEN */
   fseek(fp,prev_pos,SEEK_SET);
      
   /* CALL LD123REC() LOAD RECORD INTO THE STRUCTURE */
   if (!ld123rec()) return (0);

   /* REPOSITION FILE POINTER BACK TO BEGINNING OF RECORD WRITTEN */
   fseek(fp,prev_pos,SEEK_SET);
   
   /* SET STATUS TO OKAY */
   *status = instatus;
   
   /* RETURN SUCESS */
   return(1);
}

