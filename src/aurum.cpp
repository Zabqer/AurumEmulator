#include <string.h>
#include <fstream>
#include <sstream>
#include <signal.h>


#include "log.h"
#include "config.h"
#include "machine.h"

#define VERSION "0.2.0.0"

std::ofstream logFile;

#define AURUM_DEFAULT_PATH std::string(getenv("HOME")) + "/AurumEmulator/"

void help() {
		_log("Help: ");
		_log(" Options: ");
		_log("  -h -- display this help");
		_log("  --force-call=true|false -- ignore config call log option");
		_log("  --force-debug=true|false -- ignore config debug log option");
		_log("  --path=<path> -- set emulator work path; default - <HOME>/AurumEmulator/");
}

void interuptHandler(int dummy) {
	for (Machine* machine : AurumConfig.machines) {
		machine->close();
	}
}



int main(int argc, char** argv) {

		AurumConfig.envPath = AURUM_DEFAULT_PATH;

		for (int i = 1; i < argc; i++) {
				std::string arg = std::string(argv[i]);
				if (arg.substr(0, 2) == "--") {
						size_t pos = arg.find("=");
						std::string option = arg.substr(2, pos - 2);
						std::string value = (pos + 1) != 0 ? arg.substr(pos + 1) : "";
						if (option == "force-call") {
								if (value == "true") {
										AurumConfig.force_logging_call = new bool(true);
								} else if (value == "false") {
										AurumConfig.force_logging_call = new bool(false);
								} else {
										logE("Wrong option value: " << option << " = " << value);
										help();
										return 1;
								}
						} else if (option == "force-debug") {
								if (value == "true") {
										AurumConfig.force_logging_debug = new bool(true);
								} else if (value == "false") {
										AurumConfig.force_logging_debug = new bool(false);
								} else {
										logE("Wrong option value: " << option << " = " << value);
										help();
										return 1;
								}
						} else if (option == "path") {
								if (value.length() > 0) {
										AurumConfig.envPath = value;
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
		
		logFile = std::ofstream(AurumConfig.envPath + "/log.txt", std::ios::out);

		_log("Aurum Emulator / Zabqer / " VERSION);
		
		std::string configPath = AurumConfig.envPath + "/Config.yaml";
		
		logD("Loading configuration");

		std::ifstream ic(configPath);
		if (ic.is_open()) {
				std::stringstream iss;
				iss << ic.rdbuf();
				AurumConfigFromYAML(iss.str());
				ic.close();
		} else {
				AurumConfigFromYAML("");
		}

		signal(SIGINT, interuptHandler);
		
		logD("Starting machines");
				
		for (Machine* machine : AurumConfig.machines) {
				machine->start();
		}
		
		logD("Entering to update loop");

		clock_t deadline = clock();
		clock_t nextsave = clock();
		AurumConfig.changed = true;
		size_t works = AurumConfig.machines.size();
		while (works) {
				if (nextsave <= clock()) {
						nextsave = clock() + 1000000;
						if (AurumConfig.changed) {
								logD("Saving config...");
								std::ofstream oc(configPath);
								if (oc.is_open()) {
										std::stringstream oss;
										oss << AurumConfigToYAML();
										oc << oss.rdbuf();
										oc.close();
										logD("Config saved");
								} else {
										logW("Can't open config file for write: " << configPath);
								}
								AurumConfig.changed = false;
						}
				}
				if (deadline <= clock()) {
						deadline = clock() + 1000000 / AurumConfig.tickFrequency;
						works = 0;
						for (Machine* machine : AurumConfig.machines) {
								if (machine->isRunning()) {
									machine->update();
									works++;	
								} else if (machine->isCrashed()) {
									works++;
								}
						}
				}
		}

		logD("All machines dead; exiting");
		
		return 0;
}
