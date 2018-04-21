CC=ccache g++
CFLAGS=-c -Wall -std=c++17 -g -fdiagnostics-color
LDFLAGS=-lyaml-cpp -ldl -lboost_system -lboost_filesystem -lpthread
SOURCES=$(shell find src/ -name "*.cpp")
EXECUTABLE=AurumEmulator

OBJECTS=$(SOURCES:.cpp=.o)

all: _start res $(SOURCES) $(EXECUTABLE) clean _end

_start:
		@echo -e "\x1b[35m<===============> Building [AurumEmulator] <===============>\x1b[0m"
		rm -f $(EXECUTABLE)
		ccache -c

res:
		bin2c src/resources/machine.lua src/resources/machine_lua.h MACHINE_LUA
		bin2c src/resources/bios.lua src/resources/bios_lua.h BIOS_LUA
				
$(EXECUTABLE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -rf $(OBJECTS)
		rm -rf src/resources/machine_lua.h
		rm -rf src/resources/bios_lua.h
		
_end:
		@echo -e "\x1b[35m<================> Running [AurumEmulator] <===============>\x1b[0m"
		rm -f /a
		cp ./$(EXECUTABLE) /a
		chmod a+x /a
		@./$(EXECUTABLE) --path=./Env/
		@echo -e "\x1b[35m<=================> End [AurumEmulator] <==================>\x1b[0m"