# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: END123FILE
**
**    PURPOSE: TO CLOSE A FILE  
**
**    INVOCATION METHOD: END123FILE(FP)
**
**    ARGUMENT LIST:
**     NAME           TYPE      USE      DESCRIPTION
**     FP             PTR        I       POINTER TO FILE POINTER
**     END123FILE()   LOGICAL    O       SUCCESS FLAG
**      
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     CH123SIZE()      CHANGES SIZE OF FILE; TRUNCATES FILE IF NECESSARY
**     END123REC()      COMPLETES THE LAST DATA RECORD AND FREE ITS SPACE
**     ER123DDREC()     ERASES THE LAST DATA DESCRIPTIVE RECORD WRITTEN
**     FREE123LAB()     FREES THE LABEL POINTER STRUCTURES
**     GET123LEVEL()    RETRIEVES THE APPROPRIATE DATA STRUCTURE LAYER
**     POP123RS()       POPS A REPETITION NUMBER FROM THE REPETITION STACK
**
**    INTERNAL VARIABLES:
**     NAME            TYPE         DESCRIPTION
**     CUR_POS         INT          CURRENT POSITION OF INTERNAL FILE POINTER
**     MODE_MOD        INT          OPEN MODE MODIFICATION FLAG
**     STATUS          INT          DDR ERASURE STATUS
**                                   0 = FAILURE
**                                   1 = OKAY
**                                   2 = START OF RECORD
**                                   3 = END OF RECORD
**                                   4 = END OF FILE
**                                   5 = END OF FIELD
**                                   6 = START OF FIELD
**     TMP_LONG        LONG         TEMPORARY LONG INTEGER
**     TMP_RP          PTR          TEMPORARY POINTER TO RECORD POINTER
**                                   STRUCTURE
** 
**    GLOBAL REFERENCES:
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR    N/A   FILE POINTER
**      F_NAME[]        PTR     O    EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   I/O   OPEN MODE OF FILE
**      CR_HD           PTR    I/O   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR    I/O   HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                    STRUCTURE
**      DR_HD           PTR    N/A   HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR     I    HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    I/O   HEAD POINTER TO RECORD POINTER STRUCTURE 
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
**      CUR_RP          PTR     O    CURRENT POINTER TO RECORD POINTER
**                                    STRUCTURE
**      NLD_RP          PTR     O    POINTER TO RECORD POINTER STRUCTURE
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
**      NEXT            PTR    I/O   POINTER TO NEXT FILE MANAGEMENT
**                                    STRUCTURE
**      BACK            PTR    I/O   POINTER TO PREVIOUS FILE MANAGEMENT
**                                    STRUCTURE
**
**     RECORD POINTER STRUCTURE (RP)
**      NAME            TYPE   USE   DESCRIPTION
**      DR_START        INT    N/A   FILE LOCATION OF FIRST BYTE OF DATA RECORD
**      NEXT            PTR    I/O   POINTER TO NEXT DR_START
**      BACK            PTR     O    POINTER TO BACK DR_START
**
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR    I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**     FM_HD            PTR    I/O   HEAD POINTER TO LIST OF OPEN FILES
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/18/90  INITIAL PROLOG
**     P. HODGES                 05/18/90  INITIAL PDL
**     A. DEWITT                 05/25/90  INITIAL CODE
**     L. MCMILLION              12/05/90  ADDED LOGIC TO TRUNCATE FILE'S
**                                          CONTENTS FOLLOWING INTERNAL FILE
**                                          POINTER FOR A WRITE OPEN MODE
**     L. MCMILLION              01/21/91  ADDED STATUS TO ARGUMENT LIST OF
**                                          FUNTION ER123DDREC()
**     A. DEWITT                 02/28/91  ADDED LOGIC TO DELETE RECORD 
**                                          START LOCATION LIST
**     A. DEWITT     (TASK #40)  08/23/91  CALL TO ROUTINE CHSIZE() REMOVED  
**                                          INSERT CALL TO ROUTINE CH123SIZE()
**                                          TO RESIZE THE WRITE FILE AND
**                                          TRUNCATE FILE IF NECESSARY
**     L. MCMILLION  (TASK #40)  10/11/91  MODIFIED FILE POINTER ARGUMENT IN
**                                          CH123SIZE() CALL
**     L. MCMILLION  (TASK #40)  03/17/92  ADDED CONDITIONAL COMPILATION
**                                          DIRECTIVES FOR ENVIRONMENT TYPE
**     L. MCMILLION  92DR005     06/18/92  REMOVED COMPILER DIRECTIVE SINCE
**                                          BUG FIXED IN CH123SIZE ORIGINALLY
**                                          ENCOUNTERED UNDER MS-DOS
**     L. MCMILLION  TASK #40    12/01/92  PROLOG UPDATE
**     J. TAYLOR     93DR033     10/26/93  ADDED FREE OF REMAINING RS,
**                                          CUR_FM->LP_HD, AND FM_HD->FNAME
**                                          STRUCTURES
**
**    PDL:
**
**     INITIALIZE DDR ERASURE STATUS TO FAILURE
**     CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER
**     CALL END123REC() TO COMPLETE THE LAST DATA RECORD
**     IF OPEN MODE IS WRITE
**     THEN
**        RETRIEVE CURRENT POSITION OF INTERNAL FILE POINTER
**        CALL CH123SIZE() TO TRUNCATE CONTENTS FOLLOWING CURRENT POSITION
**     ENDIF
**     IF OPEN MODE IS READ
**     THEN
**        SET OPEN MODE TO WRITE
**        SET MODE MODIFICATION FLAG TO TRUE
**     ELSE
**        SET MODE MODIFICATION FLAG TO FALSE
**     ENDIF
**     CALL ER123DDREC() TO ERASE DATA DESCRIPTIVE RECORD INFORMATION
**     IF MODE MODIFICATION FLAG IS TRUE
**     THEN
**        RESET OPEN MODE TO READ
**     ENDIF
**     CALL LIBRARY FUNCTION FCLOSE() TO CLOSE FILE
**     IF UNABLE TO CLOSE FILE
**     THEN
**        RETURN FAILURE
**     ENDIF
**     SET FILE POINTER TO NULL
**     { REMOVE FILE ENTRY FROM FM STRUCTURE }
**     SET CUR_FM BACK NEXT POINTER TO NEXT POINTER
**     IF NOT DELETING AT END OF LIST
**     THEN
**        SET CUR_FM NEXT BACK POINTER TO BACK POINTER
**     ENDIF
**     { DELETE RECORD LOCATION POINTER LIST }
**     SET CUR_RP TO NULL
**     SET NLD_RP TO NULL
**     WHILE REC_LOC_HD NOT NULL DO
**        SET TMP_RP TO REC_LOC_HD
**        SET REC_LOC_HD TO REC_LOC_HD NEXT
**        IF REC_LOC_HD NOT NULL
**        THEN
**           SET REC_LOC_HD BACK TO NULL
**        ENDIF
**        SET TMP_RP NEXT TO NULL
**        SET TMP_RP BACK TO NULL
**        FREE TMP_RP
**     ENDWHILE
**     FREE FILE NAME AND SET TO NULL
**     FREE CR_HD AND SET TO NULL
**     FREE DL_HD AND SET TO NULL
**     SET NEXT AND BACK POINTERS TO NULL
**     IF CUR_FM->LP_HD NOT NULL 
**     THEN
**        FREE REMAINING LABELS
**     ENDIF
**     WHILE REMAINING RS STRUCTURES, POP TO FREE STRUCTURES
**     FREE CUR_FM AND SET TO NULL
**
**     IF FM_HD NEXT IS NULL
**     THEN
**        IF F_NAME EXISTS
**        THEN
**           FREE F_NAME
**        ENDIF
**        FREE FM_HD AND SET TO NULL
**     ENDIF
**
**     RETURN SUCCESS
**
*****************************************************************************
**      CODE SECTION
**
*****************************************************************************/
#include "stc123.h"

int end123file(FILE **fp)
{
   /* INTERNAL VARIABLES */
   struct dr *tmp_dr;
   long cur_pos;
   int mode_mod;
   int status = 0;
   struct rp *tmp_rp;   
   long tmp_long;

   /* CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(*fp)) return(0);
   
   /* CALL END123REC() TO COMPLETE THE LAST DATA RECORD */
   if (!end123rec(*fp)) return(0);

   /* IF OPEN MODE IS WRITE */
   if (cur_fm->open_mode == 'w') {

      /* RETRIEVE CURRENT POSITION OF INTERNAL FILE POINTER*/
      if ((cur_pos = ftell(*fp)) == -1L) return(0);

      /* CALL CH123SIZE() TO TRUNCATE CONTENTS FOLLOWING CURRENT POSITION */
      if (!ch123size(fp,cur_pos)) return(0);
   }

   /* IF OPEN MODE IS READ */
   if (cur_fm->open_mode == 'r') {
   
      /* SET OPEN MODE TO WRITE */
      cur_fm->open_mode = 'w';
      
      /* SET MODE MODIFICATION FLAG TO TRUE */
      mode_mod = 1;
   }
   else {
   
      /* SET MODE MODIFICATION FLAG TO FALSE */
      mode_mod = 0;
   }

   /* CALL ER123DDREC() TO ERASE DATA DESCRIPTIVE RECORD INFORMATION */
   if (!er123ddrec(*fp,&status)) return(0);

   /* IF MODE MODIFICATION FLAG IS TRUE, RESET OPEN MODE TO READ */
   if (mode_mod) cur_fm->open_mode = 'r';
   
   /* CALL LIBRARY FUNCTION FCLOSE() TO CLOSE FILE 
      IF UNABLE TO CLOSE FILE RETURN FAILURE
   */   
   if (fclose(*fp) == EOF) return(0);
   
   /* SET FILE POINTER TO NULL */
   *fp = NULL;

   /* { REMOVE FILE ENTRY FROM FM STRUCTURE }
      SET CUR_FM BACK NEXT POINTER TO NEXT POINTER 
   */
   cur_fm->back->next = cur_fm->next;
    
   /* IF NOT DELETING AT END OF LIST */
   if (cur_fm->next != NULL) {
   
      /* SET CUR_FM NEXT BACK POINTER TO BACK POINTER */   
      cur_fm->next->back = cur_fm->back;
   };
   
   /* { DELETE RECORD LOCATION POINTER LIST } */
   
   /* SET CUR_RP TO NULL */
   cur_fm->cur_rp = NULL;
   
   /* SET NLD_RP TO NULL */
   cur_fm->nld_rp = NULL;
   
   /* WHILE REC_LOC_HD NOT NULL DO */
   while (cur_fm->rec_loc_hd != NULL ) {
   
      /* SET TMP_RP TO REC_LOC_HD */ 
      tmp_rp = cur_fm->rec_loc_hd;
      
      /* SET REC_LOC_HD TO REC_LOC_HD NEXT */
      cur_fm->rec_loc_hd = cur_fm->rec_loc_hd->next;
      
      /* IF REC_LOC_HD NOT NULL */
      if (cur_fm->rec_loc_hd != NULL) {
      
         /* SET REC_LOC_HD BACK TO NULL */
         cur_fm->rec_loc_hd->back = NULL;
      };
      
      /* SET TMP_RP NEXT TO NULL */
      tmp_rp->next = NULL;
      
      /* SET TMP_RP BACK TO NULL */
      tmp_rp->back = NULL;
      
      /* FREE TMP_RP */
      free(tmp_rp);
   };
   
   /* FREE FILE NAME AND SET TO NULL */
   free(cur_fm->f_name);
   cur_fm->f_name = NULL;

   /* FREE CR_HD AND SET TO NULL */
   free(cur_fm->cr_hd);
   cur_fm->cr_hd = NULL;

   /* FREE DL_HD AND SET TO NULL */
   free(cur_fm->dl_hd);
   cur_fm->dl_hd = NULL;
   
   /* SET NEXT AND BACK POINTERS TO NULL */
   cur_fm->next = NULL;
   cur_fm->back = NULL;
   
   /* IF CUR_FM->LP_HD NOT NULL */
   if (cur_fm->lp_hd != NULL) {

      /* FREE REMAINING LABELS */
      free123lab();
   }

   /* WHILE REMAINING RS STRUCTURES, POP TO FREE STRUCTURES */
   while (pop123rs(&tmp_long));

   /* ================= Add to eliminate MLK ==================*/   

   if (cur_fm->rl_hd != NULL ) {
      free(cur_fm->rl_hd);
      cur_fm->rl_hd = NULL; 
    }   
    
 
    while (cur_fm->cur_dr != NULL) {
      tmp_dr = cur_fm->cur_dr->next;
      free(cur_fm->cur_dr);
      cur_fm->cur_dr = tmp_dr;
    }
  
   /* ============= End of modification ================ */

   /* FREE CUR_FM AND SET TO NULL */
   free(cur_fm);
   cur_fm = NULL;

   /* IF FM_HD NEXT IS NULL */
   if (fm_hd->next == NULL) {
   
      /* IF F_NAME EXISTS */
      if (fm_hd->f_name != NULL) {

         /* FREE F_NAME */
         free(fm_hd->f_name);
      }

      /* FREE FM_HD AND SET TO NULL */
      free(fm_hd);
      fm_hd = NULL;
   }   
      
   /* RETURN SUCCESS */
   return(1); 
}
