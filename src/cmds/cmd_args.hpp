#pragma once

#include <utility>

#include "boost/range/algorithm_ext/erase.hpp"

#include "types.hpp"

namespace lmail
{

class CmdArgs
{
public:
    CmdArgs(args_t args) : args_(std::move(args))
    {
        boost::remove_erase_if(args_, [](auto const &arg) { return arg.empty(); });
    }

    arg_t pop()
    {
        arg_t arg;
        if (!args_.empty())
            arg = std::move(args_.front()), args_.pop_front();
        return arg;
    }

    bool empty() const noexcept { return args_.empty(); }

    arg_t front() { return !args_.empty() ? args_.front() : arg_t{}; }

private:
    args_t args_;
};

} // namespace lmail
