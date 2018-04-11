#ifndef __AURUM_COMPONENT_H__
#define __AURUM_COMPONENT_H__

#include <string>
#include <vector>

class CallBudget {
		public:
				virtual double callBudget() = 0;
};

class Component {
		public:
				enum Slot {Unknown};
		protected:
				std::string _address;
				std::string _type;
				bool _internal;
				int _tier = 0;
				Slot _slot = Slot::Unknown;
		public:
				Component(std::string, bool = false);
				std::string type();
				std::string address();
				int tier();
				Slot slot();
};

#endif