CXX=ccache g++
CFLAGS=-c -Wall -std=c++17 -g -fdiagnostics-color
LDFLAGS=-lstdc++fs -lyaml-cpp -ldl -lpthread -lSDL2
SOURCES=$(shell find src/ -name "*.cpp")
EXECUTABLE=AurumEmulator
OBJECTS=$(SOURCES:.cpp=.o)

#Set here your path to sdk and ndk
SDK_HOME=/android-sdk/
NDK_HOME=/android-ndk/

all: help

help:
		@echo -e "Avialible targets: "
		@echo -e "make linux"
#		@echo -e "make android"

# android: res
# 		@echo ">> AurumEmulator: Building for android"
# 		@echo -e ">> AurumEmulator: Clearing android-build/"
# 		rm -rf android-project/build
# 		@echo -e ">> AurumEmulator: Moving sources to android-build/"
# 		cp -ru src/ -T android-project/jni/src
# 		@echo -e ">> AurumEmulator: Compiling native code"
# 		$(NDK_HOME)/ndk-build NDK_PROJECT_PATH=./android-project
# 		@echo -e ">> AurumEmulator: Building apk"
# 		mkdir -p android-project/build/gen android-project/build/obj android-project/build/apk
# 		cp -r android-project/libs android-project/build/apk/lib
# 		$(SDK_HOME)/build-tools/27.0.3/aapt package -f -m -J android-project/build/gen/ -S android-project/src/main/res -M android-project/src/main/AndroidManifest.xml -I ${SDK_HOME}/platforms/android-14/android.jar
# 		javac -source 1.7 -target 1.7 -bootclasspath ${JAVA_HOME}/jre/lib/rt.jar -classpath $(SDK_HOME)/platforms/android-14/android.jar -classpath $(SDK_HOME)/platforms/android-16/android.jar -d android-project/build/obj android-project/build/gen/net/zabqer/aurumemulator/R.java android-project/src/main/java/net/zabqer/aurumemulator/AurumEmulatorActivity.java $(shell find android-project/src/main/java -name *.java)
# 		$(SDK_HOME)/build-tools/27.0.3/dx --dex --output=android-project/build/apk/classes.dex android-project/build/obj/
# 		$(SDK_HOME)/build-tools/27.0.3/aapt package -f -M android-project/src/main/AndroidManifest.xml -S android-project/src/main/res/ -I ${SDK_HOME}/platforms/android-14/android.jar -F android-project/build/AurumEmulator.unsigned.apk android-project/build/apk/
# 		$(SDK_HOME)/build-tools/27.0.3/zipalign -f 4 android-project/build/AurumEmulator.unsigned.apk android-project/build/AurumEmulator.aligned.apk
# 		$(SDK_HOME)/build-tools/27.0.3/apksigner sign --ks android-project/keystore.jks --ks-key-alias androidkey --ks-pass pass:android --key-pass pass:android --out android-project/build/AurumEmulator.apk android-project/build/AurumEmulator.aligned.apk

linux: res _start $(EXECUTABLE) clean

_start:
		rm -f $(EXECUTABLE)

res:
		bin2c src/resources/machine.lua src/resources/machine_lua.h MACHINE_LUA
		bin2c src/resources/bios.lua src/resources/bios_lua.h BIOS_LUA
		bin2c src/resources/font.hex src/resources/font_hex.h FONT_HEX
				
$(EXECUTABLE): $(OBJECTS)
		$(CXX) $(LDFLAGS) $(OBJECTS) -o $@ -lyaml-cpp
		#/usr/local/lib/libyaml-cpp.a /usr/lib/gcc/x86_64-linux-gnu/7/libstdc++fs.a

.cpp.o:
		$(CXX) $(CFLAGS) $< -o $@

clean:
		rm -rf $(OBJECTS)
