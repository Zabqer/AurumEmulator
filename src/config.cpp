#include "config.h"

#include "log.h"

#include <yaml-cpp/yaml.h>

#include "machine.h"
#include "components/cpu.h"
#include "components/ram.h"
#include "components/eeprom.h"
#include "components/filesystem.h"
#include "components/gpu.h"
#include "components/screen.h"

AurumConfig_t AurumConfig;

const PointXY AurumConfig_t::screenResolutionsByTier[3] {{50, 16}, {80, 25}, {160, 50}};
const ColorDepth AurumConfig_t::screenDepthByTier[3] {ColorDepth::OneBit, ColorDepth::FourBit, ColorDepth::EightBit};
const unsigned int AurumConfig_t::basicScreenPixels = AurumConfig_t::screenResolutionsByTier[0].x * AurumConfig_t::screenResolutionsByTier[0].y;

void AurumConfigFromYAML(std::string yaml) {
		logC("AurumConfigFromYAML()");
		YAML::Node root = YAML::Load(yaml);
		AurumConfig.logging_call = root["logging"]["call"].as<bool>(false);
		AurumConfig.logging_debug = root["logging"]["debug"].as<bool>(false);
		AurumConfig.threads = std::max(root["computer"]["threads"].as<size_t>(4), (size_t) 1);
		Machine::threadPool.resize(AurumConfig.threads);
		AurumConfig.timeout = std::max(root["computer"]["timeout"].as<double>(5.0), (double) 0);
		AurumConfig.startupDelay = std::max(root["computer"]["startupDelay"].as<double>(0.25), 0.05);
		AurumConfig.allowBytecode = root["computer"]["allowBytecode"].as<bool>(false);
		AurumConfig.allowGC = root["computer"]["allowGC"].as<bool>(false);
		AurumConfig.eepromSize = std::max(root["computer"]["eepromSize"].as<size_t>(4096), (size_t) 0);
		AurumConfig.eepromDataSize = std::max(root["computer"]["eepromDataSize"].as<size_t>(256), (size_t) 0);
		AurumConfig.cpuComponentCount  = root["computer"]["cpuComponentCount"].as<std::array<size_t, 4>>(std::array<size_t, 4> {8, 12, 16, 1024});
		AurumConfig.callBudgets = root["computer"]["callBudgets"].as<std::array<double, 3>>(std::array<double, 3> {0.5, 1.0, 1.5});
		AurumConfig.ramSizes = root["computer"]["ramSizes"].as<std::array<size_t, 6>>(std::array<size_t, 6> {192, 256, 384, 512, 768, 1024});
		AurumConfig.ignorePower = root["power"]["ignorePower"].as<bool>(false);
		AurumConfig.tickFrequency = std::max(root["power"]["tickFrequency"].as<size_t>(10), (size_t) 1);
		AurumConfig.computerCost = std::max(root["power"]["cost"]["computer"].as<double>(0.5), (double) 0);
		AurumConfig.sleepCostFactor = std::max(root["power"]["cost"]["sleepFactor"].as<double>(0.1), (double) 0);
		AurumConfig.hddReadCost = std::max(root["power"]["cost"]["hddRead"].as<double>(0.1), (double) 0) / 1024;
		AurumConfig.hddWriteCost = std::max(root["power"]["cost"]["hddRead"].as<double>(0.25), (double) 0) / 1024;
		AurumConfig.gpuSetCost = std::max(root["power"]["cost"]["gpuSet"].as<double>(2.0), (double) 0) / AurumConfig_t::basicScreenPixels;
		AurumConfig.gpuFillCost = std::max(root["power"]["cost"]["gpuFill"].as<double>(1.0), (double) 0) / AurumConfig_t::basicScreenPixels;
		AurumConfig.gpuClearCost = std::max(root["power"]["cost"]["gpuClear"].as<double>(0.1), (double) 0) / AurumConfig_t::basicScreenPixels;
		AurumConfig.gpuCopyCost = std::max(root["power"]["cost"]["gpuCopy"].as<double>(0.25), (double) 0) / AurumConfig_t::basicScreenPixels;
		AurumConfig.eepromWriteCost = std::max(root["power"]["cost"]["eepromWrite"].as<double>(50), (double) 0);
		AurumConfig.hddSizes = root["filesystem"]["hddSizes"].as<std::array<size_t, 3>>(std::array<size_t, 3> {1024, 2048, 4096});
		AurumConfig.maxHandles = std::max(root["filesystem"]["maxHandles"].as<double>(16), (double) 0);
		AurumConfig.maxReadBuffer = std::max(root["filesystem"]["maxReadBuffer"].as<double>(2048), (double) 0);
		if (root["machines"].IsSequence()) {
				AurumConfig.machines.clear();
				for (YAML::Node machineNode : root["machines"]) {
						Machine* machine = new Machine;
						machine->load(machineNode);
						AurumConfig.machines.push_back(machine);
				}
		} else {
				Machine* machine = new Machine;
				YAML::Node machineNode;
				YAML::Node cpu;
				cpu["type"] = "cpu";
				machineNode["components"].push_back(cpu);
				YAML::Node ram;
				ram["type"] = "ram";
				machineNode["components"].push_back(ram);
				YAML::Node eeprom;
				eeprom["type"] = "eeprom";
				machineNode["components"].push_back(eeprom);
				YAML::Node gpu;
				gpu["type"] = "gpu";
				machineNode["components"].push_back(gpu);
				YAML::Node screen;
				screen["type"] = "screen";
				machineNode["components"].push_back(screen);
				YAML::Node filesystem;
				filesystem["type"] = "filesystem";
				machineNode["components"].push_back(filesystem);
				machine->load(machineNode);
				AurumConfig.machines.push_back(machine);
		}
}

std::string AurumConfigToYAML() {
		logC("AurumConfigToYAML()");
		YAML::Node root;
		root["logging"]["call"] = AurumConfig.logging_call;
		root["logging"]["debug"] = AurumConfig.logging_debug;
		root["computer"]["threads"] = AurumConfig.threads;
		root["computer"]["timeout"] = AurumConfig.timeout;
		root["computer"]["startupDelay"] = AurumConfig.startupDelay;
		root["computer"]["allowBytecode"] = AurumConfig.allowBytecode;
		root["computer"]["allowGC"] = AurumConfig.allowGC;
		root["computer"]["eepromSize"] = AurumConfig.eepromSize;
		root["computer"]["eepromDataSize"] = AurumConfig.eepromDataSize;
		root["computer"]["cpuComponentCount"] = AurumConfig.cpuComponentCount;
		root["computer"]["callBudgets"] = AurumConfig.callBudgets;
		root["computer"]["ramSizes"] = AurumConfig.ramSizes;
		root["power"]["ignorePower"] = AurumConfig.ignorePower;
		root["power"]["tickFrequency"] = AurumConfig.tickFrequency;
		root["power"]["cost"]["computer"] = AurumConfig.computerCost;
		root["power"]["cost"]["sleepFactor"] = AurumConfig.sleepCostFactor;
		root["power"]["cost"]["hddRead"] = AurumConfig.hddReadCost * 1024;
		root["power"]["cost"]["hddWrite"] = AurumConfig.hddWriteCost * 1024;
		root["power"]["cost"]["gpuSet"] = AurumConfig.gpuSetCost * AurumConfig_t::basicScreenPixels;
		root["power"]["cost"]["gpuFill"] = AurumConfig.gpuFillCost * AurumConfig_t::basicScreenPixels;
		root["power"]["cost"]["gpuClear"] = AurumConfig.gpuClearCost * AurumConfig_t::basicScreenPixels;
		root["power"]["cost"]["gpuCopy"] = AurumConfig.gpuCopyCost * AurumConfig_t::basicScreenPixels;
		root["power"]["cost"]["eepromWrite"] = AurumConfig.eepromWriteCost;
		root["filesystem"]["hddSizes"] = AurumConfig.hddSizes;
		root["filesystem"]["maxHandles"] = AurumConfig.maxHandles;
		root["filesystem"]["maxReadBuffer"] = AurumConfig.maxReadBuffer;
		for (Machine* machine : AurumConfig.machines) {
				YAML::Node machineNode;
				machine->save(machineNode);
				root["machines"].push_back(machineNode);
		}
		YAML::Emitter emitter;
		emitter << root;
		return emitter.c_str();
}

void AurumConfigSave() {
		AurumConfig.changed = true;
}