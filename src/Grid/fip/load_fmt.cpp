# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: LOAD123FMT
**
**    PURPOSE: TO CREATE A BINARY TREE STRUCTURE CONTAINING FORMAT CONTROLS
**     THAT WILL BE USED IN READING/WRITING DATA RECORDS
**
**    INVOCATION METHOD: LOAD123FMT(CH_PTR)
**
**    ARGUMENT LIST:
**     NAME         TYPE      USE      DESCRIPTION
**     CH_PTR[]     PTR        I       FORMAT CONTROL STRING
**     LOAD123FMT() LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME          TYPE               DESCRIPTION
**     DIGIT_STR[2]  CHAR               CHARACTER STRING CONTAINING A SINGLE
**                                       DIGIT USED FOR WIDTH CALCULATIONS
**     I             INT                INDEX INTO FORMAT CONTROL STRING
**     NEW_FC        PTR                POINTER TO NEWLY ALLOCATED FORMAT
**                                       CONTROL STRUCTURE
**     SL_FOUND      LOGICAL            SAME LEVEL FOUND FLAG
**     STATE         INT                CURRENT STATE
**                                       0 - ALPHA
**                                       1 - WOD
**                                       2 - SLEVEL
**                                       3 - REPETITION
**                                       4 - LP_NXTL
**     TMP_LONG      LONG               TEMPORARY LONG INTEGER
**
**    GLOBAL REFERENCES:
**
**     FORMAT CONTROLS STRUCTURE (FC)
**     NAME       TYPE       USE     DESCRIPTION
**     REP_NO     INT         I      NUMBER OF REPETITIONS
**     D_TYPE     CHAR        I      DATA TYPE - (A,I,R,S,C,B,X)
**     WIDTH      INT         I      FIELD WIDTH SPECIFICATION
**     U_DELIM    CHAR        I      USER DELIMITER
**     LEFT       PTR         I      LEFT POINTER TO FORMAT CONTROLS STRUCTURE
**     RIGHT      PTR         I      RIGHT POINTER TO FORMAT CONTROLS STRUCTURE
**     BACK       PTR         I      BACK POINTER TO FORMAT CONTROLS STRUCTURE
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
**      FMT_RT          PTR     O    ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR    I/O   POINTER TO NEXT DATA DESCRIPTIVE 
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
**      CUR_FC          PTR     O    CURRENT POINTER TO FORMAT CONTROLS
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
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR    I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS: 
**     NAME       TYPE         DESCRIPTION
**     BLNK_SP    CHAR         BLANK SPACE CHARACTER
**     NC         CHAR         NULL CHARACTER
**    
**    ALGORITHM DESCRIPTION:
**     LOAD123FMT IS IMPLEMENTED OR MODELED USING THE CONCEPT OF A STATE 
**     MACHINE.  THE CONTROL FLOW OF LOAD123FMT IS DETERMINED BY ITS CURRENT
**     STATE.  SEQUENTIAL PROCESS OF THIS ROUTINE CAN BE IN ONE OF THE 
**     FOLLOWING STATES:
**       NAME            DESCRIPTION
**       ALPHA           THIS STATE IMPLIES RETURNING FROM A STATE THAT HAS 
**                        JUST PROCESSED AN ALPHABETIC CHARACTER.
**       SLEVEL          THIS STATE IMPLIES PROCESSING FORMAT CONTROLS
**                        ON SAME LEVEL OF NESTING.  THIS STATE IS INVOKED
**                        WHEN A COMMA HAS BEEN ENCOUNTERED AND PROCESSED IN
**                        THE PREVIOUS STATE.  A COMMA SUGGESTS ANOTHER ELEMENT
**                        IN A LIST WHERE AN ELEMENT MAY BE ANOTHER LIST. 
**       REPETITION      THIS STATE IMPLIES PROCESSING FORMAT CONTROLS
**                        THAT REPRESENT REPETITIVE DATA OF THAT CONTROL TYPE
**       WOD             THIS STATE IMPLIES PROCESSING CONTROLS INVOLVING
**                        FIELD WIDTHS OR DELIMITERS
**       LP_NXTL         THIS STATE IMPLIES PROCESSING FORMAT CONTROLS ON
**                        THE NEXT LEVEL OF NESTING. THIS STATE IS INVOKED
**                        WHEN REPETITION HAS BEEN ENCOUNTERED AND A LEFT
**                        PARENTHESIS WAS JUST PROCESSED BY THE PREVIOUS STATE
**
**     THE DEFINITION OF A FORMAT CONTROL LIST IS A LIST MADE UP OF CONTROL 
**      ELEMENTS WHERE AN ELEMENT IS DEFINED AS A SINGLE FORMAT CONTROL
**      OR A LIST OF FORMAT CONTOLS.
**
**     THE BINARY TREE STRUCTURE IS CREATED WITH FORMAT CONTROLS STRUCTURES.
**      THE TREE IS CONSTRUCTED FOR A PREORDER TYPE TRAVERSAL.  THE TREE IS 
**      ALSO LOGICALLY SKEWED RIGHT MODELING A LIST.  EACH LEFT CHILD CAN BE 
**      CONSIDERED AS BEING EITHER A SINGLE FORMAT CONTROL VALUE OR A LIST OF
**      FORMAT CONTROLS.  A RIGHT CHILD CONTAINS THE NEXT ELEMENT IN THE
**      CURRENT LIST.  THE TREE IS TRAVERSED BY VISITING A NODE AND TRAVERSING
**      LEFT AND CONTINUE GOING LEFT UNTIL YOU HAVE ENCOUNTERED A NODE WHOSE
**      REPETITION NUMBER IS ZERO.  YOU THEN BACK UP AND GO LEFT AGAIN UNTIL 
**      YOU HAVE GONE LEFT THE NUMBER OF TIMES SPECIFIED IN THE REPETITION 
**      NUMBER FIELD OF THAT NODE.  AFTER EXHAUSTING THE NUMBER OF REPEATS YOU
**      THEN PROCEED RIGHT TO RESUME RETRIEVING THE ELEMENTS IN THE LIST. 
**     
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     A. DEWITT                 05/29/90  INITIAL CODE
**     L. MCMILLION              07/02/90  ADDED LOGIC TO CHECK IF FORMAT IS
**                                          DELIMITED BY PARENTHESES
**     A. DEWITT                 07/18/90  MODIFIED LOGIC TO PROCESS RIGHT
**                                          SUBTREE
**     L. MCMILLION              07/27/90  MODIFIED LOGIC TO CORRECTLY BRANCH
**                                          RIGHT FOR AN ALPHA WITH NO WIDTH NOR
**                                          USER DELIMITER
**     L. MCMILLION              08/21/90  ADDED LOGIC TO DISCARD LEADING BLANKS
**                                          IN FORMAT STRING
**     J. TAYLOR     92DR015     10/30/92  MODIFIED CALL TO FUNCTION ATOL TO
**                                          STORE RESULTS IN A TEMPORY LONG
**                                          INTEGER BEFORE USING IN ASSIGNMENT
**     L. MCMILLION  TASK #40    12/23/92  UPDATED PROLOG PER QC
**     L. MCMILLION  93DR025     03/09/93  IMPLEMENTED OPTIONAL USE OF 
**                                          AN ADDITIONAL PAIR OF PARENTHESES
**                                          FOR IMPLICIT REPETITION OF BINARY
**                                          FIELDS
**
**    PDL:
**
**     WHILE FORMAT STRING POINTER IS A BLANK DO
**        INCREMENT FORMAT STRING POINTER
**     ENDWHILE
**     IF FORMAT STRING POINTER IS EMPTY
**     THEN
**        RETURN SUCCESS
**     ENDIF
**     IF FIRST POSITION OF FORMAT CONTROL STRING IS NOT LEFT PARENTHESIS,
**     THEN
**        RETURN FAILURE
**     ENDIF
**     IF LAST POSITION OF FORMAT CONTROL STRING IS NOT RIGHT PARENTHESIS,
**     THEN
**        RETURN FAILURE
**     ENDIF
**     INITIALIZE DIGIT CHARACTER STRING TO ZERO
**
**     ALLOCATE NEW_FC
**     INITIALIZE STRUCTURE POINTERS TO NULL
**     INITIALIZE REP_NO TO 1
**     INITIALIZE D_TYPE TO NC
**     INITIALIZE WIDTH TO 0
**     INITIALIZE U_DELIM TO NC
**     SET CUR_FC TO NEW_FC
**     SET FMT_RT TO NEW_FC
**     INITIALIZE STATE TO REPETITION
**     WHILE CH_PTR NOT POINTING TO A END OF STRING CHARACTER DO
**
**        RETRIEVE CURRENT CH
**
**        CASE STATE
** 
**           WOD:
**              IF CH IS DIGIT
**              THEN
**                 CALL ATOL TO CONVERT CH TO INT
**                 WIDTH IS EQUAL TO WIDTH TIMES 10 PLUS INT
**              ELSEIF CH IS NON DIGIT AND NOT RIGHT PARENTHESIS
**              THEN 
**                 SET U_DELIM TO CH
**              ELSEIF CH IS RIGHT PARENTHESIS
**              THEN
**                 IF LEFT POINTER OF PARENT OF CUR_FC EQUALS CUR_FC
**                 THEN
**                    SET CUR_FC TO ITS BACK FIELD
**                 ENDIF
**                 SET STATE TO ALPHA
**              ENDIF
**                   
**           ALPHA: 
**              IF CH IS A COMMA
**              THEN
**                 IF AT A FORMAT NODE AND NOT A RIGHT CHILD
**                 THEN
**                    BACK UP CURRENT POINTER ONE NODE
**                 ENDIF
**                 ALLOCATE NEW_FC
**                 SET STRUCTURE POINTERS TO NULL
**                 SET BACK TO CUR_FC
**                 SET REP_NO AND WIDTH TO 0
**                 SET D_TYPE AND U_DELIM TO NC
**                 SET RIGHT POINTER OF CUR_FC TO NEW_FC
**                 SET CUR_FC TO NEW_FC
**                 SET STATE TO SLEVEL
**              ELSEIF CH IS LEFT PARENTHESIS
**              THEN
**                 SET STATE TO WOD
**              ELSEIF CH IS RIGHT PARENTHESIS
**              THEN
**                 INITIALIZE SL_FOUND FLAG TO FALSE
**                 WHILE CURRENT BACK POINTER NOT NULL AND SL_FOUND IS FALSE DO
**                    SET CURRENT FORMAT CONTROL POINTER TO ITS PARENT
**                    IF CURRENT RIGHT IS NULL AND CURRENT LEFT'S REP_NO IS NOT
**                     ZERO
**                    THEN
**                       SET SL_FOUND TO TRUE
**                    ENDIF
**                 ENDWHILE
**              ENDIF
**           
**           SLEVEL:
**              IF CH IS ALPHABETIC
**              THEN
**                 SET D_TYPE TO CH
**                 SET REP_NO TO 0
**                 SET STATE TO ALPHA
**              ELSEIF CH IS DIGIT
**              THEN
**                 CALL ATOL() TO CONVERT CH TO INT
**                 REP_NO IS EQUAL TO REP_NO TIMES 10 PLUS INT
**                 SET STATE TO REPETITION
**              ELSEIF CH IS A COMMA
**              THEN
**                 SET REP_NO TO 1
**                 SET STATE TO SLEVEL
**              ENDIF
**
**           REPETITION: 
**              IF CH IS DIGIT
**              THEN
**                 CALL ATOL TO CONVERT CH TO INT
**                 REP_NO IS EQUAL TO REP_NO TIMES 10 PLUS INT
**              ELSEIF CH IS ALPHABETIC
**              THEN
**                 ALLOCATE NEW_FC
**                 SET STRUCTURE POINTERS TO NULL
**                 SET BACK TO CUR_FC
**                 SET REP_NO AND WIDTH TO 0
**                 SET D_TYPE TO CH
**                 SET U_DELIM TO NC
**                 SET LEFT POINTER OF CUR_FC TO NEW_FC
**                 SET CUR_FC TO NEW_FC
**                 SET STATE TO ALPHA
**              ELSEIF CH IS LEFT PARENTHESIS
**              THEN
**                 ALLOCATE NEW_FC
**                 SET STRUCTURE POINTERS TO NULL
**                 SET BACK TO CUR_FC
**                 SET REP_NO AND WIDTH TO 0
**                 SET D_TYPE AND U_DELIM TO NC
**                 SET LEFT POINTER OF CUR_FC TO NEW_FC
**                 SET CUR_FC TO NEW_FC
**                 SET STATE TO LP_NXTL
**              ENDIF
**     
**           LP_NXTL:
**              IF CH IS ALPHABETIC
**              THEN
**                 SET REP_NO FIELD OF CUR_FC TO ONE
**                 ALLOCATE NEW_FC
**                 SET STRUCTURE POINTERS TO NULL
**                 SET BACK TO CUR_FC
**                 SET REP_NO AND WIDTH TO 0
**                 SET D_TYPE TO CH
**                 SET U_DELIM TO NC
**                 SET LEFT POINTER OF CUR_FC TO NEW_FC
**                 SET CUR_FC TO NEW_FC
**                 SET STATE TO ALPHA
**              ELSEIF CH IS DIGIT
**              THEN
**                 CALL ATOL TO CONVERT CH TO INT
**                 REP_NO IS EQUAL TO REP_NO TIMES 10 PLUS INT
**                 SET STATE TO REPETITION             
**              ELSEIF CH IS LEFT PARENTHESIS
**              THEN
**                 SET REPETITION NUMBER TO 1
**                 ALLOCATE NEW_FC
**                 SET STRUCTURE POINTERS TO NULL
**                 SET BACK TO CUR_FC
**                 SET REP_NO AND WIDTH TO 0
**                 SET D_TYPE AND U_DELIM TO NC
**                 SET LEFT POINTER OF CUR_FC TO NEW_FC
**                 SET CUR_FC TO NEW_FC
**                 { DO NOT CHANGE STATE }
**              ENDIF
**       END CASE
**
**       INCREMENT INDEX TO NEXT CHARACTER IN THE STRING
**
**    END WHILE                  
**                                            
**    RETURN SUCCESS
**
*****************************************************************************
**    CODE SECTION
**
*****************************************************************************/
#include "stc123.h"

int load123fmt(char *ch_ptr)

{
   /* INTERNAL VARIABLES */
   char digit_str[2];
   long i = 0       ;
   struct fc *new_fc;
   int  sl_found    ;
   int  state       ;
   long tmp_long    ;

   /* WHILE FORMAT STRING POINTER IS A BLANK DO */
   while (*ch_ptr == BLNK_SP) {

      /* INCREMENT FORMAT STRING POINTER */
      ch_ptr++;
   }

   /* IF FORMAT STRING IS EMPTY, RETURN SUCCESS */
   if (!_tcslen(ch_ptr)) return(1);

   /* IF FIRST POSITION OF FORMAT CONTROL STRING IS NOT LEFT PARENTHESIS,
       RETURN FAILURE
   */
   if (ch_ptr[0] != '(') return(0);

   /* IF LAST POSITION OF FORMAT CONTROL STRING IS NOT RIGHT PARENTHESIS,
       RETURN FAILURE
   */
   if (ch_ptr[(_tcslen(ch_ptr) - 1)] != ')') return(0);

   /* INITIALIZE DIGIT CHARACTER STRING TO ZERO */
   strcpy(digit_str,"0");

   /* ALLOCATE NEW FORMAT CONTROL STRUCTURE
       EXIT ROUTINE WITH FAILURE IF MEMORY UNAVAILABLE
   */
   new_fc = (struct fc *) malloc(sizeof(struct fc));
   if (new_fc == NULL) return(0);
 
   /* INITIALIZE NEW FORMAT CONTROL STRUCTURE */
 
   new_fc->left = NULL;
   new_fc->right = NULL;
   new_fc->back = NULL;
   new_fc->rep_no = 1;
   new_fc->d_type = NC;
   new_fc->width = 0;
   new_fc->u_delim = NC;
 
   /* SET CURRENT FORMAT CONTROL VARIABLE OF THE CURRENT LEVEL AND THE FORMAT
       ROOT OF THE CURRENT DATA DESCRIPTIVE RECORD OF THE CURRENT LEVEL TO THE
       NEW FORMAT CONTROL STRUCTURE
   */

   cur_fm->cur_fc = new_fc;
   cur_fm->cur_dd->fmt_rt = new_fc;

   /* SET STATE TO REPITITION */
   state = 3;
  
   /* WHILE INPUT CHARACTER STRING IS NOT END OF STRING CHARACTER DO */
   while (ch_ptr[i] != NC) {

      /* case state */
      switch (state) {
      
         /* CASE STATE IS WOD */
         case 1 :

            /* IF THE CURRENT CHARACTER IN THE INPUT STRING IS A DIGIT */
            if ( isdigit(ch_ptr[i]) ) {

               /* UPDATE WIDTH VALUE */
               digit_str[0] = ch_ptr[i];
               tmp_long = atol(digit_str);
               cur_fm->cur_fc->width = (cur_fm->cur_fc->width * 10) + tmp_long;
            }

            /* IF THE CURRENT CHARACTER IN THE INPUT STRING IS NOT A RIGHT PARENTHESIS */
            else if (ch_ptr[i] != ')' ) {

               /* ASSIGN CURRENT CHARACTER TO USER DEFINED DELIMITER */
               cur_fm->cur_fc->u_delim = ch_ptr[i] ;
            }

            /* IF CH IS RIGHT PARENTHESIS */
            else if (ch_ptr[i] == ')' ) {

               /* IF BACKING UP FROM LEFT CHILD */
               if (cur_fm->cur_fc->back->left  == cur_fm->cur_fc) {

                  /* UPDATE CURRENT POINTER TO PARENT OF CURRENT POINTER */
                  cur_fm->cur_fc = cur_fm->cur_fc->back;
               }

               /* SET STATE TO ALPHA */
               state = 0 ;
            }
            break;
       
         /* CASE STATE IS ALPHA */
         case 0 :

            /* IF THE CURRENT CHARACTER IS A COMMA */
            if (ch_ptr[i] == ',') {

               /* IF AT A FORMAT NODE AND NOT A RIGHT CHILD */
               if ((cur_fm->cur_fc->left == NULL) && (cur_fm->cur_fc->back->right != cur_fm->cur_fc)) {

                  /* BACK UP CURRENT POINTER ONE NODE */
                  cur_fm->cur_fc = cur_fm->cur_fc->back;
               }

               /* ALLOCATE NEW FORMAT CONTROL STRUCTURE
                   EXIT ROUTINE WITH FAILURE IF UNABLE TO DO SO
               */
               new_fc = (struct fc *) malloc(sizeof(struct fc)) ;
               if (new_fc == NULL) return(0);
             
               /* INITIALIZE FORMAT CONTROL STRUCTURE CONTENTS */
               new_fc->left = NULL;
               new_fc->right = NULL;
               new_fc->back = cur_fm->cur_fc;
               new_fc->rep_no = 0;
               new_fc->width = 0;
               new_fc->d_type = NC;
               new_fc->u_delim = NC;
            
               /* UPDATE CURRENT FORMAT CONTROL POINTER TO POINT TO NEW STRUCTURE */
               cur_fm->cur_fc->right = new_fc;
               cur_fm->cur_fc = new_fc;
            
               /* SET STATE TO SLEVEL */
               state = 2;
            }

            /* IF CURRENT CHARACTER IS A LEFT PARENTHESIS */
            else if (ch_ptr[i] == '(' ) {

               /* SET STATE TO WOD */
               state = 1;
            }

            /* IF CURRENT CHARACTER IS A RIGHT PARENTHESIS */
            else if (ch_ptr[i] == ')' ) {

               /* INITIALIZE SL_FOUND FLAG TO FALSE */
               sl_found = 0;

               /* WHILE CURRENT BACK POINTER NOT NULL AND SL_FOUND IS FALSE DO */
               while (cur_fm->cur_fc->back != NULL && !sl_found) {

                  /* SET CURRENT FORMAT CONTROL POINTER TO ITS PARENT */
                  cur_fm->cur_fc = cur_fm->cur_fc->back;

                  /* IF CURRENT RIGHT IS NULL AND CURRENT LEFT'S REP_NO
                      NOT ZERO
                  */
                  if (cur_fm->cur_fc->right == NULL && cur_fm->cur_fc->left->rep_no) {

                     /* SET SL_FOUND TO TRUE */
                     sl_found = 1;
                  }
               }
            }
            break;
         
         /* CASE STATE IS SLEVEL */
         case 2 :

            /* IF CURRENT CHARACTER IS AN ALPHABETIC CHARACTER */
            if (isalpha(ch_ptr[i]) ) {

               /* SET DATA TYPE TO CURRENT CHARACTER */
               cur_fm->cur_fc->d_type = ch_ptr[i];

               /* SET REPETITION NUMBER TO ZERO INDICATING A STRUCTURE CONTAINING
                   A FORMAT
               */
               cur_fm->cur_fc->rep_no = 0;

               /* SET STATE TO ALPHA */
               state = 0;
            }
        
            /* IF CURRENT CHARACTER IS A DIGIT */
            else if (isdigit(ch_ptr[i]) ) {

               /* UPDATE REPETITION NUMBER */
               digit_str[0] = ch_ptr[i];
               tmp_long = atol(digit_str);
               cur_fm->cur_fc->rep_no = (cur_fm->cur_fc->rep_no * 10) + tmp_long;
           
               /* SET STATE TO REPETITION */
               state = 3;
            }

            /* IF CURRENT CHARACTER IS A COMMA */
            else if (ch_ptr[i] == ',') {

               /* SET REPETITION TO 1 */
               cur_fm->cur_fc->rep_no = 1;
           
               /* SET STATE TO SLEVEL */
               state = 2;
            }
            break;
      
         /* CASE STATE IS REPETITION */
         case 3 :
      
            /* IF CURRENT CHARACTER IS A DIGIT */
            if (isdigit(ch_ptr[i]) ) {

               /* UPDATE REPETITION NUMBER */
               digit_str[0] = ch_ptr[i];
               tmp_long = atol(digit_str);
               cur_fm->cur_fc->rep_no = (cur_fm->cur_fc->rep_no * 10) + tmp_long;
            }
        
            /* IF CURRENT CHARACTER IS AN ALPHABETIC CHARACTER */
            else if (isalpha(ch_ptr[i]) ) {
           
               /* ALLOCATE NEW FORMAT CONTROL STRUCTURE
                   EXIT ROUTINE WITH FAILURE IF UNABLE TO DO SO
               */
               new_fc = (struct fc *) malloc(sizeof(struct fc)) ;
               if (new_fc == NULL) return(0);
           
               /* INITIALIZE NEW FORMAT CONTROL STRUCTURE */
               new_fc->left = NULL;
               new_fc->right = NULL;
               new_fc->back = cur_fm->cur_fc;
               new_fc->rep_no = 0;
               new_fc->width = 0;

               /* SET DATA TYPE TO CURRENT CHARACTER AND U_DELIM TO NULL CHARACTER */
               new_fc->d_type = ch_ptr[i];
               new_fc->u_delim = NC;

               /* UPDATE CURRENT FORMAT CONTROL STRUCTURE TO POINT TO NEW STRUCTURE */
               cur_fm->cur_fc->left = new_fc;
               cur_fm->cur_fc = new_fc;

               /* SET STATE TO ALPHA */
               state = 0;
            }
        
            /* IF CURRENT CHARACTER IS A LEFT PARENTHESIS */
            else if (ch_ptr[i] == '(' ) {
           
               /* ALLOCATE NEW FORMAT CONTROL STRUCTURE
                   EXIT ROUTINE WITH FAILURE IF UNABLE TO DO SO
               */
               new_fc = (struct fc *) malloc(sizeof(struct fc)) ;
               if (new_fc == NULL) return(0);

               /* INITIALIZE NEW FORMAT CONTROL STRUCTURE */
               new_fc->left = NULL;
               new_fc->right = NULL;
               new_fc->back = cur_fm->cur_fc;
               new_fc->rep_no = 0;
               new_fc->width = 0;
               new_fc->d_type = NC;
               new_fc->u_delim = NC;

               /* UPDATE CURRENT FORMAT CONTROL STRUCTURE TO POINT TO NEW STRUCTURE */
               cur_fm->cur_fc->left = new_fc;
               cur_fm->cur_fc = new_fc;

               /* SET STATE TO LP_NXTL */
               state = 4;
            }
            break;
        
         /* CASE STATE IS LP_NXTL */
         case 4 :
        
            /* IF CURRENT CHARACTER IS ALPHABETIC */
            if (isalpha(ch_ptr[i])) {

               /* SET REPETITION NUMBER TO 1 */
               cur_fm->cur_fc->rep_no = 1 ;
           
               /* ALLOCATE NEW FORMAT CONTROL STRUCTURE
                   EXIT ROUTINE WITH FAILURE IF UNABLE TO DO SO
               */
               new_fc = (struct fc *) malloc(sizeof(struct fc)) ;
               if (new_fc == NULL) return(0);

               /* INITIALIZE NEW FORMAT CONTROL STRUCTURE */
               new_fc->left = NULL;
               new_fc->right = NULL;
               new_fc->back = cur_fm->cur_fc;
               new_fc->rep_no = 0;
               new_fc->width = 0;

               /* SET DATA TYPE TO CURRENT CHARACTER AND U_DELIM TO NULL CHARACTER */
               new_fc->d_type = ch_ptr[i];
               new_fc->u_delim = NC;

               /* UPDATE CURRENT FORMAT CONTROL STRUCTURE TO POINT TO NEW STRUCTURE */
               cur_fm->cur_fc->left = new_fc;
               cur_fm->cur_fc = new_fc;

               /* SET STATE TO ALPHA */
               state = 0;
            }
            /* IF CURRENT CHARACTER IS A DIGIT */
            else if (isdigit(ch_ptr[i]) ) {

               /* UPDATE REPETITION NUMBER */
               digit_str[0] = ch_ptr[i];
               tmp_long = atol(digit_str);
               cur_fm->cur_fc->rep_no = (cur_fm->cur_fc->rep_no * 10) + tmp_long;
           
               /* SET STATE TO REPETITION */
               state = 3;
            }
        
            /* IF CURRENT CHARACTER IS A LEFT PARENTHESIS */
            else if (ch_ptr[i] == '(' ) {

               /* SET REPETITION NUMBER TO 1 */
               cur_fm->cur_fc->rep_no = 1 ;
           
               /* ALLOCATE NEW FORMAT CONTROL STRUCTURE
                   EXIT ROUTINE WITH FAILURE IF UNABLE TO DO SO
               */
               new_fc = (struct fc *) malloc(sizeof(struct fc)) ;
               if (new_fc == NULL) return(0);

               /* INITIALIZE NEW FORMAT CONTROL STRUCTURE */
               new_fc->left = NULL;
               new_fc->right = NULL;
               new_fc->back = cur_fm->cur_fc;
               new_fc->rep_no = 0;
               new_fc->width = 0;
               new_fc->d_type = NC;
               new_fc->u_delim = NC;

               /* UPDATE CURRENT FORMAT CONTROL STRUCTURE TO POINT
                   TO NEW STRUCTURE
               */
               cur_fm->cur_fc->left = new_fc;
               cur_fm->cur_fc = new_fc;

               /* { NO CHANGE IN STATE } */
            }
            break;
      
         /* STATE OUT OF BOUNDS */
         default :

            /* RETURN FUNCTION AS A FAILURE */
            return(0) ;
               
      /* END CASE */
      }
     
      /* INCREMENT INDEX TO NEXT CHARACTER IN INPUT STRING */
      i++;
     
   /* ENDWHILE */
   }

   /* RETURN SUCCESS */
   return (1);

}
