#include <string.h>

#include "log.h"

#define VERSION "0.0.0.2"

#define AURUM_DEFAULT_CONFIG_PATH "~/AurumEmulator"

void help() {
		log("AurumEmulator help: ");
		log(" Options: ");
		log("  -h -- display this help")
		log("  --force-call=true|false -- ignore config call log option");
		log("  --force-debug=true|false -- ignore config debug log option");
		log("  --config=<path> -- set config path; default - ~/AurumEmulator/")
}

int main(int argc, char** argv) {
		
		log("Aurum Emulator / Zabqer / " << VERSION);

		std::string configPath = AURUM_DEFAULT_CONFIG_PATH;

		bool* forcecall = NULL;
		bool* forcedebug = NULL;

		for (int i = 1; i < argc; i++) {
				std::string arg = std::string(argv[i]);
				if (arg.substr(0, 2) == "--") {
						size_t pos = arg.find("=");
						std::string option = arg.substr(2, pos - 2);
						std::string value = (pos + 1) != 0 ? arg.substr(pos + 1) : "";
						if (option == "force-call") {
								if (value == "true") {
										forcecall = new bool(true);
								} else if (value == "false") {
										forcecall = new bool(false);
								} else {
										logE("Wrong option value: " << option << " = " << value);
										help();
										return 1;
								}
						} else if (option == "force-debug") {
								if (value == "true") {
										forcedebug = new bool(true);
								} else if (value == "false") {
										forcedebug = new bool(false);
								} else {
										logE("Wrong option value: " << option << " = " << value);
										help();
										return 1;
								}
						} else if (option == "config") {
								if (value.length() > 0) {
										configPath = value;
								}
						} else {
								logE("Wrong option: " << option);
								help();
								return 1;
						}
				} else if (arg.substr(0, 1) == "-") {
						std::string option = arg.substr(1);
						if (option == "h") {
								help();
								return 0;
						} else {
								logE("Wrong option: " << option);
								help();
								return 1;
						}
				} else {
						logE("Wrong argument: " << arg);
						help();
						return 1;
				}
		}

		//TODO: Config

		delete forcecall;
		delete forcedebug;

		return 0;
}