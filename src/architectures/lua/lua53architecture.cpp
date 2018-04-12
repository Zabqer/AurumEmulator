#include "lua53architecture.h"
#include "lua_wrapper.h"
#include "allocator.h"

Lua53Architecture::Lua53Architecture() {}

bool Lua53Architecture::initialize(Machine* machine) {
		//FIX FIX FIX: dlclose on non use!
		lua = (new LuaWrapper("5.3"))->newState(allocator, machine);
		lua.loadBufferX("print('yeah!')"/*MACHINE_LUA*/, "=machine.lua", "t");
		return true;
}

