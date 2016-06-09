# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: DEL123DIM
**
**    PURPOSE: TO DELETE A DIMENSION LENGTHS STRUCTURE
**
**    INVOCATION METHOD: DEL123DIM(DM_PTR)
**
**    ARGUMENT LIST:
**     NAME         TYPE      USE      DESCRIPTION
**     DM_PTR       PTR        I       POINTER TO ENTRY IN DIMENSION LENGTHS
**                                      STRUCTURE TO BE DELETED
**     DEL123DIM()  LOGICAL    O       DIMENSION LENGTHS STRUCTURE POINTER
**
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES: 
**     NAME      TYPE       DESCRIPTION
**     TMP_PTR   PTR        TEMPORARY STRUCTURE POINTER 
**  
**    GLOBAL REFERENCES:
**
**     DIMENSION LENGTHS STRUCTURE (DM)
**      NAME            TYPE   USE   DESCRIPTION
**      LEN             INT    N/A   DIMENSION LENGTH
**      NXT             PTR     I    POINTER TO NEXT DIMENSION LENGTH    
**                                    STRUCTURE
**
**    GLOBAL VARIABLES: NONE
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 05/27/90  INITIAL PROLOG
**     A. DEWITT                 05/27/90  INITIAL PDL
**     A. DEWITT                 05/28/90  INITIAL CODE
**     L. MCMILLION  TASK #40    12/22/92  UPDATED PROLOG/PDL/INLINE COMMENTS
**                                          PER QC
**
**    PDL:
**
**      WHILE DM_PTR IS NOT NULL DO
**         SET TMP_PTR TO DM_PTR
**         SET DM_PTR TO DM_PTR NEXT
**         SET TMP_PTR NEXT TO NULL
**         FREE TMP_PTR
**      END WHILE
**
**     RETURN NULL POINTER
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

struct dm * del123dim(struct dm *dm_ptr)

{
   /* DECLARATION LOCAL VARIABLE */
   struct dm *tmp_ptr;
 
   /* WHILE DM_PTR IS NOT NULL DO */
   while (dm_ptr != NULL) {
 
      /* SET TMP_PTR TO DM_PTR */
      tmp_ptr = dm_ptr;
     
      /* SET DM_PTR TO DM_PTR NEXT */
      dm_ptr = dm_ptr->nxt;
  
      /* SET TMP_PTR NEXT TO NULL */
      tmp_ptr->nxt = NULL;
  
      /* FREE TMP_PTR */
      free(tmp_ptr);
   };

   /* RETURN NULL POINTER */
   return NULL;
}  
