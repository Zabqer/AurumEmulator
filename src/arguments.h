#ifndef __AURUM_ARGUMENTS_H__
#define __AURUM_ARGUMENTS_H__

#include <vector>
#include <any>
#include <string>

class Machine;
class Component;

class Null {};

class Context {
		private:
				Machine* machine;
		public:
				Context(Machine* machine);
				bool pause(double);
				bool tryChangeBuffer(double);
				void consumeCallBudget(double);
				std::vector<Component*> components();
				Component* componentByAddress(std::string);
				void signal(std::string, std::vector<std::any>);
};


class Arguments: public std::vector<std::any>  {
		private:
				static void typeError(unsigned int, std::any&, std::string);
				static std::string typeName(std::any&);
				void checkIndex(unsigned int, std::string);
				bool isDefined(unsigned int);
		public:
				Arguments(std::vector<std::any>);
				Arguments(std::initializer_list<std::any> args_);
		Arguments();
		std::any checkAny(unsigned int);
		bool checkBoolean(unsigned int);
		bool optBoolean(unsigned int, bool);
		int checkInteger(unsigned int);
		bool isString(unsigned int);
		std::string checkString(unsigned int);
		std::string optString(unsigned int, std::string);
};

#endif