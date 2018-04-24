#ifndef __LUA_LIB_H__
#define __LUA_LIB_H__

#include <string>
#include <map>
#include <stdint.h>

class LuaLib;

extern std::map<std::string, LuaLib*> luaLibs;

class Lua {
		public:
				typedef struct lua_State* State;
		private:
				LuaLib* luaWrapper;
				State state;
		public:
				typedef void* (* Allocator) (void*, void*, size_t, size_t);
				typedef int (* Function) (State);
				typedef intptr_t KContext;
				typedef int (* KFunction) (State, int, KContext);
				typedef long long Integer;
				typedef long double Number;
				typedef unsigned int Unsigned;
				static const inline int Yield = 1;
				static const inline int GCCollect = 2;
				static const inline int TNil = 0;
				static const inline int TBoolean = 1;
				static const inline int TLightUserdata = 2;
				static const inline int TNumber = 3;
				static const inline int TString = 4;
				static const inline int TTable = 5;
				static const inline int TFunction = 6;
				static const inline int TUserdata = 7;
				static const inline int MaxStack = 1000000;
				static const inline int RegistryIndex = -MaxStack - 1000;
				Lua();
				Lua(LuaLib*, State);
				void call(int, int);
				void checkType(int, int);
				Lua::Integer checkInteger(int);
				std::string checkString(int);
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
				void* newUserdata(size_t);
				void openLibs();
				int loadBufferX(const std::string, const std::string, const std::string = "bt");
				void pop(int);
				void pushBoolean(bool);
				void pushCClosure(Lua::Function, int);
				void pushInteger(Integer);;
				void pushLightUserdata(void*);
				void pushNil();
				void pushNumber(Lua::Number);
				void pushString(std::string);
				void rawSet(int);
				void rawSetI(int, Lua::Integer);
				void remove(int);
				int resume(int);
				void rotate(int, int);
				void setField(int, std::string);
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
				void* lua_callk;
				void* luaL_checktype;
				void* luaL_checkinteger;
				void* luaL_checklstring;
				void* lua_createtable;
				void* lua_gc;
				void* lua_getallocf;
				void* lua_gettop;
				void* lua_newstate;
				void* lua_newuserdata;
				void* luaL_openlibs;
				void* luaL_loadbufferx;
				void* lua_pop;
				void* lua_pushboolean;
				void* lua_pushcclosure;
				void* lua_pushinteger;
				void* lua_pushlightuserdata;
				void* lua_pushnil;
				void* lua_pushnumber;
				void* lua_pushstring;
				void* lua_rawset;
				void* lua_rawseti;
				void* lua_resume;
				void* lua_rotate;
				void* lua_setfield;
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
				void call(Lua::State, int, int);
				void checkType(Lua::State, int, int);
				Lua::Integer checkInteger(Lua::State, int);
				std::string checkString(Lua::State, int);
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
				void* newUserdata(Lua::State, size_t);
				void openLibs(Lua::State);
				int loadBufferX(Lua::State, const std::string, const std::string, const std::string);
				void pop(Lua::State, int);
				void pushBoolean(Lua::State, bool);
				void pushCClosure(Lua::State, Lua::Function, int);
				void pushInteger(Lua::State, Lua::Integer);
				void pushLightUserdata(Lua::State, void*);
				void pushNil(Lua::State);
				void pushNumber(Lua::State, Lua::Number);
				void pushString(Lua::State, std::string);
				void rawSet(Lua::State, int);
				void rawSetI(Lua::State, int, Lua::Integer);
				void remove(Lua::State, int);
				int resume(Lua::State, int);
				void rotate(Lua::State, int, int);
				void setField(Lua::State, int, std::string);
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