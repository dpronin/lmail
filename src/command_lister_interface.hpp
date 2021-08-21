#pragma once

#include "types.hpp"

namespace lmail
{

class ICommandLister
{
public:
    virtual ~ICommandLister() = default;

    virtual cmds_t const &commands() const noexcept = 0;
};

} // namespace lmail
