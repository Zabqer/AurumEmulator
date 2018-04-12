#ifndef __LUA_WRAPPER_H__
#define __LUA_WRAPPER_H__

#include <string>

class LuaWrapper;

class Lua {
		public:
				typedef struct lua_State* State;
		private:
				LuaWrapper* luaWrapper;
				State state;
		public:
				typedef void* (* Allocator) (void *, void *, size_t, size_t);
				typedef int (* Function) (lua_State *);
				Lua();
				Lua(LuaWrapper*, State);
				int loadBufferX(const std::string, const std::string, const std::string = "bt");
};

class LuaWrapper {
		private:
				void* lib;
				void* lua_newstate;
				void* luaL_loadbufferx;
		public:
				LuaWrapper(const std::string);
				~LuaWrapper();
				Lua newState(Lua::Allocator, void*);
				int loadBufferX(Lua::State, const std::string, const std::string, const std::string);
};


typedef float lua_Number;
typedef int lua_Integer;
typedef unsigned int lua_Unsigned;

#endif