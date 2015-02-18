# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: GET123ADSCR
**
**    PURPOSE: TO RETRIEVE NUMBER OF DIMENSIONS AND THE LENGTHS OF EACH 
**              DIMENSION FROM THE DATA DESCRIPTIVE RECORD LABEL SUBFIELD
**
**    INVOCATION METHOD: GET123ADSCR(IN_STR)
**
**    ARGUMENT LIST:
**     NAME            TYPE      USE      DESCRIPTION
**     IN_STR[]        PTR        I       INPUT STRING POINTER
**     GET123ADSCR()   LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     G123DSTR()       RETRIEVES CHARACTER STRING
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     I          INT                LOOP CONTROL VARIABLE
**     LEN        INT                CHARACTER STRING LENGTH
**     NEW_DM     PTR                POINTER TO NEW ENTRY IN DIMENSION
**                                    LENGTHS STRUCTURE
**     STRING[6]  CHAR               TEMPORARY CHARACTER STRING CONTAINING
**                                    NUMBERS BEFORE INTEGER CONVERSION
**     ST_TBUF[]  CHAR               POINTER TO STARTING ADDRESS OF TEMP BUFFER
**     TMPBUF[]   CHAR               TEMPORARY CHARACTER BUFFER
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
**      NUM_DIM         INT     O    NUMBER OF DIMENSIONS IN ARRAY DESCRIPTOR
**      DIM_LPTR        PTR     O    HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT     O    NUMBER OF ELEMENTS IN PRIMARY DIMENSION
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
**      LEN             INT     O    DIMENSION LENGTH
**      NXT             PTR     O    POINTER TO NEXT DIMENSION LENGTH    
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
**      CUR_DM          PTR     O    CURRENT POINTER TO DIMENSION LENGTHS
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
**     ARYD_DLM         CHAR         ARRAY DESCRIPTOR DELIMITER IN DDR
**     FT               CHAR         FIELD TERMINATOR (RS) 1/14
**     FT_STR[2]        CHAR         CHARACTER STRING CONTAINING A FIELD
**                                    TERMINATOR
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
**     ALLOCATE TEMP BUFFER 2 BYTES LARGER THAN INPUT STRING FOR FT AND NULL
**      CHARACTER
**      {G123DSTR() MODIFIES THE ADDRESS OF THE INPUT STRING WHICH MUST
**       ALSO BE TERMINATED WITH AN FT }
**     SET STARTING ADDRESS POINTER TO TEMP BUFFER
**     COPY INPUT STRING TO TEMP BUFFER
**     APPEND FT TO TEMP BUFFER
**     CALL G123DSTR() TO RETRIEVE NUMBER OF DIMENSIONS FROM TEMP BUFFER
**     CONVERT TO INTEGER AND SET TO NUMBER OF DIMENSIONS FOR CURRENT DD
**     IF NUMBER OF DIMENSIONS LESS THAN OR EQUAL TO 1
**     THEN
**        RETURN FAILURE
**     ENDIF
**     ALLOCATE NEW_DM { DUMMY }
**     SET LEN TO ZERO
**     SET NXT TO NULL
**     SET DIM_LPTR TO NEW_DM
**     SET CUR_DM TO NEW_DM
**     FOR EACH DIMENSION DO
**        ALLOCATE NEW_DM
**        SET NXT TO NULL
**        SET CUR_DM NEXT TO NEW_DM
**        SET CUR_DM TO NEW_DM
**        CALL G123DSTR() TO RETRIEVE DIMENSION LENGTH FROM TEMP BUFFER
**        GET LENGTH OF RETRIEVED DIMENSION LENGTH
**        IF STRING ENDS WITH FT
**        THEN
**           REMOVE FT FROM STRING
**        ENDIF
**        CONVERT LENGTH TO INTEGER AND STORE LEN IN NEW_DM
**     ENDFOR
**     SET PRM_DMS TO FIRST LENGTH IN LIST {NOT DUMMY}
**     RESET CUR_DM TO NULL
**     FREE STARTING POINTER TO TEMP BUFFER
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int get123adscr(char *in_str)
{
   /* INTERNAL VARIABLES */
   long i;
   size_t len;
   struct dm *new_dm;
   char string[6];
   char *st_tbuf;
   char *tmp_buf;

   /* ALLOCATE TEMP BUFFER 2 BYTES LARGER THAN INPUT STRING FOR FT AND 
      NULL CHARACTER */
   if ((tmp_buf = (char *) malloc(sizeof(char) * (_tcslen(in_str) + 2)))
    == NULL) return(0);

   /* SET STARTING ADDRESS POINTER TO TEMP BUFFER POINTER */
   st_tbuf = tmp_buf;

   /*COPY INPUT STRING TO TEMP BUFFER AND APPEND AN FT */
   strcpy(tmp_buf,in_str);
   strcat(tmp_buf,FT_STR);

   /* CALL G123DSTR() TO RETRIEVE NUMBER OF DIMENSIONS FROM TEMP BUFFER */
   if (!g123dstr(&tmp_buf,string,ARYD_DLM)) return(0);

   /* CONVERT TO INTEGER AND SET TO NUMBER OF DIMENSIONS FOR CURRENT DD */
   cur_fm->cur_dd->num_dim = atol(string);

   /* IF NUMBER OF DIMENSIONS LESS THAN OR EQUAL TO 1, RETURN FAILURE */
   if (cur_fm->cur_dd->num_dim <= 1) return (0);
   
   /* ALLOCATE NEW_DM { DUMMY } */
   if ((new_dm = (struct dm *) malloc(sizeof(struct dm))) == NULL) return(0);

   /* SET LEN TO ZERO */
   new_dm->len = 0;

   /* SET NXT TO NULL */
   new_dm->nxt = NULL;

   /* SET DIM_LPTR TO NEW_DM */
   cur_fm->cur_dd->dim_lptr = new_dm;

   /* SET CUR_DM TO NEW_DM */
   cur_fm->cur_dm = new_dm;

   /* DO FOR EACH DIMENSION */
   for(i = 1; i <= cur_fm->cur_dd->num_dim; i++) {

      /* ALLOCATE NEW_DM */
      if ((new_dm = (struct dm *) malloc (sizeof(struct dm))) == NULL)
       return(0);

      /* SET NXT TO NULL */
      new_dm->nxt = NULL;

      /* SET CUR_DM NEXT TO NEW_DM */
      cur_fm->cur_dm->nxt = new_dm;
      cur_fm->cur_dm = new_dm;

      /* CALL G123DSTR() TO RETRIEVE DIMENSION LENGTH FROM TEMP BUFFER */
      if (!g123dstr(&tmp_buf,string,ARYD_DLM)) return(0);

      /* GET LENGTH OF RETRIEVED DIMENSION LENGTH */
      len = _tcslen(string);

      /* IF STRING ENDS WITH FT, REMOVE IT */
      if (string[len-1] == (char) FT) string[len-1] = NC;

      /* CONVERT LENGTH TO INTEGER AND STORE LEN IN NEW_DM */
      cur_fm->cur_dm->len = atol(string);
   }

   /* SET PRM_DMS TO FIRST LENGTH IN LIST {NOT DUMMY} */
   cur_fm->cur_dd->prim_dms = cur_fm->cur_dd->dim_lptr->nxt->len;

   /* RESET CUR_DM TO NULL */
   cur_fm->cur_dm = NULL;

   /* FREE STARTING POINTER TO TEMP BUFFER */
   free(st_tbuf);

   /* RETURN SUCCESS */
   return(1);
}
