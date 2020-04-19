#include "uuid_generator.h"

static char hexs[] = {'0', '1', '2', '3', '4', '4', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string generateUUID() {
		std::string uuid;
		for(int i = 0; i < 36; i++) {
				if (i == 8 || i == 13 || i == 18 || i == 23) {
						uuid += "-";
				} else if (i == 14) {
						uuid += "4";
				} else {
						uuid += hexs[rand() % 16];
				}
		}
		return uuid;
}