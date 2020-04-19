#ifndef __AURUM_MACHINE_H__
#define __AURUM_MACHINE_H__

#include <stack>
#include <mutex>
#include <vector>
#include <queue>
#include <optional>

#include <yaml-cpp/yaml.h>

#include "utils/thread_pool.h"
#include "components/component.h"
#include "architectures/architecture.h"

class Machine: public Component {
		public: 
				class Signal: public std::vector<std::any> {
					public:
							std::string name;
							Signal(std::string, std::vector<std::any>);
							Signal(std::string, std::initializer_list<std::any>);
				};
				static ThreadPool threadPool;
		private:
				enum State {Stopped, Starting, Restarting, Stopping, Paused, SynchronizedCall, SynchronizedReturn, Yielded, Sleeping, Running};
				std::stack<State> state;
				std::recursive_mutex state_mutex;
				std::recursive_mutex machine_mutex;
				std::queue<Signal> signals;
				std::recursive_mutex signals_mutex;
				std::vector<Component*> _components;
				size_t componentCount = 0;
				Component* tmpfs = NULL;
				Architecture* architecture = NULL;
				bool inSynchronizedCall = false;
				size_t maxComponents = 0;
				unsigned int _uptime = 0;
				unsigned int remainingPause = 0;
				bool crashed = false;
				unsigned int remainIdle = 0;
				clock_t cpuStart = 0;
				clock_t cpuTotal = 0;
				double callBudget;
				double maxCallBudget;
				State switchTo(State);
				void onChanged();
				bool init();
				void beep(std::string);
				bool isExecuting();
				bool tryClose();
				void run();
		public:
				Machine();
				~Machine();
				void save(YAML::Node&);
				void load(YAML::Node);
				void update();
				bool start();
				bool stop();
				bool pause(double);
				bool isRunning();
				bool tryChangeBuffer(double);
				void addComponent(Component*);
				size_t totalMemory = 0;
				size_t usedMemory = 0;
				double energyBuffer = 9999; //For testing
				std::vector<Component*>& components();
				Component* componentByAddress(std::string);
				std::optional<Signal> popSignal();
				bool signal(Signal);
				void consumeCallBudget(double);
				size_t getTotalMemory();
				void setTotalMemory(size_t);
				size_t getUsedMemory();
				void setUsedMemory(size_t);
				std::map<std::string, Method> methods(std::string);
				Arguments invoke(std::string, std::string, Arguments);
				std::string documentation(std::string, std::string);
				time_t uptime();
				void crash(std::string);
				bool isCrashed();
				void close();
				Component* tmpFS();
				DMETHOD(start);
				DMETHOD(stop);
				DMETHOD(isRunning);
				DMETHOD(beep);
				DMETHOD(getDeviceInfo);
				DMETHOD(getProgramLocations);
};

#endif
