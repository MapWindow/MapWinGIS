# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: BEG123FILE
**
**    PURPOSE: TO OPEN A FILE FOR READING OR WRITING 
**
**    INVOCATION METHOD: BEG123FILE(F_NAME,MODE,INT_LEVEL,ICE,CCS,FP)
**
**    ARGUMENT LIST:
**     NAME          TYPE      USE      DESCRIPTION
**     F_NAME[]      PTR        I       FILE NAME
**     MODE          CHAR       I       MODE 'w' - write
**                                           'r' - read
**     INT_LEVEL     PTR       I/O      INTERCHANGE LEVEL
**     ICE[]         PTR       I/O      INLINE CODE EXTENSION INDICATOR
**     CCS[]         PTR       I/O      CODE CHARACTER SET INDICATOR
**     FP            PTR        O       FILE POINTER
**     BEG123FILE()  LOGICAL    O       OPEN SUCCESSFUL FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     G123INT()        READS AN INTEGER
**     G123STR()        READS A CHARACTER STRING
**
**    INTERNAL VARIABLES:
**     NAME          TYPE      DESCRIPTION
**     CUR_PTR       PTR       CURRENT POINTER TO FM STRUCTURE FOR LOCATING
**                              LAST ENTRY IN LINKED LIST
**     NEW_DL        PTR       NEW DDR LEADER STRUCTURE
**     NEW_FM        PTR       NEW FM STRUCTURE ENTRY
**     TMP_STR[20]   CHAR      TEMPORARY CHARACTER STRING
**
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT    N/A   DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT     O    INTERCHANGE LEVEL
**      LEAD_ID         CHAR   N/A   LEADER IDENTIFIER 
**      ICE_IND         CHAR    O    INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR   N/A   RESERVE SPACE CHARACTER
**      APPL_IND        CHAR   N/A   APPLICATION INDICATOR
**      FD_CNTRL_L      INT    N/A   FIELD CONTROL LENGTH
**      DA_BASE         INT    N/A   BASE ADDRESS OF DESCRIPTIVE AREA 
**      CCS_IND[4]      CHAR    O    CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH 
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION  
**      S_RESV          INT    N/A   RESERVED DIGIT
**      S_TAG           INT     O    SIZE OF FIELD TAG
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR     O    FILE POINTER
**      F_NAME[]        PTR     O    EXTERNAL FILE NAME
**      OPEN_MODE       CHAR    O    OPEN MODE OF FILE
**      CR_HD           PTR     O    HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR     O    HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR     O    HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                    STRUCTURE
**      DR_HD           PTR     O    HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR     O    HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR     O    HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR     O    HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR     O    HEAD POINTER TO RECORD POINTER STRUCTURE 
**      CUR_DD          PTR     O    CURRENT POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE ENTRY
**      CUR_DM          PTR     O    CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY
**      CUR_DR          PTR     O    CURRENT POINTER TO DATA RECORD STRUCTURE
**                                    ENTRY
**      CUR_DV          PTR     O    CURRENT POINTER TO DR DATA VALUE SUBFIELD
**                                    STRUCTURE ENTRY
**      ROW_DVH         PTR     O    CURRENT POINTER TO NEXT SET OF VALUES
**                                    IN DR DATA VALUE SUBFIELD STRUCTURE ENTRY
**      CUR_FC          PTR     O    CURRENT POINTER TO FORMAT CONTROLS
**                                    STRUCTURE ENTRY
**      CUR_LP          PTR     O    CURRENT POINTER TO LABELS POINTER
**                                    STRUCTURE ENTRY
**      CUR_SL          PTR     O    CURRENT POINTER TO DD-LABEL SUBFIELD
**                                    STRUCTURE ENTRY
**      CUR_FCR         PTR     O    CURRENT POINTER TO ROOT OF FORMAT
**                                    CONTROLS STRUCTURE
**      CUR_RP          PTR     O    CURRENT POINTER TO RECORD POINTER
**                                    STRUCTURE
**      NLD_RP          PTR     O    POINTER TO RECORD POINTER STRUCTURE
**                                    WHERE CORRESPONDING DR HAS AN 'R' 
**                                    LEADER IDENTIFIER    
**      SF_FMT          CHAR    O    FORMAT CORRESPONDING TO THE CURRENT
**                                    DATA VALUE
**      NLD             INT     O    NO LEADER FLAG
**                                    TRUE  - NO LEADER
**                                    FALSE - LEADER EXISTS
**      BIT_CNT         INT     O    COUNT OF BITS STORED IN MEMORY FOR 
**                                    A SUBSEQUENT READ FUNCTION
**      BIT_BIN         CHAR    O    BITS STORED IN MEMORY FOR SUBSEQUENT
**                                    READ FUNCTION
**      COMPRESS        INT     O    FLAG TO SPECIFY COMPRESSED OR
**                                    UNCOMPRESSED ADJACENT FIXED-LENGTH
**                                    BIT FIELD I/O
**                                    0 - UNCOMPRESSED
**                                    1 - COMPRESSED
**      SF_STATE_DD     INT     O    SUBFIELD STATE (DD)
**                                    1 - FIELD CONTROL SUBFIELD
**                                    2 - NAME SUBFIELD
**                                    3 - LABELS SUBFIELD
**                                    4 - FORMATS SUBFIELD
**                                    5 - FINISHED
**      SF_STATE_DR     INT     O    SUBFIELD STATE (DR)
**                                    1 - NUMBER OF DIMENSIONS SUBFIELD
**                                    2 - LENGTH OF A DIMENSION SUBFIELD
**                                    3 - DATA VALUE STRING SUBFIELD
**                                    4 - FINISHED
**      NEXT            PTR    I/O   POINTER TO NEXT FILE MANAGEMENT
**                                    STRUCTURE
**      BACK            PTR     O    POINTER TO PREVIOUS FILE MANAGEMENT
**                                    STRUCTURE
**
**     RECORD POINTER STRUCTURE (RP)
**      NAME            TYPE   USE   DESCRIPTION
**      DR_START        INT     O    FILE LOCATION OF FIRST BYTE OF DATA RECORD
**      NEXT            PTR     O    POINTER TO NEXT DR_START
**      BACK            PTR     O    POINTER TO BACK DR_START
**
**    GLOBAL VARIABLES:
**     NAME             TYPE     USE   DESCRIPTION
**     CUR_FM           PTR       O    CURRENT POINTER TO FILE MANAGEMENT
**                                      STRUCTURE ENTRY
**     FM_HD            PTR      I/O   HEAD POINTER TO LIST OF OPEN FILES
**     INIT_DONE        LOGICAL  I/O   INITIALIZATION FLAG OF FM STRUCTURE
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE           DESCRIPTION
**     MVS              INT            COMPILER DIRECTIVE FLAG
**     NC               CHAR           NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/08/90  INITIAL PROLOG
**     P. HODGES                 05/08/90  INITIAL PDL
**     L. MCMILLION              05/29/90  INITIAL CODE
**     L. MCMILLION              08/02/90  MOVED GLOBAL DECLARATIONS TO A NEW
**                                          INCLUDE FILE
**     A. DEWITT     92DR005     03/27/92  OPEN FILES IN BINARY MODE
**     J. TAYLOR     92DR005     05/20/92  ADDED INITIALIZATION OF COMPRESS FLAG
**     L. MCMILLION  TASK #40    11/20/92  PROLOG/PDL UPDATED PER QC; ADDED 
**                                          INITIALIZATION OF SF_FMT OF
**                                          FM STRUCTURE
**     J. TAYLOR     TASK #55    01/20/93  ADDED CONDITIONAL COMPILATION AROUND
**                                          FOPEN OF FILE OPENED FOR OUTPUT TO
**                                          ALLOW RECORD FORMAT SPECIFICATION
**                                          IN MVS
**     J. TAYLOR     TASK #55    03/16/93  MODIFIED RECORD FORMAT FOR MVS TO   
**                                          MATCH DBO
**     J. TAYLOR     TASK #55    04/05/93  MODIFIED RECORD FORMAT FOR MVS TO   
**                                          ALLOW RECORD FORMAT SPECIFICATION
**                                          TO BE SPECIFIED OUTSIDE OF FIPS123
**                                          LIBRARY
**
**    PDL:
**
**     IF MODE IS READ ONLY
**     THEN
**        SET MODE TO LOWERCASE
**        CALL FOPEN() TO OPEN FILE NAME FOR READ
**     ELSEIF MODE IS WRITE ONLY
**     THEN
**        SET MODE TO LOWERCASE
**        CALL FOPEN() TO OPEN FILE NAME FOR WRITE
**     ELSE
**        RETURN FAILURE
**     ENDIF
**
**     IF FM LINKED LIST HAS NOT BEEN INITIALIZED
**     THEN
**        SET FM_HD TO NULL
**        SET CUR_FM TO NULL
**        SET INITIALIZATION FLAG TO TRUE
**     ENDIF
**
**     IF NO ENTRIES IN FM LINKED LIST
**     THEN
**        ALLOCATE AND INSERT DUMMY
**     ENDIF
**
**     ALLOCATE NEW FM RECORD
**     SET FP IN NEW FM TO INPUT FP
**     SET FILE NAME IN NEW FM TO INPUT FILE NAME
**     SET OPEN MODE IN NEW FM TO INPUT MODE
**     ALLOCATE DUMMY HEAD POINTER TO RECORD LOCATION LIST
**     INITIALIZE DUMMY RECORD LOCATION STRUCTURE FIELDS 
**     INITIALIZE CUR_RP TO HEADER 
**     SET COMPRESS FLAG TO FALSE
**     INITIALIZE REMAINING NEW FM STRUCTURES
**     INSERT NEW FM RECORD INTO FM LINKED LIST
**     SET CUR_FM TO NEW_FM
**
**     IF READ MODE
**     THEN
**        READ AND DISCARD RECORD LENGTH
**        CALL G123INT() TO READ INTERCHANGE LEVEL
**        IF INTERCHANGE LEVEL NOT 1, 2, OR 3
**        THEN
**           RETURN FAILURE
**        ENDIF
**        READ AND DISCARD LEADER IDENTIFIER
**        CALL G123STR() TO READ INLINE CODE EXTENSION
**        READ AND DISCARD RESERVED SPACE, APPLICATION INDICATOR, FIELD
**         CONTROL INDICATOR, AND DATA BASE ADDRESS
**        CALL G123STR() TO READ CODE CHARACTER SET INDICATOR
**        REWIND FILE
**     ELSEIF WRITE MODE
**     THEN
**        ALLOCATE DATA DESCRIPTIVE LEADER
**        SET DL ILEVEL TO INPUT INT_LEVEL
**        SET DL ICE_IND TO INPUT ICE
**        SET DL CCS_IND TO INPUT CCS
**        SET DL S_TAG TO ZERO
**     ENDIF
**     SET CUR_FM TO FM_HD
**
**     RETURN SUCCESS
**     
*****************************************************************************
**      CODE SECTION
**
*****************************************************************************/
#include "stc123.h"
#include "gvd123.h"

int beg123file(char *f_name,int mode,long *int_level,char *ice,char *ccs,FILE **fp)
{
   /* LOCAL VARIABLES */
   struct fm *cur_ptr;
   struct fm *new_fm;
   struct dl *new_dl;
   char tmp_str[20];
   
   /* IF MODE IS READ ONLY, OPEN FILE FOR READ */
   if (mode == 'r' || mode == 'R') {
      mode = 'r';
          /* Fix 1.0.4  changed rb+ to only rb to not allow updates */
      if ((*fp = fopen(f_name,"rb")) == NULL) return(0);
   }
   
   /* ELSE IF MODE IS WRITE, OPEN FILE FOR WRITE */
   else if (mode == 'w' || mode == 'W') {
      mode = 'w';
#if MVS
      if ((*fp = fopen(f_name,"wb+,recfm=*")) 
          == NULL) return(0);
#else
      if ((*fp = fopen(f_name,"wb+")) == NULL) return(0);
#endif
   }

   else
      /* RETURN FAILURE */
      return(0);

   /* IF HAVE NOT INITIALIZED FM LINKED LIST */
   if (!(init_done)) {

      /* SET FM_HD AND CUR_FM TO NULL AND SET INITIALIZATION FLAG TO TRUE */
      fm_hd = NULL;
      cur_fm = NULL;
      init_done = 1;
   }

   /* IF NO ENTRIES IN FM LINKED LIST */
   if (fm_hd == NULL) {

      /* ALLOCATE NEW FILE MANAGEMENT STRUCTURE */
      if ((new_fm = (struct fm *) malloc(sizeof(struct fm))) == NULL)
       return(0);

      /* INSERT DUMMY TO ALLOCATED STRUCTURE */
      new_fm->fp = NULL;
      if ((new_fm->f_name = (char *) malloc((size_t)2*sizeof(char))) == NULL)
       return(0);
      *new_fm->f_name = NC;
      new_fm->open_mode = NC;
      new_fm->cr_hd = NULL;
      new_fm->dd_hd = NULL;
      new_fm->dl_hd = NULL;
      new_fm->dr_hd = NULL;
      new_fm->lp_hd = NULL;
      new_fm->rl_hd = NULL;
      new_fm->rs_hd = NULL;
      new_fm->rec_loc_hd = NULL;
      new_fm->cur_dd = NULL;
      new_fm->cur_dm = NULL;
      new_fm->cur_dr = NULL;
      new_fm->cur_dv = NULL;
      new_fm->row_dvh = NULL;
      new_fm->cur_fc = NULL;
      new_fm->cur_lp = NULL;
      new_fm->cur_sl = NULL;
      new_fm->cur_fcr = NULL;
      new_fm->cur_rp = NULL;
      new_fm->nld_rp = NULL;
      new_fm->sf_fmt = NC;
      new_fm->nld = 0;
      new_fm->bit_cnt = 0;
      new_fm->bit_bin = 0;
      new_fm->compress = 0;
      new_fm->sf_state_dd = 0;
      new_fm->sf_state_dr = 0;
      new_fm->next = NULL;
      new_fm->back = NULL;
      fm_hd = new_fm;
   }

   /* ALLOCATE NEW FILE MANAGEMENT STRUCTURE */
   if ((new_fm = (struct fm *) malloc(sizeof(struct fm))) == NULL) return(0);

   /* SET FP IN NEW FM TO INPUT FP */
   new_fm->fp = *fp;

   /* SET FILE NAME IN NEW FM TO INPUT FILE NAME */
   new_fm->f_name = (char *) malloc((size_t)(_tcslen(f_name) + 1)*sizeof(char));
   if (new_fm->f_name == NULL) return(0);
   *new_fm->f_name = NC;
   strcpy(new_fm->f_name,f_name);

   /* SET OPEN MODE IN NEW FM TO INPUT MODE */
   new_fm->open_mode = mode;

   /* ALLOCATE DUMMY HEAD POINTER TO RECORD LOCATION LIST */
   if ((new_fm->rec_loc_hd = (struct rp *) malloc(sizeof(struct rp))) == NULL) return(0);
   
   /* INITIALIZE DUMMY RECORD LOCATION STRUCTURE FIELDS */
   new_fm->rec_loc_hd->next = NULL;
   new_fm->rec_loc_hd->back = NULL;
   new_fm->rec_loc_hd->dr_start = 0L;
   
   /* INITIALIZE CUR_RP TO HEADER */
   new_fm->cur_rp = new_fm->rec_loc_hd;

   /* SET COMPRESS FLAG TO FALSE */
   new_fm->compress = 0;

   /* INITIALIZE REMAINING MEMBERS NEW FM STRUCTURE */
   new_fm->cr_hd = NULL;
   new_fm->dd_hd = NULL;
   new_fm->dl_hd = NULL;
   new_fm->dr_hd = NULL;
   new_fm->lp_hd = NULL;
   new_fm->rl_hd = NULL;
   new_fm->rs_hd = NULL;
   new_fm->cur_dd = NULL;
   new_fm->cur_dm = NULL;
   new_fm->cur_dr = NULL;
   new_fm->cur_dv = NULL;
   new_fm->row_dvh = NULL;
   new_fm->cur_fc = NULL;
   new_fm->cur_lp = NULL;
   new_fm->cur_sl = NULL;
   new_fm->cur_fcr = NULL;
   new_fm->nld_rp = NULL;
   new_fm->sf_fmt = NC;
   new_fm->nld = 0;
   new_fm->bit_cnt = 0;
   new_fm->bit_bin = 0;
   new_fm->sf_state_dd = 0;
   new_fm->sf_state_dr = 0;
   new_fm->next = NULL;
   new_fm->back = NULL;

   /* INSERT NEW FM RECORD INTO FM LINKED LIST */
   cur_ptr = fm_hd->next;
   fm_hd->next = new_fm;
   new_fm->back = fm_hd;
   new_fm->next = cur_ptr;
   if (cur_ptr != NULL) cur_ptr->back = new_fm;

   /* SET CUR_FM TO NEW_FM */
   cur_fm = new_fm;

   /* IF READ MODE */
   if (mode == 'r') {

      /* READ AND DISCARD RECORD LENGTH */
      if ((fread(tmp_str,sizeof(char),5,*fp)) != 5) return(0);

      /* CALL G123INT() TO READ INTERCHANGE LEVEL */
      if (!g123int(*fp,1L,int_level)) return(0);

      /* IF INTERCHANGE LEVEL NOT 1, 2, OR 3, RETURN FAILURE */
      if (*int_level != 1 && *int_level != 2 && *int_level != 3) return(0);

      /* READ AND DISCARD LEADER IDENTIFIER */
      if ((fread(tmp_str,sizeof(char),1,*fp)) != 1) return(0);

      /* CALL G123STR() TO READ INLINE CODE EXTENSION */
      if (!g123str(*fp,ice,1L)) return(0);

      /* READ AND DISCARD RESERVED SPACE, APPLICATION INDICATOR, FIELD CONTROL
          CONTROL INDICATOR, AND DATA BASE ADDRESS
      */
      if ((fread(tmp_str,sizeof(char),9,*fp)) != 9) return(0);

      /* CALL G123STR() TO READ CODE CHARACTER SET INDICATOR */
      if (!g123str(*fp,ccs,3L)) return(0);

      /* REWIND FILE */
      rewind(*fp);
   }
   /* ELSEIF WRITE MODE */
   else if (mode == 'w') {

      /* ALLOCATE DATA DESCRIPTIVE LEADER */
      if ((new_dl = (struct dl *) malloc(sizeof(struct dl))) == NULL)
       return(0);
      cur_fm->dl_hd = new_dl;

      /* SET DL ILEVEL TO INPUT INT_LEVEL */
      cur_fm->dl_hd->ilevel = *int_level;

      /* SET DL ICE_IND TO INPUT ICE */
      cur_fm->dl_hd->ice_ind = *ice;

      /* SET DL CCS_IND TO INPUT CCS */
      strcpy(cur_fm->dl_hd->ccs_ind,ccs);

      /* SET DL S_TAG TO ZERO */
      cur_fm->dl_hd->s_tag = 0;
   }

   /* SET CUR_FM TO FM_HD */
   cur_fm = fm_hd;

   /* RETURN SUCCESS */
   return(1);
}
