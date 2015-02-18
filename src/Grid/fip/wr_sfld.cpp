# include "stdafx.h"
/*************************************************************************** 
** 
**    INVOCATION NAME: WR123SFLD 
** 
**    PURPOSE: TO WRITE A DATA RECORD SUBFIELD TO AN OUTPUT BUFFER TO BE 
**              PHYSICALLY WRITTEN WHEN THE RECORD IS COMPLETE 
** 
**    INVOCATION METHOD: WR123SFLD(FP,TAG,LEADID,WR_STR,STR_LEN,OPTION) 
** 
**    ARGUMENT LIST: 
**     NAME       TYPE      USE      DESCRIPTION 
**     FP         PTR        I       FILE POINTER 
**     TAG[]      PTR        I       FIELD IDENTIFIER 
**     LEADID     CHAR       I       LEADER IDENTIFIER
**     WR_STR[]   PTR        I       STRING CONTAINING A DR SUBFIELD 
**     STR_LEN    LONG       I       LENGTH OF WR_STR
**     OPTION     INT        I       WRITE SUBFIELD OPTION 
**                                    0 = FAILURE 
**                                    1 = OKAY 
**                                    2 = START OF RECORD 
**                                    3 = END OF RECORD 
**                                    4 = END OF FILE (DR) 
**                                    5 = END OF FIELD  
**                                    6 = START OF FIELD   
**      
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION 
**     BEG123REC()      BEGINS A DATA RECORD 
**     GET123LEVEL()    RETRIEVES APPROPRIATE DATA STRUCTURE LEVEL 
**     RET123MATCH()    FINDS THE ASSOCIATED DATA DESCRIPTIVE RECORD MATCH 
**                       TO THE DATA RECORD 
**     STOR123DV()      STORES DATA IN STORAGE STRUCTURES
** 
**    INTERNAL VARIABLES: 
**     NAME       TYPE               DESCRIPTION 
**     INIT_STATE INT                INITIALIZED SF_STATE_DR
**     NEW_DM     PTR                POINTER TO DIMENSION LENGTH STRUCTURE    
**     NEW_DR     PTR                POINTER TO DATA RECORD FIELD STRUCTURE 
**     NUM_ENT    INT                NUMBER OF ENTRIES IN DIMENSION LENGTH 
**                                    LIST 
**   
**    GLOBAL REFERENCES: 
** 
**     DATA DESCRIPTIVE RECORD STRUCTURE (DD) 
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD 
**      FD_LEN          INT    N/A   LENGTH OF DATA DESCRIPTIVE AREA RECORD 
**      FD_POS          INT    N/A   POSITION OF DATA DESCRIPTIVE AREA  
**                                    FIELD RECORD 
**      FD_CNTRL[10]    CHAR    I    FIELD CONTROLS 
**      NAME[]          PTR    N/A   CHARACTER STRING POINTER TO NAME 
**      NUM_DIM         INT     I    NUMBER OF DIMENSIONS IN ARRAY DESCRIPTOR
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT    N/A   NUMBER OF ELEMENTS IN PRIMARY DIMENSION
**      LABELS          PTR     I    HEAD POINTER TO A LINKED LIST CONTAINING 
**                                    LINKED LIST OF DD LABEL SUBFIELD 
**                                    STRUCTURES 
**      FMT_RT          PTR    N/A   ROOT POINTER TO FORMAT CONTROLS BINARY 
**                                    TREE STRUCTURE 
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE  
**                                    STRUCTURE 
** 
**     DIMENSION LENGTHS STRUCTURE (DM) 
**      NAME            TYPE   USE   DESCRIPTION 
**      LEN             INT     O    DIMENSION LENGTH 
**      NXT             PTR    I/O   POINTER TO NEXT DIMENSION LENGTH     
** 
**     DATA RECORD STRUCTURE (DR) 
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR    O    INTERNAL NAME OF AN ASSOCIATED FIELD 
**      FD_LEN          INT     O    LENGTH OF DISCRIPTIVE AREA DATA RECORD  
**      FD_POS          INT     O    POSITION OF DESCRIPTIVE AREA DATA 
**                                    RECORD 
**      NUM_DIM         INT    I/O   NUMBER OF DIMENSIONS (NO LABELS) 
**      DIM_LPTR        PTR    I/O   HEAD POINTER TO DIMENSION LENGTHS 
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
**      DL_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE LEADER 
**                                    STRUCTURE 
**      DR_HD           PTR    N/A   HEAD POINTER TO DATA RECORD STRUCTURE 
**      LP_HD           PTR    N/A   HEAD POINTER TO LABELS POINTER STRUCTURE 
**      RL_HD           PTR     I    HEAD POINTER TO DATA RECORD LEADER 
**                                    STRUCTURE 
**      RS_HD           PTR    N/A   HEAD POINTER TO FORMAT REPETITION STACK 
**                                    STRUCTURE 
**      REC_LOC_HD      PTR    N/A   HEAD POINTER TO RECORD POINTER STRUCTURE
**      CUR_DD          PTR     I    CURRENT POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE ENTRY 
**      CUR_DM          PTR    I/O   CURRENT POINTER TO DIMENSION LENGTHS 
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
**    DATA RECORD LEADER STRUCTURE (RL)
**     NAME            TYPE    USE   DESCRIPTION
**     REC_LEN         INT     N/A   DATA RECORD LENGTH
**     RESV_SP         CHAR    N/A   RESERVED SPACE CHARACTER
**     LEAD_ID         CHAR     O    LEADER IDENTIFIER
**     S_RESV[6]       CHAR    N/A   RESERVED SPACE
**     DA_BASE         INT     N/A   BASE ADDRESS OF DATA AREA
**     R_SPACE[4]      CHAR    N/A   RESERVED SPACE CHARACTERS
**     S_FDLEN         INT     N/A   SIZE OF FIELD LENGTH
**     S_FDPOS         INT     N/A   SIZE OF FIELD POSITION
**     SP_SSRV         INT     N/A   RESERVED DIGIT
**     S_TAG           INT     N/A   SIZE OF FIELD TAG
** 
**    GLOBAL VARIABLES: 
**     NAME             TYPE   USE   DESCRIPTION 
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT 
**                                    STRUCTURE ENTRY 
** 
**    GLOBAL CONSTANTS:
**     NAME             TYPE         DESCRIPTION 
**     FCDSTYPE         INT          RELATIVE POSITION OF THE DATA STRUCTURE
**                                    TYPE WITHIN THE FIELD CONTROL
**     NC               CHAR         NULL CHARACTER
** 
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     A. DEWITT                 05/17/90  INITIAL PROLOG 
**     A. DEWITT                 05/17/90  INITIAL PDL 
**     P. HODGES                 06/08/90  INITIAL CODE 
**     A. DEWITT                 06/29/90  DIMENSION STATE LOGIC CHANGE
**     A. DEWITT                 11/03/90  ADD NLD LOGIC AND LOGIC
**                                          TO STORE DATA VALUES BASED
**                                          ON DIMENSION LENGTHS
**     L. MCMILLION              12/12/90  REMOVED FREE123LAB() CALL FOR
**                                          END OF RECORD/END OF FILE
**                                          OPTION TO END123REC()
**     A. DEWITT                 02/08/91  MODIFY STATE LOGIC TO REINITIALIZE
**                                          STATE AFTER CALL TO BEG_REC
**     J. TAYLOR     92DR005     05/17/92  CHANGED CALLING SEQUENCE TO INPUT 
**                                          STRING LENGTH
**     L. MCMILLION  TASK #40    12/16/92  UPDATED PROGLOG/PDL PER QC
**     J. TAYLOR     93DR036     08/10/93  CHANGED ASSIGNMENT OF NUM_DIM TO  
**                                          THE ONE POINTED TO BY CUR_DR NOT
**                                          NEW_DR
**     J. TAYLOR     93DR033     09/10/93  MODIFIED SETTING OF SF_STATE_DR 
**                                          FOR FIELDS WITH ARRAY DESCRIPTORS
**                                          IN DDR
**     K. HEUBUSCH   93DR033     02/25/94  MODIFIED ARRAY DESCRIPTOR CHECK
**                                          TO USE DIM_LPTR INSTEAD OF    
**                                          NUM_DIM
**                                           
**    PDL: 
** 
**     CALL GET123LEVEL() TO RETURN BUFFER STRUCTURE FOR INPUT FILE 
**     IF OPEN MODE NOT WRITE 
**     THEN 
**        RETURN FAILURE 
**     ENDIF 
** 
**     CALL RET123MATCH() TO RETURN A POINTER TO THE MATCHING DDR (CUR_DD) 
**     IF MATCH NOT FOUND 
**     THEN 
**        RETURN FAILURE 
**     ENDIF 
**
**     SET INIT_STATE TO SF_STATE_DR
**     IF SF_STATE_DR OUT OF BOUNDS 
**     THEN 
**        {INITIALIZE SF_STATE_DR} 
**        IF FD_CNTRL EMPTY OR INDICATES ELEMENTARY DATA 
**        THEN 
**           SET SF_STATE_DR TO DATA VALUE STRING SUBFIELD 
**        ELSE 
**           IF LABELS IS NULL AND ARRAY DATA INDICATED
**            AND ARRAY DESCRIPTORS NOT IN DDR
**           THEN 
**              {DIMENSION INFORMATION CONTAINED IN DR} 
**              SET SF_STATE_DR TO NUMBER OF DIMENSIONS SUBFIELD 
**           ELSE 
**              SET SF_STATE_DR TO DATA VALUE STRING SUBFIELD 
**           ENDIF 
**        ENDIF 
**        SET INIT_STATE TO SF_STATE_DR
**     ENDIF 
** 
**     CASE OPTION 
**        2,6: {START OF RECORD, START OF FIELD} 
**           IF OPTION IS START OF RECORD 
**           THEN 
**              CALL BEG123REC() TO BEGIN A DATA RECORD 
**              RESET SF_STATE_DR
**              SET CUR_DR LEAD_ID TO INPUT LEADID 
**           ENDIF 
**           IF NO LEADER FLAG NOT SET
**           THEN
**              ALLOCATE NEW_DR 
**              SET FD_LEN TO ZERO
**              SET FD_POS TO ZERO
**              SET NUM_DIM TO ZERO 
**              SET DIM_LPTR TO NULL 
**              SET VALUES POINTER TO NULL
**              SET NEXT POINTER TO NULL 
**              SET NEXT POINTER OF CUR_DR TO NEW_DR 
**              SET CUR_DV TO NULL
**              SET CUR_DM TO NULL
**              SET TAG OF NEW_DR TO INPUT TAG 
**              SET CUR_DR TO NEW_DR
**           ELSE 
**              SET CUR_DR TO NEXT FIELD 
**           ENDIF
** 
**           CASE SF_STATE_DR 
**              1: {NUMBER OF DIMENSIONS SUBFIELD} 
**                 CONVERT WR_STR TO INTEGER AND STORE IN NUM_DIM SUBFIELD 
**                 INCREMENT DR SUBFIELD STATE TO DIMENSION SUBFIELD
**              2: {LENGTH OF DIMENSION SUBFIELD} 
**                 RETURN FAILURE 
**              3: {DATA VALUE STRING SUBFIELD} 
**                 CALL STOR123DV() TO STORE DATA VALUE
**                 IF LEVEL 1 FILE
**                 THEN
**                    SET SF_STATE_DR TO OUT OF BOUNDS
**                 ELSE
**                    IF ELEMENTARY DATA VALUE 
**                    THEN
**                       SET SF_STATE_DR TO OUT OF BOUNDS
**                    ENDIF
**                 ENDIF
**           ENDCASE {SF_STATE_DR} 
** 
**        1,5: {OKAY, END OF FIELD} 
**           CASE SF_STATE_DR 
**              1: {NUMBER OF DIMENSIONS SUBFIELD} 
**                 RETURN FAILURE 
**              2: {LENGTH OF A DIMENSION SUBFIELD} 
**                 ALLOCATE NEW_DM 
**                 SET NXT FIELD TO NULL
**                 CONVERT WR_STR AND SET LEN FIELD TO INT VALUE OF WR_STR
**                 IF NO ENTRIES IN DIMENSION LENGTH LIST
**                 THEN
**                    ADD A DUMMY ENTRY
**                 ENDIF
**                 SET NUMBER OF ENTRIES IN DIMENSION LENGTH LIST TO ZERO
**                 SET CURRENT DIMENSION TO TOP OF DIMENSION LENGTH LIST
**                 DO UNTIL END OF LIST REACHED
**                    SET CUR_DM TO CUR_DM NEXT
**                    INCREMENT NUMBER OF ENTRIES IN DIMENSION LENGTH LIST
**                 ENDDO
**                 INSERT DIMENSION LENGTH INTO END OF STRUCTURE
**                 INCREMENT NUMBER OF ENTRIES IN DIMENSION LENGTH LIST
**                 IF NUM_ENT EQUALS NUM_DIM 
**                 THEN 
**                    SET SF_STATE_DR TO DATA VALUE STRING SUBFIELD 
**                 ENDIF 
**                 IF OPTION IS END OF FIELD
**                 THEN
**                    RETURN FAILURE
**                 ENDIF
**              3: {DATA VALUE STRING SUBFIELD} 
**                 CALL STOR123DV() TO STORE INPUT DATA VALUE
**                 IF LEVEL 1 FILE
**                 THEN
**                    SET SF_STATE_DR TO OUT OF BOUNDS
**                 ELSE
**                    IF ELEMENTARY DATA VALUE 
**                    THEN
**                       SET SF_STATE_DR TO OUT OF BOUNDS
**                    ENDIF
**                 ENDIF
**           ENDCASE {SF_STATE_DR} 
**
**           IF OPTION IS END OF FIELD 
**           THEN 
**              SET SF_STATE_DR TO OUT OF BOUNDS 
**           ENDIF 
** 
**        3,4: {END OF RECORD, END OF FILE} 
**           CASE SF_STATE_DR 
**              1: {NUMBER OF DIMENSIONS SUBFIELD} 
**                 RETURN FAILURE 
**              2: {LENGTH OF A DIMENSION SUBFIELD} 
**                 RETURN FAILURE 
**              3: {DATA VALUE STRING SUBFIELD} 
**                 CALL STOR123DV() TO STORE INPUT DATA VALUE
**           ENDCASE {SF_STATE_DR} 
**           SET SF_STATE_DR TO OUT OF BOUNDS
** 
**        0: {FAILURE} 
**           RETURN FAILURE 
**     ENDCASE {OPTION} 
** 
**     RETURN  
** 
** 
****************************************************************************** 
**    CODE SECTION 
** 
******************************************************************************/ 
#include "stc123.h"

int wr123sfld(FILE *fp,char *tag,int leadid,char *wr_str,long str_len,int option)

{
   /* LOCAL VARIABLES */
   struct dm *new_dm;
   struct dr *new_dr = NULL;
   long      num_ent;
   int    init_state;
 
   /* CALL GET123LEVEL() TO RETURN BUFFER STRUCTURE FOR INPUT FILE */
   if (!get123level(fp)) return (0);

   /* IF OPEN MODE NOT WRITE RETURN FAILURE */
   if (cur_fm->open_mode != 'w') return (0);

   /* CALL RET123MATCH() TO RETURN A POINTER TO THE MATCHING DDR (CUR_DD) 
       IF MATCH NOT FOUND RETURN FAILURE
   */
   if (!ret123match(tag)) return (0);

   /*  SET INIT_STATE TO SF_STATE_DR */
   init_state = cur_fm->sf_state_dr;

   /* IF SF_STATE_DR OUT OF BOUNDS */
   if ((cur_fm->sf_state_dr >= 4) || (cur_fm->sf_state_dr < 1)) {

      /* {INITIALIZE SF_STATE_DR} */
      
      /* IF FD_CNTRL EMPTY OR INDICATES ELEMENTARY DATA */
      if ((cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == NC) ||
       (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '0')) {
 
         /* SET SF_STATE_DR TO DATA VALUE STRING SUBFIELD */
         cur_fm->sf_state_dr = 3;
      }
      else {

         /* IF LABELS IS NULL AND ARRAY DATA INDICATED 
             AND ARRAY DESCRIPTORS NOT IN DDR */
         if (cur_fm->cur_dd->labels == NULL &&
          cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '2' &&
          cur_fm->cur_dd->dim_lptr == NULL)  {

            /* {DIMENSION INFORMATION CONTAINED IN DR} 
               SET SF_STATE_DR TO NUMBER OF DIMENSIONS SUBFIELD 
            */
            cur_fm->sf_state_dr = 1;
         }
         else {

            /* SET SF_STATE_DR TO DATA VALUE STRING SUBFIELD */
            cur_fm->sf_state_dr = 3;

         }
      }
      
      /*  SET INIT_STATE TO SF_STATE_DR */
      init_state = cur_fm->sf_state_dr;
      
   }   
    

   /* CASE OPTION */
   switch(option) {

      /* 2,6: {START OF RECORD, START OF FIELD} */
      case 2:
      case 6:

         /* IF OPTION IS START OF RECORD */
         if(option == 2) {

            /* CALL BEG123REC() TO BEGIN A DATA RECORD */
            if(!beg123rec(fp)) return (0);

            /* RESET SF_STATE_DR */
            cur_fm->sf_state_dr = init_state;
            
            /* SET CUR_DR LEAD_ID TO INPUT LEADID */
            cur_fm->rl_hd->lead_id = leadid;
         }
 
         /* IF NLD FLAG HAS NOT BEEN SET */
         if (!cur_fm->nld) {
         
            /* ALLOCATE NEW_DR */
            if ((new_dr = (struct dr *) malloc (sizeof(struct dr))) == NULL) return(0);

            /* SET FD_LEN TO ZERO */
            new_dr->fd_len = 0;
            
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

            /* SET TAG OF NEW_DR TO INPUT TAG */
            strcpy(new_dr->tag, tag);

            /* SET CUR_DR TO NEW_DR */
            cur_fm->cur_dr = new_dr;
         }
         else {
            
            /* SET CUR_DR TO NEXT FIELD */
            cur_fm->cur_dr = cur_fm->cur_dr->next;
         };

         /* CASE SF_STATE_DR */
         switch (cur_fm->sf_state_dr) {

            /* 1: {NUMBER OF DIMENSIONS SUBFIELD}
                  CONVERT WR_STR TO INTEGER AND STORE IN NUM_DIM SUBFIELD 
            */
            case 1:

               cur_fm->cur_dr->num_dim = atol(wr_str);

               /* UPDATE STATE TO DIMENSION LENGTHS FIELD*/
               cur_fm->sf_state_dr++;
            break;

            /* 2: {LENGTH OF DIMENSION SUBFIELD} */
            case 2:
            
               /* RETURN FAILURE */
               return (0);
            break;
  
            /* 3: {DATA VALUE STRING SUBFIELD} */
            case 3:

               /* CALL STOR123DV() TO STORE INPUT DATA VALUE */
               if (!stor123dv(wr_str,str_len)) return(0);
               
               /* IF INTERCHANGE LEVEL IS 1 */
               if (cur_fm->dl_hd->ilevel == 1) {

                  /* SET STATE TO OUT OF BOUNDS */
                  cur_fm->sf_state_dr++;
               }
               else {
                  /* IF DATA IS ELEMENTARY DATA */               
                  if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '0') {
                     
                     /* SET STATE TO OUT OF BOUNDS */
                     cur_fm->sf_state_dr++;
                  };   
               };
               
            break;   

         /* ENDCASE {SF_STATE_DR} */
         }

      break;

      /* 1,5: {OKAY, END OF FIELD} */
      case 1:
      case 5:

         /* CASE SF_STATE_DR */
         switch (cur_fm->sf_state_dr) {

            /* 1: {NUMBER OF DIMENSIONS SUBFIELD} */
            case 1:

               /* RETURN FAILURE */
               return (0);
            break;

            /* 2: {LENGTH OF A DIMENSION SUBFIELD} */
            case 2:

               /* ALLOCATE NEW_DM */
               if ((new_dm = (struct dm *) malloc(sizeof(struct dm))) == NULL) return(0);
        
               /* INITIALIZE NXT FIELD */
               new_dm->nxt = NULL;
               
               /* CONVERT AND STORE WR_STR TO LENGTH FIELD */
               new_dm->len = atol(wr_str);
               
               /* INSERT INTO DIMENSION LENGTH LIST */

               /* IF NO ENTRIES IN DIMENSION LENGTH LIST */
               if (cur_fm->cur_dr->dim_lptr ==  NULL) {

                  /* ADD A DUMMY ENTRY */
                  if ((cur_fm->cur_dr->dim_lptr = (struct dm *) malloc(sizeof (struct dm))) == NULL) return(0);
                  cur_fm->cur_dr->dim_lptr->nxt = NULL;
                  cur_fm->cur_dr->dim_lptr->len = 0;
               }

               /* SET NUMBER OF ENTRIES IN DIMENSION LENGTH LIST TO ZERO */
               num_ent = 0;

               /* SET CURRENT DIMENSION TO TOP OF DIMENSION LENGTH LIST */
               cur_fm->cur_dm = cur_fm->cur_dr->dim_lptr;
               
               /* DO UNTIL END OF LIST REACHED */
               while (cur_fm->cur_dm->nxt != NULL) {
                  
                  /* SET CUR_DM TO CUR_DM NEXT */
                  cur_fm->cur_dm = cur_fm->cur_dm->nxt;
                  
                  /* INCREMENT NUMBER OF ENTRIES IN DIMENSION LENGTH LIST */
                  num_ent++;
               }
               
               /* INSERT DIMENSION LENGTH INTO END OF STRUCTURE*/
               cur_fm->cur_dm->nxt = new_dm;
               cur_fm->cur_dm = new_dm;               

               /* INCREMENT NUMBER OF ENTRIES IN DIMENSION LENGTH LIST */
               num_ent++;

               /* IF NUM_ENT EQUALS NUM_DIM */
               if (num_ent == cur_fm->cur_dr->num_dim) {

                  /* SET SF_STATE_DR TO DATA VALUE STRING SUBFIELD */
                  cur_fm->sf_state_dr = 3;
               }

               /* IF OPTION EQUALS END OF FIELD, RETURN FAILURE */
               if (option == 5) return(0);

            break;   

            /* 3: {DATA VALUE STRING SUBFIELD} */
            case 3:

               /* CALL STOR123DV() TO STORE INPUT DATA VALUE */
               if (!stor123dv(wr_str,str_len)) return(0);               

               /* IF INTERCHANGE LEVEL IS 1 */
               if (cur_fm->dl_hd->ilevel == 1) {

                  /* SET STATE TO OUT OF BOUNDS */
                  cur_fm->sf_state_dr++;
               }
               else {
                  /* IF DATA IS ELEMENTARY DATA */               
                  if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '0') {
                     
                     /* SET STATE TO OUT OF BOUNDS */
                     cur_fm->sf_state_dr++;
                  };   
               };

               break;

            /* ENDCASE {SF_STATE_DR} */
            }
     
         /* IF OPTION IS END OF FIELD */
         if (option == 5) {

            /* SET SF_STATE_DR TO OUT OF BOUNDS */
            cur_fm->sf_state_dr = 4;
         }

      break;   
  
      /* 3,4: {END OF RECORD, END OF FILE} */
      case 3:
      case 4:

         /* CASE SF_STATE_DR */
         switch (cur_fm->sf_state_dr) {

            /* 1: {NUMBER OF DIMENSIONS SUBFIELD} */
            case 1:

               /* RETURN FAILURE */
               return (0);
            break; 

            /* 2: {LENGTH OF A DIMENSION SUBFIELD} */
            case 2:
                        
               /* RETURN FAILURE */
               return (0);
            break;
  
            /* 3: {DATA VALUE STRING SUBFIELD} */
            case 3:

               /* CALL STOR123DV() TO STORE INPUT DATA VALUE */
               if (!stor123dv(wr_str,str_len)) return(0);                             
               
            break;   

         /* ENDCASE {SF_STATE_DR} */
         }  
         
         /* SET STATE TO OUT OF BOUNDS */
         cur_fm->sf_state_dr = 4;
      
      break;
                    
      /* 0: {FAILURE} */
      case 0:
    
         /* RETURN FAILURE */
         return (0);
      break;

   /* ENDCASE {OPTION} */
   }

   /* RETURN SUCCESS */
   return(1);  
} 
