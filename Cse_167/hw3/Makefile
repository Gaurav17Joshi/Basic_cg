# Makefile for Ray Tracer

# Compiler
CXX = g++

# Compiler flags
# -std=c++11: Use the C++11 standard
# -Wall: Enable all warnings
# -O3: Optimization level 3
# -I/opt/homebrew/include: Tell compiler where to find FreeImage.h
CXXFLAGS = -std=c++11 -Wall -O3 -I/opt/homebrew/include

# Linker flags
# -L/opt/homebrew/lib: Tell linker where to find libFreeImage
# -lfreeimage: Link against the FreeImage library
LDFLAGS = -L/opt/homebrew/lib -lfreeimage

# Executable name
TARGET = raytracer

# Source files
# Find all .cpp files in the src directory
SRCS = $(wildcard src/*.cpp)

# Object files
# Replace the .cpp extension with .o
OBJS = $(SRCS:.cpp=.o)

# Default target
# This is what 'make' will do by default
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile a .cpp file into a .o file
# $< is the first prerequisite (the .cpp file)
# $@ is the target (the .o file)
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
