# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: CHK123NSFLD 
** 
**    PURPOSE: TO GET DESCRIPTION OF NEXT SUBFIELD TO READ OR WRITE 
** 
**    INVOCATION METHOD: CHK123NSFLD(FP,TAG,DESCR,FRMT) 
** 
**    ARGUMENT LIST: 
**     NAME           TYPE      USE      DESCRIPTION
**     FP             PTR        I       FILE POINTER
**     TAG[]          PTR        O       FIELD TAG
**     DESCR[]        PTR        O       OUTPUT DESCRIPTION OF SUBFIELD
**     FRMT[]         PTR        O       FORMAT
**     CHK123NSFLD()  LOGICAL    O       SUCCESS/END OF FIELD FLAG
**                                        O - FAILURE
**                                        1 - SUCCESS
**                                        2 - END OF FIELD
**
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     BAK123SFLD()     RETRIEVES LAST SUBFIELD READ OR WRITTEN 
**     CHK123SFLD()     GETS DESCRIPTION OF LAST SUBFIELD READ OR WRITTEN 
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER FOR AN 
**                       INPUT FILE POINTER 
**     RD123SFLD()      RETRIEVES SUBFIELD OF A DATA RECORD 
**     WR123SFLD()      WRITES A SUBFIELD OF A DATA RECORD 
** 
**    INTERNAL VARIABLES: 
**     NAME         TYPE               DESCRIPTION
**     BK_STAT      INT                BACK UP STATUS
**                                      0 = FAILURE
**                                      1 = OKAY
**                                      2 = START OF RECORD
**                                      3 = END OF RECORD
**                                      4 = END OF FILE
**                                      5 = END OF FIELD
**                                      6 = START OF FIELD
**     INIT_S_S_DR  INT                INITIAL SF_STATE_DR
**     LEADID       CHAR               LEADER IDENTIFIER
**     OPTION       INT                WRITE OPTION (OPTION RESTRICTED TO OKAY
**                                      VALUE WITHIN THIS FUNCTION)
**     STATUS       INT                READ RETURN STATUS (SAME VALUE
**                                      RANGE AS BACK UP STATUS)
**     STR_LEN      INT                STRING LENGTH
**     TMP_TAG[10]  CHAR               TEMPORARY FIELD TAG
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
**      NUM_DIM         INT     I    NUMBER OF DIMENSIONS (NO LABELS)
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
**      SF_STATE_DR     INT     I    SUBFIELD STATE (DR) 
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
**      LABEL[]         PTR     I    LABEL
**      NXT_LAB         PTR    N/A   POINTER TO NEXT LABEL IN SET
**      NXT_LSET        PTR     I    POINTER TO NEXT SET OF LABELS
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
**     FCDSTYPE         INT          RELATIVE POSITION OF THE DATA STRUCTURE
**                                    TYPE WITHIN THE FIELD CONTROL
**     NC               CHAR         NULL CHARACTER
**
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/15/90  INITIAL PROLOG 
**     P. HODGES                 05/15/90  INITIAL PDL 
**     P. HODGES                 06/13/90  INITIAL CODE 
**     L. MCMILLION              03/01/91  ADDED LOGIC TO INVOKE BAK123SFLD()
**                                          AGAIN, AND RD123SFLD() IF AT START
**                                          OF RECORD, IN ORDER TO OBTAIN LAST
**                                          SUBFIELD READ IN PREVIOUS DATA
**                                          RECORD
**     L. MCMILLION              03/01/91  ADDED LOGIC TO RETURN FAILURE IF
**                                          CUR_DR IS NULL FOR A WRITE FILE
**     J. TAYLOR     92DR005     05/16/92  CHANGED CALLING SEQUENCE TO RD123SFLD
**     J. TAYLOR     92DR005     05/16/92  CHANGED CALLING SEQUENCE TO WR123SFLD
**     J. TAYLOR     93DR031     06/10/93  MODIFIED TO SAVE THE INITIAL VALUE OF
**                                          CURRENT SF_STATE_DR AND RESTORE
**                                          BEFORE RETURNING
**     J. TAYLOR     93DR031     06/15/93  MODIFIED TO RETURN STATUS OF 2 IF
**                                          FIELD IS A CARTESIAN ARRAY W/O 
**                                          A NULL FIRST LABEL AND SUBFIELD
**                                          STATE IS OUT OF BOUNDS
**     L. MCMILLION  93DR031     07/02/93  MODIFIED TO RETURN STATUS OF 2 IF
**                                          END OF VECTOR DATA FIELD
**
**    PDL: 
** 
**     INITIALIZE WRITE OPTION AND READ STATUS TO OKAY
**     INITIALIZE GLB_STR TO NULL CHARACTER
**     INITIALIZE DESCRIPTION TO NULL
**     INITIALIZE FORMAT TO NULL
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER 
**     SAVE INITIAL SF_STATE_DR
**     IF OPEN MODE IS WRITE AND SUBFIELD STATE DR IS OUT OF BOUNDS
**     THEN
**        IF CURRENT DR STRUCTURE EXISTS
**        THEN
**           SET OUTPUT TAG TO DR TAG
**           IF DR CONTAINS ARRAY DESCRIPTORS
**           THEN
**              RETURN END OF FIELD STATUS
**           ELSE IF CURRENT DD STRUCTURE EXISTS
**           THEN
**              IF FIELD HAS LABELS
**              THEN
**                 IF VECTOR DATA
**                 THEN
**                    RETURN END OF FIELD STATUS
**                 ELSE IF FIRST LABEL SET IS NOT NULL AND ONE OR MORE
**                  LABEL SETS EXIST
**                  {FIELD IS CARTESIAN ARRAY W/O NULL FIRST VECTOR}
**                 THEN
**                    RETURN END OF FIELD STATUS
**                 ENDIF
**              ENDIF
**           ENDIF
**        ENDIF
**     ENDIF
**     IF FILE IN READ MODE 
**     THEN 
**        CALL RD123SFLD() TO RETRIEVE SUBFIELD OF A DATA RECORD 
**     ELSE {FILE IN WRITE MODE} 
**        IF CUR_DR IS NULL
**        THEN
**           RETURN FAILURE
**        ENDIF
**        CALL WR123SFLD() TO TO WRITE A SUBFIELD OF A DATA RECORD WITH 
**         DUMMY INFORMATION 
**        SET OUTPUT TAG TO CUR_DR TAG
**     ENDIF 
**     CALL CHK123SFLD() TO RETRIEVE THE DESCRIPTION OF THE LAST SUBFIELD 
**      READ OR WRITTEN 
**     CALL BAK123SFLD() TO RETRIEVE THE LAST SUBFIELD READ OR WRITTEN 
**     IF FILE IN READ MODE
**     THEN
**        CALL BAK123SFLD() TO BACK UP TO BEGINNING OF LAST SUBFIELD READ
**        IF BACK UP STATUS IS NOT FAILURE
**        THEN
**           { RESTORE SUBFIELD POINTERS TO LAST SUBFIELD READ BEFORE
**             CHK123NSFLD() WAS INVOKED }
**           CALL RD123SFLD() TO RETRIEVE SUBFIELD FROM DATA RECORD 
**        ENDIF 
**     ENDIF 
** 
**     RESTORE INITIAL SF_STATE_DR
**
**     RETURN SUCCESS 
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int chk123nsfld(FILE *fp,char *tag,char *descr,char *frmt)
{
   /* INTERNAL VARIABLES */
   int bk_stat;
   int init_s_s_dr;
   char leadid;
   int option;
   int status;
   long str_len;
   char tmp_tag[10];

   /* INITIALIZE VARIABLES */
   option = 1;
   status = 1;
   leadid = 0;
   glb_str[0] = NC;
   *descr = NC;
   *frmt = NC;

   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return(0);
 
   /* SAVE INITIAL SF_STATE_DR */
   init_s_s_dr = cur_fm->sf_state_dr;
   

   /* IF OPEN MODE IS WRITE AND SUBFIELD STATE DR IS OUT OF BOUNDS */
   if (cur_fm->open_mode == 'w' && cur_fm->sf_state_dr == 4) {

      /* IF CURRENT DR STRUCTURE EXISTS */
      if (cur_fm->cur_dr) {

         /* SET OUTPUT TAG TO DR TAG */
         strcpy(tag,cur_fm->cur_dr->tag);

         /* IF DR CONTAINS ARRAY DESCRIPTORS, RETURN END OF FIELD STATUS */
         if (cur_fm->cur_dr->num_dim != 0) return(2);

         /* ELSE IF CURRENT DD STRUCTURE EXISTS */
         else if (cur_fm->cur_dd) {

            /* IF FIELD HAS LABELS */
            if (cur_fm->cur_dd->labels) {

               /* IF VECTOR DATA, RETURN END OF FIELD STATUS */
               if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '1') return(2);

               /* ELSE IF FIRST LABEL SET IS NOT NULL AND ONE OR MORE
                   LABEL SETS EXIST, RETURN END OF FIELD STATUS
                   {FIELD IS CARTESIAN ARRAY W/O NULL FIRST VECTOR}
               */
              else if (cur_fm->cur_dd->labels->nxt_lset->label &&
               cur_fm->cur_dd->labels->nxt_lset->nxt_lset) return(2);
            }
         }
      }
   }
  
   /* IF FILE IN READ MODE */
   if (cur_fm->open_mode == 'r') {
   
      /* CALL RD123SFLD() TO RETRIEVE SUBFIELD OF A DATA RECORD */
      if (!rd123sfld(fp,tag,&leadid,glb_str,&str_len,&status)) return(0);
   }

   /* ELSE {FILE IN WRITE MODE} */
   else {

      /* IF CUR_DR IS NULL, RETURN FAILURE */
      if (cur_fm->cur_dr == NULL) return(0);

      /* CALL WR123SFLD() TO TO WRITE A SUBFIELD OF A DATA RECORD WITH 
         DUMMY INFORMATION
      */
      if (!wr123sfld(fp,cur_fm->cur_dr->tag,leadid,glb_str,0L,option))
       return(0);

      /* SET OUTPUT TAG TO CUR_DR TAG */
      strcpy(tag,cur_fm->cur_dr->tag);
   } 

   /* CALL CHK123SFLD() TO RETRIEVE THE DESCRIPTION OF THE LAST SUBFIELD 
      READ OR WRITTEN
   */
   if (!chk123sfld(fp,tag,descr,frmt)) return(0);

   /* CALL BAK123SFLD() TO RETRIEVE THE LAST SUBFIELD READ OR WRITTEN */
   if (!bak123sfld(fp,&bk_stat)) return(0);
   
   /* IF FILE IN READ MODE */
   if (cur_fm->open_mode == 'r') {

      /* CALL BAK123SFLD() TO BACK UP TO BEGINNING OF LAST SUBFIELD READ */
      bak123sfld(fp,&bk_stat);
      
      /* IF BACK UP STATUS IS NOT FAILURE { NOT TRYING TO BACK UP PAST
          THE FIRST DATA RECORD }
      */
      if (bk_stat) {
         
         /* { RESTORE SUBFIELD POINTERS TO LAST SUBFIELD READ BEFORE
               CHK123NSFLD() WAS INVOKED }
         */
         
         /* CALL RD123SFLD() TO RETRIEVE SUBFIELD FROM DATA RECORD */
         if (!rd123sfld(fp,tmp_tag,&leadid,glb_str,&str_len,&status)) return(0);
      }
   }

   /* RESTORE INITIAL SF_STATE_DR */
   cur_fm->sf_state_dr = init_s_s_dr;

   /* RETURN SUCCESS */
   return(1);
 
} 
