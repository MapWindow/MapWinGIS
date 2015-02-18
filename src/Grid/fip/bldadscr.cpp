# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: BLD123ADSCR
**
**    PURPOSE: TO BUILD THE DDR ARRAY DESCRIPTOR SUBFIELD FROM THE CURRENT
**              DATA DESCRIPTIVE RECORD (DD) STRUCTURE AND THE DIMENSION
**              LENGTH STRUCTURE LIST
**
**    INVOCATION METHOD: BLD123ADSCR(ARDESCR)
**
**    ARGUMENT LIST:
**     NAME            TYPE      USE      DESCRIPTION
**     ARDESCR[]       PTR        O       POINTER TO ARRAY DESCRIPTOR STRING
**     BLD123ADSCR()   LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     I123TOA()        CONVERTS AN INTEGER TO CHARACTER STRING
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     TMP_DM     PTR                TEMPORARY POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE
**     TMPSTR[10] CHAR               TEMPORARY CHARACTER STRING
**
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA 
**                                    FIELD RECORD
**      FD_CNTRL[10]    CHAR   N/A   FIELD CONTROLS
**      NAME[]          PTR    N/A   CHARACTER STRING POINTER TO NAME
**      NUM_DIM         INT     I    NUMBER OF DIMENSIONS IN ARRAY DESCRIPTOR
**      DIM_LPTR        PTR     I    HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT    N/A   NUMBER OF ELEMENTS IN PRIMARY DIMENSION
**      LABELS          PTR    N/A   HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
**
**     DIMENSION LENGTHS STRUCTURE (DM)
**      NAME            TYPE   USE   DESCRIPTION
**      LEN             INT     I    DIMENSION LENGTH
**      NXT             PTR     I    POINTER TO NEXT DIMENSION LENGTH    
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
**     ARYD_STR[2]      CHAR         CHARACTER STRING CONTAINING DDR ARRAY
**                                    DESCRIPTOR DELIMITER
**     NC               CHAR         NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     L. MCMILLION   93DR033    06/16/93  INITIAL PROLOG
**     L. MCMILLION   93DR033    06/16/93  INITIAL PDL
**     L. MCMILLION   93DR033    06/16/93  INITIAL CODE
**
**    PDL:
**
**     INITIALIZE ARRAY DESCRIPTOR TO EMPTY
**     IF CURRENT DDR FIELD CONTAINS ARRAY DESCRIPTOR
**     THEN
**        CALL I123TOA() TO CONVERT NUMBER OF DIMENSIONS TO CHARACTER STRING
**        CONCATENATE NUMBER OF DIMENSIONS TO ARRAY DESCRIPTOR 
**        SET TEMPORARY DIMENSION LENGTH POINTER TO FIRST LENGTH PAST DUMMY
**        WHILE TEMP DIMENSION LENGTH POINTER NOT NULL DO
**           CONCATENATE ARRAY DESCRIPTOR DELIMITER TO ARRAY DESCRIPTOR 
**           CALL I123TOA() TO CONVERT DIMENSION LENGTH TO CHARACTER STRING
**           CONCATENATE DIMENSION LENGTH TO ARRAY DESCRIPTOR 
**           SET TEMP DIMENSION LENGTH POINTER TO NEXT IN LIST
**        ENDWHILE
**     ENDIF
**
**     RETURN SUCCESS
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int bld123adscr(char *ardescr)

{
   /* INTERNAL VARIABLES */
   struct dm *tmp_dm;
   char tmpstr[10];

   /* INITIALIZE ARRAY DESCRIPTOR TO EMPTY */
   *ardescr = NC;

   /* IF CURRENT DDR FIELD CONTAINS ARRAY DESCRIPTOR */
   if (cur_fm->cur_dd->dim_lptr != NULL) {

      /* CALL I123TOA() TO CONVERT NUMBER OF DIMENSIONS TO CHARACTER STRING */
      if (!i123toa(cur_fm->cur_dd->num_dim, tmpstr)) return (0);

      /* CONCATENATE NUMBER OF DIMENSIONS TO ARRAY DESCRIPTOR */
      strcat(ardescr, tmpstr);

      /* SET TEMPORARY DIMENSION LENGTH POINTER TO FIRST LENGTH PAST DUMMY */
      tmp_dm = cur_fm->cur_dd->dim_lptr->nxt;

      /* WHILE TEMP DIMENSION LENGTH POINTER NOT NULL DO */
      while (tmp_dm != NULL) {

         /* CONCATENATE ARRAY DESCRIPTOR DELIMITER TO ARRAY DESCRIPTOR */
         strcat(ardescr, ARYD_STR);

         /* CALL I123TOA() TO CONVERT DIMENSION LENGTH TO CHARACTER STRING */
         if (!i123toa(tmp_dm->len, tmpstr)) return (0);
         
         /* CONCATENATE DIMENSION LENGTH TO ARRAY DESCRIPTOR */
         strcat(ardescr, tmpstr);

         /* SET TEMP DIMENSION LENGTH POINTER TO NEXT IN LIST */
         tmp_dm = tmp_dm->nxt;
      }
   }

   /* RETURN SUCCESS */
   return(1);
}
