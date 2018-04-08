#ifndef __AURUM_RAM_H__
#define __AURUM_RAM_H__

#include <string>

#include "component.h"

class RAM: public Component, Tiered {
		public:
				static const std::string TYPE;
				RAM();
				void save(int&);
				void load(int);
};

#endif