# include "stdafx.h"
/***************************************************************************
**
**     INVOCATION NAME: LD123DDREC
**     
**     PURPOSE:  TO READ THE DATA DESCRIPTIVE RECORD AND LOAD THE INFORMATION
**                INTO THE DATA STRUCTURES FOR REFERENCE
**
**     INVOCATION METHOD: LD123DDREC()
**
**     ARGUMENT LIST: 
**      NAME            TYPE     USE   DESCRIPTION
**      LD123DDREC()    LOGICAL   O    SUCCESS FLAG
**
**     EXTERNAL FUNCTION REFERENCES:
**      NAME             DESCRIPTION
**      BEG123DDREC()    DEALLOCATES PREVIOUSLY STORED DDR INFORMATION
**      CKI123NFLD()     CHECKS STRING FOR NULL FIELDS AND SEPARATES DELIMITERS
**      G123FSTR()       READS IN A FULL STRING
**      G123INT()        READS IN AN INTEGER
**      G123STR()        READS IN A STRING
**      GET123ADSCR()    RETRIEVES ARRAY DESCRIPTOR 
**      IS123ADSCR()     DETERMINES IF A CHARACTER STRING ARRAY DESCRIPTOR
**      IS123INTGR()     DETERMINES IF A CHARACTER STRING IS AN INTEGER
**      LOAD123FMT()     LOADS FORMAT CONTROLS STRING INTO A  BINARY TREE
**                        STRUCTURE
**      LOAD123LAB()     LOADS DESCRIPTIVE LABELS INTO A STRUCTURE
**      LOAD123TAGP()    LOADS TAG PAIR LIST INTO A STRUCTURE
**      STR123TOK()      RETURNS A POINTER TO A STRING TOKEN
**      VER123DDTAG()    VERIFIES THE ORDER OF DATA DESCRIPTIVE TAGS
**
**     INTERNAL VARIABLES:
**      NAME            TYPE         DESCRIPTION
**      BUFFER[]        PTR          CHARACTER POINTER TO BUFFER HOLDING 
**                                    CHARACTER DATA
**      BUF_LEN         INT          BUFFER LENGTH
**      CH              INT          INTEGER REPRESENTATION OF A CHARACTER
**      CUR_DD          PTR          POINTER TO CURRENT DD ENTRY 
**      CUR_SIZ         INT          CURRENT SIZE OF DDR DIRECTORY 
**      DIR_SIZ         INT          SIZE OF DIRECTORY 
**      FC_PTR[]        PTR          CHARACTER POINTER TO FORMAT CONTROL STRING
**      INT_TAG         INT          INTEGER VALUE OF TAG
**      LAB_PTR[]       PTR          CHARACTER POINTER TO LABEL STRING
**      NEW_CR          PTR          POINTER TO DATA DESCRIPTIVE CONTROL 
**                                    STRUCTURE
**      NEW_DD          PTR          POINTER TO DATA DESCRIPTIVE RECORD
**      NEW_DL          PTR          POINTER TO DATA DESCRIPTIVE LEADER
**      ST_BUFF[]       PTR          CHARACTER POINTER TO START OF BUFFER
**      STR_LEN         INT          STRING LENGTH
**      TMP_TOK[]       PTR          TEMPORARY CHARACTER STRING POINTER
**      TOK_LEN         LONG         LENGTH OF TOKEN RETURNED FROM STR123TOK()
**      VALIDSFLD       LOGICAL      FLAG INDICATING SUBFIELD (USUALLY
**                                    RETRIEVED VIA STR123TOK()) IS VALID,
**                                    SPECIFICALLY NOT A DELIMITER SEPARATOR.
**                                    ALSO USED TO CALCULATE STRING LENGTHS
**                                    FOR DYNAMIC MEMORY ALLOCATION
**                                     0 (INVALID SUBFIELD) - NO ADDTIONAL
**                                       CHARACTER FOR NC
**                                     1 (VALID SUBFIELD) - REQUIRES
**                                       ADDITIONAL CHARACTER FOR NC  
**  
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE FILE CONTROL RECORD STRUCTURE (CR)
**      NAME            TYPE   USE   DESCRIPTION
**      F_TITLE         CHAR    O    FILE TITLE
**      TAG_L           PTR     O    POINTER TO LIST OF TAG PAIR RECORDS
**      U_AFD[]         PTR     O    CHARACTER STRING POINTER TO USERS 
**                                    AUGMENTED FILE DESCRIPTION
**      
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    O    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     O    LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT     O    POSITION OF DATA DESCRIPTIVE AREA 
**                                    FIELD RECORD
**      FD_CNTRL[10]    CHAR    O    FIELD CONTROLS
**      NAME[]          PTR     O    CHARACTER STRING POINTER TO NAME
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS IN ARRAY DESCRIPTOR
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT    N/A   NUMBER OF ELEMENTS IN PRIMARY DIMENSION
**      LABELS          PTR    N/A   HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR     O    POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT     O    DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT     O    INTERCHANGE LEVEL
**      LEAD_ID         CHAR    O    LEADER IDENTIFIER 
**      ICE_IND         CHAR    O    INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR    O    RESERVE SPACE CHARACTER
**      APPL_IND        CHAR    O    APPLICATION INDICATOR
**      FD_CNTRL_L      INT     O    FIELD CONTROL LENGTH
**      DA_BASE         INT     O    BASE ADDRESS OF DESCRIPTIVE AREA 
**      CCS_IND[4]      CHAR    O    CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT     O    SIZE OF FIELD LENGTH 
**      S_FDPOS         INT     O    SIZE OF FIELD POSITION  
**      S_RESV          INT     O    RESERVED DIGIT
**      S_TAG           INT     O    SIZE OF FIELD TAG 
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR     I    FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**      CR_HD           PTR     O    HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR     O    HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR     O    HEAD POINTER TO DATA DESCRIPTIVE LEADER
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
**      SF_STATE_DD     INT     O    SUBFIELD STATE (DD)
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
**    GLOBAL CONSTANTS:
**     NAME         TYPE      DESCRIPTION
**     DEL_STR[3]   CHAR      DELIMITER STRING
**     FCDSTYPE     INT       RELATIVE POSITION OF THE DATA STRUCTURE TYPE
**                             WITHIN THE FIELD CONTROL
**     LEAD_LEN     INT       LENGTH OF LEADER
**     LVL23FCL     INT       FIELD CONTROL LENGTH FOR FILE INTERCHANGE LEVELS
**                             2 AND 3
**     NC           CHAR      NULL CHARACTER
**     SEP_STR[2]   CHAR      CHARACTER STRING CONTAINING SEPARATOR FOR
**                             ADJACENT DELIMITERS
**
**     CHANGE HISTORY:
**      AUTHOR      CHANGE-ID       DATE     CHANGE SUMMARY
**      A. DEWITT                 04/23/90   INITIAL PROLOG
**      A. DEWITT                 04/23/90   INITIAL PDL
**      A. DEWITT                 05/16/90   INITIAL CODE
**      A. DEWITT                 07/23/90   INSERTED ADDITIONAL MEMORY HANDLING
**                                          LOGIC AND CALLS TO FUNCTION
**                                          CKI123NFLD()
**      L. MCMILLION              09/25/90   REPLACED CALLS TO LIBRARY FUNCTION
**                                            STRTOK() WITH FUNCTION STR123TOK()
**                                            DUE TO NESTING PROBLEMS
**      L. MCMILLION              10/31/90   INSERTED LOGIC TO CHECK FOR VALID
**                                            FIELD CONTROL LENGTHS
**      L. MCMILLION  (TASK #40)  08/28/91   INSERTED LOGIC TO CHECK FOR VALID
**                                            VALUES FOR SIZE OF FIELD LENGTH, 
**                                            SIZE OF FIELD POSITION, AND SIZE
**                                            OF TAG
**      L. MCMILLION  92DR009     04/14/92   ADDED CONDITION TO CHECK FOR
**                                            DELIMITER SEPARATORS
**      J. TAYLOR     92DR005     05/20/92   CHANGED CALLING SEQUENCE TO
**                                            STR123TOK TO RETURN TOKEN LENGTH
**      L. MCMILLION  TASK #40    12/02/92   UPDATED PROLOG/PDL/INLINE COMMMENTS
**      J. TAYLOR     92DR002     11/12/92   ADDED CALL TO VER123DDTAG() TO
**                    92DR006                 VERIFY ORDER OF TAGS 0..0 - 0..9
**                                            IN DATA DESCRIPTIVE RECORD
**      J. TAYLOR     92DR002     11/17/92   ADDED CODE TO SET DD SUBFIELD STATE
**                    92DR006                 TO FINISHED BEFORE VERIFYING 
**                                            DDR TAGS
**      L. MCMILLION  93DR033     06/16/93   ADDED LOGIC TO TEST SUBFIELD FOR 
**                                            ARRAY DESCRIPTOR AND LOAD THE
**                                            STRUCTURES APPOPRIATELY
**      J. TAYLOR     93DR033     10/08/93   ADDED INITIALIZATION OF NUM_DIM,
**                                            PRIM_DMS, AND DIM_LPTR TO NEWLY
**                                            ALLOCATED DD STRUCTURE
**
**     PDL:
**
**      INITIALIZE LOCAL FILE POINTER
**      CALL BEG123DDREC TO CLEAR OUT ANY PREVIOUSLY ALLOCATED DDR STORAGE
**      ALLOCATE DATA DESCRIPTER LEADER FIELD RECORD
**      CALL G123INT() TO READ RECORD LENGTH 
**      CALL G123INT() TO READ INTERCHANGE LEVEL
**      IF INTERCHANGE LEVEL IS NOT A 1, 2, OR 3 
**      THEN
**         RETURN FAILURE
**      ENDIF
**      CALL G123STR() TO READ LEADER IDENTIFIER
**      IF LEADER IDENTIFIER IS NOT EQUAL TO 'L'
**      THEN 
**         RETURN FAILURE
**      ENDIF
**      CALL G123STR() TO READ INLINE CODE EXTENSION
**      CALL G123STR() TO READ RESERVED SPACE 
**      CALL G123STR() TO READ APPLICATION INDICATOR 
**      CALL G123INT() TO READ FIELD CONTROL INDICATOR
**      IF FILE INTERCHANGE LEVEL IS 1
**      THEN
**         IF FIELD CONTROL LENGTH NOT ZERO
**         THEN
**            RETURN FAILURE
**         ENDIF
**      ELSE
**         IF FIELD CONTROL LENGTH NOT VALID LENGTH FOR FILE LEVELS 2 AND 3
**         THEN
**            RETURN FAILURE
**         ENDIF
**      ENDIF
**      CALL G123INT() TO READ BASE ADDRESS OF DATA DESCRIPTIVE AREA
**      CALL G123STR() TO READ CODE CHARACTER SET INDICATOR
**      CALL G123INT() TO READ SIZE OF FIELD LENGTH
**      IF SIZE OF FIELD LENGTH IS NOT VALID SIZE
**      THEN
**         RETURN FAILURE
**      ENDIF
**      CALL G123INT() TO READ SIZE OF FIELD POSITION
**      IF SIZE OF FIELD POSITION IS NOT VALID SIZE
**      THEN
**         RETURN FAILURE
**      ENDIF
**      CALL G123INT() TO READ RESERVED DIGIT
**      CALL G123INT() TO READ SIZE OF FIELD TAG
**      IF SIZE OF FIELD TAG IS NOT VALID SIZE
**      THEN
**         RETURN FAILURE
**      ENDIF
**
**      STORE NEW DDR LEADER POINTER IN CURRENT FM RECORD
**      COMPUTE SIZE OF DATA DESCRIPTIVE DIRECTORY USING THE BASE ADDRESS OF
**       THE DESCRIPTIVE AREA, THE SIZE OF THE DDR LEADER, AND THE FT POSITION
**       FOLLOWING THE DIRECTORY
**      INITIALIZE CUR_SIZE TO ZERO
**      WHILE CUR_SIZE IS LESS THAN THE DIR_SIZE DO
** 
**         ALLOCATE NEW_DD STRUCTURE
**         INITIALIZE POINTERS AND STRINGS TO NULL
**         INITIALIZE NUMBER OF DIMENSIONS AND PRIMARY DIMENSIONS TO ZERO
**         SET NEXT POINTER FIELD OF CUR_DD TO NEW ENTRY
**         SET CUR_DD TO NEW_DD STRUCTURE
**
**         CALL G123STR() TO READ IN TAG STRING
**         CALL G123INT() TO READ IN FIELD LENGTH 
**         CALL G123INT() TO READ IN FIELD POSITION 
**
**         INCREMENT CUR_SIZ BY TAG SIZE, FIELD SIZE, AND POSITION SIZE
**
**      ENDWHILE
**
**      READ PAST FIELD TERMINATOR
**      INITIALIZE CURRENT ENTRY TO THE FIRST DIRECTORY ENTRY (DD_HD)
**      WHILE THERE ARE MORE DIRECTORY ENTRIES DO
**
**         IF TAG STRING IS ALL NUMERIC {INT}
**         THEN 
**            CONVERT TAG STRING TO INT VALUE AND STORE IN INT_TAG
**         ELSE
**            SET INT_TAG TO NEGATIVE ONE
**         ENDIF
**          
**         IF THE ENTRY IS A FILE CONTROL ENTRY {TAG OF 0..0}
**         THEN
**            ALLOCATE FILE CONTROL STRUCTURE (CR)
**            INITIALIZE UNUSED POINTERS TO NULL
**            STORE NEW STRUCTURE IN FM
**            IF THE INTERCHANGE LEVEL IS 2 OR 3
**            THEN
**               CALL G123STR() TO READ FIELD CONTROLS
**            ENDIF
**
**            RETRIEVE FIELD LENGTH AND ALLOCATE SPACE FOR BUFFER
**            CALL G123FSTR() TO READ IN FILE TITLE AND LIST OF TAG
**             PAIRS
**            CALL CKI123NFLD() TO ENSURE THAT DELIMITERS IN BUFFER ARE NOT
**             ADJACENT TO EACH OTHER
**            SET START BUFFER POINTER TO BEGINNING OF BUFFER
**            CALL STR123TOK() TO DIVIDE THE BUFFER INTO TWO STRINGS. ONE
**             STRING CONTAINING THE TITLE AND THE OTHER CONTIANING THE LIST
**             OF TAG PAIRS.
**            IF TEMPORARY TOKEN NOT NULL
**            THEN
**               IF TEMPORARY TOKEN IS NOT A DELIMITER SEPARATOR
**               THEN
**                  SET VALID SUBFIELD FLAG TO VALID
**               ELSE
**                  SET VALID SUBFIELD FLAG TO INVALID
**               ENDIF
**               SET BUFFER LENGTH TO LENGTH OF TEMPORARY TOKEN--INCLUDE
**                VALID SUBFIELD FLAG IN LENGTH CALCULATION
**               ALLOCATE SPACE FOR F_TITLE
**               IF BUFFER LENGTH GREATER THAN ONE
**               THEN
**                  SET F_TITLE TO TEMPORARY TOKEN
**               ELSE BUFFER LENGTH LESS THAN ONE
**                  FREE F_TITLE SPACE
**                  SET F_TITLE TO NULL
**               ENDIF
**
**               INITIALIZE TAG_L POINTER TO NULL
**               CALL STR123TOK() TO RETURN POINTER TO STRING CONTAINING LIST
**                OF TAG PAIRS
**
**               IF LIST OF ORDERED TAG PAIRS PRESENT
**               THEN
**                  CALL LOAD123TAGP() TO STORE TAG PAIRS LIST INTO STRUCTURE
**               ENDIF
**            ENDIF
**            FREE SPACE AT START OF BUFFER
**
**         ELSEIF ENTRY IS USER AUGMENTED FILE DESCRIPTION { TAG OF 0..2 }
**         THEN
**            IF FILE CONTROL STRUCTURE (CR) NOT ALLOCATED
**            THEN
**               ALLOCATE FILE CONTROL STRUCTURE
**               INITIALIZE UNUSED POINTERS TO NULL
**               STORE NEW STRUCTURE IN FM
**            ENDIF
**            RETRIEVE SIZE OF AUGMENTED FILE DESCRIPTION
**            ALLOCATE AUGMENTED FILE DESCRIPTION FIELD
**            CALL G123STR() TO READ IN AUGMENTED FILE DESCRIPTION
**             FIELD
**
**         ELSE { USER DATA ENTRY }
**
**            IF THE INTERCHANGE LEVEL IS 1
**            THEN
**               COMPUTE LENGTH OF NAME FIELD
**               ALLOCATE SPACE FOR NAME STRING
**               CALL G123STR() TO READ IN NAME FIELD
**            ELSEIF INTERCHANGE LEVEL IS 2 OR 3 
**            THEN
**               CALL G123STR() TO READ IN CONTROL FIELD
**               IF CONTROL FIELD INDICATES ELEMENTARY DATA  
**               THEN
**                  COMPUTE BUFFER SPACE NEEDED FOR NAME AND FORMAT
**                   CONTROLS WITH DELIMITERS
**                  IF CONTROLS PRESENT
**                  THEN
**                     ALLOCATE SPACE FOR BUFFER
**                     CALL G123FSTR() TO READ IN THE NAME AND FORMAT
**                      CONTROLS INTO BUFFER
**                     CALL CKI123NFLD() TO ENSURE THAT DELIMITERS IN BUFFER ARE
**                      NOT ADJACENT TO EACH OTHER
**                     SET START OF BUFFER TO BEGINNING OF BUFFER
**                     CALL STR123TOK() TO DIVIDE BUFFER INTO TWO STRINGS, ONE
**                      CONTAINING THE NAME FIELD AND THE OTHER STRING
**                      CONTAINING THE FORMAT CONTROLS
**
**                     IF TEMPORARY TOKEN IS NOT A DELIMITER SEPARATOR
**                     THEN
**                        SET VALID SUBFIELD FLAG TO VALID
**                     ELSE
**                        SET VALID SUBFIELD FLAG TO INVALID
**                     ENDIF
**                     SET STRING LENGTH TO LENGTH OF TEMPORARY TOKEN
**                      --INCLUDE VALID SUBFIELD FLAG IN LENGTH CALCULATION
**                     ALLOCATE SPACE FOR NAME
**                     IF VALID SUBFIELD
**                     THEN
**                        SET NAME EQUAL TO POINTER RETURNED FROM STR123TOK()
**                     ENDIF
**                     CALL STR123TOK() TO RETRIEVE POINTER TO FORMAT CONTROLS
**                      STRING
**                     SET FC_PTR EQUAL TO THAT POINTER
**                     IF FORMAT CONTROLS PRESENT
**                     THEN
**                        CALL LOAD123FMT() TO DECODE FORMAT STRING AND PLACE IN
**                         A TREE STRUCTURE INITIALIZING FORMAT ROOT
**                     ENDIF
**                     FREE SPACE AT START OF BUFFER
**                  ENDIF
**
**               ELSE { VECTOR OR CARTESIAN DATA }
**                  COMPUTE BUFFER SPACE NEEDED FOR NAME, LABELS AND
**                   FORMAT CONTROLS WITH DELIMITERS
**                  CALL G123FSTR() TO READ IN THE NAME, LABELS AND FORMAT CONTROLS
**                  CALL CKI123NFLD() TO ENSURE THAT DELIMITERS IN BUFFER ARE
**                   NOT ADJACENT TO EACH OTHER
**                  SET START OF BUFFER TO BEGINNING OF BUFFER
**                  CALL STR123TOK() TO DIVIDE BUFFER INTO TOKEN STRINGS WITH
**                   THE FIRST STRING CONTAINING THE NAME, THE SECOND STRING
**                   CONTAINING THE LABELS, AND THE THIRD STRING CONTAINING
**                   THE FORMAT CONTROLS
**
**                  IF STR123TOK() RETURNED A NON NULL POINTER {CONTROLS}
**                  THEN
**                     IF TEMPORARY TOKEN IS NOT A DELIMITER SEPARATOR
**                     THEN
**                        SET VALID SUBFIELD FLAG TO VALID
**                     ELSE
**                        SET VALID SUBFIELD FLAG TO INVALID
**                     ENDIF
**                     SET STRING LENGTH OF LENGTH OF TEMPORARY TOKEN
**                      --INCLUDE VALID SUBFIELD FLAG IN LENGTH CALCULATION
**                     ALLOCATE SPACE FOR NAME
**                     IF VALID SUBFIELD
**                     THEN
**                        SET NAME POINTER TO THE POINTER RETURNED
**                         FROM STR123TOK()
**                     ENDIF
**                     CALL STR123TOK() TO RETURN LAB_PTR
**                     CALL STR123TOK() TO RETURN FC_PTR
**                     IF LABELS POINTER NOT NULL
**                     THEN 
**                        IF CURRENT DD FIELD CARTESIAN AND LABELS POINTER IS
**                         ARRAY DESCRIPTOR {CALL IS123ADSCR()}
**                        THEN
**                           CALL GET123ADSCR() TO RETRIEVE THE ARRAY DESCRIPTOR
**                            INTO THE STRUCTURE
**                        ELSE IF LABELS ARE PRESENT
**                           CALL LOAD123LAB() TO LOAD LABELS INTO STRUCTURES
**                        ENDIF
**                     ENDIF
**                     IF FORMAT CONTROLS PRESENT
**                     THEN
**                        CALL LOAD123FMT() TO DECODE FORMAT STRING AND PLACE IN
**                         A TREE STRUCTURE INITIALIZING FORMAT ROOT
**                     ENDIF
**                  ENDIF
**                  FREE SPACE AT START OF BUFFER
**               ENDIF        
**
**            ENDIF
**         ENDIF
**                           
**         SET CUR_DD TO CUR_DD NEXT POINTER
**      ENDWHILE
**      SET CUR_DD TO DD_HD
**      SET DD SUBFIELD STATE TO FINISHED
**      CALL VER123DDTAG() TO VERIFY ORDER OF TAGS 0..0 - 0..9 IN
**       DATA DESCRIPTIVE RECORD
**
**      RETURN SUCCESS
**
****************************************************************************
**      CODE SECTION
**
****************************************************************************/
#include "stc123.h"

int ld123ddrec()

{
   /* DECLARE LOCAL VARIABLES */
   FILE *fp           ;
   int       ch       ;
   size_t    buf_len  ;
   size_t    str_len  ;
   long      cur_siz  ;
   long      dir_siz  ;
   long      int_tag  ;
   long      tok_len  ;
   char      *buffer  ;
   char      *lab_ptr ;
   char      *fc_ptr  ;
   char      *st_buff ;
   char      *tmp_tok ;
   struct cr *new_cr  ;
   struct dd *new_dd  ;
   struct dl *new_dl  ;
   int       validsfld;
   
   /* INITIALIZE LOCAL FILE POINTER */
   fp = cur_fm->fp;
 
   /* CALL BEG123DDREC TO CLEAR OUT ANY PREVIOUSLY ALLOCATED DD STORAGE */
   if (!beg123ddrec(fp)) return(0);

   /* ALLOCATE DATA DESCRIPTER LEADER FIELD RECORD  */
   if ((new_dl = (struct dl *) malloc(sizeof(struct dl))) == NULL) return(0);
 
   /* CALL G123INT() TO READ RECORD LENGTH */
   if (!g123int(fp,5L,&new_dl->rec_len))  return(0);
 
   /* CALL G123INT() TO READ INTERCHANGE LEVEL */
   if (!g123int(fp,1L,&new_dl->ilevel)) return(0);

   /* IF INTERCHANGE LEVEL IS NOT A 1, 2, OR 3  RETURN FAILURE */
   if ((new_dl->ilevel != 1) && (new_dl->ilevel != 2) && (new_dl->ilevel != 3)) return(0);

   /* CALL G123STR() TO READ LEADER IDENTIFIER */
   if (!g123str(fp,&new_dl->lead_id,1L)) return(0);
   
   /*  IF LEADER IDENTIFIER IS NOT EQUAL TO 'L'  RETURN FAILURE */
   if (new_dl->lead_id != 'L') return(0);
   
   /* CALL G123STR() TO READ INLINE CODE EXTENSION */
   if (!g123str(fp,&new_dl->ice_ind,1L)) return(0);
  
   /* CALL G123STR() TO READ RESERVED SPACE */ 
   if (!g123str(fp,&new_dl->resv_sp,1L)) return(0);
  
   /* CALL G123STR() TO READ APPLICATION INDICATOR */
   if (!g123str(fp,&new_dl->appl_ind,1L)) return(0);
  
   /* CALL G123INT() TO READ FIELD CONTROL INDICATOR */
   if (!g123int(fp,2L,&new_dl->fd_cntrl_l)) return(0);
  
   /* IF FILE INTERCHANGE LEVEL IS 1 */
   if (new_dl->ilevel == 1) {

      /* IF FIELD CONTROL LENGTH NOT ZERO, RETURN FAILURE */
      if (new_dl->fd_cntrl_l != 0) return(0);
   }
   else {

      /* IF FIELD CONTROL LENGTH NOT VALID LENGTH FOR FILE LEVELS 2 AND 3,
          RETURN FAILURE
      */
      if (new_dl->fd_cntrl_l != LVL23FCL) return(0);
   }

   /* CALL G123INT() TO READ BASE ADDRESS OF DATA DESCRIPTIVE AREA */
   if (!g123int(fp,5L,&new_dl->da_base))  return(0);

  
   /* CALL G123STR() TO READ CODE CHARACTER SET INDICATOR */
   if (!g123str(fp,new_dl->ccs_ind,3L)) return(0);
  
   /* CALL G123INT() TO READ SIZE OF FIELD LENGTH */
   if (!g123int(fp,1L,&new_dl->s_fdlen)) return(0);
 
   /* IF SIZE OF FIELD LENGTH IS NOT VALID SIZE, RETURN FAILURE */
   if (new_dl->s_fdlen < 1 || new_dl->s_fdlen > 9) return(0); 
 
   /* CALL G123INT() TO READ SIZE OF FIELD POSITION */
   if (!g123int(fp,1L,&new_dl->s_fdpos)) return(0);

   /* IF SIZE OF FIELD POSITION IS NOT VALID SIZE, RETURN FAILURE */
   if (new_dl->s_fdpos < 1 || new_dl->s_fdpos > 9) return(0);
  
   /* CALL G123INT() TO READ RESERVED DIGIT */
   if (!g123int(fp,1L,&new_dl->s_resv)) return(0);
  
   /* CALL G123INT() TO READ SIZE OF FIELD TAG */
   if (!g123int(fp,1L,&new_dl->s_tag)) return(0);
  
   /* IF SIZE OF TAG IS NOT VALID SIZE, RETURN FAILURE */
   if (new_dl->s_tag < 1 || new_dl->s_tag > 7) return(0);

  /* STORE NEW DDR LEADER POINTER IN CURRENT FM RECORD */
   cur_fm->dl_hd = new_dl;

   /* COMPUTE SIZE OF DATA DESCRIPTIVE DIRECTORY USING THE BASE ADDRESS OF  
       THE DESCRIPTIVE AREA, THE SIZE OF THE DDR LEADER, AND THE FT POSITION
       FOLLOWING THE DIRECTORY
   */
   dir_siz = cur_fm->dl_hd->da_base - LEAD_LEN - 1;
 
   /* INITIALIZE CUR_SIZE TO ZERO */
   cur_siz = 0;

   /* WHILE CUR_SIZE IS LESS THAN THE DIR_SIZE DO */
   while (cur_siz < dir_siz)   {

      /* ALLOCATE NEW_DD STRUCTURE */
      if ((new_dd = (struct dd *) malloc(sizeof(struct dd))) == NULL) return(0);
    
      /* INITIALIZE POINTERS AND STRINGS TO NULL */
      new_dd->fd_cntrl[FCDSTYPE] = NC;
      new_dd->name = NULL;
      new_dd->labels = NULL;
      new_dd->fmt_rt = NULL;
      new_dd->next = NULL;
      new_dd->dim_lptr = NULL;

      /* INITIALIZE NUMBER OF DIMENSIONS AND PRIMARY DIMENSIONS TO ZERO */
      new_dd->num_dim = 0L;
      new_dd->prim_dms = 0L;

      /* SET NEXT POINTER FIELD OF CUR_DD TO NEW ENTRY */
      cur_fm->cur_dd->next = new_dd;
    
      /* SET CUR_DD TO NEW_DD STRUCTURE */
      cur_fm->cur_dd = new_dd;

      /* CALL G123STR() TO READ IN TAG STRING */
      if (!g123str(fp,cur_fm->cur_dd->tag,cur_fm->dl_hd->s_tag)) return(0);
    
      /* CALL G123INT() TO READ IN FIELD LENGTH */
      if (!g123int(fp,cur_fm->dl_hd->s_fdlen,&cur_fm->cur_dd->fd_len)) return(0);

      /* CALL G123INT() TO READ IN FIELD POSITION */
      if (!g123int(fp,cur_fm->dl_hd->s_fdpos,&cur_fm->cur_dd->fd_pos)) return(0);

      /* INCREMENT CUR_SIZ BY TAG SIZE, FIELD SIZE, AND POSITION SIZE */
      cur_siz = cur_siz + cur_fm->dl_hd->s_tag + cur_fm->dl_hd->s_fdlen + cur_fm->dl_hd->s_fdpos;
   } 
   
   /* READ PAST FIELD TERMINATOR */
   ch = fgetc(fp);
    
   /* INITIALIZE CURRENT ENTRY TO THE FIRST DIRECTORY ENTRY  */
   cur_fm->cur_dd = cur_fm->dd_hd->next;
 
   /* WHILE THERE ARE MORE DIRECTORY ENTRIES DO */
   while (cur_fm->cur_dd != NULL) {

      /* IF TAG STRING IS ALL NUMERIC */
      if (is123intgr(cur_fm->cur_dd->tag)) {

         /* CONVERT TAG STRING TO INTEGER VALUE AND STORE IN INT_TAG */
         int_tag = atol(cur_fm->cur_dd->tag);
      }
      else {

       /* SET INT_TAG TO NEGATIVE ONE */
       int_tag = -1;
      }
           
      /* IF THE ENTRY IS A FILE CONTROL ENTRY {TAG OF 0..0} */
      if (int_tag == 0) {

         /* ALLOCATE FILE CONTROL STRUCTURE (CR) */
         if ((new_cr = (struct cr *) malloc(sizeof(struct cr))) == NULL) return(0);

         /* INITIALIZE UNUSED POINTER TO NULL */
         new_cr->u_afd = NULL;

         /* STORE NEW_CR TO FM STRUCTURE */
         cur_fm->cr_hd = new_cr;
       
         /* IF THE INTERCHANGE LEVEL IS 2 OR 3 */
         if (cur_fm->dl_hd->ilevel == 2 || cur_fm->dl_hd->ilevel == 3) {

            /* CALL G123STR() TO READ FIELD CONTROLS */
            if (!g123str(fp,cur_fm->cur_dd->fd_cntrl,cur_fm->dl_hd->fd_cntrl_l)) return(0);
         }

         /* SET THE BUFFER SIZE NEEDED FOR FILE TITLE, THE UT,
             AND THE LIST OF TAG PAIRS INCLUDING THE FT TO FIELD
             LENGTH AND ALLOCATE SPACE FOR BUFFER
         */
         if ((buffer = (char *) malloc((size_t)(cur_fm->cur_dd->fd_len - cur_fm->dl_hd->fd_cntrl_l + 1) * sizeof(char))) == NULL) return(0);

         /* CALL G123FSTR() TO READ IN FILE TITLE AND LIST OF TAG
             PAIRS
         */
         if (!g123fstr(fp,buffer,(cur_fm->cur_dd->fd_len - cur_fm->dl_hd->fd_cntrl_l))) return(0);

         /* CALL CKI123NFLD() TO ENSURE THAT DELIMITERS IN BUFFER ARE NOT
             ADJACENT TO EACH OTHER
         */
         if (!cki123nfld(&buffer)) return(0);

         /* SET START BUFFER POINTER TO BEGINNING OF BUFFER */
         st_buff = buffer;

         /* CALL STR123TOK() TO DIVIDE THE BUFFER INTO TWO STRINGS. ONE STRING
             CONTAINING THE TITLE AND THE OTHER CONTAINING THE LIST OF TAG
             PAIRS.
         */
         tmp_tok = str123tok(&buffer,DEL_STR,&tok_len);

         /* IF TEMPORARY TOKEN NOT NULL */
         if (tmp_tok != NULL) {

            /* IF TEMPORARY TOKEN NOT DELIMITER SEPARATOR */
            if (strcmp(tmp_tok,SEP_STR))

               /* SET VALID SUBFIELD FLAG TO VALID */
               validsfld = 1;
            else

               /* SET VALID SUBFIELD FLAG TO INVALID */
               validsfld = 0;

            /* SET BUFFER LENGTH TO LENGTH OF TEMPORARY TOKEN--INCLUDE
                VALID SUBFIELD FLAG IN LENGTH CALCULATION
            */
            buf_len = (size_t)_tcslen(tmp_tok) + validsfld;

            /* ALLOCATE SPACE FOR F_TITLE */
            if ((cur_fm->cr_hd->f_title = (char *) malloc(buf_len * sizeof(char))) == NULL) return(0);

            /* IF BUFFER LENGTH GREATER THAN ONE */
            if (buf_len > 1) {

               /* COPY TEMPORARY TOKEN TO F_TITLE */
               strcpy(cur_fm->cr_hd->f_title,tmp_tok);
            }

            /* ELSE BUFFER LENGTH LESS THAN ONE */
            else {

               /* FREE F_TITLE SPACE */
               free(cur_fm->cr_hd->f_title);

               /* SET F_TITLE TO NULL */
               cur_fm->cr_hd->f_title = NULL;
            }

            /* INITIALIZE TAG_L POINTER TO NULL */
            cur_fm->cr_hd->tag_l = NULL;

            /* CALL STR123TOK() TO RETURN POINTER TO STRING CONTAINING LIST OF
                TAG PAIRS
            */
            tmp_tok = str123tok(&buffer,DEL_STR,&tok_len);

            /* IF LIST OF ORDERED TAG PAIRS PRESENT */
            if (tmp_tok != NULL && strcmp(tmp_tok,SEP_STR)) {

               /* CALL LOAD123TAGP TO LOAD LIST OF TAG PAIRS INTO STRUCTURE */
               if (!load123tagp(tmp_tok)) return(0);
            }
         }

         /* FREE SPACE AT START OF BUFFER */
         free(st_buff);
      }                 

      /* ELSEIF ENTRY IS USER AUGMENTED FILE DESCRIPTION { TAG OF 0..2 } */
      else if (int_tag == 2) {      

         /* IF FILE CONTROL STRUCTURE (CR) NOT ALLOCATED */
         if (cur_fm->cr_hd == NULL) {

            /* ALLOCATE FILE CONTROL STRUCTURE (CR) */
            if ((new_cr = (struct cr *) malloc(sizeof(struct cr))) == NULL) return(0);

            /* INITIALIZE UNUSED POINTERS TO NULL */
            new_cr->f_title = NULL;
            new_cr->tag_l = NULL;

            /* STORE NEW_CR TO FM STRUCTURE */
            cur_fm->cr_hd = new_cr;
         }

         /* RETRIEVE SIZE OF AUGMENTED FILE DESCRIPTION AND ALLOCATE
             AUGMENTED FILE DESCRIPTION FIELD 
         */
         if ((cur_fm->cr_hd->u_afd = (char *) malloc((size_t)(cur_fm->cur_dd->fd_len + 1) * sizeof(char))) == NULL) return(0);
       
         /* CALL G123STR() TO READ IN AUGMENTED FILE DESCRIPTION FIELD */
         if (!g123str(fp,cur_fm->cr_hd->u_afd,cur_fm->cur_dd->fd_len)) return(0);
             
      }
      /* ELSE { USER DATA ENTRY } */
      else {

         /* IF THE INTERCHANGE LEVEL IS 1 */
         if (cur_fm->dl_hd->ilevel == 1) {

            /* COMPUTE LENGTH OF NAME FIELD AND ALLOCATE SPACE FOR STRING */
            if ((cur_fm->cur_dd->name = (char *) malloc((size_t)(cur_fm->cur_dd->fd_len + 1) * sizeof(char))) == NULL) return(0);
          
            /* CALL G123STR() TO READ IN NAME FIELD */
            if (!g123str(fp,cur_fm->cur_dd->name,cur_fm->cur_dd->fd_len)) return(0);
         }
       
         /* ELSEIF INTERCHANGE LEVEL IS 2 OR 3  */
         else if ((cur_fm->dl_hd->ilevel == 2) || (cur_fm->dl_hd->ilevel == 3)) {
       
            /* CALL G123STR() TO READ IN CONTROL FIELD */
            if (!g123str(fp,cur_fm->cur_dd->fd_cntrl,cur_fm->dl_hd->fd_cntrl_l)) return(0);
           
            /* IF CONTROL FIELD INDICATES ELEMENTARY DATA */
            if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '0') {
           
               /* COMPUTE BUFFER SPACE NEEDED FOR NAME AND FORMAT 
                   CONTROLS WITH DELIMITERS 
               */
               buf_len = (size_t)(cur_fm->cur_dd->fd_len - (cur_fm->dl_hd->fd_cntrl_l));
              
               /* IF CONTROLS PRESENT */
               if (buf_len > 1) {  

                  /* ALLOCATE BUFFER SPACE */
                  if ((buffer = (char *) malloc((size_t)(buf_len + 1) * sizeof(char))) == NULL) return(0);

                  /* CALL G123FSTR() TO READ IN THE NAME AND FORMAT
                      CONTROLS INTO BUFFER 
                  */
                  if (!g123fstr(fp,buffer,(long)buf_len)) return(0);
                                     
                  /* CALL CKI123NFLD() TO ENSURE THAT DELIMITERS IN BUFFER ARE
                      NOT ADJACENT TO EACH OTHER
                  */
                  if (!cki123nfld(&buffer)) return(0);  

                  /* SET START BUFFER POINTER TO BEGINNING OF BUFFER */
                  st_buff = buffer;

                  /* CALL STR123TOK() TO DIVIDE BUFFER INTO TWO STRINGS, ONE
                      CONTAINING THE NAME FIELD AND THE OTHER STRING CONTAINING
                      THE FORMAT CONTROLS
                  */
                  tmp_tok = str123tok(&buffer,DEL_STR,&tok_len);

                  /* IF TEMPORARY TOKEN IS NOT A DELIMITER SEPARATOR */
                  if (strcmp(tmp_tok,SEP_STR))

                     /* SET VALID SUBFIELD FLAG TO VALID */
                     validsfld = 1;
                  else

                     /* SET VALID SUBFIELD FLAG TO INVALID */
                     validsfld = 0;

                  /* FIND LENGTH OF TEMPORARY TOKEN--INCLUDE VALID SUBFIELD
                      FLAG IN LENGTH CALCULATION
                  */
                  str_len = _tcslen(tmp_tok) + validsfld;

                  /* ALLOCATE SPACE FOR NAME */
                  if ((cur_fm->cur_dd->name = (char *) malloc(str_len * sizeof(char))) == NULL) return(0);
                  *cur_fm->cur_dd->name = NC;

                  /* IF VALID SUBFIELD, SET NAME EQUAL TO VALUE RETURNED
                      FROM STR123TOK()
                  */
                  if (validsfld) strcpy(cur_fm->cur_dd->name,tmp_tok);

                  /* RETURN FORMAT CONTROLS STRING IF ONE EXISTS */
                  fc_ptr = str123tok(&buffer,DEL_STR,&tok_len);
                 
                  /* IF FORMAT CONTROLS ARE PRESENT */
                  if (fc_ptr != NULL && strcmp(fc_ptr,SEP_STR)) {

                     /* CALL LOAD123FMT() TO DECODE FORMAT STRING AND PLACE IN
                         A TREE STRUCTURE INITIALIZING FORMAT ROOT
                     */
                     if (!load123fmt(fc_ptr)) return(0);
                  }

                  /* FREE SPACE AT START OF BUFFER */
                  free(st_buff);
               }
            }
            /* ELSE { VECTOR OR CARTESIAN DATA } */
            else {

               /* COMPUTE BUFFER SPACE NEEDED FOR NAME, LABELS AND FORMAT
                   CONTROLS WITH DELIMITERS AND ALLOCATE SPACE FOR BUFFER
               */
               buf_len = (size_t) (cur_fm->cur_dd->fd_len - cur_fm->dl_hd->fd_cntrl_l);
               if ((buffer = (char *) malloc((size_t)(buf_len + 1) * sizeof(char))) == NULL) return(0);

               /* CALL G123FSTR() TO READ IN THE NAME, LABELS AND FORMAT CONTOLS */
               if (!g123fstr(fp,buffer,(long)buf_len)) return(0);

               /* CALL CKI123NFLD() TO ENSURE THAT DELIMITERS IN BUFFER ARE NOT
                   ADJACENT TO EACH OTHER
               */
               if (!cki123nfld(&buffer)) return(0);  

               /* SET START BUFFER POINTER TO BEGINNING OF BUFFER */
               st_buff = buffer;
                 
               /* CALL STR123TOK() TO DIVIDE BUFFER INTO TOKEN STRINGS WITH
                   THE FIRST STRING CONTAINING THE NAME, THE SECOND STRING
                   CONTAINING THE LABELS, AND THIRD STRING CONTAINING THE
                   FORMAT CONTROLS
               */
               tmp_tok = str123tok(&buffer,DEL_STR,&tok_len);

               /* IF STR123TOK() RETURNED A NON NULL POINTER {CONTROLS} */
               if (tmp_tok != NULL ) {

                  /* IF TEMPORARY TOKEN IS NOT A DELIMITER SEPARATOR */
                  if (strcmp(tmp_tok,SEP_STR))

                     /* SET VALID SUBFIELD FLAG TO VALID */
                     validsfld = 1;
                  else

                     /* SET VALID SUBFIELD FLAG TO INVALID */
                     validsfld = 0;

                  /* SET STRING LENGTH TO LENGTH OF TEMPORARY TOKEN--INCLUDE
                      VALID SUBFIELD FLAG IN LENGTH CALCULATION
                  */
                  str_len = _tcslen(tmp_tok) + validsfld;

                  /* ALLOCATE SPACE FOR NAME */
                  if ((cur_fm->cur_dd->name = (char *) malloc(str_len * sizeof(char))) == NULL) return(0);
                  *cur_fm->cur_dd->name = NC;

                  /* IF VALID SUBFIELD, SET NAME POINTER TO THE
                      POINTER RETURNED FROM STR123TOK()
                  */
                  if (validsfld) strcpy(cur_fm->cur_dd->name,tmp_tok);
                
                  /* CALL STR123TOK() TO RETURN LAB_PTR */
                  lab_ptr = str123tok(&buffer,DEL_STR,&tok_len);
                 
                  /* CALL STR123TOK() TO RETURN FC_PTR */
                  fc_ptr = str123tok(&buffer,DEL_STR,&tok_len);

                  /* IF LABELS POINTER NOT NULL */
                  if (lab_ptr != NULL) {

                     /* IF CURRENT DD FIELD CARTESIAN AND LABELS POINTER IS
                         ARRAY DESCRIPTOR {CALL IS123ADSCR()}
                     */
                     if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '2' &&
                      is123adscr(lab_ptr)) {

                        /* CALL GET123ADSCR() TO RETRIEVE THE ARRAY DESCRIPTOR
                            INTO THE STRUCTURE
                        */
                        if (!get123adscr(lab_ptr)) return(0);
                     }

                     /* ELSE IF LABELS ARE PRESENT */
                     else if (strcmp(lab_ptr,SEP_STR)) {

                        /* CALL LOAD123LAB() TO LOAD LABELS INTO STRUCTURES */
                        if (!load123lab(lab_ptr)) return(0);
                     }
                  }
                  
                  /* IF FORMAT CONTROLS ARE PRESENT */
                  if (fc_ptr != NULL && strcmp(fc_ptr,SEP_STR)) {

                     /* CALL LOAD123FMT() TO DECODE FORMAT STRING AND PLACE IN
                         A TREE STRUCTURE INITIALIZING FORMAT ROOT
                     */
                     if (!load123fmt(fc_ptr)) return(0);
                  }
               }

               /* FREE SPACE AT START OF BUFFER+ */
               free(st_buff);
            }
         }

      }
      
      /* SET CUR_DD TO CUR_DD NEXT POINTER */
      cur_fm->cur_dd = cur_fm->cur_dd->next;
      
   }

   /* SET CUR_DD TO DD_HD */
   cur_fm->cur_dd = cur_fm->dd_hd;

   /* SET DD SUBFIELD STATE TO FINISHED */
   cur_fm->sf_state_dd = 5;

   /* CALL VER123DDTAG() TO VERIFY ORDER OF TAGS 0..0 - 0..9 IN 
       DATA DESCRIPTIVE RECORD
   */
   if (!ver123ddtag()) return(0);

   /* RETURN SUCCESS */
   return(1);
}
