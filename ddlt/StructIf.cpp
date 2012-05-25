#include <new>
#include <string.h>

#include <DDLParser.h>

#include "DefinitionIf.h"
#include "StructIf.h"
#include "SelectIf.h"
#include "BitfieldIf.h"
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

static int PushStructFieldValue( lua_State* L, DDLParser::Definition* definition, DDLParser::StructValueInfo* value_info, uint32_t ndx )
{
  char value_str[ 128 ];

  DDLParser::StructFieldValue* value = value_info->GetValue();

  switch ( value_info->GetType() )
  {
  case DDLParser::kUint8:
    snprintf( value_str, sizeof( value_str ), "%u", value->m_Uint8[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kUint16:
    snprintf( value_str, sizeof( value_str ), "%u", value->m_Uint16[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kUint32:
    snprintf( value_str, sizeof( value_str ), "%u", value->m_Uint32[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kUint64:
    snprintf( value_str, sizeof( value_str ), "%llu", value->m_Uint64[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kInt8:
    snprintf( value_str, sizeof( value_str ), "%d", value->m_Int8[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kInt16:
    snprintf( value_str, sizeof( value_str ), "%d", value->m_Int16[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kInt32:
    snprintf( value_str, sizeof( value_str ), "%d", value->m_Int32[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kInt64:
    snprintf( value_str, sizeof( value_str ), "%lld", value->m_Int64[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kFloat32:
    snprintf( value_str, sizeof( value_str ), "%f", value->m_Float32[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kFloat64:
    snprintf( value_str, sizeof( value_str ), "%f", value->m_Float64[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kString:
    lua_pushstring( L, &*value->m_String[ ndx ] );
    return 1;
  case DDLParser::kSelect:
    {
      DDLParser::Select*     select = definition->FindAggregate( value_info->GetTypeNameHash() )->ToSelect();
      DDLParser::SelectItem* item   = select->FindItem( value->m_Select[ ndx ] );
      DDLT::SelectItem::PushNew( L, definition, select, item );
    }
    return 1;
  case DDLParser::kBitfield:
    {
      DDLParser::Bitfield*            bitfield = definition->FindAggregate( value_info->GetTypeNameHash() )->ToBitfield();
      DDLParser::StructBitfieldValue* bv       = &*value->m_Bitfield[ ndx ];

      lua_newtable( L );

      for ( uint32_t j = 0; j < bv->GetCount(); j++ )
      {
        DDLT::BitfieldFlag::PushNew( L, definition, bitfield, bitfield->FindFlag( bv->GetHash( j ) ) );
        lua_rawseti( L, -2, j + 1 );
      }
    }
    return 1;
  case DDLParser::kStruct:
    {
      DDLParser::Struct*            structure = definition->FindAggregate( value_info->GetTypeNameHash() )->ToStruct();
      DDLParser::StructStructValue* sv        = &*value->m_Struct[ ndx ];

      lua_newtable( L );

      for ( uint32_t j = 0; j < sv->GetCount(); j++ )
      {
        DDLParser::StructValueInfo* value_info = sv->GetValueInfo( j );
        DDLParser::StructField*     field      = structure->FindField( value_info->GetNameHash() );

        DDLT::StructValueInfo::PushNew( L, definition, structure, field, value_info, field->GetName() );
        lua_rawseti( L, -2, j + 1 );
      }
    }
    return 1;
  case DDLParser::kBoolean:
    strncpy( value_str, value->m_Boolean[ ndx ] ? "true" : "false", sizeof( value_str ) );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kFile:
    lua_pushstring( L, &*value->m_File[ ndx ] );
    return 1;
  case DDLParser::kTuid:
    snprintf( value_str, sizeof( value_str ), "%llu", value->m_Tuid[ ndx ] );
    lua_pushstring( L, value_str );
    return 1;
  case DDLParser::kJson:
    lua_pushstring( L, &*value->m_Json[ ndx ] );
    return 1;
  default: // To avoid warnings.
    break;
  }
  
  return 0;
}

//  ######  ######## ########  ##     ##  ######  ######## ##     ##    ###    ##       ##     ## ######## #### ##    ## ########  #######
// ##    ##    ##    ##     ## ##     ## ##    ##    ##    ##     ##   ## ##   ##       ##     ## ##        ##  ###   ## ##       ##     ##
// ##          ##    ##     ## ##     ## ##          ##    ##     ##  ##   ##  ##       ##     ## ##        ##  ####  ## ##       ##     ##
//  ######     ##    ########  ##     ## ##          ##    ##     ## ##     ## ##       ##     ## ######    ##  ## ## ## ######   ##     ##
//       ##    ##    ##   ##   ##     ## ##          ##     ##   ##  ######### ##       ##     ## ##        ##  ##  #### ##       ##     ##
// ##    ##    ##    ##    ##  ##     ## ##    ##    ##      ## ##   ##     ## ##       ##     ## ##        ##  ##   ### ##       ##     ##
//  ######     ##    ##     ##  #######   ######     ##       ###    ##     ## ########  #######  ######## #### ##    ## ##        #######

int DDLT::StructValueInfo::getNameHash( lua_State* L )
{
  StructValueInfo* self = Check( L, 1 );

  lua_pushnumber( L, self->m_ValueInfo->GetNameHash() );
  return 1;
}

int DDLT::StructValueInfo::getType( lua_State* L )
{
  StructValueInfo* self = Check( L, 1 );

  PushDDLType( L, self->m_ValueInfo->GetType() );
  return 1;
}

int DDLT::StructValueInfo::getDeclaredType( lua_State* L )
{
  StructValueInfo* self = Check( L, 1 );

  switch ( self->m_ValueInfo->GetType() )
  {
  case DDLParser::kSelect:
  case DDLParser::kBitfield:
  case DDLParser::kStruct:
    break;
  default:
    return 0;
  }

  DDLParser::Aggregate* aggregate = self->m_Definition->FindAggregate( self->m_ValueInfo->GetTypeNameHash() );

  switch ( aggregate->GetType() )
  {
  case DDLParser::kSelect:
    return Select::PushNew( L, self->m_Definition, aggregate->ToSelect() );
  case DDLParser::kBitfield:
    return Bitfield::PushNew( L, self->m_Definition, aggregate->ToBitfield() );
  case DDLParser::kStruct:
    return Struct::PushNew( L, self->m_Definition, aggregate->ToStruct() );
  }

  return 0;
}

int DDLT::StructValueInfo::getArrayType( lua_State* L )
{
  StructValueInfo* self = Check( L, 1 );

  PushDDLArrayType( L, self->m_ValueInfo->GetArrayType() );
  return 1;
}

int DDLT::StructValueInfo::getArrayCount( lua_State* L )
{
  StructValueInfo* self = Check( L, 1 );

  if ( self->m_ValueInfo->GetArrayType() == DDLParser::kFixed )
  {
    lua_pushnumber( L, self->m_ValueInfo->GetCount() );
    return 1;
  }

  return 0;
}

int DDLT::StructValueInfo::getValue( lua_State* L )
{
  StructValueInfo* self = Check( L, 1 );
  DDLParser::StructFieldValue* value = self->m_ValueInfo->GetValue();

  if ( value == NULL )
  {
    return 0;
  }

  if ( self->m_ValueInfo->GetArrayType() == DDLParser::kScalar )
  {
    PushStructFieldValue( L, self->m_Definition, self->m_ValueInfo, 0 );
    return 1;
  }

  lua_newtable( L );

  for ( uint32_t i = 0; i < self->m_ValueInfo->GetCount(); i++ )
  {
    PushStructFieldValue( L, self->m_Definition, self->m_ValueInfo, i );
    lua_rawseti( L, -2, i + 1 );
  }

  return 1;
}

int DDLT::StructValueInfo::getName( lua_State* L )
{
  StructValueInfo* self = Check( L, 1 );

  lua_pushstring( L, self->m_Name );
  return 1;
}

int DDLT::StructValueInfo::l__index( lua_State* L )
{
  const char* key  = luaL_checkstring( L, 2 );
  uint32_t    hash = DDLParser::StringCrc32( key );

  switch ( hash )
  {
  case 0x2709d01bU: // getNameHash
    lua_pushcfunction( L, getNameHash );
    return 1;
  case 0x44e9085cU: // getType
    lua_pushcfunction( L, getType );
    return 1;
  case 0x3b052225U: // getDeclaredType
    lua_pushcfunction( L, getDeclaredType );
    return 1;
  case 0x442886e9U: // getArrayType
    lua_pushcfunction( L, getArrayType );
    return 1;
  case 0xcc779f8eU: // getArrayCount
    lua_pushcfunction( L, getArrayCount );
    return 1;
  case 0xefd20555UL: // getValue
    lua_pushcfunction( L, getValue );
    return 1;
  case 0x96142173U: // getName
    lua_pushcfunction( L, getName );
    return 1;
  }

  return 0;
}

int DDLT::StructValueInfo::l__gc( lua_State* L )
{
  StructValueInfo* self = Check( L, 1 );

  self->Destroy();
  return 0;
}

int DDLT::StructValueInfo::l__eq( lua_State* L )
{
  StructValueInfo* p1 = Check( L, 1 );
  StructValueInfo* p2 = Check( L, 2 );

  lua_pushboolean( L, p1->m_ValueInfo == p2->m_ValueInfo );
  return 1;
}

int DDLT::StructValueInfo::l__tostring( lua_State* L )
{
  StructValueInfo* self = Check( L, 1 );

  lua_pushfstring( L, "DDLT::StructValueInfo@%p", self );
  return 1;
}

int DDLT::StructValueInfo::PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Struct* structure, DDLParser::StructField* field, DDLParser::StructValueInfo* value_info, const char* name )
{
  StructValueInfo* self = new ( lua_newuserdata( L, sizeof( StructValueInfo ) ) ) StructValueInfo;

  if ( !self->Init( definition, structure, field, value_info, name ) )
  {
    return luaL_error( L, "Error creating DDLT::StructValueInfo" );
  }

  if ( luaL_newmetatable( L, "DDLT::StructValueInfo" ) )
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

DDLT::StructValueInfo* DDLT::StructValueInfo::Check( lua_State* L, int index )
{
  return (StructValueInfo*)luaL_checkudata( L, index, "DDLT::StructValueInfo" );
}

//  ######  ######## ########  ##     ##  ######  ######## ######## #### ######## ##       ########
// ##    ##    ##    ##     ## ##     ## ##    ##    ##    ##        ##  ##       ##       ##     ##
// ##          ##    ##     ## ##     ## ##          ##    ##        ##  ##       ##       ##     ##
//  ######     ##    ########  ##     ## ##          ##    ######    ##  ######   ##       ##     ##
//       ##    ##    ##   ##   ##     ## ##          ##    ##        ##  ##       ##       ##     ##
// ##    ##    ##    ##    ##  ##     ## ##    ##    ##    ##        ##  ##       ##       ##     ##
//  ######     ##    ##     ##  #######   ######     ##    ##       #### ######## ######## ########

int DDLT::StructField::getName( lua_State* L )
{
  StructField* self = Check( L, 1 );

  lua_pushstring( L, self->m_Field->GetName() );
  return 1;
}

int DDLT::StructField::getAuthor( lua_State* L )
{
  StructField* self = Check( L, 1 );

  lua_pushstring( L, self->m_Field->GetAuthor() );
  return 1;
}

int DDLT::StructField::getDescription( lua_State* L )
{
  StructField* self = Check( L, 1 );

  lua_pushstring( L, self->m_Field->GetDescription() );
  return 1;
}

int DDLT::StructField::getLabel( lua_State* L )
{
  StructField* self = Check( L, 1 );

  lua_pushstring( L, self->m_Field->GetLabel() );
  return 1;
}

int DDLT::StructField::getDisplayLabel( lua_State* L )
{
  StructField* self = Check( L, 1 );

  lua_pushstring( L, self->m_Field->GetDisplayLabel() );
  return 1;
}

int DDLT::StructField::getNameHash( lua_State* L )
{
  StructField* self = Check( L, 1 );

  lua_pushnumber( L, self->m_Field->GetValueInfo()->GetNameHash() );
  return 1;
}

int DDLT::StructField::getType( lua_State* L )
{
  StructField* self = Check( L, 1 );

  PushDDLType( L, self->m_Field->GetValueInfo()->GetType() );
  return 1;
}

int DDLT::StructField::getDeclaredType( lua_State* L )
{
  StructField* self = Check( L, 1 );

  switch ( self->m_Field->GetValueInfo()->GetType() )
  {
  case DDLParser::kSelect:
  case DDLParser::kBitfield:
  case DDLParser::kStruct:
    break;
  default:
    return 0;
  }

  DDLParser::Aggregate* aggregate = self->m_Definition->FindAggregate( self->m_Field->GetValueInfo()->GetTypeNameHash() );

  switch ( aggregate->GetType() )
  {
  case DDLParser::kSelect:
    return Select::PushNew( L, self->m_Definition, aggregate->ToSelect() );
  case DDLParser::kBitfield:
    return Bitfield::PushNew( L, self->m_Definition, aggregate->ToBitfield() );
  case DDLParser::kStruct:
    return Struct::PushNew( L, self->m_Definition, aggregate->ToStruct() );
  }

  return 0;
}

int DDLT::StructField::getArrayType( lua_State* L )
{
  StructField* self = Check( L, 1 );

  PushDDLArrayType( L, self->m_Field->GetValueInfo()->GetArrayType() );
  return 1;
}

int DDLT::StructField::getArrayCount( lua_State* L )
{
  StructField* self = Check( L, 1 );

  if ( self->m_Field->GetValueInfo()->GetArrayType() == DDLParser::kFixed )
  {
    lua_pushnumber( L, self->m_Field->GetValueInfo()->GetCount() );
    return 1;
  }

  return 0;
}

int DDLT::StructField::getValue( lua_State* L )
{
  StructField* self = Check( L, 1 );
  DDLParser::StructFieldValue* value = self->m_Field->GetValueInfo()->GetValue();

  if ( value == NULL )
  {
    return 0;
  }

  if ( self->m_Field->GetValueInfo()->GetArrayType() == DDLParser::kScalar )
  {
    PushStructFieldValue( L, self->m_Definition, self->m_Field->GetValueInfo(), 0 );
    return 1;
  }

  lua_newtable( L );

  for ( uint32_t i = 0; i < self->m_Field->GetValueInfo()->GetCount(); i++ )
  {
    PushStructFieldValue( L, self->m_Definition, self->m_Field->GetValueInfo(), i );
    lua_rawseti( L, -2, i + 1 );
  }

  return 1;
}

int DDLT::StructField::getKeyType( lua_State* L )
{
  StructField* self = Check( L, 1 );

  if ( self->m_Field->GetValueInfo()->GetArrayType() == DDLParser::kHashmap )
  {
    PushDDLType( L, self->m_Field->GetValueInfo()->GetKeyType() );
    return 1;
  }
  
  return 0;
}

int DDLT::StructField::getOwner( lua_State* L )
{
  StructField* self = Check( L, 1 );

  Struct::PushNew( L, self->m_Definition, self->m_Struct );
  return 1;
}

int DDLT::StructField::isInherited( lua_State* L )
{
  StructField* self = Check( L, 1 );

  lua_pushboolean( L, self->m_Struct->IsInherited( self->m_Field ) ? 1 : 0 );
  return 1;
}

int DDLT::StructField::l__index( lua_State* L )
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
  case 0x44e9085cU: // getType
    lua_pushcfunction( L, getType );
    return 1;
  case 0x3b052225U: // getDeclaredType
    lua_pushcfunction( L, getDeclaredType );
    return 1;
  case 0x442886e9U: // getArrayType
    lua_pushcfunction( L, getArrayType );
    return 1;
  case 0xcc779f8eU: // getArrayCount
    lua_pushcfunction( L, getArrayCount );
    return 1;
  case 0xefd20555U: // getValue
    lua_pushcfunction( L, getValue );
    return 1;
  case 0x9d6b6bc8U: // getKeyType
    lua_pushcfunction( L, getKeyType );
    return 1;
  case 0x3dc5bb1dU: // getOwner
    lua_pushcfunction( L, getOwner );
    return 1;
  case 0xc5819371U: // isInherited
    lua_pushcfunction( L, isInherited );
    return 1;
  }

  return 0;
}

int DDLT::StructField::l__gc( lua_State* L )
{
  StructField* self = Check( L, 1 );

  self->Destroy();
  return 0;
}

int DDLT::StructField::l__eq( lua_State* L )
{
  StructField* p1 = Check( L, 1 );
  StructField* p2 = Check( L, 2 );

  lua_pushboolean( L, p1->m_Field == p2->m_Field );
  return 1;
}

int DDLT::StructField::l__tostring( lua_State* L )
{
  StructField* self = Check( L, 1 );

  lua_pushfstring( L, "DDLT::StructField@%p", self );
  return 1;
}

int DDLT::StructField::PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Struct* structure, DDLParser::StructField* field )
{
  StructField* self = new ( lua_newuserdata( L, sizeof( StructField ) ) ) StructField;

  if ( !self->Init( definition, structure, field ) )
  {
    return luaL_error( L, "Error creating DDLT::StructField" );
  }

  if ( luaL_newmetatable( L, "DDLT::StructField" ) )
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

DDLT::StructField* DDLT::StructField::Check( lua_State* L, int index )
{
  return (StructField*)luaL_checkudata( L, index, "DDLT::StructField" );
}

//  ######  ######## ########  ##     ##  ######  ########
// ##    ##    ##    ##     ## ##     ## ##    ##    ##
// ##          ##    ##     ## ##     ## ##          ##
//  ######     ##    ########  ##     ## ##          ##
//       ##    ##    ##   ##   ##     ## ##          ##
// ##    ##    ##    ##    ##  ##     ## ##    ##    ##
//  ######     ##    ##     ##  #######   ######     ##

int DDLT::Struct::getType( lua_State* L )
{
  Check( L, 1 );

  lua_pushliteral( L, "struct" );
  return 1;
}

int DDLT::Struct::getName( lua_State* L )
{
  Struct* self = Check( L, 1 );

  lua_pushstring( L, self->m_Struct->GetName() );

  return 1;
}

int DDLT::Struct::getAuthor( lua_State* L )
{
  Struct* self = Check( L, 1 );

  lua_pushstring( L, self->m_Struct->GetAuthor() );
  return 1;
}

int DDLT::Struct::getDescription( lua_State* L )
{
  Struct* self = Check( L, 1 );

  lua_pushstring( L, self->m_Struct->GetDescription() );
  return 1;
}

int DDLT::Struct::getLabel( lua_State* L )
{
  Struct* self = Check( L, 1 );

  lua_pushstring( L, self->m_Struct->GetLabel() );
  return 1;
}

int DDLT::Struct::getDisplayLabel( lua_State* L )
{
  Struct* self = Check( L, 1 );

  lua_pushstring( L, self->m_Struct->GetDisplayLabel() );
  return 1;
}

int DDLT::Struct::getNameHash( lua_State* L )
{
  Struct* self = Check( L, 1 );

  lua_pushnumber( L, self->m_Struct->GetNameHash() );
  return 1;
}

int DDLT::Struct::getNumFields( lua_State* L )
{
  Struct* self = Check( L, 1 );

  lua_pushnumber( L, self->m_Struct->GetNumFields() );
  return 1;
}

int DDLT::Struct::getNumInheritedFields( lua_State* L )
{
  Struct* self = Check( L, 1 );

  int num_inherited_fields = 0;

  for ( uint32_t i = 0; i < self->m_Struct->GetNumFields(); i++ )
  {
    if ( self->m_Struct->IsInherited( ( *self->m_Struct )[ i ] ) )
    {
      num_inherited_fields++;
    }
  }

  lua_pushnumber( L, num_inherited_fields );
  return 1;
}

int DDLT::Struct::getNumOwnFields( lua_State* L )
{
  Struct* self = Check( L, 1 );

  int num_own_fields = 0;

  for ( uint32_t i = 0; i < self->m_Struct->GetNumFields(); i++ )
  {
    if ( !self->m_Struct->IsInherited( ( *self->m_Struct )[ i ] ) )
    {
      num_own_fields++;
    }
  }

  lua_pushnumber( L, num_own_fields );
  return 1;
}

int DDLT::Struct::getBase( lua_State* L )
{
  Struct* self = Check( L, 1 );

  DDLParser::Struct* parent = self->m_Struct->GetParent();

  if ( parent != NULL )
  {
    return Struct::PushNew( L, self->m_Definition, parent );
  }

  return 0;
}

int DDLT::Struct::getField( lua_State* L )
{
  Struct* self = Check( L, 1 );
  uint32_t index = (uint32_t)luaL_checkint( L, 2 ) - 1;

  if ( index < self->m_Struct->GetNumFields() )
  {
    return StructField::PushNew( L, self->m_Definition, self->m_Struct, ( *self->m_Struct )[ index ] );
  }

  return 0;
}

int DDLT::Struct::findField( lua_State* L )
{
  Struct* self = Check( L, 1 );
  const char* name = luaL_checkstring( L, 2 );

  DDLParser::StructField* field = self->m_Struct->FindField( name );

  if ( field != NULL )
  {
    return StructField::PushNew( L, self->m_Definition, self->m_Struct, field );
  }

  return 0;
}

int DDLT::Struct::getOwner( lua_State* L )
{
  Struct* self = Check( L, 1 );

  return Definition::PushNew( L, self->m_Definition );
}

int DDLT::Struct::fieldsIterator( lua_State* L )
{
  Struct* self = Check( L, 1 );
  uint32_t index = (uint32_t)luaL_checkint( L, 2 );

  if ( index >= self->m_Struct->GetNumFields() )
  {
    return 0;
  }

  lua_pushnumber( L, index + 1 );
  StructField::PushNew( L, self->m_Definition, self->m_Struct, ( *self->m_Struct )[ index ] );
  return 2;
}

int DDLT::Struct::fields( lua_State* L )
{
  Check( L, 1 );

  lua_pushcfunction( L, fieldsIterator );
  lua_pushvalue( L, 1 );
  lua_pushnumber( L, 0 );
  return 3;
}

int DDLT::Struct::inheritedFieldsIterator( lua_State* L )
{
  Struct* self = Check( L, 1 );
  uint32_t index = (uint32_t)luaL_checkint( L, 2 );

  while ( index < self->m_Struct->GetNumFields() )
  {
    DDLParser::StructField* field = ( *self->m_Struct )[ index ];

    if ( self->m_Struct->IsInherited( field ) )
    {
      break;
    }

    index++;
  }

  if ( index >= self->m_Struct->GetNumFields() )
  {
    return 0;
  }

  lua_pushnumber( L, index + 1 );
  StructField::PushNew( L, self->m_Definition, self->m_Struct, ( *self->m_Struct )[ index ] );
  return 2;
}

int DDLT::Struct::inheritedFields( lua_State* L )
{
  Check( L, 1 );

  lua_pushcfunction( L, inheritedFieldsIterator );
  lua_pushvalue( L, 1 );
  lua_pushnumber( L, 0 );
  return 3;
}

int DDLT::Struct::ownFieldsIterator( lua_State* L )
{
  Struct* self = Check( L, 1 );
  uint32_t index = (uint32_t)luaL_checkint( L, 2 );

  while ( index < self->m_Struct->GetNumFields() )
  {
    DDLParser::StructField* field = ( *self->m_Struct )[ index ];

    if ( !self->m_Struct->IsInherited( field ) )
    {
      break;
    }

    index++;
  }

  if ( index >= self->m_Struct->GetNumFields() )
  {
    return 0;
  }

  lua_pushnumber( L, index + 1 );
  StructField::PushNew( L, self->m_Definition, self->m_Struct, ( *self->m_Struct )[ index ] );
  return 2;
}

int DDLT::Struct::ownFields( lua_State* L )
{
  Check( L, 1 );

  lua_pushcfunction( L, ownFieldsIterator );
  lua_pushvalue( L, 1 );
  lua_pushnumber( L, 0 );
  return 3;
}

int DDLT::Struct::l__index( lua_State* L )
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
  case 0xf28b9251U: // getNumFields
    lua_pushcfunction( L, getNumFields );
    return 1;
  case 0xc5fedf55U: // getNumInheritedFields
    lua_pushcfunction( L, getNumInheritedFields );
    return 1;
  case 0x3173613eU: // getNumOwnFields
    lua_pushcfunction( L, getNumOwnFields );
    return 1;
  case 0x0883a114U: // getBase
    lua_pushcfunction( L, getBase );
    return 1;
  case 0xa9501839U: // getField
    lua_pushcfunction( L, getField );
    return 1;
  case 0x1222ad0aU: // findField
    lua_pushcfunction( L, findField );
    return 1;
  case 0x3dc5bb1dU: // getOwner
    lua_pushcfunction( L, getOwner );
    return 1;
  case 0x4731a8d9U: // fields
    lua_pushcfunction( L, fields );
    return 1;
  case 0xc27ca09fU: // inheritedFields
    lua_pushcfunction( L, inheritedFields );
    return 1;
  case 0xd048af58U: // ownFields
    lua_pushcfunction( L, ownFields );
    return 1;
  default:
    if ( lua_isnumber( L, 2 ) )
    {
      return getField( L );
    }
    else
    {
      return findField( L );
    }
    break;
  }

  return 0;
}

int DDLT::Struct::l__gc( lua_State* L )
{
  Struct* self = Check( L, 1 );

  self->Destroy();
  return 0;
}

int DDLT::Struct::l__eq( lua_State* L )
{
  Struct* p1 = Check( L, 1 );
  Struct* p2 = Check( L, 2 );

  lua_pushboolean( L, p1->m_Struct == p2->m_Struct );
  return 1;
}

int DDLT::Struct::PushNew( lua_State* L, DDLParser::Definition* definition, DDLParser::Struct* structure )
{
  Struct* self = new ( lua_newuserdata( L, sizeof( Struct ) ) ) Struct;

  if ( !self->Init( definition, structure ) )
  {
    return luaL_error( L, "Error creating DDLT::Struct" );
  }

  if ( luaL_newmetatable( L, "DDLT::Struct" ) )
  {
    lua_pushcfunction( L, getName );
    lua_setfield( L, -2, "__tostring" );
    lua_pushcfunction( L, getNumFields );
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

DDLT::Struct* DDLT::Struct::Check( lua_State* L, int index )
{
  return (Struct*)luaL_checkudata( L, index, "DDLT::Struct" );
}
