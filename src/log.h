#ifndef __AURUM_LOG_H__
#define __AURUM_LOG_H__

#include <iostream>

#include "config.h"

#define _log(msg) std::cout << msg << std::endl;

#define logE(msg) _log("[\x1b[91mError\x1b[0m] " << msg)

#define logW(msg) _log("[\x1b[93mWarning\x1b[0m] " << msg)

#define logD(msg) if (AurumConfig.logging_debug) _log("[\x1b[96mDebug\x1b[0m] " << msg)

#define logC(msg) if (AurumConfig.logging_call) _log("[\x1b[98mCall\x1b[0m] " << msg)

#endif