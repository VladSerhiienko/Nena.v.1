#include "LuaScripting.h"

Nena::Scripting::Lua::HLuaState *Nena::Scripting::Lua::Initialize(LuaHResult *status)
{
	lua_State *lua = luaL_newstate(); if (!lua)
	{
		if (status) (*status) = LUA_HRESULT_ERROR_MEMORY;
		return lua;
	} luaL_openlibs(lua);
	return lua;
}

void Nena::Scripting::Lua::Release(HLuaState *lua)
{
	lua_close(lua);
}