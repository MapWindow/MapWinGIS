# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: CHK123FLD 
** 
**    PURPOSE: TO RETRIEVE DESCRIPTION OF LAST FIELD READ OR WRITTEN 
** 
**    INVOCATION METHOD: CHK123FLD(FP,FDTAG,FDLEN,FDNAME,FDCNTRL,FMTS,LABS) 
** 
**    ARGUMENT LIST: 
**     NAME         TYPE      USE      DESCRIPTION
**     FP           PTR        I       FILE POINTER
**     FDTAG[]      PTR        O       FIELD TAG FROM DD STRUCTURE
**     FDLEN[]      PTR        O       FIELD LENGTH FROM DR STRUCTURE
**     FDNAME[]     PTR        O       FIELD NAME FROM DD STRUCTURE
**     FDCNTRL[]    PTR        O       FIELD CONTROLS FROM DD STRUCTURE
**     FMTS[]       PTR        O       FIELD FORMAT STRING FROM DD STRUCTURE
**     LABS[]       PTR        O       FIELD LABELS FROM DD STRUCTURE
**     CHK123FLD()  LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     BAK123FLD()      RETRIEVES FIELD JUST READ OR ERASES FIELD JUST WRITTEN
**     BLD123FMT()      BUILDS THE FORMAT STRING FROM THE FORMAT CONTROL
**                       STRUCTURE 
**     BLD123LAB()      BUILDS LABEL STRING FROM THE DDR LABEL SUBFIELD
**                       STRUCTURE 
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     I123TOA()        CONVERTS AN INTEGER TO A CHARACTER STRING
**     RD123FLD()       RETRIEVES A DATA RECORD FIELD
** 
**    INTERNAL VARIABLES:
**     NAME         TYPE               DESCRIPTION
**     LEADID       CHAR               LEADER IDENTIFIER
**     LEN          INT                LENGTH OF CHARACTER STRING
**     STATUS       INT                FIELD STATUS
**                                      0 = FAILURE
**                                      1 = OKAY
**                                      2 = START OF RECORD
**                                      3 = END OF RECORD
**                                      4 = END OF FILE
**                                      5 = END OF FIELD
**                                      6 = START OF FIELD
**     STR_LEN      LONG               LENGTH OF CHARACTER STRING
**     TAG[10]      CHAR               FIELD TAG
**   
**    GLOBAL REFERENCES:  
**
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA  
**                                    FIELD RECORD 
**      FD_CNTRL[10]    CHAR    I    FIELD CONTROLS
**      NAME[]          PTR     I    CHARACTER STRING POINTER TO NAME 
**      LABELS          PTR    N/A   HEAD POINTER TO A LINKED LIST CONTAINING 
**                                    LINKED LIST OF DD LABEL SUBFIELD 
**                                    STRUCTURES 
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY 
**                                    TREE STRUCTURE 
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE  
**                                    STRUCTURE 
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     I    LENGTH OF DISCRIPTIVE AREA DATA RECORD
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSION LENGTHS
**                                    (NO LABELS)
**      VALUES          PTR    N/A   HEAD POINTER TO DATA VALUE SUBFIELD
**                                    RECORDS
**      NEXT            PTR    N/A   POINTER TO NEXT DATA RECORD
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
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE ENTRY 
**      CUR_DM          PTR    N/A   CURRENT POINTER TO DIMENSION LENGTHS 
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
**     GLB_STR2[MAXSIZ] CHAR    I    GLOBAL CHARACTER STRING USED FOR READING
**                                    READING A DATA RECORD FIELD
** 
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION 
**     NC               CHAR         NULL CHARACTER
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/15/90  INITIAL PROLOG 
**     P. HODGES                 05/15/90  INITIAL PDL 
**     P. HODGES                 06/11/90  INITIAL CODE 
**     L. MCMILLION              11/29/90  CHANGED FIELD NAME ARGUMENT FROM A
**                                          CHARACTER POINTER TO A POINTER TO
**                                          DYNAMICALLY ALLOCATED SPACE
**     L. MCMILLION              02/14/91  ADDED LOGIC TO SET LENGTH OF FIELD
**                                          NAME TO 1 AND NOT COPY FIELD NAME
**                                          IF FIELD NAME IS NULL
**     L. MCMILLION              02/19/91  ADDED LOGIC TO BACK UP AND READ A
**                                          FIELD TO RETRIEVE THE LENGTH OF
**                                          THE FIELD WRITTEN
**     L. MCMILLION  (TASK# 40)  12/09/91  REPOSITIONED RESTORATION OF
**                                          OPEN_MODE TO IMMEDIATELY FOLLOW
**                                          BAK123FLD() INSTEAD OF RD123FLD()
**                                          AS SET123STAT() INTRODUCED ERRORS
**                                          WHEN REPOSITIONING INTERNAL FILE
**                                          POINTER FOR A WRITE FILE
**     J. TAYLOR     92DR005     05/16/92  CHANGE CALLING SEQUENCE TO RD123FLD
** 
**    PDL: 
** 
**     INITIALIZE STATUS TO FAILURE
**     CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER
**     SET OUTPUT FDTAG TO TAG IN CUR_DD 
**     IF OPEN MODE IS WRITE AND FD_LEN IN CUR_DR IS ZERO
**     THEN
**        TEMPORARILY SET OPEN MODE TO READ TO PREVENT ERASURE WHEN BAKING UP
**        CALL BAK123FLD() TO BACK UP TO BEGINNING OF WRITTEN FIELD
**        RESET OPEN MODE TO WRITE
**        CALL RD123FLD TO READ THE DR FIELD
**        SET FDLEN TO LENGTH OF FIELD JUST READ
**     ELSE
**        SET OUTPUT FDLEN TO FD_LEN IN CUR_DR
**     ENDIF
**     IF CUR_DD NAME NOT NULL
**     THEN
**        GET LENGTH OF NAME IN CUR_DD
**     ELSE
**        SET LENGTH TO 1
**     ENDIF
**     ALLOCATE SPACE FOR OUTPUT FDNAME
**     INITIALIZE OUTPUT FDNAME TO NULL CHARACTER
**     IF CUR_DD NAME NOT NULL
**     THEN
**        SET OUTPUT FDNAME TO NAME IN CUR_DD
**     ENDIF
**     SET OUTPUT FDCNTRL TO FD_CNTRL IN CUR_DD 
** 
**     CALL BLD123FMT() TO GET FORMAT STRING
**     SET OUTPUT FMTS TO FORMAT STRING 
**     CALL BLD123LAB() TO GET LABELS STRING
**     SET OUTPUT LABS TO LABELS STRING 
** 
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int chk123fld(FILE *fp,char *fdtag,char *fdlen,char **fdname,char *fdcntrl,char *fmts,char *labs)

{
   /* INTERNAL VARIABLES */
   char leadid;
   size_t len;
   int status = 0;
   char tag[10];
   long str_len;

   /* CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return(0);

   /* SET OUTPUT FDTAG TO TAG IN CUR_DD */
   strcpy(fdtag,cur_fm->cur_dd->tag);
         
   /* IF OPEN MODE IS WRITE AND FD_LEN IN CUR_DR IS ZERO */
   if (cur_fm->open_mode == 'w' && !cur_fm->cur_dr->fd_len) {
   
      /* TEMPORARILY SET OPEN MODE TO READ TO PREVENT ERASURE WHEN BACKING UP */
      cur_fm->open_mode = 'r';
      
      /* CALL BAK123FLD() TO BACK UP TO BEGINNING OF WRITTEN FIELD */
      if (!bak123fld(fp,&status)) return(0);
      
      /* RESET OPEN MODE TO WRITE */
      cur_fm->open_mode = 'w';
      
      /* CALL RD123FLD() TO READ THE DR FIELD */
      if (!rd123fld(fp,tag,&leadid,glb_str2,&str_len,&status)) return(0);
      
      /* SET FDLEN TO LENGTH OF FIELD JUST READ */
      i123toa((long) str_len,fdlen);
   }

   else {
         
      /* SET OUTPUT FDLEN TO FD_LEN IN CUR_DR */
      i123toa(cur_fm->cur_dr->fd_len,fdlen);
   }

   /* IF CUR_DD NAME NOT NULL */
   if (cur_fm->cur_dd->name != NULL)
   
      /* GET LENGTH OF NAME IN CUR_DD */
      len = _tcslen(cur_fm->cur_dd->name) + 1;
      
   else
   
      /* SET LENGTH TO 1 */
      len = 1;
   
   /* ALLOCATE SPACE FOR OUTPUT FDNAME */
   if ((*fdname = (char *) malloc(len * sizeof(char))) == NULL) return(0);
   **fdname = NC;
   
   /* IF CUR_DD NAME NOT NULL, SET OUTPUT FDNAME TO NAME IN CUR_DD */
   if (cur_fm->cur_dd->name != NULL) strcpy(*fdname,cur_fm->cur_dd->name);

   /* SET OUTPUT FDCNTRL TO FD_CNTRL IN CUR_DD */
   strcpy(fdcntrl,cur_fm->cur_dd->fd_cntrl);

   /* CALL BLD123FMT() TO GET FORMAT STRING */
   if(!bld123fmt(fmts)) return(0);

   /* CALL BLD123LAB() TO GET LABELS STRING */
   if(!bld123lab(labs)) return(0);

   /* RETURN SUCCESS */
   return(1);
} 
