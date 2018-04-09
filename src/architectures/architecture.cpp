#include "architecture.h"
#include "lua/lua53architecture.h"

std::map<std::string, ArchitectureHolder*> Architecture::byName = {
		{"Lua 5.3", new ArchitectureHolderImpl<Lua53Architecture>}
};