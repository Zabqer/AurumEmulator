#include "lua53architecture.h"
#include "lua_lib.h"
#include "allocator.h"
#include "../../log.h"
#include "../../resources/machine_lua.h"
#include "computer_api.h"
#include "component_api.h"

Lua53Architecture::Lua53Architecture() {}

bool Lua53Architecture::initialize(Machine* machine_) {
		logC("Lua53Architecture::initialize()");
		machine = machine_;
		//FIX FIX FIX: dlclose on non use!
		lua = (new LuaLib("5.3"))->newState(allocator, machine);
		lua.openLibs();
		lua.loadBufferX(MACHINE_LUA, "=machine.lua", "t");
		/* APIs */
		loadComputerAPI(lua);
		loadComponentAPI(lua);
		return true;
}

void Lua53Architecture::runSynchronized() {
		logC("Lua53Architecture::runSynchronized()");
}

ExecutionResult Lua53Architecture::runThreaded(bool isSynchronizedReturn) {
		logC("Lua53Architecture::runThreaded()");
		int results;
		if (isSynchronizedReturn) {
				assert(lua.getTop() == 2);
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
								lua.pushInteger(0);
								results = 1;
						}
				} else {
						void/*Signal*/* signal = 0;//machine->popSignal();
						if (signal != NULL) {
								//lua.pushString(signal.name());
								//convert
								//results = lua.resume(1 + signal.size())
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