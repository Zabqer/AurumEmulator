#include "computer_api.h"

void loadComputerAPI(Lua lua) {
		lua.createTable(0, 0);
		lua.setGlobal("computer");
};