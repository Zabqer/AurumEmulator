#include "screen.h"
#include "../log.h"
#include "../config.h"
#include "../synchronized.h"
#include "../utils/font.h"
#include "../machine.h"

#include <locale>
#include <codecvt>

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;

const std::string Screen::TYPE = "screen";

Screen::Screen(Machine* machine): Component(machine, TYPE) {
		logC("Screen::Screen()");
		setMethod("isTouchModeInverted", {callback: wrapMethod(isTouchModeInverted), doc: "function():boolean -- Whether touch mode is inverted (sneak-activate opens GUI, instead of normal activate)."});
		setMethod("setTouchModeInverted", {callback: wrapMethod(setTouchModeInverted), doc: "function(value:boolean):boolean -- Sets whether to invert touch mode (sneak-activate opens GUI, instead of normal activate)."});
		setMethod("isPrecise", {callback: wrapMethod(isPrecise), doc: "function():boolean -- Returns whether the screen is in high precision mode (sub-pixel mouse event positions)."});
		setMethod("setPrecise", {callback: wrapMethod(setPrecise), doc: "function(enabled:boolean):boolean -- Set whether to use high precision mode (sub-pixel mouse event positions)."});
		setMethod("turnOff", {callback: wrapMethod(turnOff), doc: "function():boolean -- Turns off the screen. Returns true if it was"});
		setMethod("turnOn", {callback: wrapMethod(turnOn), doc: "function():boolean -- Turns the screen on. Returns true if it was off."});
		setMethod("isOn", {callback: wrapMethod(isOn), doc: "function():boolean -- Returns whether the screen is currently on."});
		setMethod("getAspectRatio", {callback: wrapMethod(getAspectRatio), doc: "function():number, number -- The aspect ratio of the screen. For multi-block screens this is the number of blocks, horizontal and vertical."});
		setMethod("getKeyboards", {callback: wrapMethod(getKeyboards), doc: "function():table -- The list of keyboards attached to the screen."});
		maxTier = 2;
		if (!isFontLoaded()) {
				loadFont();
		}
		useSDL();
}

Screen::~Screen() {
		logC("Screen::~Screen()");
		SDL_DestroyWindow(window);
		freeSDL();
}

int i = 0;

void Screen::drawChanges() {
		logC("Screen::drawChanges()");	
		if (!(renderer && screenTexture && screenPixels)) {
				return;
		}
		i++;
			if (i > 20) {
				i = 0;;
		uint32_t fg = foreground;	
		uint32_t bg = background;
		foreground = 0xFFFFFFFF;
		background = 0xFF000000;
		char buf[20];
		sprintf(buf, "Memory: %f%%@", ((double) machine->usedMemory) / machine->totalMemory * 100);
		set(PointXY{1, resolution.y - 1}, buf, false);
		foreground = fg;
		background = bg;
			}
		synchronized(lock);
		SDL_RenderClear(renderer);
		if (powered) {
				SDL_UpdateTexture(screenTexture, NULL, screenPixels, resolution.x * 8 * sizeof(uint32_t));
				SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
		}
		SDL_RenderPresent(renderer);
}

void Screen::resize(PointXY oldResolution) {
		logC("Screen::resize(PointXY{x=" << oldResolution.x << ",y=" << oldResolution.y << "})");
		if (oldResolution.x == resolution.x && oldResolution.y == resolution.y) {
				return;
		}
		reCreateRenderer = true;
		synchronized(lock);
		SDL_SetWindowSize(window, resolution.x * 8, resolution.y * 16);
		uint32_t* oldScreenPixels = screenPixels;
		screenPixels = new uint32_t[resolution.x * 8 * resolution.y * 16];
		std::vector<std::vector<char16_t>> oldScreenChars = screenChars;
		std::vector<std::vector<uint32_t>> oldScreenFgs = screenFgs;
		std::vector<std::vector<uint32_t>> oldScreenBgs = screenBgs;
		screenChars = std::vector<std::vector<char16_t>>(resolution.x, std::vector<char16_t>(resolution.y));
		screenFgs = std::vector<std::vector<uint32_t>>(resolution.x, std::vector<uint32_t>(resolution.y));
		screenBgs = std::vector<std::vector<uint32_t>>(resolution.x, std::vector<uint32_t>(resolution.y));
		for (int x = 0; x < resolution.x; x++) {
				for (int y = 0; y < resolution.y; y++) {
						if (oldScreenPixels && x < oldResolution.x && y < oldResolution.y) {
								screenChars[x][y] = oldScreenChars[x][y];
								for (int sx = 0; sx < 8; sx++) {
										for (int sy = 0; sy < 16; sy++) {
												screenPixels[((y * 16 + sy) * resolution.x) + (x * 8 + sx)] = oldScreenPixels[((y * 16 + sy) * oldResolution.x) + (x * 8 + sx)];
										}
								}
								screenFgs[x][y] = oldScreenFgs[x][y];
								screenBgs[x][y] = oldScreenBgs[x][y];
						} else {
								screenChars[x][y] = ' ';
								for (int sx = 0; sx < 8; sx++) {
										for (int sy = 0; sy < 16; sy++) {
												screenPixels[((y * 16 + sy) * resolution.x) + (x * 8 + sx)] = background;
										}
								}
								screenFgs[x][y] = foreground;
								screenBgs[x][y] = background;
						}
				}
		}
		delete[] oldScreenPixels;
		machine->signal({"screen_resized", {address(), resolution.x, resolution.y}});
}

void Screen::setChar(PointXY pos, char16_t ch) {
		//logC("Screen::setChar(PointXY{x=" << pos.x << ",y=" << pos.y << "}, " << ch << ")");
		if (pos.x < 0 || pos.x >= resolution.x || pos.y < 0 || pos.y >= resolution.y) {
				return;
		}
		screenChars[pos.x][pos.y] = ch;
		if (!fontBitmap.count(ch)) {
				ch = 63;
		}
		screenFgs[pos.x][pos.y] = foreground;
		screenBgs[pos.x][pos.y] = background;
		pos.x *= 8;
		pos.y *= 16;
		size_t chlen = fontBitmap[ch].size() / 8 / 16;
		for (int i = 0; i < 16; i++) {
				for (int j = 0; j < chlen * 8; j++) {
						screenPixels[(pos.y + i) * resolution.x * 8 + (pos.x + j)] = fontBitmap[ch][i * chlen * 8 + j] ? foreground : background;
				}
		}
}

void Screen::copyChar(PointXY at, PointXY to) {
		if (at.x < 0 || at.x > resolution.x || at.y < 0 || at.y > resolution.y) {
				return;
		}
		uint32_t fg = foreground;
		uint32_t bg = background;
		foreground = screenFgs[at.x][at.y];
		background = screenBgs[at.x][at.y];
		setChar(to, screenChars[at.x][at.y]);
		foreground = fg;
		background = bg;

} 

void Screen::save(YAML::Node& node) {
		logC("Screen::save()");
		Component::save(node);
		node["width"] = resolution.x;
		node["height"] = resolution.y;
}

void Screen::load(YAML::Node node) {
		logC("Screen::load()");
		Component::load(node);
		maxResolution = AurumConfig.screenResolutionsByTier[_tier];
		maxDepth = AurumConfig.screenDepthByTier[_tier];
		resolution.x = node["width"].as<unsigned int>(maxResolution.x);
		resolution.y = node["height"].as<unsigned int>(maxResolution.y);
		window = SDL_CreateWindow(("AurumEmulator > Screen [" + _address + "]").c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, resolution.x * 8, resolution.y * 16, SDL_WINDOW_OPENGL);
		if (!window) {
				logE("Error creating window: " << SDL_GetError());
				exit(1);
		}
		resize({0, 0});
}

void Screen::update() {
		logC("Screen::update()");
		if (reCreateRenderer) {
				SDL_DestroyRenderer(renderer);
				renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
				if (!renderer) {
						logE("Error creating renderer: " << SDL_GetError());
						exit(1);
				}
				SDL_DestroyTexture(screenTexture);
				screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, resolution.x * 8, resolution.y * 16);
				reCreateRenderer = false;
		}
		if (!(window && renderer)) {
				return;
		}
		drawChanges();
		for (SDL_Event& event : sdlEvents) {
				switch (event.type) {
						//case SDL_MOUSEBUTTONDOWN:
						case SDL_FINGERDOWN: 
								machine->signal({"touch", {address(), (int) (event.tfinger.x * resolution.x), (int) (event.tfinger.y * resolution.y), 0, Null()}});
								buttonPushed = true;
								break;
						case SDL_FINGERMOTION:
								if (buttonPushed) {
										if (!dragging) {
												dragging = true;
										}
										machine->signal({"drag", {address(), (int) (event.tfinger.x * resolution.x), (int) (event.tfinger.y * resolution.y), 0, Null()}});
								}
								break;
						case SDL_FINGERUP:
								if (dragging) {
										machine->signal({"drop", {address(), (int) (event.tfinger.x * resolution.x), (int) (event.tfinger.y * resolution.y), 0, Null()}});
										dragging = false;
								}
								buttonPushed = false;
								break;
				}
		}
}

char16_t Screen::get(PointXY pos) {
		logC("Screen::get()");
		synchronized(lock);
		return screenChars[pos.x][pos.y];
}

void Screen::set(PointXY pos, std::string value, bool vertical) {
		logC("Screen::set()");
		synchronized(lock);
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
		std::u16string str = convert.from_bytes(value);
		if (vertical) {
				for (int i = 0; i < str.size(); i++) {
						setChar({pos.x, pos.y + i}, str[i]);
				}
		} else {
				for (int i = 0; i < str.size(); i++) {
						setChar({pos.x + i, pos.y}, str[i]);
				}
		}
}

void Screen::fill(PointXY pos, PointXY rect, char16_t ch) {
		logC("Screen::fill()");
		synchronized(lock);
		for (int x = 0; x < rect.x; x++) {
				for (int y = 0; y < rect.y; y++) {
						setChar({pos.x + x, pos.y + y}, ch);
				}
		}
}

void Screen::copy(PointXY pos, PointXY rect, PointXY offset) {
		logC("Screen::copy()");
		synchronized(lock);
		for (int x = 0; x < rect.x; x++) {
				for (int y = 0; y < rect.y; y++) {
						copyChar({pos.x + x, pos.y + y}, {pos.x + x + offset.x, pos.y + y + offset.y});
				}
		}
}

PointXY Screen::getMaxResolution() {
		logC("Screen::getMaxResolution()");
		synchronized(lock);
		return maxResolution;
}

PointXY Screen::getResolution() {
		logC("Screen::getResolution()");
		synchronized(lock);
		return resolution;
}

ColorDepth Screen::getMaxDepth() {
		logC("Screen::getMaxDepth()");
		synchronized(lock);
		return maxDepth;
}

ColorDepth Screen::getColorDepth() {
		logC("Screen::getColorDepth()");
		synchronized(lock);
		return colorDepth;
}

uint32_t Screen::getBackgroundColor() {
		logC("Screen::getBackgroundColor()");
		synchronized(lock);
		return background & 0xFFFFFF;
}

uint32_t Screen::getForegroundColor() {
		logC("Screen::getForegroundColor()");
		synchronized(lock);
		return foreground & 0xFFFFFF;
}

uint32_t Screen::getBackgroundColor(PointXY pos) {
		logC("Screen::getBackgroundColor()");
		synchronized(lock);
		return screenBgs[pos.x][pos.y];
}

uint32_t Screen::getForegroundColor(PointXY pos) {
		logC("Screen::getForegroundColor()");
		synchronized(lock);
		return screenFgs[pos.x][pos.y];
}

uint32_t Screen::getPaletteColor(uint32_t) {
		logC("Screen::getPaletteColor()");
		synchronized(lock);
		abort();
		return 0;
}

PointXY Screen::getViewport() {
		logC("Screen::getViewport()");
		synchronized(lock);
		return resolution; 
}

bool Screen::isBackgroundFromPalette() {
		logC("Screen::isBackroundFromPalette()");
		synchronized(lock);
		return false;
}

bool Screen::isForegroundFromPalette() {
		logC("Screen::isForegroundFromPalette()");
		synchronized(lock);
		return false;
}

bool Screen::isBackgroundFromPalette(PointXY pos) {
		logC("Screen::isBackroundFromPalette()");
		synchronized(lock);
		return false;
}

bool Screen::isForegroundFromPalette(PointXY pos) {
		logC("Screen::isForegroundFromPalette()");
		synchronized(lock);
		return false;
}

void Screen::setResolution(PointXY res) {
		logC("Screen::setResolution(PointXY{x=" << res.x << ",y=" << res.y << "})");
		synchronized(lock);
		PointXY oldResolution = resolution;
		resolution = res;
		resize(oldResolution);
}

void Screen::setColorDepth(ColorDepth depth) {
		logC("Screen::setColorDepth()");
		synchronized(lock);
		if (depth <= maxDepth) {
				colorDepth = depth;
				return;
		}
		throw std::runtime_error("unsupported color depth");
}

void Screen::setBackgroundColor(uint32_t color, bool pal) {
		logC("Screen::setBackgroundColor()");
		synchronized(lock);
		background = color | 0xFF000000;
}

void Screen::setForegroundColor(uint32_t color, bool pal) {
		logC("Screen::setForegroundColor()");
		synchronized(lock);
		foreground = color | 0xFF000000;
}

void Screen::setPaletteColor(uint32_t index, uint32_t color) {
		logC("Screen::setPaletteColor");
		synchronized(lock);
}

void Screen::setViewport(PointXY rect) {
		logC("Screen::setViewport()");
		synchronized(lock);
}

METHOD(Screen, isTouchModeInverted) {
		logC("Screen::isTouchModeInverted()");
		synchronized(lock);
		return {touchModeInvert};
}

METHOD(Screen, setTouchModeInverted) {
		logC("Screen::setTouchModeInverted()");
		synchronized(lock);
		bool old = touchModeInvert;
		touchModeInvert = args.checkBoolean(0);
		return {old != touchModeInvert};
}

METHOD(Screen, isPrecise) {
		logC("Screen::isPrecise()");
		synchronized(lock);
		return {precise};
}

METHOD(Screen, setPrecise) {
		logC("Screen::setPrecise()");
		synchronized(lock);
		bool old = precise;
		precise = args.checkBoolean(0);
		return {old != precise};
}

METHOD(Screen, turnOff) {
		logC("Screen::turnOff()");
		synchronized(lock);
		bool oldState = powered;
		powered = false;
		return {oldState == true};
}

METHOD(Screen, turnOn) {
		logC("Screen::turnOn()");
		synchronized(lock);
		bool oldState = powered;
		powered = true;
		return {oldState == false};
}

METHOD(Screen, isOn) {
		logC("Screen::isOn()");
		synchronized(lock);
		return {powered};
}

METHOD(Screen, getAspectRatio) {
		logC("Screen::getAspectRatio()");
		synchronized(lock);
		return {1, 1};
}

METHOD(Screen, getKeyboards) {
		logC("Screen::getKeyboards()");
		synchronized(lock);
		std::vector<std::any> kbs;
		for (Keyboard* kb : keyboards) {
				kbs.push_back(kb->address());
		}
		return {kbs};
}
