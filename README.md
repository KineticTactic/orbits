# Orbits! - A Newtonian Gravity Sandbox

**Orbits!** is a 2D gravity simulation built in C++ using [raylib](https://www.raylib.com/). It simulates Newtonian gravitational interactions between multiple bodies and visualizes their motion and orbital paths in real time. It Uses **Velocity Verlet** integration for stable, energy-preserving orbits.

![orbits](assets/orbits.gif)

# Controls

-   **Left Click Drag** to add a body and set its velocity. **Scroll** to change the mass of the body.
-   **Right Click Drag** to pan the view.
-   **Scroll** to zoom in and out.
-   **Space** to pause the simulation.

# Building

Use CMake to build the project. Raylib is automatically downloaded and built as a dependency.

```bash
git clone https://github.com/KineticTactic/orbits.git
cd orbits
mkdir build
cd build

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .. 	# -G "MinGW Makefiles"
make 										# or mingw32-make
```

orbits.exe will be created in the build directory.
