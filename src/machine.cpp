#include "machine.h"
#include "log.h"
#include "synchronized.h"
#include "components/CPU.h"
#include "components/RAM.h"

const std::string Machine::TYPE = "computer";

Machine::Machine(): Component(TYPE) {
		state.push(State::Stopped);
		_components.push_back(this);
}

void Machine::load(std::string address_) {
		logC("Machine::load()");
		_address = address_;
}

void Machine::save(std::string& address_) {
		logC("Machine::save()");
		address_ = _address;
}

void Machine::addComponent(Component* component) {
		_components.push_back(component);
}

std::vector<Component*>& Machine::components() {
		return _components;
}

bool Machine::start() {
		logC("Machine::start()");
		synchronized(state_mutex);
		switch (state.top()) {
				case State::Stopped:
						onChanged();
						//if (!AurumConfig.ignorePower)
		}
}

void Machine::onChanged() {
		maxComponents = 0;
		totalMemory = 0;
		for (Component* component :_components) {
				if (component->type() == CPU::TYPE) {
						CPU* cpu = (CPU*) component;
						//maxComponents += cpu->supportedComponents();
						//architecture = cpu->architecture();
				} else if (component->type() == RAM::TYPE) {
						

				}
		}
		maxCallBudget = 1.0;
}

void Machine::update() {
		logC("Machine::update()");
		
}