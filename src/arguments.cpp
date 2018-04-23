#include "arguments.h"
#include "machine.h"

Context::Context(Machine* machine_): machine(machine_) {}

bool Context::pause(double seconds) {
		return machine->pause(seconds);
}

bool Context::tryChangeBuffer(double value) {
		return machine->tryChangeBuffer(value);
}

void Context::consumeCallBudget(double cost) {
		machine->consumeCallBudget(cost);
}

Arguments::Arguments(std::vector<std::any> args_): std::vector<std::any>(args_) {}

Arguments::Arguments(std::initializer_list<std::any> args_): std::vector<std::any>(args_) {}

Arguments::Arguments() {}

std::string Arguments::typeName(std::any& value) {
		if (value.type() == typeid(Null)) {
				return "nil";
		} else if (value.type() == typeid(double)) {
				return "number";
		}
		return value.type().name();
}

void Arguments::typeError(unsigned int index, std::any& value, std::string expect) {
		throw std::invalid_argument("bad argument #" + std::to_string(index + 1) + " (" + expect + " expected, got " + typeName(value) + ")");
}

void Arguments::checkIndex(unsigned int index, std::string name) {
		if (index >= size()) {
				throw std::invalid_argument("bad arguments #" + std::to_string(index + 1) + " (" + name + " expected, got no value)");
		}
}

bool Arguments::isDefined(unsigned int index) {
		return index < size() && at(index).type() != typeid(Null);
}

std::any Arguments::checkAny(unsigned int index) {
		checkIndex(index, "value");
		return at(index);
}

int Arguments::checkInteger(unsigned int index) {
		checkIndex(index, "number");
		if (at(index).type() != typeid(double)) {
				typeError(index, at(index), "number");
		}
		return (int) std::any_cast<double>(at(index));
}

std::string Arguments::checkString(unsigned int index) {
		checkIndex(index, "string");
		if (at(index).type() != typeid(std::string)) {
				typeError(index, at(index), "string");
		}
		return std::any_cast<std::string>(at(index));
}

std::string Arguments::optString(unsigned int index, std::string def) {
		return isDefined(index) ? checkString(index) : def;
}