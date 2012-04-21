#pragma once

struct lua_State;

namespace DDLParser
{
  struct Definition;
  struct Aggregate;
};

namespace DDLT
{
  class Definition
  {
  private:
    DDLParser::Definition* m_Definition;

    static int getNumAggregates( lua_State* L );
    static int getAggregate( lua_State* L );
    static int findAggregate( lua_State* L );
    static int aggregatesIterator( lua_State* L );
    static int aggregates( lua_State* L );
    static int selects( lua_State* L );
    static int bitfields( lua_State* L );
    static int structs( lua_State* L );
    static int dumpDefinition( lua_State* L );
    static int l__index( lua_State* L );
    static int l__eq( lua_State* L );
    static int l__gc( lua_State* L );

    int PushAggregate( lua_State* L, DDLParser::Aggregate* aggregate );

    inline bool Init( DDLParser::Definition* definition )
    {
      m_Definition = definition;

      return true;
    }

    inline void Destroy() {}

  public:
    static int PushNew( lua_State* L, DDLParser::Definition* definition );
    static Definition* Check( lua_State* L, int index );

    inline DDLParser::Definition* GetDefinition() const
    {
      return m_Definition;
    }
  };
}
