# include "stdafx.h"
/***************************************************************************
**
**     INVOCATION NAME: OUT123FMT
**     
**     PURPOSE:  TO WRITE FORMAT STRING TO OUTPUT FILE
**
**     INVOCATION METHOD: OUT123FMT(FP,CT_PTR)
**
**     ARGUMENT LIST: 
**      NAME        TYPE     USE      DESCRIPTION
**      FP          PTR       I       FILE POINTER TO A TEMPORARY FILE
**      CT_PTR      PTR       I       POINTER TO FORMAT CONTROL STRUCTURE
**      OUT123FMT() LOGICAL   O       SUCCESS FLAG
**
**     EXTERNAL FUNCTION REFERENCES:
**      NAME             DESCRIPTION
**      OUT123FMT()      FUNCTION TO WRITE THE FORMAT STRING TO OUTPUT FILE
**      W123INT()        FUNCTION TO WRITE AN INTEGER TO OUTPUT FILE
** 
**     INTERNAL VARIABLES: NONE
**
**     GLOBAL REFERENCES:
**
**      FORMAT CONTROLS STRUCTURE (FC)
**       NAME            TYPE   USE   DESCRIPTION
**       REP_NO          INT     I    NUMBER OF REPETITIONS
**       D_TYPE          CHAR    I    DATA TYPE - (A,I,R,S,C,B,X)
**       WIDTH           INT     I    FIELD WIDTH SPECIFICATION
**       U_DELIM         CHAR    I    USER DELIMITER
**       LEFT            PTR     I    LEFT POINTER TO FORMAT CONTROLS
**                                     STRUCTURE INDICATES REPETITION
**       RIGHT           PTR     I    RIGHT POINTER TO FORMAT CONTROLS
**                                     STRUCTURE INDICATES SAME LEVEL
**       BACK            PTR     I    BACK POINTER TO FORMAT CONTROLS
**                                     STRUCTURE
**
**     GLOBAL VARIABLES: NONE
**
**     GLOBAL CONSTANTS: NONE
** 
**     CHANGE HISTORY:
**      AUTHOR      CHANGE-ID     DATE     CHANGE SUMMARY
**      A. DEWITT               05/10/90   INITIAL PROLOG
**      A. DEWITT               05/10/90   INITIAL PDL
**      P. HODGES               06/05/90   INITIAL CODE
**      A. DEWITT               O7/11/90   MODIFIED LOGIC TO BACK UP FROM RIGHT
**                                          SUBTREE
**
**     PDL:
**
**      IF CT_PTR NOT NULL
**      THEN
**         IF REP_NO FIELD OF CT_PTR GREATER THAN ONE
**         THEN
**            CALL W123INT() TO WRITE REP_NO TO TEMP FILE
**         ELSEIF REP_NO EQUAL ZERO   { A FORMAT }
**            WRITE D_TYPE
**            IF WIDTH GREATER THAN ZERO
**            THEN
**               WRITE LEFT PARENTHESIS TO TEMP FILE
**               CALL W123INT() TO WRITE WIDTH TO TEMP FILE
**               WRITE RIGHT PARENTHESIS TO TEMP FILE
**            ELSEIF U_DELIM NOT NC { NULL CHARACTER }
**            THEN
**               WRITE LEFT PARENTHESIS TO TEMP FILE
**               WRITE U_DELIM TO TEMP FILE
**               WRITE RIGHT PARENTHESIS TO TEMP FILE
**            ENDIF
**         ENDIF
**         IF CT_PTR'S LEFT FIELD NOT NULL
**         THEN
**            IF REP_NO OF LEFT CHILD IS NOT ZERO
**            THEN
**               WRITE LEFT PARENTHESIS TO TEMP FILE
**            ENDIF
**         ENDIF
**
**         CALL OUT123FMT() TO PRINT LEFT SUBTREE
**
**         IF RIGHT FIELD OF CT_PTR NOT NULL
**         THEN
**            WRITE COMMA TO TEMP FILE
**         ENDIF
**         CALL OUT12FMT() TO PRINT RIGHT SUBTREE
**         IF NOT AT ROOT AND REPETITION NUMBER GREATER THAN OR EQUAL TO ONE
**          AND RIGHT IS NULL
**         THEN
**            WRITE RIGHT PARENTHESIS
**         ELSEIF CT_PTR'S RIGHT EQUALS NULL AND CT_PTR'S REP_NO IS ZERO
**         THEN
**            IF CURRENT NODE IS A RIGHT CHILD
**            THEN
**               WRITE RIGHT PARENTHESIS
**            ENDIF
**         ENDIF
**      ENDIF
**
**      RETURN SUCCESS
**
****************************************************************************
**      CODE SECTION
**
****************************************************************************/
#include "stc123.h"

int out123fmt(FILE *fp,struct fc *ct_ptr)
{
   /* IF CT_PTR NOT NULL */
   if (ct_ptr != NULL) {

      /* IF REP_NO FIELD OF CT_PTR GREATER THAN ONE */
      if (ct_ptr->rep_no > 1) {

         /* CALL W123INT() TO WRITE REP_NO TO TEMP FILE */
         if (!w123int(fp,ct_ptr->rep_no,0L)) return(0);
      }
      /* ELSEIF REP_NO EQUAL ZERO   { A FORMAT } */
      else if (!ct_ptr->rep_no) {

         /* WRITE D_TYPE */
         fprintf(fp,"%c",ct_ptr->d_type);

         /* IF WIDTH GREATER THAN ZERO */
         if (ct_ptr->width > 0) {

            /* WRITE LEFT PARENTHESIS TO TEMP FILE */
            fprintf(fp,"%c",'(');

            /* CALL W123INT() TO WRITE WIDTH TO TEMP FILE */
            if (!w123int(fp,ct_ptr->width,0L)) return(0);

            /* WRITE RIGHT PARENTHESIS TO TEMP FILE */
            fprintf(fp,"%c",')');

         }
         /* ELSEIF U_DELIM NOT NC { NULL CHARACTER } */
         else if (ct_ptr->u_delim != NC) {

            /* WRITE LEFT PARENTHESIS TO TEMP FILE */
            fprintf(fp,"%c",'(');

            /* WRITE U_DELIM TO TEMP FILE */
            fprintf(fp,"%c",ct_ptr->u_delim);

            /* WRITE RIGHT PARENTHESIS TO TEMP FILE */
            fprintf(fp,"%c",')');

         }
      }

      /* IF CT_PTR'S LEFT FIELD NOT NULL */
      if (ct_ptr->left != NULL) {

         /* IF REP_NO OF LEFT CHILD IS NOT ZERO */
         if (ct_ptr->left->rep_no) {

            /* WRITE LEFT PARENTHESIS TO TEMP FILE */
            fprintf(fp,"%c",'(');
         }
      }

      /* CALL OUT123FMT() TO PRINT LEFT SUBTREE */
      if (!out123fmt(fp,ct_ptr->left)) return(0);

      /* IF RIGHT FIELD OF CT_PTR NOT NULL */
      if (ct_ptr->right != NULL) {

         /* WRITE COMMA TO TEMP FILE */
         fprintf(fp,"%c",',');
      }

      /* CALL OUT123FMT() TO PRINT RIGHT SUBTREE */
      if (!out123fmt(fp,ct_ptr->right)) return(0);

      /* IF NOT AT ROOT AND REPETITION NUMBER GREATER THAN OR EQUAL TO ONE AND
          RIGHT IS NULL
      */
      if (ct_ptr->back != NULL && ct_ptr->rep_no >= 1 && ct_ptr->right == NULL) {

         /* WRITE RIGHT PARENTHESIS */
         fprintf(fp,"%c",')');
      }

      /* ELSEIF CT_PTR'S RIGHT EQUALS NULL AND CT_PTR'S REP_NO IS ZERO */
      else if (ct_ptr->right == NULL && !ct_ptr->rep_no) {

         /* IF CURRENT NODE IS A RIGHT CHILD */
         if (ct_ptr->back->right == ct_ptr) {

            /* WRITE RIGHT PARENTHESIS */
            fprintf(fp,"%c",')');
         }
      }
   }

   /* RETURN SUCCESS */
   return(1);
}
