#ifndef __API_H__
#define __API_H__

#include <string>

#include "lua_lib.h"

class Machine;

typedef int (* APIFunction) (Lua, Machine*);

#define API(name) static int name(Lua lua, Machine* machine)

void loadAPIFunction(Lua, std::string, APIFunction);

/*lua_pushstring(lua, #name); 
lua_pushlightuserdata(lua, name); 
lua_pushcclosure(lua, invokeAPI, 1); 
lua_settable(lua, -3);*/

#endif