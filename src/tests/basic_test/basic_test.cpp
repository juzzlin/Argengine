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
#include <map>
#include <sstream>

using juzzlin::Argengine;

void testValueless_CallbackCalled_ShouldFail()
{
    Argengine ae({ "test" });
    bool triggered {};
    ae.addArgument({ "-f", "--foo" }, [&] {
        triggered = true;
    });
    ae.parse();

    assert(!triggered);
}

void testValueless_CallbackCalled_ShouldSucceed()
{
    Argengine ae({ "test", "-f" });
    bool triggered {};
    ae.addArgument({ "-f", "--foo" }, [&] {
        triggered = true;
    });
    ae.parse();

    assert(triggered);
}

void testValueless_MultipleCallbacksCalled_ShouldSucceed()
{
    Argengine ae({ "test", "-a", "--bbb", "--aaa", "-a", "-b" });
    std::map<std::string, size_t> triggered;
    ae.addArgument({ "-a", "--aaa" }, [&] {
        triggered["a"]++;
    });
    ae.addArgument({ "-b", "--bbb" }, [&] {
        triggered["b"]++;
    });
    ae.parse();
    assert(triggered["a"] == 3);
    assert(triggered["b"] == 2);
}

void testSingleValue_NoValueGiven_ShouldFail()
{
    Argengine ae({ "test", "-f" });
    bool called {};
    ae.addArgument({ "-f" }, [&](std::string) {
        called = true;
    });
    std::string error;
    try {
        ae.parse();
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(!called);
    assert(error == "No value for argument '-f' given!");
}

void testSingleValue_ValueGiven_ShouldSucceed()
{
    Argengine ae({ "test", "-f", "42" });
    std::string f;
    ae.addArgument({ "-f" }, [&](std::string value) {
        f = value;
    });
    std::string error;
    ae.parse();
    assert(f == ae.arguments().at(2));
}

void testSingleValue_ValueGivenWithAssignment_ShouldSucceed()
{
    Argengine ae({ "test", "f=42" });
    std::string f;
    ae.addArgument({ "f" }, [&](std::string value) {
        f = value;
    });
    std::string error;
    ae.parse();
    assert(f == "42");
}

void testSingleValue_NoValueGivenWithAssignment_ShouldFail()
{
    Argengine ae({ "test", "f=" });
    bool called {};
    ae.addArgument({ "f" }, [&](std::string) {
        called = true;
    });
    std::string error;
    try {
        ae.parse();
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(!called);
    assert(error == "No value for argument 'f' given!");
}

void testSingleValue_MultipleValuesGivenWithAssignments_ShouldSucceed()
{
    Argengine ae({ "test", "a=1", "bb=22", "ccc=333" });
    std::map<std::string, std::string> values;
    ae.addArgument({ "a", "aa" }, [&](std::string value) {
        values["a"] = value;
    });
    ae.addArgument({ "bb" }, [&](std::string value) {
        values["bb"] = value;
    });
    ae.addArgument({ "ccc" }, [&](std::string value) {
        values["ccc"] = value;
    });
    std::string error;
    ae.parse();
}

void testSingleValue_MultipleValueArguments_ShouldSucceed()
{
    Argengine ae({ "test", "-a", "1", "-b", "2", "-c", "3" });
    std::map<std::string, std::string> values;
    ae.addArgument({ "-a" }, [&](std::string value) {
        values["a"] = value;
    });
    ae.addArgument({ "-b" }, [&](std::string value) {
        values["b"] = value;
    });
    ae.addArgument({ "-c" }, [&](std::string value) {
        values["c"] = value;
    });
    std::string error;
    ae.parse();
    assert(values["a"] == "1");
    assert(values["b"] == "2");
    assert(values["c"] == "3");
}

void testMixedArguments_MultipleArguments_ShouldSucceed()
{
    Argengine ae({ "test", "-a", "1", "--bbb", "-c", "3" });
    std::map<std::string, std::string> values;
    ae.addArgument({ "-a" }, [&](std::string value) {
        values["a"] = value;
    });
    ae.addArgument({ "--bbb" }, [&] {
        values["bbb"] = "called";
    });
    ae.addArgument({ "-c" }, [&](std::string value) {
        values["c"] = value;
    });
    std::string error;
    ae.parse();
    assert(values["a"] == "1");
    assert(values["bbb"] == "called");
    assert(values["c"] == "3");
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
    Argengine ae({ "test", "--foo1" });
    std::string error;
    ae.addArgument({ "--bar" }, [] {
    });
    ae.setUnknownArgumentBehavior(Argengine::UnknownArgumentBehavior::Ignore);
    ae.parse();
}

void testUnknownArgumentBehavior_SetThrow_ShouldThrow()
{
    Argengine ae({ "test", "--foo2" });
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
    Argengine ae({ "test", "--foo3" });
    ae.addArgument({ "--bar" }, [] {
    });

    ae.setUnknownArgumentBehavior(Argengine::UnknownArgumentBehavior::Warn);
    std::stringstream ss;
    ae.setErrorStream(ss);
    ae.parse();

    assert(ss.str() == "Uknown argument '" + ae.arguments().at(1) + "'!\n");
}

void testUnknownArgumentBehavior_DefaultIsWarn_ShouldWarn()
{
    Argengine ae({ "test", "--foo4" });
    ae.addArgument({ "--bar" }, [] {
    });

    std::stringstream ss;
    ae.setErrorStream(ss);
    ae.parse();

    assert(ss.str() == "Uknown argument '" + ae.arguments().at(1) + "'!\n");
}

int main(int, char **)
{
    testValueless_CallbackCalled_ShouldFail();

    testValueless_CallbackCalled_ShouldSucceed();

    testValueless_MultipleCallbacksCalled_ShouldSucceed();

    testSingleValue_NoValueGiven_ShouldFail();

    testSingleValue_ValueGiven_ShouldSucceed();

    testSingleValue_MultipleValueArguments_ShouldSucceed();

    testSingleValue_NoValueGivenWithAssignment_ShouldFail();

    testSingleValue_ValueGivenWithAssignment_ShouldSucceed();

    testSingleValue_MultipleValuesGivenWithAssignments_ShouldSucceed();

    testMixedArguments_MultipleArguments_ShouldSucceed();

    testDefaultHelpOverride_HelpActive_ShouldFail();

    testDefaultHelpOverride_HelpNotActive_ShouldSucceed();

    testDefaultHelp_PrintToStream_ShouldSucceed();

    testDefaultHelp_ClearHelpText_ShouldSucceed();

    testUnknownArgumentBehavior_SetIgnore_ShouldIgnore();

    testUnknownArgumentBehavior_SetThrow_ShouldThrow();

    testUnknownArgumentBehavior_SetWarn_ShouldWarn();

    testUnknownArgumentBehavior_DefaultIsWarn_ShouldWarn();

    return EXIT_SUCCESS;
}
