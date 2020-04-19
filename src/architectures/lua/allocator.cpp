#include "allocator.h"
#include "../../machine.h"

#include "../../log.h"

class no_memory {};

void* allocator(void* ud, void* ptr, size_t osize, size_t nsize) {
		Machine* machine = static_cast<Machine*>(ud);
		if (nsize == 0) {
				machine->setUsedMemory(machine->getUsedMemory() - osize);
				free(ptr);
				return NULL;
		} else {
				size_t delta = 0;
				if (ptr == NULL) {
						delta = nsize;
				} else {
						delta = nsize - osize;
				}
				if ((machine->getUsedMemory() + delta) > machine->getTotalMemory()) {
						logW("Out of memory!");
						//throw no_memory();
						machine->crash("Out of memory!");
						return NULL;
				} else {
						machine->setUsedMemory(machine->getUsedMemory() + delta);
						void* p = realloc(ptr, nsize);
						if (!p) {
								logE("OH! We don't have heap memory to allocate lua object! Exiting!");
								exit(1);
						}
						return p;
				}
		}
}
