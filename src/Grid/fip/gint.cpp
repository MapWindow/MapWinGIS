# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: G123INT()
**
**    PURPOSE: TO READ A CHARACTER STRING REPRESENTING AN INTEGER VALUE AND
**             CONVERT THAT STRING TO INTEGER AND RETURN THE VALUE
**
**    INVOCATION METHOD: G123INT(FP, STR_LEN, INT_VAL)
**
**    ARGUMENT LIST:
**     NAME       TYPE      USE      DESCRIPTION
**     FP         PTR        I       FILE POINTER
**     STR_LEN    INT        I       LENGTH OF STRING TO BE READ IN
**     INT_VAL    INT        O       STRING CONVERTED TO INTEGER
**     G123INT()  LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     G123STR()        RETRIEVES A CHARACTER STRING
**
**    INTERNAL VARIABLES:
**     NAME        TYPE               DESCRIPTION
**     BUF_STR[12] CHAR               BUFFER CHARACTER STRING
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
**     L. MCMILLION              05/09/90  INITIAL CODE
**     L. MCMILLION  TASK #40    12/22/92  UPDATED PROLOG/PDL/INLINE COMMENTS
**                                          PER QC
**
**    PDL:
**
**     CALL G123STR() TO RETRIEVE STRING FROM FILE
**     CONVERT CHARACTER INTEGER VALUE TO INTEGER TYPE
**     STORE THE CONVERTED VALUE IN INT_VAL 
**
**     RETURN SUCCESS
**    
*****************************************************************************
**     CODE SECTION
**
*****************************************************************************/ 
#include "stc123.h"

int g123int(FILE *fp,long str_len,long *int_val)
{
   /* INTERNAL VARIABLES */
   char buf_str[12];

   /* CALL G123STR() TO RETRIEVE STRING FROM FILE */
   if (!g123str(fp,buf_str,str_len)) return(0);


   /* CONVERT CHARACTER INTEGER VALUE TO INTEGER TYPE AND STORE THE
       CONVERTED VALUE IN INT_VAL 
   */
   *int_val = atol(buf_str);

   /* RETURN SUCCESS */
   return(1);
}
