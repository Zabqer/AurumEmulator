#ifndef __AURUM_ARCHITECTURE_H__
#define __AURUM_ARCHITECTURE_H__

#include <map>

#include "execution_result.h"

class Architecture;
class Machine;

class ArchitectureHolder {
		public:
				virtual Architecture* create() = 0;
};

template<typename T>
class ArchitectureHolderImpl: public ArchitectureHolder {
		public:
				Architecture* create() override {
						return new T;
				}
};

class Architecture {
		public:
				static std::map<std::string, ArchitectureHolder*> byName;
				virtual bool initialize(Machine*) = 0;
				virtual ExecutionResult runThreaded(bool) = 0;
				virtual void runSynchronized() = 0;
};

#endif