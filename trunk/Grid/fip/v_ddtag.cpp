# include "stdafx.h"
/***************************************************************************
** 
**    INVOCATION NAME: VER123DDTAG 
** 
**    PURPOSE: TO VERIFY THE ORDER OF DATA DESCRIPTIVE TAGS
** 
**    INVOCATION METHOD: VER123DDTAG()
** 
**    ARGUMENT LIST:
**     NAME           TYPE      USE      DESCRIPTION
**     VER123DDTAG()  LOGICAL    O       VERIFY DATA DESCRIPTIVE TAGS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION 
**     IS123INTGR()     DETERMINES IF A STRING IS COMPRISED OF INTEGERS
**
**    INTERNAL VARIABLES: 
**     NAME           TYPE       DESCRIPTION 
**     CUR_DD         PTR        CURRENT POINTER TO DATA DESCRIPTIVE STRUCTURE
**     I              INT        INTEGER LOOP CONTROL VARIABLE
**     ITAG           INT        INTEGER TAG
**     NON_NUM        INT        FLAG INDICATING THAT TAG OTHER THAN TAGS 
**                                0..0 - 0..9 HAVE BEEN FOUND
**     NUM[NFTAG123]  INT        FLAGS INDICATING WHETHER OR NOT 
**                                TAGS 0..0 - 0..9 HAVE BEEN FOUND
**     TMP_DD         PTR        TEMPORARY POINTER TO DATA DESCRIPTIVE
**                                STRUCTURE
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
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION 
**     NFTAG123         INT          NUMBER OF FIELD TAGS DEFINED BY FIPS123
**                                    INTERCHANGE STANDARD 
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     J. TAYLOR     92DR002     11/12/92  INITIAL PROLOG 
**     J. TAYLOR     92DR003     11/12/92  INITIAL PDL 
**     J. TAYLOR     92DR006     11/12/92  INITIAL CODE
**     L. MCMILLION  92DR002     01/15/93  MODIFIED LOGIC TO ALLOW NUMERIC
**                   92DR006                FIELD TAGS OUTSIDE 0..0 - 0..9
**                                          RANGE
**     L. MCMILLION  92DR002     01/21/93  ADDED LOGIC TO ONLY VERIFY DDR
**                   92DR006                TAG ORDER IF FILE IS NOT LEVEL 3
** 
**    PDL: 
** 
**     IF FILE IS INTERCHANGE LEVEL 3
**     THEN
**        RETURN SUCCESS--DO NOT VERIFY
**     ENDIF
**     SET LOCAL CUR_DD TO DD_HD
**     SET NON-NUMERIC FOUND FLAG TO FALSE
**     SET NUMERIC FOUND FLAGS 0..0 - 0..9 TO FALSE
**     WHILE MORE DATA DESCRIPTIVE FIELDS EXIST DO
**        GO TO NEXT DATA DESCRIPTIVE FIELD
**        IF TAG IS STRING OF INTEGERS { IS123INTGR() }
**        THEN
**           CONVERT TAG TO INTEGER
**           IF INTEGER TAG IS DEFINED BY FIPS123
**           THEN
**              IF NON-NUMERIC TAG PREVIOUSLY ENCOUNTERED
**              THEN
**                 RETURN FAILURE
**              ENDIF
**              DO FOR EACH TAG FROM CURRENT TO LAST TAG DEFINED BY FIPS123
**                 IF NUMERIC TAG OF HIGHER VALUE ALREADY ENCOUNTERED
**                 THEN
**                    RETURN FAILURE
**                 ENDIF
**              ENDDO
**              SET NUMERIC TAG FOUND FLAG 0..N TO TRUE
**           ELSE NUMERIC TAG BUT NOT FIPS123 DEFINED
**              SET NON-NUMERIC FOUND FLAG TO TRUE
**           ENDIF
**        ELSE TAG IS NON-NUMERIC
**           SET NON-NUMERIC FOUND FLAG TO TRUE
**        ENDIF
**        { DETERMINE IF CURRENT TAG IS DUPLICATED IN REMIANDER OF DD LIST }
**        SET TEMP_DD TO CUR_DD
**        WHILE TMP_DD NEXT NOT NULL DO
**           IF CURRENT TAG MATCHES TAG FOLLOWING TMP_DD
**           THEN
**              RETURN FAILURE
**           ENDIF
**           MOVE TMP_DD TO NEXT DD ENTRY
**        ENDWHILE { TMP_DD }
**     ENDWHILE { CUR_DD }
**     IF NUMERIC TAG 0..1 NOT FOUND THEN RETURN FAILURE
**     RETURN SUCCESS
**
**
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int ver123ddtag()

{
   /* INTERNAL VARIABLES */   
   struct dd *cur_dd;
   struct dd *tmp_dd;
   int itag;
   int i;
   int non_num;
   int num[NFTAG123];

   /* IF FILE IS INTERCHANGE LEVEL 3--DO NOT VERIFY TAGS, RETURN SUCCESS */
   if (cur_fm->dl_hd->ilevel == 3) return(1);

   /* SET LOCAL CUR_DD TO DD_HD */
   cur_dd = cur_fm->dd_hd;

   /* SET NON-NUMERIC FOUND FLAG TO FALSE */
   non_num = 0;

   /* SET NUMERIC FOUND FLAGS 0..0 - 0..9 TO FALSE */
   for (i=0;i<=NFTAG123-1;i++) num[i] = 0;

   /* WHILE MORE DATA DESCRIPTIVE FIELDS EXIST DO */
   while (cur_dd->next != NULL) {

      /* GO TO NEXT DATA DESCRIPTIVE FIELD */
      cur_dd = cur_dd->next;

      /* IF TAG IS INTEGER */
      if (is123intgr(cur_dd->tag)) {

         /* CONVERT TAG TO INTEGER */
         itag = atoi(cur_dd->tag);

         /* IF INTEGER TAG IS DEFINED BY FIPS123 */
         if (itag >= 0 && itag < NFTAG123) {

            /* IF NON-NUMERIC TAG PREVIOUSLY ENCOUNTERED, RETURN FAILURE */
            if (non_num) return(0);

            /* DO FOR EACH TAG FROM CURRENT TO LAST TAG DEFINED
                BY FIPS123--IF NUMERIC TAG OF HIGHER VALUE ALREADY
                ENCOUNTERED, RETURN FAILURE
            */
            for (i=itag;i<NFTAG123;i++) if (num[i]) return(0);

            /* SET NUMERIC TAG FOUND FLAG 0..N TO TRUE */
            num[itag] = 1;
         }

         /* ELSE NUMERIC TAG BUT NOT FIPS123 DEFINED */
         else

            /* SET NON-NUMERIC FOUND FLAG TO TRUE */
            non_num = 1;
      }

      /* ELSE TAG IS NON-NUMERIC */
      else {

         /* SET NON-NUMERIC FOUND FLAG TO TRUE */
         non_num = 1;
      }

      /* { DETERMINE IF CURRENT TAG IS DUPLICATED IN REMAINDER OF LIST } */

      /* SET TEMP_DD TO CUR_DD */
      tmp_dd = cur_dd;

      /* WHILE TMP_DD NEXT NOT NULL DO */
      while (tmp_dd->next != NULL) {

         /* IF CURRENT TAG MATCHES TAG FOLLOWING TMP_DD, RETURN FAILURE */
         if (!strcmp(cur_dd->tag,tmp_dd->next->tag)) return(0);

         /* MOVE TMP_DD TO NEXT DD ENTRY */
         tmp_dd = tmp_dd->next;
      }
   }

   /* IF NUMERIC TAG 0..1 NOT FOUND THEN RETURN FAILURE */
   if (!num[1]) return(0);

   /* RETURN SUCCESS */
   return(1);

}      
