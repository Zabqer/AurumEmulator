#ifndef __AURUM_LUA53ARCHITECTURE_H__
#define __AURUM_LUA53ARCHITECTURE_H__

#include "../architecture.h"
#include "lua_lib.h"

class Lua53Architecture: public Architecture {
		private:
				Lua lua;
				Machine* machine;
				size_t kernelMemory = 0;
		public:
				Lua53Architecture();
				bool initialize(Machine*) override;
				ExecutionResult runThreaded(bool) override;
				void runSynchronized() override;
};

#endif