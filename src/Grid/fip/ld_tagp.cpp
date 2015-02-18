# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: LOAD123TAGP
**
**    PURPOSE: TO LOAD TAG PAIRS INTO DATA DESCRIPTIVE TAG PAIR STRUCTURE
**
**    INVOCATION METHOD: LOAD123TAGP(STRING)
**
**    ARGUMENT LIST:
**     NAME           TYPE      USE      DESCRIPTION
**     STRING[]       PTR        I       TAG PAIR STRING
**     LOAD123TAGP()  LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES: NONE
**
**    INTERNAL VARIABLES:
**     NAME           TYPE           DESCRIPTION
**     CUR_TL         PTR            POINTER TO ENTRY IN TAG PAIR STRUCTURE
**     FIRST_TAG[10]  CHAR           FIRST TAG OF TAG PAIR
**     INDEX          INT            INDEX INTO INPUT STRING
**     LENGTH         INT            LENGTH OF INPUT STRING
**     NEW_TL         PTR            POINTER TO ENTRY IN TAG PAIR STRUCTURE
**     SECOND_TAG[10] CHAR           SECOND TAG OF TAG PAIR
**  
**    GLOBAL REFERENCES:
**
**     DATA DESCRIPTIVE FILE CONTROL RECORD STRUCTURE (CR)
**      NAME            TYPE   USE   DESCRIPTION
**      F_TITLE         CHAR   N/A   FILE TITLE
**      TAG_L           PTR     O    POINTER TO LIST OF TAG PAIR RECORDS
**      U_AFD[]         PTR    N/A   CHARACTER STRING POINTER TO USERS 
**                                    AUGMENTED FILE DESCRIPTION
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
**     FILE MANAGEMENT STRUCTURE (FM)
**      NAME            TYPE   USE   DESCRIPTION
**      FP              PTR    N/A   FILE POINTER
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE
**      CR_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE FILE
**                                    CONTROL RECORD STRUCTURE
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE
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
**      CUR_DD          PTR    N/A   CURRENT POINTER TO DATA DESCRIPTIVE
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
**      TAG_1[10]       CHAR    O    INTERNAL NAME OF AN ASSOCIATED FIELD
**      TAG_2[10]       CHAR    O    INTERNAL NAME OF AN ASSOCIATED FIELD
**      NEXT            PTR    I/O   POINTER TO DATA DESCRIPTIVE TAG PAIR
**                                    STRUCTURE
**
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT 
**                                    STRUCTURE ENTRY
**
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION
**     NC               CHAR         NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     P. HODGES                 05/18/90  INITIAL PROLOG
**     P. HODGES                 05/18/90  INITIAL PDL
**     L. MCMILLION              06/03/90  INITIAL CODE
**     L. MCMILLION  (TASK #40)  08/27/91  ADDED LOGIC TO RETURN FAILURE IF
**                                          DYNAMIC MEMORY ALLOCATION FAILS
**     L. MCMILLION  TASK #40    12/02/92  PROLOG UPDATED
**
**    PDL:
**
**     INITIALIZE INDEX OF INPUT STRING TO ZERO
**     ALLOCATE NEW_TL { DUMMY }
**     SET TAG_L TO NEW_TL
**     SET NEW_TL NEXT TO NULL
**     SET CUR_TL TO NEW_TL
**     GET INPUT STRING LENGTH
**     WHILE INDEX LESS THAN LENGTH DO
**        PARSE FIRST TAG FROM STRING
**        UPDATE INDEX OF INPUT STRING
**        PARSE SECOND TAG FROM STRING
**        UPDATE INDEX OF INPUT STRING
**        ALLOCATE NEW_TL
**        SET CUR_TL NEXT TO NEW_TL
**        SET NEW_TL NEXT TO NULL
**        SET TAG1 TO FIRST TAG
**        SET TAG2 TO SECOND TAG
**        SET CUR_TL TO NEW_TL
**     ENDDO
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int load123tagp(char *string)

{
   /* INTERNAL VARIABLES */
   struct tl *cur_tl;
   char first_tag[10];
   long index = 0;
   long length;
   struct tl *new_tl;
   char second_tag[10];

   /* ALLOCATE NEW_TL { DUMMY } */
   if ((new_tl = (struct tl *) malloc(sizeof(struct tl))) == NULL) return(0);

   /* SET TAG_L TO NEW_TL */
   cur_fm->cr_hd->tag_l = new_tl;

   /* SET NEW_TL NEXT TO NULL */
   new_tl->next = NULL;

   /* SET CUR_TL TO NEW_TL */
   cur_tl = new_tl;

   /* GET INPUT STRING LENGTH */
   length = _tcslen(string);
   
   /* WHILE INDEX LESS THAN LENGTH DO */
   while(index < length) {

      /* PARSE FIRST TAG FROM STRING */
      strncpy(first_tag,&string[index],(int) cur_fm->dl_hd->s_tag);
      first_tag[cur_fm->dl_hd->s_tag] = NC;

      /* UPDATE INDEX OF INPUT STRING */
      index += cur_fm->dl_hd->s_tag;

      /* PARSE SECOND TAG FROM STRING */
      strncpy(second_tag,&string[index],(int) cur_fm->dl_hd->s_tag);
      second_tag[cur_fm->dl_hd->s_tag] = NC;
      
      /* UPDATE INDEX OF INPUT STRING */
      index += cur_fm->dl_hd->s_tag;

      /* ALLOCATE NEW_TL */
      if ((new_tl = (struct tl *) malloc(sizeof(struct tl))) == NULL) return(0);

      /* SET CUR_TL NEXT TO NEW_TL */
      cur_tl->next = new_tl;

      /* SET NEW_TL NEXT TO NULL */
      new_tl->next = NULL;

      /* SET TAG1 TO FIRST TAG */
      strcpy(new_tl->tag_1,first_tag);

      /* SET TAG2 TO SECOND TAG */
      strcpy(new_tl->tag_2,second_tag);

      /* SET CUR_TL TO NEW_TL */
      cur_tl = new_tl;
   }

   /* RETURN SUCCESS */
   return(1);
} 
