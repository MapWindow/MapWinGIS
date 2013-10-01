# include "stdafx.h"
/***************************************************************************
** 
**    INVOCATION NAME: WR123DDSFLD 
** 
**    PURPOSE: TO WRITE A DATA DESCRIPTIVE SUBFIELD TO AN OUTPUT BUFFER 
**              TO BE PHYSICALLY WRITTEN WHEN THE RECORD IS COMPLETE 
** 
**    INVOCATION METHOD: WR123DDSFLD(FP,TAG,WR_STR,OPTION)
**
**    ARGUMENT LIST: 
**     NAME           TYPE      USE      DESCRIPTION
**     FP             PTR        I       FILE POINTER
**     TAG[]          PTR        I       FIELD IDENTIFIER
**     WR_STR[]       PTR        I       STRING CONTAINING A DDR FIELD
**     OPTION         INT        I       WRITE SUBFIELD OPTION
**                                        0 = FAILURE
**                                        1 = OKAY
**                                        2 = START OF RECORD
**                                        3 = END OF RECORD
**                                        4 = END OF FILE (DR)
**                                        5 = END OF FIELD
**                                        6 = START OF FIELD
**     WR123DDSFLD()  LOGICAL    O       SUCCESS FLAG
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     BEG123DDREC()    BEGIN A DATA DESCRIPTIVE RECORD 
**     GET123ADSCR()    RETRIEVES ARRAY DESCRIPTOR 
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     IS123ADSCR()     DETERMINES IF A CHARACTER STRING IS ARRAY DESCRIPTOR
**     IS123INTGR()     DETERMINES WHETHER OR NOT A CHARACTER STRING IS AN
**                       INTEGER
**     LOAD123FMT()     LOADS FORMATS INTO THE FORMAT TREE STRUCTURE 
**     LOAD123LAB()     LOADS LABELS INTO DD LABEL SUBFIELD STRUCTURE 
**     LOAD123TAGP()    LOADS TAG PAIRS INTO THE STRUCTURE 
** 
**    INTERNAL VARIABLES: 
**     NAME       TYPE               DESCRIPTION 
**     I_STR[]    PTR                INPUT STRING
**     INT_TAG    INT                INTEGER FORMAT OF TAG
**     LEN        INT                LENGTH OF CHARACTER STRING
**     NEW_CR     PTR                POINTER TO NEW DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**     NEW_DD     PTR                POINTER TO NEW DD ENTRY IN DATA 
**                                    DESCRIPTIVE RECORD STRUCTURE 
**     VALIDSFLD  LOGICAL            FLAG INDICATING THE SUBFIELD (WR_STR)
**                                    IS VALID, SPECIFICALLY NOT A
**                                    DELIMITER SEPARATOR.  ALSO USED TO
**                                    CALCULATE STRING LENGTHS FOR DYNAMIC
**                                    MEMORY ALLOCATION
**                                     0 (INVALID SUBFIELD) - NO ADDITIONAL
**                                       CHARACTER FOR NC
**                                     1 (VALID SUBFIELD) - REQUIRES
**                                        ADDITIONAL CHARACTER FOR NC
**   
**    GLOBAL REFERENCES: 
** 
**     DATA DESCRIPTIVE FILE CONTROL RECORD STRUCTURE (CR) 
**      NAME            TYPE   USE   DESCRIPTION 
**      F_TITLE         PTR     O    FILE TITLE 
**      TAG_L           PTR     O    POINTER TO LIST OF TAG PAIR RECORDS 
**      U_AFD[]         PTR     O    CHARACTER STRING POINTER TO USERS  
**                                    AUGMENTED FILE DESCRIPTION 
**       
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD) 
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR    O    INTERNAL NAME OF AN ASSOCIATED FIELD 
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD 
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA  
**                                    FIELD RECORD 
**      FD_CNTRL[10]    CHAR   I/O   FIELD CONTROLS 
**      NAME[]          PTR     O    CHARACTER STRING POINTER TO NAME 
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS IN ARRAY DESCRIPTOR
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT    N/A   NUMBER OF ELEMENTS IN PRIMARY DIMENSION
**      LABELS          PTR     O    HEAD POINTER TO A LINKED LIST CONTAINING 
**                                    LINKED LIST OF DD LABEL SUBFIELD 
**                                    STRUCTURES 
**      FMT_RT          PTR     O    ROOT POINTER TO FORMAT CONTROLS BINARY 
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
**      S_TAG           INT    I/O   SIZE OF FIELD TAG
** 
**     FILE MANAGEMENT STRUCTURE (FM) 
**      NAME            TYPE   USE   DESCRIPTION 
**      FP              PTR    N/A   FILE POINTER 
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME 
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE 
**      CR_HD           PTR    I/O   HEAD POINTER TO DATA DESCRIPTIVE FILE
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
**    GLOBAL VARIABLES: 
**     NAME             TYPE   USE   DESCRIPTION 
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT 
**                                    STRUCTURE ENTRY 
** 
**    GLOBAL CONSTANTS: 
**     NAME             TYPE         DESCRIPTION
**     FCDSTYPE         INT          RELATIVE POSITION OF THE DATA STRUCTURE 
**                                    TYPE WITHIN THE FIELD CONTROL
**     FCDTYPE          INT          RELATIVE POSITION OF THE DATA TYPE WITHIN
**                                    THE FIELD CONTROL 
**     NC               CHAR         NULL CHARACTER
**
**     SEP_STR[2]       CHAR         CHARACTER STRING CONTAINING SEPARATOR FOR
**                                    ADJACENT DELIMITERS
**
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     A. DEWITT                 05/17/90  INITIAL PROLOG 
**     A. DEWITT                 05/17/90  INITIAL PDL 
**     P. HODGES                 06/08/90  INITIAL CODE 
**     A. DEWITT                 06/24/90  UPDATE PDL/CODE REGARDING
**                                          LOGIC FOR SUBFIELD STATES AND
**                                          STATES AND STRING STORAGE
**     L. MCMILLION              09/05/90  MODIFIED PDL/CODE TO PROCESS DATA
**                                          CORRESPONDING TO FILE CONTROL TAG
**     L. MCMILLION              10/23/90  MODIFIED LOGIC TO PROCESS TAG 0..0
**                                          AND TAG 0..2 FOR END OF RECORD AND
**                                          END OF FILE STATES
**     L. MCMILLION  (TASK #40)  08/28/91  ADDED CHECK FOR VALID OPTION RANGE
**                                          AND ALLOCATE CR_HD IF NULL FOR 
**                                          END-OF-RECORD OR EOF OPTION
**     L. MCMILLION  92DR009     04/14/92  ADDED CONDITION TO CHECK FOR
**                                          DELIMITER SEPARATORS (WHEN
**                                          INVOKED BY WR123DDFLD())
**     L. MCMILLION  TASK #40    12/15/92  PROLOG/PDL UPDATE
**     L. MCMILLION  93DR033     06/16/93  ADDED LOGIC TO PROCESS DDR ARRAY
**                                          DESCRIPTOR AND STORE IN APPROPRIATE
**                                          STRUCTURE
**
**    PDL:
** 
**     CALL GET123LEVEL() TO RETURN BUFFER STRUCTURE FOR OUTPUT FILE
**     IF OPTION IS OUTSIDE VALID RANGE OF VALUES
**     THEN
**        RETURN FAILURE
**     ENDIF
**     IF TAG IS INTEGER
**     THEN
**        CONVERT TAG TO INTEGER
**     ELSE
**        SET INTEGER TAG TO NEGATIVE ONE
**     ENDIF
**     IF SF_STATE_DD OUT OF BOUNDS
**     THEN 
**        {INITIALIZE STATE} 
**        IF INTERCHANGE LEVEL IS 1
**        THEN 
**           SET SF_STATE_DD TO NAME SUBFIELD 
**        ELSE 
**           SET SF_STATE_DD TO FIELD CONTROL SUBFIELD 
**        ENDIF 
**     ENDIF
**
**     IF WR_STR IS NOT A DELIMITER SEPARATOR
**     THEN
**        SET VALID SUBFIELD FLAG TO VALID
**     ELSE
**        SET VALID SUBFIELD FLAG TO INVALID
**     ENDIF
**     CASE OPTION 
**        2,6: {START OF RECORD, START OF FIELD} 
**           IF OPTION IS START OF RECORD 
**           THEN 
**              CALL BEG123DDREC() TO BEGIN A DATA DESCRIPTIVE RECORD 
**           ENDIF 
**           ALLOCATE NEW_DD 
**           SET POINTERS TO NULL
**           SET FD_CNTRL TO NULL CHARACTER
**           SET TAG TO INPUT TAG 
**           SET NUMBER OF DIMENSIONS AND PRIMARY DIMENSION TO ZERO
**           SET CUR_DD NXT TO NEW_DD 
**           SET CUR_DD TO NEW_DD 
**           IF TAG IS FILE CONTROL (0..0)
**           THEN 
**              IF TAG SIZE EQUALS ZERO
**              THEN
**                 SET TAG SIZE TO LENGTH OF INPUT TAG
**              ENDIF
**              IF CR_HD NOT ALLOCATED
**              THEN
**                 ALLOCATE CR_HD
**                 SET POINTERS TO NULL
**                 SET CR_HD TO NEW_CR
**              ENDIF
**              IF INTERCHANGE LEVEL IS 2 OR 3
**              THEN
**                 COPY STRING TO FD_CNTRL
**                 INCREMENT STATE
**              ELSE
**                 ALLOCATE SPACE TO HOLD INPUT STRING I_STR--INCLUDE VALID
**                  SUBFIELD FLAG IN LENGTH CALCULATION
**                 IF VALID SUBFIELD
**                 THEN
**                    COPY WR_STR TO I_STR
**                 ENDIF
**                 SET F_TITLE TO I_STR
**                 SET STATE TO FINISHED
**              ENDIF
**           ELSEIF TAG IS USER AUGMENTED FILE DESCRIPTION (0..2)
**           THEN 
**              IF CR_HD NOT ALLOCATED
**              THEN
**                 ALLOCATE CR_HD
**                 SET POINTERS TO NULL
**                 SET CR_HD TO NEW_CR
**              ENDIF
**              ALLOCATE SPACE TO HOLD INPUT STRING I_STR
**              COPY WR_STR TO I_STR  
**              SET U_AFD TO I_STR
**              SET STATE TO FINISHED
**           ELSE {FIELD ENTRY}
**              CASE STATE 
**                 1: {FIELD CONTROL SUBFIELD} 
**                    COPY STRING TO FD_CNTRL 
**                    INCREMENT STATE
**                 2: {NAME SUBFIELD}
**                    ALLOCATE SPACE TO HOLD WR_STR--INCLUDE VALID SUBFIELD
**                     FLAG IN LENGTH CALCULATION
**                    IF VALID SUBFIELD
**                    THEN
**                       COPY STRING TO I_STR 
**                    ENDIF
**                    SET NAME FIELD POINTER TO I_STR 
**                    IF ILEVEL GREATER THAN ONE
**                    THEN
**                       IF DATA STRUCTURE TYPE OF FD_CNTRL GREATER THAN ZERO
**                       THEN
**                          INCREMENT STATE
**                       ELSE
**                          IF DATA TYPE OF FD_CNTRL EQUALS 5
**                          THEN
**                             SET STATE TO FORMATS SUBFIELD
**                          ELSE
**                             SET STATE TO FINISHED
**                          ENDIF
**                       ENDIF
**                    ELSE
**                       SET STATE TO FINISHED
**                    ENDIF
**                 3: {LABELS SUBFIELD}
**                    RETURN FAILURE (0) 
**                 4: {FORMATS SUBFIELD}
**                    RETURN FAILURE (0) 
**              ENDCASE {STATE} 
**           ENDIF 
**        1,5: {OKAY, END OF FIELD}
**           IF DD_HD NEXT IS NULL
**           THEN
**              RETURN FAILURE
**           ENDIF
**           CASE STATE 
**              1: {FIELD CONTROL SUBFIELD} 
**                 RETURN FAILURE
**              2: {NAME SUBFIELD} 
**                 IF TAG INDICATES CONTROL FIELD (0..0) 
**                 THEN
**                    ALLOCATE SPACE TO HOLD INPUT STRING I_STR--INCLUDE
**                     VALID SUBFIELD FLAG IN LENGTH CALCULATION
**                    IF VALID SUBFIELD
**                    THEN
**                       COPY WR_STR TO I_STR
**                    ENDIF
**                    SET F_TITLE TO I_STR
**                    IF INTERCHANGE LEVEL IS 3
**                    THEN
**                       INCREMENT STATE
**                    ELSE
**                       SET STATE TO FINISHED
**                    ENDIF
**                 ELSE 
**                    ALLOCATE SPACE TO HOLD WR_STR--INCLUDE VALID SUBFIELD
**                     FLAG IN LENGTH CALCULATION
**                    IF VALID SUBFIELD
**                    THEN 
**                       SET I_STR TO WR_STR
**                    ENDIF
**                    SET NAME TO I_STR
**                    IF ILEVEL GREATER THAN ONE
**                    THEN
**                       IF DATA STRUCTURE TYPE OF FD_CNTRL GREATER THAN ZERO
**                       THEN
**                          INCREMENT STATE
**                       ELSE
**                          IF DATA TYPE OF  FD_CNTRL EQUALS 5
**                          THEN
**                             SET STATE TO FORMATS SUBFIELD
**                          ELSE
**                             SET STATE TO FINISHED
**                          ENDIF
**                       ENDIF
**                    ELSE
**                       SET STATE TO FINISHED
**                    ENDIF
**                 ENDIF 
**              3: {LABELS SUBFIELD}
**                 IF TAG INDICATES FILE CONTROL (0..0)
**                 THEN
**                    IF TAG PAIRS LIST ARE PRESENT
**                    THEN
**                       CALL LOAD123TAGP() TO LOAD TAG PAIR LIST
**                    ENDIF
**                    SET STATE TO FINISHED
**                 ELSE
**                    IF LABELS ARE PRESENT
**                    THEN
**                       IF CURRENT DD FIELD CARTESIAN AND LABELS POINTER IS
**                        ARRAY DESCRIPTOR {CALL IS123ADSCR()}
**                       THEN
**                          CALL GET123ADSCR() TO RETRIEVE THE ARRAY
**                           DESCRIPTOR INTO THE STRUCTURE
**                       ELSE
**                          CALL LOAD123LAB() TO LOAD LABELS INTO STRUCTURES
**                       ENDIF
**                    ENDIF
**                    INCREMENT STATE
**                 ENDIF
**              4: {FORMATS SUBFIELD}
**                 IF FORMAT CONTROLS ARE PRESENT
**                 THEN
**                    CALL LOAD123FMT() TO CREATE A BINARY TREE CONTAINING
**                     FORMAT CONTROLS
**                 ENDIF
**                 INCREMENT STATE 
**           ENDCASE {STATE}
**           IF OPTION IS END OF FIELD
**           THEN
**              SET STATE TO FINISHED
**           ENDIF
** 
**        3,4: {END OF RECORD, END OF FILE} 
**           IF TAG IS USER AUGMENTED FILE DESCRIPTION (0..2)
**           THEN
**              IF CR_HD NOT ALLOCATED
**              THEN
**                 ALLOCATE CR_HD
**                 SET POINTERS TO NULL
**                 SET CR_HD TO NEW_CR
**              ENDIF
**              ALLOCATE SPACE TO HOLD INPUT STRING WR_STR
**              COPY WR_STR TO I_STR
**              SET U_AFD TO I_STR
**           ELSE
**              CASE STATE
**                 1: {FIELD CONTROL SUBFIELD}
**                    ALLOCATE NEW_DD
**                    SET POINTERS TO NULL
**                    SET FD_CNTRL TO NULL CHARACTER
**                    SET TAG TO INPUT TAG
**                    SET NUMBER OF DIMENSIONS AND PRIMARY DIMENSION TO ZERO
**                    SET CUR_DD NXT TO NEW_DD
**                    SET CUR_DD TO NEW_DD
**                    COPY STRING TO FD_CNTRL
**                 2: {NAME SUBFIELD}
**                    IF INTERCHANGE LEVEL IS 1
**                    THEN
**                       ALLOCATE NEW_DD
**                       SET POINTERS TO NULL
**                       SET FD_CNTRL TO NULL CHARACTER
**                       SET TAG TO INPUT TAG
**                       SET NUMBER OF DIMENSIONS AND PRIMARY DIMENSION TO ZERO
**                       SET CUR_DD NXT TO NEW_DD
**                       SET CUR_DD TO NEW_DD
**                    ENDIF
**                    IF TAG INDICATES CONTROL FIELD (0..0)
**                    THEN
**                       ALLOCATE SPACE TO HOLD INPUT STRING WR_STR--INCLUDE
**                        VALID SUBFIELD FLAG IN LENGTH CALCULATION
**                       IF VALID SUBFIELD
**                       THEN
**                          COPY WR_STR TO I_STR
**                       ENDIF
**                       SET F_TITLE TO I_STR
**                    ELSE
**                       ALLOCATE SPACE TO HOLD WR_STR--INCLUDE VALID SUBFIELD
**                        FLAG IN LENGTH CALCULATION
**                       IF VALID SUBFIELD
**                       THEN
**                          SET I_STR TO WR_STR
**                       ENDIF
**                       SET NAME TO I_STR
**                    ENDIF
**                 3: {LABELS SUBFIELD}
**                    IF TAG INDICATES CONTROL FIELD (0..0)
**                    THEN
**                       IF TAG PAIR LIST PRESENT
**                       THEN
**                          CALL LOAD123TAGP() TO LOAD TAG PAIR LIST
**                       ENDIF
**                    ELSE
**                       IF LABELS ARE PRESENT
**                       THEN
**                          IF CURRENT DD FIELD CARTESIAN AND LABELS POINTER IS
**                           ARRAY DESCRIPTOR {CALL IS123ADSCR()}
**                          THEN
**                             CALL GET123ADSCR() TO RETRIEVE THE ARRAY
**                              DESCRIPTOR INTO THE STRUCTURE
**                          ELSE
**                             CALL LOAD123LAB() TO LOAD LABELS INTO STRUCTURES
**                          ENDIF
**                       ENDIF
**                    ENDIF
**                 4: {FORMATS SUBFIELD}
**                    IF FORMAT CONTROLS ARE PRESENT
**                    THEN
**                       CALL LOAD123FMT() TO CREATE A BINARY TREE CONTAINING
**                        FORMAT CONTROLS
**                    ENDIF
**              ENDCASE {STATE}
**           ENDIF
**           SET STATE TO FINISHED
**        0: {FAILURE} 
**           RETURN FAILURE (0) 
**     ENDCASE {OPTIONS} 
** 
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int wr123ddsfld(FILE *fp,char *tag,char *wr_str,int option)


{ 
 
   /* LOCAL VARIABLES */
   char      *i_str ;
   long      int_tag;
   size_t    len    ;
   struct cr *new_cr;
   struct dd *new_dd;
   int      validsfld;

   /* CALL GET123LEVEL() TO RETURN BUFFER STRUCTURE FOR OUTPUT FILE */
   if (!get123level(fp)) return (0);

   /* IF OPTION IS OUTSIDE VALID RANGE OF VALUES, RETURN FAILURE */
   if (option < 0 || option > 6) return(0);

   /* CONVERT TAG TO INTEGER */
   if (is123intgr(tag)) {
      int_tag = atol(tag);
   }
   else {
      int_tag = -1;
   }
      
   /* IF SF_STATE_DD OUT OF BOUNDS */
   if ((cur_fm->sf_state_dd >= 5) || (cur_fm->sf_state_dd <= 0)) {

      /* IF INT_LEVEL IS 1 */
      if (cur_fm->dl_hd->ilevel == 1) {

         /* SET SF_STATE_DD TO NAME SUBFIELD */
         cur_fm->sf_state_dd = 2;
      }
      else {

         /* SET SF_STATE_DD TO FIELD CONTROL SUBFIELD */
         cur_fm->sf_state_dd = 1;

      }
   }

   /* IF WR_STR IS NOT A DELIMITER SEPARATOR */
   if (strcmp(wr_str,SEP_STR))

      /* SET VALID SUBFIELD FLAG TO VALID */
      validsfld = 1;
   else

      /*  SET VALID SUBFIELD FLAG TO INVALID */
      validsfld = 0;

   /* CASE OPTION */
   switch (option) {

      /* 2,6: {START OF RECORD, START OF FIELD} */
      case 2:
      case 6:

         /* IF OPTION IS START OF RECORD */
         if (option == 2) {

            /* CALL BEG123DDREC() TO BEGIN A DATA DESCRIPTIVE RECORD */
            if(!beg123ddrec(fp)) return (0);
         }

         /* ALLOCATE NEW_DD */
         if ((new_dd = (struct dd *) malloc(sizeof(struct dd))) == NULL) return(0);

         /* SET POINTERS TO NULL */
         new_dd->name = NULL;
         new_dd->dim_lptr = NULL;
         new_dd->labels = NULL;
         new_dd->fmt_rt = NULL;
         new_dd->next = NULL;

         /* SET FD_CNTRL TO NULL CHARACTER */
         new_dd->fd_cntrl[FCDSTYPE] = NC;

         /* SET TAG TO INPUT TAG */
         strcpy(new_dd->tag,tag);

         /* SET NUMBER OF DIMENSIONS AND PRIMARY DIMENSION TO ZERO */
         new_dd->num_dim = 0L;
         new_dd->prim_dms = 0L;

         /* SET CUR_DD NXT TO NEW_DD */
         cur_fm->cur_dd->next = new_dd;
 
         /* SET CUR_DD TO NEW_DD */
         cur_fm->cur_dd = new_dd;

         /* IF TAG IS FILE CONTROL (0..0) */
         if (int_tag == 0) {
 
            /* IF TAG SIZE EQUALS ZERO */
            if (cur_fm->dl_hd->s_tag == 0) {

               /* SET TAG SIZE TO LENGTH OF INPUT TAG */
               cur_fm->dl_hd->s_tag = _tcslen(tag);
            }

            /* IF CR_HD NOT ALLOCATED */
            if (cur_fm->cr_hd == NULL) {

               /* ALLOCATE CR_HD */
               if ((new_cr = (struct cr *) malloc (sizeof (struct cr))) == NULL) return(0);

               /* SET POINTERS TO NULL */
               new_cr->f_title = NULL;
               new_cr->tag_l = NULL;
               new_cr->u_afd = NULL;

               /* SET CR_HD TO NEW_CR */
               cur_fm->cr_hd = new_cr;
            }

            /* IF INTERCHANGE LEVEL IS 2 OR 3 */
            if (cur_fm->dl_hd->ilevel > 1) {

               /* COPY STRING TO FD_CNTRL */
               strcpy(cur_fm->cur_dd->fd_cntrl,wr_str);

               /* INCREMENT STATE */
               cur_fm->sf_state_dd++;
            }
            else {

               /* ALLOCATE SPACE TO HOLD INPUT STRING WR_STR--INCLUDE
                   VALID SUBFIELD FLAG IN LENGTH CALCULATION
               */
               len = (size_t) _tcslen(wr_str) + validsfld;
               if ((i_str = (char *) malloc (len * sizeof(char))) == NULL) return(0);
               *i_str = NC;

               /* IF VALID SUBFIELD, COPY WR_STR TO I_STR */
               if (validsfld) strcpy(i_str,wr_str);

               /* SET F_TITLE TO I_STR */
               cur_fm->cr_hd->f_title = i_str;

               /* SET STATE TO FINISHED */
               cur_fm->sf_state_dd = 5;
            }
         }
   
         /* ELSEIF TAG IS USER AUGMENTED FILE DESCRIPTION (0..2) */
         else if (int_tag == 2) {
 
            /* IF CR_HD NOT ALLOCATED */
            if (cur_fm->cr_hd == NULL) {

               /* ALLOCATE CR_HD */
               if ((new_cr = (struct cr *) malloc (sizeof (struct cr))) == NULL) return(0);

               /* SET POINTERS TO NULL */
               new_cr->f_title = NULL;
               new_cr->tag_l = NULL;
               new_cr->u_afd = NULL;

               /* SET CR_HD TO NEW_CR */
               cur_fm->cr_hd = new_cr;
            }

            /* ALLOCATE SPACE TO HOLD INPUT STRING WR_STR */
            len = (size_t) _tcslen(wr_str) + 1;
            if ((i_str = (char *) malloc (len*sizeof(char))) == NULL) return(0);
            *i_str = NC;

            /* COPY WR_STR TO I_STR */
            strcpy(i_str,wr_str);
      
            /* SET U_AFD TO I_STR */
            cur_fm->cr_hd->u_afd = i_str;

            /* SET STATE TO FINISHED */
            cur_fm->sf_state_dd = 5;
         }

         /* ELSE {FIELD ENTRY} */
         else {

            /* CASE STATE */
            switch (cur_fm->sf_state_dd) {

               /* 1: {FIELD CONTROL SUBFIELD} */
               case 1:

                  /* COPY STRING TO FD_CNTRL */
                  strcpy(cur_fm->cur_dd->fd_cntrl,wr_str);

                  /* INCREMENT STATE */
                  cur_fm->sf_state_dd++;
                  
               break;

               /* 2: {NAME SUBFIELD} */
               case 2:
 
                  /* ALLOCATE SPACE TO HOLD INPUT STRING WR_STR
                      --INCLUDE VALID SUBFIELD FLAG IN LENGTH CALCULATION
                  */
                  len = (size_t) _tcslen(wr_str) + validsfld;
                  if ((i_str = (char *) malloc (len*sizeof(char))) == NULL) return(0);
                  *i_str = NC;

                  /* IF VALID SUBFIELD, COPY WR_STR TO I_STR */
                  if (validsfld) strcpy(i_str,wr_str);

                  /* SET NAME FIELD POINTER TO I_STR */
                  cur_fm->cur_dd->name = i_str;
                  
                  /* IF ILEVEL GREATER THAN 1 */
                  if (cur_fm->dl_hd->ilevel > 1) {
                  
                     /* IF DATA STRUCTURE TYPE OF FD_CNTRL GREATER THAN 0 */
                     if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] > '0') {
                     
                        /* INCREMENT STATE */
                        cur_fm->sf_state_dd++;
                     }
                     else {
                        
                        /* IF DATA TYPE OF FD_CNTRL EQUALS 5 */
                        if (cur_fm->cur_dd->fd_cntrl[FCDTYPE] == '5') {
                        
                           /* SET STATE TO FORMATS SUBFIELD */
                           cur_fm->sf_state_dd = 4;
                        }
                        else {
                     
                           /* SET STATE TO FINISHED */
                           cur_fm->sf_state_dd = 5;
                        }
                     }
                  }
                  else {
                           
                     /* SET STATE TO FINISHED */
                     cur_fm->sf_state_dd = 5;
                  }
                  
               break;

               /* 3: {LABELS SUBFIELD} */
               case 3:

                  /* RETURN FAILURE (0) */
                  return (0);

               break;

               /* 4: {FORMATS SUBFIELD} */
               case 4:
 
                  /* RETURN FAILURE (0) */
                  return (0);
 
               break;

            /* ENDCASE STATE */
            }

         }

      break;

      /* 1,5: {OKAY, END OF FIELD} */
      case 1:
      case 5:

         /* IF DD_HD NEXT IS NULL, RETURN FAILURE */
         if (cur_fm->dd_hd->next == NULL) return(0);

         /* CASE STATE */
         switch (cur_fm->sf_state_dd) {

            /* 1: {FIELD CONTROL SUBFIELD} */
            case 1:

               /* RETURN FAILURE */
               return(0);

            break;
 
            /* 2: {NAME SUBFIELD} */
            case 2:

               /* IF TAG INDICATES CONTROL FIELD (0..0) */
               if (int_tag == 0) {

                  /* ALLOCATE SPACE TO HOLD INPUT STRING I_STR
                      --INCLUDE VALID SUBFIELD FLAG IN LENGTH CALCULATION
                  */
                  len = (size_t) _tcslen(wr_str) + validsfld;
                  if ((i_str = (char *) malloc (len * sizeof(char))) == NULL) return(0);
                  *i_str = NC;

                  /* IF VALID SUBFIELD, COPY WR_STR TO I_STR */
                  if (validsfld) strcpy(i_str,wr_str);

                  /* SET F_TITLE TO I_STR */
                  cur_fm->cr_hd->f_title = i_str;

                  /* IF INTERCHANGE LEVEL IS 3 */
                  if (cur_fm->dl_hd->ilevel == 3) {

                     /* INCREMENT STATE */
                     cur_fm->sf_state_dd++;
                  }
                  else {

                     /* SET STATE TO FINISHED */
                     cur_fm->sf_state_dd = 5;
                  }
               }
               else {

                  /* ALLOCATE SPACE TO HOLD WR_STR
                      --INCLUDE VALID SUBFIELD FLAG IN LENGTH CALCULATION
                  */
                  len = (size_t) _tcslen(wr_str) + validsfld;
                  if ((i_str = (char *) malloc (len*sizeof(char))) == NULL) return(0);
                  *i_str = NC;

                  /* IF VALID SUBFIELD, COPY WR_STR TO I_STR */
                  if (validsfld) strcpy(i_str,wr_str);
                   
                  /* SET NAME TO I_STR */
                  cur_fm->cur_dd->name = i_str;
                  
                  /* IF ILEVEL GREATER THAN 1 */
                  if (cur_fm->dl_hd->ilevel > 1) {
                  
                     /* IF DATA STRUCTURE TYPE OF FD_CNTRL GREATER THAN 0 */
                     if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] > '0') {
                     
                        /* INCREMENT STATE */
                        cur_fm->sf_state_dd++;
                     }
                     else {
                        
                        /* IF DATA TYPE OF FD_CNTRL EQUALS 5 */
                        if (cur_fm->cur_dd->fd_cntrl[FCDTYPE] == '5') {
                        
                           /* SET STATE TO FORMATS SUBFIELD */
                           cur_fm->sf_state_dd = 4;
                        }
                        else {
                     
                           /* SET STATE TO FINISHED */
                           cur_fm->sf_state_dd = 5;
                        }
                     }
                  }
                  else {
                           
                     /* SET STATE TO FINISHED */
                     cur_fm->sf_state_dd = 5;
                  }
               }

            break;

            /* 3: {LABELS SUBFIELD} */
            case 3:

               /* IF TAG INDICATES FILE CONTROL (0..0) */
               if (int_tag == 0) {

                  /* IF TAG PAIRS LIST PRESENT */
                  if (validsfld && *wr_str != NC) {

                     /* CALL LOAD123TAGP() TO LOAD TAG PAIR LIST */
                     if (!load123tagp(wr_str)) return(0);
                  }

                  /*SET STATE TO FINISHED */
                  cur_fm->sf_state_dd = 5;
               }
               else {

                  /* IF LABELS ARE PRESENT */
                  if (validsfld && *wr_str != NC) {

                     /* IF CURRENT DD FIELD CARTESIAN AND LABELS POINTER IS
                         ARRAY DESCRIPTOR {CALL IS123ADSCR()}
                     */
                     if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '2' &&
                      is123adscr(wr_str)) {

                        /* CALL GET123ADSCR() TO RETRIEVE THE ARRAY DESCRIPTOR
                            INTO THE STRUCTURE
                        */
                        if (!get123adscr(wr_str)) return(0);
                     }
                     else

                        /* CALL LOAD123LAB() TO LOAD LABELS INTO STRUCTURES */
                        if (!load123lab(wr_str)) return(0);
                  }

                  /* INCREMENT STATE */
                  cur_fm->sf_state_dd++;
               }

            break;
  
            /* 4: {FORMATS SUBFIELD} */
            case 4:

               /* IF FORMAT CONTROLS ARE PRESENT */
               if (validsfld && *wr_str != NC) {

                  /* CALL LOAD123FMT() TO CREATE A BINARY TREE CONTAINING
                      FORMAT CONTROLS
                  */
                  if(!load123fmt(wr_str)) return (0);
               }

               /* INCREMENT STATE */
               cur_fm->sf_state_dd++;
               
            break;

         /* ENDCASE {STATE} */
         }

         /* IF OPTION IS END OF FIELD */
         if (option == 5) {

            /* SET STATE TO FINISHED */
            cur_fm->sf_state_dd = 5;
         }

      break;

      /* 3,4: {END OF RECORD, END OF FILE} */
      case 3:
      case 4:

         /* IF TAG IS USER AUGMENTED FILE DESCRIPTION (0..2) */
         if (int_tag == 2) {

            /* IF CR_HD NOT ALLOCATED */
            if (cur_fm->cr_hd == NULL) {

               /* ALLOCATE CR_HD */
               if ((new_cr = (struct cr *) malloc (sizeof (struct cr))) == NULL) return(0);

               /* SET POINTERS TO NULL */
               new_cr->f_title = NULL;
               new_cr->tag_l = NULL;
               new_cr->u_afd = NULL;

               /* SET CR_HD TO NEW_CR */
               cur_fm->cr_hd = new_cr;
            }

            /* ALLOCATE SPACE TO HOLD INPUT STRING WR_STR */
            len = (size_t) _tcslen(wr_str) + 1;
            if ((i_str = (char *) malloc (len * sizeof(char))) == NULL) return(0);
            *i_str = NC;

            /* COPY WR_STR TO I_STR */
            strcpy(i_str,wr_str);

            /* SET U_AFD TO I_STR */
            cur_fm->cr_hd->u_afd = i_str;
         }
         else {

            /* CASE STATE */
            switch (cur_fm->sf_state_dd) {

               /* 1: {FIELD CONTROL SUBFIELD} */
               case 1:

                  /* ALLOCATE NEW_DD */
                  if ((new_dd = (struct dd *) malloc(sizeof(struct dd))) == NULL) return(0);

                  /* SET POINTERS TO NULL */
                  new_dd->name = NULL;
                  new_dd->dim_lptr = NULL;
                  new_dd->labels = NULL;
                  new_dd->fmt_rt = NULL;
                  new_dd->next = NULL;

                  /* SET FD_CNTRL TO NULL CHARACTER */
                  new_dd->fd_cntrl[FCDSTYPE] = NC;

                  /* SET TAG TO INPUT TAG */
                  strcpy(new_dd->tag,tag);

                  /* SET NUMBER OF DIMENSIONS AND PRIMARY DIMENSIONS TO ZERO */
                  new_dd->num_dim = 0L;
                  new_dd->prim_dms = 0L;

                  /* SET CUR_DD NXT TO NEW_DD */
                  cur_fm->cur_dd->next = new_dd;

                  /* SET CUR_DD TO NEW_DD */
                  cur_fm->cur_dd = new_dd;

                  /* COPY STRING TO FD_CNTRL */
                  strcpy(cur_fm->cur_dd->fd_cntrl,wr_str);

               break;

               /* 2: {NAME SUBFIELD} */
               case 2:

                  /* IF INTERCHANGE LEVEL IS 1 */
                  if (cur_fm->dl_hd->ilevel == 1) {

                     /* ALLOCATE NEW_DD */
                     if ((new_dd = (struct dd *) malloc(sizeof(struct dd))) == NULL) return(0);

                     /* SET POINTERS TO NULL */
                     new_dd->name = NULL;
                     new_dd->dim_lptr = NULL;
                     new_dd->labels = NULL;
                     new_dd->fmt_rt = NULL;
                     new_dd->next = NULL;

                     /* SET FD_CNTRL TO NULL CHARACTER */
                     new_dd->fd_cntrl[FCDSTYPE] = NC;

                     /* SET TAG TO INPUT TAG */
                     strcpy(new_dd->tag,tag);

                     /* SET NUMBER OF DIMENSIONS AND PRIMARY DIMENSION
                         TO ZERO
                     */
                     new_dd->num_dim = 0L;
                     new_dd->prim_dms = 0L;

                     /* SET CUR_DD NXT TO NEW_DD */
                     cur_fm->cur_dd->next = new_dd;

                     /* SET CUR_DD TO NEW_DD */
                     cur_fm->cur_dd = new_dd;
                  }

                  /* IF TAG INDICATES CONTROL FIELD (0..0) */
                  if (int_tag == 0) {

                     /* ALLOCATE SPACE TO HOLD INPUT STRING WR_STR
                         --INCLUDE VALID SUBFIELD FLAG IN LENGTH CALCULATION
                     */
                     len = (size_t) _tcslen(wr_str) + validsfld;
                     if ((i_str = (char *) malloc (len * sizeof(char))) == NULL) return(0);
                     *i_str = NC;

                     /* IF VALID SUBFIELD, COPY WR_STR TO I_STR */
                     if (validsfld) strcpy(i_str,wr_str);

                     /* SET F_TITLE TO I_STR */
                     cur_fm->cr_hd->f_title = i_str;

                  }
                  else {

                     /* ALLOCATE SPACE TO HOLD WR_STR
                         --INCLUDE VALID SUBFIELD FLAG IN LENGTH CALCULATION
                     */
                     len = (size_t) _tcslen(wr_str) + validsfld;
                     if ((i_str = (char *) malloc (len * sizeof(char))) == NULL) return(0);
                     *i_str = NC;

                     /* IF VALID SUBFIELD, COPY WR_STR TO I_STR */
                     if (validsfld) strcpy(i_str,wr_str);
                     
                     /* SET NAME TO I_STR */
                     cur_fm->cur_dd->name = i_str;

                  }

               break;
               
               /* 3: {LABELS SUBFIELD} */
               case 3:
   
                  /* IF TAG INDICATES CONTROL FIELD (0..0) */
                  if (int_tag == 0) {

                     /* IF TAG PAIRS LIST PRESENT */
                     if (validsfld && *wr_str != NC) {

                        /* CALL LOAD123TAGP() TO LOAD TAG PAIR LIST */
                        if (!load123tagp(wr_str)) return(0);
                     }
                  }
                  else {

                     /* IF LABELS ARE PRESENT */
                     if (validsfld && *wr_str != NC) {

                        /* IF CURRENT DD FIELD CARTESIAN AND LABELS POINTER IS
                            ARRAY DESCRIPTOR {CALL IS123ADSCR()}
                        */
                        if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '2' &&
                         is123adscr(wr_str)) {

                           /* CALL GET123ADSCR() TO RETRIEVE THE ARRAY
                               DESCRIPTOR INTO THE STRUCTURE
                           */
                           if (!get123adscr(wr_str)) return(0);
                        }
                        else

                           /* CALL LOAD123LAB() TO LOAD THE CONCATENATED LABELS 
                              INTO THE DD LABEL SUBFIELD STRUCTURE
                           */
                           if(!load123lab(wr_str)) return (0);
                     }
                  }

               break;

               /* 4: {FORMATS SUBFIELD} */
               case 4:

                  /* IF FORMAT CONTROLS ARE PRESENT */
                  if (validsfld && *wr_str != NC) {

                     /* CALL LOAD123FMT() TO CREATE A BINARY TREE CONTAINING
                         FORMAT CONTROLS
                     */
                     if(!load123fmt(wr_str)) return (0);
                  }

               break;
   
            /* ENDCASE {STATE} */
            }
         }

         /* SET STATE TO FINISHED */
         cur_fm->sf_state_dd = 5;
 
      break;

      /* 0: {FAILURE} */
      case 0:

         /* RETURN FAILURE (0) */
         return (0);

      break;

   /* ENDCASE {OPTIONS} */
   }

   /* RETURN SUCCESS */
   return (1);

} 
 
