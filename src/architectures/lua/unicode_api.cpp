#include "unicode_api.h"
#include "api.h"
#include "../../log.h"
#include <boost/locale.hpp>

std::string
 fromUTF16(std::u16string str) {
		return boost::locale::conv::utf_to_utf<char>(str);
}

std::u16string toUTF16(std::string str) {
		return boost::locale::conv::utf_to_utf<char16_t>(str);
}

API(unicode_char) {
		logC("UnicodeAPI::char()");
		std::u16string result;
		int top = lua.getTop();
		for (int i = 1; i <= top; i++) {
				result += lua.checkInteger(i);
		}
		lua.pushString(fromUTF16(result));
		std::string a;
		std::u16string b = toUTF16(a);
		return 1;
}

API(unicode_len) {
		logC("UnicodeAPI::len()");
		lua.pushInteger(toUTF16(lua.checkString(1)).length());
		return 1;
}

API(unicode_lower) {
		logC("UnicodeAPI::lower()");
		
		return 1;
}

API(unicode_reverse) {
		logC("UnicodeAPI::reverse()");
		
		return 1;
}

API(unicode_sub) {
		logC("UnicodeAPI::sub()");
		std::u16string result = toUTF16(lua.checkString(1));
		int at = lua.checkInteger(2);
		if (at < 0) {
				at = result.length() + at;
		} else {
				at -= 1;
		}
		at = std::max(0, at);
		int to;
		if (lua.getTop() > 2) {
				to = lua.checkInteger(3);
				if (to < 0) {
						to = result.length() + to + 1;
				}
				to = std::min((int) result.length(), to); 
		} else {
				to = result.length();
		}
		if (to <= at) {
				lua.pushString("");
		} else {
				result = result.substr(at, to);
				lua.pushString(fromUTF16(result));
		}
		return 1;
}

API(unicode_upper) {
		logC("UnicodeAPI::upper()");
		
		return 0;
}

API(unicode_isWide) {
		logC("UnicodeAPI::isWide()");
		
		return 0;
}

API(unicode_charWidth) {
		logC("UnicodeAPI::charWidth()");
		
		return 0;
}

API(unicode_wlen) {
		logC("UnicodeAPI::wlen()");
		
		return 0;
}

API(unicode_wtrunc) {
		logC("UnicodeAPI::wtrunc()");
		
		return 0;
}

void loadUnicodeAPI(Lua lua) {
		lua.createTable(0, 0);
		loadAPIFunction(lua, "char", unicode_char);
		loadAPIFunction(lua, "len", unicode_len);
		loadAPIFunction(lua, "lower", unicode_lower);
		loadAPIFunction(lua, "reverse", unicode_reverse);
		loadAPIFunction(lua, "sub", unicode_sub);
		loadAPIFunction(lua, "upper", unicode_upper);
		loadAPIFunction(lua, "isWide", unicode_isWide);
		loadAPIFunction(lua, "charWidth", unicode_charWidth);
		loadAPIFunction(lua, "wlen", unicode_wlen);
		loadAPIFunction(lua, "wtrunc", unicode_wtrunc);
		lua.setGlobal("unicode");
};