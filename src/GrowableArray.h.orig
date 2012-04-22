#pragma once

#include <stdlib.h>
#include <stddef.h>

#include <DDLParser.h>

namespace DDLParser
{
  template< typename T > class GrowableArray
  {
  private:
    enum
    {
      kMaxElemPerBlock = 256
    };

    struct Block
    {
      T      m_Elements[ kMaxElemPerBlock ];
      size_t m_Count;
      Block* m_Next;
    };

    LinearAllocator* m_Alloc;
    Block*           m_First;
    Block*           m_Last;

  public:
    class Iterator
    {
    private:
      Block* m_Current;
      size_t m_Count;
      
    public:
      inline void Init( Block* first )
      {
        m_Current = first;
        m_Count   = 0;
      }

      T* Next()
      {
        if ( m_Current == 0 )
        {
          return 0;
        }

        if ( m_Count == m_Current->m_Count )
        {
          m_Current = m_Current->m_Next;
          m_Count   = 0;
        }

        if ( m_Current == 0 )
        {
          return 0;
        }

        return &m_Current->m_Elements[ m_Count++ ];
      }
    };

    inline bool Init( LinearAllocator* alloc )
    {
      m_Alloc = alloc;
      m_First = 0;
      m_Last  = 0;

      return true;
    }

    inline void Destroy() {}

    T* Append()
    {
      struct _ { char c; Block x; };

      static const size_t block_alignment = sizeof( Block ) > 1 ? offsetof( _, x ) : 1;

      if ( m_Last != 0 )
      {
        if ( m_Last->m_Count < kMaxElemPerBlock )
        {
          return &m_Last->m_Elements[ m_Last->m_Count++ ];
        }

        Block* block = (Block*)m_Alloc->SafeAllocate( sizeof( Block ), block_alignment );

        if ( block != 0 )
        {
          block->m_Count = 0;
          block->m_Next  = 0;
          m_Last->m_Next = block;
          m_Last = block;

          return &m_Last->m_Elements[ m_Last->m_Count++ ]; 
        }

        return 0;
      }

      m_First = m_Last = (Block*)m_Alloc->SafeAllocate( sizeof( Block ), block_alignment );

      if ( m_Last != 0 )
      {
        m_Last->m_Count = 0;
        m_Last->m_Next  = 0;

        return &m_Last->m_Elements[ m_Last->m_Count++ ]; 
      }

      return 0;
    }

    inline Iterator Iterate() const
    {
      Iterator it;
      it.Init( m_First );
      return it;
    }
  };
}
