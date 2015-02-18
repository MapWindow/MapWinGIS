# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: BEG123DDREC
**
**    PURPOSE: TO INSERT HEADER STRUCTURE FOR DDR DATA STRUCTURE
**              AND INITIALIZE HEAD POINTERS AND STATUS FLAGS
**
**    INVOCATION METHOD: BEG123DDREC (FP)
**
**    ARGUMENT LIST:
**     NAME           TYPE      USE      DESCRIPTION
**     FP             PTR        I       FILE POINTER
**     BEG123DDREC()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     DEL123DIM()      DELETES DIMENSIONS LENGTHS TREE
**     DEL123FMT()      DELETES FORMAT TREE
**     DEL123LABS()     DELETES LABELS TREE
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER
**
**    INTERNAL VARIABLES:
**     NAME       TYPE        DESCRIPTION
**     NEW_DD     PTR         POINTER TO DATA DESCRIPTIVE RECORD STRUCTURE
**     TMP_PTRL   PTR         POINTER TO DATA DESCRIPTIVE TAG PAIR STRUCTURE
**
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE FILE CONTROL RECORD STRUCTURE (CR)
**      NAME            TYPE   USE   DESCRIPTION
**      F_TITLE[]       PTR    I/O   FILE TITLE
**      TAG_L           PTR    I/O   POINTER TO LIST OF TAG PAIR RECORDS
**      U_AFD[]         PTR    I/O   CHARACTER STRING POINTER TO USERS 
**                                    AUGMENTED FILE DESCRIPTION
**
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE  DESCRIPTION
**      TAG[10]         CHAR    O   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     O   LENGTH OF DATA DESCRIPTIVE AREA RECORD 
**      FD_POS          INT     O   POSITION OF DATA DESCRIPTIVE AREA 
**                                   FIELD RECORD 
**      FD_CNTRL[10]    CHAR    O   FIELD CONTROLS
**      NAME[]          PTR     O   CHARACTER STRING POINTER TO NAME 
**                                   (OPTIONAL TITLE OF DATA FIELD)
**      NUM_DIM         INT     O   NUMBER OF DIMENSIONS IN ARRAY DESCRIPTOR
**      DIM_LPTR        PTR    I/O  HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT     O   NUMBER OF ELEMENTS IN PRIMARY DIMENSION
**      LABELS          PTR    I/O  HEAD POINTER TO A LINKED LIST CONTAINING 
**                                   LINKED LIST OF DDR LABEL SUBFIELD
**                                   STRUCTURES
**      FMT_RT          PTR    I/O  ROOT POINTER TO FORMAT 
**                                   CONTROLS BINARY TREE STRUCTURE
**      NEXT            PTR    I/O  POINTER TO NEXT DATA DESCRIPTIVE STRUCTURE
**      
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT    N/A   DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT    N/A   INTERCHANGE LEVEL
**      LEAD_ID         CHAR   N/A   LEADER IDENTIFIER 
**      ICE_IND         CHAR   N/A   INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR   N/A   RESERVE SPACE CHARACTER
**      APPL_IND        CHAR   N/A   APPLICATION INDICATOR
**      FD_CNTRL_L      INT    N/A   FIELD CONTROL LENGTH
**      DA_BASE         INT    N/A   BASE ADDRESS OF DESCRIPTIVE AREA 
**      CCS_IND[4]      CHAR   N/A   CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH 
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION  
**      S_RESV          INT    N/A   RESERVED DIGIT
**      S_TAG           INT    N/A   SIZE OF FIELD TAG 
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR    N/A   FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR    I    OPEN MODE OF FILE
**      CR_HD           PTR    I/O   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR    I/O   HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR    I/O   HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                    STRUCTURE
**      DR_HD           PTR    N/A   HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR    I/O   CURRENT POINTER TO DATA DESCRIPTIVE
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
**      NLD             INT     O    NO LEADER FLAG
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
**     DATA DESCRIPTIVE TAG PAIR STRUCTURE (TL) 
**      NAME            TYPE   USE   DESCRIPTION
**      TAG_1[10]       CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      TAG_2[10]       CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      NEXT            PTR    I/O   POINTER TO DATA DESCRIPTIVE TAG PAIR
**                                    STRUCTURE
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR    I/O   CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS: 
**     NAME             TYPE         DESCRIPTION
**     NC               CHAR         NULL CHARACTER
**     FCDSTYPE         INT          RELATIVE POSITION OF THE DATA STRUCTURE
**                                    TYPE WITHIN THE FIELD CONTROL
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     L. MCMILLION              05/31/90  INITIAL CODE
**     A. DEWITT                 08/22/90  MODIFIED CALL TO DEL123FMT() AND
**                                          DEL123LABS() DUE TO PROTOTYPE
**                                          CHANGES
**     L. MCMILLION              12/19/90  ADDED LOGIC TO RELEASE DATA
**                                          DESCRIPTIVE FILE CONTROL STRUCTURE
**                                          AND DATA DESCRIPTIVE LEADER
**                                          STRUCTURE
**     L. MCMILLION  (TASK #40)  08/27/91  ADDED LOGIC TO RETURN FAILURE IF
**                                          DYNAMIC MEMORY ALLOCATION FAILED
**     L. MCMILLION  TASK #40    11/20/92  PROLOG/PDL UPDATED PER QC
**     L. MCMILLION  93DR033     06/16/93  ADDED LOGIC TO REMOVE ARRAY
**                                          DIMENSIONS LENGTHS
**
**    PDL:
**
**     REWIND FILE
**     { NEED TO CLEAR OUT ANY DD INFORMATION PREVIOUSLY ALLOCATED }
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER
**     IF CR_HD IS NOT NULL
**     THEN
**        IF CR_HD FILE TITLE EXISTS
**        THEN
**           RELEASE FILE TITLE
**        ENDIF
**        IF CR_HD USER AUGMENTED FILE DESCRIPTION EXISTS
**        THEN
**           RELEASE USER AUGMENTED FILE DESCRIPTION
**        ENDIF
**        IF CR_HD TAG PAIR LIST EXISTS
**        THEN
**           WHILE TAG LIST NOT NULL DO
**              SET TMP_PTRL TO TAG LIST
**              SET TAG LIST TO TAG LIST NEXT
**              RELEASE TMP_PTRL
**           ENDWHILE
**        ENDIF
**        RELEASE CR_HD
**     ENDIF
**     IF OPEN MODE IS READ
**     THEN
**        IF DL_HD IS NOT NULL
**        THEN
**           RELEASE DL_HD
**        ENDIF
**     ENDIF
**     IF DD_HD IS NOT NULL
**     THEN
**        SET CUR_DD TO DD_HD
**        WHILE CUR_DD NOT NULL DO
**           SET DD_HD TO CUR_DD NEXT
**           CALL DEL123DIM() TO DELETE DIMENSIONS LENGTH TREE
**           CALL DEL123FMT() TO DELETE FORMAT TREE
**           CALL DEL123LABS() TO DELETE LABELS TREE
**           SET ALL CHARACTER STRINGS TO NULL
**           RELEASE CUR_DD
**           SET CUR_DD TO DD HEAD
**        ENDWHILE
**     ENDIF
**
**     ALLOCATE NEW_DD {DUMMY}
**     SET TAG TO NULL STRING
**     SET FD_POS TO NEGATIVE ONE
**     SET FD_LEN TO NEGATIVE ONE
**     SET FIELD CONTROL TO NULL STRING
**     SET NUMBER OF DIMENSIONS AND PRIMARY DIMENSION TO ZERO
**     SET POINTERS TO NULL
**     SET DD_HD TO NEW_DD
**     SET CURRENT DD POINTER TO DD HEAD 
**     SET NLD TO FALSE
**
**     RETURN SUCCESS
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int beg123ddrec(FILE *fp)
{
   /* LOCAL VARIABLES */
   struct dd *new_dd;
   struct tl *tmp_ptrl;
   
   /* REWIND FILE */
   rewind(fp);

   /* RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */
   if (!get123level(fp)) return(0);

   /* IF CR_HD IS NOT NULL */
   if (cur_fm->cr_hd != NULL) {
   
      /* IF CR_HD FILE TITLE EXISTS */
      if (cur_fm->cr_hd->f_title != NULL) {
      
         /* RELEASE FILE TITLE */
         free(cur_fm->cr_hd->f_title);
         cur_fm->cr_hd->f_title = NULL;
      }
      
      /* IF CR_HD USER AUGMENTED FILE DESCRIPTION EXISTS */
      if (cur_fm->cr_hd->u_afd != NULL) {
      
         /* RELEASE USER AUGMENTED FILE DESCRIPTION */
         free(cur_fm->cr_hd->u_afd);
         cur_fm->cr_hd->u_afd = NULL;
      }   

      /* IF CR_HD TAG PAIR LIST EXISTS */
      if (cur_fm->cr_hd->tag_l != NULL) {
      
         /* WHILE TAG LIST NOT NULL DO */
         while (cur_fm->cr_hd->tag_l != NULL) {
         
            /* SET TMP_PTRL TO TAG LIST */
            tmp_ptrl = cur_fm->cr_hd->tag_l;
            
            /* SET TAG LIST TO TAG LIST NEXT */
            cur_fm->cr_hd->tag_l = cur_fm->cr_hd->tag_l->next;
            
            /* RELEASE TMP_PTRL */
            free(tmp_ptrl);
         }   
      }

      /* RELEASE CR_HD */
      free(cur_fm->cr_hd);
      cur_fm->cr_hd = NULL;
   }   

   /* IF OPEN MODE IS READ */
   if (cur_fm->open_mode == 'r') {
   
      /* IF DL_HD IS NOT NULL */
      if (cur_fm->dl_hd != NULL) {
   
         /* RELEASE DL_HD */
         free(cur_fm->dl_hd);
         cur_fm->dl_hd = NULL;
      }
   }
   
   /* IF DD_HD IS NOT NULL */
   if (cur_fm->dd_hd != NULL) {

      /* SET CUR_DD TO DD_HD */
      cur_fm->cur_dd = cur_fm->dd_hd;

      /* WHILE CUR_DD NOT NULL DO */
      while (cur_fm->cur_dd != NULL) {

         /* SET DD_HD TO CUR_DD NEXT */
         cur_fm->dd_hd = cur_fm->cur_dd->next;

         /* DELETE DIMENSION LENGTHS TREE */
         cur_fm->cur_dd->dim_lptr = del123dim(cur_fm->cur_dd->dim_lptr);

         /* DELETE FORMAT TREE */
         cur_fm->cur_dd->fmt_rt = del123fmt(cur_fm->cur_dd->fmt_rt);

         /* DELETE LABELS TREE */
         cur_fm->cur_dd->labels = del123labs(cur_fm->cur_dd->labels);

         /* SET ALL CHARACTER STRINGS TO NULL */
         cur_fm->cur_dd->tag[0] = NC;
         cur_fm->cur_dd->fd_cntrl[FCDSTYPE] = NC;

         /* FREE NAME POINTER */
         free(cur_fm->cur_dd->name);

         /* RELEASE CUR_DD */
         free(cur_fm->cur_dd);

         /* SET CUR_DD TO DD_HD */
         cur_fm->cur_dd = cur_fm->dd_hd;
      }
   }

   /* ALLOCATE NEW_DD {DUMMY} */
   if ((new_dd = (struct dd *) malloc(sizeof(struct dd))) == NULL) return(0);

   /* SET TAG TO NULL STRING */
   new_dd->tag[0] = NC;

   /* SET FD_POS TO NEGATIVE ONE */
   new_dd->fd_pos = -1;

   /* SET FD_LEN TO NEGATIVE ONE */
   new_dd->fd_len = -1;

   /* SET FD_CNTRL TO NULL STRING */
   new_dd->fd_cntrl[FCDSTYPE] = NC;

   /* SET NUMBER OF DIMENSIONS AND PRIMARY DIMENSION TO ZERO */
   new_dd->num_dim = 0L;
   new_dd->prim_dms = 0L;

   /* SET POINTERS TO NULL */
   new_dd->name = NULL;
   new_dd->dim_lptr = NULL;
   new_dd->labels = NULL;
   new_dd->fmt_rt = NULL;
   new_dd->next = NULL;

   /* SET DD_HD TO NEW_DD */
   cur_fm->dd_hd = new_dd;

   /* SET CURRENT DD POINTER TO DD HEAD */
   cur_fm->cur_dd = cur_fm->dd_hd;

   /* SET NLD TO FALSE */
   cur_fm->nld = 0;

   /* RETURN SUCCESS */
   return(1);
}
