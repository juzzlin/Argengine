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
    //! Sorting order of arguments in help.
    enum class HelpSorting
    {
        None,
        Ascending
    };

    //! Error structure set by parse().
    struct Error
    {
        enum class Code
        {
            Ok,
            Failed
        };

        Code code = Code::Ok;

        std::string message;
    };

    //! Constructor.
    //! \param argc Argument count as in `main(int argc, char ** argv)`
    //! \param argv Argument array as in `main(int argc, char ** argv)`
    //! \param addDefaultHelp If true, a default help action for "-h" and "--help" is added.
    Argengine(int argc, char ** argv, bool addDefaultHelp = true);

    //! Constructor.
    //! \param args The arguments as a vector of strings. It is assumed, that the first element is the name of the executed application.
    //! \param addDefaultHelp If true, a default help action for "-h" and "--help" is added.
    using ArgumentVector = std::vector<std::string>;
    explicit Argengine(ArgumentVector args, bool addDefaultHelp = true);

    //! Destructor.
    ~Argengine();

    //! Adds a valueless argument to the configuration.
    //! \param argumentVariants A set of arguments for the given action, usually the short and long form: {"-f", "--foo"}
    //! \param callback Callback to be called when the argument has been given. Signature: `void()`.
    //! \param required If true, an error will follow if the argument is not present during parse().
    //! \param infoText Short info text shown in help/usage.
    using ArgumentVariants = std::set<std::string>;
    using ValuelessCallback = std::function<void()>;
    void addArgument(ArgumentVariants argumentVariants, ValuelessCallback callback, bool required = false, std::string infoText = "");

    //! Adds an argument with a single value to the configuration.
    //! \param argumentVariants A set of arguments for the given action, usually the short and long form: {"-f", "--foo"}
    //! \param callback Callback to be called when the argument has been given with a value. Signature: `void(std::string)`.
    //! \param required \see addArgument(ArgumentVariants argumentVariants, SingleStringCallback callback, bool required).
    //! \param infoText Short info text shown in help/usage.
    using SingleStringCallback = std::function<void(std::string)>;
    void addArgument(ArgumentVariants argumentVariants, SingleStringCallback callback, bool required = false, std::string infoText = "");

    //! Special method to add custom help. Help is always executed first if present.
    //! \param argumentVariants A set of arguments for the given action, usually the short and long form: {"-h", "--help"}
    //! \param callback Callback to be called when the argument has been given. Signature: `void()`.
    void addHelp(ArgumentVariants argumentVariants, ValuelessCallback callback);

    //! \return All given arguments.
    ArgumentVector arguments() const;

    //! Set info text printed on help/usage before argument help.
    //! \param helpText Text shown in help. E.g. "MyApplication v1.0.0, Copyright (c) 2020 Foo Bar".
    void setHelpText(std::string helpText);

    //! Set sorting style of arguments in help.
    //! \param helpSorting The sorting direction enum. Default is HelpSorting::None.
    void setHelpSorting(HelpSorting helpSorting);

    //! Set handler for positional arguments.
    using StringValueVector = std::vector<std::string>;
    using MultiStringCallback = std::function<void(StringValueVector)>;
    void setPositionalArgumentCallback(MultiStringCallback callback);

    //! Set custom output stream. Default is std::cout.
    //! \param out The new output stream.
    void setOutputStream(std::ostream & out);

    //! Parses by using the current config. Throws `std::runtime_error` on error.
    void parse();

    //! Parses by using the current config.
    //! \param error Contains error info error.
    void parse(Error & error);

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
