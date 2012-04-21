#include <new>

#include "AreaManager.h"

namespace DDLParser
{
  void Area::Init(AreaManager* manager, uint32_t offset, Area* next)
  {
    m_Manager = manager;
    m_Offset  = offset;
    m_Next    = next;
    m_Size    = 0;
  }

  uint8_t* Area::GetBase() const
  {
    return (uint8_t*)m_Manager->m_Allocator->GetBase() + m_Offset;
  }

  void Area::AddRelPointer(void* pointer)
  {
    m_Manager->AddRelPointer((uint32_t)((uint8_t*)pointer - (uint8_t*)m_Manager->m_Allocator->GetBase()));
  }

  void Area::Align(uint32_t alignment)
  {
    uint32_t current  = m_Offset + m_Size;
    uint32_t new_curr = (current + alignment - 1) & ~( alignment - 1 );
    if (new_curr != current)
    {
      uint32_t new_size = new_curr - current;
      memset(&*Allocate<uint8_t>(new_size), 0, new_size);
    }
  }

  Area* Area::NewArea()
  {
    return m_Manager->NewArea(this);
  }

  void Area::Merge()
  {
    m_Manager->Merge(this);
  }

  bool AreaManager::Init( LinearAllocator* definition, LinearAllocator* scratch )
  {
    m_Allocator = definition;
    m_FirstArea = m_LastArea = 0;

    return m_AreasArray.Init( scratch ) && m_RelPointers.Init( scratch );
  }

  void AreaManager::Destroy()
  {
    m_AreasArray.Destroy();
    m_RelPointers.Destroy();
  }

  Area*
  AreaManager::NewArea()
  {
    void* mem = m_AreasArray.Allocate();
    size_t off = m_Allocator->GetCurrentOffset();
    Area* area = new (mem) Area(this, (uint32_t)off, NULL);
    if (m_FirstArea == NULL)
    {
      m_FirstArea = area;
    }
    else
    {
      m_LastArea->m_Next = area;
    }
    m_LastArea = area;
    return area;
  }

  Area*
  AreaManager::NewArea(Area* previous)
  {
    uint32_t offset = previous->m_Offset + previous->m_Size;
    Area* area = new (m_AreasArray.Allocate()) Area(this, offset, previous->m_Next);
    previous->m_Next = area;
    return area;
  }

  Area*
  AreaManager::NewArea( uint32_t offset, uint32_t size )
  {
    Area* area = new (m_AreasArray.Allocate()) Area(this, offset, NULL);
    area->m_Size = size;
    if (m_FirstArea == NULL)
    {
      m_FirstArea = area;
    }
    else
    {
      m_LastArea->m_Next = area;
    }
    m_LastArea = area;
    return area;
  }

  void
  AreaManager::Merge(Area* first)
  {
    Area* second = first->m_Next;
    if (second != NULL)
    {
      first->m_Size += second->m_Size;
      first->m_Next = second->m_Next;
      second->~Area();
    }
  }

  void
  AreaManager::Grow(Area* area, uint32_t size)
  {
    uint32_t old_size = m_Allocator->GetCurrentOffset();
    m_Allocator->SafeAllocate(size);
    Area* next = area->m_Next;
    while (next != NULL)
    {
      next->m_Offset += size;
      next = next->m_Next;
    }
    next = area->m_Next;
    if (next != NULL)
    {
      uint32_t limit   = next->m_Offset - size;
      uint8_t* base    = (uint8_t*)m_Allocator->GetBase();

      GrowableArray< size_t >::Iterator it = m_RelPointers.Iterate();
      size_t* offset;

      while ( ( offset = it.Next() ) != 0 )
      {
        uint32_t position = *(uint32_t*)(base + *offset) + *offset;
        if (*offset < limit && position >= limit)
        {
          *(uint32_t*)(base + *offset) += size;
        }
        else if (position < limit && *offset >= limit)
        {
          *(uint32_t*)(base + *offset) -= size;
        }
        if (*offset >= limit)
        {
          *offset += size;
        }
      }

      memmove(base + next->m_Offset, base + next->m_Offset - size, old_size - next->m_Offset + size);
    }
  }

  void
  AreaManager::AddRelPointer(uint32_t offset)
  {
    *(uint32_t*)m_RelPointers.Append() = offset;
  }

  void
  AreaManager::Write(void* dest)
  {
    memcpy(dest, m_Allocator->GetBase(), m_Allocator->GetCurrentOffset());
  }
};
