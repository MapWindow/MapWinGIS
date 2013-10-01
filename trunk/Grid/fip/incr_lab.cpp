# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: INCRE123LAB
**
**    PURPOSE: TO INCREMENT THE POINTER TO THE NEXT LABEL VIA RECURSION
**
**    INVOCATION METHOD: INCRE123LAB(NEXT,FIRST,PTR)
**
**    ARGUMENT LIST:
**     NAME           TYPE      USE      DESCRIPTION
**     NEXT           PTR        I       NEXT ENTRY IN LP STRUCTURE
**     FIRST          PTR        I       POINTER TO ENTRY IN SL STRUCTURE THAT
**                                        IS FIRST LABEL IN ITS LSET
**     PTR            PTR       I/O      POINTER TO CURRENT LABEL IN THE LSET OF
**                                        THE SL STRUCTURE
**     INCRE123LAB()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     INCRE123LAB()    RECURSIVELY INCREMENTS THE POINTER TO THE NEXT LABEL
**
**    INTERNAL VARIABLES: NONE
**  
**    GLOBAL REFERENCES:
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
**      LP_HD           PTR     I    HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR    N/A   CURRENT POINTER TO DATA DESCRIPTIVE
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
**      NEXT            PTR    I/O   POINTER TO NEXT LP ENTRY
**      FIRST           PTR    I/O   FIRST LABEL (SL) IN THE LABEL SET
**      CUR             PTR    I/O   CURRENT LABEL (SL) IN THE LABEL SET
**
**     DD LABEL SUBFIELD STRUCTURE (SL)
**      NAME            TYPE   USE   DESCRIPTION
**      LABEL[]         PTR    N/A   LABEL
**      NXT_LAB         PTR     I    POINTER TO NEXT LABEL IN SET
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
**     P. HODGES                 05/18/90  INITIAL PROLOG
**     P. HODGES                 05/18/90  INITIAL PDL
**     P. HODGES                 06/05/90  INITIAL CODE
**     L. MCMILLION              11/21/90  DECLARED ARGUMENT PTR TO BE A
**                                          POINTER TO AN SL STRUCTURE POINTER
**     L. MCMILLION  TASK #40    12/22/92  UPDATED PROLOG PER QC
**
**    PDL:
**
**     SET PTR TO PTR'S NXT_LAB
**     IF PTR IS NULL
**     THEN
**        SET PTR TO FIRST
**        IF INPUT NEXT NOT NULL
**        THEN
**           CALL INCRE123LAB() FOR NEXT LP ENTRY
**        ELSE
**           {SET ALL CUR TO CORRESPONDING FIRST}
**           SET CUR_LP TO LP_HD
**           WHILE CUR_LP IS NOT NULL DO
**              SET CUR_LP CUR TO FIRST
**              SET CUR_LP TO CUR_LP NEXT
**           ENDWHILE
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

int incre123lab(struct lp *next,struct sl *first,struct sl **ptr)
{
   /* SET PTR TO PTR'S NXT_LAB */
   *ptr = (*ptr)->nxt_lab;

   /* IF PTR IS NULL */
   if (*ptr == NULL) {

      /* SET PTR TO FIRST */
      *ptr = first;

      /* IF INPUT NEXT NOT NULL */
      if (next != NULL) {

         /* CALL INCRE123LAB() FOR NEXT LP ENTRY */
         if (!incre123lab(next->next,next->first,&next->cur)) return(0);
      }
      else {
         
         /* SET CUR_LP TO LP_HD */
         cur_fm->cur_lp = cur_fm->lp_hd;

         /* WHILE CUR_LP IS NOT NULL DO */
         while(cur_fm->cur_lp != NULL) {

            /* SET CUR_LP CUR TO FIRST */
            cur_fm->cur_lp->cur = cur_fm->cur_lp->first;

            /* SET CUR_LP TO CUR_LP NEXT */
            cur_fm->cur_lp = cur_fm->cur_lp->next;
         }
      }
   }

   /* RETURN SUCCESS */
   return(1);
}
