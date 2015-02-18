# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: G123BSTR
**
**    PURPOSE: TO RETRIEVE BINARY DATA INTO A CHARACTER BUFFER
**
**    INVOCATION METHOD: G123BSTR(IN_STR, COMPRESSED, BUF_STR, BIT_LEN)
**
**    ARGUMENT LIST:
**     NAME       TYPE      USE      DESCRIPTION
**     IN_STR[]   PTR        I       POINTER TO INPUT CHARACTER STRING
**     COMPRESSED INT        I       FLAG INDICATING ADJACENT FIXED-LENGTH 
**                                    BIT FIELDS ARE BEING READ
**     BUF_STR[]  PTR       I/O      POINTER TO A BUFFER 
**     BIT_LEN    LONG      I/O      LENGTH OF STRING TO BE READ
**                                    (INPUT LENGTH IS IN BITS,
**                                     OUTPUT LENGTH IS IN BYTES)
**     G123BSTR() LOGICAL    O       SUCCESS FLAG 
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     ADDBITS    LDIV_T             NUMBER OF BITS TO BE READ FOR SUBFIELD
**     ANUM1[10]  CHAR               TEMPORARY CHARACTER STRING 1
**     ANUM2[10]  CHAR               TEMPORARY CHARACTER STRING 2
**     B_CNT      INT                VARIABLE LENGTH BIT FIELD DIGIT COUNT
**     B_LEN      INT                VARIABLE LENGTH BIT FIELD LENGTH
**     BITS       LDIV_T             NUMBER OF BITS REQUIRED FOR SUBFIELD
**     BYTE_LEN   LONG               LENGTH OF STRING TO BE READ IN (BYTES)
**     I          LONG               INDEX VARIABLE
**     READ_LEN   LONG               LENGTH IN BYTES WHICH NEED TO BE READ
**                                    WHEN NOT ENOUGH BITS IN STORAGE
**
**
**    GLOBAL REFERENCES:
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
**      BIT_CNT         INT    I/O   COUNT OF BITS STORED IN MEMORY FOR 
**                                    A SUBSEQUENT READ FUNCTION
**      BIT_BIN         CHAR   I/O   BITS STORED IN MEMORY FOR SUBSEQUENT
**                                    READ FUNCTION
**      COMPRESS        INT    N/A   FLAG TO SPECIFY COMPRESSED OR
**                                    UNCOMPRESSED ADJACENT FIXED LENGTH
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
**     NAME              TYPE   USE   DESCRIPTION
**     CUR_FM            PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                     STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS:
**     NAME              TYPE         DESCRIPTION
**     BYTE_S            INT          NUMBER OF BITS IN A BYTE
**     NC                CHAR         NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT      92DR005    04/08/92  INITIAL PROLOG
**     A. DEWITT      92DR005    04/08/92  INITIAL PDL
**     A. DEWITT      92DR005    04/08/92  INITIAL CODE
**     J. TAYLOR      92DR005    05/14/92  MODIFIED TO INCLUDE ADJACENT 
**                                          FIXED-LENGTH BIT FIELD STORAGE
**     L. MCMILLION   TASK #40   11/24/92  UPDATED PROLOG/PDL PER QC
**     J. TAYLOR      93DR023    04/20/93  MODIFIED TO RETRIEVE FROM CHARACTER
**                                          RATHER THAN FILE
**     L. MCMILLION   93DR023    05/13/93  INSERTED SIZE_T CAST FOR MEMCPY
**                                          ARGUMENT
**     J. TAYLOR      93DR037    08/17/93  REMOVED MEMSET CALLS
**     J. TAYLOR      93DR039    08/17/93  REMOVED MEMSET CALLS
**
**    PDL:
**
**    IF FIELD IS VARIABLE LENGTH BIT FIELD
**    THEN
**       RETRIEVE FIELD LENGTH COUNT
**       MOVE BEGINNING OF INPUT STRING PAST FIELD LENGTH COUNT
**       EXTRACT FIELD LENGTH COUNT
**       RETRIEVE FIELD LENGTH
**       MOVE BEGINNING OF INPUT STRING PAST FIELD LENGTH
**       EXTRACT FIELD LENGTH
**       COMPUTE SIZE OF BIT FIELD TO NEAREST BYTE
**       STORE FIELD LENGTH COUNT IN BUFFER STRING
**       STORE FIELD LENGTH IN BUFFER STRING
**       RETRIEVE BIT FIELD
**       MOVE BEGINNING OF INPUT STRING PAST BIT FIELD
**       COMPUTE ACTUAL LENGTH OF BIT DATA FIELD
**       TERMINATE BUFFER STRING WITH NULL CHARACTER
**    ELSE FIELD IS FIXED LENGTH BIT FIELD
**       COMPUTE SIZE OF BIT FIELD TO NEAREST BYTE
**       TERMINATE BUFFER WITH A NULL CHARACTER
**       IF COMPRESSED
**       THEN
**          IF NO BITS IN STORAGE
**          THEN
**             RETRIEVE BIT FIELD
**             MOVE BEGINNING OF INPUT STRING PAST BIT FIELD
**             STORE LEFT OVER BITS FOR NEXT BIT FIELD
**          ELSE IF NOT ENOUGH BITS IN STORAGE FOR THIS FIELD
**          THEN
**             DETERMINE ADDITIONAL NUMBER OF BITS TO RETRIEVE
**             RETRIEVE REMAINDER OF BIT FIELD NOT STORED IN MEMORY
**             MOVE BEGINNING OF INPUT STRING PAST BIT FIELD
**             MOVE BITS IN STORAGE TO BUFFER
**             STORE LEFT OVER BITS FOR NEXT BIT FIELD
**             SHIFT ENTIRE FIELD LEFT TO BUILD BIT FIELD
**             UPDATE BIT COUNT
**             CLEAR RIGHT MOST BYTE AFTER SHIFTING EVERY BIT LEFT
**          ELSE IF ENOUGH BITS IN STORAGE FOR THIS FIELD
**          THEN
**             MOVE BITS IN STORAGE TO STRING
**             SHIFT STORAGE LEFT TO CONTAIN ONLY LEFT OVER BITS
**          ENDIF
**       ELSE NOT COMPRESSED
**          RETRIEVE BIT FIELD
**          MOVE BEGINNING OF INPUT STRING PAST BIT FIELD
**       ENDIF
**    ENDIF
**    FILL END OF STRING WITH BINARY ZEROS
**    RETURN BYTE SIZE IN PLACE OF BIT SIZE
**    RETURN SUCCESS
**
*****************************************************************************
**     CODE SECTION
**
*****************************************************************************/ 
#include "stc123.h"

int g123bstr(char **in_str,int compressed,char *buf_str,long *bit_len)
{
   ldiv_t addbits;
   char  anum1[10];
   char  anum2[10];
   int   b_cnt;
   int   b_len;
   long  byte_len;
   ldiv_t bits;
   long   i;
   long read_len;

   /* IF FIELD IS VARIABLE LENGTH BIT FIELD */
   if (*bit_len == 0) {

      /* RETRIEVE FIELD LENGTH COUNT */
      memcpy(anum1,*in_str,1);

      /* MOVE BEGINNING OF INPUT STRING PAST FIELD LENGTH COUNT */
      *in_str = *in_str + 1;
 
      /* EXTRACT FIELD LENGTH COUNT */
      anum1[1] = NC;
      b_cnt = atoi(anum1);
      
      /* RETRIEVE FIELD LENGTH */
      memcpy(anum2,*in_str,b_cnt);

      /* MOVE BEGINNING OF INPUT STRING PAST FIELD LENGTH */
      *in_str = *in_str + b_cnt;

      /* EXTRACT FIELD LENGTH */
      anum2[b_cnt] = NC;
      b_len = atoi(anum2);

      /* COMPUTE SIZE OF BIT FIELD TO NEAREST BYTE */
      bits = ldiv((long) b_len, (long) BYTE_S);
      if (bits.rem > 0) byte_len = bits.quot + 1;
      else              byte_len = bits.quot;
  
      /* STORE FIELD LENGTH COUNT IN BUFFER STRING */
      buf_str[0] = anum1[0];
      buf_str[1] = NC;

      /* STORE FIELD LENGTH IN BUFFER STRING */
      strcat(buf_str,anum2);

      /* RETRIEVE BIT FIELD */
      memcpy(&buf_str[b_cnt+1],*in_str,(size_t)byte_len);

      /* MOVE BEGINNING OF INPUT STRING PAST BIT FIELD */
      *in_str = *in_str + byte_len;

      /* COMPUTE ACTUAL LENGTH OF BIT DATA FIELD */
      byte_len = byte_len + b_cnt + 1;
         
      /* TERMINATE BUFFER STRING WITH NULL CHARACTER */
      buf_str[byte_len] = NC;
   }

   /* ELSE FIELD IS FIXED LENGTH BIT FIELD */
   else {

      /* COMPUTE SIZE OF BIT FIELD TO NEAREST BYTE */
      bits = ldiv(*bit_len, (long) BYTE_S);
      if (bits.rem > 0) byte_len = bits.quot + 1;
      else              byte_len = bits.quot;

      /* TERMINATE BUFFER WITH A NULL CHARACTER */
      buf_str[byte_len] = NC;

      /* IF COMPRESSED */
      if (compressed) {

         /* IF NO BITS IN STORAGE */
         if (cur_fm->bit_cnt == 0) {

            /* RETRIEVE BIT FIELD */
            memcpy(buf_str,*in_str,(size_t)byte_len);

            /* MOVE BEGINNING OF INPUT STRING PAST BIT FIELD */
            *in_str = *in_str + byte_len;

            /* STORE LEFT OVER BITS FOR NEXT BIT FIELD */
            if (bits.rem > 0) {
               cur_fm->bit_cnt = (int)(BYTE_S - bits.rem);
               cur_fm->bit_bin = buf_str[byte_len-1];
               cur_fm->bit_bin <<= bits.rem;
            }
         }

         /* ELSE IF NOT ENOUGH BITS IN STORAGE FOR THIS FIELD */
         else 
         if (cur_fm->bit_cnt < (int) *bit_len) {

            /* DETERMINE ADDITIONAL NUMBER OF BITS TO READ */
            addbits = ldiv(*bit_len - cur_fm->bit_cnt, BYTE_S);

            if (addbits.rem > 0) read_len = addbits.quot + 1;
            else                 read_len = addbits.quot;

            /* RETRIEVE REMAINDER OF BIT FIELD NOT STORED IN MEMORY */
            memcpy(&buf_str[1],*in_str,(size_t)read_len);

            /* MOVE BEGINNING OF INPUT STRING PAST BIT FIELD */
            *in_str = *in_str + read_len;

            /* MOVE BITS IN STORAGE TO BUFFER */
            buf_str[0] = cur_fm->bit_bin;

            /* STORE LEFT OVER BITS FOR NEXT BIT FIELD */
            cur_fm->bit_bin = buf_str[read_len];
            cur_fm->bit_bin <<= addbits.rem;

            /* SHIFT ENTIRE FIELD LEFT TO BUILD BIT FIELD */
            *buf_str >>= (BYTE_S - cur_fm->bit_cnt);
            for (i=0;i<read_len+1;i++) {
               buf_str[i] <<= (BYTE_S - cur_fm->bit_cnt);
               buf_str[i] = buf_str[i] | (buf_str[i+1] >> cur_fm->bit_cnt);
            }

            /* UPDATE BIT COUNT */
            cur_fm->bit_cnt = (int) (BYTE_S - addbits.rem);
      
            /* CLEAR RIGHT MOST BYTE AFTER SHIFTING EVERY BIT LEFT */
            buf_str[byte_len] = NC;
         }

         /* ELSE IF ENOUGH BITS IN STORAGE FOR THIS FIELD */
         else
         if ((long) cur_fm->bit_cnt >= *bit_len) {

            /* MOVE BITS IN STORAGE TO STRING */
            buf_str[0] = cur_fm->bit_bin;

            /* SHIFT STORAGE LEFT TO CONTAIN ONLY LEFT OVER BITS */
            cur_fm->bit_cnt -= (int) bits.rem;
            cur_fm->bit_bin <<= bits.rem;
         }
      }
      /* ELSE NOT COMPRESSED */
      else {

         /* RETRIEVE BIT FIELD */
         memcpy(buf_str,*in_str,(size_t)byte_len);

         /* MOVE BEGINNING OF INPUT STRING PAST BIT FIELD */
         *in_str = *in_str + byte_len;
      }
   }

   /* FILL END OF STRING WITH BINARY ZEROS */
   if (bits.rem > 0) {
      buf_str[byte_len-1] >>= (BYTE_S - bits.rem);
      buf_str[byte_len-1] <<= (BYTE_S - bits.rem);
   }

   /* RETURN BYTE SIZE IN PLACE OF BIT SIZE */
   *bit_len = byte_len;

   /* RETURN SUCCESS */
   return(1);
}
