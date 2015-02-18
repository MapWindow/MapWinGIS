# include "stdafx.h"
/***************************************************************************
** 
**    INVOCATION NAME: ER123DDSFLD 
** 
**    PURPOSE: TO ERASE DATA DESCRIPTIVE RECORD SUBFIELD 
** 
**    INVOCATION METHOD: ER123DDSFLD(FP,STATUS) 
** 
**    ARGUMENT LIST: 
**     NAME           TYPE     USE      DESCRIPTION
**     FP             PTR       I       FILE POINTER
**     STATUS         INT       O       STATUS
**                                       0 = FAILURE
**                                       1 = OKAY
**                                       2 = START OF RECORD
**                                       3 = END OF RECORD
**                                       4 = END OF FILE
**                                       5 = END OF FIELD
**                                       6 = START OF FIELD
** 
**     ER123DDSFLD()  LOGICAL   O       SUCCESS FLAG
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     DEL123DIM()      DELETES DIMENSION LENGTHS STRUCTURE
**     DEL123FMT()      DELETES FORMAT CONTROLS STRUCTURE
**     DEL123LABS()     DEALLOCATES LABELS POINTER STRUCTURE
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     IS123INTGR()     DETERMINES WHETHER OR NOT A CHARACTER STRING IS AN
**                       INTEGER
**
**    INTERNAL VARIABLES: 
**     NAME       TYPE               DESCRIPTION 
**     INT_TAG    INT                TAG IN INTEGER FORMAT
**     TMP_PTRD   PTR                TEMPORARY POINTER TO DATA DESCRIPTIVE (DD)
**                                    RECORD STRUCTURE ENTRY 
**     TMP_PTRL   PTR                TEMPORARY POINTER TO LIST OF TAG  
**                                    PAIR RECORDS 
**   
**    GLOBAL REFERENCES: 
**
**     DATA DESCRIPTIVE FILE CONTROL RECORD STRUCTURE (CR)
**      NAME            TYPE   USE   DESCRIPTION 
**      F_TITLE         CHAR    O    FILE TITLE 
**      TAG_L           PTR    I/O   POINTER TO LIST OF TAG PAIR RECORDS 
**      U_AFD[]         PTR     O    CHARACTER STRING POINTER TO USERS  
**                                    AUGMENTED FILE DESCRIPTION 
**       
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD) 
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD 
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA  
**                                    FIELD RECORD 
**      FD_CNTRL[10]    CHAR    I    FIELD CONTROLS
**      NAME[]          PTR     O    CHARACTER STRING POINTER TO NAME 
**      NUM_DIM         INT     O    NUMBER OF DIMENSIONS IN ARRAY DESCRIPTOR
**      DIM_LPTR        PTR    I/O   HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT     O    NUMBER OF ELEMENTS IN PRIMARY DIMENSION
**      LABELS          PTR     O    HEAD POINTER TO A LINKED LIST CONTAINING 
**                                    LINKED LIST OF DD LABEL SUBFIELD 
**                                    STRUCTURES 
**      FMT_RT          PTR     O    ROOT POINTER TO FORMAT CONTROLS BINARY 
**                                    TREE STRUCTURE 
**      NEXT            PTR    I/O   POINTER TO NEXT DATA DESCRIPTIVE
**                                    STRUCTURE 
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT    N/A   DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT     I    INTERCHANGE LEVEL
**      LEAD_ID         CHAR   N/A   LEADER IDENTIFIER 
**      ICE_IND         CHAR   N/A   INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR   N/A   RESERVE SPACE CHARACTER
**      APPL_IND        CHAR   N/A   APPLICATION INDICATOR
**      FD_CNTRL_L      INT    N/A   FIELD CONTROL LENGTH
**      DA_BASE         INT    N/A   BASE ADDRESS OF DESCRIPTIVE AREA 
**      CCS_IND[4]      CHAR   N/A   CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH 
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION  
**      S_RESV          INT    N/A   RESERVED DIGIT
**      S_TAG           INT    N/A   SIZE OF FIELD TAG 
**
**     FILE MANAGEMENT STRUCTURE (FM) 
**      NAME            TYPE   USE   DESCRIPTION 
**      FP              PTR    N/A   FILE POINTER 
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR    I    OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE 
**                                    CONTROL RECORD STRUCTURE 
**      DD_HD           PTR    I/O   HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE 
**      DL_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE LEADER 
**                                    STRUCTURE 
**      DR_HD           PTR    N/A   HEAD POINTER TO DATA RECORD STRUCTURE 
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE 
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER 
**                                    STRUCTURE 
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK 
**                                    STRUCTURE 
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR    I/O   CURRENT POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE ENTRY 
**      CUR_DM          PTR    N/A   CURRENT POINTER TO DIMENSION LENGTHS 
**                                    STRUCTURE ENTRY 
**      CUR_DR          PTR    N/A   CURRENT POINTER TO DATA RECORD STRUCTURE 
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
**      SF_STATE_DD     INT    I/O   SUBFIELD STATE (DD) 
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
**     DATA DESCRIPTIVE TAG PAIR STRUCTURE (TL)  
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG_1[10]       CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      TAG_2[10]       CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      NEXT            PTR    I/O   POINTER TO DATA DESCRIPTIVE TAG PAIR 
**                                    STRUCTURE 
**    GLOBAL VARIABLES: 
**     NAME             TYPE   USE   DESCRIPTION 
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT 
**                                    STRUCTURE ENTRY 
**
**    GLOBAL CONSTANTS:
**     NAME          TYPE      DESCRIPTION
**     FCDSTYPE      INT       RELATIVE POSITION OF THE DATA STRUCTURE TYPE 
**                              WITHIN THE FIELD CONTROL 
**     FCDTYPE       INT       RELATIVE POSITION OF THE DATA TYPE WITHIN THE
**                              FIELD CONTROL 
**
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/18/90  INITIAL PROLOG 
**     P. HODGES                 05/18/90  INITIAL PDL 
**     P. HODGES                 06/11/90  INITIAL CODE
**     L. MCMILLION              10/09/90  ADDED LOGIC TO PROCESS TAG 0..0
**     L. MCMILLION              10/23/90  INSERTED LOGIC TO ERASE DD ENTRY FOR
**                                          LEVEL 1 FILE WITHIN NAME SUBFIELD
**                                          STATE
**     L. MCMILLION              10/25/90  ADDED LOGIC TO ERASE DD ENTRY IF
**                                          ERASING FIELDS ARE BEING ERASED
**                                          FROM THE TOP OF THE DD LIST
**                                          (STARTING AT DD_HD)
**     L. MCMILLION              01/21/91  ADDED STATUS TO ARGUMENT LIST AND
**                                          LOGIC TO SET STATUS ACCORDINGLY
**     L. MCMILLION              02/19/91  MODIFIED LOGIC TO SET SUBFIELD
**                                          STATE AND STATUS TO FINISHED AND
**                                          START OF FIELD, RESPECTIVELY, FOR
**                                          TAG 0..2
**     L. MCMILLION  (TASK #40)  10/07/91  DIVIDED IF STATEMENT RESPONSIBLE
**                                          FOR SETTING STATUS TO START OF
**                                          RECORD DUE TO ACCESSING NULL
**                                          POINTERS WHEN ERASING FIELDS FROM
**                                          THE TOP OF THE DD LIST
**     L. MCMILLION  92DR010     03/31/92  ADDED CODE TO REMOVE DD STRUCTURE
**                                          CONTAINING TAG 0..2 FOR USER
**                                          AUGMENTED FILE DESCRIPTION
**     L. MCMILLION  TASK #40    12/01/92  PROLOG/PDL UPDATE
**     L. MCMILLION  93DR033     06/17/93  ADDED CALL TO DEL123DIM() TO DELETE
**                                          DIMENSION LENGTHS STRUCTURES OF
**                                          ARRAY DESCRIPTOR
**     L. MCMILLION  93DR033     11/11/93  RESET NUMBER OF DIMENSIONS AND 
**                                          PRIMARY DIMENSION IN DD STRUCTURE
**                                          WHEN ARRAY DESCRIPTORS DELETED
**          
**    PDL: 
**
**     INITIALIZE STATUS TO FAILURE
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER
**     ENSURE FILE IN WRITE MODE 
**     ENSURE NOT BACKED UP TO TOP OF STRUCTURE 
**     CONVERT TAG IN CUR_DD TO INTEGER
**     IF FILE INTERCHANGE LEVEL IS 1
**     THEN
**        SET SF_STATE_DD TO NAME SUBFIELD
**     ELSE
**        IF TAG IS FILE CONTROL (0..0)
**        THEN
**           IF SF_STATE_DD IS FINISHED
**           THEN
**              IF FILE INTERCHANGE LEVEL IS 2
**              THEN
**                 SET SF_STATE_DD TO NAME SUBFIELD
**              ELSE
**                 SET SF_STATE_DD TO LABELS SUBFIELD
**              ENDIF
**           ELSE
**              DECREMENT SF_STATE_DD
**           ENDIF
**        ELSE
**           IF FD_CNTRL INDICATES ELEMENTARY DATA STRUCTURE
**           THEN
**              CASE SF_STATE_DD
**
**              2: { NAME SUBFIELD }
**                 DECREMENT SF_STATE_DD
**              4: { FORMATS SUBFIELD }
**                 SET SF_STATE_DD TO NAME SUBFIELD
**              5: { FINISHED }
**                 IF FD_CNTRL INDICATES A NON-BIT FIELD
**                 THEN
**                    SET SF_STATE_DD TO NAME SUBFIELD
**                 ELSE
**                    DECREMENT SF_STATE_DD
**                 ENDIF
**              ENDCASE
**           ELSE
**              DECREMENT SF_STATE_DD
**           ENDIF
**        ENDIF
**     ENDIF
**
**     IF TAG IS USERS AUGMENTED FILE DESCRIPTION (0..2)
**     THEN 
**        FREE U_AFD 
**        SET U_AFD TO NULL
**        SET TMP_PTRD TO DD_HD 
**        IF CUR_DD NEXT IS DD_HD
**        THEN
**           { ERASING DD ENTRIES FROM TOP OF DD LIST }
**           SET CUR_DD NEXT TO NULL
**        ELSE
**           { ERASING DD ENTRIES FROM BOTTOM OF DD LIST }
**           WHILE TMP_PTRD NEXT NOT CUR_DD DO
**              SET TMP_PTRD TO TMP_PTRD NEXT
**           ENDWHILE
**           SET TMP_PTRD NEXT TO CUR_DD NEXT
**        ENDIF
**        FREE CUR_DD 
**        SET CUR_DD TO TMP_PTRD 
**        SET SF_STATE_DD TO FINISHED
**        SET STATUS TO START OF FIELD
**     ENDIF 
**     CASE SF_STATE_DD 
**        1: { FIELD CONTROL SUBFIELD } 
**           SET TMP_PTRD TO DD_HD 
**           IF CUR_DD NEXT IS DD_HD
**           THEN
**              { ERASING DD ENTRIES FROM TOP OF DD LIST }
**              SET CUR_DD NEXT TO NULL
**           ELSE
**              { ERASING DD ENTRIES FROM BOTTOM OF DD LIST }
**              WHILE TMP_PTRD NEXT NOT CUR_DD DO
**                 SET TMP_PTRD TO TMP_PTRD NEXT
**              ENDWHILE
**              SET TMP_PTRD NEXT TO CUR_DD NEXT
**           ENDIF
**           FREE CUR_DD 
**           SET CUR_DD TO TMP_PTRD 
**           SET SF_STATE_DD TO FINISHED 
**           SET STATUS TO START OF FIELD
**        2: { NAME SUBFIELD } 
**           IF TAG IS FILE CONTROL (0..0)
**           THEN 
**              FREE F_TITLE   
**              SET F_TITLE TO NULL
**              IF FILE INTERCHANGE LEVEL IS 2
**              THEN
**                 SET STATUS TO END OF FIELD
**              ELSE
**              THEN
**                 SET STATUS  TO OKAY
**              ENDIF
**           ELSE
**              IF NAME EXISTS   
**              THEN
**                 FREE NAME   
**                 SET NAME TO NULL
**              ENDIF
**           ENDIF
**           IF FILE INTERCHANGE LEVEL IS 1
**           THEN
**              SET TMP_PTRD TO DD_HD
**              IF CUR_DD NEXT IS DD_HD
**              THEN
**                 { ERASING DD ENTRIES FROM TOP OF DD LIST }
**                 SET CUR_DD NEXT TO NULL
**              ELSE
**                 { ERASING DD ENTRIES FROM BOTTOM OF DD LIST }
**                 WHILE TMP_PTRD NEXT NOT CUR_DD DO
**                    SET TMP_PTRD TO TMP_PTRD NEXT
**                 ENDWHILE
**                 SET TMP_PTRD NEXT TO CUR_DD NEXT
**              ENDIF
**              FREE CUR_DD
**              SET CUR_DD TO TMP_PTRD
**              SET SF_STATE_DD TO FINISHED
**              SET STATUS TO START OF FIELD
**           ELSE FILE INTERCHANGE LEVEL IS 2 OR 3
**              IF TAG IS NOT FILE CONTROL (0..0)
**              THEN
**                 IF FD_CNTRL INDICATES AN ELEMENTARY DATA STRUCTURE AND
**                  A NON-BIT FIELD
**                 THEN
**                    SET STATUS TO END OF FIELD
**                 ELSE
**                    SET STATUS TO OKAY
**                 ENDIF
**              ENDIF
**           ENDIF
**        3: { LABELS SUBFIELD } 
**           IF TAG IS FILE CONTROL (0..0) 
**           THEN 
**              WHILE TAG_L NOT NULL DO
**                 SET TMP_PTRL TO TAG_L 
**                 SET TAG_L TO TAG_L NEXT 
**                 FREE TMP_PTRL 
**              ENDWHILE
**              SET STATUS TO END OF FIELD
**           ELSE 
**              IF ARRAY DESCRIPTOR PRESENT
**              THEN
**                 CALL DEL123DIM() TO DELETE ARRAY DESCRIPTOR IN DIMENSION
**                  LENGTHS STRUCTURE
**                 RESET NUMBER OF DIMENSIONS AND PRIMARY DIMENSION TO ZERO
**              ELSE
**                 CALL DEL123LABS()  TO DEALLOCATE LABELS POINTER STRUCTURE
**              ENDIF
**              SET STATUS TO OKAY
**           ENDIF 
**        4: { FORMATS SUBFIELD } 
**           CALL DEL123FMT() TO DELETE FORMAT CONTROLS STRUCTURE
**           SET STATUS TO END OF FIELD
**     ENDCASE 
**
**     IF STATUS IS START OF FIELD
**     THEN
**        IF DD_HD IS NULL
**        THEN
**           SET STATUS TO START OF RECORD
**        ELSE IF DD_HD NEXT IS NULL
**        THEN
**           SET STATUS TO START OF RECORD
**        ENDIF
**     ENDIF
**
**     RETURN SUCCESS
**
******************************************************************************  
**     CODE SECTION 
******************************************************************************/ 
#include "stc123.h"

int er123ddsfld(FILE *fp,int *status)
{
   /* INTERNAL VARIABLES */
   long int_tag;
   struct dd *tmp_ptrd;
   struct tl *tmp_ptrl;

   /* INITIALIZE STATUS TO FAILURE */
   *status = 0;
 
   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return(0);
      
   /* ENSURE FILE IN WRITE MODE */
   if (cur_fm->open_mode != 'w') return(0);
   
   /* ENSURE NOT BACKED UP TO TOP OF STRUCTURE */
   if (cur_fm->cur_dd == cur_fm->dd_hd) return(0);
   
   /* CONVERT TAG IN CUR_DD TO INTEGER */
   if (is123intgr(cur_fm->cur_dd->tag)) {
      int_tag = atol(cur_fm->cur_dd->tag);
   }
   else {
      int_tag = -1;
   }
   
   /* IF FILE INTERCHANGE LEVEL IS 1 */
   if (cur_fm->dl_hd->ilevel == 1) {

      /* SET SF_STATE_DD TO NAME SUBFIELD */
      cur_fm->sf_state_dd = 2;
   }
   else {

      /* IF TAG IS FILE CONTROL (0..0) */
      if (int_tag == 0) {

         /* IF SF_STATE_DD IS FINISHED */
         if (cur_fm->sf_state_dd == 5) {

            /* IF FILE INTERCHANGE LEVEL IS 2 */
            if (cur_fm->dl_hd->ilevel == 2) {

               /* SET SF_STATE_DD TO NAME SUBFIELD */
               cur_fm->sf_state_dd = 2;
            }
            else {

               /* SET SF_STATE_DD TO LABELS SUBFIELD */
               cur_fm->sf_state_dd = 3;
            }
         }
         else {

            /* DECREMENT SF_STATE_DD */
            cur_fm->sf_state_dd--;
         }
      }
      else {

         /* IF FD_CNTRL INDICATES ELEMENTARY DATA STRUCTURE */
         if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '0'){

            /* CASE SF_STATE_DD */
            switch (cur_fm->sf_state_dd) {

               /* 2: { NAME SUBFIELD } */
               case 2:

                  /* DECREMENT SF_STATE_DD */
                  cur_fm->sf_state_dd--;

               break;

               /* 4: { FORMATS SUBFIELD } */
               case 4:

                  /* SET SF_STATE_DD TO NAME SUBFIELD */
                  cur_fm->sf_state_dd = 2;

               break;

               /* 5: { FINISHED } */
               case 5:

                  /* IF FD_CNTRL INDICATES A NON-BIT FIELD */
                  if (cur_fm->cur_dd->fd_cntrl[FCDTYPE] != '5'){

                     /* SET SF_STATE_DD TO NAME SUBFIELD */
                     cur_fm->sf_state_dd = 2;
                  }
                  else {

                     /* DECREMENT SF_STATE_DD */
                     cur_fm->sf_state_dd--;
                  }

               break;
            }
         }
         else {

            /* DECREMENT SF_STATE_DD */
            cur_fm->sf_state_dd--;
         }
      }
   }

   /* IF TAG IS USERS AUGMENTED FILE DESCRIPTION (0..2) */
   if (int_tag == 2) {
 
      /* FREE U_AFD */
      free(cur_fm->cr_hd->u_afd);

      /* SET U_AFD TO NULL */
      cur_fm->cr_hd->u_afd = NULL;

      /* SET TMP_PTRD TO DD_HD */
      tmp_ptrd = cur_fm->dd_hd;

      /* IF CUR_DD NEXT IS DD_HD */
      if (cur_fm->cur_dd->next == cur_fm->dd_hd) {

         /* { ERASING DD ENTRIES FROM TOP OF DD LIST } */

         /* SET CUR_DD NEXT TO NULL */
         cur_fm->cur_dd->next = NULL;
      }
      else {

         /* {ERASING DD ENTRIES FROM BOTTOM OF DD LIST } */

         /* WHILE TMP_PTRD NEXT NOT CUR_DD DO */
         while (tmp_ptrd->next != cur_fm->cur_dd) {

            /* SET TMP_PTRD TO TMP_PTRD NEXT */
            tmp_ptrd = tmp_ptrd->next;
         }

         /* SET TMP_PTRD NEXT TO CUR_DD NEXT */
         tmp_ptrd->next = cur_fm->cur_dd->next;
      }

      /* FREE CUR_DD */
      free(cur_fm->cur_dd);

      /* SET CUR_DD TO TMP_PTRD */
      cur_fm->cur_dd = tmp_ptrd;

      /* SET SF_STATE_DD TO FINISHED */
      cur_fm->sf_state_dd = 5;
      
      /* SET STATUS TO START OF FIELD */
      *status = 6;
   } 

   /* CASE SF_STATE_DD */
   switch (cur_fm->sf_state_dd) {

      /* 1: { FIELD CONTROL SUBFIELD } */
      case 1:

         /* SET TMP_PTRD TO DD_HD */
         tmp_ptrd = cur_fm->dd_hd;

         /* IF CUR_DD NEXT IS DD_HD */
         if (cur_fm->cur_dd->next == cur_fm->dd_hd) {

            /* { ERASING DD ENTRIES FROM TOP OF DD LIST } */

            /* SET CUR_DD NEXT TO NULL */
            cur_fm->cur_dd->next = NULL;
         }
         else {

            /* {ERASING DD ENTRIES FROM BOTTOM OF DD LIST } */

            /* WHILE TMP_PTRD NEXT NOT CUR_DD DO */
            while (tmp_ptrd->next != cur_fm->cur_dd) {

               /* SET TMP_PTRD TO TMP_PTRD NEXT */
               tmp_ptrd = tmp_ptrd->next;
            }

            /* SET TMP_PTRD NEXT TO CUR_DD NEXT */
            tmp_ptrd->next = cur_fm->cur_dd->next;
         }

         /* FREE CUR_DD */
         free(cur_fm->cur_dd);

         /* SET CUR_DD TO TMP_PTRD */
         cur_fm->cur_dd = tmp_ptrd;

         /* SET SF_STATE_DD TO FINISHED */
         cur_fm->sf_state_dd = 5;

         /* SET STATUS TO START OF FIELD */
         *status = 6;

      break;   

      /* 2: { NAME SUBFIELD } */
      case 2:

         /* IF TAG IS FILE CONTROL (0..0) */
         if (int_tag == 0) {

            /* FREE F_TITLE */
            free(cur_fm->cr_hd->f_title);
            
            /* SET F_TITLE TO NULL */
            cur_fm->cr_hd->f_title = NULL;

            /* IF FILE INTERCHANGE LEVEL IS 2 */
            if (cur_fm->dl_hd->ilevel == 2)
            
               /* SET STATUS TO END OF FIELD */
               *status = 5;

            else   

               /* SET STATUS TO OKAY */
               *status = 1;
         }
         else { 

            /* IF NAME EXISTS */
            if (cur_fm->cur_dd->name != NULL){
            
               /* FREE NAME */
               free(cur_fm->cur_dd->name);
         
               /* SET NAME TO NULL  */
               cur_fm->cur_dd->name = NULL;
            }
         }
         
         /* IF FILE INTERCHANGE LEVEL IS 1 */
         if (cur_fm->dl_hd->ilevel == 1) {

            /* SET TMP_PTRD TO DD_HD */
            tmp_ptrd = cur_fm->dd_hd;

            /* IF CUR_DD NEXT IS DD_HD */
            if (cur_fm->cur_dd->next == cur_fm->dd_hd) {

               /* { ERASING DD ENTRIES FROM TOP OF DD LIST } */

               /* SET CUR_DD NEXT TO NULL */
               cur_fm->cur_dd->next = NULL;
            }
            else {

               /* {ERASING DD ENTRIES FROM BOTTOM OF DD LIST } */

               /* WHILE TMP_PTRD NEXT NOT CUR_DD DO */
               while (tmp_ptrd->next != cur_fm->cur_dd) {

                  /* SET TMP_PTRD TO TMP_PTRD NEXT */
                  tmp_ptrd = tmp_ptrd->next;
               }

               /* SET TMP_PTRD NEXT TO CUR_DD NEXT */
               tmp_ptrd->next = cur_fm->cur_dd->next;
            }

            /* FREE CUR_DD */
            free(cur_fm->cur_dd);

            /* SET CUR_DD TO TMP_PTRD */
            cur_fm->cur_dd = tmp_ptrd;

            /* SET SF_STATE_DD TO FINISHED */
            cur_fm->sf_state_dd = 5;
         
            /* SET STATUS TO START OF FIELD */
            *status = 6;
         }
         
         /* ELSE FILE INTERCHANGE LEVEL IS 2 OR 3 */
         else {

            /* IF TAG IS NOT FILE CONTROL (0..0) */
            if (int_tag != 0) {
            
               /* IF FD_CNTRL INDICATES AN ELEMENTARY DATA STRUCTURE AND
                   A NON-BIT FIELD
               */
               if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '0'
                && cur_fm->cur_dd->fd_cntrl[FCDTYPE] != '5') {
               
                  /* SET STATUS TO END OF FIELD */
                  *status = 5;
               }
               else {
               
                  /* SET STATUS TO OKAY */
                  *status = 1;
               }
            }
         }

      break;
 
      /* 3: { LABELS SUBFIELD } */
      case 3:

         /* IF TAG IS FILE CONTROL (0..0) */
         if (int_tag == 0) {

            /* WHILE TAG_L NOT NULL DO */
            while (cur_fm->cr_hd->tag_l != NULL) {

               /* SET TMP_PTRL TO TAG_L */
               tmp_ptrl = cur_fm->cr_hd->tag_l; 

               /* SET TAG_L TO TAG_L NEXT */
               cur_fm->cr_hd->tag_l = cur_fm->cr_hd->tag_l->next;

               /* FREE TMP_PTRL */
               free(tmp_ptrl);
            } 

            /* SET STATUS TO END OF FIELD */
            *status = 5;
         }
         else { 

            /* IF ARRAY DESCRIPTOR PRESENT */
            if (cur_fm->cur_dd->dim_lptr != NULL) {

               /* CALL DEL123DIM() TO DELETE ARRAY DESCRIPTOR IN DIMENSION
                   LENGTHS STRUCTURE
               */
               cur_fm->cur_dd->dim_lptr = del123dim(cur_fm->cur_dd->dim_lptr);

               /* RESET NUMBER OF DIMENSIONS AND PRIMARY DIMENSION TO ZERO */
               cur_fm->cur_dd->num_dim = 0L;
               cur_fm->cur_dd->prim_dms = 0L;
            }
            else

               /* CALL DEL123LABS()  TO DEALLOCATE LABELS POINTER STRUCTURE */
               cur_fm->cur_dd->labels = del123labs(cur_fm->cur_dd->labels);

            /* SET STATUS TO OKAY */
            *status = 1;
         }

      break;

      /* 4: { FORMATS SUBFIELD } */
      case 4:

         /* CALL DEL123FMT() TO DELETE FORMAT CONTROLS STRUCTURE */
         cur_fm->cur_dd->fmt_rt = del123fmt(cur_fm->cur_dd->fmt_rt);

         /* SET STATUS TO END OF FIELD */
         *status = 5;
      
      break;
   }
 
   /* IF STATUS IS START OF FIELD */
   if (*status == 6) {

      /* IF DD_HD IS NULL */
      if (cur_fm->dd_hd == NULL)

         /* SET STATUS TO START OF RECORD */
         *status = 2;

      /* ELSE IF DD_HD NEXT IS NULL */ 
      else if (cur_fm->dd_hd->next == NULL)

         /* SET STATUS TO START OF RECORD */
         *status = 2;
   }   

   /* RETURN SUCCESS */
   return(1);
}
