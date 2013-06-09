#pragma once

struct lua_State;

namespace DDLParser
{
  struct Definition;
  struct Bitfield;
  struct BitfieldFlag;
};

namespace DDLT
{
  class Definition;

  class Bitfield
  {
  private:
    DDLParser::Definition* m_Definition;
    DDLParser::Bitfield*   m_Bitfield;

    static int getType( lua_State* L );
    static int getName( lua_State* L );
    static int getAuthor( lua_State* L );
    static int getDescription( lua_State* L );
    static int getLabel( lua_State* L );
    static int getDisplayLabel( lua_State* L );
    static int getNameHash( lua_State* L );
    static int getNumFlags( lua_State* L );
    static int getDefaultFlag( lua_State* L );
    static int getFlag( lua_State* L );
    static int findFlag( lua_State* L );
    static int getOwner( lua_State* L );
    static int flagsIterator( lua_State* L );
    static int flags( lua_State* L );
    static int tagsIterator( lua_State* L );
    static int tags( lua_State* L );
    static int l__index( lua_State* L );
    static int l__gc( lua_State* L );
    static int l__eq( lua_State* L );
    static int l__tostring( lua_State* L );

    inline bool Init( DDLParser::Definition* definition, DDLParser::Bitfield* bitfield )
    {
      m_Definition = definition;
      m_Bitfield = bitfield;

      return true;
    }

    inline void Destroy() {}

  public:
    static int PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Bitfield* bitfield );
    static Bitfield* Check( lua_State* L, int index );

    inline DDLParser::Definition* GetDefinition() const
    {
      return m_Definition;
    }

    inline DDLParser::Bitfield* GetBitfield() const
    {
      return m_Bitfield;
    }
  };

  class BitfieldFlag
  {
  private:
    DDLParser::Definition*   m_Definition;
    DDLParser::Bitfield*     m_Bitfield;
    DDLParser::BitfieldFlag* m_Flag;

    static int getName( lua_State* L );
    static int getAuthor( lua_State* L );
    static int getDescription( lua_State* L );
    static int getLabel( lua_State* L );
    static int getDisplayLabel( lua_State* L );
    static int getNameHash( lua_State* L );
    static int getValue( lua_State* L );
    static int getOwner( lua_State* L );
    static int tagsIterator( lua_State* L );
    static int tags( lua_State* L );
    static int l__index( lua_State* L );
    static int l__gc( lua_State* L );
    static int l__eq( lua_State* L );
    static int l__tostring( lua_State* L );

    inline bool Init( DDLParser::Definition* definition, DDLParser::Bitfield* bitfield, DDLParser::BitfieldFlag* flag )
    {
      m_Definition = definition;
      m_Bitfield = bitfield;
      m_Flag = flag;

      return true;
    }

    inline void Destroy() {}

  public:
    static int PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Bitfield* bitfield, DDLParser::BitfieldFlag* flag );
    static BitfieldFlag* Check( lua_State* L, int index );

    inline DDLParser::Definition* GetDefinition() const
    {
      return m_Definition;
    }

    inline DDLParser::Bitfield* GetSelect() const
    {
      return m_Bitfield;
    }

    inline DDLParser::BitfieldFlag* GetItem() const
    {
      return m_Flag;
    }
  };
}
