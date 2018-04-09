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
						if (!AurumConfig.ignorePower && energyBuffer < (AurumConfig.computerCost * AurumConfig.tickFrequency)) {
								crash("No energy");
								return false;
						} else if (architecture == NULL || maxComponents == 0) {
								crash("No CPU");
								beep("-");
								return false;
						} else if (componentCount > maxComponents) {
								crash("Component Overflow");
								beep("-..");
								return false;
						} else if (totalMemory == 0) {
								crash("No RAM");
								beep("- ");
								return false;
						} else if (!init()) {
								beep("--");
								return false;
						} else {
								switchTo(State::Running);
								_uptime = 0;
								return true;
						}
				case State::Paused:
						if (remainingPause > 0) {
								remainingPause = 0;
								return true;
						} else {
								return false;
						}
				case State::Stopping:
						switchTo(State::Restarting);
						return true;
				default:
						return false;
		}
}

void Machine::onChanged() {
		maxComponents = 0;
		totalMemory = 0;
		for (Component* component :_components) {
				if (component->type() == CPU::TYPE) {
						CPU* cpu = (CPU*) component;
						maxComponents += cpu->supportedComponents();
						architecture = cpu->architecture();
				} else if (component->type() == RAM::TYPE) {
						
				}
		}
		maxCallBudget = 1.0;
}

bool Machine::init() {
		
}

Machine::State Machine::switchTo(State newState) {
		logC("Machine::switchTo()");
		synchronized(state_mutex);
		State oldState = state.top();
		if (newState == State::Stopping || newState == State::Restarting) {
				/*while (!signals.empty()) {
						signals.pop()
				}*/
		}
		state.push(newState);
		if (newState == State::Yielded || newState == State::SynchronizedReturn) {
				remainIdle = 0;
				//Post to thread pool
		}
		return oldState;
}

void Machine::crash(std::string message) {
		logE(message);
		exit(0);
}

void Machine::beep(std::string pattern) {

}

void Machine::update() {
		logC("Machine::update()");
		
}