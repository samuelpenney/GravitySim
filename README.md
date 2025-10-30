# GravitySim - Linux Version

This is a gravity simulator that shows how velocity and mass affects the rotation period of two or more bodies/planets and how mass distorts space time with a grid. This version has been updated to run on Linux systems.

The project contains two gravity simulation programs:
- **GravitySim**: A 2D gravity simulation 
- **GravitySim3D**: A 3D gravity simulation with curved spacetime visualization

## Prerequisites

You need to install the following dependencies on Linux:

```bash
sudo apt update
sudo apt install libglfw3-dev libglew-dev libglm-dev libgl1-mesa-dev libglu1-mesa-dev build-essential
```

For other distributions:
- **Fedora/RHEL**: `sudo dnf install glfw-devel glew-devel glm-devel mesa-libGL-devel mesa-libGLU-devel`
- **Arch Linux**: `sudo pacman -S glfw glew glm mesa`

## Building

### Using Makefile (Recommended)

```bash
# Build both simulations
make all

# Build only 3D simulation
make 3d

# Build only 2D simulation  
make 2d

# Clean build files
make clean

# Show help
make help
```

### Using VS Code

1. Open the project in Visual Studio Code.
2. Press `Ctrl+Shift+B` and select "C/C++: g++ build active file".
3. Or choose "C/C++: g++ build GravitySim.cpp" for the 2D simulation.

## Running

### 3D Gravity Simulation
```bash
./src/GravitySim3D
```

**Controls:**
- `W/A/S/D`: Move camera
- `Shift + W/A/S/D`: Move camera faster
- `Mouse`: Look around
- `Z`: Toggle cursor capture
- `Space`: Pause/resume simulation
- `Esc`: Exit

### 2D Gravity Simulation
```bash
./src/GravitySim
```

This shows a simpler 2D view of gravitational interactions between objects.

## Debugging

To debug the programs in VS Code:

1. Set breakpoints in VS Code
2. Press `F5` or use the "Run and Debug" panel
3. Choose either:
   - "Debug GravitySim3D" 
   - "Debug GravitySim"
   - "Debug Active File"

## Features

### 3D Simulation Features
- Real-time 3D visualization of gravitational bodies
- Interactive camera with mouse and keyboard controls
- Curved spacetime grid showing gravitational field distortion
- Collision detection between objects
- Pause/resume functionality

### 2D Simulation Features  
- Simple 2D orbital mechanics
- Grid background for reference
- Real-time physics calculation display

## Units and Scaling

This simulation uses scaled units to keep numeric values reasonable and the simulation stable and visible. `GravConst` (G) is intentionally adjusted in the code; masses and distances in the examples are scaled and do not directly map to SI units unless you re-scale G, masses, and distances consistently.

## Troubleshooting

If you encounter compilation errors:

1. Ensure all dependencies are installed
2. Check that your graphics drivers support OpenGL
3. Try running with software rendering: `LIBGL_ALWAYS_SOFTWARE=1 ./src/GravitySim3D`

## Project Structure

```
LinuxBranch/
├── .vscode/           # VS Code configuration files (Linux-compatible)
├── src/
│   ├── GravitySim.cpp     # 2D simulation source
│   ├── GravitySim3D.cpp   # 3D simulation source
│   ├── GravitySim         # 2D simulation executable
│   └── GravitySim3D       # 3D simulation executable
├── Makefile           # Build configuration for Linux
└── README.md          # This file
```
