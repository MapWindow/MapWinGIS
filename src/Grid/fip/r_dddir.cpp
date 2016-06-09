# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RET123DDDIR
**
**    PURPOSE: TO RETURN THE DIRECTORY INFORMATION FOR DATA DIRECTORY RECORD
**
**    INVOCATION METHOD: RET123DDDIR(STRING)
**
**    ARGUMENT LIST:
**     NAME           TYPE      USE    DESCRIPTION
**     RET123DDDIR()  LOGICAL    O     SUCCESS FLAG
**     STRING[]       CHAR       O     STRING CONTAINING DATA DESCRIPTIVE
**                                      DIRECTORY INFORMATION
**     
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME            TYPE            DESCRIPTION
**     CUR_DD          PTR             CURRENT DATA DESCRIPTIVE STRUCTURE
**     TMP_STR[25]     CHAR            TEMPORARY CHARACTER STRING
**
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
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
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT    N/A   DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT    N/A   INTERCHANGE LEVEL
**      LEAD_ID         CHAR   N/A   LEADER IDENTIFIER 
**      ICE_IND         CHAR   N/A   INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR   N/A   RESERVE SPACE CHARACTER
**      APPL_IND        CHAR   N/A   APPLICATION INDICATOR
**      FD_CNTRL_L      INT    N/A   FIELD CONTROL LENGTH
**      DA_BASE         INT    N/A   BASE ADDRESS OF DESCRIPTIVE AREA 
**      CCS_IND[4]      CHAR   N/A   CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT     I    SIZE OF FIELD LENGTH 
**      S_FDPOS         INT     I    SIZE OF FIELD POSITION  
**      S_RESV          INT    N/A   RESERVED DIGIT
**      S_TAG           INT     I    SIZE OF FIELD TAG 
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
**     SET LOCAL CURRENT DD TO DD HEADER 
**     WHILE NEXT DD NOT NULL
**        SET CURRENT DD TO NEXT DD
**        CONCATENATE CURRENT DD TAG TO STRING
**        CONVERT CURRENT DD FIELD LENGTH TO CHARCTER STRING
**        CONCATENATE CURRENT DD FIELD LENGTH TO STRING
**        CONVERT CURRENT DD FIELD POSITION TO CHARCTER STRING
**        CONCATENATE CURRENT DD FIELD POSITION TO STRING
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

int ret123dddir(char *string)

{
   /* INTERNAL VARIABLES */
   struct dd *cur_dd;
   char tmp_str[25];

   /* SET STRING TO NULL CHARACTER */
   *string = NC;

   /* SET LOCAL CURRENT DD TO DD HEADER  */
   cur_dd = cur_fm->dd_hd;

   /* WHILE NEXT DD NOT NULL */
   while (cur_dd->next != NULL) {

      /* SET CURRENT DD TO NEXT DD */
      cur_dd = cur_dd->next;

      /* CONCATENATE CURRENT DD TAG TO STRING */
      strncat(string,cur_dd->tag,(size_t)cur_fm->dl_hd->s_tag);

      /* CONVERT CURRENT DD FIELD LENGTH TO CHARCTER STRING */
      sprintf(tmp_str,"%0*ld",(int)cur_fm->dl_hd->s_fdlen,cur_dd->fd_len);

      /* CONCATENATE CURRENT DD FIELD LENGTH TO STRING */
      strcat(string,tmp_str);

      /* CONVERT CURRENT DD FIELD POSITION TO CHARCTER STRING */
      sprintf(tmp_str,"%0*ld",(int)cur_fm->dl_hd->s_fdpos,cur_dd->fd_pos);

      /* CONCATENATE CURRENT DD FIELD POSITION TO STRING */
      strcat(string,tmp_str);

   /* ENDWHILE */
   }

   /* CONCATENATE FIELD TERMINATOR TO STRING */
   strcat(string,FT_STR);

   /* RETURN SUCCESS */
   return(1);
}
