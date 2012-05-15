#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <DDLParser.h>

#include "Util.h"
#include "CompilerIf.h"

#include "ddlc.h"

extern "C"
{
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
};

struct Args
{
  int argc;
  const char** argv;
};

// Copied from lua-5.1.4/src/ldblib.c
static lua_State *getthread (lua_State *L, int *arg) {
  if (lua_isthread(L, 1)) {
    *arg = 1;
    return lua_tothread(L, 1);
  }
  else {
    *arg = 0;
    return L;
  }
}

#define LEVELS1 12  /* size of the first part of the stack */
#define LEVELS2 10  /* size of the second part of the stack */

static int db_errorfb (lua_State *L) {
  int level;
  int firstpart = 1;  /* still before eventual `...' */
  int arg;
  lua_State *L1 = getthread(L, &arg);
  lua_Debug ar;
  if (lua_isnumber(L, arg+2)) {
    level = (int)lua_tointeger(L, arg+2);
    lua_pop(L, 1);
  }
  else
    level = (L == L1) ? 1 : 0;  /* level 0 may be this own function */
  if (lua_gettop(L) == arg)
    lua_pushliteral(L, "");
  else if (!lua_isstring(L, arg+1)) return 1;  /* message is not a string */
  else lua_pushliteral(L, "\n");
  lua_pushliteral(L, "stack traceback:");
  while (lua_getstack(L1, level++, &ar)) {
    if (level > LEVELS1 && firstpart) {
      /* no more than `LEVELS2' more levels? */
      if (!lua_getstack(L1, level+LEVELS2, &ar))
        level--;  /* keep going */
      else {
        lua_pushliteral(L, "\n\t...");  /* too many levels */
        while (lua_getstack(L1, level+LEVELS2, &ar))  /* find last levels */
          level++;
      }
      firstpart = 0;
      continue;
    }
    lua_pushliteral(L, "\n\t");
    lua_getinfo(L1, "Snl", &ar);
    lua_pushfstring(L, "%s:", ar.short_src);
    if (ar.currentline > 0)
      lua_pushfstring(L, "%d:", ar.currentline);
    if (*ar.namewhat != '\0')  /* is there a name? */
        lua_pushfstring(L, " in function " LUA_QS, ar.name);
    else {
      if (*ar.what == 'm')  /* main? */
        lua_pushfstring(L, " in main chunk");
      else if (*ar.what == 'C' || *ar.what == 't')
        lua_pushliteral(L, " ?");  /* C function or tail call */
      else
        lua_pushfstring(L, " in function <%s:%d>",
                           ar.short_src, ar.linedefined);
    }
    lua_concat(L, lua_gettop(L) - arg);
  }
  lua_concat(L, lua_gettop(L) - arg);
  return 1;
}

static int LuaMain( lua_State* L )
{
  // Get the command line arguments
  Args* args = (Args*)lua_touserdata( L, lua_upvalueindex( 1 ) );

  // Open the standard libraries and register ddlt functions
  luaL_openlibs( L );
  RegisterFunctions( L );

  // Push the main function onto the stack
  if ( luaL_loadbuffer( L, (const char*)ddlc, _ddlc_size, "ddlc.lua" ) != 0 )
  {
    return lua_error( L );
  }

  lua_call( L, 0, 1 );

  // Push the arguments array
  lua_newtable( L );
  for ( int i = 1; i < args->argc; i++ )
  {
    lua_pushstring( L, args->argv[ i ] );
    lua_rawseti( L, -2, i );
  }

  // Push the DDL compiler
  DDLT::Compiler::PushNew( L, args->argv[ 0 ], false, 32 );

  // Call main
  lua_call( L, 2, 0 );

  return 0;
}

int main( int argc, const char* argv[] )
{
  lua_State* L = luaL_newstate();

  // Push the Traceback function
  lua_pushcfunction( L, db_errorfb );

  // Push the LuaMain function with the arguments as an upvalue
  Args args;
  args.argc = argc;
  args.argv = argv;
  lua_pushlightuserdata( L, &args );
  lua_pushcclosure( L, LuaMain, 1 );

  // Run
  if ( lua_pcall( L, 0, 0, -2 ) != 0 )
  {
    fprintf( stderr, "%s\n", lua_tostring( L, -1 ) );
    lua_close( L );
    return 1;
  }

  lua_close( L );
  return 0;
}
