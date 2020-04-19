#include "keyboard.h"
#include "../utils/sdl_helper.h"
#include "../machine.h"
#include "../utils/unicode.h"

#include "../log.h"

const std::string Keyboard::TYPE = "keyboard";

static std::map<int, int> keymap {
		{SDL_SCANCODE_ESCAPE, 1},
		{SDL_SCANCODE_1, 2},
		{SDL_SCANCODE_2, 3},
		{SDL_SCANCODE_3, 4},
		{SDL_SCANCODE_4, 5},
		{SDL_SCANCODE_5, 6},
		{SDL_SCANCODE_6, 7},
		{SDL_SCANCODE_7, 8},
		{SDL_SCANCODE_8, 9},
		{SDL_SCANCODE_9, 10},
		{SDL_SCANCODE_0, 11},
		{SDL_SCANCODE_MINUS, 12},
		{SDL_SCANCODE_EQUALS, 13},
		{SDL_SCANCODE_BACKSPACE, 14},
		{SDL_SCANCODE_TAB, 15},
		{SDL_SCANCODE_Q, 16},
		{SDL_SCANCODE_W, 17},
		{SDL_SCANCODE_E, 18},
		{SDL_SCANCODE_R, 19},
		{SDL_SCANCODE_T, 20},
		{SDL_SCANCODE_Y, 21},
		{SDL_SCANCODE_U, 22},
		{SDL_SCANCODE_I, 23},
		{SDL_SCANCODE_O, 24},
		{SDL_SCANCODE_P, 25},
		{SDL_SCANCODE_LEFTBRACKET, 26},
		{SDL_SCANCODE_RIGHTBRACKET, 27},
		{SDL_SCANCODE_RETURN, 28},
		{SDL_SCANCODE_LCTRL, 29},
		{SDL_SCANCODE_A, 30},
		{SDL_SCANCODE_S, 31},
		{SDL_SCANCODE_D, 32},
		{SDL_SCANCODE_F, 33},
		{SDL_SCANCODE_G, 34},
		{SDL_SCANCODE_H, 35},
		{SDL_SCANCODE_J, 36},
		{SDL_SCANCODE_K, 37},
		{SDL_SCANCODE_L, 38},
		{SDL_SCANCODE_SEMICOLON, 39},
		{SDL_SCANCODE_APOSTROPHE, 40},
		{SDL_SCANCODE_GRAVE, 41},
		{SDL_SCANCODE_LSHIFT, 42},
		{SDL_SCANCODE_Z, 44},
		{SDL_SCANCODE_X, 45},
		{SDL_SCANCODE_C, 46},
		{SDL_SCANCODE_V, 47},
		{SDL_SCANCODE_B, 48},
		{SDL_SCANCODE_N, 49},
		{SDL_SCANCODE_M, 50},
		{SDL_SCANCODE_COMMA, 51},
		{SDL_SCANCODE_PERIOD, 52},
		{SDL_SCANCODE_SLASH, 53},
		{SDL_SCANCODE_RSHIFT, 54},
		{SDL_SCANCODE_KP_MULTIPLY, 55},
		{SDL_SCANCODE_LALT, 56},
		{SDL_SCANCODE_SPACE, 57},
		{SDL_SCANCODE_CAPSLOCK, 58},
		{SDL_SCANCODE_F1, 59},
		{SDL_SCANCODE_F2, 60},
		{SDL_SCANCODE_F3, 61},
		{SDL_SCANCODE_F4, 62},
		{SDL_SCANCODE_F5, 63},
		{SDL_SCANCODE_F6, 64},
		{SDL_SCANCODE_F7, 65},
		{SDL_SCANCODE_F8, 66},
		{SDL_SCANCODE_F9, 67},
		{SDL_SCANCODE_F10, 68},
		{SDL_SCANCODE_NUMLOCKCLEAR, 69},
		{SDL_SCANCODE_SCROLLLOCK, 70},
		{SDL_SCANCODE_KP_MINUS, 74},
		{SDL_SCANCODE_KP_PLUS, 78},
		{SDL_SCANCODE_F11, 87},
		{SDL_SCANCODE_F12, 88},
		{SDL_SCANCODE_KP_ENTER, 156},
		{SDL_SCANCODE_RCTRL, 157},
		{SDL_SCANCODE_KP_DIVIDE, 181},
		{SDL_SCANCODE_RALT, 184},
		{SDL_SCANCODE_PAUSE, 197},
		{SDL_SCANCODE_HOME, 199},
		{SDL_SCANCODE_KP_7, 199},
		{SDL_SCANCODE_KP_8, 200},
		{SDL_SCANCODE_UP, 200},
		{SDL_SCANCODE_PAGEUP, 201},
		{SDL_SCANCODE_KP_9, 201},
		{SDL_SCANCODE_LEFT, 203},
		{SDL_SCANCODE_KP_4, 203},
		{SDL_SCANCODE_KP_6, 205},
		{SDL_SCANCODE_RIGHT, 205},
		{SDL_SCANCODE_END, 207},
		{SDL_SCANCODE_KP_1, 207},
		{SDL_SCANCODE_DOWN, 208},
		{SDL_SCANCODE_KP_2, 208},
		{SDL_SCANCODE_PAGEDOWN, 209},
		{SDL_SCANCODE_KP_3, 209},
		{SDL_SCANCODE_KP_0, 210},
		{SDL_SCANCODE_INSERT, 210},
		{SDL_SCANCODE_DELETE, 211},
		{SDL_SCANCODE_KP_PERIOD, 211}
};

Keyboard::Keyboard(Machine* machine): Component(machine, TYPE, false) {
		logC("Keyboard::Keyboard()");
		useSDL();
}

Keyboard::~Keyboard() {
		logC("Keyboard::~Keyboard()");
		freeSDL();
}

void Keyboard::save(YAML::Node& node) {
		logC("Keyboard::save()");
		Component::save(node);
}

void Keyboard::load(YAML::Node node) {
		logC("Keyboard::load()");
		Component::load(node);
		SDL_StartTextInput();
}

void Keyboard::update() {
		logC("Keyboard::update()");
		for (SDL_Event& event : sdlEvents) {
				switch (event.type) {
						case SDL_KEYDOWN:
								machine->signal({"key_down", {address(), (char16_t) event.key.keysym.sym, keymap[(int) event.key.keysym.scancode], Null()}});
				}
		}
}
