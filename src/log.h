#ifndef __AURUM_LOG_H__
#define __AURUM_LOG_H__

#include <fstream>
	
#include <iostream>

extern std::ofstream logFile;

#include "config.h"

#define _canlog(param) (AurumConfig.force_logging_ ## param) ? (*AurumConfig.force_logging_ ## param) : (AurumConfig.logging_ ## param)

#define _log(msg) std::cout << msg << std::endl//; logFile << msg << std::endl
#define logE(msg) _log("[\x1b[91mError\x1b[0m] " << msg)
#define logW(msg) _log("[\x1b[93mWarning\x1b[0m] " << msg)
#define logD(msg) if (_canlog(debug)) _log("[\x1b[96mDebug\x1b[0m] " << msg)
#define logC(msg) if (_canlog(call)) _log("[\x1b[98mCall\x1b[0m] " << msg)

#endif
