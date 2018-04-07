#ifndef __AURUM_COMPONENT_H__
#define __AURUM_COMPONENT_H__

#include <vector>
#include <queue>
#include <boost/any.hpp>

class Parameters {
		private:
				std::queue<boost::any> list;
		public:
				Parameters(std::initializer_list<boost::any> list_): list(list_) {};
				std::string getString();
				void setString(std::string);
};

class Component {
		protected:
				std::string address;
		public:
				virtual void save(Parameters) {};
				virtual void load(Parameters) {};
};

#endif