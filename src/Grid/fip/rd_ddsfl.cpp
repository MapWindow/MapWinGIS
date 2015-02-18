# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RD123DDSFLD 
**
**    PURPOSE: TO RETRIEVE DESCRIPTIVE RECORD SUBFIELD 
**
**    INVOCATION METHOD: RD123DDSFLD(FP,TAG,RD_STR,STATUS)
**
**    ARGUMENT LIST:
**     NAME           TYPE      USE      DESCRIPTION
**     FP             PTR        I       FILE POINTER
**     TAG[]          CHAR       O       TAG
**     RD_STR[]       PTR        O       SUBFIELD STRING
**     STATUS         INT        O       STATUS
**                                        0 = FAILURE
**                                        1 = OKAY
**                                        2 = START OF RECORD
**                                        3 = END OF RECORD
**                                        4 = END OF FILE
**                                        5 = END OF FIELD
**                                        6 = START OF FIELD
**     RD123DDSFLD()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     BLD123ADSCR()    BUILDS A DDR ARRAY DESCRIPTOR SUBFIELD STRING FROM
**                       THE DD AND DM STRUCTURES
**     BLD123FMT()      BUILDS A FORMAT STRING FROM THE FORMAT CONTROL
**                       STRUCTURE
**     BLD123LAB()      BUILDS A LABEL STRING FROM THE DATA DESCRIPTIVE
**                       RECORD LABEL SUBFIELD STRUCTURE
**     GET123LEVEL()    RETRIEVES APPROPRIATE LEVEL FROM FILE MANAGEMENT
**                       STRUCTURE
**     IS123INTGR()     DETERMINES IF A STRING IS AN INTEGER
**     LD123DDREC()     READS THE DATA DESCRIPTION RECORD AND LOADS THE
**                       INFORMATION INTO THE DATA STRUCTURE FOR REFERENCE
**     SET123STAT()     CHECKS FOR END OF FILE AND SETS STATUS FLAG
**
**    INTERNAL VARIABLES:
**     NAME        TYPE    DESCRIPTION
**     CUR_TL      PTR     POINTER TO CURRENT TAG LIST
**     INT_TAG     INT     TAG IN INTEGER FORMAT
**  
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE FILE CONTROL RECORD STRUCTURE (CR)
**      NAME            TYPE   USE   DESCRIPTION
**      F_TITLE         CHAR    I    FILE TITLE
**      TAG_L           PTR     I    POINTER TO LIST OF TAG PAIR RECORDS
**      U_AFD[]         PTR     I    CHARACTER STRING POINTER TO USERS 
**                                    AUGMENTED FILE DESCRIPTION
**      
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA 
**                                    FIELD RECORD
**      FD_CNTRL[10]    CHAR    I    FIELD CONTROLS
**      NAME[]          PTR     I    CHARACTER STRING POINTER TO NAME
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS IN ARRAY DESCRIPTOR
**      DIM_LPTR        PTR     I    HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT    N/A   NUMBER OF ELEMENTS IN PRIMARY DIMENSION
**      LABELS          PTR    N/A   HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR    I/O   POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
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
**      CR_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE
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
**      SF_STATE_DD     INT    I/O   SUBFIELD STATE (DD)
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
**     DATA DESCRIPTIVE TAG PAIR STRUCTURE (TL) 
**      NAME            TYPE   USE   DESCRIPTION
**      TAG_1[10]       CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      TAG_2[10]       CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      NEXT            PTR     I    POINTER TO DATA DESCRIPTIVE TAG PAIR
**                                    STRUCTURE
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR    I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS:
**     NAME       TYPE      DESCRIPTION
**     FCDSTYPE   INT       RELATIVE POSITION OF FIELD CONTROL DATA STRUCTURE
**                           TYPE IN THE DATA DESCRIPTIVE FIELDS
**     FCDTYPE    INT       RELATIVE POSITION OF FIELD CONTROL DATA TYPE IN
**                           THE DATA DESCRIPTIVE FIELDS
**     NC         CHAR      NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/09/90  INITIAL PROLOG
**     P. HODGES                 05/09/90  INITIAL PDL
**     P. HODGES                 06/01/90  INITIAL CODE
**     A. DEWITT                 06/23/90  CORRECT STATE LOGIC AND TAG-LIST
**                                          TRAVERSAL LOGIC
**     A. DEWITT                 08/22/90  ADDED LOGIC TO LOAD DDR IF NOT
**                                          LOADED PRIOR TO THIS FUNCTION
**     L. MCMILLION              10/17/90  ADDED LOGIC TO RETURN EOF STATUS IF
**                                          LD123DDREC() FAILED
**     L. MCMILLION  TASK #40    12/22/92  UPDATED PROLOG/PDL/INLINE COMMENTS
**                                          PER QC
**     J. TAYLOR     93DR021     02/08/93  CHANGED OUTPUT PARAMETER TAG FROM 
**                                          A 10 CHARACTER STRING TO CHARACTER
**                                          POINTER
**     L. MCMILLION  93DR033     06/17/93  ADDED CALL TO FUNCTION BLD123ADSCR()
**                                          TO RETRIEVE ARRAY DESCRIPTOR FROM
**                                          THE STRUCTURES
**
**    PDL:
**
**     CALL GET123LEVEL() TO DETERMINE APPROPRIATE FM ENTRY
**     INITIALIZE STRING TO NULL
**     IF DD_HD NOT NULL
**     THEN
**        IF DD_HD NEXT IS NULL
**        THEN
**           CALL LD123DDREC() TO PERFORM RECORD INPUT AND LOAD INTO STRUCTURE
**           IF LD123DDREC() ENCOUNTERED EOF
**           THEN
**              SET STATUS TO EOF
**              RETURN FAILURE
**           ENDIF
**           RETURN FAILURE
**        ENDIF
**     ELSE
**        CALL LD123DDREC() TO PERFORM RECORD INPUT AND LOAD INTO STRUCTURE
**        IF LD123DDREC() ENCOUNTERED EOF
**        THEN
**           SET STATUS TO EOF
**           RETURN FAILURE
**        ENDIF
**     ENDIF
**
**     INITIALIZE STATUS TO OKAY
**     IF STATE OUTSIDE POSSIBLE RANGE
**     THEN
**        IF NEXT FIELD NOT NULL
**        THEN
**           INCREMENT CURRENT FIELD TO NEXT FIELD
**           SET STATE TO FIELD CONTROL SUBFIELD
**        ELSE
**           SET STATUS TO FAILURE
**           RETURN FAILURE
**        ENDIF
**     ENDIF
**
**     IF INTERCHANGE LEVEL IS ONE
**     THEN
**        SET SUBFIELD STATE TO START OF RECORD
**     ENDIF
**  
**     IF TAG IS ALL NUMERIC { IS123INTGR() }
**     THEN
**        CONVERT TAG TO INTEGER TAG
**     ELSE
**        SET INTEGER TAG TO NEGATIVE ONE
**     ENDIF
**     IF TAG IS FILE DESCRIPTION (0..2)
**     THEN
**        IF USER AUGMENTED FILE DESCRIPTION IS PRESENT
**        THEN
**           SET OUTPUT STRING TO U_AFD
**        ENDIF
**        SET STATE TO FINISHED
**        SET STATUS TO END OF FIELD
**     ELSE
**        CASE STATE
**        1 (FIELD CONTROL SUBFIELD):
**           SET STRING TO FIELD CONTROL
**           IF FIRST FIELD IN RECORD
**           THEN
**              SET STATUS TO START OF RECORD
**           ELSE
**              SET STATUS TO START OF FIELD
**           ENDIF
**           INCREMENT STATE
**        2 (NAME SUBFIELD):
**           IF TAG IS FILE CONTROL (0..0)
**           THEN
**              IF FILE TITLE IS PRESENT
**              THEN
**                 SET STRING TO FILE TITLE
**              ENDIF
**              IF INTERCHANGE LEVEL IS 3
**              THEN
**                 INCREMENT STATE
**              ELSE
**                 SET STATE TO FINISHED
**                 SET STATUS TO END OF FIELD
**              ENDIF
**           ELSE
**              IF NAME FIELD PRESENT        
**              THEN
**                 SET STRING TO NAME 
**              ENDIF
**              IF INTERCHANGE LEVEL IS 2 OR 3
**              THEN
**                 IF NOT ELEMENTARY DATA STRUCTURE
**                 THEN
**                    SET STATE TO LABEL SUBFIELD
**                 ELSE
**                    IF DATA TYPE IS BIT FIELD
**                    THEN
**                       SET STATE TO FORMATS SUBFIELD
**                    ELSE
**                       SET STATE TO FINISHED
**                       SET STATUS TO END OF FIELD
**                    ENDIF
**                 ENDIF
**              ELSE { INTERCHANGE LEVEL IS LEVEL 1 }
**                 IF FIELD NEXT POINTER NOT NULL
**                 THEN
**                    SET STATUS TO END OF FIELD
**                 ELSE
**                    CALL SET123STAT() TO CHECK FOR END OF FILE
**                    IF RETURNED STATUS NOT END OF FILE
**                    THEN
**                       SET STATUS TO END OF RECORD
**                    ENDIF
**                 ENDIF
**                 SET STATE TO FINISHED
**              ENDIF
**           ENDIF
**        3 (LABELS SUBFIELD):
**           IF TAG IS FILE CONTROL (0..0)
**           THEN
**              IF TAG PAIRS LIST PRESENT
**              THEN
**                 INITIALIZE CURRENT TAG PAIR POINTER FOLLOWING DUMMY IN LIST
**                 WHILE HAVE NOT REACHED END OF TAG PAIR LIST DO
**                    SET STRING TO CONCATENATED STRING, TAG1, TAG2
**                    SET CURRENT TAG PAIR TO NEXT FIELD
**                 END WHILE
**              ENDIF
**              SET STATUS TO END OF FIELD
**              SET STATE TO FINISHED
**           ELSE
**              IF ARRAY DESCRIPTOR PRESENT
**              THEN
**                 CALL BLD123ADSCR() TO BUILD ARRAY DESCRIPTOR STRING
**              ELSE LABELS MAYBE PRESENT
**                 CALL BLD123LAB() TO BUILD LABELS STRING
**              ENDIF
**              SET STRING TO STRING RETURNED FROM BUILD FUNCTIONS
**              SET STATE TO FORMATS SUBFIELD
**           ENDIF
**        4 (FORMATS SUBFIELD):   
**           CALL BLD123FMT() TO BUILD FORMAT STRING
**           SET STRING TO FORMAT STRING
**           IF FIELD NEXT POINTER NOT NULL
**           THEN
**              SET STATUS TO END OF FIELD
**           ELSE
**              CALL SET123STAT() TO CHECK FOR END OF FILE
**              IF RETURNED STATUS NOT END OF FILE
**              THEN
**                 SET STATUS TO END OF RECORD
**              ENDIF
**           ENDIF
**           INCREMENT STATE
**        END CASE
**     ENDIF
**     SET OUTPUT TAG TO TAG FIELD
**
**     RETURN SUCCESS
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int rd123ddsfld(FILE *fp,char *tag,char *rd_str,int *status)
{
   /* INTERNAL VARIABLES */
   long int_tag     ;
   struct tl *cur_tl;

   /* CALL GET123LEVEL() TO DETERMINE APPROPRIATE FM ENTRY */
   if (!get123level(fp)) return(0);

   /* INITIALIZE STRING TO NULL */
   *rd_str = NC;

   /* IF DD_HD NOT NULL */
   if (cur_fm->dd_hd != NULL) {

      /* IF DD_HD NEXT IS NULL */
      if (cur_fm->dd_hd->next == NULL) {

         /* CALL LD123DDREC() TO PERFORM RECORD INPUT AND LOAD INTO
             STRUCTURE
         */
         if (!ld123ddrec()) {

            /* IF LD123DDREC() ENCOUNTERED EOF, SET STATUS TO EOF */
            if (feof(fp)) *status = 4;

            /* RETURN FAILURE */
            return(0);
         }
      }
   }
   else {

      /* CALL LD123DDREC() TO PERFORM RECORD INPUT AND LOAD INTO STRUCTURE */
      if (!ld123ddrec()) {

         /* IF LD123DDREC() ENCOUNTERED EOF, SET STATUS TO EOF */
         if (feof(fp)) *status = 4;

         /* RETURN FAILURE */
         return(0);
      }
   }

   /* INITIALIZE STATUS TO OKAY */
   *status = 1;

   /* IF STATE OUTSIDE POSSIBLE RANGE */
   if (cur_fm->sf_state_dd < 1 || cur_fm->sf_state_dd > 4) {

      /* IF NEXT FIELD NOT NULL */
      if (cur_fm->cur_dd->next != NULL) {

         /* INCREMENT CURRENT FIELD TO NEXT FIELD */
         cur_fm->cur_dd = cur_fm->cur_dd->next;

         /* SET STATE TO FIELD CONTROL SUBFIELD */
         cur_fm->sf_state_dd = 1;
      }
      else {

         /* SET STATUS TO FAILURE */
         *status = 0;

         /* RETURN FAILURE */
         return(0);
      }
   }

   /* IF INTERCHANGE LEVEL IS ONE */
   if (cur_fm->dl_hd->ilevel == 1) {
   
      /* SET SUBFIELD STATE TO 2 {NAME SUBFIELD} */
      cur_fm->sf_state_dd = 2;
   }
   
   /* IF TAG STRING IS ALL NUMERIC { IS123INTGR(), CONVERT TO INTEGER } */
   if (is123intgr(cur_fm->cur_dd->tag)) {
      int_tag = atol(cur_fm->cur_dd->tag);
   }

   /* ELSE SET INTEGER TAG TO NEGATIVE ONE */
   else {
      int_tag = -1;
   }

   /* IF TAG IS FILE DESCRIPTION (0..2) */
   if (int_tag == 2) {

      /* IF USER AUGMENTED FILE DESCRIPTION IS PRESENT */
      if (cur_fm->cr_hd != NULL && cur_fm->cr_hd->u_afd != NULL) {

         /* SET OUTPUT STRING TO U_AFD */
         strcpy(rd_str,cur_fm->cr_hd->u_afd);
      }

      /* SET STATE TO FINISHED */
      cur_fm->sf_state_dd = 5;

      /* SET STATUS TO END OF FIELD */
      *status = 5;
   }
   else {

      /* CASE STATE */
      switch (cur_fm->sf_state_dd) {

         /* 1 (FIELD CONTROL SUBFIELD): */
         case 1:

            /* SET STRING TO FIELD CONTROL */
            strcpy(rd_str,cur_fm->cur_dd->fd_cntrl);

            /* IF FIRST FIELD IN RECORD */
            if (cur_fm->cur_dd == cur_fm->dd_hd->next) {

               /* SET STATUS TO START OF RECORD */
               *status = 2;
            }
            else {

               /* SET STATUS TO START OF FIELD */
               *status = 6;
            }

            /* INCREMENT STATE */
            cur_fm->sf_state_dd++;
         break;

         /* 2 (NAME SUBFIELD): */
         case 2:

            /* IF TAG IS FILE CONTROL (0..0) */
            if (int_tag == 0) {

               /* IF FILE TITLE PRESENT */
               if (cur_fm->cr_hd != NULL && cur_fm->cr_hd->f_title != NULL) {

                  /* SET STRING TO FILE TITLE */
                  strcpy(rd_str,cur_fm->cr_hd->f_title);
               }

               /* IF INTERCHANGE LEVEL IS 3 */
               if (cur_fm->dl_hd->ilevel == 3) {

                  /* INCREMENT STATE */
                  cur_fm->sf_state_dd++;
               }
               else {

                  /* SET STATE TO FINISHED */
                  cur_fm->sf_state_dd = 5;

                  /* SET STATUS END OF FIELD */
                  *status = 5;
               }
            }
            else {
            
               /* IF NAME FIELD PRESENT */
               if (cur_fm->cur_dd->name != NULL) {
            
                  /* SET STRING TO NAME */
                  strcpy(rd_str,cur_fm->cur_dd->name);
               }

               /* IF INTERCHANGE LEVEL IS 2 OR 3 */
               if (cur_fm->dl_hd->ilevel > 1) {
               
                  /* IF NOT ELEMENTARY DATA STRUCTURE */
                  if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] != '0'){
                     
                     /* SET STATE TO LABEL SUBFIELD */
                     cur_fm->sf_state_dd++;
                  }
                  else{
                     
                     /* IF DATA TYPE IS BIT FIELD */
                     if (cur_fm->cur_dd->fd_cntrl[FCDTYPE] == '5'){

                        /* SET STATE TO FORMAT SUBFIELDS */
                        cur_fm->sf_state_dd = 4;
                     }
                     else{

                        /* SET STATE TO FINISHED */
                        cur_fm->sf_state_dd = 5;

                        /* SET STATUS TO END OF FIELD */
                        *status = 5;
                     };
                  }
               }
               else{

                  /* IF FIELD NEXT POINTER IS NOT NULL */
                  if (cur_fm->cur_dd->next != NULL) {

                     /* SET STATUS TO END OF FIELD */
                     *status = 5;
                  }
                  else {

                     /* CALL SET123STAT() TO CHECK FOR END OF FILE */
                     if (!set123stat(fp,status)) return(0);

                     /* IF RETURNED STATUS NOT END OF FILE */
                     if (*status != 4) {

                        /* SET STATUS TO END OF RECORD */
                        *status = 3;
                     }
                  }

                  /* SET STATE TO FINISHED */
                  cur_fm->sf_state_dd = 5;
               };   
                       
            }
         break;

         /* 3 (LABELS SUBFIELD): */
         case 3:
      
            /* IF TAG IS FILE CONTROL (0..0) */
            if (int_tag == 0) {

               /* IF TAG PAIR LIST PRESENT */
               if (cur_fm->cr_hd != NULL && cur_fm->cr_hd->tag_l != NULL) {

                  /* INITIALIZE CURRENT TAG PAIR TO POINTER FOLLOWING DUMMY
                      IN LIST
                  */
                  cur_tl = cur_fm->cr_hd->tag_l->next;
               
                  /* DO FOR EACH TAG PAIR */
                  while(cur_tl != NULL) {
               
                     /* SET STRING TO CONCATENATED STRING, TAG1, TAG2 */
                     strcat(rd_str,cur_tl->tag_1);
                     strcat(rd_str,cur_tl->tag_2);
               
                     /* SET CURRENT TAG PAIR TO NEXT FIELD */
                     cur_tl = cur_tl->next;
                  }
               }
               /* SET STATUS TO END OF FIELD */
               *status = 5;

               /* SET STATE TO FINISHED */
               cur_fm->sf_state_dd = 5;
            }
            else {

               /* IF ARRAY DESCRIPTOR PRESENT */
               if (cur_fm->cur_dd->dim_lptr != NULL) {

                   /* CALL BLD123ADSCR() TO BUILD ARRAY DESCRIPTOR STRING */
                   if (!bld123adscr(rd_str)) return(0);
               }
               
               /* ELSE LABELS MAYBE PRESENT */
               else

                  /* CALL BLD123LAB() TO BUILD LABELS STRING */
                  if (!bld123lab(rd_str)) return(0);

               /* SET STATE TO FORMATS SUBFIELD */
               cur_fm->sf_state_dd = 4;
            }
         break;

         /* 4 (FORMATS SUBFIELD):    */
         case 4:

            /* CALL BLD123FMT() TO BUILD FORMAT STRING */
            if (!bld123fmt(rd_str)) return(0);

            /* IF FIELD NEXT POINTER NOT NULL */
            if (cur_fm->cur_dd->next != NULL) {

               /* SET STATUS TO END OF FIELD */
               *status = 5;
            }
            else {

               /* CALL SET123STAT() TO CHECK FOR END OF FILE */
               if (!set123stat(fp,status)) return(0);

               /* IF RETURNED STATUS NOT END OF FILE */
               if (*status != 4) {

                  /* SET STATUS TO END OF RECORD */
                  *status = 3;
               }
            }

            /* INCREMENT STATE */
            cur_fm->sf_state_dd++;
         break;
      }
   }

   /* SET OUTPUT TAG TO TAG FIELD */
   strcpy (tag,cur_fm->cur_dd->tag);
   
   /* RETURN SUCCESS */
   return(1);
}
