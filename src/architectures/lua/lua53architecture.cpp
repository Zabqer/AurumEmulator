#include "lua53architecture.h"
#include "lua_lib.h"
#include "allocator.h"
#include "../../log.h"
#include "../../resources/machine_lua.h"
#include "computer_api.h"
#include "component_api.h"
#include "unicode_api.h"
#include "system_api.h"
#include "userdata_api.h"
#include "c2lua.h"

Lua53Architecture::Lua53Architecture() {}

bool Lua53Architecture::initialize(Machine* machine_) {
		logC("Lua53Architecture::initialize()");
		machine = machine_;
		lua = luaLibs["5.3"]->newState(allocator, machine);
		lua.openLibs();
		lua.loadBufferX(MACHINE_LUA, "=machine.lua", "t");
		loadComputerAPI(lua);
		loadComponentAPI(lua);
		loadUnicodeAPI(lua);
		loadSystemAPI(lua);
		loadUserdataAPI(lua);
		return true;
}

void Lua53Architecture::runSynchronized() {
		logC("Lua53Architecture::runSynchronized()");
		assert(lua.getTop() == 1);
		assert(lua.isFunction(1));
		lua.call(0, 1);
		lua.checkType(1, Lua::TTable);
}

ExecutionResult Lua53Architecture::runThreaded(bool isSynchronizedReturn) {
		logC("Lua53Architecture::runThreaded()");
		int results;
		if (isSynchronizedReturn) {
				assert(lua.getTop() == 1);
				assert(lua.isTable(1));
				results = lua.resume(1);
		} else {
				if (kernelMemory == 0) {
						lua.resume(0);
						if (lua.getTop() > 0) {
								results = 0;
						} else {
								lua.gc(Lua::GCCollect, 0);
								kernelMemory = machine->getUsedMemory();
								machine->setTotalMemory(machine->getTotalMemory() + kernelMemory);
								lua.pushInteger(0);
								results = 1;
						}
				} else {
						std::optional<Machine::Signal> signal = machine->popSignal();
						if (signal) {
								lua.pushString((*signal).name);
								c2lua(lua, *signal);
								results = lua.resume(1 + (*signal).size());
						} else {
								results = lua.resume(0);
						}
				}
		}
		if (lua.status() == Lua::Yield) {
				if (results == 1 && lua.isFunction(1)) {
						return ExecutionResultSynchronizedCall();
				} else if (results == 1 && lua.isBoolean(1)) {
						return ExecutionResultShutdown {lua.toBoolean(1)};
				} else {
						int ticks = (results == 1 && lua.isNumber(1)) ? ((int) lua.toNumber(1) * 20) : /*TODO: max val*/9999;
						lua.pop(results);
						return ExecutionResultSleep {ticks};
				}
		} else {
				if (!lua.isBoolean(1) || !(lua.isString(2) || lua.isNoneOrNil(2))) {
						logW("Kernel returned unexpected results.");
				}
				if (lua.toBoolean(1)) {
						logW("Kernel stopped unexpectedly.");
						return ExecutionResultShutdown {false};
				} else {
						std::string message = lua.isString(2) ? lua.toString(2) : "unknown error";
						return ExecutionResultError {message};
				}
		}
}