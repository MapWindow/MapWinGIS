# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: GET123DIM
**
**    PURPOSE: TO RETRIEVE NUMBER OF DIMENSIONS AND THE LENGTHS OF EACH 
**              DIMENSION
**
**    INVOCATION METHOD: GET123DIM(IN_STR,NUM_DIM,PRM_DMS)
**
**    ARGUMENT LIST:
**     NAME         TYPE      USE      DESCRIPTION
**     IN_STR[]     PTR        I       POINTER TO INPUT STRING
**     NUM_DIM      INT        O       NUMBER OF DIMENSIONS
**     PRM_DMS      INT        O       SIZE OF PRIMARY DIMENSION
**     GET123DIM()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     G123DSTR()       RETRIEVES CHARACTER STRING
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     NEW_DM     PTR                POINTER TO NEW ENTRY IN DIMENSION
**                                    LENGTHS STRUCTURE
**     STRING[6]  CHAR               TEMPORARY CHARACTER STRING CONTAINING
**                                    NUMBERS BEFORE INTEGER CONVERSION
**     I          INT                LOOP CONTROL VARIABLE
**
**    GLOBAL REFERENCES:
**
**     DIMENSION LENGTHS STRUCTURE (DM)
**      NAME            TYPE   USE   DESCRIPTION
**      LEN             INT     O    DIMENSION LENGTH
**      NXT             PTR     O    POINTER TO NEXT DIMENSION LENGTH    
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT     O    NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR     O    HEAD POINTER TO DIMENSION LENGTHS
**                                    (NO LABELS)
**      VALUES          PTR    N/A   HEAD POINTER TO DATA VALUE SUBFIELD
**                                    RECORDS
**      NEXT            PTR    N/A   POINTER TO NEXT DATA RECORD 
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
**      CUR_DM          PTR     O    CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY
**      CUR_DR          PTR     I    CURRENT POINTER TO DATA RECORD STRUCTURE
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
**     UT               CHAR         UNIT TERMINATOR 
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     P. HODGES                 06/04/90  INITIAL CODE
**     L. MCMILLION  (TASK #40)  08/28/91  UPDATE PDL TO CODE
**     L. MCMILLION  TASK #40    12/01/92  UPDATE PROLOG/PDL
**     J. TAYLOR     93DR023     04/20/93  MODIFIED TO RETRIEVE FROM CHARACTER
**                                          RATHER THAN FILE
**
**    PDL:
**
**     CALL G123DSTR() TO RETRIEVE NUMBER OF DIMENSIONS
**     CONVERT NUMBER OF DIMENSIONS INTO INTEGER
**     IF NUMBER OF DIMENSIONS LESS THAN OR EQUAL TO 1
**     THEN
**        RETURN FAILURE
**     ENDIF
**     ALLOCATE NEW_DM { DUMMY }
**     SET NXT TO NULL
**     SET DIM_LPTR TO NEW_DM
**     SET CUR_DM TO NEW_DM
**     SET LEN TO ZERO
**
**     DO FOR EACH DIMENSION
**        ALLOCATE NEW_DM
**        SET NXT TO NULL
**        SET CUR_DM NEXT TO NEW_DM
**        CALL G123DSTR() TO RETRIEVE DIMENSION LENGTH
**        CALL ATOL() TO CONVERT LEN TO INTEGER
**        STORE LEN IN NEW_DM
**     ENDDO
**
**     SET CUR_DM TO DIM_LPTR NEXT
**     SET PRM_DMS TO CUR_DM LEN
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int get123dim(char **in_str,long *num_dim,long *prm_dms)
{
   /* INTERNAL VARIABLES */
   long i;
   struct dm *new_dm;
   char string[6];

   /* CALL G123DSTR() TO RETRIEVE NUMBER OF DIMENSIONS AND CONVERT TO INTEGER */
   if (!g123dstr(in_str,string,UT)) return(0);
   *num_dim = atol(string);

   /* IF NUMBER OF DIMENSIONS LESS THAN OR EQUAL TO 1, RETURN FAILURE */
   if (*num_dim <= 1) return (0);
   
   /* ALLOCATE NEW_DM { DUMMY } */
   if ((new_dm = (struct dm *) malloc(sizeof(struct dm))) == NULL) return(0);

   /* SET NXT TO NULL */
   new_dm->nxt = NULL;

   /* SET DIM_LPTR TO NEW_DM */
   cur_fm->cur_dr->dim_lptr = new_dm;

   /* SET CUR_DM TO NEW_DM */
   cur_fm->cur_dm = new_dm;

   /* SET LEN TO ZERO */
   new_dm->len = 0;

   /* DO FOR EACH DIMENSION */
   for(i = 1; i <= cur_fm->cur_dr->num_dim; i++) {

      /* ALLOCATE NEW_DM */
      if ((new_dm = (struct dm *) malloc (sizeof(struct dm))) == NULL) return(0);

      /* SET NXT TO NULL */
      new_dm->nxt = NULL;

      /* SET CUR_DM NEXT TO NEW_DM */
      cur_fm->cur_dm->nxt = new_dm;
      cur_fm->cur_dm = new_dm;

      /* CALL G123DSTR() TO RETRIEVE DIMENSION LENGTH */
      if (!g123dstr(in_str,string,UT)) return(0);

      /* CALL ATOL() TO CONVERT LEN TO INTEGER AND STORE LEN IN NEW_DM */
      cur_fm->cur_dm->len = atol(string);
   }

   /* SET CUR_DM TO DIM_LPTR NEXT */
   cur_fm->cur_dm = cur_fm->cur_dr->dim_lptr->nxt;

   /* SET PRM_DMS TO CUR_DM LEN */
   *prm_dms = cur_fm->cur_dm->len;

   /* RETURN SUCCESS */
   return(1);
}
