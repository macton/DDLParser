#include <new>

#include <DDLParser.h>

#include "TagIf.h"
#include "SelectIf.h"
#include "DefinitionIf.h"
#include "Util.h"

extern "C"
{
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
};

//  ######  ######## ##       ########  ######  ######## #### ######## ######## ##     ##
// ##    ## ##       ##       ##       ##    ##    ##     ##     ##    ##       ###   ###
// ##       ##       ##       ##       ##          ##     ##     ##    ##       #### ####
//  ######  ######   ##       ######   ##          ##     ##     ##    ######   ## ### ##
//       ## ##       ##       ##       ##          ##     ##     ##    ##       ##     ##
// ##    ## ##       ##       ##       ##    ##    ##     ##     ##    ##       ##     ##
//  ######  ######## ######## ########  ######     ##    ####    ##    ######## ##     ##

int DDLT::SelectItem::getName( lua_State* L )
{
  SelectItem* self = Check( L, 1 );

  lua_pushstring( L, self->m_Item->GetName() );
  return 1;
}

int DDLT::SelectItem::getAuthor( lua_State* L )
{
  SelectItem* self = Check( L, 1 );

  lua_pushstring( L, self->m_Item->GetAuthor() );
  return 1;
}

int DDLT::SelectItem::getDescription( lua_State* L )
{
  SelectItem* self = Check( L, 1 );

  lua_pushstring( L, self->m_Item->GetDescription() );
  return 1;
}

int DDLT::SelectItem::getLabel( lua_State* L )
{
  SelectItem* self = Check( L, 1 );

  lua_pushstring( L, self->m_Item->GetLabel() );
  return 1;
}

int DDLT::SelectItem::getDisplayLabel( lua_State* L )
{
  SelectItem* self = Check( L, 1 );

  lua_pushstring( L, self->m_Item->GetDisplayLabel() );
  return 1;
}

int DDLT::SelectItem::getNameHash( lua_State* L )
{
  SelectItem* self = Check( L, 1 );

  lua_pushnumber( L, self->m_Item->GetNameHash() );
  return 1;
}

int DDLT::SelectItem::getOwner( lua_State* L )
{
  SelectItem* self = Check( L, 1 );

  return Select::PushNew( L, self->m_Definition, self->m_Select );
}

int DDLT::SelectItem::tagsIterator( lua_State* L )
{
  SelectItem* self = Check( L, 1 );
  
  if ( lua_isnil( L, 2 ) )
  {
    return Tag::PushNew( L, self->m_Definition, self->m_Select, self->m_Item, self->m_Item->GetTags() );
  }
  
  return Tag::Check( L, 2 )->PushNext( L );
}

int DDLT::SelectItem::tags( lua_State* L )
{
  Check( L, 1 );
  
  lua_pushcfunction( L, tagsIterator );
  lua_pushvalue( L, 1 );
  lua_pushnil( L );
  return 3;
}

int DDLT::SelectItem::l__index( lua_State* L )
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
  case 0x3dc5bb1dU: // getOwner
    lua_pushcfunction( L, getOwner );
    return 1;
  case 0xff1efba8U: // tags
    lua_pushcfunction( L, tags );
    return 1;
  }

  return 0;
}

int DDLT::SelectItem::l__gc( lua_State* L )
{
  SelectItem* item = Check( L, 1 );

  item->Destroy();
  return 0;
}

int DDLT::SelectItem::l__eq( lua_State* L )
{
  SelectItem* p1 = Check( L, 1 );
  SelectItem* p2 = Check( L, 2 );

  lua_pushboolean( L, p1->m_Item == p2->m_Item );
  return 1;
}

int DDLT::SelectItem::l__tostring( lua_State* L )
{
  SelectItem* self = Check( L, 1 );

  lua_pushfstring( L, "DDLT::SelectItem@%p", self );
  return 1;
}

int DDLT::SelectItem::PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Select* select, DDLParser::SelectItem* item )
{
  SelectItem* self = new ( lua_newuserdata( L, sizeof( SelectItem ) ) ) SelectItem;

  if ( !self->Init( definition, select, item ) )
  {
    return luaL_error( L, "Error creating DDLT::SelectItem" );
  }

  if ( luaL_newmetatable( L, "DDLT::SelectItem" ) )
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

DDLT::SelectItem* DDLT::SelectItem::Check( lua_State* L, int index )
{
  return (SelectItem*)luaL_checkudata( L, index, "DDLT::SelectItem" );
}

//  ######  ######## ##       ########  ######  ########
// ##    ## ##       ##       ##       ##    ##    ##
// ##       ##       ##       ##       ##          ##
//  ######  ######   ##       ######   ##          ##
//       ## ##       ##       ##       ##          ##
// ##    ## ##       ##       ##       ##    ##    ##
//  ######  ######## ######## ########  ######     ##

int DDLT::Select::getType( lua_State* L )
{
  Check( L, 1 );

  lua_pushliteral( L, "select" );
  return 1;
}

int DDLT::Select::getName( lua_State* L )
{
  Select* self = Check( L, 1 );

  lua_pushstring( L, self->m_Select->GetName() );
  return 1;
}

int DDLT::Select::getAuthor( lua_State* L )
{
  Select* self = Check( L, 1 );

  lua_pushstring( L, self->m_Select->GetAuthor() );
  return 1;
}

int DDLT::Select::getDescription( lua_State* L )
{
  Select* self = Check( L, 1 );

  lua_pushstring( L, self->m_Select->GetDescription() );
  return 1;
}

int DDLT::Select::getLabel( lua_State* L )
{
  Select* self = Check( L, 1 );

  lua_pushstring( L, self->m_Select->GetLabel() );
  return 1;
}

int DDLT::Select::getDisplayLabel( lua_State* L )
{
  Select* self = Check( L, 1 );

  lua_pushstring( L, self->m_Select->GetDisplayLabel() );
  return 1;
}

int DDLT::Select::getNameHash( lua_State* L )
{
  Select* self = Check( L, 1 );

  lua_pushnumber( L, self->m_Select->GetNameHash() );
  return 1;
}

int DDLT::Select::getNumItems( lua_State* L )
{
  Select* self = Check( L, 1 );

  lua_pushinteger( L, self->m_Select->GetNumItems() );
  return 1;
}

int DDLT::Select::getDefaultItem( lua_State* L )
{
  Select* self = Check( L, 1 );

  return SelectItem::PushNew( L, self->m_Definition, self->m_Select, ( *self->m_Select )[ self->m_Select->GetDefaultItem() ] );
}

int DDLT::Select::getItem( lua_State* L )
{
  Select* self = Check( L, 1 );
  uint32_t index = (uint32_t)luaL_checkint( L, 2 ) - 1;

  if ( index < self->m_Select->GetNumItems() )
  {
    return SelectItem::PushNew( L, self->m_Definition, self->m_Select, ( *self->m_Select )[ index ] );
  }

  return 0;
}

int DDLT::Select::findItem( lua_State* L )
{
  Select* self = Check( L, 1 );
  const char* name = luaL_checkstring( L, 2 );

  DDLParser::SelectItem* item = self->m_Select->FindItem( name );

  if ( item != NULL )
  {
    return SelectItem::PushNew( L, self->m_Definition, self->m_Select, item );
  }

  return 0;
}

int DDLT::Select::getOwner( lua_State* L )
{
  Select* self = Check( L, 1 );

  return Definition::PushNew( L, self->m_Definition );
}

int DDLT::Select::itemsIterator( lua_State* L )
{
  Select* self = Check( L, 1 );
  uint32_t index = (uint32_t)luaL_checkint( L, 2 );

  if ( index >= self->m_Select->GetNumItems() )
  {
    return 0;
  }

  lua_pushinteger( L, index + 1 );
  SelectItem::PushNew( L, self->m_Definition, self->m_Select, ( *self->m_Select )[ index ] );
  return 2;
}

int DDLT::Select::items( lua_State* L )
{
  Check( L, 1 );

  lua_pushcfunction( L, itemsIterator );
  lua_pushvalue( L, 1 );
  lua_pushinteger( L, 0 );
  return 3;
}

int DDLT::Select::tagsIterator( lua_State* L )
{
  Select* self = Check( L, 1 );
  
  if ( lua_isnil( L, 2 ) )
  {
    return Tag::PushNew( L, self->m_Definition, self->m_Select, NULL, self->m_Select->GetTags() );
  }
  
  return Tag::Check( L, 2 )->PushNext( L );
}

int DDLT::Select::tags( lua_State* L )
{
  Check( L, 1 );
  
  lua_pushcfunction( L, tagsIterator );
  lua_pushvalue( L, 1 );
  lua_pushnil( L );
  return 3;
}

int DDLT::Select::l__index( lua_State* L )
{
  const char* key = luaL_checkstring( L, 2 );
  uint32_t hash = DDLParser::StringCrc32( key );

  switch ( hash )
  {
  case 0x44e9085cU: // getType
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
  case 0x5379b335U: // getNumItems
    lua_pushcfunction( L, getNumItems );
    return 1;
  case 0xa239d858U: // getDefaultItem
    lua_pushcfunction( L, getDefaultItem );
    return 1;
  case 0xd72c7a6bU: // getItem
    lua_pushcfunction( L, getItem );
    return 1;
  case 0xaede4adeU: // findItem
    lua_pushcfunction( L, findItem );
    return 1;
  case 0x3dc5bb1dU: // getOwner
    lua_pushcfunction( L, getOwner );
    return 1;
  case 0x398c0a88U: // items
    lua_pushcfunction( L, items );
    return 1;
  case 0xff1efba8U: // tags
    lua_pushcfunction( L, tags );
    return 1;
  default:
    if ( lua_isnumber( L, 2 ) )
    {
      return getItem( L );
    }
    else
    {
      return findItem( L );
    }
  }

  return 0;
}

int DDLT::Select::l__gc( lua_State* L )
{
  Select* self = Check( L, 1 );

  self->Destroy();
  return 0;
}

int DDLT::Select::l__eq( lua_State* L )
{
  Select* p1 = Check( L, 1 );
  Select* p2 = Check( L, 2 );

  lua_pushboolean( L, p1->m_Select == p2->m_Select );
  return 1;
}

int DDLT::Select::l__tostring( lua_State* L )
{
  Select* self = Check( L, 1 );

  lua_pushfstring( L, "DDLT::Select@%p", self );
  return 1;
}

int DDLT::Select::PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Select* select )
{
  Select* self = new ( lua_newuserdata( L, sizeof( Select ) ) ) Select;

  if ( !self->Init( definition, select ) )
  {
    return luaL_error( L, "Error creating DDLT::Select" );
  }

  if ( luaL_newmetatable( L, "DDLT::Select" ) )
  {
    lua_pushcfunction( L, getNumItems );
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

DDLT::Select* DDLT::Select::Check( lua_State* L, int index )
{
  return (Select*)luaL_checkudata( L, index, "DDLT::Select" );
}
