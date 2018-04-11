#include "CPU.h"
#include "../log.h"
#include "../config.h"

const std::string CPU::TYPE = "cpu";

CPU::CPU(): Component(TYPE, true) {
		logC("CPU::CPU()");
}

void CPU::save(int& tier_, std::string& architectureName_) {
		logC("CPU::save()");
		tier_ = _tier;
		architectureName_ = _architectureName;
}

void CPU::load(int tier_, std::string architectureName_) {
		logC("CPU::load()");
		_tier = tier_;
		_architectureName = architectureName_;
		if (Architecture::byName.count(_architectureName) > 0) {
				_architecture = Architecture::byName[_architectureName]->create();
		} else {
				logE("Unknown architecture '" << _architectureName << "'")
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