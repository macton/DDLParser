#pragma once

struct lua_State;

namespace DDLParser
{
  struct Definition;
  struct Struct;
  struct StructField;
  struct StructValueInfo;
};

namespace DDLT
{
  class Definition;

  class Struct
  {
  private:
    DDLParser::Definition* m_Definition;
    DDLParser::Struct*     m_Struct;

    static int getType( lua_State* L );
    static int getName( lua_State* L );
    static int getAuthor( lua_State* L );
    static int getDescription( lua_State* L );
    static int getLabel( lua_State* L );
    static int getDisplayLabel( lua_State* L );
    static int getNameHash( lua_State* L );
    static int getNumFields( lua_State* L );
    static int getNumInheritedFields( lua_State* L );
    static int getNumOwnFields( lua_State* L );
    static int getBase( lua_State* L );
    static int getField( lua_State* L );
    static int findField( lua_State* L );
    static int getOwner( lua_State* L );
    static int fieldsIterator( lua_State* L );
    static int fields( lua_State* L );
    static int inheritedFieldsIterator( lua_State* L );
    static int inheritedFields( lua_State* L );
    static int ownFieldsIterator( lua_State* L );
    static int ownFields( lua_State* L );
    static int l__index( lua_State* L );
    static int l__gc( lua_State* L );
    static int l__eq( lua_State* L );

    inline bool Init( DDLParser::Definition* definition, DDLParser::Struct* structure )
    {
      m_Definition = definition;
      m_Struct = structure;

      return true;
    }

    inline void Destroy() {}

  public:
    static int     PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Struct* structure );
    static Struct* Check( lua_State* L, int index );

    inline DDLParser::Definition* GetDefinition() const
    {
      return m_Definition;
    }

    inline DDLParser::Struct* GetStruct() const
    {
      return m_Struct;
    }
  };

  class StructField
  {
  private:
    DDLParser::Definition*  m_Definition;
    DDLParser::Struct*      m_Struct;
    DDLParser::StructField* m_Field;

    static int getName( lua_State* L );
    static int getAuthor( lua_State* L );
    static int getDescription( lua_State* L );
    static int getLabel( lua_State* L );
    static int getDisplayLabel( lua_State* L );
    static int getNameHash( lua_State* L );
    static int getType( lua_State* L );
    static int getDeclaredType( lua_State* L );
    static int getArrayType( lua_State* L );
    static int getArrayCount( lua_State* L );
    static int getValue( lua_State* L );
    static int getOwner( lua_State* L );
    static int isInherited( lua_State* L );
    static int l__index( lua_State* L );
    static int l__gc( lua_State* L );
    static int l__eq( lua_State* L );
    static int l__tostring( lua_State* L );

    inline bool Init( DDLParser::Definition* definition, DDLParser::Struct* structure, DDLParser::StructField* field )
    {
      m_Definition = definition;
      m_Struct = structure;
      m_Field = field;

      return true;
    }

    inline void Destroy() {}

  public:
    static int          PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Struct* structure, DDLParser::StructField* field );
    static StructField* Check( lua_State* L, int index );

    inline DDLParser::Definition* GetDefinition() const
    {
      return m_Definition;
    }

    inline DDLParser::Struct* GetStruct() const
    {
      return m_Struct;
    }

    inline DDLParser::StructField* GetField() const
    {
      return m_Field;
    }
  };

  class StructValueInfo
  {
  private:
    DDLParser::Definition*      m_Definition;
    DDLParser::Struct*          m_Struct;
    DDLParser::StructField*     m_Field;
    DDLParser::StructValueInfo* m_ValueInfo;
    const char*                 m_Name;

    static int getNameHash( lua_State* L );
    static int getType( lua_State* L );
    static int getDeclaredType( lua_State* L );
    static int getArrayType( lua_State* L );
    static int getArrayCount( lua_State* L );
    static int getValue( lua_State* L );
    static int getName( lua_State* L );
    static int l__index( lua_State* L );
    static int l__gc( lua_State* L );
    static int l__eq( lua_State* L );
    static int l__tostring( lua_State* L );

    inline bool Init( DDLParser::Definition* definition, DDLParser::Struct* structure, DDLParser::StructField* field, DDLParser::StructValueInfo* value_info, const char* name )
    {
      m_Definition = definition;
      m_Struct = structure;
      m_Field = field;
      m_ValueInfo = value_info;
      m_Name = name;

      return true;
    }

    inline void Destroy() {}

  public:
    static int              PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Struct* structure, DDLParser::StructField* field, DDLParser::StructValueInfo* value_info, const char* name );
    static StructValueInfo* Check( lua_State* L, int index );

    inline DDLParser::Definition* GetDefinition() const
    {
      return m_Definition;
    }

    inline DDLParser::Struct* GetStruct() const
    {
      return m_Struct;
    }

    inline DDLParser::StructField* GetField() const
    {
      return m_Field;
    }

    inline DDLParser::StructValueInfo* GetValueInfo() const
    {
      return m_ValueInfo;
    }

    inline const char* GetName() const
    {
      return m_Name;
    }
  };
}
