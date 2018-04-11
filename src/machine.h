#ifndef __AURUM_MACHINE_H__
#define __AURUM_MACHINE_H__

#include <stack>
#include <mutex>
#include <vector>
#include <queue>

#include "components/component.h"
#include "architectures/architecture.h"

class Machine: public Component {
		private:
				enum State {Stopped, Starting, Restarting, Stopping, Paused, SynchronizedCall, SynchronizedReturn, Yielded, Sleeping, Running};
				std::stack<State> state;
				std::recursive_mutex state_mutex;
				std::queue<void*> signals; //TODO: make Signal class
				std::vector<Component*> _components;
				size_t componentCount = 0;
				Architecture* architecture;
				size_t maxComponents = 0;
				size_t totalMemory = 0;
				size_t usedMemory = 0;
				size_t energyBuffer = 9999; //For testing
				time_t _uptime;
				time_t remainingPause = 0;
				time_t remainIdle = 0;
				double maxCallBudget;
				State switchTo(State);
				void onChanged();
				bool init();
				void crash(std::string);
				void beep(std::string);
		public:
				static const std::string TYPE;
				Machine();
				void save(std::string&);
				void load(std::string);
				void update();
				bool start();
				bool stop();
				void addComponent(Component*);
				std::vector<Component*>& components();
};

#endif