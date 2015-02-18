# include "stdafx.h"
/*****************************************************************************
**
**     INVOCATION NAME: LD123REC
**     
**     PURPOSE:  TO READ THE DATA RECORD LOADING THE INFORMATION
**                IN THE DATA STRUCTURES FOR REFERENCE
**
**     INVOCATION METHOD: LD123REC
**
**     ARGUMENT LIST: NONE
**     NAME         TYPE     USE   DESCRIPTION
**     LD123REC()   LOGICAL   O    SUCCESS FLAG
**
**     EXTERNAL FUNCTION REFERENCES:
**      NAME             DESCRIPTION
**      BEG123REC()      INSERTS HEADER STRUCTURE FOR DR DATA STRUCTURE
**                        AND INITIALIZES HEAD POINTERS
**      G123INT()        FUNCTION TO READ IN A INTEGER
**      G123STR()        FUNCTION TO READ IN A STRING
**      G123FSTR()       FUNCTION TO READ IN A FULL STRING
**      LOAD123FLD()     LOADS DR FIELD FROM FILE INTO DATA STRUCTURES
**      RET123MATCH()    FUNCTION RETURNS ASSOCIATED DATA DESCRIPTIVE RECORD
**                        STRUCTURE OF DR
**      VER123DRTAG()    VERIFIES DATA RECORD TAG ORDER
**
**     INTERNAL VARIABLES:
**      NAME            TYPE         DESCRIPTION
**      CH              INT          INTEGER REPRESENTATION OF A SINGLE
**                                    CHARACTER
**      COMPRSSD        INT          LOGICAL INDICATING WHETHER OR NOT
**                                    ADJACENT FIXED-LENGTH BINARY
**                                    SUBFIELDS ARE COMPRESSED WITHIN THE
**                                    FILE
**                                     0 - UNCOMPRESSED
**                                     1 - COMPRESSED
**      CUR_POS         INT          CURRENT FILE POINTER POSITION
**      CUR_SIZ         INT          CURRENT SIZE OF DR DIRECTORY 
**      DIR_SIZ         INT          SIZE OF DDR DIRECTORY
**      FLD_LEN         LONG         FIELD CHARACTER STRING LENGTH
**      FLD_STR[]       PTR          POINTER TO FIELD CHARACTER STRING
**      FLD_STR_END[]   PTR          POINTER TO END OF FIELD CHARACTER STRING
**      FP              PTR          FILE POINTER
**      NEW_DR          PTR          POINTER TO DATA RECORD
**      NEW_RP          PTR          POINTER TO RECORD POINTER STRUCTURE
**      ST_FLD_STR[]    PTR          POINTER TO SAVE START OF FIELD 
**                                    CHARACTER STRING
**
**    GLOBAL REFERENCES:
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
**      S_TAG           INT     I    SIZE OF FIELD TAG 
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   I/O   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    I/O   LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT    I/O   POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT     O    NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR     O    HEAD POINTER TO DIMENSION LENGTHS
**                                    (NO LABELS)
**      VALUES          PTR     O    HEAD POINTER TO DATA VALUE SUBFIELD
**                                    RECORDS
**      NEXT            PTR    I/O   POINTER TO NEXT DATA RECORD 
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR     I    FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR    I    OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                    STRUCTURE
**      DR_HD           PTR     I    HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR     I    HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE 
**      CUR_DD          PTR    N/A   CURRENT POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE ENTRY
**      CUR_DM          PTR    N/A   CURRENT POINTER TO DIMENSION LENGTHS
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
**      CUR_FCR         PTR     O    CURRENT POINTER TO ROOT OF FORMAT
**                                    CONTROLS STRUCTURE
**      CUR_RP          PTR    I/O   CURRENT POINTER TO RECORD POINTER
**                                    STRUCTURE
**      NLD_RP          PTR     O    POINTER TO RECORD POINTER STRUCTURE
**                                    WHERE CORRESPONDING DR HAS AN 'R' 
**                                    LEADER IDENTIFIER    
**      SF_FMT          CHAR   N/A   FORMAT CORRESPONDING TO THE CURRENT
**                                    DATA VALUE
**      NLD             INT    I/O   NO LEADER FLAG
**                                    TRUE  - NO LEADER
**                                    FALSE - LEADER EXISTS
**      BIT_CNT         INT     O    COUNT OF BITS STORED IN MEMORY FOR
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
**      SF_STATE_DR     INT     O    SUBFIELD STATE (DR)
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
**      REC_LEN         INT    I/O   DATA RECORD LENGTH     
**      RESV_SP         CHAR    O    RESERVED SPACE CHARACTER
**      LEAD_ID         CHAR   I/O   LEADER IDENTIFIER
**      S_RESV[6]       CHAR    O    RESERVED SPACE
**      DA_BASE         INT    I/O   BASE ADDRESS OF DATA AREA  
**      R_SPACE[4]      CHAR    O    RESERVED SPACE CHARACTERS
**      S_FDLEN         INT    I/O   SIZE OF FIELD LENGTH  
**      S_FDPOS         INT    I/O   SIZE OF FIELD POSITION 
**      SP_RSRV         INT     O    RESERVED DIGIT
**      S_TAG           INT    I/O   SIZE OF FIELD TAG 
**
**     RECORD POINTER STRUCTURE (RP)
**      NAME            TYPE   USE   DESCRIPTION
**      DR_START        INT     O    FILE LOCATION OF FIRST BYTE OF DATA RECORD
**      NEXT            PTR     O    POINTER TO NEXT DR_START
**      BACK            PTR     O    POINTER TO BACK DR_START
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION
**     LEAD_LEN         INT          DATA RECORDS LEADER LENGTH
**     NC               CHAR         NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE-ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     A. DEWITT                 06/05/90  INITIAL CODE
**     A. DEWITT                 06/17/90  MODIFY PDL AND CODE TO MOVE CALL TO 
**                                          BEG123REC OUTSIDE CONDITIONAL AND
**                                          TO ADD LOGIC TO INSERT TRUE RECORD
**                                          LENGTH WHEN A ZERO VALUE IS IN THE
**                                          LEADER
**     L. MCMILLION              10/16/90  REPLACED CALLS TO LIBRARY FUNCTION
**                                          STRTOK() WITH FUNCTION STR123TOK()
**                                          DUE TO NESTING PROBLEMS
**     A. DEWITT                 11/28/90  MODIFY LOGIC TO NOT READ PAST FT
**                                          IN CASE WHERE FORMATS ARE PRESENT
**                                          AND LABELS ARE NOT 
**     A. DEWITT                 02/28/91  ADD LOGIC TO RETAIN STARTING
**                                          POSITION OF DATA RECORD
**     L. MCMILLION  (TASK #40)  08/28/91  ADDED LOGIC TO CHECK FOR VALID
**                                          VALUES FOR SIZE OF FIELD LENGTH,
**                                          SIZE OF FIELD POSITION, AND SIZE
**                                          OF TAG
**     A. DEWITT     92DR005     04/09/92  IMPLEMENT BINARY DATA CHANGE
**     J. TAYLOR     92DR005     05/14/92  ADDED ADJACENT FIXED-LENGTH BIT
**                                          SUBFIELD ENTRIES INTO FM STRUCTURE
**     L. MCMILLION  92DR005     05/21/92  EXTRACTED SECTION OF CODE TO CREATE
**                                          NEW FUNCTION LOAD123FLD() TO BE
**                                          INVOKED BY WR123FLD() IN ADDITION
**                                          TO THIS FUNCTION
**     L. MCMILLION  TASK #40    11/24/92  UPDATED PROLOG AND CHANGE HISTORY
**                                          PER QC
**     J. TAYLOR     92DR003     11/11/92  ADDED CALL TO VER132DRTAG TO VERIFY
**                                          DATA RECORD TAG ORDER
**     J. TAYLOR     93DR023     04/22/93  MODIFIED TO PASS CHARACTER STRING
**                                          TO LOAD123FLD() 
**                                          RATHER THAN FILE POINTER
**
**    PDL:
**
**     SET FP TO CURRENT FILE POINTER 
**     INITIALIZE COMPRESSED FLAG TO COMPRESSED
**     CALL BEG123REC() TO CLEAR PREVIOUSLY STORED ENTRY
**     IF READ FILE
**     THEN
**        SET CUR_POS TO CURRENT POSITION OF INTERNAL FILE POINTER
**        ALLOCATE RECORD POINTER STRUCTURE 
**        INITIALIZE NEW_RP POINTERS 
**        SET DR_START TO CURRENT POSITION 
**        SET CUR_RP NEXT TO NEW_RP 
**        SET CUR_RP TO NEW_RP
**     ENDIF
**     IF NOT NLD { NO LEADER DIRECTORY FLAG SET }
**     THEN
**        CALL G123INT() TO READ RECORD LENGTH 
**        CALL G123STR() TO READ RESERVED SPACE CHARACTER
**        CALL G123STR() TO READ LEADER IDENTIFIER
**        IF LEADER IDENTIFIER IS NOT EQUAL TO 'D' OR 'R'
**        THEN
**           RETURN FAILURE
**        ENDIF
**        IF LEADER IDENTIFIER IS 'R'
**        THEN
**           SET NLD TO TRUE
**           IF READ FILE
**           THEN
**              SET NLD_RP TO CUR_RP
**           ENDIF
**        ENDIF
**        CALL G123STR() TO READ RESERVED SPACE CHARACTERS
**        CALL G123INT() TO READ BASE ADDRESS OF DATA AREA
**        CALL G123STR() TO READ RESERVED SPACE CHARACTERS
**        CALL G123INT() TO READ SIZE OF FIELD LENGTH
**        IF SIZE OF FIELD LENGTH IS NOT VALID SIZE
**        THEN
**           RETURN FAILURE
**        ENDIF
**        CALL G123INT() TO READ SIZE OF FIELD POSITION
**        IF SIZE OF FIELD POSITION IS NOT VALID SIZE
**        THEN
**           RETURN FAILURE
**        ENDIF
**        CALL G123INT() TO READ RESERVED DIGIT 
**        CALL G123INT() TO READ SIZE OF FIELD TAG
**        IF SIZE OF FIELD TAG IS NOT VALID SIZE
**        THEN
**           RETURN FAILURE
**        ENDIF
**
**        COMPUTE SIZE (IN BYTES) OF DATA DIRECTORY USING THE BASE
**         ADDRESS OF THE DATA AREA, AND THE SIZE OF THE DR LEADER.
** 
**        INITIALIZE CUR_SIZE
**        WHILE CUR_SIZE IS LESS THAN THE DIR_SIZ DO
**  
**           ALLOCATE NEW_DR STRUCTURE
**           INITIALIZE POINTERS OF NEW_DR TO NULL
**           SET NEXT POINTER FIELD OF CUR_DR TO NEW ENTRY
**           SET CUR_DR TO NEW_DR STRUCTURE
**
**           CALL G123STR() TO READ IN TAG STRING
**           CALL G123INT() TO READ IN FIELD LENGTH 
**           CALL G123INT() TO READ IN FIELD POSITION 
**
**           INCREMENT CUR_SIZ BY TAG SIZE, FIELD SIZE, AND POSITION SIZE
**
**        ENDWHILE
**        CALL VER123DRTAG() TO VERIFY DATA RECORD TAG ORDER
**        READ PAST FT
**        IF RECORD LENGTH READ IN IS ZERO
**        THEN
**           SET CURRENT DATA RECORD TO HEAD DATA RECORD
**           WHILE NOT AT END OF DATA RECORD LIST DO
**              SET CURRENT DATA RECORD TO NEXT
**           ENDWHILE
**           IF NOT AT HEADER RECORD 
**           THEN
**              SET RECORD LENGTH TO CURRENT FIELD LENGTH PLUS CURRENT FIELD
**               POSITION PLUS CURRENT BASE ADDRESS OF DATA
**           ENDIF
**        ENDIF
**
**     ENDIF
** 
**     INITIALIZE CUR_DR TO NEXT FIELD OF DR_HD
**     COMPUTE FIELD LENGTH
**     ALLOCATE SPACE FOR FIELD STRING
**     SAVE START OF FLD_STR
**     SET END OF FIELD POINTER
**     CALL G123FSTR() TO READ IN THE FIELD STRING
**     WHILE THERE ARE MORE DIRECTORY ENTRIES DO
**        RESET ADJACENT FIXED-LENGTH BIT FIELD BIT COUNT TO ZERO
**        CALL RET123MATCH() TO RETURN ASSOCIATED DDR STRUCTURE
**        CALL LOAD123FLD() TO LOAD THE DR FIELD DATA INTO THE STRUCTURES
**        SET CUR_DR TO CUR_DR NEXT POINTER
**     ENDWHILE
**
**     FREE FLD_STR
**     SET SF_STATE_DR TO NUMBER OF DIMENSIONS SUBFIELD
**     SET CUR_DR TO DR_HD
**
**     RETURN SUCCESS
**
****************************************************************************
**     CODE SECTION
**
****************************************************************************/
#include "stc123.h"

int ld123rec()

{
   /* DECLARE LOCAL VARIABLES */
   FILE      *fp       ;
   int       ch        ,
             comprssd  ;
   long      cur_siz   ,
             cur_pos   ,
             dir_siz   ,
             fld_len   ;
   char      *fld_str  ;
   char      *fld_str_end;
   struct dr *new_dr   ;
   struct rp *new_rp   ;
   char      *st_fld_str;
 
   /* INITIALIZE LOCAL FILE POINTER */
   fp = cur_fm->fp;

   /* INITIALIZE COMPRESSED FLAG TO COMPRESSED */
   comprssd = 1;
 
   /* CALL FUNCTION BEG123REC() TO CLEAR OUT ANY PREVIOUSLY ALLOCATED
       DR STORAGE 
   */
   if (!beg123rec(fp)) return(0);

   /* IF READ FILE */
   if (cur_fm->open_mode == 'r') {
   
      /* SET CUR_POS TO CURRENT POSITION OF INTERNAL FILE POINTER */
      if ((cur_pos = ftell(fp)) == -1) return(0);
   
      /* ALLOCATE RECORD POINTER STRUCTURE */
      if ((new_rp = (struct rp *) malloc(sizeof(struct rp))) == NULL) {
         return(0); 
      };   

      /* INITIALIZE NEW_RP POINTERS */
      new_rp->next = NULL;
      new_rp->back = cur_fm->cur_rp;

      /* SET DR_START TO CURRENT POSITION */
      new_rp->dr_start = cur_pos;   

      /* SET CUR_RP NEXT TO NEW_RP */
      cur_fm->cur_rp->next = new_rp;
   
      /* SET CUR_RP TO NEW_RP */
      cur_fm->cur_rp = new_rp;
   };   
   
   /* IF NOT NLD { NO LEADER DIRECTORY FLAG SET } */
   if (!cur_fm->nld) {
   
      /* CALL G123INT() TO READ RECORD LENGTH */
      if (!g123int(fp,5L,&cur_fm->rl_hd->rec_len))  return(0);
 
       /* CALL G123STR() TO READ RESERVED SPACE */ 
      if (!g123str(fp,&cur_fm->rl_hd->resv_sp,1L)) return(0);
  
      /* CALL G123STR() TO READ LEADER IDENTIFIER */
      if (!g123str(fp,&cur_fm->rl_hd->lead_id,1L)) return(0);
   
      /* IF LEADER IDENTIFIER IS NOT EQUAL TO 'D' OR 'R' RETURN ERROR */
      if ((cur_fm->rl_hd->lead_id != 'D') && (cur_fm->rl_hd->lead_id != 'R'))  return(0);
      
      /* IF LEADER IDENTIFIER IS A 'R' */
      if (cur_fm->rl_hd->lead_id == 'R')  {

         /* SET NLD FLAG */
         cur_fm->nld = 1;
         
         /* IF READ FILE */
         if (cur_fm->open_mode == 'r') { 
                
            /* SET NLD_RP TO CUR_RP */
            cur_fm->nld_rp = cur_fm->cur_rp;
         };   
      };   
  
      /* CALL G123STR() TO READ RESERVED SPACE */ 
      if (!g123str(fp,cur_fm->rl_hd->s_resv,5L)) return(0); 
  
      /* CALL G123INT() TO READ BASE ADDRESS OF DATA */
      if (!g123int(fp,5L,&cur_fm->rl_hd->da_base))  return(0); 

      /* CALL G123STR() TO READ RESERVED SPACE */ 
      if (!g123str(fp,cur_fm->rl_hd->r_space,3L)) return(0);
  
      /* CALL G123INT() TO READ SIZE OF FIELD LENGTH */
      if (!g123int(fp,1L,&cur_fm->rl_hd->s_fdlen)) return(0);
 
      /* IF SIZE OF FIELD LENGTH IS NOT VALID SIZE, RETURN FAILURE */
      if (cur_fm->rl_hd->s_fdlen < 1 || cur_fm->rl_hd->s_fdlen > 9) return(0); 

      /* CALL G123INT() TO READ SIZE OF FIELD POSITION */
      if (!g123int(fp,1L,&cur_fm->rl_hd->s_fdpos)) return(0);
 
      /* IF SIZE OF FIELD POSITION IS NOT VALID SIZE, RETURN FAILURE */
      if (cur_fm->rl_hd->s_fdpos < 1 || cur_fm->rl_hd->s_fdpos > 9) return(0);
  
      /* CALL G123INT() TO READ RESERVED DIGIT */
      if (!g123int(fp,1L,&cur_fm->rl_hd->sp_rsrv)) return(0);
  
      /* CALL G123INT() TO READ SIZE OF FIELD TAG */
      if (!g123int(fp,1L,&cur_fm->rl_hd->s_tag)) return(0); 
  
      /* IF SIZE OF TAG IS NOT VALID SIZE, RETURN FAILURE */
      if (cur_fm->rl_hd->s_tag < 1 || cur_fm->rl_hd->s_tag > 7 ||
       cur_fm->rl_hd->s_tag != cur_fm->dl_hd->s_tag) return(0);

      /* COMPUTE SIZE (IN BYTES) OF DATA DIRECTORY USING THE BASE
          ADDRESS OF THE DATA AREA, AND THE SIZE OF THE DR LEADER
      */ 
      dir_siz = cur_fm->rl_hd->da_base - LEAD_LEN - 1;
      
      /* INITIALIZE CUR_SIZE */
      cur_siz = 0;
      
      /* WHILE CUR_SIZE IS LESS THAN THE DIR_SIZ DO */
      while (cur_siz < dir_siz) {

         /* ALLOCATE NEW_DR STRUCTURE */
         if((new_dr = (struct dr *) malloc(sizeof(struct dr))) == NULL) return(0);

         /* INITIALIZE POINTERS OF NEW_DR TO NULL */
         new_dr->tag[0] = NC;
         new_dr->fd_len = 0;
         new_dr->fd_pos = 0;
         new_dr->num_dim = 0;
         new_dr->dim_lptr = NULL;
         new_dr->values = NULL;
         new_dr->next = NULL;
         
         /* SET NEXT POINTER FIELD OF CUR_DR TO NEW ENTRY */
         cur_fm->cur_dr->next = new_dr;
         
         /* SET CUR_DR TO NEW_DR STRUCTURE */
         cur_fm->cur_dr = new_dr;
 
         /* CALL G123STR() TO READ IN TAG STRING */
         if (!g123str(fp,new_dr->tag,cur_fm->rl_hd->s_tag)) return(0);
      
         /* CALL G123INT() TO READ IN FIELD LENGTH */
         if (!g123int(fp,cur_fm->rl_hd->s_fdlen,&new_dr->fd_len)) return(0);
      
         /* CALL G123INT() TO READ IN FIELD POSITION */
         if (!g123int(fp,cur_fm->rl_hd->s_fdpos,&new_dr->fd_pos)) return(0);

         /* INCREMENT CUR_SIZ BY TAG SIZE, FIELD SIZE, AND POSITION SIZE */
         cur_siz += (cur_fm->rl_hd->s_tag + cur_fm->rl_hd->s_fdlen + cur_fm->rl_hd->s_fdpos);
  
      }; 

      /* CALL VER123DRTAG() TO VERIFY DATA RECORD TAG ORDER */
      if (!ver123drtag()) return(0);

      /* READ PAST FT */
      if ((ch = getc(fp)) == EOF) return (0);
      
      /* IF REC_LEN IS ZERO */
      if (cur_fm->rl_hd->rec_len == 0L) {

         /* SET CURRENT DATA RECORD TO HEAD DATA RECORD */
         cur_fm->cur_dr = cur_fm->dr_hd;

         /* WHILE NEXT FIELD OF CURRENT DATA RECORD NOT NULL DO */
         while (cur_fm->cur_dr->next != NULL) cur_fm->cur_dr = cur_fm->cur_dr->next;
         
         /* IF NOT AT DUMMY HEADER RECORD */
         if (cur_fm->cur_dr != cur_fm->dr_hd) {
            
            /* SET ACTUAL RECORD LENGTH */
            cur_fm->rl_hd->rec_len = cur_fm->cur_dr->fd_len +
             cur_fm->cur_dr->fd_pos + cur_fm->rl_hd->da_base;
         };
      };      
   };
                  
   /* INITIALIZE CUR_DR TO NEXT FIELD OF DR_HD */
   cur_fm->cur_dr = cur_fm->dr_hd->next;

   /* COMPUTE FIELD LENGTH */
   fld_len = cur_fm->rl_hd->rec_len - cur_fm->rl_hd->da_base;

   /* ALLOCATE SPACE FOR FIELD STRING */
   if ((fld_str = (char *) malloc((size_t) (fld_len + 1) * sizeof(char))) == NULL) return(0);

   /* SAVE START OF FLD_STR */
   st_fld_str = fld_str;

   /* SET END OF FIELD POINTER */
   fld_str_end = fld_str + fld_len;

   /* CALL G123FSTR() TO READ IN THE FIELD STRING */
   if (!g123fstr(fp,fld_str,fld_len))
    return(0);

   /* WHILE THERE ARE MORE DIRECTORY ENTRIES DO */
   while (cur_fm->cur_dr != NULL) {

      /* RESET ADJACENT FIXED-LENGTH BIT FIELD STRUCTURE TO ZERO */
      cur_fm->bit_cnt = 0;
         
      /* CALL RET123MATCH() TO RETURN ASSOCIATED DDR STRUCTURE */
      if (!ret123match(cur_fm->cur_dr->tag)) return(0);
         
      /* CALL LOAD123FLD() TO LOAD THE DR FIELD DATA INTO THE STRUCTURES */
      if (!load123fld(&fld_str,fld_str_end,comprssd)) return(0);

      /* CUR_DR EQUALS CUR_DR NEXT POINTER */
      cur_fm->cur_dr = cur_fm->cur_dr->next;
   };   

   /* FREE FLD_STR */
   free(st_fld_str);
 
   /* SET SF_STATE_DR TO NUMBER OF DIMENSIONS SUBFIELD */
   cur_fm->sf_state_dr = 1;

   /* SET CURRENT RECORD POINTER TO HEAD POINTER */
   cur_fm->cur_dr = cur_fm->dr_hd;
      
   /* RETURN SUCCESS */  
   return(1);
}
