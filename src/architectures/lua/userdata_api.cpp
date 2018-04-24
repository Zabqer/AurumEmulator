#include "userdata_api.h"
#include "api.h"
#include "../../log.h"

API(userdata_methods) {
		logC("UserdataAPI::methods()");
		lua.createTable(0, 0);
		return 1;
}

API(userdata_dispose) {
		logC("UserdataAPI::dispose()");
		
		return 0;
}

void loadUserdataAPI(Lua lua) {
		lua.createTable(0, 0);
		loadAPIFunction(lua, "methods", userdata_methods);
		loadAPIFunction(lua, "dispose", userdata_dispose);
		lua.setGlobal("userdata");
};