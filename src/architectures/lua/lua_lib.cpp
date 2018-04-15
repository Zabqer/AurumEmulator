#include "lua_lib.h"

#include "dlfcn.h"

#include "../../log.h"

#define wrap(name, ret, ...) ((ret (*)(...)) name)
#define getL(name) lib = dlopen(name, RTLD_LAZY)
#define getF(name) name = dlsym(lib, #name)

LuaLib::LuaLib(const std::string version) {
		logC("LuaLib::LuaLib()");
		getL(("liblua" + version + ".so").c_str());
		getF(lua_createtable);
		getF(lua_gc);
		getF(lua_getallocf);
		getF(lua_gettop);
		getF(lua_newstate);
		getF(luaL_openlibs);
		getF(luaL_loadbufferx);
		getF(lua_pushcclosure);
		getF(lua_pushinteger);
		getF(lua_pushlightuserdata);
		getF(lua_pushstring);
		getF(lua_resume);
		getF(lua_setglobal);
		getF(lua_settable);
		getF(lua_settop);
		getF(lua_status);
		getF(lua_toboolean);
		getF(lua_tonumberx);
		getF(lua_tolstring);
		getF(lua_touserdata);
		getF(lua_type);
}

LuaLib::~LuaLib() {
		logC("LuaLib::~LuaLib()");
		if (lib) {
				dlclose(lib);
		}
}

void LuaLib::createTable(Lua::State state, int a, int b) {
		logC("LuaLib::createTable()");
		wrap(lua_createtable, void, Lua::State, int, int)(state, a, b);
}

int LuaLib::gc(Lua::State state, int what, int data) {
		logC("LuaLib::gc()");
		return wrap(lua_gc, int, Lua::State, int, int)(state, what, data);
}

Lua::Allocator LuaLib::getAllocF(Lua::State state, void** ud) {
		logC("LuaLib::getAllocF()");
		return wrap(lua_getallocf, Lua::Allocator, Lua::State, void**)(state, ud);
}

int LuaLib::getTop(Lua::State state) {
		logC("LuaLib::getTop()");
		return wrap(lua_gettop, int, Lua::State)(state);
}

bool LuaLib::isBoolean(Lua::State state, int index) {
		logC("LuaLib::isBoolean()");
		return type(state, index) == Lua::TBoolean;
}

bool LuaLib::isFunction(Lua::State state, int index) {
		logC("LuaLib::isFunction()");
		return type(state, index) == Lua::TFunction;
}

bool LuaLib::isNoneOrNil(Lua::State state, int index) {
		logC("LuaLib::isNoneOrNil()");
		return type(state, index) <= 0;
}

bool LuaLib::isNumber(Lua::State state, int index) {
		logC("LuaLib::isNumber()");
		return type(state, index) == Lua::TNumber;
}

bool LuaLib::isString(Lua::State state, int index) {
		logC("LuaLib::isString()");
		return type(state, index) == Lua::TString;
}

bool LuaLib::isTable(Lua::State state, int index) {
		logC("LuaLib::isTable()");
		return type(state, index) == Lua::TTable;
}

Lua LuaLib::newState(Lua::Allocator allocator, void* ud) {
		logC("LuaLib::newState()");
		return Lua(this, wrap(lua_newstate, Lua::State, Lua::Allocator, void*)(allocator, ud));
}

void LuaLib::openLibs(Lua::State state) {
		logC("LuaLib::openLibs()");
		wrap(luaL_openlibs, void, Lua::State)(state);
}

int LuaLib::loadBufferX(Lua::State state, std::string data, const std::string name, const std::string mode) {
		logC("LuaLib::loadBufferX()");
		return wrap(luaL_loadbufferx, int, Lua::State, const char*, size_t, const char*, const char*)(state, data.c_str(), data.length(), name.c_str(), mode.c_str());
}

void LuaLib::pop(Lua::State state, int count) {
		logC("LuaLib::pop()");
		setTop(state, -(count) - 1);
}

void LuaLib::pushCClosure(Lua::State state, Lua::Function func, int values) {
		logC("LuaLib::pushCClosure()");
		wrap(lua_pushcclosure, void, Lua::State, Lua::Function, int)(state, func, values);
}

void LuaLib::pushInteger(Lua::State state, Lua::Integer value) {
		logC("LuaLib::pushInteger()");
		wrap(lua_pushinteger, void, Lua::State, Lua::Integer)(state, value);
}

void LuaLib::pushLightUserdata(Lua::State state, void* ud) {
		logC("LuaLib::pushLightUserdata()");
		wrap(lua_pushlightuserdata, void, Lua::State, void*)(state, ud);
}

void LuaLib::pushString(Lua::State state, std::string value) {
		logC("LuaLib::pushString()");
		wrap(lua_pushstring, void, Lua::State, const char*)(state, value.c_str());
}

int LuaLib::resume(Lua::State state, int argc) {
		logC("LuaLib::resume()");
		return wrap(lua_resume, int, Lua::State, Lua::State, int)(state, NULL, argc);
}

void LuaLib::setGlobal(Lua::State state, std::string name) {
		logC("LuaLib::setGlobal()");
		wrap(lua_setglobal, void, Lua::State, const char*)(state, name.c_str());
}

void LuaLib::setTable(Lua::State state, int index) {
		logC("LuaLib::setTable()");
		wrap(lua_settable, void, Lua::State, int)(state, index);
}

void LuaLib::setTop(Lua::State state, int top) {
		logC("LuaLib::setTop()");
		wrap(lua_settop, void, Lua::State, int)(state, top);
}

int LuaLib::status(Lua::State state) {
		logC("LuaLib::status()");
		return wrap(lua_status, int, Lua::State)(state);
}

bool LuaLib::toBoolean(Lua::State state, int index) {
		logC("LuaLib::toBoolean()");
		return wrap(lua_toboolean, bool, Lua::State, int)(state, index);
}

Lua::Number LuaLib::toNumber(Lua::State state, int index) {
		logC("LuaLib::toNumber()");
		return wrap(lua_tonumberx, Lua::Number, Lua::State, int, int*)(state, index, NULL);
}

std::string LuaLib::toString(Lua::State state, int index) {
		logC("LuaLib::toString()");
		return wrap(lua_tolstring, const char*, Lua::State, int, size_t*)(state, index, NULL);
}

void* LuaLib::toUserdata(Lua::State state, int index) {
		logC("LuaLib::toUserdata()");
		return wrap(lua_touserdata, void*, Lua::State, int)(state, index);
}

int LuaLib::type(Lua::State state, int index) {
		logC("LuaLib::type()");
		return wrap(lua_type, int, Lua::State, int)(state, index);
}

Lua::Lua() {}

Lua::Lua(LuaLib* luaWrapper_, State state_): state(state_), luaWrapper(luaWrapper_) {}

void Lua::createTable(int a, int b) {
		luaWrapper->createTable(state, a, b);
}

int Lua::gc(int what, int data) {
		return luaWrapper->gc(state, what, data);
}

Lua::Allocator Lua::getAllocF(void** ud) {
		return luaWrapper->getAllocF(state, ud);
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

void Lua::pushCClosure(Function func, int values) {
		luaWrapper->pushCClosure(state, func, values);
}

void Lua::pushInteger(Integer value) {
		luaWrapper->pushInteger(state, value);
}

void Lua::pushLightUserdata(void* ud) {
		luaWrapper->pushLightUserdata(state, ud);
}

void Lua::pushString(std::string value) {
		luaWrapper->pushString(state, value);
}

int Lua::resume(int argc) {
		return luaWrapper->resume(state, argc);
}

void Lua::setGlobal(std::string name) {
		luaWrapper->setGlobal(state, name);
}

void Lua::setTable(int index) {
		luaWrapper->setTable(state, index);
}

void Lua::setTop(int top) {
		luaWrapper->setTop(state, top);
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

void* Lua::toUserdata(int index) {
		return luaWrapper->toUserdata(state, index);
}

int Lua::type(int index) {
		return luaWrapper->type(state, index);
}