#ifndef __AURUM_LOG_H__
#define __AURUM_LOG_H__

#include <iostream>

#define log(msg) std::cout << msg << std::endl;

#define logE(msg) log("[\x1b[31mError\x1b[0m] " << msg)

#endif