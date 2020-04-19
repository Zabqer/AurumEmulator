#ifndef __AURUM_SYNCHRONIZED_H__
#define __AURUM_SYNCHRONIZED_H__

#include <mutex>

#define CONCATT(a, b) a##b
#define CONCAT(a, b) CONCATT(a, b)
#define QUOTE(a) #a

#define synchronized(mutex) std::scoped_lock CONCAT(lock_line_, __LINE__)(mutex)

#endif