# include "stdafx.h"
/****************************************************************************
** 
**    INVOCATION NAME: E123TOA
** 
**    PURPOSE: TO CONVERT A CHARACTER STRING FROM THE EBCDIC CHARACTER
**              SET TO THE ASCII CHARACTER SET
** 
**    INVOCATION METHOD: E123TOA(STRING)
** 
**    ARGUMENT LIST: 
**     NAME          TYPE      USE      DESCRIPTION
**     STRING[]      PTR       I/O      CHARACTER STRING
**     E123TOA()     LOGICAL    O       SUCCESS FLAG
**
**    EXTERNAL FUNCTION REFERENCES: NONE
** 
**    INTERNAL VARIABLES:
**     NAME          TYPE               DESCRIPTION
**     ASCII[256]    CHAR               EBCDIC TO ASCII CONVERSION TABLE
**     I             INT                LOOP CONTROL VARIABLE
**   
**    GLOBAL REFERENCES: NONE
**
**    GLOBAL VARIABLES: NONE
** 
**    GLOBAL CONSTANTS: NONE
**
**    CHANGE HISTORY: 
**     AUTHOR        CHANGE_ID     DATE    CHANGE SUMMARY 
**     J. TAYLOR     TASK 55     01/07/93  INITIAL PROLOG
**     J. TAYLOR     TASK 55     01/07/93  INITIAL PDL
**     J. TAYLOR     TASK 55     01/07/93  INITIAL CODE
**     L. MCMILLION  TASK 26     09/13/93  INSERTED (INT) CAST OPERATOR IN
**                                          CONVERSION ASSIGNMENT
** 
**    PDL: 
**
**     DO FOR EACH CHARACTER IN STRING
**        SET EBCDIC CHARACTER TO ASCII EQUIVELENT IN TABLE
**     ENDDO
**       
**     RETURN SUCCESS 
** 
****************************************************************************** 
**    CODE SECTION 
**
*****************************************************************************/
#include "stc123.h"

int e123toa(char *string)
{
   /* INTERNAL VARIABLES */
   int i;
   static int ascii[256] = {0,
                            1,
                            2,
                            3,
                            156,
                            9,
                            134,
                            127,
                            151,
                            141,
                            142,
                            11,
                            12,
                            13,
                            14,
                            15,
                            16,
                            17,
                            18,
                            19,
                            157,
                            133,
                            8,
                            135,
                            24,
                            25,
                            146,
                            143,
                            28,
                            29,
                            30,
                            31,
                            128,
                            129,
                            130,
                            131,
                            132,
                            10,
                            23,
                            27,
                            136,
                            137,
                            138,
                            139,
                            140,
                            5,
                            6,
                            7,
                            144,
                            145,
                            22,
                            147,
                            148,
                            149,
                            150,
                            4,
                            152,
                            153,
                            154,
                            155,
                            20,
                            21,
                            158,
                            26,
                            32,
                            160,
                            161,
                            162,
                            163,
                            164,
                            165,
                            166,
                            167,
                            168,
                            91,
                            46,
                            60,
                            40,
                            43,
                            33,
                            38,
                            169,
                            170,
                            171,
                            172,
                            173,
                            174,
                            175,
                            176,
                            177,
                            33,
                            36,
                            42,
                            41,
                            59,
                            94,
                            45,
                            47,
                            178,
                            179,
                            180,
                            181,
                            182,
                            183,
                            184,
                            185,
                            124,
                            44,
                            37,
                            95,
                            62,
                            63,
                            186,
                            187,
                            188,
                            189,
                            190,
                            191,
                            192,
                            193,
                            194,
                            96,
                            58,
                            35,
                            64,
                            39,
                            61,
                            34,
                            195,
                            97,
                            98,
                            99,
                            100,
                            101,
                            102,
                            103,
                            104,
                            105,
                            196,
                            197,
                            198,
                            199,
                            200,
                            201,
                            202,
                            106,
                            107,
                            108,
                            109,
                            110,
                            111,
                            112,
                            113,
                            114,
                            203,
                            204,
                            205,
                            206,
                            207,
                            208,
                            209,
                            126,
                            115,
                            116,
                            117,
                            118,
                            119,
                            120,
                            121,
                            122,
                            210,
                            211,
                            212,
                            213,
                            214,
                            215,
                            216,
                            217,
                            218,
                            219,
                            220,
                            221,
                            222,
                            223,
                            224,
                            225,
                            226,
                            227,
                            228,
                            229,
                            230,
                            231,
                            123,
                            65,
                            66,
                            67,
                            68,
                            69,
                            70,
                            71,
                            72,
                            73,
                            232,
                            233,
                            234,
                            235,
                            236,
                            237,
                            125,
                            74,
                            75,
                            76,
                            77,
                            78,
                            79,
                            80,
                            81,
                            82,
                            238,
                            239,
                            240,
                            241,
                            242,
                            243,
                            92,
                            159,
                            83,
                            84,
                            85,
                            86,
                            87,
                            88,
                            89,
                            90,
                            244,
                            245,
                            246,
                            247,
                            248,
                            249,
                            48,
                            49,
                            50,
                            51,
                            52,
                            53,
                            54,
                            55,
                            56,
                            57,
                            250,
                            251,
                            252,
                            253,
                            254,
                            255};


   /* DO FOR EACH CHARACTER IN STRING */
   for (i=0;i<(int)_tcslen(string);i++) {

      /* SET EBCDIC CHARACTER TO ASCII EQUIVELENT IN TABLE */
      string[i] = ascii[(int)string[i]];

   }

   /* RETURN SUCCESS */
   return(1);
}
