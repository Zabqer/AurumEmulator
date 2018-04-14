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
				typedef int Integer;
				typedef float Number;
				typedef unsigned int Unsigned;
				static const inline int Yield = 1;
				static const inline int GCCollect = 2;
				static const inline int TBoolean = 1;
				static const inline int TNumber = 3;
				static const inline int TString = 4;
				static const inline int TTable = 5;
				static const inline int TFunction = 6;
				Lua();
				Lua(LuaWrapper*, State);
				int gc(int, int);
				int getTop();
				bool isBoolean(int);
				bool isFunction(int);
				bool isNoneOrNil(int);
				bool isNumber(int);
				bool isString(int);
				bool isTable(int);
				void openLibs();
				int loadBufferX(const std::string, const std::string, const std::string = "bt");
				void pop(int);
				void pushInteger(Integer);
				int resume(int); 
				int status();
				bool toBoolean(int);
				Number toNumber(int);
				std::string toString(int);
				int type(int);
};

class LuaWrapper {
		private:
				void* lib;
				void* lua_gc;
				void* lua_gettop;
				void* lua_newstate;
				void* luaL_openlibs;
				void* luaL_loadbufferx;
				void* lua_pop;
				void* lua_pushinteger;
				void* lua_resume;
				void* lua_status;
				void* lua_toboolean;
				void* lua_tonumber;
				void* lua_tolstring;
				void* lua_type;
		public:
				LuaWrapper(const std::string);
				~LuaWrapper();
				int gc(Lua::State, int, int);
				int getTop(Lua::State);
				bool isBoolean(Lua::State, int);
				bool isFunction(Lua::State, int);
				bool isNoneOrNil(Lua::State, int);
				bool isNumber(Lua::State, int);
				bool isString(Lua::State, int);
				bool isTable(Lua::State, int);
				Lua newState(Lua::Allocator, void*);
				void openLibs(Lua::State);
				int loadBufferX(Lua::State, const std::string, const std::string, const std::string);
				void pop(Lua::State, int);
				void pushInteger(Lua::State, Lua::Integer);
				int resume(Lua::State, int); 
				int status(Lua::State);
				bool toBoolean(Lua::State, int);
				Lua::Number toNumber(Lua::State, int);
				std::string toString(Lua::State, int);
				int type(Lua::State, int);
};

#endif