CXX = C:/msys64/mingw64/bin/g++.exe
CXXFLAGS = -Wall -std=c++17

TARGET = build/run.exe

OBJS = build/main.o build/nv_optical_flow.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile main.c
build/main.o: main.c frame1.h frame2.h nv_optical_flow.h stb_image.h
	$(CXX) $(CXXFLAGS) -c main.c -o $@

# Compile nv_optical_flow.c
build/nv_optical_flow.o: nv_optical_flow.c nv_optical_flow.h
	$(CXX) $(CXXFLAGS) -c nv_optical_flow.c -o $@
