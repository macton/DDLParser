#pragma once

#include <stdint.h>

#include "BlockAllocator.h"

namespace DDLParser
{
  template< typename T > class Set
  {
    private:
      struct Element
      {
        uint32_t m_Key;
        T        m_Payload;
        Element* m_Next;
      };

      BlockAllocator< Element > m_Elements;
      Element*                  m_First;
      Element*                  m_Last;

      Element* FindInternal ( uint32_t key ) const
      {
        Element* element = m_First;

        while ( element != 0 )
        {
          if ( element->m_Key == key )
          {
            return element;
          }

          element = element->m_Next;
        }

        return 0;
      }

    public:
      inline bool Init ( LinearAllocator* alloc )
      {
        m_First = m_Last = 0;
        return m_Elements.Init ( alloc );
      }

      inline void Destroy()
      {
        m_Elements.Destroy();
      }

      const T* Find ( uint32_t key ) const
      {
        Element* element = FindInternal ( key );

        if ( element )
        {
          return &element->m_Payload;
        }

        return 0;
      }

      const T* Insert ( uint32_t key, const T& payload )
      {
        Element* element = FindInternal ( key );

        if ( element != 0 )
        {
          element->m_Payload = payload;
          return &element->m_Payload;
        }

        element = m_Elements.Allocate();

        if ( element != 0 )
        {
          if ( m_First != 0 )
          {
            m_Last->m_Next = element;
          }
          else
          {
            m_First = element;
          }

          m_Last = element;
          element->m_Key = key;
          element->m_Payload = payload;
          element->m_Next = 0;
          return &element->m_Payload;
        }

        return 0;
      }
  };
}
