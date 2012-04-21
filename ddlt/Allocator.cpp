#if defined( __WIN__ )

#include "Allocator.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

bool LinearAllocator::Init( size_t max_size_in_bytes )
{
  SYSTEM_INFO info;
  GetSystemInfo( &info );
  m_PageSizeInBytes = info.dwPageSize;
  m_CommittedSizeInBytes = 0;
  m_MaxSizeInBytes = max_size_in_bytes;
  m_CurrentOffset = 0;
  //m_Base = VirtualAlloc( 0, ( max_size_in_bytes + m_PageSizeInBytes - 1 ) & ~( m_PageSizeInBytes - 1 ), MEM_RESERVE, PAGE_EXECUTE_READWRITE );
  m_Base = VirtualAlloc( 0, ( max_size_in_bytes + m_PageSizeInBytes - 1 ) & ~( m_PageSizeInBytes - 1 ), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE );
  return m_Base != 0;
}

void LinearAllocator::Destroy()
{
  VirtualFree( m_Base, 0, MEM_RELEASE );
}

void* LinearAllocator::Allocate( size_t size_in_bytes, size_t alignment_in_bytes )
{
  size_t offset = ( m_CurrentOffset + alignment_in_bytes - 1 ) & ~( alignment_in_bytes - 1 );

  if ( offset + size_in_bytes > m_MaxSizeInBytes )
  {
    return 0;
  }

  /*if ( offset + size_in_bytes > m_CommittedSizeInBytes )
  {
    void* start = (void*)( (char*)m_Base + offset );
    void* result = VirtualAlloc( start, size_in_bytes, MEM_COMMIT, PAGE_EXECUTE_READWRITE );

    if ( result == 0 )
    {
      return 0;
    }

    m_CommittedSizeInBytes = ( offset + size_in_bytes + m_PageSizeInBytes ) & ~( m_PageSizeInBytes - 1 );
  }*/

  m_CurrentOffset = offset + size_in_bytes;
  return (void*)( (char*)m_Base + offset );
}

#else // must be Linux...

#include <sys/mman.h>

#include "Allocator.h"

bool LinearAllocator::Init( size_t max_size_in_bytes )
{
  m_MaxSizeInBytes = max_size_in_bytes;
  m_CurrentOffset = 0;
  m_Base = mmap( 0, max_size_in_bytes, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0 ); 
  return m_Base != MAP_FAILED;
}

void LinearAllocator::Destroy()
{
  munmap( m_Base, m_MaxSizeInBytes );
}

void* LinearAllocator::Allocate( size_t size_in_bytes, size_t alignment_in_bytes )
{
  size_t offset = ( m_CurrentOffset + alignment_in_bytes - 1 ) & ~( alignment_in_bytes - 1 );

  if ( offset + size_in_bytes > m_MaxSizeInBytes )
  {
    return 0;
  }

  m_CurrentOffset = offset + size_in_bytes;
  return (void*)( (char*)m_Base + offset );
}

#endif
