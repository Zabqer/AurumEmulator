#include "lua2c.h"
#include "../../log.h"

Arguments lua2c(Lua lua) {
		logC("lua2c()");
		Arguments args;
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
logD(lua.toNumber(i));
								break;
						case Lua::TString:
								logD("lua2c: string -> std::string");
								args.push_back(lua.toString(i));
								break;
						default:
								logE("lua2c: " << lua.type(i));
								exit(1);
				}
		}
		lua.pop(top);
		return args;
}