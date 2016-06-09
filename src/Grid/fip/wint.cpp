# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: W123INT
**
**    PURPOSE: TO WRITE OUT A INTEGER VALUE IN ASCII 
**
**    INVOCATION METHOD: W123INT(FP,I_VAL,O_LEN)
**
**    ARGUMENT LIST:
**     NAME       TYPE      USE      DESCRIPTION
**     FP         PTR        I       OUTPUT FILE POINTER
**     I_VAL      INT        I       INTEGER VALUE 
**     O_LEN      INT        I       OUTPUT LENGTH
**     W123INT()  LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     I123TOA()        CONVERTS AN INTEGER INTO A CHARACTER STRING
** 
**    INTERNAL VARIABLES:
**     NAME           TYPE        DESCRIPTION
**     INT_STR[10]    CHAR        TEMPORARY STRING TO HOLD ASCII INTEGER
**     L_DIF          INT         LENGTH DIFFERENCE
**     NUMZ           INT         CURRENT NUMBER OF ZEROS USED IN PADDING
**     S_LEN          INT         LENGTH OF ASCII INTEGER STRING
**     Z_PAD          CHAR        ZERO DIGIT PAD CHARACTER
**
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     L. MCMILLION              05/10/90  INITIAL CODE
**     J. TAYLOR     92DR005     05/16/92  MODIFIED FOR BIT FIELD WRITING
**     L. MCMILLION  TASK #40    12/16/92  UPDATED PROLOG PER QC
**
**    PDL:
**
**     CALL I123TOA TO CONVERT I_VAL TO ASCII 
**     CALL _strlen TO GET STRING LENGTH OF INT_STR
**     IF LEN OF INT_STR LESS THAN O_LEN AND O_LEN IS NOT ZERO
**     THEN
**        SET L_DIF TO DIFFERENCE
**        IF L_DIF IS NEGATIVE
**        THEN
**           RETURN FAILURE
**        ENDIF
**        DO FOR DIFFERENCE
**           WRITE ZERO
**        ENDDO
**     ENDIF
**     WRITE INT_STR
** 
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int w123int(FILE *fp,long i_val,long o_len)
{
   /* INTERNAL VARIABLES */
   char int_str[10],
        z_pad = '0';
   long l_dif,
        numz,
        s_len;

   /* CONVERT I_VAL TO ASCII */
   i123toa(i_val,int_str);

   /* GET STRING LENGTH OF INT_STR */
   s_len = _tcslen(int_str);

   /* IF LEN OF INT_STR LESS THAN O_LEN AND O_LEN NOT ZERO */
   if (s_len < o_len && o_len) {

      /* IF L_DIF IS NEGATIVE, RETURN FAILURE */
      if ((l_dif = o_len - s_len) < 0) return(0);

      /* DO FOR DIFFERENCE */
      for(numz = 0; numz < l_dif; numz++) {

         /* WRITE ZERO */
         fwrite(&z_pad,sizeof(char),1,fp);
      }
   }

   /* WRITE INT_STR */
   fwrite(int_str,sizeof(char),_tcslen(int_str),fp);

   /* RETURN SUCCESS */
   return(1);
}
