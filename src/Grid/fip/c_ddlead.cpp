# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: CMP123DDLEAD
**
**    PURPOSE: TO COMPUTE INFORMATION NEEDED IN DATA DESCRIPTIVE LEADER
**              STRUCTURE
**
**    INVOCATION METHOD: CMP123DDLEAD()
**
**    ARGUMENT LIST:
**     NAME            TYPE      USE     DESCRIPTION
**     CMP123DDLEAD()  LOGICAL    O      SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     I123TOA()        CONVERTS AN INTEGER TO A CHARACTER STRING
**
**    INTERNAL VARIABLES:
**     NAME        TYPE               DESCRIPTION
**     NUM_ENT     INT                NUMBER OF ENTRIES IN DD STRUCTURE
**     PRV_TAGS    INT                PREVIOUS TAG SIZE
**     TMP_LEN     INT                TEMPORARY LENGTH OF CHARACTER STRING
**     TMP_STR[20] CHAR               TEMPORARY STRING
**  
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     I    LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT     I    POSITION OF DATA DESCRIPTIVE AREA 
**                                    FIELD RECORD
**      FD_CNTRL[10]    CHAR    I    FIELD CONTROLS
**      NAME[]          PTR    N/A   CHARACTER STRING POINTER TO NAME
**      LABELS          PTR    N/A   HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR     I    POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT     O    DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT     O    INTERCHANGE LEVEL
**      LEAD_ID         CHAR    O    LEADER IDENTIFIER 
**      ICE_IND         CHAR    O    INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR    O    RESERVE SPACE CHARACTER
**      APPL_IND        CHAR    O    APPLICATION INDICATOR
**      FD_CNTRL_L      INT     O    FIELD CONTROL LENGTH
**      DA_BASE         INT     O    BASE ADDRESS OF DESCRIPTIVE AREA 
**      CCS_IND[4]      CHAR    O    CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT     O    SIZE OF FIELD LENGTH 
**      S_FDPOS         INT     O    SIZE OF FIELD POSITION  
**      S_RESV          INT     O    RESERVED DIGIT
**      S_TAG           INT    I/O   SIZE OF FIELD TAG
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR    N/A   FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE
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
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION
**     BLNK_SP          CHAR         BLANK SPACE CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/18/90  INITIAL PROLOG
**     P. HODGES                 05/18/90  INITIAL PDL
**     P. HODGES                 05/31/90  INITIAL CODE
**     L. MCMILLION              09/07/90  INSERTED LOGIC TO CHECK FOR
**                                          PRECOMPUTED TAG SIZE AND VALID
**                                          TAG, FIELD LENGTH, AND FIELD
**                                          POSITION SIZES
*      L. MCMILLION  TASK #40    12/20/92  UPDATED PROLOG/PDL/INLINE COMMENTS
**                                          PER QC
**
**    PDL:
**
**     SET NUM_ENT TO ZERO
**     SET PREVIOUS TAG SIZE TO CURRENT TAG SIZE
**     RESET CURRENT TAG SIZE TO ZERO
**     SET S_FDLEN TO 1
**     SET FD_CNTRL_L TO ZERO
**     SET CUR_DD TO DD_HD
**     WHILE CUR_DD NEXT NOT NULL DO
**        SET CUR_DD TO CUR_DD NEXT
**        CALL I123TOA() TO CONVERT FD_LEN TO CHARACTER
**        IF NUMBER OF CHARACTERS MAKING UP FD_LEN IS LARGER THAN S_FDLEN
**        THEN
**           SET S_FDLEN TO NUMBER OF CHARACTERS IN FD_LEN
**        ENDIF
**        IF NUMBER OF CHARACTERS MAKING UP FD_CNTRL IS LARGER THAN FD_CNTRL_L
**        THEN
**           SET FD_CNTRL_L TO NUMBER OF CHARACTERS IN FD_CNTRL
**        ENDIF
**        IF NUMBER OF CHARACTERS IN TAG EXCEEDS S_TAG
**        THEN
**           SET S_TAG TO NUMBER OF CHARACTERS
**        ENDIF
**        INCREMENT NUM_ENT
**     ENDWHILE
**
**     IF TAG SIZE WAS PRECALCULATED AND NOT EQUAL TO COMPUTED TAG SIZE
**     THEN
**        RETURN FAILURE
**     ENDIF
**     IF TAG SIZE IS NOT A VALID SIZE
**     THEN
**        RETURN FAILURE
**     ENDIF
**
**     CALL I123TOA() TO SET S_FDPOS TO NUMBER OF DIGITS IN FD_POS
**      { LAST FIELD IN DD }
**
**     IF SIZE OF FIELD POSITION IS NOT A VALID SIZE
**     THEN
**        RETURN FAILURE
**     ENDIF
**     IF SIZE OF FIELD LENGTH IS NOT A VALID SIZE
**     THEN
**        RETURN FAILURE
**     ENDIF
**
**     SET DA_BASE TO 24 + NUM_ENT * (S_TAG + S_FDLEN + S_FDPOS) + 1
**     SET REC_LEN TO DA_BASE + FD_LEN + FD_POS
**     SET LEAD_ID TO 'L'
**     SET RESERVED SPACE TO BLANK SPACE
**     SET APPLICATION INDICATOR TO BLANK SPACE
**     SET RESERVED DIGIT TO ZERO
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int cmp123ddlead()

{
   /* INTERNAL VARIABLES */
   long num_ent = 0;
   long prv_tags;
   long tmp_len;
   char tmp_str[20]; 

   /* SET PREVIOUS TAG SIZE TO CURRENT TAG SIZE */
   prv_tags = cur_fm->dl_hd->s_tag;

   /* RESET CURRENT TAG SIZE TO ZERO */
   cur_fm->dl_hd->s_tag = 0;

   /* SET S_FDLEN TO 1 */
   cur_fm->dl_hd->s_fdlen = 1;

   /* SET FD_CNTRL_L TO ZERO */
   cur_fm->dl_hd->fd_cntrl_l = 0;

   /* SET CUR_DD TO DD_HD */
   cur_fm->cur_dd = cur_fm->dd_hd;

   /* WHILE CUR_DD NEXT NOT NULL DO */
   while(cur_fm->cur_dd->next != NULL) {

      /* SET CUR_DD TO CUR_DD NEXT */
      cur_fm->cur_dd = cur_fm->cur_dd->next;

      /* CALL I123TOA() TO CONVERT FD_LEN TO CHARACTER */
      i123toa(cur_fm->cur_dd->fd_len,tmp_str);
      
      /* IF NUMBER OF CHARACTERS MAKING UP FD_LEN IS LARGER THAN S_FDLEN */
      if ((tmp_len = _tcslen(tmp_str)) > cur_fm->dl_hd->s_fdlen) {

         /* SET S_FDLEN TO NUMBER OF CHARACTERS IN FD_LEN */
         cur_fm->dl_hd->s_fdlen = tmp_len;
      }
      
      /* IF NUMBER OF CHARACTERS MAKING UP FD_CNTRL LARGER THAN FD_CNTRL_L */
      if ((tmp_len = _tcslen(cur_fm->cur_dd->fd_cntrl)) > cur_fm->dl_hd->fd_cntrl_l) {

         /* SET FD_CNTRL_L TO NUMBER OF CHARACTERS IN FD_CNTRL */
         cur_fm->dl_hd->fd_cntrl_l = tmp_len;
      }
      
      /* IF NUMBER OF CHARACTERS IN TAG EXCEED S_TAG */
      if ((tmp_len = _tcslen(cur_fm->cur_dd->tag)) > cur_fm->dl_hd->s_tag) {

         /* SET S_TAG TO NUMBER OF CHARACTERS */
         cur_fm->dl_hd->s_tag = tmp_len;
      }
      
      /* INCREMENT NUM_ENT */
      num_ent++;
   }

   /* IF TAG SIZE WAS PRECALCULATED AND NOT EQUAL TO COMPUTED TAG SIZE,
       RETURN FAILURE
   */
   if (prv_tags != 0 && prv_tags != cur_fm->dl_hd->s_tag) return(0);

   /* IF TAG SIZE IS NOT A VALID SIZE, RETURN FAILURE */
   if (cur_fm->dl_hd->s_tag < 1 || cur_fm->dl_hd->s_tag > 7) return(0);

   /* CALL I123TOA() TO SET S_FDPOS TO NUMBER OF DIGITS IN FD_POS
       { LAST FIELD IN DD }
   */
   i123toa(cur_fm->cur_dd->fd_pos,tmp_str);
   cur_fm->dl_hd->s_fdpos = _tcslen(tmp_str);

   /* IF SIZE OF FIELD POSITION IS NOT A VALID SIZE, RETURN FAILURE */
   if (cur_fm->dl_hd->s_fdpos < 1 || cur_fm->dl_hd->s_fdpos > 9) return(0);

   /* IF SIZE OF FIELD LENGTH IS NOT A VALID SIZE, RETURN FAILURE */
   if (cur_fm->dl_hd->s_fdlen < 1 || cur_fm->dl_hd->s_fdlen > 9) return(0);

   /* SET DA_BASE TO 24 + NUM_ENT * (S_TAG + S_FDLEN + S_FDPOS) + 1 */
   cur_fm->dl_hd->da_base = 24 + num_ent * (cur_fm->dl_hd->s_tag + cur_fm->dl_hd->s_fdlen + cur_fm->dl_hd->s_fdpos) + 1;

   /* SET REC_LEN TO DA_BASE + FD_LEN + FD_POS */
   cur_fm->dl_hd->rec_len = cur_fm->dl_hd->da_base + cur_fm->cur_dd->fd_len + cur_fm->cur_dd->fd_pos;

   /* SET LEAD_ID TO 'L' */
   cur_fm->dl_hd->lead_id = 'L';

   /* SET RESERVED SPACE TO BLANK SPACE */
   cur_fm->dl_hd->resv_sp = BLNK_SP;

   /* SET APPLICATION INDICATOR TO BLANK SPACE */
   cur_fm->dl_hd->appl_ind = BLNK_SP;

   /* SET RESERVED DIGIT TO ZERO */
   cur_fm->dl_hd->s_resv = 0;

   /* RETURN SUCCESS */
   return(1);
}
