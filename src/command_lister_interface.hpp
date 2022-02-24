#pragma once

#include "types.hpp"

namespace lmail
{

class ICommandLister
{
public:
    ICommandLister()          = default;
    virtual ~ICommandLister() = default;

    ICommandLister(ICommandLister const&) = default;
    ICommandLister& operator=(ICommandLister const&) = default;

    ICommandLister(ICommandLister&&) = default;
    ICommandLister& operator=(ICommandLister&&) = default;

    [[nodiscard]] virtual cmds_names_t cmds() const = 0;
};

} // namespace lmail
