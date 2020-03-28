# PlotSim: 2D Plotter Simulator


## Introduction
PlotSim simulates a 2D plotter driven by two stepper motors. A pulse in the driving signal can move the motor by one step. You can control the plotting trajectory by sending pulse trains of varying frequencies to each motor.

The following protected method in class [`Simulator`](lib/simulator.hh) exposes the emulated hardware and allows you to generate pulses.
```c++
/**
 * @brief Generate a pulse at a specific clock cycle.
 * @param clk Simulator clock cycle.
 * @param pulx Step x axis.
 * @param puly Step y axis.
 * @param dirx Positive x direction.
 * @param diry Positive y direction.
 * @param down Pen down.
 */
void setpin(std::int64_t clk, bool pulx, bool puly,
            bool dirx, bool diry, bool down);
```
Just as physical motors, the emulated motors have speed and acceleration limitations which are configurable in [`config.hh`](lib/config.hh) together with many other parameters. And you must always advance in time when calling `setpin` repeatedly.

## Implement Your Own Plotter
You should implement your hardware logic in a subclass of [`Simulator`](lib/simulator.hh) and override pure virtual method `hardwareLoop` where `setpin` can be called. `hardwareLoop` will be invoked repeatedly and indefinitely by the simulator. Additionally, you can specify Arduino style init and loop functions as your user level program.

There are two paradiams of using PlotSim:
- Put all user code in `hardwareLoop`. It gives you full control over the emulated hardware, similar to what you do on a FPGA. [A stencil is given in `part1/`](part1/).
- Implement emulated peripherals, e.g. Timers and GPIO. The peripheral logic should be handled by `hardwareLoop` while the plotting logic is written in user init and loop, similar to programming a microcontroller. [A stencil is given in `part2/`](part2/).

## Build
[Install SDL](https://wiki.libsdl.org/Installation). We use SDL as the graphical backend to increase cross-platform compatability.

Use CMake and make to build the project by running the following shell commands. It will generate two executables `plotsim1` and `plotsim2` for part1 and part2 respectively.
```
$ mkdir build; cd build;
$ cmake ..
$ make
```
Alternatively, you can [work with a CMake project directly in Visual Studio](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=vs-2019) or change [CMake generator](https://cmake.org/cmake/help/v3.14/manual/cmake-generators.7.html) to build it with your favorite IDE. For example, the following command will generate an Xcode project.
```
$ cmake -GXcode
```

## Copyright
This project is made for ENGN 2920F Sensors and Actuators for Real Systems at [Brown University](brown.edu) in response to restricted lab access due to COVID-19.

ENGN 2920F is designed and taught by [Prof. Harvey Silverman](mailto:hfs@lems.brown.edu). Simulation core is designed by [Ken Silverman](http://advsys.net/ken). This project is maintained by Brown graduate student [Haoze Zhang](mailto:haoze_zhang@brown.edu).

Distributed under MIT license.
