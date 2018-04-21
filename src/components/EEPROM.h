#ifndef __AURUM_EEPROM_H__
#define __AURUM_EEPROM_H__

#include <string>

#include "component.h"

class EEPROM: public Component {
		private:
				std::string basePath;
				std::string label;
				bool ro;
				std::string code;
				std::string data;
		public:
				static const std::string TYPE;
				EEPROM();
				void save(std::string&, std::string&, bool&);
				void load(std::string, std::string, bool);
				DMETHOD(get);
				DMETHOD(set);
				DMETHOD(getLabel);
				DMETHOD(setLabel);
				DMETHOD(getSize);
				DMETHOD(getChecksum);
				DMETHOD(makeReadonly);
				DMETHOD(getDataSize);
				DMETHOD(getData);
				DMETHOD(setData);
};

#endif