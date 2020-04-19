#include "unicode_api.h"
#include "api.h"
#include "../../log.h"
#include "../../utils/font.h"

#include <locale>
#include <codecvt>
#include <algorithm>

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;

std::string fromUTF16(std::u16string str) {
		return convert.to_bytes(str);
}

std::u16string toUTF16(std::string str) {
		return convert.from_bytes(str);
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
		std::u16string str = toUTF16(lua.checkString(1));
		std::transform(str.begin(), str.end(), str.begin(), tolower);
		lua.pushString(fromUTF16(str));
		return 1;
}

API(unicode_reverse) {
		logC("UnicodeAPI::reverse()");
		std::u16string str = toUTF16(lua.checkString(1));
		std::reverse(str.begin(), str.end());
		lua.pushString(fromUTF16(str));
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
				result = result.substr(at, to - at);
				lua.pushString(fromUTF16(result));
		}
		return 1;
}

API(unicode_upper) {
		logC("UnicodeAPI::upper()");
		std::u16string str = toUTF16(lua.checkString(1));
		std::transform(str.begin(), str.end(), str.begin(), toupper);
		lua.pushString(fromUTF16(str));
		return 1;
}

static size_t chlen(char16_t ch) {
		if (!fontBitmap.count(ch)) {
				return 0;
		} else {
				return fontBitmap[ch].size() / 8 / 16;
		}
}

API(unicode_isWide) {
		logC("UnicodeAPI::isWide()");
		lua.pushBoolean(chlen(toUTF16(lua.checkString(1))[0]) > 1);
		return 1;
}

API(unicode_charWidth) {
		logC("UnicodeAPI::charWidth()");
		lua.pushInteger(chlen(toUTF16(lua.checkString(1))[0]));
		return 1;
}

API(unicode_wlen) {
		logC("UnicodeAPI::wlen()");
		std::u16string str = toUTF16(lua.checkString(1));
		size_t size = 0;
		for (const char16_t& ch : str) {
				size += std::max((size_t) 1, chlen(ch));
		}
		lua.pushInteger(size);
		return 1;
}

API(unicode_wtrunc) {
		logC("UnicodeAPI::wtrunc()");
		std::u16string str = toUTF16(lua.checkString(1));
		int count = std::max((Lua::Integer) 0, lua.checkInteger(2));
		int i = 0;
		size_t width = 0;
		while (width <= count) {
				width += std::max((size_t) 1, chlen(str[i++]));
		}
		if (i > 1) {
				lua.pushString(fromUTF16(str.substr(0, i - 1)));
		} else {
				lua.pushString("");
		}
		return 1;
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
