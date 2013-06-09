#include <new>

#include <DDLParser.h>

#include "TagIf.h"
#include "BitfieldIf.h"
#include "DefinitionIf.h"
#include "Util.h"

extern "C"
{
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
};

// ########  #### ######## ######## #### ######## ##       ########  ######## ##          ###     ######
// ##     ##  ##     ##    ##        ##  ##       ##       ##     ## ##       ##         ## ##   ##    ##
// ##     ##  ##     ##    ##        ##  ##       ##       ##     ## ##       ##        ##   ##  ##
// ########   ##     ##    ######    ##  ######   ##       ##     ## ######   ##       ##     ## ##   ####
// ##     ##  ##     ##    ##        ##  ##       ##       ##     ## ##       ##       ######### ##    ##
// ##     ##  ##     ##    ##        ##  ##       ##       ##     ## ##       ##       ##     ## ##    ##
// ########  ####    ##    ##       #### ######## ######## ########  ##       ######## ##     ##  ######

int DDLT::BitfieldFlag::getName( lua_State* L )
{
  BitfieldFlag* self = DDLT::BitfieldFlag::Check( L, 1 );

  lua_pushstring( L, self->m_Flag->GetName() );
  return 1;
}

int DDLT::BitfieldFlag::getAuthor( lua_State* L )
{
  BitfieldFlag* self = DDLT::BitfieldFlag::Check( L, 1 );

  lua_pushstring( L, self->m_Flag->GetAuthor() );
  return 1;
}

int DDLT::BitfieldFlag::getDescription( lua_State* L )
{
  BitfieldFlag* self = DDLT::BitfieldFlag::Check( L, 1 );

  lua_pushstring( L, self->m_Flag->GetDescription() );
  return 1;
}

int DDLT::BitfieldFlag::getLabel( lua_State* L )
{
  BitfieldFlag* self = DDLT::BitfieldFlag::Check( L, 1 );

  lua_pushstring( L, self->m_Flag->GetLabel() );
  return 1;
}

int DDLT::BitfieldFlag::getDisplayLabel( lua_State* L )
{
  BitfieldFlag* self = DDLT::BitfieldFlag::Check( L, 1 );

  lua_pushstring( L, self->m_Flag->GetDisplayLabel() );
  return 1;
}

int DDLT::BitfieldFlag::getNameHash( lua_State* L )
{
  BitfieldFlag* self = DDLT::BitfieldFlag::Check( L, 1 );

  lua_pushnumber( L, self->m_Flag->GetNameHash() );
  return 1;
}

int DDLT::BitfieldFlag::getValue( lua_State* L )
{
  BitfieldFlag* self = DDLT::BitfieldFlag::Check( L, 1 );

  if ( self->m_Flag->GetBit() != 0 )
  {
    lua_pushnumber( L, 1 << ( self->m_Flag->GetBit() - 1 ) );
  }
  else
  {
    DDLParser::BitfieldFlagValue* value = self->m_Flag->GetValue();

    if ( value->GetCount() == 0 )
    {
      lua_pushnumber( L, 0 );
    }
    else
    {
      lua_newtable( L );

      for ( uint32_t i = 0; i < value->GetCount(); i++ )
      {
        DDLT::BitfieldFlag::PushNew( L, self->m_Definition, self->m_Bitfield, ( *self->m_Bitfield )[ value->GetFlagIndex( i ) ] );
        lua_rawseti( L, -2, i + 1 );
      }
    }
  }

  return 1;
}

int DDLT::BitfieldFlag::getOwner( lua_State* L )
{
  BitfieldFlag* self = DDLT::BitfieldFlag::Check( L, 1 );

  return Bitfield::PushNew( L, self->m_Definition, self->m_Bitfield );
}

int DDLT::BitfieldFlag::tagsIterator( lua_State* L )
{
  BitfieldFlag* self = Check( L, 1 );
  
  if ( lua_isnil( L, 2 ) )
  {
    return Tag::PushNew( L, self->m_Definition, self->m_Bitfield, self->m_Flag, self->m_Flag->GetTags() );
  }
  
  return Tag::Check( L, 2 )->PushNext( L );
}

int DDLT::BitfieldFlag::tags( lua_State* L )
{
  Check( L, 1 );
  
  lua_pushcfunction( L, tagsIterator );
  lua_pushvalue( L, 1 );
  lua_pushnil( L );
  return 3;
}

int DDLT::BitfieldFlag::l__index( lua_State* L )
{
  const char* key  = luaL_checkstring( L, 2 );
  uint32_t    hash = DDLParser::StringCrc32( key );

  switch ( hash )
  {
  case 0x96142173U: // getName
    lua_pushcfunction( L, getName );
    return 1;
  case 0x55eac3d6U: // getAuthor
    lua_pushcfunction( L, getAuthor );
    return 1;
  case 0x13f87889U: // getDescription
    lua_pushcfunction( L, getDescription );
    return 1;
  case 0xfc020d89U: // getLabel
    lua_pushcfunction( L, getLabel );
    return 1;
  case 0x8d3caf7aU: // getDisplayLabel
    lua_pushcfunction( L, getDisplayLabel );
    return 1;
  case 0x2709d01bU: // getNameHash
    lua_pushcfunction( L, getNameHash );
    return 1;
  case 0xefd20555U: // getValue
    lua_pushcfunction( L, getValue );
    return 1;
  case 0x3dc5bb1dU: // getOwner
    lua_pushcfunction( L, getOwner );
    return 1;
  case 0xff1efba8U: // tags
    lua_pushcfunction( L, tags );
    return 1;
  }

  return 0;
}

int DDLT::BitfieldFlag::l__gc( lua_State* L )
{
  BitfieldFlag* self = Check( L, 1 );

  self->Destroy();
  return 0;
}

int DDLT::BitfieldFlag::l__eq( lua_State* L )
{
  BitfieldFlag* p1 = Check( L, 1 );
  BitfieldFlag* p2 = Check( L, 2 );

  lua_pushboolean( L, p1->m_Flag == p2->m_Flag );
  return 1;
}

int DDLT::BitfieldFlag::l__tostring( lua_State* L )
{
  BitfieldFlag* self = Check( L, 1 );

  lua_pushfstring( L, "DDLT::BitfieldFlag@%p", self );
  return 1;
}

int DDLT::BitfieldFlag::PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Bitfield* bitfield, DDLParser::BitfieldFlag* flag )
{
  BitfieldFlag* self = new ( lua_newuserdata( L, sizeof( BitfieldFlag ) ) ) BitfieldFlag;

  if ( !self->Init( definition, bitfield, flag ) )
  {
    return luaL_error( L, "Error creating DDLT::BitfieldFlag" );
  }

  if ( luaL_newmetatable( L, "DDLT::BitfieldFlag" ) )
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

DDLT::BitfieldFlag* DDLT::BitfieldFlag::Check( lua_State* L, int index )
{
  return (BitfieldFlag*)luaL_checkudata( L, index, "DDLT::BitfieldFlag" );
}

// ########  #### ######## ######## #### ######## ##       ########
// ##     ##  ##     ##    ##        ##  ##       ##       ##     ##
// ##     ##  ##     ##    ##        ##  ##       ##       ##     ##
// ########   ##     ##    ######    ##  ######   ##       ##     ##
// ##     ##  ##     ##    ##        ##  ##       ##       ##     ##
// ##     ##  ##     ##    ##        ##  ##       ##       ##     ##
// ########  ####    ##    ##       #### ######## ######## ########

int DDLT::Bitfield::getType( lua_State* L )
{
  Check( L, 1 );

  lua_pushliteral( L, "bitfield" );
  return 1;
}

int DDLT::Bitfield::getName( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  lua_pushstring( L, self->m_Bitfield->GetName() );
  return 1;
}

int DDLT::Bitfield::getAuthor( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  lua_pushstring( L, self->m_Bitfield->GetAuthor() );
  return 1;
}

int DDLT::Bitfield::getDescription( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  lua_pushstring( L, self->m_Bitfield->GetDescription() );
  return 1;
}

int DDLT::Bitfield::getLabel( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  lua_pushstring( L, self->m_Bitfield->GetLabel() );
  return 1;
}

int DDLT::Bitfield::getDisplayLabel( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  lua_pushstring( L, self->m_Bitfield->GetDisplayLabel() );
  return 1;
}

int DDLT::Bitfield::getNameHash( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  lua_pushnumber( L, self->m_Bitfield->GetNameHash() );
  return 1;
}

int DDLT::Bitfield::getNumFlags( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  lua_pushinteger( L, self->m_Bitfield->GetNumFlags() );
  return 1;
}

int DDLT::Bitfield::getDefaultFlag( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  return BitfieldFlag::PushNew( L, self->m_Definition, self->m_Bitfield, ( *self->m_Bitfield )[ self->m_Bitfield->GetDefaultFlag() ] );
}

int DDLT::Bitfield::getFlag( lua_State* L )
{
  Bitfield* self = Check( L, 1 );
  uint32_t index = (uint32_t)luaL_checkint( L, 2 ) - 1;

  if ( index < self->m_Bitfield->GetNumFlags() )
  {
    return BitfieldFlag::PushNew( L, self->m_Definition, self->m_Bitfield, ( *self->m_Bitfield )[ index ] );
  }

  return 0;
}

int DDLT::Bitfield::findFlag( lua_State* L )
{
  Bitfield* self = Check( L, 1 );
  const char* name = luaL_checkstring( L, 2 );

  DDLParser::BitfieldFlag* flag = self->m_Bitfield->FindFlag( name );

  if ( flag != NULL )
  {
    return BitfieldFlag::PushNew( L, self->m_Definition, self->m_Bitfield, flag );
  }

  return 0;
}

int DDLT::Bitfield::getOwner( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  return Definition::PushNew( L, self->m_Definition );
}

int DDLT::Bitfield::flagsIterator( lua_State* L )
{
  Bitfield* self = Check( L, 1 );
  uint32_t index = (uint32_t)luaL_checkint( L, 2 );

  if ( index >= self->m_Bitfield->GetNumFlags() )
  {
    return 0;
  }

  lua_pushinteger( L, index + 1 );
  BitfieldFlag::PushNew( L, self->m_Definition, self->m_Bitfield, ( *self->m_Bitfield )[ index ] );
  return 2;
}

int DDLT::Bitfield::flags( lua_State* L )
{
  Check( L, 1 );

  lua_pushcfunction( L, flagsIterator );
  lua_pushvalue( L, 1 );
  lua_pushinteger( L, 0 );
  return 3;
}

int DDLT::Bitfield::tagsIterator( lua_State* L )
{
  Bitfield* self = Check( L, 1 );
  
  if ( lua_isnil( L, 2 ) )
  {
    return Tag::PushNew( L, self->m_Definition, self->m_Bitfield, NULL, self->m_Bitfield->GetTags() );
  }
  
  return Tag::Check( L, 2 )->PushNext( L );
}

int DDLT::Bitfield::tags( lua_State* L )
{
  Check( L, 1 );
  
  lua_pushcfunction( L, tagsIterator );
  lua_pushvalue( L, 1 );
  lua_pushnil( L );
  return 3;
}

int DDLT::Bitfield::l__index( lua_State* L )
{
  const char* key  = luaL_checkstring( L, 2 );
  uint32_t    hash = DDLParser::StringCrc32( key );

  switch ( hash )
  {
  case 0x44e9085cU:// getType
    lua_pushcfunction( L, getType );
    return 1;
  case 0x96142173U: // getName
    lua_pushcfunction( L, getName );
    return 1;
  case 0x55eac3d6U: // getAuthor
    lua_pushcfunction( L, getAuthor );
    return 1;
  case 0x13f87889U: // getDescription
    lua_pushcfunction( L, getDescription );
    return 1;
  case 0xfc020d89U: // getLabel
    lua_pushcfunction( L, getLabel );
    return 1;
  case 0x8d3caf7aU: // getDisplayLabel
    lua_pushcfunction( L, getDisplayLabel );
    return 1;
  case 0x2709d01bU: // getNameHash
    lua_pushcfunction( L, getNameHash );
    return 1;
  case 0xb9621bc2U: // getNumFlags
    lua_pushcfunction( L, getNumFlags );
    return 1;
  case 0x6cd616dcU: // getDefaultFlag
    lua_pushcfunction( L, getDefaultFlag );
    return 1;
  case 0x19c3b4efU: // getFlag
    lua_pushcfunction( L, getFlag );
    return 1;
  case 0x6031845aU: // findFlag
    lua_pushcfunction( L, findFlag );
    return 1;
  case 0x3dc5bb1dU: // getOwner
    lua_pushcfunction( L, getOwner );
    return 1;
  case 0xd397a27fU: // flags
    lua_pushcfunction( L, flags );
    return 1;
  case 0xff1efba8U: // tags
    lua_pushcfunction( L, tags );
    return 1;
  default:
    if ( lua_isnumber( L, 2 ) )
    {
      return getFlag( L );
    }
    else
    {
      return findFlag( L );
    }
    break;
  }

  return 0;
}

int DDLT::Bitfield::l__gc( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  self->Destroy();
  return 0;
}

int DDLT::Bitfield::l__eq( lua_State* L )
{
  Bitfield* p1 = Check( L, 1 );
  Bitfield* p2 = Check( L, 2 );

  lua_pushboolean( L, p1->m_Bitfield == p2->m_Bitfield );
  return 1;
}

int DDLT::Bitfield::l__tostring( lua_State* L )
{
  Bitfield* self = Check( L, 1 );

  lua_pushfstring( L, "DDLT::Bitfield@%p", self );
  return 1;
}

int DDLT::Bitfield::PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Bitfield* bitfield )
{
  Bitfield* self = new ( lua_newuserdata( L, sizeof( Bitfield ) ) ) Bitfield;

  if ( !self->Init( definition, bitfield ) )
  {
    return luaL_error( L, "Error creating DDLT::Bitfield" );
  }

  if ( luaL_newmetatable( L, "DDLT::Bitfield" ) )
  {
    lua_pushcfunction( L, getName );
    lua_setfield( L, -2, "__tostring" );
    lua_pushcfunction( L, getNumFlags );
    lua_setfield( L, -2, "__len" );
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

DDLT::Bitfield* DDLT::Bitfield::Check( lua_State* L, int index )
{
  return (Bitfield*)luaL_checkudata( L, index, "DDLT::Bitfield" );
}
