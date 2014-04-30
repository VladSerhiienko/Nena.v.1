#include <Windows.h>
#include <string>
#include "vrc_script.h"

#ifndef __NENA_LUA_SCRIPTING_H__
#define __NENA_LUA_SCRIPTING_H__

namespace Nena
{
	namespace Scripting
	{
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

			static BOOL GetVarToStack(
				HLuaState *L, const char * varname
				)
			{
				char temp[64];
				memset(temp, 0, sizeof(temp));
				int i = 0;
				int j = 0;
				int n = 0;
				while (varname[i] != '\0')
				{
					char c = varname[i];
					if (c == '.')
					{
						if (n == 0) lua_getglobal(L, temp);
						else lua_getfield(L, -1, temp);

						++n;
						memset(temp, 0, sizeof(temp));
						j = 0;

						if (lua_isnil(L, -1))
							return FALSE;
					}
					else
					{
						temp[j] = c;
						++j;
					}
					++i;
				}

				if (n == 0) lua_getglobal(L, temp);
				else lua_getfield(L, -1, temp);

				if (lua_isnil(L, -1)) return FALSE;
				return TRUE;
			}

			template<typename T> static
			T GetVarFromStack(HLuaState *L)
			{
				return (T) lua_tonumber(L, -1);
			}
			template <> static bool GetVarFromStack<bool>(HLuaState *L)
			{
				return (bool) lua_toboolean(L, -1);
			}
			template <> static const char * GetVarFromStack<const char *>(HLuaState *L)
			{
				return lua_tostring(L, -1);
			}
			template <> static std::string GetVarFromStack<std::string>(HLuaState *L)
			{
				std::string s = lua_tostring(L, -1);
				return s;
			}
		};
	}
}

#endif // !__NENA_LUA_SCRIPTING_H__
