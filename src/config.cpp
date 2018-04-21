#include "config.h"

#include "log.h"

#include <yaml-cpp/yaml.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "components/aurum_debug.h"
#include "components/CPU.h"
#include "components/RAM.h"
#include "components/EEPROM.h"
#include "components/filesystem.h"

boost::uuids::random_generator uuid_generator;

AurumConfig_t AurumConfig;

//TODO: improve readability

void AurumConfigFromYAML(std::string yaml, std::vector<Machine*>& machines, bool* forcecall, bool* forcedebug) {
		YAML::Node root = YAML::Load(yaml);
		AurumConfig.logging_call = !forcecall ?	root["logging"]["call"].as<bool>(false) : *forcecall;
		AurumConfig.logging_debug = !forcedebug ? root["logging"]["debug"].as<bool>(false) : *forcedebug;
		AurumConfig.threads = root["computer"]["threads"].as<int>(4);
		AurumConfig.timeout = root["computer"]["timeout"].as<double>(5.0);
		AurumConfig.allowBytecode = root["computer"]["allowBytecode"].as<bool>(false);
		AurumConfig.allowGC = root["computer"]["allowGC"].as<bool>(false);
		AurumConfig.eepromSize = root["computer"]["eepromSize"].as<int>(4096);
		AurumConfig.eepromDataSize = root["computer"]["eepromDataSize"].as<int>(256);
		AurumConfig.startupDelay = root["computer"]["startupDelay"].as<double>(0.25);
		AurumConfig.cpuComponentCount = root["computer"]["cpuComponentCount"].as<std::array<int, 4>>(std::array<int, 4>{8, 12, 16, 1024});
		AurumConfig.callBudgets = root["computer"]["callBudgets"].as<std::array<double, 3>>(std::array<double, 3>{0.5, 1.0, 1.5});
		AurumConfig.ramSizes = root["computer"]["ramSizes"].as<std::array<int, 6>>(std::array<int, 6>{192, 256, 384, 512, 768, 1024});
		AurumConfig.ignorePower = root["power"]["ignorePower"].as<bool>(false);
		AurumConfig.tickFrequency = root["power"]["tickFrequency"].as<int>(10);
		AurumConfig.computerCost = root["power"]["cost"]["computer"].as<double>(0.5);
		AurumConfig.sleepCostFactor = root["power"]["cost"]["sleepFactor"].as<double>(0.1);
		AurumConfig.eepromWriteCost = root["computer"]["cost"]["eepromWrite"].as<double>(50);
		AurumConfig.hddSizes = root["filesystem"]["hddSizes"].as<std::array<int, 3>>(std::array<int, 3>{1024, 2048, 4096});
		AurumConfig.maxOpenHandles = root["filesystem"]["openHandles"].as<int>(16);
		try {
				if (root["machines"].IsSequence()) {
						for (YAML::Node node :root["machines"]) {
								Machine* machine = new Machine;
								machine->load(node["address"].as<std::string>(boost::uuids::to_string(uuid_generator())));
								for (YAML::Node node :node["components"]) {
										std::string type = node["type"].as<std::string>();
										if (type == AurumDebug::TYPE) {
												AurumDebug* aurum_debug = new AurumDebug;
												aurum_debug->load(node["address"].as<std::string>(boost::uuids::to_string(uuid_generator())));
 												machine->addComponent(aurum_debug);
										} else if (type == CPU::TYPE) {
												CPU* cpu = new CPU;
												cpu->load(node["tier"].as<int>(1), node["arch"].as<std::string>("Lua 5.3"));
												machine->addComponent(cpu);
										} else if (type == RAM::TYPE) {
												RAM* ram = new RAM;
												ram->load(node["tier"].as<int>(1));
												machine->addComponent(ram);
										} else if (type == EEPROM::TYPE) {
												EEPROM* eeprom = new EEPROM;
												eeprom->load(node["address"].as<std::string>(boost::uuids::to_string(uuid_generator())), node["label"].as<std::string>("EEPROM"), node["read-only"].as<bool>(false));
												machine->addComponent(eeprom);
										} else if (type == FileSystem::TYPE) {
												FileSystem* filesystem = new FileSystem;
												filesystem->load(node["address"].as<std::string>(boost::uuids::to_string(uuid_generator())), node["tier"].as<int>(1), (node["label"].is(YAML::Node("")) ? std::optional<std::string>(node["label"].as<std::string>()) : std::optional<std::string>()), node["read-only"].as<bool>(false));
												machine->addComponent(filesystem);
										} else {
												logW("Unsupported component type '" << type << "'; skipping");
										}
								}
								machines.push_back(machine);
						}
				} else {
						throw 0;
				}
		} catch (...) {
				machines.clear();
				logW("Failed getting machines and components; using default");
				Machine* machine = new Machine;
				machine->load(boost::uuids::to_string(uuid_generator()));
				CPU* cpu = new CPU;
				cpu->load(1, "Lua 5.3");
				machine->addComponent(cpu);
				RAM* ram = new RAM;
				ram->load(1);
				machine->addComponent(ram);
				EEPROM* eeprom = new EEPROM;
				eeprom->load(boost::uuids::to_string(uuid_generator()), "EEPROM", false);
				machine->addComponent(eeprom);
				FileSystem* filesystem = new FileSystem;
				filesystem->load(boost::uuids::to_string(uuid_generator()), 1, "Filesystem", false);
				machine->addComponent(filesystem);
				machines.push_back(machine);
		}
}

std::string AurumConfigToYAML(std::vector<Machine*>& machines) {
		YAML::Node root;
		root["logging"]["call"] = AurumConfig.logging_call;
		root["logging"]["debug"] = AurumConfig.logging_debug;
		for (Machine* machine :machines) {
				YAML::Node node;
				std::string address;
				machine->save(address);
				node["address"] = address;
				for (Component* component :machine->components()) {
						if (component->type() == AurumDebug::TYPE) {
								AurumDebug* aurum_debug = (AurumDebug*) component;
								std::string address;
								aurum_debug->save(address);
								YAML::Node entry;
								entry["type"] = AurumDebug::TYPE;
								entry["address"] = address;
								node["components"].push_back(entry);
						} else if (component->type() == CPU::TYPE) {
								CPU* cpu = (CPU*) component;
								int tier;
								std::string arch;
								cpu->save(tier, arch);
								YAML::Node entry;
								entry["type"] = CPU::TYPE;
								entry["tier"] = tier;
								entry["arch"] = arch;
								node["components"].push_back(entry);
						} else if (component->type() == RAM::TYPE) {
								RAM* ram = (RAM*) component;
								int tier;
								ram->save(tier);
								YAML::Node entry;
								entry["type"] = RAM::TYPE;
								entry["tier"] = tier;
								node["components"].push_back(entry);
						} else if (component->type() == EEPROM::TYPE) {
								EEPROM* eeprom = (EEPROM*) component;
								std::string address;
								std::string label;
								bool ro;
								eeprom->save(address, label, ro);
								YAML::Node entry;
								entry["type"] = EEPROM::TYPE;
								entry["address"] = address;
								entry["label"] = label;
								entry["read-only"] = ro;
								node["components"].push_back(entry);
						} else if (component->type() == FileSystem::TYPE) {
								FileSystem* filesystem = (FileSystem*) component;
								std::string address;
								int tier;
								std::optional<std::string> label;
								bool ro;
								filesystem->save(address, tier, label, ro);
								YAML::Node entry;
								entry["type"] = FileSystem::TYPE;
								entry["address"] = address;
								entry["tier"] = tier;
								if (label) {
										entry["label"] = label.value();
								}
								entry["read-only"] = ro;								node["components"].push_back(entry);
						}
				}
				root["machines"].push_back(node);
		}
		root["computer"]["threads"] = AurumConfig.threads;
		root["computer"]["timeout"] = AurumConfig.timeout;
		root["computer"]["allowBytecode"] = AurumConfig.allowBytecode;
		root["computer"]["allowGC"] = AurumConfig.allowGC;
		root["computer"]["eepromSize"] = AurumConfig.eepromSize;
		root["computer"]["eepromDataSize"] = AurumConfig.eepromDataSize;
		root["computer"]["startupDelay"] = AurumConfig.startupDelay;
		root["computer"]["cpuComponentCount"] = AurumConfig.cpuComponentCount;
		root["computer"]["callBudgest"] = AurumConfig.callBudgets;
		root["computer"]["ramSizes"] = AurumConfig.ramSizes;
		root["power"]["ingnorePower"] = AurumConfig.ignorePower;
		root["power"]["tickFrequency"] = AurumConfig.tickFrequency;
		root["power"]["cost"]["computer"] = AurumConfig.computerCost;
		root["power"]["cost"]["sleepFactor"] = AurumConfig.sleepCostFactor;
		root["power"]["cost"]["eepromWrite"] = AurumConfig.eepromWriteCost;
		root["filesystem"]["hddSizes"] = AurumConfig.hddSizes;
		root["filesystem"]["openHandles"] = AurumConfig.maxOpenHandles;
		YAML::Emitter emitter;
		emitter << root;
		return emitter.c_str();
}

void AurumConfigSave() {
		AurumConfig.changed = true;
}