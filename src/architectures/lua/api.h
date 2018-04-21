#ifndef __API_H__
#define __API_H__

#include <string>

#include "lua_lib.h"

class Machine;

typedef int (* APIFunction) (Lua, Machine*);

#define API(name) int name(Lua lua, Machine* machine)

void loadAPIFunction(Lua, std::string, APIFunction);

#endif