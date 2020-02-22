Argengine
=========

Looking for a simple argument parser logger for your C++ project? `Argengine` might be for you.

# Features

* Automatic help generation
* Builds with CMake
* Extensive unit tests
* Single .hpp/.cpp
* Very easy to use

# Design choices

* The PIMPL idiom is used to maintain API/ABI compatibility
* Most of the data is passed by value for a cleaner API. It is assumed that argument parsing is not the performance bottleneck of your application

# Installation

## Use from sources

Just add `src/argengine.hpp` and `src/argengine.cpp` to your project and start using it!

## Use as a CMake subproject

Copy contents of `Argengine` under your main project (or clone as a Git submodule).

In your `CMakeLists.txt`:

```
add_subdirectory(Argengine)
include_directories(Argengine/src)
```

Link to the library:

```
target_link_libraries(${YOUR_TARGET_NAME} Argengine)
```

In your code:

```
#include "argengine.hpp"
```

## Use as a library

`$ mkdir build && cd build`

`$ cmake ..`

`$ make`

`$ sudo make install`

# Examples

TODO

# Requirements

C++11

# Licence

MIT

# Projects currently using Argengine

* https://github.com/juzzlin/Heimer
