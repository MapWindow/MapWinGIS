# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: CMP123DRDIR
** 
**    PURPOSE: TO COMPUTE DIRECTORY INFORMATION FOR DATA RECORD 
** 
**    INVOCATION METHOD: CMP123DRDIR()
** 
**    ARGUMENT LIST:
**     NAME           TYPE      USE    DESCRIPTION
**     CMP123DRDIR()  LOGICAL    O     SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     RD123FLD()       READS THE NEXT FIELD 
**     VER123DRTAG()    VERIFIES ORDER OF DATA RECORD TAGS
** 
**    INTERNAL VARIABLES: 
**     NAME       TYPE               DESCRIPTION 
**     LEADID     CHAR               LEADER IDENTIFIER
**     LEN         INT               FIELD LENGTH
**     POS         INT               POSITION 
**     STATUS      INT               FIELD STATUS
**                                    0 = FAILURE
**                                    1 = OKAY
**                                    2 = START OF RECORD
**                                    3 = END OF RECORD
**                                    4 = END OF FILE 
**                                    5 = END OF FIELD
**                                    6 = START OF FIELD
**     TAG        CHAR               FIELD TAG
**  
**    GLOBAL REFERENCES: 
** 
**     DATA RECORD STRUCTURE (DR) 
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     O    LENGTH OF DISCRIPTIVE AREA DATA RECORD  
**      FD_POS          INT     O    POSITION OF DESCRIPTIVE AREA DATA 
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
**      FP              PTR     I    FILE POINTER 
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
**      NLD             INT    N/A   NO LEADER FLAG 
**                                    TRUE  - NO LEADER 
**                                    FALSE - LEADER EXISTS 
**      BIT_CNT         INT    N/A   COUNT OF BITS STORED IN MEMORY FOR 
**                                    A SUBSEQUENT READ FUNCTION
**      BIT_BIN         CHAR   N/A   BITS STORED IN MEMORY FOR SUBSEQUENT
**                                    READ FUNCTION
**      COMPRESS        INT    I/O   FLAG TO SPECIFY COMPRESSED OR
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
**     P. HODGES                 05/18/90  INITIAL PROLOG 
**     P. HODGES                 05/18/90  INITIAL PDL 
**     P. HODGES                 06/08/90  INITIAL CODE 
**     J. TAYLOR     92DR005     05/16/92  CHANGE CALLING SEQUENCE TO RD123FLD
**     J. TAYLOR     92DR005     05/20/92  MODIFIED CALL TO RD123FLD TO RETURN
**                                          COMPRESSED ADJACENT FIXED LENGTH 
**                                          BIT FIELDS
**     L. MCMILLION  TASK #40    11/20/92  PROLOG/PDL UPDATED PER QC
**     J. TAYLOR     92DR003     11/16/92  ADDED CALL TO VER123DRTAG TO VERIFY
**                                          ORDER OF TAGS IN THE DATA RECORD
** 
**    PDL: 
**
**     SET STATUS TO OKAY
**     SET POS TO ZERO
**     SET CUR_DR TO DR_HD
**     WHILE STATUS NOT END OF RECORD AND NOT FAILURE DO
**        SET COMPRESS FLAG TO COMPRESS
**        CALL RD123FLD() TO READ THE NEXT FIELD 
**        SET COMPRESS FLAG TO UNCOMPRESS
**        SET FD_LEN TO COMPUTED LENGTH OF THE FIELD 
**        SET FD_POS TO POS 
**        SET POS TO POS PLUS FD_LEN 
**     ENDWHILE
**
**     CALL VER123DRTAG() TO VERIFY ORDER OF TAGS IN DATA RECORD
** 
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/
#include "stc123.h"

int cmp123drdir()
{

   /* INTERNAL VARIABLES */
   long len;
   long pos = 0;
   int status = 1;
   char leadid[10];
   char tag [10];
   
   /* SET CUR_DR TO DR_HD */
   cur_fm->cur_dr = cur_fm->dr_hd;

   /* WHILE STATUS NOT END OF RECORD AND NOT FAILURE DO */
   while (status != 3 && status != 0) {

      /* CALL RD123FLD() TO READ THE NEXT FIELD */
      cur_fm->compress = 1;
      if (!rd123fld(cur_fm->fp,tag,leadid,glb_str,&len,&status)) return(0);
      cur_fm->compress = 0;

      /* SET FD_LEN TO COMPUTED LENGTH OF THE FIELD */
      cur_fm->cur_dr->fd_len = len ;

      /* SET FD_POS TO POS */
      cur_fm->cur_dr->fd_pos = pos;

      /* SET POS TO POS PLUS FD_LEN */
      pos += cur_fm->cur_dr->fd_len;
 
   }

   /* CALL VER123DRTAG() TO VERIFY ORDER OF TAGS IN DATA RECORD */
   if (!ver123drtag()) return(0);

   /* RETURN SUCCESS */
   return (1);
}
