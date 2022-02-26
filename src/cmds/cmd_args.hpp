#pragma once

#include <stdexcept>
#include <utility>

#include "boost/range/algorithm_ext/erase.hpp"

#include "types.hpp"

namespace lmail
{

class CmdArgs
{
    args_t args_;

public:
    CmdArgs(args_t args)
        : args_(std::move(args))
    {
        boost::remove_erase_if(args_, [](auto const& arg) { return arg.empty(); });
    }

    arg_t pop()
    {
        if (!empty()) {
            arg_t arg{std::move(args_.front())};
            args_.pop_front();
            return arg;
        }
        throw std::runtime_error("cannot access 'pop' argument, reason: argument list is empty");
    }

    [[nodiscard]] bool empty() const noexcept { return args_.empty(); }

    [[nodiscard]] arg_t const& front() const
    {
        if (!empty())
            return args_.front();
        throw std::runtime_error("cannot access 'front' argument, reason: argument list is empty");
    }
};

} // namespace lmail
