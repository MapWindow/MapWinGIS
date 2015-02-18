# include "stdafx.h"
/***************************************************************************
**
**     INVOCATION NAME: BLD123FMT
**     
**     PURPOSE:  TO BUILD FORMAT STRING FROM THE FORMAT CONTROL STRUCTURE
**
**     INVOCATION METHOD: BLD123FMT(STRING)
**
**     ARGUMENT LIST: 
**      NAME          TYPE     USE      DESCRIPTION
**      STRING[]      PTR       O       FORMATTED STRING
**      BLD123FMT()   LOGICAL   O       SUCCESS FLAG
**
**     EXTERNAL FUNCTION REFERENCES:
**      NAME             DESCRIPTION
**      OUT123FMT()      WRITES THE FORMAT CONTROL STRUCTURE
** 
**     INTERNAL VARIABLES:
**     NAME        TYPE              DESCRIPTION
**     CH          CHAR              CHARACTER FOR RETRIEVING FORMAT
**     INDEX       INT               INDEX INTO FORMAT STRING
**     TMP_PTR     PTR               POINTER TO TEMPORARY FILE
**
**     GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA 
**                                    FIELD RECORD
**      FD_CNTRL[10]    CHAR   N/A   FIELD CONTROLS
**      NAME[]          PTR    N/A   CHARACTER STRING POINTER TO NAME
**      LABELS          PTR    N/A   HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR     I    ROOT POINTER TO FORMAT CONTROLS BINARY
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
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS: 
**     NAME             TYPE         DESCRIPTION
**     NC               CHAR         NULL CHARACTER
** 
**     CHANGE HISTORY:
**      AUTHOR        CHANGE-ID     DATE     CHANGE SUMMARY
**      A. DEWITT                 04/23/90   INITIAL PROLOG
**      P. HODGES                 04/23/90   INITIAL PDL
**      P. HODGES                 06/05/90   INITIAL CODE
**      L. MCMILLION  TASK #40    12/18/92   UPDATED PROLOG/PDL PER QC
**
**     PDL:
**
**      INITIALIZE INDEX TO ZERO
**      CALL TMPFILE() TO OPEN TEMPORARY FILE FOR BUILDING FORMAT
**      CALL OUT123FMT() TO WRITE THE FORMAT CONTROL STRUCTURE
**      CALL REWIND() TO REWIND TEMPORARY FILE
**      READ TEMPORARY FILE AS A CHARACTER STRING
**      APPEND NULL CHARACTER TO STRING
**      CALL FCLOSE() TO CLOSE AND DELETE THE TEMPORARY FILE
**
**      RETURN SUCCESS
**
****************************************************************************
**      CODE SECTION
**
****************************************************************************/
#include "stc123.h"

int bld123fmt(char *string)

{
   /* INTERNAL VARIABLES */
   FILE *tmp_ptr;
   char ch;
   long index = 0;

   /* CALL TMPFILE() TO OPEN TEMPORARY FILE FOR BUILDING FORMAT */
   if ((tmp_ptr = tmpfile()) == NULL) return(0);

   /* CALL OUT123FMT() TO WRITE THE FORMAT CONTROL STRUCTURE */
   if (!out123fmt(tmp_ptr,cur_fm->cur_dd->fmt_rt)) return(0);

   /* CALL REWIND() TO REWIND TEMPORARY FILE */
   rewind(tmp_ptr);

   /* READ TEMPORARY FILE AS A CHARACTER STRING */
   ch = (char)fgetc(tmp_ptr);
   while(!feof(tmp_ptr)) {
      string[index++] = ch;
      ch = (char)fgetc(tmp_ptr);
   }

   /* APPEND NULL CHARACTER TO STRING */
   string[index] = NC;
   
   /* CALL FCLOSE() TO CLOSE AND DELETE THE TEMPORARY FILE */
   if (fclose(tmp_ptr)) return(0);

   /* RETURN SUCCESS */
   return(1);
}
