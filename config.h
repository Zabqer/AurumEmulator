#ifndef __AURUM_CONFIG_H__
#define __AURUM_CONFIG_H__

#include <string>

static struct {
		struct {
				bool call = false;
				bool debug = false;
		} logging;
} AurumConfig;

void AurumConfigFromYAML(std::string);
std::string AurumConfigToYAML();

#endif