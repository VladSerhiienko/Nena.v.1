#if 0

#include <string>
#include <iostream>

extern "C" 
{
	#include <Nena\Lua\lua.h>
	#include <Nena\Lua\lstate.h>
	#include <Nena\Lua\lualib.h>
	#include <Nena\Lua\lauxlib.h>
}

int my_function(lua_State *L)
{
	int argc = lua_gettop(L);

	std::cerr << "-- my_function() called with " << argc
		<< " arguments:" << std::endl;

	for (int n = 1; n <= argc; ++n) 
	{
		std::cerr << "-- argument " << n << ": "
			<< lua_tostring(L, n) << std::endl;
	}

	lua_pushnumber(L, 123); // return value
	return 1; // number of return values
}

void report_errors(lua_State *L, int status)
{
	if (status != 0) 
	{
		std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1); // remove error message
	}
}

int main(int argc, char** argv)
{
	const char* file = "script2.lua";

	lua_State *L = luaL_newstate();

	luaL_openlibs(L);

	// make my_function() available to Lua programs
	//lua_register(L, "my_function", my_function);
	//std::cout << "Enter code:\n";

	//std::cerr << "-- Loading file: " << file << std::endl;
	//luaL_dostring(L, "b = my_function(1, 2, 3, 4, 5); io.write(\"b = \", b, \"\\n\");");
	int s = 0; // luaL_loadfile(L, file);

	//if (s == 0)
	//{
	//	// execute Lua program
	//	s = lua_pcall(L, 0, LUA_MULTRET, 0);
	//}

	while (!s)
	{
		std::string line;
		std::getline(std::cin, line);  // read a line from std::cin into line

		s = luaL_loadstring(L, line.c_str());

		if (s == LUA_ERRSYNTAX) 
		{
			std::cout << "Syntex error\n"; 
			s = 0; 
			continue;
		}

		s = lua_pcall(L, 0, LUA_MULTRET, 0);
	}

	report_errors(L, s);
	lua_close(L);
	std::cerr << std::endl;

	getchar();
	return 0;
}

#endif