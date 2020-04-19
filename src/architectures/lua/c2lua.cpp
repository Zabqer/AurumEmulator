#include "c2lua.h"
#include "../../log.h"
#include "../../components/component.h"
#include "../../arguments.h"

void c2lua_value(Lua lua, const std::any& value) {
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
		} else if (value.type() == typeid(char16_t)) {
				logD("c2lua: char16_t -> number");
				lua.pushInteger(std::any_cast<char16_t>(value));
		} else if (value.type() == typeid(int)) {
				logD("c2lua: int -> number");
				lua.pushInteger(std::any_cast<int>(value));
		} else if (value.type() == typeid(unsigned int)) {
				logD("c2lua: unsigned int -> number");
				lua.pushInteger(std::any_cast<unsigned int>(value));
		} else if (value.type() == typeid(double)) {
				logD("c2lua: double -> number");
				lua.pushNumber(std::any_cast<double>(value));
		} else if (value.type() == typeid(size_t)) {
				logD("c2lua: size_t -> number");
				lua.pushInteger(std::any_cast<size_t>(value));
		} else if (value.type() == typeid(off_t)) {
				logD("c2lua: off_t -> number");
				lua.pushInteger(std::any_cast<off_t>(value));
		} else if (value.type() == typeid(std::vector<std::any>)) {
				logD("c2lua: std::vector<std::any> -> table");
				lua.createTable(0, 0);
				int i = 0;
				for (std::any& v :std::any_cast<std::vector<std::any>>(value)) {
						c2lua_value(lua, v);
						lua.rawSetI(-2, i++);
				}
		} else if (value.type() == typeid(std::map<std::any, std::any>)) {
				logD("c2lua: std::map<std::any, std::any> -> table");
				lua.createTable(0, 0);
				for (auto const& v :std::any_cast<std::map<std::any, std::any>>(value)) {
					c2lua_value(lua, v.first);
					c2lua_value(lua, v.second);
					lua.setTable(-3);
				}
		} else if (value.type() == typeid(Userdata*)){
				logD("c2lua: Userdata* -> userdata");
				void* ud = lua.newUserdata(sizeof(std::any));
				new (ud) std::any;
				*((std::any*) ud) = value;
		} else {
				logE("Internal error: unknown c2lua type: " << value.type().name());
				abort();
		}
}

void c2lua(Lua lua, std::vector<std::any> args) {
		logC("c2lua()");
		for (std::any& value :args) {
				c2lua_value(lua, value);
		}
}
