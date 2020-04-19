#ifndef __AURUM_SCREEN_H__
#define __AURUM_SCREEN_H__

#include <string>
#include <atomic>
#include <mutex>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "component.h"
#include "../utils/sdl_helper.h"
#include "keyboard.h"
#include "../utils/graphics.h"


class Screen: public Component {
		private:
				std::recursive_mutex lock;
				bool powered = true;
				bool precise = false;
				bool touchModeInvert = false;
				bool buttonPushed = false;
				bool dragging = false;
				std::atomic_bool reCreateRenderer = true;
				std::vector<Keyboard*> keyboards;
				PointXY maxResolution;
				PointXY resolution;
				ColorDepth maxDepth;
				ColorDepth colorDepth;
				uint32_t background = 0xFF000000;
				uint32_t foreground = 0xFFFFFFFF;
				SDL_Window* window = NULL;
				SDL_Renderer* renderer = NULL;
				std::vector<std::vector<char16_t>> screenChars;
				std::vector<std::vector<uint32_t>> screenFgs;
				std::vector<std::vector<uint32_t>> screenBgs;
				uint32_t* screenPixels = NULL;
				SDL_Texture* screenTexture = NULL;
				void setChar(PointXY, char16_t);
				void copyChar(PointXY, PointXY);
				void resize(PointXY);
		public:
				static const std::string TYPE;
				Screen(Machine*);
				~Screen();
				void save(YAML::Node&) override;
				void load(YAML::Node) override;
				void update() override;
				char16_t get(PointXY);
				void set(PointXY, std::string, bool);
				void copy(PointXY, PointXY, PointXY);
				void fill(PointXY, PointXY, char16_t);
				PointXY getMaxResolution();
				PointXY getResolution();
				ColorDepth getMaxDepth();
				ColorDepth getColorDepth();
				uint32_t getBackgroundColor();
				uint32_t getForegroundColor();
				uint32_t getBackgroundColor(PointXY);
				uint32_t getForegroundColor(PointXY);
				uint32_t getPaletteColor(uint32_t);
				PointXY getViewport();
				void drawChanges();
				bool isBackgroundFromPalette();
				bool isForegroundFromPalette();
				bool isBackgroundFromPalette(PointXY);
				bool isForegroundFromPalette(PointXY);
				void setResolution(PointXY);
				void setColorDepth(ColorDepth);
				void setForegroundColor(uint32_t, bool = false);
				void setBackgroundColor(uint32_t, bool = false);
				void setPaletteColor(uint32_t, uint32_t);
				void setViewport(PointXY);
				DMETHOD(isTouchModeInverted);
				DMETHOD(setTouchModeInverted);
				DMETHOD(isPrecise);
				DMETHOD(setPrecise);
				DMETHOD(turnOff);
				DMETHOD(turnOn);
				DMETHOD(isOn);
				DMETHOD(getAspectRatio);
				DMETHOD(getKeyboards);
};

#endif
