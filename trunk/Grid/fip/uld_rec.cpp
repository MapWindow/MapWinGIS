# include "stdafx.h"
/***************************************************************************
**
**     INVOCATION NAME: ULD123REC
**     
**     PURPOSE:  TO WRITE THE DATA RECORD TO OUTPUT FILE
**
**     INVOCATION METHOD: ULD123REC()
**
**     ARGUMENT LIST: NONE
**
**     EXTERNAL FUNCTION REFERENCES:
**      NAME             DESCRIPTION
**      CMP123DRDIR()    COMPUTES THE DATA RECORD DIRECTORY
**      CMP123DRLEAD()   COMPUTES THE DATA RECORD LEADER FIELD
**      RD123FLD()       FUNCTION TO RETRIEVE DATA RECORD FIELD
**      W123INT()        FUNCTION TO WRITE IN A INTEGER
**      
**     INTERNAL VARIABLES:
**      NAME            TYPE         DESCRIPTION
**      CUR_POS         INT          CURRENT INTERNAL FILE POINTER POSITION
**      INIT_DD         PTR          INITIAL POINTER TO CUR_DD
**      LD_ID           CHAR         LEADER IDENTIFIER
**      NEW_RP          PTR          POINTER TO RECORD POINTER STRUCTURE
**      RD_STAT         INT          STATUS
**                                    0 = FAILURE
**                                    1 = OKAY
**                                    2 = START OF RECORD
**                                    3 = END OF RECORD
**                                    4 = END OF FILE
**                                    5 = END OF FIELD
**                                    6 = START OF FIELD
**      STR_LEN         LONG         LENGTH OF STRING
**      TAG[10]         CHAR         FIELD TAG
**      TMP_LEN         INT          TEMPORARY RECORD LENGTH
**
**    GLOBAL REFERENCES:
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    I    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     I    LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT     I    POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSION LENGTHS
**                                    (NO LABELS)
**      VALUES          PTR    N/A   HEAD POINTER TO DATA VALUE SUBFIELD
**                                    RECORDS
**      NEXT            PTR     I    POINTER TO NEXT DATA RECORD 
**
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR     I    FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE
**      DL_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE LEADER
**                                    STRUCTURE
**      DR_HD           PTR     I    HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR     I    HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE 
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE ENTRY
**      CUR_DM          PTR    N/A   CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY
**      CUR_DR          PTR     I    CURRENT POINTER TO DATA RECORD STRUCTURE
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
**      CUR_RP          PTR     O    CURRENT POINTER TO RECORD POINTER
**                                    STRUCTURE
**      NLD_RP          PTR     O    POINTER TO RECORD POINTER STRUCTURE
**                                    WHERE CORRESPONDING DR HAS AN 'R' 
**                                    LEADER IDENTIFIER    
**      SF_FMT          CHAR   N/A   FORMAT CORRESPONDING TO THE CURRENT
**                                    DATA VALUE
**      NLD             INT    I/O   NO LEADER FLAG
**                                    TRUE  - NO LEADER
**                                    FALSE - LEADER EXISTS
**      BIT_CNT         INT    N/A   COUNT OF BITS STORED IN MEMORY FOR 
**                                    A SUBSEQUENT READ FUNCTION
**      BIT_BIN         CHAR   N/A   BITS STORED IN MEMORY FOR SUBSEQUENT
**                                    READ FUNCTION
**      COMPRESS        INT     O    FLAG TO SPECIFY COMPRESSED OR
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
**      REC_LEN         INT     I    DATA RECORD LENGTH     
**      RESV_SP         CHAR   N/A   RESERVED SPACE CHARACTER
**      LEAD_ID         CHAR    I    LEADER IDENTIFIER
**      S_RESV[6]       CHAR   N/A   RESERVED SPACE
**      DA_BASE         INT     I    BASE ADDRESS OF DATA AREA  
**      R_SPACE[4]      CHAR   N/A   RESERVED SPACE CHARACTERS
**      S_FDLEN         INT     I    SIZE OF FIELD LENGTH  
**      S_FDPOS         INT     I    SIZE OF FIELD POSITION 
**      SP_RSRV         INT     I    RESERVED DIGIT
**      S_TAG           INT     I    SIZE OF FIELD TAG 
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
**     GLB_STR2[MAXSIZ] CHAR    O    GLOBAL CHARACTER STRING USED FOR
**                                    PROCESSING
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE      DESCRIPTION
**     BLNK_SP          CHAR      BLANK SPACE CHARACTER
**     DB_DIGIT         INT       NUMBER OF DIGITS IN BASE ADDRESS OF DATA
**                                 AREA
**     FT_STR[2]        CHAR      CHARACTER STRING CONTAINING FIELD
**                                 TERMINATOR (RS) 1/14
**     MAXREC           INT       MAXIMUM RECORD LENGTH 
**     RES_SP[6]        CHAR      CHARACTER STRING OF RESERVED SPACE FIVE
**                                 POSITIONS IN LENGTH
**     RES_3SP[4]       CHAR      CHARACTER STRING OF RESERVED SPACE THREE
**                                 POSITIONS IN LENGTH
**     RL_DIGIT         INT       NUMBER OF DIGITS OF RECORD LENGTH
** 
**     CHANGE HISTORY:
**      AUTHOR        CHANGE-ID     DATE    CHANGE SUMMARY
**      A. DEWITT                 04/23/90  INITIAL PROLOG
**      A. DEWITT                 04/23/90  INITIAL PDL
**      A. DEWITT                 06/05/90  INITIAL CODE
**      A. DEWITT                 12/15/90  ADD GLOBAL CONSTANTS
**      A. DEWITT                 02/28/91  ADDED RECORD POSITION LOGIC
**                                           ADDED CALL TO SET NLD FLAG
**      L. MCMILLION  92DR008     03/27/92  MOVED STATEMENT TO WRITE FIELD
**                                           TERMINATOR AFTER  WRITING 
**                                           DIRECTORY AND ONLY IF LEADER
**                                           AND DIRECTORY EXIST
**      L. MCMILLION  92DR008     03/31/92  ADDED CONDITIONAL TO CHECK FOR
**                                           PRESENCE OF NLD_RP IN ADDITION
**                                           TO NO LEADER/DIRECTORY FLAG.
**                                           ADDED LOGIC TO SET NLD_RP POINTER
**                                            WHEN REQUIRED.
**      J. TAYLOR     92DR005     05/16/92  CHANGED CALLING SEQUENCE TO 
**                                           RD123SFLD
**      J. TAYLOR     92DR005     05/16/92  MODIFIED FOR BIT FIELD WRITING
**      J. TAYLOR     92DR005     05/20/92  MODIFIED CALL TO RD123FLD TO RETURN
**                                           COMPRESSED FIXED LENGTH BIT FIELDS
**      L. MCMILLION  TASK #40    12/09/92  PROLOG/PDL/INLINE COMMENTS UPDATE
**      J. TAYLOR     93DR030     06/10/93  MODIFIED TO SAVE CUR_DD AND RESET  
**
**     PDL:
**
**      SAVE INITIAL POINTER TO CUR_DD
**      INITIALIZE TEMPORARY RECORD LENGTH TO ZERO
**      INITIALIZE READ STATUS TO FAILURE
**      SET CUR_POS TO CURRENT POSITION OF INTERNAL FILE POINTER
**      ALLOCATE RECORD POINTER STRUCTURE
**      INITIALIZE NEW_RP POINTERS
**      SET DR_START TO CURRENT POSITION
**      SET CUR_RP NEXT TO NEW_RP
**      SET CUR_RP TO NEW_RP
**      IF NLD FLAG NOT SET OR NLD RECORD POINTER IS NULL
**      THEN
**         CALL CMP123DRDIR() TO COMPUTE THE DR DIRECTORY
**         CALL CMP123DRLEAD() TO COMPUTE THE DR LEADER FIELD
**         IF RECORD LENGTH EXCEEDS MAXIMUM RECORD LENGTH
**         THEN
**            CALL W123INT() TO WRITE ZERO FOR RECORD LENGTH
**         ELSE
**            CALL W123INT() TO WRITE RECORD LENGTH 
**         ENDIF
**         WRITE RESERVED SPACE CHARACTER
**         WRITE LEADER IDENTIFIER
**         IF LEADER IDENTIFIER IS 'R'
**         THEN
**            SET NLD FLAG TO TRUE
**            SET NLD_RP TO CURRENT RECORD POINTER
**         ENDIF
**         WRITE RESERVED SPACE CHARACTERS
**         CALL W123INT() TO WRITE BASE ADDRESS OF DATA AREA
**         WRITE RESERVED SPACE CHARACTERS
**         CALL W123INT() TO WRITE SIZE OF FIELD LENGTH
**         CALL W123INT() TO WRITE SIZE OF FIELD POSITION
**         CALL W123INT() TO WRITE RESERVED DIGIT
**         CALL W123INT() TO WRITE SIZE OF FIELD TAG
**
**         INITIALIZE CUR_DR TO NEXT FIELD OF DR_HD
**         WHILE CUR_DR NOT NULL DO
**   
**            WRITE IN TAG STRING
**            CALL W123INT() TO WRITE FIELD LENGTH 
**            CALL W123INT() TO WRITE FIELD POSITION 
**            SET CUR_DR TO NEXT FIELD
**
**         END WHILE
**         WRITE FT
**      ENDIF
**
**      INITIALIZE CUR_DR TO DR_HD
**      WHILE THERE ARE MORE DIRECTORY ENTRIES
**         SET COMPRESS TO COMPRESS
**         CALL RD123FLD() TO GET FORMATTED FIELD
**         RESET COMPRESS TO UNCOMPRESS
**         WRITE FIELD TO FILE
**
**      END WHILE
**
**      RESTORE INITIAL POINTER TO CUR_DD
**
**      RETURN SUCCESS
**
****************************************************************************
**      CODE SECTION
**
****************************************************************************/
#include "stc123.h"

int uld123rec(void)

{
   /* INTERNAL VARIABLE DECLARATIONS */
   long tmp_len = 0;
   long cur_pos    ;
   long str_len    ;
   int rd_stat = 0 ;
   char ld_id      ;
   char tag[10]    ;
   struct rp *new_rp;
   struct dd *init_dd;

   /* SAVE INITIAL POINTER TO CUR_DD */
   init_dd = cur_fm->cur_dd;

   /* SET CUR_POS TO CURRENT POSITION OF INTERNAL FILE POINTER */
   if ((cur_pos = ftell(cur_fm->fp)) == -1) return(0);
   
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
   
   /* IF NLD FLAG NOT SET OR NLD RECORD POINTER IS NULL */
   if (!cur_fm->nld || cur_fm->nld_rp == NULL) {
   
      /* CALL CMP123DRDIR() TO COMPUTE THE DR DIRECTORY */
      if (!cmp123drdir()) return(0);
     
      /* CALL CMP123DRLEAD() TO COMPUTE THE DR LEADER FIELD */
      if (!cmp123drlead()) return(0);
     
      /* IF RECORD LENGTH EXCEEDS MAXIMUM RECORD LENGTH */
      if (cur_fm->rl_hd->rec_len > MAXREC) {
     
         /* WRITE OUT ZERO VALUE FOR RECORD LENGTH */
         if (!w123int(cur_fm->fp,tmp_len,RL_DIGIT)) return(0);   
      
      }
      else {
      
         /* CALL W123INT() TO WRITE RECORD LENGTH */
         if (!w123int(cur_fm->fp,cur_fm->rl_hd->rec_len,RL_DIGIT)) return(0);
      };
        
      /* WRITE RESERVED SPACE CHARACTER */
      fprintf(cur_fm->fp,"%c",BLNK_SP);
     
      /* WRITE LEADER IDENTIFIER */
      fprintf(cur_fm->fp,"%c", cur_fm->rl_hd->lead_id);
     
      /* IF LEADER IDENTIFIER IS 'R' */
      if (cur_fm->rl_hd->lead_id == 'R') {
      
         /* SET NLD FLAG TO TRUE */
         cur_fm->nld = 1;

         /* SET NLD_RP TO CURRENT RECORD POINTER */
         cur_fm->nld_rp = cur_fm->cur_rp;
      };
         
      /* WRITE RESERVED SPACE CHARACTERS */
      fprintf(cur_fm->fp,"%s",RES_SP);
     
      /* CALL W123INT() TO WRITE BASE ADDRESS OF DATA AREA */
      if (!w123int(cur_fm->fp,cur_fm->rl_hd->da_base,DB_DIGIT)) return(0);
     
      /* WRITE RESERVED SPACE CHARACTERS */
      fprintf(cur_fm->fp,"%s",RES_3SP);
     
      /* CALL W123INT() TO WRITE SIZE OF FIELD LENGTH */
      if (!w123int(cur_fm->fp,cur_fm->rl_hd->s_fdlen,1L)) return(0);
     
      /* CALL W123INT() TO WRITE SIZE OF FIELD POSITION */
      if (!w123int(cur_fm->fp,cur_fm->rl_hd->s_fdpos,1L)) return(0);
     
      /* CALL W123INT() TO WRITE RESERVED DIGIT */
      if (!w123int(cur_fm->fp,cur_fm->rl_hd->sp_rsrv,1L)) return(0);
     
      /* CALL W123INT() TO WRITE SIZE OF FIELD TAG */
      if (!w123int(cur_fm->fp,cur_fm->rl_hd->s_tag,1L)) return(0);
     
      /* INITIALIZE CUR_DR TO NEXT FIELD OF DR_HD */
      cur_fm->cur_dr = cur_fm->dr_hd->next;
     
      /* WHILE CUR_DR NOT NULL DO */
      while (cur_fm->cur_dr != NULL) {
     
         /* WRITE IN TAG STRING */
         fprintf(cur_fm->fp,"%s",cur_fm->cur_dr->tag);
                
         /* CALL W123INT() TO WRITE FIELD LENGTH */
         if (!w123int(cur_fm->fp,cur_fm->cur_dr->fd_len,cur_fm->rl_hd->s_fdlen))
            return(0);
           
         /* CALL W123INT() TO WRITE FIELD POSITION */
         if (!w123int(cur_fm->fp,cur_fm->cur_dr->fd_pos,cur_fm->rl_hd->s_fdpos)) 
            return(0);
           
         /* SET CUR_DR TO NEXT FIELD */
         cur_fm->cur_dr = cur_fm->cur_dr->next;
      };

      /* WRITE FT */
      fprintf(cur_fm->fp,"%s",FT_STR);
   };     

   /* INITIALIZE CUR_DR TO DR_HD */
   cur_fm->cur_dr = cur_fm->dr_hd;
   
   /* WHILE THERE ARE MORE DIRECTORY ENTRIES */
   while (cur_fm->cur_dr->next != NULL) {
   
      /* SET COMPRESS TO COMPRESS */
      cur_fm->compress = 1;

      /* CALL RD123FLD() TO GET FORMATTED FIELD */
      if (!rd123fld(cur_fm->fp,tag,&ld_id,glb_str2,&str_len,&rd_stat))
       return(0);

      /* RESET COMPRESS TO UNCOMPRESSED */
      cur_fm->compress = 0;
      
      /* WRITE FIELD TO FILE */
      fwrite(glb_str2,sizeof(char),(size_t) str_len,cur_fm->fp);
   };

   /* RESTORE INITIAL POINTER TO CUR_DD */
   cur_fm->cur_dd = init_dd;

   /* RETURN SUCCESS */
   return(1);
}
