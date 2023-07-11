#pragma once

#include <functional>
#include <optional>
#include <stdexcept>
#include <utility>

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
        std::erase_if(args_, std::mem_fn(&arg_t::empty));
    }

    std::optional<arg_t> pop()
    {
        if (!empty()) {
            arg_t arg{std::move(args_.front())};
            args_.pop_front();
            return arg;
        }
        return {};
    }

    [[nodiscard]] bool empty() const noexcept { return args_.empty(); }

    [[nodiscard]] std::optional<arg_t> front() const
    {
        if (!empty())
            return args_.front();
        return {};
    }
};

} // namespace lmail
