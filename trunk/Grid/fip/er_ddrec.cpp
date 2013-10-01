# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: ER123DDREC
**
**    PURPOSE: TO ERASE THE LAST DATA DESCRIPTIVE RECORD WRITTEN
**
**    INVOCATION METHOD: ER123DDREC(FP,STATUS)
**
**    ARGUMENT LIST:
**     NAME          TYPE      USE      DESCRIPTION
**     FP            PTR        I       FILE POINTER
**     STATUS        INT        O       STATUS
**                                       0 = FAILURE
**                                       1 = OKAY
**                                       2 = START OF RECORD
**                                       3 = END OF RECORD
**                                       4 = END OF FILE
**                                       5 = END OF FIELD
**                                       6 = START OF FIELD
**     ER123DDREC()  LOGICAL    O       SUCCESS FLAG
** 
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     ER123DDFLD()     ERASES THE LAST DATA DESCRIPTIVE FIELD WRITTEN
**     GET123LEVEL()    RETRIEVES THE APPROPRIATE DATA STRUCTURE LAYER
**
**    INTERNAL VARIABLES:
**     NAME       TYPE     DESCRIPTION
**     TMP_DD     PTR      TEMPORARY POINTER TO STORE HEAD POINTER TO DD
**                          STRUCTURE
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
**      LABELS          PTR    N/A   HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR    I/O   POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR    N/A   FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR    I/O   HEAD POINTER TO DATA DESCRIPTIVE
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
**      CUR_DD          PTR    I/O   CURRENT POINTER TO DATA DESCRIPTIVE
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
**     NAME            TYPE   USE   DESCRIPTION
**     CUR_FM          PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/18/90  INITIAL PROLOG
**     P. HODGES                 05/18/90  INITIAL PDL
**     A. DEWITT                 05/28/90  INITIAL CODE
**     L. MCMILLION              10/25/90  INSERTED CODE TO TEMPORARILY STORE
**                                          DD STRUCTURE HEAD POINTER TO
**                                          PREVENT ERASURE
**     L. MCMILLION              01/21/91  ADDED STATUS TO ARGUMENT LIST AND
**                                          LOGIC TO SET STATUS ACCORDINGLY
**     L. MCMILLION  (TASK #40)  10/16/91  ADDED LOGIC TO AVOID DDR FIELD  
**                                          ERASURE LOOP IF STRUCTURES ARE
**                                          EMPTY (DD_HD IS NULL)
**     L. MCMILLION  TASK #40    12/01/92  PROLOG/PDL UPDATE
** 
**    PDL:
**
**     INITIALIZE STATUS TO FAILURE
**     CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LEVEL
**     IF DD_HD IS NOT NULL 
**     THEN
**        SET TMP_DD TO DD_HD
**        SET DD_HD TO DD_HD NEXT
**        SET TMP_DD NEXT TO NULL
**        WHILE DD_HD NOT NULL DO
**           SET CUR_DD TO DD_HD
**           SET DD_HD TO CUR_DD NEXT
**           CALL ER123DDFLD() TO ERASE CURRENT DD FIELD
**        ENDWHILE
**        SET DD_HD TO TMP_DD
**        FREE DD_HD
**     ENDIF
**     SET SUBFIELD STATE TO OUT OF BOUNDS
**     SET STATUS TO START OF RECORD
**     REWIND FILE
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int er123ddrec(FILE *fp,int *status)
{
   /* INTERNAL VARIABLES */
   struct dd *tmp_dd;

   /* INITIALIZE STATUS TO FAILURE */
   *status = 0;
   
   /* CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LEVEL */
   if (!get123level(fp)) return(0);

   /* IF DD_HD NOT NULL */
   if (cur_fm->dd_hd != NULL) {            

      /* SET TMP_DD TO DD_HD */
      tmp_dd = cur_fm->dd_hd;

      /* SET DD_HD TO DD_HD NEXT */
      cur_fm->dd_hd = cur_fm->dd_hd->next;

      /* SET TMP_DD NEXT TO NULL */
      tmp_dd->next = NULL;

      /* WHILE DD_HD NOT NULL DO */
      while(cur_fm->dd_hd != NULL) {

         /* SET CUR_DD TO DD_HD */
         cur_fm->cur_dd = cur_fm->dd_hd;

         /* SET DD_HD TO CUR_DD NEXT */
         cur_fm->dd_hd = cur_fm->cur_dd->next;

         /* CALL ER123DDFLD() TO ERASE CURRENT FIELD */
         if (!er123ddfld(fp,status)) return(0);
      }

      /* SET DD_HD TO TMP_DD */
      cur_fm->dd_hd = tmp_dd;

      /* FREE DD_HD */
      free(cur_fm->dd_hd);
      cur_fm->dd_hd = NULL;
   }
 
   /* SET SUBFIELD STATE TO OUT OF BOUNDS */
   cur_fm->sf_state_dd = 0;

   /* SET STATUS TO START OF RECORD */
   *status = 2;
   
   /* REWIND FILE */
   rewind(fp);

   /* RETURN SUCCESS */
   return(1);
}
