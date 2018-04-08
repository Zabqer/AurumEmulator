#include "RAM.h"

const std::string RAM::TYPE = "ram";

RAM::RAM(): Component(TYPE, true) {}

void RAM::save(int& tier) {
		tier = _tier;
}

void RAM::load(int tier) {
		_tier = tier;
}