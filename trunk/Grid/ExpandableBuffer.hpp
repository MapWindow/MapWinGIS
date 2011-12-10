#ifndef __EXPANDABLE_BUFFER__
#define __EXPANDABLE_BUFFER__
///////////////////////////////////////////////////////////////////////////////
//
// File           : $Workfile:   ExpandableBuffer.hpp  $
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
// $Log: ExpandableBuffer.hpp,v $
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
//    Rev 1.1   Jun 06 1998 07:48:38   Len
// 
//    Rev 1.0   May 18 1998 07:34:22   Len
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

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {

///////////////////////////////////////////////////////////////////////////////
// Templates defined in this file...
///////////////////////////////////////////////////////////////////////////////

template <class T> class TExpandableBuffer;

///////////////////////////////////////////////////////////////////////////////
// TExpandableBuffer
///////////////////////////////////////////////////////////////////////////////

template <class T> class TExpandableBuffer
{
   public :

      // Construction and destruction

      TExpandableBuffer(size_t intialSize = 0);
      TExpandableBuffer(const TExpandableBuffer &rhs);

      ~TExpandableBuffer();

      TExpandableBuffer &operator=(const TExpandableBuffer &rhs);

      // Access functions 

      T *GetBuffer() const; 
      operator T *() const; 

      size_t GetSize() const;

      // Make the buffer bigger 

      size_t Resize(size_t newSize);    // Destructive

      size_t Expand(size_t newSize);    // Non destructive

      T *ReleaseBuffer();

   private :

      T *m_pBuffer;

      size_t m_size;
};

///////////////////////////////////////////////////////////////////////////////
// Construction and destruction
///////////////////////////////////////////////////////////////////////////////

template <class T> 
TExpandableBuffer<T>::TExpandableBuffer(size_t initialSize) 
   : m_pBuffer(0), m_size(0) 
{ 
   Resize(initialSize); 
}

template <class T> 
TExpandableBuffer<T>::TExpandableBuffer(const TExpandableBuffer<T> &rhs) 
   : m_pBuffer(0), m_size(0) 
{ 
   Resize(rhs.m_size); 

   for (size_t i = 0; i < m_size; i++)
   {
      m_pBuffer[i] = rhs.m_pBuffer[i];
   }
}

template <class T>
TExpandableBuffer<T>::~TExpandableBuffer() 
{ 
   delete[] m_pBuffer; 
}

template <class T>
TExpandableBuffer<T> &TExpandableBuffer<T>::operator=(const TExpandableBuffer<T> &rhs)
{
   if (this != &rhs)
   {
      if (m_size != rhs.m_size)
      {
         T *pNewBuffer = new T[rhs.m_size];

         delete[] m_pBuffer;

         m_pBuffer = pNewBuffer;

         m_size = rhs.m_size;
      }

      for (size_t i = 0; i < m_size; i++)
      {
         m_pBuffer[i] = rhs.m_pBuffer[i];
      }
   }

   return *this;
}


///////////////////////////////////////////////////////////////////////////////
// Access functions
///////////////////////////////////////////////////////////////////////////////

template <class T> 
T *TExpandableBuffer<T>::GetBuffer() const 
{ 
   return m_pBuffer; 
}

template <class T> 
TExpandableBuffer<T>::operator T *() const 
{ 
   return GetBuffer(); 
}

template <class T> 
size_t TExpandableBuffer<T>::GetSize() const 
{ 
   return m_size; 
}

template <class T> 
T *TExpandableBuffer<T>::ReleaseBuffer()
{ 
   T *pBuffer = m_pBuffer;

   m_pBuffer = 0;
   m_size = 0;

   return pBuffer; 
}

///////////////////////////////////////////////////////////////////////////////
// Make the buffer bigger
///////////////////////////////////////////////////////////////////////////////

template <class T> 
size_t TExpandableBuffer<T>::Resize(size_t newSize)
{
   if (m_size < newSize)
   {
      T *pNewBuffer = new T[newSize];
      
      delete[] m_pBuffer;

      m_pBuffer = pNewBuffer;

      m_size = newSize;
   }
   
   return m_size;
}

template <class T> 
size_t TExpandableBuffer<T>::Expand(size_t newSize)
{
   if (m_size < newSize)
   {
      T *pNewBuffer = new T[newSize];

      for (size_t i = 0; i < newSize; i++)
      {
         pNewBuffer[i] = m_pBuffer[i];
      }

      delete[] m_pBuffer;

      m_pBuffer = pNewBuffer;

      m_size = newSize;
   }
   
   return m_size;
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools
///////////////////////////////////////////////////////////////////////////////

} // End of namespace JetByteTools 

#endif // __EXPANDABLE_BUFFER__

///////////////////////////////////////////////////////////////////////////////
// End of file...
///////////////////////////////////////////////////////////////////////////////
