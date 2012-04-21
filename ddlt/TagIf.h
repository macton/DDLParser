#pragma once

struct lua_State;

namespace DDLParser
{
  struct Definition;
  struct Aggregate;
  struct Select;
  struct Bitfield;
  struct Struct;
  struct SelectItem;
  struct BitfieldFlag;
  struct StructField;
  struct Tag;
};

namespace DDLT
{
  class Tag
  {
  private:
    DDLParser::Definition* m_Definition;

    union
    {
      DDLParser::Select*   m_Select;
      DDLParser::Bitfield* m_Bitfield;
      DDLParser::Struct*   m_Struct;
    };

    union
    {
      DDLParser::SelectItem*   m_Item;
      DDLParser::BitfieldFlag* m_Flag;
      DDLParser::StructField*  m_Field;
    };

    DDLParser::Tag* m_Tag;
    int             m_AggregateType;

    static int getType( lua_State* L );
    static int getExtensions( lua_State* L );
    static int getVaultHint( lua_State* L );
    static int getUIRange( lua_State* L );
    static int getUIRender( lua_State* L );
    static int getParallel( lua_State* L );
    static int getVersion( lua_State* L );
    static int getCallback( lua_State* L );
    static int getKey( lua_State* L );
    static int getUnits( lua_State* L );
    static int getGenericName( lua_State* L );
    static int getGenericNameHash( lua_State* L );
    static int getGenericValues( lua_State* L );
    static int getOwner( lua_State* L );
    static int l__index( lua_State* L );
    static int l__gc( lua_State* L );
    static int l__eq( lua_State* L );
    static int l__tostring( lua_State* L );

    inline bool Init( DDLParser::Definition* definition, DDLParser::Select* select, DDLParser::SelectItem* item, DDLParser::Tag* tag )
    {
      m_Definition = definition;
      m_Select = select;
      m_Item = item;
      m_Tag = tag;
      m_AggregateType = DDLParser::kSelect;

      return true;
    }

    inline bool Init( DDLParser::Definition* definition, DDLParser::Bitfield* bitfield, DDLParser::BitfieldFlag* flag, DDLParser::Tag* tag )
    {
      m_Definition = definition;
      m_Bitfield = bitfield;
      m_Flag = flag;
      m_Tag = tag;
      m_AggregateType = DDLParser::kBitfield;

      return true;
    }

    inline bool Init( DDLParser::Definition* definition, DDLParser::Struct* structure, DDLParser::StructField* field, DDLParser::Tag* tag )
    {
      m_Definition = definition;
      m_Struct = structure;
      m_Field = field;
      m_Tag = tag;
      m_AggregateType = DDLParser::kStruct;

      return true;
    }

    inline void Destroy() {}

    static int SetMetatable( lua_State* L );

  public:
    static int PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Select* select, DDLParser::SelectItem* item, DDLParser::Tag* tag );
    static int PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Bitfield* bitfield, DDLParser::BitfieldFlag* flag, DDLParser::Tag* tag );
    static int PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Struct* structure, DDLParser::StructField* field, DDLParser::Tag* tag );
    static Tag* Check( lua_State* L, int index );
  };
}
