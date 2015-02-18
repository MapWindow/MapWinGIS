# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: BEG123REC
**
**    PURPOSE: TO INSERT HEADER STRUCTURE FOR DR DATA STRUCTURE
**              AND INITIALIZE HEAD POINTERS. ALL NECESSARY OVERHEAD OF
**              DELETING PREVIOUS RECORD READ IS ALSO PERFORMED.
**
**    INVOCATION METHOD: BEG123REC(FP)
**
**    ARGUMENT LIST:
**     NAME         TYPE      USE      DESCRIPTION
**     FP           PTR        I       FILE POINTER
**     BEG123REC()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME              DESCRIPTION
**     END123REC()       ENDS DATA RECORD
**     GET123LEVEL()     RETRIEVES THE APPROPRIATE DATA STRUCTURE LAYER
**
**    INTERNAL VARIABLES:
**     NAME       TYPE        DESCRIPTION
**     NEW_DR     PTR         POINTER TO DATA RECORD STRUCTURE
**     NEW_RL     PTR         POINTER TO DATA RECORD LEADER STRUCTURE
**
**    GLOBAL REFERENCES:
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    O    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     O    LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT     O    POSITION OF DESCRIPTIVE AREA DATA
**                                   RECORD
**      NUM_DIM         INT     O    NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR     O    POINTER TO DIMENSION LENGTHS (NO LABELS)
**      VALUES          PTR     O    POINTER TO DATA VALUE SUBFIELD RECORDS
**      NEXT            PTR     O    POINTER TO NEXT DATA RECORD 
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
**      DR_HD           PTR    I/O   HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR     O    HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR    N/A   CURRENT POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE ENTRY
**      CUR_DM          PTR    N/A   CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY
**      CUR_DR          PTR     O    CURRENT POINTER TO DATA RECORD STRUCTURE
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
**      REC_LEN         INT     O    DATA RECORD LENGTH     
**      RESV_SP         CHAR    O    RESERVED SPACE CHARACTER
**      LEAD_ID         CHAR    O    LEADER IDENTIFIER
**      S_RESV[6]       CHAR    O    RESERVED SPACE
**      DA_BASE         INT     O    BASE ADDRESS OF DATA AREA  
**      R_SPACE[4]      CHAR    O    RESERVED SPACE CHARACTERS
**      S_FDLEN         INT     O    SIZE OF FIELD LENGTH  
**      S_FDPOS         INT     O    SIZE OF FIELD POSITION 
**      SP_RSRV         INT     O    RESERVED DIGIT
**      S_TAG           INT     O    SIZE OF FIELD TAG 
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION
**     BLNK_SP          CHAR         BLANK SPACE CHARACTER
**     NC               CHAR         NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/18/90  INITIAL PROLOG
**     P. HODGES                 05/18/90  INITIAL PDL
**     A. DEWITT                 05/27/90  INITIAL CODE
**     A. DEWITT                 11/03/90  ADD NLD LOGIC NOT TO
**                                          REALLOCATE DUMMY
**     L. MCMILLION              01/02/91  ADDED LOGIC TO ALLOCATE DATA RECORD
**                                          LEADER STRUCTURE
**     L. MCMILLION  TASK #40    12/17/92  UPDATED PROLOG PER QC
**
**    PDL:
**
**     CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER
**     CALL END123REC() TO END THE DATA RECORD
**
**     IF NO LEADER FLAG IS NOT SET 
**     THEN
**        ALLOCATE NEW_RL
**        SET CUR_FM RL_HD TO NEW_RL
**        ALLOCATE NEW_DR { DUMMY }
**        SET TAG TO NULL STRING
**        SET FD_POS TO ZERO
**        SET FD_LEN TO ZERO
**        SET NUM_DIM TO ZERO
**        SET POINTERS TO NULL
**        SET DR_HD TO NEW_DR
**        SET CUR_DR TO NEW_DR
**     ELSE
**        SET CUR_DR TO DR_HD
**     ENDIF
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
*****************************************************************************/
#include "stc123.h"

int beg123rec(FILE *fp)

{
   /* INTERNAL VARIABLES */
   struct dr *new_dr;
   struct rl *new_rl;
   
   /* CALL GET123LEVEL() TO RETRIEVE THE APPROPRIATE DATA STRUCTURE LAYER */  
   if (!get123level(fp)) return(0);

   /* CALL END123REC() TO END THE DATA RECORD */
   if (!end123rec(fp)) return(0);
   
   /* IF NO LEADER FLAG IS NOT SET */
   if (!cur_fm->nld) {
   
      /* ALLOCATE NEW_RL */
      if ((new_rl = (struct rl *) malloc(sizeof(struct rl))) == NULL)
       return(0);
      new_rl->rec_len = 0;
      new_rl->resv_sp = BLNK_SP;
      new_rl->lead_id = BLNK_SP;
      *new_rl->s_resv = NC;
      new_rl->da_base = 0;
      *new_rl->r_space = NC;
      new_rl->s_fdlen = 0;
      new_rl->s_fdpos = 0;
      new_rl->sp_rsrv = 0;
      new_rl->s_tag = 0;

      /* SET CUR_FM RL_HD TO NEW_RL */
      cur_fm->rl_hd = new_rl;
      
      /* ALLOCATE NEW_DR { DUMMY } */
      if ((new_dr = (struct dr *) malloc(sizeof(struct dr))) == NULL)
       return(0);
      
      /* SET TAG TO NULL STRING */
      new_dr->tag[0] = NC;
   
      /* SET FD_POS TO ZERO */
      new_dr->fd_pos = 0;
   
      /* SET FD_LEN TO ZERO */
      new_dr->fd_len = 0;
   
      /* SET NUM_DIM TO ZERO */
      new_dr->num_dim = 0;
   
      /* SET POINTERS TO NULL */
      new_dr->dim_lptr = NULL;
      new_dr->values = NULL;
      new_dr->next = NULL;
    
      /* SET DR_HD TO NEW_DR */
      cur_fm->dr_hd = new_dr;
   
      /* SET CUR_DR TO NEW_DR */
      cur_fm->cur_dr = new_dr;
      
   }
   else {
      
      /* SET CUR_DR TO DR_HD */
      cur_fm->cur_dr = cur_fm->dr_hd;
   }      
 
   /* RETURN SUCCESS */
   return(1);
}
