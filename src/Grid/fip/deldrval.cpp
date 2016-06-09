# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: DEL123DRVALS
**
**    PURPOSE: TO DELETE A DR DATA VALUE SUBFIELD STRUCTURE
**
**    INVOCATION METHOD: DEL123DRVALS(VALPTR)
**
**    ARGUMENT LIST:
**     NAME            TYPE      USE      DESCRIPTION
**     VALPTR          PTR        I       POINTER TO ENTRY IN DR DATA VALUE
**                                         SUBFIELD STRUCTURE
**     DEL123DRVALS()  LOGICAL    O       DR DATA VALUE STRUCTURE POINTER
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES: 
**     NAME             DESCRIPTION
**     COL_PTR          COLUMN POINTER WITH TRAVERSE COLUMNS OF ROW
**     TRAV_PTR         TRAVERSE POINTER TEMPORARY POINTER 
**  
**    GLOBAL REFERENCES:
**
**     DR DATA VALUE SUBFIELD STRUCTURE (DV)
**      NAME            TYPE   USE   DESCRIPTION
**      VALUE[]         PTR    I/O   DATA VALUE
**      NXT_VSET        PTR    I/O   POINTER TO NEXT SET OF DATA VALUES
**      NXT_VAL         PTR    I/O   POINTER TO NEXT DATA VALUE SUBFIELD
**                                    RECORD
**    GLOBAL VARIABLES: NONE
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/18/90  INITIAL PROLOG
**     P. HODGES                 05/18/90  INITIAL PDL
**     A. DEWITT                 06/04/90  INITIAL CODE
**     A. DEWITT                 08/30/90  MODIFY CODE TO DELETE STRUCTURES
**                                          NONE RECURSIVELY TO PREVENT STACK
**                                          OVERFLOW
**     L. MCMILLION  TASK #40    12/20/92  UPDATED PROLOG/PDL/INLINE COMMENTS
**                                          PER QC
**
**    PDL:
**
**     IF VALPTR IS NOT NULL
**        WHILE VALPTR NOT NULL DO
**           SET COLUMN POINTER TO VALPTR
**           SET VALPTR TO NEXT VALUE SET FIELD
**           WHILE COLUMN POINTER NOT NULL DO
**              SET TRAVERSE POINTER TO COLUMN POINTER
**              SET COLUMN POINTER TO NEXT VALUE
**              SET TRAVERSE POINTER VALUES TO NULL
**              FREE VALUE CHARACTER POINTER OF TRAVERSE POINTER
**              FREE TRAVERSE POINTER
**           ENDWHILE
**        ENDWHILE 
**        RETURN NULL POINTER
**     ELSE
**        RETURN NULL POINTER
**     ENDIF
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

struct dv * del123drvals(struct dv *valptr)

{  
   /* LOCAL DECLARATIONS */
   struct dv *col_ptr ,
             *trav_ptr;
             
   /* IF VALPTR NOT NULL */
   if (valptr != NULL) {

      /* WHILE VALPTR NOT NULL DO */
      while (valptr != NULL) {

         /* SET COLUMN POINTER TO VALPTR */
         col_ptr = valptr;

         /* SET VALPTR TO NEXT VALUE SET FIELD */
         valptr = valptr->nxt_vset;

         /* WHILE COLUMN POINTER NOT NULL DO */
         while (col_ptr != NULL) {

            /* SET TRAVERSE POINTER TO COLUMN POINTER */
            trav_ptr = col_ptr;

            /* SET COLUMN POINTER TO NEXT VALUE */
            col_ptr = col_ptr->nxt_val;

            /* SET TRAVERSE POINTERS TO NULL */
            trav_ptr->nxt_vset = NULL;
            trav_ptr->nxt_val = NULL;

            /* IF VALUE OF TRAVERSE POINTER NOT NULL */
            if ( trav_ptr->value != NULL) {

               /* FREE VALUE OF TRAVERSE POINTER */
               free(trav_ptr->value);
            };

            /* FREE TRAVERSE POINTER */
            free(trav_ptr);

         /* ENDWHILE COLUMN POINTER NOT NULL */
         };

      /* ENDWHILE VALPTR NOT NULL */
      };

      /* RETURN NULL POINTER */
      return NULL;
   }

   /* ELSE VALPTR NULL */
   else {

      /* RETURN NULL POINTER */
      return NULL;
   };   

} 
