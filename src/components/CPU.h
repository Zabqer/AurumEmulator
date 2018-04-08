#ifndef __AURUM_CPU_H__
#define __AURUM_CPU_H__

#include <string>

#include "component.h"
#include "../architectures/architecture.h"

class CPU: public Component, Tiered {
		private:
				Architecture* _architecture;
				std::string _architectureName;
		public:
				static const std::string TYPE;
				CPU();
				void save(int&, std::string&);
				void load(int, std::string);
				Architecture* architecture();
};

#endif