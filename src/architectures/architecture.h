#ifndef __AURUM_ARCHITECTURE_H__
#define __AURUM_ARCHITECTURE_H__

#include <map>

#include "execution_result.h"

class Architecture;
class Machine;

class Architecture {
		public:
				virtual bool initialize(Machine*) = 0;
				virtual ExecutionResult runThreaded(bool) = 0;
				virtual void runSynchronized() = 0;
};

class ArchitectureHolderBase {
		public:
				virtual Architecture* allocate() = 0;
};

template<typename T>
class ArchitectureHolder: public ArchitectureHolderBase {
		public:
				Architecture* allocate();
};

class ArchitectureRegistry {
		private:
				static std::map<std::string, ArchitectureHolderBase*> architectures;
		public:
				static void registerArchitecture(std::string, ArchitectureHolderBase*);
				static Architecture* allocate(std::string);
};

#endif