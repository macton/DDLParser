#pragma once

#include <stdint.h>

struct lua_State;

void RegisterFunctions( lua_State* L );

int  CompareBoxedPointers( lua_State* L );
void PushDDLType( lua_State* L, uint32_t type );
void PushDDLArrayType( lua_State* L, uint32_t type );
void PutHex( char* str, uint32_t x, int digits );