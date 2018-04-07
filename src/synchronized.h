#ifndef __AURUM_SYNCHRONIZED_H__
#define __AURUM_SYNCHRONIZED_H__

#include <mutex>

#define synchronized(mutex) std::scoped_lock lock(mutex);

#endif