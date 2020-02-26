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

#ifndef JUZZLIN_ARGENGINE_HPP
#define JUZZLIN_ARGENGINE_HPP

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace juzzlin {

class Argengine
{
public:
    enum class HelpSorting
    {
        None,
        Ascending
    };

    //! Constructor.
    Argengine(int argc, char ** argv, bool addDefaultHelp = true);

    //! Constructor.
    //! \param args The arguments as a vector of strings. It is assumed, that the first element is the name of the executed application.
    using ArgumentVector = std::vector<std::string>;
    explicit Argengine(ArgumentVector args, bool addDefaultHelp = true);

    //! Destructor.
    ~Argengine();

    using ArgumentVariants = std::set<std::string>;
    using ValuelessCallback = std::function<void()>;
    void addArgument(ArgumentVariants argumentVariants, ValuelessCallback callback);

    void addArgument(ArgumentVariants argumentVariants, ValuelessCallback callback, bool required);

    void addArgument(ArgumentVariants argumentVariants, ValuelessCallback callback, bool required, std::string infoText);

    using SingleStringCallback = std::function<void(std::string)>;
    void addArgument(ArgumentVariants argumentVariants, SingleStringCallback callback);

    void addArgument(ArgumentVariants argumentVariants, SingleStringCallback callback, bool required);

    void addArgument(ArgumentVariants argumentVariants, SingleStringCallback callback, bool required, std::string infoText);

    using StringValueVector = std::vector<std::string>;
    using MultiStringCallback = std::function<void(StringValueVector)>;
    void addArgument(ArgumentVariants argumentVariants, MultiStringCallback callback);

    void addArgument(ArgumentVariants argumentVariants, MultiStringCallback callback, bool required);

    void addArgument(ArgumentVariants argumentVariants, MultiStringCallback callback, bool required, std::string infoText);

    //! \return All given arguments.
    ArgumentVector arguments() const;

    //! Set info text printed on help/usage before argument help.
    //! \param helpText Text shown in help. E.g. "MyApplication v1.0.0, Copyright (c) 2020 Foo Bar".
    void setHelpText(std::string helpText);

    //! Set sorting style of arguments in help.
    //! \param helpSorting The sorting direction enum. Default is HelpSorting::None.
    void setHelpSorting(HelpSorting helpSorting);

    //! Set handler for positional arguments.
    void setPositionalArgumentCallback(MultiStringCallback callback);

    //! Set custom output stream. Default is std::cout.
    //! \param out The new output stream.
    void setOutputStream(std::ostream & out);

    //! Set custom error/warning stream. Default is std::cerr.
    //! \param err The new error stream.
    void setErrorStream(std::ostream & err);

    //! Parses by using the current config.
    void parse();

    //! Prints help/usage.
    void printHelp();

private:
    Argengine(const Argengine & r) = delete;
    Argengine & operator=(const Argengine & r) = delete;

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // juzzlin

#endif // JUZZLIN_ARGENGINE_HPP
