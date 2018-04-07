#include "CPU.h"

const std::string CPU::TYPE = "cpu";

CPU::CPU(): Component(TYPE) {}

Architecture* CPU::architecture() {
		return NULL;
}