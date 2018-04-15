#ifndef __LUA_LIB_H__
#define __LUA_LIB_H__

#include <string>

class LuaLib;

class Lua {
		public:
				typedef struct lua_State* State;
		private:
				LuaLib* luaWrapper;
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
				static const inline int MaxStack = 1000000;
				static const inline int RegistryIndex = -MaxStack - 1000;
				Lua();
				Lua(LuaLib*, State);
				void createTable(int, int);
				int gc(int, int);
				Lua::Allocator getAllocF(void** ud);
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
				void pushCClosure(Lua::Function, int);
				void pushInteger(Integer);;
				void pushLightUserdata(void*);
				void pushString(std::string);
				int resume(int);
				void setGlobal(std::string);
				void setTable(int);
				void setTop(int);
				int status();
				bool toBoolean(int);
				Number toNumber(int);
				std::string toString(int);
				void* toUserdata(int);
				int type(int);
};

class LuaLib {
		private:
				void* lib;
				void* lua_createtable;
				void* lua_gc;
				void* lua_getallocf;
				void* lua_gettop;
				void* lua_newstate;
				void* luaL_openlibs;
				void* luaL_loadbufferx;
				void* lua_pop;
				void* lua_pushcclosure;
				void* lua_pushinteger;
				void* lua_pushlightuserdata;
				void* lua_pushstring;
				void* lua_resume;
				void* lua_setglobal;
				void* lua_settable;
				void* lua_settop;
				void* lua_status;
				void* lua_toboolean;
				void* lua_tonumberx;
				void* lua_tolstring;
				void* lua_touserdata;
				void* lua_type;
		public:
				LuaLib(const std::string);
				~LuaLib();
				void createTable(Lua::State, int, int);
				int gc(Lua::State, int, int);
				Lua::Allocator getAllocF(Lua::State, void** ud);
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
				void pushCClosure(Lua::State, Lua::Function, int);
				void pushInteger(Lua::State, Lua::Integer);
				void pushLightUserdata(Lua::State, void*);
				void pushString(Lua::State, std::string);
				int resume(Lua::State, int); 
				void setGlobal(Lua::State, std::string);
				void setTable(Lua::State, int);
				void setTop(Lua::State, int);
				int status(Lua::State);
				bool toBoolean(Lua::State, int);
				Lua::Number toNumber(Lua::State, int);
				std::string toString(Lua::State, int);
				void* toUserdata(Lua::State, int);
				int type(Lua::State, int);
};

#endif