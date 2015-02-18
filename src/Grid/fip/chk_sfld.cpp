# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: CHK123SFLD 
** 
**    PURPOSE: TO GET DESCRIPTION OF LAST SUBFIELD READ OR WRITTEN 
** 
**    INVOCATION METHOD: CHK123SFLD(FP,TAG,DESCR,FRMT) 
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION 
**     FP            PTR        I       FILE POINTER 
**     TAG[]         PTR        I       TAG 
**     DESCR[]       PTR        O       OUTPUT LABEL/DESCRIPTION OF SUBFIELD
**     FRMT[]        PTR        O       FORMAT 
**     CHK123SFLD()  LOGICAL    O       SUCCESS FLAG 
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER FOR AN
**                       INPUT FILE POINTER
**     I123TOA()        CONVERTS AN INTEGER TO A CHARACTER STRING
**     RET123MATCH()    RETRIEVES APPROPRIATE DD ENTRY FOR A FIELD
** 
**    INTERNAL VARIABLES: 
**     NAME          TYPE               DESCRIPTION
**     FIRST         LOGICAL            FIRST TIME IN LABEL LOOP FLAG
**     INDEX         INT                INDEX INTO FORMAT CHARACTER STRING
**     P_STATE       INT                PREVIOUS DR SUBFIELD STATE
**     W_STR[10]     CHAR               CHARACTER STRING CONTAINING FORMAT
**                                       WIDTH  
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
**      FMT_RT          PTR     I    ROOT POINTER TO FORMAT CONTROLS BINARY 
**                                    TREE STRUCTURE 
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE  
**                                    STRUCTURE 
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR     I    HEAD POINTER TO DIMENSION LENGTHS
**                                    (NO LABELS)
**      VALUES          PTR     I    HEAD POINTER TO DATA VALUE SUBFIELD
**                                    RECORDS
**      NEXT            PTR    N/A   POINTER TO NEXT DATA RECORD 
**
**     FORMAT CONTROLS STRUCTURE (FC)
**      NAME            TYPE   USE   DESCRIPTION
**      REP_NO          INT    N/A   NUMBER OF REPETITIONS
**      D_TYPE          CHAR    I    DATA TYPE - (A,I,R,S,C,B,X)
**      WIDTH           INT     I    FIELD WIDTH SPECIFICATION
**      U_DELIM         CHAR    I    USER DELIMITER
**      LEFT            PTR    N/A   LEFT POINTER TO FORMAT CONTROLS STRUCTURE
**                                    INDICATES REPETITION
**      RIGHT           PTR    N/A   RIGHT POINTER TO FORMAT CONTROLS STRUCTURE
**                                    INDICATES SAME LEVEL
**      BACK            PTR    N/A   BACK POINTER TO FORMAT CONTROLS STRUCTURE
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
**      LP_HD           PTR     I    HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER 
**                                    STRUCTURE 
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK 
**                                    STRUCTURE 
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE ENTRY 
**      CUR_DM          PTR     I    CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY 
**      CUR_DR          PTR     I    CURRENT POINTER TO DATA RECORD STRUCTURE 
**                                    ENTRY 
**      CUR_DV          PTR     I    CURRENT POINTER TO DR DATA VALUE SUBFIELD 
**                                    STRUCTURE ENTRY 
**      ROW_DVH         PTR    N/A   CURRENT POINTER TO NEXT SET OF VALUES 
**                                    IN DR DATA VALUE SUBFIELD STRUCTURE ENTRY 
**      CUR_FC          PTR     I    CURRENT POINTER TO FORMAT CONTROLS 
**                                    STRUCTURE ENTRY 
**      CUR_LP          PTR    I/O   CURRENT POINTER TO LABELS POINTER
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
**      SF_STATE_DR     INT     I    SUBFIELD STATE (DR) 
**                                    1 - NUMBER OF DIMENSIONS SUBFIELD 
**                                    2 - LENGTH OF A DIMENSION SUBFIELD 
**                                    3 - DATA VALUE STRING SUBFIELD 
**                                    4 - FINISHED 
**      NEXT            PTR    N/A   POINTER TO NEXT FILE MANAGEMENT 
**                                    STRUCTURE 
**      BACK            PTR    N/A   POINTER TO PREVIOUS FILE MANAGEMENT 
**                                    STRUCTURE 
** 
**     LABELS POINTER STRUCTURE (LP) 
**      NAME            TYPE   USE   DESCRIPTION 
**      NEXT            PTR     I    POINTER TO NEXT LP ENTRY 
**      FIRST           PTR    N/A   FIRST LABEL (SL) IN THE LABEL SET 
**      CUR             PTR     I    CURRENT LABEL (SL) IN THE LABEL SET 
** 
**     DD LABEL SUBFIELD STRUCTURE (SL) 
**      NAME            TYPE   USE   DESCRIPTION 
**      LABEL[]         PTR     I    LABEL 
**      NXT_LAB         PTR    N/A   POINTER TO NEXT LABEL IN SET 
**      NXT_LSET        PTR    N/A   POINTER TO NEXT SET OF LABELS 
** 
**    GLOBAL VARIABLES: 
**     NAME             TYPE   USE   DESCRIPTION 
**     CUR_FM           PTR    I/O   POINTER TO CURRENT FILE MANAGEMENT
**                                    STRUCTURE ENTRY 
** 
**    GLOBAL CONSTANTS: 
**     NAME             TYPE         DESCRIPTION
**     NC               CHAR         NULL CHARACTER
**     VDL_STR[]        CHAR         VECTOR DATA ELEMENT DELIMITER ! 2/1
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/10/90  INITIAL PROLOG 
**     P. HODGES                 05/10/90  INITIAL PDL 
**     P. HODGES                 06/13/90  INITIAL CODE 
**     L. MCMILLION              11/16/90  ADDED LOGIC TO DETERMINE PREVIOUS
**                                          DR SUBFIELD STATE AND RETURN THE
**                                          WIDTH AND DELIMITER IN THE FORMAT
**     L. MCMILLION  TASK #40    12/20/92  UPDATED PROLOG/PDL PER QC
**     J. TAYLOR     93DR032     05/26/93  SET PREVIOUS STATE TO THE CURRENT
**                                          SF_STATE_DR IF DATA VALUE EXIST FOR
**                                          CARTESIAN DATA FIELD
**     J. TAYLOR     93DR032     05/26/93  SET OUTPUT DESCRIPTION AND FORMAT 
**                                          TO NC
** 
**    PDL: 
**
**     INITIALIZE FIRST TO TRUE
**     INITIALIZE PREVIOUS DR SUBFIELD STATE TO OUT OF BOUNDS
**     SET OUTPUT DESCRIPTION TO NULL CHARACTER
**     SET OUTPUT FORMAT TO NULL CHARACTER
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE FM LAYER
**     CALL RET123MATCH() TO RETRIEVE APPROPRIATE DD ENTRY FOR THIS FIELD
**
**     { DETERMINE PREVIOUS DR SUBFIELD STATE }
**     IF STATE IS LENGTH OF DIMENSION SUBFIELD
**     THEN
**        IF CUR_DR DIMENSION LENGTH POINTER IS CUR_DM
**        THEN
**           SET PREVIOUS STATE TO NUMBER OF DIMENSIONS SUBFIELD
**        ELSE
**           SET PREVIOUS STATE TO STATE
**        ENDIF
**     ELSEIF STATE IS DATA VALUE STRING SUBFIELD
**     THEN
**        IF CUR_DR DIMENSION LENGTH POINTER NULL
**        THEN
**           SET PREVIOUS STATE TO STATE
**        ELSE
**           IF CUR_DV IS CUR_DR VALUES OR CUR_DV IS NULL
**           THEN
**              SET PREVIOUS STATE TO DIMENSION LENGTH POINTER SUBFIELD
**           ELSE IF DATA VALUES DO EXIST FOR THIS CARTESIAN DATA FIELD
**              SET PREVIOUS STATE TO CURRENT FM SUBFIELD STATE {3 - DATA VALUE}
**           ENDIF
**        ENDIF
**     ELSE STATE IS OUT OF BOUNDS
**        SET PREVIOUS STATE TO DATA VALUE STRING SUBFIELD
**     ENDIF 
**       
**     IF PREVIOUS STATE IS NUMBER OF DIMENSIONS SUBFIELD 
**     THEN 
**        SET OUTPUT DESCRIPTION TO "NUMBER OF DIMENSIONS" 
**        SET FRMT TO "I"
**     ELSEIF PREVIOUS STATE IS LENGTH OF DIMENSION SUBFIELD 
**     THEN 
**       SET OUTPUT DESCRIPTION TO "DIMENSION LENGTH" 
**       SET FRMT TO "I"
**     ELSEIF PREVIOUS STATE IS DATA VALUE STRING SUBFIELD 
**     THEN
**        INITIALIZE INDEX TO ZERO
**        IF ROOT POINTER TO FORMAT CONTROL IS NOT NULL 
**        THEN 
**           SET FORMAT STRING TO DATA TYPE
**           INCREMENT INDEX
**           IF USER DELIMITER IS NOT NULL CHARACTER OR WIDTH IS NOT ZERO
**           THEN
**              APPEND LEFT PARENTHESIS TO FORMAT STRING
**              INCREMENT INDEX
**              IF USER DELIMITER IS NOT NULL CHARACTER
**              THEN
**                 APPEND USER DELIMITER TO FORMAT STRING
**                 INCREMENT INDEX
**              ELSE
**                 CONVERT WIDTH TO CHARACTER FORMAT
**                 APPEND NULL CHARACTER TO FORMAT STRING
**                 CONCATENATE WIDTH TO FORMAT STRING
**                 SET INDEX TO LENGTH OF FORMAT STRING
**              ENDIF
**              APPEND RIGHT PARENTHESIS TO FORMAT STRING
**              INCREMENT INDEX
**           ENDIF
**        ENDIF
**        APPEND NULL CHARACTER TO FORMAT STRING
**        SET OUTPUT DESCRIPTION TO NULL CHARACTER
**        IF LABELS NOT NULL 
**        THEN 
**           SET CUR_LP TO LP_HD 
**           WHILE CUR_LP NEXT IS NOT NULL DO
**              SET CUR_LP TO CUR_LP NEXT 
**              IF FIRST ITERATION OF LABEL LOOP
**              THEN
**                 RESET FIRST TO FALSE
**              ELSE
**                 CONCATENATE VDL_STR TO DESCRIPTION
**              ENDIF
**              IF CURRENT LABEL NOT NULL
**              THEN
**                 CONCATENATE CURRENT LABEL TO DESCRIPTION
**              ENDIF
**           ENDWHILE 
**        ENDIF 
**     ENDIF 
** 
**     RETURN SUCCESS 
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int chk123sfld(FILE *fp,char *tag,char *descr,char *frmt)

{
   /* INTERNAL VARIABLES */
   char w_str[10];
   int first = 1;
   int p_state = 4;
   long index;

   /* SET OUTPUT DESCRIPTION TO NULL CHARACTER */
   *descr = NC;

   /* SET OUTPUT FORMAT TO NULL CHARACTER */
   *frmt = NC;

   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE FM LAYER */
   if (!get123level(fp)) return (0);

   /* CALL RET123MATCH() TO RETRIEVE APPROPRIATE DD ENTRY FOR THIS FIELD */
   if (!ret123match(tag)) return (0);

   /* { DETERMINE PREVIOUS DR SUBFIELD STATE } */
   
   /* IF STATE IS LENGTH OF DIMENSION SUBFIELD */
   if (cur_fm->sf_state_dr == 2) {

      /* IF CUR_DR DIMENSION LENGTH POINTER IS CUR_DM */
      if (cur_fm->cur_dr->dim_lptr == cur_fm->cur_dm) {

         /* SET PREVIOUS STATE TO NUMBER OF DIMENSIONS SUBFIELD */
         p_state = 1;
      }
      else {
  
         /* SET PREVIOUS STATE TO STATE */
         p_state = cur_fm->sf_state_dr;
      }          
   }
   
   /* ELSEIF STATE IS DATA VALUE STRING SUBFIELD */
   else if (cur_fm->sf_state_dr == 3) {

      /* IF CUR_DR DIMENSION LENGTH POINTER NULL */
      if (cur_fm->cur_dr->dim_lptr == NULL) {
      
         /* SET PREVIOUS STATE TO STATE */
         p_state = cur_fm->sf_state_dr;
      }   
      else {
      
         /* IF CUR_DV IS CUR_DR VALUES OR CUR_DV IS NULL */
         if (cur_fm->cur_dv == cur_fm->cur_dr->values || cur_fm->cur_dv == NULL) {

            /* SET PREVIOUS STATE TO DIMENSION LENGTH POINTER SUBFIELD */
            p_state = 2;
         }

         /* ELSE IF DATA VALUES DO EXIST FOR THIS CARTESIAN DATA FIELD */
         else {

            /* SET PREVIOUS STATE TO CURRENT FM SUBFIELD STATE {3 - DATA VALUE} */
            p_state = cur_fm->sf_state_dr;
         }
      }
   }

   /* ELSE STATE IS OUT OF BOUNDS */
   else {

      /* SET PREVIOUS STATE TO DATA VALUE STRING SUBFIELD */
      p_state = 3;
   }

   /* IF PREVIOUS STATE IS NUMBER OF DIMENSIONS SUBFIELD */
   if (p_state == 1) {   
 
      /* SET OUTPUT DESCRIPTION TO "NUMBER OF DIMENSIONS" */
      strcpy(descr,"NUMBER OF DIMENSIONS");

      /* SET FRMT TO "I" */
      strcpy (frmt,"I");
   }

   /* ELSEIF PREVIOUS STATE IS LENGTH OF DIMENSION SUBFIELD */
   else if (p_state == 2) {

      /* SET OUTPUT DESCRIPTION TO "DIMENSION LENGTH" */
      strcpy (descr,"DIMENSION LENGTH");

      /* SET FRMT TO "I" */
      strcpy (frmt, "I");
   }

   /* ELSEIF PREVIOUS STATE IS DATA VALUE STRING SUBFIELD */
   else if (p_state == 3) {

      /* INITIALIZE INDEX TO ZERO */
      index = 0;   

      /* IF ROOT POINTER TO FORMAT CONTROL IS NOT NULL */ 
      if (cur_fm->cur_dd->fmt_rt != NULL) {

         /* SET FORMAT STRING TO DATA TYPE AND INCREMENT INDEX */ 
         frmt[index++] = cur_fm->cur_fc->d_type;

         /* IF USER DELIMITER IS NOT NULL CHARACTER OR WIDTH IS NOT ZERO */
         if (cur_fm->cur_fc->u_delim != NC || cur_fm->cur_fc->width !=0) {

            /* APPEND LEFT PARENTHESIS TO FORMAT STRING AND INCREMENT INDEX */
            frmt[index++] = '(';
            
            /* IF USER DELIMITER IS NOT NULL CHARACTER */
            if (cur_fm->cur_fc->u_delim != NC) {
            
               /* APPEND USER DELIMITER TO FORMAT STRING AND INCREMENT INDEX */
               frmt[index++] = cur_fm->cur_fc->u_delim;
            }
            
            /* USER DELIMITER IS NULL CHARACTER */
            else {
            
               /* CONVERT WIDTH TO CHARACTER FORMAT */
               i123toa(cur_fm->cur_fc->width,w_str);
               
               /* APPEND NULL CHARACTER TO FORMAT STRING */
               frmt[index] = NC;
               
               /* CONCATENATE WIDTH TO FORMAT STRING */
               strcat(frmt,w_str);
            
               /* SET INDEX TO LENGTH OF FORMAT STRING */
               index = _tcslen(frmt);
            }

            /* APPEND RIGHT PARENTHESIS TO FORMAT STRING AND INCREMENT INDEX */
            frmt[index++] = ')';
         }
      }
         
      /* APPEND NULL CHARACTER TO FORMAT STRING */
      frmt[index] = NC;

      /* SET OUTPUT DESCRIPTION TO NULL CHARACTER */
      *descr = NC;

      /* IF LABELS NOT NULL */
      if (cur_fm->cur_dd->labels != NULL) {

         /* SET CUR_LP TO LP_HD */
         cur_fm->cur_lp = cur_fm->lp_hd;

         /* WHILE CUR_LP NEXT IS NOT NULL DO */
         while (cur_fm->cur_lp->next != NULL) {

            /* SET CUR_LP TO CUR_LP NEXT */
            cur_fm->cur_lp = cur_fm->cur_lp->next;

            /* IF FIRST ITERATION OF LABEL LOOP */
            if (first) {

               /* RESET FIRST TO FALSE */
               first = 0;
            }
            else {
               
               /* CONCATENATE VDL_STR TO DESCRIPTION */
               strcat(descr,VDL_STR);
            }
            
            /* IF CURRENT LABEL NOT NULL */
            if (cur_fm->cur_lp->cur->label != NULL) {
            
               /* CONCATENATE CURRENT LABEL TO DESCRIPTION */
               strcat(descr,cur_fm->cur_lp->cur->label);
            }   
         }
      } 
   }    
 
   /* RETURN SUCCESS */
   return (1);
} 
