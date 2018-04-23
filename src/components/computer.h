#ifndef __AURUM_COMPUTER_H__
#define __AURUM_COMPUTER_H__

#include <string>

#include "component.h"

class Computer: public Component {
		private:
				Machine* machine;
		public:
				static const std::string TYPE;
				Computer();
				void save(std::string&);
				void load(std::string, Machine*);
				DMETHOD(start);
				DMETHOD(stop);
				DMETHOD(isRunning);
				DMETHOD(beep);
				DMETHOD(getDeviceInfo);
				DMETHOD(getProgramLocations);
};

#endif