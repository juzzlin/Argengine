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

Positional arguments (for example a file name for a text editor after other options) can be received with a single callback:

```
    ...

    juzzlin::Argengine ae(argc, argv);
    ae.setPositionalArgumentCallback([] (std::vector<std::string> args) {
        // Do something with arguments
    });
    ae.parse();

    ...
```

If callback for positional arguments is set, then no errors about `unknown arguments` will occur as all additional arguments will be taken as positional arguments.

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

## Single value arguments: The simplest possible example

Single value arguments are arguments that can have only one value.

As an example, for argument `-f` The following formats are allowed: `-f 42`, `-f42`, `-f=42`.

Preferably there should be either space or '`=`'. The spaceless format is accepted if not ambiguous.

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

## General: Error handling

For error handling there are two options: exceptions or error value.

`Argengine::parse()` will throw on error, `Argengine::parse(Error & error)` will set and error.

Example of handling exceptions:

```
    ...

    try {
        ae.parse();
    } catch(std::runtime_error & e) {
        std::cerr << e.what() << std::endl;
        ae.printHelp();
        return EXIT_FAILURE;
    }

    ...
```

Example of handling error values:

```
    ...
    Argengine::Error error;
    ae.parse(error);

    if (error.code != Argengine::Error::Code::Ok) {
        std::cerr << error.message << std::endl
                  << std::endl;
        ae.printHelp();
        return EXIT_FAILURE;
    }

    ...
```

# Requirements

C++11

# Licence

MIT

# Projects currently using Argengine

* https://github.com/juzzlin/Heimer
