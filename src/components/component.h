#ifndef __AURUM_COMPONENT_H__
#define __AURUM_COMPONENT_H__

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <limits>

#include <yaml-cpp/yaml.h>

#include "../arguments.h"

class Machine;

class limit_reached {};

class CallBudget {
		public:
				virtual double callBudget() = 0;
};

struct Method {
		typedef  std::function<Arguments(Context, Arguments)> Callback;
		Callback callback;
		std::string doc = "";
		bool direct = false;
		int limit = std::numeric_limits<int>::max();
		bool getter = false;
		bool setter = false;
};

class Callable {
		private:
				std::map<std::string, Method> _methods;
		protected:
				void setMethod(std::string, Method);
		public:
				std::map<std::string, Method> methods();
};

class Userdata: public Callable {
		public:
				virtual ~Userdata() = default;
};

#define DMETHOD(name) Arguments name(Context, Arguments)

#define METHOD(class, name) Arguments class::name(Context context, Arguments args)

#define wrapMethod(name) [this](Context context, Arguments args) {return name(context, args);}

class Component: public Callable {
		public:
				enum Slot {Unknown};
		protected:
				std::string _address;
				std::string _type;
				bool _internal;
				unsigned int _tier = 0;
				unsigned int maxTier = 0;
				Slot _slot = Slot::Unknown;
				Machine* machine = NULL;
		public:
				Component(Machine*, std::string, bool = false);
				std::string type();
				std::string address();
				int tier();
				Slot slot();
				bool internal();
				virtual void save(YAML::Node&);
				virtual void load(YAML::Node);
				virtual void update();	
};

class ComponentHolderBase {
		public:
				virtual Component* allocate(Machine*) = 0;
};

template<typename T>
class ComponentHolder: public ComponentHolderBase {
		public:
				Component* allocate(Machine*);
};

class ComponentRegistry {
		private:
				static std::map<std::string, ComponentHolderBase*> components;
		public:
				static void registerComponent(std::string, ComponentHolderBase*);
				static Component* allocate(Machine*, std::string);
};

#endif