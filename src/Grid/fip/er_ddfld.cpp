# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: ER123DDFLD 
** 
**    PURPOSE: TO ERASE DATA DESCRIPTIVE RECORD FIELD 
** 
**    INVOCATION METHOD: ER123DDFLD(FP,STATUS) 
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
**     ER123DDFLD()  LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     ER123DDSFLD()    FUNCTION THAT ERASES A DATA DESCRIPTIVE RECORD FIELD 
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
** 
**    INTERNAL VARIABLES: 
**     NAME       TYPE               DESCRIPTION 
**     TEMP_DD    PTR                TEMPORARY CURRENT POINTER TO DATA 
**                                    DESCRIPTIVE RECORD STRUCTURE ENTRY 
**   
**    GLOBAL REFERENCES: 
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION 
**      FP              PTR    N/A   FILE POINTER 
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE 
**                                    CONTROL RECORD STRUCTURE 
**      DD_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE 
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
**    GLOBAL VARIABLES: NONE 
**     NAME            TYPE   USE   DESCRIPTION 
**     CUR_FM          PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                   STRUCTURE ENTRY
** 
**    GLOBAL CONSTANTS: NONE 
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/18/90  INITIAL PROLOG 
**     P. HODGES                 05/18/90  INITIAL PDL 
**     P. HODGES                 06/14/90  INITIAL CODE 
**     L. MCMILLION              01/21/91  ADDED STATUS TO ARGUMENT LIST AND
**                                          LOGIC TO SET STATUS ACCORDINGLY
**     L. MCMILLION  (TASK #40)  10/21/91  ADDED CHECK TO ENSURE NOT BACKED
**                                           UP TO TOP OF STRUCTURE 
**     L. MCMILLION  TASK #40    12/01/92  PROLOG UPDATE
** 
**    PDL: 
** 
**     INITIALIZE STATUS TO FAILURE
**     CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER 
**     ENSURE NOT BACKED UP TO TOP OF STRUCTURE LIST
**     SET TEMP_DD TO CUR_DD 
**     WHILE TEMP_DD EQUAL TO CUR_DD DO
**        CALL ER123DDSFLD() TO ERASE A DATA DESCRIPTIVE RECORD SUBFIELD 
**     ENDWHILE
** 
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int er123ddfld(FILE *fp,int *status)
{
   /*  INTERNAL VARIABLES */
   struct dd *temp_dd;

   /* INITIALIZE STATUS TO FAILURE */
   *status = 0;

   /* CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return(0);

   /* ENSURE NOT BACKED UP TO TOP OF STRUCTURE LIST */
   if (cur_fm->cur_dd == cur_fm->dd_hd) return(0);
   
   /* SET TEMP_DD TO CUR_DD */
   temp_dd = cur_fm->cur_dd;

   /* WHILE TEMP_DD EQUAL TO CUR_DD DO */
   while (temp_dd == cur_fm->cur_dd) {

      /* CALL ER123DDSFLD() TO ERASE A DATA DESCRIPTIVE RECORD SUBFIELD */
      if (!(er123ddsfld(fp,status))) return(0);
   }

   /* RETURN SUCCESS */
   return (1);

} 
