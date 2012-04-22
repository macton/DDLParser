#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <DDLParser.h>

#include "BlockAllocator.h"
#include "GrowableArray.h"

namespace DDLParser
{
  template< class T > struct Pointer;
  struct Area;

  struct AreaManager
  {
    bool  Init ( LinearAllocator* definition, LinearAllocator* scratch );
    void  Destroy();

    Area* NewArea();
    Area* NewArea ( Area* previous );
    Area* NewArea ( uint32_t offset, uint32_t size );
    void  Merge ( Area* first );

    void  Grow ( Area* area, uint32_t size );

    void  AddRelPointer ( uint32_t offset );

    void  Write ( void* dest );

    LinearAllocator*        m_Allocator;
    BlockAllocator< Area >  m_AreasArray;
    GrowableArray< size_t > m_RelPointers;
    Area*                   m_FirstArea;
    Area*                   m_LastArea;
  };

  struct Area
  {
    Area()
    {
      Init ( 0, 0, 0 );
    }

    Area ( AreaManager* manager, uint32_t offset, Area* next )
    {
      Init ( manager, offset, next );
    }

    void    Init ( AreaManager* manager, uint32_t offset, Area* next );

    uint8_t*  GetBase() const;

    void    AddRelPointer ( void* pointer );
    void    Align ( uint32_t alignment );

    Area*    NewArea();
    void    Merge();

    template<class T> Pointer<T> Allocate ( uint32_t count = 1 )
    {
      uint32_t size = sizeof ( T ) * count;
      m_Manager->Grow ( this, size );
      void* mem = ( uint8_t* ) m_Manager->m_Allocator->GetBase() + m_Offset + m_Size;
      memset ( mem, 0, size );
      m_Size += size;
      Pointer<T> pointer = Pointer<T> ( this );
      pointer = mem;
      return pointer;
    }

    AreaManager* m_Manager;
    uint32_t     m_Offset;
    uint32_t     m_Size;
    Area*        m_Next;
  };

  template<class T>
  struct Pointer
  {
    Pointer()
      : m_Area ( 0 )
      , m_Offset ( 0 )
    {
    }

    Pointer ( const Area* area, uint32_t offset = 0 )
      : m_Area ( area )
      , m_Offset ( offset )
    {
    }

    T* operator->() const
    {
      return ( T* ) ( m_Area->GetBase() + m_Offset );
    }

    T& operator*()
    {
      static T* null = 0;

      if ( m_Offset == ( uint32_t ) - 1 )
      {
        return *null;
      }

      return * ( T* ) ( m_Area->GetBase() + m_Offset );
    }

    void operator= ( void* pointer )
    {
      if ( pointer == 0 )
      {
        m_Offset = ( uint32_t ) - 1;
        return;
      }

      m_Offset = ( uint32_t ) ( ( uint8_t* ) pointer - m_Area->GetBase() );
    }

    template< typename U > void CastTo ( Pointer< U >& result ) const
    {
      result.m_Area = m_Area;
      result.m_Offset = m_Offset;
    }

    const Area* m_Area;
    uint32_t    m_Offset;
  };
};
