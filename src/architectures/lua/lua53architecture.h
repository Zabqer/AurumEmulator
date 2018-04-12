#ifndef __AURUM_LUA53ARCHITECTURE_H__
#define __AURUM_LUA53ARCHITECTURE_H__

#include "../architecture.h"
#include "lua_wrapper.h"

class Lua53Architecture: public Architecture {
		private:
				Lua lua;
		public:
				Lua53Architecture();
				bool initialize(Machine*) override;
};

#endif