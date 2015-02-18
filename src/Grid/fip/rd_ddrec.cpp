# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RD123DDREC
**
**    PURPOSE: TO READ THE NEXT RECORD
**
**    INVOCATION METHOD: RD123DDREC(FP,STRING,STATUS)
**
**    ARGUMENT LIST:
**     NAME          TYPE      USE      DESCRIPTION
**     FP            PTR        I       FILE POINTER
**     STRING[]      PTR        O       RECORD READ
**     STATUS        INT        O       STATUS
**                                       0 = FAILURE
**                                       1 = OKAY
**                                       2 = START OF RECORD
**                                       3 = END OF RECORD
**                                       4 = END OF FILE
**                                       5 = END OF FIELD
**                                       6 = START OF FIELD
**     RD123DDREC()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     G123FSTR()       READS A FULL CHARACTER STRING
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     LD123DDREC()     LOADS THE DATA DESCRIPTIVE RECORD INFORMATION INTO
**                       STRUCTURES FOR REFERENCE
**     SET123STAT()     SET STATUS FLAG
**
**    INTERNAL VARIABLES:
**     NAME       TYPE     DESCRIPTION
**     REC_LEN    INT      RECORD LENGTH OF DDR
**     CUR_PTR    PTR      CURRENT POINTER TO DD STRUCTURE
**  
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT     I    DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT    N/A   INTERCHANGE LEVEL
**      LEAD_ID         CHAR   N/A   LEADER IDENTIFIER 
**      ICE_IND         CHAR   N/A   INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR   N/A   RESERVE SPACE CHARACTER
**      APPL_IND        CHAR   N/A   APPLICATION INDICATOR
**      FD_CNTRL_L      INT    N/A   FIELD CONTROL LENGTH
**      DA_BASE         INT     I    BASE ADDRESS OF DESCRIPTIVE AREA 
**      CCS_IND[4]      CHAR   N/A   CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH 
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION  
**      S_RESV          INT    N/A   RESERVED DIGIT
**      S_TAG           INT    N/A   SIZE OF FIELD TAG 
**      
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     I    LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT     I    POSITION OF DATA DESCRIPTIVE AREA 
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
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/17/90  INITIAL PROLOG
**     P. HODGES                 05/17/90  INITIAL PDL
**     A. DEWITT                 05/26/90  INITIAL CODE
**     A. DEWITT                 06/12/90  MODIFY CODE/PDL TO IMPLEMENT STATUS
**     A. DEWITT                 08/22/90  ADDED LOGIC TO LOAD DDR IF NOT LOADED
**                                          PRIOR TO THIS FUNCTION
**     L. MCMILLION  TASK #40    12/22/92  UPDATED PDL/INLINE COMMENTS PER QC
**
**    PDL:
**
**     INITIALIZE STATUS TO FAILURE
**     CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER
**     IF DD_HD NOT NULL
**     THEN
**        IF DD_HD NEXT IS NULL
**        THEN
**           CALL LD123DDREC() TO PERFORM RECORD INPUT AND LOAD INTO STRUCTURE
**           IF LD123DDREC() ENCOUNTERED EOF
**           THEN
**              SET STATUS TO EOF
**           ENDIF
**           RETURN FAILURE
**        ENDIF
**     ELSE
**        CALL LD123DDREC() TO PERFORM RECORD INPUT AND LOAD INTO STRUCTURE
**        IF LD123DDREC() ENCOUNTERED EOF
**        THEN
**           SET STATUS TO EOF
**        ENDIF
**        RETURN FAILURE
**     ENDIF
**     RETRIEVE RECORD LENGTH FROM DL
**     IF RECORD LENGTH 100,000 BYTES OR GREATER (INDICATED BY 0)
**     THEN
**        SET CURRENT POINTER TO DD_HD
**        WHILE CURRENT POINTER NEXT NOT NULL
**           SET CURRENT POINTER TO CURRENT POINTER NEXT
**        ENDWHILE
**        SET RECORD LENGTH TO FIELD LENGTH AND FIELD POSITION OF DD
**         STRUCTURE
**        ADD BASE ADDRESS OF DESCRIPTIVE AREA FROM DL STRUCTURE TO
**         RECORD LENGTH
**     ENDIF
**
**     REWIND FILE
**     CALL G123FSTR() TO READ RECORD AS A STRING
**
**     CALL SET123STAT() TO SET STATUS FLAG
**     IF STATUS IS NOT EOF 
**     THEN
**        SET STATUS TO END OF RECORD
**     ENDIF
**     SET SUBFIELD STATE TO FINISHED
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int rd123ddrec(FILE *fp,char *string,int *status)
{
   /* LOCAL VARIABLES */
   long rec_len;
   struct dd *cur_ptr;

   /* INITIALIZE STATUS TO FAILURE */
   *status = 0;
   
   /* RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return(0);

   /* IF DD_HD NOT NULL */
   if (cur_fm->dd_hd != NULL) {

      /* IF DD_HD NEXT IS NULL */
      if (cur_fm->dd_hd->next == NULL) {

         /* CALL LD123DDREC() TO PERFORM RECORD INPUT AND LOAD INTO
             STRUCTURE
         */
         if (!ld123ddrec()) {

            /* IF LD123DDREC() ENCOUNTERED EOF, SET STATUS TO EOF */
            if (feof(fp)) *status = 4;

            /* RETURN FAILURE */
            return(0);
         }
      }
   }
   else {

      /* CALL LD123DDREC() TO PERFORM RECORD INPUT AND LOAD INTO STRUCTURE */
      if (!ld123ddrec()) {

         /* IF LD123DDREC() ENCOUNTERED EOF, SET STATUS TO EOF */
         if (feof(fp)) *status = 4;

         /* RETURN FAILURE */
         return(0);
      }
   }

   /* RETRIEVE RECORD LENGTH FROM DL */
   rec_len = cur_fm->dl_hd->rec_len;

   /* IF RECORD LENGTH 100,000 BYTES OR GREATER (INDICATED BY 0) */
   if (rec_len == 0) {

      /* SET CURRENT POINTER TO DD_HD AND FIND END OF DD LINKED LIST */
      cur_ptr = cur_fm->dd_hd;
      while (cur_ptr->next != NULL) cur_ptr = cur_ptr->next;

      /* SET RECORD LENGTH TO FIELD LENGTH AND FIELD POSITION OF
          DD STRUCTURE 
      */
      rec_len = cur_ptr->fd_len + cur_ptr->fd_pos;

      /* ADD BASE ADDRESS OF DESCRIPTIVE AREA FROM DL STRUCTURE 
          RECORD LENGTH
      */
      rec_len += cur_fm->dl_hd->da_base;
   }

   /* REWIND FILE */
   rewind(fp);

   /* CALL G123FSTR() TO READ RECORD AS A STRING */
   if (!g123fstr(fp,string,rec_len)) return(0);

   /* CALL SET123STAT() TO SET STATUS FLAG */
   if (!set123stat(fp,status)) return(0);
   
   /* IF STATUS IS NOT EOF THEN SET STATUS TO END OF RECORD */
   if (*status != 4) *status = 3;
   
   /* SET SUBFIELD STATE TO FINISHED */
   cur_fm->sf_state_dd = 5;

   /* RETURN SUCCESS */
   return(1);
}
