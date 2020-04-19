#include "unicode.h"

#include <locale>
#include <codecvt>

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;

std::u16string u8to16(std::string str) {
		return converter.from_bytes(str);
}

std::string u16to8(std::u16string str) {
		return converter.to_bytes(str);
}