#include <new>
#include <string.h>

#include <DDLParser.h>

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

int DDLT::Definition::getNumAggregates( lua_State* L )
{
  Definition* self = Check( L, 1 );

  lua_pushinteger( L, self->m_Definition->GetNumAggregates() );
  return 1;
}

int DDLT::Definition::getAggregate( lua_State* L )
{
  Definition* self = Check( L, 1 );
  uint32_t index = (uint32_t)luaL_checkint( L, 2 ) - 1;

  if ( index < self->m_Definition->GetNumAggregates() )
  {
    return self->PushAggregate( L, ( *self->m_Definition )[ index ] );
  }

  return 0;
}

int DDLT::Definition::findAggregate( lua_State* L )
{
  Definition* self = Check( L, 1 );
  const char* name = luaL_checkstring( L, 2 );
  DDLParser::Aggregate* aggregate = self->m_Definition->FindAggregate( name );

  if ( aggregate != NULL )
  {
    return self->PushAggregate( L, aggregate );
  }

  return 0;
}

int DDLT::Definition::aggregatesIterator( lua_State* L )
{
  Definition* self = Check( L, 1 );
  uint32_t index = (uint32_t)luaL_checkint( L, 2 );
  uint32_t type  = (uint32_t)luaL_checkint( L, lua_upvalueindex( 1 ) );

  while ( index < self->m_Definition->GetNumAggregates() )
  {
    DDLParser::Aggregate* aggregate = ( *self->m_Definition )[ index ];

    if ( aggregate->GetType() == type || type == DDLParser::kBoolean )
    {
      break;
    }

    index++;
  }

  if ( index >= self->m_Definition->GetNumAggregates() )
  {
    return 0;
  }

  lua_pushinteger( L, index + 1 );
  self->PushAggregate( L, ( *self->m_Definition )[ index ] );
  return 2;
}

int DDLT::Definition::aggregates( lua_State* L )
{
  Check( L, 1 );

  lua_pushinteger( L, DDLParser::kBoolean ); // A type different from any aggregate.
  lua_pushcclosure( L, Definition::aggregatesIterator, 1 );
  lua_pushvalue( L, 1 );
  lua_pushinteger( L, 0 );
  return 3;
}

int DDLT::Definition::selects( lua_State* L )
{
  Check( L, 1 );

  lua_pushinteger( L, DDLParser::kSelect );
  lua_pushcclosure( L, Definition::aggregatesIterator, 1 );
  lua_pushvalue( L, 1 );
  lua_pushinteger( L, 0 );
  return 3;
}

int DDLT::Definition::bitfields( lua_State* L )
{
  Check( L, 1 );

  lua_pushinteger( L, DDLParser::kBitfield );
  lua_pushcclosure( L, aggregatesIterator, 1 );
  lua_pushvalue( L, 1 );
  lua_pushinteger( L, 0 );
  return 3;
}

int DDLT::Definition::structs( lua_State* L )
{
  Check( L, 1 );

  lua_pushinteger( L, DDLParser::kStruct );
  lua_pushcclosure( L, aggregatesIterator, 1 );
  lua_pushvalue( L, 1 );
  lua_pushinteger( L, 0 );
  return 3;
}

int DDLT::Definition::dumpDefinition( lua_State* L )
{
  Definition* self = Check( L, 1 );
  int indent = luaL_optint( L, 2, 0 );

  luaL_Buffer dump;
  luaL_buffinit( L, &dump );

  const uint8_t* data = (const uint8_t*)self->m_Definition;
  size_t size = self->m_Definition->GetTotalSize();

  for ( size_t i = 0; i < size; i += 16 )
  {
    char line[ 136 ];
    memset( line, ' ', sizeof( line ) );

    line[ 0 ] = '/';
    line[ 1 ] = '*';
    line[ 3 ] = '0';
    line[ 4 ] = 'x';

    PutHex( line + 5, i, 8 );

    line[ 14 ] = '*';
    line[ 15 ] = '/';

    char* curr = line + 17;

    for ( size_t j = i; j < ( i + 16 ) && j < size; j++ )
    {
      curr[ 0 ] = '0';
      curr[ 1 ] = 'x';

      PutHex( curr + 2, data[ j ], 2 );

      curr[ 4 ] = ',';
      curr += 6;
    }
    
    curr = line + 113;
    *curr++ = '/';
    *curr++ = '*';
    curr++;

    for ( size_t j = i; j < ( i + 16 ) && j < size; j++ )
    {
      uint8_t byte = data[ j ];

      // Don't print non-ANSI characters as per http://msdn.microsoft.com/en-us/library/aa245259(VS.60).aspx
      if ( byte > 31 && byte != 127 && byte != 128 && byte != 129 && byte != 141 && byte != 142 && byte != 143 && byte != 144 && byte != 157 && byte != 158 )
      {
        *curr++ = byte;
      }
      else
      {
        *curr++ = '.';
      }
    }

    line[ 133 ] = '*';
    line[ 134 ] = '/';
    line[ 135 ] = 0;

    for ( int i = indent * 2; i >= 0; i-- )
    {
      luaL_addchar( &dump, ' ' );
    }

    luaL_addlstring( &dump, line, sizeof( line ) );
  }

  luaL_pushresult( &dump );
  return 1;
}

int DDLT::Definition::l__index( lua_State* L )
{
  const char* key = luaL_checkstring( L, 2 );
  uint32_t hash = DDLParser::StringCrc32( key );

  switch ( hash )
  {
  case 0x06e0fb85U: // getNumAggregates
    lua_pushcfunction( L, getNumAggregates );
    return 1;
  case 0xf10060faU: // getAggregate
    lua_pushcfunction( L, getAggregate );
    return 1;
  case 0x91b17a3eU: // findAggregate
    lua_pushcfunction( L, findAggregate );
    return 1;
  case 0xa2d00426U: // aggregates
    lua_pushcfunction( L, aggregates );
    return 1;
  case 0x4e76a5f5U: // selects
    lua_pushcfunction( L, selects );
    return 1;
  case 0x8c34ada0U: // bitfields
    lua_pushcfunction( L, bitfields );
    return 1;
  case 0x09350ab7U: // structs
    lua_pushcfunction( L, structs );
    return 1;
  default:
    if ( lua_isnumber( L, 2 ) )
    {
      return getAggregate( L );
    }
    else
    {
      return findAggregate( L );
    }
  }

  return 0;
}

int DDLT::Definition::l__gc( lua_State* L )
{
  Definition* self = Check( L, 1 );

  self->Destroy();
  return 0;
}

int DDLT::Definition::l__eq( lua_State* L )
{
  Definition* p1 = Check( L, 1 );
  Definition* p2 = Check( L, 2 );

  lua_pushboolean( L, p1->m_Definition == p2->m_Definition );
  return 1;
}

int DDLT::Definition::PushAggregate( lua_State* L, DDLParser::Aggregate* aggregate )
{
  switch ( aggregate->GetType() )
  {
  case DDLParser::kSelect:
    return Select::PushNew( L, m_Definition, aggregate->ToSelect() );
  case DDLParser::kBitfield:
    return Bitfield::PushNew( L, m_Definition, aggregate->ToBitfield() );
  case DDLParser::kStruct:
    return Struct::PushNew( L, m_Definition, aggregate->ToStruct() );
  }

  return 0;
}

int DDLT::Definition::PushNew( lua_State* L, DDLParser::Definition* definition )
{
  Definition* self = new ( lua_newuserdata( L, sizeof( Definition ) ) ) Definition;

  if ( !self->Init( definition ) )
  {
    return luaL_error( L, "Error creating DDLT::Definition" );
  }

  if ( luaL_newmetatable( L, "DDLT::Definition" ) )
  {
    lua_pushcfunction( L, getNumAggregates );
    lua_setfield( L, -2, "__len" );
    lua_pushcfunction( L, l__index );
    lua_setfield( L, -2, "__index" );
    lua_pushcfunction( L, l__gc );
    lua_setfield( L, -2, "__gc" );
    lua_pushcfunction( L, l__eq );
    lua_setfield( L, -2, "__eq" );
  }

  lua_setmetatable( L, -2 );
  return 1;
}

DDLT::Definition* DDLT::Definition::Check( lua_State* L, int index )
{
  return (Definition*)luaL_checkudata( L, index, "DDLT::Definition" );
}
