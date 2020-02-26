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

#include <cstdlib>
#include <iostream>

using juzzlin::Argengine;

int main(int argc, char ** argv)
{
    Argengine ae(argc, argv);
    ae.addArgument({ "-a", "--arguments" }, [&] {
        for (int i = 0; i < argc; i++) {
            std::cout << argv[i] << std::endl;
        }
    });
    ae.addArgument({ "-p" }, [](std::string value) {
        std::cout << value.size() << std::endl;
    });

    try {
        ae.parse();
    } catch (const std::runtime_error & e) {
        std::cerr << e.what() << std::endl
                  << std::endl;
        ae.printHelp();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
