#include "eeprom.h"

#include <fstream>
#include <sstream>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../resources/bios_lua.h"
#include "../log.h"
#include "../config.h"
#include "../crypto/crc32.h"

const std::string EEPROM::TYPE = "eeprom";

EEPROM::EEPROM(Machine* machine): Component(machine, TYPE) {
		logC("EEPROM::EEPROM()");
		setMethod("get", {callback: wrapMethod(get), doc: "function():string -- Get the currently stored byte array.", direct: true});
		setMethod("set", {callback: wrapMethod(set), doc: "function(data:string) -- Overwrite the currently stored byte array."});
		setMethod("getLabel", {callback: wrapMethod(getLabel), doc: "function():string -- Get the label of the EEPROM." , direct: true});
		setMethod("setLabel", {callback: wrapMethod(setLabel), doc: "function(data:string):string -- Set the label of the EEPROM."});
		setMethod("getSize", {callback: wrapMethod(getSize), doc: "function():number -- Get the storage capacity of this EEPROM.", direct: true});
		setMethod("getChecksum", {callback: wrapMethod(getChecksum), doc: "function():string -- Get the checksum of the data on this EEPROM.", direct: true});
		setMethod("makeReadonly", {callback: wrapMethod(makeReadonly), doc: "function(checksum:string):boolean -- Make this EEPROM readonly if it isn't already. This process cannot be reversed!", direct: true});
		setMethod("getDataSize", {callback: wrapMethod(getDataSize), doc: "function():number -- Get the storage capacity of this EEPROM.", direct: true});
		setMethod("getData", {callback: wrapMethod(getData), doc: "function():string -- Get the currently stored byte array.", direct: true});
		setMethod("setData", {callback: wrapMethod(setData), doc: "function(data:string) -- Overwrite the currently stored byte array."});
}

void EEPROM::save(YAML::Node& node) {
		logC("EEPROM::save()");
		Component::save(node);
		node["label"] = label;
		node["read-only"] = ro;
		if (access(basePath.c_str(), F_OK) == -1) {
				mkdir(basePath.c_str(), 777);
		}
		std::ofstream oc(basePath + "code");
		if (oc.is_open()) {
				oc << code;
				oc.close();
		}
		std::ofstream od(basePath + "data");
		if (od.is_open()) {
				od << data;
				od.close();
		}
}

void EEPROM::load(YAML::Node node) {
		logC("EEPROM::load()");
		Component::load(node);
		label = node["label"].as<std::string>("EEPROM");
		ro = node["read-only"].as<bool>(false);
		basePath = AurumConfig.envPath + "/" + _address + "/";
		std::ifstream ic(basePath + "code");
		if (ic.is_open()) {
				std::stringstream iss;
				iss << ic.rdbuf();
				code = iss.str();
				ic.close();
		} else {
				code = BIOS_LUA;
		}
		std::ifstream id(basePath + "data");
		if (id.is_open()) {
				std::stringstream iss;
				iss << id.rdbuf();
				data = iss.str();
				id.close();
		} else {
				data = "";
		}
}

METHOD(EEPROM, get) {
		logC("EEPROM::get()");
		return {code};
}

METHOD(EEPROM, set) {
		logC("EEPROM::set()");
		if (ro) {
				return {Null(), "storage is readonly"};
		}
		if (!context.tryChangeBuffer(-AurumConfig.eepromWriteCost)) {
				return {Null(), "not enough energy"};
		}
		std::string newCode = args.optString(0, "");
		if (newCode.length() > AurumConfig.eepromSize) {
				throw std::invalid_argument("not enough space");
		}
		code = newCode;
		context.pause(2);
		AurumConfigSave();
		return {Null()};
}

METHOD(EEPROM, getLabel) {
		logC("EEPROM::getLabel()");
		return {label};
}

METHOD(EEPROM, setLabel) {
		logC("EEPROM::setLabel()");
		if (ro) {
				return {Null(), "storage is readonly"};
		}
		label = args.optString(0, "EEPROM").substr(0, 24);
		if (label.length() == 0) {
				label = "EEPROM";
		}
		AurumConfigSave();
		return {label};
}

METHOD(EEPROM, getSize) {
		logC("EEPROM::getSize()");
		return {AurumConfig.eepromSize};
} 

METHOD(EEPROM, getChecksum) {
		logC("EEPROM::getChecksum()");
		return {std::to_string(crc32(code))};
}

METHOD(EEPROM, makeReadonly) {
		logC("EEPROM::makeReadonly()");
		if (args.checkString(0) != std::to_string(crc32(code))) {
				return {Null(), "incorrect checksum"};
		}
		ro = true;
		AurumConfigSave();
		return {true};
}

METHOD(EEPROM, getDataSize) {
		logC("EEPROM::getDataSize()");
		return {AurumConfig.eepromDataSize};
}

METHOD(EEPROM, getData) {
		logC("EEPROM::getData()");
		return {data};
}

METHOD(EEPROM, setData) {
		logC("EEPROM::setData()");
		if (!context.tryChangeBuffer(-AurumConfig.eepromWriteCost)) {
				return {Null(), "not enough energy"};
		}
		std::string newData = args.optString(0, "");
		if (newData.length() > AurumConfig.eepromDataSize) {
				throw std::invalid_argument("not enough space");
		}
		data = newData;
		context.pause(1);
		AurumConfigSave();
		return {Null()};
}
