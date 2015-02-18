# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: LOAD123LAB
**
**    PURPOSE: TO LOAD CONCATENATED LABELS INTO THE DD LABEL SUBFIELD
**              STRUCTURE
**
**    INVOCATION METHOD: LOAD123LAB(LSTRNG)
**
**    ARGUMENT LIST:
**     NAME         TYPE      USE      DESCRIPTION
**     LSTRNG[]     PTR        I       LABELS STRING
**     LOAD123LAB() LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     STR123TOK()      RETURNS A POINTER TO A TOKEN STRING
**
**    INTERNAL VARIABLES:
**     NAME         TYPE               DESCRIPTION
**     CUR_LAB[]    PTR                POINTER CURRENT LABEL IN A SET
**     CUR_SET[]    PTR                POINTER TO CURRENT LABEL SET
**     I            INT                INDEX INTO CHARACTER STRING
**     NEW_SL       PTR                NEW POINTER TO DD-LABEL SUBFIELD
**                                      STRUCTURE ENTRY
**     SLEN         INT                STRING LENGTH
**     ST_TMPPTR[]  PTR                START OF TEMPORARY STRING POINTER
**     ST_TMPSTR[]  PTR                START OF TEMPORARY STRING POINTER
**     TMP_LAB[]    PTR                TEMPORARY LABEL POINTER
**     TMP_PTR[]    PTR                TEMPORARY STRING POINTER CONTAINING A
**                                      LABEL(S)
**     TMP_SL       PTR                TEMPORARY POINTER TO DD-LABEL SUBFIELD
**                                      STRUCTURE ENTRY
**     TMP_STR[]    PTR                TEMPORARY STRING POINTER CONTAINING
**                                      LABELS
**     TOK_LEN      LONG               LENGTH OF TOKEN RETURNED FROM STR123TOK
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
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS IN ARRAY DESCRIPTOR
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT    N/A   NUMBER OF ELEMENTS IN PRIMARY DIMENSION
**      LABELS          PTR    I/O   HEAD POINTER TO A LINKED LIST CONTAINING
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
**      CUR_SL          PTR     O    CURRENT POINTER TO DD-LABEL SUBFIELD
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
**      LABEL[]         PTR    I/O   LABEL
**      NXT_LAB         PTR    I/O   POINTER TO NEXT LABEL IN SET
**      NXT_LSET        PTR    I/O   POINTER TO NEXT SET OF LABELS
**
**    GLOBAL VARIABLES: 
**     NAME        TYPE      USE     DESCRIPTION
**     CUR_FM      PTR        I      POINTER TO CURRENT FILE MANAGEMENT 
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS:
**     NAME        TYPE              DESCRIPTION
**     CDL         CHAR              CARTESIAN VECTOR LABEL DELIMITER * 2/10
**     CDL_STR[]   CHAR              CHARACTER STRING CONTAINING CARTESIAN
**                                    DELIMITER
**     NC          CHAR              NULL CHARACTER
**     SEP_STR[]   CHAR              CHARACTER STRING CONTAINING SEPARATOR FOR
**                                    ADJACENT DELIMITERS
**     VDL         CHAR              VECTOR DATA ELEMENTS DELIMITER ! 2/1
**     VDL_STR[]   CHAR              CHARACTER STRING CONTIANING VECTOR
**                                    DATA ELEMENTS DELIMITER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/21/90  INITIAL PROLOG
**     P. HODGES                 05/21/90  INITIAL PDL
**     L. MCMILLION              06/03/90  INITIAL CODE
**     A. DEWITT                 08/08/90  ADDED LOGIC TO PROCESS LABELS STRING
**                                          CONTAINING ADJACENT DELIMITERS
**     L. MCMILLION              09/26/90  REPLACED CALLS TO LIBRARY FUNCTION
**                                          STRTOK() WITH FUNCTION STR123TOK()
**                                          DUE TO NESTING PROBLEMS
**     L. MCMILLION  92DR009     04/14/92  REPLACED SEPARATOR FOR ADJACENT
**                                          DELIMITERS
**     J. TAYLOR     92DR005     05/20/92  CHANGED CALLING SEQUENCE TO      
**                                          STR123TOK TO RETURN TOKEN LENGTH
**     L. MCMILLION  TASK #40    12/08/92  PROLOG UPDATE
**     J. TAYLOR     93DR033     10/26/93  ADDED ST_TMPPTR TO SAVE THE 
**                                          ADDRESS OF TMP_PTR SO IT CAN BE
**                                          FREED BEFORE RETURNING
**
**    PDL:
**   
**     IF INPUT STRING EMPTY
**     THEN
**        RETURN SUCCESS
**     ENDIF
**     SET TEMPORARY STRING POINTER TO INPUT STRING
**     IF FIRST CHARACTER INDICATES NULL COLUMN VALUES OF TWO OR MORE 
**      DIMENSIONS
**     THEN
**        INCREMENT STRING LENGTH
**     ENDIF
**     WHILE TMP_STR NOT POINTING TO NULL CHARACTER DO
**        IF TMP_STR IS POINTING TO CARTESIAN DELIMITER OR VECTOR DELIMITER
**        THEN
**           IF NEXT CHARACTER OF STRING EQUALS CARTESIAN DELIMITER OR A
**            VECTOR DELIMITER OR NULL CHARACTER - A NULL DIMENSION
**           THEN
**              INCREMENT STRING LENGTH TO COUNT INSTANCE
**           ENDIF
**        ENDIF
**        MOVE POINTER TO NEXT CHARACTER
**     ENDWHILE
**     INCREMENT SLEN FOR NC SPACE
**   
**     INITIALIZE INDEX
**     ALLOCATE SPACE FOR TEMPORARY STRING
**     INITIALIZE TEMPORARY STRING TO NULL CHARACTER
**     SET START TEMPORARY STRING POINTER TO BEGINNING OF TEMPORARY STRING
**     IF FIRST CHARACTER INDICATES NULL COLUMN VALUES OF TWO OR MORE
**      DIMENSIONS
**     THEN
**        APPEND SEPARATOR TO TMP_STR 
**     ENDIF
**     WHILE LSTRING NOT POINTING TO NULL CHARACTER DO
**        IF LSTRING IS POINTING TO CARTESIAN OR VECTOR DELIMITER
**        THEN
**           APPEND CHARACTER TO TEMPORARY STRING
**           IF NEXT CHARACTER OF STRING EQUALS CARTESIAN OR VECTOR DELIMITER
**            OR A NULL CHARACTER - A NULL DIMENSION
**           THEN
**              APPEND A SEPARATOR TO TEMPORARY STRING
**           ENDIF
**        ELSE
**           APPEND CHARACTER TO TMP_STR
**        ENDIF
**        UPDATE INDEX TO NEXT CHARACTER
**     ENDWHILE   
**
**     ALLOCATE NEW_SL { A DUMMY HEADER }
**     SET NXT_LAB FIELD OF NEW_SL TO NULL
**     SET NXT_LSET FIELD OF NEW_SL TO NULL
**     SET LABEL POINTER FIELD OF NEW_SL TO NULL
**     SET LABELS OF CUR_DD TO NEW_SL
**     SET CURRENT LABEL POINTER STRUCTURE TO NEW_SL
**     CALL STR123TOK() TO DIVIDE THE LABEL STRING INTO SETS OF LABELS
**      AND RETURN FIRST SET TO CUR_SET POINTER
**
**     WHILE THERE ARE MORE SETS OF LABELS DO
**        ALLOCATE NEW_SL
**        SET NXT_LAB POINTER FIELD TO NULL
**        SET NXT_LSET POINTER FIELD TO NULL
**        IF CURRENT LABEL SET RETURNED IS EMPTY
**        THEN
**           SET LABEL POINTER TO NULL
**        ELSE
**           COMPUTE SIZE OF SPACE NEEDED TO HOLD STRING
**           ALLOCATE NEEDED SPACE
**           SAVE ADDRESS OF TMP_PTR
**           INITIALIZE SPACE TO NULL CHARACTER
**           COPY STRING SET RETURNED TO NEWLY ALLOCATED SPACE
**           SET LABEL POINTER TO THIS COPY OF STRING
**        ENDIF
**        SET CUR_SL NXT_LSET TO NEW_SL
**        SET CUR_SL TO NEW_SL
**        CALL STR123TOK() TO RETURN THE NEXT SET OF LABELS
**     ENDWHILE
**   
**     SET TMP_SL TO NXT_LSET FIELD OF DUMMY
**        
**     { GO THROUGH EACH SET OF LABELS AND SEPARATE THEM INTO SINGLE LABEL 
**      STRUCTURES }
**
**     WHILE TMP_SL NOT NULL DO
**        IF SET OF LABELS IS NOT EMPTY
**        THEN
**           CALL STR123TOK() WITH LABEL FIELD OF TMP_SL TO DIVIDE
**            CUR_SET OF LABELS INTO TOKENS CONTAINING A SINGLE LABEL
**           SET TMP_LAB TO TMP_SL LABEL
**           IF THE CURRENT LABEL RETURNED FROM STR12TOK() IS EMPTY
**           THEN
**              SET LABEL POINTER TO NULL
**           ELSE
**              COMPUTE SIZE OF SPACE NEEDED TO HOLD LABEL STRING
**              ALLOCATE SPACE NEEDED
**              INITIALIZE SPACE TO NULL CHARACTER
**              COPY CURRENT LABEL TO NEWLY ALLOCATED SPACE
**              SET TMP_SL LABEL TO THIS COPY OF STRING
**           ENDIF
**           SET CUR_SL TO TMP_SL
**
**           CALL STR123TOK() TO RETURN THE NEXT LABEL
**           WHILE THERE ARE MORE LABELS IN THE SET
**              ALLOCATE NEW_SL
**              SET NXT_LAB TO NULL
**              SET NXT_LSET TO NULL
**              IF THE CURRENT LABEL RETURNED IS EMPTY
**              THEN
**                 SET LABEL POINTER TO NULL
**              ELSE
**                 COMPUTE SIZE OF SPACE NEEDED TO HOLD LABEL STRING
**                 ALLOCATE NEEDED SPACE
**                 INITIALIZE SPACE TO NULL CHARACTER
**                 COPY STRING SET RETURNED TO NEWLY ALLOCATED SPACE
**                 SET LABEL POINTER TO THIS COPY OF STRING
**              ENDIF
**              SET NXT_LAB OF CUR_SL TO NEW_SL
**              SET CUR_SL TO NEW_SL
**           ENDWHILE
**
**        ENDIF
**        SET TMP_SL TO THE NEXT SET OF LABELS
**     ENDWHILE
**
**     SET CURRENT LABEL POINTER TO HEAD POINTER TO DUMMY
**     FREE SPACE AT START OF TEMPORARY STRINGS
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int load123lab(char *lstring)
{
   /* INTERNAL VARIABLES */
   int    i         ;
   size_t slen      ;
   struct sl *new_sl;
   struct sl *tmp_sl;
   char   *cur_set  ;
   char   *cur_lab  ;
   char   *st_tmpptr;
   char   *st_tmpstr;
   char   *tmp_lab  ;
   char   *tmp_str  ;
   char   *tmp_ptr  ;
   long   tok_len   ;
   
   /* IF INPUT STRING EMPTY RETURN SUCCESS */
   if ((slen = (size_t) _tcslen(lstring)) < 1) return(1);
   
   /* SET TEMPORARY STRING POINTER TO INPUT STRING */
   tmp_str = lstring;
   
   /* IF FIRST CHARACTER INDICATES NULL COLUMN VALUES OF TWO OR MORE 
      DIMENSIONS INCREMENT STRING LENGTH 
   */
   if ((*tmp_str == VDL) || (*tmp_str == CDL)) slen++;
   
   /* WHILE TMP_STR NOT POINTING TO NULL CHARACTER DO */
   while (*tmp_str != NC) {
      
      /* IF TMP_STR IS POINTING TO CARTESIAN DELIMITER OR VECTOR DELIMITER */
      if (*tmp_str == CDL || *tmp_str == VDL) {
         
         /* IF NEXT CHARACTER OF STRING EQUALS CARTESIAN DELIMITER OR A
             VECTOR DELIMITER OR A NULL CHARACTER - A NULL DIMENSION,
             INCREMENT STRING LENGTH TO COUNT INSTANCE
         */
         if (tmp_str[1] == CDL || tmp_str[1] == VDL || tmp_str[1] == NC) slen++;
      }
      
      /* MOVE POINTER TO NEXT CHARACTER */
      tmp_str++;
   }         

   /* INCREMENT SLEN FOR NC SPACE */
   slen++;
   
   /* INITIALIZE INDEX */
   i = 0;
   
   /* ALLOCATE SPACE FOR TEMPORARY STRING */
   if ((tmp_str = (char *) malloc(slen * sizeof(char))) == NULL) return(0);

   /* INITIALIZE TEMPORARY STRING TO NULL CHARACTER */
   *tmp_str = NC;

   /* SET START TEMPORARY STRING POINTER TO BEGINNING OF TEMPORARY STRING */
   st_tmpstr = tmp_str;

   /* IF FIRST CHARACTER INDICATES NULL COLUMN VALUES OF TWO OR MORE
      DIMENSIONS APPEND SEPARATOR TO TMP_STR 
   */
   if ((lstring[i] == VDL) || (lstring[i] == CDL)) strcat(tmp_str,SEP_STR);
   
   /* WHILE LSTRING NOT POINTING TO NULL CHARACTER DO */
   while (lstring[i] != NC) {
      
      /* IF LSTRING IS POINTING TO CARTESIAN OR VECTOR DELIMITER */
      if (lstring[i] == CDL || lstring[i] == VDL) {
         
         /* APPEND CHARACTER TO TEMPORARY STRING */
         strncat(tmp_str,&lstring[i],1);
         
         /* IF NEXT CHARACTER OF STRING EQUALS CARTESIAN OR VECTOR DELIMITER
             OR NULL CHARACTER - A NULL DIMENSION, APPEND A SEPARATOR
             TO TEMPORARY STRING
         */
         if (lstring[i+1] == CDL || lstring[i+1] == VDL || lstring[i+1] == NC) {
            strcat(tmp_str,SEP_STR);
         }
      }
      
      /* ELSE APPEND CHARACTER TO TMP_STR */
      else {
         strncat(tmp_str,&lstring[i],1);
      }
         
      /* UPDATE INDEX TO NEXT CHARACTER */
      i++;
   }         

   /* ALLOCATE NEW_SL { A DUMMY HEADER } */
   if ((new_sl = (struct sl *) malloc(sizeof(struct sl))) == NULL) return(0);

   /* SET NXT_LAB FIELD OF NEW_SL TO NULL */
   new_sl->nxt_lab = NULL;

   /* SET NXT_LSET FIELD OF NEW_SL TO NULL */
   new_sl->nxt_lset = NULL;

   /* SET LABEL POINTER FIELD OF NEW_SL TO NULL */
   new_sl->label = NULL;

   /* SET LABELS OF CUR_DD TO NEW_SL { DUMMY HEADER } */
   cur_fm->cur_dd->labels = new_sl;

   /* SET CURRENT LABEL POINTER STRUCTURE TO NEW_SL */
   cur_fm->cur_sl = new_sl;

   /* CALL STR123TOK() TO DIVIDE THE LABEL STRING INTO SETS OF LABELS
       AND RETURN FIRST SET TO CUR_SET POINTER
   */
   cur_set = str123tok(&tmp_str,CDL_STR,&tok_len);

   /* WHILE THERE ARE MORE SETS OF LABELS DO */
   while (cur_set != NULL) {

      /* ALLOCATE NEW_SL */
      if ((new_sl = (struct sl *) malloc(sizeof(struct sl))) == NULL) return(0);

      /* SET NXT_LAB POINTER FIELD TO NULL */
      new_sl->nxt_lab = NULL;

      /* SET NXT_LSET POINTER FIELD TO NULL */
      new_sl->nxt_lset = NULL;

      /* IF THE CURRENT SET RETURNED IS EMPTY */
      if (!strcmp(cur_set,SEP_STR)) {
         
         /* SET LABEL POINTER TO NULL */
         new_sl->label = NULL;

      }
      /* SET IS NOT EMPTY */
      else {
         
         /* COMPUTE SIZE OF SPACE NEEDED TO HOLD STRING */
         slen = (size_t) _tcslen(cur_set) + 1;
         
         /* ALLOCATE NEEDED SPACE */
         if ((tmp_ptr = (char *) malloc(slen * sizeof(char))) == NULL) return(0);
         
         /* SAVE ADDRESS OF TMP_PTR */
         st_tmpptr = tmp_ptr;

         /* INITIALIZE SPACE TO NULL CHARACTER */
         *tmp_ptr = NC;
         
         /* COPY STRING SET RETURNED TO NEWLY ALLOCATED SPACE */
         strcpy(tmp_ptr,cur_set);
         
         /* SET LABEL POINTER TO THIS COPY OF STRING */
         new_sl->label = tmp_ptr;
      };   

      /* SET CUR_SL NXT_LSET TO NEW_SL */
      cur_fm->cur_sl->nxt_lset = new_sl;

      /* SET CUR_SL TO NEW_SL */
      cur_fm->cur_sl = new_sl;

      /* CALL STR123TOK() TO RETURN THE NEXT SET OF LABELS */
      cur_set = str123tok(&tmp_str,CDL_STR,&tok_len);
   }

   /* SET TMP_SL TO NXT_LSET FIELD OF DUMMY */
   tmp_sl = cur_fm->cur_dd->labels->nxt_lset;

   /* WHILE TMP_SL NOT NULL DO */
   while (tmp_sl != NULL) {

      /* IF SET OF LABELS IS NOT EMPTY */
      if ( tmp_sl->label != NULL) {
      
         /* CALL STR123TOK() WITH LABEL FIELD OF TMP_SL TO DIVIDE
             CUR_SET OF LABELS INTO TOKENS CONTAINING A SINGLE LABEL
             AND ASSIGN THE FIRST LABEL TO CUR_LAB
         */
         cur_lab = str123tok(&tmp_sl->label,VDL_STR,&tok_len);

         /* SET TMP_LAB TO TMP_SL LABEL { REMAINING LABELS } */
         tmp_lab = tmp_sl->label;

         /* IF THE CURRENT LABEL RETURNED IS EMPTY */
         if (!strcmp(cur_lab,SEP_STR)) {
         
            /* SET LABEL POINTER TO NULL */
            tmp_sl->label = NULL;

         }
         /* LABEL PRESENT */
         else {
         
            /* COMPUTE SIZE OF SPACE NEEDED TO HOLD LABEL STRING */
            slen = (size_t) _tcslen(cur_lab) + 1;
         
            /* ALLOCATE NEEDED SPACE */
            if ((tmp_ptr = (char *) malloc(slen * sizeof(char))) == NULL) return(0);
            
            /* INITIALIZE SPACE TO NULL CHARACTER */
            *tmp_ptr = NC;
            
            /* COPY CURRENT LABEL TO NEWLY ALLOCATED SPACE */
            strcpy(tmp_ptr,cur_lab);
         
            /* SET TMP_SL LABEL TO THIS COPY OF STRING */
            tmp_sl->label = tmp_ptr;
         };   

         /* SET CUR_SL TO TMP_SL */
         cur_fm->cur_sl = tmp_sl;

         /* WHILE THERE ARE MORE LABELS IN THE SET */
         while((cur_lab = str123tok(&tmp_lab,VDL_STR,&tok_len)) != NULL ) {

            /* ALLOCATE NEW_SL */
            if ((new_sl = (struct sl *) malloc(sizeof(struct sl))) == NULL) return(0);

            /* SET NXT_LAB TO NULL */
            new_sl->nxt_lab = NULL;

            /* SET NXT_LSET TO NULL */
            new_sl->nxt_lset = NULL;

            /* IF THE CURRENT LABEL RETURNED IS EMPTY */
            if (!strcmp(cur_lab,SEP_STR)) {
         
               /* SET LABEL POINTER TO NULL */
               new_sl->label = NULL;

            }
            /* LABEL PRESENT */
            else {
            
               /* COMPUTE SIZE OF SPACE NEEDED TO HOLD LABEL STRING */
               slen = (size_t) _tcslen(cur_lab) + 1;
         
               /* ALLOCATE NEEDED SPACE */
               if ((tmp_ptr = (char *) malloc(slen * sizeof(char))) == NULL) return(0);
            
               /* INITIALIZE SPACE TO NULL CHARACTER */
               *tmp_ptr = NC;
               
               /* COPY STRING SET RETURNED TO NEWLY ALLOCATED SPACE */  
               strcpy(tmp_ptr,cur_lab);
         
               /* SET LABEL POINTER TO THIS COPY OF STRING */
               new_sl->label = tmp_ptr;
            };   

            /* SET NXT_LAB OF CUR_SL TO NEW_SL */
            cur_fm->cur_sl->nxt_lab = new_sl;

            /* SET CUR_SL TO NEW_SL */
            cur_fm->cur_sl = new_sl;
         }
      }
      
      /* SET TMP_SL TO THE NEXT SET OF LABELS */
      tmp_sl = tmp_sl->nxt_lset;
      
   }
   
   /* SET CURRENT LABEL POINTER TO HEAD POINTER TO DUMMY */
   cur_fm->cur_sl = cur_fm->cur_dd->labels;
   
   /* FREE SPACE AT START OF TEMPORARY STRING */
   free(st_tmpptr);

   /* FREE SPACE AT START OF TEMPORARY STRING */
   free(st_tmpstr);

   /* RETURN SUCCESS */
   return(1);
}
