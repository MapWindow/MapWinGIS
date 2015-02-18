# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: SETUP123LB
**
**    PURPOSE: TO INITIALIZE LABELS POINTER STRUCTURE
**
**    INVOCATION METHOD: SETUP123LB()
**
**    ARGUMENT LIST:
**     NAME          TYPE      USE    DESCRIPTION
**     SETUP123LB()  LOGICAL    O     SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME       TYPE         DESCRIPTION
**     NEW_LP     PTR          NEW LABELS POINTER STRUCTURE ENTRY
**     TMP_SL     PTR          TEMPORARY DD SUBFIELD LABEL STRUCTURE PTR
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
**      LABELS          PTR     I    HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR    N/A   FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                    STRUCTURE
**      DR_HD           PTR    N/A   HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR     O    HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE
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
**      CUR_LP          PTR    I/O   CURRENT POINTER TO LABELS POINTER
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
**     LABELS POINTER STRUCTURE (LP)
**      NAME            TYPE   USE   DESCRIPTION
**      NEXT            PTR     O    POINTER TO NEXT LP ENTRY
**      FIRST           PTR     O    FIRST LABEL (SL) IN THE LABEL SET
**      CUR             PTR     O    CURRENT LABEL (SL) IN THE LABEL SET
**
**     DD LABEL SUBFIELD STRUCTURE (SL)
**      NAME            TYPE   USE   DESCRIPTION
**      LABEL[]         PTR    N/A   LABEL
**      NXT_LAB         PTR    N/A   POINTER TO NEXT LABEL IN SET
**      NXT_LSET        PTR     I    POINTER TO NEXT SET OF LABELS
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/10/90  INITIAL PROLOG
**     P. HODGES                 05/10/90  INITIAL PDL
**     P. HODGES                 06/04/90  INITIAL CODE
**     L. MCMILLION  (TASK #40)  08/30/91  INTRODUCED TEMP SL POINTER TO
**                                          TRAVERSE SL STRUCTURE LIST AND
**                                          RETAIN CUR_SL POINTER VALUE
**     L. MCMILLION  TASK #40    12/09/92  PROLOG UPDATE
**
**    PDL:
**
**     { ALLOCATE DUMMY LP }
**     ALLOCATE NEW_LP
**     SET LP_HD TO NEW_LP
**     SET CUR_LP TO NEW_LP
**     SET NEW_LP POINTERS TO NULL
**     SET TMP_SL TO CUR_DD LABELS
**
**     IF TMP_SL NOT NULL
**     THEN
**        SET TMP_SL TO NEXT FIELD
**        WHILE TMP_SL NOT NULL DO
**           ALLOCATE NEW_LP
**           { INSERT NEW_LP INFORMATION INTO LIST }
**           SET CUR_LP NEXT TO NEW_LP
**           SET CUR_LP TO NEW_LP
**           SET NEW_LP NEXT TO NULL
**           SET NEW_LP FIRST TO TMP_SL
**           SET NEW_LP CUR TO TMP_SL
**           INCREMENT TMP_SL TO NXT L_SET
**        ENDWHILE
**     ENDIF
**     RESET CURRENT LABEL POINTER
**
**     RETURN SUCCESS
**
*****************************************************************************
**    CODE SECTION
**
*****************************************************************************/
#include "stc123.h"

int setup123lb()

{
   /* INTERNAL VARIABLES */
   struct lp *new_lp;
   struct sl *tmp_sl;

   /* ALLOCATE NEW_LP */
   if ((new_lp = (struct lp *) malloc(sizeof(struct lp))) == NULL) return(0);

   /* SET LP_HD TO NEW_LP */
   cur_fm->lp_hd = new_lp;

   /* SET CUR_LP TO NEW_LP */
   cur_fm->cur_lp = new_lp;

   /* SET NEW_LP POINTERS TO NULL */
   new_lp->next = NULL;
   new_lp->first = NULL;
   new_lp->cur = NULL;

   /* SET TMP_SL TO CUR_DD LABELS */
   tmp_sl = cur_fm->cur_dd->labels;

   /* IF TMP_SL NOT NULL */
   if (tmp_sl != NULL) {
   
      /* SET TMP_SL TO NEXT FIELD */
      tmp_sl = tmp_sl->nxt_lset;
      
      /* WHILE TMP_SL NOT NULL DO */
      while(tmp_sl != NULL) {

         /* ALLOCATE NEW_LP */
         if ((new_lp = (struct lp *) malloc(sizeof(struct lp))) == NULL) return (0);
      
         /* SET CUR_LP NEXT TO NEW_LP */
         cur_fm->cur_lp->next = new_lp;

         /* SET CUR_LP TO NEW_LP */
         cur_fm->cur_lp = new_lp;

         /* SET NEW_LP NEXT TO NULL */
         new_lp->next = NULL;
  
         /* SET NEW_LP FIRST TO TMP_SL */
         new_lp->first = tmp_sl;

         /* SET NEW_LP CUR TO TMP_SL */
         new_lp->cur = tmp_sl;

         /* INCREMENT TMP_SL TO NXT L_SET */
         tmp_sl = tmp_sl->nxt_lset;
      }
   }
   
   /* RESET CURRENT LABEL POINTER */
   cur_fm->cur_lp = cur_fm->lp_hd;
   
   /* RETURN SUCCESS */
   return(1);
}
