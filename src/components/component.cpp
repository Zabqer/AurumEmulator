#include "component.h"
#include "../log.h"

#include "../utils/uuid_generator.h"

#include "cpu.h"
#include "ram.h"
#include "eeprom.h"
#include "gpu.h"
#include "screen.h"
#include "filesystem.h"

void Callable::setMethod(std::string name, Method method) {
		_methods[name] = method;
}

std::map<std::string, Method> Callable::methods() {
		return _methods;
}

Component::Component(Machine* machine_, std::string type_, bool internal_): machine(machine_), _type(type_), _internal(internal_) {}

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

bool Component::internal() {
		return _internal;
}

void Component::save(YAML::Node& node) {
		logC("Component::save()");
		node["type"] = type();
		node["address"] = _address;
		node["tier"] = _tier;
}

void Component::load(YAML::Node node) {
		logC("Component:load()");
		_address = node["address"].as<std::string>(generateUUID());
		_tier = std::max(std::min(node["tier"].as<unsigned int>(0), maxTier), (unsigned int) 0);
}

void Component::update() {};

template<typename T>
Component* ComponentHolder<T>::allocate(Machine* machine) {
		return new T(machine);
}

std::map<std::string, ComponentHolderBase*>ComponentRegistry::components = {
		{"cpu", new ComponentHolder<CPU>},
		{"ram", new ComponentHolder<RAM>},
		{"eeprom", new ComponentHolder<EEPROM>},
		{"gpu", new ComponentHolder<GPU>},
		{"screen", new ComponentHolder<Screen>},
		{"keyboard", new ComponentHolder<Keyboard>},
		{"filesystem", new ComponentHolder<FileSystem>}
};

void ComponentRegistry::registerComponent(std::string type, ComponentHolderBase* holder) {
		components[type] = holder;
}

Component* ComponentRegistry::allocate(Machine* machine, std::string type) {
		if (components.count(type)) {
				return components[type]->allocate(machine);
		} else {
				return NULL;
		}
}
