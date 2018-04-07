#ifndef __AURUM_CONFIG_H__
#define __AURUM_CONFIG_H__

#include <string>
#include <vector>

struct MachineEntry {
		std::string address;
};

inline struct AurumConfig_t {
		struct {
				bool call = false;
				bool debug = false;
		} logging;
		bool test = false;
		std::vector<MachineEntry> machines = {{"computer1_address"}};
} AurumConfig;

void AurumConfigFromYAML(std::string);
std::string AurumConfigToYAML();

#endif