# GravitySim

Starting to work on a gravity simulator. Shows how velocity and mass affects the rotation period of two bodies/planets

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