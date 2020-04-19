#include "font.h"

#include <math.h>

#include "../resources/font_hex.h"

std::map<char16_t, std::vector<bool>> fontBitmap;

static bool loaded = false;

bool isFontLoaded() {
		return loaded;
}

static unsigned short int getHex(char ch) {
		if (ch >= '0' && ch <= '9') {
				return ch - '0';
		} else if (ch >= 'A' && ch <= 'F') {
				return ch - 'A' + 10;
		}
		throw std::runtime_error("not a hex char");
}

void loadFont() {
		size_t pos = 0;
		size_t dlen = 0;
		char16_t index = 0;
		while (FONT_HEX[pos] != '\0') {
				while (FONT_HEX[pos + dlen] != ':') {
						dlen++;
				}
				for (unsigned int i = 0; i < dlen; i++) {
						index += pow(16, i) * getHex(FONT_HEX[pos + dlen - 1 - i]);
				}
				pos += dlen + 1;
				dlen = 0;
				while (FONT_HEX[pos + dlen] != '\n') {
						dlen++;
				}
				std::vector<bool> data;
				for (unsigned int i = 0; i < dlen; i += 2) {
						char c = (getHex(FONT_HEX[pos + i]) << 4) + getHex(FONT_HEX[pos + i + 1]);
						for (int j = 0; j < 8; j++) {
								data.push_back((c & 0x80) != 0);
								c <<= 1;
						}
				}
				pos += dlen + 1;
				fontBitmap[index] = data;
				dlen = 0;
				index = 0;
		}
		loaded = true;
}