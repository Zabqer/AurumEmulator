#ifndef __AURUM_MACHINE_H__
#define __AURUM_MACHINE_H__

#include <stack>
#include <mutex>
#include <vector>

#include "components/component.h"
#include "architectures/architecture.h"

class Machine: public Component {
		private:
				enum State {Stopped, Starting, Restarting, Stopping, Paused, SynchronizedCall, SynchronizedReturn, Yielded, Sleeping, Running};
				std::stack<State> state;
				std::recursive_mutex state_mutex;
				std::vector<Component*> _components;
				Architecture* architecture;
				size_t maxComponents = 0;
				size_t totalMemory = 0;
				size_t usedMmory = 0;
				double maxCallBudget;
				void onChanged();
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