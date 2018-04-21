#include "api.h"
#include "../../log.h"

int invokeAPI(Lua::State state) {
		logC("invokeAPI()");
		//TODO: fix this SHIT!
		Lua lua = Lua(luaLibs["5.3"], state);
		void* func = lua.toUserdata(Lua::RegistryIndex - 1);
		void* ud;
		lua.getAllocF(&ud);
		int results = ((int (*)(Lua, Machine*)) func)(lua, (Machine*) ud);
		return results;
}

void loadAPIFunction(Lua lua, std::string name, APIFunction func) {
		logC("loadAPIFunction()");
		lua.pushString(name);
		lua.pushLightUserdata((void*)func);
		lua.pushCClosure(invokeAPI, 1);
		lua.setTable(-3);
}