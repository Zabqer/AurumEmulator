#ifndef __AURUM_MACHINE_H__
#define __AURUM_MACHINE_H__

#include "components/component.h"

class Machine: public Component {
		public:
				void save(Parameters) override;
				void load(Parameters) override;
				void update();
};

#endif