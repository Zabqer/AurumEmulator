#include "component.h"

Component::Component(std::string type_, bool internal_): _type(type_), _internal(internal_) {}

std::string Component::address() {
		return _address;
}

std::string Component::type() {
		return _type;
}

int Component::tier() {
		return _tier;
}

Component::Slot Component::slot() {
		return _slot;
}