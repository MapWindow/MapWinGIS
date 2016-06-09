# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: CH123SIZE 
**
**    PURPOSE: TO RESIZE A FILE BY TRUNCATING THE FILE OR EXTENDING THE FILE 
**             WITH NULL CHARACTERS 
**
**    INVOCATION METHOD: CH123SIZE(FP,FILE_LEN) 
**
**    ARGUMENT LIST:
**     NAME          TYPE      USE      DESCRIPTION
**     FP            PTR        I       POINTER TO FILE POINTER
**     FILE_LEN      INT        I       NEW FILE LENGTH
**     CH123SIZE()   LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     STR123TOK()      SEPARATES A DELIMITED INPUT STRING INTO STRING
**                      TOKENS 
**
**    INTERNAL VARIABLES:
**     NAME          TYPE      DESCRIPTION
**     CHPTR         PTR       CHARACTER POINTER TO STRING TOKENS
**     DATA_SECT     LDIV_T    DATA SECTION
**     NAME          PTR       FILE NAME STRING
**     NEWNAME       PTR       CHARACTER POINTER TO NEW FILE NAME
**     REM_DATA      LONG      REMAINING DATA
**     TMPNAME       PTR       CHARACTER POINTER TO NEW FILE NAME
**     TMPNPTR       PTR       CHARACTER POINTER TO STRING TOKEN
**     TMP_FP        PTR       TEMPORARY FILE POINTER
**     TOK_LEN       LONG      LENGTH OF STRING TOKEN RETURNED FROM STR123TOK()
** 
**    GLOBAL REFERENCES:
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR     O    FILE POINTER
**      F_NAME[]        PTR     O    EXTERNAL FILE NAME
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
**     NAME             TYPE     USE   DESCRIPTION
**     CUR_FM           PTR      I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                      STRUCTURE ENTRY
**     GLB_STR[MAXSIZ]  CHAR     I/O   GLOBAL CHARACTER STRING USED FOR
**                                      PROCESSING
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE           DESCRIPTION
**     MAXSIZ           INT            MAXIMUM SIZE         
**     NC               CHAR           NULL CHARACTER
**     FP_LEN           INT            FILE NAME AND PATH LENGTH
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 08/24/91  INITIAL PROLOG
**     A. DEWITT                 08/24/91  INITIAL PDL
**     A. DEWITT                 08/24/91  INITIAL CODE
**     L. MCMILLION  92DR005     05/20/92  ADDED TOKEN LENGTH ARGUMENT TO
**                                          STR123TOK() CALL AND CLOSED
**                                          TEMPORARY FILE BEFORE RENAMING
**     J. TAYLOR     92DR017     10/15/92  REMOVED CHARACTER POINTER BUFFER
**                                          TO ALLEVIATE DOS MEMORY ALLOCATION
**                                          FAILURE
**     J. TAYLOR     TASK 55     01/20/93  ADDED CONDITIONAL COMPILATION AROUND
**                                          PARSING OF TEMPORARY FILE NAME TO
**                                          REMOVE THE PATH NAME
**     J. TAYLOR     92DR016     01/21/93  CHANGED EXPRESSION COMPARING LDIV 
**                                          REMAINDER TO NULL SO THAT IT
**                                          COMPARES TO ZERO INSTEAD
**     J. TAYLOR     TASK 55     02/15/93  ADDED CONDITIONAL COMPILATION AROUND
**                                          FOPEN TO AND ADDED RECFM FOR MVS
**     J. TAYLOR     TASK 55     03/16/93  MODIFIED RECORD FORMAT FOR MVS TO   
**                                          MATCH DBO
**     J. TAYLOR     TASK #55    04/05/93  MODIFIED RECORD FORMAT FOR MVS TO   
**                                          ALLOW RECORD FORMAT SPECIFICATION
**                                          TO BE SPECIFIED OUTSIDE OF FIPS123
**                                          LIBRARY
**                                         
**
**    PDL:
**
**     INITIALIZE REMAINING DATA TO ZERO
**     INITIALIZE TOKEN LENGTH TO ZERO
**     ALLOCATE AND INITIALIZE NAME STRINGS
**     SET TMPNAME TO STARTING LOCATION OF NEWNAME
**     REWIND INPUT FILE
**     GENERATE A UNIQUE FILE NAME
**     COPY NAME TO NEWNAME
**     CALL STR123TOK() TO REMOVE PATH FROM NAME
**     SET TEMPORARY NAME POINTER TO FIRST TOKEN STRING
**     WHILE MORE TOKEN STRINGS RETURNED DO
**        SET TEMPORARY NAME STRING POINTER TO CHARACTER POINTER
**     ENDWHILE
**     COPY FILE NAME WITHOUT THE PATH PREFIX TO NAME STRING
**     OPEN A NEW FILE WITH NEWLY CREATED NAME
**     DETERMINE NUMBER OF NEEDED FILE PARTITIONS
**     IF THERE ARE DATA SMALLLER THAN MAXSIZ PARTITIONS REMAINING 
**     THEN
**        COMPUTE NUMBER OF CHARACTERS IN REMAINING DATA SECTIONS
**     ENDIF
**     WHILE DATA PARTITIONS OF MAXSIZ EXIST DO
**        INITIALIZE GLB_STR TO NULL
**        IF QUOTIENT IS GREATER THAN ZERO
**        THEN
**           READ BLOCK OF DATA INTO GLB_STR 
**           WRITE BLOCK TO NEW FILE
**        ELSE
**           READ REMAINING BLOCK OF DATA INTO GLB_STR
**           WRITE BLOCK TO NEW FILE
**        ENDIF
**        DECREMENT QUOTIENT COUNTER
**      ENDWHILE
**      CLOSE INPUT FILE
**      ERASE INPUT FILE
**      CLOSE NEW FILE
**      RENAME NEW FILE TO INPUT FILE NAME
**      FREE NAME SPACE
**      OPEN NEW FILE
**      SET FP TO CURRENT FILE POINTER VALUE
**      RETURN SUCCESS
**
*****************************************************************************
**      CODE SECTION
**
*****************************************************************************/
#include "stc123.h"

int ch123size(FILE **fp, long file_len) 
{
   /* LOCAL VARIABLES */
   FILE *tmp_fp;
   ldiv_t data_sect;
   long rem_data = 0;
   long tok_len = 0;
   char *name;
   char *newname;
   char *chptr;
   char *tmpnptr;
   char *tmpname;
  
   
   /* ALLOCATE AND INITIALIZE NAME STRINGS */
   if ((name = (char *) malloc(FP_LEN*sizeof(char))) == NULL) return(0);
   if ((newname = (char *) malloc(FP_LEN*sizeof(char))) == NULL) {
       return(0);
   };
   
   *name = NC;
   *newname = NC;

   /* SET TMPNAME TO STARTING POSITION OF NEWNAME */
   tmpname = newname;

   /* REWIND INPUT FILE */
   rewind(*fp);

   /* GENERATE A UNIQUE FILE NAME */
   if (tmpnam(name) == NULL) return(0);
  
#if ! MVS 
   /* COPY NAME TO NEW NAME */
   strcpy(newname,name);

   /* CALL STR123TOK() TO REMOVE PATH FROM NAME */
   if ((chptr = str123tok(&newname,"/\\",&tok_len)) == NULL) return(0);

   /* SET TEMPORARY NAME POINTER TO FIRST TOKEN STRING */
   tmpnptr = chptr;

   /* WHILE MORE TOKEN STRINGS RETURNED DO */
   while ((chptr = str123tok(&newname,"/\\",&tok_len)) != NULL) {
      
      /* SET TEMPORARY NAME STRING POINTER TO CHARACTER POINTER */
      tmpnptr = chptr;
   }
    
   /* COPY FILE NAME WITHOUT THE PATH PREFIX TO NAME STRING */
   strcpy(name,tmpnptr);
#endif
     
   /* OPEN NEW FILE */
#if MVS
   if (( tmp_fp = fopen(name,"wb+,recfm=*")) 
       == NULL) return(0);
#else
   if (( tmp_fp = fopen(name,"wb+")) == NULL) return(0);
#endif
    
   /* DETERMINE NUMBER OF NEEDED FILE PARTITIONS */
   data_sect = ldiv(file_len,MAXSIZ);
    
   /* IF THERE ARE DATA SMALLER THAN MAXSIZ PARTITIONS REMAINING */
   if (data_sect.rem != 0) {
     
      /* COMPUTE NUMBER OF CHARACTERS IN REMAINING DATA SECTION */
      rem_data = file_len - (data_sect.quot * MAXSIZ);
    
   }
   
   /* WHILE DATA PARTITIONS OF MAXSIZ EXIST DO */
   while (data_sect.quot >= 0) {
    
      /* INITIALIZE GLB_STR TO NULL STRING */
      *glb_str = NC;
    
      /* IF QUOTIENT IS GREATER THAN ZERO */
      if (data_sect.quot > 0) {
         
         /* READ BLOCK OF DATA INTO GLB_STR */
         if (fread(glb_str,sizeof(char),(size_t)MAXSIZ,*fp) != (size_t)MAXSIZ) return(0);
        
         /* WRITE BLOCK TO NEW FILE */
         if (fwrite(glb_str,sizeof(char),MAXSIZ,tmp_fp) != (size_t)MAXSIZ) return(0);
      }
      else {
            
         /* READ BLOCK OF DATA INTO GLB_STR */
         if (fread(glb_str,sizeof(char),(size_t)rem_data,*fp) != (size_t) rem_data) return(0);
        
         /* WRITE BLOCK TO NEW FILE */
         if (fwrite(glb_str,sizeof(char),(size_t)rem_data,tmp_fp) != (size_t)rem_data) return(0);
      };

      /* DECREMENT QUOTIENT COUNTER */
      data_sect.quot--;
      
   };
    
   /* CLOSE INPUT FILE */
   fclose(*fp);

   /* ERASE INPUT FILE */
   if (remove(cur_fm->f_name)) return(0);

   /* CLOSE NEW FILE */
   fclose(tmp_fp);

   /* RENAME NEW FILE TO INPUT FILE NAME */
   if (rename(name,cur_fm->f_name) < 0) return(0);

   /* FREE NAME STRINGS */
   free(tmpname);
   free(name);

   /* OPEN NEW FILE */
   if (( cur_fm->fp = fopen(cur_fm->f_name,"rb+")) == NULL) return(0);

   /* SET FP TO CURRENT FILE POINTER VALUE */
   *fp = cur_fm->fp;

   /* RETURN SUCCESS */
   return(1);
}
