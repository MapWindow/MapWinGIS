# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: RD123SFLD 
** 
**    PURPOSE: TO RETRIEVE SUBFIELD OF DATA RECORD 
** 
**    INVOCATION METHOD: RD123SFLD(FP,TAG,LEADID,RD_STR,STR_LEN,STATUS) 
** 
**    ARGUMENT LIST: 
**     NAME       TYPE      USE      DESCRIPTION 
**     FP         PTR        I       FILE POINTER 
**     TAG[]      PTR        O       FIELD IDENTIFIER
**     LEADID[]   PTR        O       LEADER IDENTIFIER 
**     RD_STR[]   PTR        O       STRING CONSISTING OF CURRENT  
**                                    CONCATENATED FIELD OF DDR 
**     STR_LEN    LONG       O       BYTE LENGTH OF RD_STR
**     STATUS     INT        O       STATUS 
**                                    0 = FAILURE 
**                                    1 = OKAY 
**                                    2 = START OF RECORD 
**                                    3 = END OF RECORD 
**                                    4 = END OF FILE (DR) 
**                                    5 = END OF FIELD 
**                                    6 = START OF FIELD 
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     FREE123LAB()     FREES LABELS POINTER STRUCTURE 
**     GET123FMT()      RETRIEVES A SINGLE FORMAT FROM THE FORMAT STRUCTURE 
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER 
**     I123TOA()        CONVERTS AN INTEGER INTO A CHARACTER STRING
**     INCRE123LAB()    INCREMENTS A LABEL FOR THE CORRESPONDING DATA VALUE 
**     LD123REC()       READS THE DATA RECORD LOADING THE INFORMATION IN 
**                       THE DATA STRUCTURES FOR REFERENCE 
**     RET123MATCH()    FINDS ASSOCIATED DATA DESCRIPTIVE MATCH TO DATA 
**                       RECORD 
**     SET123STAT()     CHECKS FOR EOF AND SETS STATUS FLAG
**     SETUP123LB()     SETS UP LABELS POINTER STRUCTURE 
** 
**    INTERNAL VARIABLES: 
**     NAME       TYPE               DESCRIPTION 
**     ANUM[10]   CHAR               CHARACTER REPRESENTATION OF AN INTEGER
**     B_CNT      INT                NO. OF DIGITS IN BIT SUBFIELD LENGTH
**     B_LEN      INT                LENGTH OF BIT SUBFIELD IN BITS
**     B_SIZ      INT                LENGTH OF SUBFIELD DATA VALUE IN BYTES
**     DELIM      CHAR               USER DELIMITER WITHIN FORMAT CONTROL
**     DTYP       CHAR               DATA TYPE WITHIN FORMAT CONTROL
**     FIRST      INT                FIRST VALUE IN SUBFIELD FLAG
**     I          INT                ARRAY INDEX
**     WIDTH      INT                WIDTH WITHIN FORMAT CONTROL 
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
**      LABELS          PTR     I    HEAD POINTER TO A LINKED LIST CONTAINING 
**                                    LINKED LIST OF DD LABEL SUBFIELD 
**                                    STRUCTURES 
**      FMT_RT          PTR     I    ROOT POINTER TO FORMAT CONTROLS BINARY 
**                                    TREE STRUCTURE 
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE  
**                                    STRUCTURE 
** 
**     DIMENSION LENGTHS STRUCTURE (DM) 
**      NAME            TYPE   USE   DESCRIPTION 
**      LEN             INT     I    DIMENSION LENGTH 
**      NXT             PTR     I    POINTER TO NEXT DIMENSION LENGTH     
** 
**     DATA RECORD STRUCTURE (DR) 
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD 
**      FD_LEN          INT    N/A   LENGTH OF DISCRIPTIVE AREA DATA RECORD  
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA 
**                                    RECORD 
**      NUM_DIM         INT     I    NUMBER OF DIMENSIONS (NO LABELS) 
**      DIM_LPTR        PTR     I    HEAD POINTER TO DIMENSION LENGTHS 
**                                    (NO LABELS) 
**      VALUES          PTR     I    HEAD POINTER TO DATA VALUE SUBFIELD 
**                                    RECORDS 
**      NEXT            PTR    I/O   POINTER TO NEXT DATA RECORD  
** 
**     DR DATA VALUE SUBFIELD STRUCTURE (DV) 
**      NAME            TYPE   USE   DESCRIPTION 
**      VALUE           CHAR    I    DATA VALUE 
**      NXT_VSET        PTR     I    POINTER TO NEXT SET OF DATA VALUES 
**      NXT_VAL         PTR     I    POINTER TO NEXT DATA VALUE SUBFIELD  
**                                    RECORD 
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
**      DR_HD           PTR     I    HEAD POINTER TO DATA RECORD STRUCTURE 
**      LP_HD           PTR     I    HEAD POINTER TO LABELS POINTER STRUCTURE 
**      RL_HD           PTR     I    HEAD POINTER TO DATA RECORD LEADER 
**                                    STRUCTURE 
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK 
**                                    STRUCTURE 
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE ENTRY 
**      CUR_DM          PTR    I/O   CURRENT POINTER TO DIMENSION LENGTHS 
**                                    STRUCTURE ENTRY 
**      CUR_DR          PTR    I/O   CURRENT POINTER TO DATA RECORD STRUCTURE 
**                                    ENTRY 
**      CUR_DV          PTR    I/O   CURRENT POINTER TO DR DATA VALUE SUBFIELD 
**                                    STRUCTURE ENTRY 
**      ROW_DVH         PTR    I/O   CURRENT POINTER TO NEXT SET OF VALUES 
**                                    IN DR DATA VALUE SUBFIELD STRUCTURE ENTRY 
**      CUR_FC          PTR     O    CURRENT POINTER TO FORMAT CONTROLS 
**                                    STRUCTURE ENTRY 
**      CUR_LP          PTR    N/A   CURRENT POINTER TO LABELS POINTER 
**                                    STRUCTURE ENTRY 
**      CUR_SL          PTR    N/A   CURRENT POINTER TO DD-LABEL SUBFIELD 
**                                    STRUCTURE ENTRY 
**      CUR_FCR         PTR     O    CURRENT POINTER TO ROOT FORMAT CONTROLS 
**                                    STRUCTURE 
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
**      SF_STATE_DR     INT    I/O   SUBFIELD STATE (DR) 
**                                    1 - NUMBER OF DIMENSIONS SUBFIELD 
**                                    2 - LENGTH OF A DIMENSION SUBFIELD 
**                                    3 - DATA VALUE STRING SUBFIELD 
**                                    4 - FINISHED 
**      NEXT            PTR    N/A   POINTER TO NEXT FILE MANAGEMENT 
**                                    STRUCTURE 
**      BACK            PTR    N/A   POINTER TO PREVIOUS FILE MANAGEMENT 
**                                    STRUCTURE 
** 
**    LABELS POINTER STRUCTURE (LP)
**     NAME            TYPE    USE   DESCRIPTION
**     NEXT            PTR      I    POINTER TO NEXT LP ENTRY
**     FIRST           PTR      I    FIRST LABEL (SL) IN THE LABEL SET
**     CUR             PTR      O    CURRENT LABEL (SL) IN THE LABEL SET
**
**    DATA RECORD LEADER STRUCTURE (RL)
**     NAME            TYPE    USE   DESCRIPTION
**     REC_LEN         INT     N/A   DATA RECORD LENGTH
**     RESV_SP         CHAR    N/A   RESERVED SPACE CHARACTER
**     LEAD_ID         CHAR     I    LEADER IDENTIFIER
**     S_RESV[6]       CHAR    N/A   RESERVED SPACE
**     DA_BASE         INT     N/A   BASE ADDRESS OF DATA AREA
**     R_SPACE[4]      CHAR    N/A   RESERVED SPACE CHARACTERS
**     S_FDLEN         INT     N/A   SIZE OF FIELD LENGTH
**     S_FDPOS         INT     N/A   SIZE OF FIELD POSITION
**     SP_RSRV         INT     N/A   RESERVED DIGIT
**     S_TAG           INT     N/A   SIZE OF FIELD TAG
** 
**    GLOBAL VARIABLES: 
**     NAME            TYPE    USE   DESCRIPTION
**     CUR_FM          PTR     I/O   CURRENT POINTER TO FILE MANAGEMENT 
**                                    STRUCTURE ENTRY 
**
**    GLOBAL CONSTANTS:
**     NAME            TYPE          DESCRIPTION
**     BYTE_S          INT           NUMBER OF BITS IN A BYTE
**     NC              CHAR          NULL CHARACTER
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     P. HODGES                 05/10/90  INITIAL PROLOG 
**     P. HODGES                 05/10/90  INITIAL PDL 
**     P. HODGES                 06/13/90  INITIAL CODE 
**     A. DEWITT                 10/08/90  MODIFY CODE TO CHECK FOR EOF
**                                          ENSURE PRESENCE OF DDR SUBFIELDS
**                                          BEFORE RECEIVING OR TRACKING 
**                                          ROUTINES ARE CALLED.  ADD CODE
**                                          TO INITIALIZE CURRENT POINTERS OF
**                                          FM STRUCTURE TO VALUE POINTERS 
**     A. DEWITT     92DR005     04/13/92  IMPLEMENT BINARY DATA
**     J. TAYLOR     92DR005     05/16/92  CHANGED CALLING SEQUENCE TO RETURN
**                                          LENGTH
**     L. MCMILLION  TASK #40    12/16/92  UPDATED PROLOG PER QC
**     J. TAYLOR     TASK 55     03/18/93  ADDED INITIALIZATION OF DTYP
**     L. MCMILLION  93DR027     04/06/93  MOVED CODE THAT SETS STATUS TO END
**                                          OF DATA VALUE SUBFIELD STATE TO
**                                          PROCESS REPEATED VECTOR DATA WITH
**                                          ONE LABEL
**     J. TAYLOR     93DR037     07/28/93  REMOVED MEMSET CALLS
**     J. TAYLOR     93DR039     07/28/93  REMOVED MEMSET CALLS
**
**    PDL: 
** 
**     INITIALIZE DATA TYPE TO BLANK CHARACTER
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER 
**     INITIALIZE OUPUT STRING TO EMPTY STRING
**     INITIALIZE STRING LENGTH TO ZERO
**     INITIALIZE STATUS TO OKAY 
**     INITIALIZE FIRST TO FALSE
**     IF STATE OUT OF RANGE 
**     THEN 
**        SET STATE TO NUMBER OF DIMENSIONS SUBFIELD 
**        CALL FREE123LAB() TO FREE THE LABELS POINTER STRUCTURE 
**        IF CUR_DR IS NULL
**        THEN
**           CALL LD123REC() TO INPUT DATA RECORD
**           IF LD123REC() FAILED
**           THEN
**              IF END OF FILE
**              THEN
**                 SET STATUS FLAG TO END OF FILE
**              ELSE
**                 SET STATUS TO FAILURE
**              ENDIF
**              RETURN FAILURE
**           ENDIF
**           SET STATUS TO START OF RECORD { LD123REC SUCCESS }
**        ENDIF
**        IF NEXT CUR_DR NOT NULL 
**        THEN 
**           MOVE CUR_DR TO CUR_DR NEXT 
**        ELSE 
**           CALL LD123REC() TO INPUT NEW DR 
**           IF LD123REC() FAILED
**           THEN
**              IF END OF FILE
**              THEN
**                 SET STATUS FLAG TO END OF FILE
**              ELSE
**                 SET STATUS TO FAILURE 
**              ENDIF
**              RETURN FAILURE
**           ENDIF
**           MOVE CUR_DR TO CUR_DR NEXT TO PASS DUMMY
**           SET STATUS TO START OF RECORD  { LD123REC SUCCESS }
**        ENDIF 
**        CALL RET123MATCH() TO GET CORRESPONDING DD ENTRY 
**        INITIALIZE DATA VALUE POINTERS
**     ENDIF 
**
**     SET OUTPUT TAG TO CUR_DR TAG
**     SET OUTPUT LEADID TO CUR_DR LEADID
**     IF NUMBER OF DIMENSIONS GREATER THAN ZERO AND 
**      STATE IS NOT DATA VALUE STRING SUBFIELD 
**     THEN 
**        IF STATE IS NUMBER OF DIMENSIONS SUBFIELD 
**        THEN 
**           SET OUTPUT STRING TO NUMBER OF DIMENSIONS 
**           INCREMENT STATE TO LENGTH OF DIMENSION SUBFIELD 
**           IF STATUS NOT SET TO START OF RECORD
**           THEN
**              IF AT FIRST FIELD OF DATA RECORD 
**              THEN
**                 SET STATUS TO START OF RECORD 
**              ELSE               
**                 SET STATUS TO START OF FIELD
**              ENDIF 
**           ENDIF
**           SET CUR_DM TO DIM_LPTR 
**        ELSEIF STATE IS LENGTH OF DIMENSION FIELD 
**           SET CUR_DM TO NXT 
**           IF CUR_DM IS NULL 
**           THEN 
**              SET STATE TO DATA VALUE STRING SUBFIELD 
**              IF LABELS ARE PRESENT
**              THEN
**                 CALL SETUP123LB() TO SET UP LABELS POINTER STRUCTURE 
**              ENDIF
**              INITIALIZE FORMAT TREE POINTERS 
**              SET FIRST TO TRUE 
**           ELSE 
**              CALL I123TOA() TO CONVERT LEN TO A CHARACTER STRING 
**              SET OUTPUT STRING TO LEN 
**           ENDIF 
**        ENDIF 
**     ELSEIF NUMBER OF DIMENSIONS LESS THAN OR EQUAL TO ZERO AND
**      STATE NOT EQUAL TO DATA VALUE STRING SUBFIELD
**     THEN 
**        SET STATE TO DATA VALUE STRING SUBFIELD 
**        IF LABELS ARE PRESENT
**        THEN
**           CALL SETUP123LB() TO SET UP LABELS POINTER STRUCTURE 
**        ENDIF
**        INITIALIZE FORMAT TREE POINTERS 
**        SET FIRST TO TRUE 
**        IF STATUS NOT START OF RECORD
**        THEN
**           SET STATUS TO START OF FIELD
**        ENDIF
**     ENDIF 
** 
**     IF STATE IS DATA VALUE STRING SUBFIELD 
**     THEN 
**        IF CUR_DV IS EQUAL TO HEADER DATA VALUE 
**        THEN 
**           SET CUR_DV TO FIRST VALUE IN LIST
**           INITIALIZE ROW_DVH TO FIRST VALUE
**           SET FIRST TO TRUE
**           INITIALIZE FORMAT TREE POINTERS
**           IF AT FIRST FIELD OF DATA RECORD
**           THEN
**              IF NUMBER OF DIMENSIONS SUBFIELD EQUALS ZERO
**              THEN 
**                 SET STATUS TO START OF RECORD
**              ENDIF
**           ELSE
**              IF NUMBER OF DIMENSIONS FIELD EQUALS ZERO 
**              THEN
**                 SET STATUS TO START OF FIELD
**              ENDIF
**           ENDIF
**        ELSE 
**           SET CUR_DV TO NXT_VAL
**        ENDIF 
**        IF NOT FIRST 
**        THEN 
**           IF DATA DESCRIPTIVE RECORD FIELD FOR CURRENT DR FIELD HAS LABELS
**           THEN
**              CALL INCRE123LAB() TO INCREMENT LABEL TO CORRESPOND TO THIS 
**               DATA VALUE
**           ENDIF 
**        ELSE 
**           SET FIRST TO FALSE 
**        ENDIF 
**        IF CURRENT DESCRIPTIVE FIELD FOR CURRENT DR FIELD HAS FORMATS
**        THEN
**           CALL GET123FMT() TO RETRIEVE FORMAT CORRESPONDING TO THIS DATA VALUE 
**        ENDIF
**        IF CUR_DV NOT NULL 
**        THEN 
**           IF VALUE POINTER NOT NULL
**           THEN 
**              IF DATA TYPE IS NOT BINARY
**              THEN
**                 SET OUTPUT STRING TO VALUE
**                 SET STRING LENGTH
**              ELSE
**                 IF FIELD IS VARIABLE LENGTH BIT FIELD
**                 THEN
**                    EXTRACT FIELD LENGTH COUNT
**                    EXTRACT FIELD LENGTH
**                    COMPUTE BYTE SIZE OF BIT DATA FIELD
**                    COMPUTE ACTUAL LENGTH OF BIT DATA FIELD
**                    SET READ STRING TO STORED VALUE
**                    TERMINATE READ STRING WITH NULL CHARACTER
**                    SET STRING LENGTH TO B_SIZ
**                 ELSE IF FIELD IS FIXED LENGTH BIT FIELD
**                 THEN
**                    COMPUTE BYTE SIZE OF BIT DATA FIELD
**                    SET READ STRING TO STORED VALUE
**                    TERMINATE READ STRING WITH NULL CHARACTER
**                    SET STRING LENGTH TO B_SIZ
**                 ENDIF
**              ENDIF
**           ELSE
**              SET OUTPUT STRING TO NC
**           ENDIF 
**        ELSE 
**           SET ROW_DVH TO NXT_VSET 
**           SET CUR_DV TO ROW_DVH 
**           IF CUR_DV NOT NULL 
**           THEN 
**              IF VALUE POINTER NOT NULL
**              THEN
**                 IF DATA TYPE IS NOT BINARY
**                 THEN
**                    SET OUTPUT STRING TO VALUE
**                    SET STRING LENGTH
**                 ELSE
**                    IF FIELD IS VARIABLE LENGTH BIT FIELD
**                    THEN
**                       EXTRACT FIELD LENGTH COUNT
**                       EXTRACT FIELD LENGTH
**                       COMPUTE BYTE SIZE OF BIT DATA FIELD
**                       COMPUTE ACTUAL LENGTH OF BIT DATA FIELD
**                       SET READ STRING TO STORED VALUE
**                       TERMINATE READ STRING WITH NULL CHARACTER
**                       SET STRING LENGTH TO B_SIZ
**                    ELSE IF FIELD IS FIXED LENGTH BIT FIELD
**                    THEN
**                       COMPUTE BYTE SIZE OF BIT DATA FIELD
**                       SET READ STRING TO STORED VALUE
**                       TERMINATE READ STRING WITH NULL CHARACTER
**                       SET STRING LENGTH TO B_SIZ
**                    ENDIF
**                 ENDIF
**              ELSE
**                 SET OUTPUT STRING TO NC
**              ENDIF 
**           ELSE 
**              SET STATUS TO ERROR 
**              RETURN ERROR 
**           ENDIF 
**        ENDIF 
**        IF NXT_VAL IS NULL AND ROW_DVH NXT_VSET IS NULL 
**        THEN 
**           IF CUR_DR NEXT IS NULL 
**           THEN 
**              CALL SET123STAT() TO SET STATUS FLAG IF EOF
**              IF STATUS NOT EQUAL TO EOF 
**              THEN
**                 SET STATUS TO END OF RECORD 
**              ENDIF
**           ELSE 
**              IF STATUS NOT SET TO START OF RECORD 
**              THEN
**                 SET STATUS TO END OF FIELD
**              ENDIF 
**           ENDIF 
**           INCREMENT STATE TO FINISHED 
**        ENDIF 
**     ENDIF 
** 
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int rd123sfld(FILE *fp,char *tag,char *leadid,char *rd_str,long *str_len,int *status)

{

   /* LOCAL VARIABLES */
   char delim   ;
   char dtyp = ' ';
   char anum[10];
   int  first   ;
   long width   ;
   size_t b_siz ;
   int  b_cnt;
   int  b_len;
   int i;

   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return (0);

   /* SET RD_STR TO NC */
   *rd_str = NC;

   /* SET STRING LENGTH TO ZERO */
   *str_len = 0;

   /* INITIALIZE STATUS TO OKAY */
   *status = 1;
   
   /* INITIALIZE FIRST TO FALSE */
   first = 0;
   
   /* IF STATE OUT OF RANGE */
   if ((cur_fm->sf_state_dr >= 4) || (cur_fm->sf_state_dr <= 0)) {

      /* SET STATE TO NUMBER OF DIMENSIONS SUBFIELD */
      cur_fm->sf_state_dr = 1;

      /* CALL FREE123LAB() TO FREE THE LABELS POINTER STRUCTURE */
      if (!free123lab()) return (0);
      
      /* IF CUR_DR IS NULL */
      if (cur_fm->cur_dr == NULL) {
      
         /* CALL LD123REC() TO INPUT DATA RECORD */
         if (!ld123rec()) {
            
            /* IF END OF FILE */
            if (feof(fp)) {
               
               /* SET STATUS TO END OF FILE */
               *status = 4;
            }
            else {
               
               /* SET STATUS TO FAILURE */
               *status = 0;   
            };
            return(0);
         };        
          
          /* SET STATUS TO START OF RECORD */
          *status = 2;
          
      };

      /* IF NEXT CUR_DR NOT NULL */
      if (cur_fm->cur_dr->next != NULL) {

         /* MOVE CUR_DR TO CUR_DR NEXT */
         cur_fm->cur_dr = cur_fm->cur_dr->next;

      }
      else {  

         /* CALL LD123REC() TO INPUT NEW DR */
         if (!ld123rec()) {
            
            /* IF END OF FILE */
            if (feof(fp)) {
               
               /* SET STATUS TO END OF FILE */
               *status = 4;
            }
            else {
               
               /* SET STATUS TO FAILURE */
               *status = 0;
               
            };
            return(0);

          };        

          /* MOVE CUR_DR TO CUR_DR NEXT TO PASS DUMMY */
          cur_fm->cur_dr = cur_fm->cur_dr->next;
          
          /* SET STATUS TO START OF RECORD */
          *status = 2;
          
      };         

      /* CALL RET123MATCH() TO GET CORRESPONDING DD ENTRY */
      if (!ret123match(cur_fm->cur_dr->tag)) return (0);

      /* INITIALIZE DATA VALUE POINTERS */
      cur_fm->row_dvh = cur_fm->cur_dr->values->nxt_vset;
      cur_fm->cur_dv = cur_fm->cur_dr->values;
      
   }; 

   /* SET OUTPUT TAG TO CUR_DR TAG */
   strcpy(tag,cur_fm->cur_dr->tag);

   /* SET OUTPUT LEADID TO CUR_DR LEADID */
   *leadid = cur_fm->rl_hd->lead_id;

   /* IF NUMBER OF DIMENSIONS GREATER THAN ZERO AND 
       STATE IS NOT DATA VALUE STRING SUBFIELD 
   */
   if ((cur_fm->cur_dr->num_dim >0) && (cur_fm->sf_state_dr != 3)) {

      /* IF STATE IS NUMBER OF DIMENSIONS SUBFIELD */
      if (cur_fm->sf_state_dr == 1) {

         /* SET OUTPUT STRING TO NUMBER OF DIMENSIONS */
         i123toa(cur_fm->cur_dr->num_dim, rd_str);

         /* INCREMENT STATE TO LENGTH OF DIMENSION SUBFIELD */
         cur_fm->sf_state_dr++;

         /* IF STATUS NOT SET TO START OF RECORD */
         if (*status != 2) {
         
            /* IF AT FIRST FIELD OF DATA RECORD */
            if (cur_fm->cur_dr == cur_fm->dr_hd->next) {
            
               /* SET STATUS TO START OF RECORD */
               *status = 2;
            }
            else {
               
               /* SET STATUS TO START OF FIELD */
               *status = 6;
            };   
         };   

         /* SET CUR_DM TO DIM_LPTR */
         cur_fm->cur_dm = cur_fm->cur_dr->dim_lptr;

      }

      /* ELSE IF STATE IS LENGTH OF DIMENSION FIELD */
      else if (cur_fm->sf_state_dr == 2) {

         /* SET CUR_DM TO NXT */
         cur_fm->cur_dm = cur_fm->cur_dm->nxt;

        /* IF CUR_DM IS NULL */
         if (cur_fm->cur_dm == NULL) {

            /* SET STATE TO DATA VALUE STRING SUBFIELD */
            cur_fm->sf_state_dr = 3;

            /* IF LABELS ARE PRESENT */
            if (cur_fm->cur_dd->labels != NULL) {

               /* CALL SETUP123LAB() TO SET UP LABELS POINTER STRUCTURE */
               if (!setup123lb()) return (0);
            }   

            /* INITIALIZE FORMAT TREE POINTERS */
            cur_fm->cur_fc = cur_fm->cur_dd->fmt_rt;
            cur_fm->cur_fcr = cur_fm->cur_dd->fmt_rt;

            /* SET FIRST TO TRUE */
            first = 1;

         }
         else {  

            /* CALL I123TOA() TO CONVERT LEN TO A CHARACTER STRING 
                SET OUTPUT STRING TO LEN 
            */
            i123toa(cur_fm->cur_dm->len, rd_str);

         } 
      } 
   }

   /* ELSE IF NUMBER OF DIMENSIONS LESS THAN OR EQUAL TO ZERO AND 
       STATE NOT DATA VALUE STRING SUBFIELD 
   */
   else if ((cur_fm->cur_dr->num_dim <= 0) && (cur_fm->sf_state_dr != 3 )) {

      /* SET STATE TO DATA VALUE STRING SUBFIELD */
      cur_fm->sf_state_dr = 3;

      /* IF LABELS ARE PRESENT */
      if (cur_fm->cur_dd->labels != NULL) {
      
         /* CALL SETUP123LB() TO SET UP LABELS POINTER STRUCTURE */
         if (!setup123lb()) return (0);
      };   

      /* INITIALIZE FORMAT TREE POINTERS */
      cur_fm->cur_fc = cur_fm->cur_dd->fmt_rt;
      cur_fm->cur_fcr = cur_fm->cur_dd->fmt_rt;
      
      /* SET FIRST TO TRUE */
      first = 1;

      /* IF STATUS NOT START OF RECORD */
      if (*status != 2) {
      
         /* SET STATUS TO START OF FIELD */
         *status = 6;
      };
         
   } ;
 
   /* IF STATE IS DATA VALUE STRING SUBFIELD */
   if (cur_fm->sf_state_dr == 3) {
 
      /* IF CUR_DV EQUALS THE DUMMY HEADER */
      if (cur_fm->cur_dv == cur_fm->cur_dr->values ) {

         /* SET CUR_DV TO FIRST VALUE IN LIST */
         cur_fm->cur_dv = cur_fm->cur_dr->values->nxt_vset;

         /* INITIALIZE ROW_DVH TO FIRST VALUE */
         cur_fm->row_dvh = cur_fm->cur_dv;
         
         /* SET FIRST TO TRUE */
         first = 1;
         
         /* INITIALIZE FORMAT TREE POINTERS */
         cur_fm->cur_fc = cur_fm->cur_dd->fmt_rt;
         cur_fm->cur_fcr = cur_fm->cur_dd->fmt_rt;

         /* IF AT FIRST FIELD OF DATA RECORD */
         if ( cur_fm->cur_dr == cur_fm->dr_hd->next) { 
            
            /* IF NUMBER OF DIMENSIONS SUBFIELD EQUALS ZERO */
            if (cur_fm->cur_dr->num_dim == 0 ) {
               
               /* SET STATUS TO START OF RECORD */
               *status = 2;
            };
         }
         else {
            
            /* IF NUMBER OF DIMENSIONS FIELD EQUALS ZERO */
            if (cur_fm->cur_dr->num_dim == 0 ) {
               
               /* SET STATUS TO START OF FIELD */
               *status = 6;
            };
         };             
      }
      else {

         /* SET CUR_DV TO NXT_VAL */
         cur_fm->cur_dv = cur_fm->cur_dv->nxt_val;
      };   

      /* IF NOT FIRST */
      if (!first) {
     
         /* IF LABELS FIELD OF CURRENT DD NOT NULL */
         if (cur_fm->cur_dd->labels != NULL) {
         
            /* CALL INCRE123LAB() TO INCREMENT LABEL TO CORRESPOND TO THIS 
                DATA VALUE 
            */
            if (!incre123lab(cur_fm->lp_hd->next->next, cur_fm->lp_hd->next->first, 
                  &cur_fm->lp_hd->next->cur)) return (0);
         };         
      }
      else {  

         /* SET FIRST TO FALSE */
         first = 0;
      }; 

      /* IF FORMATS FIELD OF CURRENT DD NOT EQUAL TO NULL */
      if (cur_fm->cur_dd->fmt_rt != NULL ) {
      
         /* CALL GET123FMT() TO RETRIEVE FORMAT CORRESPONDING TO THIS DATA VALUE */
         if(!get123fmt(&dtyp,&width,&delim)) return (0);

      };
      
      /* IF CUR_DV NOT NULL */
      if (cur_fm->cur_dv != NULL) {

         /* IF VALUE IS NOT NULL */
         if (cur_fm->cur_dv->value != NULL) {
         
            /* IF DATA TYPE IS NOT BINARY */ 
            if (dtyp != 'B') {

               /* SET OUTPUT STRING TO VALUE */
               strcpy(rd_str,cur_fm->cur_dv->value);

               /* SET STRING LENGTH */
               *str_len = _tcslen(cur_fm->cur_dv->value);
            }
            /* ELSE BINARY DATA */
            else {

               /* IF FIELD IS VARIABLE LENGTH BIT FIELD */
               if (width == 0) {
 
                  /* EXTRACT FIELD LENGTH COUNT */
                  anum[0] = cur_fm->cur_dv->value[0];
                  anum[1] = NC;
                  b_cnt = atoi(anum);

                  /* EXTRACT FIELD LENGTH */
                  for (i=0;i<b_cnt;i++) anum[i] = cur_fm->cur_dv->value[i+1];
                  anum[b_cnt] = NC;
                  b_len = atoi(anum);

                  /* COMPUTE BYTE SIZE OF BIT DATA FIELD */
                  b_len = abs((int) ((((float) b_len) / BYTE_S) + 0.99999999));

                  /* COMPUTE ACTUAL LENGTH OF BIT DATA FIELD */
                  b_siz = b_len + b_cnt + 1;

                  /* SET RD_STR TO STORED VALUE */
                  memcpy(rd_str,cur_fm->cur_dv->value,b_siz);

                  /* TERMINATE RD_STR WITH NULL CHARACTER */
                  rd_str[b_siz] = NC;

                  /* SET STRING LENGTH */
                  *str_len = b_siz;
               } 
         
               /* ELSE FIELD IS FIXED LENGTH BIT FIELD */
               else
               {

                  /* COMPUTE BYTE SIZE OF BIT DATA FIELD */
                  b_siz = abs((int) ((((float) width) / BYTE_S) + 0.99999999));
               
                  /* SET RD_STR TO STORED VALUE */
                  memcpy(rd_str,cur_fm->cur_dv->value,b_siz);

                  /* TERMINATE RD_STR WITH NULL CHARACTER */
                  rd_str[b_siz] = NC;

                  /* SET STRING LENGTH */
                  *str_len = b_siz;
               }
            };
         }
         else {
            
            /* SET RD_STR TO NC */
            *rd_str = NC;
         };      
      }
      else {  

         /* SET ROW_DVH TO NXT_VSET */
         cur_fm->row_dvh = cur_fm->row_dvh->nxt_vset;

         /* SET CUR_DV TO ROW_DVH */
         cur_fm->cur_dv = cur_fm->row_dvh;

         /* IF CUR_DV NOT NULL */
         if (cur_fm->cur_dv != NULL) {

            /* IF VALUE IS NOT NULL */
            if (cur_fm->cur_dv->value != NULL) {
         
               /* IF DATA TYPE IS NOT BINARY */
               if (dtyp != 'B') {

                  /* SET OUTPUT STRING TO VALUE */
                  strcpy(rd_str,cur_fm->cur_dv->value);

                  /* SET STRING LENGTH */
                  *str_len = _tcslen(cur_fm->cur_dv->value);
               }
            
               /* ELSE BINARY DATA */
               else {

                  /* IF FIELD IS VARIABLE LENGTH BIT FIELD */
                  if (width == 0) {
 
                     /* EXTRACT FIELD LENGTH COUNT */
                     anum[0] = cur_fm->cur_dv->value[0];
                     anum[1] = NC;
                     b_cnt = atoi(anum);

                     /* EXTRACT FIELD LENGTH */
                     for (i=0;i<b_cnt;i++) anum[i] = cur_fm->cur_dv->value[i+1];
                     anum[b_cnt] = NC;
                     b_len = atoi(anum);

                     /* COMPUTE BYTE SIZE OF BIT DATA FIELD */
                     b_len = abs((int) ((((float) b_len) / BYTE_S) + 0.99999999));

                     /* COMPUTE ACTUAL LENGTH OF BIT DATA FIELD */
                     b_siz = b_len + b_cnt + 1;

                     /* SET RD_STR TO STORED VALUE */
                     memcpy(rd_str,cur_fm->cur_dv->value,b_siz);

                     /* TERMINATE RD_STR WITH NULL CHARACTER */
                     rd_str[b_siz] = NC;

                     /* SET STRING LENGTH */
                     *str_len = b_siz;
                  } 
         
                  /* ELSE FIELD IS FIXED LENGTH BIT FIELD */
                  else
                  {

                     /* COMPUTE BYTE SIZE OF BIT DATA FIELD */
                     b_siz = abs((int) ((((float) width) / BYTE_S) + 0.99999999));

                     /* SET RD_STR TO STORED VALUE */
                     memcpy(rd_str,cur_fm->cur_dv->value,b_siz);

                     /* TERMINATE RD_STR WITH NULL CHARACTER */
                     rd_str[b_siz] = NC;
         
                     /* SET STRING LENGTH */
                     *str_len = b_siz;
                  }
               };
            }
            else {
            
               /* SET RD_STR TO NC */
               *rd_str = NC;
            };     
 
         }
         else {  

            /* SET STATUS TO ERROR */
            *status = 0;

            /* RETURN ERROR */
            return (0);
         } 
      } 

      /* IF NXT_VAL IS NULL AND ROW_DVH NXT_VSET IS NULL */
      if ((cur_fm->cur_dv->nxt_val == NULL) && 
       (cur_fm->row_dvh->nxt_vset == NULL)) {
     
         /* IF CUR_DR NEXT IS NULL */
         if (cur_fm->cur_dr->next == NULL) {

            /* CHECK FOR END OF FILE */
            if (!set123stat(fp,status)) return (0);
               
            /* IF NOT END OF FILE */
            if (*status != 4) {

               /* SET STATUS TO END OF RECORD */
               *status = 3;
            };   
         }
         else {  

            /* IF STATUS NOT SET TO START OF RECORD */
            if ( *status != 2 ) {
            
               /* SET STATUS TO END OF FIELD */
               *status = 5;
            };   
         };
 
         /* INCREMENT STATE TO FINISHED */
         cur_fm->sf_state_dr = 4;

      } 
   } 
   
   /* RETURN SUCCESS */ 
   return (1);
}
