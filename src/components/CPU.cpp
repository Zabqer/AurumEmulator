#include "CPU.h"

const std::string CPU::TYPE = "cpu";

CPU::CPU(): Component(TYPE, true) {}

void CPU::save(int& tier, std::string& architectureName) {
		tier = _tier;
		architectureName = _architectureName;
}

void CPU::load(int tier, std::string architectureName) {
		_tier = tier;
		_architectureName = architectureName;
}

Architecture* CPU::architecture() {
		return _architecture;
}