#include "lua2c.h"
#include "../../log.h"

std::vector<std::any> lua2c(Lua lua) {
		logC("lua2c()");
		 std::vector<std::any> args;
		int top = lua.getTop();
		for (int i = 1; i <= top; i++) {
				switch (lua.type(i)) {
						case Lua::TNil:
								logD("lua2c: nil -> Null");
								args.push_back(Null());
								break;
						case Lua::TBoolean:
								logD("lua2c: boolean -> bool");
								args.push_back(lua.toBoolean(i));
								break;
						case Lua::TNumber:
								logD("lua2c: number -> double");
								args.push_back((double)lua.toNumber(i));
								break;
						case Lua::TString:
								logD("lua2c: string -> std::string (" + lua.toString(i) + ")");
								args.push_back(lua.toString(i));
								break;
						case Lua::TLightUserdata:
						case Lua::TUserdata:
								logD("lua2c: userdata -> std::any");
								args.push_back(*((std::any*)lua.toUserdata(i)));
								break;
						default:
								logE("lua2c: " << lua.type(i));
								exit(1);
				}
		}
		lua.pop(top);
		return args;
}