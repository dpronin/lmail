#pragma once

#include <boost/range/algorithm_ext/erase.hpp>

#include "types.hpp"

namespace lmail
{

struct CmdBaseArgs
{
    explicit CmdBaseArgs(args_t args) : args_(std::move(args))
    {
        boost::remove_erase_if(args_, [](auto const &arg){ return arg.empty(); });
    }
    args_t args_;
};

} // namespace lmail
