#include "component.h"

int Tiered::tier() {
		return _tier;
}

Component::Component(std::string type, bool internal): _type(type), _internal(internal) {}

std::string Component::address() {
		return _address;
}

std::string Component::type() {
		return _type;
}