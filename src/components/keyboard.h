#ifndef __AURUM_KEYBOARD_H__
#define __AURUM_KEYBOARD_H__

#include <string>

#include <yaml-cpp/yaml.h>

#include "component.h"

#include <SDL2/SDL.h>

class Keyboard: public Component {
		private:
		public:
				static const std::string TYPE;
				Keyboard(Machine*);
				~Keyboard();
				void save(YAML::Node&) override;
				void load(YAML::Node) override;
				void update() override;
};

#endif