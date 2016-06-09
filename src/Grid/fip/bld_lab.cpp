# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: BLD123LAB
**
**    PURPOSE: TO BUILD A LABEL STRING FROM THE DDR LABEL SUBFIELD STRUCTURE
**
**    INVOCATION METHOD: BLD123LAB(STRING)
**
**    ARGUMENT LIST:
**     NAME         TYPE      USE      DESCRIPTION
**     STRING[]     PTR        O       CONCATENATED LABEL STRING
**     BLD123LAB()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     CVL_PTR    PTR                POINTER TO LABELS IN LABEL SUBFIELD
**                                    STRUCTURE
**     CVS_PTR    PTR                POINTER TO LABEL SETS IN LABEL SUBFIELD
**                                    STRUCTURE
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
**      LABELS          PTR     I    HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
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
**     DD LABEL SUBFIELD STRUCTURE (SL)
**      NAME            TYPE   USE   DESCRIPTION
**      LABEL[]         PTR     I    LABEL
**      NXT_LAB         PTR     I    POINTER TO NEXT LABEL IN SET
**      NXT_LSET        PTR     I    POINTER TO NEXT SET OF LABELS
**
**    GLOBAL VARIABLES:
**     NAME              TYPE   USE   DESCRIPTION
**     CUR_FM            PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                     STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS:
**     NAME       TYPE      USE      DESCRIPTION
**     CDL_STR[]  CHAR       I       CARTESIAN VECTOR LABEL DELIMITER * 2/10
**     VDL_STR[]  CHAR       I       VECTOR DATA ELEMENT DELIMITER ! 2/1
**     NC         CHAR       I       NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     L. MCMILLION              05/10/90  INITIAL PROLOG
**     A. DEWITT                 05/10/90  INITIAL PDL
**     P. HODGES                 06/05/90  INITIAL CODE
**     L. MCMILION   TASK #40    12/20/92  UPDATED PROLOG/PDL/INLINE CODE
**                                          PER QC
**
**    PDL:
**
**     INITIALIZE OUTPUT STRING TO NULL
**     IF CUR_DD LABELS NOT NULL
**     THEN
**        SET CVS_PTR TO DUMMY NEXT LABEL SET
**        WHILE CVS_PTR NOT NULL DO
**           SET CVL_PTR TO CVS_PTR
**           WHILE CVL_PTR NOT NULL DO
**              IF LABEL PRESENT (CVL_PTR)
**              THEN
**                 CONCATENATE LABEL STRING TO OUTPUT STRING
**              ENDIF
**              SET CVL_PTR TO NEXT LABEL
**              IF CVL_PTR NOT NULL
**              THEN
**                 CONCATENATE VDL STRING TO OUTPUT STRING
**              ELSE
**                 SET CVS_PTR TO CVS_PTR NEXT LABEL SET
**                 IF CVS_PTR NOT NULL
**                 THEN
**                    CONCATENATE CDL STRING TO OUTPUT STRING
**                 ENDIF
**              ENDIF
**           ENDWHILE
**        ENDWHILE   
**     ENDIF
**
**     RETURN SUCCESS
**           
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int bld123lab(char *string)

{
   /* INTERNAL VARIABLES */
   struct sl *cvl_ptr;
   struct sl *cvs_ptr;

   /* INITIALIZE OUTPUT STRING TO NULL */
   *string = NC;

   /*IF CUR_DD LABELS NOT NULL */
   if (cur_fm->cur_dd->labels != NULL) {

      /* SET CVS_PTR TO DUMMY NEXT LABEL SET*/
      cvs_ptr = cur_fm->cur_dd->labels->nxt_lset;

      /* WHILE CVS_PTR NOT NULL DO */
      while(cvs_ptr != NULL) {

         /* SET CVL_PTR TO CVS_PTR */
         cvl_ptr = cvs_ptr;
 
         /* WHILE CVL_PTR NOT NULL DO */
         while(cvl_ptr != NULL) {

            /* IF LABEL PRESENT (CVL_PTR) */
            if (cvl_ptr->label != NULL) {
         
               /* CONCATENATE LABEL STRING TO OUTPUT STRING */
               strcat(string,cvl_ptr->label);

            }
            /* SET CVL_PTR TO NEXT LABEL */
            cvl_ptr = cvl_ptr->nxt_lab;

            /* IF CVL_PTR NOT NULL */
            if (cvl_ptr != NULL) {

               /* CONCATENATE VDL STRING TO OUTPUT STRING */
               strncat(string,VDL_STR,1);
            }
            else {

               /* SET CVS_PTR TO CVS_PTR NEXT LABEL SET */
               cvs_ptr = cvs_ptr->nxt_lset;

               /* IF CVS_PTR NOT NULL */
               if (cvs_ptr != NULL) {

                  /* CONCATENATE CDL STRING TO OUTPUT STRING */
                  strncat(string,CDL_STR,1);
               }
            }

         }

      }

   }

   /* RETURN SUCCESS */
   return(1);
}
