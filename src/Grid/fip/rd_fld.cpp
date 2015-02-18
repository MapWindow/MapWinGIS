# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RD123FLD
**
**    PURPOSE: TO RETRIEVE DATA RECORD FIELD
**
**    INVOCATION METHOD: RD123FLD(FP,TAG,LEADID,RD_STR,STR_LEN,STATUS)
**
**    ARGUMENT LIST:
**     NAME        TYPE      USE      DESCRIPTION
**     FP          PTR        I       FILE POINTER
**     TAG[]       PTR        O       FIELD TAG
**     LEADID[]    PTR        O       LEADER IDENTIFIER
**     RD_STR[]    PTR        O       READ FIELD
**     STR_LEN     LONG       O       BYTE LENGTH OF RD_STR
**     STATUS      INT        O       STATUS
**                                     0 = FAILURE
**                                     1 = OKAY
**                                     2 = START OF RECORD
**                                     3 = END OF RECORD
**                                     4 = END OF FILE (DR)
**                                     5 = END OF FIELD
**                                     6 = START OF FIELD
**     RD123FLD()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     CONV123CAT()     CONVERTS AND CONCATENATES ONE STRING TO ANOTHER
**     GET123FMT()      RETRIEVES A SINGLE FORMAT FROM THE FORMAT STRUCTURE
**     GET123LEVEL()    RETRIEVES APPROPRIATE FM ENTRY FOR THIS FILE POINTER
**     I123TOA()        CONVERTS AN INTEGER TO A CHARACTER STRING
**     LD123REC()       LOADS DATA RECORD INFORMATION INTO DATA STRUCTURES
**                       FOR REFERENCE
**     RET123MATCH()    FINDS ASSOCIATED DATA DESCRIPTIVE MATCH TO DATA
**                       RECORD
**     SET123STAT()     CHECKS IF EOF AND SETS STATUS FLAG
**    
**    INTERNAL VARIABLES:
**     NAME       TYPE                DESCRIPTION
**     ANUM[10]   CHAR                CHARACTER REPRESENTATION OF AN INTEGER
**     B_CNT      INT                 NO. OF DIGITS IN BIT SUBFIELD LENGTH
**     B_LEN      INT                 LENGTH OF BIT SUBFIELD IN BITS
**     B_SIZ      INT                 LENGTH OF SUBFIELD DATA VALUE IN BYTES
**     BIT_POS    INT                 BIT POSITION IN READ STRING
**     BYTE_POS   INT                 BYTE POSITION IN READ STRING
**     BYTES      INT                 NO. OF BYTES REQUIRED TO BE READ TO
**                                     RETRIEVE BIT SUBFIELD
**     DELIM[2]   CHAR                DELIMITER RETURNED FROM GET123FMT()
**     DTYP[2]    CHAR                DATA TYPE RETURNED FROM GET123FMT()
**     I          INT                 CHARACTER STRING INDEX
**     TMP_NOSIGN UNSIGNED CHAR       TEMPORARY VARIABLE USED FOR SHIFT 
**                                     OPERATIONS
**     WIDTH      INT                 WIDTH RETURNED FROM GET123FMT()
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
**     DATA RECORD (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     I    LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT     I    POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT     I    NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR     I    POINTER TO DIMENSION LENGTHS (NO LABELS)
**      VALUES          PTR     I    POINTER TO DATA VALUE SUBFIELD RECORDS
**      NEXT            PTR     I    POINTER TO NEXT DATA RECORD
**
**     DR DATA VALUE SUBFIELD STRUCTURE (DV)
**      NAME            TYPE   USE   DESCRIPTION
**      VALUE           CHAR    I    DATA VALUE
**      NXT_VAL         PTR     I    POINTER TO NEXT DATA VALUE SUBFIELD 
**                                    RECORD
**      NXT_VSET        PTR     I    POINTER TO NEXT SET OF DATA VALUES
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
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR     I    HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE ENTRY
**      CUR_DM          PTR    I/O   CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY
**      CUR_DR          PTR     I    CURRENT POINTER TO DATA RECORD STRUCTURE
**                                    ENTRY
**      CUR_DV          PTR    I/O   CURRENT POINTER TO DR DATA VALUE SUBFIELD
**                                    STRUCTURE ENTRY
**      ROW_DVH         PTR    I/O   CURRENT POINTER TO NEXT SET OF VALUES
**                                    IN DR DATA VALUE SUBFIELD STRUCTURE ENTRY
**      CUR_FC          PTR    I/O   CURRENT POINTER TO FORMAT CONTROLS
**                                    STRUCTURE ENTRY
**      CUR_LP          PTR    N/A   CURRENT POINTER TO LABELS POINTER
**                                    STRUCTURE ENTRY
**      CUR_SL          PTR    N/A   CURRENT POINTER TO DD-LABEL SUBFIELD
**                                    STRUCTURE ENTRY
**      CUR_FCR         PTR    I/O   CURRENT POINTER TO ROOT OF FORMAT
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
**      COMPRESS        INT     I    FLAG TO SPECIFY COMPRESSED OR
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
**      SF_STATE_DR     INT     O    SUBFIELD STATE (DR)
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
**      REC_LEN         INT    N/A   DATA RECORD LENGTH     
**      RESV_SP         CHAR   N/A   RESERVED SPACE CHARACTER
**      LEAD_ID         CHAR    O    LEADER IDENTIFIER
**      S_RESV[6]       CHAR   N/A   RESERVED SPACE
**      DA_BASE         INT    N/A   BASE ADDRESS OF DATA AREA  
**      R_SPACE[4]      CHAR   N/A   RESERVED SPACE CHARACTERS
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH  
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION 
**      SP_RSRV         INT    N/A   RESERVED DIGIT
**      S_TAG           INT    N/A   SIZE OF FIELD TAG 
**
**    GLOBAL VARIABLE:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**     
**    GLOBAL CONSTANTS:
**     NAME       TYPE      DESCRIPTION
**     BYTE_S     INT       NUMBER OF BITS IN A BYTE
**     FT_STR     CHAR      CHAR STRING WITH FIELD TERMINATOR (RS) 1/14
**                           FOLLOWED BY NULL CHARACTER
**     CONV       INT       COMPILATION DIRECTIVE FLAG USED TO COMPILE
**                           CODE FOR ASCII/EBCDIC CONVERSION OF DATA
**     NC         CHAR      NULL CHARACTER
**     UT_STR     CHAR      STRING CONTAINING UNIT TERMINATOR FOLLOWED BY THE
**                           NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     P. HODGES                 06/07/90  INITIAL CODE
**     A. DEWITT                 10/22/90  ADD LOGIC TO SET STATUS
**                                         FLAG TO EOF AND START OF
**                                         RECORD WHEN APPROPRIATE
**     A. DEWITT     92DR005     04/10/90  IMPLEMENT BINARY DATA
**     J. TAYLOR     92DR005     05/20/92  ADDED CODE TO ALLOW READ STRING
**                                          TO BE RETURN WITH ADJACENT FIXED
**                                          LENGTH BIT FIELDS COMPRESSED
**     J. TAYLOR     92DR005     05/20/92  UPDATED FIXED-LENGTH STR_LEN
**                                          FROM BYTE POSITION TO LENGTH IN
**                                          BYTES
**     J. TAYLOR     92DR005     06/02/92  MODIFIED TO NOT APPEND UNIT 
**                                          TERMINATOR TO DATA AFTER EACH
**                                          ROW OF ARRAY
**     J. TAYLOR     92DR012     10/29/92  MODIFIED TO SKIP DIMENSION DUMMY
**                                          HEADER WAS READING ARRAY DIMENSIONS
**     J. TAYLOR     TASK 55     01/13/93  ADDED CONDITIONAL COMPILATION   
**                                          CALLS TO CONVERT SUBFIELDS BETWEEN
**                                          ASCII AND EBCDIC
**     L. MCMILLION  TASK 55     03/12/93  UPDATED PROLOG PER QC
**     J. TAYLOR     93DR037     07/28/93  REMOVED MEMSET CALLS 
**     J. TAYLOR     93DR039     07/28/93  REMOVED MEMSET CALLS 
**
**    PDL:
**
**     INITIALIZE STATUS TO FAILURE
**     CALL GET123LEVEL() TO GET APPROPRIATE FM ENTRY FOR THIS FILE POINTER
**     SET CUR_DR TO NEXT FIELD
**     IF CUR_DR IS NULL
**     THEN
**        CALL LD123REC() TO LOAD DATA RECORD INFORMATION INTO DATA
**         STRUCTURES FOR REFERENCE
**        IF LD123REC() FAILED
**        THEN
**           IF END OF FILE ENCOUNTERED  { LD123REC FAILURE }
**           THEN
**              SET STATUS TO END OF FILE
**           ENDIF
**           RETURN FAILURE
**        ENDIF
**        SET STATUS TO START OF RECORD { LD123REC SUCESSFUL }
**        SET CUR_DR TO NEXT POINTER OF DR_HD
**     ENDIF
**     CALL RET123MATCH() TO RETURN DATA DESCRIPTION FOR CURRENT ENTRY
**     INITIALIZE RD_STR TO NULL
**     INITIALIZE B_SIZ, BYTE_POS, AND BIT_POS TO ZERO
**     IF NUM_DIM GREATER THAN ZERO
**     THEN
**        CONCATENATE NUM_DIM TO RD_STR
**        UPDATE BYTE POSITION INDEX
**        CONCATENATE UT TO RD_STR
**        UPDATE BYTE POSITION INDEX
**        SET CUR_DM TO DIM_LPTR
**        SKIP DUMMY DM
**        WHILE CUR_DM NOT NULL DO
**           CONCATENATE LEN TO RD_STR
**           UPDATE BYTE POSITION INDEX
**           CONCATENATE UT TO RD_STR
**           UPDATE BYTE POSITION INDEX
**           SET CUR_DM TO NXT
**        END WHILE
**     ENDIF
**
**     IF FMT_RT OF CUR_DD IS NULL
**     THEN
**        SET ROW_DVH TO NXT_VSET FIELD OF VALUES FIELD OF CUR_DR 
**        WHILE ROW_DVH NOT NULL DO
**           SET CUR_DV TO ROW_DVH
**           WHILE CUR_DV NOT NULL DO
**              IF CUR_DV VALUE NOT NULL
**              THEN
**                 DETERMINE LENGTH OF VALUE IN BYTES
**                 CONCATENATE VALUE TO RD_STR
**                 UPDATE BYTE POSITION INDEX
**              ENDIF 
**              SET CUR_DV TO NXT_VAL
**              IF CUR_DV NOT NULL
**              THEN
**                 CONCATENATE UT TO RD_STR
**                 UPDATE BYTE POSITION INDEX
**              ENDIF
**           END WHILE
**           SET ROW_DVH TO NXT_VSET
**           IF ROW_DVH NOT NULL
**           THEN
**              CONCATENATE UT TO RD_STR
**              UPDATE BYTE POSITION INDEX
**           ENDIF
**        END WHILE
**     ELSE { FORMATS }
**        SET CUR_FC TO FMT_RT OF CUR_DD  {INIT GET123FMT}
**        SET CUR_FCR TO CUR_FC
**        IF VALUES OF CUR_DR NOT NULL
**        THEN
**           SET ROW_DVH TO NXT_VSET FIELD OF VALUES FIELD OF CUR_DR 
**           WHILE ROW_DVH NOT NULL DO
**              SET CUR_DV TO ROW_DVH
**              WHILE CUR_DV NOT NULL DO
**                 INITIALIZE DELIMITER, DATA TYPE, AND WIDTH FOR FORMAT
**                 CALL GET123FMT() TO RETRIEVE FORMAT FOR THIS DATA VALUE
**                 IF DATA TYPE IS NOT BINARY
**                 THEN
**                    IF NOT POSITIONED AT BYTE BOUNDARY
**                    THEN
**                       ADVANCE POSITION TO NEXT BYTE BOUNDARY
**                    ENDIF
**                    IF CUR_DV VALUE NOT NULL
**                    THEN
**                       DETERMINE LENGTH OF VALUE IN BYTES
**                       CONCATENATE VALUE TO READ STRING
**                       UPDATE BYTE POSITION INDEX
**                    ENDIF
**                 ELSE DATA IS BINARY
**                 THEN
**                    IF FIELD IS VARIABLE LENGTH BIT FIELD
**                    THEN
**                       IF NOT POSITIONED AT BYTE BOUNDARY
**                       THEN
**                          ADVANCE POSITION TO NEXT BYTE BOUNDARY
**                       ENDIF
**                       EXTRACT FIELD LENGTH COUNT
**                       CONCATENATE FIELD LENGTH COUNT TO RD_STR
**                       UPDATE BYTE POSITION INDEX
**                       EXTRACT FIELD LENGTH
**                       CONCATENATE FIELD LENGTH TO RD_STR
**                       UPDATE BYTE POSITION INDEX
**                       COMPUTE BYTE SIZE OF BIT DATA FIELD
**                       COMPUTE ACTUAL LENGTH OF BIT DATA FIELD
**                       SET READ STRING TO STORED VALUE
**                       TERMINATE READ STRING WITH NULL CHARACTER
**                       UPDATE BYTE POSITION INDEX
**                    ELSE FIELD IS FIXED LENGTH BIT FIELD
**                    THEN
**                       IF ADJACENT SUBFIELDS TO BE COMPRESSED
**                       THEN
**                          COMPUTE BYTE SIZE OF BIT DATA FIELD
**                          IF NOT ON BYTE BOUNDARY
**                          THEN
**                             SET READ STRING TO STORED VALUE STARTING
**                              AT BYTE POS + 1
**                             SHIFT VALUE LEFT SO THAT IT IS ADJACENT TO
**                              PREVIOUS BIT SUBFIELD
**                             COMPUTE NEW BYTE AND BIT POSITION
**                             ADD TRUNCATING NULLS TO RD_STR
**                          ELSE ON BYTE BOUNDARY 
**                          THEN
**                             SET READ STRING TO STORED VALUE
**                             TERMINATE READ STRING WITH NULL CHARACTER
**                             COMPUTE NEW BYTE AND BIT POSITIONS
**                          ENDIF
**                       ELSE ADJACENT SUBFIELDS NOT TO BE COMPRESSED
**                       THEN
**                          COMPUTE BYTE SIZE OF BIT DATA FIELD
**                          SET READ STRING TO STORED VALUE
**                          TERMINATE READ STRING WITH NULL CHARACTER
**                          UPDATE BYTE POSITION INDEX
**                       ENDIF
**                    ENDIF
**                 ENDIF
**                 
**                 IF DATA TYPE IS NOT BINARY
**                 THEN
**                    IF WIDTH IS ZERO AND DELIM IS NULL CHARACTER
**                    THEN
**                       IF NEXT OF CUR_DV NOT NULL
**                       THEN
**                          CONCATENATE UT TO RD_STR
**                          UPDATE BYTE POSITION INDEX
**                       ENDIF
**                    ELSEIF DELIM RETRIEVED BY GET123FMT() NOT NULL CHARACTER
**                    THEN
**                       IF NEXT CUR_DV NOT NULL
**                       THEN 
**                          CONCATENATE DELIM RETRIEVED BY GET123FMT() TO RD_STR
**                          UPDATE BYTE POSITION INDEX
**                       ENDIF
**                    ENDIF 
**                 ENDIF
**
**                 SET CUR_DV TO NXT_VAL
**  
**              END WHILE
**   
**              SET ROW_DVH TO NXT_VSET
**              IF DATA TYPE IS NOT BINARY
**              THEN
**                 IF ROW_DVH IS NOT NULL
**                 THEN
**                    IF WIDTH IS ZERO AND DELIM IS NC
**                    THEN
**                       CONCATENATE UT TO RD_STR
**                       UPDATE BYTE POSITION INDEX
**                    ELSEIF DELIM RETRIEVED FROM GET123FMT() NOT NULL CHARACTER
**                    THEN
**                       CONCATENATE DELIM RETRIEVED FROM GET123FMT() TO RD_STR
**                       UPDATE BYTE POSITION INDEX
**                    ENDIF 
**                 ENDIF
**              ENDIF
**           END WHILE
**        ENDIF
**     ENDIF
**
**     SET TAG TO CURRENT DR TAG SUBFIELD
**     SET OUTPUT LEADID TO CUR_DR LEADER LEAD_ID
**     IF NOT ON A BYTE BOUNDARY
**     THEN
**        INCREMENT BYTE POSITION
**     ENDIF
**     CONCATENATE FIELD TERMINATOR TO RD_STR
**     UPDATE BYTE POSITION INDEX
**     SET SF_STATE_DR TO FINISHED
**     IF NOT END OF DR FIELDS
**     THEN
**        IF STATUS NOT EQUAL TO START OF RECORD
**        THEN
**           SET STATUS TO OKAY
**        ENDIF
**     ELSE
**        CALL SET123STAT() TO CHECK FOR END OF FILE
**        IF STATUS IS NOT END OF FILE 
**        THEN
**           SET STATUS TO END OF RECORD
**        ENDIF
**     ENDIF 
**     SET STRING LENGTH TO BYTE POSITION INDEX
** 
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int rd123fld(FILE *fp,char *tag,char *leadid,char *rd_str,long *str_len,int* status)

{
   /* INTERNAL VARIABLES */
   char anum[10];
   char delim[2];
   char dtyp[2] ;
   long width   ;
   size_t b_siz ;
   int byte_pos ;
   int bit_pos ;
   int  b_cnt;
   int  b_len;
   int  i;
   ldiv_t bytes;
   unsigned char tmp_nosign;

   /* INIT STATUS TO FAILURE */
   *status = 0;
    
   /* CALL GET123LEVEL() TO GET APPROPRIATE FM ENTRY FOR THIS FILE POINTER */
   if (!get123level(fp)) return(0);

   /* SET CUR_DR TO NEXT FIELD */
   if (cur_fm->cur_dr != NULL) {
      cur_fm->cur_dr = cur_fm->cur_dr->next;
   };   

   /* IF CUR_DR IS NULL */
   if (cur_fm->cur_dr == NULL) {

      /* CALL LD123REC() TO LOAD DATA RECORD INFORMATION INTO DATA
          STRUCTURES FOR REFERENCE
      */
      if (!ld123rec()) {

         /* IF END OF FILE ENCOUNTERED */
         if (feof(fp)) {

            /* SET STATUS TO END OF FILE */
            *status = 4;
         };
         return(0);
      };    
      
      /* SET STATUS TO START OF RECORD */
      *status = 2;
      
      /* SET CUR_DR TO NEXT POINTER OF DR_HD */
      cur_fm->cur_dr = cur_fm->dr_hd->next;
   };

   /* CALL RET123MATCH() TO RETURN DATA DESCRIPTION FOR CURRENT ENTRY */
   if (!ret123match(cur_fm->cur_dr->tag)) return(0);

   /* INITIALIZE RD_STR TO NULL */
   *rd_str = NC;

   /* INITIALIZE B_SIZ, BYTE_POS, AND BIT_POS TO ZERO */
   b_siz = 0;
   byte_pos = 0;
   bit_pos = 0;

   /* IF NUM_DIM GREATER THAN ZERO */
   if (cur_fm->cur_dr->num_dim > 0) {

      /* CONCATENATE NUM_DIM TO RD_STR */
      i123toa(cur_fm->cur_dr->num_dim,anum);
#if CONV
      conv123cat(rd_str,anum);
#else
      strcat(rd_str,anum);
#endif
        
      /* UPDATE INDEX POINTER */
      byte_pos += _tcslen(anum);

      /* CONCATENATE UT TO RD_STR */
      strcat(rd_str,UT_STR);

      /* UPDATE INDEX POINTER */
      byte_pos += _tcslen(UT_STR);

      /* SET CUR_DM TO DIM_LPTR */
      cur_fm->cur_dm = cur_fm->cur_dr->dim_lptr;
         
      /* SKIP DUMMY DM */
      cur_fm->cur_dm = cur_fm->cur_dm->nxt;

      /* WHILE CUR_DM NOT NULL DO */
      while(cur_fm->cur_dm != NULL) {

         /* CONCATENATE LEN TO RD_STR */
         i123toa(cur_fm->cur_dm->len,anum);
#if CONV
         conv123cat(rd_str,anum);
#else
         strcat(rd_str,anum);
#endif

         /* UPDATE INDEX POINTER */
         byte_pos += _tcslen(anum);

         /* CONCATENATE UT TO RD_STR */
         strcat(rd_str,UT_STR);

         /* UPDATE INDEX POINTER */
         byte_pos += _tcslen(UT_STR);

         /* SET CUR_DM TO NXT */
         cur_fm->cur_dm = cur_fm->cur_dm->nxt;
      }
   }

   /* IF FMT_RT OF CUR_DD IS NULL { NO FORMATS FOR VALUES } */
   if (cur_fm->cur_dd->fmt_rt == NULL) {

      /* SET ROW_DVH TO NXT_VSET FIELD OF VALUES FIELD OF CUR_DR  */
      cur_fm->row_dvh = cur_fm->cur_dr->values->nxt_vset;

      /* WHILE ROW_DVH NOT NULL DO */
      while(cur_fm->row_dvh != NULL) {

         /* SET CUR_DV TO ROW_DVH */
         cur_fm->cur_dv = cur_fm->row_dvh;

         /* WHILE CUR_DV NOT NULL DO */
         while(cur_fm->cur_dv != NULL) {

            /* IF STRING VALUE NOT NULL */
            if (cur_fm->cur_dv->value != NULL) {
            
               /* DETERMINE LENGTH OF VALUE IN BYTES */
               b_siz = _tcslen(cur_fm->cur_dv->value);

               /* CONCATENATE VALUE TO RD_STR */
#if CONV
               conv123cat(&rd_str[byte_pos],cur_fm->cur_dv->value);
#else
               strcat(&rd_str[byte_pos],cur_fm->cur_dv->value);
#endif

               /* UPDATE INDEX POINTER */
               byte_pos += b_siz;

            };
            
            /* SET CUR_DV TO NXT_VAL */
            cur_fm->cur_dv = cur_fm->cur_dv->nxt_val;

            /* IF CUR_DV NOT NULL */
            if (cur_fm->cur_dv != NULL) {

               /* CONCATENATE UT TO RD_STR */
               strcat(&rd_str[byte_pos],UT_STR);

               /* UPDATE INDEX POINTER */
               byte_pos++;
               
            }
         }

         /* SET ROW_DVH TO NXT_VSET */
         cur_fm->row_dvh = cur_fm->row_dvh->nxt_vset;

         /* IF ROW_DVH NOT NULL */
         if (cur_fm->row_dvh != NULL) {

            /* CONCATENATE UT TO RD_STR */
            strcat(&rd_str[byte_pos],UT_STR);

            /* UPDATE INDEX POINTER */
            byte_pos++;

         }
      }
   }
   /* ELSE { FORMATS } */
   else {

      /* SET CUR_FC TO FMT_RT OF CUR_DD  {INIT GET123FMT} */
      cur_fm->cur_fc = cur_fm->cur_dd->fmt_rt;

      /* SET CUR_FCR TO CUR_FC */
      cur_fm->cur_fcr = cur_fm->cur_fc;

      /* IF VALUES OF CUR_DR NOT NULL */
      if (cur_fm->cur_dr->values != NULL) {

         /* SET ROW_DVH TO NXT_VSET FIELD OF VALUES FIELD OF CUR_DR  */
         cur_fm->row_dvh = cur_fm->cur_dr->values->nxt_vset;

         /* WHILE ROW_DVH NOT NULL DO */
         while(cur_fm->row_dvh != NULL) {

            /* SET CUR_DV TO ROW_DVH */
            cur_fm->cur_dv = cur_fm->row_dvh;

            /* WHILE CUR_DV NOT NULL DO */
            while(cur_fm->cur_dv != NULL) {

               /* INITIALIZE VARIABLES */
               memset(delim,NC,2);
               memset(dtyp,NC,2);
               width = 0;

               /* CALL GET123FMT() TO RETRIEVE FORMAT FOR THIS DATA VALUE */
               if (!get123fmt(dtyp,&width,delim)) return(0);

               /* IF DATA TYPE NOT BINARY */
               if (*dtyp != 'B') {
                     
                  /* IF NOT POSITIONED AT BYTE BOUNDARY */
                  if (bit_pos != 0) {

                     /* ADVANCE POSITION TO NEXT BYTE BOUNDARY */
                     bit_pos = 0;
                     byte_pos++;
                  }

                  /* IF VALUE STRING IS NOT NULL */
                  if (cur_fm->cur_dv->value != NULL) {

                     /* DETERMINE LENGTH OF VALUE IN BYTES */
                     b_siz = _tcslen(cur_fm->cur_dv->value);

                     /* CONCATENATE VALUE TO RD_STR */
#if CONV
                     conv123cat(&rd_str[byte_pos],cur_fm->cur_dv->value);
#else
                     strcat(&rd_str[byte_pos],cur_fm->cur_dv->value);
#endif

                     /* UPDATE INDEX POINTER */
                     byte_pos += b_siz;
                  }
               } 
               /* ELSE BINARY DATA */
               else {

                  /* IF FIELD IS VARIABLE LENGTH BIT FIELD */
                  if (width == 0) {
                  
                     /* IF NOT POSITIONED AT BYTE BOUNDARY */
                     if (bit_pos != 0) {

                        /* ADVANCE POSITION TO NEXT BYTE BOUNDARY */
                        bit_pos = 0;
                        byte_pos++;
                     }

                     /* EXTRACT FIELD LENGTH COUNT */
                     anum[0] = cur_fm->cur_dv->value[0];
                     anum[1] = NC;
                     b_cnt = atoi(anum);

                     /* CONCATENATE FIELD LENGTH COUNT TO RD_STR */
#if CONV
                     conv123cat(&rd_str[byte_pos],anum);
#else
                     strcat(&rd_str[byte_pos],anum);
#endif
                     /* UPDATE BYTE POSITION INDEX */
                     byte_pos++;

                     /* EXTRACT FIELD LENGTH */
                     for (i=0;i<b_cnt;i++) anum[i] = cur_fm->cur_dv->value[i+1];
                     anum[b_cnt] = NC;
                     b_len = atoi(anum);

                     /* CONCATENATE FIELD LENGTH TO RD_STR */
#if CONV
                     conv123cat(&rd_str[byte_pos],anum);
#else
                     strcat(&rd_str[byte_pos],anum);
#endif
                     /* UPDATE BYTE POSITION INDEX */
                     byte_pos += b_cnt;

                     /* COMPUTE BYTE SIZE OF BIT DATA FIELD */
                     bytes = ldiv((long) b_len,(long) BYTE_S);
                     if (bytes.rem != 0) b_len = (int) bytes.quot + 1;
                     else                b_len = (int) bytes.quot;

                     /* SET RD_STR TO STORED VALUE */
                     memcpy(&rd_str[byte_pos],&cur_fm->cur_dv->value[b_cnt+1],b_len);
                     /* TERMINATE RD_STR WITH NULL CHARACTER */
                     rd_str[byte_pos+b_len] = NC;

                     /* UPDATE INDEX POINTER */
                     byte_pos += b_len;
                  } 
         
                  /* ELSE FIELD IS FIXED LENGTH BIT FIELD */
                  else
                  {
                     /* IF DO COMPRESS */
                     if (cur_fm->compress) {

                        /* COMPUTE BYTE SIZE OF BIT DATA FIELD */
                        bytes = ldiv(width,BYTE_S);
                        if (bytes.rem != 0) b_siz = (int) bytes.quot + 1;
                        else                b_siz = (int) bytes.quot;
                        
                        /* IF NOT ON BYTE BOUNDARY */
                        if (bit_pos != 0) {
                              
                           /* SET READ STRING TO STORED VALUE */
                           memcpy(&rd_str[byte_pos+1],cur_fm->cur_dv->value,b_siz);
                           b_siz = (size_t) (bit_pos + width);
                           bytes = ldiv((long) b_siz,(long) BYTE_S);

                           /* SHIFT VALUE LEFT IN READ STRING SO THAT IT IS
                               ADJACENT TO PREVIOUS BIT FIELD
                           */
                           rd_str[byte_pos] >>= (BYTE_S - bit_pos);
                           for (i=byte_pos;i<=byte_pos+(int)bytes.quot;i++) {
                              rd_str[i] <<= (BYTE_S - bit_pos);
                              tmp_nosign = rd_str[i+1];
                              tmp_nosign >>= bit_pos;
                              rd_str[i] = rd_str[i] | tmp_nosign;
                           }

                           /* COMPUTE NEW BYTE AND BITS POSITIONS */
                           byte_pos = byte_pos + (int) bytes.quot;
                           bit_pos = (int) bytes.rem;

                           /* ADD TRUNCATING NULL CHARACTER TO READ STRING */
                           if (bytes.rem != 0) rd_str[byte_pos+1] = NC;
                           else                rd_str[byte_pos] = NC;

                        }
             
                        /* ELSE ON BYTE BOUNDARY */
                        else
                        {
                           
                           /* SET READ STRING TO STORED VALUE */
                           memcpy(&rd_str[byte_pos],cur_fm->cur_dv->value,b_siz);
                           /* TERMINATE RD_STR WITH NULL CHARACTER */
                           rd_str[byte_pos+b_siz] = NC;
 
                           /* COMPUTE NEW BYTE AND BIT POSITIONS */
                           byte_pos = byte_pos + (int) bytes.quot;
                           bit_pos = (int) bytes.rem;
                        }
                     }

                     /* ELSE DO NOT COMPRESS */
                     else
                     {

                        /* COMPUTE BYTE SIZE OF BIT DATA FIELD */
                        bytes = ldiv(width,BYTE_S);
                        if (bytes.rem != 0) b_siz = (int) bytes.quot + 1;
                        else                b_siz = (int) bytes.quot;

                        /* SET RD_STR TO STORED VALUE */
                        memcpy(&rd_str[byte_pos],cur_fm->cur_dv->value,b_siz);

                        /* TERMINATE RD_STR WITH NULL CHARACTER */
                        rd_str[byte_pos+b_siz] = NC;

                        /* UPDATE INDEX POINTER */
                        byte_pos += b_siz;
                     }  
                  }
               };

               /* IF DATA TYPE IS NOT BINARY */
               if (*dtyp != 'B') {

                  /* IF WIDTH IS ZERO AND DELIM IS NULL CHARACTER */
                  if (!width && delim[0] == NC) {

                     /* IF NEXT OF CUR_DV NOT NULL */
                     if (cur_fm->cur_dv->nxt_val != NULL) {

                        /* CONCATENATE UT TO RD_STR */
                        strcat(&rd_str[byte_pos],UT_STR);

                        /* UPDATE INDEX POINTER */
                        byte_pos++;
                     }
                  }
                  /* ELSEIF DELIM RETRIEVED BY GET123FMT() NOT NULL CHARACTER */
                  else if (delim[0] != NC) {

                     /* IF NEXT OF CUR_DV NOT NULL */
                     if (cur_fm->cur_dv->nxt_val != NULL) {

                        /* CONCATENATE DELIM RETRIEVED BY GET123FMT() TO RD_STR */
#if CONV
                        conv123cat(&rd_str[byte_pos],delim);
#else
                        strcat(&rd_str[byte_pos],delim);
#endif

                        /* UPDATE INDEX POINTER */
                        byte_pos++;
                     };
                  }
               }

               /* SET CUR_DV TO NXT_VAL */
               cur_fm->cur_dv = cur_fm->cur_dv->nxt_val;
            }

            /* SET ROW_DVH TO NXT_VSET */
            cur_fm->row_dvh = cur_fm->row_dvh->nxt_vset;
 
            /* IF DATA TYPE IS NOT BINARY */
            if (*dtyp != 'B') {

               /* IF ROW_DVH IS NOT NULL */
               if (cur_fm->row_dvh != NULL) {
   
                  /* IF WIDTH IS ZERO AND DELIM IS NC */
                  if (!width && delim[0] == NC) {
   
                     /* CONCATENATE UT TO RD_STR */
                     strcat(&rd_str[byte_pos],UT_STR);
   
                     /* UPDATE INDEX POINTER */
                     byte_pos++;
   
                  }
                  /* ELSEIF DELIM RETRIEVED FROM GET123FMT() NOT NULL CHARACTER */
                  else if (delim[0] != NC) {
   
                     /* CONCATENATE DELIM RETRIEVED FROM GET123FMT() TO RD_STR */
#if CONV
                     conv123cat(&rd_str[byte_pos],delim);
#else
                     strcat(&rd_str[byte_pos],delim);
#endif
   
                     /* UPDATE INDEX POINTER */
                     byte_pos++;
   
                  }
               }
            }
         }
      }
   }

   /* SET TAG TO CURRENT DR TAG SUBFIELD */
   strcpy(tag,cur_fm->cur_dr->tag);
   
   /* SET OUTPUT LEADID TO CUR_DR LEADER LEAD_ID */
   *leadid = cur_fm->rl_hd->lead_id;
   
   /* IF NOT ON BYTE BOUNDARY INCREMENT BYTE POSITION */
   if (bit_pos != 0) byte_pos++;

   /* CONCATENATE FIELD TERMINATOR TO RD_STR */
   strcat(&rd_str[byte_pos],FT_STR);

   /* UPDATE INDEX POINTER */
   byte_pos++;

   /* SET SF_STATE_DR TO FINISHED */
   cur_fm->sf_state_dr = 4;

   /* IF NOT END OF DR FIELDS */
   if ( cur_fm->cur_dr->next != NULL) {
   
      /* IF STATUS NOT EQUAL TO START OF RECORD */
      if (*status != 2) {
      
         /* SET STATUS TO OK */
         *status = 1;
      };   
   }
   else {
   
      /*  CALL SET123STAT TO CHECK FOR END OF FILE */
      if (!set123stat(fp,status)) return(0);
      
      /* IF STATUS IS NOT END OF FILE */ 
      if (*status != 4) {
 
         /* SET STATUS TO END OF RECORD */
         *status = 3;
      };   
   };   
   
   /* SET STR_LEN */
   *str_len = (long) byte_pos;
      
   /* RETURN SUCCESS */
   return(1);
}

