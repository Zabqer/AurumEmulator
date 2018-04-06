#include "config.h"

#include <yaml-cpp/yaml.h>

void AurumConfigFromYAML(std::string yaml) {
		YAML::Node root = YAML::Load(yaml);
		AurumConfig.logging.call = root["logging"]["call"].as<bool>(AurumConfig.logging.call);
		AurumConfig.logging.debug = root["logging"]["debug"].as<bool>(AurumConfig.logging.debug);
}



std::string AurumConfigToYAML() {
		YAML::Node root;
		root["logging"]["call"] = AurumConfig.logging.call;
		root["logging"]["debug"] = AurumConfig.logging.debug;
		YAML::Emitter emitter;
		emitter << root;
		return emitter.c_str();
}