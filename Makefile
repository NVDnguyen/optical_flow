CXX = C:/msys64/mingw64/bin/g++.exe
CXXFLAGS = -Wall -std=c++17

TARGET = build/run.exe

OBJS = build/main.o build/optical_flow.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile main.c
build/main.o: main.c optical_flow.h stb_image.h
	$(CXX) $(CXXFLAGS) -c main.c -o $@

# Compile optical_flow.c
build/optical_flow.o: optical_flow.c optical_flow.h
	$(CXX) $(CXXFLAGS) -c optical_flow.c -o $@

run: $(TARGET)
	$(TARGET) frame1.jpg frame2.jpg

clean:
	rm -rf build

