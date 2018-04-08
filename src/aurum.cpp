#include <string.h>
#include <fstream>
#include <sstream>

#include "log.h"
#include "config.h"
#include "machine.h"

#define VERSION "0.0.0.6"

#define AURUM_DEFAULT_PATH "~/AurumEmulator/"

void help() {
		_log("AurumEmulator help: ");
		_log(" Options: ");
		_log("  -h -- display this help")
		_log("  --force-call=true|false -- ignore config call log option");
		_log("  --force-debug=true|false -- ignore config debug log option");
		_log("  --path=<path> -- set emulator work path; default - ~/AurumEmulator/");
}

int main(int argc, char** argv) {
		
		_log("Aurum Emulator / Zabqer / " << VERSION);

		std::string configPath = AURUM_DEFAULT_PATH;

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
						} else if (option == "path") {
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

		configPath += "/Config.yaml";

		std::vector<Machine*> machines;

		std::ifstream ic(configPath);
		if (ic.is_open()) {
				std::stringstream iss;
				iss << ic.rdbuf();
				AurumConfigFromYAML(iss.str(), machines);
				ic.close();
		}

		if (forcecall)
				AurumConfig.logging_call = *forcecall;

		if (forcedebug)
				AurumConfig.logging_debug = *forcedebug;

		delete forcecall;
		delete forcedebug;

		/*std::ofstream oc(configPath);
		if (oc.is_open()) {
				std::stringstream oss;
				oss << AurumConfigToYAML(machines);
				oc << oss.rdbuf();
				oc.close();
		} else {
				logW("Can't open config file for write: " << configPath);
		}*/

		clock_t deadline = 0;
		while (machines.size() > 0) {
				if (deadline <= clock()) {
						deadline = clock() + 1000000 / 20;
						for (Machine* machine :machines) {
								machine->update();
						}
				}
		}

		logD("All machines dead; exiting");

		return 0;
}