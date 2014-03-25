#include <Windows.h>
#include <Nena\Lua\lua.h>
#include <Nena\Lua\lstate.h>
#include <Nena\Lua\lualib.h>
#include <Nena\Lua\lauxlib.h>
#include <string>

#ifndef __NENA_LUA_SCRIPTING_H__
#define __NENA_LUA_SCRIPTING_H__

namespace Nena
{
	namespace Scripting
	{
		// DO:
		// http://rubenlaguna.com/wp/2012/11/26/first-steps-lua-cplusplus-integration/
		// http://rubenlaguna.com/wp/2012/12/09/accessing-cpp-objects-from-lua/
		struct Lua
		{
			typedef lua_State HLuaState; // Native API

			enum LuaHResult // LUA_XXX
			{
				LUA_HRESULT_OK = LUA_OK, // everything is ok
				LUA_HRESULT_YIELD = LUA_YIELD, // thread yield
				LUA_HRESULT_ERROR_INVALID = LUA_ERRERR, // invalid result
				LUA_HRESULT_ERROR_SYNTAX = LUA_ERRSYNTAX, // syntax error
				LUA_HRESULT_ERROR_MEMORY = LUA_ERRMEM, // cannot alloc memory
				LUA_HRESULT_ERROR_RUN = LUA_ERRRUN, // error running a code
				LUA_HRESULT_ERROR_GCMETAMETHOD = LUA_ERRGCMM, // is a new runtime error code to signify error in __gc metamethod
			};

			template <LuaHResult _Result = LUA_HRESULT_OK> static ::HRESULT ToHResult() { return S_OK; }
			template <> static ::HRESULT ToHResult<LUA_HRESULT_ERROR_GCMETAMETHOD>() { return E_FAIL; }
			template <> static ::HRESULT ToHResult<LUA_HRESULT_ERROR_MEMORY>() { return E_OUTOFMEMORY; }
			template <> static ::HRESULT ToHResult<LUA_HRESULT_ERROR_SYNTAX>() { return E_INVALIDARG; }
			template <> static ::HRESULT ToHResult<LUA_HRESULT_ERROR_INVALID>() { return E_FAIL; }
			template <> static ::HRESULT ToHResult<LUA_HRESULT_ERROR_RUN>() { return E_ABORT; }

			static HLuaState *Initialize(LuaHResult *status);
			static void Release(HLuaState *lua);
		};
	}
}

#endif // !__NENA_LUA_SCRIPTING_H__
