#ifndef __AURUM_CONFIG_H__
#define __AURUM_CONFIG_H__

#include <string>
#include <vector>
#include <array>

#include "machine.h"

struct AurumConfig_t {
		bool logging_call;
		bool logging_debug;
		std::array<int, 4> cpuComponentCount;
		bool ignorePower;
		int tickFrequency;
		double computerCost;
};

extern AurumConfig_t AurumConfig;

void AurumConfigFromYAML(std::string, std::vector<Machine*>&, bool*, bool*);
std::string AurumConfigToYAML(std::vector<Machine*>&);


#endif