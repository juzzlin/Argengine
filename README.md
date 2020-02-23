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

# Design Choices

* Argument values are passed as strings. The client is responsible for possible data conversions.
* Most of the data is passed by value for a cleaner API. It is assumed that argument parsing is not the performance bottleneck of your application
* The PIMPL idiom is used to maintain API/ABI compatibility

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

# Usage In A Nutshell

The basic principle is that for each argument a lambda callback is added.

Valueless:

```
    ...

    juzzlin::Argengine ae(argc, argv);
    ae.addArgument({"-f", "--foo"}, [] {
        // Do something
    });
    ae.parse();

    ...
```

Single value:

```
    ...

    juzzlin::Argengine ae(argc, argv);
    ae.addArgument({"-f", "--foo"}, [] (std::string value) {
        // Do something with value
    });
    ae.parse();

    ...
```

There can be as many argument variants as liked, usually the short and long version e.g `-f` and `--foo`.

Argengine doesn't care about the naming of the arguments and they can be anything: `-f`, `a`, `/c`, `foo`, `--foo`..

# Examples

## Valueless arguments: The simplest possible example

Valueless arguments are arguments without any value, so they are just flags. The lambda callback is of the form `[] {}`.

```
#include "argengine.hpp"
#include <iostream>

int main(int argc, char ** argv)
{
    juzzlin::Argengine ae(argc, argv);
    ae.addArgument({"-f", "--foo"}, [] {
        std::cout << "'-f' or '--foo' given!"  << std::endl;
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
    juzzlin::Argengine ae(argc, argv);

    bool fooSet {};
    ae.addArgument({"-f", "--foo"}, [&] {
        fooSet = true;
    });

    bool barSet {};
    ae.addArgument({"--bar"}, [&] {
        barSet = true;
    });

    ae.parse();

    std::cout << fooSet << " " << barSet << std::endl;

    return 0;
}
```

## Single value arguments: The simplest possible example

Single value arguments are arguments that can have only one value.

The following formats are allowed: `-f 42`, `--foo=42`, `a=1`..

So, there must be either space or '`=`'.

The lambda callback is of the form `[] (std::string value) {}`.

```
#include "argengine.hpp"
#include <iostream>

int main(int argc, char ** argv)
{
    juzzlin::Argengine ae(argc, argv);
    ae.addArgument({"-f", "--foo"}, [] (std::string value) {
        std::cout << "Value for '-f' or '--foo': " << value << std::endl;
    });
    ae.parse();

    return 0;
}
```

## General: Marking an argument **required**

In order to mark an argument mandatory, there's an overload that accepts `bool required` right after the callback:

```
    ...

    juzzlin::Argengine ae(argc, argv);
    ae.addArgument({"-f", "--foo"}, [] {
        // Do something
    },
    true); // Required

    ...
```

## General: Handling unknown arguments

For the handling of unknown arguments there are three modes: `Ignore`, `Warn`, and `Throw`. The default is `Warn`.

This can be selected with `Argengine::setUnknownArgumentBehavior(UnknownArgumentBehavior behavior)`:

```
    ...

    using juzzlin::Argengine;
    Argengine ae(argc, argv);
    ae.setUnknownArgumentBehavior(Argengine::UnknownArgumentBehavior::Throw);

    ...
```

# Requirements

C++11

# Licence

MIT

# Projects currently using Argengine

* https://github.com/juzzlin/Heimer
