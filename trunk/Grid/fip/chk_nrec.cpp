# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: CHK123NREC 
** 
**    PURPOSE: TO RETRIEVE DESCRIPTION OF NEXT RECORD TO READ OR WRITE 
** 
**    INVOCATION METHOD: CHK123NREC(FP,RECLEN,LEADID,DESCR) 
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION
**     FP            PTR        I       FILE POINTER
**     RECLEN        INT        O       RECORD LENGTH
**     LEADID        CHAR       O       LEADER IDENTIFIER
**     DESCR[]       PTR        O       DESCRIPTION
**     CHK123NREC()  LOGICAL    O       SUCCESS FLAG
** 
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     BAK123REC()      BACKS UP TO BEGINNING OF LAST RECORD READ OR WRITTEN 
**     CHK123REC()      GETS DESCRIPTION FOR LAST RECORD READ 
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER 
**     RD123REC()       READS A DATA RECORD 
**     
**    INTERNAL VARIABLES: 
**     NAME       TYPE               DESCRIPTION
**     BK_STAT    INT                BACK UP STATUS 
**                                    0 = FAILURE
**                                    1 = OKAY
**                                    2 = START OF RECORD
**                                    3 = END OF RECORD
**                                    4 = END OF FILE
**                                    5 = END OF FIELD
**                                    6 = START OF FIELD
**     GT_PREC    LOGICAL            GET PREVIOUS RECORD FLAG
**     STATUS     INT                RETRIEVED RECORD STATUS (SAME VALUE
**                                    RANGE AS BACK UP STATUS)
**     STR_LEN    LONG               LENGTH OF STRING
**
**    GLOBAL REFERENCES: 
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
**     GLB_STR[MAXSIZ]  CHAR   I/O   GLOBAL CHARACTER STRING USED FOR
**                                    PROCESSING
**
**    GLOBAL CONSTANTS: NONE
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/15/90  INITIAL PROLOG 
**     P. HODGES                 05/15/90  INITIAL PDL 
**     P. HODGES                 06/14/90  INITIAL CODE 
**     L. MCMILLION              03/06/91  ADDED LOGIC TO REREAD DATA RECORD
**                                          WHICH WAS LOADED PRIOR TO INVOKING
**                                          CHK123NREC()
**     J. TAYLOR     92DR005     05/16/92  CHANGED CALLING SEQUENCE TO RD123REC
**     L. MCMILLION  TASK #40    11/23/92  PROLOG CHANGE PER QC
** 
**    PDL: 
** 
**     INITIALIZE GET PREVIOUS RECORD FLAG TO FALSE
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER 
**     IF FILE IN READ MODE 
**     THEN 
**        IF DR_HD NOT NULL
**        THEN
**           SET GET PREVIOUS RECORD FLAG TO TRUE
**        ENDIF
**        CALL RD123REC() TO READ A DATA RECORD 
**        CALL CHK123REC() TO GET THE DESCRIPTION OF THE LAST RECORD READ 
**        CALL BAK123REC() TO BACK UP TO THE BEGINNING OF THE LAST RECORD 
**         READ
**        IF PREVIOUS RECORD IS TO BE READ
**        THEN
**           CALL BAK123REC() TO BACK UP TO BEGINNING OF THE RECORD READ
**            BEFORE CHK123NREC() WAS INVOKED
**           CALL RD123REC() TO READ A DATA RECORD 
**        ENDIF
**     ELSE {FILE IN WRITE MODE} 
**        CALL CHK123REC() TO GET THE DESCRIPTION OF THE LAST RECORD WRITTEN
**     ENDIF 
** 
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int chk123nrec(FILE *fp,long *reclen,char *leadid,char *descr)
{
   /* INTERNAL VARIABLES */
   int bk_stat;
   int gt_prec = 0;
   long str_len;
   int status;
   
   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */
   if(!get123level(fp)) return (0);

   /* IF FILE IN READ MODE */
   if (cur_fm->open_mode == 'r') {
 
      /* IF DR_HD NOT NULL, SET GET PREVIOUS RECORD FLAG TO TRUE */
      if (cur_fm->dr_hd != NULL) gt_prec = 1;

      /* CALL RD123REC() TO READ A DATA RECORD */
      if (!rd123rec(fp,glb_str,&str_len,&status)) return (0);

      /* CALL CHK123REC() TO GET THE DESCRIPTION OF THE LAST RECORD READ */
      if (!chk123rec(fp,reclen,leadid,descr)) return (0);

      /* CALL BAK123REC() TO BACK UP TO THE BEGINNING OF THE LAST RECORD 
         READ
      */
      if (!bak123rec(fp,&bk_stat)) return (0);
 
      /* IF PREVIOUS RECORD IS TO BE READ */
      if (gt_prec) {

         /* CALL BAK123REC() TO BACK UP TO THE BEGINNING OF THE RECORD 
             READ BEFORE CHK123NREC() WAS INVOKED
         */
         if (!bak123rec(fp,&bk_stat)) return (0);

         /* CALL RD123REC() TO READ THE DATA RECORD */
         if (!rd123rec(fp,glb_str,&str_len,&status)) return (0);
      }
   }
   /* ELSE {FILE IN WRITE MODE} */
   else {

      /* CALL CHK123REC() TO GET THE DESCRIPTION OF THE LAST RECORD WRITTEN */
      if (!chk123rec(fp,reclen,leadid,descr)) return (0);
   }
 
   /* RETURN SUCCESS */
   return (1);
}
