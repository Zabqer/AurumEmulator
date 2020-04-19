#ifndef __AURUM_CONFIG_H__
#define __AURUM_CONFIG_H__

#include <string>
#include <vector>
#include <array>

#include "utils/graphics.h"

class Machine;

struct AurumConfig_t {
		bool changed = false;
		bool* force_logging_call = NULL;
		bool* force_logging_debug = NULL;
		bool logging_call;
		bool logging_debug;
		std::string envPath;
		size_t threads;
		double timeout;
		double startupDelay;
		bool allowBytecode;
		bool allowGC;
		size_t eepromSize;
		size_t eepromDataSize;
		std::array<size_t, 4> cpuComponentCount;
		std::array<double, 3> callBudgets;
		std::array<size_t, 6> ramSizes;
		static const PointXY screenResolutionsByTier[3];
		static const ColorDepth screenDepthByTier[3];
		static const unsigned int basicScreenPixels;
		bool ignorePower;
		size_t tickFrequency;
		double computerCost;
		double sleepCostFactor;
		double hddReadCost;
		double hddWriteCost;
		double gpuSetCost;
		double gpuFillCost;
		double gpuClearCost;
		double gpuCopyCost;
		double eepromWriteCost;
		std::array<size_t, 3> hddSizes;
		size_t maxHandles;
		size_t maxReadBuffer;
		std::vector<Machine*> machines;
};

extern AurumConfig_t AurumConfig;

void AurumConfigFromYAML(std::string);
std::string AurumConfigToYAML();
void AurumConfigSave();


#endif