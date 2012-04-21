#pragma once

#include "Allocator.h"

struct lua_State;
struct DDLCompiler;

namespace DDLParser
{
  struct AreaManager;
};

namespace DDLT
{
  class Compiler
  {
  private:
    const char*     m_CompilerName;
    bool            m_TwoUSReserved;
    int             m_BitfieldLimit;
    LinearAllocator m_Definition;

    static int compile( lua_State* L );
    static int getDefinition( lua_State* L );
    static int getDisclaimer( lua_State* L );
    static int l__index( lua_State* L );
    static int l__gc( lua_State* L );

    bool Init( const char* compiler_name, bool two_us_reserved, int bitfield_limit );

    inline void Destroy()
    {
      m_Definition.Destroy();
    }

  public:
    static int PushNew( lua_State* L, const char* compiler_name, bool two_us_reserved, int bitfield_limit );
    static Compiler *Check( lua_State* L, int index );
  };
}
