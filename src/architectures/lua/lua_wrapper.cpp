#include "lua_wrapper.h"

#include "dlfcn.h"

#include "../../log.h"

#define wrap(name, ret, ...) ((ret (*)(...)) name)

LuaWrapper::LuaWrapper(const std::string version) {
		logC("LuaWrapper::LuaWrapper()");
		lib = dlopen(("liblua" + version + ".so").c_str(), RTLD_LAZY);
		lua_newstate = dlsym(lib, "lua_newstate");
		luaL_loadbufferx = dlsym(lib, "luaL_loadbufferx");
}

LuaWrapper::~LuaWrapper() {
		logC("LuaWrapper::~LuaWrapper()");
		if (lib) {
				dlclose(lib);
		}
}

Lua LuaWrapper::newState(Lua::Allocator allocator, void* ud) {
		logC("LuaWrapper::newState()");
		return Lua(this, wrap(lua_newstate, Lua::State, Lua::Allocator, void*)(allocator, ud));
}

int LuaWrapper::loadBufferX(Lua::State state, std::string data, const std::string name, const std::string mode) {
		logC("LuaWrapper::loadBufferX()");
		return wrap(luaL_loadbufferx, int, Lua::State, const char*, size_t, const char*, const char*)(state, "", 0, "=test", "t"/*data.c_str(), data.length(), name.c_str(), mode.c_str()*/);
}

Lua::Lua() {}

Lua::Lua(LuaWrapper* luaWrapper_, State state_): state(state_), luaWrapper(luaWrapper_) {}

int Lua::loadBufferX(const std::string data, const std::string name, const std::string mode) {
		return luaWrapper->loadBufferX(state, data, name, mode);
}
