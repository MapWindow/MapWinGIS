# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: BAK123REC  
**
**    PURPOSE: TO BACK UP TO THE BEGINNING OF LAST RECORD READ OR  
**              RECORD WRITTEN
**
**    INVOCATION METHOD: BAK123REC(FP,STATUS)
**
**    ARGUMENT LIST:
**     NAME         TYPE      USE      DESCRIPTION
**     FP           PTR        I       FILE POINTER
**     STATUS       INT        O       STATUS
**                                      0 = FAILURE
**                                      1 = OKAY
**                                      2 = START OF RECORD
**                                      3 = END OF RECORD
**                                      4 = END OF FILE
**                                      5 = END OF FIELD
**                                      6 = START OF FIELD
**     BAK123REC()  LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     END123REC()      TO COMPLETE THE PROCESSING OF A DATA RECORD AND FREE
**                       ITS SPACE
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     STC123EMPTY()    DETERMINES IF A DATA RECORD STRUCTURE IS EMPTY
**
**    INTERNAL VARIABLES: 
**     NAME          TYPE         DESCRIPTION
**     IS_WFIL       LOGICAL      FLAG INDICATING WHETHER A FILE IS A
**                                 WRITE FILE OR NOT
**     PARTIAL       LOGICAL      FLAG INDICATING THAT A RECORD CONTAINS DATA
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
**      NEXT            PTR    N/A   POINTER TO NEXT DATA RECORD 
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR    N/A   FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR    I    OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                    STRUCTURE
**      DR_HD           PTR    I/O   HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**      REC_LOC_HD      PTR     I    HEAD POINTER TO RECORD POINTER STRUCTURE 
**                                    STRUCTURE
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
**      CUR_RP          PTR    I/O   CURRENT POINTER TO RECORD POINTER
**                                    STRUCTURE
**      NLD_RP          PTR     O    POINTER TO RECORD POINTER STRUCTURE
**                                    WHERE CORRESPONDING DR HAS AN 'R' 
**                                    LEADER IDENTIFIER    
**      SF_FMT          CHAR   N/A   FORMAT CORRESPONDING TO THE CURRENT
**                                    DATA VALUE
**      NLD             INT    I/O   NO LEADER FLAG
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
**      SF_STATE_DR     INT     O    SUBFIELD STATE (DR)
**                                    1 - NUMBER OF DIMENSIONS SUBFIELD
**                                    2 - LENGTH OF A DIMENSION SUBFIELD
**                                    3 - DATA VALUE STRING SUBFIELD
**                                    4 - FINISHED
**      NEXT            PTR    N/A   POINTER TO NEXT FILE MANAGEMENT
**                                    STRUCTURE
**      BACK            PTR    N/A   POINTER TO PREVIOUS FILE MANAGEMENT
**                                    STRUCTURE
**
**     RECORD POINTER STRUCTURE (RP)
**      NAME            TYPE   USE   DESCRIPTION
**      DR_START        INT    I/O   FILE LOCATION OF FIRST BYTE OF DATA RECORD
**      NEXT            PTR    I/O   POINTER TO NEXT DR_START
**      BACK            PTR    I/O   POINTER TO BACK DR_START
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR    I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/16/90  INITIAL PROLOG
**     P. HODGES                 05/16/90  INITIAL PDL
**     A. DEWITT                 05/30/90  MODIFY PDL TO CALL READ ROUTINES
**                                          INSTEAD OF LOAD ROUTINES TO MOVE
**                                          THROUGH FILE
**     A. DEWITT                 05/30/90  INITIAL CODE
**     A. DEWITT                 11/29/90  REINSTATE CALLS TO LOAD FUNCTIONS
**                                          TO REPLACE READS
**     A. DEWITT                 12/06/90  ADD CALL TO END123REC() TO CLEAR
**                                          STRUCTURE
**     A. DEWITT                 02/04/91  ADDED NLD LOGIC. ADDED CALL TO 
**                                          STC123EMPTY() TO DERTERMINE IF
**                                          STRUCTURE NOT EMPTY.  ADDED STATUS
**                                          PARAMETER.
**     A. DEWITT                 02/28/91  REDESIGN OF FUNCTION. CHANGE THE
**                                          DESIGN FROM HAVING TO REWIND THE
**                                          FILE AND LOADING UNTIL FOUND 
**                                          PREVIOUS RECORD TO ACCESS NEW 
**                                          RECORD POSITION STRUCTURE AND
**                                          MOVING DIRECTLY TO STARTING 
**                                          POSITION OF PREVIOUS FILE
**     J. TAYLOR   TASK 40       10/22/92  SET CURRENT DR TO NULL FOR 
**                                          LEADER R DATA RECORDS TO MATCH
**                                          PERFORMANCE OF LEADER D FILES
**
**    PDL:
**
**     INITIALIZE STATUS TO OKAY
**     CALL GET123LEVEL() TO GET APPROPRIATE LEVEL FOR THIS FILE
**     IF OPEN MODE IS WRITE
**     THEN
**        SET IS WRITE FILE FLAG TO TRUE
**     ELSE
**        SET IS WRITE FILE FLAG TO FALSE
**     ENDIF
**     INITIALIZE PARTIAL FLAG TO FALSE  
**     IF WRITE MODE AND DR_HD IS NOT NULL  
**     THEN 
**        IF STRUCTURE IS NOT EMPTY  
**        THEN 
**           SET OPEN MODE TO READ  
**           CALL END123REC() TO END THE DATA RECORD  
**           SET OPEN MODE BACK TO WRITE  
**           SET PARTIAL TO TRUE  
**        ENDIF
**     ENDIF
**     IF NOT PARTIAL RECORD  
**     THEN 
**        IF CUR_RP EQUALS HEAD POINTER  
**        THEN
**           { TRYING TO BACK UP PRIOR TO FIRST DATA RECORD }
**           SET STATUS TO FAILURE 
**           RETURN FAILURE
**        ELSE  
**           MOVE FILE POINTER TO START POSITION OF DATA RECORD  
**           IF CURRENT RECORD POSITION STRUCTURE EQUALS NLD_RP  
**           THEN 
**              SET NLD FLAG TO FALSE  
**              SET NLD_RP TO NULL  
**           ENDIF         
**           SET CUR_RP TO BACK POINTER  
**           SET CUR_RP NEXT BACK POINTER TO NULL  
**           SET CUR_RP NEXT NEXT POINTER TO NULL  
**           FREE CUR_RP NEXT  
**           SET CUR_RP NEXT TO NULL  
**        ENDIF
**        IF READ FILE AND STRUCTURE NOT EMPTY  
**        THEN
**           CALL END123REC() TO END THE DATA RECORD { CLEAR STRUCTURE }  
**        ENDIF
**        IF LEADER R
**        THEN
**           SET CURRENT DR TO NULL
**        ENDIF
**     ENDIF
**     SET SUBFIELD STATE TO OUT OF BOUNDS
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int bak123rec(FILE *fp, int *status)
{
   /* INTERNAL VARIABLES */
   int   is_wfil   ;
   int   partial   ;
  
   /* INITIALIZE STATUS TO OKAY */
   *status = 1;
   
   /* CALL GET123LEVEL() TO GET APPROPRIATE LEVEL FOR THIS FILE */
   if (!get123level(fp)) return(0);

   /* SET IS WRITE FILE FLAG */
   is_wfil = (cur_fm->open_mode == 'w');
      
   /* INITIALIZE PARTIAL FLAG TO FALSE */
   partial = 0;
   
   /* IF WRITE MODE AND DR_HD IS NOT NULL */
   if (is_wfil && cur_fm->dr_hd != NULL) {
      
      /* IF STRUCTURE IS NOT EMPTY */
      if (!stc123empty()) {
      
         /* SET OPEN MODE TO READ */
         cur_fm->open_mode = 'r';
      
         /* CALL END123REC() TO END THE DATA RECORD */
         if (!end123rec(fp)) return(0);
      
         /* SET OPEN MODE BACK TO WRITE */
         cur_fm->open_mode = 'w';
         
         /* SET PARTIAL TO TRUE */
         partial = 1;
         
      };   
   };

   /* IF NOT PARTIAL RECORD */
   if (!partial) {
      
      /* IF CUR_RP EQUALS HEAD POINTER */
      if (cur_fm->cur_rp == cur_fm->rec_loc_hd) {
      
         /* SET STATUS TO FAILURE 
             { TRYING TO BACK UP PRIOR TO FIRST DATA RECORD }
         */
         *status = 0;
         
         /* RETURN FAILURE */
         return(0);
      }
      else {
         
         /* MOVE FILE POINTER TO START POSITION OF DATA RECORD */
         if (fseek(fp,cur_fm->cur_rp->dr_start,SEEK_SET)) return(0);
         
         /* IF CURRENT RECORD POSITION STRUCTURE EQUALS NLD_RP */
         if (cur_fm->cur_rp == cur_fm->nld_rp) {
             
            /* SET NLD FLAG TO FALSE */
            cur_fm->nld = 0;
            
            /* SET NLD_RP TO NULL */
            cur_fm->nld_rp = NULL;
         };
         
         /* SET CUR_RP TO BACK POINTER */
         cur_fm->cur_rp = cur_fm->cur_rp->back;
         
         /* SET CUR_RP NEXT BACK POINTER TO NULL */
         cur_fm->cur_rp->next->back = NULL;
         
         /* SET CUR_RP NEXT NEXT POINTER TO NULL */
         cur_fm->cur_rp->next->next = NULL;
         
         /* FREE CUR_RP NEXT */
         free(cur_fm->cur_rp->next);
         
         /* SET CUR_RP NEXT TO NULL */
         cur_fm->cur_rp->next = NULL;
      }
      
      /* IF READ FILE AND STRUCTURE NOT EMPTY */
      if (!is_wfil && cur_fm->dr_hd != NULL) {
         
         /* CALL END123REC() TO END THE DATA RECORD { CLEAR STRUCTURE } */
         if (!end123rec(fp)) return(0);
      };
  
      /* IF LEADER R */
      if (cur_fm->nld) {

         /* SET CURRENT DR TO NULL */
         cur_fm->cur_dr = NULL;
      }
   };

   /* SET SUBFIELD STATE TO OUT OF BOUNDS */
   cur_fm->sf_state_dr = 4;
   
   /* RETURN SUCCESS */
   return(1);
}   
