#include "component_api.h"
#include "api.h"
#include "../../log.h"
#include "../../machine.h"
#include "c2lua.h"
#include "lua2c.h"

#include <optional>

API(component_list) {
		logC("ComponentAPI::list()");
		std::optional<std::string> type;
		if (lua.isString(1)) {
				type = lua.toString(1);
		}
		bool exact = lua.toBoolean(2);
		lua.pop(2);
		lua.createTable(0, 0);
		for (Component* component :machine->components()) {
				if (!component->internal() && (exact ? component->type() == *type : type ? component->type().find(*type) == 0 : true)) {
						lua.pushString(component->address());
						lua.pushString(component->type());
						lua.rawSet(-3);
				}
		}
		return 1;
}

API(component_type) {
		logC("ComputerAPI::type()");
		std::string address = lua.toString(1);
		try {
				lua.pushString(machine->componentByAddress(address)->type());
				return 1;
		} catch (std::invalid_argument& ex) {
				lua.pushBoolean(false);
				lua.pushString(ex.what());
				return 2;
		}
}

API(component_slot) {
		logC("ComputerAPI::slot()");
		std::string address = lua.toString(1);
		try {
				lua.pushNumber(machine->componentByAddress(address)->slot());
				return 1;
		} catch (std::invalid_argument& ex) {
				lua.pushBoolean(false);
				lua.pushString(ex.what());
				return 2;
		}
}

API(component_methods) {
		logC("ComponentAPI::methods()");
		std::string address = lua.toString(1);
		lua.pop(1);
		try {
				std::map<std::string, Method> methods = machine->methods(address);
				lua.createTable(0, 0);
				for (auto const [name, method] :methods) {
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
		} catch (std::invalid_argument& ex) {
				lua.pushBoolean(false);
				lua.pushString(ex.what());
				return 2;
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
		loadAPIFunction(lua, "type", component_type);
		loadAPIFunction(lua, "slot", component_slot);
		loadAPIFunction(lua, "methods", component_methods);
		loadAPIFunction(lua, "invoke", component_invoke);
		lua.setGlobal("component");
}