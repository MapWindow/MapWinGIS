# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: WR123DDREC
**
**    PURPOSE: TO WRITE THE NEXT DATA DESCRIPTIVE RECORD
**
**    INVOCATION METHOD: WR123DDREC(FP,STRING,STATUS)
**
**    ARGUMENT LIST:
**     NAME          TYPE      USE      DESCRIPTION
**     FP            PTR        I       POINTER TO FILE POINTER
**     STRING[]      PTR        I       DATA DESCRIPTIVE RECORD
**     STATUS        INT       I/O      STATUS
**                                       0 = FAILURE
**                                       1 = OKAY
**                                       2 = START OF RECORD
**                                       3 = END OF RECORD
**                                       4 = END OF FILE
**                                       5 = END OF FIELD
**                                       6 = START OF FIELD
**     WR123DDREC()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     LD123DDREC()     READS THE DATA DESCRIPTIVE RECORD AND PLACES THE 
**                       INFORMATION INTO A STRUCTURE FOR REFERENCE
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     POS        PTR                FILE POSITION
**     INSTATUS   INT                INPUT STATUS
**  
**    GLOBAL REFERENCES:
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR    N/A   FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   I/O   OPEN MODE OF FILE
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
**      NEXT            PTR    I/O   POINTER TO NEXT FILE MANAGEMENT
**                                    STRUCTURE
**      BACK            PTR    N/A   POINTER TO PREVIOUS FILE MANAGEMENT
**                                    STRUCTURE
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR    I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/17/90  INITIAL PROLOG
**     P. HODGES                 05/17/90  INITIAL PDL
**     A. DEWITT                 05/24/90  INITIAL CODE
**     L. MCMILLION              10/26/90  ADDED LOGIC TO POSITION CURRENT DD
**                                          STRUCTURE POINTER TO LAST ENTRY IN
**                                          LIST AND SET SUBFIELD STATE TO
**                                          FINISHED
**     L. MCMILLION              02/27/91  ADDED LOGIC TO RETURN FAILURE IF
**                                          OPEN MODE IS READ
**     J. TAYLOR     92DR005     05/16/92  MODIFIED FOR BIT FIELD WRITING
**     L. MCMILLION  TASK #40    12/16/92  UPDATED PROLOG PER QC
**
**    PDL:
**
**     SET INSTATUS TO INPUT STATUS
**     INITIALIZE STATUS TO FAILURE
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER
**     IF OPEN MODE IS READ
**     THEN
**        RETURN FAILURE
**     ENDIF
**     REWIND FP
**     WRITE RECORD TO FP
**     CALL FGETPOS TO GET FILE POSITION
**     REWIND FILE
**     CHANGE ENTRY IN FM TO READ MODE
**     CALL LD123DDREC() TO LOAD FM STRUCTURE WITH TEMPORARY FILE POINTER
**     MODIFY ENTRY IN FM TO WRITE MODE 
**     CALL FSETPOS TO SET FILE POSITION FOR NEXT WRITE
**     WHILE CUR_DD NEXT NOT NULL DO
**        SET CUR_DD TO CUR_DD NEXT
**     ENDWHILE
**     SET SUBFIELD STATE TO FINISHED
**     SET STATUS TO INSTATUS
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int wr123ddrec(FILE *fp,char *string,int *status)
{
   /* LOCAL VARIABLES */
   fpos_t  pos     ;
   int  instatus   ;
   
   /* SET INSTATUS TO INPUT STATUS */
   instatus = *status;
   
   /* SET STATUS TO FAILURE */
   *status = 0;
   
   /* CALL GET_LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return(0);

   /* IF OPEN MODE IS READ, RETURN FAILURE */
   if (cur_fm->open_mode == 'r') return(0);

   /* REWIND FP */
   rewind(fp);
   
   /* WRITE RECORD TO FP */
   fwrite(string,sizeof(char),_tcslen(string),fp);

   /* CALL FGETPOS TO GET FILE POSITION */
   if (fgetpos(fp,&pos)) return(0);

   /* REWIND FILE */
   rewind(fp);

   /* CHANGE ENTRY IN FM TO READ MODE */
   cur_fm->open_mode = 'r';

   /* CALL LD123DDREC() TO LOAD FM STRUCTURE WITH TEMPORARY FILE POINTER */
   if (!ld123ddrec()) return(0);

   /* MODIFY ENTRY IN FM TO WRITE MODE  */
   cur_fm->open_mode = 'w';

   /* CALL FSETPOS TO SET FILE POSITION FOR NEXT WRITE */
   if (fsetpos(fp,&pos)) return(0);
      
   /* WHILE CUR_DD NEXT NOT NULL DO */
   while(cur_fm->cur_dd->next != NULL) {

      /* SET CUR_DD TO CUR_DD NEXT */
      cur_fm->cur_dd = cur_fm->cur_dd->next;
   }

   /* SET SUBFIELD STATE TO FINISHED */
   cur_fm->sf_state_dd = 5;

   /* SET STATUS TO INSTATUS */
   *status = instatus;
   
   /* RETURN SUCCESS */
   return(1);
}
