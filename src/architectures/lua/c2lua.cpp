#include "c2lua.h"
#include "../../log.h"

void c2lua_value(Lua lua, std::any& value) {
		if (value.type() == typeid(Null)) {
				logD("c2lua: Null -> nil");
				lua.pushNil();
		} else if (value.type() == typeid(bool)) {
				logD("c2lua: bool -> boolean");
				lua.pushBoolean(std::any_cast<bool>(value));
		} else if (value.type() == typeid(std::string)) {
				logD("c2lua: std::string -> string (" + (std::any_cast<std::string>(value).length() < 100 ? std::any_cast<std::string>(value) : std::any_cast<std::string>(value).substr(0, 97) + "...") +")");
				lua.pushString(std::any_cast<std::string>(value));
		} else if (value.type() == typeid(const char*)) {
				logD("c2lua: const char* -> string");
				lua.pushString(std::any_cast<const char*>(value));
		} else if (value.type() == typeid(int)) {
				logD("c2lua: int -> number");
				lua.pushNumber(std::any_cast<int>(value));
		} else if (value.type() == typeid(unsigned int)) {
				logD("c2lua: unsigned int -> number");
				lua.pushNumber(std::any_cast<unsigned int>(value));
		} else if (value.type() == typeid(double)) {
				logD("c2lua: double -> number");
				lua.pushNumber(std::any_cast<double>(value));
		} else if (value.type() == typeid(size_t)) {
				logD("c2lua: size_t -> number");
				lua.pushNumber(std::any_cast<size_t>(value));
		} else if (value.type() == typeid(std::vector<std::any>)) {
				logD("c2lua: std::vector<std::any> -> table");
				lua.createTable(0, 0);
				int i = 0;
				for (std::any& v :std::any_cast<std::vector<std::any>>(value)) {
						c2lua_value(lua, v);
						lua.rawSetI(-2, i++);
				}
		} else {
				logD("c2lua: std::any -> userdata");
				void* ud = lua.newUserdata(sizeof(std::any));
				new (ud) std::any;
				*((std::any*) ud) = value;
		}
}

void c2lua(Lua lua, std::vector<std::any> args) {
		logC("c2lua()");
		for (std::any& value :args) {
				c2lua_value(lua, value);
		}
}