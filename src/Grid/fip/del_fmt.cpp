# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: DEL123FMT
**
**    PURPOSE: TO DELETE DDR FORMAT CONTROLS TREE STRUCTURE
**
**    INVOCATION METHOD: DEL123FMT(CT_PTR)
**
**    ARGUMENT LIST:
**     NAME         TYPE      USE      DESCRIPTION
**     CT_PTR       PTR        I       CURRENT TREE POINTER TO FORMAT CONTROL
**                                      STRUCTURE
**     DEL123FMT()  PTR        O       FORMAT CONTROLS STRUCTURE POINTER
**
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     DEL123FMT()      DELETE FORMAT CONTROLS SUBTREE
**
**    INTERNAL VARIABLES: NONE
**  
**    GLOBAL REFERENCES:
**
**     FORMAT CONTROLS STRUCTURE (FC)
**      NAME            TYPE   USE  DESCRIPTION
**      REP_NO          INT    N/A  NUMBER OF REPETITIONS
**      D_TYPE          CHAR   N/A  DATA TYPE - (A,I,R,S,C,B,X)
**      WIDTH           INT    N/A  FIELD WIDTH SPECIFICATION
**      U_DELIM         CHAR   N/A  USER DELIMITER
**      LEFT            PTR    I/O  LEFT POINTER TO FORMAT CONTROLS STRUCTURE
**                                   INDICATES REPETITION
**      RIGHT           PTR    I/O  RIGHT POINTER TO FORMAT CONTROLS STRUCTURE
**                                   INDICATES SAME LEVEL
**      BACK            PTR     O   BACK POINTER TO FORMAT CONTROLS STRUCTURE
**
**    GLOBAL VARIABLES: NONE
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     A. DEWITT                 05/28/90  INITIAL CODE
**     A. DEWITT                 08/02/90  REDECLARED FUNCTION AS A STRUCTURE
**                                          POINTER
**     L. MCMILLION  TASK #40    12/20/92  UPDATED PROLOG/PDL PER QC
**
**    PDL:
**
**     IF CT_PTR IS NOT NULL
**     THEN
**        CALL DEL123FMT() TO DELETE LEFT SUBTREE 
**        CALL DEL123FMT() TO DELETE RIGHT SUBTREE
**        SET BACK POINTER TO NULL
**        FREE CT_PTR
**     ENDIF
**     RETURN NULL POINTER
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

struct fc * del123fmt(struct fc *ct_ptr)

{
   /* IF CT_PTR IS NOT NULL */
   if (ct_ptr != NULL) {

      /* CALL DEL123FMT() TO DELETE LEFT SUBTREE */
      ct_ptr->left = del123fmt(ct_ptr->left);

      /* CALL DEL123FMT() TO DELETE RIGHT SUBTREE */
      ct_ptr->right = del123fmt(ct_ptr->right);

      /* SET BACK POINTER TO NULL */
      ct_ptr->back = NULL;

      /* FREE CT_PTR */
      free(ct_ptr);
   }

   /* RETURN NULL POINTER */
   return (NULL);
}
