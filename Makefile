CXX = C:/msys64/mingw64/bin/g++.exe
CXXFLAGS = -Wall -std=c++17
TARGET = build/run.exe
OBJS = build/main.o

all: build $(TARGET)

build:
	mkdir -p build

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/main.o: main.cpp stb_image.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o $@ 

run: $(TARGET)
	$(TARGET)

clean:
	rm -rf build


