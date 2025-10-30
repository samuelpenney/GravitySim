# Makefile for Gravity Simulation
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -fdiagnostics-color=always

# Libraries needed for OpenGL, GLFW, and GLEW on Linux
LIBS = -lglfw -lGL -lGLU -lGLEW -lX11 -lpthread -lXrandr -lXi -ldl

# Source directory
SRCDIR = src

# Executables
TARGETS = $(SRCDIR)/GravitySim3D $(SRCDIR)/GravitySim

.PHONY: all clean 3d 2d

all: $(TARGETS)

3d: $(SRCDIR)/GravitySim3D

2d: $(SRCDIR)/GravitySim

$(SRCDIR)/GravitySim3D: $(SRCDIR)/GravitySim3D.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)

$(SRCDIR)/GravitySim: $(SRCDIR)/GravitySim.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ -lglfw -lGL -lGLU -lX11 -lpthread -lXrandr -lXi -ldl

clean:
	rm -f $(TARGETS)

install-deps:
	@echo "Installing required dependencies..."
	@echo "Run the following commands to install dependencies:"
	@echo "sudo apt update"
	@echo "sudo apt install libglfw3-dev libglew-dev libglm-dev libgl1-mesa-dev libglu1-mesa-dev"

help:
	@echo "Available targets:"
	@echo "  all        - Build both 2D and 3D gravity simulations"
	@echo "  3d         - Build only the 3D gravity simulation"
	@echo "  2d         - Build only the 2D gravity simulation"
	@echo "  clean      - Remove all built executables"
	@echo "  install-deps - Show commands to install required dependencies"
	@echo "  help       - Show this help message"