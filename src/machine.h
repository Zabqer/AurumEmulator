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
				std::vector<Component*> components;
				Architecture* architecture;
				size_t maxComponents = 0;
				void onChanged();
		public:
				static const std::string TYPE;
				Machine();
				void save(Parameters) override;
				void load(Parameters) override;
				void update();
				bool start();
				bool stop();
};

#endif