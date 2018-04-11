#include "RAM.h"

#include "../log.h"
#include "../config.h"

const std::string RAM::TYPE = "ram";

RAM::RAM(): Component(TYPE, true) {
		logC("RAM::RAM()");
}

void RAM::save(int& tier) {
		logC("RAM::save()");
		tier = _tier;
}

void RAM::load(int tier) {
		logC("RAM::load()");
		_tier = tier;
}

size_t RAM::amount() {
		return AurumConfig.ramSizes[_tier];
}

double RAM::callBudget() {
		logC("RAM::callBudget()");
		return AurumConfig.callBudgets[(int) _tier / 2];
}