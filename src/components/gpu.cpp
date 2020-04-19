#include "gpu.h"
#include "../log.h"
#include "../config.h"
#include "../utils/unicode.h"

const std::string GPU::TYPE = "gpu";

const double GPU::setBackgroundCosts[3] = {1.0 / 32, 1.0 / 64, 1.0 / 128};
const double GPU::setForegroundCosts[3] = {1.0 / 32, 1.0 / 64, 1.0 / 128};
const double GPU::setPaletteColorCosts[3] = {1.0 / 2, 1.0 / 8, 1.0 / 16};
const double GPU::setCosts[3] = {1.0 / 64, 1.0 / 128, 1.0 / 256};
const double GPU::copyCosts[3] = {1.0 / 16, 1.0 / 32, 1.0 / 64};
const double GPU::fillCosts[3] = {1.0 / 32, 1.0 / 64, 1.0 / 128};

GPU::GPU(Machine* machine): Component(machine, TYPE) {
		logC("GPU::GPU()");
		setMethod("bind", {callback: wrapMethod(bind), doc: "function(address:string[, reset:boolean=true]):boolean -- Binds the GPU to the screen with the specified address and resets screen settings if `reset` is true."});
		setMethod("getScreen", {callback: wrapMethod(getScreen), doc: "function():string -- Get the address of the screen the GPU is currently bound to.", direct: true});
		setMethod("getBackground", {callback: wrapMethod(getBackground), doc: "function():number, boolean -- Get the current background color and whether it's from the palette or not.", direct: true});
		setMethod("setBackground", {callback: wrapMethod(setBackground), doc: "function(value:number[, palette:boolean]):number, number or nil -- Sets the background color to the specified value. Optionally takes an explicit palette index. Returns the old value and if it was from the palette its palette index.", direct: true});
		setMethod("getForeground", {callback: wrapMethod(getForeground), doc: "function():number, boolean -- Get the current foreground color and whether it's from the palette or not.", direct: true});
		setMethod("setForeground", {callback: wrapMethod(setForeground), doc: "function(value:number[, palette:boolean]):number, number or nil -- Sets the foreground color to the specified value. Optionally takes an explicit palette index. Returns the old value and if it was from the palette its palette index.", direct: true});
		setMethod("getPaletteColor", {callback: wrapMethod(getPaletteColor), doc: "function(index:number):number -- Get the palette color at the specified palette index.", direct: true});
		setMethod("setPaletteColor", {callback: wrapMethod(setPaletteColor), doc: "function(index:number, color:number):number -- Set the palette color at the specified palette index. Returns the previous value.", direct: true});
		setMethod("getDepth", {callback: wrapMethod(getDepth), doc: "function():number -- Returns the currently set color depth.", direct: true});
		setMethod("setDepth", {callback: wrapMethod(setDepth), doc: "function(depth:number):number -- Set the color depth. Returns the previous value."});
		setMethod("maxDepth", {callback: wrapMethod(maxDepth), doc: "function():number -- Get the maximum supported color depth.", direct: true});
		setMethod("getResolution", {callback: wrapMethod(getResolution), doc: "function():number, number -- Get the current screen resolution.", direct: true});
		setMethod("setResolution", {callback: wrapMethod(setResolution), doc: "function(width:number, height:number):boolean -- Set the screen resolution. Returns true if the resolution changed.", direct: true});
		setMethod("maxResolution", {callback: wrapMethod(maxResolution), doc: "function():number, number -- Get the maximum screen resolution.", direct: true});
		setMethod("getViewport", {callback: wrapMethod(getViewport), doc: "function():number, number -- Get the current viewport resolution.", direct: true});
		setMethod("setViewport", {callback: wrapMethod(setViewport), doc: "function(width:number, height:number):boolean -- Set the viewport resolution. Cannot exceed the screen resolution. Returns true if the resolution changed."});
		setMethod("get", {callback: wrapMethod(get), doc: "function(x:number, y:number):string, number, number, number or nil, number or nil -- Get the value displayed on the screen at the specified index, as well as the foreground and background color. If the foreground or background is from the palette, returns the palette indices as fourth and fifth results, else nil, respectively", direct: true});
		setMethod("set", {callback: wrapMethod(set), doc: "function(x:number, y:number, value:string[, vertical:boolean]):boolean -- Plots a string value to the screen at the specified position. Optionally writes the string vertically.", direct: true});
		setMethod("copy", {callback: wrapMethod(copy), doc: "function(x:number, y:number, width:number, height:number, tx:number, ty:number):boolean -- Copies a portion of the screen from the specified location with the specified size by the specified translation.", direct: true});
		setMethod("fill", {callback: wrapMethod(fill), doc: "function(x:number, y:number, width:number, height:number, char:string):boolean -- Fills a portion of the screen at the specified position with the specified size with the specified character.", direct: true});
		maxTier = 2;
}

void GPU::save(YAML::Node& node) {
		logC("GPU::save()");
		Component::save(node);
}

void GPU::load(YAML::Node node) {
		logC("GPU::load()");
		Component::load(node);
		_maxResolution = AurumConfig.screenResolutionsByTier[_tier];
		maxColorDepth = AurumConfig.screenDepthByTier[_tier];
}

bool consumeEnergy(Context context, double n, double cost) {
		return context.tryChangeBuffer(-n * cost);
}

void GPU::onError(Context context, std::string message) {
		logC("GPU::onError()");
		if (!screen) {
				for (Component* component : context.components()) {
						if (component->type() == Screen::TYPE) {
								screen = (Screen*) component;
								break;
						}
				}
		}
		if (screen) {
				logD("Showing error: " << message);
				PointXY smr = screen->getMaxResolution();
				screen->setResolution({std::min(smr.x, _maxResolution.x), std::min(smr.y, _maxResolution.y)});
				screen->setColorDepth(std::min(maxColorDepth, screen->getMaxDepth()));
				screen->setForegroundColor(0xFFFFFF);
				if (screen->getColorDepth() > ColorDepth::OneBit) {
						screen->setBackgroundColor(0x0000FF);
				} else {
						screen->setBackgroundColor(0x0000FF);
				}
				PointXY res = screen->getResolution();
				screen->fill({0, 0}, res, ' ');
				//Todo - wrap lines
				std::vector<std::string> lines {message};
				int frow = std::max((int) (res.y - lines.size()) / 2, 2);
				message = "Unrecoverable Error";
				screen->set({(int) (res.x - message.length()) / 2, frow - 2}, message, false);
				unsigned int mll = 0;
				for (std::string& str : lines) {
						if (mll < str.length()) {
								mll = str.length();
						}
				}
				int col = std::max((int) (res.x - mll) / 2, 0);
				int row = 1;
				for (std::string& str : lines) {
						screen->set({col, row++}, str, false);
				}
				screen->drawChanges();
		}
}

METHOD(GPU, bind) {
		logC("GPU::bind()");
		std::string address = args.checkString(0);
		bool reset = args.optBoolean(1, true);
		try {
				Component* component = context.componentByAddress(address);
				if (!component || component->type() != Screen::TYPE) {
						return {Null(), "not a screen"};
				}
				screen = (Screen*) component;
				if (reset) {
						PointXY smr = screen->getMaxResolution();
						screen->setResolution({std::min(smr.x, _maxResolution.x), std::min(smr.y, _maxResolution.y)});
						screen->setColorDepth(std::min(maxColorDepth, screen->getMaxDepth()));
						screen->setForegroundColor(0xFFFFFF);
						screen->setBackgroundColor(0x000000);
				} else {
						context.pause(0.2);
				}
				return {true};
		} catch (std::invalid_argument) {
				return {Null(), "invalid address"};
		}
}

METHOD(GPU, getScreen) {
		logC("GPU::getScreen()");
		if (screen) {
				return {screen->address()};
		}
		return {Null()};
}

METHOD(GPU, getBackground) {
		logC("GPU::getBackground()");
		if (screen) {
				return {screen->getBackgroundColor(), screen->isBackgroundFromPalette()};
		}
		return {Null(), "no screen"};
}

METHOD(GPU, setBackground) {
		logC("GPU::setBackground()");
		context.consumeCallBudget(setBackgroundCosts[_tier]);
		uint32_t color = args.checkInteger(0);
		if (screen) {
				uint32_t oldColor = screen->getBackgroundColor();
				if (screen->isBackgroundFromPalette()) {
						uint32_t pcol = screen->getPaletteColor(oldColor);
						screen->setBackgroundColor(color, args.optBoolean(1, false));
						return {pcol, oldColor};
				} else {
						screen->setBackgroundColor(color, args.optBoolean(1, false));
						return {oldColor, Null()};
				}
		}
		return {Null(), "no screen"};
}

METHOD(GPU, getForeground) {
		logC("GPU::getForeground()");
		if (screen) {
				return {screen->getForegroundColor(), screen->isForegroundFromPalette()};
		}
		return {Null(), "no screen"};
}

METHOD(GPU, setForeground) {
		logC("GPU::setForeground()");
		context.consumeCallBudget(setForegroundCosts[_tier]);
		uint32_t color = args.checkInteger(0);
//		logE(color);
		if (screen) {
				uint32_t oldColor = screen->getForegroundColor();
				if (screen->isForegroundFromPalette()) {
						uint32_t pcol = screen->getPaletteColor(oldColor);
						screen->setForegroundColor(color, args.optBoolean(1, false));
						return {pcol, oldColor};
				} else {
						screen->setForegroundColor(color, args.optBoolean(1, false));
						return {oldColor, Null()};
				}
		}
		return {Null(), "no screen"};
}

METHOD(GPU, getPaletteColor) {
		logC("GPU::getPaletteColor()");
		if (screen) {
				return {screen->getPaletteColor(args.checkInteger(0))};
		}
		return {Null(), "no screen"};
}

METHOD(GPU, setPaletteColor) {
		logC("GPU::setPaletteColor()");
		if (screen) {
				uint32_t color = args.checkInteger(0);
				uint32_t index = args.checkInteger(1);
				context.pause(0.1);
				uint32_t oldColor = screen->getPaletteColor(index);
				screen->setPaletteColor(index, color);
				return {oldColor};
		}
		return {Null(), "no screen"};
}
METHOD(GPU, getDepth) {
		logC("GPU::getDepth()");
		if (screen) {
				switch (screen->getColorDepth()) {
						case ColorDepth::EightBit:
								return {8};
						case ColorDepth::FourBit:
								return {4};
						case ColorDepth::OneBit:
								return {1};
				}
		}
		return {Null(), "no screen"};
}

METHOD(GPU, setDepth) {
		logC("GPU::setDepth()");
		if (screen) {
				ColorDepth oldColorDepth = screen->getColorDepth();
				switch (args.checkInteger(0)) {
						case 1:
								screen->setColorDepth(ColorDepth::OneBit);
								break;
						case 4:
								if (maxColorDepth >= ColorDepth::FourBit) {
										screen->setColorDepth(ColorDepth::FourBit);
										break;
								}
						case 8:
								if (maxColorDepth >= ColorDepth::EightBit) {
										screen->setColorDepth(ColorDepth::EightBit);
										break;
								}
						default:
								throw std::invalid_argument("unsupported depth");
				}
				return {oldColorDepth};
		}
		return {Null(), "no screen"};
}

METHOD(GPU, maxDepth) {
		logC("GPU::maxDepth()");
		if (screen) {
				 switch (std::min(screen->getMaxDepth(), maxColorDepth)) {
						case ColorDepth::EightBit:
								return {8};
						case ColorDepth::FourBit:
								return {4};
						case ColorDepth::OneBit:
								return {1};
				}
		}
		return {Null(), "no screen"};
}
METHOD(GPU, getResolution) {
		logC("GPU::getResolution()");
		if (screen) {
				PointXY res = screen->getResolution();
				return {res.x, res.y};
		}
		return {Null(), "no screen"};
}

METHOD(GPU, setResolution) {
		logC("GPU::setResolution()");
		int x = args.checkInteger(0);
		int y = args.checkInteger(1);
		if (screen) {
				if (x < 1 || y < 1 || x > _maxResolution.x || y > _maxResolution.y) {
						throw std::invalid_argument("unsupported resolution");
				}
				screen->setResolution({x, y});
				context.signal("screen_resized", {x, y});
				return {true};
		}
		return {Null(), "no screen"};
}

METHOD(GPU, maxResolution) {
		logC("GPU::maxResolution()");
		if (screen) {
				PointXY res = screen->getMaxResolution();
				return {std::min(res.x, _maxResolution.x), std::min(res.y, _maxResolution.y)};
		}
		return {Null(), "no screen"};
}

METHOD(GPU, getViewport) {
		logC("GPU::getViewport()");
		if (screen) {
				PointXY viewport = screen->getViewport();
				return {viewport.x, viewport.y};
		}
		return {Null(), "no screen"};
}

METHOD(GPU, setViewport) {
		logC("GPU::setViewport()");
		int x = args.checkInteger(0);
		int y = args.checkInteger(1);
		if (screen) {
				if (x < 1 || y < 1 || x > _maxResolution.x || y > _maxResolution.y) {
						throw std::invalid_argument("unsupported viewport size");
				}
				PointXY res = screen->getResolution();
				if (x > res.x || y > res.y) {
						throw std::invalid_argument("unsupported viewport size");
				}
				screen->setViewport({x, y});
				return {true};
		}
		return {Null(), "no screen"};
}

METHOD(GPU, get) {
		logC("GPU::get()");
		PointXY pos = {args.checkInteger(0) - 1, args.checkInteger(1) - 1};
		if (screen) {
				std::any fgColor;
				std::any fgIndex;
				uint32_t fg = screen->getForegroundColor(pos);
				if (screen->isForegroundFromPalette(pos)) {
					
						fgColor = screen->getPaletteColor(fg);
						fgIndex = fg;
				} else {
						fgColor = fg;
						fgIndex = Null();
				}
				std::any bgColor;
				std::any bgIndex;
				uint32_t bg = screen->getBackgroundColor(pos);
				if (screen->isBackgroundFromPalette(pos)) {
						bgColor = screen->getPaletteColor(bg);
						bgIndex = bg;
				} else {
						bgColor = bg;
						bgIndex = Null();
				}
				return {u16to8(std::u16string{screen->get(pos)}), fgColor, bgColor, fgIndex, bgIndex};
		}
		return {Null(), "no screen"};
}

METHOD(GPU, set) {
		logC("GPU::set()");
		context.consumeCallBudget(setCosts[_tier]);
		PointXY pos = {args.checkInteger(0) - 1, args.checkInteger(1) - 1};
		std::string value = args.checkString(2);
		bool vertical = args.optBoolean(3, false);
		//logE(value);
		if (screen) {
				if (consumeEnergy(context, value.length(), AurumConfig.gpuSetCost)) {
						screen->set(pos, value, vertical);
						return {true};
				} else {
						return {Null(), "not enough energy"};
				}
		} else {
				return {Null(), "no screen"};
		}
}

METHOD(GPU, copy) {
		logC("GPU::copy()");
		context.consumeCallBudget(copyCosts[_tier]);
		PointXY pos = {args.checkInteger(0) - 1, args.checkInteger(1) - 1};
		PointXY rect = {std::max(0, args.checkInteger(2)), std::max(0, args.checkInteger(3))};
		PointXY offset = {args.checkInteger(4), args.checkInteger(5)}; ;
		if (screen) {
				if (consumeEnergy(context, rect.x * rect.y, AurumConfig.gpuCopyCost)) {
						screen->copy(pos, rect, offset);
						return {true};
				} else {
						return {Null(), "not enough energy"};
				}
		}
		return {Null(), "no screen"};
}

METHOD(GPU, fill) {
		logC("GPU::fill()");
		context.consumeCallBudget(copyCosts[_tier]);
		PointXY pos = {args.checkInteger(0) - 1, args.checkInteger(1) - 1};
		PointXY rect = {std::max(0, args.checkInteger(2)), std::max(0, args.checkInteger(3))};
		std::string str = args.checkString(4);
		if (str.length() != 1) {
				throw std::invalid_argument("invalid fill value");
		}
		char16_t ch = str[0];
		if (screen) {
				double cost = ch == ' ' ? AurumConfig.gpuClearCost : AurumConfig.gpuFillCost;
				if (consumeEnergy(context, rect.x * rect.y, cost)) {
						screen->fill(pos, rect, ch);
						return {true};
				} else {
						return {Null(), "not enough energy"};
				}
		}
		return {Null(), "no screen"};
}
