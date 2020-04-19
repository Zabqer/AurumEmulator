#ifndef __AURUM_RAM_H__
#define __AURUM_RAM_H__

#include <string>

#include <yaml-cpp/yaml.h>

#include "component.h"

class RAM: public Component, CallBudget {
		public:
				static const std::string TYPE;
				RAM(Machine*);
				size_t amount();
				double callBudget() override;
};

#endif