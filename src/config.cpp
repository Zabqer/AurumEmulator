#include "config.h"

#include "log.h"

#include <yaml-cpp/yaml.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "components/CPU.h"
#include "components/RAM.h"

boost::uuids::random_generator uuid_generator;

AurumConfig_t AurumConfig;

void AurumConfigFromYAML(std::string yaml, std::vector<Machine*>& machines, bool* forcecall, bool* forcedebug) {
		YAML::Node root = YAML::Load(yaml);
		AurumConfig.logging_call = !forcecall ?	root["logging"]["call"].as<bool>(false) : *forcecall;
		AurumConfig.logging_debug = !forcedebug ? root["logging"]["debug"].as<bool>(false) : *forcedebug;
		try {
				if (root["machines"].IsSequence()) {
						for (YAML::Node node :root["machines"]) {
								Machine* machine = new Machine;
								machine->load(node["address"].as<std::string>(boost::uuids::to_string(uuid_generator())));
								for (YAML::Node node :node["components"]) {
										std::string type = node["type"].as<std::string>();
										if (type == CPU::TYPE) {
												CPU* cpu = new CPU;
												cpu->load(node["tier"].as<int>(1), node["arch"].as<std::string>("Lua 5.3"));
												machine->addComponent(cpu);
										} else if (type == RAM::TYPE) {
												RAM* ram = new RAM;
												ram->load(node["tier"].as<int>(1));
												machine->addComponent(ram);
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
				logW("Failed getting machines and components; using default")
		}
		AurumConfig.cpuComponentCount = root["computer"]["cpuComponentCount"].as<std::array<int, 4>>(std::array<int, 4>{8, 12, 16, 1024});
		AurumConfig.callBudgets = root["computer"]["callBudgets"].as<std::array<double, 3>>(std::array<double, 3>{0.5, 1.0, 1.5});
		AurumConfig.ramSizes = root["computer"]["ramSizes"].as<std::array<int, 6>>(std::array<int, 6>{192, 256, 384, 512, 768, 1024});
		AurumConfig.ignorePower = root["power"]["ignorePower"].as<bool>(false);
		AurumConfig.tickFrequency = root["power"]["tickFrequency"].as<int>(10);
		AurumConfig.computerCost = root["power"]["cost"]["computer"].as<double>(0.5);
		AurumConfig.sleepCostFactor = root["power"]["cost"]["sleepFactor"].as<double>(0.1);
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
						if (component->type() == CPU::TYPE) {
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
						}
				}
				root["machines"].push_back(node);
		}
		root["computer"]["cpuComponentCount"] = AurumConfig.cpuComponentCount;
		root["computer"]["callBudgest"] = AurumConfig.callBudgets;
		root["computer"]["ramSizes"] = AurumConfig.ramSizes;
		root["power"]["ingnorePower"] = AurumConfig.ignorePower;
		root["power"]["tickFrequency"] = AurumConfig.tickFrequency;
		root["power"]["cost"]["computer"] = AurumConfig.computerCost;
		root["power"]["cost"]["sleepFactor"] = AurumConfig.sleepCostFactor;
		YAML::Emitter emitter;
		emitter << root;
		return emitter.c_str();
}