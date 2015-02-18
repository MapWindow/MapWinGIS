# include "stdafx.h"
/***************************************************************************
**
**    INVOCATION NAME: RET123FV
**
**    PURPOSE: TO INPUT FORMATTED DATA VALUES AND STORE IN STRUCTURE 
**
**    INVOCATION METHOD: RET123FV(IN_STR,IN_STR_END,PRIM_DMS,FLD_LEN,COMPRSSD)
**
**    ARGUMENT LIST:
**     NAME       TYPE      USE      DESCRIPTION
**     IN_STR[]   PTR        I       POINTER TO INPUT CHARACTER STRING
**     IN_STR_END[] PTR      I       POINTER TO END OF INPUT CHARACTER STRING
**     PRIM_DMS   INT        I       PRIMARY DIMENSION LENGTH
**     FLD_LEN    LONG       I       LENGTH OF FIELD
**     COMPRSSD   INT        I       INDICATOR FOR COMPRESSED ADJACENT
**                                    FIXED-LENGTH BINARY SUBFIELDS
**     RET123FV() LOGICAL    O       SUCCESS FLAG
**     
**    EXTERNAL FUNCTION REFERENCES:
**     NAME             DESCRIPTION
**     GET123DVAL()     FUNCTION TO RETRIEVE DATA VALUE
**     GET123FMT()      FUNCTION TO RETRIEVE FORMAT CONTROL
**
**    INTERNAL VARIABLES:
**     NAME         TYPE               DESCRIPTION
**     END_OF_FLD   INT                END OF FIELD FLAG
**     F_DELIM      CHAR               FORMATTED DELIMITER
**     F_DTYP       CHAR               FORMATTED DATA TYPED
**     F_WIDTH      INT                FORMATTED WIDTH
**     LEN          INT                LENGTH OF TEMPORARY VALUE STRING
**     NEW_DV       PTR                POINTER TO DATA VALUE STRUCTURE
**     SAVE_IN_STR  PTR                SAVED POINTER TO INPUT STRING
**     TMP_CPTR[]   PTR                TEMPORARY CHARACTER POINTER
**     VAL_CT       INT                NUMBER OF CURRENT VALUES IN SET
**  
**    GLOBAL REFERENCES:
**
**     DATA RECORD STRUCTURE (DR)
**      NAME            TYPE   USE   DESCRIPTION
**      TAG[10]         CHAR   N/A   INTERNAL NAME OF AN ASSOCIATED FIELD
**      FD_LEN          INT    N/A   LENGTH OF DISCRIPTIVE AREA DATA RECORD 
**      FD_POS          INT    N/A   POSITION OF DESCRIPTIVE AREA DATA
**                                    RECORD
**      NUM_DIM         INT    N/A   NUMBER OF DIMENSIONS (NO LABELS)
**      DIM_LPTR        PTR    N/A   HEAD POINTER TO DIMENSION LENGTHS
**                                    (NO LABELS)
**      VALUES          PTR     O    HEAD POINTER TO DATA VALUE SUBFIELD
**                                    RECORDS
**      NEXT            PTR    N/A   POINTER TO NEXT DATA RECORD 
**
**     DR DATA VALUE SUBFIELD STRUCTURE (DV)
**      NAME            TYPE   USE   DESCRIPTION
**      VALUE           CHAR    O    DATA VALUE
**      NXT_VSET        PTR     O    POINTER TO NEXT SET OF DATA VALUES
**      NXT_VAL         PTR     O    POINTER TO NEXT DATA VALUE SUBFIELD 
**                                    RECORD
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
**      CUR_DR          PTR    I/O   CURRENT POINTER TO DATA RECORD STRUCTURE
**                                    ENTRY
**      CUR_DV          PTR     O    CURRENT POINTER TO DR DATA VALUE SUBFIELD
**                                    STRUCTURE ENTRY
**      ROW_DVH         PTR     O    CURRENT POINTER TO NEXT SET OF VALUES
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
**    GLOBAL VARIABLES:
**     NAME             TYPE   USE   DESCRIPTION
**     CUR_FM           PTR     I    CURRENT POINTER TO FILE MANAGEMENT
**                                    STRUCTURE ENTRY
**     GLB_STR[MAXSIZ]  CHAR   I/O   GLOBAL CHARACTER STRING USED FOR 
**                                    PROCESSING 
**
**    GLOBAL CONSTANTS:
**     NAME       TYPE      DESCRIPTION
**     FT         CHAR      FIELD TERMINATOR (RS) 1/14
**     MAXSIZ     INT       MAXIMUM AMOUNT OF CONTIGUOUS MEMORY SPACE
**     NC         CHAR      NULL CHARACTER
**
**    CHANGE HISTORY:
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY
**     A. DEWITT                 04/23/90  INITIAL PROLOG
**     A. DEWITT                 04/23/90  INITIAL PDL
**     P. HODGES                 06/04/90  INITIAL CODE
**     A. DEWITT                 08/21/90  INCLUDE LOGIC/CODE TO HANDLE
**                                          FINAL SUBFIELD HAVING A NULL VALUE
**
**     A. DEWITT                 09/09/90  REMOVE SEEK LOGIC TO ALLEVIATE  EOF
**                                          PROBLEM. 
**     A. DEWITT     92DR005     04/13/92  IMPLEMENATION OF BINARY DATA
**     L. MCMILLION  92DR005     06/04/92  ADDED COMPRESSED ARGUMENT FOR
**                                          FIXED-LENGTH BIT FIELD RETRIEVAL
**     L. MCMILLION  TASK #40    12/09/92  PROLOG/PDL/INLINE COMMENTS UPDATE
**     J. TAYLOR     93DR023     04/21/93  MODIFIED TO RETRIEVE FROM CHARACTER
**                                          RATHER THAN FILE
**     J. TAYLOR     93DR034     05/03/93  MODIFIED CODE TO DETECT END OF
**                                          BINARY FIELDS
**     J. TAYLOR     93DR034     05/07/93  ADDED FIELD LENGTH PARAMETER
**     J. TAYLOR     93DR031     06/14/93  ADDED PRIM_DMS TO END OF FIELD
**                                          CHECK FOR BINARY FIELDS
**     J. TAYLOR     93DR037     07/28/93  REMOVED MEMSET CALLS
**     J. TAYLOR     93DR039     07/28/93  REMOVED MEMSET CALLS
**                                         
**    PDL:
**
**     SAVE POINTER TO INPUT STRING
**     ALLOCATE NEW_DV  { SET UP DUMMY HEADER }
**     SET VALUE POINTER TO NULL
**     SET NXT_VAL FIELD OF NEW_DV TO NULL
**     SET NXT_VSET FIELD OF NEW_DV TO NULL
**     SET VALUES OF CUR_DR TO NEW_DV
**     SET ROW_DVH TO NEW_DV
**     INITIALIZE END OF FIELD FLAG
**     WHILE NOT END OF FIELD DO
**        CALL GET123FMT() TO RETRIEVE FORMAT OF DATA VALUE
**        CLEAR GLOBAL STRING
**        CALL GET123DVAL() TO RETRIEVE DATA VALUE STRING
**        ALLOCATE NEW_DV
**        SET VALUE POINTER TO NULL
**        SET NXT_VAL TO NULL
**        SET NXT_VSET TO NULL
**        IF DATA TYPE NOT BINARY
**        THEN
**           CALL STRCHR TO RETURN POINTER TO FT IN STRING
**           IF FT IN STRING
**           THEN
**              SET END OF FIELD FLAG
**           ENDIF 
**           IF TMP_CPTR NOT NULL
**           THEN
**              REPLACE FT WITH NULL CHARACTER
**           ENDIF
**           GET LENGTH OF GLOBAL STRING CONTAINING VALUE
**           IF A VALUE IS PRESENT
**           THEN
**              ALLOCATE SPACE FOR VALUE STRING
**              INITIALIZE STRING SPACE
**              COPY GLB_STR TO NEWLY ALLOCATED SPACE
**           ELSE NO VALUE PRESENT
**              ASSIGN VALUE POINTER TO NULL
**           ENDIF   
**        ELSE BINARY DATA
**           SET LEN TO WIDTH OF BINARY DATA
**           ALLOCATE SPACE FOR VALUE STRING
**           COPY GLB_STR TO NEW_DV VALUE
**           TERMINATE NEW_DV VALUE WITH NULL CHARACTER
**        ENDIF
**        SET NXT_VSET OF ROW_DVH TO NEW_DV
**        SET CUR_DV TO NEW_DV
**        SET ROW_DVH TO NEW_DV
**        IF NOT END OF FIELD
**        THEN
**           IF END OF INPUT STRING
**           THEN
**              SET END OF FIELD FLAG TO TRUE
**           ELSE
**              IF CHARACTER IS FIELD TERMINATOR AND DATA VALUE IS 
**               NOT DELIMITED AND DIMENSION IS 1
**              THEN
**                 SET END OF FIELD FLAG TO TRUE
**                 MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR
**              ENDIF   
**           ENDIF
**        ENDIF                 
**        SET VAL_CT TO 1
**        WHILE NOT RETRIEVED A FIELD TERMINATOR 
**         AND VAL_CT LESS THAN PRIM_DMS DO
**           CALL GET123FMT() TO RETRIEVE FORMAT OF DATA VALUE
**           CLEAR STRING
**           CALL GET123DVAL() TO RETRIEVE DATA VALUE STRING
**           ALLOCATE NEW_DV
**           SET VALUE POINTER TO NULL
**           SET NXT_VAL TO NULL
**           SET NXT_VSET TO NULL
**           IF DATA TYPE NOT BINARY
**           THEN
**              CALL STRCHR TO RETURN POINTER TO FT IN STRING
**              IF FT IN STRING
**              THEN
**                 SET END OF FIELD FLAG
**              ENDIF 
**              IF TMP_CPTR NOT NULL
**              THEN
**                 REPLACE FT WITH NULL CHARACTER
**              ENDIF
**              GET LENGTH OF GLOBAL STRING CONTAINING VALUE
**              IF A VALUE IS PRESENT
**              THEN
**                 ALLOCATE SPACE FOR VALUE STRING
**                 INITIALIZE STRING SPACE
**                 COPY GLB_STR TO NEWLY ALLOCATED SPACE
**              ELSE NO VALUE PRESENT
**                 ASSIGN VALUE POINTER TO NULL
**              ENDIF      
**           ELSE BINARY DATA
**              SET LEN TO WIDTH OF BINARY DATA
**              ALLOCATE SPACE FOR VALUE STRING
**              COPY GLB_STR TO NEW_DV VALUE
**              TERMINATE NEW_DV VALUE WITH NULL CHARACTER
**           ENDIF
**           SET NXT_VAL OF CUR_DV TO NEW_DV
**           SET CUR_DV TO NEW_DV
**           IF NOT END OF FIELD
**           THEN
**              IF END OF INPUT STRING
**              THEN
**                 SET END OF FIELD FLAG TO TRUE
**              ELSE
**                 IF DATA TYPE IS BINARY AND HAVE RETRIEVED ENTIRE FIELD
**                 THEN
**                    SET END OF FIELD FLAG TO TRUE
**                    MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR
**                 ELSE IF DATA TYPE IS NOT BINARY CHARACTER IS 
**                  FIELD TERMINATOR AND DATA VALUE IS NOT DELIMITED 
**                  AND HAVE NOT PROCESSED ENTIRE DIMENSION
**                 THEN
**                    SET END OF FIELD FLAG TO TRUE
**                    MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR
**                 ENDIF   
**              ENDIF
**           ENDIF
**           INCREMENT VAL_CT
**        ENDWHILE
**     ENDWHILE
**
**     RETURN SUCCESS
**
******************************************************************************
**    CODE SECTION
**
******************************************************************************/
#include "stc123.h"

int ret123fv(char **in_str,char *in_str_end,long prim_dms,long fld_len,int comprssd)

{
  /* INTERNAL VARIABLES */
  int    end_of_fld ;
  char   f_delim, f_dtyp     ;
  long   f_width, val_ct     ;
  char * tmp_cptr   ;
  size_t len        ;
  struct dv *new_dv ;
  char *save_in_str ;

  /* SAVE POINTER TO INPUT STRING */
  save_in_str = *in_str;
  
  /* ALLOCATE NEW_DV  { SET UP DUMMY HEADER } */
  if ((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL) return(0);
  
  /* SET VALUE POINTER TO NULL */
  new_dv->value = NULL;
  
  /* SET NXT_VAL FIELD OF NEW_DV TO NULL */
  new_dv->nxt_val = NULL;

  /* SET NXT_VSET FIELD OF NEW_DV TO NULL */
  new_dv->nxt_vset = NULL;
  
  /* SET VALUES OF CUR_DR TO NEW_DV */
  cur_fm->cur_dr->values = new_dv;
  
  /* SET ROW_DVH TO NEW_DV */
  cur_fm->row_dvh = new_dv;
  
  /* INITIALIZE END OF FIELD FLAG TO FALSE */
  end_of_fld = 0;


  /* WHILE HAVE NOT RETRIEVED A FIELD TERMINATOR AND VAL_CT LESS THAN
       PRIM_DMS DO
       */
 /* WHILE NOT RETRIEVED LAST VALUE IN A FIELD DO */
  while(!end_of_fld) 
  {
    /* SET VAL_CT TO 0 */
    val_ct = 0;
    
    while( val_ct < prim_dms && !end_of_fld ) 
    {
      
      /* CALL GET123FMT() TO RETRIEVE FORMAT OF DATA VALUE */
      if (!get123fmt(&f_dtyp,&f_width,&f_delim)) return(0);
      
      /* CLEAR STRING */
      *glb_str = NC;
      
      /* CALL GET123DVAL() TO RETRIEVE IN DATA VALUE STRING */
      if (!get123dval(in_str,in_str_end,f_dtyp,&f_width,f_delim,
		      comprssd,glb_str))
	return(0);
      
    /* ALLOCATE NEW_DV */
      if ((new_dv = (struct dv *) malloc(sizeof(struct dv))) == NULL)
	return(0);
      
    /* SET VALUE POINTER TO NULL */
      new_dv->value = NULL;
      
      /* SET NXT_VAL TO NULL */
      new_dv->nxt_val = NULL;
      
      /* SET NXT_VSET TO NULL */
      new_dv->nxt_vset = NULL;
	 
      /* IF DATA TYPE NOT BINARY */
      if (f_dtyp != 'B') 
      {
	
	/* CALL STRCHR TO RETURN POINTER TO FT IN STRING IF PRESENT OTHERWISE
	   ASSIGNS NULL VALUE TO TMP_CPTR.
	   */    
	tmp_cptr = strchr(glb_str,FT);
	
	/* SET END OF FIELD FLAG IF FT IS IN STRING */
	end_of_fld = (tmp_cptr != NULL);
	
	/* IF TMP_CPTR NOT NULL REPLACE FT WITH NULL CHARACTER */
	if (tmp_cptr != NULL) *tmp_cptr = NC;
	
	/* GET LENGTH OF GLOBAL STRING CONTAINING VALUE */
	len = _tcslen(glb_str) + 1;
	
	/* IF A VALUE IS PRESENT */
	if (len > 1) 
	{
	  
	  /* ALLOCATE SPACE FOR VALUE STRING */
	  if ((new_dv->value = (char *) malloc(len * sizeof(char))) == NULL) 
	    return(0);
	  
	  /* INITIALIZE STRING SPACE */
	  new_dv->value[0] = NC;
	  
	  /* COPY GLB_STR TO NEWLY ALLOCATED SPACE */
	  strcpy(new_dv->value,glb_str);
	}
	/* NO VALUE PRESENT */
	else 
	{
	  /* ASSIGN VALUE POINTER TO NULL */
	  new_dv->value = NULL;
	};      
      }
      /* ELSE BINARY DATA */
      else 
      {
	
	/* SET LEN TO WIDTH OF BINARY DATA */
	len = (size_t) f_width + 1;

	/* ALLOCATE SPACE FOR VALUE STRING */
	if ((new_dv->value = (char *) malloc(len * sizeof(char))) == NULL)
	  return(0);
	
	/* COPY GLB_STR TO NEW_DV VALUE */
	memcpy(new_dv->value,glb_str,--len);
        
	/* TERMINATE NEW_DV VALUE WITH NULL CHARACTER */
	new_dv->value[len] =NC;
      };
      
      if ( val_ct == 0 )
      {
	cur_fm->row_dvh->nxt_vset = new_dv; /* SET NXT_VSET OF ROW_DVH TO NEW_DV */
	cur_fm->cur_dv = new_dv;	/* SET CUR_DV TO NEW_DV */
	cur_fm->row_dvh = new_dv; /* SET ROW_DVH TO NEW_DV */
      }
      else
      {
	cur_fm->cur_dv->nxt_val = new_dv; /* SET NXT_VAL OF CUR_DV TO NEW_DV */
	cur_fm->cur_dv = new_dv;	/* SET CUR_DV TO NEW_DV */
      }

      
      /* IF NOT END OF FIELD */
      if (!end_of_fld) 
      {
         
	/* IF END OF INPUT STRING */
	if (*in_str >= in_str_end) 
	{
         
	  /* SET END OF FIELD FLAG TO TRUE */
	  end_of_fld = 1;
	}
	else 
	{
	
	    /* IF DATA TYPE IS BINARY AND HAVE RETRIEVED ENTIRE FIELD */
	    if (f_dtyp == 'B' && (*in_str - save_in_str + 1) == fld_len && 
		val_ct == prim_dms-1 ) 
	    {
	  
	      /* SET END OF FIELD FLAG TO TRUE */     
	      end_of_fld = 1;
	  
	      /* MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR */
	      *in_str = *in_str + 1;
	  
	    }
	    /* ELSE IF DATA TYPE IS NOT BINARY CHARACTER IS 
	       FIELD TERMINATOR AND DATA VALUE IS NOT DELIMITED 
	       AND HAVE NOT PROCESSED ENTIRE DIMENSION
	       */
	    else if (f_dtyp != 'B' && *in_str[0] == FT && f_width != 0 && 
		     val_ct == prim_dms-1  ) 
	    {
	  
	      /* SET END OF FIELD FLAG TO TRUE */     
	      end_of_fld = 1;
	  
	      /* MOVE BEGINNING OF INPUT STRING PAST FIELD TERMINATOR */
	      *in_str = *in_str + 1;
	    } 
	};
      };                 
      /* INCREMENT VAL_CT */
      val_ct++;
    };
};
  
  /* RETURN SUCCESS */
  return(1);
}
