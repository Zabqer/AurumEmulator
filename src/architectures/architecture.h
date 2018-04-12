#ifndef __AURUM_ARCHITECTURE_H__
#define __AURUM_ARCHITECTURE_H__

#include <map>

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
};

#endif