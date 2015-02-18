# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: CMP123DDDIR
**
**    PURPOSE: TO COMPUTE DIRECTORY INFORMATION FOR DATA DIRECTORY RECORD
**
**    INVOCATION METHOD: CMP123DDDIR()
**
**    ARGUMENT LIST:
**     NAME           TYPE      USE    DESCRIPTION
**     CMP123DDDIR()  LOGICAL    O     SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     RD123DDFLD()     READS THE NEXT DATA DESCRIPTIVE FIELD
**     VER123DDTAG()    VERIFIES THE ORDER OF THE DATA DESCRIPTIVE TAGS
**
**    INTERNAL VARIABLES:
**     NAME            TYPE            DESCRIPTION
**     POS             INT             POSITION
**     STATUS          INT             STATUS
**                                      0 = FAILURE
**                                      1 = OKAY
**                                      2 = START OF RECORD
**                                      3 = END OF RECORD
**                                      4 = END OF FILE (DR)
**                                      5 = END OF FIELD
**                                      6 = START OF FIELD
**
**     TAG[10]         CHAR            FIELD TAG
**
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     O    LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT     O    POSITION OF DATA DESCRIPTIVE AREA 
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
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR     I    FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE
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
**      SF_STATE_DD     INT     O    SUBFIELD STATE (DD)
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
**     GLB_STR2[MAXSIZ] CHAR   I/O   SECOND GLOBAL CHARACTER STRING USED FOR
**                                    PROCESSING
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/18/90  INITIAL PROLOG
**     P. HODGES                 05/18/90  INITIAL PDL
**     P. HODGES                 05/30/90  INITIAL CODE
**     L. MCMILLION              10/23/90  ADDED LOGIC TO RETURN FAILURE IF
**                                          NO DD ENTRIES FOLLOW DD_HD
**     L. MCMILLION  TASK #40    12/20/92  UPDATED PROLOG/PDL/INLINE COMMENTS
**                                          PER QC
**     J. TAYLOR     92DR002     11/16/92  ADDED CALL TO VER123DDTAG TO VERIFY
**                   92DR006                ORDER OF TAGS IN DATA DESCRIPTIVE
**                                          RECORD
**     L. MCMILLION  92DR002     01/18/93  MOVED VER123DDTAG() CALL TO PRECEDE
**                   92DR006                REPEATED RD123DDFLD() CALLS
**
**    PDL:
**
**     SET POS TO ZERO
**     IF DD_HD NEXT IS NULL
**     THEN
**        RETURN FAILURE
**     ENDIF
**     CALL VER123DDTAG() TO VERIFY TAG ORDER IN DATA DESCRIPTIVE RECORD
**     SET CUR_DD TO DD_HD NEXT
**     WHILE CUR_DD NOT NULL DO
**        SET SUBFIELD STATE TO FIELD CONTROL SUBFIELD
**        CALL RD123DDFLD() TO READ THE NEXT DATA DESCRIPTIVE FIELD
**        SET FD_LEN TO LENGTH OF THE STRING RETURNED FROM RD123DDFLD()
**        SET FD_POS TO POS
**        SET POS TO POS PLUS FD_LEN
**        SET CUR_DD TO CUR_DD NEXT
**     ENDWHILE
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int cmp123dddir()

{
   /* INTERNAL VARIABLES */
   long pos = 0;
   char tag[10];
   int status;

   /* IF DD_HD NEXT IS NULL, RETURN FAILURE */
   if (cur_fm->dd_hd->next == NULL) return(0);
    
   /* CALL VER123DDTAG() TO VERIFY TAG ORDER IN DATA DESCRIPTIVE RECORD */
   if (!ver123ddtag()) return(0);

   /* SET CUR_DD TO DD_HD NEXT */
   cur_fm->cur_dd = cur_fm->dd_hd->next;

   /* WHILE CUR_DD NOT NULL DO */
   while(cur_fm->cur_dd != NULL) {

      /* SET SUBFIELD STATE TO FIELD CONTROL SUBFIELD */
      cur_fm->sf_state_dd = 1;

      /* CALL RD123DDFLD() TO READ THE NEXT DATA DESCRIPTIVE FIELD  */
      if (!rd123ddfld(cur_fm->fp,tag,glb_str2,&status)) return(0);

      /* SET FD_LEN TO LENGTH OF THE STRING RETURNED FROM RD123DDFLD() */
      cur_fm->cur_dd->fd_len = _tcslen(glb_str2);

      /* SET FD_POS TO POS */
      cur_fm->cur_dd->fd_pos = pos;

      /* SET POS TO POS PLUS FD_LEN */
      pos += cur_fm->cur_dd->fd_len;

      /* SET CUR_DD TO CUR_DD NEXT */
      cur_fm->cur_dd = cur_fm->cur_dd->next;
   }

   /* RETURN SUCCESS */
   return(1);
}
