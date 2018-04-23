#include "machine.h"
#include "log.h"
#include "synchronized.h"
#include "components/computer.h"
#include "components/CPU.h"
#include "components/RAM.h"

#include <thread>
#include <boost/asio/post.hpp>

//TODO: config `threads` value
boost::asio::thread_pool Machine::threadPool(1);

Machine::Signal::Signal(std::string name_, std::vector<std::any> args_): name(name_), std::vector<std::any>(args_) {}

Machine::Machine() {
		logC("Machine::Machine()");
		state.push(State::Stopped);
}

void Machine::load(std::string address_) {
		logC("Machine::load()");
		_address = address_;
		Computer* computer = new Computer;
		computer->load(_address, this);
		_components.push_back(computer);
}

void Machine::save(std::string& address_) {
		logC("Machine::save()");
		address_ = _address;
}

void Machine::addComponent(Component* component) {
		logC("Machine::addComponent()");
		_components.push_back(component);
}

std::vector<Component*>& Machine::components() {
		logC("Machine::components()");
		return _components;
}

bool Machine::start() {
		logC("Machine::start()");
		synchronized(state_mutex);
		switch (state.top()) {
				case State::Stopped:
						onChanged();
						if (!AurumConfig.ignorePower && energyBuffer < (AurumConfig.computerCost/* * AurumConfig.tickFrequency*/)) {
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

bool Machine::stop() {
		logC("Machine::stop()");
		synchronized(state_mutex);
		switch (state.top()) {
				case State::Stopped:
				case State::Stopping:
						return false;
				default:
						state.push(State::Stopping);
						return true;
		}
}

void Machine::onChanged() {
		logC("Machine::onChanged()");
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

bool Machine::pause(double seconds) {
		unsigned int ticksToPause = std::max(0, (int) seconds * 20);
		bool shouldPause;
		{
				synchronized(state_mutex);
				switch (state.top()) {
						case State::Stopping:
						case State::Stopped:
								shouldPause = false;
								break;
						case State::Paused:
								remainingPause = ticksToPause > remainingPause;
								break;
						default:
								shouldPause = true;
				}
		}
		if (shouldPause) {
				synchronized(machine_mutex);
				synchronized(state_mutex);
				if (state.top() != State::Paused) {
						state.push(State::Paused);
				}
				remainingPause = ticksToPause;
				return true;
		}
		return false;
}

bool Machine::isRunning() {
		logC("Machine::isRunning()");		
		synchronized(state_mutex);
		return state.top() != State::Stopped && state.top() != State::Stopping;
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
				while (!state.empty()) {
						state.pop();
				}
		}
		state.push(newState);
		if (newState == State::Yielded || newState == State::SynchronizedReturn) {
				remainIdle = 0;
				boost::asio::post(threadPool, [this] {
						std::this_thread::sleep_for(std::chrono::milliseconds((int) AurumConfig.startupDelay * 1000));
						this->run();
				});
		}
		return oldState;
}

void Machine::run() {
		logC("Machine::run()");
		synchronized(machine_mutex);
		bool isSynchronizedReturn;
		{
				synchronized(state_mutex);
				if (state.top() != State::Yielded && state.top() != State::SynchronizedReturn) {
						isSynchronizedReturn = false;
				} else if (false) { /* Paused? */
						state.push(State::Paused);
						isSynchronizedReturn = false;
				} else {
						isSynchronizedReturn = switchTo(State::Running) == State::SynchronizedReturn;
				}
		}
		cpuStart = clock();
		try {
				ExecutionResult result = architecture->runThreaded(isSynchronizedReturn);
				synchronized(state_mutex);
				switch(state.top()) {
						case State::Running:
								switch(result.index()) {
										case 0: {// ExecutionResultSleep
												synchronized(signals_mutex);
												int ticks = (std::get<0>(result)).ticks;
												if (signals.empty() && ticks > 0) {
														switchTo(State::Sleeping);
														remainIdle = ticks;
												} else {
														switchTo(State::Yielded);
												}
												break;
										}
										case 1: // ExecutionResultSynchronizedCall
												switchTo(State::SynchronizedCall);
												break;
										case 2: // ExecutionResultShutdown
												if ((std::get<2>(result)).reboot) {
														switchTo(State::Restarting);
												} else {
														switchTo(State::Stopping);
												}
												break;
										case 3: // ExecutionResultError
													beep("--");
													crash((std::get<3>(result).message));
													break;
								}
								break;
						case State::Paused:
								state.pop();
								state.pop();
								switch (result.index()) {
										case 0: // ExecutionResultSleep
												remainIdle = (std::get<0>(result)).ticks;
												break;
										case 1: // ExecutionResultSynchronizedCall
												state.push(State::SynchronizedCall);
												break;
										case 2: // ExecutionResultShutdown
												if ((std::get<2>(result)).reboot) {
														state.push(State::Restarting);
												} else {
														state.push(State::Stopping);
												}
												break;
										case 3: // ExecutionResultError
												crash((std::get<3>(result)).message);
												break;
								}
								state.push(State::Paused);
								break;
						case State::Stopping:
								while (!state.empty()) {
										state.pop();
								}
								state.push(State::Stopping);
								break;
						case State::Restarting:
								break;
						default:
								break;
				}
		} catch (std::exception ex) {
				//TODO: handle exception
				throw ex;
		}
		cpuTotal += clock() - cpuStart;
}

void Machine::crash(std::string message) {
		logE(message);
		exit(0);
}

void Machine::beep(std::string pattern) {
		logC("Machine::beep()");
};

void Machine::update() {
		logC("Machine::update()");
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
								close();
								//erase tmp
								start();
								break;
						case State::Sleeping:
								if (remainIdle <= 0 || !signals.empty()) {
										switchTo(State::Yielded);
								}
								break;
						case State::Paused:
								if (remainingPause > 0) {
										remainingPause -= 1;
								} else {
										state.pop();
										switchTo(state.top());
								}
								break;
						case State::SynchronizedCall:
								switchTo(State::Running);
								try {
										inSynchronizedCall = true;
										architecture->runSynchronized();
										inSynchronizedCall = false;
										switch(state.top()) {
												case State::Running:
														switchTo(State::SynchronizedReturn);
														break;
												case State::Paused:
														state.pop();
														state.pop();
														state.push(State::SynchronizedReturn);
														state.push(State::Paused);
														break;
												case State::Stopping:
														while (!state.empty()) {
																state.pop();
														}
														state.push(State::Stopping);
														break;
												default:
														break;
										}
								} catch (std::exception ex) {
								//TODO handle exception
										throw ex;
								}
								inSynchronizedCall = false;
						default:
								break;
				}
		}
		{
				synchronized(state_mutex);
				switch(state.top()) {
						case State::Stopping: {
								synchronized(machine_mutex);
								tryClose();
						}
						default:
								break;
				}
		}
}

bool Machine::tryChangeBuffer(double value) {
		logC("Machine::tryChangeBuffer()");
		if (energyBuffer + value < 0) {
				return false;
		} else {
				energyBuffer += value;
				return true;
		}
}

bool Machine::isExecuting() {
		logC("Machine::isExecuting()");
		synchronized(state_mutex);
		for (std::stack<State> cpy = state; !cpy.empty(); cpy.pop()) {
				if (cpy.top() == State::Running) {
						return true;
				}
		}
		return false;
}

bool Machine::tryClose() {
		logC("Machine::tryClose()");
		if (isExecuting()) {
				return false;
		} else {
				close();
				return true;
		}
}

void Machine::close() {
		logC("Machine::close()");
		synchronized(state_mutex);
		if (state.empty() || state.top() != State::Stopped) {
				synchronized(machine_mutex);
				while (!state.empty()) {
						state.pop();
				}
				state.push(State::Stopped);
				while (!signals.empty()) {
						signals.pop();
				}
				_uptime = 0;
				cpuTotal = 0;
				cpuStart = 0;
				remainIdle = 0;
		}
		exit(0);
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

std::optional<Machine::Signal> Machine::popSignal() {

}

bool Machine::signal(Signal signal) {
		logC("Machine::signal()");
		synchronized(state_mutex);
		switch (state.top()) {
				case State::Stopped:
				case State::Stopping:
						return false;
				default: {
						synchronized(signals_mutex);
						if (signals.size() >= 256) {
								return false;
						} else {
								signals.push(signal);
								return true;
						}
				}
		}
}

void Machine::consumeCallBudget(double cost) {
		logC("Machine::consumeCallBudget()");
		if (architecture != NULL && !inSynchronizedCall) {
				double claimedCost = std::max(0.001, cost);
				if (claimedCost > callBudget) {
						throw limit_reached();
				}
				callBudget -= claimedCost;
		}
}

Component* Machine::componentByAddress(std::string address) {
		for (Component* component :_components) {
				if (!component->internal() && component->address() == address) {
						return component;
				}
		}
		throw std::invalid_argument("no such component");
}



std::map<std::string, Method> Machine::methods(std::string address) {
		logC("Machine::methods()");
		return componentByAddress(address)->methods();
}

Arguments Machine::invoke(std::string address, std::string methodn, Arguments args) {
		logC("Machine::invoke()");
		Component* component = componentByAddress(address);
		if (!component->methods().count(methodn)) {
				throw std::invalid_argument("no such method");
		}
		Method method = component->methods()[methodn]; 
		if (method.direct) {
				consumeCallBudget(1 / method.limit);
		}
		logD("Invoking: `" << methodn << "`")
		return method.callback(Context(this), args);
}

std::string Machine::documentation(std::string address, std::string method) {
		Component* component = componentByAddress(address);
		if (!component->methods().count(method)) {
				throw std::invalid_argument("no such method");
		}
		return component->methods()[method].doc;
}

time_t Machine::uptime() {
		return _uptime;
}

Component* Machine::tmpFS() {
		return tmpfs;
}

std::string Machine::address() {
		return _address;
}