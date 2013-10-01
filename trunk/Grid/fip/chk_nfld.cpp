# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: CHK123NFLD 
** 
**    PURPOSE: TO GET THE DESCRIPTION OF NEXT FIELD TO READ OR WRITE 
** 
**    INVOCATION METHOD: CHK123NFLD(FP,TAG,FDLEN,FDNAME,FDCNTRL,FMTS,LABS) 
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION
**     FP            PTR        I       FILE POINTER
**     TAG[]         PTR        O       FIELD TAG
**     FDLEN[]       PTR        O       FIELD LENGTH
**     FDNAME[]      PTR        O       FIELD NAME
**     FDCNTRL[]     PTR        O       FIELD CONTROLS
**     FMTS[]        PTR        O       FIELD FORMAT STRING
**     LABS[]        PTR        O       FIELD LABELS
**     CHK123NFLD()  LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     BAK123FLD()      BACKS UP TO BEGINNING OF LAST FIELD READ OR WRITTEN 
**     CHK123FLD()      RETRIEVES DESCRIPTION OF LAST FIELD READ OR WRITTEN 
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER 
**     RD123FLD()       RETRIEVES DATA RECORD FIELD 
**     RET123MATCH()    FINDS THE ASSOCIATED DATA DESCRIPTIVE RECORD MATCH 
**                       TO THE DATA RECORD 
** 
**    INTERNAL VARIABLES:
**     NAME          TYPE            DESCRIPTION
**     BK_STAT       INT             BACK UP STATUS
**                                    0 = FAILURE
**                                    1 = OKAY
**                                    2 = START OF RECORD
**                                    3 = END OF RECORD
**                                    4 = END OF FILE
**                                    5 = END OF FIELD
**                                    6 = START OF FIELD
**     LEADID[2]     CHAR            LEADER IDENTIFIER
**     STATUS        INT             READ FIELD STATUS (SAME VALUE RANGE AS
**                                    BACK UP STATUS)
**     STR_LEN       LONG            STRING LENGTH
**
**    GLOBAL REFERENCES: 
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
**      OPEN_MODE       CHAR    I    OPEN MODE OF FILE
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
**      CUR_DD          PTR    N/A   CURRENT POINTER TO DATA DESCRIPTIVE 
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
**     GLB_STR[MAXSIZ]  CHAR   I/O   GLOBAL CHARACTER STRING USED FOR
**                                    PROCESSING
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION
**     NC               CHAR         NULL CHARACTER
**
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/15/90  INITIAL PROLOG 
**     P. HODGES                 05/15/90  INITIAL PDL 
**     P. HODGES                 06/13/90  INITIAL CODE
**     L. MCMILLION              02/25/91  ADDED LOGIC TO INVOKE BAK123FLD()
**                                          A SECOND TIME IF FIRST BACK UP
**                                          STATUS IS START OF RECORD
**     L. MCMILLION              03/01/91  ADDED LOGIC TO RETURN FAILURE IF
**                                          CUR_DR IS NULL FOR WRITE FILE
**     J. TAYLOR     92DR005     05/16/92  CHANGED CALLING SEQUENCE TO RD123FLD
**     L. MCMILLION  TASK #40    11/20/92  UPDATED PROLOG PER QC
**
**    PDL:
** 
**     INITIALIZE LEADER IDENTIFIER TO NULL CHARACTER
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LEVEL
**     IF FILE IN READ MODE 
**     THEN 
**        CALL RD123FLD() TO RETRIEVE THE NEXT DATA RECORD FIELD 
**        CALL CHK123FLD() TO RETRIEVE THE DESCRIPTION OF THE LAST FIELD 
**         READ OR WRITTEN 
**        CALL BAK123FLD() TO BACK UP TO THE LAST FIELD READ 
**        IF BACK UP STATUS IS START OF RECORD
**        THEN
**           CALL BAK123FLD() AGAIN TO CLEAR DATA STRUCTURES FROM MEMORY
**        ENDIF
**     ELSE {FILE IN WRITE MODE} 
**        IF CUR_DR IS NULL
**        THEN
**           RETURN FAILURE
**        ENDIF
**        CALL RET123MATCH() TO FIND THE ASSOCIATED DATA DESCRIPTIVE RECORD 
**         MATCH TO THE DATA RECORD WITH CUR_DR TAG 
**        CALL CHK123FLD() TO RETRIEVE THE DESCRIPTION OF LAST FIELD WRITTEN 
**     ENDIF 
**
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int chk123nfld(FILE *fp,char *tag,char *fdlen,char **fdname,char *fdcntrl,char *fmts,char *labs)
{
   /* INTERNAL VARIABLES */
   int bk_stat;
   char leadid[2];
   int status;
   long str_len;
   
   /* INITIALIZE LEADER IDENTIFIER TO NULL CHARACTER */
   leadid[0] = NC;

   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LEVEL */
   if (!get123level(fp)) return (0);

   /* IF FILE IN READ MODE */
   if (cur_fm->open_mode == 'r' ) {

      /* CALL RD123FLD() TO RETRIEVE THE NEXT DATA RECORD FIELD */
      if (!rd123fld(fp,tag,leadid,glb_str,&str_len,&status)) return (0);

      /* CALL CHK123FLD() TO RETRIEVE THE DESCRIPTION OF THE LAST FIELD 
          READ OR WRITTEN
      */
      if (!chk123fld(fp,tag,fdlen,fdname,fdcntrl,fmts,labs)) return (0);

      /* CALL BAK123FLD() TO BACK UP TO THE LAST FIELD READ */
      if (!bak123fld(fp,&bk_stat)) return (0);

      /* IF BACK UP STATUS IS START OF RECORD, CALL BAK123FLD() AGAIN
          TO CLEAR DATA STRUCTURE FROM MEMORY
      */
      if (bk_stat == 2) bak123fld(fp,&bk_stat);
   }

   /* ELSE {FILE IN WRITE MODE} */
   else {

      /* IF CUR_DR IS NULL, RETURN FAILURE */
      if (cur_fm->cur_dr == NULL) return (0);

      /* CALL RET123MATCH() TO FIND THE ASSOCIATED DATA DESCRIPTIVE RECORD 
          MATCH TO THE DATA RECORD WITH CUR_DR TAG
      */
      if (!ret123match(cur_fm->cur_dr->tag)) return (0);

      /* CALL CHK123FLD() TO RETRIEVE THE DESCRIPTION OF LAST FIELD WRITTEN */
      if (!chk123fld(fp,tag,fdlen,fdname,fdcntrl,fmts,labs)) return (0);
   }

   /* RETURN SUCCESS */
   return (1);
}
