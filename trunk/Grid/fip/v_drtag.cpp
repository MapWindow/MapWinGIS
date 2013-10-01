# include "stdafx.h"
/***************************************************************************
** 
**    INVOCATION NAME: VER123DRTAG 
** 
**    PURPOSE: TO VERIFY THE ORDER OF DATA RECORD TAGS WITH THOSE IN THE
**              DATA DESCRIPTIVE RECORD
** 
**    INVOCATION METHOD: VER123DRTAG()
** 
**    ARGUMENT LIST:
**     NAME           TYPE      USE      DESCRIPTION
**     VER123DRTAG()  LOGICAL    O       VERIFY DATA RECORD TAGS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     IS123INTGR()     DETERMINES IF A STRING IS COMPRISED OF INTEGERS
**
**    INTERNAL VARIABLES: 
**     NAME         TYPE         DESCRIPTION 
**     CUR_DD       PTR          CURRENT POINTER TO DATA DESCRIPTIVE STRUCTURE
**     CUR_DR       PTR          CURRENT POINTER TO DATA RECORD STRUCTURE
**     ITAG         INT          INTEGER TAG
** 
**    GLOBAL REFERENCES: 
**
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA 
**                                    FIELD RECORD
**      FD_CNTRL[10]    CHAR   N/A   FIELD CONTROLS
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
**      NEXT            PTR     I    POINTER TO NEXT DATA RECORD  
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
**      NEXT            PTR     I    POINTER TO NEXT FILE MANAGEMENT 
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
**     J. TAYLOR     92DR002     11/12/92  INITIAL PROLOG 
**     J. TAYLOR     92DR003     11/12/92  INITIAL PDL 
**     J. TAYLOR     92DR006     11/12/92  INITIAL CODE
**     L. MCMILLION  92DR003     01/18/93  UPDATED PROLOG PER QC
**     L. MCMILLION  92DR002     01/21/93  ADDED LOGIC TO NOT VERIFY DR TAG
**                   92DR003                ORDER IF FILE IS INTERCHANGE
**                   92DR006                LEVEL 3
** 
**    PDL: 
** 
**     IF FILE IS INTERCHANGE LEVEL 3
**     THEN
**        RETURN SUCCESS--DO NOT VERIFY TAG ORDER
**     ENDIF
**     SET LOCAL CUR_DR TO DR_HD
**     SET LOCAL CUR_DD TO DD_HD
**     SET INTEGER TAG TO -1
**     GO TO 1st TAG
**     IF DR FIELD TAG IS NUMERIC
**     THEN
**        CONVERT TAG TO INTEGER
**     ENDIF
**     IF 1st TAG IS NOT 0..1
**     THEN
**        RETURN FAILURE
**     ENDIF
**     WHILE MORE DR FIELDS EXIST DO
**        GO TO NEXT DR FIELD
**        IF DR FIELD TAG NOT FOUND IN DATA DESCRIPTIVE RECORD
**        THEN
**           WHILE DR FIELD TAG NOT FOUND IN DATA DESCRIPTIVE RECORD OR 
**            NO MORE TAGS DO
**              GO TO NEXT DATA DESCRIPTIVE TAG
**           ENDWHILE
**        ENDIF
**     ENDWHILE
**     IF DR FIELD TAG NOT FOUND
**     THEN
**        RETURN FAILURE
**     ENDIF
**     RETURN SUCCESS
**
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int ver123drtag()

{
   /* INTERNAL VARIABLES */   
   struct dr *cur_dr;
   struct dd *cur_dd;
   int itag;

   /* IF FILE IS INTERCHANGE LEVEL 3--DO NOT VERIFY TAGS, RETURN SUCCESS */
   if (cur_fm->dl_hd->ilevel == 3) return(1);

   /* SET LOCAL CUR_DR TO DR_HD */
   cur_dr = cur_fm->dr_hd;

   /* SET LOCAL CUR_DD TO DD_HD */
   cur_dd = cur_fm->dd_hd;

   /* SET INTEGER TAG TO -1 */
   itag = -1;

   /* GO TO 1st TAG */
   cur_dr = cur_dr->next;

   /* IF DR FIELD TAG IS NUMERIC */
   if (is123intgr(cur_dr->tag)) {

      /* CONVERT TAG TO INTEGER */
      itag = atoi(cur_dr->tag);
   }

   /* IF 1st TAG IS NOT 0..1, RETURN FAILURE */
   if (itag != 1) return(0);

   /* WHILE MORE DATA FIELDS EXIST DO */
   while (cur_dr->next != NULL) {

      /* GO TO NEXT DR FIELD */
      cur_dr = cur_dr->next;

      /* IF DR FIELD TAG NOT FOUND IN DATA DESCRIPTIVE RECORD */
      if (strcmp(cur_dr->tag,cur_dd->tag)) {

         /* WHILE DR FIELD TAG NOT FOUND IN DATA DESCRIPTIVE RECORD OR
             NO MORE TAGS */
         while (strcmp(cur_dr->tag,cur_dd->tag) && cur_dd->next != NULL) {

            /* GO TO NEXT DATA DECRIPTIVE TAG */
            cur_dd = cur_dd->next;

         }
      }

      /* IF DR FIELD TAG NOT FOUND, RETURN FAILURE */
      if (strcmp(cur_dr->tag,cur_dd->tag)) return(0);

   }
   /* RETURN SUCCESS */
   return(1);

}      
