# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RD123DDFLD
**
**    PURPOSE: TO RETRIEVE A FIELD OF DATA DESCRIPTIVE RECORD
**
**    INVOCATION METHOD: RD123DDFLD(FP,TAG,RD_STR,STATUS)
**
**    ARGUMENT LIST:
**     NAME          TYPE      USE      DESCRIPTION
**     FP            PTR        I       FILE POINTER
**     TAG[]         PTR        O       TAG
**     RD_STR[]      PTR        O       STRING CONSISTING OF CONCATENATED
**                                       SUBFIELD OF CURRENT DDR FIELD
**     STATUS        INT        O       STATUS
**                                       0 = FAILURE
**                                       1 = OKAY
**                                       2 = START OF RECORD
**                                       3 = END OF RECORD
**                                       4 = END OF FILE
**                                       5 = END OF FIELD
**                                       6 = START OF FIELD
**     RD123DDFLD()  LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     IS123INTGR()     DETERMINES IF INPUT STRING IS INTEGER VALUE
**     RD123DDSFLD()    RETRIEVES DESCRIPTIVE RECORD SUBFIELD
**
**    INTERNAL VARIABLES:
**     NAME        TYPE     DESCRIPTION
**     INT_TAG     INT      TAG IN INTEGER FORMAT
**
**    GLOBAL REFERENCES: NONE
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT    N/A   DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT     I    INTERCHANGE LEVEL
**      LEAD_ID         CHAR   N/A   LEADER IDENTIFIER 
**      ICE_IND         CHAR   N/A   INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR   N/A   RESERVE SPACE CHARACTER
**      APPL_IND        CHAR   N/A   APPLICATION INDICATOR
**      FD_CNTRL_L      INT    N/A   FIELD CONTROL LENGTH
**      DA_BASE         INT    N/A   BASE ADDRESS OF DESCRIPTIVE AREA 
**      CCS_IND[4]      CHAR   N/A   CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH 
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION  
**      S_RESV          INT    N/A   RESERVED DIGIT
**      S_TAG           INT    N/A   SIZE OF FIELD TAG 
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
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**     GLB_STR[MAXSIZ]  CHAR   I/O   GLOBAL CHARACTER STRING USED FOR
**                                    PROCESSING
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION
**     FT_STR[2]        CHAR         CHARACTER STRING CONTAINING THE FIELD
**                                    TERMINATOR
**     UT_STR[2]        CHAR         CHARACTER STRING CONTAINING THE UNIT
**                                    TERMINATOR
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/09/90  INITIAL PROLOG
**     P. HODGES                 05/09/90  INITIAL PDL
**     P. HODGES                 05/31/90  INITIAL CODE
**     L. MCMILLION              08/29/90  MODIFIED LOGIC TO INVOKE
**                                          RD123DDSFLD() ONLY ONCE FOR AN
**                                          INTERCHANGE LEVEL 1 FILE
**     L. MCMILLION              09/14/90  INSERTED CODE TO RETURN FIELD
**                                          CONTROL LENGTH REQUIRED BY
**                                          WR123DDFLD()
**     L. MCMILLION              10/31/90  REMOVED RETURNED FIELD CONTROL LENGTH
**                                          AS WR123DDFLD() WAS MODIFIED
**     L. MCMILLION  (TASK #40)  08/28/91  REMOVED PORTION OF ORIGINAL CODE
**                                          THAT RETRIEVED U_AFD FROM
**                                          STRUCTURES AS FUNCTION RD123DDSFLD
**                                          PERFORMS RETRIEVAL INITIALLY
**     L. MCMILLION  TASK #40    12/08/92  UPDATED PROLOG
**
**    PDL:
**
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER
**     {ENSURE THAT POINTERS ARE AT BEGINNING OF FIELD}
**     DO WHILE STATUS DOES NOT INDICATE BEGINNING OF FIELD, OR RECORD, OR
**      TAG DOES NOT EQUAL FILE DESCRIPTION { 0..2 } OR INTERCHANGE LEVEL
**      DOES NOT INDICATE LEVEL 1
**        CALL RD123DDSFLD() TO RETRIEVE SUBFIELD
**        IF RETURNED TAG IS INTEGER
**        THEN
**           CONVERT TAG TO INTEGER
**        ELSE
**           SET INTEGER TAG TO -1
**        ENDIF
**     ENDDO
**
**     IF TAG IS NOT FILE DESCRIPTION { 0..2 }
**     THEN
**        IF INTERCHANGE LEVEL IS 2 OR 3
**        THEN
**           CALL RD123DDSFLD() TO RETRIEVE SECOND SUBFIELD
**           SET OUTPUT STRING TO CONCATENATED VALUE OF FIRST AND SECOND
**            SUBFIELD {NO DELIMITER}
**
**           WHILE END OF FIELD, OR RECORD, OR FILE HAS NOT BEEN ENCOUNTERED DO
**              CALL RD123DDSFLD() TO RETRIEVE SUBFIELD
**              SET OUTPUT STRING TO OUTPUT STRING, UNIT TERMINATOR, AND
**               RETRIEVED SUBFIELD
**           ENDWHILE
**
**           SET STATUS TO RETURNED STATUS FROM RD123DDSFLD()
**           SET TAG TO RETURNED TAG FROM RD123DDSFLD()
**        ENDIF
**     ENDIF
**     CONCATENATE FIELD TERMINATOR TO OUTPUT STRING
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int rd123ddfld(FILE *fp,char *tag,char *rd_str,int *status)
{
   /* INTERNAL VARIABLES */
   long int_tag;

   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return(0);

   /* DO WHILE STATUS DOES NOT INDICATE BEGINNING OF FIELD, OR RECORD, OR
       TAG DOES NOT EQUAL FILE DESCRIPTION { 0..2 }, OR INTERCHANGE LEVEL
       DOES NOT INDICATE LEVEL 1
   */
   do {
      /* CALL RD123DDSFLD() TO RETRIEVE SUBFIELD */
      if (!rd123ddsfld(fp,tag,rd_str,status)) return(0);

      /* IF TAG IS INTEGER */
      if (is123intgr(tag)) {

         /* CONVERT TAG TO INTEGER */
         int_tag = atol(tag);
      }
      else {

         /* SET INTEGER TAG TO -1 */
         int_tag = -1;
      }

   } while(*status != 6 && *status != 2 && int_tag != 2 && cur_fm->dl_hd->ilevel != 1);

   /* IF TAG IS NOT FILE DESCRIPTION { 0..2 } */
   if (int_tag != 2) {

      /* IF INTERCHANGE LEVEL IS 2 OR 3 */
      if (cur_fm->dl_hd->ilevel > 1) {

         /* CALL RD123DDSFLD() TO RETRIEVE SECOND SUBFIELD */
         if (!rd123ddsfld(fp,tag,glb_str,status)) return(0);

         /* SET OUTPUT STRING TO CONCATENATED VALUE OF FIRST AND SECOND
             SUBFIELD {NO DELIMITER}
         */
         strcat(rd_str,glb_str);

         /* WHILE END OF FIELD, OR RECORD, OR FILE NOT  ENCOUNTERED DO */
         while(*status != 5 && *status != 3 && *status != 4) {

            /* CALL RD123DDSFLD() TO RETRIEVE SUBFIELD */
            if (!rd123ddsfld(fp,tag,glb_str,status)) return(0);

            /* SET OUTPUT STRING TO OUTPUT STRING, UNIT TERMINATOR, AND
                RETRIEVED SUBFIELD
            */
            strcat(rd_str,UT_STR);
            strcat(rd_str,glb_str);
         }
      }
   }

   /* CONCATENATE FIELD TERMINATOR TO OUTPUT STRING */
   strcat(rd_str,FT_STR);

   /* RETURN SUCCESS */
   return(1);
}
