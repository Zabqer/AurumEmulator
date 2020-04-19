#ifndef __AURUM_GPU_H__
#define __AURUM_GPU_H__

#include <string>

#include "component.h"
#include "screen.h"

class GPU: public Component {
		private:
				Screen* screen = NULL;
				PointXY _maxResolution;
				ColorDepth maxColorDepth;
				static const double setBackgroundCosts[3];
				static const double setForegroundCosts[3];
				static const double setPaletteColorCosts[3];
				static const double setCosts[3];
				static const double copyCosts[3];
				static const double fillCosts[3];
		public:
				static const std::string TYPE;
				GPU(Machine*);
				void save(YAML::Node&);
				void load(YAML::Node);
				void onError(Context, std::string);
				DMETHOD(bind);
				DMETHOD(getScreen);
				DMETHOD(getBackground);
				DMETHOD(setBackground);
				DMETHOD(getForeground);
				DMETHOD(setForeground);
				DMETHOD(getPaletteColor);
				DMETHOD(setPaletteColor);
				DMETHOD(getDepth);
				DMETHOD(setDepth);
				DMETHOD(maxDepth);
				DMETHOD(getResolution);
				DMETHOD(setResolution);
				DMETHOD(maxResolution);
				DMETHOD(getViewport);
				DMETHOD(setViewport);
				DMETHOD(get);
				DMETHOD(set);
				DMETHOD(copy);
				DMETHOD(fill);
};

#endif