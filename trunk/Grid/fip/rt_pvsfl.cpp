# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: RT123PVSFLD 
** 
**    PURPOSE: TO RETURN PREVIOUS SUBFIELD 
** 
**    INVOCATION METHOD: RT123PVSFLD() 
** 
**    ARGUMENT LIST: 
**     NAME            TYPE      USE      DESCRIPTION 
**     RT123PVSFLD()   LOGICAL    O       SUCCESS FLAG 
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     FREE123LAB()     DEALLOCATES LABELS POINTER STRUCTURE 
**     RD123SFLD()      READS THE NEXT SUBFIELD 
**     SETUP123LB()     INITIALIZES LABELS POINTER STRUCTURE 
** 
**    INTERNAL VARIABLES: 
**     NAME            TYPE               DESCRIPTION 
**     FD_PREV_VAL     LOGICAL            FLAG INDICATING IF THE PREVIOUS
**                                         DATA VALUE WAS FOUND
**     LEADID[2]       CHAR               LEADER IDENTIFIER
**     STATUS          INT                READ STATUS
**     STR_LEN         LONG               LENGTH OF STRING
**     TAG[10]         CHAR               FIELD TAG
**     TMP_PTRM        PTR                TEMPORARY DIMENSION LENGTH
**                                         STRUCTURE (DM) POINTER
**     TMP_PTRV        PTR                TEMPORARY DR DATA VALUES SUBFIELD
**                                         STRUCTURE (DV) POINTER 
**       
**    GLOBAL REFERENCES: 
**      
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA 
**                                    FIELD RECORD
**      FD_CNTRL[10]    CHAR   N/A   FIELD CONTROLS
**      NAME[]          PTR    N/A   CHARACTER STRING POINTER TO NAME
**      LABELS          PTR    N/A   HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR     I    ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
** 
**     DIMENSION LENGTHS STRUCTURE (DM) 
**      NAME            TYPE   USE   DESCRIPTION 
**      LEN             INT    N/A   DIMENSION LENGTH 
**      NXT             PTR    I/O   POINTER TO NEXT DIMENSION LENGTH     
** 
**     DATA RECORD STRUCTURE (DR) 
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD 
**      FD_LEN          INT    N/A   LENGTH OF DISCRIPTIVE AREA DATA RECORD  
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA 
**                                    RECORD 
**      NUM_DIM         INT     I    NUMBER OF DIMENSIONS (NO LABELS) 
**      DIM_LPTR        PTR     I    HEAD POINTER TO DIMENSION LENGTHS 
**                                    (NO LABELS) 
**      VALUES          PTR     I    HEAD POINTER TO DATA VALUE SUBFIELD 
**                                    RECORDS 
**      NEXT            PTR    N/A   POINTER TO NEXT DATA RECORD  
**
**     DR DATA VALUE SUBFIELD STRUCTURE (DV)
**      NAME            TYPE   USE   DESCRIPTION
**      VALUE[]         PTR    N/A   DATA VALUE
**      NXT_VSET        PTR     I    POINTER TO NEXT SET OF DATA VALUES
**      NXT_VAL         PTR     I    POINTER TO NEXT DATA VALUE SUBFIELD 
**                                    RECORD
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
**      DR_HD           PTR    N/A   HEAD POINTER TO DATA RECORD STRUCTURE 
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE 
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER 
**                                    STRUCTURE 
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK 
**                                    STRUCTURE 
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE ENTRY 
**      CUR_DM          PTR    I/O   CURRENT POINTER TO DIMENSION LENGTHS 
**                                    STRUCTURE ENTRY 
**      CUR_DR          PTR     I    CURRENT POINTER TO DATA RECORD STRUCTURE 
**                                    ENTRY 
**      CUR_DV          PTR    I/O   CURRENT POINTER TO DR DATA VALUE SUBFIELD 
**                                    STRUCTURE ENTRY 
**      ROW_DVH         PTR    I/O   CURRENT POINTER TO NEXT SET OF VALUES 
**                                    IN DR DATA VALUE SUBFIELD STRUCTURE ENTRY 
**      CUR_FC          PTR     O    CURRENT POINTER TO FORMAT CONTROLS 
**                                    STRUCTURE ENTRY 
**      CUR_LP          PTR    N/A   CURRENT POINTER TO LABELS POINTER 
**                                    STRUCTURE ENTRY 
**      CUR_SL          PTR    N/A   CURRENT POINTER TO DD-LABEL SUBFIELD 
**                                    STRUCTURE ENTRY 
**      CUR_FCR         PTR     O    CURRENT POINTER TO ROOT OF FORMAT 
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
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT 
**                                    STRUCTURE ENTRY 
**     GLB_STR[MAXSIZ]  CHAR   I/O   GLOBAL CHARACTER STRING USED FOR
**                                    PROCESSING
** 
**    GLOBAL CONSTANTS: NONE 
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/17/90  INITIAL PROLOG 
**     P. HODGES                 05/17/90  INITIAL PDL 
**     P. HODGES                 06/14/90  INITIAL CODE 
**     A. DEWITT                 07/27/90  MODIFY CODE TO FOLLOW SPECIFIED
**                                          CODING FORMAT
**     A. DEWITT                 12/02/90  MODIFY CODE/DESIGN TO ELIMINATE
**                                          MULTIPLE LIST TRAVERSAL AND 
**                                          CORRECTED START ERROR
**     A. DEWITT                 01/21/90  MODIFY CODE/DESIGN FOR SPECIAL
**                                          CASE WHEN A CALL TO BAK123FLD MADE 
**                                          THEN RT123PVSFL IS INVOKED BY A 
**                                          BAKSFLD
**     A. DEWITT                 01/22/91  MODIFY DESIGN/CODE FOR LOGIC NEEDED
**                                          WHEN A CURRENT DATA VALUE HAS NOT
**                                          BEEN ASSIGNED.
**     A. DEWITT     TASK #40    11/07/91  ADD STATEMENT TO ASSIGN ROW DATA 
**                                          VALUE HEADER TO CURRENT DATA VALUE
**                                          WHEN PREVIOUS VALUE IS DUMMY HEADER
**     J. TAYLOR     92DR005     05/16/92  CHANGED CALLING SEQUENCE TO RD123SFLD
**     L. MCMILLION  TASK #40    12/16/92  UPDATED PROLOG PER QC
**     J. TAYLOR     93DR032     05/26/93  POSITION CURRENT DM TO DIMESION
**                                          LENGTH HEADER BEFORE MOVING TO LAST
**                                          DIMENSION IN LIST
**     L. MCMILLION  93DR032     06/01/93  ADDED CHECK FOR CURRENT VALUE AT
**                                          VALUES NXT_VSET POINTER AND
**                                          CONSIDER DIMENSION LENGTH POINTER
**     J. TAYLOR     93DR031     06/10/93  MODIFIED CHECK FOR CURRENT DATA VALUE
**                                          EQUAL TO FIRST VALUE TO WORK IF
**                                          NXT_VSET HAS NOT BEEN ALLOCATED
** 
**    PDL: 
** 
**     IF DR SUBFIELD STATE IS NUMBER OF DIMENSIONS SUBFIELD 
**     THEN 
**        RETURN FAILURE 
**     ELSEIF DR SUBFIELD STATE IS LENGTH OF DIMENSION SUBFIELD 
**     THEN 
**        IF DIM_LPTR IS EQUAL TO CUR_DM { TOP OF LENGTHS } 
**        THEN 
**           SET DR SUBFIELD STATE TO NUMBER OF DIMENSIONS SUBFIELD 
**        ELSE 
**           SAVE CUR_DM TO TEMPORARY DM POINTER 
**           SET CUR_DM TO DIM_LPTR 
**           WHILE CUR_DM NEXT IS TEMPORARY DM POINTER DO
**              SET CUR_DM TO CUR_DM NEXT 
**           ENDWHILE 
**        ENDIF 
**     ELSE { DR SUBFIELD STATE IS DATA VALUE STRING SUBFIELD } 
**        IF CUR_DR IS NOT NULL
**        THEN
**           IF CUR_DV EQUALS VALUES POINTER OR, CUR_DV EQUALS VALUES NXT_VSET
**            AND DIMENSION LENGTH POINTER NOT NULL { DR FIELD CONTAINS
**            NUMBER OF DIMENSIONS AND DIMENSION LENGTHS } 
**           THEN 
**              IF NUMBER OF DIMENSIONS IS ZERO 
**              THEN 
**                 SET DR SUBFIELD STATE TO NUMBER OF DIMENSIONS SUBFIELD
**                 RETURN FAILURE 
**              ELSE { GO TO LAST DIMENSION LENGTH } 
**                 POSITION CURRENT DM AT DIMENSION LENGTH HEADER
**                 WHILE CUR_DM NEXT IS NOT NULL DO
**                    SET CUR_DM TO CUR_DM NEXT 
**                 ENDHWILE
**                 IF VALUES POINTER IS NOT NULL AND CUR_DV EQUALS FIRST VALUE 
**                 THEN
**                    SET CUR_DV TO DUMMY HEADER
**                    SET ROW_DVH TO CUR_DV POINTER VALUE
**                    IF OPEN MODE IS READ
**                    THEN
**                       SET SUBFIELD STATE DR TO DIMENSION LENGTH SUBFIELD
**                    ELSE
**                       SET SUBFIELD STATE DR TO DATA VALUE SUBFIELD
**                    ENDIF
**                 ELSE
**                    SET DR SUBFIELD STATE TO LENGTH OF DIMENSION SUBFIELD 
**                 ENDIF
**              ENDIF 
**           ELSE { PREVIOUS SUBFIELD IS A VALUE } 
**              SET DR SUBFIELD STATE TO DATA VALUE SUBFIELD
**              IF CUR_DV EQUALS NULL -- NO PREVIOUS VALUE
**              THEN
**                 { RETURN LAST VALUE }
**                 CALL FREE123LAB() TO DEALLOCATE LABELS POINTER STRUCTURE 
**                 CALL SETUP123LB() TO SET UP A NEW LABEL 
**                 { SET FORMAT TREE POINTERS }
**                 SET CUR_FC TO FORMAT ROOT
**                 SET CUR_FCR TO FORMAT ROOT
**                 INITIALIZE CURRENT POINTERS
**                 INITIALIZE STATUS TO OUT OF BOUNDS
**                 { NEED TO GET LAST SUBFIELD OF CURRENT FIELD }
**                 WHILE STATUS DOES NOT INDICATE END OF FIELD NOR
**                  END OF RECORD DO
**                    CALL RD123SFLD() TO READ THE NEXT SUBFIELD
**                    IF STATUS IS START OF RECORD
**                    THEN
**                       IF NXT_VAL FIELD OF CUR_DV AND NXT_VSET FIELD
**                        OF ROW_DVH ARE NULL
**                       THEN
**                          SET STATUS TO END OF FIELD
**                       ENDIF
**                    ENDIF
**                 ENDWHILE
**              ELSE IF CUR_DV EQUALS FIRST VALUE
**              THEN
**                 SET CUR_DV TO DUMMY HEADER
**                 SET ROW_DVH TO CUR_DV
**              ELSE { PREVIOUS VALUE EXISTS }
**                 SAVE CUR_DV TO TEMPORARY DV POINTER
**                 CALL FREE123LAB() TO DEALLOCATE LABELS POINTER STRUCTURE 
**                 CALL SETUP123LB() TO SET UP A NEW LABEL 
**                 { INITIALIZE FORMAT TREE POINTERS } 
**                 SET CUR_FC TO FORMAT ROOT
**                 SET CUR_FCR TO FORMAT ROOT
**                 SET CUR_DV TO VALUES POINTER
**                 SET FOUND PREVIOUS VALUE FLAG TO FALSE
**                 WHILE PREVIOUS VALUE NOT FOUND DO
**                    CALL RD123SFLD() TO READ THE NEXT SUBFIELD 
**                    IF NXT_VAL EQUALS TEMPORARY DV POINTER
**                    THEN
**                       SET FOUND PREVIOUS VALUE FLAG TO TRUE
**                    ELSE
**                       SET FOUND PREVIOUS VALUE FLAG TO FALSE
**                    ENDIF
**                    IF NOT FOUND PREVIOUS VALUE
**                    THEN
**                       IF NXT_VAL FIELD OF CUR_DV IS NULL
**                       THEN
**                          IF NXT_VSET OF ROW_DVH EQUALS TEMPORARY DV POINTER
**                          THEN
**                             SET FOUND PREVIOUS VALUE FLAG TO TRUE
**                          ELSE
**                             SET FOUND PREVIOUS VALUE FLAG TO FALSE
**                          ENDIF
**                       ENDIF
**                    ENDIF
**                 ENDWHILE
**              ENDIF
**           ENDIF
**        ELSE CUR_DR IS NULL
**           SET DR SUBFIELD STATE TO NUMBER OF DIMENSIONS SUBFIELD
**           RETURN FAILURE
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

int rt123pvsfld()

{

   /* LOCAL VARIABLES */
   int  fd_prev_val;
   char leadid[2];
   char tag[10];
   int  status ;
   long str_len;
   struct dm *tmp_ptrm;
   struct dv *tmp_ptrv;

   /* IF DR SUBFIELD STATE IS NUMBER OF DIMENSIONS SUBFIELD */
   if (cur_fm->sf_state_dr == 1) return(0);

   /* ELSEIF DR SUBFIELD STATE IS LENGTH OF DIMENSION SUBFIELD */
   else if (cur_fm->sf_state_dr == 2) {

      /* IF DIM_LPTR IS EQUAL TO CUR_DM { TOP OF LENGTHS } */
      if (cur_fm->cur_dr->dim_lptr == cur_fm->cur_dm) {

         /* SET DR SUBFIELD STATE TO NUMBER OF DIMENSIONS SUBFIELD */
         cur_fm->sf_state_dr = 1;

      }
      else {

         /* SAVE CUR_DM TO TEMPORARY DM POINTER */
         tmp_ptrm = cur_fm->cur_dm;

         /* SET CUR_DM TO DIM_LPTR */
         cur_fm->cur_dm = cur_fm->cur_dr->dim_lptr;

         /* WHILE CUR_DM NEXT IS TEMPORARY DM POINTER DO */
         while (cur_fm->cur_dm->nxt != tmp_ptrm) {

            /* SET CUR_DM TO CUR_DM NEXT */
            cur_fm->cur_dm = cur_fm->cur_dm->nxt;

         }

      }

   }
   /* ELSE { DR SUBFIELD STATE IS DATA VALUE STRING SUBFIELD } */
   else {

      /* IF CUR_DR IS NOT NULL */
      if (cur_fm->cur_dr != NULL) {
      
         /* IF CUR_DV EQUALS VALUES POINTER OR, CUR_DV EQUALS VALUES NXT_VSET
             AND DIMENSION LENGTH POINTER NOT NULL { DR FIELD CONTAINS
             NUMBER OF DIMENSIONS AND DIMENSION LENGTHS } 
         */
         if (cur_fm->cur_dv == cur_fm->cur_dr->values ||
          (cur_fm->cur_dv == cur_fm->cur_dr->values->nxt_vset &&
          cur_fm->cur_dr->dim_lptr != NULL)) {

            /* IF NUMBER OF DIMENSIONS IS ZERO */
            if (cur_fm->cur_dr->num_dim == 0) {
         
               /* SET DR SUBFIELD STATE TO NUMBER OF DIMENSIONS SUBFIELD */
               cur_fm->sf_state_dr = 1;
            
               /* RETURN FAILURE */
               return(0);
            }   

            /* ELSE { GO TO LAST DIMENSION LENGTH } */
            else {
 
               /* POSITION CURRENT DM AT DIMENSION LENGTH HEADER */
               cur_fm->cur_dm = cur_fm->cur_dr->dim_lptr;

               /* WHILE CUR_DM NEXT NOT NULL DO */
               while (cur_fm->cur_dm->nxt != NULL) {
 
                  /*  SET CUR_DM TO CUR_DM NEXT */
                  cur_fm->cur_dm = cur_fm->cur_dm->nxt;

               }
                
               /* IF VALUES POINTER IS NOT NULL AND CUR_DV EQUALS FIRST VALUE */
               if (cur_fm->cur_dr->values != NULL && cur_fm->cur_dv == cur_fm->cur_dr->values->nxt_vset) {
            
                  /* SET CUR_DV TO DUMMY HEADER */
                  cur_fm->cur_dv = cur_fm->cur_dr->values;
               
                  /* SET ROW_DVH TO CUR_DV POINTER VALUE */
                  cur_fm->row_dvh = cur_fm->cur_dv;
                  
                  /* IF OPEN MODE IS READ; SET SUBFIELD STATE DR 
                      TO DIMENSION LENGTH SUBFIELD */
                  if (cur_fm->open_mode == 'r') cur_fm->sf_state_dr = 2;

                  /* ELSE SET SUBFIELD STATE DR TO DATA VALUE SUBFIELD */
                  else cur_fm->sf_state_dr = 3;

               }
               /* ELSE SET DR SUBFIELD STATE TO LENGTH OF DIMENSION SUBFIELD */
               else {
                  cur_fm->sf_state_dr = 2;
               }

            };

         }
         /* ELSE { PREVIOUS SUBFIELD IS A VALUE } */
         else {
         
            /* SET DR SUBFIELD STATE TO DATA VALUE STRING SUBFIELD */
            cur_fm->sf_state_dr = 3;
         
            /* IF CUR_DV EQUALS NULL - NO PREVIOUS VALUE RETURN LAST VALUE */
            if (cur_fm->cur_dv == NULL) {
               
               /* CALL FREE123LAB() TO DEALLOCATE LABELS POINTER STRUCTURE */
               if (!free123lab()) return(0);

               /* CALL SETUP123LB() TO SET UP A NEW LABEL */
               if (!setup123lb()) return(0);

               /* { INITIALIZE FORMAT TREE POINTER } */

               /* SET CUR_FC TO FORMAT ROOT */
               cur_fm->cur_fc = cur_fm->cur_dd->fmt_rt;

               /* SET CUR_FCR TO FORMAT ROOT */
               cur_fm->cur_fcr = cur_fm->cur_dd->fmt_rt;

               /* INITIALIZE CURRENT POINTERS */
               cur_fm->cur_dv = cur_fm->cur_dr->values;
               cur_fm->cur_dm = cur_fm->cur_dr->dim_lptr;
               
               /* INITIALIZE STATUS TO OUT OF BOUNDS */
               status = -1;
                
               /* { NEED TO GET LAST SUBFIELD OF CURRENT FIELD } */

               /* WHILE STATUS DOES NOT INDICATE END OF FIELD
                   NOR END OF RECORD DO
               */
               while (status != 5 && status != 3) {
               
                  /* CALL RD123SFLD() TO READ THE NEXT SUBFIELD */
                  if (!rd123sfld(cur_fm->fp,tag,leadid,glb_str,&str_len,&status)) return (0);
                  
                  /* IF STATUS IS START OF RECORD */
                  if (status == 2) {
                  
                     /* IF NXT_VAL FIELD OF CUR_DV AND NXT_VSET FIELD OF 
                         ROW_DVH ARE NULL
                     */
                     if ((cur_fm->cur_dv->nxt_val == NULL) && 
                        (cur_fm->row_dvh->nxt_vset == NULL)) {
                     
                        /* SET STATUS TO END OF FIELD */
                        status = 5;
                     };
                  };          
               };
            }   
                
            /* ELSE IF CUR_DV EQUALS FIRST VALUE */
            else if (cur_fm->cur_dv == cur_fm->cur_dr->values->nxt_vset) {
            
               /* SET CUR_DV TO DUMMY HEADER */
               cur_fm->cur_dv = cur_fm->cur_dr->values;
               
               /* SET ROW_DVH TO CUR_DV POINTER VALUE */
               cur_fm->row_dvh = cur_fm->cur_dv;

            }
            
            /* ELSE { PREVIOUS VALUE EXISTS } */
            else {

               /* SAVE CUR_DV TO TEMPORARY DV POINTER */
               tmp_ptrv = cur_fm->cur_dv;

               /* CALL FREE123LAB() TO DEALLOCATE LABELS POINTER STRUCTURE */
               if (!free123lab()) return(0);

               /* CALL SETUP123LB() TO SET UP A NEW LABEL */
               if (!setup123lb()) return(0);

               /* { INITIALIZE FORMAT TREE POINTERS } */

               /* SET CUR_FC TO FORMAT ROOT */
               cur_fm->cur_fc = cur_fm->cur_dd->fmt_rt;

               /* SET CUR_FCR TO FORMAT ROOT */
               cur_fm->cur_fcr = cur_fm->cur_dd->fmt_rt;
  
               /* SET CUR_DV TO VALUES POINTER */
               cur_fm->cur_dv = cur_fm->cur_dr->values;

               /* SET FOUND PREVIOUS VALUE FLAG TO FALSE */
               fd_prev_val = 0;
             
               /* WHILE PREVIOUS VALUE NOT FOUND DO */
               while (!fd_prev_val) {
            
                  /* CALL RD123SFLD() TO READ THE NEXT SUBFIELD */
                  if (!rd123sfld (cur_fm->fp,tag,leadid,glb_str,&str_len,&status)) return(0);
            
                  /* IF NXT_VAL EQUALS TEMPORARY DV POINTER,
                      THEN SET FOUND PREVIOUS VALUE FLAG TO TRUE, 
                      OTHERWISE SET FOUND PREVIOUS VALUE FLAG TO FALSE
                  */
                  fd_prev_val = (cur_fm->cur_dv->nxt_val == tmp_ptrv); 
                       
                  /* IF NOT FOUND PREVIOUS VALUE */
                  if (!fd_prev_val) {
               
                     /* IF NXT_VAL FIELD OF CUR_DV IS NULL */
                     if (cur_fm->cur_dv->nxt_val == NULL ) {
                  
                        /* IF NXT_VSET OF ROW_DVH EQUALS TEMPORARY DV POINTER,
                            THEN SET FOUND PREVIOUS VALUE FLAG TO TRUE,
                            OTHERWISE SET FOUND PREVIOUS VALUE FLAG TO FALSE
                        */
                        fd_prev_val = (cur_fm->row_dvh->nxt_vset == tmp_ptrv);
                     };
                  };               
               };
            };
         };
      }

      /* CUR_DR IS NULL */
      else {

         /* SET DR SUBFIELD STATE TO NUMBER OF DIMENSIONS SUBFIELD */
         cur_fm->sf_state_dr = 1;
            
         /* RETURN FAILURE */
         return(0);          
      };     
   };

   /* RETURN SUCCESS */
   return(1);

}

