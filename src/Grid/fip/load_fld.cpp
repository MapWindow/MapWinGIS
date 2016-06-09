# include "stdafx.h"
/***************************************************************************
**
**     INVOCATION NAME: LOAD123FLD
**     
**     PURPOSE:  TO RETRIEVE THE DR FIELD FROM THE INDICATED STRING AND LOAD
**                THE DATA INTO THE DATA STRUCTURES FOR REFERENCE
**
**     INVOCATION METHOD: LOAD123FLD(IN_STR,IN_STR_END,COMPRSSD)
**
**     ARGUMENT LIST:
**     NAME           TYPE     USE   DESCRIPTION
**     IN_STR[]       PTR       I    POINTER TO AN INPUT STRING
**     IN_STR_END[]   PTR       I    POINTER TO END OF INPUT STRING
**     COMPRSSD       LOGICAL   I    INDICATOR OF COMPRESSED ADJACENT
**                                    FIXED-LENGTH BINARY SUBFIELDS
**     LOAD123FLD()   LOGICAL   O    SUCCESS FLAG
**
**     EXTERNAL FUNCTION REFERENCES:
**      NAME             DESCRIPTION
**      G123DSTR()       READS A STRING INTO A CHARACTER BUFFER BASED ON
**                        DELIMITER
**      GET123DIM()      INPUT AND STORE DIMENSION INFORMATION
**      GET123DVAL()     READS IN A DATA VALUE STRING BASED ON FORMAT CONTROL
**      GET123FMT()      RETRIEVES A SINGLE FORMAT FROM FORMAT STRUCTURE
**      RET123DV()       RETURNS POINTER TO STRUCTURES CONTAINING ALL DATA 
**                        VALUES { DELIMITED }
**      RET123FV()       RETURNS POINTER TO STRUCTURES CONTAINING ALL DATA
**                        VALUES { FORMATED }
**      RET123PDM()      RETURNS PRIMARY DIMENSION SIZE
**      STR123TOK()      RETURNS A POINTER TO A STRING TOKEN
**
**     INTERNAL VARIABLES:
**      NAME            TYPE         DESCRIPTION
**      BUFFER[]        PTR          CHARACTER POINTER TO BUFFER HOLDING 
**                                    CHARACTER DATA
**      CUR_DD          PTR          DATA DESCRIPTIVE RECORD STRUCTURE
**      DELIM           CHAR         DELIMITER WITHIN FORMAT CONTROL
**      D_TYPE          CHAR         DATA TYPE IN THE FORMAT CONTROL
**      END_OF_FLD      INT          LOGICAL END OF FIELD FLAG
**      FIELD_LENGTH    LONG         LENGTH OF FIELD
**      I[]             PTR          CHARACTER STRING TOKEN
**      LEN             INT          LENGTH OF CHARACTER STRING
**      NEW_DV          PTR          POINTER TO DR DATA VALUE SUBFIELD 
**                                    STRUCTURE
**      PRIM_DMS        INT          THE SIZE OF THE PRIMARY DIMENSION
**      SAVE_IN_STR[]   PTR          SAVED POINTER TO INPUT STRING
**      ST_BUFF[]       PTR          CHARACTER STRING POINTER TO START OF
**                                    BUFFER
**      TMP_CPTR[]      PTR          TEMPORARY CHARACTER POINTER
**      TOK_LEN         LONG         LENGTH OF TOKEN RETURNED FROM STR123TOK()
**      VAL_STR[]       PTR          DATA VALUE STRING
**      WIDTH           INT          FIELD WIDTH SPECIFICATION WITHIN FORMAT
**                                    CONTROL
**
**    GLOBAL REFERENCES:
**      
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA 
**                                    FIELD RECORD
**      FD_CNTRL[10]    CHAR    I    FIELD CONTROLS
**      NAME[]          PTR    N/A   CHARACTER STRING POINTER TO NAME
**      NUM_DIM         INT     I    NUMBER OF DIMENSIONS IN ARRAY DESCRIPTOR
**      DIM_LPTR        PTR     I    HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT     I    NUMBER OF ELEMENTS IN PRIMARY DIMENSION
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
**      LEN             INT    N/A   DIMENSION LENGTH
**      NXT             PTR     I    POINTER TO NEXT DIMENSION LENGTH
** 
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     I    LENGTH OF DISCRIPTIVE AREA DATA RECORD
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT     O    NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSION LENGTHS
**                                    (NO LABELS)
**      VALUES          PTR     O    HEAD POINTER TO DATA VALUE SUBFIELD
**                                    RECORDS
**      NEXT            PTR    N/A   POINTER TO NEXT DATA RECORD 
**
**     DR DATA VALUE SUBFIELD STRUCTURE (DV)
**      NAME            TYPE   USE   DESCRIPTION
**      VALUE           CHAR    O    DATA VALUE
**      NXT_VSET        PTR     O    POINTER TO NEXT SET OF DATA VALUES
**      NXT_VAL         PTR     O    POINTER TO NEXT DATA VALUE SUBFIELD 
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
**      DR_HD           PTR    N/A   HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE 
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE ENTRY
**      CUR_DM          PTR     O    CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY
**      CUR_DR          PTR    I/O   CURRENT POINTER TO DATA RECORD STRUCTURE
**                                    ENTRY
**      CUR_DV          PTR    I/O   CURRENT POINTER TO DR DATA VALUE SUBFIELD
**                                    STRUCTURE ENTRY
**      ROW_DVH         PTR    N/A   CURRENT POINTER TO NEXT SET OF VALUES
**                                    IN DR DATA VALUE SUBFIELD STRUCTURE ENTRY
**      CUR_FC          PTR     O    CURRENT POINTER TO FORMAT CONTROLS
**                                    STRUCTURE ENTRY
**      CUR_LP          PTR    N/A   CURRENT POINTER TO LABELS POINTER
**                                    STRUCTURE ENTRY
**      CUR_SL          PTR    N/A   CURRENT POINTER TO DD-LABEL SUBFIELD
**                                    STRUCTURE ENTRY
**      CUR_FCR         PTR     O    CURRENT POINTER TO ROOT OF FORMAT
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
**                                     0 - UNCOMPRESSED
**                                     1 - COMPRESSED
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
**     GLB_STR[MAXSIZ]  CHAR   I/O   GLOBAL CHARACTER STRING USED FOR
**                                    PROCESSING
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION
**     DEL_STR[3]       CHAR         CHARACTER STRING CONTAINING THE FIELD AND
**                                    UNIT TERMINATORS
**     FCDSTYPE         INT          RELATIVE POSITION OF THE DATA STRUCTURE
**                                    TYPE WITHIN THE FIELD CONTROL
**     FT               CHAR         FIELD TERMINATOR (RS) 1/14
**     MAXSIZ           INT          MAXIMUM AMOUNT OF CONTIGUOUS MEMORY SPACE
**     NC               CHAR         NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE-ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     A. DEWITT                 06/05/90  INITIAL CODE
**     A. DEWITT                 06/17/90  MODIFY PDL AND CODE TO MOVE CALL TO 
**                                          BEG123REC OUTSIDE CONDITIONAL AND
**                                          TO ADD LOGIC TO INSERT TRUE RECORD
**                                          LENGTH WHEN A ZERO VALUE IS IN THE
**                                          LEADER
**     L. MCMILLION              10/16/90  REPLACED CALLS TO LIBRARY FUNCTION
**                                          STRTOK() WITH FUNCTION STR123TOK()
**                                          DUE TO NESTING PROBLEMS
**     A. DEWITT                 11/28/90  MODIFY LOGIC TO NOT READ PAST FT
**                                          IN CASE WHERE FORMATS ARE PRESENT
**                                          AND LABELS ARE NOT 
**     A. DEWITT                 02/28/91  ADD LOGIC TO RETAIN STARTING
**                                          POSITION OF DATA RECORD
**     L. MCMILLION  (TASK #40)  08/28/91  ADDED LOGIC TO CHECK FOR VALID
**                                          VALUES FOR SIZE OF FIELD LENGTH,
**                                          SIZE OF FIELD POSITION, AND SIZE
**                                          OF TAG
**     A. DEWITT     92DR005     04/09/92  IMPLEMENT BINARY DATA CHANGE
**     J. TAYLOR     92DR005     05/14/92  ADDED ADJACENT FIXED LENGTH BIT
**                                          SUBFIELD STRUCTURE
**     L. MCMILLION  92DR005     05/20/92  EXTRACTED CODE FROM LD123REC()
**                                          TO CREATE THIS FUNCTION
**     L. MCMILLION  TASK #40    11/24/92  UPDATED PROLOG PER QC
**     J. TAYLOR     93DR023     04/22/93  MODIFIED TO RETRIEVE FROM CHARACTER
**                                          RATHER THAN FILE
**     L. MCMILLION  93DR023     05/13/93  INSERTED SIZE_T CAST FOR MEMCPY
**                                          ARGUMENT 
**     J. TAYLOR     93DR034     06/03/93  MODIFIED CODE TO DETECT END OF
**                                          BINARY FIELDS
**     J. TAYLOR     93DR034     06/07/93  MODIFIED CALL TO RET123FV() TO
**                                          PASS FIELD LENGTH
**     L. MCMILLION  93DR033     07/23/93  ADDED CHECK FOR ARRAY DESCRIPTOR
**                                          DATA IN DD STRUCTURE ENTRY IN
**                                          ADDITION TO THE DR STRUCTURE ENTRY
**     J. TAYLOR     93DR037     07/28/93  REMOVED MEMSET CALLS
**     J. TAYLOR     93DR039     07/28/93  REMOVED MEMSET CALLS
**     J. TAYLOR     93DR040     08/05/93  ADD COMPUTATION OF FIELD LENGTH
**                                          BEFORE CALL TO RET123FV()
**     L. MCMILLON   93DR033     11/18/93  ADDED DM STRUCTURE TO PROLOG
**
**    PDL:
**
**     SAVE POINTER TO INPUT STRING
**     SET LOCAL CUR_DD TO CUR_DD IN FM STRUCTURE
**     IF LABELS POINTER AND FMT_RT POINTER OF CUR_DD ARE NULL 
**     THEN
**        IF NO FIELD CONTROLS { ELEMENTARY DATA FIELD }
**        THEN
**           ALLOCATE NEW_DV  { SET UP DUMMY HEADER }
**           SET VALUE OF NEW_DV TO NULL
**           SET NXT_VAL FIELD OF NEW_DV TO NULL
**           SET NXT_VSET FIELD OF NEW_DV TO NULL
**           SET VALUES OF CUR_DR TO NEW_DV
**           SET CUR_DV TO NEW_DV
**              
**           ALLOCATE NEW_DV
**           SET VALUE OF NEW_DV TO NULL
**           SET NXT_VAL FIELD OF NEW_DV TO NULL
**           SET NXT_VSET FIELD OF NEW_DV TO NULL
**           SET NXT_VSET CUR_DV TO NEW_DV
**           CALCULATE SIZE OF AND ALLOCATE BUFFER SPACE FOR VALUE
**           INITIALIZE STRING
**           COPY VALUE FROM INPUT STRING MEMORY
**           MOVE BEGINNING OF INPUT STRING PAST VALUE
**           REPLACE FIELD TERMINATOR WITH NULL CHAR
**        ELSE
**           IF FIELD CONTROLS INDICATE VECTOR OR ELEMENTARY FIELD 
**           THEN
**              ALLOCATE BUFFER OF SIZE FIELD LENGTH
**              SET START BUFFER POINTER TO BEGINNING OF BUFFER
**              COPY ALL DATA VALUES TO BUFFER FROM INPUT STRING MEMORY
**              TERMINATE BUFFER WITH NULL CHARACTER
**              MOVE BEGINNING OF INPUT STRING PAST MEMORY COPIED TO BUFFER
**              CALL STR123TOK() TO SEPARATE DELIMITED DATA VALUES
**
**              ALLOCATE NEW_DV  { SET UP DUMMY HEADER }
**              SET VALUE OF NEW_DV TO NULL
**              SET NXT_VAL FIELD OF NEW_DV TO NULL
**              SET NXT_VSET FIELD OF NEW_DV TO NULL
**              SET VALUES OF CUR_DR TO NEW_DV
**              SET CUR_DV TO NEW_DV
**
**              WHILE POINTER RETURNED FROM STR123TOK() NOT NULL
**                 ALLOCATE NEW_DV
**                 SET VALUE OF NEW_DV TO NULL
**                 SET NXT_VAL TO NULL
**                 SET NXT_VSET TO NULL
**                 SET LENGTH TO LENGTH OF STRING FROM STR123TOK()
**                 ALLOCATE SPACE FOR DATA VALUE
**                 INITIALIZE ALLOCATED SPACE TO EMPTY
**                 COPY STRING FROM STR123TOK() TO VALUE STRING
**                 SET NEW_DV VALUE TO VALUE STRING
**                 IF NOT AT HEADER RECORD
**                 THEN
**                    SET NEXT FIELD OF CUR_DV TO NEW_DV
**                 ELSE
**                    SET NEXT VALUE SET FIELD TO NEW_DV
**                 ENDIF
**                 SET CUR_DV TO NEW_DV
**                 CALL STR123TOK() TO RETURN NEXT DATA VALUE
**              ENDWHILE
**              FREE SPACE OF START BUFFER POINTER
**
**           ELSE  { ARRAY DATA }
**              IF NUMBER OF DIMENSIONS IN DDR (CUR_DD) EXISTS
**              THEN
**                 SET PRIMARY DIMENSION TO PRIMARY DIMENSION IN CUR_DD
**                 SET CUR_DM TO CUR_DM IN CUR_DD
**              ELSE
**                 CALL GET123DIM() TO INPUT AND STORE DIMENSION INFORMATION
**                  FROM DR
**              ENDIF
**              IF FIELD LENGTH EXCEEDS MAXIMUM AMOUNT OF CONTIGUOUS SPACE
**              THEN
**                 RETURN FAILURE
**              ENDIF
**              CALL G123DSTR() TO LOAD BUFFER WITH ALL DATA VALUES
**              CALL RET123DV() TO EXTRACT DATA VALUES
**           ENDIF
**        ENDIF  
**
**     ELSEIF FMT_RT IS NOT NULL AND LABELS IS NULL OF CUR_DD
**     THEN
**
**        IF FIELD CONTROLS INDICATE VECTOR OR ELEMENTARY DATA
**        THEN
**           ALLOCATE NEW_DV  { SET UP DUMMY HEADER }
**           SET VALUE OF NEW_DV TO NULL
**           SET NXT_VAL FIELD OF NEW_DV TO NULL
**           SET NXT_VSET FIELD OF NEW_DV TO NULL
**           SET VALUES OF CUR_DR TO NEW_DV
**           SET CUR_DV TO NEW_DV
**              
**           SET CUR_FCR TO FMT_RT OF CUR_DD  { INITIALIZE FOR GET123FMT() }
**           SET CUR_FC TO FMT_FT OF CUR_DD   { INITIALIZE FOR GET123FMT() }
**
**           INITIALIZE END OF FIELD FLAG TO FALSE
**           WHILE HAVE NOT READ LAST VALUE OF FIELD DO
**              CALL GET123FMT() TO RETRIEVE FORMAT OF DATA VALUE
**              CLEAR STRING
**              CALL GET123DVAL() TO READ IN DATA VALUE STRING
**              ALLOCATE NEW_DV
**              INITIALIZE POINTERS OF NEW_DV
**              IF DATA TYPE NOT BINARY
**              THEN
**                 CALL C LIBRARY FUNCTION STRCHR() TO LOCATE FT IN STRING
**                 IF FT PRESENT IN STRING
**                 THEN
**                    SET TEMPORARY CHARACTER POINTER TO FT POSITION
**                 ELSE
**                    SET TEMPORARY CHARACTER POINTER TO NULL
**                 ENDIF
**                 IF TEMPORARY CHARACTER POINTER NOT NULL { FT IN STRING }
**                 THEN
**                    SET END OF FIELD FLAG TO END-OF-FIELD
**                 ELSE
**                    SET END OF FIELD FLAG TO NOT END-OF-FIELD
**                 ENDIF
**                 IF TMP_CPTR NOT NULL
**                 THEN
**                    REPLACE FT WITH A NULL CHARACTER
**                 ENDIF
**                 SET LENGTH TO LENGTH OF DATA VALUE
**                 ALLOCATE SPACE FOR VALUE STRING
**                 COPY STRING FROM BUFFER TO EMPTY STRING JUST ALLOCATED
**                 SET NEW_DV VALUE TO VALUE STRING
**              ELSE BINARY DATA TYPE
**                 SET WIDTH TO LENGTH PLUS ONE
**                 ALLOCATE SPACE FOR DATA VALUES
**                 COPY BUFFER TO ALLOCATED STRING
**                 TERMINATE ALLOCATED STRING WITH NULL CHARACTER
**                 SET NEW_DV VALUE TO ALLOCATED VALUE SPACE
**              ENDIF
**              IF CUR_DV IS NOT EQUAL TO VALUES FIELD OF CUR_DR {DUMMY}
**              THEN
**                 SET NXT_VAL FIELD OF CUR_DV TO NEW_DV
**              ELSE
**                 SET NXT_VSET FIELD OF CUR_DV TO NEW_DV
**              ENDIF
**              SET CUR_DV TO NEW_DV  
**              IF NOT END OF FIELD
**              THEN
**                 IF END OF INPUT STRING 
**                 THEN
**                    SET END OF FIELD FLAG TO TRUE
**                 ELSE
**                    IF DATA TYPE IS BINARY AND ENTIRE FIELD LENGTH HAS
**                     BEEN RETRIEVED
**                    THEN
**                       SET END OF FIELD FLAG TO TRUE 
**                       MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR
**                    ELSE IF DATA TYPE IS NOT BINARY AND CHARACTER IS
**                     FIELD TERMINATOR AND DATA VALUE IS NOT DELIMITED
**                    THEN
**                       SET END OF FIELD FLAG TO TRUE
**                       MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR
**                    ENDIF
**                 ENDIF
**              ENDIF
**           ENDWHILE
**        ELSE { ARRAY DATA }
**           IF NUMBER OF DIMENSIONS IN DDR (CUR_DD) EXISTS
**           THEN
**              SET PRIMARY DIMENSION TO PRIMARY DIMENSION IN CUR_DD
**              SET CUR_DM TO CUR_DM IN CUR_DD
**           ELSE
**              CALL GET123DIM() TO INPUT AND STORE DIMENSION INFORMATION
**               FROM DR
**           ENDIF
**           SET CUR_FCR TO FMT_RT FIELD OF CUR_DD
**           SET CUR_FC TO FMT_RT FIELD OF CUR_DD
**           COMPUTE PROPER FIELD LENGTH
**           CALL RET123FV() TO RETRIEVE FORMATTED DATA VALUES RETURN A 
**            POINTER TO A STRUCTURE STORING THE VALUES
**        ENDIF
**
**     ELSEIF LABELS IS NOT NULL AND FMT_RT IS NULL OF CUR_DD
**     THEN
**        CALL RET123PDM() TO RETURN PRIM_DMS
**        CALL G123DSTR() TO LOAD BUFFER WITH ALL DATA VALUES
**        CALL RET123DV() TO EXTRACT DATA VALUES
**
**     ELSE { LABELS AND FMT_RT POINTER FIELD OF CUR_DD NOT NULL }
**        CALL RET123PDM() TO RETURN PRIM_DMS
**        SET CUR_FCR TO FMT_RT FIELD OF CUR_DD
**        COMPUTE PROPER FIELD LENGTH
**        SET CUR_FC TO FMT_RT FIELD OF CUR_DD
**        CALL RET123FV() TO RETRIEVE FORMATTED DATA VALUES AND RETURN A 
**         POINTER TO A STRUCTURE STORING THE VALUES
**
**     ENDIF
**
**     RETURN SUCCESS
**
****************************************************************************
**     CODE SECTION
**
****************************************************************************/
#include "stc123.h"

int load123fld(char **in_str,char *in_str_end,int comprssd)

{
   /* DECLARE LOCAL VARIABLES */
   char      *buffer   ,
             *i        ,
             *st_buff  ,
             *tmp_cptr ,
             *val_str  ;
   char      d_type    ,
             delim     ;
   int       end_of_fld;
   long      prim_dms  ,
             tok_len   ,
             width     ,
             field_length;
   size_t    len       ;
   struct dd *cur_dd   ;
   struct dv *new_dv   ;
   char      *save_in_str;
  
   /* SAVE POINTER TO INPUT STRING */ 
   save_in_str = *in_str;
      
   /* SET LOCAL CUR_DD TO CUR_DD IN FM STRUCTURE */
   cur_dd = cur_fm->cur_dd;
       
   /* IF LABELS POINTER AND FMT_RT POINTER OF CUR_DD ARE NULL */
   if ((cur_dd->labels == NULL) && (cur_dd->fmt_rt == NULL)) {
          
      /* IF NO FIELD CONTROLS { ELEMENTARY DATA FIELD } */
      if (!_tcslen(cur_dd->fd_cntrl)) {

         /* ALLOCATE NEW_DV  { SET UP DUMMY HEADER } */
         if ((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL) return(0);

         /* INITIALIZE POINTERS */
         new_dv->value = NULL;
         new_dv->nxt_val = NULL;
         new_dv->nxt_vset = NULL;

         /* SET VALUES OF CUR_DR TO NEW_DV */
         cur_fm->cur_dr->values = new_dv;
            
         /* SET CUR_DV TO NEW_DV */
         cur_fm->cur_dv = new_dv;

         /* ALLOCATE NEW_DV */
         if((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL)
          return(0);

         /* INITIALIZE POINTERS */
         new_dv->value = NULL;
         new_dv->nxt_val = NULL;
         new_dv->nxt_vset = NULL;

         /* SET NXT_VSET CUR_DV TO NEW_DV */
         cur_fm->cur_dv->nxt_vset = new_dv;
               
         /* CALCULATE SIZE OF AND ALLOCATE BUFFER SPACE FOR VALUE */
         len = (size_t) (cur_fm->cur_dr->fd_len + 1);
         if ((new_dv->value = (char *) malloc(len * sizeof(char))) == NULL)
          return(0);
                
         /* INITIALIZE STRING */
         *new_dv->value = NC;
            
         /* COPY VALUE FROM INPUT STRING MEMORY */
         memcpy(new_dv->value,*in_str,(size_t)cur_fm->cur_dr->fd_len);

         /* MOVE BEGINNING OF INPUT STRING PAST VALUE */
         *in_str = *in_str + cur_fm->cur_dr->fd_len;

         /* REPLACE FIELD TERMINATOR WITH NULL CHAR */
         new_dv->value[cur_fm->cur_dr->fd_len-1] = NC;

      }
      else {
               
         /* IF FIELD CONTROLS INDICATE VECTOR OR ELEMENTARY FIELD */
         if ((cur_dd->fd_cntrl[FCDSTYPE] == '1') || (cur_dd->fd_cntrl[FCDSTYPE] == '0')) {
                  
            /*  ALLOCATE BUFFER OF SIZE FIELD LENGTH */
            len = (size_t) (cur_fm->cur_dr->fd_len + 1);
            if ((buffer = (char *) malloc(len * sizeof(char))) == NULL)
             return(0);
             
            /* SET START BUFFER POINTER TO BEGINNING OF BUFFER */
            st_buff = buffer;

            /* COPY ALL DATA VALUES TO BUFFER FROM INPUT STRING MEMORY */
            memcpy(buffer,*in_str,(size_t)cur_fm->cur_dr->fd_len);

            /* TERMINATE BUFFER WITH NULL CHARACTER */
            buffer[cur_fm->cur_dr->fd_len] = NC;

            /* MOVE BEGINNING OF INPUT STRING PAST MEMORY COPIED TO BUFFER */
            *in_str = *in_str + cur_fm->cur_dr->fd_len;

            /* CALL STR123TOK() TO SEPARATE DELIMITED DATA VALUES */
            i = str123tok(&buffer,DEL_STR,&tok_len);
               
            /* ALLOCATE NEW_DV  { SET UP DUMMY HEADER } */
            if ((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL) return(0);               

            /* INITIALIZE POINTERS */
            new_dv->value = NULL;
            new_dv->nxt_val = NULL;
            new_dv->nxt_vset = NULL;
               
            /* SET CURRENT DATA RECORD VALUES TO NEW_DV */
            cur_fm->cur_dr->values = new_dv;
               
            /* SET CUR_DV TO NEW_DV */
            cur_fm->cur_dv = new_dv;
               
            /* WHILE POINTER RETURNED FROM STR123TOK() NOT NULL */
            while ( i != NULL) {
                  
               /* ALLOCATE NEW_DV */
               if ((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL) return(0);                                      

               /* INITIALIZE POINTERS */
               new_dv->value = NULL;
               new_dv->nxt_val = NULL;
               new_dv->nxt_vset = NULL;
                                      
               /* SET LEN TO LENGTH OF DATA VALUE RETURNED */
               len = (size_t) _tcslen(i) + 1;
                      
               /* ALLOCATE SPACE FOR STRING */
               if ((val_str = (char *) malloc(len * sizeof(char))) == NULL)
                return(0); 

               /* SET STRING TO EMPTY */
               *val_str = NC;
                  
               /* COPY STRING RETURNED FROM STR123TOK() TO NEW SPACE */
               strcpy(val_str,i);
                      
               /* SET VALUE TO VAL_STR */               
               new_dv->value = val_str;
                  
               /* IF NOT AT HEADER RECORD */
               if (cur_fm->cur_dr->values != cur_fm->cur_dv) {
                  
                  /* SET NEXT FIELD OF CUR_DV TO NEW_DV */
                  cur_fm->cur_dv->nxt_val = new_dv;
               }
               else {
                  /* SET NEXT VALUE SET FIELD TO NEW_DV */
                  cur_fm->cur_dv->nxt_vset = new_dv;
               };
                    
               /* SET CUR_DV TO NEW_DV */
               cur_fm->cur_dv = new_dv;
                   
               /* CALL STR123TOK() TO RETURN NEXT DATA VALUE */
               i = str123tok(&buffer,DEL_STR,&tok_len);
                   
            };
            /* FREE SPACE OF START OF BUFFER POINTER */
            free(st_buff);
                
         }   
         /* ELSE  { ARRAY DATA } */
         else {

            /* IF NUMBER OF DIMENSIONS IN DDR (CUR_DD) EXISTS */
            if (cur_dd->num_dim) {

               /* SET PRIMARY DIMENSION AND CUR_DM TO THOSE IN CUR_DD */
               prim_dms = cur_dd->prim_dms;
               cur_fm->cur_dm = cur_dd->dim_lptr->nxt;
            }
            else {
           
               /* CALL GET123DIM() TO INPUT AND STORE DIMENSION INFORMATION
                   FROM DR
               */
               if (!get123dim(in_str,&cur_fm->cur_dr->num_dim,&prim_dms))
                return(0);
            }
                                    
            /* IF FIELD LENGTH EXCEEDS MAXIMUM AMOUNT OF CONTIGUOUS SPACE,
                RETURN FAILURE
            */
            if (cur_fm->cur_dr->fd_len > MAXSIZ) return(0);
                   
            /* CALL G123DSTR() TO LOAD BUFFER WITH ALL DATA VALUES */
            if (!g123dstr(in_str,glb_str,FT)) return(0);

            /* CALL RET123DV() TO EXTRACT DATA VALUES */
            if (!ret123dv(glb_str,prim_dms)) return(0);
                   
         };
      };
   }     
   /* ELSEIF FMT_RT IS NOT NULL AND LABELS IS NULL OF CUR_DD */
   else if ((cur_dd->fmt_rt != NULL) && (cur_dd->labels == NULL)) {
        
      /* IF FIELD CONTROLS INDICATE VECTOR OR ELEMENTARY DATA */
      if ((cur_dd->fd_cntrl[FCDSTYPE] == '1') || (cur_dd->fd_cntrl[FCDSTYPE] == '0')) {
      
         /* ALLOCATE NEW_DV  { SET UP DUMMY HEADER } */
         if ((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL)
          return(0);               

         /* INITIALIZE POINTERS */
         new_dv->value = NULL;
         new_dv->nxt_val = NULL;
         new_dv->nxt_vset = NULL;
               
         /* SET CURRENT DATA RECORD VALUES TO NEW_DV */
         cur_fm->cur_dr->values = new_dv;
               
         /* SET CUR_DV TO NEW_DV */
         cur_fm->cur_dv = new_dv;
              
         /* SET CUR_FCR TO FMT_RT OF CUR_DD  { INITIALIZE FOR GET123FMT } */
         cur_fm->cur_fcr = cur_dd->fmt_rt;

         /* SET CUR_FC TO FMT_RT FIELD OF CUR_DD */
         cur_fm->cur_fc = cur_dd->fmt_rt;
            
         /* INITIALIZE END OF FIELD FLAG */
         end_of_fld = 0;
            
         /* WHILE HAVE NOT READ LAST VALUE OF FIELD DO */
         while ( !end_of_fld) {
                   
            /* CALL GET123FMT() TO RETRIEVE FORMAT OF DATA VALUE STRING */
            if (!get123fmt(&d_type,&width,&delim)) return(0);
                      
            /* CLEAR STRING */
            glb_str[0] = NC;

            /* CALL GET123DVAL() TO READ IN DATA VALUE INTO VAL_STR */
            if (!get123dval(in_str,in_str_end,d_type,&width,delim,comprssd,glb_str))
             return(0);
             
            /* ALLOCATE NEW_DV  */
            if ((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL) return(0);               

            /* INITIALIZE POINTERS */
            new_dv->value = NULL;
            new_dv->nxt_val = NULL;
            new_dv->nxt_vset = NULL;
   
            /* IF DATA TYPE NOT 'B' */
            if (d_type != 'B') {
                
               /* CALL STRCHR TO RETURN POINTER TO FT IN STRING IF PRESENT
                   OTHERWISE ASSIGNS NULL VALUE TO TMP_CPTR.
               */
               tmp_cptr = strchr(glb_str,FT);
                  
               /* SET END OF FIELD FLAG IF FT IS IN STRING */
               end_of_fld = (tmp_cptr != NULL);
               
               /* IF TMP_CPTR NOT NULL REPLACE FT WITH NULL CHARACTER */
               if (tmp_cptr != NULL) *tmp_cptr = NC;
               
               /* SET LEN TO LENGTH OF DATA VALUE */
               len = (size_t) _tcslen(glb_str) + 1;
                         
               /* RESERVE SPACE FOR STRING */
               if ((val_str = (char *) malloc(len * sizeof(char))) == NULL) return(0); 

               /* SET VAL_STR TO EMPTY STRING */
               *val_str = NC;
               
               /* COPY STRING FROM BUFFER TO NEW SPACE */
               strcpy(val_str,glb_str);
                         
               /* SET VALUE TO VAL_STR */               
               new_dv->value = val_str;
                         
            }    

            /* ELSE DATA TYPE IS BINARY */
            else {

               /* SET LENGTH TO WIDTH PLUS ONE */
               len = (size_t) width + 1;

               /* RESERVE SPACE FOR STRING */
               if ((val_str = (char *) malloc(len * sizeof(char))) == NULL)
                return(0); 

               /* COPY BUFFER TO VALUE STRING */
               memcpy(val_str,glb_str,--len);

               /* TERMINATE VAL_STR WITH NULL CHARACTER */
               val_str[len] = NC;

               /* SET VALUE TO VAL_STR */               
               new_dv->value = val_str;
            };
 
            /* IF CUR_DV NOT EQUAL TO VALUES FIELD OF CUR_DR {DUMMY} */
            if (cur_fm->cur_dv != cur_fm->cur_dr->values) {

               /* SET NXT_VAL FIELD OF CUR_DV TO NEW_DV */
               cur_fm->cur_dv->nxt_val = new_dv;

            }
            else {
                  
               /* SET NXT_VSET FIELD OF CUR_DV TO NEW_DV */
               cur_fm->cur_dv->nxt_vset = new_dv;
            };
                      
            /* SET CUR_DV TO NEW_DV */
            cur_fm->cur_dv = new_dv;

            /* IF NOT END OF FIELD */
            if (!end_of_fld) {
            
               /* IF END OF INPUT STRING */
               if (*in_str >= in_str_end) {
                  
                  /* SET END OF FIELD FLAG TO TRUE */
                  end_of_fld = 1;
               }
               else {
                   
                  /* IF DATA TYPE IS BINARY AND ENTIRE FIELD LENGTH HAS
                      BEEN RETRIEVED
                  */
                  if (d_type == 'B' && (*in_str - save_in_str + 1) == cur_fm->cur_dr->fd_len) {

                     /* SET END OF FIELD FLAG TO TRUE */
                     end_of_fld = 1;

                     /* MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR */
                     *in_str = *in_str + 1;

                  }
                  /* ELSE IF DATA TYPE IS NOT BINARY AND CHARACTER IS
                      FIELD TERMINATOR AND DATA VALUE IS NOT DELIMITED
                  */
                  else if (d_type != 'B' && *in_str[0] == FT && width != 0) {
                     
                     /* SET END OF FIELD FLAG TO TRUE */
                     end_of_fld = 1;

                     /* MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR */
                     *in_str = *in_str + 1;

                  }
               }
            }
         }
      }  
      /* ELSE { ARRAY DATA } */
      else {
             
         /* IF NUMBER OF DIMENSIONS IN DDR (CUR_DD) EXISTS */
         if (cur_dd->num_dim) {

            /* SET PRIMARY DIMENSION AND CUR_DM TO THOSE IN CUR_DD */
            prim_dms = cur_dd->prim_dms;
            cur_fm->cur_dm = cur_dd->dim_lptr->nxt;
         }
         else {
           
            /* CALL GET123DIM() TO INPUT AND STORE DIMENSION INFORMATION
                FROM DR
            */
            if (!get123dim(in_str,&cur_fm->cur_dr->num_dim,&prim_dms))
             return(0);
         }
                                    
         /* SET CUR_FCR TO FMT_RT FIELD OF CUR_DD */
         cur_fm->cur_fcr = cur_dd->fmt_rt;

         /* SET CUR_FC TO FMT_RT FIELD OF CUR_DD */
         cur_fm->cur_fc = cur_dd->fmt_rt;
                   
         /* COMPUTE PROPER FIELD LENGTH */
         if (comprssd) field_length = cur_fm->cur_dr->fd_len-(*in_str-save_in_str);
         else          field_length = in_str_end - *in_str;
                   
         /* CALL RET123FV() TO RETRIEVE FORMATTED DATA VALUES RETURN A 
             POINTER TO A STRUCTURE STORING THE VALUES 
         */
         if (!ret123fv(in_str,in_str_end,prim_dms,field_length,comprssd)) return(0);

      }
   }   
   /* ELSEIF LABELS IS NOT NULL AND FMT_RT IS NULL OF CUR_DD */
   else if ((cur_dd->labels != NULL) && (cur_dd->fmt_rt == NULL)) {
        
      /* CALL RET123PDM() TO RETURN PRIM_DMS */
      if (!ret123pdm(&prim_dms)) return(0);
            
      /* CALL G123DSTR() TO LOAD BUFFER WITH ALL DATA VALUES */
      if (!g123dstr(in_str,glb_str,FT)) return(0);

      /* CALL RET123DV() TO EXTRACT DATA VALUES */
      if (!ret123dv(glb_str,prim_dms)) return(0);
   }   
   /* ELSE { LABELS AND FMT_RT POINTER FIELD OF CUR_DD NOT NULL } */
   else {
         
      /* CALL RET123PDM() TO RETURN PRIM_DMS */
      if (!ret123pdm(&prim_dms)) return(0);
          
      /* SET CUR_FCR TO FMT_RT FIELD OF CUR_DD */
      cur_fm->cur_fcr = cur_dd->fmt_rt;
            
      /* SET CUR_FC TO FMT_RT FIELD OF CUR_DD */
      cur_fm->cur_fc = cur_dd->fmt_rt;
            
      /* COMPUTE PROPER FIELD LENGTH */
      if (comprssd) field_length = cur_fm->cur_dr->fd_len;
      else          field_length = in_str_end - *in_str;
            
      /* CALL RET123FV() TO RETRIEVE FORMATTED DATA VALUES AND RETURN A  
          POINTER TO A STRUCTURE STORING THE VALUES
      */
      if (!ret123fv(in_str,in_str_end,prim_dms,field_length,comprssd)) return(0);
   }

   /* RETURN SUCCESS */  
   return(1);
}
