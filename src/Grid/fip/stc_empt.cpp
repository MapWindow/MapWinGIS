# include "stdafx.h"
/***************************************************************************
** 
**    INVOCATION NAME: STC123EMPTY 
** 
**    PURPOSE: TO DETERMINE IF A DATA RECORD STRUCTURE IS EMPTY
** 
**    INVOCATION METHOD: STC123EMPTY()
** 
**    ARGUMENT LIST: NONE
**     NAME           TYPE      USE      DESCRIPTION
**     STC123EMPTY()  LOGICAL    O       STRUCTURE EMPTY FLAG
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES: 
**     NAME         TYPE         DESCRIPTION 
**     S_EMPTY      LOGICAL      STRUCTURE EMPTY FLAG    
**     CUR_DR       PTR          CURRENT POINTER TO DATA RECORD STRUCTURE
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
**      VALUES          PTR     I    HEAD POINTER TO DATA VALUE SUBFIELD 
**                                    RECORDS 
**      NEXT            PTR     I    POINTER TO NEXT DATA RECORD  
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
**    GLOBAL CONSTANTS: NONE
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     A. DEWITT                 01/16/91  INITIAL PROLOG 
**     A. DEWITT                 01/16/91  INITIAL PDL 
**     A. DEWITT                 01/16/91  INITIAL CODE
**     L. MCMILLION  TASK #40    12/23/92  UPDATED PROLOG PER QC
** 
**    PDL: 
** 
**     SET S_EMPTY TO TRUE
**     SET LOCAL CUR_DR TO DR_HD
**     WHILE S_EMPTY AND LOCAL CUR_DR NOT NULL DO
**        IF VALUES FIELD IS NULL
**        THEN
**           SET S_EMPTY TO TRUE
**        ELSE
**           SET S_EMPTY TO FALSE
**        ENDIF
**        SET LOCAL CUR_DR TO NEXT FIELD
**     ENDWHILE
**
**     RETURN STRUCTURE EMPTY FLAG   
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int stc123empty()

{
   /* INTERNAL VARIABLES */   
   int       s_empty;
   struct dr *cur_dr;

   /* SET S_EMPTY TO TRUE */
   s_empty = 1;
      
   /* SET LOCAL CUR_DR TO DR_HD */
   cur_dr = cur_fm->dr_hd;
      
   /* WHILE S_EMPTY AND LOCAL CUR_DR NOT NULL DO */
   while (s_empty && cur_dr != NULL) {
         
      /* ASSIGN S_EMPTY TRUE IF VALUES FIELD IS NULL FALSE OTHERWISE */
      s_empty = (cur_dr->values == NULL);
         
      /* SET LOCAL CUR_DR TO NEXT FIELD */
      cur_dr = cur_dr->next;
   };

   /* RETURN STRUCTURE EMPTY FLAG */   
   return(s_empty);
}      
