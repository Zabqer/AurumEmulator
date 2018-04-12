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
		std::array<double, 3> callBudgets;
		std::array<int, 6> ramSizes;
		bool ignorePower;
		int tickFrequency;
		double computerCost;
		double sleepCostFactor;
};

extern AurumConfig_t AurumConfig;

void AurumConfigFromYAML(std::string, std::vector<Machine*>&, bool*, bool*);
std::string AurumConfigToYAML(std::vector<Machine*>&);


#endif