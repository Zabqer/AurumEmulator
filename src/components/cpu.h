#ifndef __AURUM_CPU_H__
#define __AURUM_CPU_H__

#include <string>

#include <yaml-cpp/yaml.h>

#include "component.h"
#include "../architectures/architecture.h"

class CPU: public Component, CallBudget {
		private:
				Architecture* _architecture;
				std::string architectureName;
		public:
				static const std::string TYPE;
				CPU(Machine*);
				void save(YAML::Node&) override;
				void load(YAML::Node) override;
				Architecture* architecture();
				int supportedComponents();
				double callBudget() override;
};

#endif