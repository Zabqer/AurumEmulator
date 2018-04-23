#include "computer.h"
#include "../log.h"
#include "../config.h"

const std::string Computer::TYPE = "computer";

Computer::Computer(): Component(TYPE) {
		logC("Computer::Computer()");
		setMethod("start", {callback: wrapMethod(start), doc: "function():boolean -- Starts the computer. Returns true if the state changed."});
		setMethod("stop", {callback: wrapMethod(stop), doc: "function():boolean -- Stops the computer. Returns true if the state changed."});
		setMethod("isRunning", {callback: wrapMethod(isRunning), doc: "function():boolean -- Returns whether the computer is running.", direct: true});
		setMethod("beep", {callback: wrapMethod(beep), doc: ""});
		setMethod("getDeviceInfo", {callback: wrapMethod(getDeviceInfo), doc: "function():table -- Collect information on all connected devices.", true});
		setMethod("getProgramLocations", {callback: wrapMethod(getProgramLocations), doc: "function():table -- Returns a map of program name to disk label for known programs."});
}

void Computer::save(std::string& address_) {
		logC("Computer::save()");
		address_ = _address;
}

void Computer::load(std::string address_, Machine* machine_) {
		logC("Computer::load()");
		_address = address_;
		machine = machine_;
}

METHOD(Computer, start) {
		logC("Computer::start()");
		return {machine->start()};
}

METHOD(Computer, stop) {
		logC("Computer::stop()");
		return {machine->stop()};
}

METHOD(Computer, isRunning) {
		logC("Computer::isRunning()");
		return {machine->isRunning()};
}

METHOD(Computer, beep) {
		logC("Computer::beep()");
		logW("Called not implemented function: Computer::beep()");
		return {Null()};
}

METHOD(Computer, getDeviceInfo) {
		logC("Computer::getDeviceInfo()");
		logW("Called not implemented function: Computer::getDeviceInfo()");
		return {Null()};
}

METHOD(Computer, getProgramLocations) {
		logC("Computer::getProgramLocations()");
		logW("Called not implemented function: Computer::getProgramLocations()");
		return {Null()};
}