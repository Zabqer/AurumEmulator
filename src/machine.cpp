#include "machine.h"
#include "log.h"

void Machine::load(Parameters param) {
		logC("Machine::load()");
		address = param.getString();
}

void Machine::save(Parameters param) {
		logC("Machine::save()");
		param.setString(address);
}

void Machine::update() {
		logC("Machine::update()");
		
}