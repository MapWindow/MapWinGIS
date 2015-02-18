# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RET123DRDIR
**
**    PURPOSE: TO RETURN THE DIRECTORY INFORMATION FOR DATA RECORD
**
**    INVOCATION METHOD: RET123DRDIR(STRING)
**
**    ARGUMENT LIST:
**     NAME           TYPE      USE    DESCRIPTION
**     RET123DRDIR()  LOGICAL    O     SUCCESS FLAG
**     STRING[]       CHAR       O     STRING CONTAINING DATA RECORD
**                                      DIRECTORY INFORMATION
**     
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME            TYPE            DESCRIPTION
**     CUR_DR          PTR             CURRENT DATA RECORD STRUCTURE
**     TMP_STR[25]     CHAR            TEMPORARY CHARACTER STRING
**
**    GLOBAL REFERENCES:
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     I    LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT     I    POSITION OF DESCRIPTIVE AREA DATA
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
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                    STRUCTURE
**      DR_HD           PTR     I    HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR     I    HEAD POINTER TO DATA RECORD LEADER
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
**     DATA RECORD LEADER STRUCTURE (RL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT    N/A   DATA RECORD LENGTH     
**      RESV_SP         CHAR   N/A   RESERVED SPACE CHARACTER
**      LEAD_ID         CHAR   N/A   LEADER IDENTIFIER
**      S_RESV[6]       CHAR   N/A   RESERVED SPACE
**      DA_BASE         INT    N/A   BASE ADDRESS OF DATA AREA  
**      R_SPACE[4]      CHAR   N/A   RESERVED SPACE CHARACTERS
**      S_FDLEN         INT     I    SIZE OF FIELD LENGTH  
**      S_FDPOS         INT     I    SIZE OF FIELD POSITION 
**      SP_RSRV         INT    N/A   RESERVED DIGIT
**      S_TAG           INT     I    SIZE OF FIELD TAG 
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS: 
**     NAME             TYPE         DESCRIPTION
**     NC               CHAR         NULL CHARACTER
**     FT_STR[2]        CHAR         CHARACTER STRING CONTAINING THE FIELD
**                                    TERMINATOR CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     J. TAYLOR     TASK 55     01/08/93  INITIAL PROLOG
**     J. TAYLOR     TASK 55     01/08/93  INITIAL PDL
**     J. TAYLOR     TASK 55     01/08/93  INITIAL CODE
**
**    PDL:
**
**     SET STRING TO NULL CHARACTER
**     SET LOCAL CURRENT DR TO DR HEADER 
**     WHILE NEXT DR NOT NULL
**        SET CURRENT DR TO NEXT DR
**        CONCATENATE CURRENT DR TAG TO STRING
**        CONVERT CURRENT DR FIELD LENGTH TO CHARCTER STRING
**        CONCATENATE CURRENT DR FIELD LENGTH TO STRING
**        CONVERT CURRENT DR FIELD POSITION TO CHARCTER STRING
**        CONCATENATE CURRENT DR FIELD POSITION TO STRING
**     ENDWHILE 
**     CONCATENATE FIELD TERMINATOR TO STRING
** 
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int ret123drdir(char *string)

{
   /* INTERNAL VARIABLES */
   struct dr *cur_dr;
   char tmp_str[25];

   /* SET STRING TO NULL CHARACTER */
   *string = NC;

   /* SET LOCAL CURRENT DR TO DR HEADER  */
   cur_dr = cur_fm->dr_hd;

   /* WHILE NEXT DR NOT NULL */
   while (cur_dr->next != NULL) {

      /* SET CURRENT DR TO NEXT DR */
      cur_dr = cur_dr->next;

      /* CONCATENATE CURRENT DR TAG TO STRING */
      strncat(string,cur_dr->tag,(size_t)cur_fm->rl_hd->s_tag);

      /* CONVERT CURRENT DR FIELD LENGTH TO CHARCTER STRING */
      sprintf(tmp_str,"%0*ld",(int)cur_fm->rl_hd->s_fdlen,cur_dr->fd_len);

      /* CONCATENATE CURRENT DR FIELD LENGTH TO STRING */
      strcat(string,tmp_str);

      /* CONVERT CURRENT DR FIELD POSITION TO CHARCTER STRING */
      sprintf(tmp_str,"%0*ld",(int)cur_fm->rl_hd->s_fdpos,cur_dr->fd_pos);

      /* CONCATENATE CURRENT DR FIELD POSITION TO STRING */
      strcat(string,tmp_str);

   /* ENDWHILE */
   }

   /* CONCATENATE FIELD TERMINATOR TO STRING */
   strcat(string,FT_STR);

   /* RETURN SUCCESS */
   return(1);
}
