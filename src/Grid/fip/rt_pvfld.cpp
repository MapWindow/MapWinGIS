# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: RT123PVFLD 
** 
**    PURPOSE: TO RETRIEVE PREVIOUS FIELD 
** 
**    INVOCATION METHOD: RT123PVFLD() 
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION 
**     RT123PVFLD()  LOGICAL    O       SUCCESS FLAG 
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     BAK123REC()      BACKS UP TO BEGINNING OF LAST RECORD READ OR WRITTEN 
**     LD123REC()       READS DATA RECORD AND STORES IN MEMORY
** 
**    INTERNAL VARIABLES: 
**     NAME       TYPE               DESCRIPTION 
**     PREV_POS   INT                PREVIOUS POSITION OF INTERNAL FILE
**                                    POINTER
**     STATUS     INT                STATUS
**                                    0 = FAILURE
**                                    1 = OKAY
**                                    2 = START OF RECORD
**                                    3 = END OF RECORD
**                                    4 = END OF FILE
**                                    5 = END OF FIELD
**                                    6 = START OF FIELD
**     TEMP_PTR   PTR                TEMPORARY POINTER TO DATA RECORD 
**                                    STRUCTURE 
**   
**    GLOBAL REFERENCES: 
** 
**     DATA RECORD STRUCTURE (DR) 
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD 
**      FD_LEN          INT    N/A   LENGTH OF DISCRIPTIVE AREA DATA RECORD  
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA 
**                                    RECORD 
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS (NO LABELS) 
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSION LENGTHS 
**                                    (NO LABELS) 
**      VALUES          PTR    N/A   HEAD POINTER TO DATA VALUE SUBFIELD 
**                                    RECORDS 
**      NEXT            PTR     I    POINTER TO NEXT DATA RECORD  
** 
**     FILE MANAGEMENT STRUCTURE (FM) 
**      NAME            TYPE   USE   DESCRIPTION 
**      FP              PTR     I    FILE POINTER 
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME 
**      OPEN_MODE       CHAR    I    OPEN MODE OF FILE 
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE 
**                                    CONTROL RECORD STRUCTURE 
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE 
**      DL_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE LEADER 
**                                    STRUCTURE 
**      DR_HD           PTR     I    HEAD POINTER TO DATA RECORD STRUCTURE 
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE 
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER 
**                                    STRUCTURE 
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK 
**                                    STRUCTURE 
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR    N/A   CURRENT POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE ENTRY 
**      CUR_DM          PTR    N/A   CURRENT POINTER TO DIMENSION LENGTHS 
**                                    STRUCTURE ENTRY 
**      CUR_DR          PTR    I/O   CURRENT POINTER TO DATA RECORD STRUCTURE 
**                                    ENTRY 
**      CUR_DV          PTR    N/A   CURRENT POINTER TO DR DATA VALUE SUBFIELD 
**                                    STRUCTURE ENTRY 
**      ROW_DVH         PTR    N/A   CURRENT POINTER TO NEXT SET OF VALUES 
**                                    IN DR DATA VALUE SUBFIELD STRUCTURE ENTRY 
**      CUR_FC          PTR    N/A   CURRENT POINTER TO FORMAT CONTROLS 
**                                    STRUCTURE ENTRY 
**      CUR_LP          PTR    N/A   CURRENT POINTER TO LABELS POINTER 
**                                    STRUCTURE ENTRY 
**      CUR_SL          PTR    N/A   CURRENT POINTER TO DD-LABEL SUBFIELD 
**                                    STRUCTURE ENTRY 
**      CUR_FCR         PTR    N/A   CURRENT POINTER TO ROOT OF FORMAT 
**                                    CONTROLS STRUCTURE 
**      CUR_RP          PTR    N/A   CURRENT POINTER TO RECORD POINTER 
**                                    STRUCTURE
**      NLD_RP          PTR    N/A   POINTER TO RECORD POINTER STRUCTURE 
**                                    WHERE CORRESPONDING DR HAS AN 'R' 
**                                    LEADER IDENTIFIER    
**      SF_FMT          CHAR   N/A   FORMAT CORRESPONDING TO THE CURRENT 
**                                    DATA VALUE 
**      NLD             INT    N/A   NO LEADER FLAG 
**                                    TRUE  - NO LEADER 
**                                    FALSE - LEADER EXISTS 
**      BIT_CNT         INT    N/A   COUNT OF BITS STORED IN MEMORY FOR 
**                                    A SUBSEQUENT READ FUNCTION
**      BIT_BIN         CHAR   N/A   BITS STORED IN MEMORY FOR SUBSEQUENT
**                                    READ FUNCTION
**      COMPRESS        INT    N/A   FLAG TO SPECIFY COMPRESSED OR
**                                    UNCOMPRESSED ADJACENT FIXED-LENGTH
**                                    BIT FIELD I/O
**                                    0 - UNCOMPRESSED
**                                    1 - COMPRESSED
**      SF_STATE_DD     INT    N/A   SUBFIELD STATE (DD) 
**                                    1 - FIELD CONTROL SUBFIELD 
**                                    2 - NAME SUBFIELD 
**                                    3 - LABELS SUBFIELD 
**                                    4 - FORMATS SUBFIELD 
**                                    5 - FINISHED 
**      SF_STATE_DR     INT    N/A   SUBFIELD STATE (DR) 
**                                    1 - NUMBER OF DIMENSIONS SUBFIELD 
**                                    2 - LENGTH OF A DIMENSION SUBFIELD 
**                                    3 - DATA VALUE STRING SUBFIELD 
**                                    4 - FINISHED 
**      NEXT            PTR    N/A   POINTER TO NEXT FILE MANAGEMENT 
**                                    STRUCTURE 
**      BACK            PTR    N/A   POINTER TO PREVIOUS FILE MANAGEMENT 
**                                    STRUCTURE 
** 
**    GLOBAL VARIABLES: 
**     NAME             TYPE   USE   DESCRIPTION 
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT 
**                                    STRUCTURE ENTRY 
** 
**    GLOBAL CONSTANTS:  NONE 
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/17/90  INITIAL PROLOG 
**     P. HODGES                 05/17/90  INITIAL PDL 
**     P. HODGES                 06/11/90  INITIAL CODE 
**     A. DEWITT                 12/05/90  MODIFY CODE/LOGIC FOR SPECIAL CASE
**                                          WRITE FILES AND GIVEN DEFINITION
**                                          OF BAK123REC FUNCTION
**     A. DEWITT                 01/15/91  ADD LOGIC TO REPOSITION FILE POINTER
**                                          TO PREV POSITION IN CASE OF WRITE
**                                          FILE. CHANGE MADE TO RESTORE BACK
**                                          REC FUNCTION RESULTS.
**     L. MCMILLION  TASK #40    12/23/92  UPDATED PROLOG PER QC
** 
**    PDL: 
** 
**     SET TEMP POINTER TO CUR_DR 
**     SET CUR_DR TO DR_HD 
**     IF CUR_DR IS NULL OR CUR_DR EQUALS TEMP POINTER { FIRST FIELD } 
**     THEN 
**        { RETRIEVE LAST FIELD OF PREVIOUS RECORD } 
**        CALL BAK123REC() TO BACK UP TO BEGINNING OF LAST RECORD 
**         READ OR WRITTEN 
**        IF OPEN MODE IS READ 
**        THEN
**           { BACK UP TO BEGINNING OF PREVIOUS RECORD READ OR WRITTEN }
**           CALL BAK123REC() TO BACK UP TO BEGINNING OF LAST RECORD 
**            READ
**        ENDIF
**        ASSIGN PREVIOUS INTERNAL FILE POINTER POSITION
**        CALL LD123REC() TO STORE PREVIOUS RECORD IN MEMORY
**        IF OPEN MODE IS WRITE
**        THEN
**           SET INTERNAL FILE POINTER TO PREVIOUS POSITION
**        ENDIF
**        SET CUR_DR TO DR_HD
**        WHILE NOT AT LAST FIELD DO
**           SET CUR_DR TO CUR_DR NEXT
**        ENDWHILE 
**     ELSE CUR_DR NOT NULL OR CUR_DR NOT EQUAL TO TEMP POINTER
**        WHILE CUR_DR NEXT IS NOT TEMP POINTER DO
**           SET CUR_DR TO CUR_DR NEXT
**        ENDWHILE
**     ENDIF 
** 
**     RETURN SUCCESS 
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int rt123pvfld()

{
   /* LOCAL VARIABLES */
   long      prev_pos ;
   int       status   ;
   struct dr *temp_ptr;

   /* SET TEMP POINTER TO CUR_DR */
   temp_ptr = cur_fm->cur_dr;

   /* SET CUR_DR TO DR_HD */
   cur_fm->cur_dr = cur_fm->dr_hd;

   /* IF CUR_DR IS NULL OR CUR_DR EQUALS TEMP POINTER { FIRST FIELD } */
   if (cur_fm->cur_dr == NULL || cur_fm->cur_dr == temp_ptr) {

      /* { RETRIEVE LAST FIELD OF PREVIOUS RECORD } */

      /* CALL BAK123REC() TO BACK UP TO BEGINNING OF LAST RECORD 
           READ OR WRITTEN
      */
      if(!bak123rec(cur_fm->fp,&status)) return(0);

      /* IF OPEN_MODE IS READ */
      if (cur_fm->open_mode == 'r') {

         /* CALL BAK123REC() TO BACK UP TO BEGINNING OF LAST RECORD READ */
         if(!bak123rec(cur_fm->fp,&status)) return(0);          
      };   

      /* ASSIGN PREVIOUS INTERNAL FILE POINTER POSITION */
      if ((prev_pos = ftell(cur_fm->fp)) == -1L) return(0);
      
      /* CALL LD123REC() TO STORE PREVIOUS RECORD IN MEMORY */
      if(!ld123rec()) return(0); 

      /* IF OPEN MODE IS WRITE */
      if (cur_fm->open_mode == 'w') {
         
         /* SET INTERNAL FILE POINTER TO PREVIOUS POSITION */
         if (fseek(cur_fm->fp,prev_pos,SEEK_SET)) return(0);
      };
         
      /* SET CUR_DR TO DR_HD */
      cur_fm->cur_dr = cur_fm->dr_hd;

      /* WHILE NOT AT LAST FIELD DO */
      while (cur_fm->cur_dr->next != NULL) {

         /* SET CUR_DR TO CUR_DR NEXT */
         cur_fm->cur_dr = cur_fm->cur_dr->next;

      };
   }

   /* ELSE CUR_DR NOT NULL OR CUR_DR NOT EQUAL TO TEMP POINTER */
   else { 

      /* WHILE CUR_DR NEXT IS NOT TEMP POINTER DO */
      while (cur_fm->cur_dr->next != temp_ptr) {

         /* SET CUR_DR TO CUR_DR NEXT      */
         cur_fm->cur_dr = cur_fm->cur_dr->next;
      } 
   } 

   /* RETURN SUCCESS */
   return(1);
} 
