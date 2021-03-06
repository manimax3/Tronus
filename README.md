[![Build Status](https://travis-ci.org/manimax3/Tronus.svg?branch=develop)](https://travis-ci.org/manimax3/Tronus)
[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/manimax3/Tronus/blob/master/LICENSE)
![Language: C++17](https://img.shields.io/badge/language-C%2B%2B17-0afce4.svg)

# Tronus Engine

My little personal game engine for personal and maybe in the future commercial projects. The Engine is supposed to be crossplatform, but currently runs on Linux but Im working on it.

Contributions in form of Pull Requests are welcome if you want to know more about how to contribute take a look into the `Contribute` section.

If you just want to build the engine look into `Compile`.

##### (Future) Features
- Multithreaded
- Cross-platform
- Modern C++
- Extremly Customizeable
- Scripting with Lua
- 2D/3D Rendering with OpenGL (Maybe Vulkan in the future)
- 2D/3D Physics
- Entity-Component-System
- Unit Tests
- Profiling
- CI

## Compile

Compiling the engine should be pretty straight forward using CMake. All the currently required libraries are shipped with the engine using git submodules.

##### Requirements
- CMake
- C++ Compiler with C++17 support
- At least OpenGL 3.3 supporting GPU

##### Build Steps
```
git clone https://github.com/manimax3/Tronus.git
cd Tronus
git submodule init && git submodule update
mkdir build && cd build
cmake .. && cmake --build .
make install
```
Done

## Contribute

Contibutions in form of spell corrections, bug fixes, feature additons, etc. are always welcome. Especially for people who want to get into C++ development.

In order to contribute just fork the engine, make your changes and create a pull request. I would appreciate if you format the code using the provided .clang-format and create a little test if there isnt one.

If you don't know what you could work on just take a look at my [Trello](https://trello.com/b/oo6t4vjO/tronus).
