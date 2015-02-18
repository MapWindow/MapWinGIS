# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: END123REC
**
**    PURPOSE: TO COMPLETE THE PROCESSING OF A DATA RECORD AND FREE
**              ITS SPACE
**
**    INVOCATION METHOD: END123REC(FP)
**
**    ARGUMENT LIST:
**     NAME         TYPE      USE      DESCRIPTION
**     FP           PTR        I       FILE POINTER
**     END123REC()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     DEL123DIM()      DELETES DIMENSION LENGTHS STRUCTURE
**     DEL123DRVALS()   DELETES DR DATA VALUE SUBFIELD STRUCTURE
**     FREE123LAB()     FREES THE LABELS POINTER STRUCTURE 
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**     STC123EMPTY()    DETERMINES IF CURRENT DR STRUCTURE IS EMPTY
**     ULD123REC()      UNLOADS DATA RECORD FROM STRUCTURE AND WRITE IT TO
**                       A FILE
**
**    INTERNAL VARIABLES: 
**     NAME       TYPE         DESCRIPTION
**     CUR_DR     PTR          POINTER TO CURRENT DATA RECORD STRUCTURE 
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
**      LABELS          PTR     I    HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT     O    NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR    I/O   HEAD POINTER TO DIMENSION LENGTHS
**                                    (NO LABELS)
**      VALUES          PTR    I/O   HEAD POINTER TO DATA VALUE SUBFIELD
**                                    RECORDS
**      NEXT            PTR    I/O   POINTER TO NEXT DATA RECORD 
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR    N/A   FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR    I    OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                    STRUCTURE
**      DR_HD           PTR    I/O   HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR    I/O   HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE ENTRY
**      CUR_DM          PTR     O    CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY
**      CUR_DR          PTR    I/O   CURRENT POINTER TO DATA RECORD STRUCTURE
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
**      NLD             INT     I    NO LEADER FLAG
**                                    TRUE  - NO LEADER
**                                    FALSE - LEADER EXISTS
**      BIT_CNT         INT    N/A   COUNT OF BITS STORED IN MEMORY FOR 
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
**     DATA RECORD LEADER STRUCTURE (RL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT    N/A   DATA RECORD LENGTH     
**      RESV_SP         CHAR   N/A   RESERVED SPACE CHARACTER
**      LEAD_ID         CHAR   N/A   LEADER IDENTIFIER
**      S_RESV[6]       CHAR   N/A   RESERVED SPACE
**      DA_BASE         INT    N/A   BASE ADDRESS OF DATA AREA  
**      R_SPACE[4]      CHAR   N/A   RESERVED SPACE CHARACTERS
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH  
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION 
**      SP_RSRV         INT    N/A   RESERVED DIGIT
**      S_TAG           INT    N/A   SIZE OF FIELD TAG 
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR    I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/18/90  INITIAL PROLOG
**     P. HODGES                 05/18/90  INITIAL PDL
**     A. DEWITT                 05/27/90  MODIFY PDL - ACCOUNT FOR REPETITIVE
**                                          CHARACTERISTICS OF SUBSEQUENT DR
**                                          LEADERS AND DIRECTORIES
**     A. DEWITT                 05/27/90  INITIAL CODE
**     A. DEWITT                 08/21/90  MODIFY CODE - CALL TO DELETE 
**                                          STRUCTURE ROUTINES TO REFLECT 
**                                          FUNCTION PROTOTYPE CHANGES
**     L. MCMILLION              12/12/90  ADDED LOGIC TO FREE LABELS POINTER
**                                          STRUCTURE
**     L. MCMILLION              12/20/90  ADDED LOGIC TO FREE DATA RECORD
**                                          LEADER STRUCTURE
**     A. DEWITT                 02/27/91  ADDED CALL TO STC123EMPTY BEFORE
**                                          CALL TO UNLOAD RECORD
**     L. MCMILLION  TASK #40    12/21/92  UPDATED PROLOG/PDL/INLINE COMMENTS
**                                          PER QC
**
**    PDL:
**
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LEVEL
**     IF DR_HD NOT NULL
**     THEN
**        IF FILE IN WRITE MODE AND STRUCTURE NOT EMPTY
**        THEN
**           CALL ULD123REC() TO WRITE RECORD TO FILE
**        ENDIF
**        SET CUR_DM TO NULL
**        IF NO LEADER FLAG IS SET
**        THEN
**           SET CUR_DR TO DR_HD
**           WHILE CUR_DR IS NOT NULL DO
**             SET NUM_DIM TO ZERO
**             IF DIMENSIONS LENGTHS PRESENT
**             THEN
**                CALL DEL123DIM() TO DELETE DIMENSION LENGTH STRUCTURE
**             ENDIF
**             IF VALUES PRESENT
**             THEN
**                CALL DEL123DRVALS() TO DELETE VALUES DATA STRUCTURE
**             ENDIF
**             SET CUR_DR TO CUR_DR NEXT
**           END WHILE
**           SET CUR_DR TO DR_HD
**        ELSE
**           IF RL_HD NOT NULL
**           THEN
**              FREE RL_HD
**           ENDIF
**           SET CURRENT DR TO DR_HD
**           SET DR_HD TO NULL
**           WHILE DR_HD IS NOT NULL DO
**              SET LOCAL CUR_DR TO DR_HD
**              SET FM CUR_DR TO NEXT
**              SET LOCAL CUR_DR TO NULL
**              IF DIMENSION LENGTHS PRESENT
**              THEN
**                 CALL DEL123DIM() TO DELETE DIMENSION LENGTH STRUCTURE
**              ENDIF
**              IF VALUES PRESENT
**              THEN
**                 CALL DEL123DRVALS() TO DELETE VALUES STRUCTURE
**              ENDIF
**              FREE CUR_DR
**           END WHILE
**           SET FM CUR_DR TO NULL
**           IF LABELS ARE PRESENT
**           THEN
**              CALL FREE123LAB() TO FREE LABELS POINTER STRUCTURE
**           ENDIF
**        ENDIF
**     ENDIF
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/

#include "stc123.h"

int end123rec(FILE *fp)

{
 
   /* DECLARATION OF LOCAL-GLOBAL VARIABLES */
   struct dr *cur_dr;

   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LEVEL */
   if (!get123level(fp)) return(0);
 
   /* IF DR_HD NOT NULL */
   if (cur_fm->dr_hd != NULL) {
   
      /* IF FILE IN WRITE MODE AND DATA VALUES STRUCTURE NOT EMPTY */
      if ((cur_fm->open_mode == 'w') && (!stc123empty())) {

         /* CALL ULD123REC() TO WRITE RECORD TO FILE */
         if (!uld123rec()) return(0);
      };
 
      /* SET CUR_DM TO NULL */
      cur_fm->cur_dm = NULL;
      
      /* IF NO LEADER FLAG IS SET */
      if (cur_fm->nld)  { 

         /* SET CUR_DR TO DR_HD */
         cur_dr = cur_fm->dr_hd->next;

         /* WHILE CUR_DR IS NOT NULL DO */
         while (cur_dr != NULL) {

            /* SET NUM_DIM TO ZERO */
            cur_dr->num_dim = 0;

            /* IF DIMENSION LENGTHS PRESENT */
            if (cur_dr->dim_lptr != NULL ) {
            
               /* CALL DEL123DIM() TO DELETE DIMENSION LENGTH STRUCTURE */
               cur_dr->dim_lptr = del123dim(cur_dr->dim_lptr);
            };
           
            /* IF DATA VALUES PRESENT */
            if (cur_dr->values != NULL) {
            
               /* CALL DEL123DRVALS() TO DELETE VALUES DATA STRUCTURE */
               cur_dr->values = del123drvals(cur_dr->values);
            };
            
            /* SET CUR_DR TO CUR_DR NEXT */
            cur_dr = cur_dr->next;
         };

         /* INITALIZE CUR_DR TO DR_HD */
         cur_fm->cur_dr = cur_fm->dr_hd;
         
      }    
      else {
      
         /* IF RL_HD NOT NULL */
         if (cur_fm->rl_hd != NULL) {
         
            /* FREE RL_HD */
            free(cur_fm->rl_hd);
            cur_fm->rl_hd = NULL;
         }   
         
         /* SET CURRENT DR TO DR_HD */
         cur_fm->cur_dr = cur_fm->dr_hd;
   
         /* SET DR_HD POINTER TO NULL */
         cur_fm->dr_hd = NULL;
              
         /* WHILE FM CUR_DR IS NOT NULL DO */
         while (cur_fm->cur_dr != NULL) {
      
            /* SET CUR_DR TO FM CUR_DR */
            cur_dr = cur_fm ->cur_dr ;
      
            /* SET FM CUR_DR TO NEXT */
            cur_fm->cur_dr = cur_fm->cur_dr->next;
      
            /* SET NEXT FIELD OF CUR_DR TO NULL */
            cur_dr->next = NULL;
            
            /* IF DIMENSION LENGTHS PRESENT */
            if (cur_dr->dim_lptr != NULL ) {
            
               /* CALL DEL123DIM() TO DELETE DIMENSION LENGTH STRUCTURE */
               cur_dr->dim_lptr = del123dim(cur_dr->dim_lptr);
            };
            
            /* IF DATA VALUES PRESENT */
            if (cur_dr->values != NULL )  {
            
               /* CALL DEL123DRVALS() TO DELETE VALUES STRUCTURE */
               cur_dr->values = del123drvals(cur_dr->values);
            };
            
            /* FREE CUR_DR */
            free(cur_dr);
         };  
         
         /* SET FM CUR_DR TO NULL */
         cur_fm->cur_dr = NULL;
         
         /* IF LABELS ARE PRESENT */
         if (cur_fm->cur_dd->labels != NULL) {
         
            /* CALL FREE123LAB() TO FREE LABELS POINTER STRUCTURE */
            if (!free123lab()) return(0);
         };
      };
   };   

   /* RETURN SUCCESS */
   return(1);
} 
