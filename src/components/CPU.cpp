#include "CPU.h"
#include "../log.h"
#include "../config.h"

const std::string CPU::TYPE = "cpu";

CPU::CPU(): Component(TYPE, true) {
		logC("CPU::CPU()");
}

void CPU::save(int& tier, std::string& architectureName) {
		logC("CPU::save()");
		tier = _tier;
		architectureName = _architectureName;
}

void CPU::load(int tier, std::string architectureName) {
		logC("CPU::load()");
		_tier = tier;
		_architectureName = architectureName;
		if (Architecture::byName.count(architectureName) > 0) {
				_architecture = Architecture::byName[architectureName]->create();
		} else {
				logE("Unknown architecture '" << architectureName << "'")
		}
}

Architecture* CPU::architecture() {
		logC("CPU::architecture()");
		return _architecture;
}

int CPU::supportedComponents() {
		logC("CPU::supportedComponents()");
		return AurumConfig.cpuComponentCount[_tier == -1 ? 3 : _tier - 1];
}