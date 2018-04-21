#ifndef __AURUM_MACHINE_H__
#define __AURUM_MACHINE_H__

#include <stack>
#include <mutex>
#include <vector>
#include <queue>

#include <boost/asio/thread_pool.hpp>

#include "components/component.h"
#include "architectures/architecture.h"

class Machine {
		private:
				std::string _address;
				enum State {Stopped, Starting, Restarting, Stopping, Paused, SynchronizedCall, SynchronizedReturn, Yielded, Sleeping, Running};
				std::stack<State> state;
				static boost::asio::thread_pool threadPool;
				std::recursive_mutex state_mutex;
				std::recursive_mutex machine_mutex;
				std::queue<void*> signals; //TODO: make Signal class
				std::recursive_mutex signals_mutex;
				std::vector<Component*> _components;
				size_t componentCount = 0;
				Component* tmpfs;
				Architecture* architecture;
				bool inSynchronizedCall = false;
				size_t maxComponents = 0;
				size_t totalMemory = 0;
				size_t usedMemory = 0;
				double energyBuffer = 9999; //For testing
				unsigned int _uptime;
				unsigned int remainingPause = 0;
				unsigned int remainIdle = 0;
				clock_t cpuStart = 0;
				clock_t cpuTotal = 0;
				double callBudget;
				double maxCallBudget;
				State switchTo(State);
				void onChanged();
				bool init();
				void crash(std::string);
				void beep(std::string);
				bool isExecuting();
				void close();
				bool tryClose();
				void run();
				Component* componentByAddress(std::string);
		public:
				Machine();
				void save(std::string&);
				void load(std::string);
				void update();
				bool start();
				bool stop();
				bool pause(double);
				bool tryChangeBuffer(double);
				void addComponent(Component*);
				std::vector<Component*>& components();
				void consumeCallBudget(double);
				size_t getTotalMemory();
				void setTotalMemory(size_t);
				size_t getUsedMemory();
				void setUsedMemory(size_t);
				std::map<std::string, Method> methods(std::string);
				Arguments invoke(std::string, std::string, Arguments);
				std::string documentation(std::string, std::string);
				time_t uptime();
				Component* tmpFS();
				std::string address();
};

#endif