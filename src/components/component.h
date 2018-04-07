#ifndef __AURUM_COMPONENT_H__
#define __AURUM_COMPONENT_H__

#include <string>
#include <vector>
#include <queue>
#include <any>

class Parameters {
		private:
				std::queue<std::any> list;
		public:
				Parameters(std::initializer_list<std::any> list_): list(list_) {};
				std::string getString();
				void setString(std::string);
};

class Component {
		protected:
				std::string _address;
				std::string _type;
		public:
				Component(std::string);
				std::string type();
				std::string address();
				virtual void save(Parameters) {};
				virtual void load(Parameters) {};
};

#endif