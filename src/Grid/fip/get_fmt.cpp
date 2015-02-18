# include "stdafx.h"
/***************************************************************************
**
**     INVOCATION NAME: GET123FMT
**     
**     PURPOSE:  TO RETRIEVE A SINGLE FORMAT FROM FORMAT STRUCTURE
**
**     INVOCATION METHOD: GET123FMT(F_DTYP,F_WIDTH,F_DELIM)
**
**     ARGUMENT LIST: 
**      NAME         TYPE      USE        DESCRIPTION
**      F_DTYP       CHAR       O         DATA TYPE
**      F_WIDTH      INT        O         FIELD WIDTH
**      F_DELIM      CHAR       O         USER DELIMITER
**      GET123FMT()  LOGICAL    O         SUCCESS FLAG
**
**     EXTERNAL FUNCTION REFERENCES:
**      NAME             DESCRIPTION
**      PUSH123RS()      PUSH A REPETITION NUMBER ON STACK
**      POP123RS()       REMOVE A REPETITION ZERO VALUE FROM STACK
**
**     INTERNAL VARIABLES:
**      NAME            TYPE         DESCRIPTION
**      NUMREP          INT          NUMBER OF REPETITIONS
**      PREV_PTR        PTR          PREVIOUS POINTER TO FORMAT CONTROLS
**                                    STRUCTURE
**      REP_FOUND       LOGICAL      REPETITION FOUND FLAG
**      RET_PATH        LOGICAL      RETURN PATH FOUND FLAG
**      VAL_FD          LOGICAL      VALUE FOUND FLAG
**
**     GLOBAL REFERENCES:
**
**      FORMAT CONTROLS STRUCTURE (FC)
**       NAME            TYPE   USE   DESCRIPTION
**       REP_NO          INT     I    NUMBER OF REPETITIONS
**       D_TYPE          CHAR    I    DATA TYPE - (A,I,R,S,C,B,X)
**       WIDTH           INT     I    FIELD WIDTH SPECIFICATION
**       U_DELIM         CHAR    I    USER DELIMITER
**       LEFT            PTR     I    LEFT POINTER TO FORMAT CONTROLS STRUCTURE
**                                     INDICATES REPETITION
**       RIGHT           PTR     I    RIGHT POINTER TO FORMAT CONTROLS STRUCTURE
**                                     INDICATES SAME LEVEL
**       BACK            PTR     I    BACK POINTER TO FORMAT CONTROLS STRUCTURE
**
**      FILE MANAGEMENT STRUCTURE (FM)
**       NAME            TYPE   USE   DESCRIPTION
**       FP              PTR    N/A   FILE POINTER
**       F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**       OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**       CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                     CONTROL RECORD STRUCTURE
**       DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE
**                                     RECORD STRUCTURE
**       DL_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                     STRUCTURE
**       DR_HD           PTR    N/A   HEAD POINTER TO DATA RECORD STRUCTURE
**       LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**       RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER
**                                     STRUCTURE
**       RS_HD           PTR     I    HEAD POINTER TO FORMAT REPETITION STACK
**                                     STRUCTURE
**       REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**       CUR_DD          PTR    N/A   CURRENT POINTER TO DATA DESCRIPTIVE
**                                     RECORD STRUCTURE ENTRY
**       CUR_DM          PTR    N/A   CURRENT POINTER TO DIMENSION LENGTHS
**                                     STRUCTURE ENTRY
**       CUR_DR          PTR    N/A   CURRENT POINTER TO DATA RECORD STRUCTURE
**                                     ENTRY
**       CUR_DV          PTR    N/A   CURRENT POINTER TO DR DATA VALUE SUBFIELD
**                                     STRUCTURE ENTRY
**       ROW_DVH         PTR    N/A   CURRENT POINTER TO NEXT SET OF VALUES
**                                     IN DR DATA VALUE SUBFIELD STRUCTURE ENTRY
**       CUR_FC          PTR    I/O   CURRENT POINTER TO FORMAT CONTROLS
**                                     STRUCTURE ENTRY
**       CUR_LP          PTR    N/A   CURRENT POINTER TO LABELS POINTER
**                                     STRUCTURE ENTRY
**       CUR_SL          PTR    N/A   CURRENT POINTER TO DD-LABEL SUBFIELD
**                                     STRUCTURE ENTRY
**       CUR_FCR         PTR    I/O   CURRENT POINTER TO ROOT OF FORMAT
**                                     CONTROLS STRUCTURE
**       CUR_RP          PTR    N/A   CURRENT POINTER TO RECORD POINTER 
**                                     STRUCTURE
**       NLD_RP          PTR    N/A   POINTER TO RECORD POINTER STRUCTURE 
**                                    WHERE CORRESPONDING DR HAS AN 'R' 
**                                    LEADER IDENTIFIER    
**       SF_FMT          CHAR   N/A   FORMAT CORRESPONDING TO THE CURRENT
**                                     DATA VALUE
**       NLD             INT    N/A   NO LEADER FLAG
**                                     TRUE  - NO LEADER
**                                     FALSE - LEADER EXISTS
**       BIT_CNT         INT    N/A   COUNT OF BITS STORED IN MEMORY FOR 
**                                     A SUBSEQUENT READ FUNCTION
**       BIT_BIN         CHAR   N/A   BITS STORED IN MEMORY FOR SUBSEQUENT
**                                     READ FUNCTION
**       COMPRESS        INT    N/A   FLAG TO SPECIFY COMPRESSED OR
**                                     UNCOMPRESSED ADJACENT FIXED-LENGTH
**                                     BIT FIELD I/O
**                                     0 - UNCOMPRESSED
**                                     1 - COMPRESSED
**       SF_STATE_DD     INT    N/A   SUBFIELD STATE (DD)
**                                     1 - FIELD CONTROL SUBFIELD
**                                     2 - NAME SUBFIELD
**                                     3 - LABELS SUBFIELD
**                                     4 - FORMATS SUBFIELD
**                                     5 - FINISHED
**       SF_STATE_DR     INT    N/A   SUBFIELD STATE (DR)
**                                     1 - NUMBER OF DIMENSIONS SUBFIELD
**                                     2 - LENGTH OF A DIMENSION SUBFIELD
**                                     3 - DATA VALUE STRING SUBFIELD
**                                     4 - FINISHED
**       NEXT            PTR    N/A   POINTER TO NEXT FILE MANAGEMENT
**                                     STRUCTURE
**       BACK            PTR    N/A   POINTER TO PREVIOUS FILE MANAGEMENT
**                                     STRUCTURE
**
**      FORMAT REPETITION STACK STRUCTURE (RS)
**       NAME            TYPE   USE   DESCRIPTION
**       REP_NUM         INT     I    NUMBER OF REPEATS
**       NXT             PTR    N/A   POINTER TO NEXT STACK VALUE
**
**     GLOBAL VARIABLES:
**      NAME              TYPE  USE   DESCRIPTION
**      CUR_FM            PTR    I    CURRENT POINTER TO FILE MANAGEMENT
**                                     STRUCTURE ENTRY
**
**     GLOBAL CONSTANTS: NONE
** 
**     CHANGE HISTORY:
**      AUTHOR        CHANGE-ID     DATE    CHANGE SUMMARY
**      A. DEWITT                 04/27/90  INITIAL PROLOG
**      A. DEWITT                 04/27/90  INITIAL PDL
**      P. HODGES                 06/05/90  INITIAL CODE
**      A. DEWITT                 07/23/90  MODIFIED LOGIC TO PROCESS RIGHT
**                                           SUBTREE
**      L. MCMILLION  92DR005     06/04/92  ADDED LOGIC TO RETURN FAILURE
**                                           IF CUR_FC OR CUR_FCR NULL
**      J. TAYLOR     92DR014     10/14/92  MODIFIED TO CLEAR STACK REPETITION
**                                           STRUCTURE IF CURRENT FORMAT
**                                           CONTROL IS THE TRUE ROOT
**
**     PDL:
**
**      IF AT TRUE ROOT
**      THEN
**         CLEAN FORMAT REPETITION STACK
**      ENDIF
**      IF EITHER CUR_FC OR CUR_FCR IS NULL
**      THEN
**         RETURN FAILURE
**      ENDIF
**      SET VALUE FOUND TO FALSE
**      WHILE FORMAT VALUE NOT FOUND DO
**         IF RIGHT POINTER OF CUR_FC IS NOT NULL OR CUR_FC EQUALS CUR_FCR
**         THEN
**            IF CURRENT FORMAT NOT EQUAL TO CURRENT FORMAT CONTROL ROOT
**            THEN
**               SET CUR_FC TO RIGHT POINTER
**            ENDIF
**            WHILE LEFT POINTER OF CUR_FC IS NOT NULL DO
**               CALL PUSH123RS() TO PUSH REP_NO
**               SET CUR_FC TO LEFT POINTER
**            END WHILE
**            SET F_DTYP TO D_TYPE
**            SET F_WIDTH TO WIDTH
**            SET F_DELIM TO U_DELIM
**            SET VALUE FOUND TO TRUE
**         ELSE
**            SET RETURN PATH FOUND TO FALSE
**            WHILE HAVE NOT RETRIEVED A FORMAT PATH DO
**               SET REP_FOUND TO FALSE
**               WHILE NOT REP_FOUND AND CUR_FC IS NOT ROOT DO
**                  SET PREV_PTR TO CUR_FC
**                  SET CUR_FC TO IT'S BACK POINTER
**                  IF REP_NO OF CUR_FC IS GREATER THAN ZERO AND THE RIGHT
**                   POINTER OF CUR_FC IS NOT POINTING TO PREVIOUS POINTER
**                  THEN
**                     SET REP_FOUND TO TRUE
**                  ENDIF
**               END WHILE
**               IF REP_FOUND
**               THEN
**                  IF STACK NOT EMPTY
**                  THEN
**                     DECREMENT VALUE AT TOP OF STACK { RS_HD POINTS TO THIS }
**                     IF THE VALUE IS GREATER THAN ZERO
**                     THEN
**                        SET CURRENT FORMAT TO LEFT POINTER
**                        WHILE LEFT POINTER OF CUR_FC IS NOT NULL
**                           CALL PUSH123RS() TO PUSH REP_NO
**                           SET CUR_FC TO LEFT POINTER
**                        END WHILE
**                        SET F_DTYP TO D_TYPE
**                        SET F_WIDTH TO WIDTH
**                        SET F_DELIM TO U_DELIM
**                        SET RETURN PATH TO TRUE
**                        SET VAL_FD TO TRUE
**                     ELSE
**                        CALL POP123RS() TO POP ZERO VALUE OFF STACK
**                        IF CURRENT FORMAT HAS A RIGHT SUBTREE
**                        THEN
**                           SET RETURN PATH TO TRUE
**                        ENDIF
**                     ENDIF
**                  ELSE STACK IS EMPTY
**                     RETURN FAILURE
**                  ENDIF
**               ELSE { REPETITION NOT FOUND }
**                  IF BACK FIELD OF CUR_FC IS NULL { AT TRUE ROOT }
**                  THEN
**                     SET CUR_FC TO LEFT POINTER FIELD OF FORMAT ROOT
**                     WHILE RIGHT POINTER NOT NULL DO
**                        SET CUR_FC TO IT'S RIGHT POINTER
**                     END WHILE
**                     SET REP_FOUND TO FALSE
**                     WHILE NOT REP_FOUND AND CUR_FC NOT ROOT PTR DO
**                        IF LEFT CHILD OF CUR_FC EXISTS
**                        THEN
**                           IF REP_NO OF LEFT CHILD OF CUR_FC IS NON-ZERO
**                           THEN
**                              SET REP_FOUND TO TRUE
**                           ENDIF
**                        ENDIF
**                        IF REP_FOUND
**                        THEN
**                           SET CURRENT FORMAT CONTROL ROOT TO CURRENT FORMAT
**                        ELSE
**                           SET CUR_FC TO ITS BACK FIELD
**                        ENDIF
**                     END WHILE
**                  ENDIF
**                  SET RETURN PATH TO TRUE
**               ENDIF
**            END WHILE
**         ENDIF
**      END WHILE
**
**      RETURN SUCCESS
**
****************************************************************************
**      CODE SECTION
**
****************************************************************************/
#include "stc123.h"

int get123fmt(char *f_dtyp,long *f_width,char *f_delim)
{
   /* INTERNAL VARIABLES */
   long numrep;
   struct fc *prev_ptr;
   int rep_found;
   int val_fd;
   int ret_path;
   
   /* IF AT TRUE ROOT */
   if (cur_fm->cur_fc->back == NULL) 
   {
      /* CLEAN FORMAT REPETITION STACK */
      while (pop123rs(&numrep));
   }

   /* IF EITHER CUR_FC OR CUR_FCR IS NULL, RETURN FAILURE */
   if (cur_fm->cur_fc == NULL || cur_fm->cur_fcr == NULL) return(0);

   /* SET VALUE FOUND TO FALSE */
   val_fd = 0;
   
   /* WHILE FORMAT VALUE NOT FOUND DO */
   while (!val_fd) {
      
      /* IF RIGHT POINTER OF CUR_FC IS NOT NULL OR CUR_FC EQUALS CUR_FCR */   
      if (cur_fm->cur_fc->right != NULL || cur_fm->cur_fc == cur_fm->cur_fcr) {

         /* IF CURRENT FORMAT NOT EQUAL TO CURRENT FORMAT CONTROL ROOT */
         if (cur_fm->cur_fc != cur_fm->cur_fcr) {

            /* SET CUR_FC TO RIGHT POINTER */
            cur_fm->cur_fc = cur_fm->cur_fc->right;
         }

         /* WHILE LEFT POINTER OF CUR_FC IS NOT NULL DO */
         while(cur_fm->cur_fc->left != NULL) {

            /* CALL PUSH123RS() TO PUSH REP_NO MINUS ONE */
            if (!push123rs(cur_fm->cur_fc->rep_no)) return(0);
  
            /* SET CUR_FC TO LEFT POINTER */
            cur_fm->cur_fc = cur_fm->cur_fc->left;
         }

         /* SET F_DTYP TO D_TYPE */
         *f_dtyp = cur_fm->cur_fc->d_type;

         /* SET F_WIDTH TO WIDTH */
         *f_width = cur_fm->cur_fc->width;

         /* SET F_DELIM TO U_DELIM */
         *f_delim = cur_fm->cur_fc->u_delim;

         /* SET VALUE FOUND TO TRUE */
         val_fd = 1;
      
      }
      else {
   
         /* SET RETURN PATH FOUND TO FALSE */
         ret_path = 0;

         /* WHILE HAVE NOT RETRIEVED A FORMAT PATH DO */
         while(!ret_path) {

            /* SET REP_FOUND TO FALSE */
            rep_found = 0;

            /* WHILE NOT REP_FOUND AND CUR_FC IS NOT ROOT DO */
            while(!rep_found && cur_fm->cur_fc != cur_fm->cur_fcr) {

               /* SET PREV_PTR TO CUR_FC */
               prev_ptr = cur_fm->cur_fc;

               /* SET CUR_FC TO IT'S BACK POINTER */
               cur_fm->cur_fc = cur_fm->cur_fc->back;

               /* IF REP_NO OF CUR_FC IS GREATER THAN ZERO AND THE RIGHT 
                   POINTER OF CUR_FC IS NOT POINTING TO PREVIOUS POINTER
               */
               if (cur_fm->cur_fc->rep_no > 0 && cur_fm->cur_fc->right != prev_ptr) {

                  /* SET REP_FOUND TO TRUE */
                  rep_found = 1;
               }
            }

            /* IF REP_FOUND */
            if (rep_found) {

               /* IF STACK NOT EMPTY */
               if (cur_fm->rs_hd != NULL) {

                  /* DECREMENT VALUE AT TOP OF STACK { RS_HD POINTS TO THIS } */
                  cur_fm->rs_hd->rep_num--;

                  /* IF THE VALUE IS GREATER THAN ZERO */
                  if (cur_fm->rs_hd->rep_num > 0) {

                     /* SET CURRENT FORMAT TO LEFT POINTER */
                     cur_fm->cur_fc = cur_fm->cur_fc->left;
                  
                     /* WHILE LEFT POINTER OF CUR_FC IS NOT NULL */
                     while(cur_fm->cur_fc->left != NULL) {

                        /* CALL PUSH123RS() TO PUSH REP_NO MINUS ONE */
                        if (!push123rs(cur_fm->cur_fc->rep_no)) return(0);

                        /* SET CUR_FC TO LEFT POINTER */
                        cur_fm->cur_fc = cur_fm->cur_fc->left;

                     }

                     /* SET F_DTYP TO D_TYPE */
                     *f_dtyp = cur_fm->cur_fc->d_type;

                     /* SET F_WIDTH TO WIDTH */
                     *f_width = cur_fm->cur_fc->width;
 
                     /* SET F_DELIM TO U_DELIM */
                     *f_delim = cur_fm->cur_fc->u_delim;

                     /* SET RETURN PATH TO TRUE */
                     ret_path = 1;
                  
                     /* SET VAL_FD TO TRUE */
                     val_fd = 1;
                  }
                  else {

                     /* CALL POP123RS() TO POP ZERO VALUE OFF STACK */
                     if (!pop123rs(&numrep)) return(0);

                     /* IF CURRENT FORMAT HAS A RIGHT SUBTREE */
                     if (cur_fm->cur_fc->right != NULL) {
                     
                        /* SET RETURN PATH TO TRUE */
                        ret_path = 1;
                     }   
                  }
               }   
               else {   /* STACK IS EMPTY RETURN FAILURE */
                  return(0);  
               }
            }
            else {

               /* IF BACK FIELD OF CUR_FC IS NULL { AT TRUE ROOT } */
               if (cur_fm->cur_fc->back == NULL) {

                  /* SET CUR_FC TO LEFT POINTER FIELD OF FORMAT ROOT */
                  cur_fm->cur_fc = cur_fm->cur_fcr->left;

                  /* WHILE RIGHT POINTER NOT NULL DO */
                  while(cur_fm->cur_fc->right != NULL) {

                     /* SET CUR_FC TO IT'S RIGHT POINTER */
                     cur_fm->cur_fc = cur_fm->cur_fc->right;
                  }

                  /* SET REP_FOUND TO FALSE */
                  rep_found = 0;

                  /* WHILE NOT REP_FOUND AND CUR_FC NOT ROOT PTR DO  */
                  while(!rep_found && cur_fm->cur_fc != cur_fm->cur_fcr) {

                     /* IF LEFT CHILD OF CUR_FC EXISTS */
                     if (cur_fm->cur_fc->left != NULL) {

                        /* IF REP_NO OF LEFT CHILD OF CUR_FC IS NON-ZERO  */
                        if (cur_fm->cur_fc->left->rep_no) {

                           /* SET REP_FOUND TO TRUE */
                           rep_found = 1;
 
                        }
                     }

                     /* IF REP_FOUND */
                     if (rep_found) {
                     
                        /* SET CURRENT FORMAT CONTROL ROOT TO CURRENT FORMAT */
                        cur_fm->cur_fcr = cur_fm->cur_fc;
                 
                     }
                     else {

                        /* SET CUR_FC TO ITS BACK FIELD */
                        cur_fm->cur_fc = cur_fm->cur_fc->back;

                     }
                  }
               }
            
               /* SET RETURN PATH TO TRUE */
               ret_path = 1;
            
            }
         }
      }
   }
   /* RETURN SUCCESS */
   return(1);
}
