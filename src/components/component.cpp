#include "component.h"

std::string Parameters::getString() {
		std::string result = boost::any_cast<std::string>(list.front());
		list.pop();
		return result;
}

void Parameters::setString(std::string str) {
		list.push(str);
}