#ifndef __AURUM_FONT_H__
#define __AURUM_FONT_H__

#include <map>
#include <vector>

bool isFontLoaded();
void loadFont();

extern std::map<char16_t, std::vector<bool>> fontBitmap;

#endif