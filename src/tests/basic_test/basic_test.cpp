// MIT License
//
// Copyright (c) 2020 Jussi Lind <jussi.lind@iki.fi>
//
// https://github.com/juzzlin/Argengine
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "argengine.hpp"

// Don't compile asserts away
#ifdef NDEBUG
    #undef NDEBUG
#endif

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>

using juzzlin::Argengine;

void testSimpleValueless_CallbackCalled_ShouldFail()
{
    Argengine ae({ "test" });
    bool triggered = false;
    ae.addArgument({ "-f", "--foo" }, [&] {
        triggered = true;
    });
    ae.parse();

    assert(!triggered);
}

void testSimpleValueless_CallbackCalled_ShouldSucceed()
{
    Argengine ae({ "test", "-f" });
    bool triggered = false;
    ae.addArgument({ "-f", "--foo" }, [&] {
        triggered = true;
    });
    ae.parse();

    assert(triggered);
}

void testDefaultHelpOverride_HelpActive_ShouldFail()
{
    Argengine ae({ "test" });
    std::string error;
    try {
        ae.addArgument({ "-h" }, [&] {
        });
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(error == "Argument '-h, --help' already defined!");
}

void testDefaultHelpOverride_HelpNotActive_ShouldSucceed()
{
    Argengine ae({ "test" }, false);
    std::string error;
    try {
        ae.addArgument({ "-h" }, [&] {
        });
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(error == "");
}

void testDefaultHelp_PrintToStream_ShouldSucceed()
{
    Argengine ae({ "test" });
    std::stringstream ss;
    ae.setOutputStream(ss);
    ae.printHelp();

    assert(ss.str() == "Usage: " + ae.arguments().at(0) + " [OPTIONS]\n\nOptions:\n\n");
}

void testDefaultHelp_ClearHelpText_ShouldSucceed()
{
    Argengine ae({ "test" });
    std::stringstream ss;
    ae.setOutputStream(ss);
    ae.setHelpText("");
    ae.printHelp();

    assert(ss.str() == "Options:\n\n");
}

void testUnknownArgumentBehavior_SetIgnore_ShouldIgnore()
{
    Argengine ae({ "test", "--foo" });
    std::string error;
    ae.addArgument({ "--bar" }, [] {
    });
    ae.parse();

    ae.setUnknownArgumentBehavior(Argengine::UnknownArgumentBehavior::Ignore);
    ae.parse();
}

void testUnknownArgumentBehavior_SetThrow_ShouldThrow()
{
    Argengine ae({ "test", "--foo" });
    ae.addArgument({ "--bar" }, [] {
    });

    ae.setUnknownArgumentBehavior(Argengine::UnknownArgumentBehavior::Throw);
    std::string error;
    try {
        ae.parse();
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(error == "Uknown argument '" + ae.arguments().at(1) + "'!");
}

void testUnknownArgumentBehavior_SetWarn_ShouldWarn()
{
    Argengine ae({ "test", "--foo" });
    ae.addArgument({ "--bar" }, [] {
    });

    ae.setUnknownArgumentBehavior(Argengine::UnknownArgumentBehavior::Warn);
    std::stringstream ss;
    ae.setErrorStream(ss);
    ae.parse();

    assert(ss.str() == "Uknown argument '" + ae.arguments().at(1) + "'!\n");
}

int main(int, char **)
{
    testSimpleValueless_CallbackCalled_ShouldFail();

    testSimpleValueless_CallbackCalled_ShouldSucceed();

    testDefaultHelpOverride_HelpActive_ShouldFail();

    testDefaultHelpOverride_HelpNotActive_ShouldSucceed();

    testDefaultHelp_PrintToStream_ShouldSucceed();

    testDefaultHelp_ClearHelpText_ShouldSucceed();

    testUnknownArgumentBehavior_SetIgnore_ShouldIgnore();

    testUnknownArgumentBehavior_SetThrow_ShouldThrow();

    testUnknownArgumentBehavior_SetWarn_ShouldWarn();

    return EXIT_SUCCESS;
}