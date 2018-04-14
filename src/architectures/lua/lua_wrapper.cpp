#include "lua_wrapper.h"

#include "dlfcn.h"

#include "../../log.h"

#define wrap(name, ret, ...) ((ret (*)(...)) name)
#define getL(name) lib = dlopen(name, RTLD_LAZY)
#define getF(name) name = dlsym(lib, #name)

LuaWrapper::LuaWrapper(const std::string version) {
		logC("LuaWrapper::LuaWrapper()");
		getL(("liblua" + version + ".so").c_str());
		getF(lua_gc);
		getF(lua_gettop);
		getF(lua_newstate);
		getF(luaL_openlibs);
		getF(luaL_loadbufferx);
		getF(lua_pushinteger);
		getF(lua_resume);
		getF(lua_status);
		getF(lua_toboolean);
		getF(lua_tolstring);
		getF(lua_type);
}

LuaWrapper::~LuaWrapper() {
		logC("LuaWrapper::~LuaWrapper()");
		if (lib) {
				dlclose(lib);
		}
}

int LuaWrapper::gc(Lua::State state, int what, int data) {
		logC("LuaWrapper::gc()");
		return wrap(lua_gc, int, Lua::State, int, int)(state, what, data);
}

int LuaWrapper::getTop(Lua::State state) {
		logC("LuaWrapper::getTop()");
		return wrap(lua_gettop, int, Lua::State)(state);
}

bool LuaWrapper::isBoolean(Lua::State state, int index) {
		logC("LuaWrapper::isBoolean()");
		return type(state, index) == Lua::TBoolean;
}

bool LuaWrapper::isFunction(Lua::State state, int index) {
		logC("LuaWrapper::isFunction()");
		return type(state, index) == Lua::TFunction;
}

bool LuaWrapper::isNoneOrNil(Lua::State state, int index) {
		logC("LuaWrapper::isNoneOrNil()");
		return type(state, index) <= 0;
}

bool LuaWrapper::isNumber(Lua::State state, int index) {
		logC("LuaWrapper::isNumber()");
		return type(state, index) == Lua::TNumber;
}

bool LuaWrapper::isString(Lua::State state, int index) {
		logC("LuaWrapper::isString()");
		return type(state, index) == Lua::TString;
}

bool LuaWrapper::isTable(Lua::State state, int index) {
		logC("LuaWrapper::isTable()");
		return type(state, index) == Lua::TTable;
}

Lua LuaWrapper::newState(Lua::Allocator allocator, void* ud) {
		logC("LuaWrapper::newState()");
		return Lua(this, wrap(lua_newstate, Lua::State, Lua::Allocator, void*)(allocator, ud));
}

void LuaWrapper::openLibs(Lua::State state) {
		logC("LuaWrapper::openLibs()");
		wrap(luaL_openlibs, void, Lua::State)(state);
}

int LuaWrapper::loadBufferX(Lua::State state, std::string data, const std::string name, const std::string mode) {
		logC("LuaWrapper::loadBufferX()");
		return wrap(luaL_loadbufferx, int, Lua::State, const char*, size_t, const char*, const char*)(state, data.c_str(), data.length(), name.c_str(), mode.c_str());
}

void LuaWrapper::pop(Lua::State state, int count) {
		logC("LuaWrapper::pop()");
		wrap(lua_pop, void, Lua::State, int)(state, count);
}

void LuaWrapper::pushInteger(Lua::State state, Lua::Integer value) {
		logC("LuaWrapper::pushInteger()");
		wrap(lua_pushinteger, void, Lua::State, Lua::Integer)(state, value);
}

int LuaWrapper::resume(Lua::State state, int argc) {
		logC("LuaWrapper::resume()");
		return wrap(lua_resume, int, Lua::State, Lua::State, int)(state, NULL, argc);
}

int LuaWrapper::status(Lua::State state) {
		logC("LuaWrapper::status()");
		return wrap(lua_status, int, Lua::State)(state);
}

bool LuaWrapper::toBoolean(Lua::State state, int index) {
		logC("LuaWrapper::toBoolean()");
		return wrap(lua_toboolean, bool, Lua::State, int)(state, index);
}

Lua::Number LuaWrapper::toNumber(Lua::State state, int index) {
		logC("LuaWrapper::toNumber()");
		return wrap(lua_tonumber, bool, Lua::State, int)(state, index);
}

std::string LuaWrapper::toString(Lua::State state, int index) {
		logC("LuaWrapper::toString()");
		return wrap(lua_tolstring, const char*, Lua::State, int, size_t*)(state, index, NULL);
}

int LuaWrapper::type(Lua::State state, int index) {
		logC("LuaWrapper::type()");
		return wrap(lua_type, int, Lua::State, int)(state, index);
}

Lua::Lua() {}

Lua::Lua(LuaWrapper* luaWrapper_, State state_): state(state_), luaWrapper(luaWrapper_) {}

int Lua::gc(int what, int data) {
		return luaWrapper->gc(state, what, data);
}

int Lua::getTop() {
		return luaWrapper->getTop(state);
}

bool Lua::isBoolean(int index) {
		return luaWrapper->isBoolean(state, index);
}

bool Lua::isFunction(int index) {
		return luaWrapper->isFunction(state, index);
}

bool Lua::isNoneOrNil(int index) {
		return luaWrapper->isNoneOrNil(state, index);
}

bool Lua::isNumber(int index) {
		return luaWrapper->isNumber(state, index);
}

bool Lua::isString(int index) {
		return luaWrapper->isString(state, index);
}

bool Lua::isTable(int index) {
		return luaWrapper->isTable(state, index);
}

void Lua::openLibs() {
		luaWrapper->openLibs(state);
}

int Lua::loadBufferX(const std::string data, const std::string name, const std::string mode) {
		return luaWrapper->loadBufferX(state, data, name, mode);
}

void Lua::pop(int count) {
		luaWrapper->pop(state, count);
}

void Lua::pushInteger(Integer value) {
		luaWrapper->pushInteger(state, value);
}

int Lua::resume(int argc) {
		return luaWrapper->resume(state, argc);
}

int Lua::status() {
		return luaWrapper->status(state);
}

bool Lua::toBoolean(int index) {
		return luaWrapper->toBoolean(state, index);
}

Lua::Number Lua::toNumber(int index) {
		return luaWrapper->toNumber(state, index);
}

std::string Lua::toString(int index) {
		return luaWrapper->toString(state, index);
}

int Lua::type(int index) {
		return luaWrapper->type(state, index);
}