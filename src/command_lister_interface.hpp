#pragma once

#include "types.hpp"

namespace lmail
{

class ICommandLister
{
public:
    virtual ~ICommandLister() = default;

    virtual cmds_names_t cmds() const = 0;
};

} // namespace lmail
