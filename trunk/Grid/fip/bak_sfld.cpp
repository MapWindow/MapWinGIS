# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: BAK123SFLD 
** 
**    PURPOSE: TO RETRIEVE LAST SUBFIELD READ FROM OR WRITTEN TO A
**              DATA RECORD
** 
**    INVOCATION METHOD: BAK123SFLD(FP,STATUS) 
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION
**     FP            PTR        I       FILE POINTER
**     STATUS        INT        O       STATUS
**                                       0 = FAILURE
**                                       1 = OKAY
**                                       2 = START OF RECORD
**                                       3 = END OF RECORD
**                                       4 = END OF FILE (DR)
**                                       5 = END OF FIELD
**                                       6 = START OF FIELD
**     BAK123SFLD()  LOGICAL    O       SUCCESS FLAG
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     BAK123SFLD()     BACKS UP TO BEGINNING OF LAST SUBFIELD READ OR WRITTEN 
**                       MAXIMUM OF 1 LEVEL OF RECURSION
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER 
**     RD123SFLD()      READS THE NEXT SUBFIELD 
**     RT123PVFLD()     RETRIEVES PREVIOUS FIELD 
**     RT123PVSFLD()    RETRIEVES PREVIOUS SUBFIELD 
** 
**    INTERNAL VARIABLES: 
**     NAME            TYPE         DESCRIPTION
**     LEADID[2]       CHAR         LEADER IDENTIFIER
**     R_STATUS        INT          READ STATUS 
**                                   0 = FAILURE
**                                   1 = OKAY
**                                   2 = START OF RECORD
**                                   3 = END OF RECORD
**                                   4 = END OF FILE (DR)
**                                   5 = END OF FIELD
**                                   6 = START OF FIELD
**     STR_LEN         LONG         STRING LENGTH
**     TAG[10]         CHAR         FIELD TAG
**     TMP_DR          PTR          TEMPORARY POINTER TO DR STRUCTURE 
**   
**    GLOBAL REFERENCES: 
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE    USE   DESCRIPTION
**      TAG[10]         CHAR    N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     N/A   LENGTH OF A DESCRIPTIVE AREA DATA RECORD
**      FD_POS          INT     N/A   POSITION OF A DESCRIPTIVE AREA DATA RECORD
**      NUM_DIM         INT      I    NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR     I/O   HEAD POINTER TO DIMENSION LENGTHS (NO LABELS)
**      VALUES          PTR     I/O   HEAD POINTER TO DATA VALUE SUBFIELD RECORDS
**      NEXT            PTR     I/O   POINTER TO NEXT DATA RECORD
**
**     DIMENSION LENGTHS STRUCTURE (DM) 
**      NAME            TYPE   USE   DESCRIPTION 
**      LEN             INT    N/A   DIMENSION LENGTH 
**      NXT             PTR    I/O   POINTER TO NEXT DIMENSION LENGTH     
** 
**     DR DATA VALUE SUBFIELD STRUCTURE (DV) 
**      NAME            TYPE   USE   DESCRIPTION 
**      VALUE           CHAR   I/O   DATA VALUE 
**      NXT_VSET        PTR    I/O   POINTER TO NEXT SET OF DATA VALUES 
**      NXT_VAL         PTR    I/O   POINTER TO NEXT DATA VALUE SUBFIELD  
**                                    RECORD 
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
**      CUR_DD          PTR    N/A   CURRENT POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE ENTRY 
**      CUR_DM          PTR    I/O   CURRENT POINTER TO DIMENSION LENGTHS 
**                                    STRUCTURE ENTRY 
**      CUR_DR          PTR    I/O   CURRENT POINTER TO DATA RECORD STRUCTURE 
**                                    ENTRY 
**      CUR_DV          PTR    I/O   CURRENT POINTER TO DR DATA VALUE SUBFIELD 
**                                    STRUCTURE ENTRY 
**      ROW_DVH         PTR    I/O   CURRENT POINTER TO NEXT SET OF VALUES 
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
**      SF_STATE_DR     INT    I/O   SUBFIELD STATE (DR) 
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
**     GLB_STR[MAXSIZ]  CHAR   I/O   GLOBAL CHARACTER STRING USED FOR
**                                    PROCESSING
** 
**    GLOBAL CONSTANTS: NONE
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/16/90  INITIAL PROLOG 
**     P. HODGES                 05/16/90  INITIAL PDL 
**     P. HODGES                 06/12/90  INITIAL CODE 
**     A. DEWITT                 12/02/90  INSERTED LOGIC TO REMOVE DR
**                                          STRUCTURE WHEN BACKED UP TO
**                                          PREVIOUS DR
**     A. DEWITT                 02/02/91  ADDED STATE LOGIC - INTEGRATED
**                                          CALLS. ADDED STATUS PARAMETER AND
**                                           CODE TO ASSIGN VALUE.
**     A. DEWITT                 03/01/91  ADDED LOGIC TO CALL TO RT123PVFLD()
**                                          A SECOND TIME TO RETREIVE PREVIOUS
**                                          FIELD OF PREVIOUS DATA RECORD
**     A. DEWITT     TASK #40    11/15/01  ADDED LOGIC TO REMOVE THE LAST
**                                          ENTRY IN LINKED LIST OF DATA
**                                          VALUES WHEN THE CURRENT DATA
**                                          VALUE IS WITHIN THE CURRENT ROW
**                                          DATA VALUES LIST.
**     L. MCMILLION  92DR008     03/27/92  MOVED LOGIC WHICH LOCATES PREVIOUS
**                                          DR STRUCTURE TO IMMEDIATELY BEFORE
**                                          NLD CONDITIONALS IN ORDER TO
**                                          BACKUP 1 SUBFIELD FOR LEADER ID 'R'
**                                          AS WELL AS LEADER ID 'D'.
**     J. TAYLOR     92DR005     05/16/92  CHANGE CALLING SEQUENCE TO RD123SFLD
**     L. MCMILLION  TASK #40    11/20/92  PROLOG UPDATE PER QC
**     J. TAYLOR     93DR031     06/15/93  POSITIONED CURRENT DIMENSION LENGTH 
**                                          POINTER TO LAST DIMENSION LENGTH 
**                                          STRUCTURE BEFORE DELETING 
**                                          
**    PDL: 
** 
**     SET OUTPUT STATUS TO OKAY
**     CALL GET123LEVEL() TO RETRIEVE RELEVANT FM LAYER 
**     CALL RT123PVSFLD() TO RETRIEVE PREVIOUS SUBFIELD 
**     IF RT123PVSFLD() FAILED
**     THEN 
**        { NEED TO GET LAST SUBFIELD OF PREVIOUS FIELD } 
**        CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD 
**        IF RT123PVFLD() FAILED
**        THEN
**           SET OUTPUT STATUS TO FAILURE
**           RETURN FAILURE
**        ENDIF
**        IF CUR_DR IS DR_HD
**        THEN
**           CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD OF PREVIOUS DR
**           IF RT123PVFLD() FAILED
**           THEN
**              SET OUTPUT STATUS TO FAILURE
**              RETURN FAILURE
**           ENDIF
**        ENDIF
**        INITIALIZE CURRENT POINTERS
**        { NEED TO GET LAST SUBFIELD OF CURRENT FIELD } 
**        INITIALIZE READ STATUS TO OUT OF BOUNDS
**        WHILE READ STATUS DOES NOT INDICATE END OF FIELD
**         NOR END OF RECORD DO
**           CALL RD123SFLD() TO READ THE NEXT SUBFIELD 
**           IF RD123SFLD() FAILED
**           THEN
**              SET OUTPUT STATUS TO FAILURE
**              RETURN FAILURE
**           ENDIF
**           IF STATUS IS START OF RECORD 
**           THEN
**              IF NXT_VAL FIELD OF CUR_DV AND NXT_VSET FIELD OF ROW_DVH
**               ARE NULL
**              THEN
**                 SET READ STATUS TO END OF FIELD 
**              ENDIF
**           ENDIF
**        ENDWHILE 
**        IF FILE IN WRITE MODE
**        THEN
**           CALL RT123PVSFLD() TO RETRIEVE PREVIOUS SUBFIELD
**           IF RT123PVSFLD() FAILED
**           THEN
**              SET OUTPUT STATUS TO FAILURE
**              RETURN FAILURE
**           ENDIF
**        ENDIF
**        IF FILE IN READ MODE
**        THEN
**           CALL BAK123SFLD() TO BACK UP TO BEGINNING OF LAST SUBFIELD READ 
**           IF BAK123SFLD() FAILED
**           THEN
**              SET OUTPUT STATUS TO FAILURE
**              RETURN FAILURE
**           ENDIF
**        ENDIF
**     ELSE NO ERROR FROM RT123PVSFLD()
**        IF SUBFIELD STATE EQUALS NUMBER OF DIMENSIONS SUBFIELD
**        THEN
**           IF CUR_DR EQUALS DR_HD OR CUR_DR EQUALS DR_HD NEXT
**           THEN
**              SET OUTPUT STATUS TO START OF RECORD
**           ELSE
**              SET OUTPUT STATUS TO START OF FIELD
**           ENDIF
**        ELSEIF SUBFIELD STATE IS DATA VALUES SUBFIELD
**        THEN
**           IF NUM_DIM IS ZERO AND CUR_DV IS EITHER HEADER VALUE OR NULL
**           THEN
**              IF CUR_DR NOT EQUAL TO NEXT FIELD OF DR_HD
**              THEN
**                 SET OUTPUT STATUS TO START OF FIELD
**              ELSE
**                 SET OUTPUT STATUS TO START OF RECORD
**              ENDIF
**           ENDIF
**        ENDIF
**     ENDIF
**
**     IF FILE IN WRITE MODE 
**     THEN 
**        IF SUBFIELD STATE IS DATA VALUE STRING SUBFIELD OR SUBFIELD STATE
**         IS OUT OF BOUNDS
**        THEN 
**           IF CUR_DV EQUALS CUR_DR VALUES
**           THEN
**              SET ROW_DVH TO NULL
**              SET CUR_DV TO NULL
**              IF CUR_DR VALUES NOT NULL
**              THEN
**                 FREE CUR_DR VALUES
**                 SET CUR_DR VALUES TO NULL
**              ENDIF
**              IF CUR_DR NUM_DIM EQUALS ZERO
**              THEN
**                 { FIND PREVIOUS DR }
**                 SET TMP_DR TO DR_HD
**                 WHILE TMP_DR NOT NULL AND TMP_DR NEXT NOT CUR_DR DO
**                    SET TMP_DR TO TMP_DR NEXT
**                 ENDWHILE
**                 SET CUR_DR TO TMP_DR
**                 { REMOVE DR IF NOT NLD FLAG }
**                 IF LEADER EXISTS
**                 THEN
**                    SET CUR_DM TO NULL
**                    FREE TMP_DR NEXT
**                    SET TMP_DR NEXT TO NULL
**                    IF TMP_DR EQUALS DR_HD
**                    THEN
**                       SET CUR_DR TO NULL
**                       SET TMP_DR TO NULL
**                       FREE DR_HD
**                       SET DR_HD TO NULL
**                    ENDIF
**                    SET SUBFIELD STATE TO OUT OF BOUNDS
**                 ENDIF
**              ELSE CUR_DR NUM_DIM NOT ZERO
**                 SET SUBFIELD STATE TO DATA VALUE SUBFIELD
**              ENDIF
**           ELSE CUR_DV NOT EQUAL TO CUR_DR VALUES
**              IF CUR_DV NOT NULL 
**              THEN
**                 IF NXT_VAL OF CUR_DV EQUAL TO NULL
**                 THEN
**                    IF NXT_VSET OF ROW_DVH IS NULL
**                       CALL RT123PVSFLD() TO RETRIEVE PREVIOUS SUBFIELD
**                    ENDIF
**                    IF CUR_DV EQUALS CUR_DR VALUES POINTER
**                       SET ROW_DVH TO NULL
**                       SET CUR_DV TO NULL
**                       IF CUR_DR VALUES NOT NULL
**                          FREE CUR_DR VALUES NODE
**                          SET CUR_DR VALUES POINTER TO NULL
**                       ELSE
**                          IF NXT_VAL VALUE STRING OF CUR_DV NOT NULL
**                             FREE NXT_VAL VALUE STRING OF CUR_DV
**                             SET NXT_VAL VALUE OF CUR_DV TO NULL
**                          ENDIF
**                          FREE CUR_DV NXT_VAL
**                          SET CUR_DV NXT_VAL TO NULL
**                       ENDIF
**                    ELSE
**                       IF NXT_VSET VALUE OF ROW_DVH NOT NULL
**                       THEN
**                          FREE NXT_VSET VALUE OF ROW_DVH
**                          SET NXT_VSET VALUE OF ROW_DVH TO NULL
**                       ENDIF
**                       FREE ROW_DVH NXT_VSET
**                       SET ROW_DVH NXT_VSET TO NULL
**                    ENDIF
**                 ELSE CUR_DV LIES WITHIN THE LIST
**                    IF NXT_VAL VALUE OF CUR_DV NOT NULL
**                    THEN
**                       FREE NXT_VAL VALUE OF CUR_DV
**                       SET NXT_VAL VALUE OF CUR_DV TO NULL
**                    ENDIF
**                    FREE CUR_DV NXT_VAL
**                    SET CUR_DV NXT_VAL TO NULL
**                 ENDIF
**              ELSE CUR_DV IS NULL
**                 SET OUTPUT STATUS TO FAILURE
**                 RETURN FAILURE
**              ENDIF
**           ENDIF
**        ELSE IF SUBFIELD STATE IS LENGTH OF DIMENSION SUBFIELD
**        THEN 
**           POSITION CURRENT DIMESION LENGTH TO DIMENSION LENGTH HEADER
**           WHILE NOT AT LAST DIMENSION LENGTH STRUCTURE
**              MOVE TO LAST DIMENSION LENGTH STRUCTURE
**           IF NEXT CUR_DM NOT NULL
**           THEN
**              FREE NEXT CUR_DM 
**              SET NEXT DIMENSION LENGTH TO NULL 
**           ENDIF
**        ELSE SUBFIELD STATE IS NUMBER OF DIMENSIONS SUBFIELD 
**           SET CUR_DM TO NULL
**           IF DIM_LPTR NOT NULL
**           THEN
**              FREE LEN POINTED TO BY DIM_LPTR
**              SET DIM_LPTR TO NULL 
**           ENDIF
**           SET SUBFIELD STATE TO OUT OF BOUNDS
**           { FIND PREVIOUS DR }
**           SET TMP_DR TO DR_HD
**           WHILE TMP_DR NOT NULL AND TMP_DR NEXT NOT CUR_DR DO
**              SET TMP_DR TO TMP_DR NEXT
**           ENDWHILE
**           SET CUR_DR TO TMP_DR
**           { REMOVE DR IF NLD FLAG NOT SET}
**           IF LEADER EXISTS
**           THEN
**              IF TMP_DR NOT NULL
**              THEN
**                 FREE TMP_DR NEXT
**                 SET TMP_DR NEXT TO NULL
**              ENDIF
**              IF TMP_DR EQUALS DR_HD
**              THEN
**                 SET CUR_DR TO NULL
**                 SET TMP_DR TO NULL
**                 FREE DR_HD
**                 SET DR_HD TO NULL
**              ENDIF
**           ENDIF 
**        ENDIF 
**     ENDIF
** 
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int bak123sfld(FILE *fp,int *status)
{ 
   /* INTERNAL VARIABLES */
   char leadid[2];
   int r_status;
   char tag[10];
   struct dr *tmp_dr;
   long str_len;
              

   /* SET OUTPUT STATUS TO OKAY */
   *status = 1;
   
   /* CALL GET123LEVEL() TO RETRIEVE RELEVANT FM LAYER */
   if (!get123level(fp)) return (0);

   /* IF ERROR RETURNED FROM RT123PVSFLD() TO RETRIEVE PREVIOUS SUBFIELD */
   if (!rt123pvsfld()) {

      /* {NEED TO GET LAST SUBFIELD OF PREVIOUS FIELD} */

      /* CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD */
      if (!rt123pvfld()) {
         
         /* SET OUTPUT STATUS TO FAILURE */
         *status = 0;
         return (0);
      }   

      /* IF CUR_DR IS DR_HD */
      if (cur_fm->cur_dr == cur_fm->dr_hd) {
      
         /* CALL RT123PVFLD() TO RETRIEVE PREVIOUS FIELD OF PREVIOUS DR */
         if (!rt123pvfld()) {
         
            /* SET OUTPUT STATUS TO FAILURE */
            *status = 0;
            return (0);
         }
      }

      /* INITIALIZE CURRENT POINTERS */
      cur_fm->cur_dv = cur_fm->cur_dr->values;
      cur_fm->cur_dm = cur_fm->cur_dr->dim_lptr;
      
      /* { NEED TO GET LAST SUBFIELD OF CURRENT FIELD } */

      /* INITIALIZE READ STATUS TO OUT OF BOUNDS */
      r_status = -1;

      /* WHILE READ STATUS DOES NOT INDICATE END OF FIELD
          NOR END OR RECORD DO
      */
      while (r_status != 5 && r_status != 3) {

         /* CALL RD123SFLD() TO READ THE NEXT SUBFIELD */
         if (!rd123sfld(fp,tag,leadid,glb_str,&str_len,&r_status)) {
         
            /* SET OUTPUT STATUS TO FAILURE */
            *status = 0;
            return (0);
         };   
         
         /* IF READ STATUS IS START OF RECORD */
         if (r_status == 2) {
            
            /* IF NXT_VAL FIELD OF CUR_DV AND NXT_VSET FIELD OF ROW_DVH ARE
                NULL
            */
            if ((cur_fm->cur_dv->nxt_val == NULL) && 
                (cur_fm->row_dvh->nxt_vset == NULL)) {
               
               /* SET READ STATUS TO END OF FIELD */
               r_status = 5;
            };
         };          
      }
 
      /* IF FILE IN WRITE MODE */
      if (cur_fm->open_mode == 'w') {
       
         /* CALL RT123PVSFLD() TO RETRIEVE PREVIOUS SUBFIELD */
         if (!rt123pvsfld()) {
            
            /* SET OUTPUT STATUS TO FAILURE */
            *status = 0;
            return (0);
         };
      };
            
      /* IF FILE IN READ MODE */
      if (cur_fm->open_mode == 'r') {
      
         /* CALL BAK123SFLD() TO BACK UP TO BEGINNING OF LAST SUBFIELD READ */
         if (!bak123sfld(fp,status)) {
            
            /* SET OUTPUT STATUS TO FAILURE */
            *status = 0;
            return (0);
         };   
      };   
   }
   
   /* ELSE NO ERROR RETURNED FROM RT123PVSFLD() */
   else {
      
      /* IF SUBFIELD STATE EQUALS NUMBER OF DIMENSIONS SUBFIELD */
      if (cur_fm->sf_state_dr == 1) {
      
         /* IF CUR_DR EQUALS DR_HD OR CUR_DR EQUALS DR_HD NEXT */
         if ((cur_fm->cur_dr == cur_fm->dr_hd) || 
             (cur_fm->cur_dr == cur_fm->dr_hd->next)) {
             
            /* SET OUTPUT STATUS TO START OF RECORD */
            *status = 2;
         }
         else {
            
            /* SET OUTPUT STATUS TO START OF FIELD */
            *status = 6;
         };
      }
      
      /* ELSEIF SUBFIELD STATE IS DATA VALUES SUBFIELD */
      else if (cur_fm->sf_state_dr == 3) {
         
         /* IF NUM_DIM IS ZERO AND CUR_DV IS EITHER HEADER VALUE OR NULL */
         if ((cur_fm->cur_dr->num_dim == 0) && 
             ( cur_fm->cur_dv == NULL ||
               cur_fm->cur_dv == cur_fm->cur_dr->values)) {
            
            /* IF CUR_DR NOT EQUAL TO NEXT FIELD OF DR_HD */
            if (cur_fm->cur_dr != cur_fm->dr_hd->next) {
            
               /* SET OUTPUT STATUS TO START OF FIELD */
               *status = 6;
            }
            else {
               
               /* SET OUTPUT STATUS TO START OF RECORD */
               *status = 2;
            };
         };
      };                  
   };   

   /* IF FILE IN WRITE MODE */
   if (cur_fm->open_mode == 'w') {
 
      /* IF SUBFIELD STATE IS DATA VALUE STRING SUBFIELD OR SUBFIELD STATE
          IS OUT OF BOUNDS
      */
      if (cur_fm->sf_state_dr >= 3 || cur_fm->sf_state_dr < 1) {
      
         /* IF CUR_DV EQUALS CUR_DR VALUES */
         if (cur_fm->cur_dv == cur_fm->cur_dr->values ) { 
         
            /* SET ROW_DVH TO NULL */
            cur_fm->row_dvh = NULL;
 
            /* SET CUR_DV TO NULL */
            cur_fm->cur_dv = NULL;
   
            /* IF CUR_DR VALUES NOT NULL */
            if (cur_fm->cur_dr->values != NULL) {
            
               /* FREE CUR_DR VALUES */
               free(cur_fm->cur_dr->values);
            
               /* SET CUR_DR VALUES TO NULL */
               cur_fm->cur_dr->values = NULL;
            };   
            
            /* IF CUR_DR NUM_DIM EQUALS ZERO */
            if (cur_fm->cur_dr->num_dim == 0) {

               /* { FIND PREVIOUS DR } */

               /* SET TMP_DR TO DR_HD */
               tmp_dr = cur_fm->dr_hd;
               
               /* WHILE TMP_DR NOT NULL AND TMP_DR NEXT NOT CUR_DR DO */
               while (tmp_dr != NULL && tmp_dr->next != cur_fm->cur_dr )
                 
                  /* SET TMP_DR TO TMP_DR NEXT */
                  tmp_dr = tmp_dr->next;
                      
               /* SET CUR_DR TO TMP_DR */
               cur_fm->cur_dr = tmp_dr;
               
               /* { REMOVE DR IF NOT NLD FLAG } */
               
               /* IF LEADER EXISTS */
               if (!cur_fm->nld) {
               
                  /* SET CUR_DM TO NULL */
                  cur_fm->cur_dm = NULL;
                    
                  /* FREE TMP_DR NEXT */
                  free(tmp_dr->next);
                    
                  /* SET TMP_DR NEXT TO NULL */
                  tmp_dr->next = NULL;
                    
                  /* IF TMP_DR EQUALS DR_HD */
                  if (tmp_dr == cur_fm->dr_hd) {
                    
                     /* SET CUR_DR TO NULL */
                     cur_fm->cur_dr = NULL;
                  
                     /* SET TMP_DR TO NULL */
                     tmp_dr = NULL;
                  
                     /* FREE DR_HD */
                     free(cur_fm->dr_hd);
                  
                     /* SET DR_HD TO NULL */
                     cur_fm->dr_hd = NULL;
                  }   
                   
                  /* SET SUBFIELD STATE TO OUT OF BOUNDS */
                  cur_fm->sf_state_dr = 4;
               };
            }   
                    
            /* ELSE CUR_DR NUM_DIM NOT ZERO */
            else {
                    
               /* SET SUBFIELD STATE TO DATA VALUE SUBFIELD */
               cur_fm->sf_state_dr = 3;
            }
         }
                       
         /* ELSE CUR_DV NOT EQUAL TO CUR_DR VALUES FIELD */
         else {
              
            /* IF CUR_DV NOT NULL */
            if (cur_fm->cur_dv != NULL) {
                
               /* IF NXT_VAL OF CUR_DV EQUAL TO NULL */
               if (cur_fm->cur_dv->nxt_val == NULL) {
                  
                  /* IF NXT_VSET OF ROW_DVH IS NULL */ 
                  if (cur_fm->row_dvh->nxt_vset == NULL) {

                     /* CALL RT123PVSFLD() TO RETRIEVE PREVIOUS SUBFIELD */
                     if (!rt123pvsfld()) {
            
                        /* SET OUTPUT STATUS TO FAILURE */
                        *status = 0;
                        return (0);
                     }

                     /* IF CUR_DV EQUALS CUR_DR VALUES */
                     if (cur_fm->cur_dv == cur_fm->cur_dr->values ) { 
         
                        /* SET ROW_DVH TO NULL */
                        cur_fm->row_dvh = NULL;
 
                        /* SET CUR_DV TO NULL */
                        cur_fm->cur_dv = NULL;
   
                        /* IF CUR_DR VALUES NOT NULL */
                        if (cur_fm->cur_dr->values != NULL) {
            
                           /* FREE CUR_DR VALUES */
                           free(cur_fm->cur_dr->values);
                         
                           /* SET CUR_DR VALUES TO NULL */
                           cur_fm->cur_dr->values = NULL;
                        };   
                     }
                     else {
            
                        /* IF NXT_VAL VALUE OF CUR_DV NOT NULL */
                        if (cur_fm->cur_dv->nxt_val->value != NULL) {
                  
                           /* FREE NXT_VAL VALUE OF CUR_DV */
                           free(cur_fm->cur_dv->nxt_val->value);
                     
                           /* SET NXT_VAL VALUE OF CUR_DV TO NULL */
                           cur_fm->cur_dv->nxt_val->value = NULL;
                        };
                     
                        /* FREE CUR_DV NXT_VAL */
                        free(cur_fm->cur_dv->nxt_val);
                  
                        /* SET CUR_DV NXT_VAL TO NULL */
                        cur_fm->cur_dv->nxt_val = NULL;
                     };
                     
                  }
                  else {
 
                     /* IF NXT_VSET VALUE OF ROW_DVH NOT NULL */
                     if (cur_fm->row_dvh->nxt_vset->value != NULL) {
                     
                        /* FREE NXT_VSET VALUE OF ROW_DVH */
                        free(cur_fm->row_dvh->nxt_vset->value);
                     
                        /* SET NXT_VSET VALUE OF ROW_DVH TO NULL */
                        cur_fm->row_dvh->nxt_vset->value = NULL;
                     };  
                  
                     /* FREE ROW_DVH NXT_VSET */
                     free(cur_fm->row_dvh->nxt_vset);
                  
                     /* SET ROW_DVH NXT_VSET TO NULL */
                     cur_fm->row_dvh->nxt_vset = NULL;
                  };
               }
               
               /* ELSE CUR_DV LIES WITHIN THE LIST */
               else {

                  /* IF NXT_VAL VALUE OF CUR_DV NOT NULL */
                  if (cur_fm->cur_dv->nxt_val->value != NULL) {
                  
                     /* FREE NXT_VAL VALUE OF CUR_DV */
                     free(cur_fm->cur_dv->nxt_val->value);
                     
                     /* SET NXT_VAL VALUE OF CUR_DV TO NULL */
                     cur_fm->cur_dv->nxt_val->value = NULL;
                  };
                     
                  /* FREE CUR_DV NXT_VAL */
                  free(cur_fm->cur_dv->nxt_val);
                  
                  /* SET CUR_DV NXT_VAL TO NULL */
                  cur_fm->cur_dv->nxt_val = NULL;
               };
            }
            
            /* ELSE CUR_DV IS NULL */
            else {

               /* SET OUTPUT STATUS TO FAILURE */
               *status = 0;            
               return (0);
            };  
         };
      }

      /* ELSEIF SUBFIELD STATE IS LENGTH OF DIMENSION SUBFIELD */
      else if (cur_fm->sf_state_dr == 2) {

         /* POSITION CURRENT DIMESION LENGTH TO DIMENSION LENGTH HEADER */
         cur_fm->cur_dm = cur_fm->cur_dr->dim_lptr;

         /* WHILE NOT AT LAST DIMENSION LENGTH STRUCTURE */
         while (cur_fm->cur_dm->nxt->nxt != NULL) {
         
            /* MOVE TO LAST DIMENSION LENGTH STRUCTURE */
            cur_fm->cur_dm = cur_fm->cur_dm->nxt;
         }

         /* IF NEXT CUR_DM NOT NULL */
         if (cur_fm->cur_dm->nxt != NULL) {
         
            /* FREE NEXT CUR_DM */
            free(cur_fm->cur_dm->nxt);

            /* SET NEXT DIMENSION LENGTH TO NULL */
            cur_fm->cur_dm->nxt = NULL;
         };   
      }

      /* ELSE SUBFIELD STATE IS NUMBER OF DIMENSIONS SUBFIELD */
      else {

         /* SET CUR_DM TO NULL */
         cur_fm->cur_dm = NULL;
         
         /* IF DIM_LPTR NOT NULL */
         if (cur_fm->cur_dr->dim_lptr != NULL) {
         
            /* FREE LEN POINTED TO BY DIM_LPTR */
            free(cur_fm->cur_dr->dim_lptr);

            /* SET DIM_LPTR TO NULL */
            cur_fm->cur_dr->dim_lptr = NULL;
         };   

         /* SET SUBFIELD STATE TO OUT OF BOUNDS */
         cur_fm->sf_state_dr = 4;

         /* { FIND PREVIOUS DR } */

         /* SET TMP_DR TO DR_HD */
         tmp_dr = cur_fm->dr_hd;
               
         /* WHILE TMP_DR NOT NULL AND TMP_DR NEXT NOT CUR_DR DO */
         while (tmp_dr != NULL && tmp_dr->next != cur_fm->cur_dr )
                 
            /* SET TMP_DR TO TMP_DR NEXT */
            tmp_dr = tmp_dr->next;
                      
         /* SET CUR_DR TO TMP_DR */
         cur_fm->cur_dr = tmp_dr;
               
         /* { IF NLD FLAG NOT SET REMOVE DR } */
         
         /* IF LEADER EXISTS */
         if (!cur_fm->nld) {
         
            /* IF TMP_DR NOT NULL */
            if (tmp_dr != NULL) {
         
               /* FREE TMP_DR NEXT */
               free(tmp_dr->next);
               
               /* SET TMP_DR NEXT TO NULL */
               tmp_dr->next = NULL;
            };   
             
            /* IF TMP_DR EQUALS DR_HD */
            if (tmp_dr == cur_fm->dr_hd) {
               
               /* SET CUR_DR TO NULL */
               cur_fm->cur_dr = NULL;
                  
               /* SET TMP_DR TO NULL */
               tmp_dr = NULL;
                  
               /* FREE DR_HD */
               free(cur_fm->dr_hd);
                  
               /* SET DR_HD TO NULL */
               cur_fm->dr_hd = NULL;
            };
         };      
      }
   } 

   /* RETURN SUCCESS */
   return (1); 
} 
