# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RET123PDM
**
**    PURPOSE: TO CALCULATE THE SIZE OF PRIMARY DIMENSION OF ARRAY DATA
**              WITH LABELS
**
**    INVOCATION METHOD: RET123PDM(PDM) 
**
**    ARGUMENT LIST:
**     NAME        TYPE      USE      DESCRIPTION
**     PDM         INT        O       SIZE OF PRIMARY DIMENSION
**     RET123PDM() LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME            DESCRIPTION
**     RET123MATCH()   RETRIEVES DATA DESCRIPTION ENTRY
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     NUM_COL    INT                NUMBER OF COLUMNS
**     NUM_ROW    INT                NUMBER OF ROWS
**  
**    GLOBAL REFERENCES:
**      
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA 
**                                    FIELD RECORD
**      FD_CNTRL[10]    CHAR    I    FIELD CONTROLS
**      NAME[]          PTR    N/A   CHARACTER STRING POINTER TO NAME
**      LABELS          PTR     I    HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
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
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
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
**      CUR_DM          PTR    N/A   CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY
**      CUR_DR          PTR     I    CURRENT POINTER TO DATA RECORD STRUCTURE
**                                    ENTRY
**      CUR_DV          PTR    N/A   CURRENT POINTER TO DR DATA VALUE SUBFIELD
**                                    STRUCTURE ENTRY
**      ROW_DVH         PTR    N/A   CURRENT POINTER TO NEXT SET OF VALUES
**                                    IN DR DATA VALUE SUBFIELD STRUCTURE ENTRY
**      CUR_FC          PTR    N/A   CURRENT POINTER TO FORMAT CONTROLS
**                                    STRUCTURE ENTRY
**      CUR_LP          PTR    N/A   CURRENT POINTER TO LABELS POINTER
**                                    STRUCTURE ENTRY
**      CUR_SL          PTR    I/O   CURRENT POINTER TO DD-LABEL SUBFIELD
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
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION
**     FCDSTYPE         INT          RELATIVE POSITION OF DATA STRUCTURE TYPE
**                                    WITHIN THE FIELD CONTROL
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     P. HODGES                 06/05/90  INITIAL CODE
**     L. MCMILLION  TASK #40    12/22/92  UPDATED PROLOG/PDL PER QC
**     L. MCMILLION  93DR027     04/02/93  UPDATED CONDITION TO RETURN THE 
**                                          PRIMARY DIMENSION FOR VECTOR
**                                          DATA STRUCTURES REGARDLESS OF THE
**                                          NUMBER OF COLUMNS
**                                          VALUE OF NUM
**
**    PDL:
**
**     SET NUM_COL TO ONE
**     SET NUM_ROW TO ONE
**     CALL RET123MATCH() TO RETRIEVE DD ENTRY CORRESPONDING TO CUR_DR
**     
**     SET CUR_SL TO LABELS POINTER IN CUR_DD
**     WHILE NXT_LAB OF C_LAB NOT NULL
**        INCREMENT NUM_COL
**        SET CUR_SL TO NXT_LAB FIELD POINTED TO CUR_SL
**     END WHILE
**     IF NUM_COL GREATER THAN ONE OR FIELD CONTROL INDICATES VECTOR DATA
**     THEN
**        SET OUTPUT PRIMARY DIMENSION TO NUM_COL
**        RETURN SUCCESS
**     ENDIF
**
**     SET CUR_SL TO NXT_LSET POINTER OF LABELS POINTER IN CUR_DD
**     IF CUR_SL IS NULL
**     THEN
**        RETURN FAILURE
**     ENDIF
**     WHILE NXT_LAB OF CUR_SL NOT NULL
**        INCREMENT NUM_ROW
**        SET CUR_SL TO NXT_LAB FIELD POINTED TO BY CUR_SL
**     END WHILE
**
**     SET OUTPUT PRIMARY DIMENSION TO NUM_ROW
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int ret123pdm(long *pdm)

{
   /* INTERNAL VARIABLES */
   long num_col = 1;
   long num_row = 1;

   /* CALL RET_MATCH() TO RETRIEVE DD ENTRY CORRESPONDING TO CUR_DR */ 
   if (!ret123match(cur_fm->cur_dr->tag)) return(0); 

   /* SET CUR_SL TO LABELS POINTER IN CUR_DD */
   cur_fm->cur_sl = cur_fm->cur_dd->labels->nxt_lset;

   /* WHILE NXT_LAB OF C_LAB NOT NULL */
   while(cur_fm->cur_sl->nxt_lab != NULL) {

      /* INCREMENT NUM_COL */
      num_col++;

      /* SET CUR_SL TO NXT_LAB FIELD POINTED TO CUR_SL */
      cur_fm->cur_sl = cur_fm->cur_sl->nxt_lab;
   }

   /* IF NUM_COL GREATER THAN ONE OR FIELD CONTROL INDICATES VECTOR DATA */
   if (num_col > 1 || cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '1') {

      /* SET OUTPUT PRIMARY DIMENSION TO NUM_COL AND RETURN SUCCESS */
      *pdm = num_col;
      return(1);
   }

   /* SET CUR_SL TO NXT_LSET POINTER OF LABELS POINTER IN CUR_DD */
   cur_fm->cur_sl = cur_fm->cur_dd->labels->nxt_lset->nxt_lset;

   /* IF CUR_SL IS NULL, RETURN FAILURE */
   if (cur_fm->cur_sl == NULL) return(0);

   /* WHILE NXT_LAB OF CUR_SL NOT NULL */
   while(cur_fm->cur_sl->nxt_lab != NULL) {

      /* INCREMENT NUM_ROW */
      num_row++;

      /* SET CUR_SL TO NXT_LAB FIELD POINTED TO BY CUR_SL */
      cur_fm->cur_sl = cur_fm->cur_sl->nxt_lab;
   }

   /* SET OUTPUT PRIMARY DIMENSION TO NUM_ROW */
   *pdm = num_row;

   /* RETURN SUCCESS */
   return(1);
}
