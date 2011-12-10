# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RET123DV
**
**    PURPOSE: TO RETURN DELIMITED DATA VALUES STORED IN STRUCTURE 
**
**    INVOCATION METHOD: RET123DV(B_PTR,PRIM_DMS)
**
**    ARGUMENT LIST:
**     NAME       TYPE      USE      DESCRIPTION
**     B_PTR[]    PTR        I       CHARACTER POINTER TO BUFFER
**     PRIM_DMS   INT        I       PRIMARY DIMENSION LENGTH
**     RET123DV() LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     STR123TOK()      RETURNS A POINTER TO A STRING TOKEN
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     LEN          INT                CHARACTER STRING LENGTH
**     NEW_DV       PTR                POINTER TO DATA VALUE STRUCTURE
**     ST_TMPBUF[]  PTR                START OF TEMPORARY BUFFER
**     TMP_BUF[]    PTR                TEMPORARY BUFFER
**     TMP_STR[]    PTR                TEMPORARY CHARACTER STRING
**     TOK_LEN      LONG               LENGTH OF TOKEN RETURNED FROM STR123TOK
**     VAL_CT       INT                NUMBER OF CURRENT VALUES IN SET
**     VAL_STR[]    PTR                VALUE CHARACTER STRING
**  
**    GLOBAL REFERENCES:
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSION LENGTHS
**                                    (NO LABELS)
**      VALUES          PTR     O    HEAD POINTER TO DATA VALUE SUBFIELD
**                                    RECORDS
**      NEXT            PTR    N/A   POINTER TO NEXT DATA RECORD 
**
**     DR DATA VALUE SUBFIELD STRUCTURE (DV)
**      NAME            TYPE   USE   DESCRIPTION
**      VALUE           CHAR    O    DATA VALUE
**      NXT_VSET        PTR     O    POINTER TO NEXT SET OF DATA VALUES
**      NXT_VAL         PTR     O    POINTER TO NEXT DATA VALUE SUBFIELD 
**                                    RECORD
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
**      CUR_DD          PTR    N/A   CURRENT POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE ENTRY
**      CUR_DM          PTR    N/A   CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY
**      CUR_DR          PTR     I    CURRENT POINTER TO DATA RECORD STRUCTURE
**                                    ENTRY
**      CUR_DV          PTR    I/O   CURRENT POINTER TO DR DATA VALUE SUBFIELD
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
**     DEL_STR[3]       CHAR         CHARACTER STRING CONTAINING THE FIELD AND
**                                    UNIT TERMINATORS
**     FT               CHAR         FIELD TERMINATOR (RS) 1/14
**     NC               CHAR         NULL CHARACTER
**     UT_STR[2]        CHAR         CHARACTER STRING CONTAINING THE UNIT
**                                    TERMINATOR
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     P. HODGES                 06/06/90  INITIAL CODE
**     A. DEWITT                 06/25/90  INCLUDED DIMENSION ROW_DVH LOGIC
**     L. MCMILLION              10/16/90  REPLACED CALLS TO LIBRARY FUNCTION
**                                          STRTOK() WITH STR123TOK() DUE TO
**                                          NESTING PROBLEMS
**     J. TAYLOR     92DR005     05/20/92  CHANGED CALLING SEQUENCE TO       
**                                          STR123TOK TO RETURN TOKEN LENGTH
**     J. TAYLOR     92DR012     10/30/92  MODIFIED TO REMOVE FT FROM VAL_STR
**                                          BEFORE RETURNING
**     L. MCMILLION  93DR027     04/02/93  CHANGED DELIMITER USED BY FIRST
**                                          STR123TOK() CALL FROM UT_STR TO
**                                          DEL_STR FOR VECTOR DATA 
**     L. MCMILLION  93DR033     07/23/93  UPDATED DR REFERENCE IN PROLOG
**
**    PDL:
**
**     SET STRING LENGTH TO LENGTH OF BUFFER
**     ALLOCATE TEMPORARY BUFFER
**     INITIALIZE TEMPORARY BUFFER TO NULL CHARACTER
**     COPY BUFFER CONTENTS TO TEMPORARY BUFFER
**     SET START OF TEMPORARY BUFFER TO BEGINNING OF TEMPORARY BUFFER
**     ALLOCATE NEW_DV  { SET UP DUMMY HEADER }
**     SET NXT_VAL FIELD OF NEW_DV TO NULL
**     SET NXT_VSET FIELD OF NEW_DV TO NULL
**     SET VALUES IN CUR_DR TO NEW_DV
**     SET ROW_DVH TO NEW_DV
**     CALL STR123TOK() TO SEPERATE THE DELIMITED DATA AND RETURN VAL_STR
**     WHILE THERE ARE MORE DATA VALUES DO
**        ALLOCATE NEW_DV
**        SET NXT_VAL TO NULL
**        SET NXT_VSET TO NULL
**        SET LENGTH TO LENGTH OF VALUE STRING PLUS 1
**        ALLOCATE TEMPORARY STRING SPACE
**        SET TEMPORARY STRING TO VALUE STRING
**        SET NEW_DV VALUE TO TEMPORARY STRING
**        SET NXT_VSET OF ROW_DVH TO NEW_DV
**        SET CUR_DV TO NEW_DV
**        SET ROW_DVH TO NEW_DV
**        SET VAL_CT TO 1
**        CALL STR123TOK() TO RETURN NEXT VAL_STR
**        WHILE THERE ARE MORE DATA VALUES AND VAL_CT IS LESS
**         THAN PRIM_DMS DO
**           ALLOCATE NEW_DV
**           SET NXT_VAL TO NULL
**           SET NXT_VSET TO NULL
**           SET LENGTH TO LENGTH OF VALUE STRING PLUS 1
**           ALLOCATE TEMPORARY STRING SPACE
**           SET TEMPORARY STRING TO VALUE STRING
**           SET NEW_DV VALUE TO TEMPORARY STRING
**           SET CUR_DV TO NEW_DV
**           INCREMENT VAL_CT
**           CALL STR123TOK() TO RETURN NEXT VAL_STR
**           IF VAL_STR IS TERMINATED BY FT, REMOVE IT
**        END WHILE
**     END WHILE
**     FREE SPACE AT START OF TEMPORARY BUFFER
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int ret123dv(char *b_ptr,long prim_dms)

{
   /* INTERNAL VARIABLES */
   struct dv *new_dv   ;
   long       val_ct   ;
   long       tok_len  ;
   size_t     len      ;
   char      *st_tmpbuf,
             *tmp_buf  ,
             *tmp_str  ,
             *val_str  ;

   /* SET STRING LENGTH TO LENGTH OF BUFFER */
   len = _tcslen(b_ptr) + 1;

   /* ALLOCATE TEMPORARY BUFFER */
   if ((tmp_buf = (char *) malloc((size_t) (len * sizeof(char)))) == NULL) return(0);

   /* INITIALIZE TEMPORARY BUFFER TO NULL CHARACTER */
   *tmp_buf = NC;

   /* COPY BUFFER CONTENTS TO TEMPORARY BUFFER */
   strcpy(tmp_buf,b_ptr);

   /* SET START OF TEMPORARY BUFFER TO BEGINNING OF TEMPORARY BUFFER */
   st_tmpbuf = tmp_buf;

   /* ALLOCATE NEW_DV  { SET UP DUMMY HEADER } */
   if((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL) return(0);

   /* SET VALUE FIELD TO NULL */
   new_dv->value = NULL;
   
   /* SET NXT_VAL FIELD OF NEW_DV TO NULL */
   new_dv->nxt_val = NULL;

   /* SET NXT_VSET FIELD OF NEW_DV TO NULL */
   new_dv->nxt_vset = NULL;

   /* SET VALUES IN CUR_DR TO NEW_DV */
   cur_fm->cur_dr->values = new_dv;

   /* SET ROW_DVH TO NEW_DV */
   cur_fm->row_dvh = new_dv;

   /* CALL STR123TOK() TO SEPARATE THE DELIMITED DATA AND RETURN VAL_STR */
   val_str = str123tok(&tmp_buf,DEL_STR,&tok_len);

   /* WHILE THERE ARE MORE DATA VALUES DO */
   while(val_str != NULL) {
  
      /* ALLOCATE NEW_DV */
      if ((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL) return(0);

      /* SET VALUE POINTER TO NULL */
      new_dv->value = NULL;
      
      /* SET NXT_VAL TO NULL */
      new_dv->nxt_val = NULL;

      /* SET NXT_VSET TO NULL */
      new_dv->nxt_vset = NULL;

      /* SET LENGTH TO LENGTH OF VALUE STRING PLUS 1 */
      len = (size_t) _tcslen(val_str) + 1;
      
      /* ALLOCATE TEMPORARY STRING SPACE */
      if ((tmp_str = (char *) malloc(len * sizeof(char))) == NULL) return(0);
      
      /* INITIALIZE STRING */
      tmp_str[0] = NC;
      
      /* SET TEMPORARY STRING TO VALUE STRING */
      strcpy(tmp_str, val_str);
      
      /* SET NEW_DV VALUE TO TEMPORARY STRING */
      new_dv->value = tmp_str;

      /* SET NXT_VSET OF ROW_DVH TO NEW_DV */
      cur_fm->row_dvh->nxt_vset = new_dv;
      
      /* SET CUR_DV TO NEW_DV */
      cur_fm->cur_dv = new_dv;

      /* SET ROW_DVH TO NEW_DV */
      cur_fm->row_dvh = new_dv;

      /* SET VAL_CT TO 1 */
      val_ct = 1;

      /* CALL STR123TOK() TO RETURN NEXT VAL_STR */
      val_str = str123tok(&tmp_buf,DEL_STR,&tok_len);

      /* WHILE THERE ARE MORE DATA VALUES AND VAL_CT IS LESS
          THAN PRIM_DMS DO
      */
      while(val_str != NULL && val_ct < prim_dms) {

         /* ALLOCATE NEW_DV */
         if ((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL) return(0);

         /* SET VALUE FIELD TO NULL */
         new_dv->value = NULL;
         
         /* SET NXT_VAL TO NULL */
         new_dv->nxt_val = NULL;

         /* SET NXT_VSET TO NULL */
         new_dv->nxt_vset = NULL;

         /* SET LENGTH TO LENGTH OF VALUE STRING PLUS 1 */
         len = (size_t) _tcslen(val_str) + 1;
      
         /* ALLOCATE TEMPORARY STRING SPACE */
         if ((tmp_str = (char *) malloc(len * sizeof(char))) == NULL) return(0);
      
         /* INITIALIZE STRING */
         tmp_str[0] = NC;
         
         /* SET TEMPORARY STRING TO VALUE STRING */
         strcpy(tmp_str, val_str);
      
         /* SET NEW_DV VALUE TO TEMPORARY STRING */
         new_dv->value = tmp_str;

         /* SET NXT_VAL OF CUR_DV TO NEW_DV */
         cur_fm->cur_dv->nxt_val = new_dv;

         /* SET CUR_DV TO NEW_DV */
         cur_fm->cur_dv = new_dv;

         /* INCREMENT VAL_CT */
         val_ct++;

         /* CALL STR123TOK() TO RETURN NEXT VAL_STR */
         val_str = str123tok(&tmp_buf,UT_STR,&tok_len);

         /* IF VAL_STR IS TERMINATED BY FT, REMOVE IT */
         if (val_str != NULL) {

            /* SET LENGTH TO LENGTH OF VALUE STRING */
            len = (size_t) _tcslen(val_str);

            if (val_str[len-1] == FT) {
               val_str[len-1] = NC;
               len--;
            }
         }
      }
   }

   /* FREE SPACE AT START OF TEMPORARY BUFFER */
   free(st_tmpbuf);

   /* RETURN SUCCESS */
   return(1);
}
