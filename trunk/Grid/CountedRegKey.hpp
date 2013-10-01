#ifndef __COUNTED_REG_KEY__
#define __COUNTED_REG_KEY__
///////////////////////////////////////////////////////////////////////////////
//
// File           : $Workfile:   countedregkey.hpp  $
// Version        : $Revision: 1.1.1.1 $
// Function       : 
//
// Author         : $Author: amesdani $
// Date           : $Date: 2005/05/02 19:34:11 $
//
// Notes          : 
//
// Modifications  :
//
// $Log: CountedRegKey.hpp,v $
// Revision 1.1.1.1  2005/05/02 19:34:11  amesdani
// no message
//
// Revision 1.1.1.1  2002/09/05 21:54:57  jonathan
// Initial Version
//
// Revision 1.1.1.1  2002/04/01 21:15:10  BrianS
// no message
//
// 
//    Rev 1.0   Aug 27 1998 07:44:10   len
// Initial revision.
// 
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1998 JetByte Limited.
//
// JetByte Limited grants you ("Licensee") a non-exclusive, royalty free, 
// licence to use, modify and redistribute this software in source and binary 
// code form, provided that i) this copyright notice and licence appear on all 
// copies of the software; and ii) Licensee does not utilize the software in a 
// manner which is disparaging to JetByte Limited.
//
// This software is provided "AS IS," without a warranty of any kind. ALL
// EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING 
// ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
// OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED. JETBYTE LIMITED AND ITS LICENSORS 
// SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF 
// USING, MODIFYING OR DISTRIBUTING THE SOFTWARE OR ITS DERIVATIVES. IN NO 
// EVENT WILL JETBYTE LIMITED BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, 
// OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE 
// DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING 
// OUT OF THE USE OF OR INABILITY TO USE SOFTWARE, EVEN IF JETBYTE LIMITED 
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// This software is not designed or intended for use in on-line control of
// aircraft, air traffic, aircraft navigation or aircraft communications; or in
// the design, construction, operation or maintenance of any nuclear
// facility. Licensee represents and warrants that it will not use or
// redistribute the Software for such purposes.
//
///////////////////////////////////////////////////////////////////////////////

#pragma warning(disable: 4201)   // nameless struct/union

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#pragma warning(default: 4201)

#ifndef __REFERENCE_COUNTED__
#include "ReferenceCounted.hpp"
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {

///////////////////////////////////////////////////////////////////////////////
// Classes defined in this file...
///////////////////////////////////////////////////////////////////////////////

class CCountedRegKey;

///////////////////////////////////////////////////////////////////////////////
// CCountedRegKey
///////////////////////////////////////////////////////////////////////////////

class CCountedRegKey : public TReferenceCounted<HKEY, CCountedRegKey>
{
   public :

      explicit CCountedRegKey(const HKEY hKey)
         : TReferenceCounted<HKEY, CCountedRegKey>(hKey)
      {
      }


   protected : 

      virtual void PreRelease()
      {
         RegCloseKey(m_counted);
      }

      ~CCountedRegKey()
      {
      }

   private :

      // Restrict copying... Do not implement...

      CCountedRegKey(const CCountedRegKey &rhs);   
      CCountedRegKey &operator=(const CCountedRegKey &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools
///////////////////////////////////////////////////////////////////////////////

} // End of namespace JetByteTools 

#endif // __COUNTED_REG_KEY__

///////////////////////////////////////////////////////////////////////////////
// End of file...
///////////////////////////////////////////////////////////////////////////////
