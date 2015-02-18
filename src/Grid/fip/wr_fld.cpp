# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: WR123FLD 
** 
**    PURPOSE: TO WRITE A DATA RECORD FIELD TO AN OUTPUT BUFFER TO 
**              BE PHYSICALLY WRITTEN WHEN THE RECORD IS COMPLETE 
** 
**    INVOCATION METHOD:  WR123FLD(FP,TAG,LEADID,WR_STR,STR_LEN,OPTION)
** 
**    ARGUMENT LIST: 
**     NAME        TYPE      USE      DESCRIPTION
**     FP          PTR        I       FILE POINTER
**     TAG[]       PTR        I       FIELD IDENTIFIER
**     LEADID      CHAR       I       LEADER IDENTIFIER
**     WR_STR[]    PTR        I       STRING CONTAINING A DR FIELD
**     STR_LEN     LONG       I       LENGTH OF WR_STR
**     OPTION      INT        I       WRITE FIELD OPTION
**                                    0 = FAILURE 
**                                    1 = OKAY 
**                                    2 = START OF RECORD 
**                                    3 = END OF RECORD 
**                                    4 = END OF FILE (DR) 
**                                    5 = END OF FIELD  
**                                    6 = START OF FIELD   
**     WR123FLD()  LOGICAL   O        SUCCESS FLAG
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     BEG123REC()      BEGINS A DATA RECORD
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LAYER 
**     LOAD123FLD()     RETRIEVES DR FIELD VALUES FROM THE INDICATED FILE
**     RET123MATCH()    RETURNS ASSOCIATED DATA DESCRIPTIVE RECORD STRUCTURE
**     STC123EMPTY()    DETERMINES IF A DATA RECORD STRUCTURE IS EMPTY
** 
**    INTERNAL VARIABLES: 
**     NAME         TYPE          DESCRIPTION
**     COMPRSSD     INT           LOGICAL INDICATING WHETHER OR NOT ADJACENT
**                                 FIXED-LENGTH BINARY SUBFIELDS ARE 
**                                  COMPRESSED WITHIN THE FILE
**                                      0 - UNCOMPRESSED
**                                      1 - COMPRESSED
**     FLD_STR[]    PTR           CHARACTER STRING CONTAINING DR FIELD
**     ST_FLD_STR[] PTR           START CHARACTER STRING CONTAINING DR FIELD
**     NEW_DR       PTR           DATA RECORD STRUCTURE
**   
**    GLOBAL REFERENCES:
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR    O    INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT     O    LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT     O    POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT     O    NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR     O    HEAD POINTER TO DIMENSION LENGTHS
**                                    (NO LABELS)
**      VALUES          PTR     O    HEAD POINTER TO DATA VALUE SUBFIELD
**                                    RECORDS
**      NEXT            PTR     O    POINTER TO NEXT DATA RECORD 
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
**      DR_HD           PTR     I    HEAD POINTER TO DATA RECORD STRUCTURE
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE
**      RL_HD           PTR     O    HEAD POINTER TO DATA RECORD LEADER
**                                    STRUCTURE
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK
**                                    STRUCTURE
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR    N/A   CURRENT POINTER TO DATA DESCRIPTIVE
**                                    RECORD STRUCTURE ENTRY
**      CUR_DM          PTR     O    CURRENT POINTER TO DIMENSION LENGTHS
**                                    STRUCTURE ENTRY
**      CUR_DR          PTR     O    CURRENT POINTER TO DATA RECORD STRUCTURE
**                                    ENTRY
**      CUR_DV          PTR     O    CURRENT POINTER TO DR DATA VALUE SUBFIELD
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
**      SF_STATE_DR     INT    I/O   SUBFIELD STATE (DR)
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
**      LEAD_ID         CHAR    O    LEADER IDENTIFIER
**      S_RESV[6]       CHAR   N/A   RESERVED SPACE
**      DA_BASE         INT    N/A   BASE ADDRESS OF DATA AREA  
**      R_SPACE[4]      CHAR   N/A   RESERVED SPACE CHARACTERS
**      S_FDLEN         INT    N/A   SIZE OF FIELD LENGTH  
**      S_FDPOS         INT    N/A   SIZE OF FIELD POSITION 
**      SP_RSRV         INT    N/A   RESERVED DIGIT
**      S_TAG           INT    N/A   SIZE OF FIELD TAG 
**
**    GLOBAL VARIABLES:
**     NAME              TYPE   USE   DESCRIPTION
**     CUR_FM            PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                     STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS:
**     NAME          TYPE      DESCRIPTION
**     FT            CHAR      FIELD TERMINATOR (RS) 1/14
**     NC            CHAR      NULL CHARACTER
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     A. DEWITT                 04/23/90  INITIAL PROLOG 
**     P. HODGES                 05/16/90  INITIAL PDL 
**     P. HODGES                 06/08/90  INITIAL CODE
**     L. MCMILLION              11/08/90  REPLACED CALLS TO LIBRARY FUNCTION
**                                          STRTOK() WITH FUNCTION STR123TOK()
**                                          DUE TO NESTING PROBLEMS
**     A. DEWITT                 02/07/91  ADD CALL TO CKI123NFLD TO PAD NULL
**                                          SUBFIELDS. CORRECT SUBFIELD OPTION
**                                          LOGIC TO ALLOW FOR START OF RECORDS
**                                          AND START OF FIELDS
**     J. TAYLOR     92DR005     05/17/92  CHANGED CALLING SEQUENCE TO INPUT 
**                                          STRING LENGTH
**     L. MCMILLION  92DR005     05/21/92  REPLACED LOGIC INVOKING WR123SFLD()
**                                          WITH LOGIC TO LOAD DATA STRUCTURES
**                                          FROM A TEMPORARY FILE
**     L. MCMILLION  92DR013     07/17/92  ADDED CONDITIONALS TO DETERMINE IF
**                                          THE APPLICATION PROGRAM INVOKED
**                                          BEG123REC() EXPLICITLY.  IF NOT,
**                                          INVOKE BEG123REC() FOR OPTION 2.
**     J. TAYLOR     92DR014     10/14/92  ADDED STATEMENT TO FREE MEMORY 
**                                          ALLOCATED FOR CHARACTER POINTER
**                                          FLD_STR
**     J. TAYLOR     TASK 55     01/20/93  ADDED CONDITIONAL COMPILATION AROUND
**                                          PARSING OF TEMPORARY FILE NAME TO
**                                          REMOVE THE PATH NAME
**     L. MCMILLION  TASK 55     03/12/93  UPDATED PROGLOG PER QC
**     J. TAYLOR     TASK 55     03/15/93  MOVED STORING START OF TEMPORARY FILE
**                                          FILE NAME OUTSIDE OF NOT MVS IFDEF
**     J. TAYLOR     93DR023     04/22/93  MODIFIED TO PASS CHARACTER STRING
**                                          TO LOAD123FLD() 
**                                          RATHER THAN FILE POINTER
**     J. TAYLOR     93DR037     07/28/93  REMOVED MEMSET CALLS
**     J. TAYLOR     93DR039     07/28/93  REMOVED MEMSET CALLS
** 
**    PDL: 
** 
**     SET COMPRESSED FLAG TO UNCOMPRESSED
**     CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER 
**     IF OPEN MODE NOT WRITE
**     THEN
**        RETURN FAILURE
**     ENDIF
**     IF INPUT OPTION IS INVALID
**     THEN 
**        RETURN FAILURE 
**     ENDIF 
**     IF INPUT STRING DOES NOT END WITH AN FT
**     THEN
**        INCREMENT STRING LENGTH
**     ENDIF
**     ALLOCATE SPACE FOR FIELD STRING
**     SAVE START OF FIELD STRING
**     COPY WR_STR TO FIELD STRING
**     TERMINATE FIELD STRING WITH NULL CHARACTER
**     SET LAST POSITION OF FIELD STRING TO FT
**     IF INPUT OPTION IS START OF RECORD
**     THEN
**        IF DR_HD IS NULL
**        THEN
**           CALL BEG123REC() TO CLEAR PREVIOUSLY STORED DR
**        ELSE IF NLD NOT SET AND DR_HD NEXT NOT NULL
**        THEN
**           CALL BEG123REC() TO CLEAR PREVIOUSLY STORED DR
**        ELSE IF NLD SET AND DR_HD NEXT NOT NULL AND DATA VALUES STILL EXIST
**         IN MEMORY
**        THEN
**           CALL BEG123REC() TO CLEAR PREVIOUSLY STORED DR
**        ENDIF
**        SET DR LEADER ID TO INPUT LEADER ID
**     ENDIF
**
**     IF NLD FLAG HAS NOT BEEN SET
**     THEN
**        ALLOCATE NEW_DR
**        SET FD_LEN TO STR_LEN
**        SET FD_POS TO ZERO
**        SET NUM_DIM TO ZERO
**        SET DIM_LPTR TO NULL
**        SET VALUES POINTER TO NULL
**        SET NEXT POINTER TO NULL
**        SET NEXT POINTER OF CUR_DR TO NEW_DR
**        SET CUR_DV TO NULL
**        SET CUR_DM TO NULL
**        TAG OF NEW DR TO INPUT TAG
**        SET CUR_DR TO NEW_DR
**     ELSE
**        SET CUR_DR TO NEXT FIELD
**     ENDIF
**     CALL RET123MATCH() TO RETURN A POINTER TO THE MATCHING DDR (CUR_DR),
**      RETURN FAILURE IF MATCH NOT FOUND
**
**     CALL LOAD123FLD() TO RETRIEVE THE DR FIELD DATA FROM THE INPUT STRING
**      AND STORE INTO DATA STRUCTURES IN MEMORY
**     RELEASE START OF FIELD STRING
**     SET SF_STATE_DR TO FINISHED
** 
**     RETURN SUCCESS
** 
****************************************************************************** 
**    CODE SECTION 
** 
*****************************************************************************/
#include "stc123.h"

int wr123fld(FILE *fp,char *tag,char leadid,char *wr_str,long str_len,int option)


{
   /* LOCAL VARIABLES */
   int comprssd;
   struct dr *new_dr;
   char *fld_str;
   char *st_fld_str;
 
   /* SET COMPRESSED FLAG TO UNCOMPRESSED */
   comprssd = 0;

   /* CALL GET123LEVEL() TO RETRIEVE APPROPRIATE DATA STRUCTURE LAYER */ 
   if (!get123level(fp)) return(0);

   /* IF OPEN MODE IS NOT WRITE, RETURN FAILURE */
   if (cur_fm->open_mode != 'w') return(0);

   /* IF INPUT OPTION INVALID, RETURN FAILURE */
   if ((option <= 0) || (option > 6)) return(0);

   /* IF INPUT STRING DOES NOT END WITH AN FT, INCREMENT STRING LENGTH */
   if (wr_str[str_len-1] != FT) str_len++;

   /* ALLOCATE SPACE FOR FIELD STRING */
   if ((fld_str = (char *) malloc((size_t) (str_len+1) * sizeof(char))) == NULL) return(0);

   /* SAVE START OF FIELD STRING */
   st_fld_str = fld_str; 

   /* COPY WR_STR TO FIELD STRING */
   memcpy(fld_str,wr_str,(size_t) str_len-1);

   /* TERMINATE FIELD STRING WITH NULL CHARACTER */
   fld_str[str_len] = NC;
  
   /* SET LAST POSITION OF FIELD STRING TO FT */
   fld_str[str_len-1] = FT;

   /* IF OPTION IS START OF RECORD */
   if (option == 2) {

      /* IF DR_HD IS NULL */
      if (cur_fm->dr_hd == NULL) {

         /* CALL BEG123REC() TO CLEAR PREVIOUSLY STORED DR */
         if (!beg123rec(fp)) return(0);
      }
      /* ELSE IF NLD NOT SET AND DR_HD NEXT NOT NULL */
      else if (!cur_fm->nld && cur_fm->dr_hd->next != NULL) {

         /* CALL BEG123REC() TO CLEAR PREVIOUSLY STORED DR */
         if (!beg123rec(fp)) return(0);
      }
      /* ELSE IF NLD SET AND DR_HD NEXT NOT NULL AND DATA VALUES STILL EXIST
          IN MEMORY
      */
      else if (cur_fm->nld && cur_fm->dr_hd->next != NULL && !stc123empty()) {

         /* CALL BEG123REC() TO CLEAR PREVIOUSLY STORED DR */
         if (!beg123rec(fp)) return(0);
      }

      /* SET DR LEADER ID TO INPUT LEADER ID */
      cur_fm->rl_hd->lead_id = leadid;
   }

   /* IF NLD FLAG HAS NOT BEEN SET */
   if (!cur_fm->nld) {

      /* ALLOCATE NEW_DR */
      if ((new_dr = (struct dr *) malloc(sizeof(struct dr))) == NULL)
       return(0);

      /* SET FD_LEN TO STRING LENGTH */
      new_dr->fd_len = str_len;

      /* SET FD_POS TO ZERO */
      new_dr->fd_pos = 0;

      /* SET NUM_DIM TO ZERO */
      new_dr->num_dim = 0;

      /* SET DIM_LPTR TO NULL */
      new_dr->dim_lptr = NULL;

      /* SET VALUES POINTER TO NULL */
      new_dr->values = NULL;

      /* SET NEXT POINTER TO NULL */
      new_dr->next = NULL;

      /* SET NEXT POINTER OF CUR_DR TO NEW_DR */
      cur_fm->cur_dr->next = new_dr;
 
      /* SET CUR_DV TO NULL */
      cur_fm->cur_dv = NULL;

      /* SET CUR_DM TO NULL */
      cur_fm->cur_dm = NULL;

      /* TAG OF NEW DR TO INPUT TAG */
      strcpy(new_dr->tag,tag);

      /* SET CUR_DR TO NEW_DR */
      cur_fm->cur_dr = new_dr;
   }

   /* ELSE */
   else

      /* SET CUR_DR TO NEXT FIELD */
      cur_fm->cur_dr = cur_fm->cur_dr->next;

   /* CALL RET123MATCH() TO RETURN A POINTER TO THE MATHCING DDR (CUR_DR),
       RETURN FAILURE IF MATCH NOT FOUND
   */
   if (!ret123match(tag)) return(0);

   /* CALL LOAD123FLD() TO RETRIEVE THE DR FIELD DATA FROM THE INPUT STRING
       AND STORE INTO DATA STRUCTURES IN MEMORY
   */
   if (!load123fld(&fld_str,fld_str+str_len,comprssd)) return(0);

   /* RELEASE START OF FIELD STRING */
   free(st_fld_str);

   /* SET SF_STATE_DR TO FINISHED */
   cur_fm->sf_state_dr = 4;
 
   /* RETURN SUCCESS */
   return(1);
}
