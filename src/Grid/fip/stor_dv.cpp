# include "stdafx.h"
/****************************************************************************
**
**    INVOCATION NAME: STOR123DV
**
**    PURPOSE: TO STORE A DATA VALUE STRING IN STORAGE STRUCTURE
**    
**    INVOCATION METHOD: STOR123DV(WR_STR,STR_LEN)
**
**    ARGUMENT LIST:
**     NAME         TYPE     USE      DESCRIPTION
**     WR_STR[]     PTR       I       STRING CONTAINING A DR SUBFIELD
**     STR_LEN      LONG      I       LENGTH OF WR_STR
**     STOR123DV()  LOGICAL   O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES: 
**     NAME             DESCRIPTION
**     FREE123LAB()     FREES THE LABEL POINTER STRUCTURE
**     GET123FMT()      RETRIEVES A SINGLE FORMAT FROM THE FORMAT STRUCTURE
**     INCRE123LB()     INCREMENTS THE POINTER TO THE NEXT LABEL
**     SETUP123LB()     SETS UP LABELS POINTER STRUCTURE
**    
**    INTERNAL VARIABLES:
**     NAME       TYPE              DESCRIPTION
**     COUNT      INT               COUNTER
**     DELIM      CHAR              USER DELIMITER OF FORMAT
**     DTYP       CHAR              FORMAT DATA TYPE 
**     FD_CURDV   INT               LOGICAL FLAG FOUND CURRENT DATA VALUE
**     LEN        INT               LENGTH OF A CHARACTER STRING
**     NEW_DV     PTR               POINTER TO DATA VALUE STRUCTURE
**     PDM        INT               PRIMARY DIMENSION
**     SET        INT               LOGICAL FLAG
**     TMP_DV     PTR               POINTER TO DATA VALUE STRUCTURE
**     TMP_VCOL   PTR               POINTER TO DATA VALUE STRUCTURE
**     WIDTH      INT               DATA WIDTH OF FORMAT
**     
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
**      DIM_LPTR        PTR     I    HEAD POINTER TO DIMENSIONS LENGTHS
**                                    (NO LABELS)
**      PRIM_DMS        INT    N/A   NUMBER OF ELEMENTS IN PRIMARY DIMENSION
**      LABELS          PTR     I    HEAD POINTER TO A LINKED LIST CONTAINING 
**                                    LINKED LIST OF DD LABEL SUBFIELD 
**                                    STRUCTURES 
**      FMT_RT          PTR     I    ROOT POINTER TO FORMAT CONTROLS BINARY 
**                                    TREE STRUCTURE 
**      NEXT            PTR    N/A   POINTER TO NEXT DATA DESCRIPTIVE  
**                                    STRUCTURE 
**
**     DATA DESCRIPTIVE LEADER STRUCTURE (DL)
**      NAME            TYPE   USE   DESCRIPTION
**      REC_LEN         INT    N/A   DATA DESCRIPTIVE RECORD LENGTH      
**      ILEVEL          INT     I    INTERCHANGE LEVEL
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
**     DIMENSION LENGTHS STRUCTURE (DM)
**      NAME            TYPE   USE   DESCRIPTION
**      LEN             INT     I    DIMENSION LENGTH
**      NXT             PTR     I    POINTER TO NEXT DIMENSION LENGTH    
**
**     DATA RECORD STRUCTURE (DR) 
**      NAME            TYPE   USE   DESCRIPTION 
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD 
**      FD_LEN          INT    N/A   LENGTH OF DISCRIPTIVE AREA DATA RECORD  
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA 
**                                    RECORD 
**      NUM_DIM         INT     I    NUMBER OF DIMENSIONS (NO LABELS) 
**      DIM_LPTR        PTR     I    HEAD POINTER TO DIMENSION LENGTHS 
**                                    (NO LABELS) 
**      VALUES          PTR     O    HEAD POINTER TO DATA VALUE SUBFIELD 
**                                    RECORDS 
**      NEXT            PTR    N/A   POINTER TO NEXT DATA RECORD  
** 
**     DR DATA VALUE SUBFIELD STRUCTURE (DV) 
**      NAME            TYPE   USE   DESCRIPTION 
**      VALUE           PTR    I/O   DATA VALUE 
**      NXT_VSET        PTR    I/O   POINTER TO NEXT SET OF DATA VALUES 
**      NXT_VAL         PTR    I/O   POINTER TO NEXT DATA VALUE SUBFIELD  
**                                    RECORD 
**     FILE MANAGEMENT STRUCTURE (FM) 
**      NAME            TYPE   USE   DESCRIPTION 
**      FP              PTR    N/A   FILE POINTER 
**      F_NAME[]        PTR    N/A   EXTERNAL FILE NAME 
**      OPEN_MODE       CHAR   N/A   OPEN MODE OF FILE 
**      CR_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE FILE 
**                                    CONTROL RECORD STRUCTURE 
**      DD_HD           PTR    N/A   HEAD POINTER TO DATA DESCRIPTIVE 
**                                    RECORD STRUCTURE 
**      DL_HD           PTR     I    HEAD POINTER TO DATA DESCRIPTIVE LEADER 
**                                    STRUCTURE 
**      DR_HD           PTR    N/A   HEAD POINTER TO DATA RECORD STRUCTURE 
**      LP_HD           PTR     I    HEAD POINTER TO LABELS POINTER STRUCTURE 
**      RL_HD           PTR    N/A   HEAD POINTER TO DATA RECORD LEADER 
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
**      CUR_DV          PTR     I    CURRENT POINTER TO DR DATA VALUE SUBFIELD 
**                                    STRUCTURE ENTRY 
**      ROW_DVH         PTR     O    CURRENT POINTER TO NEXT SET OF VALUES 
**                                    IN DR DATA VALUE SUBFIELD STRUCTURE ENTRY 
**      CUR_FC          PTR     O    CURRENT POINTER TO FORMAT CONTROLS 
**                                    STRUCTURE ENTRY 
**      CUR_LP          PTR    N/A   CURRENT POINTER TO LABELS POINTER 
**                                    STRUCTURE ENTRY 
**      CUR_SL          PTR    I/O   CURRENT POINTER TO DD-LABEL SUBFIELD 
**                                    STRUCTURE ENTRY 
**      CUR_FCR         PTR     O    CURRENT POINTER TO ROOT OF FORMAT 
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
**     LABELS POINTER STRUCTURE (LP)
**      NAME            TYPE   USE   DESCRIPTION
**      NEXT            PTR     I    POINTER TO NEXT LP ENTRY
**      FIRST           PTR     I    FIRST LABEL (SL) IN THE LABEL SET
**      CUR             PTR     I    CURRENT LABEL (SL) IN THE LABEL SET
**
**     DD LABEL SUBFIELD STRUCTURE (SL)
**      NAME            TYPE   USE   DESCRIPTION
**      LABEL[]         PTR    N/A   LABEL
**      NXT_LAB         PTR     I    POINTER TO NEXT LABEL IN SET
**      NXT_LSET        PTR     I    POINTER TO NEXT SET OF LABELS
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
**     A. DEWITT                 11/16/90  INITIAL PROLOG 
**     A. DEWITT                 11/16/90  INITIAL PDL 
**     A. DEWITT                 11/16/90  INITIAL CODE 
**     A. DEWITT                 11/29/90  MODIFY CODE TO KEEP TRACK OF CUR_DV
**     A. DEWITT                 02/11/91  MODIFY CODE TO HANDLE NULL SUBFIELDS
**     L. MCMILLION  92DR009     04/14/92  REPLACED BLANK WR_STR CHECK WITH
**                                          CHECK FOR DELIMITER SEPARATOR
**     J. TAYLOR     92DR005     05/26/92  CHANGED CALLING SEQUENCE TO INPUT
**                                          STRING LENGTH
**     J. TAYLOR     92DR005     05/26/92  COMPLETED BINARY DATA
**     J. TAYLOR     92DR014     10/14/92  PLACED CALL TO FREE123LAB BEFORE
**                                          CALL TO SETUP123LB
**     J. TAYLOR     93DR038     08/10/93  REMOVED SUBSTIUTUION OF NC FOR SEP
**     L. MCMILLION  93DR038     09/08/93  PROLOG MODIFICATION
**     J. TAYLOR     93DR033     11/01/93  MODIFIED SETTING OF PRIMARY DIMENSION
**                                          FROM DR OR DDR
**
**    PDL:
**     ALLOCATE NEW_DV 
**     SET POINTERS TO NULL 
**     SET LENGTH TO STRING LENGTH
**     IF WR_STR NOT EMPTY
**     THEN
**        ALLOCATE NEW_DV
**        SET VALUE TO WR_STR 
**     ENDIF
**     IF VALUES STRUCTURE IS EMPTY 
**     THEN
**        ALLOCATE VALUES DUMMY HEADER 
**        SET FIELDS TO NULL 
**        SET CUR_DR VALUES TO TMP_DV 
**        IF INTERCHANGE LEVEL IS NOT 1 { BUT 2 OR 3 } 
**        THEN
**           IF LABELS FIELD IS PRESENT 
**           THEN
**              SET CUR_SL OF CURRENT LEVEL TO FIRST LABEL SET 
**              SET VARIABLE SET TO FALSE 
**              WHILE LABEL SET HAS SINGLE VALUE AND NOT END OF SETS DO
**                 SET CUR_SL TO NXT_LSET 
**              ENDWHILE        
**              CALL FREE123LAB() TO FREE PREVIOUSLY ALLOCATED LABELS 
**               POINTER STRUCTURE
**              CALL SETUP123LB() TO SET UP LABELS POINTER STRUCTURE 
**           ENDIF
**           IF FORMATS FIELD PRESENT 
**           THEN
**              INITIALIZE FORMAT TREE POINTERS 
**              CALL GET123FMT() TO RETIEVE FORMAT STRUCTURE 
**           ENDIF
**        ENDIF   
**        SET NXT_VSET FIELD OF DR VALUES FIELD TO NEW_DV 
**        SET ROW_DVH TO NEW_DV
**     ELSE
**        IF FORMATS FIELD PRESENT 
**        THEN
**           CALL GET123FMT() TO RETIEVE FORMAT STRUCTURE 
**        ENDIF
**        IF LABELS FIELD PRESENT 
**        THEN
**           CALL INCRE123LAB() TO UPDATE LABELS POINTER STRUCTURE 
**        ENDIF
**        IF DATA VALUE IS ARRAY DATA 
**        THEN
**           IF LABELS PRESENT 
**           THEN
**              IF NXT_LAB POINTER OF CUR_SL IS NULL 
**              THEN
**                 SET NXT_VSET OF ROW_DVH TO NEW_DV
**                 SET ROW_DVH TO NEW_DV
**                 SET CUR_SL OF CURRENT LEVEL TO FIRST LABEL SET 
**                 SET VARIABLE SET TO FALSE 
**                 WHILE LABEL SET HAS SINGLE VALUE AND NOT END OF SETS DO
**                    SET CUR_SL TO NXT_LSET 
**                 ENDWHILE
**              ELSE
**                 SET CUR_SL TO NXT_LAB POINTER 
**                 SET NXT_VAL OF CUR_DV TO NEW_DV 
**              ENDIF
**           ELSE
**              IF NUMBER OF DIMENSIONS IN DR GREATER THAN ZERO
**              THEN
**                 SET PRIMARY DIMENSION TO LENGTH FIRST DR DIMENSION
**              ELSE IF NUMBER OF DIMENSIONS IN DDR GREATER THAN ZERO
**                 SET PRIMARY DIMENSION TO LENGTH FIRST DDR DIMENSION
**              ENDIF
**              SET VALUE COUNTER TO ZERO 
**              SET TEMPORARY DATA VALUE COLUMN POINTER TO ROW POINTER 
**              INITIALIZE FD_CURDV TO FALSE
**              WHILE THERE ARE MORE COLUMN VALUE ENTRIES AND CURRENT 
**               DATA VALUE NOT ENCOUNTERED DO
**                 IF TMP_VCOL EQUALS CUR_DV
**                 THEN
**                    SET FD_CURDV TO TRUE
**                 ENDIF
**                 IF CURRENT DATA VALUE NOT FOUND 
**                 THEN
**                    INCREMENT COUNTER 
**                    SET TMP_VCOL TO NXT_VAL FIELD 
**                 ENDIF
**              ENDWHILE
**              IF COUNTER LESS THE PRIMARY DIMENSION 
**              THEN
**                 SET CUR_DV NXT_VAL FIELD TO NEW_DV
**              ELSE
**                 SET ROW_DVH NXT FIELD TO NEW_DV 
**                 SET ROW_DVH TO NEW_DV
**              ENDIF
**           ENDIF
**        ELSE
**           SET CUR_DV NXT_VAL TO NEW_DV 
**        ENDIF
**     ENDIF
**     SET CUR_DV TO NEW_DV 
**
**     RETURN SUCCESS 
** 
*****************************************************************************
**    CODE SECTION
**
*****************************************************************************/
#include "stc123.h"

int stor123dv(char *wr_str,long str_len)


{
    /* INTERNAL VARIABLES */
    int     fd_curdv ,
            set      ;
    size_t  len      ;
    char    dtyp     ,
            delim    ;
    long    count    ,
            pdm      ,
            width    ;     
    struct dv *new_dv  ,
              *tmp_dv  ,
              *tmp_vcol;
              
    /* ALLOCATE NEW_DV */
    if ((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL) return(0);

    /* SET POINTERS TO NULL */
    new_dv->value = NULL;
    new_dv->nxt_vset = NULL;
    new_dv->nxt_val = NULL;
  
    /* SET LENGTH TO STRING LENGTH */
    len = (size_t) str_len + 1;
    
    /* IF WR_STR IS NOT EMPTY */
    if (len > 1) {

       /* ALLOCATE NEW_DV */
       if ((new_dv->value = (char *) malloc (len * sizeof(char))) == NULL)
        return(0);
       new_dv->value[str_len] = NC;

       /* SET VALUE TO WR_STR */
       memcpy(new_dv->value, wr_str, (size_t) str_len);
    };   
     
    /* IF VALUES STRUCTURE IS EMPTY */
    if (cur_fm->cur_dr->values == NULL) {

       /* ALLOCATE VALUES DUMMY HEADER */
       if ((tmp_dv = (struct dv *) malloc (sizeof(struct dv))) == NULL) return(0);
       
       /* SET FIELDS TO NULL */
       tmp_dv->value = NULL;
       tmp_dv->nxt_vset = NULL;
       tmp_dv->nxt_val = NULL;
       
       /* SET CUR_DR VALUES TO TMP_DV */
       cur_fm->cur_dr->values = tmp_dv;
       
       /* IF INTERCHANGE LEVEL IS NOT 1 { BUT 2 OR 3 } */
       if (cur_fm->dl_hd->ilevel != 1) {
          
          /* IF LABELS FIELD IS PRESENT */
          if (cur_fm->cur_dd->labels != NULL) {
       
             /* SET CUR_SL OF CURRENT LEVEL TO FIRST LABEL SET */
             cur_fm->cur_sl = cur_fm->cur_dd->labels->nxt_lset;

             /* SET VARIABLE SET TO FALSE */
             set = 0;
                     
             /* WHILE LABEL SET HAS SINGLE VALUE AND NOT END OF SETS 
                 DO 
             */
             while (cur_fm->cur_sl->nxt_lab == NULL 
              && cur_fm->cur_sl->nxt_lset != NULL) {
                      
                /* SET CUR_SL TO NXT_LSET */
                cur_fm->cur_sl = cur_fm->cur_sl->nxt_lset;
             }        
          
             /* CALL FREE123LAB() TO FREE PREVIOUSLY ALLOCATED LABELS 
                 POINTER STRUCTURE */
             if (!free123lab()) return(0);
          
             /* CALL SETUP123LB() TO SET UP LABELS POINTER STRUCTURE */
             if (!setup123lb()) return(0);
          
          }; 
          
          /* IF FORMATS FIELD PRESENT */
          if (cur_fm->cur_dd->fmt_rt != NULL) {
          
             /* INITIALIZE FORMAT TREE POINTERS */
             cur_fm->cur_fc = cur_fm->cur_dd->fmt_rt;
             cur_fm->cur_fcr = cur_fm->cur_dd->fmt_rt;
             
             /* CALL GET123FMT TO RETIEVE FORMAT STRUCTURE */
             if (!get123fmt(&dtyp,&width,&delim)) return(0);
          }   
       }; 
        
       /* SET NXT_VSET FIELD OF DR VALUES FIELD TO NEW_DV */
       cur_fm->cur_dr->values->nxt_vset = new_dv;

       /* SET ROW_DVH TO NEW_DV */
       cur_fm->row_dvh = new_dv;   
    }   
    else {
       
       /* IF FORMATS FIELD PRESENT */
       if (cur_fm->cur_dd->fmt_rt != NULL) {
          
          /* CALL GET123FMT TO RETIEVE FORMAT STRUCTURE */
          if (!get123fmt(&dtyp,&width,&delim)) return(0);
       };   

       /* IF LABELS FIELD PRESENT */
       if (cur_fm->cur_dd->labels != NULL) {
       
          /* CALL INCRE123LAB TO UPDATE LABELS POINTER STRUCTURE */
          if (!incre123lab(cur_fm->lp_hd->next->next,
                cur_fm->lp_hd->next->first,&cur_fm->lp_hd->next->cur))
                return(0);  
       };  

       /* IF DATA VALUE IS ARRAY DATA */
       if (cur_fm->cur_dd->fd_cntrl[FCDSTYPE] == '2') {

          /* IF LABELS PRESENT */
          if (cur_fm->cur_dd->labels != NULL) {
        
             /* IF NXT_LAB POINTER OF CUR_SL IS NULL */
             if (cur_fm->cur_sl->nxt_lab == NULL) {

                /* SET NXT_VSET OF ROW_DVH TO NEW DV */
                cur_fm->row_dvh->nxt_vset = new_dv;
                
                /* SET ROW_DVH TO NEW_DV */                
                cur_fm->row_dvh = new_dv;
 
                /* SET CUR_SL OF CURRENT LEVEL TO FIRST LABEL SET */
                cur_fm->cur_sl = cur_fm->cur_dd->labels->nxt_lset;

                /* SET VARIABLE SET TO FALSE */
                set = 0;
                     
                /* WHILE LABEL SET HAS SINGLE VALUE AND NOT END OF SETS 
                    DO 
                */
                while (cur_fm->cur_sl->nxt_lab == NULL 
                 && cur_fm->cur_sl->nxt_lset != NULL) {
                        
                    /* SET CUR_SL TO NXT_LSET */
                    cur_fm->cur_sl = cur_fm->cur_sl->nxt_lset;
                }
                             
             }  
             else {

                /* SET CUR_SL TO NXT_LAB POINTER */
                cur_fm->cur_sl = cur_fm->cur_sl->nxt_lab;
   
                /* SET NXT_VAL OF CUR_DV TO NEW_DV */
                cur_fm->cur_dv->nxt_val = new_dv;
  
             };
 
          }
          else {
                  
             /* IF NUMBER OF DIMENSIONS IN DR GREATER THAN ZERO */
             if (cur_fm->cur_dr->num_dim > 0) {

                /* SET PRIMARY DIMENSION TO LENGTH FIRST DR DIMENSION */
                pdm = cur_fm->cur_dr->dim_lptr->nxt->len;
             }
 
             /* ELSE IF NUMBER OF DIMENSIONS IN DDR GREATER THAN ZERO */
             else if (cur_fm->cur_dd->num_dim > 0) {

                /* SET PRIMARY DIMENSION TO LENGTH FIRST DDR DIMENSION */
                pdm = cur_fm->cur_dd->dim_lptr->nxt->len;
             }
                        
             /* INITIALIZE VALUE COUNTER TO ZERO */
             count = 0;
                           
             /* SET TEMPORARY DATA VALUE COLUMN POINTER TO 
                 ROW POINTER 
             */
             tmp_vcol = cur_fm->row_dvh;
                          
             /* INITIALIZE FD_CURDV TO FALSE */
             fd_curdv = 0;
               
             /* WHILE THERE ARE MORE COLUMN VALUE ENTRIES AND 
                 CURRENT DATA VALUE NOT ENCOUNTERED DO
             */
             while ((tmp_vcol != NULL) && (!fd_curdv)) {
                            
                /* SET FD_CURDV TRUE IF TMP_VCOL EQUALS CUR_DV 
                    FALSE OTHERWISE
                   IF CURRENT DATA VALUE NOT FOUND 
                */                                     
                if (!(fd_curdv = (cur_fm->cur_dv == tmp_vcol))) {
                              
                   /* INCREMENT COUNTER */
                   count++;
                                 
                   /* SET TMP_VCOL TO NXT_VAL FIELD */
                   tmp_vcol = tmp_vcol->nxt_val;
                };
             };
                            
             /* IF COUNTER LESS THE PRIMARY DIMENSION */
             if (count < pdm) {
                                 
                /* SET CUR_DV NXT_VAL FIELD TO NEW_DV*/
                cur_fm->cur_dv->nxt_val = new_dv;
             }
             else {
                                 
                /* SET ROW_DVH NXT FIELD TO NEW_DV */
                cur_fm->row_dvh->nxt_vset = new_dv;

                /* SET ROW_DVH TO NEW_DV */
                cur_fm->row_dvh = new_dv;
             };
                
          }; 
       }    
       else {
                     
          /* SET CUR_DV NXT_VAL TO NEW_DV */
          cur_fm->cur_dv->nxt_val = new_dv;
     
       };
                                 
    };   
               
    /* SET CUR_DV TO NEW_DV */
    cur_fm->cur_dv = new_dv;
    
    /* RETURN SUCCESS */
    return(1);
    
}
