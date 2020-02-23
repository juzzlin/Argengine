Argengine
=========

Looking for a simple CLI argument parser for your C++ project? `Argengine` might be for you.

# Features

* Automatic help generation
* Based on simple lambda callbacks
* Builds with CMake
* Designed for modern C++
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

The basic principle is that for each argument a lambda callback is added. 

For valueless arguments it's of the form `[] {}` and for single value arguments it's of the form `[] (std::string value) {}`.

There can be as many argument variants as liked, usually the short and long version e.g `-f` and `--foo`.

## Valueless arguments: The simplest possible example

```
#include "argengine.hpp"
#include <iostream>

int main(int argc, char ** argv)
{
    using juzzlin::Argengine;
    Argengine ae(argc, argv);
    ae.addArgument({"-f", "--foo"}, [] {
        std::cout << "-f or --foo set!"  << std::endl;
    });
    ae.parse();

    return 0;
}
```

## Valueless arguments: Set a flag when set

Flags can be set by using standard lambda captures:

```
#include "argengine.hpp"
#include <iostream>

int main(int argc, char ** argv)
{
    using juzzlin::Argengine;
    Argengine ae(argc, argv);

    bool fooSet = false;
    ae.addArgument({"-f", "--foo"}, [&] {
        fooSet = true;
    });

    bool barSet = false;
    ae.addArgument({"--bar"}, [&] {
        barSet = true;
    });

    ae.parse();

    std::cout << fooSet << " " << barSet << std::endl;

    return 0;
}
```

# Requirements

C++11

# Licence

MIT

# Projects currently using Argengine

* https://github.com/juzzlin/Heimer
