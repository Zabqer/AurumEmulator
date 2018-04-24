#include "lua_lib.h"

#include "dlfcn.h"

#include "../../log.h"

std::map<std::string, LuaLib*> luaLibs = {{"5.3", new LuaLib("5.3")}};

#define wrap(name, ret, ...) ((ret (*)(...)) name)
#define getL(name) lib = dlopen(name, RTLD_LAZY)
#define getF(name) name = dlsym(lib, #name)

LuaLib::LuaLib(const std::string version) {
		logC("LuaLib::LuaLib()");
		getL(("liblua" + version + ".so").c_str());
		getF(lua_callk);
		getF(luaL_checktype);
		getF(luaL_checkinteger);
		getF(luaL_checklstring);
		getF(lua_createtable);
		getF(lua_gc);
		getF(lua_getallocf);
		getF(lua_gettop);
		getF(lua_newstate);
		getF(lua_newuserdata);
		getF(luaL_openlibs);
		getF(luaL_loadbufferx);
		getF(lua_pushboolean);
		getF(lua_pushcclosure);
		getF(lua_pushinteger);
		getF(lua_pushlightuserdata);
		getF(lua_pushnil);
		getF(lua_pushnumber);
		getF(lua_pushstring);
		getF(lua_rawset);
		getF(lua_rawseti);
		getF(lua_resume);
		getF(lua_rotate);
		getF(lua_setfield);
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

void LuaLib::call(Lua::State state, int argc, int resc) {
		logC("LuaLib::call()");
		wrap(lua_callk, void, Lua::State, int, int, Lua::KContext, Lua::KFunction)(state, argc, resc, 0, NULL);
}

void LuaLib::checkType(Lua::State state, int index, int type) {
		logC("LuaLib::checkType()");
		wrap(luaL_checktype, void, Lua::State, int, int)(state, index, type);
}

Lua::Integer LuaLib::checkInteger(Lua::State state, int index) {
		logC("LuaLib::checkInteger()");
		return wrap(luaL_checkinteger, Lua::Integer, Lua::State, int)(state, index);
}

std::string LuaLib::checkString(Lua::State state, int index) {
		logC("LuaLib::checkString()");
		return wrap(luaL_checklstring, const char*, Lua::State, int, size_t*)(state, index, NULL);
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

void* LuaLib::newUserdata(Lua::State state, size_t size) {
		logC("LuaLib::newUserdata()");
		return wrap(lua_newuserdata, void*, Lua::State, size_t)(state, size);
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

void LuaLib::pushBoolean(Lua::State state, bool value) {
		logC("LuaLib::pushBoolean()");
		wrap(lua_pushboolean, void, Lua::State, bool)(state, value);
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

void LuaLib::pushNil(Lua::State state) {
		logC("LuaLib::pushNil()");
		wrap(lua_pushnil, void, Lua::State)(state);
}

void LuaLib::pushNumber(Lua::State state, Lua::Number value) {
		logC("LuaLib::pushNumber()");
		wrap(lua_pushnumber, void, Lua::State, Lua::Number)(state, value);
}

void LuaLib::pushString(Lua::State state, std::string value) {
		logC("LuaLib::pushString()");
		wrap(lua_pushstring, void, Lua::State, const char*)(state, value.c_str());
}

void LuaLib::rawSet(Lua::State state, int index) {
		logC("LuaLib::rawSet()");
		wrap(lua_rawset, void, Lua::State, int)(state, index);
}

void LuaLib::rawSetI(Lua::State state, int index, Lua::Integer i) {
		logC("LuaLib::rawSetI()");
		wrap(lua_rawseti, void, Lua::State, int, Lua::Integer)(state, index, i);
}

void LuaLib::remove(Lua::State state, int index) {
		logC("LuaLib::remove()");
		rotate(state, index, -1);
		pop(state, 1);
}

int LuaLib::resume(Lua::State state, int argc) {
		logC("LuaLib::resume()");
		return wrap(lua_resume, int, Lua::State, Lua::State, int)(state, NULL, argc);
}

void LuaLib::rotate(Lua::State state, int index, int n) {
		logC("LuaLib::rotate()");
		wrap(lua_rotate, void, Lua::State, int, int)(state, index, n);
}

void LuaLib::setField(Lua::State state, int index, std::string name) {
		logC("LuaLib::setField()");
		wrap(lua_setfield, void, Lua::State, int, const char*)(state, index, name.c_str());
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

void Lua::call(int argc, int resc) {
		luaWrapper->call(state, argc, resc);
}

void Lua::checkType(int index, int type) {
		luaWrapper->checkType(state, index, type);
}

Lua::Integer Lua::checkInteger(int index) {
		return luaWrapper->checkInteger(state, index);
}

std::string Lua::checkString(int index) {
		return luaWrapper->checkString(state, index);
}

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

void* Lua::newUserdata(size_t size) {
		return luaWrapper->newUserdata(state, size);
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

void Lua::pushBoolean(bool value) {
		luaWrapper->pushBoolean(state, value);
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

void Lua::pushNil() {
		luaWrapper->pushNil(state);
}

void Lua::pushNumber(Number value) {
		luaWrapper->pushNumber(state, value);
}

void Lua::pushString(std::string value) {
		luaWrapper->pushString(state, value);
}

void Lua::rawSet(int index) {
		luaWrapper->rawSet(state, index);
}

void Lua::rawSetI(int index, Integer i) {
		luaWrapper->rawSetI(state, index, i);
}

void Lua::remove(int index) {
		luaWrapper->remove(state, index);
}

int Lua::resume(int argc) {
		return luaWrapper->resume(state, argc);
}

void Lua::rotate(int index, int n) {;
		luaWrapper->rotate(state, index, n);
}

void Lua::setField(int index, std::string name) {
		luaWrapper->setField(state, index, name);
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