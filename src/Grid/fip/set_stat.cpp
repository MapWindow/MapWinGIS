# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: SET123STAT
**
**    PURPOSE: TO CHECK FOR END OF FILE AND SET STATUS FLAG
**
**    INVOCATION METHOD: SET123STAT(FP, STATUS)
**
**    ARGUMENT LIST:
**     NAME           TYPE      USE      DESCRIPTION
**     FP             PTR        I       FILE POINTER
**     STATUS         INT        O       STATUS
**                                        0 = FAILURE
**                                        1 = OKAY
**                                        2 = START OF RECORD
**                                        3 = END OF RECORD
**                                        4 = END OF FILE
**                                        5 = END OF FIELD
**                                        6 = START OF FIELD
**     SET123STAT()   LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     FUNCTION         DESCRIPTION
**     GET123LEVEL()    RETRIEVES THE APPROPRIATE DATA STRUCTURE LEVEL
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     CH         INT                NUMERIC FORMAT OF A CHARACTER
**     CUR_POS    LONG               CURRENT FILE POSITION
**  
**    GLOBAL REFERENCES:
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR    N/A   FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR    I    OPEN MODE OF FILE
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
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 06/11/90  INITIAL PROLOG
**     A. DEWITT                 O6/11/90  INITIAL PDL
**     A. DEWITT                 06/11/90  INITIAL CODE
**     L. MCMILLION              11/01/90  INSERTED LOGIC TO TEST READ
**                                          FILES ONLY
**     L. MCMILLION              11/05/90  ADDED A CHECK FOR ASCII CHARACTER
**                                          ^ (ASCII DECIMAL CODE 94) IN
**                                          ADDITION TO EOF
**     L. MCMILLION              02/22/91  REPLACED LIBRARY FUNCTION UNGETC()
**                                          WITH FSEEK() AS UNGETC() REPLACED
**                                          CHARACTERS TO BE OVERWRITTEN IN A
**                                          FILE ORIGINALLY OPENED IN WRITE
**                                          MODE
**     L. MCMILLION  (TASK #40)  09/04/91  ADDED CHECK FOR ASCII DECIMAL CODE
**                                          26 IN ADDITON TO ASCII 94 AND EOF
**     A. DEWITT      92DR005    04/08/92  ADD FEOF() CALL FOR BINARY FILE
**     J. TAYLOR      92DR015    10/30/92  MODIFIED CALL TO FSEEK TO SEEK FROM 
**                                          BEGINNING OF FILE INSTEAD OF
**                                          CURRENT POSITION
**
**    PDL:
**
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LEVEL
**     IF OPEN MODE IS READ
**     THEN
**        READ CHARACTER
**        IF CHARACTER IS END OF FILE OR EITHER ASCII CODE 26 OR 94 
**        THEN 
**           SET STATUS TO EOF
**        ENDIF
**        IF CHARACTER NOT END OF FILE
**        THEN
**        GET CURRENT FILE POSITION, DECREMENT, THEN SEEK POSITION
**         FROM BEGINNING OF FILE
**        ENDIF
**     ENDIF
**
**     RETURN SUCCESS
**    
*****************************************************************************
**     CODE SECTION
**
*****************************************************************************/ 
#include "stc123.h"


int set123stat(FILE *fp,int *status)

{
   /* LOCAL VARIABLES */
   int ch;
   long cur_pos;

   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return(0);
   
   /* IF OPEN MODE IS READ */
   if (cur_fm->open_mode == 'r') {
   
      /* GET CHARACTER */
      ch = getc(fp);
      
      /* IF END OF FILE REACHED OR EITHER ASCII CODE 26 OR 94 ENCOUNTERED */
      if (ch == EOF || ch == 26 || ch == 94 || feof(fp)) {
      
         /* SET STATUS TO EOF */
         *status = 4;
      }
      
      /* IF CHARACTER NOT END OF FILE */
      if (ch != EOF && !feof(fp)) {
      
         /* GET CURRENT FILE POSITION, DECREMENT, THEN SEEK POSITION
             FROM BEGINNING OF FILE
         */
         cur_pos = ftell(fp);
         cur_pos--;
         if (fseek(fp,cur_pos,SEEK_SET)) return(0);
      }
   };
   
   /*RETURN SUCCESS */
   return(1);
}
