#include "config.h"

#include <yaml-cpp/yaml.h>

void AurumConfigFromYAML(std::string yaml) {
		YAML::Node root = YAML::Load(yaml);
		AurumConfig.logging.call = root["logging"]["call"].as<bool>(AurumConfig.logging.call);
		AurumConfig.logging.debug = root["logging"]["debug"].as<bool>(AurumConfig.logging.debug);
		if (root["machines"].IsSequence()) {
				AurumConfig.machines.clear();
				for (YAML::Node node : root["machines"]) {
						MachineEntry entry;
						entry.address = node["address"].as<std::string>();
						AurumConfig.machines.push_back(entry);
				}
		}
}



std::string AurumConfigToYAML() {
		YAML::Node root;
		root["logging"]["call"] = AurumConfig.logging.call;
		root["logging"]["debug"] = AurumConfig.logging.debug;
		for (const MachineEntry& entry : AurumConfig.machines) {
				YAML::Node node;
				node["address"] = entry.address;
				root["machines"].push_back(node);
		}
		YAML::Emitter emitter;
		emitter << root;
		return emitter.c_str();
}