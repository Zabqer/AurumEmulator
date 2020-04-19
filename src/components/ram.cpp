#include "ram.h"

#include "../log.h"
#include "../config.h"

const std::string RAM::TYPE = "ram";

RAM::RAM(Machine* machine): Component(machine, TYPE, true) {
		logC("RAM::RAM()");
		maxTier = 5;
}

size_t RAM::amount() {
		logC("RAM::amount()");
		return AurumConfig.ramSizes[_tier];
}

double RAM::callBudget() {
		logC("RAM::callBudget()");
		return AurumConfig.callBudgets[((int) (_tier + 1) / 2) - 1];
}