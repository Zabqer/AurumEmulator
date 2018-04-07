#ifndef __AURUM_CPU_H__
#define __AURUM_CPU_H__

#include <string>

#include "component.h"
#include "../architectures/architecture.h"

class CPU: public Component {
		public:
				static const std::string TYPE;
				CPU();
				Architecture* architecture();
};

#endif