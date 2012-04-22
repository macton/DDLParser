#pragma once

#include <stdlib.h>
#include <stddef.h>

#include <DDLParser.h>

namespace DDLParser
{
  template< typename T > class BlockAllocator
  {
    private:
      LinearAllocator* m_Alloc;
      void*            m_Free;

    public:
      inline BlockAllocator() {}

      inline bool Init ( LinearAllocator* alloc )
      {
        m_Alloc = alloc;
        m_Free  = 0;
        return true;
      }

      inline void Destroy() {}

      T* Allocate()
      {
        struct _
        {
          char c;
          T x;
        };
        static const size_t element_size      = sizeof ( T ) > sizeof ( void* ) ? sizeof ( T ) : sizeof ( void* );
        static const size_t element_alignment = sizeof ( T ) > 1 ? offsetof ( _, x ) : 1;
        void* node;

        if ( m_Free != 0 )
        {
          node   = m_Free;
          m_Free = * ( void** ) node;
        }
        else
        {
          node = m_Alloc->SafeAllocate ( element_size, element_alignment );
        }

        return ( T* ) node;
      }

      inline void Free ( T* element )
      {
        * ( void** ) element = m_Free;
        m_Free = element;
      }
  }; // class BlockAllocator
} // namespace DDLParser
