# GravitySim

This is a gravity simulator that I worked on my first semester of my sophomore year of college. Shows how velocity and mass affects the rotation period of two or more bodies/planets and how mass distorts space time with a grid.

## How to Run

### Prerequisites
1. Install [MSYS2](https://www.msys2.org/).
2. Install the following packages using the MSYS2 terminal:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-glfw mingw-w64-ucrt-x86_64-glew
   ```
3. Ensure the following libraries are available:
   - GLFW
   - GLEW
   - OpenGL

### Build Instructions
1. Open the project in Visual Studio Code.
2. Ensure the `tasks.json` and `launch.json` files are correctly configured for your desired view (2D or 3D).
3. Use the default build task:
   - Press `Ctrl+Shift+B` and select "C/C++: g++.exe build active file".

### Run Instructions
1. After building, navigate to the `src` directory.
2. Run the appropriate executable:
   - For 3D view: `GravitySim3D.exe`
   - For 2D view: `GravitySim.exe`

### Notes
- To switch between a 3D view and 2D view, update both `launch.json` and `tasks.json` accordingly.
- Press 'esc' to leave simulation
- Press 'space' to pause the simulation
- Press 'z' to let the mouse exit the simulation just if you want to move the window

### Units and scaling
This simulation uses scaled units to keep numeric values reasonable and the simulation stable and visible. `GravConst` (G) is intentionally adjusted in the code; masses and distances in the examples are scaled and do not directly map to SI units unless you re-scale G, masses, and distances consistently.
