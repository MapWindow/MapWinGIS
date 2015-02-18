# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: CMP123DRLEAD
** 
**    PURPOSE: TO COMPUTE INFORMATION NEEDED IN DATA RECORD LEADER 
** 
**    INVOCATION METHOD: CMP123DRLEAD()
** 
**    ARGUMENT LIST:
**     NAME            TYPE               DESCRIPTION
**     CMP123DRLEAD()  LOGICAL            SUCCESS FLAG
**      
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     I123TOA()        CONVERTS AN INTEGER TO A CHARACTER STRING
** 
**    INTERNAL VARIABLES: 
**     NAME       TYPE               DESCRIPTION 
**     ANUM[20]   CHAR               INTEGER IN CHARACTER FORMAT
**     LEN        INT                LENGTH OF INTEGER IN CHARACTER FORMAT
**     NUM_ENT    INT                NUMBER OF ENTRIES IN DR STRUCTURE
**   
**    GLOBAL REFERENCES: 
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT    N/A   DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT    N/A   INTERCHANGE LEVEL
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
**      S_TAG           INT     I    SIZE OF FIELD TAG 
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     I    LENGTH OF DISCRIPTIVE AREA DATA RECORD  
**      FD_POS          INT     I    POSITION OF DESCRIPTIVE AREA DATA 
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
**      FP              PTR    N/A   FILE POINTER 
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE 
**                                    CONTROL RECORD STRUCTURE 
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE 
**      DL_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE LEADER 
**                                    STRUCTURE 
**      DR_HD           PTR     I    HEAD POINTER TO DATA RECORD STRUCTURE 
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE 
**      RL_HD           PTR    I/O   HEAD POINTER TO DATA RECORD LEADER 
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
**      NLD             INT     O    NO LEADER FLAG 
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
**     DATA RECORD LEADER STRUCTURE (RL) 
**      NAME            TYPE   USE   DESCRIPTION 
**      REC_LEN         INT     O    DATA RECORD LENGTH      
**      RESV_SP         CHAR   N/A   RESERVED SPACE CHARACTER 
**      LEAD_ID         CHAR    O    LEADER IDENTIFIER 
**      S_RESV[6]       CHAR   N/A   RESERVED SPACE
**      DA_BASE         INT     O    BASE ADDRESS OF DATA AREA   
**      R_SPACE[4]      CHAR   N/A   RESERVED SPACE CHARACTERS
**      S_FDLEN         INT     O    SIZE OF FIELD LENGTH   
**      S_FDPOS         INT     O    SIZE OF FIELD POSITION  
**      SP_RSRV         INT    N/A   RESERVED DIGIT 
**      S_TAG           INT     O    SIZE OF FIELD TAG  
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
**     P. HODGES                 05/18/90  INITIAL PROLOG 
**     P. HODGES                 05/18/90  INITIAL PDL 
**     P. HODGES                 06/08/90  INITIAL CODE 
**     A. DEWITT                 10/11/90  MODIFY CODE TO CALCULATE DR LENGTH
**                                          USING DA_BASE ALSO
**     A. DEWITT                 11/03/90  MODIFY NLD LOGIC TO SET FLAG IF
**                                          LEADER_ID IS 'R'
**     L. MCMILLION  (TASK #40)  08/28/91  ADDED LOGIC TO CHECK THE VALUES FOR
**                                          SIZE OF FIELD LENGTH, SIZE OF FIELD
**                                          POSITION, AND SIZE OF TAG
**     L. MCMILLION  TASK #40    11/20/92  UPDATED PROLOG PER QC
** 
**    PDL: 
** 
**     IF LEAD_ID EQUAL TO 'R' 
**        SET NLD FLAG
**     ENDIF 
**     SET S_FDLEN TO 1 
**     SET S_TAG TO 1 
** 
**     SET NUM_ENT TO ZERO 
**     SET CUR_DR TO DR_HD  
**     WHILE CUR_DR NOT NULL DO
**        SET CUR_DR TO CUR_DR NEXT 
**        CONVERT INTEGER FD_LEN TO ASCII
**        SET LEN TO LENGTH OF ANUM STRING
**        IF NUMBER OF CHARACTERS IN FD_LEN EXCEEDS S_FDLEN 
**        THEN 
**           SET S_FDLEN TO NUMBER OF CHARACTERS IN FD_LEN 
**        ENDIF 
**        IF NUMBER OF CHARACTERS IN TAG EXCEED S_TAG 
**        THEN 
**           SET S_TAG TO NUMBER OF CHARACTERS IN TAG 
**        ENDIF 
**        INCREMENT NUM_ENT 
**     ENDWHILE
** 
**     SET S_FDPOS TO NUMBER OF CHARACTERS IN FD_POS { LAST DR FIELD ENTRY } 
**     IF SIZE OF FIELD POSITION IS NOT A VALID SIZE
**     THEN
**        RETURN FAILURE
**     ENDIF
**     IF SIZE OF FIELD LENGTH IS NOT A VALID SIZE
**     THEN
**        RETURN FAILURE
**     ENDIF
**     IF TAG SIZE IS NOT A VALID SIZE
**     THEN
**        RETURN FAILURE
**     ENDIF
**
**     SET DA_BASE TO 24 + NUM_ENT * (S_TAG + S_FDLEN + S_FDPOS) + 1 
**     SET REC_LEN TO FD_LEN + FD_POS + DABASE
**      
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int cmp123drlead()

{
   /* INTERNAL VARIABLES */
   char anum[20];
   long len;
   long num_ent;

   /* IF LEAD_ID EQUAL TO 'R' */
   if (cur_fm->rl_hd->lead_id == 'R') {
      
      /* SET NLD FLAG */
      cur_fm->nld = 1;
      
   }; 

   /* SET S_FDLEN TO 1 */
   cur_fm->rl_hd->s_fdlen = 1;

   /* SET S_TAG TO 1 */
   cur_fm->rl_hd->s_tag = 1;

   /* SET NUM_ENT TO ZERO */
   num_ent = 0;

   /* SET CUR_DR TO DR_HD  */
   cur_fm->cur_dr = cur_fm->dr_hd;

   /* WHILE CUR_DR NEXT NOT NULL DO */
   while (cur_fm->cur_dr->next != NULL) {

      /* SET CUR_DR TO CUR_DR NEXT */ 
      cur_fm->cur_dr = cur_fm->cur_dr->next;

      /* CONVERT INTEGER FD_LEN TO ASCII */
      i123toa(cur_fm->cur_dr->fd_len,anum);
      
      /* SET LEN TO NUMBER OF CHARACTERS IN ANUM */
      len = _tcslen(anum);

      /* IF NUMBER OF CHARACTERS IN FD_LEN EXCEEDS S_FDLEN */
      if (len > cur_fm->rl_hd->s_fdlen) {
  
         /* SET S_FDLEN TO NUMBER OF CHARACTERS IN FD_LEN */
         cur_fm->rl_hd->s_fdlen = len;
      } 
   
      /* IF NUMBER OF CHARACTERS IN TAG EXCEED S_TAG */
      len = _tcslen (cur_fm->cur_dr->tag);
      if (len > cur_fm->rl_hd->s_tag) {

         /* SET S_TAG TO NUMBER OF CHARACTERS IN TAG */
         cur_fm->rl_hd->s_tag = len;
      } 

      /* INCREMENT NUM_ENT */
      num_ent++;
   } 

   /* SET S_FDPOS TO NUMBER OF CHARACTERS IN 
       FD_POS { LAST DR FIELD ENTRY }
   */
   i123toa(cur_fm->cur_dr->fd_pos,anum);
   len = _tcslen(anum);
   cur_fm->rl_hd->s_fdpos = len;

   /* IF SIZE OF FIELD POSITION IS NOT VALID SIZE, RETURN FAILURE */
   if (cur_fm->rl_hd->s_fdpos < 1 || cur_fm->rl_hd->s_fdpos > 9) return(0);
  
   /* IF SIZE OF FIELD LENGTH IS NOT VALID SIZE, RETURN FAILURE */
   if (cur_fm->rl_hd->s_fdlen < 1 || cur_fm->rl_hd->s_fdlen > 9) return(0); 

   /* IF SIZE OF TAG IS NOT VALID SIZE, RETURN FAILURE */
   if (cur_fm->rl_hd->s_tag < 1 || cur_fm->rl_hd->s_tag > 7 ||
    cur_fm->rl_hd->s_tag != cur_fm->dl_hd->s_tag) return(0);

   /* SET DA_BASE TO 24 + NUM_ENT * (S_TAG + S_FDLEN + S_FDPOS) + 1 */ 
   cur_fm->rl_hd->da_base = 24 + num_ent * 
    (cur_fm->rl_hd->s_tag + cur_fm->rl_hd->s_fdlen + cur_fm->rl_hd->s_fdpos) + 1;

   /* SET REC_LEN TO FD_LEN + FD_POS + DA_BASE */
   cur_fm->rl_hd->rec_len = cur_fm->cur_dr->fd_len + cur_fm->cur_dr->fd_pos + 
    cur_fm->rl_hd->da_base;

   /* RETURN SUCCESS */
   return (1);
}
