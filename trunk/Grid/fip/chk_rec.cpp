# include "stdafx.h"
/***************************************************************************
** 
**    INVOCATION NAME: CHK123REC 
** 
**    PURPOSE: TO GET DESCRIPTION FOR LAST READ OR WRITTEN RECORD
** 
**    INVOCATION METHOD: CHK123REC(FP,RECLEN,LEADID,DESCR) 
** 
**    ARGUMENT LIST: 
**     NAME         TYPE      USE      DESCRIPTION
**     FP           PTR        I       FILE POINTER
**     RECLEN       INT        O       RECORD LENGTH
**     LEADID[]     PTR        O       LEADER IDENTIFIER
**     DESCR[]      PTR        O       DESCRIPTION
**     CHK123REC()  LOGICAL    O       SUCCESS FLAG
**
**     NOTE: DESCRIPTION WILL BE IN THE FOLLOWING FORMAT 
** 
**        TAG<UT>FD_CNTRL<UT>NAME<UT>LABELS<UT>FORMAT<FT>... 
**        TAG<UT>... 
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     CHK123FLD()      GETS DESCRIPTION FOR LAST READ SUBFIELD 
**     CMP123DRDIR()    COMPUTES DIRECTORY INFORMATION FOR DATA RECORD 
**     CMP123DRLEAD()   COMPUTES INFORMATION NEEDED IN DATA RECORD LEADER 
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER 
**     RET123MATCH()    FINDS THE ASSOCIATED DATA DESCRIPTIVE RECORD MATCH TO
**                       THE DATA RECORD
**
**    INTERNAL VARIABLES: 
**     NAME         TYPE         DESCRIPTION 
**     CUR_TG       PTR          CURRENT POINTER TO TAG_LIST STRUCTURE
**     FDCNTRL[10]  CHAR         FIELD CONTROL
**     FDLEN[10]    CHAR         FIELD LENGTH
**     FDNAME[]     PTR          FIELD NAME
**     FDTAG[10]    CHAR         FIELD TAG 
**     FOUND        LOGICAL      MATCHING TAG FOUND FLAG 
**     NEW_TG       PTR          TAG_LIST STRUCTURE POINTER
**     TG_HD        PTR          HEAD POINTER TO TAG_LIST STRUCTURE
** 
**    LOCAL STRUCTURE: 
**     TAG_LIST STRUCTURE 
**      NAME       TYPE               DESCRIPTION 
**      TAG_PTR    PTR                POINTER TO DD ENTRIES 
**      NEXT       PTR                POINTER TO NEXT TAG_PTR 
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
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD 
**      FD_LEN          INT    N/A   LENGTH OF DISCRIPTIVE AREA DATA RECORD  
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA 
**                                    RECORD 
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS (NO LABELS) 
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSION LENGTHS 
**                                    (NO LABELS) 
**      VALUES          PTR    N/A   HEAD POINTER TO DATA VALUE SUBFIELD 
**                                    RECORDS 
**      NEXT            PTR     I    POINTER TO NEXT DATA RECORD  
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
**      DR_HD           PTR     I    HEAD POINTER TO DATA RECORD STRUCTURE 
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE 
**      RL_HD           PTR     I    HEAD POINTER TO DATA RECORD LEADER 
**                                    STRUCTURE 
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK 
**                                    STRUCTURE 
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE ENTRY 
**      CUR_DM          PTR    N/A   CURRENT POINTER TO DIMENSION LENGTHS 
**                                    STRUCTURE ENTRY 
**      CUR_DR          PTR    I/O   CURRENT POINTER TO DATA RECORD STRUCTURE 
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
**      LEAD_ID         CHAR    I    LEADER IDENTIFIER 
**      S_RESV[6]       CHAR   N/A   RESERVED SPACE
**      DA_BASE         INT    N/A   BASE ADDRESS OF DATA AREA   
**      R_SPACE[4]      CHAR   N/A   RESERVED SPACE CHARACTERS
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH   
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION  
**      SP_RSRV         INT    N/A   RESERVED DIGIT 
**      S_TAG           INT    N/A   SIZE OF FIELD TAG  
** 
**    GLOBAL VARIABLES: 
**     NAME             TYPE   USE   DESCRIPTION 
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT 
**                                    STRUCTURE ENTRY 
**     GLB_STR[MAXSIZ]  CHAR    I    GLOBAL CHARACTER STRING USED FOR
**                                    PROCESSING
**     GLB_STR2[MAXSIZ] CHAR    I    SECOND GLOBAL CHARACTER STRING USED FOR 
**                                    PROCESSING
** 
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION
**     FT_STR[2]        CHAR         CHARACTER STRING CONTAINING THE FIELD
**                                    TERMINATOR
**     NC               CHAR         NULL CHARACTER
**     UT_STR[2]        CHAR         CHARACTER STRING CONTAINING THE UNIT
**                                    TERMINATOR
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/15/90  INITIAL PROLOG 
**     P. HODGES                 05/15/90  INITIAL PDL 
**     P. HODGES                 06/11/90  INITIAL CODE
**     L. MCMILION               11/06/90  MODIFIED LOGIC TO CREATE LOCAL TAG
**                                          LIST IN SAME ORDER AS TAGS IN DD
**                                          STRUCTURE LIST AND IMPLEMENTED
**                                          FUNCTION RET123MATCH() FOR TAG
**                                          SEARCH IN DD STRUCTURES
**     L. MCMILLION  TASK #40    12/20/91  ADDED LOGIC TO INSERT TAG VALUES
**                                          INTO DESCRIPTION
**     L. MCMILLION  TASK #40    11/30/92  PROLOG UPDATE
** 
**    PDL: 
** 
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER
**     IF OPEN MODE IS WRITE
**     THEN
**        IF LEADER EXISTS
**        THEN
**           CALL CMP123DRDIR() TO COMPUTE DIRECTORY INFORMATION FOR DATA RECORD
**           CALL CMP123DRLEAD() TO COMPUTE INFORMATION NEEDED IN DATA RECORD LEADER
**        ENDIF
**     ENDIF
**     ALLOCATE TAG_LIST STRUCTURE
**     INITIALIZE POINTERS TO NULL
**     SET TAG_LIST HEAD POINTER TO ALLOCATED TAG_LIST STRUCTURE { DUMMY }
**     SET CUR_DR TO DR_HD
**     WHILE CUR_DR NEXT NOT NULL DO
**        SET FOUND TO FALSE 
**        SET CUR_DR TO CUR_DR NEXT 
**        SET CURRENT TAG_LIST POINTER TO HEAD OF TAG_LIST
**        WHILE TAG_LIST NEXT POINTER NOT NULL AND MATCHING TAG NOT FOUND DO
**           INCREMENT TAG_LIST POINTER TO TAG_LIST NEXT 
**           IF TAG_LIST POINTER TO DD_TAG MATCHES CUR_DR TAG 
**           THEN 
**              SET FOUND TO TRUE 
**           ENDIF 
**        ENDWHILE
** 
**        IF NOT FOUND 
**        THEN 
**           CALL RET123MATCH() TO FIND THE ASSOCIATED DATA DESCRIPTIVE RECORD
**            MATCH TO THE DATA RECORD
**           ALLOCATE NEW TAG_LIST ENTRY
**           APPEND NEW ENTRY INTO TAG_LIST
**           SET TAG_LIST POINTER TO CUR_DD
**           SET CURRENT TAG_LIST NEXT TO NULL
**        ENDIF 
**     ENDWHILE
** 
**     INITIALIZE DESCRIPTION TO NULL CHARACTER
**     SET CURRENT TAG_LIST POINTER TO HEAD TAG_LIST NEXT
**     WHILE CURRENT TAG_LIST POINTER NOT NULL DO
**        SET CUR_DD TO CURRENT TAG_LIST POINTER DD ENTRY 
**        CALL CHK123FLD() TO GET DESCRIPTION OF LAST READ FIELD 
**        SET STRING TO CONCATENATION OF STRING AND CALLING SEQUENCE 
**         RETURNED FROM CHK123FLD() { COMPLETE WITH DELIMITERS } 
**        FREE FIELD NAME
**        SET CURRENT TAG_LIST POINTER TO NEXT ENTRY IN LIST
**     ENDWHILE 
** 
**     SET RECLEN TO REC_LEN IN RL 
**     SET LEADID TO LEAD_ID IN RL 
** 
**     { DELETE LOCAL STRUCTURE } 
**     SET CURRENT TAG_LIST POINTER TO HEAD TAG_LIST 
**     WHILE HEAD TAG_LIST POINTER NOT NULL DO
**        SET CURRENT TAG_LIST POINTER TO CURRENT TAG_LIST NEXT 
**        FREE HEAD TAG_LIST NEXT 
**        SET HEAD TAG_LIST NEXT TO CURRENT TAG_LIST 
**     ENDWHILE 
** 
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int chk123rec (FILE *fp,long *reclen,char *leadid,char *descr)
{
   /* INTERNAL VARIABLES */
   char fdcntrl[10];
   char fdlen[10];
   char *fdname;
   char fdtag[10];
   int found;

   struct tag_list {
                    struct dd *tag_ptr;
                    struct tag_list *next;
                    };
   struct tag_list *cur_tg;
   struct tag_list *new_tg;
   struct tag_list *tg_hd;

   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */
   if(!get123level(fp)) return(0);

   /* IF OPEN_MODE IS WRITE */
   if (cur_fm->open_mode == 'w') {

      /* IF LEADER EXISTS */
      if (!cur_fm->nld) {

         /* CALL CMP123DRDIR() TO COMPUTE DIRECTORY INFORMATION
             FOR DATA RECORD
         */
         if (!cmp123drdir()) return(0);

         /* CALL CMP123DRLEAD() TO COMPUTE INFORMATION NEEDED
             IN DATA RECORD LEADER
         */
         if (!cmp123drlead()) return(0);
      }
   }

   /* ALLOCATE TAG_LIST STRUCTURE */
   if ((new_tg = (struct tag_list *) malloc(sizeof(struct tag_list))) == NULL)
    return(0);

   /* INITIALIZE POINTERS TO NULL */
   new_tg->tag_ptr = NULL;
   new_tg->next = NULL;

   /* SET TAG_LIST HEAD POINTER TO ALLOCATED TAG_LIST STRUCTURE { DUMMY } */
   tg_hd = new_tg;

   /* SET CUR_DR TO DR_HD */
   cur_fm->cur_dr = cur_fm->dr_hd;

   /* WHILE CUR_DR NEXT NOT NULL DO */
   while (cur_fm->cur_dr->next != NULL) {

      /* SET FOUND TO FALSE */
      found = 0;

      /* SET CUR_DR TO CUR_DR NEXT */
      cur_fm->cur_dr = cur_fm->cur_dr->next;

      /* SET CURRENT TAG_LIST POINTER TO HEAD OF TAG_LIST */
      cur_tg = tg_hd;

      /* WHILE TAG_LIST NEXT POINTER NOT NULL AND MATCHING TAG NOT FOUND DO */
      while ((cur_tg->next != NULL) && (!found)) {

         /* INCREMENT TAG_LIST POINTER TO TAG_LIST NEXT */
         cur_tg = cur_tg->next;

         /* IF TAG_LIST POINTER TO DD_TAG MATCHES CUR_DR TAG,
             SET FOUND TO TRUE
         */
         if (!strcmp(cur_tg->tag_ptr->tag,cur_fm->cur_dr->tag)) found = 1;
      }
 
      /* IF NOT FOUND */
      if (!found) {

         /* CALL RET123MATCH() TO FIND THE ASSOCIATED DATA DESCRIPTIVE RECORD
             MATCH TO THE DATA RECORD
         */
         if (!ret123match(cur_fm->cur_dr->tag)) return(0);

         /* ALLOCATE NEW TAG_LIST ENTRY */
         if ((new_tg = (struct tag_list *) malloc(sizeof(struct tag_list))) == NULL) return(0);

         /* APPEND NEW ENTRY INTO LIST */
         cur_tg->next = new_tg;
         cur_tg = new_tg;

         /* SET TAG_LIST POINTER TO CUR_DD */
         cur_tg->tag_ptr = cur_fm->cur_dd;

         /* SET CURRENT TAG_LIST NEXT TO NULL */
         cur_tg->next = NULL;
      }   
   }

   /* INITIALIZE DESCRIPTION TO NULL CHARACTER */
   descr[0] = NC;

   /* SET CURRENT TAG_LIST POINTER TO HEAD TAG_LIST NEXT */
   cur_tg = tg_hd->next;
 
   /* WHILE CURRENT TAG_LIST POINTER NOT NULL DO */
   while (cur_tg != NULL) {

      /* SET CUR_DD TO CURRENT TAG_LIST POINTER DD ENTRY */
      cur_fm->cur_dd = cur_tg->tag_ptr;

      /* CALL CHK123FLD() TO GET DESCRIPTION OF LAST READ FIELD */
      if (!chk123fld(fp,fdtag,fdlen,&fdname,fdcntrl,glb_str,glb_str2)) return(0);

      /* SET STRING TO CONCATENATION OF STRING AND CALLING SEQUENCE     
          RETURNED FROM CHK123FLD() { COMPLETE WITH DELIMITERS }
      */
      strcat(descr,fdtag);
      strcat(descr,UT_STR);
      strcat(descr,fdcntrl);
      strcat(descr,UT_STR);
      strcat(descr,fdname);
      strcat(descr,UT_STR);
      strcat(descr,glb_str2);
      strcat(descr,UT_STR);
      strcat(descr,glb_str);
      strcat(descr,FT_STR);

      /* FREE FIELD NAME */
      free(fdname);

      /* SET CUR_TG TO CUR_TG NEXT */
      cur_tg = cur_tg->next;
   } 
    
   /* SET RECLEN TO REC_LEN IN RL */
   *reclen = cur_fm->rl_hd->rec_len;

   /* SET LEADID TO LEAD_ID IN RL */
   *leadid = cur_fm->rl_hd->lead_id;

   /* { DELETE LOCAL STRUCTURE } */
   
   /* SET CURRENT TAG_LIST POINTER TO HEAD TAG_LIST */
   cur_tg = tg_hd;

   /* WHILE HEAD TAG_LIST POINTER NOT NULL DO */
   while (tg_hd != NULL) {
 
      /* SET CURRENT TAG_LIST POINTER TO CURRENT TAG_LIST NEXT */
      cur_tg = cur_tg->next;

      /* FREE HEAD TAG_LIST NEXT */
      free(tg_hd);

      /* SET HEAD TAG_LIST NEXT TO CURRENT TAG_LIST */
      tg_hd = cur_tg;
   }

   /* RETURN SUCCESS */
   return(1);
} 
