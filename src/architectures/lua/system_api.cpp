#include "system_api.h"
#include "api.h"
#include "../../log.h"

API(system_allowBytecode) {
		logC("SystemAPI::allowBytecode()");
		lua.pushBoolean(AurumConfig.allowBytecode);
		return 1;
}

API(system_allowGC) {
		logC("SystemAPI::allowGC()");
		lua.pushBoolean(AurumConfig.allowGC);
		return 1;
}

API(system_timeout) {
		logC("SystemAPI::allowBytecode()");
		lua.pushNumber(AurumConfig.timeout);
		return 1;
}

void loadSystemAPI(Lua lua) {
		lua.createTable(0, 0);
		loadAPIFunction(lua, "allowBytecode", system_allowBytecode);
		loadAPIFunction(lua, "allowGC", system_allowGC);
		loadAPIFunction(lua, "timeout", system_timeout);
		lua.setGlobal("system");
};