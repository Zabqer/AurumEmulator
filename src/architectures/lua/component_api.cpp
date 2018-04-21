#include "component_api.h"
#include "api.h"
#include "../../log.h"
#include "../../machine.h"
#include "c2lua.h"
#include "lua2c.h"

API(component_list) {
		logC("ComponentAPI::list()");
		std::string type = lua.toString(1);
		bool exact = lua.toBoolean(2);
		lua.pop(2);
		lua.createTable(0, 0);
		for (Component* component :machine->components()) {
				if (!component->internal() && (exact ? component->type() == type : component->type().find(type) == 0)) {
						lua.pushString(component->address());
						lua.pushString(component->type());
						lua.rawSet(-3);
				}
		}
		return 1;
}

API(component_methods) {
		logC("ComponentAPI::methods()");
		std::string address = lua.toString(1);
		lua.pop(1);
		lua.createTable(0, 0);
		for (auto const [name, method] :machine->methods(address)) {
				lua.pushString(name);
				lua.createTable(0, 0);
				lua.pushBoolean(method.direct);
				lua.setField(-2, "direct");
				lua.pushBoolean(method.getter);
				lua.setField(-2, "getter");
				lua.pushBoolean(method.setter);
				lua.setField(-2, "setter");
				lua.rawSet(-3);
		}
		return 1;
}

API(component_invoke) {
		logC("ComponentAPI::invoke()");
		std::string address = lua.toString(1);
		std::string method = lua.toString(2);
		lua.remove(1);
		lua.remove(1);
		try {
				Arguments args = machine->invoke(address, method, lua2c(lua));
				lua.pushBoolean(true);
				c2lua(lua, args);
				return lua.getTop();
		} catch (limit_reached) {
				return 0;
		} catch (std::invalid_argument& ex) {
				lua.pushBoolean(false);
				lua.pushString(ex.what());
				return 2;
		} catch (std::exception& ex) {
				lua.pushBoolean(true);
				lua.pushNil();
				lua.pushString(ex.what());
				return 3;
		}
}

//UNCOMPLETED!

void loadComponentAPI(Lua lua) {
		logC("loadComponentAPI()");
		lua.createTable(0, 0);
		loadAPIFunction(lua, "list", component_list);
		loadAPIFunction(lua, "methods", component_methods);
		loadAPIFunction(lua, "invoke", component_invoke);
		lua.setGlobal("component");
}