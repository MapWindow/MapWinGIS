# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RET123DDLEAD
**
**    PURPOSE: TO RETURN THE DATA DESCRIPTIVE LEADER INFORMATION
**
**    INVOCATION METHOD: RET123DDLEAD(STRING)
**
**    ARGUMENT LIST:
**     NAME            TYPE      USE     DESCRIPTION
**     RET123DDLEAD()  LOGICAL    O      SUCCESS FLAG
**     STRING[]        CHAR       O      STRING CONTAINING DATA DESCRIPTIVE
**                                        LEADER INFORMATION
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME        TYPE               DESCRIPTION
**     TMP_STR[25] CHAR               TEMPORARY CHARACTER STRING
**  
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT     O    DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT     O    INTERCHANGE LEVEL
**      LEAD_ID         CHAR    O    LEADER IDENTIFIER 
**      ICE_IND         CHAR    O    INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR    O    RESERVE SPACE CHARACTER
**      APPL_IND        CHAR    O    APPLICATION INDICATOR
**      FD_CNTRL_L      INT     O    FIELD CONTROL LENGTH
**      DA_BASE         INT     O    BASE ADDRESS OF DESCRIPTIVE AREA 
**      CCS_IND[4]      CHAR    O    CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT     O    SIZE OF FIELD LENGTH 
**      S_FDPOS         INT     O    SIZE OF FIELD POSITION  
**      S_RESV          INT     O    RESERVED DIGIT
**      S_TAG           INT     O    SIZE OF FIELD TAG
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
**     CUR_FM           PTR    I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION
**     NC               CHAR         NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     J. TAYLOR     TASK 55     01/07/93  INITIAL PROLOG
**     J. TAYLOR     TASK 55     01/07/93  INITIAL PDL
**     J. TAYLOR     TASK 55     01/07/93  INITIAL CODE
**
**    PDL:
**
**     INTIALIZE STRING TO NC
**     CONVERT RECORD LENGTH TO CHARACTER STRING
**     COPY RECORD LENGTH CHARACTER STRING TO STRING
**     CONVERT INTERCHANGE LEVEL TO CHARACTER STRING
**     CONCATENATE INTERCHANGE LEVEL CHARACTER STRING TO STRING
**     CONCATENATE LEADER ID TO STRING
**     CONCATENATE INLINE CODE EXTENTION INDICATOR TO STRING
**     CONCATENATE RESERVED SPACE TO STRING
**     CONCATENATE APPLICATION INDICATOR TO STRING
**     CONVERT FIELD CONTROL LENGTH TO CHARACTER STRING
**     CONCATENATE FIELD CONTROL LENGTH CHARACTER STRING TO STRING
**     CONVERT BASE ADDRESS TO CHARACTER STRING
**     CONCATENATE BASE ADDRESS CHARACTER STRING TO STRING
**     CONCATENATE EXTENDED CHARACTER SET INDICATOR TO STRING
**     CONVERT SIZE OF FIELD LENGTH TO CHARACTER STRING
**     CONCATENATE SIZE OF FIELD LENGTH CHARACTER STRING TO STRING
**     CONVERT SIZE OF FIELD POSITION TO CHARACTER STRING
**     CONCATENATE SIZE OF FIELD POSITION CHARACTER STRING TO STRING
**     CONVERT RESERVED DIGIT TAG TO CHARACTER STRING
**     CONCATENATE RESERVED DIGIT CHARACTER STRING TO STRING
**     CONVERT SIZE OF FIELD TAG TO CHARACTER STRING
**     CONCATENATE SIZE OF FIELD TAG CHARACTER STRING TO STRING
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int ret123ddlead(char *string)
{
   /* INTERNAL VARIABLES */
   char tmp_str[25]; 

   /* INTIALIZE STRING TO NC */
   *string = NC;

   /* CONVERT RECORD LENGTH TO CHARACTER STRING */
   sprintf(tmp_str,"%05ld",cur_fm->dl_hd->rec_len);

   /* COPY RECORD LENGTH CHARACTER STRING TO STRING */
   strcpy(string,tmp_str);

   /* CONVERT INTERCHANGE LEVEL TO CHARACTER STRING */
   sprintf(tmp_str,"%1ld",cur_fm->dl_hd->ilevel);

   /* CONCATENATE INTERCHANGE LEVEL CHARACTER STRING TO STRING */
   strcat(string,tmp_str);

   /* CONCATENATE LEADER ID TO STRING */
   strncat(string,&cur_fm->dl_hd->lead_id,1);

   /* CONCATENATE INLINE CODE EXTENTION INDICATOR TO STRING */
   strncat(string,&cur_fm->dl_hd->ice_ind,1);

   /* CONCATENATE RESERVED SPACE TO STRING */
   strncat(string,&cur_fm->dl_hd->resv_sp,1);

   /* CONCATENATE APPLICATION INDICATOR TO STRING */
   strncat(string,&cur_fm->dl_hd->appl_ind,1);

   /* CONVERT FIELD CONTROL LENGTH TO CHARACTER STRING */
   sprintf(tmp_str,"%02ld",cur_fm->dl_hd->fd_cntrl_l);

   /* CONCATENATE FIELD CONTROL LENGTH CHARACTER STRING TO STRING */
   strcat(string,tmp_str);

   /* CONVERT BASE ADDRESS TO CHARACTER STRING */
   sprintf(tmp_str,"%05ld",cur_fm->dl_hd->da_base);

   /* CONCATENATE BASE ADDRESS CHARACTER STRING TO STRING */
   strcat(string,tmp_str);

   /* CONCATENATE EXTENDED CHARACTER SET INDICATOR TO STRING */
   strncat(string,cur_fm->dl_hd->ccs_ind,3);

   /* CONVERT SIZE OF FIELD LENGTH TO CHARACTER STRING */
   sprintf(tmp_str,"%1ld",cur_fm->dl_hd->s_fdlen);

   /* CONCATENATE SIZE OF FIELD LENGTH CHARACTER STRING TO STRING */
   strcat(string,tmp_str);

   /* CONVERT SIZE OF FIELD POSITION TO CHARACTER STRING */
   sprintf(tmp_str,"%1ld",cur_fm->dl_hd->s_fdpos);

   /* CONCATENATE SIZE OF FIELD POSITION CHARACTER STRING TO STRING */
   strcat(string,tmp_str);

   /* CONVERT RESERVED DIGIT TAG TO CHARACTER STRING */
   sprintf(tmp_str,"%1ld",cur_fm->dl_hd->s_resv);

   /* CONCATENATE RESERVED DIGIT CHARACTER STRING TO STRING */
   strcat(string,tmp_str);

   /* CONVERT SIZE OF FIELD TAG TO CHARACTER STRING */
   sprintf(tmp_str,"%1ld",cur_fm->dl_hd->s_tag);

   /* CONCATENATE SIZE OF FIELD TAG CHARACTER STRING TO STRING */
   strcat(string,tmp_str);


   /* RETURN SUCCESS */
   return(1);
}
