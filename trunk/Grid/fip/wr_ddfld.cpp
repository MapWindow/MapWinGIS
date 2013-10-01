# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: WR123DDFLD 
** 
**    PURPOSE: TO WRITE A DATA DESCRIPTIVE FIELD TO AN OUTPUT BUFFER TO 
**              BE PHYSICALLY WRITTEN WHEN THE RECORD IS COMPLETE 
** 
**    INVOCATION METHOD:  WR123DDFLD(FP,TAG,WR_STR,OPTION)
** 
**    ARGUMENT LIST: 
**     NAME          TYPE     USE     DESCRIPTION
**     FP            PTR       I      FILE POINTER
**     TAG[]         PTR       I      FIELD IDENTIFIER
**     WR_STR[]      PTR       I      STRING CONTAINING A DDR FIELD
**     OPTION        INT       I      WRITE FIELD OPTION
**                                     0 = FAILURE
**                                     1 = OKAY
**                                     2 = START OF RECORD
**                                     3 = END OF RECORD
**                                     4 = END OF FILE
**                                     5 = END OF FIELD
**                                     6 = START OF FIELD
**     WR123DDFLD()  LOGICAL   O      SUCCESS FLAG
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     CKI123NFLD()     CHECKS STRING FOR NULL FIELDS AND SEPARATES DELIMITERS
**     ER123DDSFLD()    ERASES PREVIOUS SUBFIELD IN A DATA DESCRIPTIVE RECORD
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     IS123INTGR()     DETERMINES IF CHARACTER STRING IS AN INTEGER
**     STR123TOK()      RETURNS A POINTER TO A STRING TOKEN
**     WR123DDSFLD()    WRITES THE NEXT DATA DESCRIPTIVE SUBFIELD 
** 
**    INTERNAL VARIABLES: 
**     NAME        TYPE               DESCRIPTION
**     CURSF[]     PTR                CURRENT SUBFIELD
**     FCNTRL[10]  CHAR               FIELD CONTROL
**     INT_TAG     INT                TAG IN INTEGER FORMAT
**     LEN         INT                LENGTH OF INPUT STRING
**     SBFOPT      INT                SUBFIELD WRITE OPTION
**                                     0 = FAILURE
**                                     1 = OKAY
**                                     2 = START OF RECORD
**                                     3 = END OF RECORD
**                                     4 = END OF FILE (DR)
**                                     5 = END OF FIELD
**                                     6 = START OF FIELD
**     STATUS      INT                STATUS INDICATING SUBFIELD ERASED WHICH
**                                     HAS THE SAME VALUES RANGE AS SUBFIELD
**                                     WRITE OPTION
**                                
**     SUB_STR[]   PTR                SUBFIELD STRING
**     TOK_LEN     LONG               LENGTH OF TOKEN RETURNED FROM STR123TOK
**   
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT    N/A   DATA DESCRIPTIVE RECORD LENGTH
**      ILEVEL          INT     I    INTERCHANGE LEVEL
**      LEAD_ID         CHAR   N/A   LEADER IDENTIFIER
**      ICE_IND         CHAR   N/A   INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR   N/A   RESERVE SPACE CHARACTER
**      APPL_IND        CHAR   N/A   APPLICATION INDICATOR
**      FD_CNTRL_L      INT    N/A   FIELD CONTROL LENGTH
**      DA_BASE         INT    N/A   BASE ADDRESS OF DESCRIPTIVE AREA
**      CCS_IND[4]      CHAR   N/A   CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION
**      S_RESV          INT    N/A   RESERVED DIGIT
**      S_TAG           INT    N/A   SIZE OF FIELD TAG
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
**      DL_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE LEADER
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
**                                    STRUCTURE
**     GLB_STR[MAXSIZ]  CHAR   I/O   GLOBAL STRING BUFFER 
** 
**    GLOBAL CONSTANTS:  
**     NAME        TYPE             DESCRIPTION
**     DEL_STR[3]  CHAR             CHARACTER STRING CONTAINING THE UNIT AND
**                                   FIELD TERMINATORS
**     LVL23FCL    INT              FIELD CONTROL LENGTH FOR FILE INTERCHANGE
**                                   LEVELS 2 AND 3
**     NC          CHAR             NULL CHARACTER
**     SEP_STR[2]  CHAR             CHARACTER STRING CONTAINING SEPARATOR
**                                   FOR ADJACENT DELIMITERS
**     UT          CHAR             UNIT TERMINATOR (US) 1/15
**
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     A. DEWITT                 04/23/90  INITIAL PROLOG 
**     P. HODGES                 05/16/90  INITIAL PDL 
**     P. HODGES                 06/08/90  INITIAL CODE
**     L. MCMILLION              09/19/90  MODIFIED CODE TO PROCESS FIELD
**                                          CONTROL SUBFIELD USING ADDED LENGTH
**                                           IN ARGUMENT LIST
**     L. MCMILLION              10/09/90  MODIFIED CODE TO PROCESS USER
**                                          AUGMENTED FILE DESCRIPTION
**     L. MCMILLION              10/23/90  MODIFIED CODE TO SET SUBFIELD OPTION
**                                          TO START OF FIELD FOR LEVEL 1 FILE
**                                          AFTER INVOKING ER123DDSFLD()
**     L. MCMILLION              10/31/90  REMOVED FIELD CONTROL LENGTH FROM
**                                          FROM ARGUMENT LIST AND USING THE
**                                          CONSTANT FIELD CONTROL LENGTH
**     L. MCMILLION              01/21/91  ADDED STATUS TO ARGUMENT LIST OF
**                                          FUNCTION ER123DDSFLD()
**     L. MCMILLION  (TASK #40)  08/28/91  MODIFIED CODE WHICH CHECKS OPTION 
**                                          VALUE TO RETURN FAILURE IF OPTION
**                                          IS INVALID ( < 0 OR > 6)
**     L. MCMILLION  92DR010     03/31/92  ADDED CONDITIONAL TO SET SUBFIELD
**                                          OPTION TO START OF FIELD AFTER
**                                          AFTER INVOKING ER123DDSFLD() FOR
**                                          TAG 0..2
**     J. TAYLOR     92DR005     05/20/92  CHANGED CALLING SEQUENCE OF       
**                                          STR123TOK TO RETURN TOKEN LENGTH
**     J. TAYLOR     92DR012     10/29/92  MODIFIED TO ALLOW WRITING OF DATA
**                                          DESCRIPTIVE RECORD WITH NO NAME
**                                          SUBFIELD
**
**    PDL: 
** 
**     INITIALIZE ERASURE STATUS TO FAILURE
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER 
**     IF INPUT OPTION IS FAILURE OR OUT OF BOUNDS
**     THEN 
**        RETURN FAILURE 
**     ENDIF 
**     GET LENGTH OF INPUT STRING
**     ALLOCATE SPACE FOR TEMPORARY SUB_STR
**     INITIALIZE SUB_STR TO NULL CHARACTER
**     COPY INPUT STRING TO SUB_STR
**     CALL CKI123NFLD() TO ENSURE THAT DELIMITERS IN SUBFIELD STRING ARE NOT
**      ADJACENT TO EACH OTHER
**     IF INPUT OPTION IS START OF RECORD
**     THEN
**        SET SUBFIELD WRITE OPTION TO START OF RECORD
**     ELSE
**        SET SUBFIELD WRITE OPTION TO START OF FIELD
**     ENDIF
**     IF TAG IS INTEGER
**     THEN
**        CONVERT TAG CHARACTER STRING TO INTEGER FORMAT
**     ENDIF
**     IF TAG NOT USER AUGMENTED FILE DESCRIPTION AND FILE INTERCHANGE LEVEL IS
**      NOT 1
**     THEN
**        GET FIELD CONTROL IN SUBFIELD STRING
**        CALL WR123DDSFLD() TO WRITE FIELD CONTROL
**        UPDATE BEGINNING OF SUBFIELD STRING 
**        SET SUBFIELD WRITE OPTION TO OKAY 
**     ENDIF
**     IF SUBSTRING CONTAINS NO NAME SUBFIELD
**     THEN
**        WRITE DD SUBFIELD FOR BLANK NAME
**     ENDIF
**     
**     CALL STR123TOK() TO RETRIEVE NEXT SUBFIELD FROM STRING
**     WHILE STR123TOK() DOES NOT RETURN NULL STRING DO
**        CALL WR123DDSFLD() WITH CURRENT SUBFIELD
**        SET OLD SUBFIELD TO CURRENT SUBFIELD
**        CALL STR123TOK() TO DIVIDE STRING
**        SET CURRENT SUBFIELD TO POINTER RETURNED FROM STR123TOK()
**     ENDWHILE
** 
**     CALL ER123DDSFLD() TO ERASE PREVIOUS SUBFIELD SO THAT IT CAN BE
**      WRITTEN WITH CORRECT OPTION
**     IF INPUT OPTION IS END OF FILE 
**     THEN 
**        SET SUBFIELD WRITE OPTION TO END OF FILE
**     ELSEIF INPUT OPTION IS END OF RECORD 
**     THEN 
**        SET SUBFIELD WRITE OPTION TO END OF RECORD
**     ELSE
**        IF FILE INTERCHANGE LEVEL IS 1 OR SUBFIELD TO REWRITE IS USER
**         AUGMENTED FILE DESCRIPTION
**        THEN
**           SET SUBFIELD WRITE OPTION TO START OF FIELD
**        ELSE
**           SET SUBFIELD WRITE OPTION TO END OF FIELD
**        ENDIF
**     ENDIF
**     CALL WR123DDSFLD() WITH OLD SUBFIELD
** 
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
*****************************************************************************/
#include "stc123.h"

int wr123ddfld(FILE *fp,char *tag,char *wr_str,int option)

{

   /* LOCAL VARIABLES */
   char   *cursf    ;
   char   *sub_str  ;
   char   *tmp_str  ;   /* charactor pointer to the begining of sub_str */
   char   fcntrl[10];
   int    sbfopt    ;
   int    status = 0;
   long   int_tag   ;
   long   tok_len   ;
   size_t len       ;
    
   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */ 
   if (!get123level(fp)) return(0);

   /* IF INPUT OPTION IS FAILURE OR OUT OF BOUNDS, RETURN FAILURE */
   if ((option <= 0) || (option > 6)) return(0);

   /* GET LENGTH OF INPUT STRING */
   len = (size_t) _tcslen(wr_str) + 1;
   
   /* ALLOCATE SPACE FOR TEMPORARY STRING SUB_STR */
   if ((sub_str = (char *) malloc(len * sizeof(char))) == NULL) return(0);
   
   /* INITIALIZE SUBFIELD STRING TO NULL CHARACTER */
   *sub_str = NC;

   /* COPY INPUT STRING TO SUBFIELD STRING */
   strcpy(sub_str,wr_str);

   /* CALL CKI123NFLD() TO ENSURE THAT DELIMITERS IN SUBFIELD STRING ARE NOT
       ADJACENT TO EACH OTHER
   */
   if (!cki123nfld(&sub_str)) return(0);

   tmp_str = sub_str;   /* set tmp_str pointer to the begining of sub_str */

   /* IF INPUT OPTION IS START OF RECORD */
   if (option == 2) {

      /* SET SUBFIELD WRITE OPTION TO START OF RECORD */
      sbfopt = 2;
   }
   else {

      /* SET SUBFIELD WRITE OPTION TO START OF FIELD */
      sbfopt = 6;
   }

   /* IF TAG IS INTEGER, CONVERT TAG TO INTEGER FORMAT */
   if (is123intgr(tag)) {
      int_tag = atoi(tag);
   }
   else {
      int_tag = -1;
   }

   /* IF TAG NOT USER AUGMENTED FILE DESCRIPTION AND FILE INTERCHANGE LEVEL IS
       NOT 1
   */
   if (int_tag != 2 && cur_fm->dl_hd->ilevel != 1) {

      /* GET FIELD CONTROL IN SUBFIELD STRING */
      strncpy(fcntrl,sub_str,(size_t) LVL23FCL);
      fcntrl[LVL23FCL] = NC;

      /* CALL WR123DDSFLD() TO WRITE FIELD CONTROL */
      if (!wr123ddsfld(fp,tag,fcntrl,sbfopt)) return(0);

      /* UPDATE BEGINNING OF SUBFIELD STRING */
      sub_str += LVL23FCL;

      /* SET SUBFIELD WRITE OPTION TO OKAY */
      sbfopt = 1;
   }

   /* IF SUBSTRING CONTAINS NO NAME SUBFIELD */
   if (sub_str[0] == UT) {

      /* WRITE DD SUBFIELD FOR BLANK NAME */
      cursf = SEP_STR;
      if (!wr123ddsfld(fp,tag,cursf,sbfopt)) return(0);
   }

   /* CALL STR123TOK() TO RETRIEVE NEXT SUBFIELD FROM STRING */
   cursf = str123tok(&sub_str,DEL_STR,&tok_len);
   
   /* WHILE STR123TOK() DOES NOT RETURN NULL STRING DO */
   while (cursf != NULL) {

      /* CALL WR123DDSFLD() WITH CURRENT SUBFIELD */
      if (!wr123ddsfld(fp,tag,cursf,sbfopt)) return(0);

      /* SET OLD SUBFIELD TO CURRENT SUBFIELD */
      strcpy(glb_str,cursf);

      /* CALL STR123TOK() TO DIVIDE STRING AND SET CURRENT SUBFIELD TO RESULT
          OF STR123TOK()
      */
      cursf = str123tok(&sub_str,DEL_STR,&tok_len);

   }
 
   /* CALL ER123DDSFLD() TO ERASE PREVIOUS SUBFIELD SO THAT IT CAN BE
       WRITTEN WITH CORRECT OPTION
   */
   if (!er123ddsfld(fp,&status)) return(0);

   /* IF INPUT OPTION IS END OF FILE */
   if (option == 4) { 

      /* SET SUBFIELD WRITE OPTION TO END OF FILE */
      sbfopt = 4;
   }
   /* ELSEIF INPUT OPTION IS END OF RECORD */
   else if (option == 3) {

      /* SET SUBFIELD WRITE OPTION TO END OF RECORD */
      sbfopt = 3;
   }
   else {

      /* IF FILE INTERCHANGE LEVEL IS 1 OR SUBFIELD TO REWRITE IS USER
          AUGMENTED FILE DESCRIPTION
      */
      if (cur_fm->dl_hd->ilevel == 1 || int_tag == 2) {

         /* SET SUBFIELD WRITE OPTION TO START OF FIELD */
         sbfopt = 6;
      }
      else {

         /* SET SUBFIELD WRITE OPTION TO END OF FIELD */
         sbfopt = 5;
      }
   }

   /* CALL WR123DDSFLD() WITH OLD SUBFIELD */
   if (!wr123ddsfld(fp,tag,glb_str,sbfopt)) return(0);
 
   /* RETURN SUCCESS */
   free (tmp_str);  /* free up tmp_str */
   return (1);
}
