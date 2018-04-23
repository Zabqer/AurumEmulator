#include "computer_api.h"
#include "api.h"
#include "../../log.h"
#include "lua2c.h"

#include <chrono>

API(computer_realtime) {
		logC("ComputerAPI::realTime()");
		lua.pushNumber(std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count());
		return 1;
}

API(computer_uptime) {
		logC("ComputerAPI::uptime()");
		lua.pushInteger(machine->uptime());
		return 1;
}

API(computer_address) {
		logC("ComputerAPI::address()");
		lua.pushString(machine->address());
		return 1;
}

API(computer_freeMemory) {
		logC("ComputerAPI::freeMemory()");
		lua.pushInteger(machine->getTotalMemory() - machine->getUsedMemory());
		return 1;
}

API(computer_totalMemory) {
		logC("ComputerAPI::totalMemory()");
		lua.pushInteger(machine->getTotalMemory());
		return 1;
}

API(computer_pushSignal) {
		logC("ComputerAPI::pushSignal()");
		std::string name = lua.checkString(1);
		lua.remove(1);
		lua.pushBoolean(machine->signal({name, lua2c(lua)}));
		return 1;
}

API(computer_tmpAddress) {
		logC("ComputerAPI::tmpAddress()");
		if (machine->tmpFS() != NULL) {
				lua.pushString(machine->tmpFS()->address());
		} else {
				lua.pushNil();
		}
		return 1;
}

// Maybe real users? How?

API(computer_users) {
		logC("ComputerAPI::users()");
		lua.createTable(0, 0);
		lua.pushString("AurumEmulator");
		lua.rawSetI(-2, 1);
		return 1;
}

API(computer_addUser) {
		logC("ComputerAPI::addUser()");
		lua.pushNil();
		lua.pushString("player must be online");
		return 2;
}

API(computer_removeUser) {
		logC("ComputerAPI::removeUser()");
		lua.pushBoolean(false);
		return 1;
}

API(computer_energy) {
		logC("ComputerAPI::energy()");
		lua.pushNumber(0);
		return 1;
}

API(computer_maxEnergy) {
		logC("ComputerAPI::maxEnergy()");
		lua.pushNumber(0);
		return 1;
}

API(computer_getArchitectures) {
		logC("ComputerAPI::getArchitectures()");
		lua.createTable(0, 0);
		return 1;
}

API(computer_getArchitecture) {
		logC("ComputerAPI::getArchitecture()");
		lua.pushString("Lua 5.3");
		return 1;
}

API(computer_setArchitecture) {
		logC("ComputerAPI::setArchitecture()");
		lua.pushBoolean(false);
		return 1;
}

void loadComputerAPI(Lua lua) {
		lua.createTable(0, 0);
		loadAPIFunction(lua, "realTime", computer_realtime);
		loadAPIFunction(lua, "uptime", computer_uptime);
		loadAPIFunction(lua, "address", computer_address);
		loadAPIFunction(lua, "freeMemory", computer_freeMemory);
		loadAPIFunction(lua, "totalMemory", computer_totalMemory);
		loadAPIFunction(lua, "pushSignal", computer_pushSignal);
		loadAPIFunction(lua, "tmpAddress", computer_tmpAddress);
		loadAPIFunction(lua, "users", computer_users);
		loadAPIFunction(lua, "addUser", computer_addUser);
		loadAPIFunction(lua, "removeUser", computer_removeUser);
		loadAPIFunction(lua, "energy", computer_energy);
		loadAPIFunction(lua, "maxEnergy", computer_maxEnergy);
		loadAPIFunction(lua, "getArchitectures", computer_getArchitectures);
		loadAPIFunction(lua, "getArchitecture", computer_getArchitecture);
		loadAPIFunction(lua, "setArchitecture", computer_setArchitecture);
		lua.setGlobal("computer");
};