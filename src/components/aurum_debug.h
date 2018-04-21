#ifndef __AURUM_AURUMDEBUG_H__
#define __AURUM_AURUMDEBUG_H__

#include <string>

#include "component.h"

class AurumDebug: public Component {
		public:
				static const std::string TYPE;
				AurumDebug();
				void save(std::string&);
				void load(std::string);
				DMETHOD(printDebug);
};

#endif