#include "architecture.h"
#include "lua/lua53architecture.h"

template<typename T>
Architecture* ArchitectureHolder<T>::allocate() {
		return new T;
}

std::map<std::string, ArchitectureHolderBase*>ArchitectureRegistry::architectures = {
		{"Lua 5.3", new ArchitectureHolder<Lua53Architecture>}
};

void ArchitectureRegistry::registerArchitecture(std::string name, ArchitectureHolderBase* holder) {
		architectures[name] = holder;
}

Architecture* ArchitectureRegistry::allocate(std::string name) {
		if (architectures.count(name)) {
				return architectures[name]->allocate();
		} else {
				return NULL;
		}
}