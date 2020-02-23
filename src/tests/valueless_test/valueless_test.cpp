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

int main(int, char **)
{
    testValueless_CallbackCalled_ShouldFail();

    testValueless_CallbackCalled_ShouldSucceed();

    testValueless_MultipleCallbacksCalled_ShouldSucceed();

    return EXIT_SUCCESS;
}
