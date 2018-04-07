#include "machine.h"
#include "log.h"
#include "synchronized.h"
#include "components/CPU.h"

const std::string Machine::TYPE = "computer";

Machine::Machine(): Component(TYPE) {
		state.push(State::Stopped);
		components.push_back(this);
}

void Machine::load(Parameters param) {
		logC("Machine::load()");
		_address = param.getString();
}

void Machine::save(Parameters param) {
		logC("Machine::save()");
		param.setString(_address);
}

bool Machine::start() {
		logC("Machine::start()");
		synchronized(state_mutex);
		switch (state.top()) {
				case State::Stopped:
						onChanged();
		}
}

void Machine::onChanged() {
		maxComponents = 0;
		for (Component* component :components) {
				if (component->type() == CPU::TYPE) {
						CPU* cpu = (CPU*) component;
						maxComponents += 0;
						architecture = cpu->architecture();
				}
		}
}

void Machine::update() {
		logC("Machine::update()");
		
}