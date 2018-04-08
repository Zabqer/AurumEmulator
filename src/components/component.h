#ifndef __AURUM_COMPONENT_H__
#define __AURUM_COMPONENT_H__

#include <string>
#include <vector>

class Tiered {
		protected:
				int _tier;
		public:
				int tier();
};

class Component {
		protected:
				std::string _address;
				std::string _type;
				bool _internal;
		public:
				Component(std::string, bool = false);
				std::string type();
				std::string address();
};

#endif