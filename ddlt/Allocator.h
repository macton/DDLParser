#pragma once

#include <DDLParser.h>

#if defined( __WIN__ )

class LinearAllocator: public DDLParser::LinearAllocator
{
private:
  size_t m_PageSizeInBytes;
  void*  m_Base;
  size_t m_CommittedSizeInBytes;
  size_t m_MaxSizeInBytes;
  size_t m_CurrentOffset;

public:
  bool  Init( size_t max_size_in_bytes );
  void  Destroy();
  void* Allocate( size_t size_in_bytes, size_t alignment_in_bytes = 1 );
  
  inline void* GetBase() const
  {
    return m_Base;
  }

  inline size_t GetCurrentOffset() const
  {
    return m_CurrentOffset;
  }
};

#else // must be Linux...

class LinearAllocator: public DDLParser::LinearAllocator
{
private:
  void*  m_Base;
  size_t m_MaxSizeInBytes;
  size_t m_CurrentOffset;

public:
  bool  Init( size_t max_size_in_bytes );
  void  Destroy();
  void* Allocate( size_t size_in_bytes, size_t alignment_in_bytes = 1 );
  
  inline void* GetBase() const
  {
    return m_Base;
  }

  inline size_t GetCurrentOffset() const
  {
    return m_CurrentOffset;
  }
};

#endif
