#include "userdata_api.h"
#include "api.h"
#include "../../log.h"

API(userdata_) {
		logC("UserdataAPI::()");
		
		return 0;
}

/*API(userdata_) {
		logC("UserdataAPI::()");
		
		return 0;
}*/

void loadUserdataAPI(Lua lua) {
		lua.createTable(0, 0);
		loadAPIFunction(lua, "a", userdata_);
		lua.setGlobal("userdata");
};