#ifndef __AURUM_CONFIG_H__
#define __AURUM_CONFIG_H__

#include <string>
#include <vector>

#include "machine.h"

struct AurumConfig_t {
		bool logging_call;
		bool logging_debug;
};

extern AurumConfig_t AurumConfig;

void AurumConfigFromYAML(std::string yaml, std::vector<Machine*>& machines);
std::string AurumConfigToYAML(std::vector<Machine*>& machines);


#endif