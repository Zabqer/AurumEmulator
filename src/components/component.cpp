#include "component.h"

Component::Component(std::string type): _type(type) {}

std::string Component::address() {
		return _address;
}

std::string Component::type() {
		return _type;
}

std::string Parameters::getString() {
		std::string result = std::any_cast<std::string>(list.front());
		list.pop();
		return result;
}

void Parameters::setString(std::string str) {
		list.push(str);
}