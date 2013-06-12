#include <new>

#include <DDLParser.h>

#include "TagIf.h"
#include "DefinitionIf.h"
#include "SelectIf.h"
#include "BitfieldIf.h"
#include "StructIf.h"
#include "Util.h"

extern "C"
{
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
};

// ##     ## ######## #### ##
// ##     ##    ##     ##  ##
// ##     ##    ##     ##  ##
// ##     ##    ##     ##  ##
// ##     ##    ##     ##  ##
// ##     ##    ##     ##  ##
//  #######     ##    #### ########

static int PushUIRangeValue( lua_State* L, DDLParser::StructField* field, DDLParser::UIRangeValue* value )
{
  char value_str[ 128 ];

  switch ( field->GetValueInfo()->GetType() )
  {
  case DDLParser::kUint8:
    snprintf( value_str, sizeof( value_str ), "%u", value->m_Uint8 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kUint16:
    snprintf( value_str, sizeof( value_str ), "%u", value->m_Uint16 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kUint32:
    snprintf( value_str, sizeof( value_str ), "%u", value->m_Uint32 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kUint64:
    snprintf( value_str, sizeof( value_str ), "%llu", value->m_Uint64 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kInt8:
    snprintf( value_str, sizeof( value_str ), "%d", value->m_Int8 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kInt16:
    snprintf( value_str, sizeof( value_str ), "%d", value->m_Int16 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kInt32:
    snprintf( value_str, sizeof( value_str ), "%d", value->m_Int32 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kInt64:
    snprintf( value_str, sizeof( value_str ), "%lld", value->m_Int64 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kFloat32:
    snprintf( value_str, sizeof( value_str ), "%f", value->m_Float32 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kFloat64:
    snprintf( value_str, sizeof( value_str ), "%f", value->m_Float64 );
    lua_pushstring( L, value_str );
    return 1;
  default:
    break;
  }

  return 0;
}

static int PushGenericTagValue( lua_State* L, DDLParser::GenericTagValue* value )
{
  char value_str[ 128 ];

  switch ( value->m_Type )
  {
  case DDLParser::kInt64:
    snprintf( value_str, sizeof( value_str ), "%lld", value->m_Int64 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kFloat64:
    snprintf( value_str, sizeof( value_str ), "%f", value->m_Float64 );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kString:
    lua_pushstring( L, &*value->m_String );
    return 1;
  default:
    break;
  }

  return 0;
}

// ########    ###     ######
//    ##      ## ##   ##    ##
//    ##     ##   ##  ##
//    ##    ##     ## ##   ####
//    ##    ######### ##    ##
//    ##    ##     ## ##    ##
//    ##    ##     ##  ######

int DDLT::Tag::getType( lua_State* L )
{
  Tag* self = Check( L, 1 );

  switch ( self->m_Tag->GetType() )
  {
  case DDLParser::kExtensions:
    lua_pushliteral( L, "extensions" );
    return 1;
  case DDLParser::kVaultHints:
    lua_pushliteral( L, "vaulthints" );
    return 1;
  case DDLParser::kUIRange:
    lua_pushliteral( L, "uirange" );
    return 1;
  case DDLParser::kUIRender:
    lua_pushliteral( L, "uirender" );
    return 1;
  case DDLParser::kParallel:
    lua_pushliteral( L, "parallel" );
    return 1;
  case DDLParser::kVersion:
    lua_pushliteral( L, "version" );
    return 1;
  case DDLParser::kCallback:
    lua_pushliteral( L, "callback" );
    return 1;
  case DDLParser::kKey:
    lua_pushliteral( L, "key" );
    return 1;
  case DDLParser::kUnits:
    lua_pushliteral( L, "units" );
    return 1;
  case DDLParser::kAbstract:
    lua_pushliteral( L, "abstract" );
    return 1;
  case DDLParser::kGeneric:
    lua_pushliteral( L, "generic" );
    return 1;
  }

  return 0;
}

int DDLT::Tag::getExtensions( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kExtensions )
  {
    DDLParser::Extensions* tag = (DDLParser::Extensions*)self->m_Tag;

    lua_newtable( L );

    for ( uint32_t i = 0; i < tag->GetCount(); i++ )
    {
      lua_pushstring( L, tag->GetExtension( i ) );
      lua_rawseti( L, -2, i + 1 );
    }

    return 1;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getVaultHint( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kVaultHints )
  {
    DDLParser::VaultHints* tag = (DDLParser::VaultHints*)self->m_Tag;

    lua_newtable( L );

    for ( uint32_t i = 0; i < tag->GetCount(); i++ )
    {
      lua_pushstring( L, tag->GetVaultHint( i ) );
      lua_rawseti( L, -2, i + 1 );
    }

    return 1;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getUIRange( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kUIRange )
  {
    DDLParser::UIRange* tag = (DDLParser::UIRange*)self->m_Tag;

    PushUIRangeValue( L, self->m_Field, tag->GetSoftMin() );
    PushUIRangeValue( L, self->m_Field, tag->GetSoftMax() );
    PushUIRangeValue( L, self->m_Field, tag->GetHardMin() );
    PushUIRangeValue( L, self->m_Field, tag->GetHardMax() );
    PushUIRangeValue( L, self->m_Field, tag->GetStep() );
    return 5;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getUIRender( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kUIRender )
  {
    DDLParser::UIRender* tag = (DDLParser::UIRender*)self->m_Tag;

    lua_pushstring( L, tag->GetRender() );
    return 1;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getParallel( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kParallel )
  {
    DDLParser::Parallel* tag = (DDLParser::Parallel*)self->m_Tag;

    return StructField::PushNew( L, self->m_Definition, self->m_Struct, tag->GetArray() );
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getVersion( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kVersion )
  {
    DDLParser::Version* tag = (DDLParser::Version*)self->m_Tag;

    lua_pushstring( L, tag->GetVersion() );
    return 1;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getCallback( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kCallback )
  {
    DDLParser::Callback* tag = (DDLParser::Callback*)self->m_Tag;

    lua_pushstring( L, tag->GetCallback() );
    return 1;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getKey( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kKey )
  {
    DDLParser::Key* tag = (DDLParser::Key*)self->m_Tag;

    lua_pushstring( L, tag->GetKey() );
    return 1;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getUnits( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kUnits )
  {
    DDLParser::Units* tag = (DDLParser::Units*)self->m_Tag;

    lua_pushstring( L, tag->GetUnits() );
    return 1;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getGenericName( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kGeneric )
  {
    DDLParser::GenericTag* tag = (DDLParser::GenericTag*)self->m_Tag;

    lua_pushstring( L, tag->GetName() );
    return 1;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getGenericNameHash( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kGeneric )
  {
    DDLParser::GenericTag* tag = (DDLParser::GenericTag*)self->m_Tag;

    lua_pushnumber( L, tag->GetNameHash() );
    return 1;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getGenericValues( lua_State* L )
{
  Tag* self = Check( L, 1 );

  if ( self->m_Tag->GetType() == DDLParser::kGeneric )
  {
    DDLParser::GenericTag* tag = (DDLParser::GenericTag*)self->m_Tag;

    lua_newtable( L );

    for ( uint32_t i = 0; i < tag->GetNumValues(); i++ )
    {
      PushGenericTagValue( L, ( *tag )[ i ] );
      lua_rawseti( L, -2, i + 1 );
    }

    return 1;
  }

  return luaL_error( L, "Wrong tag type" );
}

int DDLT::Tag::getOwner( lua_State* L )
{
  Tag* self = Check( L, 1 );

  switch ( self->m_AggregateType )
  {
  case DDLParser::kSelect:
    if ( self->m_Item != 0 )
    {
      return SelectItem::PushNew( L, self->m_Definition, self->m_Select, self->m_Item );
    }
    else
    {
      return Select::PushNew( L, self->m_Definition, self->m_Select );
    }

  case DDLParser::kBitfield:
    if ( self->m_Flag != 0 )
    {
      return BitfieldFlag::PushNew( L, self->m_Definition, self->m_Bitfield, self->m_Flag );
    }
    else
    {
      return Bitfield::PushNew( L, self->m_Definition, self->m_Bitfield );
    }

  case DDLParser::kStruct:
    if ( self->m_Field != 0 )
    {
      return StructField::PushNew( L, self->m_Definition, self->m_Struct, self->m_Field );
    }
    else
    {
      return Struct::PushNew( L, self->m_Definition, self->m_Struct );
    }
  }

  return 0;
}

int DDLT::Tag::l__index( lua_State* L )
{
  const char* key  = luaL_checkstring( L, 2 );
  uint32_t    hash = DDLParser::StringCrc32( key );

  switch ( hash )
  {
  case 0x44e9085cU: // getType
    lua_pushcfunction( L, getType );
    return 1;
  case 0x77defe82U: // getExtensions
    lua_pushcfunction( L, getExtensions );
    return 1;
  case 0x470c2202U: // getVaultHint
    lua_pushcfunction( L, getVaultHint );
    return 1;
  case 0xb7ea6ef3U: // getUIRange
    lua_pushcfunction( L, getUIRange );
    return 1;
  case 0x6dce4141U: // getUIRender
    lua_pushcfunction( L, getUIRender );
    return 1;
  case 0x75453f78U: // getParallel
    lua_pushcfunction( L, getParallel );
    return 1;
  case 0x97f94436U: // getVersion
    lua_pushcfunction( L, getVersion );
    return 1;
  case 0x660478afU: // getCallback
    lua_pushcfunction( L, getCallback );
    return 1;
  case 0x65f42ebdU: // getKey
    lua_pushcfunction( L, getKey );
    return 1;
  case 0x1b152928U: // getUnits
    lua_pushcfunction( L, getUnits );
    return 1;
  case 0x02dbad0cU: // getGenericName
    lua_pushcfunction( L, getGenericName );
    return 1;
  case 0x477a1ee9U: // getGenericNameHash
    lua_pushcfunction( L, getGenericNameHash );
    return 1;
  case 0x9e4b6905U: // getGenericValues
    lua_pushcfunction( L, getGenericValues );
    return 1;
  case 0x3dc5bb1dU: // getOwner
    lua_pushcfunction( L, getOwner );
    return 1;
  }

  return 0;
}

int DDLT::Tag::l__gc( lua_State* L )
{
  Tag* self = Check( L, 1 );

  self->Destroy();
  return 0;
}

int DDLT::Tag::l__eq( lua_State* L )
{
  Tag* p1 = Check( L, 1 );
  Tag* p2 = Check( L, 2 );

  lua_pushboolean( L, p1->m_Tag == p2->m_Tag );
  return 1;
}

int DDLT::Tag::l__tostring( lua_State* L )
{
  Tag* self = Check( L, 1 );

  lua_pushfstring( L, "DDLT::Tag@%p(", self );
  lua_pushcfunction( L, getType );
  lua_pushvalue( L, 1 );
  lua_call( L, 1, 1 );
  lua_pushliteral( L, ")" );
  lua_concat( L, 3 );
  return 1;
}

int DDLT::Tag::SetMetatable( lua_State* L )
{
  if ( luaL_newmetatable( L, "DDLT::Tag" ) )
  {
    lua_pushcfunction( L, l__index );
    lua_setfield( L, -2, "__index" );
    lua_pushcfunction( L, l__gc );
    lua_setfield( L, -2, "__gc" );
    lua_pushcfunction( L, l__eq );
    lua_setfield( L, -2, "__eq" );
    lua_pushcfunction( L, l__tostring );
    lua_setfield( L, -2, "__tostring" );
  }

  lua_setmetatable( L, -2 );
  return 1;
}

int DDLT::Tag::PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Select* select, DDLParser::SelectItem* item, DDLParser::Tag* tag )
{
  if ( tag != NULL )
  {
    Tag* self = new ( lua_newuserdata( L, sizeof( Tag ) ) ) Tag;

    if ( !self->Init( definition, select, item, tag ) )
    {
      return luaL_error( L, "Error creating DDLT::Tag" );
    }
    
    return SetMetatable( L );
  }
  
  return 0;
}

int DDLT::Tag::PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Bitfield* bitfield, DDLParser::BitfieldFlag* flag, DDLParser::Tag* tag )
{
  if ( tag != NULL )
  {
    Tag* self = new ( lua_newuserdata( L, sizeof( Tag ) ) ) Tag;

    if ( !self->Init( definition, bitfield, flag, tag ) )
    {
      return luaL_error( L, "Error creating DDLT::Tag" );
    }

    return SetMetatable( L );
  }
  
  return 0;
}

int DDLT::Tag::PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Struct* structure, DDLParser::StructField* field, DDLParser::Tag* tag )
{
  if ( tag != NULL )
  {
    Tag* self = new ( lua_newuserdata( L, sizeof( Tag ) ) ) Tag;

    if ( !self->Init( definition, structure, field, tag ) )
    {
      return luaL_error( L, "Error creating DDLT::Tag" );
    }

    return SetMetatable( L );
  }
  
  return 0;
}

DDLT::Tag* DDLT::Tag::Check( lua_State* L, int index )
{
  return (Tag*)luaL_checkudata( L, index, "DDLT::Tag" );
}

int DDLT::Tag::PushNext( lua_State* L )
{
  DDLParser::Tag* next = m_Tag->GetNext();
  
  switch ( m_AggregateType )
  {
  case DDLParser::kSelect:
    return PushNew( L, m_Definition, m_Select, m_Item, next );
  case DDLParser::kBitfield:
    return PushNew( L, m_Definition, m_Bitfield, m_Flag, next );
  case DDLParser::kStruct:
    return PushNew( L, m_Definition, m_Struct, m_Field, next );
  }
  
  return 0;
}
