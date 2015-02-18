# include "stdafx.h"
/********************+*******************************************************
** 
**    INVOCATION NAME: I123TOA
** 
**    PURPOSE: TO CONVERT AN INTEGER INTO A CHARACTER STRING
** 
**    INVOCATION METHOD: I123TOA(NUM,STRING)
** 
**    Since sprintf() is an ansii standard function, us it !!
**
**     RETURN SUCCESS 
** 
****************************************************************************** 
**    CODE SECTION 
**
*****************************************************************************/
#include "stc123.h"

int i123toa(long num,char *string)
{
   sprintf(string, "%ld", num);

   /* RETURN SUCCESS */
   return(1);
}
