# include "stdafx.h"
/***************************************************************************
**
**     INVOCATION NAME: ULD123DDREC
**     
**     PURPOSE:  TO WRITE THE DATA DESCRIPTIVE RECORD TO OUTPUT FILE
**
**     INVOCATION METHOD: ULD123DDREC()
**
**     ARGUMENT LIST: NONE
**
**     EXTERNAL FUNCTION REFERENCES:
**      NAME             DESCRIPTION
**      CMP123DDDIR()    COMPUTES DATA DESCRIPTIVE RECORD DIRECTORY
**      CMP123DDLEAD()   COMPUTES DATA DESCRIPTIVE RECORD LEADER FIELD
**      RD123DDFLD()     RETRIEVES A FIELD OF DATA DESCRIPTIVE RECORD
**      W123INT()        FUNCTION TO WRITE AN INTEGER
** 
**     INTERNAL VARIABLES: 
**      NAME        TYPE    DESCRIPTION
**      STATUS      INT     STATUS
**                           0 = FAILURE
**                           1 = OKAY
**                           2 = START OF RECORD
**                           3 = END OF RECORD
**                           4 = END OF FILE
**                           5 = END OF FIELD
**                           6 = START OF FIELD
**      TAG[10]     CHAR    INTERNAL NAME OF AN ASSOCIATED FIELD
** 
**
**     GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     I    LENGTH OF DATA DESCRIPTIVE AREA RECORD
**      FD_POS          INT     I    POSITION OF DATA DESCRIPTIVE AREA 
**                                    FIELD RECORD
**      FD_CNTRL[10]    CHAR   N/A   FIELD CONTROLS
**      NAME[]          PTR    N/A   CHARACTER STRING POINTER TO NAME
**      LABELS          PTR    N/A   HEAD POINTER TO A LINKED LIST CONTAINING
**                                    LINKED LIST OF DD LABEL SUBFIELD
**                                    STRUCTURES
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY
**                                    TREE STRUCTURE
**      NEXT            PTR     I    POINTER TO NEXT DATA DESCRIPTIVE 
**                                    STRUCTURE
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT     I    DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT     I    INTERCHANGE LEVEL
**      LEAD_ID         CHAR    I    LEADER IDENTIFIER 
**      ICE_IND         CHAR    I    INLINE CODE EXTENSION INDICATOR
**      RESV_SP         CHAR    I    RESERVE SPACE CHARACTER
**      APPL_IND        CHAR    I    APPLICATION INDICATOR
**      FD_CNTRL_L      INT     I    FIELD CONTROL LENGTH
**      DA_BASE         INT     I    BASE ADDRESS OF DESCRIPTIVE AREA 
**      CCS_IND[3]      CHAR    I    CODE CHARACTER SET INDICATOR
**      S_FDLEN         INT     I    SIZE OF FIELD LENGTH 
**      S_FDPOS         INT     I    SIZE OF FIELD POSITION  
**      S_RESV          INT     I    RESERVED DIGIT
**      S_TAG           INT     I    SIZE OF FIELD TAG 
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR     I    FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE LEADER
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
**      NLD             INT    N/A   NO LEADER FLAG
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
**      SF_STATE_DD     INT     O    SUBFIELD STATE (DD)
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
**     GLOBAL VARIABLES:
**      NAME             TYPE   USE   DESCRIPTION
**      CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                     STRUCTURE ENTRY
**      GLB_STR2[MAXSIZ] CHAR   I/O   GLOBAL CHARACTER STRING USED FOR
**                                     PROCESSING
**
**     GLOBAL CONSTANTS:
**      NAME             TYPE         DESCRIPTION
**      FT               CHAR         FIELD TERMINATOR (RS) 1/14
**      MAXREC           INT          MAXIMUM RECORD LENGTH
** 
**     CHANGE HISTORY:
**      AUTHOR        CHANGE-ID     DATE     CHANGE SUMMARY
**      A. DEWITT                 04/23/90   INITIAL PROLOG
**      A. DEWITT                 04/23/90   INITIAL PDL
**      L. MCMILLION              06/03/90   INITIAL CODE
**      J. TAYLOR     92DR005     05/16/92   MODIFIED FOR BIT FIELD WRITING
**      L. MCMILLION  TASK #40    12/16/92   UPDATED PROLOG PER QC
**
**     PDL:
**
**      CALL CMP123DDDIR() TO COMPUTE DATA DESCRIPTIVE RECORD DIRECTORY
**      CALL CMP123DDLEAD() TO COMPUTE DATA DESCRIPTIVE RECORD LEADER FIELD
**      IF RECORD LENGTH GREATER THAN MAXREC
**      THEN
**         CALL W123INT() TO WRITE ZERO
**      ELSE
**         CALL W123INT() TO WRITE RECORD LENGTH 
**      ENDIF
**      CALL W123INT() TO WRITE INTERCHANGE LEVEL
**      WRITE LEADER IDENTIFIER
**      WRITE INLINE CODE EXTENSION
**      WRITE RESERVED SPACE 
**      WRITE APPLICATION INDICATOR 
**      CALL W123INT() TO WRITE FIELD CONTROL INDICATOR
**      CALL W123INT() TO WRITE BASE ADDRESS OF DATA DESCRIPTIVE AREA
**      WRITE CODE CHARACTER SET INDICATOR
**      CALL W123INT() TO WRITE SIZE OF FIELD LENGTH
**      CALL W123INT() TO WRITE SIZE OF FIELD POSITION
**      CALL W123INT() TO WRITE RESERVED DIGIT  
**      CALL W123INT() TO WRITE SIZE OF FIELD TAG
**
**      INITIALIZE CUR_DD TO NEXT FIELD OF DD_HD
**      WHILE CUR_DD NOT NULL DO
**         WRITE TAG STRING
**         CALL W123INT() TO WRITE FIELD LENGTH 
**         CALL W123INT() TO WRITE FIELD POSITION 
**         SET CUR_DD TO NEXT FIELD OF CUR_DD
**      ENDWHILE
**      WRITE FT
**
**      INITIALIZE CURRENT ENTRY TO THE FIRST DIRECTORY ENTRY (DD_HD)
**       FOLLOWING DUMMY ENTRY
**      WHILE THERE ARE MORE DIRECTORY ENTRIES DO
**         SET SUBFIELD STATE TO FIELD CONTROL SUBFIELD
**         CALL RD123DDFLD() TO RETRIEVE A FIELD OF THE DATA DESCRIPTIVE RECORD
**         WRITE RETURNED STRING
**         SET CUR_DD TO NEXT FIELD OF CUR_DD
**      ENDWHILE
** 
**      RETURN SUCCESS
**
****************************************************************************
**      CODE SECTION
**
****************************************************************************/
#include "stc123.h"

int uld123ddrec()

{
   /* INTERNAL VARIABLES */
   int status;
   char tag[10];

   /* CALL CMP123DDDIR() TO COMPUTE DATA DESCRIPTIVE RECORD DIRECTORY */
   if (!cmp123dddir()) return(0);

   /* CALL CMP123DDLEAD() TO COMPUTE DATA DESCRIPTIVE RECORD LEADER FIELD */
   if (!cmp123ddlead()) return(0);

   /* IF RECORD LENGTH GREATER THAN MAXREC */
   if (cur_fm->dl_hd->rec_len > MAXREC) {

      /* CALL W123INT() TO WRITE ZERO */
      if (!w123int(cur_fm->fp,0L,5L)) return(0);
   }
   else {

      /* CALL W123INT() TO WRITE RECORD LENGTH  */
      if (!w123int(cur_fm->fp,cur_fm->dl_hd->rec_len,5L)) return(0);
   }

   /* CALL W123INT() TO WRITE INTERCHANGE LEVEL */
   if (!w123int(cur_fm->fp,cur_fm->dl_hd->ilevel,1L)) return(0);

   /* WRITE LEADER IDENTIFIER */
   fputc(cur_fm->dl_hd->lead_id,cur_fm->fp);

   /* WRITE INLINE CODE EXTENSION */
   fputc(cur_fm->dl_hd->ice_ind,cur_fm->fp);

   /* WRITE RESERVED SPACE */
   fputc(cur_fm->dl_hd->resv_sp,cur_fm->fp);

   /* WRITE APPLICATION INDICATOR */
   fputc(cur_fm->dl_hd->appl_ind,cur_fm->fp);

   /* CALL W123INT() TO WRITE FIELD CONTROL INDICATOR */
   if (!w123int(cur_fm->fp,cur_fm->dl_hd->fd_cntrl_l,2L)) return(0);

   /* CALL W123INT() TO WRITE BASE ADDRESS OF DATA DESCRIPTIVE AREA */
   if (!w123int(cur_fm->fp,cur_fm->dl_hd->da_base,5L)) return(0);

   /* WRITE CODE CHARACTER SET INDICATOR */
   fwrite(cur_fm->dl_hd->ccs_ind,sizeof(char),_tcslen(cur_fm->dl_hd->ccs_ind),cur_fm->fp);

   /* CALL W123INT() TO WRITE SIZE OF FIELD LENGTH */
   if (!w123int(cur_fm->fp,cur_fm->dl_hd->s_fdlen,1L)) return(0);

   /* CALL W123INT() TO WRITE SIZE OF FIELD POSITION */
   if (!w123int(cur_fm->fp,cur_fm->dl_hd->s_fdpos,1L)) return(0);

   /* CALL W123INT() TO WRITE RESERVED DIGIT */
   if (!w123int(cur_fm->fp,cur_fm->dl_hd->s_resv,1L)) return(0);

   /* CALL W123INT() TO WRITE SIZE OF FIELD TAG */
   if (!w123int(cur_fm->fp,cur_fm->dl_hd->s_tag,1L)) return(0);

   /* INITIALIZE CUR_DD TO NEXT FIELD OF DD_HD */
   cur_fm->cur_dd = cur_fm->dd_hd->next;

   /* WHILE CUR_DD NOT NULL DO */
   while(cur_fm->cur_dd != NULL) {

      /* WRITE TAG STRING */
      fwrite(cur_fm->cur_dd->tag,sizeof(char),_tcslen(cur_fm->cur_dd->tag),cur_fm->fp);

      /* CALL W123INT() TO WRITE FIELD LENGTH  */
      if (!w123int(cur_fm->fp,cur_fm->cur_dd->fd_len,cur_fm->dl_hd->s_fdlen))
         return(0);

      /* CALL W123INT() TO WRITE FIELD POSITION  */
      if (!w123int(cur_fm->fp,cur_fm->cur_dd->fd_pos,cur_fm->dl_hd->s_fdpos)) return(0);

      /* SET CUR_DD TO NEXT FIELD OF CUR_DD */
      cur_fm->cur_dd = cur_fm->cur_dd->next;
   }

   /* WRITE FT */
   fputc(FT,cur_fm->fp);

   /* INITIALIZE CURRENT ENTRY TO THE FIRST DIRECTORY ENTRY (DD_HD) */
   cur_fm->cur_dd = cur_fm->dd_hd->next;

   /* WHILE THERE ARE MORE DIRECTORY ENTRIES DO */
   while(cur_fm->cur_dd != NULL) {

      /* SET SUBFIELD STATE TO FIELD CONTROL SUBFIELD */
      cur_fm->sf_state_dd = 1;

      /* CALL RD123DDFLD TO RETRIEVE A FIELD OF THE DATA DESCRIPTIVE RECORD */
      if (!rd123ddfld(cur_fm->fp,tag,glb_str2,&status)) return(0);

      /* WRITE RETURNED STRING */
      fwrite(glb_str2,sizeof(char),_tcslen(glb_str2),cur_fm->fp);

      /* SET CUR_DD TO NEXT FIELD OF CUR_DD */
      cur_fm->cur_dd = cur_fm->cur_dd->next;
   }

   /* RETURN SUCCESS */
   return(1);
}
