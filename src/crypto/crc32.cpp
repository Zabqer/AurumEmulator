#include "crc32.h"

uint32_t crc32(std::string data, uint32_t crc) {
		int k;
		crc = ~crc;
		for (char c :data) {
				crc ^= c;
				for (k = 0; k < 8; k++) {
						crc = crc & 1 ? (crc >> 1) ^ 0x82f63b78 : crc >> 1;
				}
		}
		return ~crc;
}