#include "component_api.h"
#include "api.h"
#include "../../log.h"
#include "../../machine.h"

API(component_list) {
		logC("Component::list()");
		std::string address = lua.toString(1);
		bool exact = lua.toBoolean(2);
		lua.pop(2);
		lua.createTable(0, 0);
		for (Component* component :machine->components()) {
				if (!component->internal() && (exact ? component->address() == address : address.find(component->address()) == 0)) {
						lua.pushString(component->address());
						lua.pushString(component->type());
						lua.setTable(-3);
				}
		}
		return 1;
}

void loadComponentAPI(Lua lua) {
		logC("loadComponentAPI()");
		lua.createTable(0, 0);
		loadAPIFunction(lua, "list", component_list);
		lua.setGlobal("component");
}