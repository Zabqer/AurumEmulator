CC=ccache g++
CFLAGS=-c -Wall -std=c++17
LDFLAGS=-lyaml-cpp
SOURCES=$(shell find src/ -name "*.cpp")
EXECUTABLE=AurumEmulator

OBJECTS=$(SOURCES:.cpp=.o)

all: _start $(SOURCES) $(EXECUTABLE) clean _end

_start:
		@echo -e "\x1b[35m<===============> Building [AurumEmulator] <===============>\x1b[0m"
		rm -f $(EXECUTABLE)
		ccache -c
		
$(EXECUTABLE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -rf $(OBJECTS)
		
_end:
		@echo -e "\x1b[35m<================> Running [AurumEmulator] <===============>\x1b[0m"
		@./$(EXECUTABLE) --path=./Env/
		@echo -e "\x1b[35m<=================> End [AurumEmulator] <==================>\x1b[0m"