#include "aurum_debug.h"

#include "../log.h"
#include "../config.h"

const std::string AurumDebug::TYPE = "aurum_debug";

AurumDebug::AurumDebug(): Component(TYPE) {
		logC("AurumDebug::AurumDebug()");
		setMethod("printDebug", {callback: wrapMethod(printDebug)});
}

void AurumDebug::save(std::string& address_) {
		logC("AurumDebug::save()");
		address_ = _address;
}

void AurumDebug::load(std::string address_) {
		logC("AurumDebug::load()");
		_address = address_;
}

METHOD(AurumDebug, printDebug) {
		logC("AurumDebug::printDebug()");
		std::cout << "[AurumDebug]: ";
		unsigned int i = 0;
		for (std::any& value :args) {
				if (value.type() == typeid(Null)) {
						std::cout << "nil";
				} else if (value.type() == typeid(bool)) {
						std::cout << (std::any_cast<bool>(value) ? "true" : "false");
				} else if (value.type() == typeid(double)) {
						std::cout << std::any_cast<double>(value);
				} else if (value.type() == typeid(std::string)) {
						std::cout << std::any_cast<std::string>(value);
				}
				if (i++ < args.size()) {
						std::cout << "	";
				}
		}
		std::cout << std::endl;
		return {Null()};
}
