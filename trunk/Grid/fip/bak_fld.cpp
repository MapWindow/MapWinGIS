# include "stdafx.h"
/*****************************************************************************
** 
**    INVOCATION NAME: BAK123FLD 
** 
**    PURPOSE: TO RETRIEVE FIELD JUST READ OR TO ERASE FIELD JUST WRITTEN 
** 
**    INVOCATION METHOD: BAK123FLD(FP,STATUS)
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
**     BAK123FLD()  LOGICAL    O       SUCCESS FLAG
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME               DESCRIPTION 
**     DEL123DIM()        DELETE DIMENSION LENGTHS STRUCTURE
**     DEL123DRVALS()     DELETE DATA RECORD VALUES STRUCTURE
**     GET123LEVEL()      RETRIEVES APPROPRIATE DATA STRUCTURE LAYER 
**     RT123PVFLD()       RETRIEVES PREVIOUS FIELD 
**     STC123EMPTY()      DETERMINES IF A DATA RECORD STRUCTURE IS EMPTY
** 
**    INTERNAL VARIABLES: NONE 
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
**      DIM_LPTR        PTR    I/O   HEAD POINTER TO DIMENSION LENGTHS 
**                                    (NO LABELS) 
**      VALUES          PTR    I/O   HEAD POINTER TO DATA VALUE SUBFIELD 
**                                    RECORDS 
**      NEXT            PTR    I/O   POINTER TO NEXT DATA RECORD  
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
**                                    STRUCTURE 
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE ENTRY 
**      CUR_DM          PTR     O    CURRENT POINTER TO DIMENSION LENGTHS 
**                                    STRUCTURE ENTRY 
**      CUR_DR          PTR    I/O   CURRENT POINTER TO DATA RECORD STRUCTURE 
**                                    ENTRY 
**      CUR_DV          PTR     O    CURRENT POINTER TO DR DATA VALUE SUBFIELD 
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
**      NLD             INT     I    NO LEADER FLAG 
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
**     P. HODGES                 06/11/90  INITIAL CODE 
**     A. DEWITT                 12/10/90  REDESIGN/CODE TO HANDLE WRITE
**                                         MODE SPECIAL CASES
**     A. DEWITT                 02/04/91  1) ADDED LOGIC/CODE TO DELETE
**                                          CURRENT DR STRUCTURE IF ERASED ALL
**                                          SUBFIELDS.  IF AT HEADER DELETE
**                                          HEADER.  
**                                         2) ADDED CALL TO STC123EMPTY() TO
**                                          DETERMINE IF NEED TO CALL 
**                                          RT123PVFLD.
**                                         3) ADDED NLD LOGIC.
**                                         4) ADDED STATUS TO PARAMETER AND
**                                          ASSOCIATED LOGIC TO CODE.
**     L. MCMILLION              02/25/91  ADDED LOGIC TO SET STATUS TO
**                                          FAILURE IF RT123PVFLD() FAILED AND
**                                          START OF RECORD IF CUR_DR AT DR_HD
**     J. TAYLOR     TASK 40     10/22/92  ADD CALL TO RT123PVFLD FOR DATA 
**                                          FILE WITH LEADER R SO THAT 
**                                          BAK123FLD WOULD RETURN SAME RESULTS
**                                          FOR LEADER D & R
**     J. TAYLOR     93DR033     09/15/93  ADD CHECK TO DETERMINE IF THERE IS
**                                          AN ARRAY DESCRIPTOR IN THE DR 
**                                          BEFORE CALLING DEL123DIM
**
**    PDL: 
** 
**     INITIALIZE STATUS TO OKAY
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE STRUCTURE LEVEL
**      FOR THIS FILE 
**     IF FILE IN READ MODE
**     THEN
**        CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD
**        IF RT123PVFLD() RETURNED FAILURE
**        THEN
**           SET STATUS TO FAILURE
**           RETURN FAILURE
**        ENDIF
**        INITIALIZE CURRENT POINTERS
**        IF ARRAY DESCRIPTOR IN DR 
**        THEN
**           INITIALIZE CUR_DM TO DR DIM_LPTR 
**        ELSE IF ARRAY DESCRIPTOR IN DDR 
**        THEN
**           INITIALIZE CUR_DM TO DDR DIM_LPTR 
**        ENDIF
**        IF CUR_DR IS EQUAL TO DR_HD { FIRST FIELD IN RECORD }
**        THEN
**           SET STATUS TO START OF RECORD
**        ELSE IF CUR_DR NEXT FIELD IS NULL
**           SET STATUS TO END OF RECORD
**        ENDIF
**     ELSE FILE IN WRITE MODE
**        IF DR_HD IS NULL OR STRUCTURE EMPTY { EMPTY RECORD }
**        THEN
**           CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD
**           IF RT123PVFLD() FAILED
**           THEN
**              SET STATUS TO FAILURE
**              RETURN FAILURE
**           ENDIF
**           IF LEADER IS R AND CURRENT DR IS THE HEADER DR
**           THEN
**              IF RT123PVFLD() FAILED
**              THEN
**                 SET STATUS TO FAILURE
**                 RETURN FAILURE
**              ENDIF
**           ENDIF
**           IF CUR_DR NOT EQUAL TO DR_HD NEXT { RETURNED CUR_DR IS NOT
**            SINGLE FIELD IN PREVIOUS RECORD }
**           THEN
**              CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD
**              IF RT123PVFLD() FAILED
**              THEN
**                 SET STATUS TO FAILURE
**                 RETURN FAILURE
**              ENDIF
**              IF CUR_DR NEXT NOT NULL
**              THEN
**                 IF ARRAY DESCRIPTOR PRESENT IN DR
**                 THEN
**                    CALL DEL123DIM() TO DELETE NEXT DR DIMENSION LENGTH LIST
**                 ENDIF
**                 CALL DEL123DRVALS() TO DELETE NEXT DATA FIELD VALUES
**                 IF NLD FLAG IS NOT SET { LEADER/DIRECTORY EXISTS }
**                 THEN
**                    FREE NEXT DR STRUCTURE
**                    SET CUR_DR NEXT TO NULL 
**                 ENDIF
**              ENDIF 
**           ELSE CUR_DR IS EQUAL TO DR_HD NEXT
**              { DELETE DR STRUCTURE INCLUDING HEADER }
**              IF ARRAY DESCRIPTOR PRESENT IN DR
**              THEN
**                 CALL DEL123DIM() TO DELETE DR DIMENSION LENGTH LIST
**              ENDIF
**              CALL DEL123DRVALS() TO DELETE NEXT DATA FIELD VALUES
**              IF NLD FLAG IS NOT SET { LEADER/DIRECTORY EXISTS }
**              THEN
**                 FREE DR_HD NEXT
**                 SET DR_HD NEXT TO NULL
**                 FREE DR_HD
**                 SET DR_HD TO NULL
**                 SET CUR_DR TO NULL
**              ENDIF
**              SET STATUS TO END OF RECORD
**           ENDIF
**        ELSE RECORD NOT EMPTY
**           IF CUR_DR NOT EQUAL TO DR_HD NEXT
**           THEN
**              CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD
**              IF CUR_DR NEXT NOT NULL
**              THEN
**                 IF ARRAY DESCRIPTOR PRESENT IN DR
**                 THEN
**                    CALL DEL123DIM() TO DELETE DR DIMENSION LENGTH LIST
**                 ENDIF
**                 CALL DEL123DRVALS() TO DELETE NEXT DATA FIELD VALUES
**                 IF NLD FLAG IS NOT SET { LEADER EXISTS }
**                 THEN
**                    FREE CUR_DR NEXT DR STRUCTURE
**                    SET CUR_DR NEXT TO NULL
**                 ENDIF
**              ENDIF
**              IF CUR_DR EQUALS DR_HD NEXT
**              THEN
**                 SET STATUS TO START OF RECORD
**              ENDIF
**           ELSE CUR_DR EQUAL TO DR_HD NEXT
**              { DELETE DR STRUCTURE INCLUDING HEADER }
**              IF ARRAY DESCRIPTOR PRESENT IN DR
**              THEN
**                 CALL DEL123DIM() TO DELETE DR DIMENSION LENGTH LIST
**              ENDIF
**              CALL DEL123DRVALS() TO DELETE NEXT DATA FIELD VALUES
**              IF NLD FLAG IS NOT SET { LEADER EXISTS }
**              THEN
**                 FREE DR_HD NEXT DR STRUCTURE
**                 SET DR_HD NEXT TO NULL
**                 FREE DR_HD
**                 SET DR_HD TO NULL
**                 SET CUR_DR TO NULL
**              ENDIF
**              SET STATUS TO END OF RECORD
**           ENDIF
**        ENDIF
**        SET CURRENT POINTERS TO NULL
**     ENDIF
**     SET DR SUBFIELD STATE TO OUT OF BOUNDS
**     
**     RETURN SUCCESS 
** 
******************************************************************************
**    CODE SECTION 
**
*****************************************************************************/
#include "stc123.h"

int bak123fld(FILE *fp, int *status)
{

   /* INITIALIZE STATUS TO OKAY */
   *status = 1;
   
   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE STRUCTURE LEVEL
       FOR THIS FILE
   */
   if (!get123level(fp)) return (0);

   /* IF FILE IN READ MODE */
   if (cur_fm->open_mode == 'r') {

      /* CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD */
      if (!rt123pvfld()) {
      
         /* SET STATUS TO FAILURE, RETURN FAILURE */
         *status = 0;
         return (0);
      }

      /* INITIALIZE CURRENT POINTERS */
      cur_fm->cur_dv = cur_fm->cur_dr->values;

      /* IF ARRAY DESCRIPTOR IN DR */
      if (cur_fm->cur_dr->dim_lptr != NULL) {

         /* INITIALIZE CUR_DM TO DR DIM_LPTR */
         cur_fm->cur_dm = cur_fm->cur_dr->dim_lptr;
      }

      /* ELSE IF ARRAY DESCRIPTOR IN DDR */
      else if (cur_fm->cur_dd->dim_lptr != NULL) {

         /* INITIALIZE CUR_DM TO DDR DIM_LPTR */
         cur_fm->cur_dm = cur_fm->cur_dd->dim_lptr;
      }
      
      /* IF CUR_DR EQUAL TO DR_HD  { FIRST FIELD IN RECORD } */
      if (cur_fm->cur_dr == cur_fm->dr_hd) {
         
         /* SET STATUS TO START OF RECORD */
         *status = 2;

      }
      /* ELSE IF CUR_DR NEXT FIELD IS NULL */
      else if (cur_fm->cur_dr->next == NULL) {
         
         /* SET STATUS TO END OF RECORD */
         *status = 3;

      };
         
   }
   
   /* ELSE FILE IN WRITE MODE */
   else {
      
      /* IF DR_HD IS NULL OR STRUCTURE EMPTY  - { EMPTY RECORD } */
      if (cur_fm->dr_hd == NULL || stc123empty()) {

         /* CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD */
         if (!rt123pvfld()) {
            
            /* SET STATUS TO FAILURE AND RETURN FAILURE */
            *status = 0;
            return (0); 
         };     
         
         /* IF LEADER IS R AND CURRENT DR IS HEADER DR */
         if (cur_fm->nld && cur_fm->cur_dr == cur_fm->dr_hd) {
         
            /* CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD */
            if (!rt123pvfld()) {
            
               /* SET STATUS TO FAILURE AND RETURN FAILURE */
               *status = 0;
               return (0); 
            }
         };     
         
         /* IF CUR_DR NOT EQUAL TO DR_HD NEXT { RETURNED CUR_DR IS NOT 
             SINGLE FIELD IN PREVIOUS RECORD }
         */
         if (cur_fm->cur_dr != cur_fm->dr_hd->next) {

            /* CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD */
            if (!rt123pvfld()) { 
               
               /* SET STATUS TO FAILURE AND RETURN FAILURE */
               *status = 0;             
               return (0);
            };    
            
            /* IF CUR_DR NEXT NOT NULL */
            if (cur_fm->cur_dr->next != NULL) {
           
               /* IF ARRAY DESCRIPTOR PRESENT IN DR */
               if (cur_fm->cur_dr->dim_lptr != NULL) { 

                  /* DELETE DIMENSION LENGTH LIST */
                  cur_fm->cur_dr->next->dim_lptr = del123dim(cur_fm->cur_dr->next->dim_lptr);
               }

               /* DELETE VALUES LIST */
               cur_fm->cur_dr->next->values = del123drvals(cur_fm->cur_dr->next->values);
               
               /* IF NLD FLAG IS NOT SET */
               if (!cur_fm->nld) {
               
                  /* FREE DR STRUCTURE */
                  free(cur_fm->cur_dr->next);
               
                  /* SET NEXT FIELD OF CUR_DR TO NULL */
                  cur_fm->cur_dr->next = NULL;
               };   
            };
         }

         /* ELSE CUR_DR EQUALS DR_HD NEXT */
         else {
            
            /* { DELETE DR STRUCTURE INCLUDING HEADER } */

            /* IF ARRAY DESCRIPTOR PRESENT IN DR */
            if (cur_fm->cur_dr->dim_lptr != NULL) { 

               /* DELETE DIMENSION LENGTH LIST */
               cur_fm->dr_hd->next->dim_lptr = del123dim(cur_fm->dr_hd->next->dim_lptr);
            }
               
            /* DELETE VALUES LIST */
            cur_fm->dr_hd->next->values = del123drvals(cur_fm->dr_hd->next->values);
               
            /* IF NLD FLAG IS NOT SET */
            if (!cur_fm->nld) {
            
               /* FREE DR STRUCTURE */
               free(cur_fm->dr_hd->next);
               
               /* SET DR_HD NEXT TO NULL */
               cur_fm->dr_hd->next = NULL;
            
               /* FREE HEADER */
               free(cur_fm->dr_hd);
            
               /* SET HEADER TO NULL */
               cur_fm->dr_hd = NULL;
              
               /* SET CUR_DR TO NULL */
               cur_fm->cur_dr = NULL;
            };   
         
         /* SET STATUS TO END OF RECORD */
         *status = 3;
         
         };          
      }

      /* ELSE RECORD IS NOT EMPTY */
      else {        

         /* IF CUR_DR NOT EQUAL TO HEADER NEXT */
         if (cur_fm->cur_dr != cur_fm->dr_hd->next) {

            /* CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD */
            if (!rt123pvfld()) return (0); 
            
            /* IF CUR_DR NEXT NOT NULL */
            if (cur_fm->cur_dr->next != NULL) {

               /* IF ARRAY DESCRIPTOR PRESENT IN DR */
               if (cur_fm->cur_dr->dim_lptr != NULL) { 
            
                  /* DELETE DIMENSION LENGTH LIST */
                  cur_fm->cur_dr->next->dim_lptr = del123dim(cur_fm->cur_dr->next->dim_lptr);
               }

               /* DELETE VALUES LIST */
               cur_fm->cur_dr->next->values = del123drvals(cur_fm->cur_dr->next->values);
               
               /* IF NLD FLAG IS NOT SET */
               if (!cur_fm->nld) {
               
                  /* FREE DR STRUCTURE */
                  free(cur_fm->cur_dr->next);
               
                  /* SET NEXT FIELD OF CUR_DR TO NULL */
                  cur_fm->cur_dr->next = NULL;
               };   
            };
            
            /* IF CUR_DR EQUALS NEXT FIELD OF DR_HD  { FIRST FIELD } */
            if (cur_fm->cur_dr == cur_fm->dr_hd->next) {
               
               /* SET STATUS TO START OF RECORD */
               *status = 2;
            };   
         }
         
         /* ELSE CUR_DR EQUALS HEADER NEXT */
         else {
            
            /* { DELETE DR STRUCTURE INCLUDING HEADER } */
           
            /* IF ARRAY DESCRIPTOR PRESENT IN DR */
            if (cur_fm->cur_dr->dim_lptr != NULL) { 

               /* DELETE DIMENSION LENGTH LIST */
               cur_fm->dr_hd->next->dim_lptr = del123dim(cur_fm->dr_hd->next->dim_lptr);
            }

            /* DELETE VALUES LIST */
            cur_fm->dr_hd->next->values = del123drvals(cur_fm->dr_hd->next->values);
               
            /* IF NLD FLAG IS NOT SET */
            if (!cur_fm->nld) {
            
               /* FREE DR STRUCTURE */
               free(cur_fm->dr_hd->next);
               
               /* SET DR_HD NEXT TO NULL */
               cur_fm->dr_hd->next = NULL;
            
               /* FREE HEADER */
               free(cur_fm->dr_hd);
            
               /* SET HEADER TO NULL */
               cur_fm->dr_hd = NULL;
               
               /* SET CUR_DR TO NULL */
               cur_fm->cur_dr = NULL;
               
            };   

            /* SET STATUS TO END OF RECORD */
            *status = 3;
         
         };          
      };         
   
      /* SET CURRENT POINTERS TO NULL */
      cur_fm->cur_dv = NULL;
      cur_fm->cur_dm = NULL;
   
   };


   /* SET DR SUBFIELD STATE TO OUT OF BOUNDS */
   cur_fm->sf_state_dr = 4;
   
   /* RETURN SUCCESS */
   return (1);
}
