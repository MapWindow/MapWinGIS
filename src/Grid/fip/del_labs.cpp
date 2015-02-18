# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: DEL123LABS
**
**    PURPOSE: TO DELETE DDR LABEL SUBFIELD STRUCTURE
**
**    INVOCATION METHOD: DEL123LABS(D_LAB)
**
**    ARGUMENT LIST:
**     NAME           TYPE      USE      DESCRIPTION
**     D_LAB          PTR        I       POINTER TO DATA DESCRIPTIVE LABEL
**                                        SUBFIELD STRUCTURE
**     DEL123LABS()   PTR        O       POINTER TO LABEL STRUCTURE
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     DEL123LABS()     DELETES LABEL STRUCTURE
**
**    INTERNAL VARIABLES: NONE
**  
**    GLOBAL REFERENCES:
**
**     DD LABEL SUBFIELD STRUCTURE (SL)
**      NAME            TYPE   USE   DESCRIPTION
**      LABEL[]         PTR    I/O   LABEL
**      NXT_LAB         PTR    I/O   POINTER TO NEXT LABEL IN SET
**      NXT_LSET        PTR    I/O   POINTER TO NEXT SET OF LABELS
**
**    GLOBAL VARIABLES: NONE
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     P. HODGES                 06/01/90  INITIAL CODE
**     A. DEWITT                 06/20/90  UPDATE CODE TO DECLARE 
**                                         FUNCTION TO OF TYPE POINTER
**     L. MCMILLION  TASK #40    12/20/92  UPDATED PROLOG/PDL PER QC
**
**    PDL:
**
**     IF D_LAB IS NOT NULL
**     THEN
**        CALL DEL123LABS() TO DELETE NEXT LABEL IN SET
**        CALL DEL123LABS() TO DELETE NEXT LABEL SET
**        IF LABEL PRESENT
**        THEN
**           FREE LABEL STRING
**        ENDIF
**        FREE D_LAB
**     ENDIF
**     RETURN NULL
**        
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

struct sl * del123labs(struct sl *d_lab)

{
   /* IF D_LAB IS NOT NULL */
   if (d_lab != NULL) {

      /* CALL DEL123LABS() TO DELETE NEXT LABEL IN SET */
      d_lab->nxt_lab = del123labs(d_lab->nxt_lab);

      /* CALL DEL123LABS() TO DELETE NEXT LABEL SET */
      d_lab->nxt_lset = del123labs(d_lab->nxt_lset);

      /* IF LABEL PRESENT */
      if (d_lab-> label != NULL) {

         /* FREE LABEL STRING */
         free(d_lab->label);
      }   
      
      /* FREE D_LAB */
      free(d_lab);
   }

   /* RETURN NULL VALUE FOR FREED NODE */
   return (NULL);
}
