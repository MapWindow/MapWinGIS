# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RD123REC
**
**    PURPOSE: TO READ THE NEXT RECORD
**
**    INVOCATION METHOD: RD123REC(FP,STRING,STR_LEN,STATUS)
**
**    ARGUMENT LIST:
**     NAME        TYPE      USE      DESCRIPTION
**     FP          PTR        I       FILE POINTER
**     STRING[]    PTR        O       RECORD READ    
**     STR_LEN     LONG       O       LENGTH OF STRING
**     STATUS      INT        O       STATUS
**                                     0 = FAILURE
**                                     1 = OKAY
**                                     2 = START OF RECORD
**                                     3 = END OF RECORD
**                                     4 = END OF FILE
**                                     5 = END OF FIELD
**                                     6 = START OF FIELD       
**     RD123REC()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     END123REC()      COMPLETE PREVIOUS RECORD
**     G123FSTR()       READS A CHARACTER STRING
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     LD123REC()       LOAD A DATA RECORD INTO STRUCTURE
**     SET123STAT()     SET STATUS FLAG
**
**    INTERNAL VARIABLES:
**     NAME        TYPE               DESCRIPTION
**     R_LEN       INT                LENGTH OF RECORD 
**     START_POS   LONG               FILE POSITION OF BEGINNING OF RECORD
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
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                    STRUCTURE
**      DR_HD           PTR    N/A   HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR     I    HEAD POINTER TO DATA RECORD LEADER
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
**      NLD             INT     I    NO LEADER FLAG
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
**     DATA RECORD LEADER STRUCTURE (RL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT     I    DATA RECORD LENGTH     
**      RESV_SP         CHAR   N/A   RESERVED SPACE CHARACTER
**      LEAD_ID         CHAR   N/A   LEADER IDENTIFIER
**      S_RESV[6]       CHAR   N/A   RESERVED SPACE
**      DA_BASE         INT     I    BASE ADDRESS OF DATA AREA  
**      R_SPACE[4]      CHAR   N/A   RESERVED SPACE CHARACTERS
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH  
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION 
**      SP_RSRV         INT    N/A   RESERVED DIGIT
**      S_TAG           INT    N/A   SIZE OF FIELD TAG 
**
**    GLOBAL VARIABLES:
**     NAME            TYPE   USE   DESCRIPTION
**     CUR_FM          PTR    I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                   STRUCTURE ENTRY 
**
**    GLOBAL CONSTANTS:
**     NAME            TYPE         DESCRIPTION
**     NC              CHAR         NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/17/90  INITIAL PROLOG
**     P. HODGES                 05/17/90  INITIAL PDL
**     A. DEWITT                 05/26/90  MODIFY PDL - LOGIC FOR REPETITIVE
**                                          CHARACTERISTICS OF SUBSEQUENT DR
**                                          LEADERS AND DIRECTORIES
**     A. DEWITT                 05/26/90  INITIAL CODE
**     L. MCMILLION  92DR008     03/31/92  REMOVED '- 1' FROM RECORD LENGTH
**                                          CALCULATION FOR RECORD HAVING
**                                          NEITHER LEADER NOR DIRECTORY
**     J. TAYLOR     92DR005     05/16/92  CHANGED CALLING SEQUENCE TO RETURN
**                                          STRING LENGTH
**     L. MCMILLION  TASK #40    12/08/92  UPDATED PROLOG/PDL
**
**    PDL:
**
**     INITIALIZE STATUS TO FAILURE
**     INITIALIZE STRING TO NULL CHARACTER
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LEVEL
**     IF NO LEADER FLAG INDICATES NO LEADER OR DIRECORY EXIST FOR NEXT RECORD
**     THEN
**        RETRIEVE RECORD LENGTH FROM PREVIOUSLY READ RECORD
**        CALCULATE TRUE RECORD LENGTH WHICH EXCLUDES LEADER AND DIRECTORY
**        CALL END123REC() TO CLEAR OUT PREVIOUSLY USED DATA RECORD
**        SAVE STARTING POSITION OF RECORD
**        CALL LD123REC() TO LOAD DATA RECORD INTO STRUCTURE
**        RETURN TO STARTING POSITION OF RECORD LOADED
**        CALL G123FSTR() TO READ RECORD AS A STRING
**     ELSE { LEADER AND DIRECTORY EXIST FOR NEXT DATA RECORD }
**        CALL END123REC() TO CLEAR OUT PREVIOUSLY USED DATA RECORD   
**        SAVE STARTING POSITION OF RECORD   
**        CALL LD123REC() TO LOAD DATA RECORD INTO STRUCTURE
**        GO TO BEGINNING OF RECORD
**        ACCESS RECORD LENGTH FROM LEADER STRUCTURE   
**        CALL G123FSTR() TO READ RECORD AS A STRING   
**     ENDIF
**
**     CALL SET123STAT() TO SET STATUS FLAG
**     IF STATUS FLAG IS NOT EOF 
**     THEN
**        SET STATUS FLAG TO OKAY
**     ENDIF
**     SET STRING LENGTH TO RECORD LENGTH
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/

#include "stc123.h"

int rd123rec(FILE *fp,char *string,long *str_len,int *status)
{
   /* VARIABLE DECLARATIONS */
   long  r_len      ;
   long  start_pos  ;   

   /* INITIALIZE STATUS TO FAILURE */
   *status = 0;
   
   /* INITIALIZE STRING TO NULL CHARACTER */
   *string = NC;
   
   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LEVEL */
   if (!get123level(fp)) return(0); 
 
   /* IF NO LEADER FLAG INDICATES NO LEADER EXISTS FOR NEXT RECORD */
   if (cur_fm->nld) {
   
      /* RETRIEVE RECORD LENGTH FROM PREVIOUSLY READ RECORD */
      r_len = cur_fm->rl_hd->rec_len;
   
      /* CALCULATE TRUE RECORD LENGTH - EXCLUDE LEADER AND DIRECTORY */
      r_len = r_len - cur_fm->rl_hd->da_base;
      
      /* CALL END123REC() TO CLEAR OUT PREVIOUSLY USED DATA RECORD */
      if (!end123rec(fp)) return(0); 
   
      /* SAVE STARTING POSITION OF RECORD */
      if ((start_pos = ftell(fp)) == -1L) return(0);
      
      /* CALL LD123REC() TO LOAD DATA RECORD INTO STRUCTURE */
      if (!ld123rec()) return(0);
      
      /* RETURN TO STARTING POSITION OF RECORD LOADED */
      if (fseek(fp,start_pos,SEEK_SET)) return(0);
      
      /* CALL G123FSTR() TO READ RECORD AS A STRING */
      if (!g123fstr(fp,string,r_len)) return(0);
         
   }  

   /* NO LEADER FLAG INDICATES LEADER EXISTS FOR NEXT RECORD */
   else {

      /* CALL END123REC() TO CLEAR OUT PREVIOUSLY USED DATA RECORD */
      if (!end123rec(fp)) return(0); 

      /* SAVE STARTING POSITION OF RECORD */
      if ((start_pos = ftell(fp)) == -1L) return (0);      

      /* CALL LD123REC() TO LOAD DATA RECORD INTO STRUCTURE */
      if (!ld123rec()) return(0);
         
      /* GO TO BEGINING OF RECORD */
      if (fseek(fp,start_pos,SEEK_SET)) return(0);         

      /* ACCESS RECORD LENGTH FROM LEADER STRUCTURE */
      r_len = cur_fm->rl_hd->rec_len;
         
      /* CALL G123FSTR() TO READ RECORD AS A STRING */
      if (!g123fstr(fp,string,r_len)) return(0);
         
   };   

   /* CALL SET123STAT() TO SET STATUS FLAG */
   if (!set123stat(fp,status)) return(0);
   
   /* IF NOT END OF FILE SET STATUS TO OKAY */
   if (*status != 4) *status = 1;

   /* SET STRING LENGTH TO RECORD LENGTH */
   *str_len = r_len;
      
   /* RETURN SUCCESS */
   return(1);
}
