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
						} else if (!architecture || maxComponents == 0) {
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
								switchTo(State::Starting);
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
		std::vector<double> callBudgets;
		for (Component* component :_components) {
				if (component->type() == CPU::TYPE) {
						CPU* cpu = (CPU*) component;
						maxComponents += cpu->supportedComponents();
						architecture = cpu->architecture();
						callBudgets.push_back(cpu->callBudget());
				} else if (component->type() == RAM::TYPE) {
						RAM* ram = (RAM*) component;
						totalMemory += ram->amount() * 1024;
						callBudgets.push_back(ram->callBudget());
				}
		}
		if (callBudgets.empty()) {
				maxCallBudget = 1.0;
		} else {
				double sum = 0;
				for (double callBudget :callBudgets) {
						sum += callBudget;
				}
				maxCallBudget = sum / callBudgets.size();
		}
}

bool Machine::init() {
		logC("Machine::init()");
		if (!architecture) return false;
		while (!signals.empty()) {
				signals.pop();
		}
		try {
				return architecture->initialize(this);
		} catch (std::exception ex) {
				logE("Error while architecture initialization: " << ex.what());
				return false;
		}
}

Machine::State Machine::switchTo(State newState) {
		logC("Machine::switchTo()");
		synchronized(state_mutex);
		State oldState = state.top();
		if (newState == State::Stopping || newState == State::Restarting) {
				while (!signals.empty()) {
						signals.pop();
				}
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
		logC("Machine::beep()")};

void Machine::update() {
		logC("Machine::update()");
		logD(energyBuffer)
		if (componentCount > maxComponents) {
				crash("Component Overflow");
				return;
		}
		_uptime += 1;
		if (remainIdle > 0) {
				remainIdle -= 1;
		}
		callBudget = maxCallBudget;
		if (_uptime % AurumConfig.tickFrequency == 0) {
				synchronized(state_mutex);
				switch(state.top()) {
						case State::Paused:
						case State::Restarting:
						case State::Stopping:
						case State::Stopped:
								break;
						case State::Sleeping:
								if (remainIdle > 0 && signals.empty()) {
										if (!tryChangeBuffer(-AurumConfig.computerCost * AurumConfig.sleepCostFactor)) {
												crash("No Energy");
												return;
										}
										break;
								}
						default:
							if (!tryChangeBuffer(-AurumConfig.computerCost))	{
									crash("No Energy");
									return;
							}
				}
		}
		{
				synchronized(state_mutex);
				switch(state.top()) {
						case State::Starting:
								switchTo(State::Yielded);
								break;
						case State::Restarting:
								logE("Unhandled: Restarting");
								break;
						//case State::Sleeping:
								
				}
		}
}

bool Machine::tryChangeBuffer(size_t value) {
		if (energyBuffer + value < 0) {
				return false;
		} else {
				energyBuffer += value;
				return true;
		}
}

size_t Machine::getTotalMemory() {
		return totalMemory;
}

void Machine::setTotalMemory(size_t memory) {
		totalMemory = memory;
}

size_t Machine::getUsedMemory() {
		return usedMemory;
}

void Machine::setUsedMemory(size_t memory) {
		usedMemory = memory;
}