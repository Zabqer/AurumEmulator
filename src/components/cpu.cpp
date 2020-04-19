#include "cpu.h"
#include "../log.h"
#include "../config.h"

const std::string CPU::TYPE = "cpu";

CPU::CPU(Machine* machine): Component(machine, TYPE, true) {
		logC("CPU::CPU()");
		maxTier = 2;
}

void CPU::save(YAML::Node& node) {
		logC("CPU::save()");
		Component::save(node);
		node["architecture"] = architectureName;
}

void CPU::load(YAML::Node node) {
		logC("CPU::load()");
		Component::load(node);
		architectureName = node["architecture"].as<std::string>("Lua 5.3");
		_architecture = ArchitectureRegistry::allocate(architectureName);
		if (!_architecture) {
				logE("Unknown architecture: \"" << architectureName << "\"");
				return;
		}
}

Architecture* CPU::architecture() {
		logC("CPU::architecture()");
		return _architecture;
}

int CPU::supportedComponents() {
		logC("CPU::supportedComponents()");
		return AurumConfig.cpuComponentCount[_tier];
}

double CPU::callBudget() {
		logC("CPU::callBudget()");
		return AurumConfig.callBudgets[_tier];
}