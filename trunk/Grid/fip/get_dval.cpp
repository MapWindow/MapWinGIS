# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: GET123DVAL
**
**    PURPOSE: TO RETRIEVE A DATA VALUE STRING BASED ON FORMAT CONTROL
**
**    INVOCATION METHOD: GET123DVAL(IN_STR,IN_STR_END,D_TYP,WIDTH,DELIM,
**                                   COMPRSSD,STRING)
**
**    ARGUMENT LIST:
**     NAME          TYPE      USE      DESCRIPTION
**     IN_STR[]      PTR        I       POINTER TO INPUT CHARACTER STRING
**     IN_STR_END[]  PTR        I       POINTER TO END OF INPUT CHARACTER STRING
**     D_TYP         CHAR       I       DATA FIELD TYPE
**     WIDTH         INT        I       DATA FIELD WIDTH SPECIFICATION
**     DELIM         CHAR       I       DATA FIELD DELIMITER
**     COMPRSSD      INT        I       INDICATOR OF COMPRESSED ADJACENT
**                                       FIXED-LENGTH BINARY SUBFIELDS
**     STRING[]      PTR        O       DATA VALUE STRING
**     GET123DVAL()  LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     G123BSTR()       RETRIEVE BINARY DATA
**     G123DSTR()       RETRIEVE A DELIMITED STRING
**     G123SSTR()       RETRIEVE A SUB STRING
**
**    INTERNAL VARIABLES:
**     NAME       TYPE               DESCRIPTION
**     B_LENGTH   INT                BIT FIELD LENGTH COUNT
**     I_STR[10]  CHAR               CHARACTER STRING CONTAINING AN INTEGER
**     L_WIDTH    INT                LENGTH OF BIT FIELD IN BITS
**     POS        INT                POSITION INDICATOR INTO STRING
**     TMP_DEL    CHAR               TEMP FIELD DELIMITER
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
**      BIT_CNT         INT     O    COUNT OF BITS STORED IN MEMORY FOR 
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
**
**    GLOBAL CONSTANTS:
**     NAME       TYPE               DESCRIPTION
**     NC         CHAR               NULL CHARACTER  
**     UT         CHAR               UNIT TERMINATOR
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     P. HODGES                 06/04/90  INITIAL CODE
**     A. DEWITT     92DR005     04/02/92  IMPLEMENT TO READ BINARY
**                                          DATA 
**     J. TAYLOR     92DR005     05/14/92  ADDED ADJACENT FIXED-LENGTH
**                                          BIT FIELD STORAGE
**     L. MCMILLION  TASK #40    12/15/92  UPDATED PROLOG/PDL
**     J. TAYLOR     93DR023     04/20/93  MODIFIED TO RETRIEVE FROM CHARACTER
**                                          RATHER THAN FILE
**
**    PDL:
**
**     SET TMP_DEL TO DELIM   
**     INITIALIZE STRING CONTAINING INTEGER TO NULL CHARACTER
**     IF NOT A FIXED LENGTH BIT FIELD
**     THEN
**        RESET BIT FIELD STORAGE   
**     ENDIF
**     IF WIDTH IS GREATER THAN ZERO   
**     THEN
**        IF THE DATA TYPE IS NOT BINARY
**        THEN   
**           CALL G123SSTR() RETRIEVE DATA STRING
**        ELSE THE DATA TYPE IS BINARY   
**           CALL G123BSTR() TO RETRIEVE BINARY DATA BYTES
**        ENDIF
**     ELSE WIDTH IS LESS OR EQUAL TO ZERO
**        IF THE DATA TYPE IS NOT BINARY
**        THEN   
**           IF TMP_DEL IS NC { NULL CHARACTER }   
**           THEN
**              SET TMP_DEL TO UT
**           ENDIF   
**           CALL G123DSTR() TO RETRIEVE DATA STRING
**        ELSE THE DATA TYPE IS BINARY
**        THEN   
**           IF NOT END OF INPUT STRING
**           THEN
**              RETRIEVE BYTE ZERO OF VARIABLE BINARY DATA TYPE
**              MOVE BEGINNING OF INPUT STRING PAST BYTE ZERO
**              TERMINATE DIGIT STRING WITH NULL CHAR
**              SET FIELD LENGTH COUNT
**              SET STRING TO ISTR
**           ELSE
**              RETURN FAILURE
**           ENDIF
**           INITIALIZE POS
**           IF NOT END OF INPUT STRING
**           THEN
**              RETRIEVE BIT FIELD LENGTH DIGITS
**              MOVE BEGINNING OF INPUT STRING PAST FIELD LENGTH DIGITS
**              TERMINATE DIGIT STRING WITH NULL CHAR
**              MOVE STRING TO ISTR
**              UPDATE POS
**              SET L_WIDTH
**           ELSE
**              RETURN FAILURE
**           ENDIF
**           CALL G123BSTR() TO RETRIEVE BINARY DATA BYTES
**           RESET BIT FIELD STORAGE
**           UPDATE L_WIDTH TO TOTAL NUMBER OF BYTES IN STRING   
**           SET WIDTH TO L_WIDTH   
**        ENDIF
**     ENDIF
**
**     RETURN SUCCESS   
**
*****************************************************************************
**     CODE SECTION
**     
*****************************************************************************/
#include "stc123.h"

int get123dval(char **in_str,char *in_str_end,int d_typ,long *width,int delim,int comprssd,char *string)
{
   /* INTERNAL VARIABLES */
   char tmp_del;
   long l_width;
   int  b_length;
   int  pos;
   char i_str[10];

   /* SET TMP_DEL TO DELIM */
   tmp_del = delim;

   /* INITIALIZE STRING CONTAINING INTEGER TO NULL CHARACTER */
   *i_str = NC;

   /* IF NOT A FIXED LENGTH BIT FIELD, RESET BIT FIELD STORAGE */
   if ((d_typ != 'B') || (*width == 0)) cur_fm->bit_cnt = 0;

   /* IF WIDTH IS GREATER THAN ZERO */
   if (*width > 0) {

       /*  IF THE DATA TYPE IS NOT BINARY */
       if (d_typ != 'B') {
       
          /* CALL G123SSTR() RETRIEVE DATA STRING */
          if (!g123sstr(in_str,string,*width)) return(0);
       }

       /*  ELSE THE DATA TYPE IS BINARY */
       else {

          /* CALL G123BSTR() TO RETRIEVE BINARY DATA BYTES */
          if (!g123bstr(in_str,comprssd,string,width)) return(0);
       };
   }
   else {

      /*  IF THE DATA TYPE IS NOT BINARY */
      if (d_typ != 'B') {

         /* IF TMP_DEL IS NC { NULL CHARACTER } */
         if (tmp_del == NC) {

            /* SET TMP_DEL TO UT */
            tmp_del = UT;
         }

         /* CALL G123DSTR() TO RETRIEVE DATA STRING */
         if (!g123dstr(in_str,string,tmp_del)) return(0);
      }
      /*  ELSE THE DATA TYPE IS BINARY */
      else {

         /* IF NOT END OF INPUT STRING */
         if (*in_str < in_str_end) {

            /* RETRIEVE BYTE ZERO OF VARIABLE BINARY DATA TYPE */
            *i_str = *in_str[0];
          
            /* MOVE BEGINNING OF INPUT STRING PAST BYTE ZERO */
            *in_str = *in_str + 1;
  
            /* TERMINATE DIGIT STRING WITH NULL CHAR */
            i_str[1] = NC;

            /* SET FIELD LENGTH COUNT */
            b_length = atoi(i_str);
            
            /* SET STRING TO ISTR */
            strcpy(string,i_str);
         }
         /* ELSE RETURN FAILURE */
         else return(0);

         /* INITIALIZE POS */
         pos = 1;

         /* IF NOT END OF INPUT STRING */
         if (*in_str < in_str_end) {
         
            /* RETRIEVE BIT FIELD LENGTH DIGITS */
            memcpy(i_str,*in_str,b_length);

            /* MOVE BEGINNING OF INPUT STRING PAST FIELD LENGTH DIGITS */
            *in_str = *in_str + b_length;

            /* TERMINATE DIGIT STRING WITH NULL CHAR */
            i_str[b_length] = NC;
            
            /* MOVE STRING TO ISTR */
            strcat(string,i_str);

            /* UPDATE POS */
            pos = pos + b_length;
            
            /* SET L_WIDTH */
            l_width = atoi(i_str);
         }
         /* ELSE RETURN FAILURE */
         else return(0);

         /* CALL G123BSTR() TO RETRIEVE DATA STRING */
         if (!g123bstr(in_str,comprssd,&string[pos],&l_width)) return(0);

         /* RESET BIT FIELD STORAGE */
         cur_fm->bit_cnt = 0;

         /* UPDATE L_WIDTH TO TOTAL NUMBER OF BYTES IN STRING */
         l_width += pos;

         /* SET WIDTH TO L_WIDTH */
         *width = l_width;

      }   

   }

   /* RETURN SUCCESS */
   return(1);
}
