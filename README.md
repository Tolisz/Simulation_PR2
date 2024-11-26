# Spinning Top Simulator

![Spinning Top](/docs/SpinningTop.png)

## About project 

This is the second project created during *"Physical simulations in a virtual environment"* course, specialty **CAD/CAM systems design** of degree in *Computer Science and Information Systems* at *Warsaw University of Technology*. 

The project is a simulation of a spinning top using Euler's equations describing rigid body dynamics. The simulation parameters are easily changeable via a convenient to use user interface. Additionally, various simulation presets were added to observe different behaviors of the spinning top. Colors of all rendered elements are changeable. For trajectory's line, there are two draw modes available: points and lines. The first one is good for observation of different integration step values.

## Compilation

The project is written in C++ (OpenGL) using CMake as a build system. It has two configuration CMake options: 

| Option | Description | 
| :---:         |     :---      |
| `PR2_EMBEDDED_SHADERS`   | If `ON` shader code will be embedded into a resulting app's binary. If `OFF` shader code will be read from `shaders` directory, so a binary has to be run from repository root directory.      |
| `PR2_SHOW_DEBUG_CONSOLE` | If `ON` - beside app's window - a debug console will be shown on app start up. If `OFF` only app's window will be shown.        |

### MS Windows

All subsequent commands should be called from Windows Power Shell in repository root directory.

```
cmake -S . -B build -DPR2_EMBEDDED_SHADERS=ON -DPR2_SHOW_DEBUG_CONSOLE=OFF
cmake --build build --config Release
```

Resulting binary will be located in `.\build\Release\SimulationPr2.exe` directory.

### Linux

```
It will be as soon as possible :)
```

### macOS

I am a poor man and I don't have **any** computer with macOS :(

## Used Libraries

Libraries `GLFW`, `GLM` and `ImGui` are used as submodules located in `externals` directory. `GLAD` is used as a generated file from [this](https://glad.dav1d.de/) web page. 

| Library | Licence |
| :---:   | :---:   |
| [GLFW](https://github.com/glfw/glfw) | [Zlib](https://github.com/glfw/glfw?tab=Zlib-1-ov-file#readme) |
| [GLM](https://github.com/g-truc/glm) | [Modified MIT](https://github.com/g-truc/glm?tab=License-1-ov-file#readme)|
| [ImGui](https://github.com/ocornut/imgui) | [MIT](https://github.com/ocornut/imgui?tab=MIT-1-ov-file#readme)| 
| [GLAD](https://github.com/Dav1dde/glad) | [MIT](https://github.com/Dav1dde/glad?tab=License-1-ov-file#readme) | 

