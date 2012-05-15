#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <astyle.h>

#ifdef __WIN__
#include <direct.h>
#else
#include <unistd.h>
#endif


#include <DDLParser.h>

#include "Util.h"

extern "C"
{
  #include "lua.h"
  #include "lauxlib.h"
};

#ifndef PATH_MAX
#define PATH_MAX 512
#endif

static int EscapeCPP( lua_State* L )
{
  const char* str = luaL_checkstring( L, 1 );

  size_t size   = strlen( str ) * 2 + 1;
  char*  buffer = (char*)alloca( size );
  size_t count  = 0;

  // Always leave room for NULL terminator
  while ( *str != 0 ) 
  {
    switch ( *str )
    {
    case '\n':
      buffer[ count++ ] = '\\';
      buffer[ count++ ] = 'n';
      break;

    case '\t':
      buffer[ count++ ] = '\\';
      buffer[ count++ ] = 't';
      break;

    case '"':
      buffer[ count++ ] = '\\';
      buffer[ count++ ] = '"';
      break;

    case '\\':
      buffer[ count++ ] = '\\';
      buffer[ count++ ] = '\\';
      break;

    default:
      buffer[ count++ ] = *str;
      break;
    }

    str++;
  }
  
  buffer[ count ] = 0;

  lua_pushstring( L, buffer );

  return 1;
}

static int Crc32( lua_State* L )
{
  const char* str = luaL_checkstring( L, 1 );

  uint32_t hash = DDLParser::StringCrc32( str );
  char hex[ 9 ];
  snprintf( hex, sizeof( hex ), "%08x", hash );
  lua_pushstring( L, hex );

  return 1;
}

// AStyle API sucks so we have to define this global
static const char* s_AStyleError;

#ifdef __WIN__
#define ASTYLECALL __stdcall
#else
#define ASTYLECALL
#endif

static void ASTYLECALL ASErrorHandler( int errorNumber, char* errorMessage )
{
  s_AStyleError = errorMessage;
}

static char* ASTYLECALL ASMemoryAlloc( unsigned long memoryNeeded )
{
  return new( nothrow ) char[ memoryNeeded ];
}

static int FormatCode( lua_State* L )
{
  const char* code = luaL_checkstring( L, 1 );
  const char* options = luaL_optstring( L, 1,
    "--style=ansi "
    "--indent=spaces=2 "
    "--indent-classes "
    "--indent-switches "
    "--indent-namespaces "
    "--indent-col1-comments "
    "--max-instatement-indent=80 "
    "--break-blocks "
    "--pad-oper "
    "--pad-header "
    "--unpad-paren "
    "--pad-paren-in "
    "--pad-paren-out "
    "--delete-empty-lines "
    "--add-brackets "
    "--convert-tabs "
    "--align-pointer=type"
  );

  char* formatted = AStyleMain( code, options, (fpError)ASErrorHandler, (fpAlloc)ASMemoryAlloc );

  if ( formatted != NULL )
  {
    lua_pushstring( L, formatted );
    lua_pushnil( L );
  }
  else
  {
    lua_pushnil( L );
    lua_pushstring( L, s_AStyleError );
  }

  delete[] formatted;
  return 2;
}

static int CompileTemplate( lua_State* L )
{
  size_t      template_length;
  const char* template_contents = luaL_checklstring( L, 1, &template_length );
  
  const char* template_begin = template_contents;
  const char* template_end   = template_begin + template_length;
  
  luaL_Buffer template_code;
  luaL_buffinit( L, &template_code );
  
  for ( ;; )
  {
    const char* start = strstr( template_begin, "/*" );
    
    if ( start == NULL )
    {
      luaL_addlstring( &template_code, "emit[===[", 9 );
      luaL_addlstring( &template_code, template_begin, template_end - template_begin );
      luaL_addlstring( &template_code, "]===]", 5 );
      break;
    }
    
    const char* finish = strstr( start + 2, "*/" );
    
    if ( finish == NULL )
    {
      finish = template_end;
    }
    
    if ( *template_begin == '\n' || *template_begin == '\r' )
    {
      luaL_addlstring( &template_code, "emit( \'\\n\' ) emit[===[", 22 );
    }
    else
    {
      luaL_addlstring( &template_code, "emit[===[", 9 );
    }
    
    luaL_addlstring( &template_code, template_begin, start - template_begin );
    luaL_addlstring( &template_code, "]===] ", 6 );
    
    char block_type = start[ 2 ];
    
    switch ( block_type )
    {
    case '=':
      luaL_addlstring( &template_code, "emit( tostring( ", 16 );
      luaL_addlstring( &template_code, start + 3, finish - 1 - ( start + 3 ) + 1 );
      luaL_addlstring( &template_code, " ) ) ", 5 );
      break;
      
    case '!':
      luaL_addlstring( &template_code, start + 3, finish - 1 - ( start + 3 ) + 1 );
      luaL_addchar( &template_code, ' ' );
      break;

    default:
      luaL_addlstring( &template_code, "emit[===[", 9 );
      luaL_addlstring( &template_code, start, finish + 2 - start );
      luaL_addlstring( &template_code, "]===] ", 6 );
      break;
    }
    
    template_begin = finish + 2;
  }
  
  luaL_pushresult( &template_code );
  return 1;
}

static int GetSearchPaths( lua_State* L )
{
  lua_newtable( L );
  int index = 1;
  
  // Get the current working directory
  char cwd[ PATH_MAX ];
  
  if ( getcwd( cwd, sizeof( cwd ) ) == NULL )
  {
    return luaL_error( L, "Error getting the current working directory: %s", strerror( errno ) );
  }

  lua_pushstring( L, cwd );
  lua_pushliteral( L, "/" );
  lua_concat( L, 2 );
  
  lua_rawseti( L, -2, index++ );

#if defined( DDLT_TEMPLATE_DIR )
  // Get the template folder (/usr/local/share/ddlt on MinGW and Linux)
  lua_pushstring( L, DDLT_TEMPLATE_DIR );
  lua_rawseti( L, -2, index++ );
#endif

  return 1;
}

void RegisterFunctions( lua_State* L )
{
  // Most path functions go into the string namespace
  lua_getglobal( L, "string" );

  lua_pushcfunction( L, EscapeCPP );
  lua_setfield( L, -2, "escapeCPP" );
  lua_pushcfunction( L, Crc32 );
  lua_setfield( L, -2, "crc32" );
  lua_pushcfunction( L, FormatCode );
  lua_setfield( L, -2, "formatCode" );

  lua_pop( L, 1 );

  lua_pushcfunction( L, CompileTemplate );
  lua_setglobal( L, "compileTemplate" );

  lua_pushcfunction( L, GetSearchPaths );
  lua_setglobal( L, "getSearchPaths" );
}

int CompareBoxedPointers( lua_State* L )
{
  void* ud1 = *(void**)lua_touserdata( L, 1 );
  void* ud2 = *(void**)lua_touserdata( L, 2 );

  lua_pushboolean( L, ud1 == ud2 );
  return 1;
}

void PushDDLType( lua_State* L, uint32_t type )
{
  switch ( type )
  {
  case DDLParser::kUint8:
    lua_pushliteral( L, "uint8" );
    break;
  case DDLParser::kUint16:
    lua_pushliteral( L, "uint16" );
    break;
  case DDLParser::kUint32:
    lua_pushliteral( L, "uint32" );
    break;
  case DDLParser::kUint64:
    lua_pushliteral( L, "uint64" );
    break;
  case DDLParser::kInt8:
    lua_pushliteral( L, "int8" );
    break;
  case DDLParser::kInt16:
    lua_pushliteral( L, "int16" );
    break;
  case DDLParser::kInt32:
    lua_pushliteral( L, "int32" );
    break;
  case DDLParser::kInt64:
    lua_pushliteral( L, "int64" );
    break;
  case DDLParser::kFloat32:
    lua_pushliteral( L, "float32" );
    break;
  case DDLParser::kFloat64:
    lua_pushliteral( L, "float64" );
    break;
  case DDLParser::kString:
    lua_pushliteral( L, "string" );
    break;
  case DDLParser::kSelect:
    lua_pushliteral( L, "select" );
    break;
  case DDLParser::kBitfield:
    lua_pushliteral( L, "bitfield" );
    break;
  case DDLParser::kStruct:
    lua_pushliteral( L, "struct" );
    break;
  case DDLParser::kBoolean:
    lua_pushliteral( L, "boolean" );
    break;
  case DDLParser::kFile:
    lua_pushliteral( L, "file" );
    break;
  case DDLParser::kTuid:
    lua_pushliteral( L, "tuid" );
    break;
  case DDLParser::kJson:
    lua_pushliteral( L, "json" );
    break;
  default:
    lua_pushnil( L );
    break;
  }
}

void PushDDLArrayType( lua_State* L, uint32_t type )
{
  switch ( type )
  {
  case DDLParser::kScalar:
    lua_pushliteral( L, "scalar" );
    break;
  case DDLParser::kFixed:
    lua_pushliteral( L, "fixed" );
    break;
  case DDLParser::kDynamic:
    lua_pushliteral( L, "dynamic" );
    break;
  }
}

void PutHex( char* str, uint32_t x, int digits )
{
  str += digits;

  while ( digits-- > 0 )
  {
    *--str = "0123456789abcdef"[ x & 15 ];
    x >>= 4;
  }
}
