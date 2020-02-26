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

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>

namespace juzzlin {

class Argengine::Impl
{
public:
    Impl(const ArgumentVector & args, bool addDefaultHelp)
      : m_args(args)
    {
        if (m_args.empty()) {
            throw std::runtime_error(name() + ": Argument vector is empty!");
        }

        if (addDefaultHelp) {
            addHelp();
        }
    }

    void addArgument(ArgumentVariants argumentVariants, ValuelessCallback callback, bool required, std::string infoText)
    {
        if (const auto existing = getArgumentDefinition(argumentVariants)) {
            throwArgumentExistingError(*existing);
        } else {
            const auto argumentDefinition = std::make_shared<ArgumentDefinition>(argumentVariants, callback, required, infoText);
            m_argumentDefinitions.push_back(argumentDefinition);
        }
    }

    void addArgument(ArgumentVariants argumentVariants, SingleStringCallback callback, bool required, std::string infoText)
    {
        if (const auto existing = getArgumentDefinition(argumentVariants)) {
            throwArgumentExistingError(*existing);
        } else {
            const auto argumentDefinition = std::make_shared<ArgumentDefinition>(argumentVariants, callback, required, infoText);
            m_argumentDefinitions.push_back(argumentDefinition);
        }
    }

    ArgumentVector arguments() const
    {
        return m_args;
    }

    void setOutputStream(std::ostream & out)
    {
        m_out = &out;
    }

    void setErrorStream(std::ostream & err)
    {
        m_err = &err;
    }

    void setHelpText(std::string helpText)
    {
        m_helpText = helpText;
    }

    void setHelpSorting(HelpSorting helpSorting)
    {
        m_helpSorting = helpSorting;
    }

    void setPositionalArgumentCallback(MultiStringCallback callback)
    {
        m_positionalArgumentCallback = callback;
    }

    void printHelp()
    {
        if (!m_helpText.empty()) {
            *m_out << m_helpText << std::endl
                   << std::endl;
        }

        *m_out << "Options:" << std::endl
               << std::endl;

        if (m_helpSorting == HelpSorting::Ascending) {
            std::sort(m_argumentDefinitions.begin(), m_argumentDefinitions.end(), [](const ArgumentDefinitionPtr & l, const ArgumentDefinitionPtr & r) {
                return l->getVariantsString() < r->getVariantsString();
            });
        }

        std::vector<std::pair<std::string, std::string>> optionTexts;
        size_t maxLength = 0;
        for (auto && option : m_argumentDefinitions) {
            const auto variantsString = option->getVariantsString();
            maxLength = std::max(variantsString.size(), maxLength);
            optionTexts.push_back({ variantsString, option->infoText });
        }
        const size_t margin = 2;
        for (auto && optionText : optionTexts) {
            *m_out << optionText.first;
            for (size_t i = 0; i < maxLength + margin - optionText.first.size(); i++) {
                *m_out << " ";
            }
            *m_out << optionText.second;
            *m_out << std::endl;
        }
        *m_out << std::endl;
    }

    void parse()
    {
        for (size_t i = 1; i < m_args.size(); i++) {
            const auto arg = m_args.at(i);
            // Try to reason out 'ARG' or 'ARG VALUE'
            if (const auto match = getArgumentDefinition(arg)) {
                i = processTrivialMatch(match, i);
                // Try to reason out 'ARG=VALUE'
            } else if (!tryProcessAssigmentFormat(arg)) {
                // Try to reason out 'ARGVALUE'
                tryProcessSpacelessFormat(arg);
            }
        }

        checkRequired();
    }

    ~Impl() = default;

private:
    struct ArgumentDefinition
    {
        ArgumentDefinition(const ArgumentVariants & variants, ValuelessCallback callback, bool required, std::string infoText)
          : variants(variants)
          , valuelessCallback(callback)
          , singleStringCallback(nullptr)
          , required(required)
          , infoText(infoText)
        {
        }

        ArgumentDefinition(const ArgumentVariants & variants, SingleStringCallback callback, bool required, std::string infoText)
          : variants(variants)
          , valuelessCallback(nullptr)
          , singleStringCallback(callback)
          , required(required)
          , infoText(infoText)
        {
        }

        bool isMatch(ArgumentVariants variants) const
        {
            for (auto && variant : variants) {
                if (this->variants.count(variant)) {
                    return true;
                }
            }
            return false;
        }

        std::string getVariantsString() const
        {
            std::string str;
            size_t count = 0;
            for (auto rit = variants.rbegin(); rit != variants.rend(); rit++) {
                str += *rit;
                if (++count < variants.size()) {
                    str += ", ";
                }
            }
            return str;
        }

        ArgumentVariants variants;

        ValuelessCallback valuelessCallback = nullptr;

        SingleStringCallback singleStringCallback = nullptr;

        bool required = false;

        bool applied = false;

        std::string infoText;
    };

    void addHelp()
    {
        m_helpText = "Usage: " + m_args.at(0) + " [OPTIONS]";

        addArgument(
          { "-h", "--help" }, [=] {
              printHelp();
              exit(EXIT_SUCCESS);
          },
          false, "Show this help.");
    }

    void checkRequired()
    {
        for (auto && definition : m_argumentDefinitions) {
            if (definition->required && !definition->applied) {
                throwRequiredError(*definition);
            }
        }
    }

    using ArgumentDefinitionPtr = std::shared_ptr<ArgumentDefinition>;
    ArgumentDefinitionPtr getArgumentDefinition(ArgumentVariants variants) const
    {
        for (auto && definition : m_argumentDefinitions) {
            if (definition->isMatch(variants)) {
                return definition;
            }
        }
        return nullptr;
    }

    ArgumentDefinitionPtr getArgumentDefinition(std::string argument) const
    {
        return getArgumentDefinition(ArgumentVariants { argument });
    }

    std::string name() const
    {
        return "Argengine";
    }

    size_t processTrivialMatch(ArgumentDefinitionPtr match, size_t currentIndex)
    {
        if (match->valuelessCallback) {
            match->valuelessCallback();
            match->applied = true;
        } else if (match->singleStringCallback) {
            if (++currentIndex < m_args.size()) {
                match->singleStringCallback(m_args.at(currentIndex));
                match->applied = true;
            } else {
                throwNoValueError(*match);
            }
        }
        return currentIndex;
    }

    bool tryProcessAssigmentFormat(std::string arg)
    {
        std::string assignmentFormatArg;
        const auto pos = arg.find_first_of('=');
        if (pos != arg.npos) {
            assignmentFormatArg = arg.substr(0, pos);
            const auto match = getArgumentDefinition(assignmentFormatArg);
            if (match && match->singleStringCallback) {
                const auto valueLength = arg.size() - (pos + 1);
                if (!valueLength) {
                    throwNoValueError(*match);
                }
                const auto value = arg.substr(pos + 1, valueLength);
                match->singleStringCallback(value);
                match->applied = true;
                return true;
            }
        }

        if (!assignmentFormatArg.empty()) {
            throwUnknownArgumentError(assignmentFormatArg);
        }

        return false;
    }

    void tryProcessSpacelessFormat(std::string arg)
    {
        bool spacelessFormatSucceeded {};
        std::map<ArgumentDefinitionPtr, std::string> matchingDefinitions;
        for (auto && definition : m_argumentDefinitions) {
            for (auto && variant : definition->variants) {
                if (arg.find(variant) == 0) {
                    matchingDefinitions[definition] = variant;
                }
            }
        }

        if (matchingDefinitions.size() == 1) {
            const auto match = matchingDefinitions.begin()->first;
            if (match && match->singleStringCallback) {
                const auto pos = matchingDefinitions.begin()->second.size();
                const auto valueLength = arg.size() - pos;
                if (!valueLength) {
                    throwNoValueError(*match);
                }
                const auto value = arg.substr(pos, valueLength);
                match->singleStringCallback(value);
                match->applied = true;
                spacelessFormatSucceeded = true;
            }
        } else if (matchingDefinitions.size() > 1) {
            throwAmbiguousArgumentError(arg, matchingDefinitions);
        }

        if (!spacelessFormatSucceeded) {
            throwUnknownArgumentError(arg);
        }
    }

    [[noreturn]] void throwAmbiguousArgumentError(std::string argument, std::map<ArgumentDefinitionPtr, std::string> matchingDefinitions)
    {
        std::string ambiguos;
        size_t count = 0;
        for (auto iter : matchingDefinitions) {
            ambiguos += "'" + iter.second + "'";
            if (++count < matchingDefinitions.size()) {
                ambiguos += ", ";
            }
        }

        throw std::runtime_error(name() + ": Argument '" + argument + "' is ambiguos due to arguments: " + ambiguos);
    }

    [[noreturn]] void throwArgumentExistingError(const ArgumentDefinition & existing)
    {
        throw std::runtime_error(name() + ": Argument '" + existing.getVariantsString() + "' already defined!");
    }

    [[noreturn]] void throwRequiredError(const ArgumentDefinition & existing)
    {
        throw std::runtime_error(name() + ": Argument '" + existing.getVariantsString() + "' is required!");
    }

    [[noreturn]] void throwUnknownArgumentError(std::string arg)
    {
        throw std::runtime_error(name() + ": Uknown argument '" + arg + "'!");
    }

    [[noreturn]] void throwNoValueError(const ArgumentDefinition & existing)
    {
        throw std::runtime_error(name() + ": No value for argument '" + existing.getVariantsString() + "' given!");
    }

    ArgumentVector m_args;

    std::string m_helpText;

    HelpSorting m_helpSorting = HelpSorting::None;

    std::vector<ArgumentDefinitionPtr> m_argumentDefinitions;

    MultiStringCallback m_positionalArgumentCallback = nullptr;

    std::ostream * m_out = &std::cout;

    std::ostream * m_err = &std::cerr;
};

Argengine::Argengine(int argc, char ** argv, bool addDefaultHelp)
  : m_impl(new Impl(std::vector<std::string>(argv, argv + argc), addDefaultHelp))
{
}

Argengine::Argengine(ArgumentVector args, bool addDefaultHelp)
  : m_impl(new Impl(args, addDefaultHelp))
{
}

void Argengine::addArgument(ArgumentVariants argumentVariants, ValuelessCallback callback)
{
    m_impl->addArgument(argumentVariants, callback, false, "");
}

void Argengine::addArgument(ArgumentVariants argumentVariants, ValuelessCallback callback, bool required)
{
    m_impl->addArgument(argumentVariants, callback, required, "");
}

void Argengine::addArgument(ArgumentVariants argumentVariants, ValuelessCallback callback, bool required, std::string infoText)
{
    m_impl->addArgument(argumentVariants, callback, required, infoText);
}

void Argengine::addArgument(ArgumentVariants argumentVariants, SingleStringCallback callback)
{
    m_impl->addArgument(argumentVariants, callback, false, "");
}

void Argengine::addArgument(ArgumentVariants argumentVariants, SingleStringCallback callback, bool required)
{
    m_impl->addArgument(argumentVariants, callback, required, "");
}

void Argengine::addArgument(ArgumentVariants argumentVariants, SingleStringCallback callback, bool required, std::string infoText)
{
    m_impl->addArgument(argumentVariants, callback, required, infoText);
}

Argengine::ArgumentVector Argengine::arguments() const
{
    return m_impl->arguments();
}

void Argengine::setHelpText(std::string helpText)
{
    m_impl->setHelpText(helpText);
}

void Argengine::setHelpSorting(HelpSorting helpSorting)
{
    m_impl->setHelpSorting(helpSorting);
}

void Argengine::setPositionalArgumentCallback(MultiStringCallback callback)
{
    m_impl->setPositionalArgumentCallback(callback);
}

void Argengine::setOutputStream(std::ostream & out)
{
    m_impl->setOutputStream(out);
}

void Argengine::setErrorStream(std::ostream & err)
{
    m_impl->setErrorStream(err);
}

void Argengine::printHelp()
{
    m_impl->printHelp();
}

void Argengine::parse()
{
    m_impl->parse();
}

Argengine::~Argengine() = default;

} // juzzlin
