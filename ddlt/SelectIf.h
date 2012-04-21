#pragma once

struct lua_State;

namespace DDLParser
{
  struct Definition;
  struct Select;
  struct SelectItem;
};

namespace DDLT
{
  class Definition;

  class Select
  {
  private:
    DDLParser::Definition* m_Definition;
    DDLParser::Select*     m_Select;

    static int getType( lua_State* L );
    static int getName( lua_State* L );
    static int getAuthor( lua_State* L );
    static int getDescription( lua_State* L );
    static int getLabel( lua_State* L );
    static int getDisplayLabel( lua_State* L );
    static int getNameHash( lua_State* L );
    static int getNumItems( lua_State* L );
    static int getDefaultItem( lua_State* L );
    static int getItem( lua_State* L );
    static int findItem( lua_State* L );
    static int getOwner( lua_State* L );
    static int itemsIterator( lua_State* L );
    static int items( lua_State* L );
    static int l__index( lua_State* L );
    static int l__gc( lua_State* L );
    static int l__eq( lua_State* L );
    static int l__tostring( lua_State* L );

    inline bool Init( DDLParser::Definition* definition, DDLParser::Select* select )
    {
      m_Definition = definition;
      m_Select = select;

      return true;
    }

    inline void Destroy() {}

  public:
    static int PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Select* select );
    static Select* Check( lua_State* L, int index );

    inline DDLParser::Definition* GetDefinition() const
    {
      return m_Definition;
    }

    inline DDLParser::Select* GetSelect() const
    {
      return m_Select;
    }
  };

  class SelectItem
  {
  private:
    DDLParser::Definition* m_Definition;
    DDLParser::Select*     m_Select;
    DDLParser::SelectItem* m_Item;

    static int getName( lua_State* L );
    static int getAuthor( lua_State* L );
    static int getDescription( lua_State* L );
    static int getLabel( lua_State* L );
    static int getDisplayLabel( lua_State* L );
    static int getNameHash( lua_State* L );
    static int getOwner( lua_State* L );
    static int l__index( lua_State* L );
    static int l__gc( lua_State* L );
    static int l__eq( lua_State* L );
    static int l__tostring( lua_State* L );

    inline bool Init( DDLParser::Definition* definition, DDLParser::Select* select, DDLParser::SelectItem* item )
    {
      m_Definition = definition;
      m_Select = select;
      m_Item = item;

      return true;
    }

    inline void Destroy() {}

  public:
    static int PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Select* select, DDLParser::SelectItem* item );
    static SelectItem* Check( lua_State* L, int index );

    inline DDLParser::Definition* GetDefinition() const
    {
      return m_Definition;
    }

    inline DDLParser::Select* GetSelect() const
    {
      return m_Select;
    }

    inline DDLParser::SelectItem* GetItem() const
    {
      return m_Item;
    }
  };
}
