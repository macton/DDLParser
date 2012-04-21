#pragma once

#include <DDLParser.h>

namespace DDLParser
{
  template< typename T > class Stack
  {
  private:
    struct Element
    {
      T        m_Payload;
      Element* m_Previous;
    };

    BlockAllocator< Element > m_Elements;
    Element*                  m_Top;

  public:
    inline Stack() {}

    inline bool Init( LinearAllocator* alloc )
    {
      m_Top   = 0;

      return m_Elements.Init( alloc );
    }

    inline void Destroy()
    {
      m_Elements.Destroy();
    }

    inline bool Pop( T& payload )
    {
      if ( m_Top == 0 )
      {
        return false;
      }

      payload = m_Top->m_Payload;
      Element* previous = m_Top->m_Previous;
      m_Elements.Free( m_Top );
      m_Top = previous;

      return true;
    }

    bool Push( const T& payload )
    {
      Element* element = m_Elements.Allocate();

      if ( element != 0 )
      {
        element->m_Payload = payload;
        element->m_Previous = m_Top;
        m_Top = element;
        return true;
      }

      return false;
    }

    void Reset()
    {
      T payload;

      while ( m_Top != 0 )
      {
        Pop( payload );
      }
    }
  };
};
