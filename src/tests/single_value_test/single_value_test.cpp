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

using juzzlin::Argengine;

const std::string name = "Argengine";

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
    assert(error == name + ": No value for argument '-f' given!");
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
    Argengine ae({ "test", "f=42", "g==" });
    std::string f, g;
    ae.addArgument({ "f" }, [&](std::string value) {
        f = value;
    });
    ae.addArgument({ "g" }, [&](std::string value) {
        g = value;
    });
    std::string error;
    ae.parse();
    assert(f == "42");
    assert(g == "=");
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
    assert(error == name + ": No value for argument 'f' given!");
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

    assert(values["a"] == "1");
    assert(values["bb"] == "22");
    assert(values["ccc"] == "333");
}

void testSingleValue_NoValueGivenWithoutSpace_ShouldFail()
{
    Argengine ae({ "test", "f" });
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
    assert(error == name + ": No value for argument 'f' given!");
}

void testSingleValue_ValueGivenWithoutSpace_ShouldSucceed()
{
    Argengine ae({ "test", "-o1" });
    std::string o;
    ae.addArgument({ "-o" }, [&](std::string value) {
        o = value;
    });
    std::string error;
    ae.parse();
    assert(o == "1");
}

void testSingleValue_MultipleValuesGivenWithoutSpaces_ShouldSucceed()
{
    Argengine ae({ "test", "a1", "bb22", "ccc333" });
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

    assert(values["a"] == "1");
    assert(values["bb"] == "22");
    assert(values["ccc"] == "333");
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

void testSingleValue_RequiredButNotGiven_ShouldFail()
{
    Argengine ae({ "test" });
    ae.addArgument(
      { "-f", "--foo" }, [](std::string) {
      },
      true);

    std::string error;
    try {
        ae.parse();
    } catch (std::runtime_error & e) {
        error = e.what();
    }

    assert(error == name + ": Argument '-f, --foo' is required!");
}

void testSingleValue_RequiredAndGiven_ShouldSucceed()
{
    Argengine ae({ "test", "--foo", "42", "--bar=666" });
    std::string fooValue;
    ae.addArgument(
      { "-f", "--foo" }, [&](std::string value) {
          fooValue = value;
      },
      true);
    std::string barValue;
    ae.addArgument(
      { "-b", "--bar" }, [&](std::string value) {
          barValue = value;
      },
      true);

    ae.parse();

    assert(fooValue == "42");
    assert(barValue == "666");
}

void testMixedArguments_MultipleArguments_ShouldSucceed()
{
    Argengine ae({ "test", "-a", "1", "--bbb", "-c", "3", "-d444" });
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
    ae.addArgument({ "-d" }, [&](std::string value) {
        values["d"] = value;
    });
    std::string error;
    ae.parse();
    assert(values["a"] == "1");
    assert(values["bbb"] == "called");
    assert(values["c"] == "3");
    assert(values["d"] == "444");
}

int main(int, char **)
{
    testSingleValue_NoValueGiven_ShouldFail();

    testSingleValue_ValueGiven_ShouldSucceed();

    testSingleValue_MultipleValueArguments_ShouldSucceed();

    testSingleValue_NoValueGivenWithAssignment_ShouldFail();

    testSingleValue_ValueGivenWithAssignment_ShouldSucceed();

    testSingleValue_MultipleValuesGivenWithAssignments_ShouldSucceed();

    testSingleValue_NoValueGivenWithoutSpace_ShouldFail();

    testSingleValue_ValueGivenWithoutSpace_ShouldSucceed();

    testSingleValue_MultipleValuesGivenWithoutSpaces_ShouldSucceed();

    testSingleValue_RequiredButNotGiven_ShouldFail();

    testSingleValue_RequiredAndGiven_ShouldSucceed();

    testMixedArguments_MultipleArguments_ShouldSucceed();

    return EXIT_SUCCESS;
}